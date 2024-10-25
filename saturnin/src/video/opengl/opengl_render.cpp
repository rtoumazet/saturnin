//
// opengl_render.cpp
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
#include <saturnin/src/video/opengl/opengl_render.h>
#include <ranges>
#include <glbinding/glbinding.h>
#include <glbinding/Version.h>
#include <glbinding-aux/ContextInfo.h>
#include <glbinding/gl21/gl.h>
#include <glbinding/gl21ext/gl.h>
#include <glbinding/gl33core/gl.h>
#include <glbinding/gl33ext/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <saturnin/src/video/opengl/opengl_shaders.h>
#include <saturnin/src/video/opengl/opengl_textures.h>
#include <saturnin/src/video/opengl/opengl_utilities.h>
#include <saturnin/src/video/vdp1.h>
#include <saturnin/src/video/vdp2/vdp2.h>

namespace saturnin::video {

using namespace gl21;
using namespace gl21ext;

using core::tr;

const std::unordered_map<VdpLayer, std::string> layer_to_name = {
    {VdpLayer::nbg0,      "nbg0"     },
    {VdpLayer::rbg1,      "rbg1"     },
    {VdpLayer::nbg1,      "nbg1"     },
    {VdpLayer::exbg,      "exbg"     },
    {VdpLayer::nbg2,      "nbg2"     },
    {VdpLayer::nbg3,      "nbg3"     },
    {VdpLayer::rbg0,      "rbg0"     },
    {VdpLayer::back,      "back"     },
    {VdpLayer::sprite,    "sprite"   },
    {VdpLayer::undefined, "undefined"}
};

void OpenglRender::initialize() {
    shaders_.graphics        = initializeShaders();
    const auto main_vertex   = createVertexShader(shaders_.graphics, "main.vert");
    const auto main_fragment = createFragmentShader(shaders_.graphics, "main.frag");
    shaders_.programs.try_emplace(ProgramShader::main, createProgramShader(main_vertex, main_fragment));

    const auto shaders_to_delete = std::vector<u32>{main_vertex, main_fragment};
    deleteShaders(shaders_to_delete);
}

void OpenglRender::shutdown() const {
    for (auto& [name, id] : shaders_.programs) {
        glDeleteProgram(id);
    }
}

void OpenglRender::preRender() {
    switchRenderedBuffer();

    glBindFramebuffer(GLenum::GL_FRAMEBUFFER, opengl_->texturing()->getFboId(FboType::general));

    opengl_->texturing()->attachTextureLayerToFbo(opengl_->texturing()->getFboTextureArrayId(),
                                                  opengl_->texturing()->getFboTextureLayer(currentRenderedBuffer()),
                                                  GLenum::GL_FRAMEBUFFER,
                                                  GLenum::GL_COLOR_ATTACHMENT0);

    // Viewport is the entire Saturn framebuffer
    glViewport(0, 0, saturn_framebuffer_width, saturn_framebuffer_height);

    gl::glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Scissor test calculation, to remove from display everything outside the current display area.
    glEnable(GL_SCISSOR_TEST);
    auto [scissor_x, scissor_y, scissor_width, scissor_height] = calculateViewportPosAndSize();
    glScissor(scissor_x, scissor_y, scissor_width, scissor_height);
};

void OpenglRender::postRender() const {
    // Framebuffer is released
    checkGlError();

    glDisable(GL_SCISSOR_TEST);

    glBindFramebuffer(GLenum::GL_FRAMEBUFFER, 0);
};

void OpenglRender::renderToAvailableTexture(const FboKey& key, const PartsList& parts_list) {
    using enum Logger;
    Log::debug(opengl, "renderToAvailableFbo() call");
    const auto index = opengl_->getAvailableFboTextureIndex();
    if (!index) {
        Log::debug(opengl, "- No FBO available for rendering");
        Log::debug(opengl, "renderToAvailableFbo() return");
        return;
    }

    const auto& [priority, layer] = key;
    Log::debug(opengl, "- Rendering key [priority={}, layer={}] to FBO with index {}", priority, layer_to_name.at(layer), *index);

    renderParts(parts_list, opengl_->fbo_texture_pool_[*index]);

    Log::debug(opengl, "- Changing FBO status at index {} to 'reuse'", *index);

    opengl_->fbo_texture_pool_status_[*index] = FboTextureStatus::reuse;
    opengl_->fbo_key_to_fbo_pool_index_[key]  = *index;

    Log::debug(opengl, "renderToAvailableFbo() return");
}

void OpenglRender::renderParts(const PartsList& parts_list, const u32 texture_id) {
    if (!parts_list.empty()) {
        const auto [vao, vertex_buffer] = initializeVao();

        glUseProgram(shaders_.programs[ProgramShader::main]);
        glBindVertexArray(vao);                       // binding VAO
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer); // binding vertex buffer

        // Calculating the ortho projection matrix
        const auto proj_matrix = calculateDisplayViewportMatrix();

        // Sending the ortho projection matrix to the shader
        const auto uni_proj_matrix = glGetUniformLocation(shaders_.programs[ProgramShader::main], "proj_matrix");
        glUniformMatrix4fv(uni_proj_matrix, 1, GL_FALSE, glm::value_ptr(proj_matrix));

        glActiveTexture(GLenum::GL_TEXTURE0);
        const auto sampler_loc = glGetUniformLocation(shaders_.programs[ProgramShader::main], "sampler");

        glUniform1i(sampler_loc, 0);
        glBindTexture(GL_TEXTURE_2D_ARRAY, texture_id);

        const auto texture_used_loc = glGetUniformLocation(shaders_.programs[ProgramShader::main], "is_texture_used");

        auto elements_buffer = u32{};
        glGenBuffers(1, &elements_buffer); // This buffer will be used to send indices data to the GPU
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elements_buffer);

