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
using core::tr;

using util::toUnderlying;

constexpr auto use_concurrent_read_for_cells = bool{false};
constexpr auto vdp2_vram_4mb_mask            = u16{0x3FFF};
constexpr auto vdp2_vram_8mb_mask            = u16{0x7FFF};
constexpr auto bits_in_a_byte                = u8{8};

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
auto Vdp2::getColorOffset(const Layer layer) -> ColorOffset {
    using Clofen    = Vdp2Regs::Clofen;
    using Clofsl    = Vdp2Regs::Clofsl;
    auto enable_bit = Clofen::ColorOffsetEnable{};
    auto select_bit = Clofsl::ColorOffsetSelect{};
    switch (layer) {
        using enum Layer;
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

//--------------------------------------------------------------------------------------------------------------
// MEMORY ACCESS methods
//--------------------------------------------------------------------------------------------------------------

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto Vdp2::read16(const u32 addr) const -> u16 {
    switch (addr & core::vdp2_registers_memory_mask) {
        case tv_screen_mode: return regs_.tvmd.data();
        case external_signal_enable: return regs_.exten.data();
        case screen_status: return regs_.tvstat.data();
        case vram_size: return regs_.vrsize.data();
        case h_counter: return regs_.hcnt.data();
        case v_counter: return regs_.vcnt.data();
        case reserve_1: return regs_.rsv1.data();
        case ram_control: return regs_.ramctl.data();
        case vram_cycle_pattern_bank_a0_lower: return regs_.cyca0l.data();
        case vram_cycle_pattern_bank_a0_upper: return regs_.cyca0u.data();
        case vram_cycle_pattern_bank_a1_lower: return regs_.cyca1l.data();
        case vram_cycle_pattern_bank_a1_upper: return regs_.cyca1u.data();
        case vram_cycle_pattern_bank_b0_lower: return regs_.cycb0l.data();
        case vram_cycle_pattern_bank_b0_upper: return regs_.cycb0u.data();
        case vram_cycle_pattern_bank_b1_lower: return regs_.cycb1l.data();
        case vram_cycle_pattern_bank_b1_upper: return regs_.cycb1u.data();
        case screen_display_enable: return regs_.bgon.data();
        case mosaic_control: return regs_.mzctl.data();
        case special_function_code_select: return regs_.sfsel.data();
        case special_function_code: return regs_.sfcode.data();
        case character_control_a: return regs_.chctla.data();
        case character_control_b: return regs_.chctlb.data();
        case bitmap_palette_number_a: return regs_.bmpna.data();
        case bitmap_palette_number_b: return regs_.bmpnb.data();
        case pattern_name_control_nbg0: return regs_.pncn0.data();
        case pattern_name_control_nbg1: return regs_.pncn1.data();
        case pattern_name_control_nbg2: return regs_.pncn2.data();
        case pattern_name_control_nbg3: return regs_.pncn3.data();
        case pattern_name_control_rbg0: return regs_.pncr.data();
        case plane_size: return regs_.plsz.data();
        case map_offset_n: return regs_.mpofn.data();
        case map_offset_r: return regs_.mpofr.data();
        case map_nbg0_plane_a_b: return regs_.mpabn0.data();
        case map_nbg0_plane_c_d: return regs_.mpcdn0.data();
        case map_nbg1_plane_a_b: return regs_.mpabn1.data();
        case map_nbg1_plane_c_d: return regs_.mpcdn1.data();
        case map_nbg2_plane_a_b: return regs_.mpabn2.data();
        case map_nbg2_plane_c_d: return regs_.mpcdn2.data();
        case map_nbg3_plane_a_b: return regs_.mpabn3.data();
        case map_nbg3_plane_c_d: return regs_.mpcdn3.data();
        case map_rotation_parameter_a_plane_a_b: return regs_.mpabra.data();
        case map_rotation_parameter_a_plane_c_d: return regs_.mpcdra.data();
        case map_rotation_parameter_a_plane_e_f: return regs_.mpefra.data();
        case map_rotation_parameter_a_plane_g_h: return regs_.mpghra.data();
        case map_rotation_parameter_a_plane_i_j: return regs_.mpijra.data();
        case map_rotation_parameter_a_plane_k_l: return regs_.mpklra.data();
        case map_rotation_parameter_a_plane_m_n: return regs_.mpmnra.data();
        case map_rotation_parameter_a_plane_o_p: return regs_.mpopra.data();
        case map_rotation_parameter_b_plane_a_b: return regs_.mpabrb.data();
        case map_rotation_parameter_b_plane_c_d: return regs_.mpcdrb.data();
        case map_rotation_parameter_b_plane_e_f: return regs_.mpefrb.data();
        case map_rotation_parameter_b_plane_g_h: return regs_.mpghrb.data();
        case map_rotation_parameter_b_plane_i_j: return regs_.mpijrb.data();
        case map_rotation_parameter_b_plane_k_l: return regs_.mpklrb.data();
        case map_rotation_parameter_b_plane_m_n: return regs_.mpmnrb.data();
        case map_rotation_parameter_b_plane_o_p: return regs_.mpoprb.data();
        case screen_scroll_value_nbg0_h_int_part: return regs_.scxin0.data();
        case screen_scroll_value_nbg0_h_fract_part: return regs_.scxdn0.data();
        case screen_scroll_value_nbg0_v_int_part: return regs_.scyin0.data();
        case screen_scroll_value_nbg0_v_fract_part: return regs_.scydn0.data();
        case coordinate_increment_nbg0_h_int_part: return regs_.zmxin0.data();
        case coordinate_increment_nbg0_h_fract_part: return regs_.zmxdn0.data();
        case coordinate_increment_nbg0_v_int_part: return regs_.zmyin0.data();
        case coordinate_increment_nbg0_v_fract_part: return regs_.zmydn0.data();
        case screen_scroll_value_nbg1_h_int_part: return regs_.scxin1.data();
        case screen_scroll_value_nbg1_h_fract_part: return regs_.scxdn1.data();
        case screen_scroll_value_nbg1_v_int_part: return regs_.scyin1.data();
        case screen_scroll_value_nbg1_v_fract_part: return regs_.scydn1.data();
        case coordinate_increment_nbg1_h_int_part: return regs_.zmxin1.data();
        case coordinate_increment_nbg1_h_fract_part: return regs_.zmxdn1.data();
        case coordinate_increment_nbg1_v_int_part: return regs_.zmyin1.data();
        case coordinate_increment_nbg1_v_fract_part: return regs_.zmydn1.data();
        case screen_scroll_value_nbg2_h: return regs_.scxn2.data();
        case screen_scroll_value_nbg2_v: return regs_.scyn2.data();
        case screen_scroll_value_nbg3_h: return regs_.scxn3.data();
        case screen_scroll_value_nbg3_v: return regs_.scyn3.data();
        case reduction_enable: return regs_.zmctl.data();
        case line_and_vertical_cell_scroll_control: return regs_.scrctl.data();
        case vertical_cell_scroll_table_address_upper: return regs_.vcstau.data();
        case vertical_cell_scroll_table_address_lower: return regs_.vcstal.data();
        case line_scroll_table_address_nbg0_upper: return regs_.lsta0u.data();
        case line_scroll_table_address_nbg0_lower: return regs_.lsta0l.data();
        case line_scroll_table_address_nbg1_upper: return regs_.lsta1u.data();
        case line_scroll_table_address_nbg1_lower: return regs_.lsta1l.data();
        case line_color_screen_table_address_upper: return regs_.lctau.data();
        case line_color_screen_table_address_lower: return regs_.lctal.data();
        case back_screen_table_address_upper: return regs_.bktau.data();
        case back_screen_table_address_lower: return regs_.bktal.data();
        case rotation_parameter_mode: return regs_.rpmd.data();
        case rotation_parameter_read_control: return regs_.rprctl.data();
        case coefficient_table_control: return regs_.ktctl.data();
        case coefficient_table_address_offset: return regs_.ktaof.data();
        case screen_over_pattern_name_a: return regs_.ovpnra.data();
        case screen_over_pattern_name_b: return regs_.ovpnrb.data();
        case rotation_parameter_table_address_upper: return regs_.rptau.data();
        case rotation_parameter_table_address_lower: return regs_.rptal.data();
        case window_position_w0_h_start_point: return regs_.wpsx0.data();
        case window_position_w0_v_start_point: return regs_.wpsy0.data();
        case window_position_w0_h_end_point: return regs_.wpex0.data();
        case window_position_w0_v_end_point: return regs_.wpey0.data();
        case window_position_w1_h_start_point: return regs_.wpsx1.data();
        case window_position_w1_v_start_point: return regs_.wpsy1.data();
        case window_position_w1_h_end_point: return regs_.wpex1.data();
        case window_position_w1_v_end_point: return regs_.wpey1.data();
        case window_control_a: return regs_.wctla.data();
        case window_control_b: return regs_.wctlb.data();
        case window_control_c: return regs_.wctlc.data();
        case window_control_d: return regs_.wctld.data();
        case line_window_table_address_w0_upper: return regs_.lwta0u.data();
        case line_window_table_address_w0_lower: return regs_.lwta0l.data();
        case line_window_table_address_w1_upper: return regs_.lwta1u.data();
        case line_window_table_address_w1_lower: return regs_.lwta1l.data();
        case sprite_control: return regs_.spctl.data();
        case shadow_control: return regs_.sdctl.data();
        case color_ram_address_offset_a: return regs_.craofa.data();
        case color_ram_address_offset_b: return regs_.craofb.data();
        case line_color_screen_enable: return regs_.lnclen.data();
        case special_priority_mode: return regs_.sfprmd.data();
        case color_calculation_control: return regs_.ccctl.data();
        case special_color_calculation_mode: return regs_.sfccmd.data();
        case priority_number_sprite_0_1: return regs_.prisa.data();
        case priority_number_sprite_2_3: return regs_.prisb.data();
        case priority_number_sprite_4_5: return regs_.prisc.data();
        case priority_number_sprite_6_7: return regs_.prisd.data();
        case priority_number_nbg0_nbg1: return regs_.prina.data();
        case priority_number_nbg2_nbg3: return regs_.prinb.data();
        case priority_number_rbg0: return regs_.prir.data();
        case reserve_2: return regs_.rsv2.data();
        case color_calculation_ratio_sprite_0_1: return regs_.ccrsa.data();
        case color_calculation_ratio_sprite_2_3: return regs_.ccrsb.data();
        case color_calculation_ratio_sprite_4_5: return regs_.ccrsc.data();
        case color_calculation_ratio_sprite_6_7: return regs_.ccrsd.data();
        case color_calculation_ratio_nbg0_nbg1: return regs_.ccrna.data();
        case color_calculation_ratio_nbg2_nbg3: return regs_.ccrnb.data();
        case color_calculation_ratio_rbg0: return regs_.ccrr.data();
        case color_calculation_ratio_line_color_back: return regs_.ccrlb.data();
        case color_offset_enable: return regs_.clofen.data();
        case color_offset_select: return regs_.clofsl.data();
        case color_offset_a_red: return regs_.coar.data();
        case color_offset_a_green: return regs_.coag.data();
        case color_offset_a_blue: return regs_.coab.data();
        case color_offset_b_red: return regs_.cobr.data();
        case color_offset_b_green: return regs_.cobg.data();
        case color_offset_b_blue: return regs_.cobb.data();

        default: core::Log::warning(Logger::vdp2, core::tr("Unimplemented register read (16) {:#010x}"), addr);
    }

    return 0;
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void Vdp2::write8(const u32 addr, const u8 data) {
    switch (addr & core::vdp2_registers_memory_mask) {
        // case tv_screen_mode: tvmd_.upper_8_bits = data; break;
        case tv_screen_mode: regs_.tvmd.upd(Vdp2Regs::Tvmd::hiByte(data)); break;
        case tv_screen_mode + 1: regs_.tvmd.upd(Vdp2Regs::Tvmd::loByte(data)); break;
        case external_signal_enable: regs_.exten.upd(Vdp2Regs::Exten::hiByte(data)); break;
        case external_signal_enable + 1: regs_.exten.upd(Vdp2Regs::Exten::loByte(data)); break;
        case screen_status: regs_.tvstat.upd(Vdp2Regs::Tvstat::hiByte(data)); break;
        case screen_status + 1: regs_.tvstat.upd(Vdp2Regs::Tvstat::loByte(data)); break;
        case vram_size: regs_.vrsize.upd(Vdp2Regs::Vrsize::hiByte(data)); break;
        case vram_size + 1: regs_.vrsize.upd(Vdp2Regs::Vrsize::loByte(data)); break;
        case h_counter: regs_.hcnt.upd(Vdp2Regs::Hcnt::hiByte(data)); break;
        case h_counter + 1: regs_.hcnt.upd(Vdp2Regs::Hcnt::loByte(data)); break;
        case v_counter: regs_.vcnt.upd(Vdp2Regs::Vcnt::hiByte(data)); break;
        case v_counter + 1: regs_.vcnt.upd(Vdp2Regs::Vcnt::loByte(data)); break;
        case reserve_1: regs_.rsv1.upd(Vdp2Regs::Reserve::hiByte(data)); break;
        case reserve_1 + 1: regs_.rsv1.upd(Vdp2Regs::Reserve::loByte(data)); break;
        case ram_control: regs_.ramctl.upd(Vdp2Regs::Ramctl::hiByte(data)); break;
        case ram_control + 1: regs_.ramctl.upd(Vdp2Regs::Ramctl::loByte(data)); break;
        case vram_cycle_pattern_bank_a0_lower: regs_.cyca0l.upd(Vdp2Regs::Cycxxl::hiByte(data)); break;
        case vram_cycle_pattern_bank_a0_lower + 1: regs_.cyca0l.upd(Vdp2Regs::Cycxxl::loByte(data)); break;
        case vram_cycle_pattern_bank_a0_upper: regs_.cyca0u.upd(Vdp2Regs::Cycxxu::hiByte(data)); break;
        case vram_cycle_pattern_bank_a0_upper + 1: regs_.cyca0u.upd(Vdp2Regs::Cycxxu::loByte(data)); break;
        case vram_cycle_pattern_bank_a1_lower: regs_.cyca1l.upd(Vdp2Regs::Cycxxl::hiByte(data)); break;
        case vram_cycle_pattern_bank_a1_lower + 1: regs_.cyca1l.upd(Vdp2Regs::Cycxxl::loByte(data)); break;
        case vram_cycle_pattern_bank_a1_upper: regs_.cyca1u.upd(Vdp2Regs::Cycxxu::hiByte(data)); break;
        case vram_cycle_pattern_bank_a1_upper + 1: regs_.cyca1u.upd(Vdp2Regs::Cycxxu::loByte(data)); break;
        case vram_cycle_pattern_bank_b0_lower: regs_.cycb0l.upd(Vdp2Regs::Cycxxl::hiByte(data)); break;
        case vram_cycle_pattern_bank_b0_lower + 1: regs_.cycb0l.upd(Vdp2Regs::Cycxxl::loByte(data)); break;
        case vram_cycle_pattern_bank_b0_upper: regs_.cycb0u.upd(Vdp2Regs::Cycxxu::hiByte(data)); break;
        case vram_cycle_pattern_bank_b0_upper + 1: regs_.cycb0u.upd(Vdp2Regs::Cycxxu::loByte(data)); break;
        case vram_cycle_pattern_bank_b1_lower: regs_.cycb1l.upd(Vdp2Regs::Cycxxl::hiByte(data)); break;
        case vram_cycle_pattern_bank_b1_lower + 1: regs_.cycb1l.upd(Vdp2Regs::Cycxxl::loByte(data)); break;
        case vram_cycle_pattern_bank_b1_upper: regs_.cycb1u.upd(Vdp2Regs::Cycxxu::hiByte(data)); break;
        case vram_cycle_pattern_bank_b1_upper + 1: regs_.cycb1u.upd(Vdp2Regs::Cycxxu::loByte(data)); break;
        case screen_display_enable: regs_.bgon.upd(Vdp2Regs::Bgon::hiByte(data)); break;
        case screen_display_enable + 1: regs_.bgon.upd(Vdp2Regs::Bgon::loByte(data)); break;
        case mosaic_control: regs_.mzctl.upd(Vdp2Regs::Mzctl::hiByte(data)); break;
        case mosaic_control + 1: regs_.mzctl.upd(Vdp2Regs::Mzctl::loByte(data)); break;
        case special_function_code_select: regs_.sfsel.upd(Vdp2Regs::Sfsel::hiByte(data)); break;
        case special_function_code_select + 1: regs_.sfsel.upd(Vdp2Regs::Sfsel::loByte(data)); break;
        case special_function_code: regs_.sfcode.upd(Vdp2Regs::Sfcode::hiByte(data)); break;
        case special_function_code + 1: regs_.sfcode.upd(Vdp2Regs::Sfcode::loByte(data)); break;
        case character_control_a: regs_.chctla.upd(Vdp2Regs::Chctla::hiByte(data)); break;
        case character_control_a + 1: regs_.chctla.upd(Vdp2Regs::Chctla::loByte(data)); break;
        case character_control_b: regs_.chctlb.upd(Vdp2Regs::Chctlb::hiByte(data)); break;
        case character_control_b + 1: regs_.chctlb.upd(Vdp2Regs::Chctlb::loByte(data)); break;
        case bitmap_palette_number_a: regs_.bmpna.upd(Vdp2Regs::Bmpna::hiByte(data)); break;
        case bitmap_palette_number_a + 1: regs_.bmpna.upd(Vdp2Regs::Bmpna::loByte(data)); break;
        case bitmap_palette_number_b: regs_.bmpnb.upd(Vdp2Regs::Bmpnb::hiByte(data)); break;
        case bitmap_palette_number_b + 1: regs_.bmpnb.upd(Vdp2Regs::Bmpnb::loByte(data)); break;
        case pattern_name_control_nbg0: regs_.pncn0.upd(Vdp2Regs::Pcnxx::hiByte(data)); break;
        case pattern_name_control_nbg0 + 1: regs_.pncn0.upd(Vdp2Regs::Pcnxx::loByte(data)); break;
        case pattern_name_control_nbg1: regs_.pncn1.upd(Vdp2Regs::Pcnxx::hiByte(data)); break;
        case pattern_name_control_nbg1 + 1: regs_.pncn1.upd(Vdp2Regs::Pcnxx::loByte(data)); break;
        case pattern_name_control_nbg2: regs_.pncn2.upd(Vdp2Regs::Pcnxx::hiByte(data)); break;
        case pattern_name_control_nbg2 + 1: regs_.pncn2.upd(Vdp2Regs::Pcnxx::loByte(data)); break;
        case pattern_name_control_nbg3: regs_.pncn3.upd(Vdp2Regs::Pcnxx::hiByte(data)); break;
        case pattern_name_control_nbg3 + 1: regs_.pncn3.upd(Vdp2Regs::Pcnxx::loByte(data)); break;
        case pattern_name_control_rbg0: regs_.pncr.upd(Vdp2Regs::Pcnxx::hiByte(data)); break;
        case pattern_name_control_rbg0 + 1: regs_.pncr.upd(Vdp2Regs::Pcnxx::loByte(data)); break;
        case plane_size: regs_.plsz.upd(Vdp2Regs::Plsz::hiByte(data)); break;
        case plane_size + 1: regs_.plsz.upd(Vdp2Regs::Plsz::loByte(data)); break;
        case map_offset_n: regs_.mpofn.upd(Vdp2Regs::Mpofn::hiByte(data)); break;
        case map_offset_n + 1: regs_.mpofn.upd(Vdp2Regs::Mpofn::loByte(data)); break;
        case map_offset_r: regs_.mpofr.upd(Vdp2Regs::Mpofr::hiByte(data)); break;
        case map_offset_r + 1: regs_.mpofr.upd(Vdp2Regs::Mpofr::loByte(data)); break;
        case map_nbg0_plane_a_b: regs_.mpabn0.upd(Vdp2Regs::Mpab::hiByte(data)); break;
        case map_nbg0_plane_a_b + 1: regs_.mpabn0.upd(Vdp2Regs::Mpab::loByte(data)); break;
        case map_nbg0_plane_c_d: regs_.mpcdn0.upd(Vdp2Regs::Mpcd::hiByte(data)); break;
        case map_nbg0_plane_c_d + 1: regs_.mpcdn0.upd(Vdp2Regs::Mpcd::loByte(data)); break;
        case map_nbg1_plane_a_b: regs_.mpabn1.upd(Vdp2Regs::Mpab::hiByte(data)); break;
        case map_nbg1_plane_a_b + 1: regs_.mpabn1.upd(Vdp2Regs::Mpab::loByte(data)); break;
        case map_nbg1_plane_c_d: regs_.mpcdn1.upd(Vdp2Regs::Mpcd::hiByte(data)); break;
        case map_nbg1_plane_c_d + 1: regs_.mpcdn1.upd(Vdp2Regs::Mpcd::loByte(data)); break;
        case map_nbg2_plane_a_b: regs_.mpabn2.upd(Vdp2Regs::Mpab::hiByte(data)); break;
        case map_nbg2_plane_a_b + 1: regs_.mpabn2.upd(Vdp2Regs::Mpab::loByte(data)); break;
        case map_nbg2_plane_c_d: regs_.mpcdn2.upd(Vdp2Regs::Mpcd::hiByte(data)); break;
        case map_nbg2_plane_c_d + 1: regs_.mpcdn2.upd(Vdp2Regs::Mpcd::loByte(data)); break;
        case map_nbg3_plane_a_b: regs_.mpabn3.upd(Vdp2Regs::Mpab::hiByte(data)); break;
        case map_nbg3_plane_a_b + 1: regs_.mpabn3.upd(Vdp2Regs::Mpab::loByte(data)); break;
        case map_nbg3_plane_c_d: regs_.mpcdn3.upd(Vdp2Regs::Mpcd::hiByte(data)); break;
        case map_nbg3_plane_c_d + 1: regs_.mpcdn3.upd(Vdp2Regs::Mpcd::loByte(data)); break;
        case map_rotation_parameter_a_plane_a_b: regs_.mpabra.upd(Vdp2Regs::Mpab::hiByte(data)); break;
        case map_rotation_parameter_a_plane_a_b + 1: regs_.mpabra.upd(Vdp2Regs::Mpab::loByte(data)); break;
        case map_rotation_parameter_a_plane_c_d: regs_.mpcdra.upd(Vdp2Regs::Mpcd::hiByte(data)); break;
        case map_rotation_parameter_a_plane_c_d + 1: regs_.mpcdra.upd(Vdp2Regs::Mpcd::loByte(data)); break;
        case map_rotation_parameter_a_plane_e_f: regs_.mpefra.upd(Vdp2Regs::Mpef::hiByte(data)); break;
        case map_rotation_parameter_a_plane_e_f + 1: regs_.mpefra.upd(Vdp2Regs::Mpef::loByte(data)); break;
        case map_rotation_parameter_a_plane_g_h: regs_.mpghra.upd(Vdp2Regs::Mpgh::hiByte(data)); break;
        case map_rotation_parameter_a_plane_g_h + 1: regs_.mpghra.upd(Vdp2Regs::Mpgh::loByte(data)); break;
        case map_rotation_parameter_a_plane_i_j: regs_.mpijra.upd(Vdp2Regs::Mpij::hiByte(data)); break;
        case map_rotation_parameter_a_plane_i_j + 1: regs_.mpijra.upd(Vdp2Regs::Mpij::loByte(data)); break;
        case map_rotation_parameter_a_plane_k_l: regs_.mpklra.upd(Vdp2Regs::Mpkl::hiByte(data)); break;
        case map_rotation_parameter_a_plane_k_l + 1: regs_.mpklra.upd(Vdp2Regs::Mpkl::loByte(data)); break;
        case map_rotation_parameter_a_plane_m_n: regs_.mpmnra.upd(Vdp2Regs::Mpmn::hiByte(data)); break;
        case map_rotation_parameter_a_plane_m_n + 1: regs_.mpmnra.upd(Vdp2Regs::Mpmn::loByte(data)); break;
        case map_rotation_parameter_a_plane_o_p: regs_.mpopra.upd(Vdp2Regs::Mpop::hiByte(data)); break;
        case map_rotation_parameter_a_plane_o_p + 1: regs_.mpopra.upd(Vdp2Regs::Mpop::loByte(data)); break;
        case map_rotation_parameter_b_plane_a_b: regs_.mpabrb.upd(Vdp2Regs::Mpab::hiByte(data)); break;
        case map_rotation_parameter_b_plane_a_b + 1: regs_.mpabrb.upd(Vdp2Regs::Mpab::loByte(data)); break;
        case map_rotation_parameter_b_plane_c_d: regs_.mpcdrb.upd(Vdp2Regs::Mpcd::hiByte(data)); break;
        case map_rotation_parameter_b_plane_c_d + 1: regs_.mpcdrb.upd(Vdp2Regs::Mpcd::loByte(data)); break;
        case map_rotation_parameter_b_plane_e_f: regs_.mpefrb.upd(Vdp2Regs::Mpef::hiByte(data)); break;
        case map_rotation_parameter_b_plane_e_f + 1: regs_.mpefrb.upd(Vdp2Regs::Mpef::loByte(data)); break;
        case map_rotation_parameter_b_plane_g_h: regs_.mpghrb.upd(Vdp2Regs::Mpgh::hiByte(data)); break;
        case map_rotation_parameter_b_plane_g_h + 1: regs_.mpghrb.upd(Vdp2Regs::Mpgh::loByte(data)); break;
        case map_rotation_parameter_b_plane_i_j: regs_.mpijrb.upd(Vdp2Regs::Mpij::hiByte(data)); break;
        case map_rotation_parameter_b_plane_i_j + 1: regs_.mpijrb.upd(Vdp2Regs::Mpij::loByte(data)); break;
        case map_rotation_parameter_b_plane_k_l: regs_.mpklrb.upd(Vdp2Regs::Mpkl::hiByte(data)); break;
        case map_rotation_parameter_b_plane_k_l + 1: regs_.mpklrb.upd(Vdp2Regs::Mpkl::loByte(data)); break;
        case map_rotation_parameter_b_plane_m_n: regs_.mpmnrb.upd(Vdp2Regs::Mpmn::hiByte(data)); break;
        case map_rotation_parameter_b_plane_m_n + 1: regs_.mpmnrb.upd(Vdp2Regs::Mpmn::loByte(data)); break;
        case map_rotation_parameter_b_plane_o_p: regs_.mpoprb.upd(Vdp2Regs::Mpop::hiByte(data)); break;
        case map_rotation_parameter_b_plane_o_p + 1: regs_.mpoprb.upd(Vdp2Regs::Mpop::loByte(data)); break;
        case screen_scroll_value_nbg0_h_int_part: regs_.scxin0.upd(Vdp2Regs::Scin::hiByte(data)); break;
        case screen_scroll_value_nbg0_h_int_part + 1: regs_.scxin0.upd(Vdp2Regs::Scin::loByte(data)); break;
        case screen_scroll_value_nbg0_h_fract_part: regs_.scxdn0.upd(Vdp2Regs::Scdn::hiByte(data)); break;
        case screen_scroll_value_nbg0_h_fract_part + 1: regs_.scxdn0.upd(Vdp2Regs::Scdn::loByte(data)); break;
        case screen_scroll_value_nbg0_v_int_part: regs_.scyin0.upd(Vdp2Regs::Scin::hiByte(data)); break;
        case screen_scroll_value_nbg0_v_int_part + 1: regs_.scyin0.upd(Vdp2Regs::Scin::loByte(data)); break;
        case screen_scroll_value_nbg0_v_fract_part: regs_.scydn0.upd(Vdp2Regs::Scdn::hiByte(data)); break;
        case screen_scroll_value_nbg0_v_fract_part + 1: regs_.scydn0.upd(Vdp2Regs::Scdn::loByte(data)); break;
        case coordinate_increment_nbg0_h_int_part: regs_.zmxin0.upd(Vdp2Regs::Zmin::hiByte(data)); break;
        case coordinate_increment_nbg0_h_int_part + 1: regs_.zmxin0.upd(Vdp2Regs::Zmin::loByte(data)); break;
        case coordinate_increment_nbg0_h_fract_part: regs_.zmxdn0.upd(Vdp2Regs::Zmdn::hiByte(data)); break;
        case coordinate_increment_nbg0_h_fract_part + 1: regs_.zmxdn0.upd(Vdp2Regs::Zmdn::loByte(data)); break;
        case coordinate_increment_nbg0_v_int_part: regs_.zmyin0.upd(Vdp2Regs::Zmin::hiByte(data)); break;
        case coordinate_increment_nbg0_v_int_part + 1: regs_.zmyin0.upd(Vdp2Regs::Zmin::loByte(data)); break;
        case coordinate_increment_nbg0_v_fract_part: regs_.zmydn0.upd(Vdp2Regs::Zmdn::hiByte(data)); break;
        case coordinate_increment_nbg0_v_fract_part + 1: regs_.zmydn0.upd(Vdp2Regs::Zmdn::loByte(data)); break;
        case screen_scroll_value_nbg1_h_int_part: regs_.scxin1.upd(Vdp2Regs::Scin::hiByte(data)); break;
        case screen_scroll_value_nbg1_h_int_part + 1: regs_.scxin1.upd(Vdp2Regs::Scin::loByte(data)); break;
        case screen_scroll_value_nbg1_h_fract_part: regs_.scxdn1.upd(Vdp2Regs::Scdn::hiByte(data)); break;
        case screen_scroll_value_nbg1_h_fract_part + 1: regs_.scxdn1.upd(Vdp2Regs::Scdn::loByte(data)); break;
        case screen_scroll_value_nbg1_v_int_part: regs_.scyin1.upd(Vdp2Regs::Scin::hiByte(data)); break;
        case screen_scroll_value_nbg1_v_int_part + 1: regs_.scyin1.upd(Vdp2Regs::Scin::loByte(data)); break;
        case screen_scroll_value_nbg1_v_fract_part: regs_.scydn1.upd(Vdp2Regs::Scdn::hiByte(data)); break;
        case screen_scroll_value_nbg1_v_fract_part + 1: regs_.scydn1.upd(Vdp2Regs::Scdn::loByte(data)); break;
        case coordinate_increment_nbg1_h_int_part: regs_.zmxin1.upd(Vdp2Regs::Zmin::hiByte(data)); break;
        case coordinate_increment_nbg1_h_int_part + 1: regs_.zmxin1.upd(Vdp2Regs::Zmin::loByte(data)); break;
        case coordinate_increment_nbg1_h_fract_part: regs_.zmxdn1.upd(Vdp2Regs::Zmdn::hiByte(data)); break;
        case coordinate_increment_nbg1_h_fract_part + 1: regs_.zmxdn1.upd(Vdp2Regs::Zmdn::loByte(data)); break;
        case coordinate_increment_nbg1_v_int_part: regs_.zmyin1.upd(Vdp2Regs::Zmin::hiByte(data)); break;
        case coordinate_increment_nbg1_v_int_part + 1: regs_.zmyin1.upd(Vdp2Regs::Zmin::loByte(data)); break;
        case coordinate_increment_nbg1_v_fract_part: regs_.zmydn1.upd(Vdp2Regs::Zmdn::hiByte(data)); break;
        case coordinate_increment_nbg1_v_fract_part + 1: regs_.zmydn1.upd(Vdp2Regs::Zmdn::loByte(data)); break;
        case screen_scroll_value_nbg2_h: regs_.scxn2.upd(Vdp2Regs::Scin::hiByte(data)); break;
        case screen_scroll_value_nbg2_h + 1: regs_.scxn2.upd(Vdp2Regs::Scin::loByte(data)); break;
        case screen_scroll_value_nbg2_v: regs_.scyn2.upd(Vdp2Regs::Scin::hiByte(data)); break;
        case screen_scroll_value_nbg2_v + 1: regs_.scyn2.upd(Vdp2Regs::Scin::loByte(data)); break;
        case screen_scroll_value_nbg3_h: regs_.scxn3.upd(Vdp2Regs::Scin::hiByte(data)); break;
        case screen_scroll_value_nbg3_h + 1: regs_.scxn3.upd(Vdp2Regs::Scin::loByte(data)); break;
        case screen_scroll_value_nbg3_v: regs_.scyn3.upd(Vdp2Regs::Scin::hiByte(data)); break;
        case screen_scroll_value_nbg3_v + 1: regs_.scyn3.upd(Vdp2Regs::Scin::loByte(data)); break;
        case reduction_enable: regs_.zmctl.upd(Vdp2Regs::Zmctl::hiByte(data)); break;
        case reduction_enable + 1: regs_.zmctl.upd(Vdp2Regs::Zmctl::loByte(data)); break;
        case line_and_vertical_cell_scroll_control: regs_.scrctl.upd(Vdp2Regs::Scrctl::hiByte(data)); break;
        case line_and_vertical_cell_scroll_control + 1: regs_.scrctl.upd(Vdp2Regs::Scrctl::loByte(data)); break;
        case vertical_cell_scroll_table_address_upper: regs_.vcstau.upd(Vdp2Regs::Vcstau::hiByte(data)); break;
        case vertical_cell_scroll_table_address_upper + 1: regs_.vcstau.upd(Vdp2Regs::Vcstau::loByte(data)); break;
        case vertical_cell_scroll_table_address_lower: regs_.vcstal.upd(Vdp2Regs::Vcstal::hiByte(data)); break;
        case vertical_cell_scroll_table_address_lower + 1: regs_.vcstal.upd(Vdp2Regs::Vcstal::loByte(data)); break;
        case line_scroll_table_address_nbg0_upper: regs_.lsta0u.upd(Vdp2Regs::Lstau::hiByte(data)); break;
        case line_scroll_table_address_nbg0_upper + 1: regs_.lsta0u.upd(Vdp2Regs::Lstau::loByte(data)); break;
        case line_scroll_table_address_nbg0_lower: regs_.lsta0l.upd(Vdp2Regs::Lstal::hiByte(data)); break;
        case line_scroll_table_address_nbg0_lower + 1: regs_.lsta0l.upd(Vdp2Regs::Lstal::loByte(data)); break;
        case line_scroll_table_address_nbg1_upper: regs_.lsta1u.upd(Vdp2Regs::Lstau::hiByte(data)); break;
        case line_scroll_table_address_nbg1_upper + 1: regs_.lsta1u.upd(Vdp2Regs::Lstau::loByte(data)); break;
        case line_scroll_table_address_nbg1_lower: regs_.lsta1l.upd(Vdp2Regs::Lstal::hiByte(data)); break;
        case line_scroll_table_address_nbg1_lower + 1: regs_.lsta1l.upd(Vdp2Regs::Lstal::loByte(data)); break;
        case line_color_screen_table_address_upper: regs_.lctau.upd(Vdp2Regs::Lctau::hiByte(data)); break;
        case line_color_screen_table_address_upper + 1: regs_.lctau.upd(Vdp2Regs::Lctau::loByte(data)); break;
        case line_color_screen_table_address_lower: regs_.lctal.upd(Vdp2Regs::Lctal::hiByte(data)); break;
        case line_color_screen_table_address_lower + 1: regs_.lctal.upd(Vdp2Regs::Lctal::loByte(data)); break;
        case back_screen_table_address_upper: regs_.bktau.upd(Vdp2Regs::Bktau::hiByte(data)); break;
        case back_screen_table_address_upper + 1: regs_.bktau.upd(Vdp2Regs::Bktau::loByte(data)); break;
        case back_screen_table_address_lower: regs_.bktal.upd(Vdp2Regs::Bktal::hiByte(data)); break;
        case back_screen_table_address_lower + 1: regs_.bktal.upd(Vdp2Regs::Bktal::loByte(data)); break;
        case rotation_parameter_mode: regs_.rpmd.upd(Vdp2Regs::Rpmd::hiByte(data)); break;
        case rotation_parameter_mode + 1: regs_.rpmd.upd(Vdp2Regs::Rpmd::loByte(data)); break;
        case rotation_parameter_read_control: regs_.rprctl.upd(Vdp2Regs::Rprctl::hiByte(data)); break;
        case rotation_parameter_read_control + 1: regs_.rprctl.upd(Vdp2Regs::Rprctl::loByte(data)); break;
        case coefficient_table_control: regs_.ktctl.upd(Vdp2Regs::Ktctl::hiByte(data)); break;
        case coefficient_table_control + 1: regs_.ktctl.upd(Vdp2Regs::Ktctl::loByte(data)); break;
        case coefficient_table_address_offset: regs_.ktaof.upd(Vdp2Regs::Ktaof::hiByte(data)); break;
        case coefficient_table_address_offset + 1: regs_.ktaof.upd(Vdp2Regs::Ktaof::loByte(data)); break;
        case screen_over_pattern_name_a: regs_.ovpnra.upd(Vdp2Regs::Ovprn::hiByte(data)); break;
        case screen_over_pattern_name_a + 1: regs_.ovpnra.upd(Vdp2Regs::Ovprn::loByte(data)); break;
        case screen_over_pattern_name_b: regs_.ovpnrb.upd(Vdp2Regs::Ovprn::hiByte(data)); break;
        case screen_over_pattern_name_b + 1: regs_.ovpnrb.upd(Vdp2Regs::Ovprn::loByte(data)); break;
        case rotation_parameter_table_address_upper: regs_.rptau.upd(Vdp2Regs::Rptau::hiByte(data)); break;
        case rotation_parameter_table_address_upper + 1: regs_.rptau.upd(Vdp2Regs::Rptau::loByte(data)); break;
        case rotation_parameter_table_address_lower: regs_.rptal.upd(Vdp2Regs::Rptal::hiByte(data)); break;
        case rotation_parameter_table_address_lower + 1: regs_.rptal.upd(Vdp2Regs::Rptal::loByte(data)); break;
        case window_position_w0_h_start_point: regs_.wpsx0.upd(Vdp2Regs::Wpsx::hiByte(data)); break;
        case window_position_w0_h_start_point + 1: regs_.wpsx0.upd(Vdp2Regs::Wpsx::loByte(data)); break;
        case window_position_w0_v_start_point: regs_.wpsy0.upd(Vdp2Regs::Wpsy::hiByte(data)); break;
        case window_position_w0_v_start_point + 1: regs_.wpsy0.upd(Vdp2Regs::Wpsy::loByte(data)); break;
        case window_position_w0_h_end_point: regs_.wpex0.upd(Vdp2Regs::Wpsx::hiByte(data)); break;
        case window_position_w0_h_end_point + 1: regs_.wpex0.upd(Vdp2Regs::Wpsx::loByte(data)); break;
        case window_position_w0_v_end_point: regs_.wpey0.upd(Vdp2Regs::Wpsy::hiByte(data)); break;
        case window_position_w0_v_end_point + 1: regs_.wpey0.upd(Vdp2Regs::Wpsy::loByte(data)); break;
        case window_position_w1_h_start_point: regs_.wpsx1.upd(Vdp2Regs::Wpsx::hiByte(data)); break;
        case window_position_w1_h_start_point + 1: regs_.wpsx1.upd(Vdp2Regs::Wpsx::loByte(data)); break;
        case window_position_w1_v_start_point: regs_.wpsy1.upd(Vdp2Regs::Wpsy::hiByte(data)); break;
        case window_position_w1_v_start_point + 1: regs_.wpsy1.upd(Vdp2Regs::Wpsy::loByte(data)); break;
        case window_position_w1_h_end_point: regs_.wpex1.upd(Vdp2Regs::Wpsx::hiByte(data)); break;
        case window_position_w1_h_end_point + 1: regs_.wpex1.upd(Vdp2Regs::Wpsx::loByte(data)); break;
        case window_position_w1_v_end_point: regs_.wpey1.upd(Vdp2Regs::Wpsy::hiByte(data)); break;
        case window_position_w1_v_end_point + 1: regs_.wpey1.upd(Vdp2Regs::Wpsy::loByte(data)); break;
        case window_control_a: regs_.wctla.upd(Vdp2Regs::Wctla::hiByte(data)); break;
        case window_control_a + 1: regs_.wctla.upd(Vdp2Regs::Wctla::loByte(data)); break;
        case window_control_b: regs_.wctlb.upd(Vdp2Regs::Wctlb::hiByte(data)); break;
        case window_control_b + 1: regs_.wctlb.upd(Vdp2Regs::Wctlb::loByte(data)); break;
        case window_control_c: regs_.wctlc.upd(Vdp2Regs::Wctlc::hiByte(data)); break;
        case window_control_c + 1: regs_.wctlc.upd(Vdp2Regs::Wctlc::loByte(data)); break;
        case window_control_d: regs_.wctld.upd(Vdp2Regs::Wctld::hiByte(data)); break;
        case window_control_d + 1: regs_.wctld.upd(Vdp2Regs::Wctld::loByte(data)); break;
        case line_window_table_address_w0_upper: regs_.lwta0u.upd(Vdp2Regs::Lwtau::hiByte(data)); break;
        case line_window_table_address_w0_upper + 1: regs_.lwta0u.upd(Vdp2Regs::Lwtau::loByte(data)); break;
        case line_window_table_address_w0_lower: regs_.lwta0l.upd(Vdp2Regs::Lwtal::hiByte(data)); break;
        case line_window_table_address_w0_lower + 1: regs_.lwta0l.upd(Vdp2Regs::Lwtal::loByte(data)); break;
        case line_window_table_address_w1_upper: regs_.lwta1u.upd(Vdp2Regs::Lwtau::hiByte(data)); break;
        case line_window_table_address_w1_upper + 1: regs_.lwta1u.upd(Vdp2Regs::Lwtau::loByte(data)); break;
        case line_window_table_address_w1_lower: regs_.lwta1l.upd(Vdp2Regs::Lwtal::hiByte(data)); break;
        case line_window_table_address_w1_lower + 1: regs_.lwta1l.upd(Vdp2Regs::Lwtal::loByte(data)); break;
        case sprite_control: regs_.spctl.upd(Vdp2Regs::Spctl::hiByte(data)); break;
        case sprite_control + 1: regs_.spctl.upd(Vdp2Regs::Spctl::loByte(data)); break;
        case shadow_control: regs_.sdctl.upd(Vdp2Regs::Sdctl::hiByte(data)); break;
        case shadow_control + 1: regs_.sdctl.upd(Vdp2Regs::Sdctl::loByte(data)); break;
        case color_ram_address_offset_a: regs_.craofa.upd(Vdp2Regs::Craofa::hiByte(data)); break;
        case color_ram_address_offset_a + 1: regs_.craofa.upd(Vdp2Regs::Craofa::loByte(data)); break;
        case color_ram_address_offset_b: regs_.craofb.upd(Vdp2Regs::Craofb::hiByte(data)); break;
        case color_ram_address_offset_b + 1: regs_.craofb.upd(Vdp2Regs::Craofb::loByte(data)); break;
        case line_color_screen_enable: regs_.lnclen.upd(Vdp2Regs::Lnclen::hiByte(data)); break;
        case line_color_screen_enable + 1: regs_.lnclen.upd(Vdp2Regs::Lnclen::loByte(data)); break;
        case special_priority_mode: regs_.sfprmd.upd(Vdp2Regs::Sfprmd::hiByte(data)); break;
        case special_priority_mode + 1: regs_.sfprmd.upd(Vdp2Regs::Sfprmd::loByte(data)); break;
        case color_calculation_control: regs_.ccctl.upd(Vdp2Regs::Ccctl::hiByte(data)); break;
        case color_calculation_control + 1: regs_.ccctl.upd(Vdp2Regs::Ccctl::loByte(data)); break;
        case special_color_calculation_mode: regs_.sfccmd.upd(Vdp2Regs::Sfccmd::hiByte(data)); break;
        case special_color_calculation_mode + 1: regs_.sfccmd.upd(Vdp2Regs::Sfccmd::loByte(data)); break;
        case priority_number_sprite_0_1: regs_.prisa.upd(Vdp2Regs::Prisa::hiByte(data)); break;
        case priority_number_sprite_0_1 + 1: regs_.prisa.upd(Vdp2Regs::Prisa::loByte(data)); break;
        case priority_number_sprite_2_3: regs_.prisb.upd(Vdp2Regs::Prisb::hiByte(data)); break;
        case priority_number_sprite_2_3 + 1: regs_.prisb.upd(Vdp2Regs::Prisb::loByte(data)); break;
        case priority_number_sprite_4_5: regs_.prisc.upd(Vdp2Regs::Prisc::hiByte(data)); break;
        case priority_number_sprite_4_5 + 1: regs_.prisc.upd(Vdp2Regs::Prisc::loByte(data)); break;
        case priority_number_sprite_6_7: regs_.prisd.upd(Vdp2Regs::Prisd::hiByte(data)); break;
        case priority_number_sprite_6_7 + 1: regs_.prisd.upd(Vdp2Regs::Prisd::loByte(data)); break;
        case priority_number_nbg0_nbg1: regs_.prina.upd(Vdp2Regs::Prina::hiByte(data)); break;
        case priority_number_nbg0_nbg1 + 1: regs_.prina.upd(Vdp2Regs::Prina::loByte(data)); break;
        case priority_number_nbg2_nbg3: regs_.prinb.upd(Vdp2Regs::Prinb::hiByte(data)); break;
        case priority_number_nbg2_nbg3 + 1: regs_.prinb.upd(Vdp2Regs::Prinb::loByte(data)); break;
        case priority_number_rbg0: regs_.prir.upd(Vdp2Regs::Prir::hiByte(data)); break;
        case priority_number_rbg0 + 1: regs_.prir.upd(Vdp2Regs::Prir::loByte(data)); break;
        case reserve_2: regs_.rsv2.upd(Vdp2Regs::Reserve::hiByte(data)); break;
        case reserve_2 + 1: regs_.rsv2.upd(Vdp2Regs::Reserve::loByte(data)); break;
        case color_calculation_ratio_sprite_0_1: regs_.ccrsa.upd(Vdp2Regs::Ccrsa::hiByte(data)); break;
        case color_calculation_ratio_sprite_0_1 + 1: regs_.ccrsa.upd(Vdp2Regs::Ccrsa::loByte(data)); break;
        case color_calculation_ratio_sprite_2_3: regs_.ccrsb.upd(Vdp2Regs::Ccrsb::hiByte(data)); break;
        case color_calculation_ratio_sprite_2_3 + 1: regs_.ccrsb.upd(Vdp2Regs::Ccrsb::loByte(data)); break;
        case color_calculation_ratio_sprite_4_5: regs_.ccrsc.upd(Vdp2Regs::Ccrsc::hiByte(data)); break;
        case color_calculation_ratio_sprite_4_5 + 1: regs_.ccrsc.upd(Vdp2Regs::Ccrsc::loByte(data)); break;
        case color_calculation_ratio_sprite_6_7: regs_.ccrsd.upd(Vdp2Regs::Ccrsd::hiByte(data)); break;
        case color_calculation_ratio_sprite_6_7 + 1: regs_.ccrsd.upd(Vdp2Regs::Ccrsd::loByte(data)); break;
        case color_calculation_ratio_nbg0_nbg1: regs_.ccrna.upd(Vdp2Regs::Ccrna::hiByte(data)); break;
        case color_calculation_ratio_nbg0_nbg1 + 1: regs_.ccrna.upd(Vdp2Regs::Ccrna::loByte(data)); break;
        case color_calculation_ratio_nbg2_nbg3: regs_.ccrnb.upd(Vdp2Regs::Ccrnb::hiByte(data)); break;
        case color_calculation_ratio_nbg2_nbg3 + 1: regs_.ccrnb.upd(Vdp2Regs::Ccrnb::loByte(data)); break;
        case color_calculation_ratio_rbg0: regs_.ccrr.upd(Vdp2Regs::Ccrr::hiByte(data)); break;
        case color_calculation_ratio_rbg0 + 1: regs_.ccrr.upd(Vdp2Regs::Ccrr::loByte(data)); break;
        case color_calculation_ratio_line_color_back: regs_.ccrlb.upd(Vdp2Regs::Ccrlb::hiByte(data)); break;
        case color_calculation_ratio_line_color_back + 1: regs_.ccrlb.upd(Vdp2Regs::Ccrlb::loByte(data)); break;
        case color_offset_enable: regs_.clofen.upd(Vdp2Regs::Clofen::hiByte(data)); break;
        case color_offset_enable + 1: regs_.clofen.upd(Vdp2Regs::Clofen::loByte(data)); break;
        case color_offset_select: regs_.clofsl.upd(Vdp2Regs::Clofsl::hiByte(data)); break;
        case color_offset_select + 1: regs_.clofsl.upd(Vdp2Regs::Clofsl::loByte(data)); break;
        case color_offset_a_red: regs_.coar.upd(Vdp2Regs::Coar::hiByte(data)); break;
        case color_offset_a_red + 1: regs_.coar.upd(Vdp2Regs::Coar::loByte(data)); break;
        case color_offset_a_green: regs_.coag.upd(Vdp2Regs::Coag::hiByte(data)); break;
        case color_offset_a_green + 1: regs_.coag.upd(Vdp2Regs::Coag::loByte(data)); break;
        case color_offset_a_blue: regs_.coab.upd(Vdp2Regs::Coab::hiByte(data)); break;
        case color_offset_a_blue + 1: regs_.coab.upd(Vdp2Regs::Coab::loByte(data)); break;
        case color_offset_b_red: regs_.cobr.upd(Vdp2Regs::Cobr::hiByte(data)); break;
        case color_offset_b_red + 1: regs_.cobr.upd(Vdp2Regs::Cobr::loByte(data)); break;
        case color_offset_b_green: regs_.cobg.upd(Vdp2Regs::Cobg::hiByte(data)); break;
        case color_offset_b_green + 1: regs_.cobg.upd(Vdp2Regs::Cobg::loByte(data)); break;
        case color_offset_b_blue: regs_.cobb.upd(Vdp2Regs::Cobb::hiByte(data)); break;
        case color_offset_b_blue + 1: regs_.cobb.upd(Vdp2Regs::Cobb::loByte(data)); break;
        default: core::Log::warning(Logger::vdp2, core::tr("Unimplemented register write (8) {:#010x} {:#04x}"), addr, data);
    }
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void Vdp2::write16(const u32 addr, const u16 data) {
    switch (addr & core::vdp2_registers_memory_mask) {
        case tv_screen_mode: regs_.tvmd = data; break;
        case external_signal_enable: regs_.exten = data; break;
        case screen_status: regs_.tvstat = data; break;
        case vram_size: regs_.vrsize = data; break;
        case h_counter: regs_.hcnt = data; break;
        case v_counter: regs_.vcnt = data; break;
        case reserve_1: regs_.rsv1 = data; break;
        case ram_control: regs_.ramctl = data; break;
        case vram_cycle_pattern_bank_a0_lower: regs_.cyca0l = data; break;
        case vram_cycle_pattern_bank_a0_upper: regs_.cyca0u = data; break;
        case vram_cycle_pattern_bank_a1_lower: regs_.cyca1l = data; break;
        case vram_cycle_pattern_bank_a1_upper: regs_.cyca1u = data; break;
        case vram_cycle_pattern_bank_b0_lower: regs_.cycb0l = data; break;
        case vram_cycle_pattern_bank_b0_upper: regs_.cycb0u = data; break;
        case vram_cycle_pattern_bank_b1_lower: regs_.cycb1l = data; break;
        case vram_cycle_pattern_bank_b1_upper: regs_.cycb1u = data; break;
        case screen_display_enable: regs_.bgon = data; break;
        case mosaic_control: regs_.mzctl = data; break;
        case special_function_code_select: regs_.sfsel = data; break;
        case special_function_code: regs_.sfcode = data; break;
        case character_control_a: regs_.chctla = data; break;
        case character_control_b: regs_.chctlb = data; break;
        case bitmap_palette_number_a: regs_.bmpna = data; break;
        case bitmap_palette_number_b: regs_.bmpnb = data; break;
        case pattern_name_control_nbg0: regs_.pncn0 = data; break;
        case pattern_name_control_nbg1: regs_.pncn1 = data; break;
        case pattern_name_control_nbg2: regs_.pncn2 = data; break;
        case pattern_name_control_nbg3: regs_.pncn3 = data; break;
        case pattern_name_control_rbg0: regs_.pncr = data; break;
        case plane_size: regs_.plsz = data; break;
        case map_offset_n: regs_.mpofn = data; break;
        case map_offset_r: regs_.mpofr = data; break;
        case map_nbg0_plane_a_b: regs_.mpabn0 = data; break;
        case map_nbg0_plane_c_d: regs_.mpcdn0 = data; break;
        case map_nbg1_plane_a_b: regs_.mpabn1 = data; break;
        case map_nbg1_plane_c_d: regs_.mpcdn1 = data; break;
        case map_nbg2_plane_a_b: regs_.mpabn2 = data; break;
        case map_nbg2_plane_c_d: regs_.mpcdn2 = data; break;
        case map_nbg3_plane_a_b: regs_.mpabn3 = data; break;
        case map_nbg3_plane_c_d: regs_.mpcdn3 = data; break;
        case map_rotation_parameter_a_plane_a_b: regs_.mpabra = data; break;
        case map_rotation_parameter_a_plane_c_d: regs_.mpcdra = data; break;
        case map_rotation_parameter_a_plane_e_f: regs_.mpefra = data; break;
        case map_rotation_parameter_a_plane_g_h: regs_.mpghra = data; break;
        case map_rotation_parameter_a_plane_i_j: regs_.mpijra = data; break;
        case map_rotation_parameter_a_plane_k_l: regs_.mpklra = data; break;
        case map_rotation_parameter_a_plane_m_n: regs_.mpmnra = data; break;
        case map_rotation_parameter_a_plane_o_p: regs_.mpopra = data; break;
        case map_rotation_parameter_b_plane_a_b: regs_.mpabrb = data; break;
        case map_rotation_parameter_b_plane_c_d: regs_.mpcdrb = data; break;
        case map_rotation_parameter_b_plane_e_f: regs_.mpefrb = data; break;
        case map_rotation_parameter_b_plane_g_h: regs_.mpghrb = data; break;
        case map_rotation_parameter_b_plane_i_j: regs_.mpijrb = data; break;
        case map_rotation_parameter_b_plane_k_l: regs_.mpklrb = data; break;
        case map_rotation_parameter_b_plane_m_n: regs_.mpmnrb = data; break;
        case map_rotation_parameter_b_plane_o_p: regs_.mpoprb = data; break;
        case screen_scroll_value_nbg0_h_int_part: regs_.scxin0 = data; break;
        case screen_scroll_value_nbg0_h_fract_part: regs_.scxdn0 = data; break;
        case screen_scroll_value_nbg0_v_int_part: regs_.scyin0 = data; break;
        case screen_scroll_value_nbg0_v_fract_part: regs_.scydn0 = data; break;
        case coordinate_increment_nbg0_h_int_part: regs_.zmxin0 = data; break;
        case coordinate_increment_nbg0_h_fract_part: regs_.zmxdn0 = data; break;
        case coordinate_increment_nbg0_v_int_part: regs_.zmyin0 = data; break;
        case coordinate_increment_nbg0_v_fract_part: regs_.zmydn0 = data; break;
        case screen_scroll_value_nbg1_h_int_part: regs_.scxin1 = data; break;
        case screen_scroll_value_nbg1_h_fract_part: regs_.scxdn1 = data; break;
        case screen_scroll_value_nbg1_v_int_part: regs_.scyin1 = data; break;
        case screen_scroll_value_nbg1_v_fract_part: regs_.scydn1 = data; break;
        case coordinate_increment_nbg1_h_int_part: regs_.zmxin1 = data; break;
        case coordinate_increment_nbg1_h_fract_part: regs_.zmxdn1 = data; break;
        case coordinate_increment_nbg1_v_int_part: regs_.zmyin1 = data; break;
        case coordinate_increment_nbg1_v_fract_part: regs_.zmydn1 = data; break;
        case screen_scroll_value_nbg2_h: regs_.scxn2 = data; break;
        case screen_scroll_value_nbg2_v: regs_.scyn2 = data; break;
        case screen_scroll_value_nbg3_h: regs_.scxn3 = data; break;
        case screen_scroll_value_nbg3_v: regs_.scyn3 = data; break;
        case reduction_enable: regs_.zmctl = data; break;
        case line_and_vertical_cell_scroll_control: regs_.scrctl = data; break;
        case vertical_cell_scroll_table_address_upper: regs_.vcstau = data; break;
        case vertical_cell_scroll_table_address_lower: regs_.vcstal = data; break;
        case line_scroll_table_address_nbg0_upper: regs_.lsta0u = data; break;
        case line_scroll_table_address_nbg0_lower: regs_.lsta0l = data; break;
        case line_scroll_table_address_nbg1_upper: regs_.lsta1u = data; break;
        case line_scroll_table_address_nbg1_lower: regs_.lsta1l = data; break;
        case line_color_screen_table_address_upper: regs_.lctau = data; break;
        case line_color_screen_table_address_lower: regs_.lctal = data; break;
        case back_screen_table_address_upper: regs_.bktau = data; break;
        case back_screen_table_address_lower: regs_.bktal = data; break;
        case rotation_parameter_mode: regs_.rpmd = data; break;
        case rotation_parameter_read_control: regs_.rprctl = data; break;
        case coefficient_table_control: regs_.ktctl = data; break;
        case coefficient_table_address_offset: regs_.ktaof = data; break;
        case screen_over_pattern_name_a: regs_.ovpnra = data; break;
        case screen_over_pattern_name_b: regs_.ovpnrb = data; break;
        case rotation_parameter_table_address_upper: regs_.rptau = data; break;
        case rotation_parameter_table_address_lower: regs_.rptal = data; break;
        case window_position_w0_h_start_point: regs_.wpsx0 = data; break;
        case window_position_w0_v_start_point: regs_.wpsy0 = data; break;
        case window_position_w0_h_end_point: regs_.wpex0 = data; break;
        case window_position_w0_v_end_point: regs_.wpey0 = data; break;
        case window_position_w1_h_start_point: regs_.wpsx1 = data; break;
        case window_position_w1_v_start_point: regs_.wpsy1 = data; break;
        case window_position_w1_h_end_point: regs_.wpex1 = data; break;
        case window_position_w1_v_end_point: regs_.wpey1 = data; break;
        case window_control_a: regs_.wctla = data; break;
        case window_control_b: regs_.wctlb = data; break;
        case window_control_c: regs_.wctlc = data; break;
        case window_control_d: regs_.wctld = data; break;
        case line_window_table_address_w0_upper: regs_.lwta0u = data; break;
        case line_window_table_address_w0_lower: regs_.lwta0l = data; break;
        case line_window_table_address_w1_upper: regs_.lwta1u = data; break;
        case line_window_table_address_w1_lower: regs_.lwta1l = data; break;
        case sprite_control: regs_.spctl = data; break;
        case shadow_control: regs_.sdctl = data; break;
        case color_ram_address_offset_a: regs_.craofa = data; break;
        case color_ram_address_offset_b: regs_.craofb = data; break;
        case line_color_screen_enable: regs_.lnclen = data; break;
        case special_priority_mode: regs_.sfprmd = data; break;
        case color_calculation_control: regs_.ccctl = data; break;
        case special_color_calculation_mode: regs_.sfccmd = data; break;
        case priority_number_sprite_0_1: regs_.prisa = data; break;
        case priority_number_sprite_2_3: regs_.prisb = data; break;
        case priority_number_sprite_4_5: regs_.prisc = data; break;
        case priority_number_sprite_6_7: regs_.prisd = data; break;
        case priority_number_nbg0_nbg1: regs_.prina = data; break;
        case priority_number_nbg2_nbg3: regs_.prinb = data; break;
        case priority_number_rbg0: regs_.prir = data; break;
        case reserve_2: regs_.rsv2 = data; break;
        case color_calculation_ratio_sprite_0_1: regs_.ccrsa = data; break;
        case color_calculation_ratio_sprite_2_3: regs_.ccrsb = data; break;
        case color_calculation_ratio_sprite_4_5: regs_.ccrsc = data; break;
        case color_calculation_ratio_sprite_6_7: regs_.ccrsd = data; break;
        case color_calculation_ratio_nbg0_nbg1: regs_.ccrna = data; break;
        case color_calculation_ratio_nbg2_nbg3: regs_.ccrnb = data; break;
        case color_calculation_ratio_rbg0: regs_.ccrr = data; break;
        case color_calculation_ratio_line_color_back: regs_.ccrlb = data; break;
        case color_offset_enable: regs_.clofen = data; break;
        case color_offset_select: regs_.clofsl = data; break;
        case color_offset_a_red: regs_.coar = data; break;
        case color_offset_a_green: regs_.coag = data; break;
        case color_offset_a_blue: regs_.coab = data; break;
        case color_offset_b_red: regs_.cobr = data; break;
        case color_offset_b_green: regs_.cobg = data; break;
        case color_offset_b_blue: regs_.cobb = data; break;
        default: core::Log::warning(Logger::vdp2, core::tr("Unimplemented register write (16) {:#010x}"), addr);
    }
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void Vdp2::write32(const u32 addr, const u32 data) {
    const auto h = static_cast<u16>(data >> 16);
    const auto l = static_cast<u16>(data & 0xFFFF);
    switch (addr & core::vdp2_registers_memory_mask) {
        case vram_cycle_pattern_bank_a0_lower:
            regs_.cyca0l = h;
            regs_.cyca0u = l;
            break;
        case vram_cycle_pattern_bank_a1_lower:
            regs_.cyca1l = h;
            regs_.cyca1u = l;
            break;
        case vram_cycle_pattern_bank_b0_lower:
            regs_.cycb0l = h;
            regs_.cycb0u = l;
            break;
        case vram_cycle_pattern_bank_b1_lower:
            regs_.cycb1l = h;
            regs_.cycb1u = l;
            break;
        case screen_display_enable:
            regs_.bgon  = h;
            regs_.mzctl = l;
            break;
        case special_function_code_select:
            regs_.sfsel  = h;
            regs_.sfcode = l;
            break;
        case character_control_a:
            regs_.chctla = h;
            regs_.chctlb = l;
            break;
        case bitmap_palette_number_a:
            regs_.bmpna = h;
            regs_.bmpnb = l;
            break;
        case pattern_name_control_nbg0:
            regs_.pncn0 = h;
            regs_.pncn1 = l;
            break;
        case pattern_name_control_nbg2:
            regs_.pncn2 = h;
            regs_.pncn3 = l;
            break;
        case pattern_name_control_rbg0:
            regs_.pncr = h;
            regs_.plsz = l;
            break;
        case map_offset_n:
            regs_.mpofn = h;
            regs_.mpofr = l;
            break;
        case map_nbg0_plane_a_b:
            regs_.mpabn0 = h;
            regs_.mpcdn0 = l;
            break;
        case map_nbg1_plane_a_b:
            regs_.mpabn1 = h;
            regs_.mpcdn1 = l;
            break;
        case map_nbg2_plane_a_b:
            regs_.mpabn2 = h;
            regs_.mpcdn2 = l;
            break;
        case map_nbg3_plane_a_b:
            regs_.mpabn3 = h;
            regs_.mpcdn3 = l;
            break;
        case map_rotation_parameter_a_plane_a_b:
            regs_.mpabra = h;
            regs_.mpcdra = l;
            break;
        case map_rotation_parameter_a_plane_e_f:
            regs_.mpefra = h;
            regs_.mpghra = l;
            break;
        case map_rotation_parameter_a_plane_i_j:
            regs_.mpijra = h;
            regs_.mpklra = l;
            break;
        case map_rotation_parameter_a_plane_m_n:
            regs_.mpmnra = h;
            regs_.mpopra = l;
            break;
        case map_rotation_parameter_b_plane_a_b:
            regs_.mpabrb = h;
            regs_.mpcdrb = l;
            break;
        case map_rotation_parameter_b_plane_e_f:
            regs_.mpefrb = h;
            regs_.mpghrb = l;
            break;
        case map_rotation_parameter_b_plane_i_j:
            regs_.mpijrb = h;
            regs_.mpklrb = l;
            break;
        case map_rotation_parameter_b_plane_m_n:
            regs_.mpmnrb = h;
            regs_.mpoprb = l;
            break;
        case screen_scroll_value_nbg0_h_int_part:
            regs_.scxin0 = h;
            regs_.scxdn0 = l;
            break;
        case screen_scroll_value_nbg0_v_int_part:
            regs_.scyin0 = h;
            regs_.scydn0 = l;
            break;
        case coordinate_increment_nbg0_h_int_part:
            regs_.zmxin0 = h;
            regs_.zmxdn0 = l;
            break;
        case coordinate_increment_nbg0_v_int_part:
            regs_.zmyin0 = h;
            regs_.zmydn0 = l;
            break;
        case screen_scroll_value_nbg1_h_int_part:
            regs_.scxin1 = h;
            regs_.scxdn1 = l;
            break;
        case screen_scroll_value_nbg1_v_int_part:
            regs_.scyin1 = h;
            regs_.scydn1 = l;
            break;
        case coordinate_increment_nbg1_h_int_part:
            regs_.zmxin1 = h;
            regs_.zmxdn1 = l;
            break;
        case coordinate_increment_nbg1_v_int_part:
            regs_.zmyin1 = h;
            regs_.zmydn1 = l;
            break;
        case screen_scroll_value_nbg2_h:
            regs_.scxn2 = h;
            regs_.scyn2 = l;
            break;
        case screen_scroll_value_nbg3_h:
            regs_.scxn3 = h;
            regs_.scyn3 = l;
            break;
        case reduction_enable:
            regs_.zmctl  = h;
            regs_.scrctl = l;
            break;
        case vertical_cell_scroll_table_address_upper:
            regs_.vcstau = h;
            regs_.vcstal = l;
            break;
        case line_scroll_table_address_nbg0_upper:
            regs_.lsta0u = h;
            regs_.lsta0l = l;
            break;
        case line_scroll_table_address_nbg1_upper:
            regs_.lsta1u = h;
            regs_.lsta1l = l;
            break;
        case line_color_screen_table_address_upper:
            regs_.lctau = h;
            regs_.lctal = l;
            break;
        case back_screen_table_address_upper:
            regs_.bktau = h;
            regs_.bktal = l;
            break;
        case rotation_parameter_mode:
            regs_.rpmd   = h;
            regs_.rprctl = l;
            break;
        case coefficient_table_control:
            regs_.ktctl = h;
            regs_.ktaof = l;
            break;
        case screen_over_pattern_name_a:
            regs_.ovpnra = h;
            regs_.ovpnrb = l;
            break;
        case rotation_parameter_table_address_upper:
            regs_.rptau = h;
            regs_.rptal = l;
            break;
        case window_position_w0_h_start_point:
            regs_.wpsx0 = h;
            regs_.wpsy0 = l;
            break;
        case window_position_w0_h_end_point:
            regs_.wpex0 = h;
            regs_.wpey0 = l;
            break;
        case window_position_w1_h_start_point:
            regs_.wpsx1 = h;
            regs_.wpsy1 = l;
            break;
        case window_position_w1_h_end_point:
            regs_.wpex1 = h;
            regs_.wpey1 = l;
            break;
        case window_control_a:
            regs_.wctla = h;
            regs_.wctlb = l;
            break;
        case window_control_c:
            regs_.wctlc = h;
            regs_.wctld = l;
            break;
        case line_window_table_address_w0_upper:
            regs_.lwta0u = h;
            regs_.lwta0l = l;
            break;
        case line_window_table_address_w1_upper:
            regs_.lwta1u = h;
            regs_.lwta1l = l;
            break;
        case sprite_control:
            regs_.spctl = h;
            regs_.sdctl = l;
            break;
        case color_ram_address_offset_a:
            regs_.craofa = h;
            regs_.craofb = l;
            break;
        case line_color_screen_enable:
            regs_.lnclen = h;
            regs_.sfprmd = l;
            break;
        case color_calculation_control:
            regs_.ccctl  = h;
            regs_.sfccmd = l;
            break;
        case priority_number_sprite_0_1:
            regs_.prisa = h;
            regs_.prisb = l;
            break;
        case priority_number_sprite_4_5:
            regs_.prisc = h;
            regs_.prisd = l;
            break;
        case priority_number_nbg0_nbg1:
            regs_.prina = h;
            regs_.prinb = l;
            break;
        case priority_number_rbg0:
            regs_.prir = h;
            regs_.rsv2 = l;
            break;
        case color_calculation_ratio_sprite_0_1:
            regs_.ccrsa = h;
            regs_.ccrsb = l;
            break;
        case color_calculation_ratio_sprite_4_5:
            regs_.ccrsc = h;
            regs_.ccrsd = l;
            break;
        case color_calculation_ratio_nbg0_nbg1:
            regs_.ccrna = h;
            regs_.ccrnb = l;
            break;
        case color_calculation_ratio_rbg0:
            regs_.ccrr  = h;
            regs_.ccrlb = l;
            break;
        case color_offset_enable:
            regs_.clofen = h;
            regs_.clofsl = l;
            break;
        case color_offset_a_red:
            regs_.coar = h;
            regs_.coag = l;
            break;
        case color_offset_a_blue:
            regs_.coab = h;
            regs_.cobr = l;
            break;
        case color_offset_b_green:
            regs_.cobg = h;
            regs_.cobb = l;
            break;
        default: core::Log::warning(Logger::vdp2, core::tr("Unimplemented register write (16) {:#010x}"), addr);
    }
}

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

    using Bgon   = Vdp2Regs::Bgon;
    using Chctla = Vdp2Regs::Chctla;
    using Chctlb = Vdp2Regs::Chctlb;
    using Zmctl  = Vdp2Regs::Zmctl;

    getScreen(s).is_display_enabled = false;

    switch (s) {
        using enum ScrollScreen;
        case nbg0: {
            if ((regs_.bgon >> Bgon::n0on_enum) == Bgon::ScreenDisplayEnableBit::cannot_display) { return false; }

            // Pattern name data reads depend on the reduction setting of the screen
            const auto reduction = getReductionSetting(regs_.zmctl >> Zmctl::n0zmqt_enum, regs_.zmctl >> Zmctl::n0zmhf_enum);

            // Character / Bitmap pattern data reads depend on the reduction setting and the number of colors
            if ((regs_.chctla >> Chctla::n0bmen_enum) == Vdp2Regs::BitmapEnable::bitmap_format) {
                // Bitmap format needs only bitmap pattern data.
                const auto color_number = regs_.chctla >> Chctla::n0chcn_enum;
                const auto required_bpd_reads
                    = util::toUnderlying(calculateRequiredVramCharacterPatternReads(reduction, color_number));
                const auto current_bdp_reads
                    = getVramAccessByCommand(Vdp2Regs::VramAccessCommand::nbg0_character_pattern_data_read, reduction);
                if (current_bdp_reads < required_bpd_reads) { return false; }
            } else {
                // Character format needs character pattern data (cpd) and pattern name data (pnd).
                const auto required_pnd_reads = util::toUnderlying(calculateRequiredVramPatternNameReads(reduction));
                const auto current_pnd_reads
                    = getVramAccessByCommand(Vdp2Regs::VramAccessCommand::nbg0_pattern_name_read, reduction);
                if (current_pnd_reads < required_pnd_reads) { return false; }

                const auto color_number = regs_.chctla >> Chctla::n0chcn_enum;
                const auto required_cpd_reads
                    = util::toUnderlying(calculateRequiredVramCharacterPatternReads(reduction, color_number));
                const auto current_cpd_reads
                    = getVramAccessByCommand(Vdp2Regs::VramAccessCommand::nbg0_character_pattern_data_read, reduction);
                if (current_cpd_reads < required_cpd_reads) { return false; }
            }
            break;
        }
        case nbg1: {
            if ((regs_.bgon >> Bgon::n1on_enum) == Bgon::ScreenDisplayEnableBit::cannot_display) { return false; }
            // Pattern name data reads depend on the reduction setting of the screen
            const auto reduction = getReductionSetting(regs_.zmctl >> Zmctl::n1zmqt_enum, regs_.zmctl >> Zmctl::n1zmhf_enum);

            // Character / Bitmap pattern data reads depend on the reduction setting and the number of colors
            if ((regs_.chctla >> Chctla::n1bmen_enum) == Vdp2Regs::BitmapEnable::bitmap_format) {
                // Bitmap format needs only bitmap pattern data.
                const auto color_number = regs_.chctla >> Chctla::n1chcn_enum;
                const auto required_bpd_reads
                    = util::toUnderlying(calculateRequiredVramCharacterPatternReads(reduction, color_number));
                const auto current_bdp_reads
                    = getVramAccessByCommand(Vdp2Regs::VramAccessCommand::nbg1_character_pattern_data_read, reduction);
                if (current_bdp_reads < required_bpd_reads) { return false; }
            } else {
                // Character format needs character pattern data (cpd) and pattern name data (pnd).
                const auto required_pnd_reads = util::toUnderlying(calculateRequiredVramPatternNameReads(reduction));
                const auto current_pnd_reads
                    = getVramAccessByCommand(Vdp2Regs::VramAccessCommand::nbg1_pattern_name_read, reduction);
                if (current_pnd_reads < required_pnd_reads) { return false; }

                const auto color_number = regs_.chctla >> Chctla::n1chcn_enum;
                const auto required_cpd_reads
                    = util::toUnderlying(calculateRequiredVramCharacterPatternReads(reduction, color_number));
                const auto current_cpd_reads
                    = getVramAccessByCommand(Vdp2Regs::VramAccessCommand::nbg1_character_pattern_data_read, reduction);
                if (current_cpd_reads < required_cpd_reads) { return false; }
            }

            break;
        }
        case nbg2: {
            if ((regs_.bgon >> Bgon::n2on_enum) == Bgon::ScreenDisplayEnableBit::cannot_display) { return false; }

            if (isScreenDisplayLimitedByReduction(s)) { return false; }

            const auto reduction = ReductionSetting::none;

            // Character format needs character pattern data (cpd) and pattern name data (pnd).
            const auto required_pnd_reads = util::toUnderlying(calculateRequiredVramPatternNameReads(reduction));
            const auto current_pnd_reads = getVramAccessByCommand(Vdp2Regs::VramAccessCommand::nbg2_pattern_name_read, reduction);
            if (current_pnd_reads < required_pnd_reads) { return false; }

            const auto color_number = regs_.chctlb >> Chctlb::n2chcn_enum;
            const auto required_cpd_reads
                = util::toUnderlying(calculateRequiredVramCharacterPatternReads(reduction, color_number));
            const auto current_cpd_reads
                = getVramAccessByCommand(Vdp2Regs::VramAccessCommand::nbg2_character_pattern_data_read, reduction);
            if (current_cpd_reads < required_cpd_reads) { return false; }

            break;
        }
        case nbg3: {
            if ((regs_.bgon >> Bgon::n3on_enum) == Bgon::ScreenDisplayEnableBit::cannot_display) { return false; }
            if (isScreenDisplayLimitedByReduction(s)) { return false; }

            const auto reduction = ReductionSetting::none;

            // Character format needs character pattern data (cpd) and pattern name data (pnd).
            const auto required_pnd_reads = util::toUnderlying(calculateRequiredVramPatternNameReads(reduction));
            const auto current_pnd_reads = getVramAccessByCommand(Vdp2Regs::VramAccessCommand::nbg3_pattern_name_read, reduction);
            if (current_pnd_reads < required_pnd_reads) { return false; }

            const auto color_number = regs_.chctlb >> Chctlb::n3chcn_enum;
            const auto required_cpd_reads
                = util::toUnderlying(calculateRequiredVramCharacterPatternReads(reduction, color_number));
            const auto current_cpd_reads
                = getVramAccessByCommand(Vdp2Regs::VramAccessCommand::nbg3_character_pattern_data_read, reduction);
            if (current_cpd_reads < required_cpd_reads) { return false; }
            break;
        }
        case rbg0: {
            if ((regs_.bgon >> Bgon::r0on_enum) == Bgon::ScreenDisplayEnableBit::cannot_display) { return false; }
            core::Log::unimplemented(core::tr("VDP2 RBG0 display"));
            break;
        }
        case rbg1: {
            if ((regs_.bgon >> Bgon::r1on_enum) == Bgon::ScreenDisplayEnableBit::cannot_display) { return false; }
            core::Log::unimplemented(core::tr("VDP2 RBG1 display"));
            break;
        }
        default: {
            Log::warning(Logger::vdp2, tr("Scroll screen not set"));
        }
    }

    getScreen(s).is_display_enabled = true;

    return true;
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto Vdp2::isScreenDisplayLimitedByReduction(ScrollScreen s) const -> bool {
    using Chctla = Vdp2Regs::Chctla;
    using Chctlb = Vdp2Regs::Chctlb;
    using Zmctl  = Vdp2Regs::Zmctl;

    switch (s) {
        using enum ScrollScreen;
        case nbg2: {
            const auto reduction    = getReductionSetting(regs_.zmctl >> Zmctl::n0zmqt_enum, regs_.zmctl >> Zmctl::n0zmhf_enum);
            const auto color_number = regs_.chctla >> Chctla::n0chcn_enum;

            if ((reduction == ReductionSetting::up_to_one_quarter)
                && (color_number == Vdp2Regs::CharacterColorNumber3Bits::palette_16)) {
                return true;
            }
            if ((reduction == ReductionSetting::up_to_one_half)
                && (color_number == Vdp2Regs::CharacterColorNumber3Bits::palette_256)) {
                return true;
            }
            break;
        }
        case nbg3: {
            const auto reduction    = getReductionSetting(regs_.zmctl >> Zmctl::n1zmqt_enum, regs_.zmctl >> Zmctl::n1zmhf_enum);
            const auto color_number = regs_.chctla >> Chctla::n1chcn_enum;

            if ((reduction == ReductionSetting::up_to_one_quarter)
                && (color_number == Vdp2Regs::CharacterColorNumber2Bits::palette_16)) {
                return true;
            }
            if ((reduction == ReductionSetting::up_to_one_half)
                && (color_number == Vdp2Regs::CharacterColorNumber2Bits::palette_256)) {
                return true;
            }
            break;
        }
        default: return false;
    }
    return false;
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto Vdp2::getVramAccessByCommand(const Vdp2Regs::VramAccessCommand command, const ReductionSetting reduction) const -> u8 {
    using Tvmd   = Vdp2Regs::Tvmd;
    using Cycxxl = Vdp2Regs::Cycxxl;
    using Cycxxu = Vdp2Regs::Cycxxu;

    auto is_normal_mode = ((regs_.tvmd >> Tvmd::hreso_enum) == Tvmd::HorizontalResolution::normal_320);
    is_normal_mode |= ((regs_.tvmd >> Tvmd::hreso_enum) == Tvmd::HorizontalResolution::normal_352);

    VramTiming bank_a0 = {regs_.cyca0l >> Cycxxl::t0_enum,
                          regs_.cyca0l >> Cycxxl::t1_enum,
                          regs_.cyca0l >> Cycxxl::t2_enum,
                          regs_.cyca0l >> Cycxxl::t3_enum,
                          is_normal_mode ? regs_.cyca0u >> Cycxxu::t4_enum : Vdp2Regs::VramAccessCommand::no_access,
                          is_normal_mode ? regs_.cyca0u >> Cycxxu::t5_enum : Vdp2Regs::VramAccessCommand::no_access,
                          is_normal_mode ? regs_.cyca0u >> Cycxxu::t6_enum : Vdp2Regs::VramAccessCommand::no_access,
                          is_normal_mode ? regs_.cyca0u >> Cycxxu::t7_enum : Vdp2Regs::VramAccessCommand::no_access};

    VramTiming bank_a1 = {regs_.cyca1l >> Cycxxl::t0_enum,
                          regs_.cyca1l >> Cycxxl::t1_enum,
                          regs_.cyca1l >> Cycxxl::t2_enum,
                          regs_.cyca1l >> Cycxxl::t3_enum,
                          is_normal_mode ? regs_.cyca1u >> Cycxxu::t4_enum : Vdp2Regs::VramAccessCommand::no_access,
                          is_normal_mode ? regs_.cyca1u >> Cycxxu::t5_enum : Vdp2Regs::VramAccessCommand::no_access,
                          is_normal_mode ? regs_.cyca1u >> Cycxxu::t6_enum : Vdp2Regs::VramAccessCommand::no_access,
                          is_normal_mode ? regs_.cyca1u >> Cycxxu::t7_enum : Vdp2Regs::VramAccessCommand::no_access};

    VramTiming bank_b0 = {regs_.cycb0l >> Cycxxl::t0_enum,
                          regs_.cycb0l >> Cycxxl::t1_enum,
                          regs_.cycb0l >> Cycxxl::t2_enum,
                          regs_.cycb0l >> Cycxxl::t3_enum,
                          is_normal_mode ? regs_.cycb0u >> Cycxxu::t4_enum : Vdp2Regs::VramAccessCommand::no_access,
                          is_normal_mode ? regs_.cycb0u >> Cycxxu::t5_enum : Vdp2Regs::VramAccessCommand::no_access,
                          is_normal_mode ? regs_.cycb0u >> Cycxxu::t6_enum : Vdp2Regs::VramAccessCommand::no_access,
                          is_normal_mode ? regs_.cycb0u >> Cycxxu::t7_enum : Vdp2Regs::VramAccessCommand::no_access};

    VramTiming bank_b1 = {regs_.cycb1l >> Cycxxl::t0_enum,
                          regs_.cycb1l >> Cycxxl::t1_enum,
                          regs_.cycb1l >> Cycxxl::t2_enum,
                          regs_.cycb1l >> Cycxxl::t3_enum,
                          is_normal_mode ? regs_.cycb1u >> Cycxxu::t4_enum : Vdp2Regs::VramAccessCommand::no_access,
                          is_normal_mode ? regs_.cycb1u >> Cycxxu::t5_enum : Vdp2Regs::VramAccessCommand::no_access,
                          is_normal_mode ? regs_.cycb1u >> Cycxxu::t6_enum : Vdp2Regs::VramAccessCommand::no_access,
                          is_normal_mode ? regs_.cycb1u >> Cycxxu::t7_enum : Vdp2Regs::VramAccessCommand::no_access};

    using Chctla = Vdp2Regs::Chctla;
    using Chctlb = Vdp2Regs::Chctlb;

    switch (command) {
        using enum Vdp2Regs::VramAccessCommand;
        case nbg0_character_pattern_data_read: {
            if ((regs_.chctla >> Chctla::n0bmen_enum) == Vdp2Regs::BitmapEnable::bitmap_format) {
                return getVramBitmapReads(bank_a0, bank_a1, bank_b0, bank_b1, command);
            }
            const auto cp_size = regs_.chctla >> Chctla::n0chsz_enum;
            return getVramCharacterPatternDataReads(bank_a0,
                                                    bank_a1,
                                                    bank_b0,
                                                    bank_b1,
                                                    command,
                                                    reduction,
                                                    is_normal_mode,
                                                    (cp_size == Vdp2Regs::CharacterSize::two_by_two));
        }
        case nbg1_character_pattern_data_read: {
            if ((regs_.chctla >> Chctla::n1bmen_enum) == Vdp2Regs::BitmapEnable::bitmap_format) {
                return getVramBitmapReads(bank_a0, bank_a1, bank_b0, bank_b1, command);
            }
            const auto cp_size = regs_.chctla >> Chctla::n1chsz_enum;
            return getVramCharacterPatternDataReads(bank_a0,
                                                    bank_a1,
                                                    bank_b0,
                                                    bank_b1,
                                                    command,
                                                    reduction,
                                                    is_normal_mode,
                                                    (cp_size == Vdp2Regs::CharacterSize::two_by_two));
        }
        case nbg2_character_pattern_data_read: {
            const auto cp_size = regs_.chctlb >> Chctlb::n2chsz_enum;
            return getVramCharacterPatternDataReads(bank_a0,
                                                    bank_a1,
                                                    bank_b0,
                                                    bank_b1,
                                                    command,
                                                    reduction,
                                                    is_normal_mode,
                                                    (cp_size == Vdp2Regs::CharacterSize::two_by_two));
        }
        case nbg3_character_pattern_data_read: {
            const auto cp_size = regs_.chctlb >> Chctlb::n3chsz_enum;
            return getVramCharacterPatternDataReads(bank_a0,
                                                    bank_a1,
                                                    bank_b0,
                                                    bank_b1,
                                                    command,
                                                    reduction,
                                                    is_normal_mode,
                                                    (cp_size == Vdp2Regs::CharacterSize::two_by_two));
        }
        case nbg0_pattern_name_read:
        case nbg1_pattern_name_read:
        case nbg2_pattern_name_read:
        case nbg3_pattern_name_read: {
            return getVramPatternNameDataReads(bank_a0, bank_a1, bank_b0, bank_b1, command);
        }
        case nbg0_vertical_cell_scroll_table_data_read:
        case nbg1_vertical_cell_scroll_table_data_read: {
            core::Log::unimplemented(core::tr("VDP2 vertical cell scroll table data read"));
            break;
        }
        case cpu_read_write: {
            break;
        }
        default: core::Log::warning(Logger::vdp2, core::tr("VDP2 VRAM access command not allowed"));
    }

    constexpr auto not_found = u8{0x0};
    return not_found;
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto Vdp2::getVramBitmapReads(const VramTiming&                 bank_a0,
                              const VramTiming&                 bank_a1,
                              const VramTiming&                 bank_b0,
                              const VramTiming&                 bank_b1,
                              const Vdp2Regs::VramAccessCommand command) const -> u8 {
    using Ramctl      = Vdp2Regs::Ramctl;
    auto bitmap_reads = std::ranges::count(bank_a0, command);
    if ((regs_.ramctl >> Ramctl::vramd_enum) == Ramctl::VramMode::partition_in_2_banks) {
        bitmap_reads += std::ranges::count(bank_a1, command);
    }
    bitmap_reads += std::ranges::count(bank_b0, command);
    if ((regs_.ramctl >> Ramctl::vrbmd_enum) == Ramctl::VramMode::partition_in_2_banks) {
        bitmap_reads += std::ranges::count(bank_b1, command);
    }
    return static_cast<u8>(bitmap_reads);
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto Vdp2::getVramPatternNameDataReads(const VramTiming&                 bank_a0,
                                       const VramTiming&                 bank_a1,
                                       const VramTiming&                 bank_b0,
                                       const VramTiming&                 bank_b1,
                                       const Vdp2Regs::VramAccessCommand command) const -> u8 {
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
    using Ramctl   = Vdp2Regs::Ramctl;
    auto pnd_reads = ptrdiff_t{};
    if ((regs_.ramctl >> Ramctl::vramd_enum) == Ramctl::VramMode::partition_in_2_banks
        && ((regs_.ramctl >> Ramctl::vrbmd_enum) == Ramctl::VramMode::partition_in_2_banks)) {
        auto pnd_reads_bank_0 = std::ranges::count(bank_a0, command);
        pnd_reads_bank_0 += std::ranges::count(bank_b0, command);

        auto pnd_reads_bank_1 = std::ranges::count(bank_a1, command);
        pnd_reads_bank_1 += std::ranges::count(bank_b1, command);

        pnd_reads = (pnd_reads_bank_0 > pnd_reads_bank_1) ? pnd_reads_bank_0 : pnd_reads_bank_1;
    } else {
        pnd_reads += std::ranges::count(bank_a0, command);
        pnd_reads += std::ranges::count(bank_b0, command);
    }
    return static_cast<u8>(pnd_reads);
}

// static
auto Vdp2::getReductionSetting(Vdp2Regs::Zmctl::ZoomQuarter zq, Vdp2Regs::Zmctl::ZoomHalf zh) -> ReductionSetting {
    if (zq == Vdp2Regs::Zmctl::ZoomQuarter::up_to_one_quarter) { return ReductionSetting::up_to_one_quarter; }
    if (zh == Vdp2Regs::Zmctl::ZoomHalf::up_to_one_half) { return ReductionSetting::up_to_one_half; }
    return ReductionSetting::none;
};

// static
auto Vdp2::calculateRequiredVramCharacterPatternReads(ReductionSetting r, Vdp2Regs::CharacterColorNumber3Bits ccn)
    -> VramAccessNumber {
    switch (ccn) {
        using enum Vdp2Regs::CharacterColorNumber3Bits;
        case palette_16:
            switch (r) {
                using enum ReductionSetting;
                case none: return VramAccessNumber::one;
                case up_to_one_half: return VramAccessNumber::two;
                case up_to_one_quarter: return VramAccessNumber::four;
            }
            break;
        case palette_256:
            switch (r) {
                using enum ReductionSetting;
                case none: return VramAccessNumber::two;
                case up_to_one_half: return VramAccessNumber::four;
                default: return VramAccessNumber::none;
            }
            break;
        case palette_2048:
            switch (r) {
                using enum ReductionSetting;
                case none: return VramAccessNumber::four;
                default: return VramAccessNumber::none;
            }
            break;
        case rgb_32k:
            switch (r) {
                using enum ReductionSetting;
                case none: return VramAccessNumber::four;
                default: return VramAccessNumber::none;
            }
            break;
        case rgb_16m:
            switch (r) {
                using enum ReductionSetting;
                case none: return VramAccessNumber::eight;
                default: return VramAccessNumber::none;
            }
            break;
    }

    return VramAccessNumber::none;
}

// static
auto Vdp2::calculateRequiredVramCharacterPatternReads(ReductionSetting r, Vdp2Regs::CharacterColorNumber2Bits ccn)
    -> VramAccessNumber {
    switch (ccn) {
        using enum Vdp2Regs::CharacterColorNumber2Bits;
        case palette_16:
            switch (r) {
                using enum ReductionSetting;
                case none: return VramAccessNumber::one;
                case up_to_one_half: return VramAccessNumber::two;
                case up_to_one_quarter: return VramAccessNumber::four;
            }
            break;
        case palette_256:
            switch (r) {
                using enum ReductionSetting;
                case none: return VramAccessNumber::two;
                case up_to_one_half: return VramAccessNumber::four;
                default: return VramAccessNumber::none;
            }
            break;
        case palette_2048:
            switch (r) {
                using enum ReductionSetting;
                case none: return VramAccessNumber::four;
                default: return VramAccessNumber::none;
            }
            break;
        case rgb_32k:
            switch (r) {
                using enum ReductionSetting;
                case none: return VramAccessNumber::four;
                default: return VramAccessNumber::none;
            }
            break;
    }

    return VramAccessNumber::none;
}

// static
auto Vdp2::calculateRequiredVramCharacterPatternReads(ReductionSetting r, Vdp2Regs::CharacterColorNumber1Bit ccn)
    -> VramAccessNumber {
    switch (ccn) {
        using enum Vdp2Regs::CharacterColorNumber1Bit;
        case palette_16:
            switch (r) {
                using enum ReductionSetting;
                case none: return VramAccessNumber::one;
                case up_to_one_half: return VramAccessNumber::two;
                case up_to_one_quarter: return VramAccessNumber::four;
            }
            break;
        case palette_256:
            switch (r) {
                using enum ReductionSetting;
                case none: return VramAccessNumber::two;
                case up_to_one_half: return VramAccessNumber::four;
                default: return VramAccessNumber::none;
            }
            break;
    }

    return VramAccessNumber::none;
}

// static
auto Vdp2::calculateRequiredVramPatternNameReads(ReductionSetting r) -> VramAccessNumber {
    switch (r) {
        using enum ReductionSetting;
        case up_to_one_quarter: return VramAccessNumber::four;
        case up_to_one_half: return VramAccessNumber::two;
        default: return VramAccessNumber::one;
    }
};

// static
auto Vdp2::getPatternNameFromCharacterPattern(const Vdp2Regs::VramAccessCommand character_pattern)
    -> Vdp2Regs::VramAccessCommand {
    switch (character_pattern) {
        using enum Vdp2Regs::VramAccessCommand;
        case nbg0_character_pattern_data_read: {
            return Vdp2Regs::VramAccessCommand::nbg0_pattern_name_read;
        }
        case nbg1_character_pattern_data_read: {
            return Vdp2Regs::VramAccessCommand::nbg1_pattern_name_read;
        }
        case nbg2_character_pattern_data_read: {
            return Vdp2Regs::VramAccessCommand::nbg2_pattern_name_read;
        }
        case nbg3_character_pattern_data_read: {
            return Vdp2Regs::VramAccessCommand::nbg3_pattern_name_read;
        }
        default: return Vdp2Regs::VramAccessCommand::no_access;
    }
}

// static
void Vdp2::setPatternNameAccess(const VramTiming&                   bank,
                                const Vdp2Regs::VramAccessCommand   pattern,
                                std::array<bool, vram_timing_size>& pnd_access) {
    auto it = std::ranges::find(bank, pattern);
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
    auto it = std::ranges::find(pnd_access, true);
    while (it != pnd_access.end()) {
        switch (std::distance(pnd_access.begin(), it)) {
            case t0: {
                if (is_screen_mode_normal) {
                    allowed_cpd_timing[t0] = true;
                    allowed_cpd_timing[t1] = true;
                    allowed_cpd_timing[t2] = true;
                    allowed_cpd_timing[t3] = true; // should be false
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
auto Vdp2::getVramCharacterPatternDataReads(const VramTiming&                 bank_a0,
                                            const VramTiming&                 bank_a1,
                                            const VramTiming&                 bank_b0,
                                            const VramTiming&                 bank_b1,
                                            const Vdp2Regs::VramAccessCommand command,
                                            const ReductionSetting            reduction,
                                            const bool                        is_screen_mode_normal,
                                            const bool                        is_using_2_by_2_cp) -> u8 {
    // From the command we must use the linked Pattern Name Data. The limitations are based on the PND read position.
    // Step 1 : find PND reads for the current command
    const auto pnd = getPatternNameFromCharacterPattern(command);
    // const auto pnd_reads = getVramPatternNameDataReads(bank_a0, bank_a1, bank_b0, bank_b1, pnd);

    auto pnd_timing_access = std::array{false, false, false, false, false, false, false, false};
    setPatternNameAccess(bank_a0, pnd, pnd_timing_access);
    setPatternNameAccess(bank_b0, pnd, pnd_timing_access);
    setPatternNameAccess(bank_a1, pnd, pnd_timing_access);
    setPatternNameAccess(bank_b1, pnd, pnd_timing_access);

    // If there's no reduction, observe selection limits when CPD read access >= 2.
    // If reduction = 1/2 or 1/4, the behavior isn't clear from the docs ... I'll just
    // observe selection limits for every access.
    auto are_limitations_applied = bool{true};
    if (reduction == ReductionSetting::none) {
        auto unlimited_cpd_reads = std::ranges::count(bank_a0, command);
        unlimited_cpd_reads += std::ranges::count(bank_b0, command);
        unlimited_cpd_reads += std::ranges::count(bank_a1, command);
        unlimited_cpd_reads += std::ranges::count(bank_b1, command);
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

        // Timings limitations are disabled for now (cf https://github.com/rtoumazet/saturnin/issues/255)
        // setCharacterPatternLimitations(is_screen_mode_normal, is_using_2_by_2_cp, pnd_timing_access, allowed_cpd_timing);
        if (is_screen_mode_normal) {
            allowed_cpd_timing = std::array{true, true, true, true, true, true, true, true};
        } else {
            allowed_cpd_timing = std::array{true, true, true, true, false, false, false, false};
        }

        // Step 3 : get the reads
        // First access not available are changed to no access
        auto it = std::ranges::find(allowed_cpd_timing, false);
        while (it != allowed_cpd_timing.end()) {
            limited_bank_a0[std::distance(allowed_cpd_timing.begin(), it)] = Vdp2Regs::VramAccessCommand::no_access;
            limited_bank_b0[std::distance(allowed_cpd_timing.begin(), it)] = Vdp2Regs::VramAccessCommand::no_access;
            limited_bank_a1[std::distance(allowed_cpd_timing.begin(), it)] = Vdp2Regs::VramAccessCommand::no_access;
            limited_bank_b1[std::distance(allowed_cpd_timing.begin(), it)] = Vdp2Regs::VramAccessCommand::no_access;
            ++it;
            it = std::find(it, allowed_cpd_timing.end(), false);
        }
    }
    // Counting cpd access
    auto cpd_reads = std::ranges::count(limited_bank_a0, command);
    cpd_reads += std::ranges::count(limited_bank_b0, command);
    cpd_reads += std::ranges::count(limited_bank_a1, command);
    cpd_reads += std::ranges::count(limited_bank_b1, command);

    return static_cast<u8>(cpd_reads);
};

//--------------------------------------------------------------------------------------------------------------
// DISPLAY methods
//--------------------------------------------------------------------------------------------------------------

void Vdp2::clearRenderData(const ScrollScreen s) { std::vector<video::Vdp2Part>().swap(vdp2_parts_[toUnderlying(s)]); }

void Vdp2::populateRenderData() {
    clearRenderData(ScrollScreen::rbg1);
    if (isScreenDisplayed(ScrollScreen::rbg1)) {
        updateScrollScreenStatus(ScrollScreen::rbg1);
        if (getScreen(ScrollScreen::rbg1).priority_number != 0) { readScrollScreenData(ScrollScreen::rbg1); }
    } else {
        // discardCache(ScrollScreen::rbg1);
    }

    clearRenderData(ScrollScreen::rbg0);
    if (isScreenDisplayed(ScrollScreen::rbg0)) {
        updateScrollScreenStatus(ScrollScreen::rbg0);
        if (getScreen(ScrollScreen::rbg0).priority_number != 0) { readScrollScreenData(ScrollScreen::rbg0); }
    } else {
        // discardCache(ScrollScreen::rbg0);
    }
    auto is_nbg_displayed
        = !(getScreen(ScrollScreen::rbg0).is_display_enabled && getScreen(ScrollScreen::rbg1).is_display_enabled);

    if (is_nbg_displayed) {
        clearRenderData(ScrollScreen::nbg0);
        if (isScreenDisplayed(ScrollScreen::nbg0)) {
            updateScrollScreenStatus(ScrollScreen::nbg0);
            if (getScreen(ScrollScreen::nbg0).priority_number != 0) { readScrollScreenData(ScrollScreen::nbg0); }
        }

        clearRenderData(ScrollScreen::nbg1);
        if (canScrollScreenBeDisplayed(ScrollScreen::nbg1)) {
            if (isScreenDisplayed(ScrollScreen::nbg1)) {
                updateScrollScreenStatus(ScrollScreen::nbg1);
                if (getScreen(ScrollScreen::nbg1).priority_number != 0) { readScrollScreenData(ScrollScreen::nbg1); }
            }
        }

        clearRenderData(ScrollScreen::nbg2);
        if (canScrollScreenBeDisplayed(ScrollScreen::nbg2)) {
            if (isScreenDisplayed(ScrollScreen::nbg2)) {
                updateScrollScreenStatus(ScrollScreen::nbg2);
                if (getScreen(ScrollScreen::nbg2).priority_number != 0) { readScrollScreenData(ScrollScreen::nbg2); }
            }
        } else {
            // bg_[util::toUnderlying(ScrollScreen::nbg2)] = {};
        }

        // clearRenderData(ScrollScreen::nbg3);
        // if (canScrollScreenBeDisplayed(ScrollScreen::nbg3)) {
        //     if (isScreenDisplayed(ScrollScreen::nbg3)) {
        //         updateScrollScreenStatus(ScrollScreen::nbg3);
        //         if (getScreen(ScrollScreen::nbg3).priority_number != 0) { readScrollScreenData(ScrollScreen::nbg3); }
        //     }
        // }

        // WIP
        if (canScrollScreenBeDisplayed(ScrollScreen::nbg3) && isScreenDisplayed(ScrollScreen::nbg3)) {
            updateScrollScreenStatus(ScrollScreen::nbg3);
            if (isCacheDirty(ScrollScreen::nbg3) && getScreen(ScrollScreen::nbg3).priority_number != 0) {
                discardCache(ScrollScreen::nbg3);
                clearRenderData(ScrollScreen::nbg3);
                readScrollScreenData(ScrollScreen::nbg3);
                // modules_.opengl()->setFboStatus(getScreen(ScrollScreen::nbg3).priority_number,
                //                                 ScrollScreen::nbg3,
                //                                 FboStatus::to_set);
            } else {
                // Clear data (how ?)
            }
        }
    }
}

auto Vdp2::canScrollScreenBeDisplayed(const ScrollScreen s) const -> bool {
    const auto nbg0_color_nb = getScreen(ScrollScreen::nbg0).character_color_number;
    const auto nbg1_color_nb = getScreen(ScrollScreen::nbg1).character_color_number;
    switch (s) {
        using enum ScrollScreen;
        case nbg1: {
            return (nbg0_color_nb != ColorCount::rgb_16m);
        }
        case nbg2: {
            const auto colors_preventing_display = std::array{ColorCount::palette_2048, ColorCount::rgb_32k, ColorCount::rgb_16m};
            return std::ranges::none_of(colors_preventing_display,
                                        [&nbg0_color_nb](const ColorCount cc) { return cc == nbg0_color_nb; });
        }
        case nbg3: {
            if (nbg0_color_nb == ColorCount::rgb_16m) { return false; }
            const auto colors_preventing_display = std::array{ColorCount::palette_2048, ColorCount::rgb_32k};
            return std::ranges::none_of(colors_preventing_display,
                                        [&nbg1_color_nb](const ColorCount cc) { return cc == nbg1_color_nb; });
        }
        default: return true;
    }
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void Vdp2::updateScrollScreenStatus(const ScrollScreen s) {
    using Pcnxx = Vdp2Regs::Pcnxx;
    using Plsz  = Vdp2Regs::Plsz;
    using Scin  = Vdp2Regs::Scin;
    using Scdn  = Vdp2Regs::Scdn;

    constexpr auto map_size_nbg = u8{2 * 2};
    constexpr auto map_size_rbg = u8{4 * 4};
    constexpr auto cell_size    = u8{8 * 8};

    const auto getCharacterColorNumber3Bits = [](const Vdp2Regs::CharacterColorNumber3Bits c, const ScreenModeType t) {
        switch (c) {
            using enum Vdp2Regs::CharacterColorNumber3Bits;
            case palette_16: return ColorCount::palette_16;
            case palette_256: return ColorCount::palette_256;
            case palette_2048: return ColorCount::palette_2048;
            case rgb_32k: {
                return (t == ScreenModeType::normal) ? ColorCount::rgb_32k : ColorCount::not_allowed;
            }
            case rgb_16m: return ColorCount::rgb_16m;
            default: return ColorCount::not_allowed;
        }
    };
    const auto getCharacterColorNumber2Bits = [](const Vdp2Regs::CharacterColorNumber2Bits c, const ScreenModeType t) {
        switch (c) {
            using enum Vdp2Regs::CharacterColorNumber2Bits;
            case palette_16: return ColorCount::palette_16;
            case palette_256: return ColorCount::palette_256;
            case palette_2048: return ColorCount::palette_2048;
            case rgb_32k: {
                return (t == ScreenModeType::exclusive) ? ColorCount::rgb_16m : ColorCount::rgb_32k;
            }
            default: return ColorCount::not_allowed;
        }
    };
    const auto getCharacterColorNumber1Bit = [](const Vdp2Regs::CharacterColorNumber1Bit c) {
        switch (c) {
            using enum Vdp2Regs::CharacterColorNumber1Bit;
            case palette_16: return ColorCount::palette_16;
            case palette_256: return ColorCount::palette_256;
            default: return ColorCount::not_allowed;
        }
    };
    const auto getCharacterColorNumberRbg0 = [](const Vdp2Regs::CharacterColorNumber3Bits c, const ScreenModeType t) {
        if (t == ScreenModeType::exclusive) { return ColorCount::cannot_display; }

        switch (c) {
            using enum Vdp2Regs::CharacterColorNumber3Bits;
            case palette_16: return ColorCount::palette_16;
            case palette_256: return ColorCount::palette_256;
            case palette_2048: return ColorCount::palette_2048;
            case rgb_32k: return ColorCount::rgb_32k;
            case rgb_16m: {
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
            using enum ColorCount;
            case palette_16: return dot_size_4;
            case palette_256: return dot_size_8;
            case palette_2048: return dot_size_16;
            case rgb_32k: return dot_size_16;
            default: return dot_size_32;
        }
    };
    const auto getPageSize = [](const Pcnxx::PatternNameDataSize pnd_sz, const Vdp2Regs::CharacterSize ch_sz) {
        constexpr auto boundary_1_word_1_by_1_cell  = u16{0x2000};
        constexpr auto boundary_1_word_2_by_2_cells = u16{0x800};
        if (pnd_sz == Pcnxx::PatternNameDataSize::one_word) {
            return (ch_sz == Vdp2Regs::CharacterSize::one_by_one) ? boundary_1_word_1_by_1_cell : boundary_1_word_2_by_2_cells;
        }

        // The other case is 2 words.
        constexpr auto boundary_2_words_1_by_1_cell  = u16{0x4000};
        constexpr auto boundary_2_words_2_by_2_cells = u16{0x1000};
        return (ch_sz == Vdp2Regs::CharacterSize::one_by_one) ? boundary_2_words_1_by_1_cell : boundary_2_words_2_by_2_cells;
    };

    const auto getScrollScreenFormat = [](const Vdp2Regs::BitmapEnable be) {
        return (be == Vdp2Regs::BitmapEnable::cell_format) ? ScrollScreenFormat::cell : ScrollScreenFormat::bitmap;
    };

    const auto getBitmapSize = [](const Vdp2Regs::BitmapSize2Bits sz) {
        switch (sz) {
            using enum Vdp2Regs::BitmapSize2Bits;
            case size_512_by_256: return BitmapSize::size_512_by_256;
            case size_512_by_512: return BitmapSize::size_512_by_512;
            case size_1024_by_256: return BitmapSize::size_1024_by_256;
            case size_1024_by_512: return BitmapSize::size_1024_by_512;
            default: return BitmapSize::not_set;
        }
    };

    const auto getBitmapStartAddress = [](const u8 map_offset) {
        constexpr auto boundary = u32{0x20000};
        return vram_start_address + map_offset * boundary;
    };

    auto& screen         = getScreen(s);
    screen.scroll_screen = s;

    using Bgon   = Vdp2Regs::Bgon;
    using Chctla = Vdp2Regs::Chctla;
    using Chctlb = Vdp2Regs::Chctlb;
    using Bmpna  = Vdp2Regs::Bmpna;
    using Bmpnb  = Vdp2Regs::Bmpnb;
    using Mpofn  = Vdp2Regs::Mpofn;
    using Mpofr  = Vdp2Regs::Mpofr;
    using Mpab   = Vdp2Regs::Mpab;
    using Mpcd   = Vdp2Regs::Mpcd;
    using Mpef   = Vdp2Regs::Mpef;
    using Mpgh   = Vdp2Regs::Mpgh;
    using Mpij   = Vdp2Regs::Mpij;
    using Mpkl   = Vdp2Regs::Mpkl;
    using Mpmn   = Vdp2Regs::Mpmn;
    using Mpop   = Vdp2Regs::Mpop;
    using Craofa = Vdp2Regs::Craofa;
    using Craofb = Vdp2Regs::Craofb;
    using Prina  = Vdp2Regs::Prina;
    using Prinb  = Vdp2Regs::Prinb;
    using Prir   = Vdp2Regs::Prir;

    switch (s) {
        using enum ScrollScreen;
        case nbg0:
            // Color RAM
            screen.color_ram_address_offset = getColorRamAddressOffset(static_cast<u8>(regs_.craofa >> Craofa::n0caos_shft));

            // Transparency
            screen.is_transparency_code_valid
                = ((regs_.bgon >> Bgon::n0tpon_enum) == Bgon::TransparentDisplayEnable::transparency_code_valid);

            // Priority
            screen.priority_number = static_cast<u8>(regs_.prina >> Prina::n0prin_shft);

            // Map
            screen.map_size   = map_size_nbg;
            screen.map_offset = static_cast<u8>(regs_.mpofn >> Mpofn::n0mp_shft);

            // Plane
            screen.plane_size            = regs_.plsz >> Vdp2Regs::Plsz::n0plsz_enum;
            screen.plane_a_start_address = calculatePlaneStartAddress(s, regs_.mpabn0 >> Mpab::mpa_shft);
            screen.plane_b_start_address = calculatePlaneStartAddress(s, regs_.mpabn0 >> Mpab::mpb_shft);
            screen.plane_c_start_address = calculatePlaneStartAddress(s, regs_.mpcdn0 >> Mpcd::mpc_shft);
            screen.plane_d_start_address = calculatePlaneStartAddress(s, regs_.mpcdn0 >> Mpcd::mpd_shft);

            // Page
            screen.page_size = getPageSize(regs_.pncn0 >> Pcnxx::pnb_enum, regs_.chctla >> Chctla::n0chsz_enum);

            // Pattern name data
            screen.pattern_name_data_size           = regs_.pncn0 >> Pcnxx::pnb_enum;
            screen.character_number_supplement_mode = regs_.pncn0 >> Pcnxx::cnsm_enum;
            screen.special_priority                 = static_cast<u8>(regs_.pncn0 >> Pcnxx::spr_shft);
            screen.special_color_calculation        = static_cast<u8>(regs_.pncn0 >> Pcnxx::scc_shft);
            screen.supplementary_palette_number     = static_cast<u8>(regs_.pncn0 >> Pcnxx::splt_shft);
            screen.supplementary_character_number   = static_cast<u8>(regs_.pncn0 >> Pcnxx::scn_shft);

            // Character pattern
            screen.character_pattern_size = regs_.chctla >> Chctla::n0chsz_enum;
            screen.character_color_number
                = getCharacterColorNumber3Bits(regs_.chctla >> Chctla::n0chcn_enum, tv_screen_status_.screen_mode_type);
            screen.format = getScrollScreenFormat(regs_.chctla >> Chctla::n0bmen_enum);

            // Cell
            screen.cell_size = cell_size * getDotSize(screen.character_color_number) / bits_in_a_byte;

            // Bitmap
            screen.bitmap_size                      = getBitmapSize(regs_.chctla >> Chctla::n0bmsz_enum);
            screen.bitmap_palette_number            = static_cast<u8>(regs_.bmpna >> Bmpna::n0bmpx_shft);
            screen.bitmap_special_priority          = static_cast<u8>(regs_.bmpna >> Bmpna::n0bmpr_shft);
            screen.bitmap_special_color_calculation = static_cast<u8>(regs_.bmpna >> Bmpna::n0bmcc_shft);
            screen.bitmap_start_address             = getBitmapStartAddress(screen.map_offset);

            // Scroll screen
            screen.screen_scroll_horizontal_integer    = regs_.scxin0 >> Scin::nsci_shft;
            screen.screen_scroll_horizontal_fractional = static_cast<u8>(regs_.scxdn0 >> Scdn::nscd_shft);
            screen.screen_scroll_vertical_integer      = regs_.scyin0 >> Scin::nsci_shft;
            screen.screen_scroll_vertical_fractional   = static_cast<u8>(regs_.scydn0 >> Scdn::nscd_shft);

            // Color offset
            screen.color_offset = getColorOffset(Layer::nbg0);

            break;
        case nbg1:
            // Color RAM
            screen.color_ram_address_offset = getColorRamAddressOffset(static_cast<u8>(regs_.craofa >> Craofa::n1caos_shft));

            // Transparency
            screen.is_transparency_code_valid
                = ((regs_.bgon >> Bgon::n1tpon_enum) == Bgon::TransparentDisplayEnable::transparency_code_valid);
            // Priority
            screen.priority_number = static_cast<u8>(regs_.prina >> Prina::n1prin_shft);

            // Map
            screen.map_size   = map_size_nbg;
            screen.map_offset = static_cast<u8>(regs_.mpofn >> Mpofn::n1mp_shft);

            // Plane
            screen.plane_size            = regs_.plsz >> Vdp2Regs::Plsz::n1plsz_enum;
            screen.plane_a_start_address = calculatePlaneStartAddress(s, regs_.mpabn1 >> Mpab::mpa_shft);
            screen.plane_b_start_address = calculatePlaneStartAddress(s, regs_.mpabn1 >> Mpab::mpb_shft);
            screen.plane_c_start_address = calculatePlaneStartAddress(s, regs_.mpcdn1 >> Mpcd::mpc_shft);
            screen.plane_d_start_address = calculatePlaneStartAddress(s, regs_.mpcdn1 >> Mpcd::mpd_shft);

            // Page
            screen.page_size = getPageSize(regs_.pncn1 >> Pcnxx::pnb_enum, regs_.chctla >> Chctla::n1chsz_enum);

            // Pattern name data
            screen.pattern_name_data_size           = regs_.pncn1 >> Pcnxx::pnb_enum;
            screen.character_number_supplement_mode = regs_.pncn1 >> Pcnxx::cnsm_enum;
            screen.special_priority                 = static_cast<u8>(regs_.pncn1 >> Pcnxx::spr_shft);
            screen.special_color_calculation        = static_cast<u8>(regs_.pncn1 >> Pcnxx::scc_shft);
            screen.supplementary_palette_number     = static_cast<u8>(regs_.pncn1 >> Pcnxx::splt_shft);
            screen.supplementary_character_number   = static_cast<u8>(regs_.pncn1 >> Pcnxx::scn_shft);

            // Character pattern
            screen.character_pattern_size = regs_.chctla >> Chctla::n1chsz_enum;
            screen.character_color_number
                = getCharacterColorNumber2Bits(regs_.chctla >> Chctla::n1chcn_enum, tv_screen_status_.screen_mode_type);
            screen.format = getScrollScreenFormat(regs_.chctla >> Chctla::n1bmen_enum);

            // Cell
            screen.cell_size = cell_size * getDotSize(screen.character_color_number) / bits_in_a_byte;

            // Bitmap
            screen.bitmap_size                      = getBitmapSize(regs_.chctla >> Chctla::n1bmsz_enum);
            screen.bitmap_palette_number            = static_cast<u8>(regs_.bmpna >> Bmpna::n1bmpx_shft);
            screen.bitmap_special_priority          = static_cast<u8>(regs_.bmpna >> Bmpna::n1bmpr_shft);
            screen.bitmap_special_color_calculation = static_cast<u8>(regs_.bmpna >> Bmpna::n1bmcc_shft);
            screen.bitmap_start_address             = getBitmapStartAddress(screen.map_offset);

            // Scroll screen
            screen.screen_scroll_horizontal_integer    = regs_.scxin1 >> Scin::nsci_shft;
            screen.screen_scroll_horizontal_fractional = static_cast<u8>(regs_.scxdn1 >> Scdn::nscd_shft);
            screen.screen_scroll_vertical_integer      = regs_.scyin1 >> Scin::nsci_shft;
            screen.screen_scroll_vertical_fractional   = static_cast<u8>(regs_.scydn1 >> Scdn::nscd_shft);

            // Color offset
            screen.color_offset = getColorOffset(Layer::nbg1);
            break;
        case nbg2:
            // Color RAM
            screen.color_ram_address_offset = getColorRamAddressOffset(static_cast<u8>(regs_.craofa >> Craofa::n2caos_shft));

            // Transparency
            screen.is_transparency_code_valid
                = ((regs_.bgon >> Bgon::n2tpon_enum) == Bgon::TransparentDisplayEnable::transparency_code_valid);

            // Priority
            screen.priority_number = static_cast<u8>(regs_.prinb >> Prinb::n2prin_shft);

            // Map
            screen.map_size   = map_size_nbg;
            screen.map_offset = static_cast<u8>(regs_.mpofn >> Mpofn::n2mp_shft);

            // Plane
            screen.plane_size            = regs_.plsz >> Vdp2Regs::Plsz::n2plsz_enum;
            screen.plane_a_start_address = calculatePlaneStartAddress(s, regs_.mpabn2 >> Mpab::mpa_shft);
            screen.plane_b_start_address = calculatePlaneStartAddress(s, regs_.mpabn2 >> Mpab::mpb_shft);
            screen.plane_c_start_address = calculatePlaneStartAddress(s, regs_.mpcdn2 >> Mpcd::mpc_shft);
            screen.plane_d_start_address = calculatePlaneStartAddress(s, regs_.mpcdn2 >> Mpcd::mpd_shft);

            // Page
            screen.page_size = getPageSize(regs_.pncn2 >> Pcnxx::pnb_enum, regs_.chctlb >> Chctlb::n2chsz_enum);

            // Pattern name data
            screen.pattern_name_data_size           = regs_.pncn2 >> Pcnxx::pnb_enum;
            screen.character_number_supplement_mode = regs_.pncn2 >> Pcnxx::cnsm_enum;
            screen.special_priority                 = static_cast<u8>(regs_.pncn2 >> Pcnxx::spr_shft);
            screen.special_color_calculation        = static_cast<u8>(regs_.pncn2 >> Pcnxx::scc_shft);
            screen.supplementary_palette_number     = static_cast<u8>(regs_.pncn2 >> Pcnxx::splt_shft);
            screen.supplementary_character_number   = static_cast<u8>(regs_.pncn2 >> Pcnxx::scn_shft);

            // Character pattern
            screen.character_pattern_size = regs_.chctlb >> Chctlb::n2chsz_enum;
            screen.character_color_number = getCharacterColorNumber1Bit(regs_.chctlb >> Chctlb::n2chcn_enum);
            screen.bitmap_size            = BitmapSize::not_set;
            screen.bitmap_start_address   = 0;

            // Cell
            screen.cell_size = cell_size * getDotSize(screen.character_color_number) / bits_in_a_byte;

            // Scroll screen
            screen.screen_scroll_horizontal_integer = regs_.scxn2 >> Scin::nsci_shft;
            screen.screen_scroll_vertical_integer   = regs_.scyn2 >> Scin::nsci_shft;

            // Color offset
            screen.color_offset = getColorOffset(Layer::nbg2);
            break;

        case nbg3:
            // Color RAM
            screen.color_ram_address_offset = getColorRamAddressOffset(static_cast<u8>(regs_.craofa >> Craofa::n3caos_shft));

            // Transparency
            screen.is_transparency_code_valid
                = ((regs_.bgon >> Bgon::n3tpon_enum) == Bgon::TransparentDisplayEnable::transparency_code_valid);

            // Priority
            screen.priority_number = static_cast<u8>(regs_.prinb >> Prinb::n3prin_shft);

            // Map
            screen.map_size   = map_size_nbg;
            screen.map_offset = static_cast<u8>(regs_.mpofn >> Mpofn::n3mp_shft);

            // Plane
            screen.plane_size            = regs_.plsz >> Vdp2Regs::Plsz::n3plsz_enum;
            screen.plane_a_start_address = calculatePlaneStartAddress(s, regs_.mpabn3 >> Mpab::mpa_shft);
            screen.plane_b_start_address = calculatePlaneStartAddress(s, regs_.mpabn3 >> Mpab::mpb_shft);
            screen.plane_c_start_address = calculatePlaneStartAddress(s, regs_.mpcdn3 >> Mpcd::mpc_shft);
            screen.plane_d_start_address = calculatePlaneStartAddress(s, regs_.mpcdn3 >> Mpcd::mpd_shft);

            // Page
            screen.page_size = getPageSize(regs_.pncn3 >> Pcnxx::pnb_enum, regs_.chctlb >> Chctlb::n3chsz_enum);

            // Pattern name data
            screen.pattern_name_data_size           = regs_.pncn3 >> Pcnxx::pnb_enum;
            screen.character_number_supplement_mode = regs_.pncn3 >> Pcnxx::cnsm_enum;
            screen.special_priority                 = static_cast<u8>(regs_.pncn3 >> Pcnxx::spr_shft);
            screen.special_color_calculation        = static_cast<u8>(regs_.pncn3 >> Pcnxx::scc_shft);
            screen.supplementary_palette_number     = static_cast<u8>(regs_.pncn3 >> Pcnxx::splt_shft);
            screen.supplementary_character_number   = static_cast<u8>(regs_.pncn3 >> Pcnxx::scn_shft);

            // Character pattern
            screen.character_pattern_size = regs_.chctlb >> Chctlb::n3chsz_enum;
            screen.character_color_number = getCharacterColorNumber1Bit(regs_.chctlb >> Chctlb::n3chcn_enum);
            screen.bitmap_size            = BitmapSize::not_set;
            screen.bitmap_start_address   = 0;

            // Cell
            screen.cell_size = cell_size * getDotSize(screen.character_color_number) / bits_in_a_byte;

            // Scroll screen
            screen.screen_scroll_horizontal_integer = regs_.scxn3 >> Scin::nsci_shft;
            screen.screen_scroll_vertical_integer   = regs_.scyn3 >> Scin::nsci_shft;

            // Color offset
            screen.color_offset = getColorOffset(Layer::nbg3);
            break;

        case rbg0:
            // Color RAM
            screen.color_ram_address_offset = getColorRamAddressOffset(static_cast<u8>(regs_.craofb >> Craofb::r0caos_shft));

            // Transparency
            screen.is_transparency_code_valid
                = ((regs_.bgon >> Bgon::r0tpon_enum) == Bgon::TransparentDisplayEnable::transparency_code_valid);

            // Priority
            screen.priority_number = static_cast<u8>(regs_.prir >> Prir::r0prin_shft);

            // Map
            screen.map_size   = map_size_rbg;
            screen.map_offset = static_cast<u8>(regs_.mpofr >> Mpofr::ramp_shft);

            // Plane
            screen.plane_size            = regs_.plsz >> Vdp2Regs::Plsz::raplsz_enum;
            screen.plane_a_start_address = calculatePlaneStartAddress(s, regs_.mpabra >> Mpab::mpa_shft);
            screen.plane_b_start_address = calculatePlaneStartAddress(s, regs_.mpabra >> Mpab::mpb_shft);
            screen.plane_c_start_address = calculatePlaneStartAddress(s, regs_.mpcdra >> Mpcd::mpc_shft);
            screen.plane_d_start_address = calculatePlaneStartAddress(s, regs_.mpcdra >> Mpcd::mpd_shft);
            screen.plane_e_start_address = calculatePlaneStartAddress(s, regs_.mpefra >> Mpef::mpe_shft);
            screen.plane_f_start_address = calculatePlaneStartAddress(s, regs_.mpefra >> Mpef::mpf_shft);
            screen.plane_g_start_address = calculatePlaneStartAddress(s, regs_.mpghra >> Mpgh::mpg_shft);
            screen.plane_h_start_address = calculatePlaneStartAddress(s, regs_.mpghra >> Mpgh::mph_shft);
            screen.plane_i_start_address = calculatePlaneStartAddress(s, regs_.mpijra >> Mpij::mpi_shft);
            screen.plane_j_start_address = calculatePlaneStartAddress(s, regs_.mpijra >> Mpij::mpj_shft);
            screen.plane_k_start_address = calculatePlaneStartAddress(s, regs_.mpklra >> Mpkl::mpk_shft);
            screen.plane_l_start_address = calculatePlaneStartAddress(s, regs_.mpklra >> Mpkl::mpl_shft);
            screen.plane_m_start_address = calculatePlaneStartAddress(s, regs_.mpmnra >> Mpmn::mpm_shft);
            screen.plane_n_start_address = calculatePlaneStartAddress(s, regs_.mpmnra >> Mpmn::mpn_shft);
            screen.plane_o_start_address = calculatePlaneStartAddress(s, regs_.mpopra >> Mpop::mpo_shft);
            screen.plane_p_start_address = calculatePlaneStartAddress(s, regs_.mpopra >> Mpop::mpp_shft);

            // Page
            screen.page_size = getPageSize(regs_.pncr >> Pcnxx::pnb_enum, regs_.chctlb >> Chctlb::r0chsz_enum);

            // Pattern name data
            screen.pattern_name_data_size           = regs_.pncr >> Pcnxx::pnb_enum;
            screen.character_number_supplement_mode = regs_.pncr >> Pcnxx::cnsm_enum;
            screen.special_priority                 = static_cast<u8>(regs_.pncr >> Pcnxx::spr_shft);
            screen.special_color_calculation        = static_cast<u8>(regs_.pncr >> Pcnxx::scc_shft);
            screen.supplementary_palette_number     = static_cast<u8>(regs_.pncr >> Pcnxx::splt_shft);
            screen.supplementary_character_number   = static_cast<u8>(regs_.pncr >> Pcnxx::scn_shft);

            // Character pattern
            screen.character_pattern_size = regs_.chctlb >> Chctlb::r0chsz_enum;
            screen.character_color_number
                = getCharacterColorNumberRbg0(regs_.chctlb >> Chctlb::r0chcn_enum, tv_screen_status_.screen_mode_type);
            screen.format = getScrollScreenFormat(regs_.chctlb >> Chctlb::r0bmen_enum);

            // Cell
            screen.cell_size = cell_size * getDotSize(screen.character_color_number) / bits_in_a_byte;

            // Bitmap
            screen.bitmap_size = getBitmapSize(static_cast<Vdp2Regs::BitmapSize2Bits>(regs_.chctlb >> Chctlb::r0bmsz_enum));
            screen.bitmap_palette_number            = static_cast<u8>(regs_.bmpnb >> Bmpnb::r0bmpx_shft);
            screen.bitmap_special_priority          = static_cast<u8>(regs_.bmpnb >> Bmpnb::r0bmpr_shft);
            screen.bitmap_special_color_calculation = static_cast<u8>(regs_.bmpnb >> Bmpnb::r0bmcc_shft);
            screen.bitmap_start_address             = getBitmapStartAddress(screen.map_offset);

            // Color offset
            screen.color_offset = getColorOffset(Layer::rbg0);
            break;

        case rbg1:
            // Color RAM
            screen.color_ram_address_offset = getColorRamAddressOffset(static_cast<u8>(regs_.craofa >> Craofa::n0caos_shft));

            // Transparency
            screen.is_transparency_code_valid
                = ((regs_.bgon >> Bgon::n0tpon_enum) == Bgon::TransparentDisplayEnable::transparency_code_valid);

            // Priority
            screen.priority_number = static_cast<u8>(regs_.prina >> Prina::n0prin_shft);

            // Map
            screen.map_size   = map_size_rbg;
            screen.map_offset = static_cast<u8>(regs_.mpofr >> Mpofr::rbmp_shft);

            // Plane
            screen.plane_size            = regs_.plsz >> Vdp2Regs::Plsz::rbplsz_enum;
            screen.plane_a_start_address = calculatePlaneStartAddress(s, regs_.mpabrb >> Mpab::mpa_shft);
            screen.plane_b_start_address = calculatePlaneStartAddress(s, regs_.mpabrb >> Mpab::mpb_shft);
            screen.plane_c_start_address = calculatePlaneStartAddress(s, regs_.mpcdrb >> Mpcd::mpc_shft);
            screen.plane_d_start_address = calculatePlaneStartAddress(s, regs_.mpcdrb >> Mpcd::mpd_shft);
            screen.plane_e_start_address = calculatePlaneStartAddress(s, regs_.mpefrb >> Mpef::mpe_shft);
            screen.plane_f_start_address = calculatePlaneStartAddress(s, regs_.mpefrb >> Mpef::mpf_shft);
            screen.plane_g_start_address = calculatePlaneStartAddress(s, regs_.mpghrb >> Mpgh::mpg_shft);
            screen.plane_h_start_address = calculatePlaneStartAddress(s, regs_.mpghrb >> Mpgh::mph_shft);
            screen.plane_i_start_address = calculatePlaneStartAddress(s, regs_.mpijrb >> Mpij::mpi_shft);
            screen.plane_j_start_address = calculatePlaneStartAddress(s, regs_.mpijrb >> Mpij::mpj_shft);
            screen.plane_k_start_address = calculatePlaneStartAddress(s, regs_.mpklrb >> Mpkl::mpk_shft);
            screen.plane_l_start_address = calculatePlaneStartAddress(s, regs_.mpklrb >> Mpkl::mpl_shft);
            screen.plane_m_start_address = calculatePlaneStartAddress(s, regs_.mpmnrb >> Mpmn::mpm_shft);
            screen.plane_n_start_address = calculatePlaneStartAddress(s, regs_.mpmnrb >> Mpmn::mpn_shft);
            screen.plane_o_start_address = calculatePlaneStartAddress(s, regs_.mpoprb >> Mpop::mpo_shft);
            screen.plane_p_start_address = calculatePlaneStartAddress(s, regs_.mpoprb >> Mpop::mpp_shft);

            // Page
            screen.page_size = getPageSize(regs_.pncn0 >> Pcnxx::pnb_enum, regs_.chctla >> Chctla::n0chsz_enum);

            // Pattern name data
            screen.pattern_name_data_size           = regs_.pncn0 >> Pcnxx::pnb_enum;
            screen.character_number_supplement_mode = regs_.pncn0 >> Pcnxx::cnsm_enum;
            screen.special_priority                 = static_cast<u8>(regs_.pncn0 >> Pcnxx::spr_shft);
            screen.special_color_calculation        = static_cast<u8>(regs_.pncn0 >> Pcnxx::scc_shft);
            screen.supplementary_palette_number     = static_cast<u8>(regs_.pncn0 >> Pcnxx::splt_shft);
            screen.supplementary_character_number   = static_cast<u8>(regs_.pncn0 >> Pcnxx::scn_shft);

            // Character pattern
            screen.character_pattern_size = regs_.chctla >> Chctla::n0chsz_enum;
            screen.character_color_number
                = getCharacterColorNumber3Bits(regs_.chctla >> Chctla::n0chcn_enum, tv_screen_status_.screen_mode_type);
            screen.bitmap_size          = BitmapSize::not_set;
            screen.bitmap_start_address = 0;

            // Cell
            screen.cell_size = cell_size * getDotSize(screen.character_color_number) / bits_in_a_byte;

            // Color offset
            screen.color_offset = getColorOffset(Layer::rbg1);
            break;
        default: Log::warning(Logger::vdp2, tr("Scroll screen not set !"));
    }

    // Position calculation helpers
    constexpr auto cells_nb_64             = u16{64};
    constexpr auto cells_nb_128            = u16{128};
    screen.character_pattern_screen_offset = [](const Vdp2Regs::CharacterSize sz) {
        return (sz == Vdp2Regs::CharacterSize::one_by_one) ? ScreenOffset{1, 1} : ScreenOffset{2, 2};
    }(screen.character_pattern_size);
    screen.page_screen_offset  = {cells_nb_64, cells_nb_64};
    screen.plane_screen_offset = [&](const Plsz::PlaneSize sz) {
        switch (sz) {
            using enum Vdp2Regs::Plsz::PlaneSize;
            case size_1_by_1: return ScreenOffset{cells_nb_64, cells_nb_64};
            case size_2_by_1: return ScreenOffset{cells_nb_128, cells_nb_64};
            case size_2_by_2: return ScreenOffset{cells_nb_128, cells_nb_128};
            default: Log::warning(Logger::vdp2, tr("Plane screen offset wasn't properly calculated"));
        }
        return ScreenOffset{0, 0};
    }(screen.plane_size);

    // Calculating the total number of cells for current scroll screen. Doesn't take into account identicals
    // map start addresses, meaning the the real number of cells can be lower than the calculated one.

    screen.cells_number = [&screen]() {
        constexpr auto cells_per_page = 64 * 64; // 32 * 32 * 4 or 64 * 64 * 1
        auto           plane_size     = u16{};
        switch (screen.plane_size) {
            using enum Vdp2Regs::Plsz::PlaneSize;
            case size_1_by_1: plane_size = 1; break;
            case size_2_by_1: plane_size = 2; break;
            case size_2_by_2: plane_size = 4; break;
            default: Log::warning(Logger::vdp2, tr("Unknown plane size for {}!"), screenName(screen.scroll_screen));
        }

        return cells_per_page * plane_size * screen.map_size;
    }();

    // Calculating the total width of the scroll screen, will be used for scrolling calculation.
    screen.total_screen_scroll_width = [&screen]() -> u16 {
        constexpr auto pixels_per_cell      = 8;
        constexpr auto cells_per_page_width = 64;
        auto           plane_width          = u8{};
        switch (screen.plane_size) {
            using enum Vdp2Regs::Plsz::PlaneSize;
            case size_1_by_1: plane_width = 1; break;
            case size_2_by_1:
            case size_2_by_2: plane_width = 2; break;
            default: Log::warning(Logger::vdp2, tr("Unknown plane size for {}!"), screenName(screen.scroll_screen));
        }
        auto map_width = u8{};
        switch (screen.scroll_screen) {
            using enum ScrollScreen;
            case nbg0:
            case nbg1:
            case nbg2:
            case nbg3: {
                map_width = 2;
                break;
            }
            case rbg0:
            case rbg1: {
                map_width = 4;
                break;
            }
            default: Log::warning(Logger::vdp2, tr("Unknown scroll screen!"));
        }

        return pixels_per_cell * cells_per_page_width * plane_width * map_width;
    }();

    // Calculating the total height of the scroll screen, will be used for scrolling calculation.
    screen.total_screen_scroll_height = [&screen]() -> u16 {
        constexpr auto pixels_per_cell       = 8;
        constexpr auto cells_per_page_height = 64;
        auto           plane_height          = u8{};
        switch (screen.plane_size) {
            using enum Vdp2Regs::Plsz::PlaneSize;
            case size_1_by_1:
            case size_2_by_1: {
                plane_height = 1;
                break;
            }
            case size_2_by_2: {
                plane_height = 2;
                break;
            }
            default: Log::warning(Logger::vdp2, tr("Unknown plane height for {}!"), screenName(screen.scroll_screen));
        }
        auto map_height = u8{};
        switch (screen.scroll_screen) {
            using enum ScrollScreen;
            case nbg0:
            case nbg1:
            case nbg2:
            case nbg3: {
                map_height = 2;
                break;
            }
            case rbg0:
            case rbg1: {
                map_height = 4;
                break;
            }
            default: Log::warning(Logger::vdp2, tr("Unknown map height for {}!"), screenName(screen.scroll_screen));
        }

        return pixels_per_cell * cells_per_page_height * plane_height * map_height;
    }();

    screen.scroll_offset_horizontal = [&]() -> u16 {
        constexpr auto plane_width  = 512;
        auto           nb_of_planes = u8{};
        switch (screen.plane_size) {
            using enum Vdp2Regs::Plsz::PlaneSize;
            case size_1_by_1: nb_of_planes = 1; break;
            case size_2_by_1:
            case size_2_by_2: nb_of_planes = 2; break;
            default: nb_of_planes = 1;
        }

        return screen.screen_scroll_horizontal_integer % (plane_width * nb_of_planes);
    }();

    screen.scroll_offset_vertical = [&]() -> u16 {
        constexpr auto plane_height = 512;
        auto           nb_of_planes = u8{};
        switch (screen.plane_size) {
            using enum Vdp2Regs::Plsz::PlaneSize;
            case size_1_by_1:
            case size_2_by_1: nb_of_planes = 1; break;
            case size_2_by_2: nb_of_planes = 2; break;
            default: nb_of_planes = 1;
        }

        return screen.screen_scroll_vertical_integer % (plane_height * nb_of_planes);
    }();

    if (isCacheDirty(s)) { discardCache(s); }
    clearRenderData(s);
    vdp2_parts_[util::toUnderlying(s)].reserve(screen.cells_number);

    saved_bg_[util::toUnderlying(s)] = screen;
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto Vdp2::calculatePlaneStartAddress(const ScrollScreen s, const u32 map_addr) -> u32 {
    using Vrsize = Vdp2Regs::Vrsize;
    using Chctla = Vdp2Regs::Chctla;
    using Chctlb = Vdp2Regs::Chctlb;
    using Pcnxx  = Vdp2Regs::Pcnxx;
    using Plsz   = Vdp2Regs::Plsz;

    constexpr auto multiplier_800   = u32{0x800};
    constexpr auto multiplier_1000  = u32{0x1000};
    constexpr auto multiplier_2000  = u32{0x2000};
    constexpr auto multiplier_4000  = u32{0x4000};
    constexpr auto multiplier_8000  = u32{0x8000};
    constexpr auto multiplier_10000 = u32{0x10000};

    const auto& screen                 = getScreen(s);
    const auto  is_vram_size_4mb       = (regs_.vrsize >> Vrsize::vramsz_enum) == Vrsize::VramSize::size_4_mbits;
    auto        plane_size             = Plsz::PlaneSize{};
    auto        pattern_name_data_size = Pcnxx::PatternNameDataSize{};
    auto        character_size         = Vdp2Regs::CharacterSize{};
    auto        start_address          = u32{screen.map_offset << 6 | map_addr};

    switch (s) {
        using enum ScrollScreen;
        case nbg0:
            plane_size             = regs_.plsz >> Plsz::n0plsz_enum;
            pattern_name_data_size = regs_.pncn0 >> Pcnxx::pnb_enum;
            character_size         = regs_.chctla >> Chctla::n0chsz_enum;
            break;
        case nbg1:
            plane_size             = regs_.plsz >> Plsz::n1plsz_enum;
            pattern_name_data_size = regs_.pncn1 >> Pcnxx::pnb_enum;
            character_size         = regs_.chctla >> Chctla::n1chsz_enum;
            break;
        case nbg2:
            plane_size             = regs_.plsz >> Plsz::n2plsz_enum;
            pattern_name_data_size = regs_.pncn2 >> Pcnxx::pnb_enum;
            character_size         = regs_.chctlb >> Chctlb::n2chsz_enum;
            break;
        case nbg3:
            plane_size             = regs_.plsz >> Plsz::n3plsz_enum;
            pattern_name_data_size = regs_.pncn3 >> Pcnxx::pnb_enum;
            character_size         = regs_.chctlb >> Chctlb::n3chsz_enum;
            break;
        case rbg0:
            plane_size             = regs_.plsz >> Plsz::raplsz_enum;
            pattern_name_data_size = regs_.pncr >> Pcnxx::pnb_enum;
            character_size         = regs_.chctlb >> Chctlb::r0chsz_enum;
            break;
        case rbg1:
            plane_size             = regs_.plsz >> Plsz::rbplsz_enum;
            pattern_name_data_size = regs_.pncn0 >> Pcnxx::pnb_enum;
            character_size         = regs_.chctla >> Chctla::n0chsz_enum;
            break;
        default: break;
    }

    auto mask       = u32{};
    auto multiplier = u32{};
    switch (plane_size) {
        using enum Plsz::PlaneSize;
        case size_1_by_1:
            if (pattern_name_data_size == Pcnxx::PatternNameDataSize::one_word) {
                if (character_size == Vdp2Regs::CharacterSize::one_by_one) {
                    constexpr auto mask_4mb = u16{0x003f};
                    constexpr auto mask_8mb = u16{0x007f};
                    mask                    = is_vram_size_4mb ? mask_4mb : mask_8mb;
                    multiplier              = multiplier_2000;
                } else {
                    // CharacterSize::two_by_two
                    constexpr auto mask_4mb = u16{0x00ff};
                    constexpr auto mask_8mb = u16{0x01ff};
                    mask                    = is_vram_size_4mb ? mask_4mb : mask_8mb;
                    multiplier              = multiplier_800;
                }
            } else {
                // PatternNameDataSize::two_words
                if (character_size == Vdp2Regs::CharacterSize::one_by_one) {
                    constexpr auto mask_4mb = u16{0x001f};
                    constexpr auto mask_8mb = u16{0x003f};
                    mask                    = is_vram_size_4mb ? mask_4mb : mask_8mb;
                    multiplier              = multiplier_4000;
                } else {
                    // CharacterSize::two_by_two
                    constexpr auto mask_4mb = u16{0x007f};
                    constexpr auto mask_8mb = u16{0x00ff};
                    mask                    = is_vram_size_4mb ? mask_4mb : mask_8mb;
                    multiplier              = multiplier_1000;
                }
            }
            return vram_start_address + (start_address & mask) * multiplier;
            break;
        case size_2_by_1:
            if (pattern_name_data_size == Pcnxx::PatternNameDataSize::one_word) {
                if (character_size == Vdp2Regs::CharacterSize::one_by_one) {
                    constexpr auto mask_4mb = u16{0x003e};
                    constexpr auto mask_8mb = u16{0x007e};
                    mask                    = is_vram_size_4mb ? mask_4mb : mask_8mb;
                    multiplier              = multiplier_4000;
                } else {
                    // CharacterSize::two_by_two
                    constexpr auto mask_4mb = u16{0x00fe};
                    constexpr auto mask_8mb = u16{0x01fe};
                    mask                    = is_vram_size_4mb ? mask_4mb : mask_8mb;
                    multiplier              = multiplier_1000;
                }
            } else {
                // PatternNameDataSize::two_words
                if (character_size == Vdp2Regs::CharacterSize::one_by_one) {
                    constexpr auto mask_4mb = u16{0x001e};
                    constexpr auto mask_8mb = u16{0x003e};
                    mask                    = is_vram_size_4mb ? mask_4mb : mask_8mb;
                    multiplier              = multiplier_8000;
                } else {
                    // CharacterSize::two_by_two
                    constexpr auto mask_4mb = u16{0x007e};
                    constexpr auto mask_8mb = u16{0x00fe};
                    mask                    = is_vram_size_4mb ? mask_4mb : mask_8mb;
                    multiplier              = multiplier_2000;
                }
            }
            return vram_start_address + ((start_address & mask) >> 1) * multiplier;
            break;
        case size_2_by_2:
            if (pattern_name_data_size == Pcnxx::PatternNameDataSize::one_word) {
                if (character_size == Vdp2Regs::CharacterSize::one_by_one) {
                    constexpr auto mask_4mb = u16{0x003c};
                    constexpr auto mask_8mb = u16{0x007c};
                    mask                    = is_vram_size_4mb ? mask_4mb : mask_8mb;
                    multiplier              = multiplier_8000;
                } else {
                    // CharacterSize::two_by_two
                    constexpr auto mask_4mb = u16{0x00fc};
                    constexpr auto mask_8mb = u16{0x01fc};
                    mask                    = is_vram_size_4mb ? mask_4mb : mask_8mb;
                    multiplier              = multiplier_2000;
                }
            } else {
                // PatternNameDataSize::two_words
                if (character_size == Vdp2Regs::CharacterSize::one_by_one) {
                    constexpr auto mask_4mb = u16{0x001c};
                    constexpr auto mask_8mb = u16{0x003c};
                    mask                    = is_vram_size_4mb ? mask_4mb : mask_8mb;
                    multiplier              = multiplier_10000;
                } else {
                    // CharacterSize::two_by_two
                    constexpr auto mask_4mb = u16{0x007c};
                    constexpr auto mask_8mb = u16{0x00fc};
                    mask                    = is_vram_size_4mb ? mask_4mb : mask_8mb;
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
    // if (isCacheDirty(s)) { discardCache(s); }
    // vdp2_parts_[util::toUnderlying(s)].clear();

    if (screen.format == ScrollScreenFormat::cell) {
        std::vector<CellData>().swap(cell_data_to_process_);
        cell_data_to_process_.reserve(screen.cells_number);

        // Using a set to prevent calculating same address data multiple times
        // auto unique_addresses = std::unordered_set<u32>{};
        auto start_addresses = std::vector<std::pair<u32, ScreenOffset>>{};

        const auto addPlane
            = [&](const u32 address, const ScreenOffset& offset) { start_addresses.emplace_back(address, offset); };

        const auto offset_x         = screen.plane_screen_offset.x;
        const auto offset_y         = screen.plane_screen_offset.y;
        const auto rotation_screens = std::array{ScrollScreen::rbg0, ScrollScreen::rbg1};
        if (std::ranges::none_of(rotation_screens, [&s](const ScrollScreen rss) { return rss == s; })) {
            // For NBG
            addPlane(screen.plane_a_start_address, ScreenOffset{0, 0});
            addPlane(screen.plane_b_start_address, ScreenOffset{offset_x, 0});
            addPlane(screen.plane_c_start_address, ScreenOffset{0, offset_y});
            addPlane(screen.plane_d_start_address, ScreenOffset{offset_x, offset_y});
        } else {
            // For RBG
            addPlane(screen.plane_a_start_address, ScreenOffset{0, 0});
            addPlane(screen.plane_b_start_address, ScreenOffset{offset_x, 0});
            addPlane(screen.plane_c_start_address, ScreenOffset{static_cast<u16>(offset_x * 2), 0});
            addPlane(screen.plane_d_start_address, ScreenOffset{static_cast<u16>(offset_x * 3), 0});
            addPlane(screen.plane_e_start_address, ScreenOffset{0, offset_y});
            addPlane(screen.plane_f_start_address, ScreenOffset{offset_x, offset_y});
            addPlane(screen.plane_g_start_address, ScreenOffset{static_cast<u16>(offset_x * 2), offset_y});
            addPlane(screen.plane_h_start_address, ScreenOffset{static_cast<u16>(offset_x * 3), offset_y});
            addPlane(screen.plane_i_start_address, ScreenOffset{0, static_cast<u16>(offset_y * 2)});
            addPlane(screen.plane_j_start_address, ScreenOffset{offset_x, static_cast<u16>(offset_y * 2)});
            addPlane(screen.plane_k_start_address, ScreenOffset{static_cast<u16>(offset_x * 2), static_cast<u16>(offset_y * 2)});
            addPlane(screen.plane_l_start_address, ScreenOffset{static_cast<u16>(offset_x * 3), static_cast<u16>(offset_y * 2)});
            addPlane(screen.plane_m_start_address, ScreenOffset{0, static_cast<u16>(offset_y * 3)});
            addPlane(screen.plane_n_start_address, ScreenOffset{offset_x, static_cast<u16>(offset_y * 3)});
            addPlane(screen.plane_o_start_address, ScreenOffset{static_cast<u16>(offset_x * 2), static_cast<u16>(offset_y * 3)});
            addPlane(screen.plane_p_start_address, ScreenOffset{static_cast<u16>(offset_x * 3), static_cast<u16>(offset_y * 3)});
        }

        // Reset planes data. Currently done at every frame, but could be saved between frames in order to speed up display ...
        // needs an update detection (size + color mode + nbg + etc. )
        // for (auto& [address, parts] : address_to_plane_data_) {
        //    std::vector<Vdp2Part>().swap(parts);
        //}
        auto address_to_plane_data = AddressToPlaneData{};

        std::vector<PlaneDetail>().swap(plane_details_[util::toUnderlying(screen.scroll_screen)]);

        auto size = Size{};
        switch (screen.plane_size) {
            using PlaneSize = Vdp2Regs::Plsz::PlaneSize;
            case PlaneSize::size_1_by_1: {
                size.w = 512;
                size.h = 512;
                break;
            }
            case PlaneSize::size_2_by_1: {
                size.w = 1024;
                size.h = 512;
                break;
            }
            case PlaneSize::size_2_by_2: {
                size.w = 1024;
                size.h = 1024;
                break;
            }
            default: {
                Log::warning(Logger::vdp2, tr("Unknown plane size value !"));
            }
        }

        // Each plane similar data is only read once, even if it's used multiple times.
        for (const auto& [addr, offset] : start_addresses) {
            plane_details_[util::toUnderlying(screen.scroll_screen)].emplace_back(addr, offset);

            if (address_to_plane_data.contains(addr)) { continue; }

            current_plane_address_ = addr;
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
        using enum BitmapSize;
        case size_1024_by_256: {
            texture_width  = width_1024;
            texture_height = height_256;
            break;
        }
        case size_1024_by_512: {
            texture_width  = width_1024;
            texture_height = height_512;
            break;
        }
        case size_512_by_256: {
            texture_width  = width_512;
            texture_height = height_256;
            break;
        }
        case size_512_by_512: {
            texture_width  = width_512;
            texture_height = height_512;
            break;
        }
        default: {
            Log::warning(Logger::vdp2, tr("Bitmap size not set !"));
        }
    }
    // const auto      texture_size = texture_width * texture_height * 4;
    std::vector<u8> texture_data;
    // texture_data.reserve(texture_size);
    const auto key = Texture::calculateKey(VdpType::vdp2_bitmap,
                                           screen.bitmap_start_address,
                                           toUnderlying(screen.character_color_number),
                                           screen.bitmap_palette_number);

    if (Texture::isTextureLoadingNeeded(key)) {
        if (ram_status_.color_ram_mode == Vdp2Regs::Ramctl::ColorRamMode::mode_2_rgb_8_bits_1024_colors) {
            // 32 bits access to color RAM
            switch (screen.character_color_number) {
                using enum ColorCount;
                case palette_16: {
                    texture_data.reserve(static_cast<size_t>(texture_width * texture_height * 2));
                    read16ColorsBitmapData<u32>(texture_data, screen);

                    break;
                }
                case palette_256: {
                    texture_data.reserve(static_cast<size_t>(texture_width * texture_height * 4));
                    read256ColorsBitmapData<u32>(texture_data, screen);
                    break;
                }
                case palette_2048: {
                    texture_data.reserve(static_cast<size_t>(texture_width * texture_height * 8));
                    read2048ColorsBitmapData<u32>(texture_data, screen);
                    break;
                }
                case rgb_32k: {
                    texture_data.reserve(static_cast<size_t>(texture_width * texture_height * 8));
                    read32KColorsBitmapData(texture_data, screen);
                    break;
                }
                case rgb_16m: {
                    texture_data.reserve(static_cast<size_t>(texture_width * texture_height * 16));
                    read16MColorsBitmapData(texture_data, screen);

                    break;
                }
                default: {
                    Log::warning(Logger::vdp2, tr("Character color number invalid !"));
                }
            }
        } else {
            // 16 bits access to color RAM
            switch (screen.character_color_number) {
                using enum ColorCount;
                case palette_16: {
                    texture_data.reserve(static_cast<size_t>(texture_width * texture_height * 2));
                    read16ColorsBitmapData<u16>(texture_data, screen);
                    break;
                }
                case palette_256: {
                    texture_data.reserve(static_cast<size_t>(texture_width * texture_height * 4));
                    read256ColorsBitmapData<u16>(texture_data, screen);
                    break;
                }
                case palette_2048: {
                    texture_data.reserve(static_cast<size_t>(texture_width * texture_height * 8));
                    read2048ColorsBitmapData<u16>(texture_data, screen);
                    break;
                }
                case rgb_32k: {
                    texture_data.reserve(static_cast<size_t>(texture_width * texture_height * 8));
                    read32KColorsBitmapData(texture_data, screen);
                    break;
                }
                case rgb_16m: {
                    texture_data.reserve(static_cast<size_t>(texture_width * texture_height * 16));
                    read16MColorsBitmapData(texture_data, screen);
                    break;
                }
                default: {
                    Log::warning(Logger::vdp2, tr("Character color number invalid !"));
                }
            }
        }
        Texture::storeTexture(Texture(VdpType::vdp2_bitmap,
                                      scrollScreenToLayer(screen.scroll_screen),
                                      screen.bitmap_start_address,
                                      toUnderlying(screen.character_color_number),
                                      screen.bitmap_palette_number,
                                      texture_data,
                                      texture_width,
                                      texture_height));
        modules_.opengl()->addOrUpdateTexture(key, scrollScreenToLayer(screen.scroll_screen));
    }
    saveBitmap(screen, texture_width, texture_height, key);
}

void Vdp2::saveBitmap(const ScrollScreenStatus& screen, const u16 width, const u16 height, const size_t key) {
    // auto pos = ScreenPos{0, 0};

    // auto p = Vdp2Part(key, width, height, screen.priority_number, screen.linked_layer);
    //  auto p = std::make_unique<Vdp2Part>(key, width, height);
    //  p.priority(screen.priority_number);
    //  vdp2_parts_[util::toUnderlying(screen.scroll_screen)].push_back(
    //     std::make_unique<Vdp2Part>(key, width, height, screen.priority_number));
    vdp2_parts_[util::toUnderlying(screen.scroll_screen)]
        .emplace_back(key, width, height, screen.priority_number, screen.color_offset, VdpType::vdp2_bitmap);
}

void Vdp2::readPlaneData(const ScrollScreenStatus& screen, const u32 plane_address, const ScreenOffset& plane_offset) {
    auto page_start_address = plane_address;
    switch (screen.plane_size) {
        using enum Vdp2Regs::Plsz::PlaneSize;
        case size_1_by_1: readPageData(screen, page_start_address, plane_offset); break;
        case size_2_by_1: {
            readPageData(screen, page_start_address, plane_offset);
            page_start_address += screen.page_size;
            const auto page_offset_x = static_cast<u16>(plane_offset.x + screen.page_screen_offset.x);
            readPageData(screen, page_start_address, ScreenOffset{page_offset_x, plane_offset.y});
            break;
        }
        case size_2_by_2: {
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

    using Vrsize = Vdp2Regs::Vrsize;
    using Pcnxx  = Vdp2Regs::Pcnxx;

    static auto current_pnd_config = PatternNameDataEnum{};

    if (screen.pattern_name_data_size == Pcnxx::PatternNameDataSize::two_words) {
        current_pnd_config = PatternNameDataEnum::two_words;
    } else {
        if (screen.character_pattern_size == Vdp2Regs::CharacterSize::one_by_one) {
            if (screen.character_color_number == ColorCount::palette_16) {
                if (screen.character_number_supplement_mode == Pcnxx::CharacterNumberSupplementMode::character_number_10_bits) {
                    current_pnd_config = PatternNameDataEnum::one_word_1_cell_16_colors_10_bits;
                } else { // CharacterNumberSupplementMode::character_number_12_bits
                    current_pnd_config = PatternNameDataEnum::one_word_1_cell_16_colors_12_bits;
                }
            } else { // Over 16 colors
                if (screen.character_number_supplement_mode == Pcnxx::CharacterNumberSupplementMode::character_number_10_bits) {
                    current_pnd_config = PatternNameDataEnum::one_word_1_cell_over_16_colors_10_bits;
                } else { // CharacterNumberSupplementMode::character_number_12_bits
                    current_pnd_config = PatternNameDataEnum::one_word_1_cell_over_16_colors_12_bits;
                }
            }
        } else { // CharacterSize::two_by_two
            if (screen.character_color_number == ColorCount::palette_16) {
                if (screen.character_number_supplement_mode == Pcnxx::CharacterNumberSupplementMode::character_number_10_bits) {
                    current_pnd_config = PatternNameDataEnum::one_word_4_cells_16_colors_10_bits;
                } else { // CharacterNumberSupplementMode::character_number_12_bits
                    current_pnd_config = PatternNameDataEnum::one_word_4_cells_16_colors_12_bits;
                }
            } else { // Over 16 colors
                if (screen.character_number_supplement_mode == Pcnxx::CharacterNumberSupplementMode::character_number_10_bits) {
                    current_pnd_config = PatternNameDataEnum::one_word_4_cells_over_16_colors_10_bits;
                } else { // CharacterNumberSupplementMode::character_number_12_bits
                    current_pnd_config = PatternNameDataEnum::one_word_4_cells_over_16_colors_12_bits;
                }
            }
        }
    }

    // Assigning the current configuration function to a function pointer
    // Not using std::function here because of the performance cost.
    using PatterNameDataFunc                                       = PatternNameData (*)(const u32, const ScrollScreenStatus&);
    auto                                       readPatternNameData = PatterNameDataFunc();
    switch (current_pnd_config) {
        using enum PatternNameDataEnum;
        case two_words: readPatternNameData = std::bit_cast<PatterNameDataFunc>(&getPatternNameData2Words); break;
        case one_word_1_cell_16_colors_10_bits:
            readPatternNameData = std::bit_cast<PatterNameDataFunc>(&getPatternNameData1Word1Cell16Colors10Bits);
            break;
        case one_word_1_cell_16_colors_12_bits:
            readPatternNameData = std::bit_cast<PatterNameDataFunc>(&getPatternNameData1Word1Cell16Colors12Bits);
            break;
        case one_word_1_cell_over_16_colors_10_bits:
            readPatternNameData = std::bit_cast<PatterNameDataFunc>(&getPatternNameData1Word1CellOver16Colors10Bits);
            break;
        case one_word_1_cell_over_16_colors_12_bits:
            readPatternNameData = std::bit_cast<PatterNameDataFunc>(&getPatternNameData1Word1CellOver16Colors12Bits);
            break;
        case one_word_4_cells_16_colors_10_bits:
            readPatternNameData = std::bit_cast<PatterNameDataFunc>(&getPatternNameData1Word4Cells16Colors10Bits);
            break;
        case one_word_4_cells_16_colors_12_bits:
            readPatternNameData = std::bit_cast<PatterNameDataFunc>(&getPatternNameData1Word4Cells16Colors12Bits);
            break;
        case one_word_4_cells_over_16_colors_10_bits:
            readPatternNameData = std::bit_cast<PatterNameDataFunc>(&getPatternNameData1Word4CellsOver16Colors10Bits);
            break;
        case one_word_4_cells_over_16_colors_12_bits:
            readPatternNameData = std::bit_cast<PatterNameDataFunc>(&getPatternNameData1Word4CellsOver16Colors12Bits);
            break;
    }

    const auto cp_number   = (screen.character_pattern_size == Vdp2Regs::CharacterSize::one_by_one) ? u32{64 * 64} : u32{32 * 32};
    const auto cp_width    = (screen.character_pattern_size == Vdp2Regs::CharacterSize::one_by_one) ? u32{1} : u32{2};
    const auto cp_height   = cp_width;
    const auto pnd_size    = (screen.pattern_name_data_size == Pcnxx::PatternNameDataSize::one_word) ? 2 : 4;
    auto       pnd_address = page_address;
    auto       cp_offset   = page_offset;

    // Choosing the right precalculated modulo check function
    const auto& modulo_values = (screen.character_pattern_size == Vdp2Regs::CharacterSize::one_by_one)
                                    ? pre_calculated_modulo_64_
                                    : pre_calculated_modulo_32_;

    const auto isEndOfRowReached = [](const std::vector<u32>& modulos, const u32 val) {
        // Value 0 isn't added to the vector.
        return std::ranges::any_of(modulos, [&](const u32 m) { return m == val; });
    };

    const auto character_number_mask
        = (ram_status_.vram_size == Vrsize::VramSize::size_4_mbits) ? vdp2_vram_4mb_mask : vdp2_vram_8mb_mask;

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
    // if (pnd.character_number != 0x3000 && pnd.character_number != 0x3345 && pnd.character_number != 0x3741) __debugbreak();
    const auto character_number_address = pnd.character_number * character_pattern_boundary;
    if (screen.character_pattern_size == Vdp2Regs::CharacterSize::one_by_one) {
        readCellDispatch(screen, pnd, character_number_address, cp_offset);
    } else { // CharacterSize::two_by_two

        // const auto pnd_offset           = (screen.pattern_name_data_size == PatternNameDataSize::one_word) ? 2 : 4;
        auto current_cell_address = character_number_address;
        auto cells_address        = std::vector<u32>{};

        const auto cell_number = u32{4};
        for (u32 i = 0; i < cell_number; ++i) {
            cells_address.emplace_back(current_cell_address);
            current_cell_address += screen.cell_size;
        }

        const auto readCells = [&](const std::array<u8, cell_number>& cells_pos) {
            auto cell_offset = cp_offset;
            readCellDispatch(screen, pnd, cells_address[cells_pos[0]], cell_offset);
            cell_offset.x++;
            readCellDispatch(screen, pnd, cells_address[cells_pos[1]], cell_offset);
            cell_offset.x = cp_offset.x;
            cell_offset.y++;
            readCellDispatch(screen, pnd, cells_address[cells_pos[2]], cell_offset);
            cell_offset.x++;
            readCellDispatch(screen, pnd, cells_address[cells_pos[3]], cell_offset);
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

void Vdp2::readCellDispatch(const ScrollScreenStatus& screen,
                            const PatternNameData&    pnd,
                            const u32                 cell_address,
                            const ScreenOffset&       cell_offset) {
    const auto key = Texture::calculateKey(VdpType::vdp2_cell,
                                           cell_address,
                                           toUnderlying(screen.character_color_number),
                                           pnd.palette_number);

    if (Texture::isTextureLoadingNeeded(key)) {
        // if (use_concurrent_read_for_cells) {
        // ThreadPool::pool_.push_task(&Vdp2::readCellMT,
        //                             this,
        //                             std::cref(screen),
        //                             pnd.palette_number,
        //                             cell_address,
        //                             key,
        //                             std::span<const u8>{modules_.memory()->vdp2_vram_},
        //                             std::span<const u8>{modules_.memory()->vdp2_cram_});
        //} else {
        readCell(screen, pnd, cell_address, key);
        //}
    }
    saveCell(screen, pnd, cell_address, cell_offset, key);
}

void Vdp2::readCell(const ScrollScreenStatus& screen, const PatternNameData& pnd, const u32 cell_address, const size_t key) {
    constexpr auto  texture_width  = u16{8};
    constexpr auto  texture_height = u16{8};
    constexpr auto  texture_size   = texture_width * texture_height * 4;
    std::vector<u8> texture_data;
    texture_data.reserve(texture_size);

    auto is_access_32bits = (ram_status_.color_ram_mode == Vdp2Regs::Ramctl::ColorRamMode::mode_2_rgb_8_bits_1024_colors);

    switch (screen.character_color_number) {
        using enum ColorCount;
        case palette_16: {
            if (is_access_32bits) {
                read16ColorsCellData<u32>(texture_data, screen, pnd.palette_number, cell_address);
            } else {
                read16ColorsCellData<u16>(texture_data, screen, pnd.palette_number, cell_address);
            }
            break;
        }
        case palette_256: {
            if (is_access_32bits) {
                read256ColorsCellData<u32>(texture_data, screen, pnd.palette_number, cell_address);
            } else {
                read256ColorsCellData<u16>(texture_data, screen, pnd.palette_number, cell_address);
            }
            break;
        }
        case palette_2048: {
            if (is_access_32bits) {
                read2048ColorsCellData<u32>(texture_data, screen, cell_address);
            } else {
                read2048ColorsCellData<u16>(texture_data, screen, cell_address);
            }
            break;
        }
        case rgb_32k: {
            read32KColorsCellData(texture_data, screen, cell_address);
            break;
        }
        case rgb_16m: {
            read16MColorsCellData(texture_data, screen, cell_address);
            break;
        }
        default: {
            Log::warning(Logger::vdp2, tr("Character color number invalid !"));
        }
    }

    Texture::storeTexture(Texture(VdpType::vdp2_cell,
                                  scrollScreenToLayer(screen.scroll_screen),
                                  cell_address,
                                  static_cast<u8>(toUnderlying(screen.character_color_number)),
                                  pnd.palette_number,
                                  texture_data,
                                  texture_width,
                                  texture_height));
    modules_.opengl()->addOrUpdateTexture(key, scrollScreenToLayer(screen.scroll_screen));
}

void Vdp2::readCellMT(const ScrollScreenStatus& screen,
                      const u16                 palette_number,
                      const u32                 cell_address,
                      const size_t              key,
                      const std::span<const u8> vram,
                      const std::span<const u8> cram) {
    constexpr auto  texture_width  = u16{8};
    constexpr auto  texture_height = u16{8};
    constexpr auto  texture_size   = texture_width * texture_height * 4;
    std::vector<u8> texture_data;
    texture_data.reserve(texture_size);

    auto is_access_32bits = (ram_status_.color_ram_mode == Vdp2Regs::Ramctl::ColorRamMode::mode_2_rgb_8_bits_1024_colors);

    switch (screen.character_color_number) {
        using enum ColorCount;
        case palette_16: {
            if (is_access_32bits) {
                read16ColorsCellData<u32>(texture_data, screen, palette_number, cell_address);
            } else {
                read16ColorsCellData<u16>(texture_data, screen, palette_number, cell_address);
            }
            break;
        }
        case palette_256: {
            if (is_access_32bits) {
                read256ColorsCellData<u32>(texture_data, screen, palette_number, cell_address);
            } else {
                if (use_concurrent_read_for_cells) {
                    read256ColorsCellDataMT<u16>(texture_data, screen, palette_number, cell_address, vram, cram);
                } else {
                    read256ColorsCellData<u16>(texture_data, screen, palette_number, cell_address);
                }
            }
            break;
        }
        case palette_2048: {
            if (is_access_32bits) {
                read2048ColorsCellData<u32>(texture_data, screen, cell_address);
            } else {
                read2048ColorsCellData<u16>(texture_data, screen, cell_address);
            }
            break;
        }
        case rgb_32k: {
            read32KColorsCellData(texture_data, screen, cell_address);
            break;
        }
        case rgb_16m: {
            read16MColorsCellData(texture_data, screen, cell_address);
            break;
        }
        default: {
            Log::warning(Logger::vdp2, tr("Character color number invalid !"));
        }
    }
    Texture::storeTexture(Texture(VdpType::vdp2_cell,
                                  scrollScreenToLayer(screen.scroll_screen),
                                  cell_address,
                                  static_cast<u8>(toUnderlying(screen.character_color_number)),
                                  palette_number,
                                  texture_data,
                                  texture_width,
                                  texture_height));
    modules_.opengl()->addOrUpdateTexture(key, scrollScreenToLayer(screen.scroll_screen));
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void Vdp2::saveCell(const ScrollScreenStatus& screen,
                    const PatternNameData&    pnd,
                    const u32                 cell_address,
                    const ScreenOffset&       cell_offset,
                    const size_t              key) {
    constexpr auto texture_width  = u16{8};
    constexpr auto texture_height = u16{8};

    auto pos = ScreenPos{static_cast<u16>(cell_offset.x * texture_width), static_cast<u16>(cell_offset.y * texture_height)};

    //: TODO: scroll offset must be handled during display, put in comments here for now
    pos.x -= screen.scroll_offset_horizontal;
    pos.y -= screen.scroll_offset_vertical;

    vdp2_parts_[util::toUnderlying(screen.scroll_screen)]
        .emplace_back(pnd, pos, key, screen.priority_number, screen.color_offset, current_plane_address_);
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

//--------------------------------------------------------------------------------------------------------------
// CACHE methods
//--------------------------------------------------------------------------------------------------------------

void Vdp2::resetCacheState() const {
    modules_.memory()->was_vdp2_cram_accessed_ = false;
    for (auto& accessed : modules_.memory()->was_vdp2_page_accessed_) {
        accessed = false;
    }
    for (auto& accessed : modules_.memory()->was_vdp2_bitmap_accessed_) {
        accessed = false;
    }
}

auto Vdp2::isCacheDirty(const ScrollScreen screen) -> bool {
    if (modules_.memory()->was_vdp2_cram_accessed_) { return true; }

    const auto& bg       = bg_[toUnderlying(screen)];
    const auto& saved_bg = saved_bg_[toUnderlying(screen)];

    if (bg.is_display_enabled != saved_bg.is_display_enabled) { return true; }
    if (bg.is_transparency_code_valid != saved_bg.is_transparency_code_valid) { return true; }
    if (bg.character_color_number != saved_bg.character_color_number) { return true; }
    if (bg.bitmap_size != saved_bg.bitmap_size) { return true; }
    if (bg.format != saved_bg.format) { return true; }
    if (bg.character_pattern_size != saved_bg.character_pattern_size) { return true; }
    if (bg.pattern_name_data_size != saved_bg.pattern_name_data_size) { return true; }
    if (bg.plane_size != saved_bg.plane_size) { return true; }
    if (bg.map_offset != saved_bg.map_offset) { return true; }
    if (bg.bitmap_palette_number != saved_bg.bitmap_palette_number) { return true; }

    if (bg.format == ScrollScreenFormat::cell) {
        //  Checking the pages
        const auto page_address_start
            = (getScreen(screen).plane_a_start_address & core::vdp2_vram_memory_mask) >> core::vdp2_page_disp;
        const auto page_address_end
            = ((getScreen(screen).plane_a_start_address + getScreen(screen).page_size) & core::vdp2_vram_memory_mask)
              >> core::vdp2_page_disp;
        for (u32 i = page_address_start; i < page_address_end; ++i) {
            if (modules_.memory()->was_vdp2_page_accessed_[i]) { return true; }
        }
    } else {
        const auto bitmap_index
            = (getScreen(screen).bitmap_start_address & core::vdp2_vram_memory_mask) >> core::vdp2_bitmap_disp;

        if (modules_.memory()->was_vdp2_bitmap_accessed_[bitmap_index]) { return true; }
    }
    return false;
}

void Vdp2::discardCache([[maybe_unused]] const ScrollScreen screen) const {
    // 1) Textures used by the vdp2 parts of the screen are discarded
    Texture::discardCache(VdpType::vdp2_cell);
    Texture::discardCache(VdpType::vdp2_bitmap);

    // 2) Vdp2 parts are deleted
    // clearRenderData(screen);
}

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

auto getPatternNameData2Words(const u32 data, [[maybe_unused]] const ScrollScreenStatus& screen) -> PatternNameData {
    auto pattern_name_data                      = PatternNameData{};
    auto reg                                    = PatternNameData2WordsType{data};
    pattern_name_data.character_number          = static_cast<u16>(reg >> PatternNameData2Words::character_number_shft);
    pattern_name_data.palette_number            = static_cast<u16>(reg >> PatternNameData2Words::palette_number_shft);
    pattern_name_data.special_color_calculation = static_cast<u8>(reg >> PatternNameData2Words::special_color_calculation_shft);
    pattern_name_data.special_priority          = static_cast<u8>(reg >> PatternNameData2Words::special_priority_shft);
    pattern_name_data.is_horizontally_flipped   = reg >> PatternNameData2Words::horizontal_flip_shft;
    pattern_name_data.is_vertically_flipped     = reg >> PatternNameData2Words::vertical_flip_shft;
    return pattern_name_data;
};

auto getPatternNameData1Word1Cell16Colors10Bits(const u32 data, const ScrollScreenStatus& screen) -> PatternNameData {
    using std::byte;
    auto pattern_name_data = PatternNameData{};
    auto reg               = PatternNameData1Word1Cell16Colors10BitsType{data};

    const auto scn                     = static_cast<byte>(screen.supplementary_character_number);
    pattern_name_data.character_number = static_cast<u16>(std::to_integer<u16>(scn) << 10);
    pattern_name_data.character_number |= reg >> PatternNameData1Word1Cell16Colors10Bits::character_number_shft;

    pattern_name_data.palette_number = static_cast<u16>(static_cast<byte>(screen.supplementary_palette_number) << 4);
    pattern_name_data.palette_number |= reg >> PatternNameData1Word1Cell16Colors10Bits::palette_number_shft;

    pattern_name_data.special_color_calculation = screen.special_color_calculation;
    pattern_name_data.special_priority          = screen.special_priority;
    pattern_name_data.is_horizontally_flipped   = reg >> PatternNameData1Word1Cell16Colors10Bits::horizontal_flip_shft;
    pattern_name_data.is_vertically_flipped     = reg >> PatternNameData1Word1Cell16Colors10Bits::vertical_flip_shft;

    return pattern_name_data;
};

auto getPatternNameData1Word1Cell16Colors12Bits(const u32 data, const ScrollScreenStatus& screen) -> PatternNameData {
    using std::byte;

    auto pattern_name_data = PatternNameData{};
    auto reg               = PatternNameData1Word1Cell16Colors12BitsType{data};

    const auto scn                     = static_cast<byte>(screen.supplementary_character_number) & byte{0x1C};
    pattern_name_data.character_number = static_cast<u16>(std::to_integer<u16>(scn) << 10);
    pattern_name_data.character_number |= reg >> PatternNameData1Word1Cell16Colors12Bits::character_number_shft;

    pattern_name_data.palette_number = static_cast<u16>(static_cast<byte>(screen.supplementary_palette_number) << 4);
    pattern_name_data.palette_number |= reg >> PatternNameData1Word1Cell16Colors12Bits::palette_number_shft;

    pattern_name_data.special_color_calculation = screen.special_color_calculation;
    pattern_name_data.special_priority          = screen.special_priority;
    pattern_name_data.is_horizontally_flipped   = false;
    pattern_name_data.is_vertically_flipped     = false;

    return pattern_name_data;
};

auto getPatternNameData1Word1CellOver16Colors10Bits(const u32 data, const ScrollScreenStatus& screen) -> PatternNameData {
    using std::byte;

    auto pattern_name_data = PatternNameData{};
    auto reg               = PatternNameData1Word1CellOver16Colors10BitsType{data};

    pattern_name_data.character_number
        = static_cast<u16>(reg >> PatternNameData1Word1CellOver16Colors10Bits::palette_number_shft << 10);
    pattern_name_data.character_number |= reg >> PatternNameData1Word1CellOver16Colors10Bits::character_number_shft;

    const auto spn                   = static_cast<u16>(screen.supplementary_palette_number) << 8;
    pattern_name_data.palette_number = static_cast<u16>(spn);

    pattern_name_data.special_color_calculation = screen.special_color_calculation;
    pattern_name_data.special_priority          = screen.special_priority;
    pattern_name_data.is_horizontally_flipped   = reg >> PatternNameData1Word1CellOver16Colors10Bits::horizontal_flip_shft;
    pattern_name_data.is_vertically_flipped     = reg >> PatternNameData1Word1CellOver16Colors10Bits::vertical_flip_shft;

    return pattern_name_data;
};

auto getPatternNameData1Word1CellOver16Colors12Bits(const u32 data, const ScrollScreenStatus& screen) -> PatternNameData {
    using std::byte;

    auto pattern_name_data = PatternNameData{};
    auto reg               = PatternNameData1Word1CellOver16Colors12BitsType{data};

    const auto scn                     = static_cast<byte>(screen.supplementary_character_number) & byte{0x1C};
    pattern_name_data.character_number = static_cast<u16>(std::to_integer<u16>(scn) << 10);
    pattern_name_data.character_number |= reg >> PatternNameData1Word1CellOver16Colors12Bits::character_number_shft;

    pattern_name_data.palette_number
        = static_cast<u16>((reg >> PatternNameData1Word1CellOver16Colors12Bits::palette_number_shft) << 8);

    pattern_name_data.special_color_calculation = screen.special_color_calculation;
    pattern_name_data.special_priority          = screen.special_priority;
    pattern_name_data.is_horizontally_flipped   = false;
    pattern_name_data.is_vertically_flipped     = false;

    return pattern_name_data;
};

auto getPatternNameData1Word4Cells16Colors10Bits(const u32 data, const ScrollScreenStatus& screen) -> PatternNameData {
    using std::byte;

    auto pattern_name_data = PatternNameData{};
    auto reg               = PatternNameData1Word4Cells16Colors10BitsType{data};

    const auto scn                     = static_cast<byte>(screen.supplementary_character_number) & byte{0x1C};
    pattern_name_data.character_number = static_cast<u16>(std::to_integer<u16>(scn) << 10);
    pattern_name_data.character_number |= (reg >> PatternNameData1Word4Cells16Colors10Bits::character_number_shft << 2);
    pattern_name_data.character_number |= static_cast<u16>(static_cast<byte>(screen.supplementary_character_number) & byte{0x3});

    const auto spn                   = byte{screen.supplementary_palette_number};
    pattern_name_data.palette_number = static_cast<u16>(std::to_integer<u16>(spn) << 4);
    pattern_name_data.palette_number |= reg >> PatternNameData1Word4Cells16Colors10Bits::palette_number_shft;

    pattern_name_data.special_color_calculation = screen.special_color_calculation;
    pattern_name_data.special_priority          = screen.special_priority;
    pattern_name_data.is_horizontally_flipped   = reg >> PatternNameData1Word4Cells16Colors10Bits::horizontal_flip_shft;
    pattern_name_data.is_vertically_flipped     = reg >> PatternNameData1Word4Cells16Colors10Bits::vertical_flip_shft;

    return pattern_name_data;
};

auto getPatternNameData1Word4Cells16Colors12Bits(const u32 data, const ScrollScreenStatus& screen) -> PatternNameData {
    using std::byte;

    auto pattern_name_data = PatternNameData{};
    auto reg               = PatternNameData1Word4Cells16Colors12BitsType{data};

    const auto scn                     = static_cast<byte>(screen.supplementary_character_number) & byte{0x10};
    pattern_name_data.character_number = static_cast<u16>(std::to_integer<u16>(scn) << 10);
    pattern_name_data.character_number |= (reg >> PatternNameData1Word4Cells16Colors12Bits::character_number_shft << 2);
    const auto scn2 = static_cast<byte>(screen.supplementary_character_number) & byte{0x3};
    pattern_name_data.character_number |= std::to_integer<u16>(scn2);

    const auto spn                   = byte{screen.supplementary_palette_number};
    pattern_name_data.palette_number = static_cast<u16>(std::to_integer<u16>(spn) << 4);
    pattern_name_data.palette_number |= reg >> PatternNameData1Word4Cells16Colors12Bits::palette_number_shft;

    pattern_name_data.special_color_calculation = screen.special_color_calculation;
    pattern_name_data.special_priority          = screen.special_priority;
    pattern_name_data.is_horizontally_flipped   = false;
    pattern_name_data.is_vertically_flipped     = false;

    return pattern_name_data;
};

auto getPatternNameData1Word4CellsOver16Colors10Bits(const u32 data, const ScrollScreenStatus& screen) -> PatternNameData {
    using std::byte;

    auto pattern_name_data = PatternNameData{};
    auto reg               = PatternNameData1Word4CellsOver16Colors10BitsType{data};

    const auto scn                     = static_cast<byte>(screen.supplementary_character_number) & byte{0x1C};
    pattern_name_data.character_number = static_cast<u16>(std::to_integer<u16>(scn) << 10);
    constexpr auto cn_disp_2           = u8{2};
    pattern_name_data.character_number
        |= (reg >> PatternNameData1Word4CellsOver16Colors10Bits::character_number_shft << cn_disp_2);
    pattern_name_data.character_number |= static_cast<u16>(static_cast<byte>(screen.supplementary_character_number) & byte{0x3});

    pattern_name_data.palette_number
        = static_cast<u16>(reg >> PatternNameData1Word4CellsOver16Colors10Bits::palette_number_shft << 8);

    pattern_name_data.special_color_calculation = screen.special_color_calculation;
    pattern_name_data.special_priority          = screen.special_priority;
    pattern_name_data.is_horizontally_flipped   = reg >> PatternNameData1Word4CellsOver16Colors10Bits::horizontal_flip_shft;
    pattern_name_data.is_vertically_flipped     = reg >> PatternNameData1Word4CellsOver16Colors10Bits::vertical_flip_shft;

    return pattern_name_data;
};

auto getPatternNameData1Word4CellsOver16Colors12Bits(const u32 data, const ScrollScreenStatus& screen) -> PatternNameData {
    using std::byte;

    auto pattern_name_data = PatternNameData{};
    auto reg               = PatternNameData1Word4CellsOver16Colors12BitsType{data};

    const auto scn                     = static_cast<byte>(screen.supplementary_character_number) & byte{0x10};
    pattern_name_data.character_number = static_cast<u16>(std::to_integer<u16>(scn) << 10);
    pattern_name_data.character_number |= (reg >> PatternNameData1Word4CellsOver16Colors12Bits::character_number_shft << 2);
    const auto scn2 = static_cast<byte>(screen.supplementary_character_number) & byte{0x3};
    pattern_name_data.character_number |= std::to_integer<u16>(scn2);

    pattern_name_data.palette_number
        = static_cast<u16>(reg >> PatternNameData1Word4CellsOver16Colors12Bits::palette_number_shft << 8);

    pattern_name_data.special_color_calculation = screen.special_color_calculation;
    pattern_name_data.special_priority          = screen.special_priority;
    pattern_name_data.is_horizontally_flipped   = false;
    pattern_name_data.is_vertically_flipped     = false;

    return pattern_name_data;
};

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

auto scrollScreenToLayer(const ScrollScreen& ss) -> Layer {
    switch (ss) {
        using enum ScrollScreen;
        case nbg0: return Layer::nbg0;
        case nbg1: return Layer::nbg1;
        case nbg2: return Layer::nbg2;
        case nbg3: return Layer::nbg3;
        case rbg0: return Layer::rbg0;
        case rbg1: return Layer::rbg1;
        default: return Layer::undefined;
    }
}

} // namespace saturnin::video
