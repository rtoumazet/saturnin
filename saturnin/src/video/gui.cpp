// 
// gui.cpp
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

#include "../../lib/imgui/imgui.h"
#include "gui.h"

namespace saturnin {
namespace gui {

    void show_simple_window(bool& show_test_window, bool& show_another_window) {
        // 1. Show a simple window
        // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
        {
            ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
            static float f = 0.0f;
            ImGui::Text("Hello, world!");
            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
            ImGui::ColorEdit3("clear color", (float*)&clear_color);
            if (ImGui::Button("Test Window")) show_test_window ^= 1;
            if (ImGui::Button("Another Window")) show_another_window ^= 1;
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        }
    }

    void show_another_window(bool& show_another_window) {
        // 2. Show another simple window, this time using an explicit Begin/End pair
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);
            ImGui::Text("Hello from another window!");
            ImGui::End();
        }
    }

    void show_test_window(bool& show_test_window) {
        // 3. Show the ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
        if (show_test_window)
        {
            ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
            ImGui::ShowTestWindow();
        }
    }

    void show_core_window(const uint32_t tex) {
        ImGuiWindowFlags window_flags = 0;
        window_flags |= ImGuiWindowFlags_NoTitleBar;
        window_flags |= ImGuiWindowFlags_NoResize;
        window_flags |= ImGuiWindowFlags_NoScrollbar;

        ImGui::SetNextWindowPos(ImVec2(400, 0), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(150, 20));

        
        bool show_window;
        ImGui::Begin("Core", &show_window, window_flags);
        //if (ImGui::Button("Play")) show_test_window ^= 1;
        if (ImGui::Button("Play")) {
            ImGui::OpenPopup("testpopup");
        }

        if (ImGui::BeginPopup("testpopup")) {
            ImGui::Text("TEST");
            ImGui::EndPopup();
        }

        //ImGui::ImageButton((ImTextureID)tex, ImVec2(20, 20), ImVec2(0.1000, 0.1000), ImVec2(0.2333, 0.2333), 0);
        ImGui::SameLine();
        ImGui::Button("Pause");
        //ImGui::ImageButton((ImTextureID)tex, ImVec2(20, 20), ImVec2(0.1000, 0.5000), ImVec2(0.2333, 0.5000), 0);
        ImGui::SameLine();
        ImGui::Button("Stop");
        //ImGui::ImageButton((ImTextureID)tex, ImVec2(20, 20), ImVec2(0.5000, 0.1000), ImVec2(0.5333, 0.2333), 0);
        ImGui::SameLine();

        //ImGui::SameLine();
        //ImGui::PushID(0);
        //ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0 / 7.0f, 0.6f, 0.6f));
        //ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0 / 7.0f, 0.7f, 0.7f));
        //ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0 / 7.0f, 0.8f, 0.8f));
        //ImGui::Button("Click");
        //ImGui::PopStyleColor(3);
        //ImGui::PopID();

        ImGui::End();
    }

    void show_image_window(const uint32_t tex) {
        bool test = true;

        ImGui::Begin("Another Window", &test);
        //ImGui::Image((ImTextureID)tex, ImVec2(230, 230), ImVec2(0, 0), ImVec2(1, 1), ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 128));
        ImGui::Image((ImTextureID)tex, ImVec2(230, 230), ImVec2(0, 0), ImVec2(0.3333, 0.3333), ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 128));
        ImGui::End();
    }
}
}