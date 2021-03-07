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

#include <saturnin/src/pch.h>
#include <saturnin/src/video/gui.h>
//#include <imgui.h>
#include <istream>
#include <saturnin/src/config.h>
#include <saturnin/src/emulator_enums.h> // EmulationStatus
#include <saturnin/src/locale.h>         // tr
#include <saturnin/src/log.h>            // Log
#include <saturnin/src/sh2.h>            // Sh2
#include <saturnin/src/sh2_instructions.h>
#include <saturnin/src/smpc.h>                        // SaturnDigitalPad, PeripheralKey
#include <saturnin/src/utilities.h>                   // stringToVector
#include <saturnin/src/cdrom/scsi.h>                  // ScsiDriveInfo
#include <saturnin/src/video/opengl.h>                // Opengl
#include <saturnin/src/video/vdp2.h>                  // vram_timing_size
#include <saturnin/lib/imgui/imgui_custom_controls.h> // peripheralKeyCombo
#include <saturnin/lib/imgui/imgui_memory_editor.h>   // MemoryEditor

#if defined(__clang__)
#pragma clang diagnostic ignored "-Wformat-security" // warning: format string is not a string literal
#endif

namespace util  = saturnin::utilities;
namespace cdrom = saturnin::cdrom;
namespace video = saturnin::video;

