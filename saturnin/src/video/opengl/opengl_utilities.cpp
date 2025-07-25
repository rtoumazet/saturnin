//
// opengl_utilities.cpp
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
#include <lodepng.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glbinding/glbinding.h>
#include <glbinding/Version.h>
#include <glbinding-aux/ContextInfo.h>
#include <glbinding/gl21/gl.h>
#include <glbinding/gl21ext/gl.h>
#include <glbinding/gl33core/gl.h>
#include <glbinding/gl33ext/gl.h>

namespace saturnin::video {

using namespace gl21;
using namespace gl21ext;

using core::tr;

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

    glbinding::initialize(glfwGetProcAddress, false);
    const auto version = glbinding::aux::ContextInfo::version();

    glfwDestroyWindow(window);
    glfwTerminate();

    Log::info(Logger::opengl, "Max version supported : {}", version.toString());

    return (version >= glbinding::Version(3, 3));
}

void updateMainWindowSizeAndRatio(GLFWwindow* window, const u32 width, const u32 height) {
    glfwSetWindowSizeLimits(window, width, height, GLFW_DONT_CARE, GLFW_DONT_CARE);
}

auto createMainWindow(const u32 width, const u32 height, const std::string& title) -> GLFWwindow* {
    return glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
}

void windowCloseCallback(GLFWwindow* window) {
    auto state = static_cast<core::EmulatorContext*>(glfwGetWindowUserPointer(window));
    state->stopEmulation();
    state->renderingStatus(core::RenderingStatus::stopped);

    // Adding a delay to allow the thread to finish cleanly.
    using namespace std::this_thread;     // sleep_for
    using namespace std::chrono_literals; // ms
    const auto time_to_sleep = 20ms;
    sleep_for(time_to_sleep);
}

auto loadPngImage(const char* filename) -> GLFWimage {
    const auto full_path{std::filesystem::current_path() / "res" / filename};
    auto       image  = GLFWimage{};
    auto       width  = u32{};
    auto       height = u32{};

    if (const auto error = lodepng_decode32_file(&(image.pixels), &width, &height, full_path.string().c_str()); error != 0) {
        Log::warning(Logger::opengl, lodepng_error_text(error));
        return image;
    }
    image.width  = width;
    image.height = height;

    return image;
}

auto loadPngImage(const u8* data, const size_t size) -> GLFWimage {
    auto image  = GLFWimage{};
    auto width  = u32{};
    auto height = u32{};

    if (const auto error = lodepng_decode32(&(image.pixels), &width, &height, data, size); error != 0) {
        Log::warning(Logger::opengl, lodepng_error_text(error));
        return image;
    }
    image.width  = width;
    image.height = height;

    return image;
}

void checkGlError() {
    if (check_gl_error) {
        const auto error = glGetError();
        if (error != GLenum::GL_NO_ERROR) { Log::warning(Logger::opengl, "OpenGL error : {}", (int)error); }
    }
}

void glDebugOutput(gl::GLenum                   source,
                   gl::GLenum                   type,
                   unsigned int                 id,
                   gl::GLenum                   severity,
                   [[maybe_unused]] int         length,
                   const char*                  message,
                   [[maybe_unused]] const void* userParam) {
    // ignore non-significant error/warning codes
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

    Log::warning(Logger::opengl, "---------------");
    Log::warning(Logger::opengl, "Debug message ({}): {}", id, message);

    auto source_str = std::string{};
    switch (source) {
        case GL_DEBUG_SOURCE_API: source_str = "API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM: source_str = "Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: source_str = "Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY: source_str = "Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION: source_str = "Application"; break;
        case GL_DEBUG_SOURCE_OTHER: {
            source_str = "Other";
            break;
        }
            [[fallthrough]];
        default: {
            source_str = "Unknown";
            break;
        }
    }
    Log::warning(Logger::opengl, "Source: {}", source_str);

    auto type_str = std::string{};
    switch (type) {
        case GL_DEBUG_TYPE_ERROR: type_str = "Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: type_str = "Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: type_str = "Undefined Behaviour"; break;
        case GL_DEBUG_TYPE_PORTABILITY: type_str = "Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE: type_str = "Performance"; break;
        case GL_DEBUG_TYPE_MARKER: type_str = "Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP: type_str = "Type: Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP: type_str = "Type: Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER: {
            type_str = "Type: Other";
            break;
        }
        default: {
            type_str = "Type: Unknown";
            break;
        }
    }
    Log::warning(Logger::opengl, "Type: {}", type_str);

    auto severity_str = std::string{};
    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH: severity_str = "high"; break;
        case GL_DEBUG_SEVERITY_MEDIUM: severity_str = "medium"; break;
        case GL_DEBUG_SEVERITY_LOW: severity_str = "low"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: {
            severity_str = "notification";
            break;
        }
        default: {
            severity_str = "unknown";
            break;
        }
    }
    Log::warning(Logger::opengl, "Severity: {}", severity_str);
}

void windowSizeCallback(GLFWwindow* window, const int width, const int height) {
    const auto state = std::bit_cast<core::EmulatorContext*>(glfwGetWindowUserPointer(window));
    state->opengl()->onWindowResize(static_cast<u16>(width), static_cast<u16>(height));
}

void errorCallback(int error, const char* description) { fprintf(stderr, "Error %d: %s\n", error, description); }

void Opengl::onWindowResize(const u16 new_width, const u16 new_height) { hostScreenResolution({new_width, new_height}); }

auto initializeVao() -> std::tuple<u32, u32> {
    auto vao = u32{};
    glGenVertexArrays(1, &vao);

    // Creating the vertex buffer
    auto vertex_buffer = u32{};
    glGenBuffers(1, &vertex_buffer);

    // Binding the VAO
    glBindVertexArray(vao);

    //// Declaring vertex buffer data
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

    // position pointer
    glVertexAttribPointer(0, 2, GLenum::GL_SHORT, GL_FALSE, sizeof(Vertex), nullptr); // NOLINT: this is an index
    glEnableVertexAttribArray(0);                                                     // NOLINT: this is an index

    // texture coords pointer
    auto offset = GLintptr(sizeof(VertexPosition));
    glVertexAttribPointer(1, 3, GLenum::GL_FLOAT, GL_FALSE, sizeof(Vertex), std::bit_cast<GLvoid*>(offset));
    glEnableVertexAttribArray(1);

    // color pointer
    offset += GLintptr(sizeof(TextureCoordinates));
    glVertexAttribPointer(2, 4, GLenum::GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), std::bit_cast<GLvoid*>(offset));
    glEnableVertexAttribArray(2);

    // gouraud color pointer
    offset += GLintptr(sizeof(VertexColor));
    glVertexAttribPointer(3, 4, GLenum::GL_BYTE, GL_TRUE, sizeof(Vertex), std::bit_cast<GLvoid*>(offset));
    glEnableVertexAttribArray(3);

    // color offset pointer R sign
    offset += GLintptr(sizeof(Gouraud));
    glVertexAttribPointer(4, 4, GLenum::GL_UNSIGNED_BYTE, GL_FALSE, sizeof(Vertex), std::bit_cast<GLvoid*>(offset));
    glEnableVertexAttribArray(4);

    // color offset pointer R value
    offset += GLintptr(sizeof(std::array<u8, 4>));
    glVertexAttribPointer(5, 4, GLenum::GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), std::bit_cast<GLvoid*>(offset));
    glEnableVertexAttribArray(5);

    glBindVertexArray(0);
    return std::make_tuple(vao, vertex_buffer);
}

} // namespace saturnin::video
