// ImGui - standalone example application for Glfw + OpenGL 2, using fixed pipeline
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.

#include <epoxy/gl.h>
#include <epoxy/wgl.h>
#include <imgui.h>
#include "imgui_impl_glfw.h"
#include <iostream> // cout
#include <GLFW/glfw3.h>
#include "opengl_legacy.h"

using namespace std;

static HPBUFFERARB pBuffer;
static HDC         pBufferHDC, screenHDC;
static HGLRC       pBufferCtx, screenCtx;
static GLuint      pBufferTex, screenTex;

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

        cout << epoxy_gl_version() << endl;

        epoxy_handle_external_wglMakeCurrent();
        if (!epoxy_has_wgl_extension(wglGetCurrentDC(), "WGL_ARB_pbuffer"))
            cout << "WGL_ARB_pbuffer not found !" << endl;

        if (!epoxy_has_wgl_extension(wglGetCurrentDC(), "WGL_ARB_pixel_format"))
            cout << "WGL_ARB_pixel_format not found !" << endl;

        if (!epoxy_has_wgl_extension(wglGetCurrentDC(), "WGL_ARB_render_texture"))
            cout << "WGL_ARB_render_texture not found !" << endl;

        init();
        glDisable(GL_SCISSOR_TEST);
        glDisable(GL_ALPHA_TEST);
        glDisable(GL_STENCIL_TEST);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_DITHER);
        glDisable(GL_INDEX_LOGIC_OP);
        glDisable(GL_COLOR_LOGIC_OP);
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glShadeModel(GL_FLAT);

        //glEnable(GL_BLEND);
        glEnable(GL_TEXTURE_2D);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        //glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
        glDisable(GL_LIGHTING);

        glLoadIdentity();

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


                if (!wglMakeCurrent(pBufferHDC, pBufferCtx)) __debugbreak();
                glBindTexture(GL_TEXTURE_2D, pBufferTex);

                glClearColor(0, 0, 0, 0);
                glClear(GL_COLOR_BUFFER_BIT);
                glMatrixMode(GL_MODELVIEW);
                glLoadIdentity();

                glBegin(GL_TRIANGLES);
                // Top face (y = 1.0f)
                glColor3f(0.0f, 1.0f, 0.0f);     // Green
                glVertex2f(-0.5f, -0.5);
                glVertex2f(0.0f, 0.5f);
                glVertex2f(0.5f, -0.5f);
                glEnd();

                if (!wglMakeCurrent(screenHDC, screenCtx))   __debugbreak();
                glBindTexture(GL_TEXTURE_2D, screenTex);

                glMatrixMode(GL_MODELVIEW);
                glLoadIdentity();

                if (!wglBindTexImageARB(pBuffer, WGL_FRONT_LEFT_ARB))                           __debugbreak();
                //glCallList(list);

                
                ImGui::GetWindowDrawList()->AddImage(
                    (void *)pBufferTex,
                    ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y),
                    ImVec2(ImGui::GetCursorScreenPos().x + 320, ImGui::GetCursorScreenPos().y + 200),
                    ImVec2(0, 1), ImVec2(1, 0));

                if (!wglReleaseTexImageARB(pBuffer, WGL_FRONT_LEFT_ARB))                        __debugbreak();
                

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

    static void initPbuffer()
    {
        // Create the pbuffer with the help of glew and glut
        int ia[] = {
            WGL_DRAW_TO_PBUFFER_ARB, true,
            WGL_BIND_TO_TEXTURE_RGBA_ARB, true,
            WGL_DEPTH_BITS_ARB, 24,
            WGL_RED_BITS_ARB,   8,
            WGL_GREEN_BITS_ARB, 8,
            WGL_BLUE_BITS_ARB,  8,
            0, 0
        };

        float fa[] = {
            0, 0
        };
        int fmts[64]; unsigned nfmts = 0;
        if (!wglChoosePixelFormatARB(wglGetCurrentDC(), ia, fa, _countof(fmts), fmts, &nfmts) || !nfmts) {
            printf("wglChoosePixelFormat FAILED -- nfmts %d,  GetLastError 0x%08X\n", nfmts, GetLastError());
            getchar();  exit(0);
        }

        int pb[] = {
            WGL_TEXTURE_FORMAT_ARB, WGL_TEXTURE_RGBA_ARB,
            WGL_TEXTURE_TARGET_ARB, WGL_TEXTURE_2D_ARB,
            WGL_PBUFFER_LARGEST_ARB, true,
            0, 0
        };
        if (!(pBuffer = wglCreatePbufferARB(wglGetCurrentDC(), fmts[0], 320, 200, pb)))         __debugbreak();
        if (!(pBufferHDC = wglGetPbufferDCARB(pBuffer)))                                        __debugbreak();
        if (!(pBufferCtx = wglCreateContext(pBufferHDC)))                                       __debugbreak();

        // Get it's actual size
        int w;  if (!wglQueryPbufferARB(pBuffer, WGL_PBUFFER_WIDTH_ARB, &w))                    __debugbreak();
        int h;  if (!wglQueryPbufferARB(pBuffer, WGL_PBUFFER_HEIGHT_ARB, &h))                   __debugbreak();

        // Initialize it's projection matrix
        if (!wglMakeCurrent(pBufferHDC, pBufferCtx))                                            __debugbreak();
        //reshape(w, h);
        glViewport(0, 0, 320, 200);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        if (!wglMakeCurrent(screenHDC, screenCtx))                                              __debugbreak();
        if (!wglShareLists(screenCtx, pBufferCtx))                                              __debugbreak();
    }

    static char *circles[] = {
        "................",
        "................",
        "......xxxx......",
        "....xxxxxxxx....",
        "...xxxxxxxxxx...",
        "...xxx....xxx...",
        "..xxx......xxx..",
        "..xxx......xxx..",
        "..xxx......xxx..",
        "..xxx......xxx..",
        "...xxx....xxx...",
        "...xxxxxxxxxx...",
        "....xxxxxxxx....",
        "......xxxx......",
        "................",
        "................",
    };

    static void initTextures()
    {
        GLubyte floorTexture[16][16][3];
        GLubyte *loc;
        int s, t;

        /* Setup RGB image for the texture. */
        loc = (GLubyte*)floorTexture;
        for (t = 0; t < 16; t++) {
            for (s = 0; s < 16; s++) {
                if (circles[t][s] == 'x') {
                    /* Nice green. */
                    loc[0] = 0x1f;
                    loc[1] = 0x8f;
                    loc[2] = 0x1f;
                }
                else {
                    /* Light gray. */
                    loc[0] = 0xaa;
                    loc[1] = 0xaa;
                    loc[2] = 0xaa;
                }
                loc += 3;
            }
        }

        // create, configure and initialize the textures
        glGenTextures(1, &pBufferTex);
        glBindTexture(GL_TEXTURE_2D,  pBufferTex);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, 3, 16, 16, 0, GL_RGB, GL_UNSIGNED_BYTE, floorTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    }

    void init(void)
    {
        screenHDC = wglGetCurrentDC();
        screenCtx = wglGetCurrentContext();
        initPbuffer();
        initTextures();
    }
};
};