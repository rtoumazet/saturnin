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
#include <windows.h>  // removes C4005 warning
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

// using namespace gl;
using namespace gl21;
using namespace gl21ext;

using namespace std::string_literals; // enables s-suffix for std::string literals

namespace saturnin::video {

using core::Log;
using core::Logger;
using core::tr;

Opengl::Opengl(core::Config* config) { config_ = config; }

Opengl::~Opengl() { shutdown(); }

void Opengl::initialize(GLFWwindow* gui_context) {
    is_legacy_opengl_ = config_->readValue(core::AccessKeys::cfg_rendering_legacy_opengl);

    guiRenderingContext(gui_context);
    hostScreenResolution(ScreenResolution{video::minimum_window_width, video::minimum_window_height});

    glbinding::initialize(glfwGetProcAddress);

    initializeFbos();
    initializeShaders();
    const auto vertex_textured   = createVertexShader(ShaderName::textured);
    const auto fragment_textured = createFragmentShader(ShaderName::textured);
    program_shader_textured_     = createProgramShader(vertex_textured, fragment_textured);

    const auto vertex_simple   = createVertexShader(ShaderName::simple);
    const auto fragment_simple = createFragmentShader(ShaderName::simple);
    program_shader_simple_     = createProgramShader(vertex_simple, fragment_simple);

    const auto shaders_to_delete = std::vector<u32>{vertex_textured, fragment_textured, vertex_simple, fragment_simple};
    deleteShaders(shaders_to_delete);
}

void Opengl::shutdown() {
    glDeleteProgram(program_shader_textured_);
    if (is_legacy_opengl_) {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        for (auto f : fbos_) {
            glDeleteFramebuffersEXT(1, &f);
        }
    } else {
        gl33core::glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
        for (auto f : fbos_) {
            gl33core::glDeleteFramebuffers(1, &f);
        }
    }
    for (auto t : fbo_textures_) {
        glDeleteTextures(1, &t);
    }
}

void Opengl::preRender() {
    const auto current_index = 1 - displayed_texture_index_; // toggles the value
    if (is_legacy_opengl_) {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbos_[current_index]);
    } else {
        gl33core::glBindFramebuffer(GL_FRAMEBUFFER, fbos_[current_index]);
    }

    // Viewport is the entire Saturn framebuffer
    glViewport(0, 0, saturn_framebuffer_width, saturn_framebuffer_height);

    gl::glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    const auto host_res     = hostScreenResolution();
    const auto host_ratio   = static_cast<float>(host_res.width) / static_cast<float>(host_res.height);
    const auto saturn_res   = saturnScreenResolution();
    const auto saturn_ratio = static_cast<float>(saturn_res.width) / static_cast<float>(saturn_res.height);

    // Scissor test calculation, to remove from display everything outside the current Saturn resolution.
    auto scissor_x      = u32{};
    auto scissor_y      = u32{};
    auto scissor_width  = u32{};
    auto scissor_height = u32{};

    if (host_ratio >= saturn_ratio) {
        // Pillarbox display (wide viewport), use full height
        const auto empty_zone = host_res.width - saturn_res.width * host_res.height / saturn_res.height;
        scissor_x             = empty_zone * saturn_framebuffer_width / host_res.width / 2;
        scissor_y             = 0;
        scissor_width         = (host_res.width - empty_zone) * saturn_framebuffer_width / host_res.width;
        scissor_height        = saturn_framebuffer_height;
    } else {
        // Letterbox display (tall viewport) use full width
        const auto empty_zone = host_res.height - saturn_res.height * host_res.width / saturn_res.width;
        scissor_x             = 0;
        scissor_y             = empty_zone * saturn_framebuffer_height / host_res.height / 2;
        scissor_width         = saturn_framebuffer_width;
        scissor_height        = (host_res.height - empty_zone) * saturn_framebuffer_height / host_res.height;
    }

    glEnable(GL_SCISSOR_TEST);
    glScissor(scissor_x, scissor_y, scissor_width, scissor_height);
};