        const auto&& [indices, draw_ranges] = generateVertexIndicesAndDrawRanges(parts_list);
        const auto vertexes                 = opengl_->texturing()->readVertexes(parts_list);

        // Sending data to the GPU
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * indices.size(), indices.data(), GL_STATIC_DRAW);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertexes.size(), vertexes.data(), GL_STATIC_DRAW);

        for (const auto& range : draw_ranges) {
            const auto is_texture_used = GLboolean(range.is_textured);
            glUniform1i(texture_used_loc, is_texture_used);

            glDrawRangeElements(range.primitive,
                                range.vertex_array_start,
                                range.vertex_array_end,
                                range.indices_nb,
                                GL_UNSIGNED_INT,
                                static_cast<GLuint*>(nullptr) + range.indices_array_start);
        }

        gl::glDeleteBuffers(1, &vertex_buffer);
        gl::glDeleteVertexArrays(1, &vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
        gl::glDeleteBuffers(1, &elements_buffer);
    }
}

void OpenglRender::renderFboTexture(const u32 texture_id) {
    const auto [vao, vertex_buffer] = initializeVao();

    glUseProgram(shaders_.programs[ProgramShader::main]);
    glBindVertexArray(vao);                       // binding VAO
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer); // binding vertex buffer

    // Calculating the ortho projection matrix
    const auto proj_matrix = calculateDisplayViewportMatrix();

    // Sending the ortho projection matrix to the shader
    const auto uni_proj_matrix = glGetUniformLocation(shaders_.programs[ProgramShader::main], "proj_matrix");
    glUniformMatrix4fv(uni_proj_matrix, 1, GL_FALSE, glm::value_ptr(proj_matrix));

    glActiveTexture(GLenum::GL_TEXTURE0);
    const auto sampler_loc = glGetUniformLocation(shaders_.programs[ProgramShader::main], "sampler");

    glUniform1i(sampler_loc, 0);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    const auto texture_used_loc = glGetUniformLocation(shaders_.programs[ProgramShader::main], "is_texture_used");
    glUniform1i(texture_used_loc, GLboolean(true));

    auto vertexes = std::vector<Vertex>{
        {0,    0,    0.0f, 0.0f, 0.0f, 0, 0, 0xff, 0xff, Gouraud()},
        {0,    2048, 1.0f, 0.0f, 0.0f, 0, 0, 0xff, 0xff, Gouraud()},
        {2048, 2048, 1.0f, 1.0f, 0.0f, 0, 0, 0xff, 0xff, Gouraud()},
        {0,    0,    0.0f, 0.0f, 0.0f, 0, 0, 0xff, 0xff, Gouraud()},
        {2048, 2048, 1.0f, 1.0f, 0.0f, 0, 0, 0xff, 0xff, Gouraud()},
        {2048, 0,    0.0f, 1.0f, 0.0f, 0, 0, 0xff, 0xff, Gouraud()}
    };

    // Sending vertex buffer data to the GPU
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertexes.size(), vertexes.data(), GL_STATIC_DRAW);

    // Drawing the list
    glDrawArrays(GL_TRIANGLES, 0, vertexes_per_tessellated_quad);

    // auto elements_buffer = u32{};
    // glGenBuffers(1, &elements_buffer); // This buffer will be used to send indices data to the GPU
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elements_buffer);

    // const auto&& [indices, draw_ranges] = generateVertexIndicesAndDrawRanges(parts_list);
    // const auto vertexes                 = readVertexes(parts_list);

    //// Sending data to the GPU
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * indices.size(), indices.data(), GL_STATIC_DRAW);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertexes.size(), vertexes.data(), GL_STATIC_DRAW);

    // for (const auto& range : draw_ranges) {
    //     const auto is_texture_used = GLboolean(range.is_textured);
    //     glUniform1i(texture_used_loc, is_texture_used);

    //    glDrawRangeElements(range.primitive,
    //                        range.vertex_array_start,
    //                        range.vertex_array_end,
    //                        range.indices_nb,
    //                        GL_UNSIGNED_INT,
    //                        static_cast<GLuint*>(nullptr) + range.indices_array_start);
    //}

    gl::glDeleteBuffers(1, &vertex_buffer);
    gl::glDeleteVertexArrays(1, &vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    // gl::glDeleteBuffers(1, &elements_buffer);
}

