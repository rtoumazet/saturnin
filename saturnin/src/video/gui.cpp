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
#include <fstream>    // ifstream
#include <optional>
#include <thread> // thread
#include <tuple>  //tuple
#include "gui.h"
#include "../locale.h"                             // tr
#include "../../lib/imgui/imgui_custom_controls.h" // peripheralKeyCombo

#pragma warning(push)
#pragma warning(disable : 4996)
#include "../../lib/imgui/imgui_memory_editor.h" // MemoryEditor
#pragma warning(pop)

#include "../emulator_enums.h" // EmulationStatus
#include "../config.h"
#include "../sh2.h" // Sh2
#include "../sh2_instructions.h"
#include "../smpc.h"        // SaturnDigitalPad, PeripheralKey
#include "../utilities.h"   // stringToVector
#include "../cdrom/cdrom.h" // Cdrom

// namespace core  = saturnin::core;
namespace util  = saturnin::utilities;
namespace cdrom = saturnin::cdrom;
namespace fs    = std::filesystem;

namespace saturnin::gui {

static bool show_load_stv     = false;
static bool show_load_binary  = false;
static bool show_debug_sh2    = false;
static bool show_debug_memory = false;
static bool show_options      = false;
static bool show_demo         = false;
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

void showCoreWindow(core::EmulatorContext& state, video::Opengl& opengl) {
    auto window_flags = ImGuiWindowFlags{ImGuiWindowFlags_NoDecoration};
    window_flags |= ImGuiWindowFlags_NoMove;

    const auto window_pos = ImVec2(0, 0);
    ImGui::SetNextWindowPos(window_pos, ImGuiCond_Once);

    auto window = glfwGetCurrentContext();
    auto width  = s32{};
    auto height = s32{};
    glfwGetWindowSize(window, &width, &height);

    const auto window_size = ImVec2(static_cast<float>(width), 42);
    ImGui::SetNextWindowSize(window_size);

    auto show_window = bool{};

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(2, 2));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2, 3));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);

    ImGui::PushStyleColor(ImGuiCol_Button, ImColor(180, 180, 180, 102).Value);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor(110, 110, 110, 255).Value);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor(51, 51, 51, 255).Value);

    ImGui::Begin("Core", &show_window, window_flags);

    const auto button_size = ImVec2(30, 30);

    {
        // File icon
        ImGui::ImageButton(opengl.getIconTexture(video::IconId::file), button_size);
        if (ImGui::IsItemClicked()) {
            ImGui::SetNextWindowPos(ImVec2(0, 40));
            ImGui::OpenPopup("file_popup");
        }

        auto popup_flags = ImGuiWindowFlags{ImGuiWindowFlags_NoMove};
        if (ImGui::BeginPopup("file_popup", popup_flags)) {
            ImGui::MenuItem(tr("Load ST-V rom"), nullptr, &show_load_stv);
            ImGui::MenuItem(tr("Load binary file"), nullptr, &show_load_binary);

            ImGui::EndPopup();
        }
        if (show_load_stv) { showStvWindow(&show_debug_sh2); };
    }

    ImGui::SameLine();

    {
        // Debug icon
        switch (state.emulationStatus()) {
            case core::EmulationStatus::running:
            case core::EmulationStatus::reset: {
                ImGui::ImageButton(opengl.getIconTexture(video::IconId::debug), button_size);
                if (ImGui::IsItemClicked()) {
                    ImGui::SetNextWindowPos(ImVec2(40, 40));
                    ImGui::OpenPopup("debug_popup");
                }

                if (ImGui::BeginPopup("debug_popup", ImGuiWindowFlags_NoMove)) {
                    ImGui::MenuItem(tr("SH2"), nullptr, &show_debug_sh2);
                    ImGui::MenuItem(tr("Memory editor"), nullptr, &show_debug_memory);

                    ImGui::EndPopup();
                }

                if (show_debug_sh2) { showSh2DebugWindow(state, opengl, &show_debug_sh2); };
                if (show_debug_memory) { showMemoryDebugWindow(state, &show_debug_memory); };
            }
        }
    }

    ImGui::SameLine();

    {
        // Config icon
        ImGui::ImageButton(opengl.getIconTexture(video::IconId::config), button_size);
        if (ImGui::IsItemClicked()) { show_options = !show_options; }
        if (show_options) { showOptionsWindow(state, &show_options); }
    }

    ImGui::SameLine((static_cast<float>(width - 120)) / 2);

    {
        // Play icon
        ImGui::ImageButton(opengl.getIconTexture(video::IconId::play), button_size);
        if (ImGui::IsItemClicked()) { state.startEmulation(); }
    }

    ImGui::SameLine();

    {
        // Pause icon
        ImGui::ImageButton(opengl.getIconTexture(video::IconId::pause), button_size);
        if (ImGui::IsItemClicked()) { state.pauseEmulation(); }
    }

    ImGui::SameLine();

    {
        // Stop icon
        ImGui::ImageButton(opengl.getIconTexture(video::IconId::stop), button_size);
        if (ImGui::IsItemClicked()) {
            state.stopEmulation();
            show_debug_sh2 = false;
        }
    }

    ImGui::End();

    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar(5);
} // namespace saturnin::gui

