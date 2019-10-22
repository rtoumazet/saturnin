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

#include <imgui.h>
#include <filesystem> // filesystem
#include "gui.h"
#include "../locale.h" // tr
#include "../utilities.h" // stringToVector
#include "../cdrom/cdrom.h" // Cdrom


//namespace core  = saturnin::core;
namespace util  = saturnin::utilities;
namespace cdrom = saturnin::cdrom;

namespace saturnin {
namespace gui {

    using core::Log;
    using core::tr;

    static const float icon_map_height{ 1139 };
    static const float icon_height{ 104 };

    static const ImVec2 icon_play_uv0(0, icon_height * 0 / icon_map_height);
    static const ImVec2 icon_play_uv1(1, icon_height * 1 / icon_map_height);

    static const ImVec2 icon_pause_uv0(0, icon_height * 1 / icon_map_height);
    static const ImVec2 icon_pause_uv1(1, icon_height * 2 / icon_map_height);

    static const ImVec2 icon_stop_uv0(0, icon_height * 2 / icon_map_height);
    static const ImVec2 icon_stop_uv1(1, icon_height * 3 / icon_map_height);

    //void show_simple_window(bool& show_test_window, bool& show_another_window) {
    //    // 1. Show a simple window
    //    // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
    //    {
    //        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    //        static float f = 0.0f;
    //        ImGui::Text("Hello, world!");
    //        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
    //        ImGui::ColorEdit3("clear color", (float*)&clear_color);
    //        if (ImGui::Button("Test Window")) show_test_window ^= 1;
    //        if (ImGui::Button("Another Window")) show_another_window ^= 1;
    //        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    //    }
    //}

    //void show_another_window(bool& show_another_window) {
    //    // 2. Show another simple window, this time using an explicit Begin/End pair
    //    if (show_another_window)
    //    {
    //        ImGui::Begin("Another Window", &show_another_window);
    //        ImGui::Text("Hello from another window!");
    //        ImGui::End();
    //    }
    //}

    void show_test_window(bool& show_test_window) {
        // 3. Show the ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
        if (show_test_window)
        {
            ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
            ImGui::ShowDemoWindow();
        }
    }

