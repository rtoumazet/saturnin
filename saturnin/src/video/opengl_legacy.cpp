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

#define GLFW_INCLUDE_NONE
#include <windows.h> // removes C4005 warning
#include <glbinding/gl21/gl.h>
#include <glbinding/gl21ext/gl.h>
#include <glbinding/glbinding.h>
#include <GLFW/glfw3.h>
#include <imgui.h>

#include "gui.h"
#include "opengl.h"
#include "opengl_legacy.h"
#include "../../lib/imgui/imgui_impl_glfw.h"
#include "../../lib/imgui/imgui_impl_opengl2.h"
#include "../emulator_context.h"
#include "../log.h"

using namespace gl21;
using namespace gl21ext;

namespace saturnin::video {

using core::Log;
using core::tr;

void OpenglLegacy::initialize() {
    GLFWwindow* window = glfwGetCurrentContext();
    s32         display_w{};
    s32         display_h{};
    glfwGetFramebufferSize(window, &display_w, &display_h);
    initializeTexture(display_w, display_h);

    glGenFramebuffersEXT(1, &fbo_);
    bindTextureToFbo();
    auto status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    if (status != gl::GLenum::GL_FRAMEBUFFER_COMPLETE) {
        Log::error("opengl", tr("Could not initialize framebuffer object !"));
        throw std::runtime_error("Opengl error !");
    }

    if (!generateUiIcons()) { Log::warning("opengl", tr("Could not generate textures for UI icons !")); }
}

void OpenglLegacy::shutdown() {
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    glDeleteFramebuffersEXT(1, &fbo_);
    glDeleteTextures(1, &texture_);
}

// void OpenglLegacy::initializeTexture(const u32 width, const u32 height) {
//    if (texture_ != 0) glDeleteTextures(1, &texture_);
//    this->texture_ = generateEmptyTexture(width, height);
//    setTextureDimension(width, height);
//
//    // New texture is attached to the fbo
//    bindTextureToFbo();
//}

void OpenglLegacy::preRender() {
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo_);
    glViewport(0, 0, current_texture_width_, current_texture_height_);

    glClear(GL_COLOR_BUFFER_BIT);
};

void OpenglLegacy::render() {
    static float i = 0;
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);
    glRotatef(i, 0.0f, 0.0f, 1.0f);

    //    glBegin(GL_TRIANGLES);
    constexpr float red{1.0f};
    constexpr float green{0.5f};
    constexpr float blue{0.2f};
    constexpr float alpha{1.0f};
    glColor4f(red, green, blue, alpha);

    constexpr std::array<float, 9> verts = {-0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f};

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

void OpenglLegacy::updateTextureSize(const u32 width, const u32 height) {
    initializeTexture(width, height);
    // setTextureDimension(width, height);
}

auto OpenglLegacy::generateEmptyTexture(const u32 width, const u32 height) const -> u32 {
    u32 texture{};
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    return texture;
}

void OpenglLegacy::bindTextureToFbo() const {
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo_);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, texture_, 0);
}

void OpenglLegacy::deleteTexture() const {
    if (texture_ != 0) { glDeleteTextures(1, &texture_); }
}

u32 OpenglLegacy::generateTextureFromVector(const u32 width, const u32 height, const std::vector<u8>& data) const {
    glEnable(GL_TEXTURE_2D);

    u32 texture{};
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());

    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    return texture;
}

static void error_callback(int error, const char* description) {
    Log::error("opengl", "Error {}: {}", error, description);
    throw std::runtime_error("Opengl error !");
}

auto runLegacyOpengl(core::EmulatorContext& state) -> s32 {
    // Setup window
    glfwSetErrorCallback(error_callback);
    if (glfwInit() == GLFW_FALSE) { return EXIT_FAILURE; }

    std::string   window_title = fmt::format(core::tr("Saturnin {0} - Legacy rendering"), core::saturnin_version);
    constexpr u16 width{1280};
    constexpr u16 height{720};
    auto          window = glfwCreateWindow(width, height, window_title.c_str(), nullptr, nullptr);
    if (window == nullptr) { return EXIT_FAILURE; }

    state.openglWindow(window);

    glfwSetWindowCloseCallback(window, windowCloseCallback);
    // glfwSetWindowUserPointer(window, (void*)&state);
    glfwSetWindowUserPointer(window, static_cast<void*>(&state));

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // cout << epoxy_gl_version() << endl;

    // epoxy_handle_external_wglMakeCurrent();
    // if (!epoxy_has_wgl_extension(wglGetCurrentDC(), "WGL_ARB_pbuffer"))
    //    cout << "WGL_ARB_pbuffer not found !" << endl;

    // if (!epoxy_has_wgl_extension(wglGetCurrentDC(), "WGL_ARB_pixel_format"))
    //    cout << "WGL_ARB_pixel_format not found !" << endl;

    // if (!epoxy_has_wgl_extension(wglGetCurrentDC(), "WGL_ARB_render_texture"))
    //    cout << "WGL_ARB_render_texture not found !" << endl;
    //
    // if (!epoxy_has_gl_extension("GL_ARB_framebuffer_object"))
    //    cout << "GL_ARB_framebuffer_object not found !" << endl;
    //
    // if (!epoxy_has_gl_extension("GL_EXT_framebuffer_object"))
    //    cout << "GL_EXT_framebuffer_object not found !" << endl;

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

    glbinding::initialize(glfwGetProcAddress);

    OpenglLegacy opengl(state.config());

    // Setup Dear ImGui binding
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
    //

    // Setup style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsClassic();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL2_Init();

    // Load Fonts
    // (there is a default font, this is only if you want to change it. see extra_fonts/README.txt for more details)
    // ImGuiIO& io = ImGui::GetIO();
    // io.Fonts->AddFontDefault();
    // io.Fonts->AddFontFromFileTTF("../../extra_fonts/Cousine-Regular.ttf", 15.0f);
    // io.Fonts->AddFontFromFileTTF("../../extra_fonts/DroidSans.ttf", 16.0f);
    // io.Fonts->AddFontFromFileTTF("../../extra_fonts/Roboto-Medium.ttf", 16.0f);
    // io.Fonts->AddFontFromFileTTF("../../extra_fonts/ProggyTiny.ttf", 10.0f);
    // io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());

    // ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    const ImVec4 clear_color(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
    while (glfwWindowShouldClose(window) == GLFW_FALSE) {
        glfwPollEvents();
        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        // glfwSetFramebufferSizeCallback(window, opengl.framebufferSizeCallback );

        // Rendering
        s32 display_w{};
        s32 display_h{};
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        gui::buildGui(state, opengl, display_w, display_h);

        if (state.renderingStatus() == core::RenderingStatus::reset) { glfwSetWindowShouldClose(window, GLFW_TRUE); }

        ImGui::Render();

        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

        glfwMakeContextCurrent(window);
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}

}; // namespace saturnin::video
