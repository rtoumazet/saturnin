//
// opengl_shaders.cpp
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

namespace saturnin::video {

using namespace gl21;
using namespace gl21ext;

using core::tr;

void Opengl::initializeShaders() {
    const auto readFile = [](const std::string& filename) {
        std::ifstream input_file("./shaders/" + filename, std::ios::in);
        if (!input_file) { Log::exception(Logger::opengl, tr("Could not load shader '{}' !"), filename); }

        auto buffer = std::stringstream{};
        buffer << input_file.rdbuf();
        input_file.close();

        return buffer.str();
    };

    shaders_list_.try_emplace("main.vert", readFile("main.vert"));
    shaders_list_.try_emplace("main.frag", readFile("main.frag"));
}

// static
void Opengl::deleteShaders(const std::vector<u32>& shaders) {
    for (auto shader : shaders) {
        glDeleteShader(shader);
    }
}

auto Opengl::getShaderSource(std::string_view name) -> const char* {
    if (auto search = shaders_list_.find(name); search != shaders_list_.end()) {
        return search->second.c_str();

    } else {
        Log::exception(Logger::opengl, "Shader {} not found !", name);
    }
}

auto Opengl::createVertexShader(std::string_view name) -> u32 {
    const auto vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    auto       source        = getShaderSource(name);
    glShaderSource(vertex_shader, 1, &source, nullptr);
    glCompileShader(vertex_shader);
    checkShaderCompilation(vertex_shader);

    return vertex_shader;
}

auto Opengl::createFragmentShader(std::string_view name) -> u32 {
    const auto fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    auto       source          = getShaderSource(name);
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

        Log::exception(Logger::opengl, "{} compilation failed : {}", shader_type, info);
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

        Log::exception(Logger::opengl, "Shader program link failed : {}", info);
    }
}

} // namespace saturnin::video
