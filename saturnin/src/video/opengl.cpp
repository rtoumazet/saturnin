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

#include <windows.h> // removes C4005 warning
#include <iostream> // cout
#include <lodepng.h>
#include <GLFW/glfw3.h>
#include <glbinding/glbinding.h>
#include <glbinding/Version.h>
#include <glbinding-aux/ContextInfo.h>

#include "opengl.h"
#include "gui.h"
#include "../config.h"
#include "../log.h"

namespace saturnin {
namespace video {

using core::Log;
    
Opengl::Opengl(core::Config* config) {
    config_ = config;
    bool is_legacy = config->readValue(core::AccessKeys::cfg_rendering_legacy_opengl);
    //if (is_legacy)  calculateRendering = std::bind(&Opengl::calculateLegacyRendering, this);
    //else            calculateRendering = std::bind(&Opengl::calculateModernRendering, this);
    
    iconsTextureId = generateIconsTexture();
}

Config* Opengl::config() const {
    return config_;
}

bool Opengl::isWindowResized(const u32 new_width, const u32 new_height) const {
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
    current_texture_width_ = width;
    current_texture_height_= height;

};

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error %d: %s\n", error, description);
}

bool Opengl::loadPngImage(const std::vector<uint8_t>& source_data, std::vector<uint8_t>& image) {
    // Load file and decode image.
    uint32_t width{};
    uint32_t height{};
    uint32_t error = lodepng::decode(image, width, height, source_data, LCT_RGBA);

    // If there's an error, display it.
    if (error != 0) {
        std::cout << "error " << error << ": " << lodepng_error_text(error) << std::endl;
        return false;
    }

    //// "F" texture
    //uint8_t texture[] = {
    //    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    //    0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,
    //    0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    //    0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    //    0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    //    0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    //    0x00,0x00,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    //    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 
    //};
    //glEnable(GL_TEXTURE_2D);
    //
    //glGenTextures(1, &iconsTextureId);
    //glBindTexture(GL_TEXTURE_2D, iconsTextureId);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //glTexImage2D(GL_TEXTURE_2D, 0, 4, 0x8, 0x8, 0, GL_RGB, GL_UNSIGNED_BYTE, texture);

    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, source_data.data());
    //glObjectLabel(GL_TEXTURE, iconsTextureId, -1, "Example Texture");

    return true;
}

uint32_t Opengl::loadIcons(std::vector<uint8_t>& image) {
    //Opengl.loadPngImage("D:/Dev/Sources/VS2017/saturnin-vs2017/saturnin/res/icons.png");
    //std::vector<uint8_t> icons_vector(icons_png, icons_png + sizeof(icons_png));
    //return loadPngImage(icons_vector, image);
    
        // "F" texture
    //uint8_t texture[] = {
    //    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    //    0x00,0x00,0x00,0x0F,0xFF,0xFF,0x0F,0xFF,0xFF,0x0F,0xFF,0xFF,0x0F,0xFF,0xFF,0x0F,0xFF,0xFF,0x0F,0xFF,0xFF,0x00,0x00,0x00,
    //    0x00,0x00,0x00,0x0F,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    //    0x00,0x00,0x00,0x0F,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    //    0x00,0x00,0x00,0x0F,0xFF,0xFF,0x0F,0xFF,0xFF,0x0F,0xFF,0xFF,0x0F,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    //    0x00,0x00,0x00,0x0F,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    //    0x00,0x00,0x00,0x0F,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    //    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
    //};
    //glEnable(GL_TEXTURE_2D);

    //glGenTextures(1, &iconsTextureId);
    //glBindTexture(GL_TEXTURE_2D, iconsTextureId);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //glTexImage2D(GL_TEXTURE_2D, 0, 4, 0x8, 0x8, 0, GL_RGB, GL_UNSIGNED_BYTE, texture);

    //return iconsTextureId;
    return 0;
}

uint32_t Opengl::generateIconsTexture() {
    //uint32_t width{};
    //uint32_t height{};
    //std::vector<uint8_t> icons_raw_data(icons_png, icons_png + sizeof(icons_png));
    //std::vector<uint8_t> icons_decoded;

    //uint32_t error = lodepng::decode(icons_decoded, width, height, icons_raw_data);
    //if (error != 0) {
    //    std::cout << "error " << error << ": " << lodepng_error_text(error) << std::endl;
    //    //return false;
    //}

    //glEnable(GL_TEXTURE_2D);
    //uint32_t texId;
    //glGenTextures(1, &texId);
    //glBindTexture(GL_TEXTURE_2D, texId);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &icons_decoded[0]);

    //return texI
    return 0;
}

bool isModernOpenglCapable()
{
    if (!glfwInit()) return false;

    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(1280, 720, "", nullptr, nullptr);
    if (window == nullptr) {
        glfwTerminate();
        return false;
    }
            
    glfwMakeContextCurrent(window);
 
    glbinding::initialize(glfwGetProcAddress);
    const glbinding::Version version = glbinding::aux::ContextInfo::version();

    glfwDestroyWindow(window);
    glfwTerminate();

    Log::info("opengl", "Max version supported : {}", version.toString());

    return (version < glbinding::Version(3,3)) ? false : true;
}

void windowCloseCallback(GLFWwindow* window) {
    core::EmulatorContext *state = reinterpret_cast<core::EmulatorContext*>(glfwGetWindowUserPointer(window));
    state->renderingStatus( core::RenderingStatus::stopped );
}



};
};