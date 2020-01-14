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
#include <fstream> // ifstream
#include <thread> // thread
#include "gui.h"
#include "../../lib/imgui/imgui_custom_controls.h" // peripheralKeyCombo
#include "../emulator_enums.h" // EmulationStatus
#include "../config.h"
#include "../locale.h" // tr
#include "../smpc.h" // SaturnDigitalPad, PeripheralKey
#include "../utilities.h" // stringToVector
#include "../cdrom/cdrom.h" // Cdrom


//namespace core  = saturnin::core;
namespace util  = saturnin::utilities;
namespace cdrom = saturnin::cdrom;
namespace fs    = std::filesystem;

namespace saturnin {
namespace gui {

    static bool show_options     = false;
    static bool show_load_stv    = false;
    static bool show_load_binary = false;
    static bool show_demo        = true;
    static bool show_log         = true;

    using core::Log;
    using core::tr;
    using core::SaturnDigitalPad;
    using core::StvBoardControls;
    using core::StvPlayerControls;
    using core::PeripheralKey;
    using core::PeripheralLayout;

    void showImguiDemoWindow(const bool show_test_window) {
        // 3. Show the ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
        if (show_test_window)
        {
            ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
            ImGui::ShowDemoWindow();
        }
    }

    void showCoreWindow(core::EmulatorContext& state, const video::Opengl& opengl) {
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

        if (ImGui::Button("Play")) state.startEmulation();
        ImGui::SameLine();
        ImGui::Button("Pause");
        ImGui::SameLine();
        if (ImGui::Button("Stop")) state.stopEmulation();

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

    void showRenderingWindow(video::Opengl& opengl, const u32 width, const u32 height) {
        ImGui::SetNextWindowPos(ImVec2(0, 0 + 20), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(static_cast<float>(width), static_cast<float>(height + 20))); // + 20 

        //ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        //ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.0f);
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize;
        flags |= ImGuiWindowFlags_NoScrollbar;
        flags |= ImGuiWindowFlags_NoCollapse;
        flags |= ImGuiWindowFlags_NoSavedSettings;
        flags |= ImGuiWindowFlags_NoFocusOnAppearing;
        flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

        ImGui::Begin("Video rendering", nullptr, flags);

        if (opengl.isWindowResized(width, height)) {
            //opengl.initializeTexture(width, height);
            opengl.updateTextureSize(width, height);
        }
        
        opengl.preRender();

        opengl.render();
        if (opengl.texture() != 0) {
            gui::addTextureToDrawList(opengl.texture(), width, height);
        }

        opengl.postRender();

        ImGui::End();
        ImGui::PopStyleVar();
        //ImGui::PopStyleVar();
        //ImGui::PopStyleVar();
    }

    void showStvWindow(bool* opened) {
        auto files = core::listStvConfigurationFiles();

        static int listbox_item_current = 1;
        
        ImGui::Begin("ST-V window", opened);
        ImGui::Combo("", &listbox_item_current, files);
        ImGui::End();
    }

    void showOptionsWindow(core::EmulatorContext& state, bool *opened) {

        static bool reset_rendering{}; // used to check if rendering has to be reset after changing the option
        //ImGui::SetNextWindowSize(ImVec2(600, 300));
        ImGui::Begin("Options", opened);
        ImGui::PushItemWidth(-10);
        ImGui::Spacing();

        
        // General header
        if (ImGui::CollapsingHeader(tr("General").c_str())) {
            ImGui::Text(tr("Hardware mode").c_str());
            ImGui::SameLine(150);
            
            std::string hm = state.config()->readValue(core::AccessKeys::cfg_global_hardware_mode);
            static int mode = util::toUnderlying(core::Config::hardware_mode[hm]);

            if (ImGui::RadioButton("Saturn", &mode, util::toUnderlying(core::HardwareMode::saturn))) {
                core::Config::MapHardwareMode::const_iterator it = util::getKeyFromValue(core::Config::hardware_mode, core::HardwareMode::saturn);
                if (it != core::Config::hardware_mode.end()) state.config()->writeValue(core::AccessKeys::cfg_global_hardware_mode, it->first);
                else Log::warning("config", tr("Unknown hardware mode ..."));
            }
            ImGui::SameLine();
            if (ImGui::RadioButton("ST-V", &mode, util::toUnderlying(core::HardwareMode::stv))) {
                core::Config::MapHardwareMode::const_iterator it = util::getKeyFromValue(core::Config::hardware_mode, core::HardwareMode::stv);
                if (it != core::Config::hardware_mode.end()) state.config()->writeValue(core::AccessKeys::cfg_global_hardware_mode, it->first);
                else Log::warning("config", tr("Unknown hardware mode ..."));
            }

            ImGui::Text(tr("Language").c_str());
            ImGui::SameLine(150);

            static auto locales = state.config()->listAvailableLanguages();
            std::string l = state.config()->readValue(core::AccessKeys::cfg_global_language);
            auto it = std::find_if(locales.begin(), locales.end(), [&l](std::string& str) {
                return l == str;
            });
            static s32 index = static_cast<s32>(it - locales.begin());
            if (ImGui::Combo("##language", &index, locales)) {
                state.config()->writeValue(core::AccessKeys::cfg_global_language, locales[index]);
            }
        }

        // Rendering header
        if (ImGui::CollapsingHeader(tr("Rendering").c_str())) {
            ImGui::Text(tr("TV standard").c_str());
            ImGui::SameLine(150);

            std::string ts = state.config()->readValue(core::AccessKeys::cfg_rendering_tv_standard);
            static int standard = util::toUnderlying(core::Config::tv_standard[ts]);

            if (ImGui::RadioButton("PAL", &standard, util::toUnderlying(video::TvStandard::pal))) {
                core::Config::MapTvStandard::const_iterator it = util::getKeyFromValue(core::Config::tv_standard, video::TvStandard::pal);
                if (it != core::Config::tv_standard.end()) state.config()->writeValue(core::AccessKeys::cfg_rendering_tv_standard, it->first);
                else Log::warning("config", tr("Unknown TV standard ..."));
            }
            ImGui::SameLine();
            if (ImGui::RadioButton("NTSC", &standard, util::toUnderlying(video::TvStandard::ntsc))) {
                core::Config::MapTvStandard::const_iterator it = util::getKeyFromValue(core::Config::tv_standard, video::TvStandard::ntsc);
                if (it != core::Config::tv_standard.end()) state.config()->writeValue(core::AccessKeys::cfg_rendering_tv_standard, it->first);
                else Log::warning("config", tr("Unknown TV standard ..."));
            }

            ImGui::Text(tr("Legacy OpenGL").c_str());
            ImGui::SameLine(150);

            static bool is_legacy = state.config()->readValue(core::AccessKeys::cfg_rendering_legacy_opengl);
            bool initial_rendering = is_legacy;
            if (ImGui::Checkbox("", &is_legacy)) {
                state.config()->writeValue(core::AccessKeys::cfg_rendering_legacy_opengl, is_legacy);
                if (initial_rendering != is_legacy) reset_rendering = true;
            }
        }

        // Paths header
        if (ImGui::CollapsingHeader(tr("Paths").c_str())) {
            
            ImGui::Text(tr("Saturn bios").c_str());
            ImGui::SameLine(150);

            auto bios_saturn = util::stringToVector(state.config()->readValue(core::AccessKeys::cfg_paths_bios_saturn), 255);
            if (ImGui::InputText("##bios_saturn", bios_saturn.data(), bios_saturn.capacity())) {
                state.config()->writeValue(core::AccessKeys::cfg_paths_bios_saturn, bios_saturn.data());
            }

            ImGui::Text(tr("ST-V bios").c_str());
            ImGui::SameLine(150);
            auto bios_stv = util::stringToVector(state.config()->readValue(core::AccessKeys::cfg_paths_bios_stv), 255);
            if (ImGui::InputText("##bios_stv", bios_stv.data(), bios_stv.capacity())) {
                state.config()->writeValue(core::AccessKeys::cfg_paths_bios_stv, bios_stv.data());
            }

            ImGui::Text(tr("ST-V roms").c_str());
            ImGui::SameLine(150);
            auto roms_stv = util::stringToVector(state.config()->readValue(core::AccessKeys::cfg_paths_roms_stv), 255);
            if (ImGui::InputText("##roms_stv", roms_stv.data(), roms_stv.capacity())) {
                state.config()->writeValue(core::AccessKeys::cfg_paths_roms_stv, roms_stv.data());
            }

            ImGui::Separator();
        }

        // CD-ROM header
        if (ImGui::CollapsingHeader(tr("CD-Rom").c_str())) {
            // Drive
            ImGui::Text(tr("Drive").c_str());
            ImGui::SameLine(150);

            std::string drive = state.config()->readValue(core::AccessKeys::cfg_cdrom_drive);
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
                state.config()->writeValue(core::AccessKeys::cfg_cdrom_drive, value);
            }
            
            // Access method
            // For now ASPI isn't supported, SPTI is used in every case
            ImGui::Text(tr("Access method").c_str());
            ImGui::SameLine(150);

            std::string access_method = state.config()->readValue(core::AccessKeys::cfg_cdrom_access_method);
            static int method = util::toUnderlying(core::Config::cdrom_access[access_method]);
            if (ImGui::RadioButton("SPTI", &method, util::toUnderlying(cdrom::CdromAccessMethod::spti))) {
                core::Config::MapCdromAccess::const_iterator it = util::getKeyFromValue(core::Config::cdrom_access, cdrom::CdromAccessMethod::spti);
                if (it != core::Config::cdrom_access.end()) {
                    state.config()->writeValue(core::AccessKeys::cfg_cdrom_access_method, it->first);
                }
                else {
                    Log::warning("config", tr("Unknown drive access method ..."));
                }
            }
            ImGui::SameLine();
            if (ImGui::RadioButton("ASPI", &method, util::toUnderlying(cdrom::CdromAccessMethod::aspi))) {
                core::Config::MapCdromAccess::const_iterator it = util::getKeyFromValue(core::Config::cdrom_access, cdrom::CdromAccessMethod::aspi);
                if (it != core::Config::cdrom_access.end()) {
                    state.config()->writeValue(core::AccessKeys::cfg_cdrom_access_method, it->first);
                }
                else {
                    Log::warning("config", tr("Unknown drive access method ..."));
                }
            }

            // CD-Rom system ID
        }

        // Sound header
        if (ImGui::CollapsingHeader(tr("Sound").c_str())) {
            ImGui::Text(tr("Sound disabled").c_str());
            ImGui::SameLine(150);

            static bool disabled = state.config()->readValue(core::AccessKeys::cfg_sound_disabled);
            if (ImGui::Checkbox("", &disabled)) {
                state.config()->writeValue(core::AccessKeys::cfg_sound_disabled, disabled);
            }
        }

        // Peripheral header
        if (ImGui::CollapsingHeader(tr("Peripherals").c_str())) {

            static const auto keys{ state.smpc()->listAvailableKeys() };

            ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
            if (ImGui::BeginTabBar("PeripheralTabBar", tab_bar_flags)) {
                ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
                ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
                if (ImGui::BeginTabItem("Saturn")) {
                    {
                        //**** Saturn Player 1 ****//
                        static SaturnDigitalPad pad;
                        pad.fromConfig(state.config()->readPeripheralConfiguration(core::AccessKeys::cfg_controls_saturn_player_1));
                        
                        ImGui::BeginChild("ChildSaturnPlayer1", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.5f, 280), true, window_flags);

                        ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth() - 150);

                        ImGui::CenteredText(tr("Player 1"));

                        ImGui::Text(tr("Left").c_str());
                        ImGui::SameLine(150);
                        ImGui::peripheralKeyCombo(keys, pad.direction_left, "direction_left");

                        ImGui::Text(tr("Right").c_str());
                        ImGui::SameLine(150);
                        ImGui::peripheralKeyCombo(keys, pad.direction_right, "direction_right");

                        ImGui::Text(tr("Up").c_str());
                        ImGui::SameLine(150);
                        ImGui::peripheralKeyCombo(keys, pad.direction_up, "direction_up");

                        ImGui::Text(tr("Down").c_str());
                        ImGui::SameLine(150);
                        ImGui::peripheralKeyCombo(keys, pad.direction_down, "direction_down");

                        ImGui::Text(tr("Left shoulder").c_str());
                        ImGui::SameLine(150);
                        ImGui::peripheralKeyCombo(keys, pad.button_shoulder_left, "button_left_shoulder");

                        ImGui::Text(tr("Right shoulder").c_str());
                        ImGui::SameLine(150);
                        ImGui::peripheralKeyCombo(keys, pad.button_shoulder_right, "button_right_shoulder");

                        ImGui::Text(tr("Button A").c_str());
                        ImGui::SameLine(150);
                        ImGui::peripheralKeyCombo(keys, pad.button_a, "button_a");

                        ImGui::Text(tr("Button B").c_str());
                        ImGui::SameLine(150);
                        ImGui::peripheralKeyCombo(keys, pad.button_b, "button_b");

                        ImGui::Text(tr("Button C").c_str());
                        ImGui::SameLine(150);
                        ImGui::peripheralKeyCombo(keys, pad.button_c, "button_c");

                        ImGui::Text(tr("Button X").c_str());
                        ImGui::SameLine(150);
                        ImGui::peripheralKeyCombo(keys, pad.button_x, "button_x");

                        ImGui::Text(tr("Button Y").c_str());
                        ImGui::SameLine(150);
                        ImGui::peripheralKeyCombo(keys, pad.button_y, "button_y");

                        ImGui::Text(tr("Button Z").c_str());
                        ImGui::SameLine(150);
                        ImGui::peripheralKeyCombo(keys, pad.button_z, "button_z");

                        ImGui::Text(tr("Button Start").c_str());
                        ImGui::SameLine(150);
                        ImGui::peripheralKeyCombo(keys, pad.button_start, "button_start");

                        state.config()->writeValue(core::AccessKeys::cfg_controls_saturn_player_1, pad.toConfig(PeripheralLayout::current));

                        ImGui::EndChild();
                    }
                    ImGui::SameLine();
                    {
                        //**** Saturn Player 2 ****//
                        static SaturnDigitalPad pad_p2;
                        pad_p2.fromConfig(state.config()->readPeripheralConfiguration(core::AccessKeys::cfg_controls_saturn_player_2));

                        ImGui::BeginChild("ChildSaturnPlayer2", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.5f, 280), true, window_flags);
                        ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth() - 150);

                        ImGui::CenteredText(tr("Player 2"));

                        ImGui::Text(tr("Left").c_str());
                        ImGui::SameLine(150);
                        ImGui::peripheralKeyCombo(keys, pad_p2.direction_left, "direction_left");

                        ImGui::Text(tr("Right").c_str());
                        ImGui::SameLine(150);
                        ImGui::peripheralKeyCombo(keys, pad_p2.direction_right, "direction_right");

                        ImGui::Text(tr("Up").c_str());
                        ImGui::SameLine(150);
                        ImGui::peripheralKeyCombo(keys, pad_p2.direction_up, "direction_up");

                        ImGui::Text(tr("Down").c_str());
                        ImGui::SameLine(150);
                        ImGui::peripheralKeyCombo(keys, pad_p2.direction_down, "direction_down");

                        ImGui::Text(tr("Left shoulder").c_str());
                        ImGui::SameLine(150);
                        ImGui::peripheralKeyCombo(keys, pad_p2.button_shoulder_left, "button_left_shoulder");

                        ImGui::Text(tr("Right shoulder").c_str());
                        ImGui::SameLine(150);
                        ImGui::peripheralKeyCombo(keys, pad_p2.button_shoulder_right, "button_right_shoulder");

                        ImGui::Text(tr("Button A").c_str());
                        ImGui::SameLine(150);
                        ImGui::peripheralKeyCombo(keys, pad_p2.button_a, "button_a");

                        ImGui::Text(tr("Button B").c_str());
                        ImGui::SameLine(150);
                        ImGui::peripheralKeyCombo(keys, pad_p2.button_b, "button_b");

                        ImGui::Text(tr("Button C").c_str());
                        ImGui::SameLine(150);
                        ImGui::peripheralKeyCombo(keys, pad_p2.button_c, "button_c");

                        ImGui::Text(tr("Button X").c_str());
                        ImGui::SameLine(150);
                        ImGui::peripheralKeyCombo(keys, pad_p2.button_x, "button_x");

                        ImGui::Text(tr("Button Y").c_str());
                        ImGui::SameLine(150);
                        ImGui::peripheralKeyCombo(keys, pad_p2.button_y, "button_y");

                        ImGui::Text(tr("Button Z").c_str());
                        ImGui::SameLine(150);
                        ImGui::peripheralKeyCombo(keys, pad_p2.button_z, "button_z");

                        ImGui::Text(tr("Button Start").c_str());
                        ImGui::SameLine(150);
                        ImGui::peripheralKeyCombo(keys, pad_p2.button_start, "button_start");

                        state.config()->writeValue(core::AccessKeys::cfg_controls_saturn_player_2, pad_p2.toConfig(PeripheralLayout::current));

                        ImGui::EndChild();
                    }
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("ST-V")) {
                    {
                        //**** ST-V Board ****//
                        static StvBoardControls board;
                        board.fromConfig(state.config()->readPeripheralConfiguration(core::AccessKeys::cfg_controls_stv_board));

                        ImGui::BeginChild("ChildStvBoard", ImVec2(0, 160), true, window_flags);
                        
                        ImGui::CenteredText(tr("Board"));

                        ImGui::Text(tr("Service switch").c_str());
                        ImGui::SameLine(150);
                        ImGui::peripheralKeyCombo(keys, board.service_switch, "service_switch");

                        ImGui::Text(tr("Test switch").c_str());
                        ImGui::SameLine(150);
                        ImGui::peripheralKeyCombo(keys, board.test_switch, "test_switch");

                        ImGui::Text(tr("Player 1 coin switch").c_str());
                        ImGui::SameLine(150);
                        ImGui::peripheralKeyCombo(keys, board.p1_coin_switch, "p1_coin_switch");

                        ImGui::Text(tr("Player 2 coin switch").c_str());
                        ImGui::SameLine(150);
                        ImGui::peripheralKeyCombo(keys, board.p2_coin_switch, "p2_coin_switch");

                        ImGui::Text(tr("Player 1 start").c_str());
                        ImGui::SameLine(150);
                        ImGui::peripheralKeyCombo(keys, board.p1_start, "p1_start");

                        ImGui::Text(tr("Player 2 start").c_str());
                        ImGui::SameLine(150);
                        ImGui::peripheralKeyCombo(keys, board.p2_start, "p2_start");

                        state.config()->writeValue(core::AccessKeys::cfg_controls_stv_board, board.toConfig(PeripheralLayout::current));

                        ImGui::EndChild();
                    }

                    {
                        //**** ST-V Player 1 ****//
                        static StvPlayerControls controls;
                        controls.fromConfig(state.config()->readPeripheralConfiguration(core::AccessKeys::cfg_controls_stv_player_1));

                        ImGui::BeginChild("ChildStvPlayer1", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.5f, 220), true, window_flags);

                        ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth() - 150);