void Opengl::postRender() {
    // Framebuffer is released
    if (is_legacy_opengl_) {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    } else {
        gl33core::glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    displayed_texture_index_ = 1 - displayed_texture_index_; // toggles the value

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
        attribute vec4 vtx_grd_color;
        varying vec2   frg_tex_coord;
        varying vec4   frg_grd_color; 
        uniform mat4 proj_matrix;

        void main() {
            gl_Position = proj_matrix * vec4(vtx_position, 0.0, 1.0);
            frg_tex_coord = vtx_tex_coord;
            frg_grd_color = vtx_grd_color;
        }
    )");

    shaders_list_.try_emplace({GlslVersion::glsl_330, ShaderType::vertex, ShaderName::textured}, R"(
        #version 330 core

        layout (location = 0) in vec2 vtx_position;
        layout (location = 1) in vec2 vtx_tex_coord;
        layout (location = 2) in vec2 vtx_color;
        layout (location = 3) in vec4 vtx_grd_color;
        uniform mat4 proj_matrix;

        out vec2 frg_tex_coord;
        out vec4 frg_color;
        out vec4 frg_grd_color;

        void main() {
            gl_Position = proj_matrix * vec4(vtx_position, 0.0, 1.0);
            frg_tex_coord = vec2(vtx_tex_coord);
            frg_color = vec4(vtx_color);
            frg_grd_color = vec4(vtx_grd_color);
        }
    )");

    // shaders_list_.try_emplace({GlslVersion::glsl_120, ShaderType::vertex, ShaderName::simple}, R"(
    //    #version 120

    //    attribute vec2 vtx_position;
    //    attribute vec4 vtx_color;
    //    varying vec4   frg_color;
    //    uniform mat4 proj_matrix;

    //    void main() {
    //        gl_Position = proj_matrix * vec4(vtx_position, 0.0, 1.0);
    //        frg_color = vtx_color;
    //    }
    //)");

    // shaders_list_.try_emplace({GlslVersion::glsl_330, ShaderType::vertex, ShaderName::simple}, R"(
    //    #version 330 core

    //    layout (location = 0) in vec2 vtx_position;
    //    layout (location = 1) in vec4 vtx_color;
    //    uniform mat4 proj_matrix;

    //    out vec4 frg_color;

    //    void main() {
    //        gl_Position = proj_matrix * vec4(vtx_position, 0.0, 1.0);
    //        frg_color = vec4(vtx_color);
    //    }
    //)");

    //-----------------------------
    // Fragment shaders
    //-----------------------------

    shaders_list_.try_emplace({GlslVersion::glsl_120, ShaderType::fragment, ShaderName::textured}, R"(
        #version 120
        
        varying vec4 frg_color;
        varying vec2 frg_tex_coord;
        varying vec4 frg_grd_color;

        uniform sampler2D texture1;

        void main()
        {
            gl_FragColor = texture2D(texture1,frg_tex_coord);
        }
    )");

    shaders_list_.try_emplace({GlslVersion::glsl_330, ShaderType::fragment, ShaderName::textured}, R"(
        #version 330 core
        
        in vec2 frg_tex_coord;
        in vec4 frg_grd_color;

        out vec4 frg_color;

        uniform sampler2D texture1;

        //vec4 test = vec4(1.0,0.0,0.0,1.0);
        
        void main()
        {
            frg_color = texture(texture1, frg_tex_coord);
            frg_color.r -= 0.1;
            frg_color.g -= 0.1;
            frg_color.b -= 0.1;
            //frg_color.a = 0.5;
        } 
    )");

    shaders_list_.try_emplace({GlslVersion::glsl_120, ShaderType::fragment, ShaderName::simple}, R"(
        #version 120
        
        varying vec4 frg_color;

        void main()
        {
            gl_FragColor = vec4(frg_color);
        }
    )");

    shaders_list_.try_emplace({GlslVersion::glsl_330, ShaderType::fragment, ShaderName::simple}, R"(
        #version 330 core
        
        in vec4 frg_color;
        out vec4 out_color;

        void main()
        {
            out_color = vec4(frg_color);
        } 
    )");
}

