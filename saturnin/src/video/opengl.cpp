//
// opengl.cpp
// Saturnin
//
// Copyright (c) 2018 Renaud Toumazet
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
#include <saturnin/src/video/opengl.h>
#include <windows.h> // removes C4005 warning
#include <algorithm>
#include <fstream>    // ifstream
#include <filesystem> // filesystem
#include <iostream>   // cout
#include <lodepng.h>
#include <sstream> // stringstream
#include <glbinding/glbinding.h>
//#include <glbinding/gl/gl.h>
#include <glbinding/Version.h>
#include <glbinding-aux/ContextInfo.h>
#include <glbinding/gl21/gl.h>
#include <glbinding/gl21ext/gl.h>
#include <glbinding/gl33core/gl.h>
#include <glbinding/gl33ext/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <saturnin/src/config.h>
#include <saturnin/src/locale.h> // tr
#include <saturnin/src/log.h>
#include <saturnin/src/video/gui.h>
#include <saturnin/src/video/texture.h>
#include <saturnin/src/video/vdp_common.h>
#include <saturnin/src/video/vdp1.h>
#include <saturnin/src/video/base_rendering_part.h>
#include <saturnin/src/resource_holder.hpp>

// using namespace gl;
using namespace gl21;
using namespace gl21ext;

using namespace std::string_literals; // enables s-suffix for std::string literals

