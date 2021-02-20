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
#include <saturnin/src/video/opengl_modern.h>
#include <array> // array
#include <istream>
#include <vector> // vector

#include <glbinding/glbinding.h>
#include <glbinding/gl/gl.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <saturnin/src/emulator_context.h>
#include <saturnin/src/log.h>
#include <saturnin/src/locale.h>
#include <saturnin/src/video/gui.h>
#include <saturnin/src/video/opengl.h>
#include <saturnin/src/video/vdp2.h>

using namespace gl;

namespace saturnin::video {

using core::Log;
using core::tr;

void OpenglModern::initialize() {
    glGenFramebuffers(1, &saturn_framebuffer_);
    glBindFramebuffer(GL_FRAMEBUFFER, saturn_framebuffer_);

    // Creating a texture for the color buffer
    auto texture = u32{};
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 2048, 2048);

    // No need for mipmaps, they are turned off
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Attaching the color texture to the fbo
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0);

    static const GLenum draw_buffers[] = {GL_COLOR_ATTACHMENT0};

    renderingTexture(texture);

    const auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != gl::GLenum::GL_FRAMEBUFFER_COMPLETE) {
        Log::error("opengl", tr("Could not initialize framebuffer object !"));
        throw std::runtime_error("Opengl error !");
    }

    const auto vertex_shader     = createVertexShader();
    const auto fragment_shader   = createFragmentShader();
    program_shader_              = createProgramShader(vertex_shader, fragment_shader);
    const auto shaders_to_delete = std::vector<u32>{vertex_shader, fragment_shader};
    deleteShaders(shaders_to_delete);
}

void OpenglModern::shutdown() {
    glDeleteProgram(program_shader_);
    glDeleteFramebuffers(1, &saturn_framebuffer_);
    const auto texture = renderingTexture();
    glDeleteTextures(1, &texture);
}

/* static */
auto OpenglModern::createVertexShader() -> u32 {
    const char* vertex_shader_source = R"(
        #version 330 core

        in vec2 position;
        uniform mat4 proj_matrix;

        void main() {
            gl_Position = proj_matrix * vec4(position, 0.0, 1.0);
        }
    )";

    const auto vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, nullptr);
    glCompileShader(vertex_shader);
    checkShaderCompilation(vertex_shader);

    return vertex_shader;
}
/* static */
auto OpenglModern::createFragmentShader() -> u32 {
    const char* fragment_shader_source = R"(
        #version 330 core
        
        out vec4 color;
        
        void main()
        {
            //color = vec4(1.0f, 0.5f, 0.2f, 1.0f);
            color = vec4(1.0f, 0.5f, 0.2f, 1.0f);
        } 
    )";

    const auto fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, nullptr);
    glCompileShader(fragment_shader);

    checkShaderCompilation(fragment_shader);

    return fragment_shader;
}

/* static */
auto OpenglModern::createProgramShader(const u32 vertex_shader, const u32 fragment_shader) -> u32 {
    const auto shader_program = glCreateProgram();

    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    checkProgramCompilation(shader_program);

    return shader_program;
}

/* static */
void OpenglModern::deleteShaders(std::vector<u32> shaders) {
    for (auto shader : shaders) {
        glDeleteShader(shader);
    }
}

void OpenglModern::setupTriangle() {
    /* clang-format off */ 
    //constexpr std::array<u16, 18> vertices = {
    //    0,  0,  0, // 0
    //    0,  224, 0, // 1
    //    320, 224, 0, // 2

    //    0,  0, 0, // 0
    //    320, 224, 0, // 2
    //    320, 0, 0,  // 3
    //};
    constexpr std::array<u16, 12> vertices = {
        0,  0,   // 0
        0,  224, // 1
        320, 224, // 2

        0,  0, // 0
        320, 224, // 2
        320, 0,   // 3
    };

    /* clang-format on */

    glGenVertexArrays(1, &vao_);
    auto vertex_buffer = u32{};
    glGenBuffers(1, &vertex_buffer);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(vao_);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(decltype(vertexes_)::value_type) * vertexes_.size(), vertexes_.data(), GL_STATIC_DRAW);
    ;
    glVertexAttribPointer(0, 2, GLenum::GL_UNSIGNED_SHORT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer
    // object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying
    // other VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly
    // necessary.
    glBindVertexArray(0);
}
void OpenglModern::drawTriangle() {
    glUseProgram(program_shader_);
    glBindVertexArray(vao_); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep
                             // things a bit more organized

    const auto host_res     = hostScreenResolution();
    const auto host_ratio   = static_cast<float>(host_res.width) / static_cast<float>(host_res.height);
    const auto saturn_res   = saturnScreenResolution();
    const auto saturn_ratio = static_cast<float>(saturn_res.width) / static_cast<float>(saturn_res.height);

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

    const auto proj_matrix = view * projection;

    const auto uni_proj_matrix = glGetUniformLocation(program_shader_, "proj_matrix");
    glUniformMatrix4fv(uni_proj_matrix, 1, GL_FALSE, glm::value_ptr(proj_matrix));

    glDrawArrays(GL_TRIANGLES, 0, 12);
}

void OpenglModern::preRender() {
    glBindFramebuffer(GL_FRAMEBUFFER, saturn_framebuffer_);

    // Viewport is the entire Saturn framebuffer
    glViewport(0, 0, saturn_framebuffer_width, saturn_framebuffer_height);

    gl::glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
};

void OpenglModern::render() {
    this->setupTriangle();
    this->drawTriangle();
};

void OpenglModern::postRender() {
    // Framebuffer is released
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
};

static void error_callback(int error, const char* description) {
    Log::error("opengl", "Error {}: {}", error, description);
    throw std::runtime_error("Opengl error !");
}

}; // namespace saturnin::video