void OpenglRender::render() {
    if constexpr (uses_fbo) {
        renderByScreenPriority();
    } else {
        renderByParts();
    }
}

void OpenglRender::renderByScreenPriority() {
    MapOfPartsList global_parts_list;

    const auto getPartsFromThread = [&]() {
        std::lock_guard lock(opengl_->getMutex(MutexType::parts_list));
        if (!parts_lists_.empty()) { global_parts_list = std::move(parts_lists_); }
    };
    getPartsFromThread();

    opengl_->clearFboTextures();
    opengl_->clearFboKeys();

    // Rendering is done to FBOs depending on the priority and layer combo.
    for (const auto& [key, parts] : global_parts_list) {
        renderToAvailableTexture(key, parts);
    }

    preRender();

    // :TODO: Render the FBOs to the current framebuffer
    std::ranges::reverse_view rv{opengl_->fbo_key_to_fbo_pool_index_};
    for (const auto& [key, index] : rv) {
        renderFboTexture(opengl_->fbo_texture_pool_[index]);
    }

    postRender();

    const auto notifyMainThread = [this, &global_parts_list]() {
        std::lock_guard lk(opengl_->getMutex(MutexType::parts_list));
        MapOfPartsList().swap(global_parts_list);
        data_condition_.notify_one();
    };
    notifyMainThread();
}

void OpenglRender::renderByParts() {
    PartsList parts_list;

    preRender();

    const auto getParts = [this, &parts_list]() {
        std::lock_guard lock(opengl_->getMutex(MutexType::parts_list));
        if (!parts_lists_[mixed_parts_key].empty()) { parts_list = std::move(parts_lists_[mixed_parts_key]); }
    };
    getParts();

    renderParts(parts_list, opengl_->texturing()->getTextureArrayId());

    const auto notifyMainThread = [this, &parts_list]() {
        std::lock_guard lk(opengl_->getMutex(MutexType::parts_list));
        PartsList().swap(parts_list);
        data_condition_.notify_one();
    };
    notifyMainThread();

    postRender();
}