namespace saturnin::video {

using core::Log;
using core::Logger;
using core::tr;

constexpr auto vertexes_per_tessellated_quad = u32{6}; // 2 triangles
constexpr auto vertexes_per_polyline         = u32{4};
constexpr auto vertexes_per_line             = u32{2};

Opengl::Opengl(core::Config* config) { config_ = config; }

Opengl::~Opengl() { shutdown(); }

void Opengl::initialize() {
    is_legacy_opengl_ = config_->readValue(core::AccessKeys::cfg_rendering_legacy_opengl);

    hostScreenResolution(ScreenResolution{video::minimum_window_width, video::minimum_window_height});

    glbinding::initialize(glfwGetProcAddress);

    initializeFbos();
    initializeShaders();
    const auto vertex_textured   = createVertexShader(ShaderName::textured);
    const auto fragment_textured = createFragmentShader(ShaderName::textured);
    program_shader_              = createProgramShader(vertex_textured, fragment_textured);

    const auto shaders_to_delete = std::vector<u32>{vertex_textured, fragment_textured};
    deleteShaders(shaders_to_delete);
}

void Opengl::shutdown() {
    glDeleteProgram(program_shader_);
    if (is_legacy_opengl_) {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        for (const auto& [type, fbo_data] : fbo_list_) {
            glDeleteFramebuffersEXT(1, &fbo_data.first);
            deleteTexture(fbo_data.second);
        }
    } else {
        gl33core::glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
        for (const auto& [type, fbo_data] : fbo_list_) {
            glDeleteFramebuffers(1, &fbo_data.first);
            deleteTexture(fbo_data.second);
        }
    }
}

void Opengl::preRender() {
    // const auto current_index = 1 - displayed_texture_index_; // toggles the value
    switchRenderedBuffer();

    if (is_legacy_opengl_) {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, getFboId(currentRenderedBuffer()));
    } else {
        gl33core::glBindFramebuffer(GL_FRAMEBUFFER, getFboId(currentRenderedBuffer()));
    }

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
    if (is_legacy_opengl_) {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    } else {
        gl33core::glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    glDisable(GL_SCISSOR_TEST);
};

void Opengl::initializeShaders() {
    //-----------------------------
    // Vertex shaders
    //-----------------------------
    shaders_list_.try_emplace({GlslVersion::glsl_120, ShaderType::vertex, ShaderName::textured}, R"(
        #version 120

        attribute vec2 vtx_position;
        attribute vec2 vtx_tex_coord;
        attribute vec4 vtx_color;
        attribute vec4 vtx_grd_color;
        
        varying vec2   frg_tex_coord;
        varying vec4   frg_color; 
        varying vec4   frg_grd_color; 
        
        uniform mat4 proj_matrix;

        void main() {
            gl_Position     = proj_matrix * vec4(vtx_position, 0.0, 1.0);
            frg_tex_coord   = vec2(vtx_tex_coord);
            frg_color       = vtx_color;
            frg_grd_color   = vtx_grd_color;
        }
    )");

    shaders_list_.try_emplace({GlslVersion::glsl_330, ShaderType::vertex, ShaderName::textured}, R"(
        #version 330 core

        layout (location = 0) in vec2 vtx_position;
        layout (location = 1) in vec2 vtx_tex_coord;
        layout (location = 2) in vec4 vtx_color;
        layout (location = 3) in vec4 vtx_grd_color;

        out vec2 frg_tex_coord;
        out vec4 frg_color;
        out vec4 frg_grd_color;

        uniform mat4 proj_matrix;

        void main() {
            gl_Position     = proj_matrix * vec4(vtx_position, 0.0, 1.0);
            frg_tex_coord   = vec2(vtx_tex_coord);
            frg_color       = vtx_color;
            frg_grd_color   = vtx_grd_color;
        }
    )");

    //-----------------------------
    // Fragment shaders
    //-----------------------------

    shaders_list_.try_emplace({GlslVersion::glsl_120, ShaderType::fragment, ShaderName::textured}, R"(
        #version 120
        
        varying vec2 frg_tex_coord;
        varying vec4 frg_color;
        varying vec4 frg_grd_color;

        uniform sampler2D texture1;
        uniform bool is_texture_used;

        void main()
        {
            if(is_texture_used){            
                gl_FragColor = texture2D(texture1,frg_tex_coord);
            }else{
                gl_FragColor = frg_color;
            }
        }
    )");

    shaders_list_.try_emplace({GlslVersion::glsl_330, ShaderType::fragment, ShaderName::textured}, R"(
        #version 330 core
        
        in vec2 frg_tex_coord;
        in vec4 frg_color;
        in vec4 frg_grd_color;

        out vec4 out_color;

        uniform sampler2D texture1;
        uniform bool is_texture_used;

        //vec4 test = vec4(1.0,0.0,0.0,1.0);
        
        void main()
        {
            if(is_texture_used){
                out_color = texture(texture1, frg_tex_coord);
            }else{
                out_color = frg_color;
            }
            out_color.rgb += frg_grd_color.rgb;
        } 
    )");
}

void Opengl::displayFramebuffer(core::EmulatorContext& state) {
    // if (is_saturn_data_available_) return;

    using PartsList = std::vector<std::unique_ptr<video::BaseRenderingPart>>;
    auto parts_list = PartsList{};

    const auto addVdp2PartsToList = [&](const ScrollScreen s) {
        // const auto& vdp2_parts = state.vdp2()->vdp2Parts(s);
        auto vdp2_parts = state.vdp2()->vdp2Parts(s);
        if (!vdp2_parts.empty()) {
            parts_list.reserve(parts_list.size() + vdp2_parts.size());
            for (const auto& p : vdp2_parts) {
                parts_list.emplace_back(std::make_unique<Vdp2Part>(p));
            }
        }
    };

    const auto addVdp1PartsToList = [&]() {
        const auto& vdp1_parts = state.vdp1()->vdp1Parts();
        if (!vdp1_parts.empty()) {
            parts_list.reserve(parts_list.size() + vdp1_parts.size());
            for (const auto& p : vdp1_parts) {
                parts_list.push_back(std::make_unique<Vdp1Part>(p));
            }
        }
    };

    if (!state.vdp2()->isLayerDisabled(ScrollScreen::nbg0)) { addVdp2PartsToList(ScrollScreen::nbg0); };
    if (!state.vdp2()->isLayerDisabled(ScrollScreen::nbg1)) { addVdp2PartsToList(ScrollScreen::nbg1); };
    if (!state.vdp2()->isLayerDisabled(ScrollScreen::nbg2)) { addVdp2PartsToList(ScrollScreen::nbg2); };
    if (!state.vdp2()->isLayerDisabled(ScrollScreen::nbg3)) { addVdp2PartsToList(ScrollScreen::nbg3); };
    if (!state.vdp2()->isLayerDisabled(ScrollScreen::rbg0)) { addVdp2PartsToList(ScrollScreen::rbg0); };
    if (!state.vdp2()->isLayerDisabled(ScrollScreen::rbg1)) { addVdp2PartsToList(ScrollScreen::rbg1); };
    addVdp1PartsToList();
    std::sort(parts_list.begin(),
              parts_list.end(),
              [](const std::unique_ptr<BaseRenderingPart>& a, const std::unique_ptr<BaseRenderingPart>& b) {
                  return a->priority() < b->priority();
              });

    if (parts_list_.empty()) {
        std::unique_lock lk(parts_list_mutex_);
        parts_list_ = std::move(parts_list);
        data_condition_.wait(lk, [&]() { return parts_list_.empty(); });
    }
}

void Opengl::render() {
    std::vector<std::unique_ptr<video::BaseRenderingPart>> parts_list;

    preRender();
    {
        std::lock_guard lock(parts_list_mutex_);
        // Log::info(Logger::main, "parts_list_ : {}", parts_list_.size());
        if (!parts_list_.empty()) { parts_list = std::move(parts_list_); }
    }

    if (!parts_list.empty()) {
        const auto [vao, vertex_buffer] = initializeVao(ShaderName::textured);

        glUseProgram(program_shader_);
        glBindVertexArray(vao);                       // binding VAO
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer); // binding vertex buffer

        // Calculating the ortho projection matrix
        const auto proj_matrix = calculateDisplayViewportMatrix();

        // Sending the ortho projection matrix to the shader
        const auto uni_proj_matrix = glGetUniformLocation(program_shader_, "proj_matrix");
        glUniformMatrix4fv(uni_proj_matrix, 1, GL_FALSE, glm::value_ptr(proj_matrix));

        const auto uni_use_texture = glGetUniformLocation(program_shader_, "is_texture_used");

        for (const auto& part : parts_list) {
            if (part->vertexes_.empty()) { continue; }

            switch (part->drawType()) {
                case DrawType::textured_polygon: {
                    // Quad is tessellated into 2 triangles, using a texture

                    auto vertexes = std::vector<Vertex>{};

                    vertexes.reserve(vertexes_per_tessellated_quad);
                    // Transforming one quad in 2 triangles
                    vertexes.emplace_back(part->vertexes_[0]);
                    vertexes.emplace_back(part->vertexes_[1]);
                    vertexes.emplace_back(part->vertexes_[2]);
                    vertexes.emplace_back(part->vertexes_[0]);
                    vertexes.emplace_back(part->vertexes_[2]);
                    vertexes.emplace_back(part->vertexes_[3]);

                    // Sending vertex buffer data to the GPU
                    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertexes.size(), vertexes.data(), GL_STATIC_DRAW);
                    // glActiveTexture(GLenum::GL_TEXTURE0);

                    // Sending the variable to configure the shader to use texture data.
                    // const auto uni_use_texture = glGetUniformLocation(program_shader_, "is_texture_used");
                    const auto is_texture_used = GLboolean(true);
                    glUniform1i(uni_use_texture, is_texture_used);

                    const auto id = getTextureId(part->textureKey());
                    if (id.has_value()) { glBindTexture(GL_TEXTURE_2D, *id); }

                    glDrawArrays(GL_TRIANGLES, 0, vertexes_per_tessellated_quad);
                    break;
                }
                case DrawType::non_textured_polygon: {
                    // Quad is tessellated into 2 triangles, using color
                    auto vertexes = std::vector<Vertex>{};

                    vertexes.reserve(vertexes_per_tessellated_quad);
                    // Transforming one quad in 2 triangles
                    vertexes.emplace_back(part->vertexes_[0]);
                    vertexes.emplace_back(part->vertexes_[1]);
                    vertexes.emplace_back(part->vertexes_[2]);
                    vertexes.emplace_back(part->vertexes_[0]);
                    vertexes.emplace_back(part->vertexes_[2]);
                    vertexes.emplace_back(part->vertexes_[3]);

                    // Sending vertex buffer data to the GPU
                    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertexes.size(), vertexes.data(), GL_STATIC_DRAW);

                    // Sending the variable to configure the shader to use color.
                    // const auto uni_use_texture = glGetUniformLocation(program_shader_, "is_texture_used");
                    const auto is_texture_used = GLboolean(false);
                    glUniform1i(uni_use_texture, is_texture_used);

                    // Drawing the list, rendering 2 triangles (one quad) at a time while changing the current texture
                    glDrawArrays(GL_TRIANGLES, 0, vertexes_per_tessellated_quad);
                    break;
                }
                case DrawType::polyline: {
                    // Quad is drawn using LINE_LOOP (4 vertexes)
                    auto vertexes = std::vector<Vertex>{};
                    vertexes.reserve(vertexes_per_polyline);
                    vertexes.emplace_back(part->vertexes_[0]);
                    vertexes.emplace_back(part->vertexes_[1]);
                    vertexes.emplace_back(part->vertexes_[2]);
                    vertexes.emplace_back(part->vertexes_[3]);

                    // Sending vertex buffer data to the GPU
                    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertexes.size(), vertexes.data(), GL_STATIC_DRAW);

                    // Sending the variable to configure the shader to use color.
                    // const auto uni_use_texture = glGetUniformLocation(program_shader_, "is_texture_used");
                    const auto is_texture_used = GLboolean(false);
                    glUniform1i(uni_use_texture, is_texture_used);

                    // Drawing the list
                    glDrawArrays(GL_LINE_LOOP, 0, vertexes_per_polyline);
                    break;
                }
                case DrawType::line: {
                    // Single line (2 vertexes)
                    auto vertexes = std::vector<Vertex>{};
                    vertexes.reserve(vertexes_per_polyline);
                    vertexes.emplace_back(part->vertexes_[0]);
                    vertexes.emplace_back(part->vertexes_[1]);

                    // Sending vertex buffer data to the GPU
                    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertexes.size(), vertexes.data(), GL_STATIC_DRAW);

