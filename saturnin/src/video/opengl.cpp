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
#include <epoxy/gl.h>
#include <epoxy/wgl.h>
#include <GLFW/glfw3.h>
#include "opengl.h"
#include "gui.h"
#include "bindings/imgui_impl_glfw.h"
#include "bindings/imgui_impl_glfw_gl3.h"
#include "../../lib/lodepng/lodepng.h"
#include "../../res/icons.png.inc"

using namespace std;

namespace saturnin {
namespace video {
    
    uint32_t OpenGl::createFramebuffer()
    {
        uint32_t framebuffer = 0;
        glGenFramebuffers(1, &framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        // create a color attachment texture
        uint32_t textureColorbuffer;
        glGenTextures(1, &textureColorbuffer);
        glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 320, 200, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureColorbuffer, 0);
        GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
        glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers
        // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        return framebuffer;
    }
    
    uint32_t OpenGl::createVertexShader()
    {
        uint32_t vertexShader;
        vertexShader = glCreateShader(GL_VERTEX_SHADER);

        const char* vertexShaderSource = R"(
		    #version 330 core
		    layout(location = 0) in vec3 aPos;

		    void main()
		    {
			    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
		    }
        )";

        glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
        glCompileShader(vertexShader);

        int32_t  success;
        
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            char infoLog[512];
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        return vertexShader;
    }
    uint32_t OpenGl::createFragmentShader()
    {
        const char* fragmentShaderSource = R"(
        	#version 330 core
        	out vec4 FragColor;

        	void main()
        	{
        	    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
        	} 
        )";

        uint32_t fragmentShader;
        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
        glCompileShader(fragmentShader);

        return fragmentShader;
    }
    uint32_t OpenGl::createVertexBufferObject(const float vertices[])
    {
        uint32_t vbo;
        glGenBuffers(1, &vbo);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        return vbo;
    }
    
    void OpenGl::deleteShaders(std::vector<uint32_t> shaders)
    {
        for (auto shader : shaders) {
            glDeleteShader(shader);
        }
    }
    void OpenGl::setupTriangle()
    {
        float vertices[] = {
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            0.0f,  0.5f, 0.0f
        };

        //uint32_t vbo = createVertexBufferObject(vertices);
        uint32_t vertexShader = createVertexShader();
        uint32_t fragmentShader = createFragmentShader();
        programShader_ = createProgramShader(vertexShader, fragmentShader);
        vector<uint32_t> shadersToDelete = {vertexShader, fragmentShader};
        deleteShaders(shadersToDelete);

        unsigned int VBO;
        glGenVertexArrays(1, &vao_);
        glGenBuffers(1, &VBO);
        // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
        glBindVertexArray(vao_);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
        // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
        glBindVertexArray(0);

    }
    void OpenGl::drawTriangle()
    {
        glUseProgram(programShader_);
        glBindVertexArray(vao_); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    /* static */
    bool OpenGl::isModernOpenGlCapable()
    {
        bool isCapable = true;
        if (!glfwInit()) isCapable = false;
        else {
            GLFWwindow* window = glfwCreateWindow(1280, 720, "Test", NULL, NULL);
            if (window == nullptr) isCapable = false;
            else {
                uint32_t major = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR);
                uint32_t minor = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR);

                if (major = 3) {
                    if (minor < 3) isCapable = false;
                }
                else if (major < 3) isCapable = false;

                glfwDestroyWindow(window);
            }
            glfwTerminate();
        }
        return isCapable;
    }

    uint32_t OpenGl::createProgramShader(const uint32_t vertexShader, const uint32_t fragmentShader)
    {
        uint32_t shaderProgram;
        shaderProgram = glCreateProgram();

        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);

        GLint success = 0;
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            char infoLog[256];
            glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
            cout << "ERROR::SHADER::PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
        }

        return shaderProgram;
    }
    uint32_t OpenGl::createVertexArrayObject(const uint32_t vertexBufferObject, const float vertices[])
    {
        uint32_t VAO;
        glGenVertexArrays(1, &VAO);

        // 1. bind Vertex Array Object
        glBindVertexArray(VAO);
        // 2. copy our vertices array in a buffer for OpenGL to use
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // 3. then set our vertex attributes pointers
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        return VAO;
    }

    bool OpenGl::loadPngImage(const vector<uint8_t>& sourceData, vector<uint8_t>& image) {
        // Load file and decode image.
        uint32_t width {};
        uint32_t height {};
        uint32_t error = lodepng::decode(image, width, height, sourceData, LCT_RGBA);

        // If there's an error, display it.
        if (error != 0)
        {
            std::cout << "error " << error << ": " << lodepng_error_text(error) << std::endl;
            return false;
        }

        return true;
    }

    bool OpenGl::loadIcons(vector<uint8_t>& image) {
        //opengl.loadPngImage("D:/Dev/Sources/VS2017/saturnin-vs2017/saturnin/res/icons.png");
        std::vector<uint8_t> icons_vector(icons_png, icons_png + sizeof(icons_png));
        return loadPngImage(icons_vector, image);
    }

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

        bool show_test_window = true;
        bool show_another_window = false;
        bool show_video = true;
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

    int32_t runModernOpenGl()
    {
        // Setup window
        glfwSetErrorCallback(error_callback);
        if (!glfwInit())
            return 1;
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
#if __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
        auto window = glfwCreateWindow(1280, 720, "ImGui OpenGL3 example", NULL, NULL);
        if (window == nullptr) {

            return 1;
        }
        glfwMakeContextCurrent(window);
        glfwSwapInterval(1); // Enable vsync

                             // Setup ImGui binding
        ImGui_ImplGlfwGL3_Init(window, true);

        // Load Fonts
        // (there is a default font, this is only if you want to change it. see extra_fonts/README.txt for more details)
        //ImGuiIO& io = ImGui::GetIO();
        //io.Fonts->AddFontDefault();
        //io.Fonts->AddFontFromFileTTF("../../extra_fonts/Cousine-Regular.ttf", 15.0f);
        //io.Fonts->AddFontFromFileTTF("../../extra_fonts/DroidSans.ttf", 16.0f);
        //io.Fonts->AddFontFromFileTTF("../../extra_fonts/Roboto-Medium.ttf", 16.0f);
        //io.Fonts->AddFontFromFileTTF("../../extra_fonts/ProggyTiny.ttf", 10.0f);
        //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());

        bool show_test_window = true;
        bool show_another_window = false;
        bool show_video = true;
        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

        OpenGl opengl;
        uint32_t fbo = opengl.createFramebuffer();
        opengl.setupTriangle();

        std::vector<uint8_t> image;
        opengl.loadIcons(image);
        GLuint tex;
        glEnable(GL_TEXTURE_2D);
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexImage2D(GL_TEXTURE_2D, 0, 4, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);

        // Main loop
        while (!glfwWindowShouldClose(window))
        {
            glfwPollEvents();
            ImGui_ImplGlfwGL3_NewFrame();

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

                //opengl.setupTriangle();
                opengl.drawTriangle();

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
            ImGui::Render();
            glfwSwapBuffers(window);
        }

        // Cleanup
        ImGui_ImplGlfwGL3_Shutdown();
        glfwTerminate();

        return 0;
    }

};
};