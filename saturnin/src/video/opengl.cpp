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

// using namespace gl;
using namespace gl21;
using namespace gl21ext;

using namespace std::string_literals; // enables s-suffix for std::string literals

namespace saturnin::video {

using core::Log;
using core::tr;

Opengl::Opengl(core::Config* config) {
    config_           = config;
    is_legacy_opengl_ = config_->readValue(core::AccessKeys::cfg_rendering_legacy_opengl);
    initialize();
}

Opengl::~Opengl() { shutdown(); }

void Opengl::initialize() {
    if (is_legacy_opengl_) {
        glGenFramebuffersEXT(1, &saturn_framebuffer_);
    } else {
        gl33core::glGenFramebuffers(1, &saturn_framebuffer_);
    }
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

    // static const GLenum draw_buffers[] = {GL_COLOR_ATTACHMENT0};

    renderingTexture(texture);

    if (is_legacy_opengl_) {
        const auto status = glCheckFramebufferStatusEXT(GLenum::GL_FRAMEBUFFER_EXT);
        if (status != gl::GLenum::GL_FRAMEBUFFER_COMPLETE_EXT) {
            Log::error("opengl", tr("Could not initialize framebuffer object !"));
            throw std::runtime_error("Opengl error !");
        }
    } else {
        const auto status = gl33core::glCheckFramebufferStatus(GLenum::GL_FRAMEBUFFER);
        if (status != gl::GLenum::GL_FRAMEBUFFER_COMPLETE) {
            Log::error("opengl", tr("Could not initialize framebuffer object !"));
            throw std::runtime_error("Opengl error !");
        }
    }
    const auto vertex_shader     = createVertexShader();
    const auto fragment_shader   = createFragmentShader();
    program_shader_              = createProgramShader(vertex_shader, fragment_shader);
    const auto shaders_to_delete = std::vector<u32>{vertex_shader, fragment_shader};
    deleteShaders(shaders_to_delete);
}

void Opengl::shutdown() {
    glDeleteProgram(program_shader_);
    if (is_legacy_opengl_) {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        glDeleteFramebuffersEXT(1, &saturn_framebuffer_);
    } else {
        gl33core::glDeleteProgram(program_shader_);
        gl33core::glDeleteFramebuffers(1, &saturn_framebuffer_);
    }
    const auto texture = renderingTexture();
    glDeleteTextures(1, &texture);
}

void Opengl::preRender() {
    if (is_legacy_opengl_) {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, saturn_framebuffer_);
    } else {
        gl33core::glBindFramebuffer(GL_FRAMEBUFFER, saturn_framebuffer_);
    }

    // Viewport is the entire Saturn framebuffer
    glViewport(0, 0, saturn_framebuffer_width, saturn_framebuffer_height);

    gl::glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
};

void Opengl::render() {
    this->setupTriangle();
    this->drawTriangle();
};

void Opengl::postRender() {
    // Framebuffer is released
    if (is_legacy_opengl_) {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    } else {
        gl33core::glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
};

void Opengl::displayFramebuffer(core::EmulatorContext& state) {
    vertexes_.clear();
    // vertexes_.reserve(state.vdp2()->getRenderVertexes().size() / 4); // Will have to be changed as some vertexes are not quad
    // for (const auto& v : state.vdp2()->getRenderVertexes()) {
    //    vertexes_.emplace_back(v.x);
    //    vertexes_.emplace_back(v.y);
    //}
    if (!state.vdp2()->getRenderVertexes().empty()) {
        // vertexes_.emplace_back(state.vdp2()->getRenderVertexes()[0].x);
        // vertexes_.emplace_back(state.vdp2()->getRenderVertexes()[0].y);
        // vertexes_.emplace_back(state.vdp2()->getRenderVertexes()[1].x);
        // vertexes_.emplace_back(state.vdp2()->getRenderVertexes()[1].y);
        // vertexes_.emplace_back(state.vdp2()->getRenderVertexes()[2].x);
        // vertexes_.emplace_back(state.vdp2()->getRenderVertexes()[2].y);
        // vertexes_.emplace_back(state.vdp2()->getRenderVertexes()[0].x);
        // vertexes_.emplace_back(state.vdp2()->getRenderVertexes()[0].y);
        // vertexes_.emplace_back(state.vdp2()->getRenderVertexes()[2].x);
        // vertexes_.emplace_back(state.vdp2()->getRenderVertexes()[2].y);
        // vertexes_.emplace_back(state.vdp2()->getRenderVertexes()[3].x);
        // vertexes_.emplace_back(state.vdp2()->getRenderVertexes()[3].y);
        vertexes_.emplace_back(state.vdp2()->getRenderVertexes()[0]);
        vertexes_.emplace_back(state.vdp2()->getRenderVertexes()[1]);
        vertexes_.emplace_back(state.vdp2()->getRenderVertexes()[2]);
        vertexes_.emplace_back(state.vdp2()->getRenderVertexes()[0]);
        vertexes_.emplace_back(state.vdp2()->getRenderVertexes()[2]);
        vertexes_.emplace_back(state.vdp2()->getRenderVertexes()[3]);
    }
    preRender();
    render();
    postRender();
}

void Opengl::updateScreenResolution(){};

auto Opengl::createVertexShader() -> u32 {
    auto vertex_shader_source = std::string{};
    if (is_legacy_opengl_) {
        vertex_shader_source = R"(
        #version 120

        attribute vec2 position;
        uniform mat4 proj_matrix;

        void main() {
            gl_Position = proj_matrix * vec4(position, 0.0, 1.0);
        }
    )"s;
    } else {
        vertex_shader_source = R"(
        #version 330 core

        layout (location = 0) in vec2 vtx_position;
        layout (location = 1) in vec2 vtx_tex_coord;
        uniform mat4 proj_matrix;

        out vec2 frg_tex_coord;

        void main() {
            gl_Position = proj_matrix * vec4(vtx_position, 0.0, 1.0);
            //frg_tex_coord = vec2(vtx_tex_coord.x, vtx_tex_coord.y);
            frg_tex_coord = vec2(vtx_tex_coord);
        }
    )"s;
    }
    const auto vertex_shader            = glCreateShader(GL_VERTEX_SHADER);
    auto       vertex_shader_source_tmp = vertex_shader_source.c_str();
    glShaderSource(vertex_shader, 1, &vertex_shader_source_tmp, nullptr);
    glCompileShader(vertex_shader);
    checkShaderCompilation(vertex_shader);

    return vertex_shader;
}