                    // Sending the variable to configure the shader to use color.
                    // const auto uni_use_texture = glGetUniformLocation(program_shader_, "is_texture_used");
                    const auto is_texture_used = GLboolean(false);
                    glUniform1i(uni_use_texture, is_texture_used);

                    // Drawing the list
                    glDrawArrays(GL_LINES, 0, vertexes_per_line);
                    break;
                }
                default: {
                    break;
                }
            }
        }

        // glDeleteBuffers(elements_nb, vertex_buffer_ids_array.data());
        // glDeleteVertexArrays(elements_nb, vao_ids_array.data());
        glDeleteBuffers(1, &vertex_buffer);
        glDeleteVertexArrays(1, &vao);

        // Texture::cleanCache(this);
    }

    {
        std::lock_guard lk(parts_list_mutex_);
        PartsList().swap(parts_list_);
        data_condition_.notify_one();
    }

    postRender();
    calculateFps();
}

auto Opengl::isThereSomethingToRender() -> bool { return !parts_list_.empty(); }

auto Opengl::getRenderedBufferTextureId() -> u32 { return getFboTextureId(current_rendered_buffer_); }

void Opengl::renderVdp1DebugOverlay() {
    //----------- Pre render -----------//
    if (is_legacy_opengl_) {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, getFboId(FboType::vdp1_debug_overlay));
    } else {
        gl33core::glBindFramebuffer(GL_FRAMEBUFFER, getFboId(FboType::vdp1_debug_overlay));
    }

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
    const auto [vao_simple, vertex_buffer_simple] = initializeVao(ShaderName::textured);
    const auto proj_matrix                        = calculateDisplayViewportMatrix();

    auto part = partToHighlight();

    auto vertexes = std::vector<Vertex>{};

    if (!part.vertexes_.empty()) {
        const auto debug_color  = VertexColor({0xff, 0, 0, 0xff});
        part.vertexes_[0].color = debug_color;
        part.vertexes_[1].color = debug_color;
        part.vertexes_[2].color = debug_color;
        part.vertexes_[3].color = debug_color;

        vertexes.reserve(vertexes_per_tessellated_quad);
        vertexes.emplace_back(part.vertexes_[0]);
        vertexes.emplace_back(part.vertexes_[1]);
        vertexes.emplace_back(part.vertexes_[2]);
        vertexes.emplace_back(part.vertexes_[0]);
        vertexes.emplace_back(part.vertexes_[2]);
        vertexes.emplace_back(part.vertexes_[3]);

        glUseProgram(program_shader_);
        glBindVertexArray(vao_simple);                       // binding VAO
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_simple); // binding vertex buffer

        // Sending vertex buffer data to the GPU
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertexes.size(), vertexes.data(), GL_STATIC_DRAW);

        // Sending the ortho projection matrix to the shader
        const auto uni_proj_matrix = glGetUniformLocation(program_shader_, "proj_matrix");
        glUniformMatrix4fv(uni_proj_matrix, 1, GL_FALSE, glm::value_ptr(proj_matrix));

        // Sending the variable to configure the shader to use color.
        const auto uni_use_texture = glGetUniformLocation(program_shader_, "is_texture_used");
        const auto is_texture_used = GLboolean(false);
        glUniform1i(uni_use_texture, is_texture_used);

        // Drawing the list
        glDrawArrays(GL_TRIANGLES, 0, vertexes_per_tessellated_quad);
    }

    //------ Post render --------//
    // Framebuffer is released
    if (is_legacy_opengl_) {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    } else {
        gl33core::glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    glDisable(GL_SCISSOR_TEST);
};