void showRenderingWindow(video::Opengl& opengl, const u32 width, const u32 height) {
    constexpr auto offset = float{40};
    ImGui::SetNextWindowPos(ImVec2(0, 0 + offset), ImGuiCond_Once);

    const auto window_size = ImVec2(static_cast<float>(width), static_cast<float>(height + offset));
    ImGui::SetNextWindowSize(window_size);

    // ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    // ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.0f);
    auto flags = ImGuiWindowFlags{ImGuiWindowFlags_NoResize};
    flags |= ImGuiWindowFlags_NoScrollbar;
    flags |= ImGuiWindowFlags_NoCollapse;
    flags |= ImGuiWindowFlags_NoSavedSettings;
    flags |= ImGuiWindowFlags_NoFocusOnAppearing;
    flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::Begin("Video rendering", nullptr, flags);

    if (opengl.isWindowResized(width, height)) {
        // opengl.initializeTexture(width, height);
        opengl.updateTextureSize(width, height);
    }

    opengl.preRender();

    opengl.render();
    if (opengl.texture() != 0) { gui::addTextureToDrawList(opengl.texture(), width, height); }

    opengl.postRender();

    ImGui::End();
    ImGui::PopStyleVar();
    // ImGui::PopStyleVar();
    // ImGui::PopStyleVar();
}

void showStvWindow(bool* opened) {
    auto files = core::listStvConfigurationFiles();

    static auto listbox_item_current = int{1};

    ImGui::Begin("ST-V window", opened);
    ImGui::Combo("", &listbox_item_current, files);
    ImGui::End();
}

