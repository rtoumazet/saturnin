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
#include <glbinding/gl/gl.h>
#include <glbinding/Version.h>
#include <glbinding-aux/ContextInfo.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <saturnin/src/config.h>
#include <saturnin/src/locale.h> // tr
#include <saturnin/src/log.h>
#include <saturnin/src/video/gui.h>
#include <saturnin/src/video/opengl_legacy.h>
#include <saturnin/src/video/opengl_modern.h>

using namespace gl;

namespace saturnin::video {

using core::Log;
using core::tr;

Opengl::Opengl(core::Config* config) {
    config_ = config;
    // bool is_legacy = config->readValue(core::AccessKeys::cfg_rendering_legacy_opengl);

    // iconsTextureId = generateIconsTexture();
}

auto Opengl::config() const -> Config* { return config_; }

auto Opengl::isWindowResized(const u32 new_width, const u32 new_height) const -> bool {
    return (new_width != current_texture_width_ || new_height != current_texture_height_);
}

void Opengl::initializeTexture(const u32 width, const u32 height) {
    deleteTexture();
    this->texture_ = generateEmptyTexture(width, height);
    setTextureDimension(width, height);

    // New texture is attached to the fbo
    bindTextureToFbo();
}

void Opengl::setTextureDimension(const u32 width, const u32 height) {
    current_texture_width_  = width;
    current_texture_height_ = height;
};

static void error_callback(int error, const char* description) { fprintf(stderr, "Error %d: %s\n", error, description); }

auto Opengl::generateTextureFromFile(const std::string& filename) const -> u32 {
    const auto full_path{std::filesystem::current_path() / "res" / filename};

    auto input_file = std::ifstream(full_path, std::ios::binary);
    if (!input_file) {
        Log::warning("opengl", tr("Image res/{} not found !"), filename);
        return 0;
    }

    auto buffer = std::stringstream{};
    buffer << input_file.rdbuf();
    input_file.close();

    const auto str         = buffer.str();
    auto       source_data = std::vector<u8>{};
    source_data.reserve(str.size());

    for (const auto s : str) {
        source_data.emplace_back(s);
    }

    auto       width        = u32{};
    auto       height       = u32{};
    auto       decoded_data = std::vector<u8>{};
    const auto error        = lodepng::decode(decoded_data, width, height, source_data, LCT_RGBA);

    // If there's an error, display it.
    if (error != 0) {
        Log::warning("opengl", lodepng_error_text(error));
        return 0;
    }

    // glEnable(GL_TEXTURE_2D);

    auto texture = generateTextureFromVector(width, height, decoded_data);
    // glGenTextures(1, &texture);
    // glBindTexture(GL_TEXTURE_2D, texture);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, decoded_data.data());

    return texture;
};

auto Opengl::generateUiIcons() -> bool {
    icons_map_.emplace(IconId::play, generateTextureFromFile("icon-play.png"));
    icons_map_.emplace(IconId::pause, generateTextureFromFile("icon-pause.png"));
    icons_map_.emplace(IconId::stop, generateTextureFromFile("icon-stop.png"));
    icons_map_.emplace(IconId::config, generateTextureFromFile("icon-config.png"));
    icons_map_.emplace(IconId::step_into, generateTextureFromFile("icon-step-into.png"));
    icons_map_.emplace(IconId::step_over, generateTextureFromFile("icon-step-over.png"));
    icons_map_.emplace(IconId::step_out, generateTextureFromFile("icon-step-out.png"));
    icons_map_.emplace(IconId::file, generateTextureFromFile("icon-file.png"));
    icons_map_.emplace(IconId::debug, generateTextureFromFile("icon-debug.png"));

    return true;
}

auto Opengl::getIconTexture(const IconId id) -> ImTextureID {
    return reinterpret_cast<ImTextureID>(static_cast<uptr>(icons_map_[id]));
}

/* static */
auto Opengl::loadPngImage(const std::vector<uint8_t>& source_data, std::vector<uint8_t>& image) -> bool {
    // Load file and decode image.
    auto       width  = u32{};
    auto       height = u32{};
    const auto error  = lodepng::decode(image, width, height, source_data, LCT_RGBA);

    // If there's an error, display it.
    if (error != 0) {
        std::cout << "error " << error << ": " << lodepng_error_text(error) << std::endl;
        return false;
    }

    //// "F" texture
    // uint8_t texture[] = {
    //    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    //    0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,
    //    0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    //    0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    //    0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    //    0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    //    0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    //    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
    //};
    // glEnable(GL_TEXTURE_2D);
    //
    // glGenTextures(1, &iconsTextureId);
    // glBindTexture(GL_TEXTURE_2D, iconsTextureId);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // glTexImage2D(GL_TEXTURE_2D, 0, 4, 0x8, 0x8, 0, GL_RGB, GL_UNSIGNED_BYTE, texture);

    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, source_data.data());
    // glObjectLabel(GL_TEXTURE, iconsTextureId, -1, "Example Texture");