void Opengl::displayFramebuffer(core::EmulatorContext& state) {
    std::vector<std::unique_ptr<video::BaseRenderingPart>> parts_list;

    const auto addVdp2PartsToList = [&](const ScrollScreen s) {
        const auto& vdp2_parts = state.vdp2()->vdp2Parts(s);
        if (!vdp2_parts.empty()) {
            parts_list.reserve(parts_list.size() + vdp2_parts.size());
            for (auto&& p : vdp2_parts) {
                parts_list.push_back(std::make_unique<Vdp2Part>(p));
            }
        }
    };

    const auto addVdp1PartsToList = [&]() {
        const auto& vdp1_parts = state.vdp1()->vdp1Parts();
        if (!vdp1_parts.empty()) {
            parts_list.reserve(parts_list.size() + vdp1_parts.size());
            for (auto&& p : vdp1_parts) {
                parts_list.push_back(std::make_unique<Vdp1Part>(p));
            }
        }
    };

    addVdp2PartsToList(ScrollScreen::nbg0);
    addVdp2PartsToList(ScrollScreen::nbg1);
    addVdp2PartsToList(ScrollScreen::nbg2);
    addVdp2PartsToList(ScrollScreen::nbg3);
    addVdp2PartsToList(ScrollScreen::rbg0);
    addVdp2PartsToList(ScrollScreen::rbg1);
    std::sort(parts_list.begin(),
              parts_list.end(),
              [](const std::unique_ptr<BaseRenderingPart>& a, const std::unique_ptr<BaseRenderingPart>& b) {
                  return a->priority() < b->priority();
              });

    addVdp1PartsToList();

    // :TODO: Ordering needs to be done depending on priorities

    {
        std::lock_guard<std::mutex> lock(parts_list_mutex_);
        // If the destination vector isn't empty, it means rendering isn't finished.
        // In that case current frame data is dropped.
        if (parts_list_.empty()) { parts_list_ = std::move(parts_list); }
    }
}

void Opengl::render() {
    std::vector<std::unique_ptr<video::BaseRenderingPart>> parts_list;

    preRender();
    {
        std::lock_guard<std::mutex> lock(parts_list_mutex_);
        if (!parts_list_.empty()) { parts_list = std::move(parts_list_); }
    }

    if (!parts_list.empty()) {
        constexpr auto vertexes_per_tessellated_quad = u32{6}; // 2 triangles
        constexpr auto vertexes_per_polyline         = u32{4};
        constexpr auto vertexes_per_line             = u32{2};

        const auto [vao_textured, vertex_buffer_textured] = initializeVao(ShaderName::textured);
        const auto [vao_simple, vertex_buffer_simple]     = initializeVao(ShaderName::simple);

        constexpr auto elements_nb             = u8{2};
        const auto     vao_ids_array           = std::array<u32, elements_nb>{vao_textured, vao_simple};
        const auto     vertex_buffer_ids_array = std::array<u32, elements_nb>{vertex_buffer_textured, vertex_buffer_simple};

        // Calculating the ortho projection matrix
        const auto proj_matrix = calculateDisplayViewportMatrix();

        for (const auto& part : parts_list) {
            if (part->vertexes_.empty()) continue;

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

                    glUseProgram(program_shader_textured_);
                    glBindVertexArray(vao_textured);                       // binding VAO
                    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_textured); // binding vertex buffer

                    // Sending vertex buffer data to the GPU
                    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertexes.size(), vertexes.data(), GL_STATIC_DRAW);
                    glActiveTexture(GLenum::GL_TEXTURE0);

                    // Sending the ortho projection matrix to the shader
                    const auto uni_proj_matrix = glGetUniformLocation(program_shader_textured_, "proj_matrix");
                    glUniformMatrix4fv(uni_proj_matrix, 1, GL_FALSE, glm::value_ptr(proj_matrix));

                    // Drawing the list, rendering 2 triangles (one quad) at a time while changing the current texture
                    auto& t = Texture::getTexture(part->textureKey());
                    if (t.deleteOnGpu() || t.apiHandle() == 0) {
                        // Creation / replacement of the texture on the GPU
                        if (t.deleteOnGpu()) { deleteTexture(t.apiHandle()); }
                        t.apiHandle(generateTexture(t.width(), t.height(), t.rawData()));
                    }

                    glBindTexture(GL_TEXTURE_2D, t.apiHandle());
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

                    glUseProgram(program_shader_simple_);
                    glBindVertexArray(vao_simple);                       // binding VAO
                    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_simple); // binding vertex buffer

                    // Sending vertex buffer data to the GPU
                    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertexes.size(), vertexes.data(), GL_STATIC_DRAW);

                    // Sending the ortho projection matrix to the shader
                    const auto uni_proj_matrix = glGetUniformLocation(program_shader_simple_, "proj_matrix");
                    glUniformMatrix4fv(uni_proj_matrix, 1, GL_FALSE, glm::value_ptr(proj_matrix));

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

                    glUseProgram(program_shader_simple_);
                    glBindVertexArray(vao_simple);                       // binding VAO
                    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_simple); // binding vertex buffer

                    // Sending vertex buffer data to the GPU
                    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertexes.size(), vertexes.data(), GL_STATIC_DRAW);

                    // Sending the ortho projection matrix to the shader
                    const auto uni_proj_matrix = glGetUniformLocation(program_shader_simple_, "proj_matrix");
                    glUniformMatrix4fv(uni_proj_matrix, 1, GL_FALSE, glm::value_ptr(proj_matrix));

                    // Drawing the list
                    glDrawArrays(GL_LINE_LOOP, 0, vertexes_per_polyline);
                    break;
                }
                case DrawType::line: {
                    // Single line (2 vertexes)
                    break;
                }
                default: {
                    break;
                }
            }
        }

        glDeleteBuffers(elements_nb, vertex_buffer_ids_array.data());
        glDeleteVertexArrays(elements_nb, vao_ids_array.data());
    }

    postRender();
    calculateFps();
}