                        ImGui::CenteredText(tr("Player 1"));

                        ImGui::Text(tr("Left").c_str());
                        ImGui::SameLine(150);
                        ImGui::peripheralKeyCombo(keys, controls.direction_left, "direction_left");

                        ImGui::Text(tr("Right").c_str());
                        ImGui::SameLine(150);
                        ImGui::peripheralKeyCombo(keys, controls.direction_right, "direction_right");

                        ImGui::Text(tr("Up").c_str());
                        ImGui::SameLine(150);
                        ImGui::peripheralKeyCombo(keys, controls.direction_up, "direction_up");

                        ImGui::Text(tr("Down").c_str());
                        ImGui::SameLine(150);
                        ImGui::peripheralKeyCombo(keys, controls.direction_down, "direction_down");


                        ImGui::Text(tr("Button 1").c_str());
                        ImGui::SameLine(150);
                        ImGui::peripheralKeyCombo(keys, controls.button_1, "button_1");

                        ImGui::Text(tr("Button 2").c_str());
                        ImGui::SameLine(150);
                        ImGui::peripheralKeyCombo(keys, controls.button_2, "button_2");

                        ImGui::Text(tr("Button 3").c_str());
                        ImGui::SameLine(150);
                        ImGui::peripheralKeyCombo(keys, controls.button_3, "button_3");

