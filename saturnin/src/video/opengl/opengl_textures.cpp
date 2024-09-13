//
// opengl_textures.cpp
// Saturnin
//
// Copyright (c) 2024 Renaud Toumazet
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <saturnin/src/pch.h>
#include <saturnin/src/video/opengl/opengl.h>

#include <glbinding/glbinding.h>
#include <glbinding/Version.h>
#include <glbinding-aux/ContextInfo.h>
#include <glbinding/gl21/gl.h>
#include <glbinding/gl21ext/gl.h>
#include <glbinding/gl33core/gl.h>
#include <glbinding/gl33ext/gl.h>
#include <saturnin/src/video/texture.h>

namespace saturnin::video {

using namespace gl21;
using namespace gl21ext;

using core::tr;

void Opengl::addOrUpdateTexture(const size_t key, const VdpLayer layer) {
    // If the key doesn't exist it will be automatically added.
    // If it does, opengl link will be reset in order to regenerate the texture.
    // const auto opengl_tex = getOpenglTexture(key);
    // const auto& texture = Texture::getTexture(key);

    // if (texture) {
    std::lock_guard lock(textures_link_mutex_);
    textures_link_[key].key                 = key;
    textures_link_[key].opengl_id           = 0;
    textures_link_[key].texture_array_index = 0;
    layer_to_cache_reload_state_[layer]     = true; // This layer will have to be reloaded
    // textures_link_[key].width     = (*texture)->width();
    // textures_link_[key].height    = (*texture)->height();
    // textures_link_[key].size      = (*texture)->size();
    // }
}

void Opengl::removeTextureLink(const size_t key) { textures_link_.erase(key); }

void Opengl::generateTextures() {
    // Textures are generated in a 128 layers texture array, each layer being a texture atlas of
    // 1024*1024 pixels.
    // In theory, the maximum number of different VDP2 cells that could be stored by the Saturn at a given time is 0x80000
    // (ie both RBG at maximum size, with every cell different).
    // Maximum VDP1 texture size is 504*255.
    // Textures are grouped by layers, meaning that every layer can be linked to multiple texture array indexes. This way
    // it's not mandatory to regenerate the whole texture atlas every frame.

    // OpenglTextures don't have to be deleted, as they are part of a texture atlas now

    //

    auto local_textures_link = TexturesLink();
    local_textures_link      = textures_link_;

    auto layer_to_textures = LayerToTextures{};

    for (const auto& [key, ogl_tex] : local_textures_link) {
        const auto t = Texture::getTexture(key);

        auto opengl_tex = getOpenglTexture(key);
        if (opengl_tex) { (*opengl_tex).size = {(**t).width(), (**t).height()}; }
        layer_to_textures[(*t)->layer()].push_back(*opengl_tex);
    }

    for (auto& [layer, textures] : layer_to_textures) {
        if (layer_to_cache_reload_state_[layer]) { packTextures(textures, layer); }
    }

    // Reset cache reload state
    for (auto& [layer, state] : layer_to_cache_reload_state_) {
        state = false;
    }
}

// Using the simplest (and fastest) method to pack textures in the atlas. Better algorithms could be used to
// improve packing, but there's a non trivial performance tradeoff, with a big increase in complexity.
// So for now, keeping things simple.
void Opengl::packTextures(std::vector<OpenglTexture>& textures, const VdpLayer layer) {
    glBindTexture(GL_TEXTURE_2D_ARRAY, texture_array_id_);
    // First, indexes used by the layer are cleared and removed from the used list.
    auto empty_data = std::vector<u8>(texture_array_width * texture_array_height * 4);
    if (!layer_to_texture_array_indexes_[layer].empty()) {
        for (const auto& index : layer_to_texture_array_indexes_[layer]) {
            glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
                            0,
                            0,
                            0,
                            index,
                            texture_array_width,
                            texture_array_height,
                            1,
                            GLenum::GL_RGBA,
                            GLenum::GL_UNSIGNED_BYTE,
                            empty_data.data());
        }
        std::vector<u8>().swap(layer_to_texture_array_indexes_[layer]);
    }