auto Opengl::createFragmentShader() -> u32 {
    auto fragment_shader_source = std::string{};
    if (is_legacy_opengl_) {
        fragment_shader_source = R"(
        #version 120
        
        void main()
        {
            gl_FragColor  = vec4(1.0f, 0.5f, 0.2f, 1.0f);
        } 
    )"s;
    } else {
        fragment_shader_source = R"(
        #version 330 core
        
        in vec2 frg_tex_coord;

        //out vec4 color;
        out vec4 frg_color;

        uniform sampler2D texture1;
        
        void main()
        {
            //color = vec4(1.0f, 0.5f, 0.2f, 1.0f);
            frg_color = texture(texture1, frg_tex_coord);
        } 
    )"s;
    }

    const auto fragment_shader            = glCreateShader(GL_FRAGMENT_SHADER);
    auto       fragment_shader_source_tmp = fragment_shader_source.c_str();
    glShaderSource(fragment_shader, 1, &fragment_shader_source_tmp, nullptr);
    glCompileShader(fragment_shader);

    checkShaderCompilation(fragment_shader);

    return fragment_shader;
}

auto Opengl::createProgramShader(const u32 vertex_shader, const u32 fragment_shader) -> u32 {
    const auto shader_program = glCreateProgram();

    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    checkProgramCompilation(shader_program);

    return shader_program;
}

void Opengl::deleteShaders(std::vector<u32> shaders) {
    for (auto shader : shaders) {
        glDeleteShader(shader);
    }
}

void Opengl::setupTriangle() {
    glGenVertexArrays(1, &vao_);
    auto vertex_buffer = u32{};
    glGenBuffers(1, &vertex_buffer);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(vao_);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(decltype(vertexes_)::value_type) * vertexes_.size(), vertexes_.data(), GL_STATIC_DRAW);

    // position
    glVertexAttribPointer(0, 2, GLenum::GL_SHORT, GL_FALSE, 16, 0);
    glEnableVertexAttribArray(0);

    // texture coords
    auto stride = GLint(2 * sizeof(s16) + 4 * sizeof(u8) + 2 * sizeof(float));
    auto offset = GLintptr(2 * sizeof(s16) + 4 * sizeof(u8));
    glVertexAttribPointer(1, 2, GLenum::GL_FLOAT, GL_FALSE, stride, reinterpret_cast<GLvoid*>(offset));
    glEnableVertexAttribArray(1);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer
    // object so afterwards we can safely unbind
    glBindBuffer(GLenum::GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying
    // other VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly
    // necessary.
    glBindVertexArray(0);

    //********************************************************//
    // Texture
    auto texture = u32{};
    glGenTextures(1, &texture_);
    glBindTexture(GLenum::GL_TEXTURE_2D,
                  texture_); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object

    // set the texture wrapping parameters
    glTexParameteri(GLenum::GL_TEXTURE_2D,
                    GLenum::GL_TEXTURE_WRAP_S,
                    GLenum::GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GLenum::GL_TEXTURE_2D, GLenum::GL_TEXTURE_WRAP_T, GLenum::GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GLenum::GL_TEXTURE_2D, GLenum::GL_TEXTURE_MIN_FILTER, GLenum::GL_NEAREST);
    glTexParameteri(GLenum::GL_TEXTURE_2D, GLenum::GL_TEXTURE_MAG_FILTER, GLenum::GL_NEAREST);
    auto window = glfwGetCurrentContext();
    auto state  = reinterpret_cast<core::EmulatorContext*>(glfwGetWindowUserPointer(window));
    if (!state->vdp2()->vdp2_parts_[3].empty()) {
        auto  key = state->vdp2()->vdp2_parts_[3][0].getTextureKey();
        auto& tex = Texture::getTexture(key);

        glTexImage2D(GLenum::GL_TEXTURE_2D,
                     0,
                     GLenum::GL_RGB,
                     tex.width_,
                     tex.height_,
                     0,
                     GLenum::GL_RGB,
                     GLenum::GL_UNSIGNED_BYTE,
                     tex.raw_data_.data());
    }
    //********************************************************//
}
void Opengl::drawTriangle() {
    glBindTexture(GL_TEXTURE_2D, texture_);
    glUseProgram(program_shader_);
    glBindVertexArray(vao_); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep
                             // things a bit more organized

    const auto host_res     = hostScreenResolution();
    const auto host_ratio   = static_cast<float>(host_res.width) / static_cast<float>(host_res.height);
    const auto saturn_res   = saturnScreenResolution();
    const auto saturn_ratio = static_cast<float>(saturn_res.width) / static_cast<float>(saturn_res.height);

    // If the Saturn resolution isn't set yet, calculation is aborted
    if ((saturn_res.height == 0) || (saturn_res.width == 0)) return;

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

auto Opengl::config() const -> Config* { return config_; }

static void error_callback(int error, const char* description) { fprintf(stderr, "Error %d: %s\n", error, description); }

auto Opengl::generateTextureFromVector(const u32 width, const u32 height, const std::vector<u8>& data) const -> u32 {
    glEnable(GL_TEXTURE_2D);

    auto texture = u32{};
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());

    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    return texture;
}

