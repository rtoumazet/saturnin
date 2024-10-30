//
// opengl_textures.h
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

#pragma once

#include <saturnin/src/emulator_defs.h>
#include <saturnin/src/video/opengl/opengl.h>
#include <saturnin/src/video/vdp_common.h>

namespace saturnin::video {

enum class TextureArrayType : u8 { saturn_part, framebuffer };
enum class FboTextureType : u8 { front_buffer, back_buffer, vdp1_debug_overlay, vdp2_debug_layer, priority };
enum class GuiTextureType : u8 { render_buffer, vdp1_debug_buffer, vdp2_debug_buffer, layer_buffer };

using FboTextureTypeToLayer = std::array<FboTextureType, fbo_texture_array_depth>; // Defines the type of each FBO texture layer,
                                                                                   // the index of the array being the layer.
using GuiTextureTypeToId = std::unordered_map<GuiTextureType, u32>; // Defines the type of each texture used to render to GUI.

class OpenglTexturing {
  public:
    OpenglTexturing() = default;
    explicit OpenglTexturing(Opengl* opengl) : opengl_(opengl) {};
    ~OpenglTexturing()                                 = default;
    OpenglTexturing(const OpenglTexturing&)            = delete;
    OpenglTexturing(OpenglTexturing&&)                 = delete;
    OpenglTexturing& operator=(const OpenglTexturing&) = delete;
    OpenglTexturing& operator=(OpenglTexturing&&)      = delete;

    void initialize();

    // Shuts down this object and frees any resources it is using
    void shutdown();

    // Initializes a texture array.
    auto initializeTextureArray(const u32 width, const u32 height, const u32 depth) const -> u32;

    // Generates a texture from a vector of raw data. Returns the OpenGL id of the generated texture.
    [[nodiscard]] static auto generateTexture(u32 width, u32 height, const std::vector<u8>& data) -> u32;

    // Generates the textures that will be used during rendering.
    void generateTextures();

    // Generates a subtexture in the texture array of texture atlas.
    void generateSubTexture(const size_t key);

    // Deletes the texture on the GPU using its OpenGL texture id.
    static void deleteTexture(const u32 texture_id);

    // Pack textures in a texture array of texture atlases for the selected layer.
    void packTextures(std::vector<OpenglTexture>& textures, const VdpLayer layer);

    // Texture key is added so the linked texture is created or marked for recreation. Layer is used for cache management.
    void addOrUpdateTexture(const size_t key, const VdpLayer layer);

    // Removes the link between the Saturn texture and the OpenGL texture id.
    void removeTextureLink(const size_t key);

    // Gets texture identifier corresponding to the key if found.
    auto getOpenglTexture(const size_t key) -> std::optional<OpenglTexture>;

    // Calculates the texture coordinates in the texture array from the texture position and size.
    auto calculateTextureCoordinates(const ScreenPos& texture_pos_in_pixels,
                                     const Size&      texture_size,
                                     const u8         texture_array_index) const -> std::vector<TextureCoordinates>;

    // Returns all the vertexes from a parts list.
    auto readVertexes(const PartsList& parts) -> std::vector<Vertex>;

    // Returns the details of an Opengl texture
    auto getOpenglTextureDetails(const size_t key) -> std::string;

    auto getTextureId(const TextureArrayType type) -> u32;

    // Returns the current texture array index used by a specific layer.
    auto getCurrentTextureArrayIndex(const VdpLayer layer) -> u8;

    // Returns the next available texture array index regardless of the layer.
    auto getNextAvailableTextureArrayIndex() const -> u8;

    // Generates a texture from a texture array layer. Returns the id of the generated texture.
    auto generateTextureFromTextureArrayLayer(const GuiTextureType dst_texture_type, const u8 layer) -> u32;

    // Attachs a texture to the curently bound FBO.
    void attachTextureToFbo(const u32 texture_id, const gl::GLenum framebuffer_target, const gl::GLenum color_attachment) const;

    // Attachs a texture array layer to the curently bound FBO.
    void attachTextureLayerToFbo(const u32        texture_id,
                                 const u8         layer,
                                 const gl::GLenum framebuffer_target,
                                 const gl::GLenum color_attachment) const;

    // Initializes the framebuffer object and related elements.
    void initializeFbo();

    // Generates a framebuffer object.
    auto generateFbo(const FboType fbo_type) -> u32;

    auto getFboId(const FboType type) const { return fbo_type_to_id_.at(type); };

    // Gets the FBO texture layer currently used by the FboTextureType.
    auto getFboTextureLayer(const FboTextureType type) const -> u8;

    auto getGuiTextureId(const GuiTextureType type) const { return gui_texture_type_to_id_.at(type); };

    auto getTextureArrayId() const { return texture_array_id_; };
    auto getFboTextureArrayId() const { return fbo_texture_array_id_; };

    [[nodiscard]] auto vdp2DebugLayerTextureId() const -> u32 {
        return gui_texture_type_to_id_.at(GuiTextureType::vdp2_debug_buffer);
    };

    // auto textureId

  private:
    Opengl* opengl_;

    u32          texture_array_id_;               // Identifier for the texture array.
    TexturesLink textures_link_;                  // Link between the Texture key and the OpenglTexture.
    u32          texture_array_debug_layer_id_{}; // Identifier for the texture array debug layer.

    u32                   fbo_texture_array_id_;      // Identifier for the FBO texture array.
    GuiTextureTypeToId    gui_texture_type_to_id_;    // Links the texture to be used in the GUI to a type.
    FboTypeToId           fbo_type_to_id_;            // The framebuffer objects used in the app.
    FboTextureTypeToLayer fbo_texture_type_to_layer_; // Links the used FBO texture layer to a texture type. Index of the array
                                                      // is the layer, content is the type.

    LayerToCacheReloadState    layer_to_cache_reload_state_{};    // Stores if a layer needs its cache to be reloaded .
    LayerToTextureArrayIndexes layer_to_texture_array_indexes_{}; // Link between layers and texture array indexes.
};

}; // namespace saturnin::video