void Opengl::renderVdp2DebugLayer(core::EmulatorContext& state) {
    //----------- Pre render -----------//
    if (is_legacy_opengl_) {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, getFboId(FboType::vdp2_debug_layer));
    } else {
        gl33core::glBindFramebuffer(GL_FRAMEBUFFER, getFboId(FboType::vdp2_debug_layer));
    }

    // Viewport is the entire Saturn framebuffer
    glViewport(0, 0, saturn_framebuffer_width, saturn_framebuffer_height);

    gl::glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    //----------- Render -----------------//
    std::vector<std::unique_ptr<video::BaseRenderingPart>> parts_list;
    if (state.vdp2()->screenInDebug() != video::ScrollScreen::none) {
        const auto& vdp2_parts = state.vdp2()->vdp2Parts(state.vdp2()->screenInDebug());
        if (!vdp2_parts.empty()) {
            parts_list.reserve(parts_list.size() + vdp2_parts.size());
            for (const auto& p : vdp2_parts) {
                parts_list.push_back(std::make_unique<Vdp2Part>(p));
            }
        }

        // parts_list = state.vdp2()->vdp2Parts(state.vdp2()->screenInDebug());
    }
    if (!parts_list.empty()) {
        constexpr auto vertexes_per_tessellated_quad = u32{6}; // 2 triangles
        // constexpr auto elements_nb                   = u8{2};

        // Calculating the ortho projection matrix
        const auto [vao, vertex_buffer] = initializeVao(ShaderName::textured);
        const auto proj_matrix          = calculateDisplayViewportMatrix();

        // const auto vao_ids_array           = std::array<u32, elements_nb>{vao};
        // const auto vertex_buffer_ids_array = std::array<u32, elements_nb>{vertex_buffer};

        for (const auto& part : parts_list) {
            if (part->vertexes_.empty()) { continue; }

            // Quad is tessellated into 2 triangles, using a texture

            auto vertexes = std::vector<Vertex>{};

            vertexes.reserve(vertexes_per_tessellated_quad);
            // Transforming one quad in 2 triangles
            vertexes.emplace_back(part->vertexes_[0]);
            vertexes.emplace_back(part->vertexes_[1]);
            vertexes.emplace_back(part->vertexes_[2]);
            vertexes.emplace_back(part->vertexes_[0]);
            vertexes.emplace_back(part->vertexes_[2]);
            vertexes.emplace_back(part->vertexes_[3]);

            glUseProgram(program_shader_);
            glBindVertexArray(vao);                       // binding VAO
            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer); // binding vertex buffer

            // Sending vertex buffer data to the GPU
            glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertexes.size(), vertexes.data(), GL_STATIC_DRAW);
            glActiveTexture(GLenum::GL_TEXTURE0);

            // Sending the ortho projection matrix to the shader
            const auto uni_proj_matrix = glGetUniformLocation(program_shader_, "proj_matrix");
            glUniformMatrix4fv(uni_proj_matrix, 1, GL_FALSE, glm::value_ptr(proj_matrix));

            // Sending the variable to configure the shader to use texture data.
            const auto uni_use_texture = glGetUniformLocation(program_shader_, "is_texture_used");
            const auto is_texture_used = GLboolean(true);
            glUniform1i(uni_use_texture, is_texture_used);

            // Drawing the list, rendering 2 triangles (one quad) at a time while changing the current texture
            // if (Texture::isTextureStored(part->textureKey())) {
            //    auto& t = Texture::getTexture(part->textureKey());
            //    // if (t.deleteOnGpu() || t.apiHandle() == 0) {
            //    //    // Creation / replacement of the texture on the GPU
            //    //    if (t.deleteOnGpu()) {
            //    //        deleteTexture(t.apiHandle());
            //    //        t.deleteOnGpu(false);
            //    //    }
            //    //    t.apiHandle(generateTexture(t.width(), t.height(), t.rawData()));
            //    //}
            //    t.apiHandle(generateTexture(t.width(), t.height(), t.rawData()));
            //    glBindTexture(GL_TEXTURE_2D, t.apiHandle());
            //}

            // glBindTexture(GL_TEXTURE_2D, texture_key_id_link_[part->textureKey()]);
            const auto id = getTextureId(part->textureKey());
            if (id.has_value()) { glBindTexture(GL_TEXTURE_2D, *id); }

            glDrawArrays(GL_TRIANGLES, 0, vertexes_per_tessellated_quad);
        }

        // glDeleteBuffers(elements_nb, vertex_buffer_ids_array.data());
        // glDeleteVertexArrays(elements_nb, vao_ids_array.data());
        glDeleteBuffers(1, &vertex_buffer);
        glDeleteVertexArrays(1, &vao);
    }

    //------ Post render --------//
    // Framebuffer is released
    if (is_legacy_opengl_) {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    } else {
        gl33core::glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
};

