// ImGui - standalone example application for Glfw + OpenGL 2, using fixed pipeline
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.

#include <epoxy/gl.h>
#include <epoxy/wgl.h>
#include <imgui.h>
#include "imgui_impl_glfw.h"
#include <iostream> // cout
#include <GLFW/glfw3.h>
#include "opengl_legacy.h"
#include "opengl.h"

using namespace std;

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

        //uint32_t framebuffer = 0;
        //glGenFramebuffers(1, &framebuffer);
        //glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

        OpenGl opengl;
        uint32_t fbo = opengl.createFramebuffer();
        
        //init();
        //glDisable(GL_SCISSOR_TEST);
        //glDisable(GL_ALPHA_TEST);
        //glDisable(GL_STENCIL_TEST);
        //glDisable(GL_DEPTH_TEST);
        //glDisable(GL_DITHER);
        //glDisable(GL_INDEX_LOGIC_OP);
        //glDisable(GL_COLOR_LOGIC_OP);
        //glClearColor(0.0, 0.0, 0.0, 0.0);
        //glShadeModel(GL_FLAT);

        ////glEnable(GL_BLEND);
        //glEnable(GL_TEXTURE_2D);
        ////glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        ////glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
        //glDisable(GL_LIGHTING);

        //glLoadIdentity();

        //glOrtho(0, 0, 320, 200); // Positionning Saturn coordinates
     

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
        ImVec4 clear_color       = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

        // Main loop
        while (!glfwWindowShouldClose(window))
        {
            glfwPollEvents();
            ImGui_ImplGlfwGL2_NewFrame();

            // 1. Show a simple window
            // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
            {
                static float f = 0.0f;
                ImGui::Text("Hello, world!");
                ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
                ImGui::ColorEdit3("clear color", (float*)&clear_color);
                if (ImGui::Button("Test Window")) show_test_window ^= 1;
                if (ImGui::Button("Another Window")) show_another_window ^= 1;
                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            }

            // 2. Show another simple window, this time using an explicit Begin/End pair
            if (show_another_window)
            {
                ImGui::Begin("Another Window", &show_another_window);
                ImGui::Text("Hello from another window!");
                ImGui::End();
            }

            // 3. Show the ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
            if (show_test_window)
            {
                ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
                ImGui::ShowTestWindow(&show_test_window);
            }

            if (show_video) {
                ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
                ImGui::SetNextWindowSize(ImVec2(320, 200 + 20)); // + 20 

                ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
                
                ImGui::Begin("Video rendering", &show_video);


                ////if (!wglMakeCurrent(pBufferHDC, pBufferCtx)) __debugbreak();
                ////glBindTexture(GL_TEXTURE_2D, pBufferTex);

                ////glClearColor(0, 0, 0, 0);
                ////glClear(GL_COLOR_BUFFER_BIT);
                ////glMatrixMode(GL_MODELVIEW);
                ////glLoadIdentity();

                ////glBegin(GL_TRIANGLES);
                ////// Top face (y = 1.0f)
                ////glColor3f(0.0f, 1.0f, 0.0f);     // Green
                ////glTexCoord2f(0.0, 0.0); glVertex3f(1.0f, 1.0f, -1.0f);
                ////glTexCoord2f(0.0, 1.0); glVertex3f(1.0f, 1.0f, 1.0f);
                ////glTexCoord2f(1.0, 1.0); glVertex3f(1.0f, -1.0f, 1.0f);
                ////glTexCoord2f(1.0, 0.0); glVertex3f(1.0f, -1.0f, -1.0f);
                ////glVertex2f(-0.5f, -0.5);
                ////glVertex2f(0.0f, 0.5f);
                ////glVertex2f(0.5f, -0.5f);
                ////glEnd();

                ////if (!wglMakeCurrent(screenHDC, screenCtx))   __debugbreak();
                ////glBindTexture(GL_TEXTURE_2D, screenTex);

                ////glMatrixMode(GL_MODELVIEW);
                ////glLoadIdentity();

                ////if (!wglBindTexImageARB(pBuffer, WGL_FRONT_LEFT_ARB))                           __debugbreak();
                //////glCallList(list);

                ////
                ////ImGui::GetWindowDrawList()->AddImage(
                ////    (void *)pBufferTex,
                ////    ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y),
                ////    ImVec2(ImGui::GetCursorScreenPos().x + 320, ImGui::GetCursorScreenPos().y + 200),
                ////    ImVec2(0, 1), ImVec2(1, 0));

                ////if (!wglReleaseTexImageARB(pBuffer, WGL_FRONT_LEFT_ARB))                        __debugbreak();
                

                //glBindFramebuffer(GL_FRAMEBUFFER, fbo);
                //glViewport(0, 0, 320, 200);
                //glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                //glClear(GL_COLOR_BUFFER_BIT);

                ////opengl.drawTriangle();

                //GLint ret;
                //glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &ret);
                //if (ret != GL_NONE) {
                //    int32_t texture = 0;
                //    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &texture);

                //    ImGui::GetWindowDrawList()->AddImage(
                //        (void *)texture,
                //        ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y),
                //        ImVec2(ImGui::GetCursorScreenPos().x + 320, ImGui::GetCursorScreenPos().y + 200),
                //        ImVec2(0, 1), ImVec2(1, 0));

                //}
                //glBindFramebuffer(GL_FRAMEBUFFER, 0);

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