void OpenglRender::renderTest() {
    preRender();

    const auto [vao, vertex_buffer] = initializeVao();

    glUseProgram(shaders_.programs[ProgramShader::main]);
    glBindVertexArray(vao);                       // binding VAO
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer); // binding vertex buffer

    // Calculating the ortho projection matrix
    const auto proj_matrix = calculateDisplayViewportMatrix();

    // Sending the ortho projection matrix to the shader
    const auto uni_proj_matrix = glGetUniformLocation(shaders_.programs[ProgramShader::main], "proj_matrix");
    glUniformMatrix4fv(uni_proj_matrix, 1, GL_FALSE, glm::value_ptr(proj_matrix));

    glActiveTexture(GLenum::GL_TEXTURE0);
    const auto sampler_loc = glGetUniformLocation(shaders_.programs[ProgramShader::main], "sampler");
    // glUniform1i(sampler_loc, GLenum::GL_TEXTURE0);
    glUniform1i(sampler_loc, 0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, opengl_->texturing()->getTextureArrayId());

    const auto texture_used_loc = glGetUniformLocation(shaders_.programs[ProgramShader::main], "is_texture_used");
    // Sending the variable to configure the shader to use texture data.
    const auto is_texture_used = GLboolean(false);
    glUniform1i(texture_used_loc, is_texture_used);

    auto elements_buffer = u32{};
    glGenBuffers(1, &elements_buffer); // This buffer will be used to send indices data to the GPU
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elements_buffer);

    if constexpr (false) {
        // std::array<GLuint, 20> indices = {0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
        std::vector<GLuint> indices = {0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
        // std::span<GLuint>   indices(vindices);
        //      std::array<GLuint, 5> indices = {0, 1, 2, 3, 0};
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), &indices.front(), GL_STATIC_DRAW);

        auto vertexes = std::vector<Vertex>{
            {100, 100, 0.0f, 0.0f, 0.0f, 0xff, 0,    0,    0xff, Gouraud()},
            {100, 150, 1.0f, 0.0f, 0.0f, 0xff, 0,    0,    0xff, Gouraud()},
            {150, 150, 1.0f, 1.0f, 0.0f, 0xff, 0,    0,    0xff, Gouraud()},
            {150, 100, 0.0f, 1.0f, 0.0f, 0xff, 0,    0,    0xff, Gouraud()},

            {200, 200, 0.0f, 0.0f, 0.0f, 0,    0,    0xff, 0xff, Gouraud()},
            {200, 250, 1.0f, 0.0f, 0.0f, 0,    0,    0xff, 0xff, Gouraud()},
            {250, 250, 1.0f, 1.0f, 0.0f, 0,    0,    0xff, 0xff, Gouraud()},
            {250, 200, 0.0f, 1.0f, 0.0f, 0,    0,    0xff, 0xff, Gouraud()},

            {50,  50,  0.0f, 0.0f, 0.0f, 0,    0xff, 0,    0xff, Gouraud()},
            {100, 100, 1.0f, 0.0f, 0.0f, 0,    0xff, 0,    0xff, Gouraud()},

            {70,  70,  0.0f, 0.0f, 0.0f, 0xff, 0,    0,    0xff, Gouraud()},
            {120, 120, 1.0f, 0.0f, 0.0f, 0xff, 0,    0,    0xff, Gouraud()},

            {70,  70,  0.0f, 0.0f, 0.0f, 0,    0,    0xff, 0xff, Gouraud()},
            {70,  120, 1.0f, 0.0f, 0.0f, 0,    0,    0xff, 0xff, Gouraud()},
            {120, 120, 1.0f, 1.0f, 0.0f, 0,    0,    0xff, 0xff, Gouraud()},
            {120, 70,  0.0f, 1.0f, 0.0f, 0,    0,    0xff, 0xff, Gouraud()}
        };

        // Sending data to the GPU
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertexes.size(), vertexes.data(), GL_STATIC_DRAW);

        glDrawRangeElements(GL_TRIANGLES, 0, 7, 12, GL_UNSIGNED_INT, static_cast<GLuint*>(nullptr));
        glDrawRangeElements(GL_LINES, 8, 11, 4, GL_UNSIGNED_INT, static_cast<GLuint*>(nullptr) + 12);
        glDrawRangeElements(GL_LINE_LOOP, 12, 15, 4, GL_UNSIGNED_INT, static_cast<GLuint*>(nullptr) + 16);
    } else {
        auto parts = PartsList{};

        auto render_part                      = Vdp1Part{};
        render_part.common_vdp_data_.vertexes = {
            {100, 100, 0.0f, 0.0f, 0.0f, 0xff, 0, 0, 0xff, Gouraud()},
            {100, 150, 1.0f, 0.0f, 0.0f, 0xff, 0, 0, 0xff, Gouraud()},
            {150, 150, 1.0f, 1.0f, 0.0f, 0xff, 0, 0, 0xff, Gouraud()},
            {150, 100, 0.0f, 1.0f, 0.0f, 0xff, 0, 0, 0xff, Gouraud()}
        };
        render_part.common_vdp_data_.draw_type    = DrawType::non_textured_polygon;
        render_part.common_vdp_data_.vdp_type     = VdpType::vdp1;
        render_part.common_vdp_data_.color_offset = {
            {true, true, true},
            {0x80, 0x80, 0x80}
        };

        parts.emplace_back(render_part);

        render_part.common_vdp_data_.vertexes = {
            {200, 200, 0.0f, 0.0f, 0.0f, 0, 0, 0xff, 0xff, Gouraud()},
            {200, 250, 1.0f, 0.0f, 0.0f, 0, 0, 0xff, 0xff, Gouraud()},
            {250, 250, 1.0f, 1.0f, 0.0f, 0, 0, 0xff, 0xff, Gouraud()},
            {250, 200, 0.0f, 1.0f, 0.0f, 0, 0, 0xff, 0xff, Gouraud()}
        };
        render_part.common_vdp_data_.draw_type    = DrawType::non_textured_polygon;
        render_part.common_vdp_data_.vdp_type     = VdpType::vdp1;
        render_part.common_vdp_data_.color_offset = {
            {1, 1, 1},
            {0, 0, 0}
        };
        parts.emplace_back(render_part);

        render_part.common_vdp_data_.vertexes = {
            {50,  50,  0.0f, 0.0f, 0.0f, 0, 0xff, 0, 0xff, Gouraud()},
            {100, 100, 1.0f, 0.0f, 0.0f, 0, 0xff, 0, 0xff, Gouraud()}
        };
        render_part.common_vdp_data_.draw_type = DrawType::line;
        render_part.common_vdp_data_.vdp_type  = VdpType::vdp1;
        // render_part.common_vdp_data_.color_offset = {0x50, 0x50, 0x50};
        parts.emplace_back(render_part);

        render_part.common_vdp_data_.vertexes = {
            {70,  70,  0.0f, 0.0f, 0.0f, 0xff, 0, 0, 0xff, Gouraud()},
            {120, 120, 1.0f, 0.0f, 0.0f, 0xff, 0, 0, 0xff, Gouraud()}
        };
        render_part.common_vdp_data_.draw_type = DrawType::line;
        render_part.common_vdp_data_.vdp_type  = VdpType::vdp1;
        // render_part.common_vdp_data_.color_offset = {0x50, 0x50, 0x50};
        parts.emplace_back(render_part);

        render_part.common_vdp_data_.vertexes = {
            {70,  70,  0.0f, 0.0f, 0.0f, 0, 0, 0xff, 0xff, Gouraud()},
            {70,  120, 1.0f, 0.0f, 0.0f, 0, 0, 0xff, 0xff, Gouraud()},
            {120, 120, 1.0f, 1.0f, 0.0f, 0, 0, 0xff, 0xff, Gouraud()},
            {120, 70,  0.0f, 1.0f, 0.0f, 0, 0, 0xff, 0xff, Gouraud()}
        };
        render_part.common_vdp_data_.draw_type = DrawType::polyline;
        render_part.common_vdp_data_.vdp_type  = VdpType::vdp1;
        // render_part.common_vdp_data_.color_offset = {0x50, 0x50, 0x50};
        parts.emplace_back(render_part);

        const auto&& [indices, draw_ranges] = generateVertexIndicesAndDrawRanges(parts);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);

        const auto vertexes = opengl_->texturing()->readVertexes(parts);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertexes.size(), vertexes.data(), GL_STATIC_DRAW);

        for (const auto& range : draw_ranges) {
            const auto is_texture_used = GLboolean(range.is_textured);
            glUniform1i(texture_used_loc, is_texture_used);

            glDrawRangeElements(range.primitive,
                                range.vertex_array_start,
                                range.vertex_array_end,
                                range.indices_nb,
                                GL_UNSIGNED_INT,
                                static_cast<GLuint*>(nullptr) + range.indices_array_start);
        }
    }

    gl::glDeleteBuffers(1, &vertex_buffer);
    gl::glDeleteVertexArrays(1, &vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    gl::glDeleteBuffers(1, &elements_buffer);

    postRender();
}