namespace saturnin::gui {

// static bool show_load_stv     = false;
static bool show_load_binary  = false;
static bool show_debug_memory = false;
static bool show_debug_sh2    = false;
static bool show_debug_vdp2   = false;
static bool show_demo         = true;
static bool show_log          = true;

using core::Log;
using core::MemoryMapArea;
using core::PeripheralKey;
using core::PeripheralLayout;
using core::SaturnDigitalPad;
using core::StvBoardControls;
using core::StvPlayerControls;
using core::tr;
using sh2::Sh2Register;
using sh2::Sh2Type;

void showImguiDemoWindow(const bool show_window) {
    // 3. Show the ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
    if (show_window) {
        const auto window_pos = ImVec2(650, 20);
        ImGui::SetNextWindowPos(window_pos, ImGuiCond_FirstUseEver);
        ImGui::ShowDemoWindow();
    }
}

void showCoreWindow(core::EmulatorContext& state) {
    auto window_flags = ImGuiWindowFlags{ImGuiWindowFlags_MenuBar};
    window_flags |= ImGuiWindowFlags_NoCollapse;
    window_flags |= ImGuiWindowFlags_NoResize;

    const auto pos_x = float{ImGui::GetMainViewport()->Pos.x + ImGui::GetMainViewport()->Size.x};
    const auto pos_y = float{ImGui::GetMainViewport()->Pos.y};
    ImGui::SetNextWindowPos(ImVec2(pos_x, pos_y), ImGuiCond_Once);
    const auto size = ImVec2(275, 80);
    ImGui::SetNextWindowSize(size);

    ImGui::Begin(tr("Core").c_str(), nullptr, window_flags);

    showMainMenu(state);

    const auto button_width = ImVec2(80, 0);
    const auto label_run    = icon_play + tr("Run");
    if (ImGui::Button(label_run.c_str(), button_width)) { state.startEmulation(); }
    ImGui::SameLine();
    const auto label_pause = icon_pause + tr("Pause");
    if (ImGui::Button(label_pause.c_str(), button_width)) { state.pauseEmulation(); }
    ImGui::SameLine();
    const auto label_stop = icon_stop + tr("Stop");
    if (ImGui::Button(label_stop.c_str(), button_width)) {
        state.stopEmulation();
        show_debug_sh2 = false;
    }

    ImGui::End();
}

void showMainMenu(core::EmulatorContext& state) {
    if (ImGui::BeginMenuBar()) {
        // File
        if (ImGui::BeginMenu(tr("File").c_str())) {
            if (ImGui::BeginMenu(tr("Load ST-V rom").c_str())) {
                auto files = core::listStvConfigurationFiles();

                static auto listbox_item_current = int{1};

                ImGui::BeginChild("ST-V window", ImVec2(200, 100));
                ImGui::Combo("", &listbox_item_current, files);
                ImGui::EndChild();
                ImGui::EndMenu();
            };
            ImGui::MenuItem(tr("Load binary file").c_str(), nullptr, &show_load_binary);
            ImGui::EndMenu();
        }

        // Debug
        switch (state.emulationStatus()) {
            case core::EmulationStatus::running:
            case core::EmulationStatus::reset: {
                if (ImGui::BeginMenu(tr("Debug").c_str())) {
                    ImGui::MenuItem(tr("Memory editor").c_str(), nullptr, &show_debug_memory);
                    ImGui::MenuItem(tr("SH2").c_str(), nullptr, &show_debug_sh2);
                    ImGui::MenuItem(tr("VDP2").c_str(), nullptr, &show_debug_vdp2);
                    ImGui::EndMenu();
                }

                if (show_debug_memory) { showMemoryDebugWindow(state, &show_debug_memory); };
                if (show_debug_sh2) { showSh2DebugWindow(state, &show_debug_sh2); };
                if (show_debug_vdp2) { showVdp2DebugWindow(state, &show_debug_vdp2); };
            }
            default: break;
        }

        if (ImGui::BeginMenu(tr("Options").c_str())) {
            enum class Header : u8 { general = 0, rendering = 1, path = 2, cd_rom = 3, sound = 4, peripherals = 5, none = 255 };
            using HeaderMap                = std::map<const Header, const std::string>;
            const auto  headers            = HeaderMap{{Header::general, tr("General")},
                                           {Header::rendering, tr("Rendering")},
                                           {Header::path, tr("Paths")},
                                           {Header::cd_rom, tr("CD-Rom")},
                                           {Header::sound, ("Sound")},
                                           {Header::peripherals, tr("Peripherals")}};
            static auto last_opened_header = Header::none;
            auto        setHeaderState     = [](const Header header) {
                const auto state = (last_opened_header == header);
                ImGui::SetNextItemOpen(state);
            };

            static auto    reset_rendering      = bool{}; // used to check if rendering has to be reset after changing the option
            constexpr auto second_column_offset = u8{150};
            // General header
            setHeaderState(Header::general);
            if (ImGui::CollapsingHeader(headers.at(Header::general).c_str())) {
                last_opened_header = Header::general;
                // Hardware mode
                ImGui::TextUnformatted(tr("Hardware mode").c_str());
                ImGui::SameLine(second_column_offset);

                std::string hm   = state.config()->readValue(core::AccessKeys::cfg_global_hardware_mode);
                static auto mode = int{util::toUnderlying(state.config()->getHardwareMode(hm))};

                if (ImGui::RadioButton("Saturn", &mode, util::toUnderlying(core::HardwareMode::saturn))) {
                    const auto hm = state.config()->getHardwareModeKey(core::HardwareMode::saturn);
                    if (hm != std::nullopt) {
                        state.config()->writeValue(core::AccessKeys::cfg_global_hardware_mode, *hm);
                    } else {
                        Log::warning("config", tr("Unknown hardware mode ...").c_str());
                    }
                }
                ImGui::SameLine();
                if (ImGui::RadioButton("ST-V", &mode, util::toUnderlying(core::HardwareMode::stv))) {
                    const auto hm = state.config()->getHardwareModeKey(core::HardwareMode::stv);
                    if (hm != std::nullopt) {
                        state.config()->writeValue(core::AccessKeys::cfg_global_hardware_mode, *hm);
                    } else {
                        Log::warning("config", tr("Unknown hardware mode ...").c_str());
                    }
                }

                // Language
                ImGui::TextUnformatted(tr("Language").c_str());
                ImGui::SameLine(second_column_offset);

                static auto locales = core::Config::listAvailableLanguages();
                std::string l       = state.config()->readValue(core::AccessKeys::cfg_global_language);
                const auto  it      = std::find_if(locales.begin(), locales.end(), [&l](std::string& str) { return l == str; });
                static auto index   = static_cast<s32>(it - locales.begin());
                if (ImGui::Combo("##language", &index, locales)) {
                    state.config()->writeValue(core::AccessKeys::cfg_global_language, locales[index]);
                }

                // Area code
                ImGui::TextUnformatted(tr("Area code").c_str());
                ImGui::SameLine(second_column_offset);

                static auto codes      = state.config()->listAreaCodes();
                std::string c          = state.config()->readValue(core::AccessKeys::cfg_global_area_code);
                const auto  it_code    = std::find_if(codes.begin(), codes.end(), [&c](std::string& str) { return c == str; });
                static auto index_code = static_cast<s32>(it_code - codes.begin());
                if (ImGui::Combo("##area_code", &index_code, codes)) {
                    state.config()->writeValue(core::AccessKeys::cfg_global_area_code, codes[index_code]);
                }
            }

            // Rendering header
            setHeaderState(Header::rendering);
            if (ImGui::CollapsingHeader(headers.at(Header::rendering).c_str())) {
                last_opened_header = Header::rendering;
                // TV standard
                ImGui::TextUnformatted(tr("TV standard").c_str());
                ImGui::SameLine(second_column_offset);

                std::string ts       = state.config()->readValue(core::AccessKeys::cfg_rendering_tv_standard);
                static auto standard = int{util::toUnderlying(state.config()->getTvStandard(ts))};

                if (ImGui::RadioButton("PAL", &standard, util::toUnderlying(video::TvStandard::pal))) {
                    const auto key = state.config()->getTvStandardKey(video::TvStandard::pal);
                    if (key != std::nullopt) {
                        state.config()->writeValue(core::AccessKeys::cfg_rendering_tv_standard, *key);
                    } else {
                        Log::warning("config", tr("Unknown TV standard ...").c_str());
                    }
                }
                ImGui::SameLine();
                if (ImGui::RadioButton("NTSC", &standard, util::toUnderlying(video::TvStandard::ntsc))) {
                    const auto key = state.config()->getTvStandardKey(video::TvStandard::ntsc);
                    if (key != std::nullopt) {
                        state.config()->writeValue(core::AccessKeys::cfg_rendering_tv_standard, *key);
                    } else {
                        Log::warning("config", tr("Unknown TV standard ...").c_str());
                    }
                }

                // Legacy opengl
                ImGui::TextUnformatted(tr("Legacy OpenGL").c_str());
                ImGui::SameLine(second_column_offset);

                static bool is_legacy         = state.config()->readValue(core::AccessKeys::cfg_rendering_legacy_opengl);
                const auto  initial_rendering = bool{is_legacy};
                if (ImGui::Checkbox("", &is_legacy)) {
                    state.config()->writeValue(core::AccessKeys::cfg_rendering_legacy_opengl, is_legacy);
                    if (initial_rendering != is_legacy) { reset_rendering = true; }
                }
            }

            // Paths header
            setHeaderState(Header::path);
            if (ImGui::CollapsingHeader(headers.at(Header::path).c_str())) {
                last_opened_header = Header::path;
                // Saturn bios
                ImGui::TextUnformatted(tr("Saturn bios").c_str());
                ImGui::SameLine(second_column_offset);

                constexpr auto string_size = u8{255};
                auto           bios_saturn
                    = util::stringToVector(state.config()->readValue(core::AccessKeys::cfg_paths_bios_saturn), string_size);
                if (ImGui::InputText("##bios_saturn", bios_saturn.data(), bios_saturn.capacity())) {
                    state.config()->writeValue(core::AccessKeys::cfg_paths_bios_saturn, bios_saturn.data());
                }

                // ST-V bios
                ImGui::TextUnformatted(tr("ST-V bios").c_str());
                ImGui::SameLine(second_column_offset);
                auto bios_stv
                    = util::stringToVector(state.config()->readValue(core::AccessKeys::cfg_paths_bios_stv), string_size);
                if (ImGui::InputText("##bios_stv", bios_stv.data(), bios_stv.capacity())) {
                    state.config()->writeValue(core::AccessKeys::cfg_paths_bios_stv, bios_stv.data());
                }

                // ST-V roms
                ImGui::TextUnformatted(tr("ST-V roms").c_str());
                ImGui::SameLine(second_column_offset);
                auto roms_stv
                    = util::stringToVector(state.config()->readValue(core::AccessKeys::cfg_paths_roms_stv), string_size);
                if (ImGui::InputText("##roms_stv", roms_stv.data(), roms_stv.capacity())) {
                    state.config()->writeValue(core::AccessKeys::cfg_paths_roms_stv, roms_stv.data());
                }

                ImGui::Separator();
            }

            // CD-ROM header
            setHeaderState(Header::cd_rom);
            if (ImGui::CollapsingHeader(headers.at(Header::cd_rom).c_str())) {
                last_opened_header = Header::cd_rom;
                // Drive
                ImGui::TextUnformatted(tr("Drive").c_str());
                ImGui::SameLine(second_column_offset);

                std::string drive        = state.config()->readValue(core::AccessKeys::cfg_cdrom_drive);
                static auto current_item = int{};
                const auto  drive_parts  = util::explode(drive, ':');
                if (drive_parts.size() == 3) {
                    current_item = cdrom::Cdrom::getDriveIndice(std::stoi(drive_parts[0]),
                                                                std::stoi(drive_parts[1]),
                                                                std::stoi(drive_parts[2]));
                }

                if (ImGui::Combo("", &current_item, cdrom::Cdrom::scsi_drives_list)) {
                    auto value = std::to_string(cdrom::Cdrom::di_list[current_item].path);
                    value += ':';
                    value += std::to_string(cdrom::Cdrom::di_list[current_item].target);
                    value += ':';
                    value += std::to_string(cdrom::Cdrom::di_list[current_item].lun);
                    state.config()->writeValue(core::AccessKeys::cfg_cdrom_drive, value);
                }

                // Access method
                // For now ASPI isn't supported, SPTI is used in every case
                ImGui::TextUnformatted(tr("Access method").c_str());
                ImGui::SameLine(second_column_offset);

                std::string access_method = state.config()->readValue(core::AccessKeys::cfg_cdrom_access_method);
                static auto method        = int{util::toUnderlying(state.config()->getCdromAccess(access_method))};
                if (ImGui::RadioButton("SPTI", &method, util::toUnderlying(cdrom::CdromAccessMethod::spti))) {
                    const auto key = state.config()->getCdromAccessKey(cdrom::CdromAccessMethod::spti);
                    if (key != std::nullopt) {
                        state.config()->writeValue(core::AccessKeys::cfg_cdrom_access_method, *key);
                    } else {
                        Log::warning("config", tr("Unknown drive access method ...").c_str());
                    }
                }
                ImGui::SameLine();
                if (ImGui::RadioButton("ASPI", &method, util::toUnderlying(cdrom::CdromAccessMethod::aspi))) {
                    const auto key = state.config()->getCdromAccessKey(cdrom::CdromAccessMethod::aspi);
                    if (key != std::nullopt) {
                        state.config()->writeValue(core::AccessKeys::cfg_cdrom_access_method, *key);
                    } else {
                        Log::warning("config", tr("Unknown drive access method ...").c_str());
                    }
                }

                // CD-Rom system ID
            }

            // Sound header
            setHeaderState(Header::sound);
            if (ImGui::CollapsingHeader(headers.at(Header::sound).c_str())) {
                last_opened_header = Header::sound;
                // Sound disabled
                ImGui::TextUnformatted(tr("Sound disabled").c_str());
                ImGui::SameLine(second_column_offset);

                static bool disabled = state.config()->readValue(core::AccessKeys::cfg_sound_disabled);
                if (ImGui::Checkbox("", &disabled)) {
                    state.config()->writeValue(core::AccessKeys::cfg_sound_disabled, disabled);
                }
            }

            // Peripheral header
            setHeaderState(Header::peripherals);
            if (ImGui::CollapsingHeader(headers.at(Header::peripherals).c_str())) {
                last_opened_header = Header::peripherals;
                static const auto keys{core::Smpc::listAvailableKeys()};
                auto              tab_bar_flags = ImGuiTabBarFlags{ImGuiTabBarFlags_None};
                if (ImGui::BeginTabBar("PeripheralTabBar", tab_bar_flags)) {
                    auto           window_flags = ImGuiWindowFlags{ImGuiWindowFlags_None};
                    constexpr auto rounding     = float{5.0f};
                    constexpr auto child_width  = u16{260};
                    constexpr auto child_height = u16{280};
                    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, rounding);
                    if (ImGui::BeginTabItem(tr("Saturn").c_str())) {
                        {
                            //**** Saturn Player 1 ****//
                            static auto pad = SaturnDigitalPad{};
                            pad.fromConfig(
                                state.config()->readPeripheralConfiguration(core::AccessKeys::cfg_controls_saturn_player_1));

                            ImGui::BeginChild("ChildSaturnPlayer1", ImVec2(child_width, child_height), true, window_flags);

                            ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth() - second_column_offset);

                            ImGui::CenteredText(tr("Player 1"));

                            ImGui::TextUnformatted(tr("Connection").c_str());
                            ImGui::SameLine(second_column_offset);
                            static auto connections = state.config()->listPeripheralConnections();
                            std::string c = state.config()->readValue(core::AccessKeys::cfg_controls_saturn_player_1_connection);
                            const auto  it_connection    = std::find_if(connections.begin(),
                                                                    connections.end(),
                                                                    [&c](std::string& str) { return c == str; });
                            static auto index_connection = static_cast<s32>(it_connection - connections.begin());
                            if (ImGui::Combo("##peripheral_connection_1", &index_connection, connections)) {
                                state.config()->writeValue(core::AccessKeys::cfg_controls_saturn_player_1_connection,
                                                           connections[index_connection]);
                            }

                            ImGui::TextUnformatted(tr("Left").c_str());
                            ImGui::SameLine(second_column_offset);
                            ImGui::peripheralKeyCombo(keys, pad.direction_left, "direction_left");

                            ImGui::TextUnformatted(tr("Right").c_str());
                            ImGui::SameLine(second_column_offset);
                            ImGui::peripheralKeyCombo(keys, pad.direction_right, "direction_right");

                            ImGui::TextUnformatted(tr("Up").c_str());
                            ImGui::SameLine(second_column_offset);
                            ImGui::peripheralKeyCombo(keys, pad.direction_up, "direction_up");

                            ImGui::TextUnformatted(tr("Down").c_str());
                            ImGui::SameLine(second_column_offset);
                            ImGui::peripheralKeyCombo(keys, pad.direction_down, "direction_down");

                            ImGui::TextUnformatted(tr("Left shoulder").c_str());
                            ImGui::SameLine(second_column_offset);
                            ImGui::peripheralKeyCombo(keys, pad.button_shoulder_left, "button_left_shoulder");

                            ImGui::TextUnformatted(tr("Right shoulder").c_str());
                            ImGui::SameLine(second_column_offset);
                            ImGui::peripheralKeyCombo(keys, pad.button_shoulder_right, "button_right_shoulder");

                            ImGui::TextUnformatted(tr("Button A").c_str());
                            ImGui::SameLine(second_column_offset);
                            ImGui::peripheralKeyCombo(keys, pad.button_a, "button_a");

                            ImGui::TextUnformatted(tr("Button B").c_str());
                            ImGui::SameLine(second_column_offset);
                            ImGui::peripheralKeyCombo(keys, pad.button_b, "button_b");

                            ImGui::TextUnformatted(tr("Button C").c_str());
                            ImGui::SameLine(second_column_offset);
                            ImGui::peripheralKeyCombo(keys, pad.button_c, "button_c");

                            ImGui::TextUnformatted(tr("Button X").c_str());
                            ImGui::SameLine(second_column_offset);
                            ImGui::peripheralKeyCombo(keys, pad.button_x, "button_x");

                            ImGui::TextUnformatted(tr("Button Y").c_str());
                            ImGui::SameLine(second_column_offset);
                            ImGui::peripheralKeyCombo(keys, pad.button_y, "button_y");

                            ImGui::TextUnformatted(tr("Button Z").c_str());
                            ImGui::SameLine(second_column_offset);
                            ImGui::peripheralKeyCombo(keys, pad.button_z, "button_z");

                            ImGui::TextUnformatted(tr("Button Start").c_str());
                            ImGui::SameLine(second_column_offset);
                            ImGui::peripheralKeyCombo(keys, pad.button_start, "button_start");

                            state.config()->writeValue(core::AccessKeys::cfg_controls_saturn_player_1,
                                                       pad.toConfig(PeripheralLayout::current_layout));

                            ImGui::EndChild();
                        }
                        ImGui::SameLine();
                        {
                            //**** Saturn Player 2 ****//
                            static auto pad_p2 = SaturnDigitalPad{};
                            pad_p2.fromConfig(
                                state.config()->readPeripheralConfiguration(core::AccessKeys::cfg_controls_saturn_player_2));

                            ImGui::BeginChild("ChildSaturnPlayer2", ImVec2(child_width, child_height), true, window_flags);
                            ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth() - second_column_offset);

                            ImGui::CenteredText(tr("Player 2"));

                            ImGui::TextUnformatted(tr("Connection").c_str());
                            ImGui::SameLine(second_column_offset);
                            static auto connections = state.config()->listPeripheralConnections();
                            std::string c = state.config()->readValue(core::AccessKeys::cfg_controls_saturn_player_2_connection);
                            const auto  it_connection    = std::find_if(connections.begin(),
                                                                    connections.end(),
                                                                    [&c](std::string& str) { return c == str; });
                            static auto index_connection = static_cast<s32>(it_connection - connections.begin());
                            if (ImGui::Combo("##peripheral_connection_2", &index_connection, connections)) {
                                state.config()->writeValue(core::AccessKeys::cfg_controls_saturn_player_1_connection,
                                                           connections[index_connection]);
                            }

                            ImGui::TextUnformatted(tr("Left").c_str());
                            ImGui::SameLine(second_column_offset);
                            ImGui::peripheralKeyCombo(keys, pad_p2.direction_left, "direction_left");

                            ImGui::TextUnformatted(tr("Right").c_str());
                            ImGui::SameLine(second_column_offset);
                            ImGui::peripheralKeyCombo(keys, pad_p2.direction_right, "direction_right");

                            ImGui::TextUnformatted(tr("Up").c_str());
                            ImGui::SameLine(second_column_offset);
                            ImGui::peripheralKeyCombo(keys, pad_p2.direction_up, "direction_up");

                            ImGui::TextUnformatted(tr("Down").c_str());
                            ImGui::SameLine(second_column_offset);
                            ImGui::peripheralKeyCombo(keys, pad_p2.direction_down, "direction_down");

                            ImGui::TextUnformatted(tr("Left shoulder").c_str());
                            ImGui::SameLine(second_column_offset);
                            ImGui::peripheralKeyCombo(keys, pad_p2.button_shoulder_left, "button_left_shoulder");

                            ImGui::TextUnformatted(tr("Right shoulder").c_str());
                            ImGui::SameLine(second_column_offset);
                            ImGui::peripheralKeyCombo(keys, pad_p2.button_shoulder_right, "button_right_shoulder");

                            ImGui::TextUnformatted(tr("Button A").c_str());
                            ImGui::SameLine(second_column_offset);
                            ImGui::peripheralKeyCombo(keys, pad_p2.button_a, "button_a");

                            ImGui::TextUnformatted(tr("Button B").c_str());
                            ImGui::SameLine(second_column_offset);
                            ImGui::peripheralKeyCombo(keys, pad_p2.button_b, "button_b");

                            ImGui::TextUnformatted(tr("Button C").c_str());
                            ImGui::SameLine(second_column_offset);
                            ImGui::peripheralKeyCombo(keys, pad_p2.button_c, "button_c");

                            ImGui::TextUnformatted(tr("Button X").c_str());
                            ImGui::SameLine(second_column_offset);
                            ImGui::peripheralKeyCombo(keys, pad_p2.button_x, "button_x");

                            ImGui::TextUnformatted(tr("Button Y").c_str());
                            ImGui::SameLine(second_column_offset);
                            ImGui::peripheralKeyCombo(keys, pad_p2.button_y, "button_y");

                            ImGui::TextUnformatted(tr("Button Z").c_str());
                            ImGui::SameLine(second_column_offset);
                            ImGui::peripheralKeyCombo(keys, pad_p2.button_z, "button_z");

                            ImGui::TextUnformatted(tr("Button Start").c_str());
                            ImGui::SameLine(second_column_offset);
                            ImGui::peripheralKeyCombo(keys, pad_p2.button_start, "button_start");

                            state.config()->writeValue(core::AccessKeys::cfg_controls_saturn_player_2,
                                                       pad_p2.toConfig(PeripheralLayout::current_layout));

                            ImGui::EndChild();
                        }
                        ImGui::EndTabItem();
                    }
                    if (ImGui::BeginTabItem(tr("ST-V").c_str())) {
                        {
                            //**** ST-V Board ****//
                            static auto board = StvBoardControls{};
                            board.fromConfig(
                                state.config()->readPeripheralConfiguration(core::AccessKeys::cfg_controls_stv_board));

                            constexpr auto h_size = u16{0};
                            constexpr auto v_size = u16{160};
                            ImGui::BeginChild("ChildStvBoard", ImVec2(h_size, v_size), true, window_flags);

                            ImGui::CenteredText(tr("Board"));

                            ImGui::TextUnformatted(tr("Service switch").c_str());
                            ImGui::SameLine(second_column_offset);
                            ImGui::peripheralKeyCombo(keys, board.service_switch, "service_switch");

                            ImGui::TextUnformatted(tr("Test switch").c_str());
                            ImGui::SameLine(second_column_offset);
                            ImGui::peripheralKeyCombo(keys, board.test_switch, "test_switch");

                            ImGui::TextUnformatted(tr("Player 1 coin switch").c_str());
                            ImGui::SameLine(second_column_offset);
                            ImGui::peripheralKeyCombo(keys, board.p1_coin_switch, "p1_coin_switch");

                            ImGui::TextUnformatted(tr("Player 2 coin switch").c_str());
                            ImGui::SameLine(second_column_offset);
                            ImGui::peripheralKeyCombo(keys, board.p2_coin_switch, "p2_coin_switch");

                            ImGui::TextUnformatted(tr("Player 1 start").c_str());
                            ImGui::SameLine(second_column_offset);
                            ImGui::peripheralKeyCombo(keys, board.p1_start, "p1_start");

                            ImGui::TextUnformatted(tr("Player 2 start").c_str());
                            ImGui::SameLine(second_column_offset);
                            ImGui::peripheralKeyCombo(keys, board.p2_start, "p2_start");

                            state.config()->writeValue(core::AccessKeys::cfg_controls_stv_board,
                                                       board.toConfig(PeripheralLayout::current_layout));

                            ImGui::EndChild();
                        }

                        {
                            //**** ST-V Player 1 ****//
                            static auto controls = StvPlayerControls{};
                            controls.fromConfig(
                                state.config()->readPeripheralConfiguration(core::AccessKeys::cfg_controls_stv_player_1));

                            constexpr auto child_height = u16{220};
                            ImGui::BeginChild("ChildStvPlayer1", ImVec2(child_width, child_height), true, window_flags);

                            ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth() - second_column_offset);

                            ImGui::CenteredText(tr("Player 1"));

                            ImGui::TextUnformatted(tr("Left").c_str());
                            ImGui::SameLine(second_column_offset);
                            ImGui::peripheralKeyCombo(keys, controls.direction_left, "direction_left");

                            ImGui::TextUnformatted(tr("Right").c_str());
                            ImGui::SameLine(second_column_offset);
                            ImGui::peripheralKeyCombo(keys, controls.direction_right, "direction_right");

                            ImGui::TextUnformatted(tr("Up").c_str());
                            ImGui::SameLine(second_column_offset);
                            ImGui::peripheralKeyCombo(keys, controls.direction_up, "direction_up");

                            ImGui::TextUnformatted(tr("Down").c_str());
                            ImGui::SameLine(second_column_offset);
                            ImGui::peripheralKeyCombo(keys, controls.direction_down, "direction_down");

                            ImGui::TextUnformatted(tr("Button 1").c_str());
                            ImGui::SameLine(second_column_offset);
                            ImGui::peripheralKeyCombo(keys, controls.button_1, "button_1");

                            ImGui::TextUnformatted(tr("Button 2").c_str());
                            ImGui::SameLine(second_column_offset);
                            ImGui::peripheralKeyCombo(keys, controls.button_2, "button_2");

                            ImGui::TextUnformatted(tr("Button 3").c_str());
                            ImGui::SameLine(second_column_offset);
                            ImGui::peripheralKeyCombo(keys, controls.button_3, "button_3");

                            ImGui::TextUnformatted(tr("Button 4").c_str());
                            ImGui::SameLine(second_column_offset);
                            ImGui::peripheralKeyCombo(keys, controls.button_4, "button_4");

                            state.config()->writeValue(core::AccessKeys::cfg_controls_stv_player_1,
                                                       controls.toConfig(PeripheralLayout::current_layout));

                            ImGui::EndChild();
                        }
                        ImGui::SameLine();
                        {
                            //**** ST-V Player 2 ****//
                            static auto controls = StvPlayerControls{};
                            controls.fromConfig(
                                state.config()->readPeripheralConfiguration(core::AccessKeys::cfg_controls_stv_player_2));

                            constexpr auto child_height = u16{220};
                            ImGui::BeginChild("ChildStvPlayer2", ImVec2(child_width, child_height), true, window_flags);

                            ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth() - second_column_offset);

                            ImGui::CenteredText(tr("Player 2"));

                            ImGui::TextUnformatted(tr("Left").c_str());
                            ImGui::SameLine(second_column_offset);
                            ImGui::peripheralKeyCombo(keys, controls.direction_left, "direction_left");

                            ImGui::TextUnformatted(tr("Right").c_str());
                            ImGui::SameLine(second_column_offset);
                            ImGui::peripheralKeyCombo(keys, controls.direction_right, "direction_right");

                            ImGui::TextUnformatted(tr("Up").c_str());
                            ImGui::SameLine(second_column_offset);
                            ImGui::peripheralKeyCombo(keys, controls.direction_up, "direction_up");

                            ImGui::TextUnformatted(tr("Down").c_str());
                            ImGui::SameLine(second_column_offset);
                            ImGui::peripheralKeyCombo(keys, controls.direction_down, "direction_down");

                            ImGui::TextUnformatted(tr("Button 1").c_str());
                            ImGui::SameLine(second_column_offset);
                            ImGui::peripheralKeyCombo(keys, controls.button_1, "button_1");

                            ImGui::TextUnformatted(tr("Button 2").c_str());
                            ImGui::SameLine(second_column_offset);
                            ImGui::peripheralKeyCombo(keys, controls.button_2, "button_2");

                            ImGui::TextUnformatted(tr("Button 3").c_str());
                            ImGui::SameLine(second_column_offset);
                            ImGui::peripheralKeyCombo(keys, controls.button_3, "button_3");

                            ImGui::TextUnformatted(tr("Button 4").c_str());
                            ImGui::SameLine(second_column_offset);
                            ImGui::peripheralKeyCombo(keys, controls.button_4, "button_4");

                            state.config()->writeValue(core::AccessKeys::cfg_controls_stv_player_2,
                                                       controls.toConfig(PeripheralLayout::current_layout));

                            ImGui::EndChild();
                        }

                        ImGui::EndTabItem();
                    }
                    ImGui::EndTabBar();
                    ImGui::PopStyleVar();
                }
                // ImGui::PopItemWidth();
            }
            static auto counter        = u16{};
            static auto status_message = std::string{};
            if (ImGui::Button("Save")) {
                state.config()->writeFile();
                state.smpc()->initializePeripheralMappings();

                // Updating global state variables
                std::string hm = state.config()->readValue(core::AccessKeys::cfg_global_hardware_mode);
                state.hardwareMode(state.config()->getHardwareMode(hm));

                status_message                   = tr("Configuration saved.");
                constexpr auto frames_per_second = u8{60};
                constexpr auto number_of_seconds = u8{5};
                counter                          = number_of_seconds * frames_per_second;

                if (reset_rendering) {
                    state.renderingStatus(core::RenderingStatus::reset);
                    reset_rendering = false;
                }
            }

            if (counter > 0) {
                --counter;
            } else {
                status_message.clear();
            }

            ImGui::TextUnformatted(status_message.c_str());
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }
}