void showOptionsWindow(core::EmulatorContext& state, bool* opened) {
    static auto reset_rendering = bool{}; // used to check if rendering has to be reset after changing the option
    // ImGui::SetNextWindowSize(ImVec2(600, 300));
    constexpr auto item_width = s8{-10};
    ImGui::Begin("Options", opened);
    ImGui::PushItemWidth(item_width);
    ImGui::Spacing();

    constexpr auto second_column_offset = u8{150};
    // General header
    if (ImGui::CollapsingHeader(tr("General"))) {
        // Hardware mode
        ImGui::TextUnformatted(tr("Hardware mode"));
        ImGui::SameLine(second_column_offset);

        std::string hm   = state.config()->readValue(core::AccessKeys::cfg_global_hardware_mode);
        static auto mode = int{util::toUnderlying(state.config()->getHardwareMode(hm))};

        if (ImGui::RadioButton("Saturn", &mode, util::toUnderlying(core::HardwareMode::saturn))) {
            const auto hm = state.config()->getHardwareModeKey(core::HardwareMode::saturn);
            if (hm != std::nullopt) {
                state.config()->writeValue(core::AccessKeys::cfg_global_hardware_mode, *hm);
            } else {
                Log::warning("config", tr("Unknown hardware mode ..."));
            }
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("ST-V", &mode, util::toUnderlying(core::HardwareMode::stv))) {
            const auto hm = state.config()->getHardwareModeKey(core::HardwareMode::stv);
            if (hm != std::nullopt) {
                state.config()->writeValue(core::AccessKeys::cfg_global_hardware_mode, *hm);
            } else {
                Log::warning("config", tr("Unknown hardware mode ..."));
            }
        }

        // Language
        ImGui::TextUnformatted(tr("Language"));
        ImGui::SameLine(second_column_offset);

        static auto locales = core::Config::listAvailableLanguages();
        std::string l       = state.config()->readValue(core::AccessKeys::cfg_global_language);
        const auto  it      = std::find_if(locales.begin(), locales.end(), [&l](std::string& str) { return l == str; });
        static auto index   = static_cast<s32>(it - locales.begin());
        if (ImGui::Combo("##language", &index, locales)) {
            state.config()->writeValue(core::AccessKeys::cfg_global_language, locales[index]);
        }

        // Area code
        ImGui::TextUnformatted(tr("Area code"));
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
    if (ImGui::CollapsingHeader(tr("Rendering"))) {
        // TV standard
        ImGui::TextUnformatted(tr("TV standard"));
        ImGui::SameLine(second_column_offset);

        std::string ts       = state.config()->readValue(core::AccessKeys::cfg_rendering_tv_standard);
        static auto standard = int{util::toUnderlying(state.config()->getTvStandard(ts))};

        if (ImGui::RadioButton("PAL", &standard, util::toUnderlying(video::TvStandard::pal))) {
            const auto key = state.config()->getTvStandardKey(video::TvStandard::pal);
            if (key != std::nullopt) {
                state.config()->writeValue(core::AccessKeys::cfg_rendering_tv_standard, *key);
            } else {
                Log::warning("config", tr("Unknown TV standard ..."));
            }
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("NTSC", &standard, util::toUnderlying(video::TvStandard::ntsc))) {
            const auto key = state.config()->getTvStandardKey(video::TvStandard::ntsc);
            if (key != std::nullopt) {
                state.config()->writeValue(core::AccessKeys::cfg_rendering_tv_standard, *key);
            } else {
                Log::warning("config", tr("Unknown TV standard ..."));
            }
        }

        // Legacy opengl
        ImGui::TextUnformatted(tr("Legacy OpenGL"));
        ImGui::SameLine(second_column_offset);

        static bool is_legacy         = state.config()->readValue(core::AccessKeys::cfg_rendering_legacy_opengl);
        const auto  initial_rendering = bool{is_legacy};
        if (ImGui::Checkbox("", &is_legacy)) {
            state.config()->writeValue(core::AccessKeys::cfg_rendering_legacy_opengl, is_legacy);
            if (initial_rendering != is_legacy) { reset_rendering = true; }
        }
    }

    // Paths header
    if (ImGui::CollapsingHeader(tr("Paths"))) {
        // Saturn bios
        ImGui::TextUnformatted(tr("Saturn bios"));
        ImGui::SameLine(second_column_offset);

        constexpr auto string_size = u8{255};
        auto bios_saturn = util::stringToVector(state.config()->readValue(core::AccessKeys::cfg_paths_bios_saturn), string_size);
        if (ImGui::InputText("##bios_saturn", bios_saturn.data(), bios_saturn.capacity())) {
            state.config()->writeValue(core::AccessKeys::cfg_paths_bios_saturn, bios_saturn.data());
        }

        // ST-V bios
        ImGui::TextUnformatted(tr("ST-V bios"));
        ImGui::SameLine(second_column_offset);
        auto bios_stv = util::stringToVector(state.config()->readValue(core::AccessKeys::cfg_paths_bios_stv), string_size);
        if (ImGui::InputText("##bios_stv", bios_stv.data(), bios_stv.capacity())) {
            state.config()->writeValue(core::AccessKeys::cfg_paths_bios_stv, bios_stv.data());
        }

        // ST-V roms
        ImGui::TextUnformatted(tr("ST-V roms"));
        ImGui::SameLine(second_column_offset);
        auto roms_stv = util::stringToVector(state.config()->readValue(core::AccessKeys::cfg_paths_roms_stv), string_size);
        if (ImGui::InputText("##roms_stv", roms_stv.data(), roms_stv.capacity())) {
            state.config()->writeValue(core::AccessKeys::cfg_paths_roms_stv, roms_stv.data());
        }

        ImGui::Separator();
    }

    // CD-ROM header
    if (ImGui::CollapsingHeader(tr("CD-Rom"))) {
        // Drive
        ImGui::TextUnformatted(tr("Drive"));
        ImGui::SameLine(second_column_offset);

        std::string drive        = state.config()->readValue(core::AccessKeys::cfg_cdrom_drive);
        static auto current_item = int{};
        const auto  drive_parts  = util::explode(drive, ':');
        if (drive_parts.size() == 3) {
            current_item
                = cdrom::Cdrom::getDriveIndice(std::stoi(drive_parts[0]), std::stoi(drive_parts[1]), std::stoi(drive_parts[2]));
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
        ImGui::TextUnformatted(tr("Access method"));
        ImGui::SameLine(second_column_offset);

        std::string access_method = state.config()->readValue(core::AccessKeys::cfg_cdrom_access_method);
        static auto method        = int{util::toUnderlying(state.config()->getCdromAccess(access_method))};
        if (ImGui::RadioButton("SPTI", &method, util::toUnderlying(cdrom::CdromAccessMethod::spti))) {
            const auto key = state.config()->getCdromAccessKey(cdrom::CdromAccessMethod::spti);
            if (key != std::nullopt) {
                state.config()->writeValue(core::AccessKeys::cfg_cdrom_access_method, *key);
            } else {
                Log::warning("config", tr("Unknown drive access method ..."));
            }
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("ASPI", &method, util::toUnderlying(cdrom::CdromAccessMethod::aspi))) {
            const auto key = state.config()->getCdromAccessKey(cdrom::CdromAccessMethod::aspi);
            if (key != std::nullopt) {
                state.config()->writeValue(core::AccessKeys::cfg_cdrom_access_method, *key);
            } else {
                Log::warning("config", tr("Unknown drive access method ..."));
            }
        }

        // CD-Rom system ID
    }

    // Sound header
    if (ImGui::CollapsingHeader(tr("Sound"))) {
        // Sound disabled
        ImGui::TextUnformatted(tr("Sound disabled"));
        ImGui::SameLine(second_column_offset);

        static bool disabled = state.config()->readValue(core::AccessKeys::cfg_sound_disabled);
        if (ImGui::Checkbox("", &disabled)) { state.config()->writeValue(core::AccessKeys::cfg_sound_disabled, disabled); }
    }

    // Peripheral header
    if (ImGui::CollapsingHeader(tr("Peripherals"))) {
        static const auto keys{core::Smpc::listAvailableKeys()};

        auto tab_bar_flags = ImGuiTabBarFlags{ImGuiTabBarFlags_None};
        if (ImGui::BeginTabBar("PeripheralTabBar", tab_bar_flags)) {
            auto           window_flags      = ImGuiWindowFlags{ImGuiWindowFlags_None};
            constexpr auto rounding          = float{5.0f};
            constexpr auto child_width_ratio = float{0.5f};
            constexpr auto child_height      = u16{280};
            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, rounding);
            if (ImGui::BeginTabItem("Saturn")) {
                {
                    //**** Saturn Player 1 ****//
                    static auto pad = SaturnDigitalPad{};
                    pad.fromConfig(state.config()->readPeripheralConfiguration(core::AccessKeys::cfg_controls_saturn_player_1));

                    ImGui::BeginChild("ChildSaturnPlayer1",
                                      ImVec2(ImGui::GetWindowContentRegionWidth() * child_width_ratio, child_height),
                                      true,
                                      window_flags);

                    ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth() - second_column_offset);

                    ImGui::CenteredText(tr("Player 1"));

                    ImGui::TextUnformatted(tr("Connection"));
                    ImGui::SameLine(second_column_offset);
                    static auto connections = state.config()->listPeripheralConnections();
                    std::string c = state.config()->readValue(core::AccessKeys::cfg_controls_saturn_player_1_connection);
                    const auto  it_connection
                        = std::find_if(connections.begin(), connections.end(), [&c](std::string& str) { return c == str; });
                    static auto index_connection = static_cast<s32>(it_connection - connections.begin());
                    if (ImGui::Combo("##peripheral_connection_1", &index_connection, connections)) {
                        state.config()->writeValue(core::AccessKeys::cfg_controls_saturn_player_1_connection,
                                                   connections[index_connection]);
                    }

                    ImGui::TextUnformatted(tr("Left"));
                    ImGui::SameLine(second_column_offset);
                    ImGui::peripheralKeyCombo(keys, pad.direction_left, "direction_left");

                    ImGui::TextUnformatted(tr("Right"));
                    ImGui::SameLine(second_column_offset);
                    ImGui::peripheralKeyCombo(keys, pad.direction_right, "direction_right");

                    ImGui::TextUnformatted(tr("Up"));
                    ImGui::SameLine(second_column_offset);
                    ImGui::peripheralKeyCombo(keys, pad.direction_up, "direction_up");

                    ImGui::TextUnformatted(tr("Down"));
                    ImGui::SameLine(second_column_offset);
                    ImGui::peripheralKeyCombo(keys, pad.direction_down, "direction_down");

                    ImGui::TextUnformatted(tr("Left shoulder"));
                    ImGui::SameLine(second_column_offset);
                    ImGui::peripheralKeyCombo(keys, pad.button_shoulder_left, "button_left_shoulder");

                    ImGui::TextUnformatted(tr("Right shoulder"));
                    ImGui::SameLine(second_column_offset);
                    ImGui::peripheralKeyCombo(keys, pad.button_shoulder_right, "button_right_shoulder");

                    ImGui::TextUnformatted(tr("Button A"));
                    ImGui::SameLine(second_column_offset);
                    ImGui::peripheralKeyCombo(keys, pad.button_a, "button_a");

                    ImGui::TextUnformatted(tr("Button B"));
                    ImGui::SameLine(second_column_offset);
                    ImGui::peripheralKeyCombo(keys, pad.button_b, "button_b");

                    ImGui::TextUnformatted(tr("Button C"));
                    ImGui::SameLine(second_column_offset);
                    ImGui::peripheralKeyCombo(keys, pad.button_c, "button_c");

                    ImGui::TextUnformatted(tr("Button X"));
                    ImGui::SameLine(second_column_offset);
                    ImGui::peripheralKeyCombo(keys, pad.button_x, "button_x");

                    ImGui::TextUnformatted(tr("Button Y"));
                    ImGui::SameLine(second_column_offset);
                    ImGui::peripheralKeyCombo(keys, pad.button_y, "button_y");

                    ImGui::TextUnformatted(tr("Button Z"));
                    ImGui::SameLine(second_column_offset);
                    ImGui::peripheralKeyCombo(keys, pad.button_z, "button_z");

                    ImGui::TextUnformatted(tr("Button Start"));
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

                    ImGui::BeginChild("ChildSaturnPlayer2",
                                      ImVec2(ImGui::GetWindowContentRegionWidth() * child_width_ratio, child_height),
                                      true,
                                      window_flags);
                    ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth() - second_column_offset);

                    ImGui::CenteredText(tr("Player 2"));

                    ImGui::TextUnformatted(tr("Connection"));
                    ImGui::SameLine(second_column_offset);
                    static auto connections = state.config()->listPeripheralConnections();
                    std::string c = state.config()->readValue(core::AccessKeys::cfg_controls_saturn_player_2_connection);
                    const auto  it_connection
                        = std::find_if(connections.begin(), connections.end(), [&c](std::string& str) { return c == str; });
                    static auto index_connection = static_cast<s32>(it_connection - connections.begin());
                    if (ImGui::Combo("##peripheral_connection_2", &index_connection, connections)) {
                        state.config()->writeValue(core::AccessKeys::cfg_controls_saturn_player_1_connection,
                                                   connections[index_connection]);
                    }

                    ImGui::TextUnformatted(tr("Left"));
                    ImGui::SameLine(second_column_offset);
                    ImGui::peripheralKeyCombo(keys, pad_p2.direction_left, "direction_left");

                    ImGui::TextUnformatted(tr("Right"));
                    ImGui::SameLine(second_column_offset);
                    ImGui::peripheralKeyCombo(keys, pad_p2.direction_right, "direction_right");

                    ImGui::TextUnformatted(tr("Up"));
                    ImGui::SameLine(second_column_offset);
                    ImGui::peripheralKeyCombo(keys, pad_p2.direction_up, "direction_up");

                    ImGui::TextUnformatted(tr("Down"));
                    ImGui::SameLine(second_column_offset);
                    ImGui::peripheralKeyCombo(keys, pad_p2.direction_down, "direction_down");

                    ImGui::TextUnformatted(tr("Left shoulder"));
                    ImGui::SameLine(second_column_offset);
                    ImGui::peripheralKeyCombo(keys, pad_p2.button_shoulder_left, "button_left_shoulder");

                    ImGui::TextUnformatted(tr("Right shoulder"));
                    ImGui::SameLine(second_column_offset);
                    ImGui::peripheralKeyCombo(keys, pad_p2.button_shoulder_right, "button_right_shoulder");

                    ImGui::TextUnformatted(tr("Button A"));
                    ImGui::SameLine(second_column_offset);
                    ImGui::peripheralKeyCombo(keys, pad_p2.button_a, "button_a");

                    ImGui::TextUnformatted(tr("Button B"));
                    ImGui::SameLine(second_column_offset);
                    ImGui::peripheralKeyCombo(keys, pad_p2.button_b, "button_b");

                    ImGui::TextUnformatted(tr("Button C"));
                    ImGui::SameLine(second_column_offset);
                    ImGui::peripheralKeyCombo(keys, pad_p2.button_c, "button_c");

                    ImGui::TextUnformatted(tr("Button X"));
                    ImGui::SameLine(second_column_offset);
                    ImGui::peripheralKeyCombo(keys, pad_p2.button_x, "button_x");

                    ImGui::TextUnformatted(tr("Button Y"));
                    ImGui::SameLine(second_column_offset);
                    ImGui::peripheralKeyCombo(keys, pad_p2.button_y, "button_y");

                    ImGui::TextUnformatted(tr("Button Z"));
                    ImGui::SameLine(second_column_offset);
                    ImGui::peripheralKeyCombo(keys, pad_p2.button_z, "button_z");

                    ImGui::TextUnformatted(tr("Button Start"));
                    ImGui::SameLine(second_column_offset);
                    ImGui::peripheralKeyCombo(keys, pad_p2.button_start, "button_start");

                    state.config()->writeValue(core::AccessKeys::cfg_controls_saturn_player_2,
                                               pad_p2.toConfig(PeripheralLayout::current_layout));

                    ImGui::EndChild();
                }
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("ST-V")) {
                {
                    //**** ST-V Board ****//
                    static auto board = StvBoardControls{};
                    board.fromConfig(state.config()->readPeripheralConfiguration(core::AccessKeys::cfg_controls_stv_board));

                    constexpr auto h_size = u16{0};
                    constexpr auto v_size = u16{160};
                    ImGui::BeginChild("ChildStvBoard", ImVec2(h_size, v_size), true, window_flags);

                    ImGui::CenteredText(tr("Board"));

                    ImGui::TextUnformatted(tr("Service switch"));
                    ImGui::SameLine(second_column_offset);
                    ImGui::peripheralKeyCombo(keys, board.service_switch, "service_switch");

                    ImGui::TextUnformatted(tr("Test switch"));
                    ImGui::SameLine(second_column_offset);
                    ImGui::peripheralKeyCombo(keys, board.test_switch, "test_switch");

                    ImGui::TextUnformatted(tr("Player 1 coin switch"));
                    ImGui::SameLine(second_column_offset);
                    ImGui::peripheralKeyCombo(keys, board.p1_coin_switch, "p1_coin_switch");

                    ImGui::TextUnformatted(tr("Player 2 coin switch"));
                    ImGui::SameLine(second_column_offset);
                    ImGui::peripheralKeyCombo(keys, board.p2_coin_switch, "p2_coin_switch");

                    ImGui::TextUnformatted(tr("Player 1 start"));
                    ImGui::SameLine(second_column_offset);
                    ImGui::peripheralKeyCombo(keys, board.p1_start, "p1_start");

                    ImGui::TextUnformatted(tr("Player 2 start"));
                    ImGui::SameLine(second_column_offset);
                    ImGui::peripheralKeyCombo(keys, board.p2_start, "p2_start");

                    state.config()->writeValue(core::AccessKeys::cfg_controls_stv_board,
                                               board.toConfig(PeripheralLayout::current_layout));

                    ImGui::EndChild();
                }

                {
                    //**** ST-V Player 1 ****//
                    static auto controls = StvPlayerControls{};
                    controls.fromConfig(state.config()->readPeripheralConfiguration(core::AccessKeys::cfg_controls_stv_player_1));

                    constexpr auto child_height = u16{220};
                    ImGui::BeginChild("ChildStvPlayer1",
                                      ImVec2(ImGui::GetWindowContentRegionWidth() * child_width_ratio, child_height),
                                      true,
                                      window_flags);

                    ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth() - second_column_offset);

                    ImGui::CenteredText(tr("Player 1"));

                    ImGui::TextUnformatted(tr("Left"));
                    ImGui::SameLine(second_column_offset);
                    ImGui::peripheralKeyCombo(keys, controls.direction_left, "direction_left");

                    ImGui::TextUnformatted(tr("Right"));
                    ImGui::SameLine(second_column_offset);
                    ImGui::peripheralKeyCombo(keys, controls.direction_right, "direction_right");

                    ImGui::TextUnformatted(tr("Up"));
                    ImGui::SameLine(second_column_offset);
                    ImGui::peripheralKeyCombo(keys, controls.direction_up, "direction_up");

                    ImGui::TextUnformatted(tr("Down"));
                    ImGui::SameLine(second_column_offset);
                    ImGui::peripheralKeyCombo(keys, controls.direction_down, "direction_down");

                    ImGui::TextUnformatted(tr("Button 1"));
                    ImGui::SameLine(second_column_offset);
                    ImGui::peripheralKeyCombo(keys, controls.button_1, "button_1");

                    ImGui::TextUnformatted(tr("Button 2"));
                    ImGui::SameLine(second_column_offset);
                    ImGui::peripheralKeyCombo(keys, controls.button_2, "button_2");

                    ImGui::TextUnformatted(tr("Button 3"));
                    ImGui::SameLine(second_column_offset);
                    ImGui::peripheralKeyCombo(keys, controls.button_3, "button_3");

                    ImGui::TextUnformatted(tr("Button 4"));
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
                    controls.fromConfig(state.config()->readPeripheralConfiguration(core::AccessKeys::cfg_controls_stv_player_2));

                    constexpr auto child_height = u16{220};
                    ImGui::BeginChild("ChildStvPlayer2",
                                      ImVec2(ImGui::GetWindowContentRegionWidth() * child_width_ratio, child_height),
                                      true,
                                      window_flags);

                    ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth() - second_column_offset);

                    ImGui::CenteredText(tr("Player 2"));

                    ImGui::TextUnformatted(tr("Left"));
                    ImGui::SameLine(second_column_offset);
                    ImGui::peripheralKeyCombo(keys, controls.direction_left, "direction_left");

                    ImGui::TextUnformatted(tr("Right"));
                    ImGui::SameLine(second_column_offset);
                    ImGui::peripheralKeyCombo(keys, controls.direction_right, "direction_right");

                    ImGui::TextUnformatted(tr("Up"));
                    ImGui::SameLine(second_column_offset);
                    ImGui::peripheralKeyCombo(keys, controls.direction_up, "direction_up");

                    ImGui::TextUnformatted(tr("Down"));
                    ImGui::SameLine(second_column_offset);
                    ImGui::peripheralKeyCombo(keys, controls.direction_down, "direction_down");

                    ImGui::TextUnformatted(tr("Button 1"));
                    ImGui::SameLine(second_column_offset);
                    ImGui::peripheralKeyCombo(keys, controls.button_1, "button_1");

                    ImGui::TextUnformatted(tr("Button 2"));
                    ImGui::SameLine(second_column_offset);
                    ImGui::peripheralKeyCombo(keys, controls.button_2, "button_2");

                    ImGui::TextUnformatted(tr("Button 3"));
                    ImGui::SameLine(second_column_offset);
                    ImGui::peripheralKeyCombo(keys, controls.button_3, "button_3");

                    ImGui::TextUnformatted(tr("Button 4"));
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
        ImGui::PopItemWidth();
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

    ImGui::End();
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

    const auto v_pos{(float)height - window_size.y};
    const auto window_pos = ImVec2(0, v_pos);
    ImGui::SetNextWindowPos(window_pos, ImGuiCond_Once);

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

void showSh2DebugWindow(core::EmulatorContext& state, video::Opengl& opengl, bool* opened) {
    const auto window_size = ImVec2(650, 420);
    ImGui::SetNextWindowSize(window_size);

    auto window_flags = ImGuiWindowFlags{ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings
                                         | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar};
    ImGui::Begin("Sh2 debug", opened, window_flags);

    static auto sh2_type   = Sh2Type{Sh2Type::master};
    static auto current_pc = u32{state.slaveSh2()->getRegister(Sh2Register::pc)};
    if (ImGui::RadioButton(tr("Master"), sh2_type == Sh2Type::master)) {
        sh2_type = Sh2Type::master;
        //        current_pc = state.masterSh2()->getRegister(Sh2Register::pc);
    }
    ImGui::SameLine();
    if (ImGui::RadioButton(tr("Slave"), sh2_type == Sh2Type::slave)) {
        sh2_type = Sh2Type::slave;
        // current_pc = state.slaveSh2()->getRegister(Sh2Register::pc);
    };

    sh2::Sh2* current_sh2{nullptr};
    switch (sh2_type) {
        case Sh2Type::master: current_sh2 = state.masterSh2(); break;
        case Sh2Type::slave: current_sh2 = state.slaveSh2(); break;
    }

    current_pc = current_sh2->getRegister(Sh2Register::pc);

    ImGui::SameLine(ImGui::GetWindowWidth() / 2);

    {
        // Debug buttons
        const ImVec2 button_size(30, 30);
        if (ImGui::ImageButton(opengl.getIconTexture(video::IconId::step_into), button_size)) {
            state.debugStatus(core::DebugStatus::step_into);
        }
        ImGui::SameLine();
        ImGui::PushButtonRepeat(true);
        if (ImGui::ImageButton(opengl.getIconTexture(video::IconId::step_over), button_size)) {
            state.debugStatus(core::DebugStatus::step_over);
        }
        ImGui::SameLine();
        if (ImGui::ImageButton(opengl.getIconTexture(video::IconId::step_out), button_size)) {
            state.debugStatus(core::DebugStatus::step_out);
        }
    }

    {
        // General registers
        const auto child_size = ImVec2(300, 240);
        ImGui::BeginChild("ChildRegisters", child_size, true, window_flags);

        ImGui::TextDisabled(tr("General registers"));
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
        ImGui::TextDisabled(tr("System registers"));
        ImGui::NextColumn();
        ImGui::TextDisabled(tr("Control registers"));

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
    ImGui::SameLine();
    {
        // Disassembly
        const auto child_size = ImVec2(330, 240);
        ImGui::BeginChild("ChildDisassembly", child_size, true, window_flags);

        ImGui::TextDisabled(tr("Disassembly"));
        ImGui::Separator();

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, 305.0f);
        ImGui::SetColumnWidth(1, 25.0f);

        for (u32 i = (current_pc - 6); i < (current_pc + 20); i += 2) {
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
    ImGui::SameLine(304);

    {
        // Callstack

        const auto child_size = ImVec2(330, 100);
        ImGui::BeginChild("ChildCallstack", child_size, true, window_flags);

        ImGui::TextDisabled(tr("Callstack"));
        ImGui::Separator();

        const auto callstack_mask = std::string{"{:#010x}"};
        for (auto i = current_sh2->callstack().rbegin(); i != current_sh2->callstack().rend(); ++i) {
            ImGui::TextUnformatted(fmt::format(callstack_mask, (*i).call_address).c_str());
        }

        ImGui::EndChild();
    }

    ImGui::End();
}

void showMemoryDebugWindow(core::EmulatorContext& state, bool* opened) {
    const auto window_size = ImVec2(600, 320);
    ImGui::SetNextWindowSize(window_size);

    auto window_flags
        = ImGuiWindowFlags{ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse};
    ImGui::Begin(tr("Memory debug"), opened, window_flags);

    static auto current_area = MemoryMapArea::rom;

    if (ImGui::BeginCombo(tr("Memory area"), state.memory()->memory_map_[current_area].c_str())) {
        for (const auto& [k, v] : state.memory()->memory_map_) {
            const auto is_selected = bool{current_area == k};
            if (ImGui::Selectable(v.c_str(), is_selected)) { current_area = k; }
            if (is_selected) { ImGui::SetItemDefaultFocus(); }
        }

        ImGui::EndCombo();
    }

    switch (current_area) {
        case MemoryMapArea::vdp2_registers:
            for (auto const& r : state.memory()->vdp2()->getRegisters()) {
                const auto mask = std::string{"{:#010x} {:<35} : {:#06x}"};

                ImGui::TextUnformatted(
                    fmt::format(mask, r.first, r.second, state.memory()->vdp2()->readRegisters<u16>(r.first)).c_str());
                // r.first
            }
            break;
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

void buildGui(core::EmulatorContext& state, video::Opengl& opengl, const u32 width, const u32 height) {
    showCoreWindow(state, opengl);

    constexpr auto window_width  = u16{320};
    constexpr auto window_height = u16{200};
    showRenderingWindow(opengl, window_width, window_height);

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
} // namespace saturnin::gui
