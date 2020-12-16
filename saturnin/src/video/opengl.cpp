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
#include <glbinding/glbinding.h>
#include <glbinding/Version.h>
#include <glbinding-aux/ContextInfo.h>
#include <GLFW/glfw3.h>
#include <saturnin/src/config.h>
#include <saturnin/src/video/gui.h>

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

}; // namespace saturnin::video