auto Opengl::isThereSomethingToRender() -> const bool {
    std::lock_guard<std::mutex> lock(parts_list_mutex_);
    return !parts_list_.empty();
}

void Opengl::renderDebugVertexes() {
    //----------- Pre render -----------//
    if (is_legacy_opengl_) {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbos_[index_2]);
    } else {
        gl33core::glBindFramebuffer(GL_FRAMEBUFFER, fbos_[index_2]);
    }

    // Viewport is the entire Saturn framebuffer
    glViewport(0, 0, saturn_framebuffer_width, saturn_framebuffer_height);

    gl::glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    //----------- Render -----------------//
    // Calculating the ortho projection matrix
    const auto [vao_simple, vertex_buffer_simple] = initializeVao(ShaderName::simple);
    const auto proj_matrix                        = calculateDisplayViewportMatrix();

    auto part = partToHighlight();

    auto vertexes = std::vector<Vertex>{};

    if (!part.vertexes_.empty()) {
        const auto debug_color  = VertexColor({0xff, 0, 0, 0xff});
        part.vertexes_[0].color = debug_color;
        part.vertexes_[1].color = debug_color;
        part.vertexes_[2].color = debug_color;
        part.vertexes_[3].color = debug_color;

        vertexes.reserve(6);
        vertexes.emplace_back(part.vertexes_[0]);
        vertexes.emplace_back(part.vertexes_[1]);
        vertexes.emplace_back(part.vertexes_[2]);
        vertexes.emplace_back(part.vertexes_[0]);
        vertexes.emplace_back(part.vertexes_[2]);
        vertexes.emplace_back(part.vertexes_[3]);

        glUseProgram(program_shader_simple_);
        glBindVertexArray(vao_simple);                       // binding VAO
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_simple); // binding vertex buffer

        // Sending vertex buffer data to the GPU
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertexes.size(), vertexes.data(), GL_STATIC_DRAW);

        // Sending the ortho projection matrix to the shader
        const auto uni_proj_matrix = glGetUniformLocation(program_shader_simple_, "proj_matrix");
        glUniformMatrix4fv(uni_proj_matrix, 1, GL_FALSE, glm::value_ptr(proj_matrix));

        // Drawing the list
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    //------ Post render --------//
    // Framebuffer is released
    if (is_legacy_opengl_) {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    } else {
        gl33core::glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
};

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
void Opengl::deleteTexture(const u32 texture) { glDeleteTextures(1, &texture); }

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
        case ShaderName::simple: {
            // position pointer
            glVertexAttribPointer(0, 2, GLenum::GL_SHORT, GL_FALSE, sizeof(Vertex), 0); // NOLINT: this is an index
            glEnableVertexAttribArray(0);                                               // NOLINT: this is an index

            // color pointer
            auto offset = GLintptr(2 * sizeof(s16));
            glVertexAttribPointer(1, 4, GLenum::GL_UNSIGNED_BYTE, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(offset));
            glEnableVertexAttribArray(1);
            break;
        }
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
            glVertexAttribPointer(2, 4, GLenum::GL_UNSIGNED_BYTE, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(offset));
            glEnableVertexAttribArray(2);

            // gouraud color pointer
            offset += GLintptr(sizeof(VertexColor));
            glVertexAttribPointer(3, 3, GLenum::GL_BYTE, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(offset));
            glEnableVertexAttribArray(3);
            break;
        }
    }
    glBindVertexArray(0);
    return std::make_tuple(vao, vertex_buffer);
}

