//
// vdp2_debug.cpp
// Saturnin
//
// Copyright (c) 2021 Renaud Toumazet
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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file	vdp2_debug.h
///
/// \brief	Declares the debug methods of the Vdp2 class.
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <saturnin/src/pch.h>
#include <saturnin/src/utilities.h> // format
#include <saturnin/src/video/vdp2.h>

namespace uti = saturnin::utilities;

namespace saturnin::video {

using core::tr;

//--------------------------------------------------------------------------------------------------------------
// DEBUG methods
//--------------------------------------------------------------------------------------------------------------

auto Vdp2::getDebugGlobalMainData() const -> std::vector<LabelValue> {
    auto values = std::vector<LabelValue>{};

    { // Resolution
        if (tv_screen_status_.screen_mode == ScreenMode::not_set) {
            values.emplace_back(tr("Resolution"), tr("Not set"));
        } else {
            auto screen_mode = std::string{};
            switch (tv_screen_status_.screen_mode) {
                using enum ScreenMode;
                case normal_320_224:
                case normal_320_240:
                case normal_320_256:
                case normal_320_448:
                case normal_320_480:
                case normal_320_512: screen_mode = tr("Normal Graphic A"); break;
                case normal_352_224:
                case normal_352_240:
                case normal_352_256:
                case normal_352_448:
                case normal_352_480:
                case normal_352_512: screen_mode = tr("Normal Graphic B"); break;
                case hi_res_640_224:
                case hi_res_640_240:
                case hi_res_640_256:
                case hi_res_640_448:
                case hi_res_640_480:
                case hi_res_640_512: screen_mode = tr("Hi-Res Graphic A"); break;
                case hi_res_704_224:
                case hi_res_704_240:
                case hi_res_704_256:
                case hi_res_704_448:
                case hi_res_704_480:
                case hi_res_704_512: screen_mode = tr("Hi-Res Graphic B"); break;
                case exclusive_320_480: screen_mode = tr("Exclusive Normal Graphic A"); break;
                case exclusive_352_480: screen_mode = tr("Exclusive Normal Graphic B"); break;
                case exclusive_640_480: screen_mode = tr("Exclusive Hi-Res Graphic A"); break;
                case exclusive_704_480: screen_mode = tr("Exclusive Hi-Res Graphic B"); break;
                default: screen_mode = tr("Unknown");
            }
            values.emplace_back(tr("Resolution"),
                                utilities::format("{:d}x{:d} - {:s}",
                                                  tv_screen_status_.horizontal_res,
                                                  tv_screen_status_.vertical_res,
                                                  screen_mode));
        }
    }

    { // Interlace mode
        auto mode = std::string{};
        switch (tv_screen_status_.interlace_mode) {
            using enum InterlaceMode;
            case non_interlace: mode = tr("Non interlace"); break;
            case single_density: mode = tr("Single density"); break;
            case double_density: mode = tr("Double density"); break;
            default: mode = tr("Not allowed"); break;
        }
        values.emplace_back(tr("Interlace mode"), mode);
    }

    //{ // Screen display enable

    //    auto nbgDisplayStatus = [&](const ScrollScreen s) {
    //        const auto& nbg = getScreen(s).is_display_enabled ? tr("Can display") : tr("Cannot display");
    //        values.emplace_back(format("NBG{}", toUnderlying(s)), nbg);
    //    };
    //    nbgDisplayStatus(ScrollScreen::nbg0);
    //    nbgDisplayStatus(ScrollScreen::nbg1);
    //    nbgDisplayStatus(ScrollScreen::nbg2);
    //    nbgDisplayStatus(ScrollScreen::nbg3);

    //    auto rbgDisplayStatus = [&](const ScrollScreen s) {
    //        const auto& rbg = getScreen(s).is_display_enabled ? tr("Can display") : tr("Cannot display");
    //        values.emplace_back(format("RBG{}", toUnderlying(s) - 4), rbg);
    //    };
    //    rbgDisplayStatus(ScrollScreen::rbg0);
    //    rbgDisplayStatus(ScrollScreen::rbg1);
    //}

    return values;
}

auto Vdp2::getDebugRamMainData() -> std::vector<LabelValue> {
    auto values = std::vector<LabelValue>{};

    const auto getVramSize = [](const VramSize sz) {
        switch (sz) {
            using enum VramSize;
            case size_4_mbits: return tr("4 mbits");
            case size_8_mbits: return tr("8 mbits");
            default: return tr("Not set");
        }
    };
    values.emplace_back(tr("VRAM size"), getVramSize(ram_status_.vram_size));

    const auto getVramMode = [](const VramMode mode) {
        switch (mode) {
            using enum VramMode;
            case no_partition: return tr("No partition");
            case partition_in_2_banks: return tr("Partition in 2 banks");
            default: return tr("Not set");
        }
    };
    values.emplace_back(tr("VRAM A mode"), getVramMode(ram_status_.vram_a_mode));
    values.emplace_back(tr("VRAM B mode"), getVramMode(ram_status_.vram_b_mode));

    const auto getColorRamMode = [](const ColorRamMode mode) {
        switch (mode) {
            using enum ColorRamMode;
            case mode_0_rgb_5_bits_1024_colors: return tr("Mode 0 (RGB 5 bits, 1024 colors) ");
            case mode_1_rgb_5_bits_2048_colors: return tr("Mode 1 (RGB 5 bits, 2048 colors) ");
            case mode_2_rgb_8_bits_1024_colors: return tr("Mode 2 (RGB 8 bits, 1024 colors) ");
            case setting_not_allowed: return tr("Setting not allowed");
            default: return tr("Not set");
        }
    };
    values.emplace_back(tr("Color RAM mode"), getColorRamMode(ram_status_.color_ram_mode));

    const auto getCoefficientTable = [](const CoefficientTableStorage cts) {
        switch (cts) {
            using enum CoefficientTableStorage;
            case stored_in_color_ram: return tr("Stored in color RAM");
            case stored_in_vram: return tr("Stored in VRAM");
            default: return tr("Not set");
        }
    };
    values.emplace_back(tr("Coefficient table"), getCoefficientTable(ram_status_.coefficient_table_storage));

    const auto getRotationBankSelect = [](const RotationDataBankSelect rdbs) {
        switch (rdbs) {
            using enum RotationDataBankSelect;
            case not_used: return tr("Not used as RBG0 RAM");
            case used_as_rbg0_character_pattern_table: return tr("Used for RBG0 Character Pattern table (or Bitmap Pattern)");
            case used_as_rbg0_coefficient_table: return tr("Used for RBG0 Coefficient table");
            case used_as_rbg0_pattern_name_table: return tr("Used for RBG0 Pattern Name table");
            default: return tr("Not set");
        }
    };
    values.emplace_back(tr("VRAM A0 rotation data bank select"), getRotationBankSelect(ram_status_.vram_a0_rotation_bank_select));
    values.emplace_back(tr("VRAM A1 rotation data bank select"), getRotationBankSelect(ram_status_.vram_a1_rotation_bank_select));
    values.emplace_back(tr("VRAM B0 rotation data bank select"), getRotationBankSelect(ram_status_.vram_b0_rotation_bank_select));
    values.emplace_back(tr("VRAM B1 rotation data bank select"), getRotationBankSelect(ram_status_.vram_b1_rotation_bank_select));
    return values;
}

auto Vdp2::getDebugVramAccessMainData() -> std::vector<LabelValue> {
    auto values = std::vector<LabelValue>{};

    const auto banks_used   = getDebugVramAccessBanksUsed();
    const auto addBankValue = [&](const std::string& bank_name, const u8 bank_index) {
        if (banks_used[bank_index]) {
            values.emplace_back(bank_name, tr("2 banks"));
        } else {
            values.emplace_back(bank_name, tr("1 bank"));
        }
    };
    addBankValue("VRAM-A", vram_bank_a1_index);
    addBankValue("VRAM-B", vram_bank_b1_index);

    return values;
}

auto Vdp2::getDebugVramAccessBanks() -> std::vector<VramTiming> {
    auto is_normal_mode = (toEnum<HorizontalResolution>(tvmd_.horizontal_resolution) == HorizontalResolution::normal_320);
    is_normal_mode |= (toEnum<HorizontalResolution>(tvmd_.horizontal_resolution) == HorizontalResolution::normal_352);

    const auto banks_used = getDebugVramAccessBanksUsed();
    auto       banks      = std::vector<VramTiming>{};

    const VramTiming bank_a0 = {toEnum<VramAccessCommand>(cyca0l_.t0),
                                toEnum<VramAccessCommand>(cyca0l_.t1),
                                toEnum<VramAccessCommand>(cyca0l_.t2),
                                toEnum<VramAccessCommand>(cyca0l_.t3),
                                is_normal_mode ? toEnum<VramAccessCommand>(cyca0u_.t4) : VramAccessCommand::no_access,
                                is_normal_mode ? toEnum<VramAccessCommand>(cyca0u_.t5) : VramAccessCommand::no_access,
                                is_normal_mode ? toEnum<VramAccessCommand>(cyca0u_.t6) : VramAccessCommand::no_access,
                                is_normal_mode ? toEnum<VramAccessCommand>(cyca0u_.t7) : VramAccessCommand::no_access};
    banks.emplace_back(bank_a0);

    if (banks_used[vram_bank_a1_index]) {
        const VramTiming bank_a1 = {toEnum<VramAccessCommand>(cyca1l_.t0),
                                    toEnum<VramAccessCommand>(cyca1l_.t1),
                                    toEnum<VramAccessCommand>(cyca1l_.t2),
                                    toEnum<VramAccessCommand>(cyca1l_.t3),
                                    is_normal_mode ? toEnum<VramAccessCommand>(cyca1u_.t4) : VramAccessCommand::no_access,
                                    is_normal_mode ? toEnum<VramAccessCommand>(cyca1u_.t5) : VramAccessCommand::no_access,
                                    is_normal_mode ? toEnum<VramAccessCommand>(cyca1u_.t6) : VramAccessCommand::no_access,
                                    is_normal_mode ? toEnum<VramAccessCommand>(cyca1u_.t7) : VramAccessCommand::no_access};
        banks.emplace_back(bank_a1);
    }

    const VramTiming bank_b0 = {toEnum<VramAccessCommand>(cycb0l_.t0),
                                toEnum<VramAccessCommand>(cycb0l_.t1),
                                toEnum<VramAccessCommand>(cycb0l_.t2),
                                toEnum<VramAccessCommand>(cycb0l_.t3),
                                is_normal_mode ? toEnum<VramAccessCommand>(cycb0u_.t4) : VramAccessCommand::no_access,
                                is_normal_mode ? toEnum<VramAccessCommand>(cycb0u_.t5) : VramAccessCommand::no_access,
                                is_normal_mode ? toEnum<VramAccessCommand>(cycb0u_.t6) : VramAccessCommand::no_access,
                                is_normal_mode ? toEnum<VramAccessCommand>(cycb0u_.t7) : VramAccessCommand::no_access};
    banks.emplace_back(bank_b0);

    if (banks_used[vram_bank_a1_index]) {
        const VramTiming bank_b1 = {toEnum<VramAccessCommand>(cycb1l_.t0),
                                    toEnum<VramAccessCommand>(cycb1l_.t1),
                                    toEnum<VramAccessCommand>(cycb1l_.t2),
                                    toEnum<VramAccessCommand>(cycb1l_.t3),
                                    is_normal_mode ? toEnum<VramAccessCommand>(cycb1u_.t4) : VramAccessCommand::no_access,
                                    is_normal_mode ? toEnum<VramAccessCommand>(cycb1u_.t5) : VramAccessCommand::no_access,
                                    is_normal_mode ? toEnum<VramAccessCommand>(cycb1u_.t6) : VramAccessCommand::no_access,
                                    is_normal_mode ? toEnum<VramAccessCommand>(cycb1u_.t7) : VramAccessCommand::no_access};
        banks.emplace_back(bank_b1);
    }
    return banks;
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto Vdp2::getDebugVramAccessBanksUsed() -> std::array<bool, vram_banks_number> {
    // Bank A/A0 and B/B0 are always used
    auto banks = std::array{true, false, true, false};
    if (toEnum<VramMode>(ramctl_.vram_a_mode) == VramMode::partition_in_2_banks) { banks[vram_bank_a1_index] = true; }
    if (toEnum<VramMode>(ramctl_.vram_b_mode) == VramMode::partition_in_2_banks) { banks[vram_bank_b1_index] = true; }

    return banks;
}

auto Vdp2::getDebugVramAccessBanksName() -> std::vector<std::string> {
    const auto banks_used = getDebugVramAccessBanksUsed();
    auto       banks_name = std::vector<std::string>{};

    if (!banks_used[video::vram_bank_a1_index]) {
        banks_name.emplace_back("VRAM-A");
    } else {
        banks_name.emplace_back("VRAM-A0");
        banks_name.emplace_back("VRAM-A1");
    }
    if (!banks_used[video::vram_bank_b1_index]) {
        banks_name.emplace_back("VRAM-B");
    } else {
        banks_name.emplace_back("VRAM-B0");
        banks_name.emplace_back("VRAM-B1");
    }

    return banks_name;
}

// static
auto Vdp2::getDebugVramAccessCommandDescription(const VramAccessCommand command) -> LabelValue {
    using VramCommandsDescription         = std::map<VramAccessCommand, LabelValue>;
    const auto vram_commands_descriptions = VramCommandsDescription{
        {VramAccessCommand::nbg0_pattern_name_read, {"N0PN", tr("NBG0 Pattern Name Data Read")}},
        {VramAccessCommand::nbg1_pattern_name_read, {"N1PN", tr("NBG1 Pattern Name Data Read")}},
        {VramAccessCommand::nbg2_pattern_name_read, {"N2PN", tr("NBG2 Pattern Name Data Read")}},
        {VramAccessCommand::nbg3_pattern_name_read, {"N3PN", tr("NBG3 Pattern Name Data Read")}},
        {VramAccessCommand::nbg0_character_pattern_data_read, {"N0CG", tr("NBG0 Character Pattern Data Read")}},
        {VramAccessCommand::nbg1_character_pattern_data_read, {"N1CG", tr("NBG1 Character Pattern Data Read")}},
        {VramAccessCommand::nbg2_character_pattern_data_read, {"N2CG", tr("NBG2 Character Pattern Data Read")}},
        {VramAccessCommand::nbg3_character_pattern_data_read, {"N3CG", tr("NBG3 Character Pattern Data Read")}},
        {VramAccessCommand::setting_not_allowed_1, {"XXX", tr("Setting not allowed")}},
        {VramAccessCommand::setting_not_allowed_2, {"XXX", tr("Setting not allowed")}},
        {VramAccessCommand::setting_not_allowed_3, {"XXX", tr("Setting not allowed")}},
        {VramAccessCommand::setting_not_allowed_4, {"XXX", tr("Setting not allowed")}},
        {VramAccessCommand::nbg0_vertical_cell_scroll_table_data_read, {"N0CE", tr("NBG0 Vertical Cell Scroll Table Data Read")}},
        {VramAccessCommand::nbg1_vertical_cell_scroll_table_data_read, {"N1CE", tr("NBG1 Vertical Cell Scroll Table Data Read")}},
        {VramAccessCommand::cpu_read_write, {"CPU", tr("CPU Read/Write")}},
        {VramAccessCommand::no_access, {"NA", tr("No Access")}}};

    return vram_commands_descriptions.at(command);
}

auto Vdp2::getDebugScrollScreenData(const ScrollScreen s) -> std::optional<std::vector<LabelValue>> {
    auto        values = std::vector<LabelValue>{};
    const auto& screen = getScreen(s);

    if (!screen.is_display_enabled) { return std::nullopt; }

    const auto colorNumber = [](const ColorCount c) {
        switch (c) {
            using enum ColorCount;
            case cannot_display: return tr("Cannot display");
            case not_allowed: return tr("Not allowed");
            case palette_16: return tr("16 colors (palette)");
            case palette_256: return tr("256 colors (palette)");
            case palette_2048: return tr("2048 colors (palette)");
            case rgb_32k: return tr("32K colors (RGB)");
            case rgb_16m: return tr("16M colors (RGB)");
            default: return tr("Not set");
        }
    };

    if (screen.format == ScrollScreenFormat::bitmap) {
        // Format
        values.emplace_back(tr("Format"), tr("Bitmap"));

        // Bitmap color number
        values.emplace_back(tr("Bitmap color number"), colorNumber(screen.character_color_number));

        // Bitmap size
        const auto bitmapSize = [](const BitmapSize sz) {
            switch (sz) {
                using enum BitmapSize;
                case size_512_by_256: return tr("512 H x 256 V dots");
                case size_512_by_512: return tr("512 H x 512 V dots");
                case size_1024_by_256: return tr("1024 H x 256 V dots");
                case size_1024_by_512: return tr("1024 H x 512 V dots");
                default: return tr("Not set");
            }
        };
        values.emplace_back(tr("Bitmap size"), bitmapSize(screen.bitmap_size));
        values.emplace_back(tr("Bitmap palette number"), uti::format("{:#x}", screen.bitmap_palette_number));
        values.emplace_back(tr("Priority"), uti::format("{}", screen.priority_number));
        values.emplace_back(tr("Bitmap special priority bit"), uti::format("{:#x}", screen.bitmap_special_priority));
        values.emplace_back(tr("Bitmap special color calculation bit"),
                            uti::format("{:#x}", screen.bitmap_special_color_calculation));
        values.emplace_back(tr("Bitmap start address"), uti::format("{:#x}", screen.bitmap_start_address));
        return values;
    }

    // Format
    values.emplace_back(tr("Format"), tr("Cell"));

    // Scroll size
    // const auto size = screen.page_size * screen.plane_size * screen.map_size;
    // values.emplace_back(tr("Total screen size"), format("{:#x}", size));

    // Map size
    const auto mapSize = [](const ScrollScreen s) {
        switch (s) {
            using enum ScrollScreen;
            case rbg0:
            case rbg1: return tr("4x4 planes");
            default: return tr("2x2 planes");
        }
    };
    values.emplace_back(tr("Map size"), mapSize(s));

    // Plane size
    const auto planeSize = [](const ScrollScreenStatus s) {
        switch (s.plane_size) {
            using enum PlaneSize;
            case size_1_by_1: return tr("1x1 page");
            case size_2_by_1: return tr("2x1 pages");
            case size_2_by_2: return tr("2x2 pages");
            default: return tr("Unknown");
        }
    };
    values.emplace_back(tr("Plane size"), planeSize(screen));

    // Pattern Name Data size
    const auto& pnd_size = screen.pattern_name_data_size == PatternNameDataSize::one_word ? tr("1 word") : tr("2 words");
    values.emplace_back(tr("Pattern Name Data size"), pnd_size);

    if (screen.pattern_name_data_size == PatternNameDataSize::one_word) {
        // Character number supplement mode
        const auto& character_mode
            = (screen.character_number_supplement_mode == CharacterNumberSupplementMode::character_number_10_bits)
                  ? tr("10 bits, can flip")
                  : tr("12 bits, cannot flip");
        values.emplace_back(tr("Character number supplement mode"), character_mode);
        values.emplace_back(tr("Special priority bit"), uti::format("{:#x}", screen.special_priority));
        values.emplace_back(tr("Special color calculation bit"), uti::format("{:#x}", screen.special_color_calculation));
        values.emplace_back(tr("Supplementary palette number bit"), uti::format("{:#x}", screen.supplementary_palette_number));
        values.emplace_back(tr("Supplementary character number bit"),
                            uti::format("{:#x}", screen.supplementary_character_number));
    }

    // Character Pattern size
    const auto& cp_size = screen.character_pattern_size == CharacterSize::one_by_one ? tr("1x1 cell") : tr("2x2 cells");
    values.emplace_back(tr("Character Pattern size"), cp_size);

    // Character Pattern color number
    values.emplace_back(tr("Character color number"), colorNumber(screen.character_color_number));

    // Color RAM offset
    values.emplace_back(tr("Color RAM address offset"), uti::format("{:#x}", screen.color_ram_address_offset));

    // Cell size
    values.emplace_back(tr("Cell size"), tr("8x8 dots"));

    // Priority
    values.emplace_back(tr("Priority"), uti::format("{}", screen.priority_number));

    // Plane start address
    values.emplace_back(tr("Plane A start address"), uti::format("{:#010x}", screen.plane_a_start_address));
    values.emplace_back(tr("Plane B start address"), uti::format("{:#010x}", screen.plane_b_start_address));
    values.emplace_back(tr("Plane C start address"), uti::format("{:#010x}", screen.plane_c_start_address));
    values.emplace_back(tr("Plane D start address"), uti::format("{:#010x}", screen.plane_d_start_address));
    const auto rotation_screens = std::array{ScrollScreen::rbg0, ScrollScreen::rbg1};
    if (std::any_of(rotation_screens.begin(), rotation_screens.end(), [&s](const ScrollScreen rss) { return rss == s; })) {
        values.emplace_back(tr("Plane E start address"), uti::format("{:#010x}", screen.plane_e_start_address));
        values.emplace_back(tr("Plane F start address"), uti::format("{:#010x}", screen.plane_f_start_address));
        values.emplace_back(tr("Plane G start address"), uti::format("{:#010x}", screen.plane_g_start_address));
        values.emplace_back(tr("Plane H start address"), uti::format("{:#010x}", screen.plane_h_start_address));
        values.emplace_back(tr("Plane I start address"), uti::format("{:#010x}", screen.plane_i_start_address));
        values.emplace_back(tr("Plane J start address"), uti::format("{:#010x}", screen.plane_j_start_address));
        values.emplace_back(tr("Plane K start address"), uti::format("{:#010x}", screen.plane_k_start_address));
        values.emplace_back(tr("Plane L start address"), uti::format("{:#010x}", screen.plane_l_start_address));
        values.emplace_back(tr("Plane M start address"), uti::format("{:#010x}", screen.plane_m_start_address));
        values.emplace_back(tr("Plane N start address"), uti::format("{:#010x}", screen.plane_n_start_address));
        values.emplace_back(tr("Plane O start address"), uti::format("{:#010x}", screen.plane_o_start_address));
        values.emplace_back(tr("Plane P start address"), uti::format("{:#010x}", screen.plane_p_start_address));
    }

    // Scrolling
    switch (s) {
        using enum ScrollScreen;
        case nbg0:
        case nbg1:
            values.emplace_back(tr("Screen scroll"),
                                uti::format("x = {}.{}, y = {}.{}",
                                            screen.screen_scroll_horizontal_integer,
                                            screen.screen_scroll_horizontal_fractional,
                                            screen.screen_scroll_vertical_integer,
                                            screen.screen_scroll_vertical_fractional));
            break;
        case nbg2:
        case nbg3:
            values.emplace_back(
                tr("Screen scroll"),
                uti::format("x = {}, y = {}", screen.screen_scroll_horizontal_integer, screen.screen_scroll_vertical_integer));
            break;
        default: break;
    }

    values.emplace_back(tr("Scroll width"), uti::format("{}px", screen.total_screen_scroll_width));
    values.emplace_back(tr("Scroll height"), uti::format("{}px", screen.total_screen_scroll_height));

    auto offset = screen.scroll_offset_horizontal;
    values.emplace_back(tr("Scroll offset"), uti::format("{}", offset));

    // Color offset
    if (screen.is_color_offset_enabled) {
        values.emplace_back(tr("Color offset"),
                            uti::format("R:{:+d} G:{:+d} B:{:+d}",
                                        screen.color_offset.as_s16.r,
                                        screen.color_offset.as_s16.g,
                                        screen.color_offset.as_s16.b));
        // values.emplace_back(
        //     tr("  R:G:B"),
        //     format("R:{:+d} G:{:+d} B:{:+d}", screen.color_offset_red, screen.color_offset_green,
        //     screen.color_offset_blue));
    } else {
        values.emplace_back(tr("Color offset"), tr("Disabled"));
    }
    return values;
}

auto Vdp2::isLayerDisabled(const ScrollScreen s) -> bool {
    try {
        return disabled_scroll_screens_.at(s);
    } catch (std::exception const& e) { Log::warning(Logger::exception, e.what()); }

    return false;
}

void Vdp2::disableLayer(const ScrollScreen s, const bool is_disabled) { disabled_scroll_screens_[s] = is_disabled; }
} // namespace saturnin::video
