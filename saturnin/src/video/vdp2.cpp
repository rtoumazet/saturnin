//
// vdp2.cpp
// Saturnin
//
// Copyright (c) 2020 Renaud Toumazet
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
#include <saturnin/src/video/vdp2.h>
#include <istream>
#include <set>
#include <unordered_set>
#include <saturnin/src/config.h>
#include <saturnin/src/interrupt_sources.h>
#include <saturnin/src/scu_registers.h>
#include <saturnin/src/utilities.h> // toUnderlying
#include <saturnin/src/video/opengl.h>
#include <saturnin/src/video/texture.h>
#include <saturnin/src/video/vdp1.h>
#include <saturnin/src/video/vdp2_registers.h>

namespace saturnin::video {
namespace interrupt_source = core::interrupt_source;
namespace util             = saturnin::utilities;
using namespace vdp2_register_address;

using core::Config;
using core::EmulatorContext;
using core::Log;
using core::Logger;
using core::StartingFactorSelect;
using core::tr;
using util::toUnderlying;

//--------------------------------------------------------------------------------------------------------------
// PUBLIC section
//--------------------------------------------------------------------------------------------------------------

void Vdp2::initialize() {
    initializeRegisterNameMap();
    computePrecalculatedData();

    const std::string ts = modules_.config()->readValue(core::AccessKeys::cfg_rendering_tv_standard);
    switch (modules_.config()->getTvStandard(ts)) {
        case video::TvStandard::pal: tvstat_.set(ScreenStatus::tv_standard_flag, TvStandardFlag::pal_standard); break;
        case video::TvStandard::ntsc: tvstat_.set(ScreenStatus::tv_standard_flag, TvStandardFlag::ntsc_standard); break;
        default: Log::warning(Logger::vdp2, tr("Unknown TV standard"));
    }
    calculateDisplayDuration();
}

void Vdp2::run(const u8 cycles) {
    elapsed_frame_cycles_ += cycles;
    if (elapsed_frame_cycles_ > cycles_per_vactive_) {
        if (!is_vblank_current_) {
            // Entering vertical blanking
            is_vblank_current_ = true;
            tvstat_.set(ScreenStatus::vertical_blank_flag, VerticalBlankFlag::during_vertical_retrace);
            tvmd_.set(TvScreenMode::display, Display::not_displayed);

            Log::debug(Logger::vdp2, tr("VBlankIn interrupt request"));

            // if (modules_.memory()->was_vdp2_cram_accessed_) {
            //    Texture::discardCache();
            //    modules_.memory()->was_vdp2_cram_accessed_ = false;
            //}

            modules_.vdp1()->onVblankIn();
            this->onVblankIn();

            modules_.scu()->onVblankIn();

            modules_.opengl()->displayFramebuffer(*(modules_.context()));
            if (modules_.context()->debugStatus() == core::DebugStatus::next_frame) {
                modules_.context()->debugStatus(core::DebugStatus::paused);
            }
        }
    }

    if (elapsed_frame_cycles_ > cycles_per_frame_) {
        // End of the frame display (active + vblank)
        elapsed_frame_cycles_ = 0;
        is_vblank_current_    = false;
        tvstat_.set(ScreenStatus::vertical_blank_flag, VerticalBlankFlag::during_vertical_scan);

        elapsed_line_cycles_ = 0;
        is_hblank_current_   = false;
        tvstat_.set(ScreenStatus::horizontal_blank_flag, HorizontalBlankFlag::during_horizontal_scan);

        tvmd_.set(TvScreenMode::display, Display::displayed);

        Log::debug(Logger::vdp2, tr("VBlankOut interrupt request"));
        modules_.scu()->onVblankOut();

        timer_0_counter_ = 0;

        calculateDisplayDuration();
        return;
    }

    elapsed_line_cycles_ += cycles;
    if (elapsed_line_cycles_ > cycles_per_hactive_) {
        if (!is_hblank_current_) {
            // Entering horizontal blanking
            is_hblank_current_ = true;
            tvstat_.set(ScreenStatus::horizontal_blank_flag, HorizontalBlankFlag::during_horizontal_retrace);

            modules_.scu()->onHblankIn();

            timer_0_counter_++;

            if (timer_0_counter_ == modules_.scu()->getTimer0CompareValue()) { modules_.scu()->onTimer0(); }
        }
    }

    if (elapsed_line_cycles_ > cycles_per_line_) {
        // End of line display (active + hblank)
        elapsed_line_cycles_ = 0;
        is_hblank_current_   = false;
        tvstat_.set(ScreenStatus::horizontal_blank_flag, HorizontalBlankFlag::during_horizontal_scan);
    }

    // Yet to implement : H Counter, V Counter, Timer 1
}

auto Vdp2::getRegisters() const -> const AddressToNameMap& { return address_to_name_; };

void Vdp2::onSystemClockUpdate() { calculateDisplayDuration(); }

void Vdp2::calculateDisplayDuration() {
    // - A full frame vertical resolution is :
    //      - 262.5 lines for NTSC
    //      - 312.5 for PAL

    constexpr auto lines_nb_224 = u16{224};
    constexpr auto lines_nb_240 = u16{240};
    constexpr auto lines_nb_256 = u16{256};

    std::string ts = modules_.context()->config()->readValue(core::AccessKeys::cfg_rendering_tv_standard);
    switch (modules_.context()->config()->getTvStandard(ts)) {
        case video::TvStandard::pal: {
            constexpr auto frame_duration = seconds{1.0 / 50.0};
            cycles_per_frame_             = modules_.smpc()->calculateCyclesNumber(frame_duration);

            constexpr auto total_lines   = u16{313};
            auto           visible_lines = u16{};
            switch (tvmd_.get(TvScreenMode::vertical_resolution)) {
                case VerticalResolution::lines_nb_224: visible_lines = lines_nb_224; break;
                case VerticalResolution::lines_nb_240: visible_lines = lines_nb_240; break;
                case VerticalResolution::lines_nb_256: visible_lines = lines_nb_256; break;
                default: core::Log::warning(Logger::vdp2, core::tr("Unknown PAL vertical resolution."));
            }
            const auto vblank_lines = u16{static_cast<u16>(total_lines - visible_lines)};
            cycles_per_vblank_      = vblank_lines * cycles_per_frame_ / total_lines;
            cycles_per_vactive_     = cycles_per_frame_ - cycles_per_vblank_;

            constexpr auto pal_total_line_duration = micro{64};
            constexpr auto pal_hblank_duration     = micro{12};
            calculateLineDuration(pal_total_line_duration, pal_hblank_duration);

            // constexpr auto total_line_duration{nano(64)};
            // constexpr auto hblank_duration{nano(12)};
            // constexpr auto active_line_duration{total_line_duration - hblank_duration};
            // cycles_per_hblank_  = emulator_context_->smpc()->calculateCyclesNumber(hblank_duration);
            // cycles_per_hactive_ = emulator_context_->smpc()->calculateCyclesNumber(active_line_duration);
            // cycles_per_line_    = cycles_per_hactive_ + cycles_per_hblank_;
            break;
        }
        case video::TvStandard::ntsc: {
            constexpr auto frame_duration = seconds{1.0 / 60.0};
            cycles_per_frame_             = modules_.smpc()->calculateCyclesNumber(frame_duration);

            constexpr auto total_lines   = u16{263};
            auto           visible_lines = u16{};
            switch (tvmd_.get(TvScreenMode::vertical_resolution)) {
                case VerticalResolution::lines_nb_224: visible_lines = lines_nb_224; break;
                case VerticalResolution::lines_nb_240: visible_lines = lines_nb_240; break;
                default: core::Log::warning(Logger::vdp2, core::tr("Unknown NTSC vertical resolution."));
            }
            const auto vblank_lines = u16{static_cast<u16>(total_lines - visible_lines)};
            cycles_per_vblank_      = vblank_lines * cycles_per_frame_ / total_lines;
            cycles_per_vactive_     = cycles_per_frame_ - cycles_per_vblank_;

            constexpr auto ntsc_total_line_duration = micro{63.5};
            constexpr auto ntsc_hblank_duration     = micro{10.9};
            calculateLineDuration(ntsc_total_line_duration, ntsc_hblank_duration);
            break;
        }
        default: {
            core::Log::warning(Logger::vdp2, core::tr("Undefined TV standard."));
        }
    }
}

void Vdp2::onVblankIn() {
    updateResolution();
    updateRamStatus();
    clearRenderData();
    populateRenderData();
}

auto Vdp2::getSpriteColorAddressOffset() -> u16 {
    return getColorRamAddressOffset(craofb_.get(ColorRamAddressOffsetB::color_ram_address_offset_sprite));
}

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
                case ScreenMode::normal_320_224:
                case ScreenMode::normal_320_240:
                case ScreenMode::normal_320_256:
                case ScreenMode::normal_320_448:
                case ScreenMode::normal_320_480:
                case ScreenMode::normal_320_512: screen_mode = tr("Normal Graphic A"); break;
                case ScreenMode::normal_352_224:
                case ScreenMode::normal_352_240:
                case ScreenMode::normal_352_256:
                case ScreenMode::normal_352_448:
                case ScreenMode::normal_352_480:
                case ScreenMode::normal_352_512: screen_mode = tr("Normal Graphic B"); break;
                case ScreenMode::hi_res_640_224:
                case ScreenMode::hi_res_640_240:
                case ScreenMode::hi_res_640_256:
                case ScreenMode::hi_res_640_448:
                case ScreenMode::hi_res_640_480:
                case ScreenMode::hi_res_640_512: screen_mode = tr("Hi-Res Graphic A"); break;
                case ScreenMode::hi_res_704_224:
                case ScreenMode::hi_res_704_240:
                case ScreenMode::hi_res_704_256:
                case ScreenMode::hi_res_704_448:
                case ScreenMode::hi_res_704_480:
                case ScreenMode::hi_res_704_512: screen_mode = tr("Hi-Res Graphic B"); break;
                case ScreenMode::exclusive_320_480: screen_mode = tr("Exclusive Normal Graphic A"); break;
                case ScreenMode::exclusive_352_480: screen_mode = tr("Exclusive Normal Graphic B"); break;
                case ScreenMode::exclusive_640_480: screen_mode = tr("Exclusive Hi-Res Graphic A"); break;
                case ScreenMode::exclusive_704_480: screen_mode = tr("Exclusive Hi-Res Graphic B"); break;
                default: screen_mode = tr("Unknown");
            }
            values.emplace_back(
                tr("Resolution"),
                fmt::format("{:d}x{:d} - {:s}", tv_screen_status_.horizontal_res, tv_screen_status_.vertical_res, screen_mode));
        }
    }

    { // Interlace mode
        auto mode = std::string{};
        switch (tv_screen_status_.interlace_mode) {
            case InterlaceMode::non_interlace: mode = tr("Non interlace"); break;
            case InterlaceMode::single_density: mode = tr("Single density"); break;
            case InterlaceMode::double_density: mode = tr("Double density"); break;
            default: mode = tr("Not allowed"); break;
        }
        values.emplace_back(tr("Interlace mode"), mode);
    }

    { // Screen display enable

        auto nbgDisplayStatus = [&](const ScrollScreen s) {
            const auto& nbg = getScreen(s).is_display_enabled ? tr("Can display") : tr("Cannot display");
            values.emplace_back(fmt::format("NBG{}", toUnderlying(s)), nbg);
        };
        nbgDisplayStatus(ScrollScreen::nbg0);
        nbgDisplayStatus(ScrollScreen::nbg1);
        nbgDisplayStatus(ScrollScreen::nbg2);
        nbgDisplayStatus(ScrollScreen::nbg3);

        auto rbgDisplayStatus = [&](const ScrollScreen s) {
            const auto& rbg = getScreen(s).is_display_enabled ? tr("Can display") : tr("Cannot display");
            values.emplace_back(fmt::format("RBG{}", toUnderlying(s) - 4), rbg);
        };
        rbgDisplayStatus(ScrollScreen::rbg0);
        rbgDisplayStatus(ScrollScreen::rbg1);
    }

    return values;
}