void showRenderingWindow(core::EmulatorContext& state) {
    // The rendering window is stretched to fill the area of the main window minus the core window.
    auto window = glfwGetCurrentContext();
    auto width  = s32{};
    auto height = s32{};
    glfwGetWindowSize(window, &width, &height);

    const auto pos_x = float{ImGui::GetMainViewport()->Pos.x};
    // const auto pos_y = float{ImGui::GetMainViewport()->Pos.y + core_window_height};
    const auto pos_y = float{ImGui::GetMainViewport()->Pos.y};
    ImGui::SetNextWindowPos(ImVec2(pos_x, pos_y), ImGuiCond_Always);

    // const auto window_size = ImVec2(static_cast<float>(width), static_cast<float>(height + core_window_height));
    const auto window_size = ImVec2(static_cast<float>(width), static_cast<float>(height));
    ImGui::SetNextWindowSize(window_size);

    ImGui::SetNextWindowViewport(ImGui::GetMainViewport()->ID);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.0f);
    auto flags = ImGuiWindowFlags{ImGuiWindowFlags_NoSavedSettings};
    flags |= ImGuiWindowFlags_NoDecoration;
    flags |= ImGuiWindowFlags_NoMove;

    ImGui::Begin("Video rendering", nullptr, flags);

    state.opengl()->displayFramebuffer(state);
    if (state.opengl()->renderingTexture() != 0) { gui::addTextureToDrawList(state.opengl()->renderingTexture(), width, height); }

    // showMainMenu(state);

    ImGui::End();
    ImGui::PopStyleVar();
    ImGui::PopStyleVar();
    ImGui::PopStyleVar();
}