void Opengl::onWindowResize(u16 width, u16 height) { hostScreenResolution({width, height}); };

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

    Log::info("opengl", "Max version supported : {}", version.toString());

    return (version >= glbinding::Version(3, 3));
}

void windowCloseCallback(GLFWwindow* window) {
    auto state = reinterpret_cast<core::EmulatorContext*>(glfwGetWindowUserPointer(window));
    state->renderingStatus(core::RenderingStatus::stopped);
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

    glfwSetWindowCloseCallback(window, windowCloseCallback);
    glfwSetWindowSizeCallback(window, windowSizeCallback);

    glfwSetWindowUserPointer(window, static_cast<void*>(&state));

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    auto icons = std::array<GLFWimage, 3>{
        {loadPngImage("saturnin-ico-16.png"), loadPngImage("saturnin-ico-32.png"), loadPngImage("saturnin-ico-48.png")}};
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
    static const std::array<ImWchar, 3> icons_ranges = {0xe900, 0xe907, 0}; // Will not be copied by AddFont* so keep in scope.
    ImFontConfig                        config;
    config.MergeMode = true;
    const auto font_path{std::filesystem::current_path() / "res" / "saturnin-icons.ttf"};
    const auto glyph_offset = ImVec2(0, 2);
    config.GlyphOffset      = glyph_offset;
    io.Fonts->AddFontFromFileTTF(font_path.string().c_str(), 13.0f, &config, icons_ranges.data());
    io.Fonts->Build();

    const auto clear_color = ImVec4{0.45f, 0.55f, 0.60f, 1.00f};

    // Getting the right rendering context
    auto opengl = std::make_unique<Opengl>(state.config());
    state.opengl(opengl.get());

    updateMainWindowSizeAndRatio(window, minimum_window_width, minimum_window_height);

    // opengl->saturnScreenResolution(ScreenResolution{320, 224});
    opengl->hostScreenResolution(ScreenResolution{minimum_window_width, minimum_window_height});

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

        if (state.renderingStatus() == core::RenderingStatus::reset) { glfwSetWindowShouldClose(window, GLFW_TRUE); }

        ImGui::Render();
        // auto display_w = s32{};
        // auto display_h = s32{};
        // glfwGetFramebufferSize(window, &display_w, &display_h);
        // glViewport(0, 0, display_w, display_h);
        // glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        // glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        // glfwMakeContextCurrent(window);
        glfwSwapBuffers(window);
    }

    // Cleanup
    state.stopEmulation();
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
        Log::warning("opengl", lodepng_error_text(error));
        return image;
    }
    image.width  = width;
    image.height = height;

    return image;
}

void windowSizeCallback(GLFWwindow* window, int width, int height) {
    const auto state = reinterpret_cast<core::EmulatorContext*>(glfwGetWindowUserPointer(window));

    state->opengl()->onWindowResize(width, height);
    Log::warning("opengl", "Window was resized: {} {}", width, height);
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
        Log::error("opengl", "{} compilation failed : {}", shader_type, info);
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

        Log::error("opengl", "Shader program link failed : {}", info);
        throw std::runtime_error("Opengl error !");
    }
}

}; // namespace saturnin::video