void Opengl::addOrUpdateTexture(const size_t key) {
    // If the key doesn't exist it will be automatically added.
    const auto texture_id = getTextureId(key);
    if (texture_id && (*texture_id > 0)) {
        std::lock_guard lock(texture_delete_mutex_);
        textures_to_delete_.push_back(*texture_id);
    }

    std::lock_guard lock(texture_link_mutex_);
    texture_key_id_link_[key] = 0;
}

void Opengl::generateTextures() {
    auto local_textures_to_delete = std::vector<u32>();
    {
        // vector is swapped to a local copy to keep the data structure locked for the minimum possible time.
        std::lock_guard lock(texture_delete_mutex_);
        local_textures_to_delete.swap(textures_to_delete_);
    }
    for (const auto id : local_textures_to_delete) {
        deleteTexture(id);
    }

    auto local_texture_key_id_link = std::unordered_map<size_t, u32>();
    {
        // unordered_map is copied locally to keep the data structure locked for the minimum possible time.
        std::lock_guard tl_lock(texture_link_mutex_);
        local_texture_key_id_link = texture_key_id_link_;
    }
    for (auto& [key, id] : local_texture_key_id_link) {
        if (id == 0) {
            const auto& t = Texture::getTexture(key);
            if (t) {
                const auto texture_id = generateTexture(t->width(), t->height(), t->rawData());

                std::lock_guard tl_lock(texture_link_mutex_);
                texture_key_id_link_[key] = texture_id;
            }
        }
    }
}

auto Opengl::getTextureId(const size_t key) -> std::optional<u32> {
    // std::lock_guard lock(texture_link_mutex_);
    // auto it = std::find_if(texture_key_id_link_.begin(), texture_key_id_link_.end(), [&key](const std::pair<size_t, u32>& v) {
    //     return v.first == key;
    // });
    // return (it != texture_key_id_link_.end()) ? std::optional<u32>(it->second) : std::nullopt;

    std::lock_guard lock(texture_link_mutex_);
    const auto      it = texture_key_id_link_.find(key);
    return (it == texture_key_id_link_.end()) ? std::nullopt : std::optional<u32>(it->second);
}

void Opengl::onWindowResize(const u16 width, const u16 height) { hostScreenResolution({width, height}); }

void Opengl::updateScreenResolution() {}

auto Opengl::createVertexShader(const ShaderName name) -> u32 {
    const auto vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    auto       source        = getShaderSource(ShaderType::vertex, name);
    glShaderSource(vertex_shader, 1, &source, nullptr);
    glCompileShader(vertex_shader);
    checkShaderCompilation(vertex_shader);

    return vertex_shader;
}

auto Opengl::createFragmentShader(const ShaderName name) -> u32 {
    const auto fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    auto       source          = getShaderSource(ShaderType::fragment, name);
    glShaderSource(fragment_shader, 1, &source, nullptr);
    glCompileShader(fragment_shader);

    checkShaderCompilation(fragment_shader);

    return fragment_shader;
}

// static
auto Opengl::createProgramShader(const u32 vertex_shader, const u32 fragment_shader) -> u32 {
    const auto shader_program = glCreateProgram();

    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);

    // glBindAttribLocation() calls are only needed for glsl 120 shader as the glsl 330 uses the "location" attribute.
    glBindAttribLocation(shader_program, 0, "vtx_position");
    glBindAttribLocation(shader_program, 1, "vtx_tex_coord");
    glBindAttribLocation(shader_program, 2, "vtx_color");
    glBindAttribLocation(shader_program, 3, "vtx_grd_color");

    glLinkProgram(shader_program);
    checkProgramCompilation(shader_program);

    return shader_program;
}

// static
void Opengl::deleteShaders(std::vector<u32> shaders) {
    for (auto shader : shaders) {
        glDeleteShader(shader);
    }
}

// static
auto Opengl::generateTexture(const u32 width, const u32 height, const std::vector<u8>& data) -> u32 {
    auto texture = u32{};
    glGenTextures(1, &texture);
    glActiveTexture(GLenum::GL_TEXTURE0);
    glBindTexture(GLenum::GL_TEXTURE_2D, texture);

    // set the texture wrapping parameters
    glTexParameteri(GLenum::GL_TEXTURE_2D, GLenum::GL_TEXTURE_WRAP_S, GLenum::GL_REPEAT);
    glTexParameteri(GLenum::GL_TEXTURE_2D, GLenum::GL_TEXTURE_WRAP_T, GLenum::GL_REPEAT);

    // set texture filtering parameters
    glTexParameteri(GLenum::GL_TEXTURE_2D, GLenum::GL_TEXTURE_MIN_FILTER, GLenum::GL_NEAREST);
    glTexParameteri(GLenum::GL_TEXTURE_2D, GLenum::GL_TEXTURE_MAG_FILTER, GLenum::GL_NEAREST);

    glTexImage2D(GLenum::GL_TEXTURE_2D,
                 0,
                 GLenum::GL_RGBA,
                 width,
                 height,
                 0,
                 GLenum::GL_RGBA,
                 GLenum::GL_UNSIGNED_BYTE,
                 data.data());

    return texture;
}