void showStvWindow() {
    auto files = core::listStvConfigurationFiles();

    static auto listbox_item_current = int{1};

    ImGui::BeginChild("ST-V window");
    ImGui::Combo("", &listbox_item_current, files);
    ImGui::EndChild();
}

void showLogWindow(bool* opened) {
    auto window_flags = ImGuiWindowFlags{ImGuiWindowFlags_None};
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoSavedSettings;

    const auto window_size = ImVec2(700, 150);
    ImGui::SetNextWindowSize(window_size);

    auto window = glfwGetCurrentContext();
    auto width  = s32{};
    auto height = s32{};
    glfwGetWindowSize(window, &width, &height);

    const auto pos_x = float{ImGui::GetMainViewport()->Pos.x};
    const auto pos_y = float{ImGui::GetMainViewport()->Pos.y + static_cast<float>(height)};
    ImGui::SetNextWindowPos(ImVec2(pos_x, pos_y), ImGuiCond_Once);

    ImGui::Begin("Log", opened, window_flags);

    const auto stream = Log::getStream();
    ImGui::TextUnformatted(stream.c_str());

    static auto current_size = size_t{};
    if (stream.size() > current_size) {
        ImGui::SetScrollHereY(1.0f);
        current_size = stream.size();
    }

    ImGui::End();
}

