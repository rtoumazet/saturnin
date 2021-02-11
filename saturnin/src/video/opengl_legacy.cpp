//
// opengl_legacy.cpp
// Saturnin
//
// Copyright (c) 2019 Renaud Toumazet
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
#include <saturnin/src/video/opengl_legacy.h>
#include <windows.h> // removes C4005 warning
#include <istream>

#include <glbinding/gl21/gl.h>
#include <glbinding/gl21ext/gl.h>
#include <glbinding/glbinding.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <saturnin/src/emulator_context.h>
#include <saturnin/src/locale.h>
#include <saturnin/src/log.h>
#include <saturnin/src/video/gui.h> // buildGui
#include <saturnin/src/video/opengl.h>

using namespace gl21;
using namespace gl21ext;

namespace saturnin::video {

using core::Log;
using core::tr;

void OpenglLegacy::initialize() {
    const auto window    = glfwGetCurrentContext();
    auto       display_w = s32{};
    auto       display_h = s32{};
    glfwGetFramebufferSize(window, &display_w, &display_h);
    // initializeTexture(display_w, display_h);

    glGenFramebuffersEXT(1, &fbo_);
    bindTextureToFbo();
    const auto status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    if (status != gl::GLenum::GL_FRAMEBUFFER_COMPLETE) {
        Log::error("opengl", tr("Could not initialize framebuffer object !"));
        throw std::runtime_error("Opengl error !");
    }
}

void OpenglLegacy::shutdown() {
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    glDeleteFramebuffersEXT(1, &fbo_);
    // glDeleteTextures(1, &texture_);
    const auto texture = renderingTexture();
    glDeleteTextures(1, &texture);
}

void OpenglLegacy::preRender() {
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo_);
    // glViewport(0, 0, current_texture_width_, current_texture_height_);

    glClear(GL_COLOR_BUFFER_BIT);
};

void OpenglLegacy::render() {
    static auto i = float{0};
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);
    glRotatef(i, 0.0f, 0.0f, 1.0f);

    //    glBegin(GL_TRIANGLES);
    constexpr auto red   = float{1.0f};
    constexpr auto green = float{0.5f};
    constexpr auto blue  = float{0.2f};
    constexpr auto alpha = float{1.0f};
    glColor4f(red, green, blue, alpha);

    constexpr auto verts = std::array<float, 9>{-0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f};

    // activate and specify pointer to vertex array
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, verts.data());

    // draw a cube
    glDrawArrays(GL_TRIANGLES, 0, static_cast<s32>(verts.size()));

    // deactivate vertex arrays after drawing
    glDisableClientState(GL_VERTEX_ARRAY);

    // glVertex3f(-0.5f, -0.5f, 0.0f);
    // glVertex3f(0.5f, -0.5f, 0.0f);
    // glVertex3f(0.0f, 0.5f, 0.0f);
    //   glEnd();

    glPopMatrix();
    ++i;
};

void OpenglLegacy::postRender() { glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0); };

void OpenglLegacy::onWindowResize(u16 width, u16 height){};

auto OpenglLegacy::generateEmptyTexture(const u32 width, const u32 height) const -> u32 {
    auto texture = u32{};
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    return texture;
}

void OpenglLegacy::bindTextureToFbo() const {
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo_);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, renderingTexture(), 0);
}

static void error_callback(int error, const char* description) {
    Log::error("opengl", "Error {}: {}", error, description);
    throw std::runtime_error("Opengl error !");
}

}; // namespace saturnin::video