void OpenglRender::renderVdp1DebugOverlay() {
    //----------- Pre render -----------//
    glBindFramebuffer(GLenum::GL_FRAMEBUFFER, opengl_->texturing()->getFboId(FboType::general));

    opengl_->texturing()->attachTextureLayerToFbo(opengl_->texturing()->getFboTextureArrayId(),
                                                  opengl_->texturing()->getFboTextureLayer(FboTextureType::vdp1_debug_overlay),
                                                  GLenum::GL_FRAMEBUFFER,
                                                  GLenum::GL_COLOR_ATTACHMENT0);

    // Viewport is the entire Saturn framebuffer
    glViewport(0, 0, saturn_framebuffer_width, saturn_framebuffer_height);

    gl::glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Scissor test calculation, to remove from display everything outside the current Saturn resolution.
    glEnable(GL_SCISSOR_TEST);
    auto [scissor_x, scissor_y, scissor_width, scissor_height] = calculateViewportPosAndSize();
    glScissor(scissor_x, scissor_y, scissor_width, scissor_height);

    //----------- Render -----------------//
    // Calculating the ortho projection matrix
    const auto [vao_simple, vertex_buffer_simple] = initializeVao();
    const auto proj_matrix                        = calculateDisplayViewportMatrix();

    auto part = partToHighlight();

    auto vertexes = std::vector<Vertex>{};

    if (!part.common_vdp_data_.vertexes.empty()) {
        const auto debug_color                  = VertexColor({0xff, 0, 0, 0xff});
        part.common_vdp_data_.vertexes[0].color = debug_color;
        part.common_vdp_data_.vertexes[1].color = debug_color;
        part.common_vdp_data_.vertexes[2].color = debug_color;
        part.common_vdp_data_.vertexes[3].color = debug_color;

        vertexes.reserve(vertexes_per_tessellated_quad);
        vertexes.emplace_back(part.common_vdp_data_.vertexes[0]);
        vertexes.emplace_back(part.common_vdp_data_.vertexes[1]);
        vertexes.emplace_back(part.common_vdp_data_.vertexes[2]);
        vertexes.emplace_back(part.common_vdp_data_.vertexes[0]);
        vertexes.emplace_back(part.common_vdp_data_.vertexes[2]);
        vertexes.emplace_back(part.common_vdp_data_.vertexes[3]);

        glUseProgram(shaders_.programs[ProgramShader::main]);
        glBindVertexArray(vao_simple);                       // binding VAO
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_simple); // binding vertex buffer

        // Sending vertex buffer data to the GPU
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertexes.size(), vertexes.data(), GL_STATIC_DRAW);

        // Sending the ortho projection matrix to the shader
        const auto uni_proj_matrix = glGetUniformLocation(shaders_.programs[ProgramShader::main], "proj_matrix");
        glUniformMatrix4fv(uni_proj_matrix, 1, GL_FALSE, glm::value_ptr(proj_matrix));

        // Sending the variable to configure the shader to use color.
        const auto uni_use_texture = glGetUniformLocation(shaders_.programs[ProgramShader::main], "is_texture_used");
        const auto is_texture_used = GLboolean(false);
        glUniform1i(uni_use_texture, is_texture_used);

        // Drawing the list
        glDrawArrays(GL_TRIANGLES, 0, vertexes_per_tessellated_quad);
    }

    //------ Post render --------//

    glDisable(GL_SCISSOR_TEST);
    glBindFramebuffer(GLenum::GL_FRAMEBUFFER, 0);
};

