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
#include <saturnin/src/video/vdp2/vdp2.h>
#include <any> // any
#include <istream>
#include <numeric> // accumulate
#include <set>
#include <unordered_set>
#include <variant> // variant
#include <saturnin/src/config.h>
#include <saturnin/src/interrupt_sources.h>
#include <saturnin/src/scu_registers.h>
#include <saturnin/src/timer.h>
#include <saturnin/src/utilities.h> // toUnderlying
#include <saturnin/src/video/opengl/opengl.h>
#include <saturnin/src/video/texture.h>
#include <saturnin/src/video/vdp1.h>
#include <saturnin/src/video/vdp2/vdp2_registers.h>

namespace saturnin::video {
namespace interrupt_source = core::interrupt_source;
namespace util             = saturnin::utilities;
using namespace vdp2_register_address;

using core::Config;
using core::EmulatorContext;
using core::Log;
using core::Logger;
using core::tr;

using util::toUnderlying;

// constexpr auto use_concurrent_read_for_cells = bool{false};
// constexpr auto vdp2_vram_4mb_mask            = u16{0x3FFF};
// constexpr auto vdp2_vram_8mb_mask            = u16{0x7FFF};

static std::vector<long long> measures;

//--------------------------------------------------------------------------------------------------------------
// PUBLIC section
//--------------------------------------------------------------------------------------------------------------

void Vdp2::initialize() {
    using Tvstat = Vdp2Regs::Tvstat;

    initializeRegisterNameMap();
    computePrecalculatedData();

    const std::string ts = modules_.config()->readValue(core::AccessKeys::cfg_rendering_tv_standard);
    switch (modules_.config()->getTvStandard(ts)) {
        using enum video::TvStandard;
        case pal: regs_.tvstat.upd(Tvstat::pal_enum, Tvstat::TvStandardFlag::pal_standard); break;
        case ntsc: regs_.tvstat.upd(Tvstat::pal_enum, Tvstat::TvStandardFlag::ntsc_standard); break;
        default: Log::warning(Logger::vdp2, tr("Unknown TV standard"));
    }
    calculateDisplayDuration();

    disabled_scroll_screens_[ScrollScreen::nbg0] = false;
    disabled_scroll_screens_[ScrollScreen::nbg1] = false;
    disabled_scroll_screens_[ScrollScreen::nbg2] = false;
    disabled_scroll_screens_[ScrollScreen::nbg3] = false;
    disabled_scroll_screens_[ScrollScreen::rbg0] = false;
    disabled_scroll_screens_[ScrollScreen::rbg1] = false;
}

void Vdp2::run(const u8 cycles) {
    using Tvmd   = Vdp2Regs::Tvmd;
    using Tvstat = Vdp2Regs::Tvstat;

    elapsed_frame_cycles_ += cycles;
    if (elapsed_frame_cycles_ > cycles_per_vactive_) {
        if (!is_vblank_current_) {
            // Entering vertical blanking
            is_vblank_current_ = true;
            regs_.tvstat.upd(Tvstat::vblank_enum, Tvstat::VerticalBlankFlag::during_vertical_retrace);
            regs_.tvmd.upd(Tvmd::disp_enum, Tvmd::Display::not_displayed);

            Log::debug(Logger::vdp2, tr("VBlankIn interrupt request"));

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
        regs_.tvstat.upd(Tvstat::vblank_enum, Tvstat::VerticalBlankFlag::during_vertical_scan);

        elapsed_line_cycles_ = 0;
        is_hblank_current_   = false;
        regs_.tvstat.upd(Tvstat::hblank_enum, Tvstat::HorizontalBlankFlag::during_horizontal_scan);

        regs_.tvmd.upd(Tvmd::disp_enum, Tvmd::Display::displayed);

        Log::debug(Logger::vdp2, tr("VBlankOut interrupt request"));
        modules_.scu()->onVblankOut();

        modules_.smpc()->clearStvSwitchs();

        timer_0_counter_ = 0;

        calculateDisplayDuration();
        return;
    }

    elapsed_line_cycles_ += cycles;
    if (elapsed_line_cycles_ > cycles_per_hactive_) {
        if (!is_hblank_current_) {
            // Entering horizontal blanking
            is_hblank_current_ = true;
            regs_.tvstat.upd(Tvstat::hblank_enum, Tvstat::HorizontalBlankFlag::during_horizontal_retrace);

            modules_.scu()->onHblankIn();

            timer_0_counter_++;

            if (timer_0_counter_ == modules_.scu()->getTimer0CompareValue()) { modules_.scu()->onTimer0(); }

            if ((regs_.tvmd >> Tvmd::lsmd_enum) == Tvmd::InterlaceMode::non_interlace) {
                regs_.tvstat.upd(Tvstat::odd_enum, Tvstat::ScanFieldFlag::during_odd_field_scan);
            }
        }
    }

    if (elapsed_line_cycles_ > cycles_per_line_) {
        // End of line display (active + hblank)
        elapsed_line_cycles_ = 0;
        is_hblank_current_   = false;
        regs_.tvstat.upd(Tvstat::hblank_enum, Tvstat::HorizontalBlankFlag::during_horizontal_scan);
    }

    // Yet to implement : H Counter, V Counter, Timer 1
}

auto Vdp2::getRegisters() const -> const AddressToNameMap& { return address_to_name_; };

void Vdp2::onSystemClockUpdate() { calculateDisplayDuration(); }

void Vdp2::calculateDisplayDuration() {
    // - A full frame vertical resolution is :
    //      - 262.5 lines for NTSC
    //      - 312.5 for PAL

    constexpr auto lines_224 = u16{224};
    constexpr auto lines_240 = u16{240};
    constexpr auto lines_256 = u16{256};

    using Tvmd = Vdp2Regs::Tvmd;

    std::string ts = modules_.context()->config()->readValue(core::AccessKeys::cfg_rendering_tv_standard);
    switch (modules_.context()->config()->getTvStandard(ts)) {
        using enum video::TvStandard;
        case pal: {
            constexpr auto frame_duration = seconds{1.0 / 50.0};
            cycles_per_frame_             = modules_.smpc()->calculateCyclesNumber(frame_duration);

            constexpr auto total_lines   = u16{313};
            auto           visible_lines = u16{};
            // switch (toEnum<VerticalResolution>(tvmd_.vertical_resolution)) {
            switch (regs_.tvmd >> Tvmd::vreso_enum) {
                using enum Tvmd::VerticalResolution;
                case lines_nb_224: visible_lines = lines_224; break;
                case lines_nb_240: visible_lines = lines_240; break;
                case lines_nb_256: visible_lines = lines_256; break;
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
        case ntsc: {
            constexpr auto frame_duration = seconds{1.0 / 60.0};
            cycles_per_frame_             = modules_.smpc()->calculateCyclesNumber(frame_duration);

            constexpr auto total_lines   = u16{263};
            auto           visible_lines = u16{};
            switch (regs_.tvmd >> Tvmd::vreso_enum) {
                using enum Tvmd::VerticalResolution;
                case lines_nb_224: visible_lines = lines_224; break;
                case lines_nb_240: visible_lines = lines_240; break;
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
    using enum VdpType;
    calculateFps();
    Texture::cleanCache(modules_.opengl(), vdp2_cell);
    Texture::cleanCache(modules_.opengl(), vdp2_bitmap);
    updateResolution();
    updateRamStatus();
    Texture::setCache(vdp2_cell);
    Texture::setCache(vdp2_bitmap);
    populateRenderData();
    resetCacheState();
}

auto Vdp2::vdp2Parts(const ScrollScreen s, const VdpType t) const -> std::vector<video::Vdp2Part> {
    auto parts = std::vector<video::Vdp2Part>{};
    std::ranges::copy_if(vdp2_parts_[utilities::toUnderlying(s)], std::back_inserter(parts), [=](const Vdp2Part& part) {
        return part.common_vdp_data_.vdp_type == t;
    });

    return parts;
}

auto Vdp2::vdp2Parts(const ScrollScreen s, const u32 p) const -> std::vector<video::Vdp2Part> {
    auto parts = std::vector<video::Vdp2Part>{};
    std::ranges::copy_if(vdp2_parts_[utilities::toUnderlying(s)], std::back_inserter(parts), [=](const Vdp2Part& part) {
        return part.common_vdp_data_.priority == p;
    });

    return parts;
}
auto Vdp2::getSpriteColorAddressOffset() -> u16 {
    return getColorRamAddressOffset(static_cast<u8>(regs_.craofb >> Vdp2Regs::Craofb::spcaos_shft));
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto Vdp2::getSpritePriority(const u8 register_number) const -> u8 {
    // Next line works from LLVM version 14.0.0
    // NOLINTBEGIN(readability-magic-numbers)
    using Prisa = Vdp2Regs::Prisa;
    using Prisb = Vdp2Regs::Prisb;
    using Prisc = Vdp2Regs::Prisc;
    using Prisd = Vdp2Regs::Prisd;
    switch (register_number) {
        case 0: return static_cast<u8>(regs_.prisa >> Prisa::s0prin_shft);
        case 1: return static_cast<u8>(regs_.prisa >> Prisa::s1prin_shft);
        case 2: return static_cast<u8>(regs_.prisb >> Prisb::s2prin_shft);
        case 3: return static_cast<u8>(regs_.prisb >> Prisb::s3prin_shft);
        case 4: return static_cast<u8>(regs_.prisc >> Prisc::s4prin_shft);
        case 5: return static_cast<u8>(regs_.prisc >> Prisc::s5prin_shft);
        case 6: return static_cast<u8>(regs_.prisd >> Prisd::s6prin_shft);
        case 7: return static_cast<u8>(regs_.prisd >> Prisd::s7prin_shft);
        default: core::Log::warning(Logger::vdp2, core::tr("Unknown sprite priority."));
    }
    // NOLINTEND(readability-magic-numbers)

    return 0;
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto Vdp2::getColorOffset(const VdpLayer layer) -> ColorOffset {
    using Clofen    = Vdp2Regs::Clofen;
    using Clofsl    = Vdp2Regs::Clofsl;
    auto enable_bit = Clofen::ColorOffsetEnable{};
    auto select_bit = Clofsl::ColorOffsetSelect{};
    switch (layer) {
        using enum VdpLayer;
        case nbg0: {
            enable_bit = regs_.clofen >> Clofen::n0coen_enum;
            select_bit = regs_.clofsl >> Clofsl::n0cosl_enum;
            break;
        }
        case nbg1: {
            enable_bit = regs_.clofen >> Clofen::n1coen_enum;
            select_bit = regs_.clofsl >> Clofsl::n1cosl_enum;
            break;
        }
        case nbg2: {
            enable_bit = regs_.clofen >> Clofen::n2coen_enum;
            select_bit = regs_.clofsl >> Clofsl::n2cosl_enum;
            break;
        }
        case nbg3: {
            enable_bit = regs_.clofen >> Clofen::n3coen_enum;
            select_bit = regs_.clofsl >> Clofsl::n3cosl_enum;
            break;
        }
        case rbg0: {
            enable_bit = regs_.clofen >> Clofen::r0coen_enum;
            select_bit = regs_.clofsl >> Clofsl::r0cosl_enum;
            break;
        }
        case back: {
            enable_bit = regs_.clofen >> Clofen::bkcoen_enum;
            select_bit = regs_.clofsl >> Clofsl::bkcosl_enum;
            break;
        }
        case sprite: {
            enable_bit = regs_.clofen >> Clofen::spcoen_enum;
            select_bit = regs_.clofsl >> Clofsl::spcosl_enum;
            break;
        }
        default: {
            core::Log::warning(Logger::vdp2, core::tr("Undefined layer for color offset."));
            break;
        }
    }

    auto color_offset = ColorOffset{};
    if (enable_bit == Clofen::ColorOffsetEnable::enabled) {
        using Sign = Vdp2Regs::Sign;
        if (select_bit == Clofsl::ColorOffsetSelect::use_color_offset_a) {
            auto r_is_positive = (regs_.coar >> Vdp2Regs::Coar::sign_enum) == Sign::positive;
            auto r_value
                = r_is_positive ? (regs_.coar >> Vdp2Regs::Coar::coard_shft) : ~(regs_.coar >> Vdp2Regs::Coar::coard_shft) + 1;

            auto g_is_positive = (regs_.coag >> Vdp2Regs::Coag::sign_enum) == Sign::positive;
            auto g_value
                = g_is_positive ? (regs_.coag >> Vdp2Regs::Coag::coagd_shft) : ~(regs_.coag >> Vdp2Regs::Coag::coagd_shft) + 1;

            auto b_is_positive = (regs_.coab >> Vdp2Regs::Coab::sign_enum) == Sign::positive;
            auto b_value
                = b_is_positive ? (regs_.coab >> Vdp2Regs::Coab::coarb_shft) : ~(regs_.coab >> Vdp2Regs::Coab::coarb_shft) + 1;

            color_offset = {
                .signs{r_is_positive,            g_is_positive,            b_is_positive           },
                .values{static_cast<u8>(r_value), static_cast<u8>(g_value), static_cast<u8>(b_value)}
            };

        } else {
            auto r_is_positive = (regs_.cobr >> Vdp2Regs::Cobr::sign_enum) == Sign::positive;
            auto r_value
                = r_is_positive ? (regs_.cobr >> Vdp2Regs::Cobr::cobrd_shft) : ~(regs_.cobr >> Vdp2Regs::Cobr::cobrd_shft) + 1;

            auto g_is_positive = (regs_.cobg >> Vdp2Regs::Cobg::sign_enum) == Sign::positive;
            auto g_value
                = g_is_positive ? (regs_.cobg >> Vdp2Regs::Cobg::cobgr_shft) : ~(regs_.cobg >> Vdp2Regs::Cobg::cobgr_shft) + 1;

            auto b_is_positive = (regs_.cobb >> Vdp2Regs::Cobb::sign_enum) == Sign::positive;
            auto b_value
                = b_is_positive ? (regs_.cobb >> Vdp2Regs::Cobb::cobbl_shft) : ~(regs_.cobb >> Vdp2Regs::Cobb::cobbl_shft) + 1;

            color_offset = {
                .signs{r_is_positive,            g_is_positive,            b_is_positive           },
                .values{static_cast<u8>(r_value), static_cast<u8>(g_value), static_cast<u8>(b_value)}
            };
        }
    }
    return color_offset;
}

void Vdp2::refreshRegisters() {
    constexpr auto regs_max{0x120};
    for (u32 i = 0; i < regs_max; i += 2) {
        write16(i, modules_.memory()->vdp2_registers_[i] << 8 | modules_.memory()->vdp2_registers_[i + 1]);
    }
}

auto Vdp2::fps() -> std::string {
    std::lock_guard lock(fps_mutex_);
    return fps_;
}

//--------------------------------------------------------------------------------------------------------------
// PRIVATE section
//--------------------------------------------------------------------------------------------------------------

auto Vdp2::screenInDebug() const -> ScrollScreen { return screen_in_debug_; }
void Vdp2::screenInDebug(const ScrollScreen s) { screen_in_debug_ = s; }

//--------------------------------------------------------------------------------------------------------------
// MISC methods
//--------------------------------------------------------------------------------------------------------------

void Vdp2::addToRegisterNameMap(const u32 addr, const std::string& name) { address_to_name_.try_emplace(addr, name); }

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

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
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
    using Tvmd = Vdp2Regs::Tvmd;

    tv_screen_status_.is_picture_displayed = ((regs_.tvmd >> Tvmd::disp_enum) == Tvmd::Display::displayed);
    tv_screen_status_.border_color_mode    = regs_.tvmd >> Tvmd::bdclmd_enum;
    tv_screen_status_.interlace_mode       = regs_.tvmd >> Tvmd::lsmd_enum;

    switch (regs_.tvmd >> Tvmd::hreso_enum) {
        using enum Tvmd::HorizontalResolution;
        case normal_320:
            tv_screen_status_.horizontal_res   = horizontal_res_320;
            tv_screen_status_.screen_mode_type = ScreenModeType::normal;
            if (tv_screen_status_.interlace_mode == Tvmd::InterlaceMode::non_interlace) {
                switch (regs_.tvmd >> Tvmd::vreso_enum) {
                    using enum Tvmd::VerticalResolution;
                    case lines_nb_224:
                        tv_screen_status_.vertical_res = vertical_res_224;
                        tv_screen_status_.screen_mode  = ScreenMode::normal_320_224;
                        break;
                    case lines_nb_240:
                        tv_screen_status_.vertical_res = vertical_res_240;
                        tv_screen_status_.screen_mode  = ScreenMode::normal_320_240;
                        break;
                    case lines_nb_256:
                        tv_screen_status_.vertical_res = vertical_res_256;
                        tv_screen_status_.screen_mode  = ScreenMode::normal_320_256;
                        break;
                    default: tv_screen_status_.vertical_res = 0; tv_screen_status_.screen_mode = ScreenMode::not_set;
                }
            } else {
                switch (regs_.tvmd >> Tvmd::vreso_enum) {
                    using enum Tvmd::VerticalResolution;
                    case lines_nb_224:
                        tv_screen_status_.vertical_res = vertical_res_448;
                        tv_screen_status_.screen_mode  = ScreenMode::normal_320_448;
                        break;
                    case lines_nb_240:
                        tv_screen_status_.vertical_res = vertical_res_480;
                        tv_screen_status_.screen_mode  = ScreenMode::normal_320_480;
                        break;
                    case lines_nb_256:
                        tv_screen_status_.vertical_res = vertical_res_512;
                        tv_screen_status_.screen_mode  = ScreenMode::normal_320_512;
                        break;
                    default: tv_screen_status_.vertical_res = 0; tv_screen_status_.screen_mode = ScreenMode::not_set;
                }
            }

            break;
        case normal_352:
            tv_screen_status_.horizontal_res   = horizontal_res_352;
            tv_screen_status_.screen_mode_type = ScreenModeType::normal;
            if (tv_screen_status_.interlace_mode == Tvmd::InterlaceMode::non_interlace) {
                switch (regs_.tvmd >> Tvmd::vreso_enum) {
                    using enum Tvmd::VerticalResolution;
                    case lines_nb_224:
                        tv_screen_status_.vertical_res = vertical_res_224;
                        tv_screen_status_.screen_mode  = ScreenMode::normal_352_224;
                        break;
                    case lines_nb_240:
                        tv_screen_status_.vertical_res = vertical_res_240;
                        tv_screen_status_.screen_mode  = ScreenMode::normal_352_240;
                        break;
                    case lines_nb_256:
                        tv_screen_status_.vertical_res = vertical_res_256;
                        tv_screen_status_.screen_mode  = ScreenMode::normal_352_256;
                        break;
                    default: tv_screen_status_.vertical_res = 0; tv_screen_status_.screen_mode = ScreenMode::not_set;
                }
            } else {
                switch (regs_.tvmd >> Tvmd::vreso_enum) {
                    using enum Tvmd::VerticalResolution;
                    case lines_nb_224:
                        tv_screen_status_.vertical_res = vertical_res_448;
                        tv_screen_status_.screen_mode  = ScreenMode::normal_352_448;
                        break;
                    case lines_nb_240:
                        tv_screen_status_.vertical_res = vertical_res_480;
                        tv_screen_status_.screen_mode  = ScreenMode::normal_352_480;
                        break;
                    case lines_nb_256:
                        tv_screen_status_.vertical_res = vertical_res_512;
                        tv_screen_status_.screen_mode  = ScreenMode::normal_352_512;
                        break;
                    default: tv_screen_status_.vertical_res = 0; tv_screen_status_.screen_mode = ScreenMode::not_set;
                }
            }

            break;
        case hi_res_640:
            tv_screen_status_.horizontal_res   = horizontal_res_640;
            tv_screen_status_.screen_mode_type = ScreenModeType::hi_res;
            if (tv_screen_status_.interlace_mode == Tvmd::InterlaceMode::non_interlace) {
                switch (regs_.tvmd >> Tvmd::vreso_enum) {
                    using enum Tvmd::VerticalResolution;
                    case lines_nb_224:
                        tv_screen_status_.vertical_res = vertical_res_224;
                        tv_screen_status_.screen_mode  = ScreenMode::hi_res_640_224;
                        break;
                    case lines_nb_240:
                        tv_screen_status_.vertical_res = vertical_res_240;
                        tv_screen_status_.screen_mode  = ScreenMode::hi_res_640_240;
                        break;
                    case lines_nb_256:
                        tv_screen_status_.vertical_res = vertical_res_256;
                        tv_screen_status_.screen_mode  = ScreenMode::hi_res_640_256;
                        break;
                    default: tv_screen_status_.vertical_res = 0; tv_screen_status_.screen_mode = ScreenMode::not_set;
                }
            } else {
                switch (regs_.tvmd >> Tvmd::vreso_enum) {
                    using enum Tvmd::VerticalResolution;
                    case lines_nb_224:
                        tv_screen_status_.vertical_res = vertical_res_448;
                        tv_screen_status_.screen_mode  = ScreenMode::hi_res_640_448;
                        break;
                    case lines_nb_240:
                        tv_screen_status_.vertical_res = vertical_res_480;
                        tv_screen_status_.screen_mode  = ScreenMode::hi_res_640_480;
                        break;
                    case lines_nb_256:
                        tv_screen_status_.vertical_res = vertical_res_512;
                        tv_screen_status_.screen_mode  = ScreenMode::hi_res_640_512;
                        break;
                    default: tv_screen_status_.vertical_res = 0; tv_screen_status_.screen_mode = ScreenMode::not_set;
                }
            }
            break;
        case hi_res_704:
            tv_screen_status_.horizontal_res   = horizontal_res_704;
            tv_screen_status_.screen_mode_type = ScreenModeType::hi_res;
            if (tv_screen_status_.interlace_mode == Tvmd::InterlaceMode::non_interlace) {
                switch (regs_.tvmd >> Tvmd::vreso_enum) {
                    using enum Tvmd::VerticalResolution;
                    case lines_nb_224:
                        tv_screen_status_.vertical_res = vertical_res_224;
                        tv_screen_status_.screen_mode  = ScreenMode::hi_res_704_224;
                        break;
                    case lines_nb_240:
                        tv_screen_status_.vertical_res = vertical_res_240;
                        tv_screen_status_.screen_mode  = ScreenMode::hi_res_704_240;
                        break;
                    case lines_nb_256:
                        tv_screen_status_.vertical_res = vertical_res_256;
                        tv_screen_status_.screen_mode  = ScreenMode::hi_res_704_256;
                        break;
                    default: tv_screen_status_.vertical_res = 0; tv_screen_status_.screen_mode = ScreenMode::not_set;
                }
            } else {
                switch (regs_.tvmd >> Tvmd::vreso_enum) {
                    using enum Tvmd::VerticalResolution;
                    case lines_nb_224:
                        tv_screen_status_.vertical_res = vertical_res_448;
                        tv_screen_status_.screen_mode  = ScreenMode::hi_res_704_448;
                        break;
                    case lines_nb_240:
                        tv_screen_status_.vertical_res = vertical_res_480;
                        tv_screen_status_.screen_mode  = ScreenMode::hi_res_704_480;
                        break;
                    case lines_nb_256:
                        tv_screen_status_.vertical_res = vertical_res_512;
                        tv_screen_status_.screen_mode  = ScreenMode::hi_res_704_512;
                        break;
                    default: tv_screen_status_.vertical_res = 0; tv_screen_status_.screen_mode = ScreenMode::not_set;
                }
            }
            break;
        case exclusive_normal_320:
            tv_screen_status_.horizontal_res   = horizontal_res_320;
            tv_screen_status_.vertical_res     = vertical_res_480;
            tv_screen_status_.screen_mode      = ScreenMode::exclusive_320_480;
            tv_screen_status_.screen_mode_type = ScreenModeType::exclusive;
            break;
        case exclusive_normal_352:
            tv_screen_status_.horizontal_res   = horizontal_res_352;
            tv_screen_status_.vertical_res     = vertical_res_480;
            tv_screen_status_.screen_mode      = ScreenMode::exclusive_352_480;
            tv_screen_status_.screen_mode_type = ScreenModeType::exclusive;
            break;
        case exclusive_hi_res_640:
            tv_screen_status_.horizontal_res   = horizontal_res_640;
            tv_screen_status_.vertical_res     = vertical_res_480;
            tv_screen_status_.screen_mode      = ScreenMode::exclusive_640_480;
            tv_screen_status_.screen_mode_type = ScreenModeType::exclusive;
            break;
        case exclusive_hi_res_704:
            tv_screen_status_.horizontal_res   = horizontal_res_704;
            tv_screen_status_.vertical_res     = vertical_res_480;
            tv_screen_status_.screen_mode      = ScreenMode::exclusive_704_480;
            tv_screen_status_.screen_mode_type = ScreenModeType::exclusive;
            break;
    }

    modules_.opengl()->saturnScreenResolution({tv_screen_status_.horizontal_res, tv_screen_status_.vertical_res});
};

void Vdp2::updateRamStatus() {
    using Vrsize = Vdp2Regs::Vrsize;
    using Ramctl = Vdp2Regs::Ramctl;

    ram_status_.vram_size                    = regs_.vrsize >> Vrsize::vramsz_enum;
    ram_status_.vram_a_mode                  = regs_.ramctl >> Ramctl::vramd_enum;
    ram_status_.vram_b_mode                  = regs_.ramctl >> Ramctl::vrbmd_enum;
    ram_status_.color_ram_mode               = regs_.ramctl >> Ramctl::crmd_enum;
    ram_status_.coefficient_table_storage    = regs_.ramctl >> Ramctl::crkte_enum;
    ram_status_.vram_a0_rotation_bank_select = regs_.ramctl >> Ramctl::rdbsa0_enum;
    ram_status_.vram_a1_rotation_bank_select = regs_.ramctl >> Ramctl::rdbsa1_enum;
    ram_status_.vram_b0_rotation_bank_select = regs_.ramctl >> Ramctl::rdbsb0_enum;
    ram_status_.vram_b1_rotation_bank_select = regs_.ramctl >> Ramctl::rdbsb1_enum;
}

auto Vdp2::getColorRamAddressOffset(const u8 register_offset) const -> u16 {
    constexpr auto color_size_2_bytes = u8{2};
    constexpr auto color_size_4_bytes = u8{4};
    auto           color_size         = u8{};
    constexpr auto mask_2_bits        = u8{0x3};
    constexpr auto mask_3_bits        = u8{0x7};
    auto           register_mask      = u8{};
    switch (ram_status_.color_ram_mode) {
        using enum Vdp2Regs::Ramctl::ColorRamMode;
        case mode_0_rgb_5_bits_1024_colors:
            color_size    = color_size_2_bytes;
            register_mask = mask_2_bits;
            break;
        case mode_1_rgb_5_bits_2048_colors:
            color_size    = color_size_2_bytes;
            register_mask = mask_3_bits;
            break;
        case mode_2_rgb_8_bits_1024_colors:
            color_size    = color_size_4_bytes;
            register_mask = mask_2_bits;
            break;
        default: core::Log::warning(Logger::vdp2, core::tr("Can't calculate color RAM address offset."));
    }
    return ((register_offset & register_mask) << 8) * color_size;
};

void Vdp2::calculateFps() {
    using namespace std::literals::chrono_literals;

    static auto    previous_frame_start   = std::chrono::steady_clock::time_point{};
    const auto     current_frame_start    = std::chrono::steady_clock::now();
    const auto     current_frame_duration = std::chrono::duration_cast<micro>(current_frame_start - previous_frame_start);
    static auto    frames_fps             = std::deque<double>{};
    constexpr auto frames_max{30};

    if (frames_fps.size() >= frames_max) { frames_fps.pop_front(); }
    auto fps = 1s / current_frame_duration;
    frames_fps.push_back(fps);

    auto fps_sum = std::accumulate(frames_fps.begin(), frames_fps.end(), 0.0);

    std::string    ts           = modules_.config()->readValue(core::AccessKeys::cfg_rendering_tv_standard);
    const auto     standard     = modules_.config()->getTvStandard(ts);
    constexpr auto max_fps_pal  = 50;
    constexpr auto max_fps_ntsc = 60;
    static auto    max_frames   = (standard == TvStandard::pal) ? max_fps_pal : max_fps_ntsc;

    static auto update_counter = 0;
    if (constexpr auto update_rate = 5; update_counter % update_rate == 0) {
        std::lock_guard lock(fps_mutex_);
        fps_ = util::format("{:.1f} / {}", fps_sum / frames_max, max_frames);
    }
    ++update_counter;

    previous_frame_start = current_frame_start;
}

//--------------------------------------------------------------------------------------------------------------
// Free functions
//--------------------------------------------------------------------------------------------------------------

auto screenName(const ScrollScreen& ss) -> std::string {
    switch (ss) {
        using enum ScrollScreen;
        case nbg0: return "nbg0";
        case nbg1: return "nbg1";
        case nbg2: return "nbg2";
        case nbg3: return "nbg3";
        case rbg0: return "rbg0";
        case rbg1: return "rbg1";
        default: return "unknown scroll screen";
    }
}

auto scrollScreenToLayer(const ScrollScreen& ss) -> VdpLayer {
    switch (ss) {
        using enum ScrollScreen;
        case nbg0: return VdpLayer::nbg0;
        case nbg1: return VdpLayer::nbg1;
        case nbg2: return VdpLayer::nbg2;
        case nbg3: return VdpLayer::nbg3;
        case rbg0: return VdpLayer::rbg0;
        case rbg1: return VdpLayer::rbg1;
        default: return VdpLayer::undefined;
    }
}

} // namespace saturnin::video