void showSh2DebugWindow(core::EmulatorContext& state, bool* opened) {
    const auto window_size = ImVec2(670, 440);
    ImGui::SetNextWindowSize(window_size);

    auto window_flags = ImGuiWindowFlags{ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings
                                         | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar};
    ImGui::Begin("Sh2 debug", opened, window_flags);

    static auto sh2_type   = Sh2Type{Sh2Type::master};
    static auto current_pc = u32{state.slaveSh2()->getRegister(Sh2Register::pc)};
    if (ImGui::RadioButton(tr("Master").c_str(), sh2_type == Sh2Type::master)) { sh2_type = Sh2Type::master; }
    ImGui::SameLine();
    if (ImGui::RadioButton(tr("Slave").c_str(), sh2_type == Sh2Type::slave)) { sh2_type = Sh2Type::slave; };

    sh2::Sh2* current_sh2{nullptr};
    switch (sh2_type) {
        case Sh2Type::master: current_sh2 = state.masterSh2(); break;
        case Sh2Type::slave: current_sh2 = state.slaveSh2(); break;
        default: break;
    }

    current_pc = current_sh2->getRegister(Sh2Register::pc);

    ImGui::SameLine(ImGui::GetWindowWidth() / 2);

    {
        // Debug buttons
        const auto button_width    = ImVec2(90, 0);
        const auto label_step_into = icon_step_into + tr("Step into");
        if (ImGui::Button(label_step_into.c_str(), button_width)) { state.debugStatus(core::DebugStatus::step_into); }
        ImGui::SameLine();
        const auto label_step_over = icon_step_over + tr("Step over");
        if (ImGui::Button(label_step_over.c_str(), button_width)) { state.debugStatus(core::DebugStatus::step_over); }
        ImGui::SameLine();
        const auto label_step_out = icon_step_out + tr("Step out");
        if (ImGui::Button(label_step_out.c_str(), button_width)) { state.debugStatus(core::DebugStatus::step_out); }
    }

    {
        // General registers
        const auto child_size = ImVec2(300, 270);
        ImGui::BeginChild("ChildRegisters", child_size, true, window_flags);

        ImGui::TextDisabled(tr("General registers").c_str());
        ImGui::Separator();

        const auto mask = std::string{"R{:<2d} = {:#010x}"};
        auto       i    = u8{};

        ImGui::Columns(2);
        ImGui::TextUnformatted(fmt::format(mask, i++, current_sh2->getRegister(Sh2Register::r0)).c_str());
        ImGui::TextUnformatted(fmt::format(mask, i++, current_sh2->getRegister(Sh2Register::r1)).c_str());
        ImGui::TextUnformatted(fmt::format(mask, i++, current_sh2->getRegister(Sh2Register::r2)).c_str());
        ImGui::TextUnformatted(fmt::format(mask, i++, current_sh2->getRegister(Sh2Register::r3)).c_str());
        ImGui::TextUnformatted(fmt::format(mask, i++, current_sh2->getRegister(Sh2Register::r4)).c_str());
        ImGui::TextUnformatted(fmt::format(mask, i++, current_sh2->getRegister(Sh2Register::r5)).c_str());
        ImGui::TextUnformatted(fmt::format(mask, i++, current_sh2->getRegister(Sh2Register::r6)).c_str());
        ImGui::TextUnformatted(fmt::format(mask, i++, current_sh2->getRegister(Sh2Register::r7)).c_str());

        ImGui::NextColumn();

        ImGui::TextUnformatted(fmt::format(mask, i++, current_sh2->getRegister(Sh2Register::r8)).c_str());
        ImGui::TextUnformatted(fmt::format(mask, i++, current_sh2->getRegister(Sh2Register::r9)).c_str());
        ImGui::TextUnformatted(fmt::format(mask, i++, current_sh2->getRegister(Sh2Register::r10)).c_str());
        ImGui::TextUnformatted(fmt::format(mask, i++, current_sh2->getRegister(Sh2Register::r11)).c_str());
        ImGui::TextUnformatted(fmt::format(mask, i++, current_sh2->getRegister(Sh2Register::r12)).c_str());
        ImGui::TextUnformatted(fmt::format(mask, i++, current_sh2->getRegister(Sh2Register::r13)).c_str());
        ImGui::TextUnformatted(fmt::format(mask, i++, current_sh2->getRegister(Sh2Register::r14)).c_str());
        ImGui::TextUnformatted(fmt::format(mask, i++, current_sh2->getRegister(Sh2Register::r15)).c_str());

        ImGui::Columns(1);
        ImGui::Separator();

        ImGui::Columns(2);
        ImGui::TextDisabled(tr("System registers").c_str());
        ImGui::NextColumn();
        ImGui::TextDisabled(tr("Control registers").c_str());

        ImGui::Columns(1);
        ImGui::Separator();

        ImGui::Columns(2);
        const auto system_mask = std::string{"{:<4} = {:#010x}"};
        ImGui::TextUnformatted(fmt::format(system_mask, "MACH", current_sh2->getRegister(Sh2Register::mach)).c_str());
        ImGui::TextUnformatted(fmt::format(system_mask, "MACL", current_sh2->getRegister(Sh2Register::macl)).c_str());
        ImGui::TextUnformatted(fmt::format(system_mask, "PR", current_sh2->getRegister(Sh2Register::pr)).c_str());
        ImGui::TextUnformatted(fmt::format(system_mask, "PC", current_sh2->getRegister(Sh2Register::pc)).c_str());

        ImGui::NextColumn();

        // ImGui::Separator();
        const auto control_mask = std::string{"{:<3} = {:#010x}"};
        ImGui::TextUnformatted(fmt::format(control_mask, "VBR", current_sh2->getRegister(Sh2Register::vbr)).c_str());
        ImGui::TextUnformatted(fmt::format(control_mask, "GBR", current_sh2->getRegister(Sh2Register::gbr)).c_str());
        ImGui::TextUnformatted(fmt::format(control_mask, "SR", current_sh2->getRegister(Sh2Register::sr)).c_str());

        ImGui::EndChild();
    }
    ImGui::SameLine(ImGui::GetWindowWidth() / 2);
    {
        // Disassembly
        const auto child_size = ImVec2(330, 270);
        ImGui::BeginChild("ChildDisassembly", child_size, true, window_flags);

        ImGui::TextDisabled(tr("Disassembly").c_str());
        ImGui::Separator();

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, 300.0f);
        ImGui::SetColumnWidth(1, 30.0f);

        const auto lower_bound = u32{current_pc - 6};
        const auto upper_bound = u32{current_pc + 20};
        for (u32 i = lower_bound; i < upper_bound; i += 2) {
            const auto opcode = state.memory()->read<u16>(i);
            if (i == current_pc) {
                ImGui::TextColored(ImVec4(0.5f, 0.5f, 1.0f, 1.0f), sh2::disasm(i, opcode).c_str());
            } else {
                ImGui::TextUnformatted(sh2::disasm(i, opcode).c_str());
            }
        }

        ImGui::NextColumn();

        ImGui::NewLine();
        ImGui::NewLine();
        ImGui::NewLine();
        ImGui::NewLine();
        ImGui::NewLine();
        ImGui::PushButtonRepeat(true);
        if (ImGui::ArrowButton("##up", ImGuiDir_Up)) {
            if (current_pc > 0) { current_pc -= 2; }
        }
        if (ImGui::ArrowButton("##down", ImGuiDir_Down)) { current_pc += 2; }
        ImGui::PopButtonRepeat();

        ImGui::EndChild();
    }
    ImGui::NewLine();

    ImGui::SameLine(ImGui::GetWindowWidth() / 2);

    {
        // Callstack

        const auto child_size = ImVec2(330, 100);
        ImGui::BeginChild("ChildCallstack", child_size, true, window_flags);

        ImGui::TextDisabled(tr("Callstack").c_str());
        ImGui::Separator();

        const auto callstack_mask = std::string{"{:#010x}"};
        std::for_each(current_sh2->callstack().rbegin(), current_sh2->callstack().rend(), [&](const auto& item) {
            ImGui::TextUnformatted(fmt::format(callstack_mask, item.call_address).c_str());
        });

        ImGui::EndChild();
    }

    ImGui::End();
}