void OpenglRender::renderVdp2DebugLayer(core::EmulatorContext& state) {
    //----------- Pre render -----------//
    glBindFramebuffer(GLenum::GL_FRAMEBUFFER, opengl_->texturing()->getFboId(FboType::vdp2_debug));

    // Viewport is the entire Saturn framebuffer
    glViewport(0, 0, saturn_framebuffer_width, saturn_framebuffer_height);

    gl::glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    //----------- Render -----------------//
    PartsList parts_list;
    if (state.vdp2()->screenInDebug() != video::ScrollScreen::none) {
        const auto vdp2_parts = state.vdp2()->vdp2Parts(state.vdp2()->screenInDebug(), VdpType::vdp2_cell);
        if (!vdp2_parts.empty()) {
            parts_list.reserve(parts_list.size() + vdp2_parts.size());
            for (const auto& p : vdp2_parts) {
                parts_list.emplace_back(p);
            }
        }
    }

    if (!parts_list.empty()) { renderParts(parts_list, opengl_->texturing()->getTextureArrayId()); }

    //------ Post render --------//
    // Framebuffer is released
    gl::glBindFramebuffer(GL_FRAMEBUFFER, 0);
};

void OpenglRender::renderSelector() {
    if constexpr (render_type == RenderType::RenderType_drawElements) { render(); }
    if constexpr (render_type == RenderType::RenderType_drawTest) { renderTest(); }
};

auto OpenglRender::isThereSomethingToRender() const -> bool {
    if constexpr (render_type == RenderType::RenderType_drawElements) {
        if constexpr (uses_fbo) {
            return !parts_lists_.empty();
        } else {
            if (parts_lists_.contains(mixed_parts_key)) { return !parts_lists_.at(mixed_parts_key).empty(); }
            return false;
        }
    }
    if constexpr (render_type == RenderType::RenderType_drawTest) { return true; }
}

void OpenglRender::switchRenderedBuffer() {
    using enum FboTextureType;
    current_rendered_buffer_ = (current_rendered_buffer_ == back_buffer) ? front_buffer : back_buffer;
}