auto Vdp2::getDebugRamMainData() -> std::vector<LabelValue> {
    auto values = std::vector<LabelValue>{};

    const auto getVramSize = [](const VramSize sz) {
        switch (sz) {
            case VramSize::size_4_mbits: return tr("4 mbits");
            case VramSize::size_8_mbits: return tr("8 mbits");
            default: return tr("Not set");
        }
    };
    values.emplace_back(tr("VRAM size"), getVramSize(ram_status_.vram_size));

    const auto getVramMode = [](const VramMode mode) {
        switch (mode) {
            case VramMode::no_partition: return tr("No partition");
            case VramMode::partition_in_2_banks: return tr("Partition in 2 banks");
            default: return tr("Not set");
        }
    };
    values.emplace_back(tr("VRAM A mode"), getVramMode(ram_status_.vram_a_mode));
    values.emplace_back(tr("VRAM B mode"), getVramMode(ram_status_.vram_b_mode));

    const auto getColorRamMode = [](const ColorRamMode mode) {
        switch (mode) {
            case ColorRamMode::mode_0_rgb_5_bits_1024_colors: return tr("Mode 0 (RGB 5 bits, 1024 colors) ");
            case ColorRamMode::mode_1_rgb_5_bits_2048_colors: return tr("Mode 1 (RGB 5 bits, 2048 colors) ");
            case ColorRamMode::mode_2_rgb_8_bits_1024_colors: return tr("Mode 2 (RGB 8 bits, 1024 colors) ");
            case ColorRamMode::setting_not_allowed: return tr("Setting not allowed");
            default: return tr("Not set");
        }
    };
    values.emplace_back(tr("Color RAM mode"), getColorRamMode(ram_status_.color_ram_mode));

    const auto getCoefficientTable = [](const CoefficientTableStorage cts) {
        switch (cts) {
            case CoefficientTableStorage::stored_in_color_ram: return tr("Stored in color RAM");
            case CoefficientTableStorage::stored_in_vram: return tr("Stored in VRAM");
            default: return tr("Not set");
        }
    };
    values.emplace_back(tr("Coefficient table"), getCoefficientTable(ram_status_.coefficient_table_storage));

    const auto getRotationBankSelect = [](const RotationDataBankSelect rdbs) {
        switch (rdbs) {
            case RotationDataBankSelect::not_used: return tr("Not used as RBG0 RAM");
            case RotationDataBankSelect::used_as_rbg0_character_pattern_table:
                return tr("Used for RBG0 Character Pattern table (or Bitmap Pattern)");
            case RotationDataBankSelect::used_as_rbg0_coefficient_table: return tr("Used for RBG0 Coefficient table");
            case RotationDataBankSelect::used_as_rbg0_pattern_name_table: return tr("Used for RBG0 Pattern Name table");
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
    auto is_normal_mode = (tvmd_.get(TvScreenMode::horizontal_resolution) == HorizontalResolution::normal_320);
    is_normal_mode |= (tvmd_.get(TvScreenMode::horizontal_resolution) == HorizontalResolution::normal_352);

    const auto banks_used = getDebugVramAccessBanksUsed();
    auto       banks      = std::vector<VramTiming>{};

    const VramTiming bank_a0 = {cyca0l_.get(VramCyclePatternBankA0Lower::t0),
                                cyca0l_.get(VramCyclePatternBankA0Lower::t1),
                                cyca0l_.get(VramCyclePatternBankA0Lower::t2),
                                cyca0l_.get(VramCyclePatternBankA0Lower::t3),
                                is_normal_mode ? cyca0u_.get(VramCyclePatternBankA0Upper::t4) : VramAccessCommand::no_access,
                                is_normal_mode ? cyca0u_.get(VramCyclePatternBankA0Upper::t5) : VramAccessCommand::no_access,
                                is_normal_mode ? cyca0u_.get(VramCyclePatternBankA0Upper::t6) : VramAccessCommand::no_access,
                                is_normal_mode ? cyca0u_.get(VramCyclePatternBankA0Upper::t7) : VramAccessCommand::no_access};
    banks.emplace_back(bank_a0);

    if (banks_used[vram_bank_a1_index]) {
        const VramTiming bank_a1 = {cyca1l_.get(VramCyclePatternBankA1Lower::t0),
                                    cyca1l_.get(VramCyclePatternBankA1Lower::t1),
                                    cyca1l_.get(VramCyclePatternBankA1Lower::t2),
                                    cyca1l_.get(VramCyclePatternBankA1Lower::t3),
                                    is_normal_mode ? cyca1u_.get(VramCyclePatternBankA1Upper::t4) : VramAccessCommand::no_access,
                                    is_normal_mode ? cyca1u_.get(VramCyclePatternBankA1Upper::t5) : VramAccessCommand::no_access,
                                    is_normal_mode ? cyca1u_.get(VramCyclePatternBankA1Upper::t6) : VramAccessCommand::no_access,
                                    is_normal_mode ? cyca1u_.get(VramCyclePatternBankA1Upper::t7) : VramAccessCommand::no_access};
        banks.emplace_back(bank_a1);
    }

    const VramTiming bank_b0 = {cycb0l_.get(VramCyclePatternBankB0Lower::t0),
                                cycb0l_.get(VramCyclePatternBankB0Lower::t1),
                                cycb0l_.get(VramCyclePatternBankB0Lower::t2),
                                cycb0l_.get(VramCyclePatternBankB0Lower::t3),
                                is_normal_mode ? cycb0u_.get(VramCyclePatternBankB0Upper::t4) : VramAccessCommand::no_access,
                                is_normal_mode ? cycb0u_.get(VramCyclePatternBankB0Upper::t5) : VramAccessCommand::no_access,
                                is_normal_mode ? cycb0u_.get(VramCyclePatternBankB0Upper::t6) : VramAccessCommand::no_access,
                                is_normal_mode ? cycb0u_.get(VramCyclePatternBankB0Upper::t7) : VramAccessCommand::no_access};
    banks.emplace_back(bank_b0);

    if (banks_used[vram_bank_a1_index]) {
        const VramTiming bank_b1 = {cycb1l_.get(VramCyclePatternBankB1Lower::t0),
                                    cycb1l_.get(VramCyclePatternBankB1Lower::t1),
                                    cycb1l_.get(VramCyclePatternBankB1Lower::t2),
                                    cycb1l_.get(VramCyclePatternBankB1Lower::t3),
                                    is_normal_mode ? cycb1u_.get(VramCyclePatternBankB1Upper::t4) : VramAccessCommand::no_access,
                                    is_normal_mode ? cycb1u_.get(VramCyclePatternBankB1Upper::t5) : VramAccessCommand::no_access,
                                    is_normal_mode ? cycb1u_.get(VramCyclePatternBankB1Upper::t6) : VramAccessCommand::no_access,
                                    is_normal_mode ? cycb1u_.get(VramCyclePatternBankB1Upper::t7) : VramAccessCommand::no_access};
        banks.emplace_back(bank_b1);
    }
    return banks;
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto Vdp2::getDebugVramAccessBanksUsed() -> std::array<bool, vram_banks_number> {
    // Bank A/A0 and B/B0 are always used
    auto banks = std::array{true, false, true, false};
    if (ramctl_.get(RamControl::vram_a_mode) == VramMode::partition_in_2_banks) { banks[vram_bank_a1_index] = true; }
    if (ramctl_.get(RamControl::vram_b_mode) == VramMode::partition_in_2_banks) { banks[vram_bank_b1_index] = true; }

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
            case ColorCount::cannot_display: return tr("Cannot display");
            case ColorCount::not_allowed: return tr("Not allowed");
            case ColorCount::palette_16: return tr("16 colors (palette)");
            case ColorCount::palette_256: return tr("256 colors (palette)");
            case ColorCount::palette_2048: return tr("2048 colors (palette)");
            case ColorCount::rgb_32k: return tr("32K colors (RGB)");
            case ColorCount::rgb_16m: return tr("16M colors (RGB)");
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
                case BitmapSize::size_512_by_256: return tr("512 H x 256 V dots");
                case BitmapSize::size_512_by_512: return tr("512 H x 512 V dots");
                case BitmapSize::size_1024_by_256: return tr("1024 H x 256 V dots");
                case BitmapSize::size_1024_by_512: return tr("1024 H x 512 V dots");
                default: return tr("Not set");
            }
        };
        values.emplace_back(tr("Bitmap size"), bitmapSize(screen.bitmap_size));
        values.emplace_back(tr("Bitmap palette number"), fmt::format("{:#x}", screen.bitmap_palette_number));
        values.emplace_back(tr("Bitmap special priority bit"), fmt::format("{:#x}", screen.bitmap_special_priority));
        values.emplace_back(tr("Bitmap special color calculation bit"),
                            fmt::format("{:#x}", screen.bitmap_special_color_calculation));
        values.emplace_back(tr("Bitmap start address"), fmt::format("{:#x}", screen.bitmap_start_address));
        return values;
    }

    // Format
    values.emplace_back(tr("Format"), tr("Cell"));

    // Scroll size
    // const auto size = screen.page_size * screen.plane_size * screen.map_size;
    // values.emplace_back(tr("Total screen size"), fmt::format("{:#x}", size));

    // Map size
    const auto mapSize = [](const ScrollScreen s) {
        switch (s) {
            case ScrollScreen::rbg0:
            case ScrollScreen::rbg1: return tr("4x4 planes");
            default: return tr("2x2 planes");
        }
    };
    values.emplace_back(tr("Map size"), mapSize(s));

    // Plane size
    const auto planeSize = [](const ScrollScreenStatus s) {
        switch (s.plane_size) {
            case PlaneSize::size_1_by_1: return tr("1x1 page");
            case PlaneSize::size_2_by_1: return tr("2x1 pages");
            case PlaneSize::size_2_by_2: return tr("2x2 pages");
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
        values.emplace_back(tr("Special priority bit"), fmt::format("{:#x}", screen.special_priority));
        values.emplace_back(tr("Special color calculation bit"), fmt::format("{:#x}", screen.special_color_calculation));
        values.emplace_back(tr("Supplementary palette number bit"), fmt::format("{:#x}", screen.supplementary_palette_number));
        values.emplace_back(tr("Supplementary character number bit"),
                            fmt::format("{:#x}", screen.supplementary_character_number));
    }

    // Character Pattern size
    const auto& cp_size = screen.character_pattern_size == CharacterSize::one_by_one ? tr("1x1 cell") : tr("2x2 cells");
    values.emplace_back(tr("Character Pattern size"), cp_size);

    // Character Pattern color number
    values.emplace_back(tr("Character color number"), colorNumber(screen.character_color_number));

    // Color RAM offset
    values.emplace_back(tr("Color RAM address offset"), fmt::format("{:#x}", screen.color_ram_address_offset));

    // Cell size
    values.emplace_back(tr("Cell size"), tr("8x8 dots"));

    // Priority
    values.emplace_back(tr("Priority"), fmt::format("{}", screen.priority_number));

    // Plane start address
    values.emplace_back(tr("Plane A start address"), fmt::format("{:#010x}", screen.plane_a_start_address));
    values.emplace_back(tr("Plane B start address"), fmt::format("{:#010x}", screen.plane_b_start_address));
    values.emplace_back(tr("Plane C start address"), fmt::format("{:#010x}", screen.plane_c_start_address));
    values.emplace_back(tr("Plane D start address"), fmt::format("{:#010x}", screen.plane_d_start_address));
    const auto rotation_screens = std::array{ScrollScreen::rbg0, ScrollScreen::rbg1};
    if (std::any_of(rotation_screens.begin(), rotation_screens.end(), [&s](const ScrollScreen rss) { return rss == s; })) {
        values.emplace_back(tr("Plane E start address"), fmt::format("{:#010x}", screen.plane_e_start_address));
        values.emplace_back(tr("Plane F start address"), fmt::format("{:#010x}", screen.plane_f_start_address));
        values.emplace_back(tr("Plane G start address"), fmt::format("{:#010x}", screen.plane_g_start_address));
        values.emplace_back(tr("Plane H start address"), fmt::format("{:#010x}", screen.plane_h_start_address));
        values.emplace_back(tr("Plane I start address"), fmt::format("{:#010x}", screen.plane_i_start_address));
        values.emplace_back(tr("Plane J start address"), fmt::format("{:#010x}", screen.plane_j_start_address));
        values.emplace_back(tr("Plane K start address"), fmt::format("{:#010x}", screen.plane_k_start_address));
        values.emplace_back(tr("Plane L start address"), fmt::format("{:#010x}", screen.plane_l_start_address));
        values.emplace_back(tr("Plane M start address"), fmt::format("{:#010x}", screen.plane_m_start_address));
        values.emplace_back(tr("Plane N start address"), fmt::format("{:#010x}", screen.plane_n_start_address));
        values.emplace_back(tr("Plane O start address"), fmt::format("{:#010x}", screen.plane_o_start_address));
        values.emplace_back(tr("Plane P start address"), fmt::format("{:#010x}", screen.plane_p_start_address));
    }

    // Scrolling
    switch (s) {
        case ScrollScreen::nbg0:
        case ScrollScreen::nbg1:
            values.emplace_back(tr("Screen scroll"),
                                fmt::format("x = {}.{}, y = {}.{}",
                                            screen.screen_scroll_horizontal_integer,
                                            screen.screen_scroll_horizontal_fractional,
                                            screen.screen_scroll_vertical_integer,
                                            screen.screen_scroll_vertical_fractional));
            break;
        case ScrollScreen::nbg2:
        case ScrollScreen::nbg3:
            values.emplace_back(
                tr("Screen scroll"),
                fmt::format("x = {}, y = {}", screen.screen_scroll_horizontal_integer, screen.screen_scroll_vertical_integer));
            break;
        default: break;
    }
    return values;
}

//--------------------------------------------------------------------------------------------------------------
// PRIVATE section
//--------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------
// MEMORY ACCESS methods
//--------------------------------------------------------------------------------------------------------------

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto Vdp2::read16(const u32 addr) const -> u16 {
    switch (addr) {
        case tv_screen_mode: return tvmd_.toU16();
        case external_signal_enable: return exten_.toU16();
        case screen_status: return tvstat_.toU16();
        case vram_size: return vrsize_.toU16();
        case h_counter: return hcnt_.toU16();
        case v_counter: return vcnt_.toU16();
        case reserve_1: return rsv1_.toU16();
        case ram_control: return ramctl_.toU16();
        case vram_cycle_pattern_bank_a0_lower: return cyca0l_.toU16();
        case vram_cycle_pattern_bank_a0_upper: return cyca0u_.toU16();
        case vram_cycle_pattern_bank_a1_lower: return cyca1l_.toU16();
        case vram_cycle_pattern_bank_a1_upper: return cyca1u_.toU16();
        case vram_cycle_pattern_bank_b0_lower: return cycb0l_.toU16();
        case vram_cycle_pattern_bank_b0_upper: return cycb0u_.toU16();
        case vram_cycle_pattern_bank_b1_lower: return cycb1l_.toU16();
        case vram_cycle_pattern_bank_b1_upper: return cycb1u_.toU16();
        case screen_display_enable: return bgon_.toU16();
        case mosaic_control: return mzctl_.toU16();
        case special_function_code_select: return sfsel_.toU16();
        case special_function_code: return sfcode_.toU16();
        case character_control_a: return chctla_.toU16();
        case character_control_b: return chctlb_.toU16();
        case bitmap_palette_number_a: return bmpna_.toU16();
        case bitmap_palette_number_b: return bmpnb_.toU16();
        case pattern_name_control_nbg0: return pncn0_.toU16();
        case pattern_name_control_nbg1: return pncn1_.toU16();
        case pattern_name_control_nbg2: return pncn2_.toU16();
        case pattern_name_control_nbg3: return pncn3_.toU16();
        case pattern_name_control_rbg0: return pncr_.toU16();
        case plane_size: return plsz_.toU16();
        case map_offset_n: return mpofn_.toU16();
        case map_offset_r: return mpofr_.toU16();
        case map_nbg0_plane_a_b: return mpabn0_.toU16();
        case map_nbg0_plane_c_d: return mpcdn0_.toU16();
        case map_nbg1_plane_a_b: return mpabn1_.toU16();
        case map_nbg1_plane_c_d: return mpcdn1_.toU16();
        case map_nbg2_plane_a_b: return mpabn2_.toU16();
        case map_nbg2_plane_c_d: return mpcdn2_.toU16();
        case map_nbg3_plane_a_b: return mpabn3_.toU16();
        case map_nbg3_plane_c_d: return mpcdn3_.toU16();
        case map_rotation_parameter_a_plane_a_b: return mpabra_.toU16();
        case map_rotation_parameter_a_plane_c_d: return mpcdra_.toU16();
        case map_rotation_parameter_a_plane_e_f: return mpefra_.toU16();
        case map_rotation_parameter_a_plane_g_h: return mpghra_.toU16();
        case map_rotation_parameter_a_plane_i_j: return mpijra_.toU16();
        case map_rotation_parameter_a_plane_k_l: return mpklra_.toU16();
        case map_rotation_parameter_a_plane_m_n: return mpmnra_.toU16();
        case map_rotation_parameter_a_plane_o_p: return mpopra_.toU16();
        case map_rotation_parameter_b_plane_a_b: return mpabrb_.toU16();
        case map_rotation_parameter_b_plane_c_d: return mpcdrb_.toU16();
        case map_rotation_parameter_b_plane_e_f: return mpefrb_.toU16();
        case map_rotation_parameter_b_plane_g_h: return mpghrb_.toU16();
        case map_rotation_parameter_b_plane_i_j: return mpijrb_.toU16();
        case map_rotation_parameter_b_plane_k_l: return mpklrb_.toU16();
        case map_rotation_parameter_b_plane_m_n: return mpmnrb_.toU16();
        case map_rotation_parameter_b_plane_o_p: return mpoprb_.toU16();
        case screen_scroll_value_nbg0_h_int_part: return scxin0_.toU16();
        case screen_scroll_value_nbg0_h_fract_part: return scxdn0_.toU16();
        case screen_scroll_value_nbg0_v_int_part: return scyin0_.toU16();
        case screen_scroll_value_nbg0_v_fract_part: return scydn0_.toU16();
        case coordinate_increment_nbg0_h_int_part: return zmxin0_.toU16();
        case coordinate_increment_nbg0_h_fract_part: return zmxdn0_.toU16();
        case coordinate_increment_nbg0_v_int_part: return zmyin0_.toU16();
        case coordinate_increment_nbg0_v_fract_part: return zmydn0_.toU16();
        case screen_scroll_value_nbg1_h_int_part: return scxin1_.toU16();
        case screen_scroll_value_nbg1_h_fract_part: return scxdn1_.toU16();
        case screen_scroll_value_nbg1_v_int_part: return scyin1_.toU16();
        case screen_scroll_value_nbg1_v_fract_part: return scydn1_.toU16();
        case coordinate_increment_nbg1_h_int_part: return zmxin1_.toU16();
        case coordinate_increment_nbg1_h_fract_part: return zmxdn1_.toU16();
        case coordinate_increment_nbg1_v_int_part: return zmyin1_.toU16();
        case coordinate_increment_nbg1_v_fract_part: return zmydn1_.toU16();
        case screen_scroll_value_nbg2_h: return scxn2_.toU16();
        case screen_scroll_value_nbg2_v: return scyn2_.toU16();
        case screen_scroll_value_nbg3_h: return scxn3_.toU16();
        case screen_scroll_value_nbg3_v: return scyn3_.toU16();
        case reduction_enable: return zmctl_.toU16();
        case line_and_vertical_cell_scroll_control: return scrctl_.toU16();
        case vertical_cell_scroll_table_address_upper: return vcstau_.toU16();
        case vertical_cell_scroll_table_address_lower: return vcstal_.toU16();
        case line_scroll_table_address_nbg0_upper: return lsta0u_.toU16();
        case line_scroll_table_address_nbg0_lower: return lsta0l_.toU16();
        case line_scroll_table_address_nbg1_upper: return lsta1u_.toU16();
        case line_scroll_table_address_nbg1_lower: return lsta1l_.toU16();
        case line_color_screen_table_address_upper: return lctau_.toU16();
        case line_color_screen_table_address_lower: return lctal_.toU16();
        case back_screen_table_address_upper: return bktau_.toU16();
        case back_screen_table_address_lower: return bktal_.toU16();
        case rotation_parameter_mode: return rpmd_.toU16();
        case rotation_parameter_read_control: return rprctl_.toU16();
        case coefficient_table_control: return ktctl_.toU16();
        case coefficient_table_address_offset: return ktaof_.toU16();
        case screen_over_pattern_name_a: return ovpnra_.toU16();
        case screen_over_pattern_name_b: return ovpnrb_.toU16();
        case rotation_parameter_table_address_upper: return rptau_.toU16();
        case rotation_parameter_table_address_lower: return rptal_.toU16();
        case window_position_w0_h_start_point: return wpsx0_.toU16();
        case window_position_w0_v_start_point: return wpsy0_.toU16();
        case window_position_w0_h_end_point: return wpex0_.toU16();
        case window_position_w0_v_end_point: return wpey0_.toU16();
        case window_position_w1_h_start_point: return wpsx1_.toU16();
        case window_position_w1_v_start_point: return wpsy1_.toU16();
        case window_position_w1_h_end_point: return wpex1_.toU16();
        case window_position_w1_v_end_point: return wpey1_.toU16();
        case window_control_a: return wctla_.toU16();
        case window_control_b: return wctlb_.toU16();
        case window_control_c: return wctlc_.toU16();
        case window_control_d: return wctld_.toU16();
        case line_window_table_address_w0_upper: return lwta0u_.toU16();
        case line_window_table_address_w0_lower: return lwta0l_.toU16();
        case line_window_table_address_w1_upper: return lwta1u_.toU16();
        case line_window_table_address_w1_lower: return lwta1l_.toU16();
        case sprite_control: return spctl_.toU16();
        case shadow_control: return sdctl_.toU16();
        case color_ram_address_offset_a: return craofa_.toU16();
        case color_ram_address_offset_b: return craofb_.toU16();
        case line_color_screen_enable: return lnclen_.toU16();
        case special_priority_mode: return sfprmd_.toU16();
        case color_calculation_control: return ccctl_.toU16();
        case special_color_calculation_mode: return sfccmd_.toU16();
        case priority_number_sprite_0_1: return prisa_.toU16();
        case priority_number_sprite_2_3: return prisb_.toU16();
        case priority_number_sprite_4_5: return prisc_.toU16();
        case priority_number_sprite_6_7: return prisd_.toU16();
        case priority_number_nbg0_nbg1: return prina_.toU16();
        case priority_number_nbg2_nbg3: return prinb_.toU16();
        case priority_number_rbg0: return prir_.toU16();
        case reserve_2: return rsv2_.toU16();
        case color_calculation_ratio_sprite_0_1: return ccrsa_.toU16();
        case color_calculation_ratio_sprite_2_3: return ccrsb_.toU16();
        case color_calculation_ratio_sprite_4_5: return ccrsc_.toU16();
        case color_calculation_ratio_sprite_6_7: return ccrsd_.toU16();
        case color_calculation_ratio_nbg0_nbg1: return ccrna_.toU16();
        case color_calculation_ratio_nbg2_nbg3: return ccrnb_.toU16();
        case color_calculation_ratio_rbg0: return ccrr_.toU16();
        case color_calculation_ratio_line_color_back: return ccrlb_.toU16();
        case color_offset_enable: return clofen_.toU16();
        case color_offset_select: return clofsl_.toU16();
        case color_offset_a_red: return coar_.toU16();
        case color_offset_a_green: return coag_.toU16();
        case color_offset_a_blue: return coab_.toU16();
        case color_offset_b_red: return cobr_.toU16();
        case color_offset_b_green: return cobg_.toU16();
        case color_offset_b_blue: return cobb_.toU16();

        default: core::Log::warning(Logger::vdp2, core::tr("Unimplemented register read (16) {:#010x}"), addr);
    }

    return 0;
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void Vdp2::write16(const u32 addr, const u16 data) {
    switch (addr) {
        case tv_screen_mode: tvmd_.set(bits_0_15, data); break;
        case external_signal_enable: exten_.set(bits_0_15, data); break;
        case vram_size: vrsize_.set(bits_0_15, data); break;
        case h_counter: hcnt_.set(bits_0_15, data); break;
        case v_counter: vcnt_.set(bits_0_15, data); break;
        case reserve_1: rsv1_.set(bits_0_15, data); break;
        case ram_control: ramctl_.set(bits_0_15, data); break;
        case vram_cycle_pattern_bank_a0_lower: cyca0l_.set(bits_0_15, data); break;
        case vram_cycle_pattern_bank_a0_upper: cyca0u_.set(bits_0_15, data); break;
        case vram_cycle_pattern_bank_a1_lower: cyca1l_.set(bits_0_15, data); break;
        case vram_cycle_pattern_bank_a1_upper: cyca1u_.set(bits_0_15, data); break;
        case vram_cycle_pattern_bank_b0_lower: cycb0l_.set(bits_0_15, data); break;
        case vram_cycle_pattern_bank_b0_upper: cycb0u_.set(bits_0_15, data); break;
        case vram_cycle_pattern_bank_b1_lower: cycb1l_.set(bits_0_15, data); break;
        case vram_cycle_pattern_bank_b1_upper: cycb1u_.set(bits_0_15, data); break;
        case screen_display_enable: bgon_.set(bits_0_15, data); break;
        case mosaic_control: mzctl_.set(bits_0_15, data); break;
        case special_function_code_select: sfsel_.set(bits_0_15, data); break;
        case special_function_code: sfcode_.set(bits_0_15, data); break;
        case character_control_a: chctla_.set(bits_0_15, data); break;
        case character_control_b: chctlb_.set(bits_0_15, data); break;
        case bitmap_palette_number_a: bmpna_.set(bits_0_15, data); break;
        case bitmap_palette_number_b: bmpnb_.set(bits_0_15, data); break;
        case pattern_name_control_nbg0: pncn0_.set(bits_0_15, data); break;
        case pattern_name_control_nbg1: pncn1_.set(bits_0_15, data); break;
        case pattern_name_control_nbg2: pncn2_.set(bits_0_15, data); break;
        case pattern_name_control_nbg3: pncn3_.set(bits_0_15, data); break;
        case pattern_name_control_rbg0: pncr_.set(bits_0_15, data); break;
        case plane_size: plsz_.set(bits_0_15, data); break;
        case map_offset_n: mpofn_.set(bits_0_15, data); break;
        case map_offset_r: mpofr_.set(bits_0_15, data); break;
        case map_nbg0_plane_a_b: mpabn0_.set(bits_0_15, data); break;
        case map_nbg0_plane_c_d: mpcdn0_.set(bits_0_15, data); break;
        case map_nbg1_plane_a_b: mpabn1_.set(bits_0_15, data); break;
        case map_nbg1_plane_c_d: mpcdn1_.set(bits_0_15, data); break;
        case map_nbg2_plane_a_b: mpabn2_.set(bits_0_15, data); break;
        case map_nbg2_plane_c_d: mpcdn2_.set(bits_0_15, data); break;
        case map_nbg3_plane_a_b: mpabn3_.set(bits_0_15, data); break;
        case map_nbg3_plane_c_d: mpcdn3_.set(bits_0_15, data); break;
        case map_rotation_parameter_a_plane_a_b: mpabra_.set(bits_0_15, data); break;
        case map_rotation_parameter_a_plane_c_d: mpcdra_.set(bits_0_15, data); break;
        case map_rotation_parameter_a_plane_e_f: mpefra_.set(bits_0_15, data); break;
        case map_rotation_parameter_a_plane_g_h: mpghra_.set(bits_0_15, data); break;
        case map_rotation_parameter_a_plane_i_j: mpijra_.set(bits_0_15, data); break;
        case map_rotation_parameter_a_plane_k_l: mpklra_.set(bits_0_15, data); break;
        case map_rotation_parameter_a_plane_m_n: mpmnra_.set(bits_0_15, data); break;
        case map_rotation_parameter_a_plane_o_p: mpopra_.set(bits_0_15, data); break;
        case map_rotation_parameter_b_plane_a_b: mpabrb_.set(bits_0_15, data); break;
        case map_rotation_parameter_b_plane_c_d: mpcdrb_.set(bits_0_15, data); break;
        case map_rotation_parameter_b_plane_e_f: mpefrb_.set(bits_0_15, data); break;
        case map_rotation_parameter_b_plane_g_h: mpghrb_.set(bits_0_15, data); break;
        case map_rotation_parameter_b_plane_i_j: mpijrb_.set(bits_0_15, data); break;
        case map_rotation_parameter_b_plane_k_l: mpklrb_.set(bits_0_15, data); break;
        case map_rotation_parameter_b_plane_m_n: mpmnrb_.set(bits_0_15, data); break;
        case map_rotation_parameter_b_plane_o_p: mpoprb_.set(bits_0_15, data); break;
        case screen_scroll_value_nbg0_h_int_part: scxin0_.set(bits_0_15, data); break;
        case screen_scroll_value_nbg0_h_fract_part: scxdn0_.set(bits_0_15, data); break;
        case screen_scroll_value_nbg0_v_int_part: scyin0_.set(bits_0_15, data); break;
        case screen_scroll_value_nbg0_v_fract_part: scydn0_.set(bits_0_15, data); break;
        case coordinate_increment_nbg0_h_int_part: zmxin0_.set(bits_0_15, data); break;
        case coordinate_increment_nbg0_h_fract_part: zmxdn0_.set(bits_0_15, data); break;
        case coordinate_increment_nbg0_v_int_part: zmyin0_.set(bits_0_15, data); break;
        case coordinate_increment_nbg0_v_fract_part: zmydn0_.set(bits_0_15, data); break;
        case screen_scroll_value_nbg1_h_int_part: scxin1_.set(bits_0_15, data); break;
        case screen_scroll_value_nbg1_h_fract_part: scxdn1_.set(bits_0_15, data); break;
        case screen_scroll_value_nbg1_v_int_part: scyin1_.set(bits_0_15, data); break;
        case screen_scroll_value_nbg1_v_fract_part: scydn1_.set(bits_0_15, data); break;
        case coordinate_increment_nbg1_h_int_part: zmxin1_.set(bits_0_15, data); break;
        case coordinate_increment_nbg1_h_fract_part: zmxdn1_.set(bits_0_15, data); break;
        case coordinate_increment_nbg1_v_int_part: zmyin1_.set(bits_0_15, data); break;
        case coordinate_increment_nbg1_v_fract_part: zmydn1_.set(bits_0_15, data); break;
        case screen_scroll_value_nbg2_h: scxn2_.set(bits_0_15, data); break;
        case screen_scroll_value_nbg2_v: scyn2_.set(bits_0_15, data); break;
        case screen_scroll_value_nbg3_h: scxn3_.set(bits_0_15, data); break;
        case screen_scroll_value_nbg3_v: scyn3_.set(bits_0_15, data); break;
        case reduction_enable: zmctl_.set(bits_0_15, data); break;
        case line_and_vertical_cell_scroll_control: scrctl_.set(bits_0_15, data); break;
        case vertical_cell_scroll_table_address_upper: vcstau_.set(bits_0_15, data); break;
        case vertical_cell_scroll_table_address_lower: vcstal_.set(bits_0_15, data); break;
        case line_scroll_table_address_nbg0_upper: lsta0u_.set(bits_0_15, data); break;
        case line_scroll_table_address_nbg0_lower: lsta0l_.set(bits_0_15, data); break;
        case line_scroll_table_address_nbg1_upper: lsta1u_.set(bits_0_15, data); break;
        case line_scroll_table_address_nbg1_lower: lsta1l_.set(bits_0_15, data); break;
        case line_color_screen_table_address_upper: lctau_.set(bits_0_15, data); break;
        case line_color_screen_table_address_lower: lctal_.set(bits_0_15, data); break;
        case back_screen_table_address_upper: bktau_.set(bits_0_15, data); break;
        case back_screen_table_address_lower: bktal_.set(bits_0_15, data); break;
        case rotation_parameter_mode: rpmd_.set(bits_0_15, data); break;
        case rotation_parameter_read_control: rprctl_.set(bits_0_15, data); break;
        case coefficient_table_control: ktctl_.set(bits_0_15, data); break;
        case coefficient_table_address_offset: ktaof_.set(bits_0_15, data); break;
        case screen_over_pattern_name_a: ovpnra_.set(bits_0_15, data); break;
        case screen_over_pattern_name_b: ovpnrb_.set(bits_0_15, data); break;
        case rotation_parameter_table_address_upper: rptau_.set(bits_0_15, data); break;
        case rotation_parameter_table_address_lower: rptal_.set(bits_0_15, data); break;
        case window_position_w0_h_start_point: wpsx0_.set(bits_0_15, data); break;
        case window_position_w0_v_start_point: wpsy0_.set(bits_0_15, data); break;
        case window_position_w0_h_end_point: wpex0_.set(bits_0_15, data); break;
        case window_position_w0_v_end_point: wpey0_.set(bits_0_15, data); break;
        case window_position_w1_h_start_point: wpsx1_.set(bits_0_15, data); break;
        case window_position_w1_v_start_point: wpsy1_.set(bits_0_15, data); break;
        case window_position_w1_h_end_point: wpex1_.set(bits_0_15, data); break;
        case window_position_w1_v_end_point: wpey1_.set(bits_0_15, data); break;
        case window_control_a: wctla_.set(bits_0_15, data); break;
        case window_control_b: wctlb_.set(bits_0_15, data); break;
        case window_control_c: wctlc_.set(bits_0_15, data); break;
        case window_control_d: wctld_.set(bits_0_15, data); break;
        case line_window_table_address_w0_upper: lwta0u_.set(bits_0_15, data); break;
        case line_window_table_address_w0_lower: lwta0l_.set(bits_0_15, data); break;
        case line_window_table_address_w1_upper: lwta1u_.set(bits_0_15, data); break;
        case line_window_table_address_w1_lower: lwta1l_.set(bits_0_15, data); break;
        case sprite_control: spctl_.set(bits_0_15, data); break;
        case shadow_control: sdctl_.set(bits_0_15, data); break;
        case color_ram_address_offset_a: craofa_.set(bits_0_15, data); break;
        case color_ram_address_offset_b: craofb_.set(bits_0_15, data); break;
        case line_color_screen_enable: lnclen_.set(bits_0_15, data); break;
        case special_priority_mode: sfprmd_.set(bits_0_15, data); break;
        case color_calculation_control: ccctl_.set(bits_0_15, data); break;
        case special_color_calculation_mode: sfccmd_.set(bits_0_15, data); break;
        case priority_number_sprite_0_1: prisa_.set(bits_0_15, data); break;
        case priority_number_sprite_2_3: prisb_.set(bits_0_15, data); break;
        case priority_number_sprite_4_5: prisc_.set(bits_0_15, data); break;
        case priority_number_sprite_6_7: prisd_.set(bits_0_15, data); break;
        case priority_number_nbg0_nbg1: prina_.set(bits_0_15, data); break;
        case priority_number_nbg2_nbg3: prinb_.set(bits_0_15, data); break;
        case priority_number_rbg0: prir_.set(bits_0_15, data); break;
        case reserve_2: rsv2_.set(bits_0_15, data); break;
        case color_calculation_ratio_sprite_0_1: ccrsa_.set(bits_0_15, data); break;
        case color_calculation_ratio_sprite_2_3: ccrsb_.set(bits_0_15, data); break;
        case color_calculation_ratio_sprite_4_5: ccrsc_.set(bits_0_15, data); break;
        case color_calculation_ratio_sprite_6_7: ccrsd_.set(bits_0_15, data); break;
        case color_calculation_ratio_nbg0_nbg1: ccrna_.set(bits_0_15, data); break;
        case color_calculation_ratio_nbg2_nbg3: ccrnb_.set(bits_0_15, data); break;
        case color_calculation_ratio_rbg0: ccrr_.set(bits_0_15, data); break;
        case color_calculation_ratio_line_color_back: ccrlb_.set(bits_0_15, data); break;
        case color_offset_enable: clofen_.set(bits_0_15, data); break;
        case color_offset_select: clofsl_.set(bits_0_15, data); break;
        case color_offset_a_red: coar_.set(bits_0_15, data); break;
        case color_offset_a_green: coag_.set(bits_0_15, data); break;
        case color_offset_a_blue: coab_.set(bits_0_15, data); break;
        case color_offset_b_red: cobr_.set(bits_0_15, data); break;
        case color_offset_b_green: cobg_.set(bits_0_15, data); break;
        case color_offset_b_blue: cobb_.set(bits_0_15, data); break;
        default: core::Log::warning(Logger::vdp2, core::tr("Unimplemented register write (16) {:#010x}"), addr);
    }
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void Vdp2::write32(const u32 addr, const u32 data) {
    const auto h = static_cast<u16>(data >> displacement_16);
    const auto l = static_cast<u16>(data & bitmask_FFFF);
    switch (addr) {
        case vram_cycle_pattern_bank_a0_lower:
            cyca0l_.set(bits_0_15, h);
            cyca0u_.set(bits_0_15, l);
            break;
        case vram_cycle_pattern_bank_a1_lower:
            cyca1l_.set(bits_0_15, h);
            cyca1u_.set(bits_0_15, l);
            break;
        case vram_cycle_pattern_bank_b0_lower:
            cycb0l_.set(bits_0_15, h);
            cycb0u_.set(bits_0_15, l);
            break;
        case vram_cycle_pattern_bank_b1_lower:
            cycb1l_.set(bits_0_15, h);
            cycb1u_.set(bits_0_15, l);
            break;
        case screen_display_enable:
            bgon_.set(bits_0_15, h);
            mzctl_.set(bits_0_15, l);
            break;
        case special_function_code_select:
            sfsel_.set(bits_0_15, h);
            sfcode_.set(bits_0_15, l);
            break;
        case character_control_a:
            chctla_.set(bits_0_15, h);
            chctlb_.set(bits_0_15, l);
            break;
        case bitmap_palette_number_a:
            bmpna_.set(bits_0_15, h);
            bmpnb_.set(bits_0_15, l);
            break;
        case pattern_name_control_nbg0:
            pncn0_.set(bits_0_15, h);
            pncn1_.set(bits_0_15, l);
            break;
        case pattern_name_control_nbg2:
            pncn2_.set(bits_0_15, h);
            pncn3_.set(bits_0_15, l);
            break;
        case pattern_name_control_rbg0:
            pncr_.set(bits_0_15, h);
            plsz_.set(bits_0_15, l);
            break;
        case map_offset_n:
            mpofn_.set(bits_0_15, h);
            mpofr_.set(bits_0_15, l);
            break;
        case map_nbg0_plane_a_b:
            mpabn0_.set(bits_0_15, h);
            mpcdn0_.set(bits_0_15, l);
            break;
        case map_nbg1_plane_a_b:
            mpabn1_.set(bits_0_15, h);
            mpcdn1_.set(bits_0_15, l);
            break;
        case map_nbg2_plane_a_b:
            mpabn2_.set(bits_0_15, h);
            mpcdn2_.set(bits_0_15, l);
            break;
        case map_nbg3_plane_a_b:
            mpabn3_.set(bits_0_15, h);
            mpcdn3_.set(bits_0_15, l);
            break;
        case map_rotation_parameter_a_plane_a_b:
            mpabra_.set(bits_0_15, h);
            mpcdra_.set(bits_0_15, l);
            break;
        case map_rotation_parameter_a_plane_e_f:
            mpefra_.set(bits_0_15, h);
            mpghra_.set(bits_0_15, l);
            break;
        case map_rotation_parameter_a_plane_i_j:
            mpijra_.set(bits_0_15, h);
            mpklra_.set(bits_0_15, l);
            break;
        case map_rotation_parameter_a_plane_m_n:
            mpmnra_.set(bits_0_15, h);
            mpopra_.set(bits_0_15, l);
            break;
        case map_rotation_parameter_b_plane_a_b:
            mpabrb_.set(bits_0_15, h);
            mpcdrb_.set(bits_0_15, l);
            break;
        case map_rotation_parameter_b_plane_e_f:
            mpefrb_.set(bits_0_15, h);
            mpghrb_.set(bits_0_15, l);
            break;
        case map_rotation_parameter_b_plane_i_j:
            mpijrb_.set(bits_0_15, h);
            mpklrb_.set(bits_0_15, l);
            break;
        case map_rotation_parameter_b_plane_m_n:
            mpmnrb_.set(bits_0_15, h);
            mpoprb_.set(bits_0_15, l);
            break;
        case screen_scroll_value_nbg0_h_int_part:
            scxin0_.set(bits_0_15, h);
            scxdn0_.set(bits_0_15, l);
            break;
        case screen_scroll_value_nbg0_v_int_part:
            scyin0_.set(bits_0_15, h);
            scydn0_.set(bits_0_15, l);
            break;
        case coordinate_increment_nbg0_h_int_part:
            zmxin0_.set(bits_0_15, h);
            zmxdn0_.set(bits_0_15, l);
            break;
        case coordinate_increment_nbg0_v_int_part:
            zmyin0_.set(bits_0_15, h);
            zmydn0_.set(bits_0_15, l);
            break;
        case screen_scroll_value_nbg1_h_int_part:
            scxin1_.set(bits_0_15, h);
            scxdn1_.set(bits_0_15, l);
            break;
        case screen_scroll_value_nbg1_v_int_part:
            scyin1_.set(bits_0_15, h);
            scydn1_.set(bits_0_15, l);
            break;
        case coordinate_increment_nbg1_h_int_part:
            zmxin1_.set(bits_0_15, h);
            zmxdn1_.set(bits_0_15, l);
            break;
        case coordinate_increment_nbg1_v_int_part:
            zmyin1_.set(bits_0_15, h);
            zmydn1_.set(bits_0_15, l);
            break;
        case screen_scroll_value_nbg2_h:
            scxn2_.set(bits_0_15, h);
            scyn2_.set(bits_0_15, l);
            break;
        case screen_scroll_value_nbg3_h:
            scxn3_.set(bits_0_15, h);
            scyn3_.set(bits_0_15, l);
            break;
        case reduction_enable:
            zmctl_.set(bits_0_15, h);
            scrctl_.set(bits_0_15, l);
            break;
        case vertical_cell_scroll_table_address_upper:
            vcstau_.set(bits_0_15, h);
            vcstal_.set(bits_0_15, l);
            break;
        case line_scroll_table_address_nbg0_upper:
            lsta0u_.set(bits_0_15, h);
            lsta0l_.set(bits_0_15, l);
            break;
        case line_scroll_table_address_nbg1_upper:
            lsta1u_.set(bits_0_15, h);
            lsta1l_.set(bits_0_15, l);
            break;
        case line_color_screen_table_address_upper:
            lctau_.set(bits_0_15, h);
            lctal_.set(bits_0_15, l);
            break;
        case back_screen_table_address_upper:
            bktau_.set(bits_0_15, h);
            bktal_.set(bits_0_15, l);
            break;
        case rotation_parameter_mode:
            rpmd_.set(bits_0_15, h);
            rprctl_.set(bits_0_15, l);
            break;
        case coefficient_table_control:
            ktctl_.set(bits_0_15, h);
            ktaof_.set(bits_0_15, l);
            break;
        case screen_over_pattern_name_a:
            ovpnra_.set(bits_0_15, h);
            ovpnrb_.set(bits_0_15, l);
            break;
        case rotation_parameter_table_address_upper:
            rptau_.set(bits_0_15, h);
            rptal_.set(bits_0_15, l);
            break;
        case window_position_w0_h_start_point:
            wpsx0_.set(bits_0_15, h);
            wpsy0_.set(bits_0_15, l);
            break;
        case window_position_w0_h_end_point:
            wpex0_.set(bits_0_15, h);
            wpey0_.set(bits_0_15, l);
            break;
        case window_position_w1_h_start_point:
            wpsx1_.set(bits_0_15, h);
            wpsy1_.set(bits_0_15, l);
            break;
        case window_position_w1_h_end_point:
            wpex1_.set(bits_0_15, h);
            wpey1_.set(bits_0_15, l);
            break;
        case window_control_a:
            wctla_.set(bits_0_15, h);
            wctlb_.set(bits_0_15, l);
            break;
        case window_control_c:
            wctlc_.set(bits_0_15, h);
            wctld_.set(bits_0_15, l);
            break;
        case line_window_table_address_w0_upper:
            lwta0u_.set(bits_0_15, h);
            lwta0l_.set(bits_0_15, l);
            break;
        case line_window_table_address_w1_upper:
            lwta1u_.set(bits_0_15, h);
            lwta1l_.set(bits_0_15, l);
            break;
        case sprite_control:
            spctl_.set(bits_0_15, h);
            sdctl_.set(bits_0_15, l);
            break;
        case color_ram_address_offset_a:
            craofa_.set(bits_0_15, h);
            craofb_.set(bits_0_15, l);
            break;
        case line_color_screen_enable:
            lnclen_.set(bits_0_15, h);
            sfprmd_.set(bits_0_15, l);
            break;
        case color_calculation_control:
            ccctl_.set(bits_0_15, h);
            sfccmd_.set(bits_0_15, l);
            break;
        case priority_number_sprite_0_1:
            prisa_.set(bits_0_15, h);
            prisb_.set(bits_0_15, l);
            break;
        case priority_number_sprite_4_5:
            prisc_.set(bits_0_15, h);
            prisd_.set(bits_0_15, l);
            break;
        case priority_number_nbg0_nbg1:
            prina_.set(bits_0_15, h);
            prinb_.set(bits_0_15, l);
            break;
        case priority_number_rbg0:
            prir_.set(bits_0_15, h);
            rsv2_.set(bits_0_15, l);
            break;
        case color_calculation_ratio_sprite_0_1:
            ccrsa_.set(bits_0_15, h);
            ccrsb_.set(bits_0_15, l);
            break;
        case color_calculation_ratio_sprite_4_5:
            ccrsc_.set(bits_0_15, h);
            ccrsd_.set(bits_0_15, l);
            break;
        case color_calculation_ratio_nbg0_nbg1:
            ccrna_.set(bits_0_15, h);
            ccrnb_.set(bits_0_15, l);
            break;
        case color_calculation_ratio_rbg0:
            ccrr_.set(bits_0_15, h);
            ccrlb_.set(bits_0_15, l);
            break;
        case color_offset_enable:
            clofen_.set(bits_0_15, h);
            clofsl_.set(bits_0_15, l);
            break;
        case color_offset_a_red:
            coar_.set(bits_0_15, h);
            coag_.set(bits_0_15, l);
            break;
        case color_offset_a_blue:
            coab_.set(bits_0_15, h);
            cobr_.set(bits_0_15, l);
            break;
        case color_offset_b_green:
            cobg_.set(bits_0_15, h);
            cobb_.set(bits_0_15, l);
            break;
        default: core::Log::warning(Logger::vdp2, core::tr("Unimplemented register write (16) {:#010x}"), addr);
    }
}

//--------------------------------------------------------------------------------------------------------------
// MISC methods
//--------------------------------------------------------------------------------------------------------------

void Vdp2::reset() {}

void Vdp2::addToRegisterNameMap(const u32 addr, const std::string& name) {
    address_to_name_.insert(AddressToNameMap::value_type(addr, name));
}

void Vdp2::initializeRegisterNameMap() {
    addToRegisterNameMap(tv_screen_mode, "TV Screen Mode");
    addToRegisterNameMap(external_signal_enable, "External Signal Enable");
    addToRegisterNameMap(screen_status, "Screen Status");
    addToRegisterNameMap(vram_size, "VRAM Size");
    addToRegisterNameMap(h_counter, "H-Counter");
    addToRegisterNameMap(v_counter, "V-Counter");
    addToRegisterNameMap(ram_control, "RAM Control");
    addToRegisterNameMap(vram_cycle_pattern_bank_a0_lower, "VRAM Cycle Pattern (Bank A0) lower");
    addToRegisterNameMap(vram_cycle_pattern_bank_a0_upper, "VRAM Cycle Pattern (Bank A0) upper");
    addToRegisterNameMap(vram_cycle_pattern_bank_a1_lower, "VRAM Cycle Pattern (Bank A1) lower");
    addToRegisterNameMap(vram_cycle_pattern_bank_a1_upper, "VRAM Cycle Pattern (Bank A1) upper");
    addToRegisterNameMap(vram_cycle_pattern_bank_b0_lower, "VRAM Cycle Pattern (Bank B0) lower");
    addToRegisterNameMap(vram_cycle_pattern_bank_b0_upper, "VRAM Cycle Pattern (Bank B0) upper");
    addToRegisterNameMap(vram_cycle_pattern_bank_b1_lower, "VRAM Cycle Pattern (Bank B1) lower");
    addToRegisterNameMap(vram_cycle_pattern_bank_b1_upper, "VRAM Cycle Pattern (Bank B1) upper");
    addToRegisterNameMap(screen_display_enable, "Screen Display Enable");
    addToRegisterNameMap(mosaic_control, "Mosaic Control");
    addToRegisterNameMap(special_function_code_select, "Special Function Code Select");
    addToRegisterNameMap(special_function_code, "Special Function Code");
    addToRegisterNameMap(character_control_a, "Character Control (NBG0, NBG1)");
    addToRegisterNameMap(character_control_b, "Character Control (NBG2, NBG3, RBG0)");
    addToRegisterNameMap(bitmap_palette_number_a, "Bitmap Palette Number (NBG0, NBG1)");
    addToRegisterNameMap(bitmap_palette_number_b, "Bitmap Palette Number (RBG0)");
    addToRegisterNameMap(pattern_name_control_nbg0, "Pattern Name Control (NBG0)");
    addToRegisterNameMap(pattern_name_control_nbg1, "Pattern Name Control (NBG1)");
    addToRegisterNameMap(pattern_name_control_nbg2, "Pattern Name Control (NBG2)");
    addToRegisterNameMap(pattern_name_control_nbg3, "Pattern Name Control (NBG3)");
    addToRegisterNameMap(pattern_name_control_rbg0, "Pattern Name Control (RBG0)");
    addToRegisterNameMap(plane_size, "Plane Size");
    addToRegisterNameMap(map_offset_n, "Map Offset (NBG0 - NBG3)");
    addToRegisterNameMap(map_offset_r, "Map Offset (Rotation Parameter A,B)");
    addToRegisterNameMap(map_nbg0_plane_a_b, "Map (NBG0, Plane A,B)");
    addToRegisterNameMap(map_nbg0_plane_c_d, "Map (NBG0, Plane C,D)");
    addToRegisterNameMap(map_nbg1_plane_a_b, "Map (NBG1, Plane A,B)");
    addToRegisterNameMap(map_nbg1_plane_c_d, "Map (NBG1, Plane C,D)");
    addToRegisterNameMap(map_nbg2_plane_a_b, "Map (NBG2, Plane A,B)");
    addToRegisterNameMap(map_nbg2_plane_c_d, "Map (NBG2, Plane C,D)");
    addToRegisterNameMap(map_nbg3_plane_a_b, "Map (NBG3, Plane A,B)");
    addToRegisterNameMap(map_nbg3_plane_c_d, "Map (NBG3, Plane C,D)");
    addToRegisterNameMap(map_rotation_parameter_a_plane_a_b, "Map (Rotation Parameter A, Plane A,B)");
    addToRegisterNameMap(map_rotation_parameter_a_plane_c_d, "Map (Rotation Parameter A, Plane C,D)");
    addToRegisterNameMap(map_rotation_parameter_a_plane_e_f, "Map (Rotation Parameter A, Plane E,F)");
    addToRegisterNameMap(map_rotation_parameter_a_plane_g_h, "Map (Rotation Parameter A, Plane G,H)");
    addToRegisterNameMap(map_rotation_parameter_a_plane_i_j, "Map (Rotation Parameter A, Plane I,J)");
    addToRegisterNameMap(map_rotation_parameter_a_plane_k_l, "Map (Rotation Parameter A, Plane K,L)");
    addToRegisterNameMap(map_rotation_parameter_a_plane_m_n, "Map (Rotation Parameter A, Plane M,N)");
    addToRegisterNameMap(map_rotation_parameter_a_plane_o_p, "Map (Rotation Parameter A, Plane O,P)");
    addToRegisterNameMap(map_rotation_parameter_b_plane_a_b, "Map (Rotation Parameter B, Plane A,B)");
    addToRegisterNameMap(map_rotation_parameter_b_plane_c_d, "Map (Rotation Parameter B, Plane C,D)");
    addToRegisterNameMap(map_rotation_parameter_b_plane_e_f, "Map (Rotation Parameter B, Plane E,F)");
    addToRegisterNameMap(map_rotation_parameter_b_plane_g_h, "Map (Rotation Parameter B, Plane G,H)");
    addToRegisterNameMap(map_rotation_parameter_b_plane_i_j, "Map (Rotation Parameter B, Plane I,J)");
    addToRegisterNameMap(map_rotation_parameter_b_plane_k_l, "Map (Rotation Parameter B, Plane K,L)");
    addToRegisterNameMap(map_rotation_parameter_b_plane_m_n, "Map (Rotation Parameter B, Plane M,N)");
    addToRegisterNameMap(map_rotation_parameter_b_plane_o_p, "Map (Rotation Parameter B, Plane O,P)");
    addToRegisterNameMap(screen_scroll_value_nbg0_h_int_part, "Screen Scroll Value (NBG0, Horizontal Integer Part)");
    addToRegisterNameMap(screen_scroll_value_nbg0_h_fract_part, "Screen Scroll Value (NBG0, Horizontal Fractional Part)");
    addToRegisterNameMap(screen_scroll_value_nbg0_v_int_part, "Screen Scroll Value (NBG0, Vertical Integer Part)");
    addToRegisterNameMap(screen_scroll_value_nbg0_v_fract_part, "Screen Scroll Value (NBG0, Vertical Fractional Part)");
    addToRegisterNameMap(coordinate_increment_nbg0_h_int_part, "Coordinate Increment (NBG0, Horizontal Integer Part)");
    addToRegisterNameMap(coordinate_increment_nbg0_h_fract_part, "Coordinate Increment (NBG0, Horizontal Fractional Part)");
    addToRegisterNameMap(coordinate_increment_nbg0_v_int_part, "Coordinate Increment (NBG0, Vertical Integer Part)");
    addToRegisterNameMap(coordinate_increment_nbg0_v_fract_part, "Coordinate Increment (NBG0, Vertical Fractional Part)");
    addToRegisterNameMap(screen_scroll_value_nbg1_h_int_part, "Screen Scroll Value (NBG1, Horizontal Integer Part)");
    addToRegisterNameMap(screen_scroll_value_nbg1_h_fract_part, "Screen Scroll Value (NBG1, Horizontal Fractional Part)");
    addToRegisterNameMap(screen_scroll_value_nbg1_v_int_part, "Screen Scroll Value (NBG1, Vertical Integer Part)");
    addToRegisterNameMap(screen_scroll_value_nbg1_v_fract_part, "Screen Scroll Value (NBG1, Vertical Fractional Part)");
    addToRegisterNameMap(coordinate_increment_nbg1_h_int_part, "Coordinate Increment (NBG1, Horizontal Integer Part)");
    addToRegisterNameMap(coordinate_increment_nbg1_h_fract_part, "Coordinate Increment (NBG1, Horizontal Fractional Part)");
    addToRegisterNameMap(coordinate_increment_nbg1_v_int_part, "Coordinate Increment (NBG1, Vertical Integer Part)");
    addToRegisterNameMap(coordinate_increment_nbg1_v_fract_part, "Coordinate Increment (NBG1, Vertical Fractional Part)");
    addToRegisterNameMap(screen_scroll_value_nbg2_h, "Screen Scroll Value (NBG2, Horizontal)");
    addToRegisterNameMap(screen_scroll_value_nbg2_v, "Screen Scroll Value (NBG2, Vertical)");
    addToRegisterNameMap(screen_scroll_value_nbg3_h, "Screen Scroll Value (NBG3, Horizontal)");
    addToRegisterNameMap(screen_scroll_value_nbg3_v, "Screen Scroll Value (NBG3, Vertical)");
    addToRegisterNameMap(reduction_enable, "Reduction Enable)");
    addToRegisterNameMap(line_and_vertical_cell_scroll_control, "Line And Vertical Cell Scroll Control (NBG0, NBG1)");
    addToRegisterNameMap(vertical_cell_scroll_table_address_upper, "Vertical Cell Scroll Table Address Upper (NBG0, NBG1)");
    addToRegisterNameMap(vertical_cell_scroll_table_address_lower, "Vertical Cell Scroll Table Address Lower (NBG0, NBG1)");
    addToRegisterNameMap(line_scroll_table_address_nbg0_upper, "Line Scroll Table Address Upper (NBG0)");
    addToRegisterNameMap(line_scroll_table_address_nbg0_lower, "Line Scroll Table Address Lower (NBG0)");
    addToRegisterNameMap(line_scroll_table_address_nbg1_upper, "Line Scroll Table Address Upper (NBG1)");
    addToRegisterNameMap(line_scroll_table_address_nbg1_lower, "Line Scroll Table Address Lower (NBG1)");
    addToRegisterNameMap(line_color_screen_table_address_upper, "Line Color Screen Table Address Upper");
    addToRegisterNameMap(line_color_screen_table_address_lower, "Line Color Screen Table Address Lower");
    addToRegisterNameMap(back_screen_table_address_upper, "Back Screen Table Address Upper");
    addToRegisterNameMap(back_screen_table_address_lower, "Back Screen Table Address Lower");
    addToRegisterNameMap(rotation_parameter_mode, "Rotation Parameter Mode");
    addToRegisterNameMap(rotation_parameter_read_control, "Rotation Parameter Read Control");
    addToRegisterNameMap(coefficient_table_control, "Coefficient Table Control");
    addToRegisterNameMap(coefficient_table_address_offset, "Coefficient Table Address Offset (Rotation Parameter A,B)");
    addToRegisterNameMap(screen_over_pattern_name_a, "Screen Over Pattern Name (Rotation Parameter A)");
    addToRegisterNameMap(screen_over_pattern_name_b, "Screen Over Pattern Name (Rotation Parameter B)");
    addToRegisterNameMap(rotation_parameter_table_address_upper,
                         "Rotation Parameter Table Address Upper (Rotation Parameter A,B)");
    addToRegisterNameMap(rotation_parameter_table_address_lower,
                         "Rotation Parameter Table Address Lower (Rotation Parameter A,B)");
    addToRegisterNameMap(window_position_w0_h_start_point, "Window Position (W0, Horizontal Start Point)");
    addToRegisterNameMap(window_position_w0_v_start_point, "Window Position (W0, Vertical Start Point)");
    addToRegisterNameMap(window_position_w0_h_end_point, "Window Position (W0, Horizontal End Point)");
    addToRegisterNameMap(window_position_w0_v_end_point, "Window Position (W0, Vertical End Point)");
    addToRegisterNameMap(window_position_w1_h_start_point, "Window Position (W1, Horizontal Start Point)");
    addToRegisterNameMap(window_position_w1_v_start_point, "Window Position (W1, Vertical Start Point)");
    addToRegisterNameMap(window_position_w1_h_end_point, "Window Position (W1, Horizontal End Point)");
    addToRegisterNameMap(window_position_w1_v_end_point, "Window Position (W1, Vertical End Point)");
    addToRegisterNameMap(window_control_a, "Window Control (NBG0, NBG1)");
    addToRegisterNameMap(window_control_b, "Window Control (NBG2, NBG3)");
    addToRegisterNameMap(window_control_c, "Window Control (RBG0, SPRITE)");
    addToRegisterNameMap(window_control_d, "Window Control (Parameter Window, Color Calc. Window)");
    addToRegisterNameMap(line_window_table_address_w0_upper, "Line Window Table Address Upper (W0)");
    addToRegisterNameMap(line_window_table_address_w0_lower, "Line Window Table Address Lower (W0)");
    addToRegisterNameMap(line_window_table_address_w1_upper, "Line Window Table Address Upper (W1)");
    addToRegisterNameMap(line_window_table_address_w1_lower, "Line Window Table Address Lower (W1)");
    addToRegisterNameMap(sprite_control, "Sprite Control");
    addToRegisterNameMap(shadow_control, "Shadow Control");
    addToRegisterNameMap(color_ram_address_offset_a, "Color RAM Address Offset (NBG0 - NBG3)");
    addToRegisterNameMap(color_ram_address_offset_b, "Color RAM Address Offset (RBG0, SPRITE)");
    addToRegisterNameMap(line_color_screen_enable, "Line Color Screen Enable");
    addToRegisterNameMap(special_priority_mode, "Special Priority Mode");
    addToRegisterNameMap(color_calculation_control, "Color Calculation Control");
    addToRegisterNameMap(special_color_calculation_mode, "Special Color Calculation Mode");
    addToRegisterNameMap(priority_number_sprite_0_1, "Priority Number (SPRITE 0,1)");
    addToRegisterNameMap(priority_number_sprite_2_3, "Priority Number (SPRITE 2,3)");
    addToRegisterNameMap(priority_number_sprite_4_5, "Priority Number (SPRITE 4,5)");
    addToRegisterNameMap(priority_number_sprite_6_7, "Priority Number (SPRITE 6,7)");
    addToRegisterNameMap(priority_number_nbg0_nbg1, "Priority Number (NBG0, NBG1)");
    addToRegisterNameMap(priority_number_nbg2_nbg3, "Priority Number (NBG2, NBG3)");
    addToRegisterNameMap(priority_number_rbg0, "Priority Number (RBG0)");
    addToRegisterNameMap(color_calculation_ratio_sprite_0_1, "Color Calculation Ratio (SPRITE 0,1)");
    addToRegisterNameMap(color_calculation_ratio_sprite_2_3, "Color Calculation Ratio (SPRITE 2,3)");
    addToRegisterNameMap(color_calculation_ratio_sprite_4_5, "Color Calculation Ratio (SPRITE 4,5)");
    addToRegisterNameMap(color_calculation_ratio_sprite_6_7, "Color Calculation Ratio (SPRITE 6,7)");
    addToRegisterNameMap(color_calculation_ratio_nbg0_nbg1, "Color Calculation Ratio (NBG0, NBG1)");
    addToRegisterNameMap(color_calculation_ratio_nbg2_nbg3, "Color Calculation Ratio (NBG2, NBG3)");
    addToRegisterNameMap(color_calculation_ratio_rbg0, "Color Calculation Ratio (RBG0)");
    addToRegisterNameMap(color_calculation_ratio_line_color_back, "Color Calculation Ratio (Line Color Screen, Back Screen)");
    addToRegisterNameMap(color_offset_enable, "Color Offset Enable");
    addToRegisterNameMap(color_offset_select, "Color Offset Select");
    addToRegisterNameMap(color_offset_a_red, "Color Offset A (Red)");
    addToRegisterNameMap(color_offset_a_green, "Color Offset A (Green)");
    addToRegisterNameMap(color_offset_a_blue, "Color Offset A (Blue)");
    addToRegisterNameMap(color_offset_b_red, "Color Offset B (Red)");
    addToRegisterNameMap(color_offset_b_green, "Color Offset B (Green)");
    addToRegisterNameMap(color_offset_b_blue, "Color Offset B (Blue)");
}

void Vdp2::computePrecalculatedData() {
    // Modulo values used for 1 cell character pattern positioning
    constexpr auto modulo_64_number = u8{64};
    pre_calculated_modulo_64_.reserve(modulo_64_number); // the extra element allocation is harmless
    for (u32 i = 1; i < modulo_64_number; ++i) {         // value 0 isn't added
        pre_calculated_modulo_64_.emplace_back(i * modulo_64_number);
    }

    // Modulo values used for 4 cells character pattern positioning
    constexpr auto modulo_32_number = u8{32};
    pre_calculated_modulo_32_.reserve(modulo_32_number); // the extra element allocation is harmless
    for (u32 i = 1; i < modulo_32_number; ++i) {         // value 0 isn't added
        pre_calculated_modulo_32_.emplace_back(i * modulo_32_number);
    }
}

void Vdp2::calculateLineDuration(const micro& total_line_duration, const micro& hblank_duration) {
    const auto active_line_duration = micro{total_line_duration - hblank_duration};
    cycles_per_hblank_              = modules_.smpc()->calculateCyclesNumber(hblank_duration);
    cycles_per_hactive_             = modules_.smpc()->calculateCyclesNumber(active_line_duration);
    cycles_per_line_                = cycles_per_hactive_ + cycles_per_hblank_;
}

void Vdp2::updateResolution() {
    tv_screen_status_.is_picture_displayed = (tvmd_.get(TvScreenMode::display) == Display::displayed);
    tv_screen_status_.border_color_mode    = tvmd_.get(TvScreenMode::border_color_mode);
    tv_screen_status_.interlace_mode       = tvmd_.get(TvScreenMode::interlace_mode);

    switch (tvmd_.get(TvScreenMode::horizontal_resolution)) {
        case HorizontalResolution::normal_320:
            tv_screen_status_.horizontal_res   = horizontal_res_320;
            tv_screen_status_.screen_mode_type = ScreenModeType::normal;
            if (tv_screen_status_.interlace_mode == InterlaceMode::non_interlace) {
                switch (tvmd_.get(TvScreenMode::vertical_resolution)) {
                    case VerticalResolution::lines_nb_224:
                        tv_screen_status_.vertical_res = vertical_res_224;
                        tv_screen_status_.screen_mode  = ScreenMode::normal_320_224;
                        break;
                    case VerticalResolution::lines_nb_240:
                        tv_screen_status_.vertical_res = vertical_res_240;
                        tv_screen_status_.screen_mode  = ScreenMode::normal_320_240;
                        break;
                    case VerticalResolution::lines_nb_256:
                        tv_screen_status_.vertical_res = vertical_res_256;
                        tv_screen_status_.screen_mode  = ScreenMode::normal_320_256;
                        break;
                    default: tv_screen_status_.vertical_res = 0; tv_screen_status_.screen_mode = ScreenMode::not_set;
                }
            } else {
                switch (tvmd_.get(TvScreenMode::vertical_resolution)) {
                    case VerticalResolution::lines_nb_224:
                        tv_screen_status_.vertical_res = vertical_res_448;
                        tv_screen_status_.screen_mode  = ScreenMode::normal_320_448;
                        break;
                    case VerticalResolution::lines_nb_240:
                        tv_screen_status_.vertical_res = vertical_res_480;
                        tv_screen_status_.screen_mode  = ScreenMode::normal_320_480;
                        break;
                    case VerticalResolution::lines_nb_256:
                        tv_screen_status_.vertical_res = vertical_res_512;
                        tv_screen_status_.screen_mode  = ScreenMode::normal_320_512;
                        break;
                    default: tv_screen_status_.vertical_res = 0; tv_screen_status_.screen_mode = ScreenMode::not_set;
                }
            }

            break;
        case HorizontalResolution::normal_352:
            tv_screen_status_.horizontal_res   = horizontal_res_352;
            tv_screen_status_.screen_mode_type = ScreenModeType::normal;
            if (tv_screen_status_.interlace_mode == InterlaceMode::non_interlace) {
                switch (tvmd_.get(TvScreenMode::vertical_resolution)) {
                    case VerticalResolution::lines_nb_224:
                        tv_screen_status_.vertical_res = vertical_res_224;
                        tv_screen_status_.screen_mode  = ScreenMode::normal_352_224;
                        break;
                    case VerticalResolution::lines_nb_240:
                        tv_screen_status_.vertical_res = vertical_res_240;
                        tv_screen_status_.screen_mode  = ScreenMode::normal_352_240;
                        break;
                    case VerticalResolution::lines_nb_256:
                        tv_screen_status_.vertical_res = vertical_res_256;
                        tv_screen_status_.screen_mode  = ScreenMode::normal_352_256;
                        break;
                    default: tv_screen_status_.vertical_res = 0; tv_screen_status_.screen_mode = ScreenMode::not_set;
                }
            } else {
                switch (tvmd_.get(TvScreenMode::vertical_resolution)) {
                    case VerticalResolution::lines_nb_224:
                        tv_screen_status_.vertical_res = vertical_res_448;
                        tv_screen_status_.screen_mode  = ScreenMode::normal_352_448;
                        break;
                    case VerticalResolution::lines_nb_240:
                        tv_screen_status_.vertical_res = vertical_res_480;
                        tv_screen_status_.screen_mode  = ScreenMode::normal_352_480;
                        break;
                    case VerticalResolution::lines_nb_256:
                        tv_screen_status_.vertical_res = vertical_res_512;
                        tv_screen_status_.screen_mode  = ScreenMode::normal_352_512;
                        break;
                    default: tv_screen_status_.vertical_res = 0; tv_screen_status_.screen_mode = ScreenMode::not_set;
                }
            }

            break;
        case HorizontalResolution::hi_res_640:
            tv_screen_status_.horizontal_res   = horizontal_res_640;
            tv_screen_status_.screen_mode_type = ScreenModeType::hi_res;
            if (tv_screen_status_.interlace_mode == InterlaceMode::non_interlace) {
                switch (tvmd_.get(TvScreenMode::vertical_resolution)) {
                    case VerticalResolution::lines_nb_224:
                        tv_screen_status_.vertical_res = vertical_res_224;
                        tv_screen_status_.screen_mode  = ScreenMode::hi_res_640_224;
                        break;
                    case VerticalResolution::lines_nb_240:
                        tv_screen_status_.vertical_res = vertical_res_240;
                        tv_screen_status_.screen_mode  = ScreenMode::hi_res_640_240;
                        break;
                    case VerticalResolution::lines_nb_256:
                        tv_screen_status_.vertical_res = vertical_res_256;
                        tv_screen_status_.screen_mode  = ScreenMode::hi_res_640_256;
                        break;
                    default: tv_screen_status_.vertical_res = 0; tv_screen_status_.screen_mode = ScreenMode::not_set;
                }
            } else {
                switch (tvmd_.get(TvScreenMode::vertical_resolution)) {
                    case VerticalResolution::lines_nb_224:
                        tv_screen_status_.vertical_res = vertical_res_448;
                        tv_screen_status_.screen_mode  = ScreenMode::hi_res_640_448;
                        break;
                    case VerticalResolution::lines_nb_240:
                        tv_screen_status_.vertical_res = vertical_res_480;
                        tv_screen_status_.screen_mode  = ScreenMode::hi_res_640_480;
                        break;
                    case VerticalResolution::lines_nb_256:
                        tv_screen_status_.vertical_res = vertical_res_512;
                        tv_screen_status_.screen_mode  = ScreenMode::hi_res_640_512;
                        break;
                    default: tv_screen_status_.vertical_res = 0; tv_screen_status_.screen_mode = ScreenMode::not_set;
                }
            }
            break;
        case HorizontalResolution::hi_res_704:
            tv_screen_status_.horizontal_res   = horizontal_res_704;
            tv_screen_status_.screen_mode_type = ScreenModeType::hi_res;
            if (tv_screen_status_.interlace_mode == InterlaceMode::non_interlace) {
                switch (tvmd_.get(TvScreenMode::vertical_resolution)) {
                    case VerticalResolution::lines_nb_224:
                        tv_screen_status_.vertical_res = vertical_res_224;
                        tv_screen_status_.screen_mode  = ScreenMode::hi_res_704_224;
                        break;
                    case VerticalResolution::lines_nb_240:
                        tv_screen_status_.vertical_res = vertical_res_240;
                        tv_screen_status_.screen_mode  = ScreenMode::hi_res_704_240;
                        break;
                    case VerticalResolution::lines_nb_256:
                        tv_screen_status_.vertical_res = vertical_res_256;
                        tv_screen_status_.screen_mode  = ScreenMode::hi_res_704_256;
                        break;
                    default: tv_screen_status_.vertical_res = 0; tv_screen_status_.screen_mode = ScreenMode::not_set;
                }
            } else {
                switch (tvmd_.get(TvScreenMode::vertical_resolution)) {
                    case VerticalResolution::lines_nb_224:
                        tv_screen_status_.vertical_res = vertical_res_448;
                        tv_screen_status_.screen_mode  = ScreenMode::hi_res_704_448;
                        break;
                    case VerticalResolution::lines_nb_240:
                        tv_screen_status_.vertical_res = vertical_res_480;
                        tv_screen_status_.screen_mode  = ScreenMode::hi_res_704_480;
                        break;
                    case VerticalResolution::lines_nb_256:
                        tv_screen_status_.vertical_res = vertical_res_512;
                        tv_screen_status_.screen_mode  = ScreenMode::hi_res_704_512;
                        break;
                    default: tv_screen_status_.vertical_res = 0; tv_screen_status_.screen_mode = ScreenMode::not_set;
                }
            }
            break;
        case HorizontalResolution::exclusive_normal_320:
            tv_screen_status_.horizontal_res   = horizontal_res_320;
            tv_screen_status_.vertical_res     = vertical_res_480;
            tv_screen_status_.screen_mode      = ScreenMode::exclusive_320_480;
            tv_screen_status_.screen_mode_type = ScreenModeType::exclusive;
            break;
        case HorizontalResolution::exclusive_normal_352:
            tv_screen_status_.horizontal_res   = horizontal_res_352;
            tv_screen_status_.vertical_res     = vertical_res_480;
            tv_screen_status_.screen_mode      = ScreenMode::exclusive_352_480;
            tv_screen_status_.screen_mode_type = ScreenModeType::exclusive;
            break;
        case HorizontalResolution::exclusive_hi_res_640:
            tv_screen_status_.horizontal_res   = horizontal_res_640;
            tv_screen_status_.vertical_res     = vertical_res_480;
            tv_screen_status_.screen_mode      = ScreenMode::exclusive_640_480;
            tv_screen_status_.screen_mode_type = ScreenModeType::exclusive;
            break;
        case HorizontalResolution::exclusive_hi_res_704:
            tv_screen_status_.horizontal_res   = horizontal_res_704;
            tv_screen_status_.vertical_res     = vertical_res_480;
            tv_screen_status_.screen_mode      = ScreenMode::exclusive_704_480;
            tv_screen_status_.screen_mode_type = ScreenModeType::exclusive;
            break;
    }

    modules_.opengl()->saturnScreenResolution({tv_screen_status_.horizontal_res, tv_screen_status_.vertical_res});
};

void Vdp2::updateRamStatus() {
    ram_status_.vram_size      = vrsize_.get(VramSizeRegister::vram_size);
    ram_status_.vram_a_mode    = ramctl_.get(RamControl::vram_a_mode);
    ram_status_.vram_b_mode    = ramctl_.get(RamControl::vram_b_mode);
    ram_status_.color_ram_mode = ramctl_.get(RamControl::color_ram_mode);
    // memory()->vdp2_cram_memory_mask_         = (ram_status_.color_ram_mode == ColorRamMode::mode_1_rgb_5_bits_2048_colors)
    //                                               ? core::vdp2_cram_32Kb_memory_mask
    //                                               : core::vdp2_cram_16Kb_memory_mask;
    ram_status_.coefficient_table_storage    = ramctl_.get(RamControl::coefficient_table_storage);
    ram_status_.vram_a0_rotation_bank_select = ramctl_.get(RamControl::vram_a0_rotation_bank_select);
    ram_status_.vram_a1_rotation_bank_select = ramctl_.get(RamControl::vram_a1_rotation_bank_select);
    ram_status_.vram_b0_rotation_bank_select = ramctl_.get(RamControl::vram_b0_rotation_bank_select);
    ram_status_.vram_b1_rotation_bank_select = ramctl_.get(RamControl::vram_b1_rotation_bank_select);
}

//--------------------------------------------------------------------------------------------------------------
// VRAM CYCLE PATTERNS methods
//--------------------------------------------------------------------------------------------------------------

auto Vdp2::isScreenDisplayed(ScrollScreen s) -> bool {
    // First check to ensure scroll screen must be displayed. If the screen cannot display, no vram access will be performed.

    // For normal scroll screens (NBG0 to NBG3) :
    // - in cell display format, required image data is pattern name data and character pattern data
    // - in bitmap format, required image data is bitmap pattern data (same as character pattern data)
    //
    // Pattern name data (PND) read access during 1 cycle must be set to a maximum of 2 banks, one being either VRAM-A0 or
    // VRAM-B0, the other being VRAM-A1 or VRAM-B1.
    // When the VRAM is not divided in 2 partitions, VRAM-A0 is used as VRAM-A and VRAM-B0 as VRAM-B.
    // Selectable timing in normal mode, :
    // - TO-T7 in bank VRAM-A0 or VRAM-B0, TO-T7 in bank VRAM-A1 or VRAM-B1 (case where both banks are splitted)
    // - TO-T7 in bank VRAM-A or VRAM-B (case where no bank is splitted)
    // In Hi-Res or Exclusive mode, selectable timing is reduced to T0-T3, the bank access is identical.

    getScreen(s).is_display_enabled = false;

    switch (s) {
        case ScrollScreen::nbg0: {
            if (bgon_.get(ScreenDisplayEnable::screen_display_enable_nbg0) == ScreenDisplayEnableBit::cannot_display) {
                return false;
            }

            // Pattern name data reads depend on the reduction setting of the screen
            const auto reduction = getReductionSetting(zmctl_.get(ReductionEnable::zoom_quarter_nbg0),
                                                       zmctl_.get(ReductionEnable::zoom_half_nbg0));

            // Character / Bitmap pattern data reads depend on the reduction setting and the number of colors
            if (chctla_.get(CharacterControlA::bitmap_enable_nbg0) == BitmapEnable::bitmap_format) {
                // Bitmap format needs only bitmap pattern data.
                const auto color_number = chctla_.get(CharacterControlA::character_color_number_nbg0);
                const auto required_bpd_reads
                    = util::toUnderlying(calculateRequiredVramCharacterPatternReads(reduction, color_number));
                const auto current_bdp_reads
                    = getVramAccessByCommand(VramAccessCommand::nbg0_character_pattern_data_read, reduction);
                if (current_bdp_reads < required_bpd_reads) { return false; }
            } else {
                // Character format needs character pattern data (cpd) and pattern name data (pnd).
                const auto required_pnd_reads = util::toUnderlying(calculateRequiredVramPatternNameReads(reduction));
                const auto current_pnd_reads  = getVramAccessByCommand(VramAccessCommand::nbg0_pattern_name_read, reduction);
                if (current_pnd_reads < required_pnd_reads) { return false; }

                const auto color_number = chctla_.get(CharacterControlA::character_color_number_nbg0);
                const auto required_cpd_reads
                    = util::toUnderlying(calculateRequiredVramCharacterPatternReads(reduction, color_number));
                const auto current_cpd_reads
                    = getVramAccessByCommand(VramAccessCommand::nbg0_character_pattern_data_read, reduction);
                if (current_cpd_reads < required_cpd_reads) { return false; }
            }
            break;
        }
        case ScrollScreen::nbg1: {
            if (bgon_.get(ScreenDisplayEnable::screen_display_enable_nbg1) == ScreenDisplayEnableBit::cannot_display) {
                return false;
            }
            // Pattern name data reads depend on the reduction setting of the screen
            const auto reduction = getReductionSetting(zmctl_.get(ReductionEnable::zoom_quarter_nbg1),
                                                       zmctl_.get(ReductionEnable::zoom_half_nbg1));

            // Character / Bitmap pattern data reads depend on the reduction setting and the number of colors
            if (chctla_.get(CharacterControlA::bitmap_enable_nbg1) == BitmapEnable::bitmap_format) {
                // Bitmap format needs only bitmap pattern data.
                const auto color_number = chctla_.get(CharacterControlA::character_color_number_nbg1);
                const auto required_bpd_reads
                    = util::toUnderlying(calculateRequiredVramCharacterPatternReads(reduction, color_number));
                const auto current_bdp_reads
                    = getVramAccessByCommand(VramAccessCommand::nbg1_character_pattern_data_read, reduction);
                if (current_bdp_reads < required_bpd_reads) { return false; }
            } else {
                // Character format needs character pattern data (cpd) and pattern name data (pnd).
                const auto required_pnd_reads = util::toUnderlying(calculateRequiredVramPatternNameReads(reduction));
                const auto current_pnd_reads  = getVramAccessByCommand(VramAccessCommand::nbg1_pattern_name_read, reduction);
                if (current_pnd_reads < required_pnd_reads) { return false; }

                const auto color_number = chctla_.get(CharacterControlA::character_color_number_nbg1);
                const auto required_cpd_reads
                    = util::toUnderlying(calculateRequiredVramCharacterPatternReads(reduction, color_number));
                const auto current_cpd_reads
                    = getVramAccessByCommand(VramAccessCommand::nbg1_character_pattern_data_read, reduction);
                if (current_cpd_reads < required_cpd_reads) { return false; }
            }

            break;
        }
        case ScrollScreen::nbg2: {
            if (bgon_.get(ScreenDisplayEnable::screen_display_enable_nbg2) == ScreenDisplayEnableBit::cannot_display) {
                return false;
            }

            if (isScreenDisplayLimitedByReduction(s)) { return false; }

            const auto reduction = ReductionSetting::none;

            // Character format needs character pattern data (cpd) and pattern name data (pnd).
            const auto required_pnd_reads = util::toUnderlying(calculateRequiredVramPatternNameReads(reduction));
            const auto current_pnd_reads  = getVramAccessByCommand(VramAccessCommand::nbg2_pattern_name_read, reduction);
            if (current_pnd_reads < required_pnd_reads) { return false; }

            const auto color_number = chctlb_.get(CharacterControlB::character_color_number_nbg2);
            const auto required_cpd_reads
                = util::toUnderlying(calculateRequiredVramCharacterPatternReads(reduction, color_number));
            const auto current_cpd_reads = getVramAccessByCommand(VramAccessCommand::nbg2_character_pattern_data_read, reduction);
            if (current_cpd_reads < required_cpd_reads) { return false; }

            break;
        }
        case ScrollScreen::nbg3: {
            if (bgon_.get(ScreenDisplayEnable::screen_display_enable_nbg3) == ScreenDisplayEnableBit::cannot_display) {
                return false;
            }
            if (isScreenDisplayLimitedByReduction(s)) { return false; }

            const auto reduction = ReductionSetting::none;

            // Character format needs character pattern data (cpd) and pattern name data (pnd).
            const auto required_pnd_reads = util::toUnderlying(calculateRequiredVramPatternNameReads(reduction));
            const auto current_pnd_reads  = getVramAccessByCommand(VramAccessCommand::nbg3_pattern_name_read, reduction);
            if (current_pnd_reads < required_pnd_reads) { return false; }

            const auto color_number = chctlb_.get(CharacterControlB::character_color_number_nbg3);
            const auto required_cpd_reads
                = util::toUnderlying(calculateRequiredVramCharacterPatternReads(reduction, color_number));
            const auto current_cpd_reads = getVramAccessByCommand(VramAccessCommand::nbg3_character_pattern_data_read, reduction);
            if (current_cpd_reads < required_cpd_reads) { return false; }
            break;
        }
        case ScrollScreen::rbg0: {
            if (bgon_.get(ScreenDisplayEnable::screen_display_enable_rbg0) == ScreenDisplayEnableBit::cannot_display) {
                return false;
            }
            core::Log::unimplemented(core::tr("VDP2 RBG0 display"));
            break;
        }
        case ScrollScreen::rbg1: {
            if (bgon_.get(ScreenDisplayEnable::screen_display_enable_rbg1) == ScreenDisplayEnableBit::cannot_display) {
                return false;
            }
            core::Log::unimplemented(core::tr("VDP2 RBG1 display"));
            break;
        }
    }

    getScreen(s).is_display_enabled = true;
    return true;
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto Vdp2::isScreenDisplayLimitedByReduction(ScrollScreen s) -> bool {
    switch (s) {
        case ScrollScreen::nbg2: {
            const auto reduction    = getReductionSetting(zmctl_.get(ReductionEnable::zoom_quarter_nbg0),
                                                       zmctl_.get(ReductionEnable::zoom_half_nbg0));
            const auto color_number = chctla_.get(CharacterControlA::character_color_number_nbg0);

            if ((reduction == ReductionSetting::up_to_one_quarter) && (color_number == CharacterColorNumber3bits::palette_16)) {
                return true;
            }
            if ((reduction == ReductionSetting::up_to_one_half) && (color_number == CharacterColorNumber3bits::palette_256)) {
                return true;
            }
            break;
        }
        case ScrollScreen::nbg3: {
            const auto reduction    = getReductionSetting(zmctl_.get(ReductionEnable::zoom_quarter_nbg1),
                                                       zmctl_.get(ReductionEnable::zoom_half_nbg1));
            const auto color_number = chctla_.get(CharacterControlA::character_color_number_nbg1);

            if ((reduction == ReductionSetting::up_to_one_quarter) && (color_number == CharacterColorNumber2Bits::palette_16)) {
                return true;
            }
            if ((reduction == ReductionSetting::up_to_one_half) && (color_number == CharacterColorNumber2Bits::palette_256)) {
                return true;
            }
            break;
        }
        default: return false;
    }
    return false;
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto Vdp2::getVramAccessByCommand(const VramAccessCommand command, const ReductionSetting reduction) -> u8 {
    constexpr auto vram_timing_size = u8{8};
    using VramTiming                = std::array<VramAccessCommand, vram_timing_size>;

    auto is_normal_mode = (tvmd_.get(TvScreenMode::horizontal_resolution) == HorizontalResolution::normal_320);
    is_normal_mode |= (tvmd_.get(TvScreenMode::horizontal_resolution) == HorizontalResolution::normal_352);

    static VramTiming bank_a0 = {cyca0l_.get(VramCyclePatternBankA0Lower::t0),
                                 cyca0l_.get(VramCyclePatternBankA0Lower::t1),
                                 cyca0l_.get(VramCyclePatternBankA0Lower::t2),
                                 cyca0l_.get(VramCyclePatternBankA0Lower::t3),
                                 is_normal_mode ? cyca0u_.get(VramCyclePatternBankA0Upper::t4) : VramAccessCommand::no_access,
                                 is_normal_mode ? cyca0u_.get(VramCyclePatternBankA0Upper::t5) : VramAccessCommand::no_access,
                                 is_normal_mode ? cyca0u_.get(VramCyclePatternBankA0Upper::t6) : VramAccessCommand::no_access,
                                 is_normal_mode ? cyca0u_.get(VramCyclePatternBankA0Upper::t7) : VramAccessCommand::no_access};

    static VramTiming bank_a1 = {cyca1l_.get(VramCyclePatternBankA1Lower::t0),
                                 cyca1l_.get(VramCyclePatternBankA1Lower::t1),
                                 cyca1l_.get(VramCyclePatternBankA1Lower::t2),
                                 cyca1l_.get(VramCyclePatternBankA1Lower::t3),
                                 is_normal_mode ? cyca1u_.get(VramCyclePatternBankA1Upper::t4) : VramAccessCommand::no_access,
                                 is_normal_mode ? cyca1u_.get(VramCyclePatternBankA1Upper::t5) : VramAccessCommand::no_access,
                                 is_normal_mode ? cyca1u_.get(VramCyclePatternBankA1Upper::t6) : VramAccessCommand::no_access,
                                 is_normal_mode ? cyca1u_.get(VramCyclePatternBankA1Upper::t7) : VramAccessCommand::no_access};

    static VramTiming bank_b0 = {cycb0l_.get(VramCyclePatternBankB0Lower::t0),
                                 cycb0l_.get(VramCyclePatternBankB0Lower::t1),
                                 cycb0l_.get(VramCyclePatternBankB0Lower::t2),
                                 cycb0l_.get(VramCyclePatternBankB0Lower::t3),
                                 is_normal_mode ? cycb0u_.get(VramCyclePatternBankB0Upper::t4) : VramAccessCommand::no_access,
                                 is_normal_mode ? cycb0u_.get(VramCyclePatternBankB0Upper::t5) : VramAccessCommand::no_access,
                                 is_normal_mode ? cycb0u_.get(VramCyclePatternBankB0Upper::t6) : VramAccessCommand::no_access,
                                 is_normal_mode ? cycb0u_.get(VramCyclePatternBankB0Upper::t7) : VramAccessCommand::no_access};

    static VramTiming bank_b1 = {cycb1l_.get(VramCyclePatternBankB1Lower::t0),
                                 cycb1l_.get(VramCyclePatternBankB1Lower::t1),
                                 cycb1l_.get(VramCyclePatternBankB1Lower::t2),
                                 cycb1l_.get(VramCyclePatternBankB1Lower::t3),
                                 is_normal_mode ? cycb1u_.get(VramCyclePatternBankB1Upper::t4) : VramAccessCommand::no_access,
                                 is_normal_mode ? cycb1u_.get(VramCyclePatternBankB1Upper::t5) : VramAccessCommand::no_access,
                                 is_normal_mode ? cycb1u_.get(VramCyclePatternBankB1Upper::t6) : VramAccessCommand::no_access,
                                 is_normal_mode ? cycb1u_.get(VramCyclePatternBankB1Upper::t7) : VramAccessCommand::no_access};

    switch (command) {
        case VramAccessCommand::nbg0_character_pattern_data_read: {
            if (chctla_.get(CharacterControlA::bitmap_enable_nbg0) == BitmapEnable::bitmap_format) {
                return getVramBitmapReads(bank_a0, bank_a1, bank_b0, bank_b1, command);
            }
            const auto cp_size = chctla_.get(CharacterControlA::character_size_nbg0);
            return getVramCharacterPatternDataReads(bank_a0,
                                                    bank_a1,
                                                    bank_b0,
                                                    bank_b1,
                                                    command,
                                                    reduction,
                                                    is_normal_mode,
                                                    (cp_size == CharacterSize::two_by_two));
        }
        case VramAccessCommand::nbg1_character_pattern_data_read: {
            if (chctla_.get(CharacterControlA::bitmap_enable_nbg1) == BitmapEnable::bitmap_format) {
                return getVramBitmapReads(bank_a0, bank_a1, bank_b0, bank_b1, command);
            }
            const auto cp_size = chctla_.get(CharacterControlA::character_size_nbg1);
            return getVramCharacterPatternDataReads(bank_a0,
                                                    bank_a1,
                                                    bank_b0,
                                                    bank_b1,
                                                    command,
                                                    reduction,
                                                    is_normal_mode,
                                                    (cp_size == CharacterSize::two_by_two));
        }
        case VramAccessCommand::nbg2_character_pattern_data_read: {
            const auto cp_size = chctlb_.get(CharacterControlB::character_size_nbg2);
            return getVramCharacterPatternDataReads(bank_a0,
                                                    bank_a1,
                                                    bank_b0,
                                                    bank_b1,
                                                    command,
                                                    reduction,
                                                    is_normal_mode,
                                                    (cp_size == CharacterSize::two_by_two));
        }
        case VramAccessCommand::nbg3_character_pattern_data_read: {
            const auto cp_size = chctlb_.get(CharacterControlB::character_size_nbg3);
            return getVramCharacterPatternDataReads(bank_a0,
                                                    bank_a1,
                                                    bank_b0,
                                                    bank_b1,
                                                    command,
                                                    reduction,
                                                    is_normal_mode,
                                                    (cp_size == CharacterSize::two_by_two));
        }
        case VramAccessCommand::nbg0_pattern_name_read:
        case VramAccessCommand::nbg1_pattern_name_read:
        case VramAccessCommand::nbg2_pattern_name_read:
        case VramAccessCommand::nbg3_pattern_name_read: {
            return getVramPatternNameDataReads(bank_a0, bank_a1, bank_b0, bank_b1, command);
        }
        case VramAccessCommand::nbg0_vertical_cell_scroll_table_data_read:
        case VramAccessCommand::nbg1_vertical_cell_scroll_table_data_read: {
            core::Log::unimplemented(core::tr("VDP2 vertical cell scroll table data read"));
            break;
        }
        case VramAccessCommand::cpu_read_write: {
            break;
        }
        default: core::Log::warning(Logger::vdp2, core::tr("VDP2 VRAM access command not allowed"));
    }

    constexpr auto not_found = u8{0x0};
    return not_found;
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto Vdp2::getVramBitmapReads(const VramTiming&       bank_a0,
                              const VramTiming&       bank_a1,
                              const VramTiming&       bank_b0,
                              const VramTiming&       bank_b1,
                              const VramAccessCommand command) -> u8 {
    auto bitmap_reads = std::count(bank_a0.begin(), bank_a0.end(), command);
    if (ramctl_.get(RamControl::vram_a_mode) == VramMode::partition_in_2_banks) {
        bitmap_reads += std::count(bank_a1.begin(), bank_a1.end(), command);
    }
    bitmap_reads += std::count(bank_b0.begin(), bank_b0.end(), command);
    if (ramctl_.get(RamControl::vram_b_mode) == VramMode::partition_in_2_banks) {
        bitmap_reads += std::count(bank_b1.begin(), bank_b1.end(), command);
    }
    return static_cast<u8>(bitmap_reads);
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto Vdp2::getVramPatternNameDataReads(const VramTiming&       bank_a0,
                                       const VramTiming&       bank_a1,
                                       const VramTiming&       bank_b0,
                                       const VramTiming&       bank_b1,
                                       const VramAccessCommand command) -> u8 {
    // Pattern name data (PND) read access during 1 cycle must be set to a maximum of 2 banks, one being either
    // VRAM-A0 or VRAM-B0, the other being VRAM-A1 or VRAM-B1. When the VRAM is not divided in 2 partitions, VRAM-A0
    // is used as VRAM-A and VRAM-B0 as VRAM-B.
    // -------------------------------------------
    // | Partition | Partition |   Banks used    |
    // |  Bank A   |  Bank B   |                 |
    // |-----------------------------------------|
    // |    no     |    no     | A0, B0          |
    // |   yes     |    no     | A0, B0          |
    // |    no     |   yes     | A0, B0          |
    // |   yes     |   yes     | A0, B0 or A1,B1 |
    // -------------------------------------------
    auto pnd_reads = ptrdiff_t{};
    if (ramctl_.get(RamControl::vram_a_mode) == VramMode::partition_in_2_banks
        && (ramctl_.get(RamControl::vram_b_mode) == VramMode::partition_in_2_banks)) {
        auto pnd_reads_bank_0 = std::count(bank_a0.begin(), bank_a0.end(), command);
        pnd_reads_bank_0 += std::count(bank_b0.begin(), bank_b0.end(), command);

        auto pnd_reads_bank_1 = std::count(bank_a1.begin(), bank_a1.end(), command);
        pnd_reads_bank_1 += std::count(bank_b1.begin(), bank_b1.end(), command);

        pnd_reads = (pnd_reads_bank_0 > pnd_reads_bank_1) ? pnd_reads_bank_0 : pnd_reads_bank_1;
    } else {
        pnd_reads += std::count(bank_a0.begin(), bank_a0.end(), command);
        pnd_reads += std::count(bank_b0.begin(), bank_b0.end(), command);
    }
    return static_cast<u8>(pnd_reads);
}

// static
auto Vdp2::getReductionSetting(ZoomQuarter zq, ZoomHalf zh) -> ReductionSetting {
    if (zq == ZoomQuarter::up_to_one_quarter) { return ReductionSetting::up_to_one_quarter; }
    if (zh == ZoomHalf::up_to_one_half) { return ReductionSetting::up_to_one_half; }
    return ReductionSetting::none;
};

// static
auto Vdp2::calculateRequiredVramCharacterPatternReads(ReductionSetting r, CharacterColorNumber3bits ccn) -> VramAccessNumber {
    switch (ccn) {
        case CharacterColorNumber3bits::palette_16:
            switch (r) {
                case ReductionSetting::none: return VramAccessNumber::one;
                case ReductionSetting::up_to_one_half: return VramAccessNumber::two;
                case ReductionSetting::up_to_one_quarter: return VramAccessNumber::four;
            }
            break;
        case CharacterColorNumber3bits::palette_256:
            switch (r) {
                case ReductionSetting::none: return VramAccessNumber::two;
                case ReductionSetting::up_to_one_half: return VramAccessNumber::four;
                default: return VramAccessNumber::none;
            }
            break;
        case CharacterColorNumber3bits::palette_2048:
            switch (r) {
                case ReductionSetting::none: return VramAccessNumber::four;
                default: return VramAccessNumber::none;
            }
            break;
        case CharacterColorNumber3bits::rgb_32k:
            switch (r) {
                case ReductionSetting::none: return VramAccessNumber::four;
                default: return VramAccessNumber::none;
            }
            break;
        case CharacterColorNumber3bits::rgb_16m:
            switch (r) {
                case ReductionSetting::none: return VramAccessNumber::eight;
                default: return VramAccessNumber::none;
            }
            break;
    }

    return VramAccessNumber::none;
}

// static
auto Vdp2::calculateRequiredVramCharacterPatternReads(ReductionSetting r, CharacterColorNumber2Bits ccn) -> VramAccessNumber {
    switch (ccn) {
        case CharacterColorNumber2Bits::palette_16:
            switch (r) {
                case ReductionSetting::none: return VramAccessNumber::one;
                case ReductionSetting::up_to_one_half: return VramAccessNumber::two;
                case ReductionSetting::up_to_one_quarter: return VramAccessNumber::four;
            }
            break;
        case CharacterColorNumber2Bits::palette_256:
            switch (r) {
                case ReductionSetting::none: return VramAccessNumber::two;
                case ReductionSetting::up_to_one_half: return VramAccessNumber::four;
                default: return VramAccessNumber::none;
            }
            break;
        case CharacterColorNumber2Bits::palette_2048:
            switch (r) {
                case ReductionSetting::none: return VramAccessNumber::four;
                default: return VramAccessNumber::none;
            }
            break;
        case CharacterColorNumber2Bits::rgb_32k:
            switch (r) {
                case ReductionSetting::none: return VramAccessNumber::four;
                default: return VramAccessNumber::none;
            }
            break;
    }

    return VramAccessNumber::none;
}

// static
auto Vdp2::calculateRequiredVramCharacterPatternReads(ReductionSetting r, CharacterColorNumber1Bit ccn) -> VramAccessNumber {
    switch (ccn) {
        case CharacterColorNumber1Bit::palette_16:
            switch (r) {
                case ReductionSetting::none: return VramAccessNumber::one;
                case ReductionSetting::up_to_one_half: return VramAccessNumber::two;
                case ReductionSetting::up_to_one_quarter: return VramAccessNumber::four;
            }
            break;
        case CharacterColorNumber1Bit::palette_256:
            switch (r) {
                case ReductionSetting::none: return VramAccessNumber::two;
                case ReductionSetting::up_to_one_half: return VramAccessNumber::four;
                default: return VramAccessNumber::none;
            }
            break;
    }

    return VramAccessNumber::none;
}

// static
auto Vdp2::calculateRequiredVramPatternNameReads(ReductionSetting r) -> VramAccessNumber {
    switch (r) {
        case ReductionSetting::up_to_one_quarter: return VramAccessNumber::four;
        case ReductionSetting::up_to_one_half: return VramAccessNumber::two;
        default: return VramAccessNumber::one;
    }
};

// static
auto Vdp2::getPatternNameFromCharacterPattern(const VramAccessCommand character_pattern) -> VramAccessCommand {
    switch (character_pattern) {
        case VramAccessCommand::nbg0_character_pattern_data_read: {
            return VramAccessCommand::nbg0_pattern_name_read;
        }
        case VramAccessCommand::nbg1_character_pattern_data_read: {
            return VramAccessCommand::nbg1_pattern_name_read;
        }
        case VramAccessCommand::nbg2_character_pattern_data_read: {
            return VramAccessCommand::nbg2_pattern_name_read;
        }
        case VramAccessCommand::nbg3_character_pattern_data_read: {
            return VramAccessCommand::nbg3_pattern_name_read;
        }
        default: return VramAccessCommand::no_access;
    }
}

// static
void Vdp2::setPatternNameAccess(const VramTiming&                   bank,
                                const VramAccessCommand             pattern,
                                std::array<bool, vram_timing_size>& pnd_access) {
    auto it = std::find(bank.begin(), bank.end(), pattern);
    while (it != bank.end()) {
        pnd_access[std::distance(bank.begin(), it)] = true;
        ++it;
        it = std::find(it, bank.end(), pattern);
    }
}

// static
void Vdp2::setCharacterPatternLimitations(const bool                                is_screen_mode_normal,
                                          const bool                                is_character_pattern_2_by_2,
                                          const std::array<bool, vram_timing_size>& pnd_access,
                                          std::array<bool, vram_timing_size>&       allowed_cpd_timing) {
    constexpr auto t0 = u8{0};
    constexpr auto t1 = u8{1};
    constexpr auto t2 = u8{2};
    constexpr auto t3 = u8{3};
    constexpr auto t4 = u8{4};
    constexpr auto t5 = u8{5};
    constexpr auto t6 = u8{6};
    constexpr auto t7 = u8{7};

    // For each pnd access set, allowed timing is set for cpd
    auto it = std::find(pnd_access.begin(), pnd_access.end(), true);
    while (it != pnd_access.end()) {
        switch (std::distance(pnd_access.begin(), it)) {
            case t0: {
                if (is_screen_mode_normal) {
                    allowed_cpd_timing[t0] = true;
                    allowed_cpd_timing[t1] = true;
                    allowed_cpd_timing[t2] = true;
                    allowed_cpd_timing[t4] = true;
                    allowed_cpd_timing[t5] = true;
                    allowed_cpd_timing[t6] = true;
                    allowed_cpd_timing[t7] = true;
                } else {
                    allowed_cpd_timing[t0] = true;
                    allowed_cpd_timing[t1] = true;
                    allowed_cpd_timing[t2] = true;
                }
                break;
            }
            case t1: {
                if (is_screen_mode_normal) {
                    allowed_cpd_timing[t0] = true;
                    allowed_cpd_timing[t1] = true;
                    allowed_cpd_timing[t2] = true;
                    allowed_cpd_timing[t3] = true;
                    allowed_cpd_timing[t5] = true;
                    allowed_cpd_timing[t6] = true;
                    allowed_cpd_timing[t7] = true;
                } else {
                    allowed_cpd_timing[t1] = true;
                    allowed_cpd_timing[t2] = true;
                    allowed_cpd_timing[t3] = true;
                }
                break;
            }
            case t2: {
                if (is_screen_mode_normal) {
                    allowed_cpd_timing[t0] = true;
                    allowed_cpd_timing[t1] = true;
                    allowed_cpd_timing[t2] = true;
                    allowed_cpd_timing[t3] = true;
                    allowed_cpd_timing[t6] = true;
                    allowed_cpd_timing[t7] = true;
                } else {
                    // Sattech #37 states that accessing t2 in high res mode while using 2*2 character pattern restricts access to
                    // t2 and t3
                    if (!is_character_pattern_2_by_2) { allowed_cpd_timing[t0] = true; }
                    allowed_cpd_timing[t2] = true;
                    allowed_cpd_timing[t3] = true;
                }
                break;
            }
            case t3: {
                if (is_screen_mode_normal) {
                    allowed_cpd_timing[t0] = true;
                    allowed_cpd_timing[t1] = true;
                    allowed_cpd_timing[t2] = true;
                    allowed_cpd_timing[t3] = true;
                    allowed_cpd_timing[t7] = true;
                } else {
                    // Sattech #37 states that accessing t3 in high res mode while using 2*2 character pattern restricts access to
                    // t3
                    if (!is_character_pattern_2_by_2) {
                        allowed_cpd_timing[t0] = true;
                        allowed_cpd_timing[t1] = true;
                    }
                    allowed_cpd_timing[t3] = true;
                }
                break;
            }
            case t4: {
                if (is_screen_mode_normal) {
                    allowed_cpd_timing[t0] = true;
                    allowed_cpd_timing[t1] = true;
                    allowed_cpd_timing[t2] = true;
                    allowed_cpd_timing[t3] = true;
                }
                break;
            }
            case t5: {
                if (is_screen_mode_normal) {
                    allowed_cpd_timing[t1] = true;
                    allowed_cpd_timing[t2] = true;
                    allowed_cpd_timing[t3] = true;
                }
                break;
            }
            case t6: {
                if (is_screen_mode_normal) {
                    allowed_cpd_timing[t2] = true;
                    allowed_cpd_timing[t3] = true;
                }
                break;
            }
            case t7: {
                if (is_screen_mode_normal) { allowed_cpd_timing[t3] = true; }
                break;
            }
        }

        ++it;
        it = std::find(it, pnd_access.end(), true);
    }
}

// static
auto Vdp2::getVramCharacterPatternDataReads(const VramTiming&       bank_a0,
                                            const VramTiming&       bank_a1,
                                            const VramTiming&       bank_b0,
                                            const VramTiming&       bank_b1,
                                            const VramAccessCommand command,
                                            const ReductionSetting  reduction,
                                            const bool              is_screen_mode_normal,
                                            const bool              is_using_cp_4_by_4) -> u8 {
    // From the command we must use the linked Pattern Name Data. The limitations are based on the PND read position.
    // Step 1 : find PND reads for the current command
    // const auto pnd = getPatternNameFromCharacterPattern(command);
    // const auto pnd_reads = getVramPatternNameDataReads(bank_a0, bank_a1, bank_b0, bank_b1, pnd);

    auto pnd_timing_access = std::array{false, false, false, false, false, false, false, false};
    setPatternNameAccess(bank_a0, command, pnd_timing_access);
    setPatternNameAccess(bank_b0, command, pnd_timing_access);
    setPatternNameAccess(bank_a1, command, pnd_timing_access);
    setPatternNameAccess(bank_b1, command, pnd_timing_access);

    // If there's no reduction, observe selection limits when CPD read access >= 2.
    // If reduction = 1/2 or 1/4, the behavior isn't clear from the docs ... I'll just
    // observe selection limits for every access.
    auto are_limitations_applied = bool{true};
    if (reduction == ReductionSetting::none) {
        auto unlimited_cpd_reads = std::count(bank_a0.begin(), bank_a0.end(), command);
        unlimited_cpd_reads += std::count(bank_b0.begin(), bank_b0.end(), command);
        unlimited_cpd_reads += std::count(bank_a1.begin(), bank_a1.end(), command);
        unlimited_cpd_reads += std::count(bank_b1.begin(), bank_b1.end(), command);
        if (unlimited_cpd_reads < 2) { are_limitations_applied = false; }
    }

    VramTiming limited_bank_a0 = {bank_a0};
    VramTiming limited_bank_b0 = {bank_b0};
    VramTiming limited_bank_a1 = {bank_a1};
    VramTiming limited_bank_b1 = {bank_b1};

    if (are_limitations_applied) {
        // Step 2 : apply selection limits on accessed timings
        // Some more restrictions can be applied when displaying NBG screens in high res when using 2*2 CP (cf sattechs #37 for
        // details)
        auto allowed_cpd_timing = std::array{false, false, false, false, false, false, false, false};
        setCharacterPatternLimitations(is_screen_mode_normal, is_using_cp_4_by_4, pnd_timing_access, allowed_cpd_timing);

        // Step 3 : get the reads
        // First access not available are changed to no access
        auto it = std::find(allowed_cpd_timing.begin(), allowed_cpd_timing.end(), false);
        while (it != allowed_cpd_timing.end()) {
            limited_bank_a0[std::distance(allowed_cpd_timing.begin(), it)] = VramAccessCommand::no_access;
            limited_bank_b0[std::distance(allowed_cpd_timing.begin(), it)] = VramAccessCommand::no_access;
            limited_bank_a1[std::distance(allowed_cpd_timing.begin(), it)] = VramAccessCommand::no_access;
            limited_bank_b1[std::distance(allowed_cpd_timing.begin(), it)] = VramAccessCommand::no_access;
            ++it;
            it = std::find(it, allowed_cpd_timing.end(), false);
        }
    }
    // Counting cpd access
    auto cpd_reads = std::count(limited_bank_a0.begin(), limited_bank_a0.end(), command);
    cpd_reads += std::count(limited_bank_b0.begin(), limited_bank_b0.end(), command);
    cpd_reads += std::count(limited_bank_a1.begin(), limited_bank_a1.end(), command);
    cpd_reads += std::count(limited_bank_b1.begin(), limited_bank_b1.end(), command);

    return static_cast<u8>(cpd_reads);
};

//--------------------------------------------------------------------------------------------------------------
// DISPLAY methods
//--------------------------------------------------------------------------------------------------------------

void Vdp2::clearRenderData() {
    vdp2_parts_[util::toUnderlying(ScrollScreen::nbg0)].clear();
    vdp2_parts_[util::toUnderlying(ScrollScreen::nbg1)].clear();
    vdp2_parts_[util::toUnderlying(ScrollScreen::nbg2)].clear();
    vdp2_parts_[util::toUnderlying(ScrollScreen::nbg3)].clear();
    vdp2_parts_[util::toUnderlying(ScrollScreen::rbg0)].clear();
    vdp2_parts_[util::toUnderlying(ScrollScreen::rbg1)].clear();
}

void Vdp2::populateRenderData() {
    if (isScreenDisplayed(ScrollScreen::rbg1)) {
        updateScrollScreenStatus(ScrollScreen::rbg1);
        if (getScreen(ScrollScreen::rbg1).priority_number != 0) { readScrollScreenData(ScrollScreen::rbg1); }
    }
    if (isScreenDisplayed(ScrollScreen::rbg0)) {
        updateScrollScreenStatus(ScrollScreen::rbg0);
        if (getScreen(ScrollScreen::rbg0).priority_number != 0) { readScrollScreenData(ScrollScreen::rbg0); }
    }

    auto is_nbg_displayed
        = !(getScreen(ScrollScreen::rbg0).is_display_enabled && getScreen(ScrollScreen::rbg1).is_display_enabled);

    if (is_nbg_displayed) {
        if (isScreenDisplayed(ScrollScreen::nbg0)) {
            updateScrollScreenStatus(ScrollScreen::nbg0);
            if (getScreen(ScrollScreen::nbg0).priority_number != 0) { readScrollScreenData(ScrollScreen::nbg0); }
        }

        getScreen(ScrollScreen::nbg1).is_display_enabled = false;
        if (canScrollScreenBeDisplayed(ScrollScreen::nbg1)) {
            if (isScreenDisplayed(ScrollScreen::nbg1)) {
                updateScrollScreenStatus(ScrollScreen::nbg1);
                if (getScreen(ScrollScreen::nbg1).priority_number != 0) { readScrollScreenData(ScrollScreen::nbg1); }
            }
        }
        if (canScrollScreenBeDisplayed(ScrollScreen::nbg2)) {
            if (isScreenDisplayed(ScrollScreen::nbg2)) {
                updateScrollScreenStatus(ScrollScreen::nbg2);
                if (getScreen(ScrollScreen::nbg2).priority_number != 0) { readScrollScreenData(ScrollScreen::nbg2); }
            }
        }

        if (canScrollScreenBeDisplayed(ScrollScreen::nbg3)) {
            if (isScreenDisplayed(ScrollScreen::nbg3)) {
                updateScrollScreenStatus(ScrollScreen::nbg3);
                if (getScreen(ScrollScreen::nbg3).priority_number != 0) { readScrollScreenData(ScrollScreen::nbg3); }
            }
        }
    }
}

auto Vdp2::canScrollScreenBeDisplayed(const ScrollScreen s) const -> bool {
    const auto nbg0_color_nb = getScreen(ScrollScreen::nbg0).character_color_number;
    const auto nbg1_color_nb = getScreen(ScrollScreen::nbg1).character_color_number;
    switch (s) {
        case ScrollScreen::nbg1: {
            return (nbg0_color_nb != ColorCount::rgb_16m);
        }
        case ScrollScreen::nbg2: {
            const auto colors_preventing_display = std::array{ColorCount::palette_2048, ColorCount::rgb_32k, ColorCount::rgb_16m};
            return std::none_of(colors_preventing_display.begin(),
                                colors_preventing_display.end(),
                                [&nbg0_color_nb](const ColorCount cc) { return cc == nbg0_color_nb; });
        }
        case ScrollScreen::nbg3: {
            if (nbg0_color_nb == ColorCount::rgb_16m) { return false; }
            const auto colors_preventing_display = std::array{ColorCount::palette_2048, ColorCount::rgb_32k};
            return std::none_of(colors_preventing_display.begin(),
                                colors_preventing_display.end(),
                                [&nbg1_color_nb](const ColorCount cc) { return cc == nbg1_color_nb; });
        }
        default: return true;
    }
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void Vdp2::updateScrollScreenStatus(const ScrollScreen s) {
    constexpr auto map_size_nbg = u8{2 * 2};
    constexpr auto map_size_rbg = u8{4 * 4};
    constexpr auto cell_size    = u8{8 * 8};

    const auto getCharacterColorNumber3Bits = [](const CharacterColorNumber3bits c, const ScreenModeType t) {
        switch (c) {
            case CharacterColorNumber3bits::palette_16: return ColorCount::palette_16;
            case CharacterColorNumber3bits::palette_256: return ColorCount::palette_256;
            case CharacterColorNumber3bits::palette_2048: return ColorCount::palette_2048;
            case CharacterColorNumber3bits::rgb_32k: {
                return (t == ScreenModeType::normal) ? ColorCount::rgb_32k : ColorCount::not_allowed;
            }
            case CharacterColorNumber3bits::rgb_16m: return ColorCount::rgb_16m;
            default: return ColorCount::not_allowed;
        }
    };
    const auto getCharacterColorNumber2Bits = [](const CharacterColorNumber2Bits c, const ScreenModeType t) {
        switch (c) {
            case CharacterColorNumber2Bits::palette_16: return ColorCount::palette_16;
            case CharacterColorNumber2Bits::palette_256: return ColorCount::palette_256;
            case CharacterColorNumber2Bits::palette_2048: return ColorCount::palette_2048;
            case CharacterColorNumber2Bits::rgb_32k: {
                return (t == ScreenModeType::exclusive) ? ColorCount::rgb_16m : ColorCount::rgb_32k;
            }
            default: return ColorCount::not_allowed;
        }
    };
    const auto getCharacterColorNumber1Bit = [](const CharacterColorNumber1Bit c) {
        switch (c) {
            case CharacterColorNumber1Bit::palette_16: return ColorCount::palette_16;
            case CharacterColorNumber1Bit::palette_256: return ColorCount::palette_256;
            default: return ColorCount::not_allowed;
        }
    };
    const auto getCharacterColorNumberRbg0 = [](const CharacterColorNumber3bits c, const ScreenModeType t) {
        if (t == ScreenModeType::exclusive) { return ColorCount::cannot_display; }

        switch (c) {
            case CharacterColorNumber3bits::palette_16: return ColorCount::palette_16;
            case CharacterColorNumber3bits::palette_256: return ColorCount::palette_256;
            case CharacterColorNumber3bits::palette_2048: return ColorCount::palette_2048;
            case CharacterColorNumber3bits::rgb_32k: return ColorCount::rgb_32k;
            case CharacterColorNumber3bits::rgb_16m: {
                return (t == ScreenModeType::normal) ? ColorCount::rgb_16m : ColorCount::not_allowed;
            }
            default: return ColorCount::not_allowed;
        }
    };
    const auto getDotSize = [](const ColorCount cc) {
        const auto dot_size_4  = u8{4};
        const auto dot_size_8  = u8{8};
        const auto dot_size_16 = u8{16};
        const auto dot_size_32 = u8{32};
        switch (cc) {
            case ColorCount::palette_16: return dot_size_4;
            case ColorCount::palette_256: return dot_size_8;
            case ColorCount::palette_2048: return dot_size_16;
            case ColorCount::rgb_32k: return dot_size_16;
            default: return dot_size_32;
        }
    };
    const auto getPageSize = [](const PatternNameDataSize pnd_sz, const CharacterSize ch_sz) {
        constexpr auto boundary_1_word_1_by_1_cell  = u16{0x2000};
        constexpr auto boundary_1_word_2_by_2_cells = u16{0x800};
        if (pnd_sz == PatternNameDataSize::one_word) {
            return (ch_sz == CharacterSize::one_by_one) ? boundary_1_word_1_by_1_cell : boundary_1_word_2_by_2_cells;
        }

        // The other case is 2 words.
        constexpr auto boundary_2_words_1_by_1_cell  = u16{0x4000};
        constexpr auto boundary_2_words_2_by_2_cells = u16{0x1000};
        return (ch_sz == CharacterSize::one_by_one) ? boundary_2_words_1_by_1_cell : boundary_2_words_2_by_2_cells;
    };

    const auto getScrollScreenFormat = [](const BitmapEnable be) {
        return (be == BitmapEnable::cell_format) ? ScrollScreenFormat::cell : ScrollScreenFormat::bitmap;
    };

    const auto getBitmapSize = [](const BitmapSize2Bits sz) {
        switch (sz) {
            case BitmapSize2Bits::size_512_by_256: return BitmapSize::size_512_by_256;
            case BitmapSize2Bits::size_512_by_512: return BitmapSize::size_512_by_512;
            case BitmapSize2Bits::size_1024_by_256: return BitmapSize::size_1024_by_256;
            case BitmapSize2Bits::size_1024_by_512: return BitmapSize::size_1024_by_512;
            default: return BitmapSize::not_set;
        }
    };

    const auto getBitmapStartAddress = [](const u8 map_offset) {
        constexpr auto boundary = u32{0x20000};
        return vram_start_address + map_offset * boundary;
    };

    auto& screen         = getScreen(s);
    screen.scroll_screen = s;

    switch (s) {
        case ScrollScreen::nbg0:
            // Color RAM
            screen.color_ram_address_offset
                = getColorRamAddressOffset(craofa_.get(ColorRamAddressOffsetA::color_ram_address_offset_nbg0));

            // Transparency
            screen.is_transparency_code_valid = (bgon_.get(ScreenDisplayEnable::transparency_display_enable_nbg0)
                                                 == TransparentDisplayEnable::transparency_code_valid);

            // Priority
            screen.priority_number = prina_.get(PriorityNumberA::nbg0);

            // Map
            screen.map_size   = map_size_nbg;
            screen.map_offset = mpofn_.get(MapOffsetNbg::map_offset_nbg0);

            // Plane
            screen.plane_size            = plsz_.get(PlaneSizeRegister::plane_size_nbg0);
            screen.plane_a_start_address = calculatePlaneStartAddress(s, mpabn0_.get(MapNbg0PlaneAB::plane_a));
            screen.plane_b_start_address = calculatePlaneStartAddress(s, mpabn0_.get(MapNbg0PlaneAB::plane_b));
            screen.plane_c_start_address = calculatePlaneStartAddress(s, mpabn0_.get(MapNbg0PlaneCD::plane_c));
            screen.plane_d_start_address = calculatePlaneStartAddress(s, mpabn0_.get(MapNbg0PlaneCD::plane_d));

            // Page
            screen.page_size = getPageSize(pncn0_.get(PatternNameControlNbg0::pattern_name_data_size),
                                           chctla_.get(CharacterControlA::character_size_nbg0));

            // Pattern name data
            screen.pattern_name_data_size           = pncn0_.get(PatternNameControlNbg0::pattern_name_data_size);
            screen.character_number_supplement_mode = pncn0_.get(PatternNameControlNbg0::character_number_mode);
            screen.special_priority                 = pncn0_.get(PatternNameControlNbg0::special_priority);
            screen.special_color_calculation        = pncn0_.get(PatternNameControlNbg0::special_color_calculation);
            screen.supplementary_palette_number     = pncn0_.get(PatternNameControlNbg0::supplementary_palette_number);
            screen.supplementary_character_number   = pncn0_.get(PatternNameControlNbg0::supplementary_character_number);

            // Character pattern
            screen.character_pattern_size = chctla_.get(CharacterControlA::character_size_nbg0);
            screen.character_color_number
                = getCharacterColorNumber3Bits(chctla_.get(CharacterControlA::character_color_number_nbg0),
                                               tv_screen_status_.screen_mode_type);
            screen.format = getScrollScreenFormat(chctla_.get(CharacterControlA::bitmap_enable_nbg0));

            // Cell
            screen.cell_size = cell_size * getDotSize(screen.character_color_number) / bits_in_a_byte;

            // Bitmap
            screen.bitmap_size                      = getBitmapSize(chctla_.get(CharacterControlA::bitmap_size_nbg0));
            screen.bitmap_palette_number            = bmpna_.get(BitmapPaletteNumberA::bitmap_palette_number_nbg0);
            screen.bitmap_special_priority          = bmpna_.get(BitmapPaletteNumberA::bitmap_special_priority_nbg0);
            screen.bitmap_special_color_calculation = bmpna_.get(BitmapPaletteNumberA::bitmap_special_color_calculation_nbg0);
            screen.bitmap_start_address             = getBitmapStartAddress(screen.map_offset);

            // Scroll screen
            screen.screen_scroll_horizontal_integer = scxin0_.get(ScreenScrollValueNbg0HorizontalIntegerPart::horizontal_integer);
            screen.screen_scroll_horizontal_fractional
                = scxdn0_.get(ScreenScrollValueNbg0HorizontalFractionalPart::horizontal_fractional);
            screen.screen_scroll_vertical_integer = scyin0_.get(ScreenScrollValueNbg0VerticalIntegerPart::vertical_integer);
            screen.screen_scroll_vertical_fractional
                = scydn0_.get(ScreenScrollValueNbg0VerticalFractionalPart::vertical_fractional);

            break;
        case ScrollScreen::nbg1:
            // Color RAM
            screen.color_ram_address_offset
                = getColorRamAddressOffset(craofa_.get(ColorRamAddressOffsetA::color_ram_address_offset_nbg1));

            // Transparency
            screen.is_transparency_code_valid = (bgon_.get(ScreenDisplayEnable::transparency_display_enable_nbg1)
                                                 == TransparentDisplayEnable::transparency_code_valid);
            // Priority
            screen.priority_number = prina_.get(PriorityNumberA::nbg1);

            // Map
            screen.map_size   = map_size_nbg;
            screen.map_offset = mpofn_.get(MapOffsetNbg::map_offset_nbg1);

            // Plane
            screen.plane_size            = plsz_.get(PlaneSizeRegister::plane_size_nbg1);
            screen.plane_a_start_address = calculatePlaneStartAddress(s, mpabn1_.get(MapNbg1PlaneAB::plane_a));
            screen.plane_b_start_address = calculatePlaneStartAddress(s, mpabn1_.get(MapNbg1PlaneAB::plane_b));
            screen.plane_c_start_address = calculatePlaneStartAddress(s, mpabn1_.get(MapNbg1PlaneCD::plane_c));
            screen.plane_d_start_address = calculatePlaneStartAddress(s, mpabn1_.get(MapNbg1PlaneCD::plane_d));

            // Page
            screen.page_size = getPageSize(pncn1_.get(PatternNameControlNbg1::pattern_name_data_size),
                                           chctla_.get(CharacterControlA::character_size_nbg1));

            // Pattern name data
            screen.pattern_name_data_size           = pncn1_.get(PatternNameControlNbg1::pattern_name_data_size);
            screen.character_number_supplement_mode = pncn1_.get(PatternNameControlNbg1::character_number_mode);
            screen.special_priority                 = pncn1_.get(PatternNameControlNbg1::special_priority);
            screen.special_color_calculation        = pncn1_.get(PatternNameControlNbg1::special_color_calculation);
            screen.supplementary_palette_number     = pncn1_.get(PatternNameControlNbg1::supplementary_palette_number);
            screen.supplementary_character_number   = pncn1_.get(PatternNameControlNbg1::supplementary_character_number);

            // Character pattern
            screen.character_pattern_size = chctla_.get(CharacterControlA::character_size_nbg1);
            screen.character_color_number
                = getCharacterColorNumber2Bits(chctla_.get(CharacterControlA::character_color_number_nbg1),
                                               tv_screen_status_.screen_mode_type);
            screen.format = getScrollScreenFormat(chctla_.get(CharacterControlA::bitmap_enable_nbg1));

            // Cell
            screen.cell_size = cell_size * getDotSize(screen.character_color_number) / bits_in_a_byte;

            // Bitmap
            screen.bitmap_size                      = getBitmapSize(chctla_.get(CharacterControlA::bitmap_size_nbg1));
            screen.bitmap_palette_number            = bmpna_.get(BitmapPaletteNumberA::bitmap_palette_number_nbg1);
            screen.bitmap_special_priority          = bmpna_.get(BitmapPaletteNumberA::bitmap_special_priority_nbg1);
            screen.bitmap_special_color_calculation = bmpna_.get(BitmapPaletteNumberA::bitmap_special_color_calculation_nbg1);
            screen.bitmap_start_address             = getBitmapStartAddress(screen.map_offset);

            // Scroll screen
            screen.screen_scroll_horizontal_integer = scxin1_.get(ScreenScrollValueNbg1HorizontalIntegerPart::horizontal_integer);
            screen.screen_scroll_horizontal_fractional
                = scxdn1_.get(ScreenScrollValueNbg1HorizontalFractionalPart::horizontal_fractional);
            screen.screen_scroll_vertical_integer = scyin1_.get(ScreenScrollValueNbg1VerticalIntegerPart::vertical_integer);
            screen.screen_scroll_vertical_fractional
                = scydn1_.get(ScreenScrollValueNbg1VerticalFractionalPart::vertical_fractional);

            break;
        case ScrollScreen::nbg2:
            // Color RAM
            screen.color_ram_address_offset
                = getColorRamAddressOffset(craofa_.get(ColorRamAddressOffsetA::color_ram_address_offset_nbg2));

            // Transparency
            screen.is_transparency_code_valid = (bgon_.get(ScreenDisplayEnable::transparency_display_enable_nbg2)
                                                 == TransparentDisplayEnable::transparency_code_valid);

            // Priority
            screen.priority_number = prinb_.get(PriorityNumberB::nbg2);

            // Map
            screen.map_size   = map_size_nbg;
            screen.map_offset = mpofn_.get(MapOffsetNbg::map_offset_nbg2);

            // Plane
            screen.plane_size            = plsz_.get(PlaneSizeRegister::plane_size_nbg2);
            screen.plane_a_start_address = calculatePlaneStartAddress(s, mpabn2_.get(MapNbg2PlaneAB::plane_a));
            screen.plane_b_start_address = calculatePlaneStartAddress(s, mpabn2_.get(MapNbg2PlaneAB::plane_b));
            screen.plane_c_start_address = calculatePlaneStartAddress(s, mpabn2_.get(MapNbg2PlaneCD::plane_c));
            screen.plane_d_start_address = calculatePlaneStartAddress(s, mpabn2_.get(MapNbg2PlaneCD::plane_d));

            // Page
            screen.page_size = getPageSize(pncn2_.get(PatternNameControlNbg2::pattern_name_data_size),
                                           chctlb_.get(CharacterControlB::character_size_nbg2));

            // Pattern name data
            screen.pattern_name_data_size           = pncn2_.get(PatternNameControlNbg2::pattern_name_data_size);
            screen.character_number_supplement_mode = pncn2_.get(PatternNameControlNbg2::character_number_mode);
            screen.special_priority                 = pncn2_.get(PatternNameControlNbg2::special_priority);
            screen.special_color_calculation        = pncn2_.get(PatternNameControlNbg2::special_color_calculation);
            screen.supplementary_palette_number     = pncn2_.get(PatternNameControlNbg2::supplementary_palette_number);
            screen.supplementary_character_number   = pncn2_.get(PatternNameControlNbg2::supplementary_character_number);

            // Character pattern
            screen.character_pattern_size = chctlb_.get(CharacterControlB::character_size_nbg2);
            screen.character_color_number
                = getCharacterColorNumber1Bit(chctlb_.get(CharacterControlB::character_color_number_nbg2));
            screen.bitmap_size          = BitmapSize::not_set;
            screen.bitmap_start_address = 0;

            // Cell
            screen.cell_size = cell_size * getDotSize(screen.character_color_number) / bits_in_a_byte;

            // Scroll screen
            screen.screen_scroll_horizontal_integer = scxn2_.get(ScreenScrollValueNbg2Horizontal::horizontal);
            screen.screen_scroll_vertical_integer   = scyn2_.get(ScreenScrollValueNbg2Vertical::vertical);

            break;

        case ScrollScreen::nbg3:
            // Color RAM
            screen.color_ram_address_offset
                = getColorRamAddressOffset(craofa_.get(ColorRamAddressOffsetA::color_ram_address_offset_nbg3));

            // Transparency
            screen.is_transparency_code_valid = (bgon_.get(ScreenDisplayEnable::transparency_display_enable_nbg3)
                                                 == TransparentDisplayEnable::transparency_code_valid);

            // Priority
            screen.priority_number = prinb_.get(PriorityNumberB::nbg3);

            // Map
            screen.map_size   = map_size_nbg;
            screen.map_offset = mpofn_.get(MapOffsetNbg::map_offset_nbg3);

            // Plane
            screen.plane_size            = plsz_.get(PlaneSizeRegister::plane_size_nbg3);
            screen.plane_a_start_address = calculatePlaneStartAddress(s, mpabn3_.get(MapNbg3PlaneAB::plane_a));
            screen.plane_b_start_address = calculatePlaneStartAddress(s, mpabn3_.get(MapNbg3PlaneAB::plane_b));
            screen.plane_c_start_address = calculatePlaneStartAddress(s, mpabn3_.get(MapNbg3PlaneCD::plane_c));
            screen.plane_d_start_address = calculatePlaneStartAddress(s, mpabn3_.get(MapNbg3PlaneCD::plane_d));

            // Page
            screen.page_size = getPageSize(pncn3_.get(PatternNameControlNbg3::pattern_name_data_size),
                                           chctlb_.get(CharacterControlB::character_size_nbg3));

            // Pattern name data
            screen.pattern_name_data_size           = pncn3_.get(PatternNameControlNbg3::pattern_name_data_size);
            screen.character_number_supplement_mode = pncn3_.get(PatternNameControlNbg3::character_number_mode);
            screen.special_priority                 = pncn3_.get(PatternNameControlNbg3::special_priority);
            screen.special_color_calculation        = pncn3_.get(PatternNameControlNbg3::special_color_calculation);
            screen.supplementary_palette_number     = pncn3_.get(PatternNameControlNbg3::supplementary_palette_number);
            screen.supplementary_character_number   = pncn3_.get(PatternNameControlNbg3::supplementary_character_number);

            // Character pattern
            screen.character_pattern_size = chctlb_.get(CharacterControlB::character_size_nbg3);
            screen.character_color_number
                = getCharacterColorNumber1Bit(chctlb_.get(CharacterControlB::character_color_number_nbg3));
            screen.bitmap_size          = BitmapSize::not_set;
            screen.bitmap_start_address = 0;

            // Cell
            screen.cell_size = cell_size * getDotSize(screen.character_color_number) / bits_in_a_byte;

            // Scroll screen
            screen.screen_scroll_horizontal_integer = scxn3_.get(ScreenScrollValueNbg3Horizontal::horizontal);
            screen.screen_scroll_vertical_integer   = scyn3_.get(ScreenScrollValueNbg3Vertical::vertical);

            break;

        case ScrollScreen::rbg0:
            // Color RAM
            screen.color_ram_address_offset
                = getColorRamAddressOffset(craofb_.get(ColorRamAddressOffsetB::color_ram_address_offset_rbg0));

            // Transparency
            screen.is_transparency_code_valid = (bgon_.get(ScreenDisplayEnable::transparency_display_enable_rbg0)
                                                 == TransparentDisplayEnable::transparency_code_valid);

            // Priority
            screen.priority_number = prina_.get(PriorityNumberA::nbg0);

            // Map
            screen.map_size   = map_size_rbg;
            screen.map_offset = mpofr_.get(MapOffsetRbg::map_offset_rpa);

            // Plane
            screen.plane_size            = plsz_.get(PlaneSizeRegister::plane_size_rpa);
            screen.plane_a_start_address = calculatePlaneStartAddress(s, mpabra_.get(MapRotationParameterAPlaneAB::plane_a));
            screen.plane_b_start_address = calculatePlaneStartAddress(s, mpabra_.get(MapRotationParameterAPlaneAB::plane_b));
            screen.plane_c_start_address = calculatePlaneStartAddress(s, mpabra_.get(MapRotationParameterAPlaneCD::plane_c));
            screen.plane_d_start_address = calculatePlaneStartAddress(s, mpabra_.get(MapRotationParameterAPlaneCD::plane_d));
            screen.plane_e_start_address = calculatePlaneStartAddress(s, mpabra_.get(MapRotationParameterAPlaneEF::plane_e));
            screen.plane_f_start_address = calculatePlaneStartAddress(s, mpabra_.get(MapRotationParameterAPlaneEF::plane_f));
            screen.plane_g_start_address = calculatePlaneStartAddress(s, mpabra_.get(MapRotationParameterAPlaneGH::plane_g));
            screen.plane_h_start_address = calculatePlaneStartAddress(s, mpabra_.get(MapRotationParameterAPlaneGH::plane_h));
            screen.plane_i_start_address = calculatePlaneStartAddress(s, mpabra_.get(MapRotationParameterAPlaneIJ::plane_i));
            screen.plane_j_start_address = calculatePlaneStartAddress(s, mpabra_.get(MapRotationParameterAPlaneIJ::plane_j));
            screen.plane_k_start_address = calculatePlaneStartAddress(s, mpabra_.get(MapRotationParameterAPlaneKL::plane_k));
            screen.plane_l_start_address = calculatePlaneStartAddress(s, mpabra_.get(MapRotationParameterAPlaneKL::plane_l));
            screen.plane_m_start_address = calculatePlaneStartAddress(s, mpabra_.get(MapRotationParameterAPlaneMN::plane_m));
            screen.plane_n_start_address = calculatePlaneStartAddress(s, mpabra_.get(MapRotationParameterAPlaneMN::plane_n));
            screen.plane_o_start_address = calculatePlaneStartAddress(s, mpabra_.get(MapRotationParameterAPlaneOP::plane_o));
            screen.plane_p_start_address = calculatePlaneStartAddress(s, mpabra_.get(MapRotationParameterAPlaneOP::plane_p));

            // Page
            screen.page_size = getPageSize(pncr_.get(PatternNameControlRbg0::pattern_name_data_size),
                                           chctlb_.get(CharacterControlB::character_size_rbg0));

            // Pattern name data
            screen.pattern_name_data_size           = pncr_.get(PatternNameControlRbg0::pattern_name_data_size);
            screen.character_number_supplement_mode = pncr_.get(PatternNameControlRbg0::character_number_mode);
            screen.special_priority                 = pncr_.get(PatternNameControlRbg0::special_priority);
            screen.special_color_calculation        = pncr_.get(PatternNameControlRbg0::special_color_calculation);
            screen.supplementary_palette_number     = pncr_.get(PatternNameControlRbg0::supplementary_palette_number);
            screen.supplementary_character_number   = pncr_.get(PatternNameControlRbg0::supplementary_character_number);

            // Character pattern
            screen.character_pattern_size = chctlb_.get(CharacterControlB::character_size_rbg0);
            screen.character_color_number
                = getCharacterColorNumberRbg0(chctlb_.get(CharacterControlB::character_color_number_rbg0),
                                              tv_screen_status_.screen_mode_type);
            screen.format = getScrollScreenFormat(chctlb_.get(CharacterControlB::bitmap_enable_rbg0));

            // Cell
            screen.cell_size = cell_size * getDotSize(screen.character_color_number) / bits_in_a_byte;

            // Bitmap
            screen.bitmap_size = getBitmapSize(static_cast<BitmapSize2Bits>(chctlb_.get(CharacterControlB::bitmap_size_rbg0)));
            screen.bitmap_palette_number            = bmpnb_.get(BitmapPaletteNumberB::bitmap_palette_number_rbg0);
            screen.bitmap_special_priority          = bmpnb_.get(BitmapPaletteNumberB::bitmap_special_priority_rbg0);
            screen.bitmap_special_color_calculation = bmpnb_.get(BitmapPaletteNumberB::bitmap_special_color_calculation_rbg0);
            screen.bitmap_start_address             = getBitmapStartAddress(screen.map_offset);
            break;

        case ScrollScreen::rbg1:
            // Color RAM
            screen.color_ram_address_offset
                = getColorRamAddressOffset(craofa_.get(ColorRamAddressOffsetA::color_ram_address_offset_nbg0));

            // Transparency
            screen.is_transparency_code_valid = (bgon_.get(ScreenDisplayEnable::transparency_display_enable_nbg0)
                                                 == TransparentDisplayEnable::transparency_code_valid);

            // Priority
            screen.priority_number = prina_.get(PriorityNumberR::rbg0);

            // Map
            screen.map_size   = map_size_rbg;
            screen.map_offset = mpofr_.get(MapOffsetRbg::map_offset_rpb);

            // Plane
            screen.plane_size            = plsz_.get(PlaneSizeRegister::plane_size_rpb);
            screen.plane_a_start_address = calculatePlaneStartAddress(s, mpabrb_.get(MapRotationParameterBPlaneAB::plane_a));
            screen.plane_b_start_address = calculatePlaneStartAddress(s, mpabrb_.get(MapRotationParameterBPlaneAB::plane_b));
            screen.plane_c_start_address = calculatePlaneStartAddress(s, mpabrb_.get(MapRotationParameterBPlaneCD::plane_c));
            screen.plane_d_start_address = calculatePlaneStartAddress(s, mpabrb_.get(MapRotationParameterBPlaneCD::plane_d));
            screen.plane_e_start_address = calculatePlaneStartAddress(s, mpabrb_.get(MapRotationParameterBPlaneEF::plane_e));
            screen.plane_f_start_address = calculatePlaneStartAddress(s, mpabrb_.get(MapRotationParameterBPlaneEF::plane_f));
            screen.plane_g_start_address = calculatePlaneStartAddress(s, mpabrb_.get(MapRotationParameterBPlaneGH::plane_g));
            screen.plane_h_start_address = calculatePlaneStartAddress(s, mpabrb_.get(MapRotationParameterBPlaneGH::plane_h));
            screen.plane_i_start_address = calculatePlaneStartAddress(s, mpabrb_.get(MapRotationParameterBPlaneIJ::plane_i));
            screen.plane_j_start_address = calculatePlaneStartAddress(s, mpabrb_.get(MapRotationParameterBPlaneIJ::plane_j));
            screen.plane_k_start_address = calculatePlaneStartAddress(s, mpabrb_.get(MapRotationParameterBPlaneKL::plane_k));
            screen.plane_l_start_address = calculatePlaneStartAddress(s, mpabrb_.get(MapRotationParameterBPlaneKL::plane_l));
            screen.plane_m_start_address = calculatePlaneStartAddress(s, mpabrb_.get(MapRotationParameterBPlaneMN::plane_m));
            screen.plane_n_start_address = calculatePlaneStartAddress(s, mpabrb_.get(MapRotationParameterBPlaneMN::plane_n));
            screen.plane_o_start_address = calculatePlaneStartAddress(s, mpabrb_.get(MapRotationParameterBPlaneOP::plane_o));
            screen.plane_p_start_address = calculatePlaneStartAddress(s, mpabrb_.get(MapRotationParameterBPlaneOP::plane_p));

            // Page
            screen.page_size = getPageSize(pncn0_.get(PatternNameControlNbg0::pattern_name_data_size),
                                           chctla_.get(CharacterControlA::character_size_nbg0));

            // Pattern name data
            screen.pattern_name_data_size           = pncn0_.get(PatternNameControlNbg0::pattern_name_data_size);
            screen.character_number_supplement_mode = pncn0_.get(PatternNameControlNbg0::character_number_mode);
            screen.special_priority                 = pncn0_.get(PatternNameControlNbg0::special_priority);
            screen.special_color_calculation        = pncn0_.get(PatternNameControlNbg0::special_color_calculation);
            screen.supplementary_palette_number     = pncn0_.get(PatternNameControlNbg0::supplementary_palette_number);
            screen.supplementary_character_number   = pncn0_.get(PatternNameControlNbg0::supplementary_character_number);

            // Character pattern
            screen.character_pattern_size = chctla_.get(CharacterControlA::character_size_nbg0);
            screen.character_color_number
                = getCharacterColorNumber3Bits(chctla_.get(CharacterControlA::character_color_number_nbg0),
                                               tv_screen_status_.screen_mode_type);
            screen.bitmap_size          = BitmapSize::not_set;
            screen.bitmap_start_address = 0;

            // Cell
            screen.cell_size = cell_size * getDotSize(screen.character_color_number) / bits_in_a_byte;
            break;
    }

    // Position calculation helpers
    constexpr auto cells_nb_64             = u16{64};
    constexpr auto cells_nb_128            = u16{128};
    screen.character_pattern_screen_offset = [](const CharacterSize sz) {
        return (sz == CharacterSize::one_by_one) ? ScreenOffset{1, 1} : ScreenOffset{2, 2};
    }(screen.character_pattern_size);
    screen.page_screen_offset  = {cells_nb_64, cells_nb_64};
    screen.plane_screen_offset = [&](const PlaneSize sz) {
        switch (sz) {
            case PlaneSize::size_1_by_1: return ScreenOffset{cells_nb_64, cells_nb_64};
            case PlaneSize::size_2_by_1: return ScreenOffset{cells_nb_128, cells_nb_64};
            case PlaneSize::size_2_by_2: return ScreenOffset{cells_nb_128, cells_nb_128};
            default: Log::warning(Logger::vdp2, tr("Plane screen offset wasn't properly calculated"));
        }
        return ScreenOffset{0, 0};
    }(screen.plane_size);
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto Vdp2::calculatePlaneStartAddress(const ScrollScreen s, const u32 map_addr) -> u32 {
    constexpr auto multiplier_800   = u32{0x800};
    constexpr auto multiplier_1000  = u32{0x1000};
    constexpr auto multiplier_2000  = u32{0x2000};
    constexpr auto multiplier_4000  = u32{0x4000};
    constexpr auto multiplier_8000  = u32{0x8000};
    constexpr auto multiplier_10000 = u32{0x10000};

    auto&      screen                 = getScreen(s);
    const auto is_vram_size_4mb       = (vrsize_.get(VramSizeRegister::vram_size) == VramSize::size_4_mbits);
    auto       plane_size             = PlaneSize{};
    auto       pattern_name_data_size = PatternNameDataSize{};
    auto       character_size         = CharacterSize{};
    auto       start_address          = u32{screen.map_offset << displacement_6 | map_addr};

    switch (s) {
        case ScrollScreen::nbg0:
            plane_size             = plsz_.get(PlaneSizeRegister::plane_size_nbg0);
            pattern_name_data_size = pncn0_.get(PatternNameControlNbg0::pattern_name_data_size);
            character_size         = chctla_.get(CharacterControlA::character_size_nbg0);
            break;
        case ScrollScreen::nbg1:
            plane_size             = plsz_.get(PlaneSizeRegister::plane_size_nbg1);
            pattern_name_data_size = pncn1_.get(PatternNameControlNbg1::pattern_name_data_size);
            character_size         = chctla_.get(CharacterControlA::character_size_nbg1);
            break;
        case ScrollScreen::nbg2:
            plane_size             = plsz_.get(PlaneSizeRegister::plane_size_nbg2);
            pattern_name_data_size = pncn2_.get(PatternNameControlNbg2::pattern_name_data_size);
            character_size         = chctlb_.get(CharacterControlB::character_size_nbg2);
            break;
        case ScrollScreen::nbg3:
            plane_size             = plsz_.get(PlaneSizeRegister::plane_size_nbg3);
            pattern_name_data_size = pncn3_.get(PatternNameControlNbg3::pattern_name_data_size);
            character_size         = chctlb_.get(CharacterControlB::character_size_nbg3);
            break;
        case ScrollScreen::rbg0:
            plane_size             = plsz_.get(PlaneSizeRegister::plane_size_rpa);
            pattern_name_data_size = pncr_.get(PatternNameControlRbg0::pattern_name_data_size);
            character_size         = chctlb_.get(CharacterControlB::character_size_rbg0);
            break;
        case ScrollScreen::rbg1:
            plane_size             = plsz_.get(PlaneSizeRegister::plane_size_rpb);
            pattern_name_data_size = pncn0_.get(PatternNameControlNbg0::pattern_name_data_size);
            character_size         = chctla_.get(CharacterControlA::character_size_nbg0);
            break;
        default: break;
    }

    auto mask       = u32{};
    auto multiplier = u32{};
    switch (plane_size) {
        case PlaneSize::size_1_by_1:
            if (pattern_name_data_size == PatternNameDataSize::one_word) {
                if (character_size == CharacterSize::one_by_one) {
                    constexpr auto mask_4mb = u16{0x003f};
                    constexpr auto mask_8mb = u16{0x007f};
                    mask                    = (is_vram_size_4mb) ? mask_4mb : mask_8mb;
                    multiplier              = multiplier_2000;
                } else {
                    // CharacterSize::two_by_two
                    constexpr auto mask_4mb = u16{0x00ff};
                    constexpr auto mask_8mb = u16{0x01ff};
                    mask                    = (is_vram_size_4mb) ? mask_4mb : mask_8mb;
                    multiplier              = multiplier_800;
                }
            } else {
                // PatternNameDataSize::two_words
                if (character_size == CharacterSize::one_by_one) {
                    constexpr auto mask_4mb = u16{0x001f};
                    constexpr auto mask_8mb = u16{0x003f};
                    mask                    = (is_vram_size_4mb) ? mask_4mb : mask_8mb;
                    multiplier              = multiplier_4000;
                } else {
                    // CharacterSize::two_by_two
                    constexpr auto mask_4mb = u16{0x007f};
                    constexpr auto mask_8mb = u16{0x00ff};
                    mask                    = (is_vram_size_4mb) ? mask_4mb : mask_8mb;
                    multiplier              = multiplier_1000;
                }
            }
            return vram_start_address + (start_address & mask) * multiplier;
            break;
        case PlaneSize::size_2_by_1:
            if (pattern_name_data_size == PatternNameDataSize::one_word) {
                if (character_size == CharacterSize::one_by_one) {
                    constexpr auto mask_4mb = u16{0x003e};
                    constexpr auto mask_8mb = u16{0x007e};
                    mask                    = (is_vram_size_4mb) ? mask_4mb : mask_8mb;
                    multiplier              = multiplier_4000;
                } else {
                    // CharacterSize::two_by_two
                    constexpr auto mask_4mb = u16{0x00fe};
                    constexpr auto mask_8mb = u16{0x01fe};
                    mask                    = (is_vram_size_4mb) ? mask_4mb : mask_8mb;
                    multiplier              = multiplier_1000;
                }
            } else {
                // PatternNameDataSize::two_words
                if (character_size == CharacterSize::one_by_one) {
                    constexpr auto mask_4mb = u16{0x001e};
                    constexpr auto mask_8mb = u16{0x003e};
                    mask                    = (is_vram_size_4mb) ? mask_4mb : mask_8mb;
                    multiplier              = multiplier_8000;
                } else {
                    // CharacterSize::two_by_two
                    constexpr auto mask_4mb = u16{0x007e};
                    constexpr auto mask_8mb = u16{0x00fe};
                    mask                    = (is_vram_size_4mb) ? mask_4mb : mask_8mb;
                    multiplier              = multiplier_2000;
                }
            }
            return vram_start_address + ((start_address & mask) >> 1) * multiplier;
            break;
        case PlaneSize::size_2_by_2:
            if (pattern_name_data_size == PatternNameDataSize::one_word) {
                if (character_size == CharacterSize::one_by_one) {
                    constexpr auto mask_4mb = u16{0x003c};
                    constexpr auto mask_8mb = u16{0x007c};
                    mask                    = (is_vram_size_4mb) ? mask_4mb : mask_8mb;
                    multiplier              = multiplier_8000;
                } else {
                    // CharacterSize::two_by_two
                    constexpr auto mask_4mb = u16{0x00fc};
                    constexpr auto mask_8mb = u16{0x01fc};
                    mask                    = (is_vram_size_4mb) ? mask_4mb : mask_8mb;
                    multiplier              = multiplier_2000;
                }
            } else {
                // PatternNameDataSize::two_words
                if (character_size == CharacterSize::one_by_one) {
                    constexpr auto mask_4mb = u16{0x001c};
                    constexpr auto mask_8mb = u16{0x003c};
                    mask                    = (is_vram_size_4mb) ? mask_4mb : mask_8mb;
                    multiplier              = multiplier_10000;
                } else {
                    // CharacterSize::two_by_two
                    constexpr auto mask_4mb = u16{0x007c};
                    constexpr auto mask_8mb = u16{0x00fc};
                    mask                    = (is_vram_size_4mb) ? mask_4mb : mask_8mb;
                    multiplier              = multiplier_4000;
                }
            }
            return vram_start_address + ((start_address & mask) >> 2) * multiplier;
            break;
        default: Log::warning(Logger::vdp2, tr("Plane start address wasn't properly calculated")); return 0;
    }
}

auto Vdp2::getScreen(const ScrollScreen s) -> ScrollScreenStatus& { return bg_[util::toUnderlying(s)]; };
auto Vdp2::getScreen(const ScrollScreen s) const -> const ScrollScreenStatus& { return bg_[util::toUnderlying(s)]; };

void Vdp2::readScrollScreenData(const ScrollScreen s) {
    const auto& screen = getScreen(s);
    vdp2_parts_[util::toUnderlying(s)].clear();

    if (screen.format == ScrollScreenFormat::cell) {
        // Using a set to prevent calculating same address data multiple times
        auto unique_addresses = std::unordered_set<u32>{};
        auto start_addresses  = std::vector<std::pair<u32, ScreenOffset>>{};

        const auto addUniquePlane = [&](const u32 address, const ScreenOffset& offset) {
            if (unique_addresses.insert(address).second) { start_addresses.emplace_back(address, offset); }
        };

        const auto offset_x         = screen.plane_screen_offset.x;
        const auto offset_y         = screen.plane_screen_offset.y;
        const auto rotation_screens = std::array{ScrollScreen::rbg0, ScrollScreen::rbg1};
        if (std::none_of(rotation_screens.begin(), rotation_screens.end(), [&s](const ScrollScreen rss) { return rss == s; })) {
            // For NBG
            addUniquePlane(screen.plane_a_start_address, ScreenOffset{0, 0});
            addUniquePlane(screen.plane_b_start_address, ScreenOffset{offset_x, 0});
            addUniquePlane(screen.plane_c_start_address, ScreenOffset{0, offset_y});
            addUniquePlane(screen.plane_d_start_address, ScreenOffset{offset_x, offset_y});
        } else {
            // For RBG
            addUniquePlane(screen.plane_a_start_address, ScreenOffset{0, 0});
            addUniquePlane(screen.plane_b_start_address, ScreenOffset{offset_x, 0});
            addUniquePlane(screen.plane_c_start_address, ScreenOffset{static_cast<u16>(offset_x * 2), 0});
            addUniquePlane(screen.plane_d_start_address, ScreenOffset{static_cast<u16>(offset_x * 3), 0});
            addUniquePlane(screen.plane_e_start_address, ScreenOffset{0, offset_y});
            addUniquePlane(screen.plane_f_start_address, ScreenOffset{offset_x, offset_y});
            addUniquePlane(screen.plane_g_start_address, ScreenOffset{static_cast<u16>(offset_x * 2), offset_y});
            addUniquePlane(screen.plane_h_start_address, ScreenOffset{static_cast<u16>(offset_x * 3), offset_y});
            addUniquePlane(screen.plane_i_start_address, ScreenOffset{0, static_cast<u16>(offset_y * 2)});
            addUniquePlane(screen.plane_j_start_address, ScreenOffset{offset_x, static_cast<u16>(offset_y * 2)});
            addUniquePlane(screen.plane_k_start_address,
                           ScreenOffset{static_cast<u16>(offset_x * 2), static_cast<u16>(offset_y * 2)});
            addUniquePlane(screen.plane_l_start_address,
                           ScreenOffset{static_cast<u16>(offset_x * 3), static_cast<u16>(offset_y * 2)});
            addUniquePlane(screen.plane_m_start_address, ScreenOffset{0, static_cast<u16>(offset_y * 3)});
            addUniquePlane(screen.plane_n_start_address, ScreenOffset{offset_x, static_cast<u16>(offset_y * 3)});
            addUniquePlane(screen.plane_o_start_address,
                           ScreenOffset{static_cast<u16>(offset_x * 2), static_cast<u16>(offset_y * 3)});
            addUniquePlane(screen.plane_p_start_address,
                           ScreenOffset{static_cast<u16>(offset_x * 3), static_cast<u16>(offset_y * 3)});
        }

        // Unique addresses are handled
        for (const auto& [addr, offset] : start_addresses) {
            readPlaneData(screen, addr, offset);
        }
    } else { // ScrollScreenFormat::bitmap
        readBitmapData(screen);
    }
}

void Vdp2::readBitmapData(const ScrollScreenStatus& screen) {
    constexpr auto width_512      = u16{512};
    constexpr auto width_1024     = u16{1024};
    constexpr auto height_256     = u16{256};
    constexpr auto height_512     = u16{512};
    auto           texture_width  = u16{};
    auto           texture_height = u16{};

    switch (screen.bitmap_size) {
        case BitmapSize::size_1024_by_256: {
            texture_width  = width_1024;
            texture_height = height_256;
            break;
        }
        case BitmapSize::size_1024_by_512: {
            texture_width  = width_1024;
            texture_height = height_512;
            break;
        }
        case BitmapSize::size_512_by_256: {
            texture_width  = width_512;
            texture_height = height_256;
            break;
        }
        case BitmapSize::size_512_by_512: {
            texture_width  = width_512;
            texture_height = height_512;
            break;
        }
    }
    const auto      texture_size = texture_width * texture_height * 4;
    std::vector<u8> texture_data;
    texture_data.reserve(texture_size);
    const auto key = Texture::calculateKey(VdpType::vdp2,
                                           screen.bitmap_start_address,
                                           toUnderlying(screen.character_color_number),
                                           screen.bitmap_palette_number);

    // if (!Texture::isTextureStored(key)) {
    if (Texture::isTextureLoadingNeeded(key)) {
        if (ram_status_.color_ram_mode == ColorRamMode::mode_2_rgb_8_bits_1024_colors) {
            // 32 bits access to color RAM
            switch (screen.character_color_number) {
                case ColorCount::palette_16: {
                    break;
                }
                case ColorCount::palette_256: {
                    read256ColorsBitmapData<u32>(texture_data, screen);
                    break;
                }
                case ColorCount::palette_2048: {
                    break;
                }
                case ColorCount::rgb_32k: {
                    break;
                }
                case ColorCount::rgb_16m: {
                    break;
                }
                default: {
                    Log::warning(Logger::vdp2, tr("Character color number invalid !"));
                }
            }
        } else {
            // 16 bits access to color RAM
            switch (screen.character_color_number) {
                case ColorCount::palette_16: {
                    break;
                }
                case ColorCount::palette_256: {
                    read256ColorsBitmapData<u16>(texture_data, screen);
                    break;
                }
                case ColorCount::palette_2048: {
                    break;
                }
                case ColorCount::rgb_32k: {
                    break;
                }
                case ColorCount::rgb_16m: {
                    break;
                }
                default: {
                    Log::warning(Logger::vdp2, tr("Character color number invalid !"));
                }
            }
        }
        saveBitmap(screen, texture_data, texture_width, texture_height, key);
    }
}

void Vdp2::saveBitmap(const ScrollScreenStatus& screen,
                      std::vector<u8>&          texture_data,
                      const u16                 width,
                      const u16                 height,
                      const size_t              key) {
    if (!Texture::isTextureStored(key)) {
        Texture::storeTexture(Texture(VdpType::vdp2,
                                      screen.bitmap_start_address,
                                      toUnderlying(screen.character_color_number),
                                      screen.bitmap_palette_number,
                                      texture_data,
                                      width,
                                      height));
    }
    auto pos = ScreenPos{0, 0};

    auto p = Vdp2Part(key, width, height);
    p.priority(screen.priority_number);
    vdp2_parts_[util::toUnderlying(screen.scroll_screen)].push_back(std::move(p));
}

void Vdp2::readPlaneData(const ScrollScreenStatus& screen, const u32 plane_address, const ScreenOffset& plane_offset) {
    auto page_start_address = plane_address;
    switch (screen.plane_size) {
        case PlaneSize::size_1_by_1: readPageData(screen, page_start_address, plane_offset); break;
        case PlaneSize::size_2_by_1: {
            readPageData(screen, page_start_address, plane_offset);
            page_start_address += screen.page_size;
            const auto page_offset_x = static_cast<u16>(plane_offset.x + screen.page_screen_offset.x);
            readPageData(screen, page_start_address, ScreenOffset{page_offset_x, plane_offset.y});
            break;
        }
        case PlaneSize::size_2_by_2: {
            readPageData(screen, page_start_address, plane_offset);
            page_start_address += screen.page_size;
            const auto page_offset_x = static_cast<u16>(plane_offset.x + screen.page_screen_offset.x);
            readPageData(screen, page_start_address, ScreenOffset{page_offset_x, plane_offset.y});
            page_start_address += screen.page_size;
            const auto page_offset_y = static_cast<u16>(plane_offset.y + screen.page_screen_offset.y);
            readPageData(screen, page_start_address, ScreenOffset{plane_offset.x, page_offset_y});
            page_start_address += screen.page_size;
            readPageData(screen, page_start_address, ScreenOffset{page_offset_x, page_offset_y});

            break;
        }
        default: Log::warning(Logger::vdp2, tr("Plane size invalid !"));
    }
}

void Vdp2::readPageData(const ScrollScreenStatus& screen, const u32 page_address, const ScreenOffset& page_offset) {
    // Getting the right function depending on the pattern name data configuration.
    static auto current_pnd_config = PatternNameDataEnum{};

    if (screen.pattern_name_data_size == PatternNameDataSize::two_words) {
        current_pnd_config = PatternNameDataEnum::two_words;
    } else {
        if (screen.character_pattern_size == CharacterSize::one_by_one) {
            if (screen.character_color_number == ColorCount::palette_16) {
                if (screen.character_number_supplement_mode == CharacterNumberSupplementMode::character_number_10_bits) {
                    current_pnd_config = PatternNameDataEnum::one_word_1_cell_16_colors_10_bits;
                } else { // CharacterNumberSupplementMode::character_number_12_bits
                    current_pnd_config = PatternNameDataEnum::one_word_1_cell_16_colors_12_bits;
                }
            } else { // Over 16 colors
                if (screen.character_number_supplement_mode == CharacterNumberSupplementMode::character_number_10_bits) {
                    current_pnd_config = PatternNameDataEnum::one_word_1_cell_over_16_colors_10_bits;
                } else { // CharacterNumberSupplementMode::character_number_12_bits
                    current_pnd_config = PatternNameDataEnum::one_word_1_cell_over_16_colors_12_bits;
                }
            }
        } else { // CharacterSize::two_by_two
            if (screen.character_color_number == ColorCount::palette_16) {
                if (screen.character_number_supplement_mode == CharacterNumberSupplementMode::character_number_10_bits) {
                    current_pnd_config = PatternNameDataEnum::one_word_4_cells_16_colors_10_bits;
                } else { // CharacterNumberSupplementMode::character_number_12_bits
                    current_pnd_config = PatternNameDataEnum::one_word_4_cells_16_colors_12_bits;
                }
            } else { // Over 16 colors
                if (screen.character_number_supplement_mode == CharacterNumberSupplementMode::character_number_10_bits) {
                    current_pnd_config = PatternNameDataEnum::one_word_4_cells_over_16_colors_10_bits;
                } else { // CharacterNumberSupplementMode::character_number_12_bits
                    current_pnd_config = PatternNameDataEnum::one_word_4_cells_over_16_colors_12_bits;
                }
            }
        }
    }

    // Assigning the current configuration function to a function pointer
    // Not using std::function here because of the performance cost.
    using PatterNameDataFunc = PatternNameData (*)(const u32, const ScrollScreenStatus&);
    auto readPatternNameData = PatterNameDataFunc();
    switch (current_pnd_config) {
        case PatternNameDataEnum::two_words:
            readPatternNameData = reinterpret_cast<PatterNameDataFunc>(&getPatternNameData2Words);
            break;
        case PatternNameDataEnum::one_word_1_cell_16_colors_10_bits:
            readPatternNameData = reinterpret_cast<PatterNameDataFunc>(&getPatternNameData1Word1Cell16Colors10Bits);
            break;
        case PatternNameDataEnum::one_word_1_cell_16_colors_12_bits:
            readPatternNameData = reinterpret_cast<PatterNameDataFunc>(&getPatternNameData1Word1Cell16Colors12Bits);
            break;
        case PatternNameDataEnum::one_word_1_cell_over_16_colors_10_bits:
            readPatternNameData = reinterpret_cast<PatterNameDataFunc>(&getPatternNameData1Word1CellOver16Colors10Bits);
            break;
        case PatternNameDataEnum::one_word_1_cell_over_16_colors_12_bits:
            readPatternNameData = reinterpret_cast<PatterNameDataFunc>(&getPatternNameData1Word1CellOver16Colors12Bits);
            break;
        case PatternNameDataEnum::one_word_4_cells_16_colors_10_bits:
            readPatternNameData = reinterpret_cast<PatterNameDataFunc>(&getPatternNameData1Word4Cells16Colors10Bits);
            break;
        case PatternNameDataEnum::one_word_4_cells_16_colors_12_bits:
            readPatternNameData = reinterpret_cast<PatterNameDataFunc>(&getPatternNameData1Word4Cells16Colors12Bits);
            break;
        case PatternNameDataEnum::one_word_4_cells_over_16_colors_10_bits:
            readPatternNameData = reinterpret_cast<PatterNameDataFunc>(&getPatternNameData1Word4CellsOver16Colors10Bits);
            break;
        case PatternNameDataEnum::one_word_4_cells_over_16_colors_12_bits:
            readPatternNameData = reinterpret_cast<PatterNameDataFunc>(&getPatternNameData1Word4CellsOver16Colors12Bits);
            break;
    }

    const auto cp_number   = (screen.character_pattern_size == CharacterSize::one_by_one) ? u32{64 * 64} : u32{32 * 32};
    const auto cp_width    = (screen.character_pattern_size == CharacterSize::one_by_one) ? u32{1} : u32{2};
    const auto cp_height   = cp_width;
    const auto pnd_size    = (screen.pattern_name_data_size == PatternNameDataSize::one_word) ? 2 : 4;
    auto       pnd_address = page_address;
    auto       cp_offset   = page_offset;

    // Choosing the right precalculated modulo check function
    const auto& modulo_values
        = (screen.character_pattern_size == CharacterSize::one_by_one) ? pre_calculated_modulo_64_ : pre_calculated_modulo_32_;

    const auto isEndOfRowReached = [](const std::vector<u32>& modulos, const u32 val) {
        // Value 0 isn't added to the vector.
        return std::any_of(modulos.begin(), modulos.end(), [&](const u32 m) { return m == val; });
    };

    const auto character_number_mask
        = (ram_status_.vram_size == VramSize::size_4_mbits) ? bitmask_vdp2_vram_4mb : bitmask_vdp2_vram_8mb;

    for (u32 i = 0; i < cp_number; ++i) {
        const auto raw_data
            = (pnd_size == 2) ? modules_.memory()->read<u16>(pnd_address) : modules_.memory()->read<u32>(pnd_address);
        // if (raw_data != 0x0) __debugbreak();
        auto pn_data = readPatternNameData(raw_data, screen);
        pn_data.character_number &= character_number_mask;

        if (isEndOfRowReached(modulo_values, i)) {
            cp_offset.x = page_offset.x;
            cp_offset.y += cp_height;
        }

        readCharacterPattern(screen, pn_data, cp_offset);

        cp_offset.x += cp_width;
        pnd_address += pnd_size;
    }
}

void Vdp2::readCharacterPattern(const ScrollScreenStatus& screen, const PatternNameData& pnd, const ScreenOffset& cp_offset) {
    constexpr auto character_pattern_boundary = u8{0x20};
    // if (pnd.character_number != 0x3000) __debugbreak();
    const auto character_number_address = pnd.character_number * character_pattern_boundary;
    if (screen.character_pattern_size == CharacterSize::one_by_one) {
        readCell(screen, pnd, character_number_address, cp_offset);
    } else { // CharacterSize::two_by_two

        const auto pnd_offset           = (screen.pattern_name_data_size == PatternNameDataSize::one_word) ? 2 : 4;
        auto       current_cell_address = character_number_address;
        auto       cells_address        = std::vector<u32>{};

        const auto cell_number = u32{4};
        for (u32 i = 0; i < cell_number; ++i) {
            cells_address.emplace_back(current_cell_address);
            current_cell_address += pnd_offset;
        }

        const auto readCells = [&](const std::array<u8, cell_number>& cells_pos) {
            auto cell_offset = cp_offset;
            readCell(screen, pnd, cells_address[cells_pos[0]], cell_offset);
            cell_offset.x++;
            readCell(screen, pnd, cells_address[cells_pos[1]], cell_offset);
            cell_offset.x = cp_offset.x;
            cell_offset.y++;
            readCell(screen, pnd, cells_address[cells_pos[2]], cell_offset);
            cell_offset.x++;
            readCell(screen, pnd, cells_address[cells_pos[3]], cell_offset);
        };

        // Depending on the inversion, cells have to be read in a different order.
        // Flipping itself will be done while rendering.
        if (!pnd.is_horizontally_flipped && !pnd.is_vertically_flipped) {
            // Cells layout
            //  |0|1|
            //  |2|3|
            readCells({0, 1, 2, 3});
            return;
        }
        if (pnd.is_horizontally_flipped && pnd.is_vertically_flipped) {
            // Cells layout
            // |3|2|
            // |1|0|
            readCells({3, 2, 1, 0});
            return;
        }

        if (pnd.is_horizontally_flipped) {
            // Cells layout
            // |1|0|
            // |3|2|
            readCells({1, 0, 3, 2});
            return;
        }
        if (pnd.is_vertically_flipped) {
            // Cells layout
            // |2|3|
            // |0|1|
            readCells({2, 3, 0, 1});
            return;
        }
    }
}

void Vdp2::readCell(const ScrollScreenStatus& screen,
                    const PatternNameData&    pnd,
                    const u32                 cell_address,
                    const ScreenOffset&       cell_offset) {
    constexpr auto  texture_width  = u16{8};
    constexpr auto  texture_height = u16{8};
    constexpr auto  texture_size   = texture_width * texture_height * 4;
    std::vector<u8> texture_data;
    texture_data.reserve(texture_size);
    const auto key
        = Texture::calculateKey(VdpType::vdp2, cell_address, toUnderlying(screen.character_color_number), pnd.palette_number);

    //    if (!Texture::isTextureStored(key)) {
    if (Texture::isTextureLoadingNeeded(key)) {
        if (ram_status_.color_ram_mode == ColorRamMode::mode_2_rgb_8_bits_1024_colors) {
            // 32 bits access to color RAM
            switch (screen.character_color_number) {
                case ColorCount::palette_16: {
                    read16ColorsCellData<u32>(texture_data, screen, pnd.palette_number, cell_address);
                    break;
                }
                case ColorCount::palette_256: {
                    read256ColorsCellData<u32>(texture_data, screen, pnd.palette_number, cell_address);
                    break;
                }
                case ColorCount::palette_2048: {
                    break;
                }
                case ColorCount::rgb_32k: {
                    break;
                }
                case ColorCount::rgb_16m: {
                    break;
                }
                default: {
                    Log::warning(Logger::vdp2, tr("Character color number invalid !"));
                }
            }
        } else {
            // 16 bits access to color RAM
            switch (screen.character_color_number) {
                case ColorCount::palette_16: {
                    read16ColorsCellData<u16>(texture_data, screen, pnd.palette_number, cell_address);
                    break;
                }
                case ColorCount::palette_256: {
                    read256ColorsCellData<u16>(texture_data, screen, pnd.palette_number, cell_address);
                    break;
                }
                case ColorCount::palette_2048: {
                    break;
                }
                case ColorCount::rgb_32k: {
                    break;
                }
                case ColorCount::rgb_16m: {
                    break;
                }
                default: {
                    Log::warning(Logger::vdp2, tr("Character color number invalid !"));
                }
            }
        }
    }
    saveCell(screen, pnd, cell_address, cell_offset, texture_data, key);
    // Log::info("vdp2", "(Cell address : {:#x},{:#x})", cell_offset.x, cell_offset.y);
}

void Vdp2::saveCell(const ScrollScreenStatus& screen,
                    const PatternNameData&    pnd,
                    const u32                 cell_address,
                    const ScreenOffset&       cell_offset,
                    std::vector<u8>&          texture_data,
                    const size_t              key) {
    constexpr auto texture_width  = u16{8};
    constexpr auto texture_height = u16{8};

    if (!Texture::isTextureStored(key)) {
        Texture::storeTexture(Texture(VdpType::vdp2,
                                      cell_address,
                                      toUnderlying(screen.character_color_number),
                                      pnd.palette_number,
                                      texture_data,
                                      texture_width,
                                      texture_height));
    }
    auto pos = ScreenPos{static_cast<u16>(cell_offset.x * texture_width), static_cast<u16>(cell_offset.y * texture_height)};
    pos.x -= screen.screen_scroll_horizontal_integer;
    pos.y -= screen.screen_scroll_vertical_integer;

    auto p = Vdp2Part(pnd, pos, key);
    p.priority(screen.priority_number);
    vdp2_parts_[util::toUnderlying(screen.scroll_screen)].push_back(std::move(p));
}

auto Vdp2::getColorRamAddressOffset(const u8 register_offset) -> u16 {
    constexpr auto color_size_2_bytes = u8{2};
    constexpr auto color_size_4_bytes = u8{4};
    auto           color_size         = u8{};
    constexpr auto mask_2_bits        = u8{0x3};
    constexpr auto mask_3_bits        = u8{0x7};
    auto           register_mask      = u8{};
    switch (ram_status_.color_ram_mode) {
        case ColorRamMode::mode_0_rgb_5_bits_1024_colors:
            color_size    = color_size_2_bytes;
            register_mask = mask_2_bits;
            break;
        case ColorRamMode::mode_1_rgb_5_bits_2048_colors:
            color_size    = color_size_2_bytes;
            register_mask = mask_3_bits;
            break;
        case ColorRamMode::mode_2_rgb_8_bits_1024_colors:
            color_size    = color_size_4_bytes;
            register_mask = mask_2_bits;
            break;
        default: core::Log::warning(Logger::vdp2, core::tr("Can't calculate color RAM address offset."));
    }
    return ((register_offset & register_mask) << displacement_8) * color_size;
};

auto getPatternNameData2Words(const u32 data, [[maybe_unused]] const ScrollScreenStatus& screen) -> PatternNameData {
    auto pattern_name_data                      = PatternNameData{};
    auto reg                                    = PatternNameData2Words{data};
    pattern_name_data.character_number          = reg.get(PatternNameData2Words::character_number);
    pattern_name_data.palette_number            = reg.get(PatternNameData2Words::palette_number);
    pattern_name_data.special_color_calculation = reg.get(PatternNameData2Words::special_color_calculation);
    pattern_name_data.special_priority          = reg.get(PatternNameData2Words::special_priority);
    pattern_name_data.is_horizontally_flipped   = reg.get(PatternNameData2Words::horizontal_flip);
    pattern_name_data.is_vertically_flipped     = reg.get(PatternNameData2Words::vertical_flip);
    return pattern_name_data;
};

auto getPatternNameData1Word1Cell16Colors10Bits(const u32 data, const ScrollScreenStatus& screen) -> PatternNameData {
    auto pattern_name_data = PatternNameData{};
    auto reg               = PatternNameData1Word1Cell16Colors10Bits{data};

    constexpr auto cn_disp             = u8{10};
    pattern_name_data.character_number = (screen.supplementary_character_number << cn_disp);
    pattern_name_data.character_number |= reg.get(PatternNameData1Word1Cell16Colors10Bits::character_number);

    constexpr auto pn_disp           = u8{4};
    pattern_name_data.palette_number = (screen.supplementary_palette_number << pn_disp);
    pattern_name_data.palette_number |= reg.get(PatternNameData1Word1Cell16Colors10Bits::palette_number);

    pattern_name_data.special_color_calculation = screen.special_color_calculation;
    pattern_name_data.special_priority          = screen.special_priority;
    pattern_name_data.is_horizontally_flipped   = reg.get(PatternNameData1Word1Cell16Colors10Bits::horizontal_flip);
    pattern_name_data.is_vertically_flipped     = reg.get(PatternNameData1Word1Cell16Colors10Bits::vertical_flip);

    return pattern_name_data;
};

auto getPatternNameData1Word1Cell16Colors12Bits(const u32 data, const ScrollScreenStatus& screen) -> PatternNameData {
    auto pattern_name_data = PatternNameData{};
    auto reg               = PatternNameData1Word1Cell16Colors12Bits{data};

    constexpr auto cn_disp             = u8{10};
    constexpr auto cn_mask             = u8{0x1C};
    pattern_name_data.character_number = ((screen.supplementary_character_number & cn_mask) << cn_disp);
    pattern_name_data.character_number |= reg.get(PatternNameData1Word1Cell16Colors12Bits::character_number);

    constexpr auto pn_disp           = u8{4};
    pattern_name_data.palette_number = (screen.supplementary_palette_number << pn_disp);
    pattern_name_data.palette_number |= reg.get(PatternNameData1Word1Cell16Colors12Bits::palette_number);

    pattern_name_data.special_color_calculation = screen.special_color_calculation;
    pattern_name_data.special_priority          = screen.special_priority;
    pattern_name_data.is_horizontally_flipped   = false;
    pattern_name_data.is_vertically_flipped     = false;

    return pattern_name_data;
};

auto getPatternNameData1Word1CellOver16Colors10Bits(const u32 data, const ScrollScreenStatus& screen) -> PatternNameData {
    auto pattern_name_data = PatternNameData{};
    auto reg               = PatternNameData1Word1CellOver16Colors10Bits{data};

    constexpr auto cn_disp             = u8{10};
    pattern_name_data.character_number = (reg.get(PatternNameData1Word1CellOver16Colors10Bits::palette_number) << cn_disp);
    pattern_name_data.character_number |= reg.get(PatternNameData1Word1CellOver16Colors10Bits::character_number);

    constexpr auto pn_disp           = u8{4};
    pattern_name_data.palette_number = (screen.supplementary_palette_number << pn_disp);

    pattern_name_data.special_color_calculation = screen.special_color_calculation;
    pattern_name_data.special_priority          = screen.special_priority;
    pattern_name_data.is_horizontally_flipped   = reg.get(PatternNameData1Word1CellOver16Colors10Bits::horizontal_flip);
    pattern_name_data.is_vertically_flipped     = reg.get(PatternNameData1Word1CellOver16Colors10Bits::vertical_flip);

    return pattern_name_data;
};

auto getPatternNameData1Word1CellOver16Colors12Bits(const u32 data, const ScrollScreenStatus& screen) -> PatternNameData {
    auto pattern_name_data = PatternNameData{};
    auto reg               = PatternNameData1Word1CellOver16Colors12Bits{data};

    constexpr auto cn_disp             = u8{10};
    constexpr auto cn_mask             = u8{0x1C};
    pattern_name_data.character_number = ((screen.supplementary_character_number & cn_mask) << cn_disp);
    pattern_name_data.character_number |= reg.get(PatternNameData1Word1CellOver16Colors12Bits::character_number);

    constexpr auto pn_disp           = u8{4};
    pattern_name_data.palette_number = (reg.get(PatternNameData1Word1CellOver16Colors12Bits::palette_number) << pn_disp);

    pattern_name_data.special_color_calculation = screen.special_color_calculation;
    pattern_name_data.special_priority          = screen.special_priority;
    pattern_name_data.is_horizontally_flipped   = false;
    pattern_name_data.is_vertically_flipped     = false;

    return pattern_name_data;
};

auto getPatternNameData1Word4Cells16Colors10Bits(const u32 data, const ScrollScreenStatus& screen) -> PatternNameData {
    auto pattern_name_data = PatternNameData{};
    auto reg               = PatternNameData1Word4Cells16Colors10Bits{data};

    constexpr auto cn_disp_1           = u8{10};
    constexpr auto cn_mask_1           = u8{0x1C};
    pattern_name_data.character_number = (screen.supplementary_character_number & cn_mask_1) << cn_disp_1;
    constexpr auto cn_disp_2           = u8{2};
    pattern_name_data.character_number |= (reg.get(PatternNameData1Word4Cells16Colors10Bits::character_number) << cn_disp_2);
    constexpr auto cn_mask_2 = u8{0x3};
    pattern_name_data.character_number |= (screen.supplementary_character_number & cn_mask_2);

    constexpr auto pn_disp           = u8{4};
    pattern_name_data.palette_number = (screen.supplementary_palette_number << pn_disp);
    pattern_name_data.palette_number |= reg.get(PatternNameData1Word4Cells16Colors10Bits::palette_number);

    pattern_name_data.special_color_calculation = screen.special_color_calculation;
    pattern_name_data.special_priority          = screen.special_priority;
    pattern_name_data.is_horizontally_flipped   = reg.get(PatternNameData1Word4Cells16Colors10Bits::horizontal_flip);
    pattern_name_data.is_vertically_flipped     = reg.get(PatternNameData1Word4Cells16Colors10Bits::vertical_flip);

    return pattern_name_data;
};

auto getPatternNameData1Word4Cells16Colors12Bits(const u32 data, const ScrollScreenStatus& screen) -> PatternNameData {
    auto pattern_name_data = PatternNameData{};
    auto reg               = PatternNameData1Word4Cells16Colors12Bits{data};

    constexpr auto cn_disp_1           = u8{10};
    constexpr auto cn_mask_1           = u8{0x10};
    pattern_name_data.character_number = (screen.supplementary_character_number & cn_mask_1) << cn_disp_1;
    constexpr auto cn_disp_2           = u8{2};
    pattern_name_data.character_number |= (reg.get(PatternNameData1Word4Cells16Colors12Bits::character_number) << cn_disp_2);
    constexpr auto cn_mask_2 = u8{0x3};
    pattern_name_data.character_number |= (screen.supplementary_character_number & cn_mask_2);

    constexpr auto pn_disp           = u8{4};
    pattern_name_data.palette_number = (screen.supplementary_palette_number << pn_disp);
    pattern_name_data.palette_number |= reg.get(PatternNameData1Word4Cells16Colors12Bits::palette_number);

    pattern_name_data.special_color_calculation = screen.special_color_calculation;
    pattern_name_data.special_priority          = screen.special_priority;
    pattern_name_data.is_horizontally_flipped   = false;
    pattern_name_data.is_vertically_flipped     = false;

    return pattern_name_data;
};

auto getPatternNameData1Word4CellsOver16Colors10Bits(const u32 data, const ScrollScreenStatus& screen) -> PatternNameData {
    auto pattern_name_data = PatternNameData{};
    auto reg               = PatternNameData1Word4CellsOver16Colors10Bits{data};

    constexpr auto cn_disp_1           = u8{10};
    constexpr auto cn_mask_1           = u8{0x1C};
    pattern_name_data.character_number = (screen.supplementary_character_number & cn_mask_1) << cn_disp_1;
    constexpr auto cn_disp_2           = u8{2};
    pattern_name_data.character_number |= (reg.get(PatternNameData1Word4CellsOver16Colors10Bits::character_number) << cn_disp_2);
    constexpr auto cn_mask_2 = u8{0x3};
    pattern_name_data.character_number |= (screen.supplementary_character_number & cn_mask_2);

    constexpr auto pn_disp           = u8{4};
    pattern_name_data.palette_number = (reg.get(PatternNameData1Word4CellsOver16Colors10Bits::palette_number) << pn_disp);

    pattern_name_data.special_color_calculation = screen.special_color_calculation;
    pattern_name_data.special_priority          = screen.special_priority;
    pattern_name_data.is_horizontally_flipped   = reg.get(PatternNameData1Word4CellsOver16Colors10Bits::horizontal_flip);
    pattern_name_data.is_vertically_flipped     = reg.get(PatternNameData1Word4CellsOver16Colors10Bits::vertical_flip);

    return pattern_name_data;
};

auto getPatternNameData1Word4CellsOver16Colors12Bits(const u32 data, const ScrollScreenStatus& screen) -> PatternNameData {
    auto pattern_name_data = PatternNameData{};
    auto reg               = PatternNameData1Word4CellsOver16Colors12Bits{data};

    constexpr auto cn_disp_1           = u8{10};
    constexpr auto cn_mask_1           = u8{0x10};
    pattern_name_data.character_number = (screen.supplementary_character_number & cn_mask_1) << cn_disp_1;
    constexpr auto cn_disp_2           = u8{2};
    pattern_name_data.character_number |= (reg.get(PatternNameData1Word4CellsOver16Colors12Bits::character_number) << cn_disp_2);
    constexpr auto cn_mask_2 = u8{0x3};
    pattern_name_data.character_number |= (screen.supplementary_character_number & cn_mask_2);

    constexpr auto pn_disp           = u8{4};
    pattern_name_data.palette_number = (reg.get(PatternNameData1Word4CellsOver16Colors12Bits::palette_number) << pn_disp);

    pattern_name_data.special_color_calculation = screen.special_color_calculation;
    pattern_name_data.special_priority          = screen.special_priority;
    pattern_name_data.is_horizontally_flipped   = false;
    pattern_name_data.is_vertically_flipped     = false;

    return pattern_name_data;
};

} // namespace saturnin::video