// static
void Opengl::deleteTexture(const u32 texture) {
    //
    glDeleteTextures(1, &texture);
}

static void error_callback(int error, const char* description) { fprintf(stderr, "Error %d: %s\n", error, description); }

auto Opengl::getShaderSource(const ShaderType type, const ShaderName name) -> const char* {
    const auto glsl_version = (is_legacy_opengl_) ? GlslVersion::glsl_120 : GlslVersion::glsl_330;
    return shaders_list_[{glsl_version, type, name}];
}

auto Opengl::calculateDisplayViewportMatrix() -> glm::highp_mat4 {
    const auto host_res     = hostScreenResolution();
    const auto host_ratio   = static_cast<float>(host_res.width) / static_cast<float>(host_res.height);
    const auto saturn_res   = saturnScreenResolution();
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

// static
auto Opengl::initializeVao(const ShaderName name) -> std::tuple<u32, u32> {
    auto vao = u32{};
    glGenVertexArrays(1, &vao);

    // Creating the vertex buffer
    auto vertex_buffer = u32{};
    glGenBuffers(1, &vertex_buffer);

    // Binding the VAO
    glBindVertexArray(vao);

    //// Declaring vertex buffer data
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

    switch (name) {
        case ShaderName::textured: {
            // position pointer
            glVertexAttribPointer(0, 2, GLenum::GL_SHORT, GL_FALSE, sizeof(Vertex), 0); // NOLINT: this is an index
            glEnableVertexAttribArray(0);                                               // NOLINT: this is an index

            // texture coords pointer
            auto offset = GLintptr(sizeof(VertexPosition));
            glVertexAttribPointer(1, 2, GLenum::GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(offset));
            glEnableVertexAttribArray(1);

            // color pointer
            offset += GLintptr(sizeof(TextureCoordinates));
            glVertexAttribPointer(2, 4, GLenum::GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), reinterpret_cast<GLvoid*>(offset));
            glEnableVertexAttribArray(2);

            // gouraud color pointer
            offset += GLintptr(sizeof(VertexColor));
            glVertexAttribPointer(3, 3, GLenum::GL_BYTE, GL_TRUE, sizeof(Vertex), reinterpret_cast<GLvoid*>(offset));
            glEnableVertexAttribArray(3);
            break;
        }
    }
    glBindVertexArray(0);
    return std::make_tuple(vao, vertex_buffer);
}

void Opengl::initializeFbos() {
    // A FBO (and its related texture) is generated for every FboType.
    // Front and back buffers are switched every frame : one will be used as the last complete rendering by the GUI while the
    // other will be rendered to.

    initializeFbo(FboType::front_buffer);
    initializeFbo(FboType::back_buffer);
    initializeFbo(FboType::vdp1_debug_overlay);
    initializeFbo(FboType::vdp2_debug_layer);
    initializeFbo(FboType::priority_level_1);
    initializeFbo(FboType::priority_level_2);
    initializeFbo(FboType::priority_level_3);
    initializeFbo(FboType::priority_level_4);
    initializeFbo(FboType::priority_level_5);
    initializeFbo(FboType::priority_level_6);
    initializeFbo(FboType::priority_level_7);

    currentRenderedBuffer(FboType::front_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, getFboTextureId(currentRenderedBuffer()));
}

void Opengl::initializeFbo(const FboType type) {
    auto fbo     = u32{};
    auto texture = u32{};
    if (is_legacy_opengl_) {
        glGenFramebuffersEXT(1, &fbo);
    } else {
        gl33core::glGenFramebuffers(1, &fbo);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // Creating a texture for the color buffer
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA8,
                 saturn_framebuffer_width,
                 saturn_framebuffer_height,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 nullptr);

    // No need for mipmaps, they are turned off
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Attaching the color texture to the fbo
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0);

    if (is_legacy_opengl_) {
        const auto status = glCheckFramebufferStatusEXT(GLenum::GL_FRAMEBUFFER_EXT);
        if (status != gl::GLenum::GL_FRAMEBUFFER_COMPLETE_EXT) {
            Log::error(Logger::opengl, tr("Could not initialize framebuffer object !"));
            throw std::runtime_error("Opengl error !");
        }
    } else {
        const auto status = gl33core::glCheckFramebufferStatus(GLenum::GL_FRAMEBUFFER);
        if (status != gl::GLenum::GL_FRAMEBUFFER_COMPLETE) {
            Log::error(Logger::opengl, tr("Could not initialize framebuffer object !"));
            throw std::runtime_error("Opengl error !");
        }
    }
    // fbos_.emplace_back(fbo);
    // fbo_textures_.emplace_back(texture);
    fbo_list_.try_emplace(type, fbo, texture);

    currentRenderedBuffer(FboType::front_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, getFboTextureId(currentRenderedBuffer()));
}