auto OpenglRender::calculateDisplayViewportMatrix() const -> glm::highp_mat4 {
    const auto host_res     = opengl_->hostScreenResolution();
    const auto host_ratio   = static_cast<float>(host_res.width) / static_cast<float>(host_res.height);
    const auto saturn_res   = opengl_->saturnScreenResolution();
    const auto saturn_ratio = static_cast<float>(saturn_res.width) / static_cast<float>(saturn_res.height);

    // If the Saturn resolution isn't set yet, calculation is aborted
    if ((saturn_res.height == 0) || (saturn_res.width == 0)) { return glm::mat4{}; }

    auto projection = glm::mat4{};
    auto view       = glm::mat4{1.0f};

    if (host_ratio >= saturn_ratio) {
        // Pillarbox display (wide viewport), use full height
        projection = glm::mat4{
            glm::ortho(0.0f, host_ratio / saturn_ratio * saturn_res.width, 0.0f, static_cast<float>(saturn_res.height))};

        // Centering the viewport
        const auto empty_zone = host_res.width - saturn_res.width * host_res.height / saturn_res.height;
        const auto amount     = static_cast<float>(empty_zone) / host_res.width;
        view                  = glm::translate(view, glm::vec3(amount, 0.0f, 0.0f));

    } else {
        // Letterbox display (tall viewport) use full width
        projection = glm::mat4{
            glm::ortho(0.f, static_cast<float>(saturn_res.width), 0.f, saturn_ratio / host_ratio * saturn_res.height)};

        // Centering the viewport
        const auto empty_zone = host_res.height - saturn_res.height * host_res.width / saturn_res.width;
        const auto amount     = static_cast<float>(empty_zone) / host_res.height;
        view                  = glm::translate(view, glm::vec3(0.0f, amount, 0.0f));
    }

    return view * projection;
}

auto OpenglRender::calculateViewportPosAndSize() const -> std::tuple<u32, u32, u32, u32> {
    const auto host_res     = opengl_->hostScreenResolution();
    const auto host_ratio   = static_cast<float>(host_res.width) / static_cast<float>(host_res.height);
    const auto saturn_res   = opengl_->saturnScreenResolution();
    const auto saturn_ratio = static_cast<float>(saturn_res.width) / static_cast<float>(saturn_res.height);

    // Viewport position and size
    auto x      = u32{};
    auto y      = u32{};
    auto width  = u32{};
    auto height = u32{};

    if (host_ratio >= saturn_ratio) {
        // Pillarbox display (wide viewport), use full height
        if ((saturn_res.height != 0) && (host_res.width != 0)) {
            const auto empty_zone = host_res.width - saturn_res.width * host_res.height / saturn_res.height;
            x                     = empty_zone * saturn_framebuffer_width / host_res.width / 2;
            y                     = 0;
            width                 = (host_res.width - empty_zone) * saturn_framebuffer_width / host_res.width;
            height                = saturn_framebuffer_height;
        }
    } else {
        // Letterbox display (tall viewport) use full width
        if ((saturn_res.width != 0) && (host_res.height != 0)) {
            const auto empty_zone = host_res.height - saturn_res.height * host_res.width / saturn_res.width;
            x                     = 0;
            y                     = empty_zone * saturn_framebuffer_height / host_res.height / 2;
            width                 = saturn_framebuffer_width;
            height                = (host_res.height - empty_zone) * saturn_framebuffer_height / host_res.height;
        }
    }

    return std::make_tuple(x, y, width, height);
}

void OpenglRender::displayFramebuffer(core::EmulatorContext& state) {
    if constexpr (uses_fbo) {
        displayFramebufferByScreenPriority(state);
    } else {
        displayFramebufferByParts(state);
    }
}