    // Second, texture are sorted by horizontal size.
    std::ranges::sort(textures, [](const OpenglTexture& a, const OpenglTexture& b) { return a.size.h > b.size.h; });

    auto current_index          = getCurrentTextureArrayIndex(layer);
    auto x_pos                  = u16{};
    auto y_pos                  = u16{};
    auto current_row_max_height = u16{};

    for (const auto& texture : textures) {
        if ((x_pos + texture.size.w) > texture_array_width) {
            // Texture doesn't fit in the remaining space of the row ... looping around to next row using
            // the maximum height from the previous row.
            y_pos += current_row_max_height;
            x_pos                  = 0;
            current_row_max_height = 0;
        }

        if ((y_pos + texture.size.h) > texture_array_width) {
            // Texture doesn't fit in the remaining space of the last row ... moving one layer forward.
            current_index = getNextAvailableTextureArrayIndex();
            layer_to_texture_array_indexes_.at(layer).push_back(current_index);
            x_pos                  = 0;
            y_pos                  = 0;
            current_row_max_height = 0;
        }
        // This is the position of the rectangle
        textures_link_[texture.key].texture_array_index = current_index;
        textures_link_[texture.key].size                = texture.size;
        textures_link_[texture.key].pos                 = {x_pos, y_pos};

        textures_link_[texture.key].coords = calculateTextureCoordinates({x_pos, y_pos}, texture.size, current_index);

        generateSubTexture(texture.key);

        x_pos += texture.size.w; // Move along to the next spot in the row.

        // Just saving the largest height in the new row
        if (texture.size.h > current_row_max_height) { current_row_max_height = texture.size.h; }
    }
    // texture_array_max_used_layer_ = current_layer;
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

auto Opengl::calculateTextureCoordinates(const ScreenPos& pos,
                                         const Size&      size,
                                         const u8         texture_array_index) const -> std::vector<TextureCoordinates> {
    // Calculating the texture coordinates in the atlas
    // (x1,y1)     (x2,y1)
    //        .---.
    //        |   |
    //        .---.
    // (x1,y2)     (x2,y2)

    auto x1 = static_cast<float>(pos.x) / static_cast<float>(texture_array_width);
    auto x2 = static_cast<float>(pos.x + size.w) / static_cast<float>(texture_array_width);
    auto y1 = static_cast<float>(pos.y) / static_cast<float>(texture_array_height);
    auto y2 = static_cast<float>(pos.y + size.h) / static_cast<float>(texture_array_height);

    return std::vector{
        TextureCoordinates{x1, y1, static_cast<float>(texture_array_index)},
        TextureCoordinates{x2, y1, static_cast<float>(texture_array_index)},
        TextureCoordinates{x2, y2, static_cast<float>(texture_array_index)},
        TextureCoordinates{x1, y2, static_cast<float>(texture_array_index)}
    };
}

auto Opengl::readVertexes(const PartsList& parts) -> std::vector<Vertex> {
    auto vertexes = std::vector<Vertex>{};
    for (auto p : parts) {
        const auto opengl_tex = getOpenglTexture(p.texture_key);
        if (opengl_tex.has_value()) {
            // Replacing texture coordinates of the vertex by those of the OpenGL texture.
            for (auto& v : p.vertexes) {
                v.color_offset = p.color_offset;
                if ((v.tex_coords.s == 0.0) && (v.tex_coords.t == 0.0)) {
                    v.tex_coords = opengl_tex->coords[0];
                    continue;
                }
                if ((v.tex_coords.s == 1.0) && (v.tex_coords.t == 0.0)) {
                    v.tex_coords = opengl_tex->coords[1];
                    continue;
                }
                if ((v.tex_coords.s == 1.0) && (v.tex_coords.t == 1.0)) {
                    v.tex_coords = opengl_tex->coords[2];
                    continue;
                }
                if ((v.tex_coords.s == 0.0) && (v.tex_coords.t == 1.0)) {
                    v.tex_coords = opengl_tex->coords[3];
                    continue;
                }
            }
        } else {
            for (auto& v : p.vertexes) {
                v.color_offset = p.color_offset;
            }
        }
        std::ranges::copy(p.vertexes.begin(), p.vertexes.end(), std::back_inserter(vertexes));
    }

    return vertexes;
}

auto Opengl::getOpenglTexture(const size_t key) -> std::optional<OpenglTexture> {
    std::lock_guard lock(textures_link_mutex_);
    const auto      it = textures_link_.find(key);
    return (it == textures_link_.end()) ? std::nullopt : std::optional<OpenglTexture>(it->second);
}

auto Opengl::getOpenglTextureDetails(const size_t key) -> std::string {
    auto            details = std::string{};
    std::lock_guard lock(textures_link_mutex_);
    if (const auto it = textures_link_.find(key); it != textures_link_.end()) {
        details += util::format("Key: 0x{:x}\n", it->second.key);
        details += util::format("Position: {},{}\n", it->second.pos.x, it->second.pos.y);
        details += util::format("Size: {},{}\n", it->second.size.w, it->second.size.h);
        details += util::format("Layer: {}\n", it->second.texture_array_index);
        details += util::format("X1: {},{},{}\n", it->second.coords[0].s, it->second.coords[0].t, it->second.coords[0].p);
        details += util::format("X2: {},{},{}\n", it->second.coords[1].s, it->second.coords[1].t, it->second.coords[1].p);
        details += util::format("Y2: {},{},{}\n", it->second.coords[2].s, it->second.coords[2].t, it->second.coords[2].p);
        details += util::format("Y1: {},{},{}\n", it->second.coords[3].s, it->second.coords[3].t, it->second.coords[3].p);
    }
    return details;
}

// static
auto Opengl::generateTexture(const u32 width, const u32 height, const std::vector<u8>& data) -> u32 {
    using enum GLenum;
    auto texture = u32{};
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());

    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
}