void Opengl::initializeFbos() {
    // 3 FBOs are generated.
    // One will be used as the last complete rendering by the GUI while the other will be rendered to.
    // The last one is used as a debug overlay
    constexpr auto fbos_number = u8{3};
    for (int i = 0; i < fbos_number; ++i) {
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
        fbos_.emplace_back(fbo);
        fbo_textures_.emplace_back(texture);
    }

    displayedTexture(index_0); // Setting the texture of first fbo used.
    glBindFramebuffer(GL_FRAMEBUFFER, fbos_[index_0]);
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
            std::string ts       = config_->readValue(core::AccessKeys::cfg_rendering_tv_standard);
            const auto  standard = config_->getTvStandard(ts);
            max_frames           = (standard == TvStandard::pal) ? 50 : 60;
        }
        fps_                 = fmt::format("{:d} / {}", frames_count, max_frames);
        previous_frame_start = current_frame_start;
        frames_count         = 0;
        time_elapsed         = micro{0};
    }
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

    const auto ihm_window = createMainWindow(minimum_window_width, minimum_window_height, window_title);
    if (ihm_window == nullptr) { return EXIT_FAILURE; }
    state.openglWindow(ihm_window);

    // glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    // const auto render_window = glfwCreateWindow(1, 1, "invisible", nullptr, ihm_window);

    glfwSetWindowCloseCallback(ihm_window, windowCloseCallback);
    glfwSetWindowSizeCallback(ihm_window, windowSizeCallback);

    glfwSetWindowUserPointer(ihm_window, static_cast<void*>(&state));

    glfwSetInputMode(ihm_window, GLFW_STICKY_KEYS, GLFW_TRUE);

    glfwMakeContextCurrent(ihm_window);
    glfwSwapInterval(1); // Enable vsync

    auto icons = std::array<GLFWimage, 3>{
        {loadPngImage("saturnin-ico-16.png"), loadPngImage("saturnin-ico-32.png"), loadPngImage("saturnin-ico-48.png")}};
    glfwSetWindowIcon(ihm_window, 3, icons.data());

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

    ImGui_ImplGlfw_InitForOpenGL(ihm_window, true);
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
    config.MergeMode = true;
    const auto font_path{std::filesystem::current_path() / "res" / "saturnin-icons.ttf"};
    const auto glyph_offset  = ImVec2(0, 2);
    config.GlyphOffset       = glyph_offset;
    constexpr auto font_size = 13.0f;
    io.Fonts->AddFontFromFileTTF(font_path.string().c_str(), font_size, &config, icons_ranges.data());
    io.Fonts->Build();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    const auto clear_color = ImVec4{0.0f, 0.0f, 0.0f, 1.00f};

    // Getting the right rendering context
    // auto opengl = std::make_unique<Opengl>(state.config());
    // state.opengl(opengl.get());
    // opengl->ihmRenderingContext(ihm_window);
    // opengl->initializeRenderingContext();

    updateMainWindowSizeAndRatio(ihm_window, minimum_window_width, minimum_window_height);

    // opengl->hostScreenResolution(ScreenResolution{minimum_window_width, minimum_window_height});

    state.opengl()->initialize(ihm_window);

    // Main loop
    while (glfwWindowShouldClose(ihm_window) == GLFW_FALSE) {
        // Start the ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Rendering
        auto display_w = s32{};
        auto display_h = s32{};
        glfwMakeContextCurrent(ihm_window);
        glfwGetFramebufferSize(ihm_window, &display_w, &display_h);

        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        gui::buildGui(state);
        if (state.renderingStatus() == core::RenderingStatus::reset) { glfwSetWindowShouldClose(ihm_window, GLFW_TRUE); }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        if ((ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) > 0) {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        glfwSwapBuffers(ihm_window);

        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.

        glfwPollEvents();
    }

    // Cleanup
    state.stopEmulation();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // opengl->shutdownRenderingContext();
    glfwDestroyWindow(ihm_window);
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

void windowSizeCallback(GLFWwindow* window, int width, int height) {
    const auto state = reinterpret_cast<core::EmulatorContext*>(glfwGetWindowUserPointer(window));

    state->opengl()->onWindowResize(width, height);
    Log::warning(Logger::opengl, "Window was resized: {} {}", width, height);
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