    return true;
}

/* static */
auto Opengl::loadIcons(std::vector<uint8_t>& image) -> u32 {
    // Opengl.loadPngImage("D:/Dev/Sources/VS2017/saturnin-vs2017/saturnin/res/icons.png");
    // std::vector<uint8_t> icons_vector(icons_png, icons_png + sizeof(icons_png));
    // return loadPngImage(icons_vector, image);

    // "F" texture
    // uint8_t texture[] = {
    //    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    //    0x00,0x00,0x00,0x0F,0xFF,0xFF,0x0F,0xFF,0xFF,0x0F,0xFF,0xFF,0x0F,0xFF,0xFF,0x0F,0xFF,0xFF,0x0F,0xFF,0xFF,0x00,0x00,0x00,
    //    0x00,0x00,0x00,0x0F,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    //    0x00,0x00,0x00,0x0F,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    //    0x00,0x00,0x00,0x0F,0xFF,0xFF,0x0F,0xFF,0xFF,0x0F,0xFF,0xFF,0x0F,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    //    0x00,0x00,0x00,0x0F,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    //    0x00,0x00,0x00,0x0F,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    //    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
    //};
    // glEnable(GL_TEXTURE_2D);

    // glGenTextures(1, &iconsTextureId);
    // glBindTexture(GL_TEXTURE_2D, iconsTextureId);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // glTexImage2D(GL_TEXTURE_2D, 0, 4, 0x8, 0x8, 0, GL_RGB, GL_UNSIGNED_BYTE, texture);

    // return iconsTextureId;
    return 0;
}

/* static */
auto Opengl::generateIconsTexture() -> u32 {
    // uint32_t width{};
    // uint32_t height{};
    // std::vector<uint8_t> icons_raw_data(icons_png, icons_png + sizeof(icons_png));
    // std::vector<uint8_t> icons_decoded;

    // uint32_t error = lodepng::decode(icons_decoded, width, height, icons_raw_data);
    // if (error != 0) {
    //    std::cout << "error " << error << ": " << lodepng_error_text(error) << std::endl;
    //    //return false;
    //}

    // glEnable(GL_TEXTURE_2D);
    // uint32_t texId;
    // glGenTextures(1, &texId);
    // glBindTexture(GL_TEXTURE_2D, texId);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &icons_decoded[0]);

    // return texI
    return 0;
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
    std::string window_title = fmt::format(core::tr("Saturnin {0} - Modern rendering"), core::saturnin_version);

    constexpr auto h_window_size = minimum_viewport_width;
    constexpr auto v_window_size = h_window_size;
    const auto     window        = glfwCreateWindow(h_window_size, v_window_size, window_title.c_str(), nullptr, nullptr);
    if (window == nullptr) { return EXIT_FAILURE; }

    state.openglWindow(window);

    glfwSetWindowCloseCallback(window, windowCloseCallback);

    glfwSetWindowUserPointer(window, static_cast<void*>(&state));

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    glbinding::initialize(glfwGetProcAddress);

    // Setup Dear ImGui binding
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    auto io = ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    (void)io;
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

    const auto clear_color = ImVec4{0.45f, 0.55f, 0.60f, 1.00f};

    std::unique_ptr<Opengl> opengl = nullptr;
    if (is_legacy_opengl) {
        auto legacy = std::make_unique<OpenglLegacy>(state.config());
        opengl      = std::move(legacy);
    } else {
        auto modern = std::make_unique<OpenglModern>(state.config());
        opengl      = std::move(modern);
    }

    // Main loop
    while (glfwWindowShouldClose(window) == GLFW_FALSE) {
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

        gui::buildGui(state, *opengl, display_w, display_h);

        if (state.renderingStatus() == core::RenderingStatus::reset) { glfwSetWindowShouldClose(window, GLFW_TRUE); }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

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

}; // namespace saturnin::video