void Opengl::generateSubTexture(const size_t key) {
    const auto ogl_tex = getOpenglTexture(key);

    if (ogl_tex) {
        const auto tex = Texture::getTexture(key);
        if (tex) {
            glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
                            0,
                            ogl_tex->pos.x,
                            ogl_tex->pos.y,
                            ogl_tex->texture_array_index,
                            ogl_tex->size.w,
                            ogl_tex->size.h,
                            1,
                            GLenum::GL_RGBA,
                            GLenum::GL_UNSIGNED_BYTE,
                            (*tex)->rawData().data());
            checkGlError();
        }
    }
}

// static
void Opengl::deleteTexture(const u32 texture) { glDeleteTextures(1, &texture); }

auto Opengl::getTextureId(const TextureArrayType type) -> u32 {
    // There are 2 texture arrays used in the program :
    // - one is used as a texture atlas for VDP parts rendering (texture_array_id_)
    // - the other as framebuffers linked to the main FBO, handling debug layers, priority rendering, front and back
    // framebuffer (fbo_texture_array_id_), etc.

    switch (type) {
        using enum TextureArrayType;
        case saturn_part: {
            // Checking if the texture already exists. If not it'll be created.
            if (texture_array_debug_layer_id_ == 0) {
                texture_array_debug_layer_id_ = generateTexture(texture_array_width, texture_array_height, std::vector<u8>{});
            }
            // :TODO: update texture data based on the texture array layer specified.

            return texture_array_debug_layer_id_;
        }
        case framebuffer: {
            // :TODO: Search in the texture pool the id of the texture ... is it really needed ?
            break;
        }
    }

    return u32();
};

} // namespace saturnin::video