    //void showCoreWindow(const uint32_t tex) {
    void showCoreWindow(video::Opengl& opengl) {
        ImGuiWindowFlags window_flags = 0;
        window_flags |= ImGuiWindowFlags_NoTitleBar;
        window_flags |= ImGuiWindowFlags_NoResize;
        window_flags |= ImGuiWindowFlags_NoScrollbar;

        
        //std::vector<uint8_t> icons;
        //uint32_t tex = opengl.loadIcons(icons);
        
        ImGui::SetNextWindowPos(ImVec2(400, 0), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(300, 40));

        
        bool show_window;
        ImGui::Begin("Core", &show_window, window_flags);
        //if (ImGui::Button("Play")) show_test_window ^= 1;

        if (ImGui::ImageButton(reinterpret_cast<ImTextureID>(opengl.iconsTextureId), ImVec2(20, 20), icon_play_uv0, icon_play_uv1)) {

        }

        //if (ImGui::Button("Play")) {
        //    ImGui::OpenPopup("testpopup");
        //}

        //if (ImGui::BeginPopup("testpopup")) {
        //    ImGui::Text("TEST");
        //    ImGui::EndPopup();
        //}

        //ImGui::ImageButton((ImTextureID)tex, ImVec2(20, 20), ImVec2(0.1000, 0.1000), ImVec2(0.2333, 0.2333), 0);
        ImGui::SameLine();
        ImGui::ImageButton(reinterpret_cast<ImTextureID>(opengl.iconsTextureId), ImVec2(20, 20), icon_pause_uv0, icon_pause_uv1);
        //ImGui::ImageButton((ImTextureID)tex, ImVec2(20, 20), ImVec2(0.1000, 0.5000), ImVec2(0.2333, 0.5000), 0);
        ImGui::SameLine();
        ImGui::ImageButton(reinterpret_cast<ImTextureID>(opengl.iconsTextureId), ImVec2(20, 20), icon_stop_uv0, icon_stop_uv1);
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

    void showRenderingWindow(video::Opengl& opengl, u32 width, u32 height) {
        ImGui::SetNextWindowPos(ImVec2(0, 0 + 20), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(static_cast<float>(width), static_cast<float>(height + 20))); // + 20 


        //ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        //ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.0f);
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar;
        flags |= ImGuiWindowFlags_NoResize;
        flags |= ImGuiWindowFlags_NoScrollbar;
        flags |= ImGuiWindowFlags_NoInputs;
        flags |= ImGuiWindowFlags_NoSavedSettings;
        flags |= ImGuiWindowFlags_NoFocusOnAppearing;
        flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

        ImGui::Begin("Video rendering", NULL, flags);
        //ImGui::BeginChild("Video rendering");

        if (opengl.isWindowResized(width, height)) {
            opengl.initializeTexture(width, height);
        }
        
        opengl.preRender();

        opengl.render();
        if (opengl.texture() != 0) {
            gui::renderToTexture(opengl.texture(), width, height);
        }

        opengl.postRender();

        ImGui::End();
        ImGui::PopStyleVar();
        //ImGui::PopStyleVar();
        //ImGui::PopStyleVar();
    }

    void showStvWindow(bool *opened) {
        auto files = core::listStvConfigurationFiles();

        static int listbox_item_current = 1;
        
        ImGui::Begin("ST-V window", opened);
        ImGui::Combo("", &listbox_item_current, files);
        ImGui::End();
    }

    void showOptionsWindow(core::Emulator_context& state, bool *opened) {

        static bool reset_rendering{}; // used to check if rendering has to be reset after changing the option
        ImGui::SetNextWindowSize(ImVec2(600, 300));
        ImGui::Begin("Options", opened);
        ImGui::PushItemWidth(-10);
        ImGui::Spacing();

        
        // General header
        if (ImGui::CollapsingHeader(tr("General").c_str())) {
            ImGui::Text(tr("Hardware mode").c_str());
            ImGui::SameLine(150);
            
            std::string HardwareMode = state.config()->readValue(core::Access_keys::config_HardwareMode);
            static int mode = util::toUnderlying(core::Config::HardwareMode[HardwareMode]);

            if (ImGui::RadioButton("Saturn", &mode, util::toUnderlying(core::HardwareMode::saturn))) {
                core::Config::Map_HardwareMode::const_iterator it = util::getKeyFromValue(core::Config::HardwareMode, core::HardwareMode::saturn);
                if (it != core::Config::HardwareMode.end()) state.config()->writeValue(core::Access_keys::config_HardwareMode, it->first);
                else Log::warning("config", tr("Hardware mode unknown ..."));
            }
            ImGui::SameLine();
            if (ImGui::RadioButton("ST-V", &mode, util::toUnderlying(core::HardwareMode::stv))) {
                core::Config::Map_HardwareMode::const_iterator it = util::getKeyFromValue(core::Config::HardwareMode, core::HardwareMode::stv);
                if (it != core::Config::HardwareMode.end()) state.config()->writeValue(core::Access_keys::config_HardwareMode, it->first);
                else Log::warning("config", tr("Hardware mode unknown ..."));
            }

            ImGui::Text(tr("Language").c_str());
            ImGui::SameLine(150);

            static auto locales = state.config()->listAvailableLanguages();
            std::string l = state.config()->readValue(core::Access_keys::config_language);
            auto it = std::find_if(locales.begin(), locales.end(), [&l](std::string& str) {
                return l == str;
            });
            static int index = it - locales.begin();
            if (ImGui::Combo("##language", &index, locales)) {
                state.config()->writeValue(core::Access_keys::config_language, locales[index]);
            }
        }

        // Rendering header
        if (ImGui::CollapsingHeader(tr("Rendering").c_str())) {
            ImGui::Text(tr("Legacy OpenGL").c_str());
            ImGui::SameLine(150);

            static bool is_legacy = state.config()->readValue(core::Access_keys::config_legacy_opengl);
            bool initial_rendering = is_legacy;
            if (ImGui::Checkbox("", &is_legacy)) {
                state.config()->writeValue(core::Access_keys::config_legacy_opengl, is_legacy);
                if (initial_rendering != is_legacy) reset_rendering = true;
            }
        }

        // Paths header
        if (ImGui::CollapsingHeader(tr("Paths").c_str())) {
            
            ImGui::Text(tr("Saturn bios").c_str());
            ImGui::SameLine(150);

            auto bios_saturn = util::stringToVector(state.config()->readValue(core::Access_keys::config_bios_saturn), 255);
            if (ImGui::InputText("##bios_saturn", bios_saturn.data(), bios_saturn.capacity())) {
                state.config()->writeValue(core::Access_keys::config_bios_saturn, bios_saturn.data());
            }

            ImGui::Text(tr("ST-V bios").c_str());
            ImGui::SameLine(150);
            auto bios_stv = util::stringToVector(state.config()->readValue(core::Access_keys::config_bios_stv), 255);
            if (ImGui::InputText("##bios_stv", bios_stv.data(), bios_stv.capacity())) {
                state.config()->writeValue(core::Access_keys::config_bios_stv, bios_stv.data());
            }

            ImGui::Text(tr("ST-V roms").c_str());
            ImGui::SameLine(150);
            auto roms_stv = util::stringToVector(state.config()->readValue(core::Access_keys::config_roms_stv), 255);
            if (ImGui::InputText("##roms_stv", roms_stv.data(), roms_stv.capacity())) {
                state.config()->writeValue(core::Access_keys::config_roms_stv, roms_stv.data());
            }

            ImGui::Separator();
        }

        // CD-ROM header
        if (ImGui::CollapsingHeader(tr("CD-Rom").c_str())) {
            // Drive
            ImGui::Text(tr("Drive").c_str());
            ImGui::SameLine(150);

            std::string drive = state.config()->readValue(core::Access_keys::config_drive);
            static int current_item{};
            auto drive_parts = util::explode(drive, ':');
            if (drive_parts.size() == 3) {
                current_item = cdrom::Cdrom::getDriveIndice(std::stoi(drive_parts[0]), std::stoi(drive_parts[1]), std::stoi(drive_parts[2]));
            }
            
            if (ImGui::Combo("", &current_item, cdrom::Cdrom::scsi_drives_list)) {
                std::string value = std::to_string(cdrom::Cdrom::di_list[current_item].path);
                value += ':';
                value += std::to_string(cdrom::Cdrom::di_list[current_item].target);
                value += ':';
                value += std::to_string(cdrom::Cdrom::di_list[current_item].lun);
                state.config()->writeValue(core::Access_keys::config_drive, value);
            }
            
            // Access method
            // For now ASPI isn't supported, SPTI is used in every case
            ImGui::Text(tr("Access method").c_str());
            ImGui::SameLine(150);

            std::string access_method = state.config()->readValue(core::Access_keys::config_access_method);
            static int method = util::toUnderlying(core::Config::cdrom_access[access_method]);
            if (ImGui::RadioButton("SPTI", &method, util::toUnderlying(cdrom::Cdrom_access_method::spti))) {
                core::Config::Map_cdrom_access::const_iterator it = util::getKeyFromValue(core::Config::cdrom_access, cdrom::Cdrom_access_method::spti);
                if (it != core::Config::cdrom_access.end()) {
                    state.config()->writeValue(core::Access_keys::config_access_method, it->first);
                }
                else {
                    Log::warning("config", tr("Drive access method unknown ..."));
                }
            }
            ImGui::SameLine();
            if (ImGui::RadioButton("ASPI", &method, util::toUnderlying(cdrom::Cdrom_access_method::aspi))) {
                core::Config::Map_cdrom_access::const_iterator it = util::getKeyFromValue(core::Config::cdrom_access, cdrom::Cdrom_access_method::aspi);
                if (it != core::Config::cdrom_access.end()) {
                    state.config()->writeValue(core::Access_keys::config_access_method, it->first);
                }
                else {
                    Log::warning("config", tr("Drive access method unknown ..."));
                }
            }

            // CD-Rom system ID
        }

        // Sound header
        if (ImGui::CollapsingHeader(tr("Sound").c_str())) {
            ImGui::Text(tr("Sound disabled").c_str());
            ImGui::SameLine(150);

            static bool disabled = state.config()->readValue(core::Access_keys::config_sound_disabled);
            if (ImGui::Checkbox("", &disabled)) {
                state.config()->writeValue(core::Access_keys::config_sound_disabled, disabled);
            }
        }

        static uint16_t counter{};
        static std::string status_message{};
        if (ImGui::Button("Save")) {
            state.config()->writeFile();
            status_message = tr("Configuration saved.");
            counter = 5 * 60;

            if (reset_rendering) {
                state.renderingStatus_ = core::RenderingStatus::reset;
                reset_rendering = false;
            }
        }
        
        if (counter > 0) --counter;
        else status_message.clear();

        ImGui::Text(status_message.c_str());

        ImGui::End();
    }

    void buildGui(core::Emulator_context& state, video::Opengl& opengl, u32 width, u32 height) {
        static bool show_options     = false;
        static bool show_load_stv    = false;
        static bool show_load_binary = false;

        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu(tr("File").c_str())) {
                ImGui::MenuItem(tr("Load ST-V rom").c_str(), NULL, &show_load_stv);
                ImGui::MenuItem(tr("Load binary file").c_str(), NULL, &show_load_binary);
                ImGui::EndMenu();
            }

            ImGui::MenuItem(tr("Options").c_str(), NULL, &show_options);
        }
        ImGui::EndMainMenuBar();

        showCoreWindow(opengl);
        
        showRenderingWindow(opengl, width, height);

        if (show_options)   showOptionsWindow(state, &show_options);
        if (show_load_stv)  showStvWindow(&show_load_stv);
    }

    void renderToTexture(int32_t texture, const uint32_t width, const uint32_t height) {
        //ImGui::GetWindowDrawList()->AddImage(
        //    (ImTextureID)(intptr_t)texture,
        //    ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y), 
        //    ImVec2(ImGui::GetCursorScreenPos().x + width, ImGui::GetCursorScreenPos().y + height),
        //    ImVec2(0, 0), ImVec2(1, 1));
        ImGui::GetWindowDrawList()->AddImage(
            (ImTextureID)(intptr_t)texture,
            ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y), 
            ImVec2(ImGui::GetCursorScreenPos().x + width, ImGui::GetCursorScreenPos().y + height),
            ImVec2(0, 1), ImVec2(1, 0));
    }
}
}
