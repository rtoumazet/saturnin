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

#define GLFW_INCLUDE_NONE
#include <windows.h> // removes C4005 warning
#include <glbinding/gl/gl.h>
#include <glbinding/glbinding.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

#include "gui.h"
#include "opengl.h"
#include "opengl_modern.h"
#include "../../lib/imgui/imgui_impl_glfw.h"
#include "../../lib/imgui/imgui_impl_opengl3.h"
#include "../emulator_context.h"
#include "../log.h"

using namespace gl;

namespace saturnin {
namespace video {

using core::Log;

void OpenglModern::initialize() {
    GLFWwindow* window = glfwGetCurrentContext();
    s32 display_w{};
    s32 display_h{};
    glfwGetFramebufferSize(window, &display_w, &display_h);
    initializeTexture(display_w, display_h);

    glGenFramebuffers(1, &fbo_);
    bindTextureToFbo();
    auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != gl::GLenum::GL_FRAMEBUFFER_COMPLETE) {
        Log::error("opengl", "Could not initialize framebuffer object !");
    }

    u32 vertex_shader   = createVertexShader();
    u32 fragment_shader = createFragmentShader();
    program_shader_     = createProgramShader(vertex_shader, fragment_shader);
    std::vector<uint32_t> shaders_to_delete = { vertex_shader, fragment_shader };
    deleteShaders(shaders_to_delete);
}

void OpenglModern::shutdown() {
    glDeleteProgram(program_shader_);
}
    
u32 OpenglModern::generateEmptyTexture(const u32 width, const u32 height) const {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);

    u32 texture{};
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GLenum::GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GLenum::GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    gl::GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

    return texture;
}

void OpenglModern::updateTextureSize( const u32 width, const u32 height) {
    initializeTexture(width, height);
}

void OpenglModern::bindTextureToFbo() const {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_, 0);
}

void OpenglModern::deleteTexture() const {
    if (texture_ != 0) glDeleteTextures(1, &texture_);
}

u32 OpenglModern::createVertexShader() {
    const char* vertex_shader_source = R"(
        #version 330 core

        in vec2 position;        
        uniform mat4 rotate;
        //in vec3 color;
        //in vec2 texcoord;

        //out vec3 Color;
        //out vec2 Texcoord;
        
        void main()
        {
        	//Color = color;
            //Texcoord = texcoord;
            gl_Position = rotate * vec4(position, 0.0, 1.0);
            //gl_Position = vec4(position, 0.0, 1.0);
            
            //gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
        	//gl_Position.xyz = aPos.xyz;
            //gl_Position.w = 1.0;
        }
    )";
        
    u32 vertex_shader{};
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, nullptr);
    glCompileShader(vertex_shader);
    checkShaderCompilation(vertex_shader);

    return vertex_shader;
}
u32 OpenglModern::createFragmentShader()
{
    const char* fragment_shader_source = R"(
        #version 330 core
        
        out vec4 color;
        
        void main()
        {
            //color = vec4(1.0f, 0.5f, 0.2f, 1.0f);
            color = vec4(1.0f, 0.5f, 0.2f, 1.0f);
        } 
    )";
        
    u32 fragment_shader {};
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, nullptr);
    glCompileShader(fragment_shader);

    checkShaderCompilation(fragment_shader);
        
    return fragment_shader;
}
            
u32 OpenglModern::createProgramShader(const u32 vertex_shader, const u32 fragment_shader) {
    u32 shader_program = glCreateProgram();

    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    checkProgramCompilation(shader_program);

    return shader_program;
}

void OpenglModern::deleteShaders(std::vector<u32> shaders)
{
    for (auto shader : shaders) {
        glDeleteShader(shader);
    }
}

void OpenglModern::setupTriangle() {
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f,  0.5f, 0.0f
    };

    glGenVertexArrays(1, &vao_);
    u32 vertex_buffer{};
    glGenBuffers(1, &vertex_buffer);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(vao_);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GLenum::GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);

}
void OpenglModern::drawTriangle() {
    glUseProgram(program_shader_);
    glBindVertexArray(vao_); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized

    glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));

    GLint uni_rotate = glGetUniformLocation(program_shader_, "rotate");
    glUniformMatrix4fv(uni_rotate, 1, GL_FALSE, glm::value_ptr(rotate));

    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void OpenglModern::preRender() {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    glViewport(0, 0, current_texture_width_, current_texture_height_);
    gl::glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
};

void OpenglModern::render() {
    this->setupTriangle();
    this->drawTriangle();
};

void OpenglModern::postRender() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
};

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error %d: %s\n", error, description);
}


int32_t runModernOpengl(core::Emulator_context& state) {
    // Setup window
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        return EXIT_FAILURE;

    // Decide GL+GLSL versions
#if __APPLE__
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif
    std::string window_title = fmt::format(core::tr("Saturnin {0} - Modern rendering"), core::saturnin_version);
    auto window = glfwCreateWindow(1280, 720, window_title.c_str(), NULL, NULL);
    if (window == nullptr) {
        return EXIT_FAILURE;
    }

    glfwSetWindowCloseCallback(window, windowCloseCallback);
    //glfwSetWindowUserPointer(window, (void*)&state);
    glfwSetWindowUserPointer(window, static_cast<void*>(&state));

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

	glbinding::initialize(glfwGetProcAddress);

    // Setup Dear ImGui binding
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
    // 
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Setup style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Load Fonts
    // (there is a default font, this is only if you want to change it. see extra_fonts/README.txt for more details)
    //ImGuiIO& io = ImGui::GetIO();
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../extra_fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../extra_fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../extra_fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../extra_fonts/ProggyTiny.ttf", 10.0f);
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    OpenglModern opengl(state.config());
    
    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Start the ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Rendering
        int display_w, display_h;
        glfwMakeContextCurrent(window);
        glfwGetFramebufferSize(window, &display_w, &display_h);
        //glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        gui::buildGui(state, opengl, display_w, display_h);
 
        if (state.renderingStatus_ == core::RenderingStatus::reset) glfwSetWindowShouldClose(window, true);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwMakeContextCurrent(window);
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

void checkShaderCompilation(const u32 shader) {
    s32 success{};
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    s32 length{};
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    if (!success) {
        std::vector<char> v(length);
        glGetShaderInfoLog(shader, length, nullptr, v.data());
        std::string info(v.begin(), v.end());

        GLenum type{};
        std::string shader_type{};
        glGetShaderiv(shader, GL_SHADER_TYPE, &type);
        switch (type) {
            case GL_VERTEX_SHADER:   shader_type = "Vertex shader"; break;
            case GL_FRAGMENT_SHADER: shader_type = "Fragment shader"; break;
            default: shader_type = "Unknown shader"; break;
        }
        Log::error("opengl", "{} compilation failed : {}", shader_type, info);
    }
}

void checkProgramCompilation(const u32 program) {
    s32 success{};
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    s32 length{};
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

    if (!success) {
        std::vector<char> v(length);
        glGetProgramInfoLog(program, length, nullptr, v.data());
        std::string info(v.begin(), v.end());
        
        Log::error("opengl", "Shader program link failed : {}", info);
    }
}

};
};