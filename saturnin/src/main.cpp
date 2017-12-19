// 
// main.cpp
// Saturnin
//
// Copyright (c) 2003-2017 Renaud Toumazet
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

#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>
#include <stdio.h>
#include "video\opengl.h"
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <boost/system/config.hpp>
#include <boost/locale.hpp>

using namespace std;
using namespace boost::locale;
using namespace saturnin::video;

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error %d: %s\n", error, description);
}

int main(int argc, char *argv[])
{
	// Setup window
	glfwSetErrorCallback(error_callback);
	
	if (!glfwInit())
		return 1;

    
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	GLFWwindow* window = glfwCreateWindow(1280, 720, "ImGui OpenGL3 example", NULL, NULL);
	
    uint32_t major = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR);
    uint32_t minor = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR);
    uint32_t revision = glfwGetWindowAttrib(window, GLFW_CONTEXT_REVISION);
    cout << "Context version :" << major << "." <<minor << " Revision: " << revision << endl;
    glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync
	gl3wInit();

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

    generator gen;
	// Specify location of dictionaries
	gen.add_messages_path(".");
	gen.add_messages_domain("saturnin");
	// Generate locales and imbue them to iostream
	locale::global(gen(""));
	cout.imbue(locale());
	// Display a message using current system locale
	cout << translate("Hello World") << endl;

	// Main loop
	while (!glfwWindowShouldClose(window))
	{

		glfwPollEvents();
		ImGui_ImplGlfwGL3_NewFrame();

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
                    (void *)texture,
                    ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y),
                    ImVec2(ImGui::GetCursorScreenPos().x + 320, ImGui::GetCursorScreenPos().y + 200),
                    ImVec2(0, 1), ImVec2(1, 0));

            }
            glBindFramebuffer(GL_FRAMEBUFFER, 0);



            //ImGui::Text("Hello from another window!");
			
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