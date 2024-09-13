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
#include <saturnin/src/video/opengl/opengl.h>
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
#include <saturnin/src/video/vdp2.h>

namespace saturnin::video {

using namespace gl21;
using namespace gl21ext;

using core::tr;

void Opengl::preRender() {
    switchRenderedBuffer();

    glBindFramebuffer(GLenum::GL_FRAMEBUFFER, fbo_type_to_id_[FboType::general]);

    attachTextureLayerToFbo(fbo_texture_array_id_,
                            getFboTextureLayer(currentRenderedBuffer()),
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

void Opengl::postRender() {
    // Framebuffer is released
    checkGlError();

    glDisable(GL_SCISSOR_TEST);

    glBindFramebuffer(GLenum::GL_FRAMEBUFFER, 0);
};

void Opengl::renderToAvailableTexture(const FboKey& key, const PartsList& parts_list) {
    using enum Logger;
    Log::debug(opengl, "renderToAvailableFbo() call");
    const auto index = getAvailableFboTextureIndex();
    if (!index) {
        Log::debug(opengl, "- No FBO available for rendering");
        Log::debug(opengl, "renderToAvailableFbo() return");
        return;
    }

    const auto& [priority, layer] = key;
    Log::debug(opengl, "- Rendering key [priority={}, layer={}] to FBO with index {}", priority, layer_to_name.at(layer), *index);

    renderParts(parts_list, fbo_texture_pool_[*index]);

    Log::debug(opengl, "- Changing FBO status at index {} to 'reuse'", *index);

    fbo_texture_pool_status_[*index] = FboTextureStatus::reuse;
    fbo_key_to_fbo_pool_index_[key]  = *index;

    Log::debug(opengl, "renderToAvailableFbo() return");
}

void Opengl::renderParts(const PartsList& parts_list, const u32 texture_id) {
    if (!parts_list.empty()) {
        const auto [vao, vertex_buffer] = initializeVao();

        glUseProgram(program_shaders_[ProgramShader::main]);
        glBindVertexArray(vao);                       // binding VAO
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer); // binding vertex buffer

        // Calculating the ortho projection matrix
        const auto proj_matrix = calculateDisplayViewportMatrix();

        // Sending the ortho projection matrix to the shader
        const auto uni_proj_matrix = glGetUniformLocation(program_shaders_[ProgramShader::main], "proj_matrix");
        glUniformMatrix4fv(uni_proj_matrix, 1, GL_FALSE, glm::value_ptr(proj_matrix));

        glActiveTexture(GLenum::GL_TEXTURE0);
        const auto sampler_loc = glGetUniformLocation(program_shaders_[ProgramShader::main], "sampler");

        glUniform1i(sampler_loc, 0);
        glBindTexture(GL_TEXTURE_2D_ARRAY, texture_id);

        const auto texture_used_loc = glGetUniformLocation(program_shaders_[ProgramShader::main], "is_texture_used");

        auto elements_buffer = u32{};
        glGenBuffers(1, &elements_buffer); // This buffer will be used to send indices data to the GPU
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elements_buffer);

        const auto&& [indices, draw_ranges] = generateVertexIndicesAndDrawRanges(parts_list);
        const auto vertexes                 = readVertexes(parts_list);

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

void Opengl::renderFboTexture(const u32 texture_id) {
    const auto [vao, vertex_buffer] = initializeVao();

    glUseProgram(program_shaders_[ProgramShader::main]);
    glBindVertexArray(vao);                       // binding VAO
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer); // binding vertex buffer

    // Calculating the ortho projection matrix
    const auto proj_matrix = calculateDisplayViewportMatrix();

    // Sending the ortho projection matrix to the shader
    const auto uni_proj_matrix = glGetUniformLocation(program_shaders_[ProgramShader::main], "proj_matrix");
    glUniformMatrix4fv(uni_proj_matrix, 1, GL_FALSE, glm::value_ptr(proj_matrix));

    glActiveTexture(GLenum::GL_TEXTURE0);
    const auto sampler_loc = glGetUniformLocation(program_shaders_[ProgramShader::main], "sampler");

    glUniform1i(sampler_loc, 0);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    const auto texture_used_loc = glGetUniformLocation(program_shaders_[ProgramShader::main], "is_texture_used");
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

void Opengl::render() {
    if constexpr (uses_fbo) {
        GlobalPartsList global_parts_list;

        const auto getPartsFromThread = [&]() {
            std::lock_guard lock(parts_list_mutex_);
            if (!global_parts_list_.empty()) { global_parts_list = std::move(global_parts_list_); }
        };
        getPartsFromThread();

        clearFboTextures();
        clearFboKeys();

        // Rendering is done to FBOs depending on the priority and layer combo.
        for (const auto& [key, parts] : global_parts_list) {
            renderToAvailableTexture(key, parts);
        }

        preRender();

        // :TODO: Render the FBOs to the current framebuffer
        std::ranges::reverse_view rv{fbo_key_to_fbo_pool_index_};
        for (const auto& [key, index] : rv) {
            renderFboTexture(fbo_texture_pool_[index]);
        }

        postRender();

        const auto notifyMainThread = [this, &global_parts_list]() {
            std::lock_guard lk(parts_list_mutex_);
            GlobalPartsList().swap(global_parts_list);
            data_condition_.notify_one();
        };
        notifyMainThread();

    } else {
        PartsList parts_list;

        preRender();

        const auto getParts = [this, &parts_list]() {
            std::lock_guard lock(parts_list_mutex_);
            if (!parts_list_.empty()) { parts_list = std::move(parts_list_); }
        };
        getParts();

        renderParts(parts_list, texture_array_id_);

        const auto notifyMainThread = [this, &parts_list]() {
            std::lock_guard lk(parts_list_mutex_);
            PartsList().swap(parts_list);
            data_condition_.notify_one();
        };
        notifyMainThread();

        postRender();
    }
}

void Opengl::renderTest() {
    preRender();

    const auto [vao, vertex_buffer] = initializeVao();

    glUseProgram(program_shaders_[ProgramShader::main]);
    glBindVertexArray(vao);                       // binding VAO
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer); // binding vertex buffer

    // Calculating the ortho projection matrix
    const auto proj_matrix = calculateDisplayViewportMatrix();

    // Sending the ortho projection matrix to the shader
    const auto uni_proj_matrix = glGetUniformLocation(program_shaders_[ProgramShader::main], "proj_matrix");
    glUniformMatrix4fv(uni_proj_matrix, 1, GL_FALSE, glm::value_ptr(proj_matrix));

    glActiveTexture(GLenum::GL_TEXTURE0);
    const auto sampler_loc = glGetUniformLocation(program_shaders_[ProgramShader::main], "sampler");
    // glUniform1i(sampler_loc, GLenum::GL_TEXTURE0);
    glUniform1i(sampler_loc, 0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, texture_array_id_);

    const auto texture_used_loc = glGetUniformLocation(program_shaders_[ProgramShader::main], "is_texture_used");
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

        const auto vertexes = readVertexes(parts);
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

void Opengl::renderSelector() {
    if constexpr (render_type == RenderType::RenderType_drawElements) { render(); }
    if constexpr (render_type == RenderType::RenderType_drawTest) { renderTest(); }
}

auto Opengl::isThereSomethingToRender() const -> bool {
    if constexpr (render_type == RenderType::RenderType_drawElements) {
        if constexpr (uses_fbo) {
            return !global_parts_list_.empty();
        } else {
            return !parts_list_.empty();
        }
    }
    if constexpr (render_type == RenderType::RenderType_drawTest) { return true; }
}

void Opengl::renderVdp1DebugOverlay() {
    //----------- Pre render -----------//
    glBindFramebuffer(GLenum::GL_FRAMEBUFFER, fbo_type_to_id_[FboType::general]);

    attachTextureLayerToFbo(fbo_texture_array_id_,
                            getFboTextureLayer(FboTextureType::vdp1_debug_overlay),
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

        glUseProgram(program_shaders_[ProgramShader::main]);
        glBindVertexArray(vao_simple);                       // binding VAO
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_simple); // binding vertex buffer

        // Sending vertex buffer data to the GPU
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertexes.size(), vertexes.data(), GL_STATIC_DRAW);

        // Sending the ortho projection matrix to the shader
        const auto uni_proj_matrix = glGetUniformLocation(program_shaders_[ProgramShader::main], "proj_matrix");
        glUniformMatrix4fv(uni_proj_matrix, 1, GL_FALSE, glm::value_ptr(proj_matrix));

        // Sending the variable to configure the shader to use color.
        const auto uni_use_texture = glGetUniformLocation(program_shaders_[ProgramShader::main], "is_texture_used");
        const auto is_texture_used = GLboolean(false);
        glUniform1i(uni_use_texture, is_texture_used);

        // Drawing the list
        glDrawArrays(GL_TRIANGLES, 0, vertexes_per_tessellated_quad);
    }

    //------ Post render --------//

    glDisable(GL_SCISSOR_TEST);
    glBindFramebuffer(GLenum::GL_FRAMEBUFFER, 0);
};

void Opengl::renderVdp2DebugLayer(core::EmulatorContext& state) {
    //----------- Pre render -----------//
    glBindFramebuffer(GLenum::GL_FRAMEBUFFER, fbo_type_to_id_[FboType::vdp2_debug]);

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

    if (!parts_list.empty()) { renderParts(parts_list, texture_array_id_); }

    //------ Post render --------//
    // Framebuffer is released
    gl::glBindFramebuffer(GL_FRAMEBUFFER, 0);
};

} // namespace saturnin::video