                        ImGui::Text(tr("Button 4").c_str());
                        ImGui::SameLine(150);
                        ImGui::peripheralKeyCombo(keys, controls.button_4, "button_4");

                        state.config()->writeValue(core::AccessKeys::cfg_controls_stv_player_1, controls.toConfig(PeripheralLayout::current));

                        ImGui::EndChild();
                    }
                    ImGui::SameLine();
                    {
                        //**** ST-V Player 2 ****//
                        static StvPlayerControls controls;
                        controls.fromConfig(state.config()->readPeripheralConfiguration(core::AccessKeys::cfg_controls_stv_player_2));

                        ImGui::BeginChild("ChildStvPlayer2", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.5f, 220), true, window_flags);

                        ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth() - 150);

                        ImGui::CenteredText(tr("Player 2"));

                        ImGui::Text(tr("Left").c_str());
                        ImGui::SameLine(150);
                        ImGui::peripheralKeyCombo(keys, controls.direction_left, "direction_left");

                        ImGui::Text(tr("Right").c_str());
                        ImGui::SameLine(150);
                        ImGui::peripheralKeyCombo(keys, controls.direction_right, "direction_right");

                        ImGui::Text(tr("Up").c_str());
                        ImGui::SameLine(150);
                        ImGui::peripheralKeyCombo(keys, controls.direction_up, "direction_up");

                        ImGui::Text(tr("Down").c_str());
                        ImGui::SameLine(150);
                        ImGui::peripheralKeyCombo(keys, controls.direction_down, "direction_down");


                        ImGui::Text(tr("Button 1").c_str());
                        ImGui::SameLine(150);
                        ImGui::peripheralKeyCombo(keys, controls.button_1, "button_1");

                        ImGui::Text(tr("Button 2").c_str());
                        ImGui::SameLine(150);
                        ImGui::peripheralKeyCombo(keys, controls.button_2, "button_2");

                        ImGui::Text(tr("Button 3").c_str());
                        ImGui::SameLine(150);
                        ImGui::peripheralKeyCombo(keys, controls.button_3, "button_3");

                        ImGui::Text(tr("Button 4").c_str());
                        ImGui::SameLine(150);
                        ImGui::peripheralKeyCombo(keys, controls.button_4, "button_4");

                        state.config()->writeValue(core::AccessKeys::cfg_controls_stv_player_2, controls.toConfig(PeripheralLayout::current));

                        ImGui::EndChild();
                    }

                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
                ImGui::PopStyleVar();
            }
            ImGui::PopItemWidth();
        }


        static u16 counter{};
        static std::string status_message{};
        if (ImGui::Button("Save")) {
            state.config()->writeFile();
            state.smpc()->initializePeripheralMappings();
            status_message = tr("Configuration saved.");
            counter = 5 * 60;

            if (reset_rendering) {
                state.renderingStatus(core::RenderingStatus::reset);
                reset_rendering = false;
            }
        }
        
        if (counter > 0) --counter;
        else status_message.clear();

        ImGui::Text(status_message.c_str());

        ImGui::End();
    }

    void showLogWindow(bool* opened) {
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
        window_flags |= ImGuiWindowFlags_NoResize;
        

        ImGui::SetNextWindowPos(ImVec2(0, 400), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(600, 150));


        ImGui::Begin("Log", opened, window_flags);

        static std::filesystem::file_time_type log_time;
        auto log_file = fs::current_path() / "logs" / "saturnin.log";

        static std::string value_displayed{};
        if(log_time != fs::last_write_time(log_file)){
            std::ifstream input_file(log_file.c_str(), std::ios::in);
            if (input_file.is_open()) {
                std::stringstream buffer;
                buffer << input_file.rdbuf();
                input_file.close();

                value_displayed = buffer.str();
            }
            log_time = fs::last_write_time(log_file);
        }
        ImGui::TextUnformatted(value_displayed.c_str());
        ImGui::End();
    }

    void buildGui(core::EmulatorContext& state, video::Opengl& opengl, const u32 width, const u32 height) {
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu(tr("File").c_str())) {
                ImGui::MenuItem(tr("Load ST-V rom").c_str(), NULL, &show_load_stv);
                ImGui::MenuItem(tr("Load binary file").c_str(), NULL, &show_load_binary);
                ImGui::EndMenu();
            }

            ImGui::MenuItem(tr("Options").c_str(), NULL, &show_options);
        }
        ImGui::EndMainMenuBar();

        showCoreWindow(state, opengl);
        
        //showRenderingWindow(opengl, width, height);
        showRenderingWindow(opengl, 320, 240);

        if (show_options)   showOptionsWindow(state, &show_options);
        if (show_load_stv)  showStvWindow(&show_load_stv);
        if (show_demo)      showImguiDemoWindow(&show_demo);
        if (show_log)       showLogWindow(&show_log);
    }

    void addTextureToDrawList(int32_t texture, const uint32_t width, const uint32_t height) {
        ImGui::GetWindowDrawList()->AddImage(
            (ImTextureID)(intptr_t)texture,
            ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y), 
            ImVec2(ImGui::GetCursorScreenPos().x + width, ImGui::GetCursorScreenPos().y + height),
            ImVec2(0, 1), ImVec2(1, 0));
    }
}
}