void Opengl::calculateFps() {
    using namespace std::literals::chrono_literals;
    static auto time_elapsed = micro{0};
    static auto frames_count = u8{0};
    ++frames_count;
    static auto previous_frame_start   = std::chrono::steady_clock::time_point{};
    const auto  current_frame_start    = std::chrono::steady_clock::now();
    const auto  current_frame_duration = std::chrono::duration_cast<micro>(current_frame_start - previous_frame_start);

    time_elapsed += current_frame_duration;

    static auto max_frames = 0;

    if (time_elapsed > 1s) {
        if (max_frames == 0) {
            std::string    ts           = config_->readValue(core::AccessKeys::cfg_rendering_tv_standard);
            const auto     standard     = config_->getTvStandard(ts);
            constexpr auto max_fps_pal  = 50;
            constexpr auto max_fps_ntsc = 60;
            max_frames                  = (standard == TvStandard::pal) ? max_fps_pal : max_fps_ntsc;
        }
        fps_                 = fmt::format("{:d} / {}", frames_count, max_frames);
        previous_frame_start = current_frame_start;
        frames_count         = 0;
        time_elapsed         = micro{0};
    }
}

void Opengl::switchRenderedBuffer() {
    current_rendered_buffer_ = (current_rendered_buffer_ == FboType::back_buffer) ? FboType::front_buffer : FboType::back_buffer;
}

auto Opengl::calculateViewportPosAndSize() const -> std::tuple<u32, u32, u32, u32> {
    const auto host_res     = hostScreenResolution();
    const auto host_ratio   = static_cast<float>(host_res.width) / static_cast<float>(host_res.height);
    const auto saturn_res   = saturnScreenResolution();
    const auto saturn_ratio = static_cast<float>(saturn_res.width) / static_cast<float>(saturn_res.height);

    // Viewport position and size
    auto x      = u32{};
    auto y      = u32{};
    auto width  = u32{};
    auto height = u32{};

    if (host_ratio >= saturn_ratio) {
        // Pillarbox display (wide viewport), use full height
        const auto empty_zone = host_res.width - saturn_res.width * host_res.height / saturn_res.height;
        x                     = empty_zone * saturn_framebuffer_width / host_res.width / 2;
        y                     = 0;
        width                 = (host_res.width - empty_zone) * saturn_framebuffer_width / host_res.width;
        height                = saturn_framebuffer_height;
    } else {
        // Letterbox display (tall viewport) use full width
        const auto empty_zone = host_res.height - saturn_res.height * host_res.width / saturn_res.width;
        x                     = 0;
        y                     = empty_zone * saturn_framebuffer_height / host_res.height / 2;
        width                 = saturn_framebuffer_width;
        height                = (host_res.height - empty_zone) * saturn_framebuffer_height / host_res.height;
    }

    return std::make_tuple(x, y, width, height);
}

auto isModernOpenglCapable() -> bool {
    if (glfwInit() == GLFW_FALSE) { return false; }

    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    constexpr auto h_window_size = u16{1280};
    constexpr auto v_window_size = u16{720};

    auto window = glfwCreateWindow(h_window_size, v_window_size, "", nullptr, nullptr);
    if (window == nullptr) {
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);

    glbinding::initialize(glfwGetProcAddress);
    const auto version = glbinding::aux::ContextInfo::version();

    glfwDestroyWindow(window);
    glfwTerminate();

    Log::info(Logger::opengl, "Max version supported : {}", version.toString());

    return (version >= glbinding::Version(3, 3));
}

void windowCloseCallback(GLFWwindow* window) {
    auto state = reinterpret_cast<core::EmulatorContext*>(glfwGetWindowUserPointer(window));
    state->stopEmulation();
    state->renderingStatus(core::RenderingStatus::stopped);

    // Adding a delay to allow the thread to finish cleanly.
    using namespace std::this_thread;     // sleep_for
    using namespace std::chrono_literals; // ms
    const auto time_to_sleep = 20ms;
    sleep_for(time_to_sleep);
}