void showMemoryDebugWindow(core::EmulatorContext& state, bool* opened) {
    const auto window_size = ImVec2(610, 320);
    ImGui::SetNextWindowSize(window_size);

    auto window_flags
        = ImGuiWindowFlags{ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse};
    ImGui::Begin(tr("Memory debug").c_str(), opened, window_flags);

    static auto current_area = MemoryMapArea::rom;

    if (ImGui::BeginCombo(tr("Memory area").c_str(), state.memory()->memory_map_[current_area].c_str())) {
        for (const auto& [k, v] : state.memory()->memory_map_) {
            const auto is_selected = bool{current_area == k};
            if (ImGui::Selectable(v.c_str(), is_selected)) { current_area = k; }
            if (is_selected) { ImGui::SetItemDefaultFocus(); }
        }

        ImGui::EndCombo();
    }

    switch (current_area) {
        case MemoryMapArea::vdp2_registers: {
            break;
        }
        case MemoryMapArea::cd_block:
            for (auto const& r : state.memory()->cdrom()->getRegisters()) {
                ImGui::TextUnformatted(r.c_str());
            }
            break;
        default:
            auto        area_data = state.memory()->getMemoryMapAreaData(current_area);
            static auto editor    = MemoryEditor{}; // store your state somewhere
            editor.DrawContents(std::get<0>(area_data), std::get<1>(area_data), std::get<2>(area_data));
    }

    ImGui::End();
}

