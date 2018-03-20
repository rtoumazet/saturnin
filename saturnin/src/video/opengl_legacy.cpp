// 
// opengl_legacy.cpp
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
#include <epoxy/gl.h>
#include <epoxy/wgl.h>
//#include "imgui/imgui.h"
#include "gui.h"
#include "bindings/imgui_impl_glfw.h"
#include <GLFW/glfw3.h>
#include "opengl_legacy.h"
#include "opengl.h"

using namespace std;
using namespace saturnin;

//static HPBUFFERARB pBuffer;
//static HDC         pBufferHDC, screenHDC;
//static HGLRC       pBufferCtx, screenCtx;
//static GLuint      pBufferTex, screenTex;

namespace saturnin {
namespace video {

    static void error_callback(int error, const char* description)
    {
        fprintf(stderr, "Error %d: %s\n", error, description);
    }

    int32_t runLegacyOpenGl()
    {
        // Setup window
        glfwSetErrorCallback(error_callback);
        if (!glfwInit())
            return 1;
        
        auto window = glfwCreateWindow(1280, 720, "ImGui OpenGL2 example", NULL, NULL);
        glfwMakeContextCurrent(window);
        glfwSwapInterval(1); // Enable vsync

        //cout << epoxy_gl_version() << endl;

        //epoxy_handle_external_wglMakeCurrent();
        //if (!epoxy_has_wgl_extension(wglGetCurrentDC(), "WGL_ARB_pbuffer"))
        //    cout << "WGL_ARB_pbuffer not found !" << endl;

        //if (!epoxy_has_wgl_extension(wglGetCurrentDC(), "WGL_ARB_pixel_format"))
        //    cout << "WGL_ARB_pixel_format not found !" << endl;

        //if (!epoxy_has_wgl_extension(wglGetCurrentDC(), "WGL_ARB_render_texture"))
        //    cout << "WGL_ARB_render_texture not found !" << endl;
        //
        //if (!epoxy_has_gl_extension("GL_ARB_framebuffer_object"))
        //    cout << "GL_ARB_framebuffer_object not found !" << endl;
        //
        //if (!epoxy_has_gl_extension("GL_EXT_framebuffer_object"))
        //    cout << "GL_EXT_framebuffer_object not found !" << endl;

        OpenGl opengl;
        uint32_t fbo = opengl.createFramebuffer();
        //opengl.setupTriangle();
        

        // Setup ImGui binding
        ImGui_ImplGlfwGL2_Init(window, true);

        // Load Fonts
        // (there is a default font, this is only if you want to change it. see extra_fonts/README.txt for more details)
        //ImGuiIO& io = ImGui::GetIO();
        //io.Fonts->AddFontDefault();
        //io.Fonts->AddFontFromFileTTF("../../extra_fonts/Cousine-Regular.ttf", 15.0f);
        //io.Fonts->AddFontFromFileTTF("../../extra_fonts/DroidSans.ttf", 16.0f);
        //io.Fonts->AddFontFromFileTTF("../../extra_fonts/Roboto-Medium.ttf", 16.0f);
        //io.Fonts->AddFontFromFileTTF("../../extra_fonts/ProggyTiny.ttf", 10.0f);
        //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());

        bool show_test_window    = true;
        bool show_another_window = false;
        bool show_video          = true;
        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

        std::vector<uint8_t> image;
        opengl.loadIcons(image);
        GLuint tex;
        glEnable(GL_TEXTURE_2D);
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
        //glTexParameteri(GL_TEXTURE_2D, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        //std::vector<uint8_t> img {0xFF,0x00,0x00,0xFF,0xFF,0x00,0x00,0xFF ,0xFF,0x00,0x00,0xFF ,0x00,0xFF,0x00,0xFF };
        //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, &img[0]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 0xe6, 0xe6, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);

        // Main loop
        while (!glfwWindowShouldClose(window))
        {
            glfwPollEvents();
            ImGui_ImplGlfwGL2_NewFrame();

            gui::showImageWindow(tex);
            
            gui::showCoreWindow(tex);

            gui::showSimpleWindow(show_test_window, show_another_window);

            gui::showAnotherWindow(show_another_window);

            gui::showTestWindow(show_test_window);

            {
                ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
                ImGui::SetNextWindowSize(ImVec2(320, 200 + 20)); // + 20 

                ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
                
                ImGui::Begin("Video rendering", &show_video);

                glBindFramebuffer(GL_FRAMEBUFFER, fbo);
                glViewport(0, 0, 320, 200);
                glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);

                //opengl.drawTriangle();
                glBegin(GL_TRIANGLES);  
                glColor4f(1.0f, 0.5f, 0.2f, 1.0f);
                glVertex3f(-0.5f, -0.5f, 0.0f);
                glVertex3f(0.5f, -0.5f, 0.0f);
                glVertex3f(0.0f, 0.5f, 0.0f);
                glEnd();                      

                GLint ret;
                glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &ret);
                if (ret != GL_NONE) {
                    int32_t texture = 0;
                    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &texture);

                    ImGui::GetWindowDrawList()->AddImage(
                        reinterpret_cast<GLuint*>(texture),
                        ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y),
                        ImVec2(ImGui::GetCursorScreenPos().x + 320, ImGui::GetCursorScreenPos().y + 200),
                        ImVec2(0, 1), ImVec2(1, 0));

                }
                glBindFramebuffer(GL_FRAMEBUFFER, 0);

                ImGui::End();
                ImGui::PopStyleVar();
                ImGui::PopStyleVar();
            }

            // Rendering
            int display_w, display_h;
            glfwGetFramebufferSize(window, &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);
            glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
            glClear(GL_COLOR_BUFFER_BIT);
            //glUseProgram(0); // You may want this if using this code in an OpenGL 3+ context where shaders may be bound



            ImGui::Render();
            glfwSwapBuffers(window);
        }

        // Cleanup
        ImGui_ImplGlfwGL2_Shutdown();
        glfwTerminate();

        return 0;
    }
};
};