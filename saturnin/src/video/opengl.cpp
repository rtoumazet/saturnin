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
    // glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, saturn_framebuffer_width, saturn_framebuffer_height);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA8,
                 saturn_framebuffer_width,
                 saturn_framebuffer_height,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 0);

    // No need for mipmaps, they are turned off
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Attaching the color texture to the fbo
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0);

    // static const GLenum draw_buffers[] = {GL_COLOR_ATTACHMENT0};

    renderedTexture(texture);

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
    initializeShaders();
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
    const auto texture = renderedTexture();
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

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
};

void Opengl::postRender() {
    // Framebuffer is released
    if (is_legacy_opengl_) {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    } else {
        gl33core::glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
};

void Opengl::initializeShaders() {
    shaders_list_.try_emplace({GlslVersion::glsl_120, ShaderName::vertex}, R"(
        #version 120

        attribute vec2 vtx_position;
        attribute vec2 vtx_tex_coord;
        varying vec2   frg_tex_coord;
        uniform mat4 proj_matrix;

        void main() {
            gl_Position = proj_matrix * vec4(vtx_position, 0.0, 1.0);
            frg_tex_coord = vtx_tex_coord;
        }
    )");

    shaders_list_.try_emplace({GlslVersion::glsl_330, ShaderName::vertex}, R"(
        #version 330 core

        layout (location = 0) in vec2 vtx_position;
        layout (location = 1) in vec2 vtx_tex_coord;
        uniform mat4 proj_matrix;

        out vec2 frg_tex_coord;

        void main() {
            gl_Position = proj_matrix * vec4(vtx_position, 0.0, 1.0);
            frg_tex_coord = vec2(vtx_tex_coord);
        }
    )");

    shaders_list_.try_emplace({GlslVersion::glsl_120, ShaderName::fragment}, R"(
        #version 120
        
        varying vec4 frg_color;
        varying vec2 frg_tex_coord;

        uniform sampler2D texture1;

        void main()
        {
            gl_FragColor = texture2D(texture1,frg_tex_coord);
        }
    )");

    shaders_list_.try_emplace({GlslVersion::glsl_330, ShaderName::fragment}, R"(
        #version 330 core
        
        in vec2 frg_tex_coord;

        out vec4 frg_color;

        uniform sampler2D texture1;
        
        void main()
        {
            frg_color = texture(texture1, frg_tex_coord);
        } 
    )");
}

void Opengl::displayFramebuffer(core::EmulatorContext& state) {
    preRender();
    Texture::deleteTexturesOnGpu();
    const auto vdp2_parts    = state.vdp2()->vdp2Parts(ScrollScreen::nbg3);
    const auto vertex_nb     = (4 * vdp2_parts.size());
    auto       draw_list     = std::vector<Vertex>{};
    auto       textures_list = std::vector<u32>{};
    if (!vdp2_parts.empty()) {
        draw_list.reserve(vertex_nb);
        for (u32 i = 0; i < vdp2_parts.size(); ++i) {
            const auto& part_vertexes = vdp2_parts[i].partVertexes();
            draw_list.insert(std::end(draw_list), std::begin(part_vertexes), std::end(part_vertexes));
            // Texture should have been created in the texture pool, checking if it exists on the GPU
            auto& t = Texture::getTexture(vdp2_parts[i].getTextureKey());
            if (t.apiHandle() == 0) {
                // Creation / replacement of the texture on the GPU
                // deleteTexture(t.apiHandle());
                t.apiHandle(generateTexture(t.width(), t.height(), t.rawData()));
            }
            textures_list.emplace_back(t.apiHandle());
        }
    }
    // render();
    renderBatch(DrawType::textured_polygon, draw_list, textures_list);
    postRender();
    // state.notifyRenderingDone();
}

void Opengl::onWindowResize(const u16 width, const u16 height) { hostScreenResolution({width, height}); };

void Opengl::updateScreenResolution(){};

auto Opengl::createVertexShader() -> u32 {
    const auto vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    auto       source        = getShaderSource(ShaderName::vertex);
    glShaderSource(vertex_shader, 1, &source, nullptr);
    glCompileShader(vertex_shader);
    checkShaderCompilation(vertex_shader);

    return vertex_shader;
}

auto Opengl::createFragmentShader() -> u32 {
    const auto fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    auto       source          = getShaderSource(ShaderName::fragment);
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

    // disable mipmaps
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

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

void Opengl::renderBatch(const DrawType type, const std::vector<Vertex>& draw_list, const std::vector<u32>& textures_list) {
    switch (type) {
        case DrawType::textured_polygon: {
            constexpr auto vertexes_per_quad      = u8{4};
            constexpr auto vertexes_per_triangles = u32{6};
            auto           vertexes               = std::vector<Vertex>{};
            auto           elements_nb            = draw_list.size() / vertexes_per_quad;
            const auto     vertexes_nb            = elements_nb * vertexes_per_triangles;
            if (!draw_list.empty()) {
                vertexes.reserve(vertexes_nb);
                for (u32 i = 0; i < draw_list.size(); i += 4) {
                    // Transforming one quad in 2 triangles
                    vertexes.emplace_back(draw_list[i + 0]);
                    vertexes.emplace_back(draw_list[i + 1]);
                    vertexes.emplace_back(draw_list[i + 2]);
                    vertexes.emplace_back(draw_list[i + 0]);
                    vertexes.emplace_back(draw_list[i + 2]);
                    vertexes.emplace_back(draw_list[i + 3]);
                }
            }
            // Creating the VAO
            auto vao = initializeVao(vertexes);
            glUseProgram(program_shader_);
            glBindVertexArray(vao); // binding VAO
            glActiveTexture(GLenum::GL_TEXTURE0);

            // Calculating the ortho projection matrix and sending it to the shader
            const auto proj_matrix     = calculateDisplayViewportMatrix();
            const auto uni_proj_matrix = glGetUniformLocation(program_shader_, "proj_matrix");
            glUniformMatrix4fv(uni_proj_matrix, 1, GL_FALSE, glm::value_ptr(proj_matrix));

            // Drawing the list, rendering 2 triangles (one quad) at a time while changing the current texture
            auto texture_index = u32{};
            for (u32 i = 0; i < vertexes.size(); i += vertexes_per_triangles) {
                glBindTexture(GL_TEXTURE_2D, textures_list.at(texture_index));
                glDrawArrays(GL_TRIANGLES, i, vertexes_per_triangles);
                ++texture_index;
            }
            break;
        }
        case DrawType::non_textured_polygon: {
            break;
        }
        case DrawType::polyline: {
            break;
        }
        case DrawType::line: {
            break;
        }
    }
}

static void error_callback(int error, const char* description) { fprintf(stderr, "Error %d: %s\n", error, description); }

auto Opengl::getShaderSource(const ShaderName name) -> const char* {
    const auto glsl_version = (is_legacy_opengl_) ? GlslVersion::glsl_120 : GlslVersion::glsl_330;
    return shaders_list_[{glsl_version, name}];
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

auto Opengl::initializeVao(const std::vector<Vertex>& vertexes) -> u32 {
    // Creating the VAO
    auto vao = u32{};
    glGenVertexArrays(1, &vao);

    // Creating the vertex buffer
    auto vertex_buffer = u32{};
    glGenBuffers(1, &vertex_buffer);

    // Binding the VAO
    glBindVertexArray(vao);

    // Declaring vertex buffer data, and sending it to the GPU
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertexes.size(), vertexes.data(), GL_STATIC_DRAW);

    // position pointer
    glVertexAttribPointer(0, 2, GLenum::GL_SHORT, GL_FALSE, sizeof(Vertex), 0); // NOLINT: this is an index
    glEnableVertexAttribArray(0);                                               // NOLINT: this is an index

    // texture coords pointer
    auto offset = GLintptr(2 * sizeof(s16) + 4 * sizeof(u8));
    glVertexAttribPointer(1, 2, GLenum::GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(offset));
    glEnableVertexAttribArray(1);

    return vao;
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

    Log::info("opengl", "Max version supported : {}", version.toString());

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

    const auto main_window = createMainWindow(minimum_window_width, minimum_window_height, window_title);
    if (main_window == nullptr) { return EXIT_FAILURE; }
    state.openglWindow(main_window);

    glfwSetWindowCloseCallback(main_window, windowCloseCallback);
    glfwSetWindowSizeCallback(main_window, windowSizeCallback);

    glfwSetWindowUserPointer(main_window, static_cast<void*>(&state));

    glfwSetInputMode(main_window, GLFW_STICKY_KEYS, GLFW_TRUE);

    glfwMakeContextCurrent(main_window);
    glfwSwapInterval(1); // Enable vsync

    auto icons = std::array<GLFWimage, 3>{
        {loadPngImage("saturnin-ico-16.png"), loadPngImage("saturnin-ico-32.png"), loadPngImage("saturnin-ico-48.png")}};
    glfwSetWindowIcon(main_window, 3, icons.data());

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

    ImGui_ImplGlfw_InitForOpenGL(main_window, true);
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
    const auto glyph_offset  = ImVec2(0, 2);
    config.GlyphOffset       = glyph_offset;
    constexpr auto font_size = 13.0f;
    io.Fonts->AddFontFromFileTTF(font_path.string().c_str(), font_size, &config, icons_ranges.data());
    io.Fonts->Build();

    const auto clear_color = ImVec4{0.45f, 0.55f, 0.60f, 1.00f};

    // Getting the right rendering context
    auto opengl = std::make_unique<Opengl>(state.config());
    state.opengl(opengl.get());
    opengl->mainContext(main_window);

    updateMainWindowSizeAndRatio(main_window, minimum_window_width, minimum_window_height);

    // opengl->saturnScreenResolution(ScreenResolution{320, 224});
    opengl->hostScreenResolution(ScreenResolution{minimum_window_width, minimum_window_height});

    // Main loop
    while (glfwWindowShouldClose(main_window) == GLFW_FALSE) {
        // Start the ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Rendering
        auto display_w = s32{};
        auto display_h = s32{};
        glfwMakeContextCurrent(main_window);
        glfwGetFramebufferSize(main_window, &display_w, &display_h);

        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        gui::buildGui(state);
        if (state.renderingStatus() == core::RenderingStatus::reset) { glfwSetWindowShouldClose(main_window, GLFW_TRUE); }

        ImGui::Render();
        // auto display_w = s32{};
        // auto display_h = s32{};
        // glfwGetFramebufferSize(window, &display_w, &display_h);
        // glViewport(0, 0, display_w, display_h);
        // glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        // glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        if ((ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) > 0) {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        // glfwMakeContextCurrent(window);
        glfwSwapBuffers(main_window);

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

    glfwDestroyWindow(main_window);
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