void showVdp2DebugWindow(core::EmulatorContext& state, bool* opened) {
    const auto window_size = ImVec2(610, 320);
    ImGui::SetNextWindowSize(window_size);

    auto window_flags
        = ImGuiWindowFlags{ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse};
    ImGui::Begin(tr("VDP2 debug").c_str(), opened, window_flags);

    auto tab_bar_flags = ImGuiTabBarFlags{ImGuiTabBarFlags_None};
    if (ImGui::BeginTabBar("Vdp2DebugTabBar", tab_bar_flags)) {
        if (ImGui::BeginTabItem(tr("Global").c_str())) {
            constexpr auto column_offset{150};
            for (const auto& [label, value] : state.vdp2()->getDebugGlobalMainData()) {
                ImGui::TextUnformatted(label.c_str());
                ImGui::SameLine(column_offset);
                ImGui::Text(": ");
                ImGui::SameLine();
                ImGui::TextUnformatted(value.c_str());
            }

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem(tr("VRAM access").c_str())) {
            static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;
            if (ImGui::BeginTable("vram_access", video::vram_timing_size + 1, flags)) {
                ImGui::TableSetupColumn("Bank");
                ImGui::TableSetupColumn("T0");
                ImGui::TableSetupColumn("T1");
                ImGui::TableSetupColumn("T2");
                ImGui::TableSetupColumn("T3");
                ImGui::TableSetupColumn("T4");
                ImGui::TableSetupColumn("T5");
                ImGui::TableSetupColumn("T6");
                ImGui::TableSetupColumn("T7");
                ImGui::TableHeadersRow();

                const auto banks = state.vdp2()->getDebugVramBanks();
                // const auto banks_used = state.vdp2()->getDebugVramBanksUsed();
                const auto banks_name = state.vdp2()->getDebugVramBanksName();

                for (u8 row = 0; row < banks.size(); ++row) {
                    ImGui::TableNextRow();
                    for (u8 column = 0; column < video::vram_timing_size + 1; ++column) {
                        ImGui::TableSetColumnIndex(column);
                        if (column == 0) {
                            ImU32 row_bg_color = ImGui::GetColorU32(ImVec4(0.2f, 0.2f, 0.2f, 1.f));
                            ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, row_bg_color);
                            ImGui::TextUnformatted(banks_name[row].c_str());
                            continue;
                        };
                        const auto command_desc = state.vdp2()->getDebugVramCommandDescription(banks[row][column - 1]);
                        ImGui::TextUnformatted(command_desc.first.c_str());
                        ImGui::SameLine();
                        ImGui::HelpMarker(command_desc.second.c_str());
                    }
                }
                ImGui::EndTable();
            }

            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem(tr("Registers").c_str())) {
            static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit;
            constexpr auto         columns_number = u8{3};
            if (ImGui::BeginTable("vdp2_registers_table", columns_number, flags)) {
                ImGui::TableSetupColumn(tr("Address").c_str());
                ImGui::TableSetupColumn(tr("Description").c_str());
                ImGui::TableSetupColumn(tr("Value").c_str());
                ImGui::TableHeadersRow();

                const auto& registers = state.memory()->vdp2()->getRegisters();
                for (const auto& [address, desc] : registers) {
                    ImGui::TableNextRow();
                    auto column_index = u8{0};
                    ImGui::TableSetColumnIndex(column_index++);
                    ImGui::TextUnformatted(fmt::format("{:#010x}", address).c_str());

                    ImGui::TableSetColumnIndex(column_index++);
                    ImGui::TextUnformatted(desc.c_str());

                    ImGui::TableSetColumnIndex(column_index);
                    ImGui::TextUnformatted(fmt::format("{:#06x}", state.memory()->vdp2()->readRegisters<u16>(address)).c_str());
                }
                ImGui::EndTable();
            }

            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem(tr("Scroll Screens").c_str())) {
            using video::ScrollScreen;
            using ScrollScreenName    = std::unordered_map<ScrollScreen, std::string>;
            const auto scroll_screens = ScrollScreenName{{ScrollScreen::nbg0, "NBG0"},
                                                         {ScrollScreen::nbg1, "NBG1"},
                                                         {ScrollScreen::nbg2, "NBG2"},
                                                         {ScrollScreen::nbg3, "NBG3"},
                                                         {ScrollScreen::rbg0, "RBG0"},
                                                         {ScrollScreen::rbg1, "RBG1"}};

            static auto current_screen = ScrollScreen::nbg0;
            const auto  flags          = ImGuiComboFlags{ImGuiComboFlags_None};
            ImGui::PushItemWidth(100);
            if (ImGui::BeginCombo("##scroll_screen", scroll_screens.at(current_screen).c_str(), flags)) {
                for (const auto& [key, name] : scroll_screens) {
                    const auto is_selected = bool{current_screen == key};
                    if (ImGui::Selectable(name.c_str(), is_selected)) { current_screen = key; }
                    if (is_selected) { ImGui::SetItemDefaultFocus(); }
                }

                ImGui::EndCombo();
            }
            ImGui::PopItemWidth();

            constexpr auto column_offset{150};
            // using video::ScrollScreenStatus;
            // using ScrollScreenValue = std::unordered_map<ScrollScreen, std::unique_ptr<ScrollScreenStatus>>;
            // const auto scroll_screen_values
            //    = ScrollScreenValue{{ScrollScreen::nbg0, std::make_unique<ScrollScreenStatus>(nbg_[0])},
            //                        {ScrollScreen::nbg1, "NBG1"},
            //                        {ScrollScreen::nbg2, "NBG2"},
            //                        {ScrollScreen::nbg3, "NBG3"},
            //                        {ScrollScreen::rbg0, "RBG0"},
            //                        {ScrollScreen::rbg1, "RBG1"}};
            for (const auto& [label, value] : state.vdp2()->getDebugScrollScreenData(current_screen)) {
                ImGui::TextUnformatted(label.c_str());
                ImGui::SameLine(column_offset);
                ImGui::Text(": ");
                ImGui::SameLine();
                ImGui::TextUnformatted(value.c_str());
            }

            // ImGui::TextUnformatted(
            //    fmt::format(tr("Size : {:#08x}"), state.vdp2()->getDebugScrollScreenData(current_screen)).c_str());

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();
}

void buildGui(core::EmulatorContext& state) {
    showCoreWindow(state);

    showRenderingWindow(state);

    if (show_demo) { showImguiDemoWindow(show_demo); }
    if (show_log) { showLogWindow(&show_log); }
}

void addTextureToDrawList(int32_t texture, const uint32_t width, const uint32_t height) {
    ImGui::GetWindowDrawList()->AddImage((ImTextureID)(intptr_t)texture,
                                         ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y),
                                         ImVec2(ImGui::GetCursorScreenPos().x + width, ImGui::GetCursorScreenPos().y + height),
                                         ImVec2(0, 1),
                                         ImVec2(1, 0));
}

auto getMouseCoordinates(core::EmulatorContext& state) -> Coord {
    auto x_pos_cursor = double{};
    auto y_pos_cursor = double{};
    glfwGetCursorPos(state.openglWindow(), &x_pos_cursor, &y_pos_cursor);

    auto x_pos_window = s32{};
    auto y_pos_window = s32{};
    glfwGetWindowPos(state.openglWindow(), &x_pos_window, &y_pos_window);

    const auto x_pos = x_pos_cursor + x_pos_window;
    const auto y_pos = y_pos_cursor + y_pos_window;

    return Coord{static_cast<float>(x_pos), static_cast<float>(y_pos)};
}
} // namespace saturnin::gui