auto runOpengl(core::EmulatorContext& state) -> s32 {
    const bool is_legacy_opengl = state.config()->readValue(core::AccessKeys::cfg_rendering_legacy_opengl);

    // Setup window
    glfwSetErrorCallback(error_callback);
    if (glfwInit() == GLFW_FALSE) { return EXIT_FAILURE; }

    // Decide GL+GLSL versions
#if __APPLE__
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    if (!is_legacy_opengl) {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // Required on Mac
    }
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    if (!is_legacy_opengl) {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE,
                       GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
        // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // 3.0+ only
    }
#endif
    const std::string rendering_mode = (is_legacy_opengl) ? core::tr("Legacy") : core::tr("Modern");
    const std::string window_title
        = fmt::format(core::tr("Saturnin {0} - {1} rendering"), core::saturnin_version, rendering_mode);

    const auto window = createMainWindow(minimum_window_width, minimum_window_height, window_title);
    if (window == nullptr) { return EXIT_FAILURE; }
    state.openglWindow(window);

    // glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    // const auto render_window = glfwCreateWindow(1, 1, "invisible", nullptr, ihm_window);

    glfwSetWindowCloseCallback(window, windowCloseCallback);
    glfwSetWindowSizeCallback(window, windowSizeCallback);

    glfwSetWindowUserPointer(window, static_cast<void*>(&state));

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync
    const auto ico_16 = rh::embed("saturnin-ico-16.png");
    const auto ico_32 = rh::embed("saturnin-ico-32.png");
    const auto ico_48 = rh::embed("saturnin-ico-48.png");
    auto       icons  = std::array<GLFWimage, 3>{{loadPngImage(ico_16.data(), ico_16.size()),
                                           loadPngImage(ico_32.data(), ico_32.size()),
                                           loadPngImage(ico_48.data(), ico_48.size())}};
    glfwSetWindowIcon(window, 3, icons.data());

    glbinding::initialize(glfwGetProcAddress);

    // Setup Dear ImGui binding
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    auto flags = ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    (void)flags;
    // io.ConfigViewportDecorations
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
    // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Viewports

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    (is_legacy_opengl) ? ImGui_ImplOpenGL3_Init() : ImGui_ImplOpenGL3_Init(glsl_version);

    // Setup style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsClassic();

    // Load Fonts
    // (there is a default font, this is only if you want to change it. see extra_fonts/README.txt for more details)
    // ImGuiIO& io = ImGui::GetIO();
    // io.Fonts->AddFontDefault();
    // io.Fonts->AddFontFromFileTTF("../../extra_fonts/Cousine-Regular.ttf", 15.0f);
    // io.Fonts->AddFontFromFileTTF("../../extra_fonts/DroidSans.ttf", 16.0f);
    // io.Fonts->AddFontFromFileTTF("../../extra_fonts/Roboto-Medium.ttf", 16.0f);
    // io.Fonts->AddFontFromFileTTF("../../extra_fonts/ProggyTiny.ttf", 10.0f);
    // io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());

    // Adding glyphs that will be used as images in text
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontDefault();
    static const std::array<ImWchar, 3> icons_ranges = {0xe900, 0xe908, 0}; // Will not be copied by AddFont* so keep in scope.
    ImFontConfig                        config;
    config.MergeMode            = true;
    config.FontDataOwnedByAtlas = false;
    auto       data             = rh::embed("saturnin-icons.ttf");
    const auto glyph_offset     = ImVec2(0, 2);
    config.GlyphOffset          = glyph_offset;
    constexpr auto font_size    = 13.0f;

    io.Fonts->AddFontFromMemoryTTF((void*)data.data(), static_cast<u32>(data.size()), font_size, &config, icons_ranges.data());
    io.Fonts->Build();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    const auto clear_color = ImVec4{0.0f, 0.0f, 0.0f, 1.00f};

    updateMainWindowSizeAndRatio(window, minimum_window_width, minimum_window_height);

    // state.opengl()->initialize(window);
    state.opengl()->initialize();

    // Main loop
    while (glfwWindowShouldClose(window) == GLFW_FALSE) {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.

        glfwPollEvents();

        // Start the ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Rendering
        auto display_w = s32{};
        auto display_h = s32{};
        glfwMakeContextCurrent(window);
        glfwGetFramebufferSize(window, &display_w, &display_h);

        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        gui::buildGui(state);
        if (state.renderingStatus() != core::RenderingStatus::running) { glfwSetWindowShouldClose(window, GLFW_TRUE); }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        if ((io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) > 0) {
            if (ImGui::GetIO().KeyAlt) { //
                printf("");
            } // Set a debugger breakpoint here!

            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}

void updateMainWindowSizeAndRatio(GLFWwindow* window, const u32 width, const u32 height) {
    glfwSetWindowSizeLimits(window, width, height, GLFW_DONT_CARE, GLFW_DONT_CARE);
}

auto createMainWindow(const u32 width, const u32 height, const std::string title) -> GLFWwindow* {
    return glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
}

auto loadPngImage(const char* filename) -> GLFWimage {
    const auto full_path{std::filesystem::current_path() / "res" / filename};
    auto       image  = GLFWimage{};
    auto       width  = u32{};
    auto       height = u32{};
    const auto error  = lodepng_decode32_file(&(image.pixels), &width, &height, full_path.string().c_str());
    if (error != 0) {
        Log::warning(Logger::opengl, lodepng_error_text(error));
        return image;
    }
    image.width  = width;
    image.height = height;

    return image;
}

auto loadPngImage(const u8* data, const size_t size) -> GLFWimage {
    // const auto full_path{std::filesystem::current_path() / "res" / filename};
    auto       image  = GLFWimage{};
    auto       width  = u32{};
    auto       height = u32{};
    const auto error  = lodepng_decode32(&(image.pixels), &width, &height, data, size);
    if (error != 0) {
        Log::warning(Logger::opengl, lodepng_error_text(error));
        return image;
    }
    image.width  = width;
    image.height = height;

    return image;
}

void windowSizeCallback(GLFWwindow* window, int width, int height) {
    const auto state = reinterpret_cast<core::EmulatorContext*>(glfwGetWindowUserPointer(window));
    state->opengl()->onWindowResize(width, height);
}

void checkShaderCompilation(const u32 shader) {
    auto success = GLboolean{};
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    auto length = s32{};
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    if (success == GL_FALSE) {
        auto v = std::vector<char>(length);
        glGetShaderInfoLog(shader, length, nullptr, v.data());
        const auto info = std::string(v.begin(), v.end());

        auto type        = GLenum{};
        auto shader_type = std::string{};
        glGetShaderiv(shader, GL_SHADER_TYPE, &type);
        switch (type) {
            case GL_VERTEX_SHADER: shader_type = "Vertex shader"; break;
            case GL_FRAGMENT_SHADER: shader_type = "Fragment shader"; break;
            default: shader_type = "Unknown shader"; break;
        }
        Log::error(Logger::opengl, "{} compilation failed : {}", shader_type, info);
        throw std::runtime_error("Opengl error !");
    }
}

void checkProgramCompilation(const u32 program) {
    auto success = GLboolean{};
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    auto length = s32{};
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

    if (success == GL_FALSE) {
        auto v = std::vector<char>(length);
        glGetProgramInfoLog(program, length, nullptr, v.data());
        const auto info = std::string(v.begin(), v.end());

        Log::error(Logger::opengl, "Shader program link failed : {}", info);
        throw std::runtime_error("Opengl error !");
    }
}

}; // namespace saturnin::video