void OpenglRender::displayFramebufferByScreenPriority(core::EmulatorContext& state) {
    // - Render each layer + priority to one specific FBO
    // - Put render() content in its own function, and name it renderToAvailableFbo()
    // - Render() itself must take care of rendering the different FBOs in order
    // - Find which FBO to use in the FBO priority list
    // - displayFramebuffer() must generate one PartList by layer + priority to display to the FBO

    MapOfPartsList global_parts_list;

    // Step one : get displayable layers
    using enum ScrollScreen;
    std::vector<ScrollScreen> screens_to_display;
    if (!state.vdp2()->isLayerDisabled(nbg3)) screens_to_display.push_back(nbg3);
    if (!state.vdp2()->isLayerDisabled(nbg2)) screens_to_display.push_back(nbg2);
    if (!state.vdp2()->isLayerDisabled(nbg1)) screens_to_display.push_back(nbg1);
    if (!state.vdp2()->isLayerDisabled(nbg0)) screens_to_display.push_back(nbg0);
    if (!state.vdp2()->isLayerDisabled(rbg1)) screens_to_display.push_back(rbg1);
    if (!state.vdp2()->isLayerDisabled(rbg0)) screens_to_display.push_back(rbg0);

    // Step two : populate parts list for each priority + layer couple
    const auto addVdp1PartsToList = [&](const u8 priority) {
        auto        local_parts = PartsList();
        const auto& vdp1_parts  = state.vdp1()->vdp1Parts(priority);
        if (!vdp1_parts.empty()) {
            local_parts.reserve(vdp1_parts.size());
            for (const auto& p : vdp1_parts) {
                local_parts.emplace_back(p);
            }
            global_parts_list[{priority, VdpLayer::sprite}] = std::move(local_parts);
            // Sprite layer is recalculated every time, there's no cache for now.
            opengl_->setFboTextureStatus(priority, VdpLayer::sprite, FboTextureStatus::to_clear);
        }
    };

    const auto addVdp2PartsToList = [&](const ScrollScreen screen, const u32 priority) {
        auto        local_parts = PartsList();
        const auto& vdp2_parts  = state.vdp2()->vdp2Parts(screen, priority);
        if (!vdp2_parts.empty()) {
            local_parts.reserve(vdp2_parts.size());
            for (const auto& p : vdp2_parts) {
                local_parts.emplace_back(p);
            }
            global_parts_list[{priority, screen_to_layer.at(screen)}] = std::move(local_parts);
        }
    };

    for (u8 priority = 7; priority > 0; --priority) {
        for (const auto& screen : screens_to_display) {
            addVdp2PartsToList(screen, priority);
        }
        addVdp1PartsToList(priority);
    }

    // Step three : wait to send data to the render thread.
    if constexpr (render_type == RenderType::RenderType_drawElements) {
        if (parts_lists_.empty()) {
            std::unique_lock lk(opengl_->getMutex(MutexType::parts_list));
            parts_lists_ = std::move(global_parts_list);
            data_condition_.wait(lk, [this]() { return parts_lists_.empty(); });
        }
    }
}

void OpenglRender::displayFramebufferByParts(core::EmulatorContext& state) {
    auto parts_list = PartsList{};

    const auto addVdp2PartsToList = [&](const ScrollScreen s) {
        if (const auto& vdp2_planes = state.vdp2()->vdp2Parts(s, VdpType::vdp2_cell); !vdp2_planes.empty()) {
            parts_list.reserve(parts_list.size() + vdp2_planes.size());
            for (const auto& p : vdp2_planes) {
                parts_list.emplace_back(p);
            }
        }

        const auto& vdp2_bitmaps = state.vdp2()->vdp2Parts(s, VdpType::vdp2_bitmap);
        if (!vdp2_bitmaps.empty()) {
            parts_list.reserve(parts_list.size() + vdp2_bitmaps.size());
            for (const auto& p : vdp2_bitmaps) {
                parts_list.emplace_back(p);
            }
        }
    };

    const auto addVdp1PartsToList = [&]() {
        const auto& vdp1_parts = state.vdp1()->vdp1Parts();
        if (!vdp1_parts.empty()) {
            parts_list.reserve(parts_list.size() + vdp1_parts.size());
            for (const auto& p : vdp1_parts) {
                parts_list.emplace_back(p);
            }
        }
    };

    if (!state.vdp2()->isLayerDisabled(ScrollScreen::nbg3)) { addVdp2PartsToList(ScrollScreen::nbg3); }
    if (!state.vdp2()->isLayerDisabled(ScrollScreen::nbg2)) { addVdp2PartsToList(ScrollScreen::nbg2); }
    if (!state.vdp2()->isLayerDisabled(ScrollScreen::nbg1)) { addVdp2PartsToList(ScrollScreen::nbg1); }
    if (!state.vdp2()->isLayerDisabled(ScrollScreen::nbg0)) { addVdp2PartsToList(ScrollScreen::nbg0); }
    if (!state.vdp2()->isLayerDisabled(ScrollScreen::rbg1)) { addVdp2PartsToList(ScrollScreen::rbg1); }
    if (!state.vdp2()->isLayerDisabled(ScrollScreen::rbg0)) { addVdp2PartsToList(ScrollScreen::rbg0); }
    addVdp1PartsToList();
    std::ranges::stable_sort(parts_list, [](const RenderPart& a, const RenderPart& b) { return a.priority < b.priority; });
    if constexpr (render_type == RenderType::RenderType_drawElements) {
        if (parts_lists_[mixed_parts_key].empty()) {
            std::unique_lock lk(opengl_->getMutex(MutexType::parts_list));
            parts_lists_[mixed_parts_key] = std::move(parts_list);
            data_condition_.wait(lk, [this]() { return parts_lists_[mixed_parts_key].empty(); });
        }
    }
}

} // namespace saturnin::video
