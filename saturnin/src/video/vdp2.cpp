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

using LockGuard = std::lock_guard<Mutex>;

constexpr auto use_concurrent_read_for_cells = bool{false};

//--------------------------------------------------------------------------------------------------------------
// PUBLIC section
//--------------------------------------------------------------------------------------------------------------

void Vdp2::initialize() {
    initializeRegisterNameMap();
    computePrecalculatedData();

    const std::string ts = modules_.config()->readValue(core::AccessKeys::cfg_rendering_tv_standard);
    switch (modules_.config()->getTvStandard(ts)) {
        using enum video::TvStandard;
        case pal: tvstat_.tv_standard_flag = TvStandardFlag::pal_standard; break;
        case ntsc: tvstat_.tv_standard_flag = TvStandardFlag::ntsc_standard; break;
        default: Log::warning(Logger::vdp2, tr("Unknown TV standard"));
    }
    calculateDisplayDuration();

    disabled_scroll_screens_[ScrollScreen::nbg0] = false;
    disabled_scroll_screens_[ScrollScreen::nbg1] = false;
    disabled_scroll_screens_[ScrollScreen::nbg2] = false;
    disabled_scroll_screens_[ScrollScreen::nbg3] = false;
    disabled_scroll_screens_[ScrollScreen::rbg0] = false;
    disabled_scroll_screens_[ScrollScreen::rbg1] = false;

    current_time_ = steady_clock::now();
}

void Vdp2::run(const u8 cycles) {
    elapsed_frame_cycles_ += cycles;
    if (elapsed_frame_cycles_ > cycles_per_vactive_) {
        if (!is_vblank_current_) {
            // Entering vertical blanking
            is_vblank_current_          = true;
            tvstat_.vertical_blank_flag = VerticalBlankFlag::during_vertical_retrace;
            tvmd_.display               = Display::not_displayed;

            Log::debug(Logger::vdp2, tr("VBlankIn interrupt request"));

            modules_.vdp1()->onVblankIn();
            this->onVblankIn();

            modules_.scu()->onVblankIn();

            modules_.opengl()->displayFramebuffer(*(modules_.context()));
            if (modules_.context()->debugStatus() == core::DebugStatus::next_frame) {
                modules_.context()->debugStatus(core::DebugStatus::paused);
            }
        }
        // new_time_ = steady_clock::now();
        // auto diff = milli{new_time_ - current_time_};

        // Log::warning(Logger::vdp2, tr("Saturn frame duration : {}ms"), diff.count());
        // current_time_ = new_time_;
    }

    if (elapsed_frame_cycles_ > cycles_per_frame_) {
        // End of the frame display (active + vblank)
        elapsed_frame_cycles_       = 0;
        is_vblank_current_          = false;
        tvstat_.vertical_blank_flag = VerticalBlankFlag::during_vertical_scan;

        elapsed_line_cycles_          = 0;
        is_hblank_current_            = false;
        tvstat_.horizontal_blank_flag = HorizontalBlankFlag::during_horizontal_scan;

        tvmd_.display = Display::displayed;

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
            is_hblank_current_            = true;
            tvstat_.horizontal_blank_flag = HorizontalBlankFlag::during_horizontal_retrace;

            modules_.scu()->onHblankIn();

            timer_0_counter_++;

            if (timer_0_counter_ == modules_.scu()->getTimer0CompareValue()) { modules_.scu()->onTimer0(); }
        }
    }

    if (elapsed_line_cycles_ > cycles_per_line_) {
        // End of line display (active + hblank)
        elapsed_line_cycles_          = 0;
        is_hblank_current_            = false;
        tvstat_.horizontal_blank_flag = HorizontalBlankFlag::during_horizontal_scan;
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

    std::string ts = modules_.context()->config()->readValue(core::AccessKeys::cfg_rendering_tv_standard);
    switch (modules_.context()->config()->getTvStandard(ts)) {
        using enum video::TvStandard;
        case pal: {
            constexpr auto frame_duration = seconds{1.0 / 50.0};
            cycles_per_frame_             = modules_.smpc()->calculateCyclesNumber(frame_duration);

            constexpr auto total_lines   = u16{313};
            auto           visible_lines = u16{};
            switch (toEnum<VerticalResolution>(tvmd_.vertical_resolution)) {
                using enum VerticalResolution;
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
            switch (toEnum<VerticalResolution>(tvmd_.vertical_resolution)) {
                using enum VerticalResolution;
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
    Texture::cleanCache(modules_.opengl(), VdpType::vdp2);
    updateResolution();
    updateRamStatus();
    Texture::setCache(VdpType::vdp2);
    populateRenderData();
    resetCacheState();
}

auto Vdp2::getSpriteColorAddressOffset() -> u16 { return getColorRamAddressOffset(craofb_.color_ram_address_offset_sprite); }

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto Vdp2::getSpritePriority(const u8 register_number) -> u8 {
    // Next line works from LLVM version 14.0.0
    // NOLINTBEGIN(readability-magic-numbers)
    switch (register_number) {
        case 0: return prisa_.sprite_register_0;
        case 1: return static_cast<u8>(prisa_.sprite_register_1);
        case 2: return prisb_.sprite_register_2;
        case 3: return static_cast<u8>(prisb_.sprite_register_3);
        case 4: return prisc_.sprite_register_4;
        case 5: return static_cast<u8>(prisc_.sprite_register_5);
        case 6: return prisd_.sprite_register_6;
        case 7: return static_cast<u8>(prisd_.sprite_register_7);
    }
    // NOLINTEND(readability-magic-numbers)
    core::Log::warning(Logger::vdp2, core::tr("Unknown sprite priority."));
    return 0;
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto Vdp2::getColorOffset(const Layer layer) -> ColorOffset {
    auto enable_bit = ColorOffsetEnableBit{};
    auto select_bit = ColorOffsetSelectBit{};
    switch (layer) {
        using enum Layer;
        case nbg0: {
            enable_bit = toEnum<ColorOffsetEnableBit>(clofen_.nbg0);
            select_bit = toEnum<ColorOffsetSelectBit>(clofsl_.nbg0);
            break;
        }
        case nbg1: {
            enable_bit = toEnum<ColorOffsetEnableBit>(clofen_.nbg1);
            select_bit = toEnum<ColorOffsetSelectBit>(clofsl_.nbg1);
            break;
        }
        case nbg2: {
            enable_bit = toEnum<ColorOffsetEnableBit>(clofen_.nbg2);
            select_bit = toEnum<ColorOffsetSelectBit>(clofsl_.nbg2);
            break;
        }
        case nbg3: {
            enable_bit = toEnum<ColorOffsetEnableBit>(clofen_.nbg3);
            select_bit = toEnum<ColorOffsetSelectBit>(clofsl_.nbg3);
            break;
        }
        case rbg0: {
            enable_bit = toEnum<ColorOffsetEnableBit>(clofen_.rbg0);
            select_bit = toEnum<ColorOffsetSelectBit>(clofsl_.rbg0);
            break;
        }
        case back: {
            enable_bit = toEnum<ColorOffsetEnableBit>(clofen_.back);
            select_bit = toEnum<ColorOffsetSelectBit>(clofsl_.back);
            break;
        }
        case sprite: {
            enable_bit = toEnum<ColorOffsetEnableBit>(clofen_.sprite);
            select_bit = toEnum<ColorOffsetSelectBit>(clofsl_.sprite);
            break;
        }
        default: {
            core::Log::warning(Logger::vdp2, core::tr("Undefined layer for color offset."));
            break;
        }
    }

    auto color_offset = ColorOffset{};
    if (enable_bit == ColorOffsetEnableBit::enabled) {
        constexpr auto sign_ext_mask = u16{0xFF00};

        if (select_bit == ColorOffsetSelectBit::use_color_offset_a) {
            if (toEnum<Sign>(coar_.sign) == Sign::negative) { coar_.raw |= sign_ext_mask; }
            if (toEnum<Sign>(coag_.sign) == Sign::negative) { coag_.raw |= sign_ext_mask; }
            if (toEnum<Sign>(coab_.sign) == Sign::negative) { coab_.raw |= sign_ext_mask; }
            color_offset.as_s16.r = static_cast<s16>(coar_.raw);
            color_offset.as_s16.g = static_cast<s16>(coag_.raw);
            color_offset.as_s16.b = static_cast<s16>(coab_.raw);
        } else {
            if (cobr_.sign == 1) { cobr_.raw |= sign_ext_mask; }
            if (cobg_.sign == 1) { cobg_.raw |= sign_ext_mask; }
            if (cobb_.sign == 1) { cobb_.raw |= sign_ext_mask; }
            color_offset.as_s16.r = static_cast<s16>(cobr_.raw);
            color_offset.as_s16.g = static_cast<s16>(cobg_.raw);
            color_offset.as_s16.b = static_cast<s16>(cobb_.raw);
        }

        color_offset.as_float.r = static_cast<float>(color_offset.as_s16.r) / static_cast<float>(u8_max);
        color_offset.as_float.g = static_cast<float>(color_offset.as_s16.g) / static_cast<float>(u8_max);
        color_offset.as_float.b = static_cast<float>(color_offset.as_s16.b) / static_cast<float>(u8_max);
    }
    return color_offset;
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
        case tv_screen_mode: return tvmd_.raw;
        case external_signal_enable: return exten_.raw;
        case screen_status: return tvstat_.raw;
        case vram_size: return vrsize_.raw;
        case h_counter: return hcnt_.raw;
        case v_counter: return vcnt_.raw;
        case reserve_1: return rsv1_.raw;
        case ram_control: return ramctl_.raw;
        case vram_cycle_pattern_bank_a0_lower: return cyca0l_.raw;
        case vram_cycle_pattern_bank_a0_upper: return cyca0u_.raw;
        case vram_cycle_pattern_bank_a1_lower: return cyca1l_.raw;
        case vram_cycle_pattern_bank_a1_upper: return cyca1u_.raw;
        case vram_cycle_pattern_bank_b0_lower: return cycb0l_.raw;
        case vram_cycle_pattern_bank_b0_upper: return cycb0u_.raw;
        case vram_cycle_pattern_bank_b1_lower: return cycb1l_.raw;
        case vram_cycle_pattern_bank_b1_upper: return cycb1u_.raw;
        case screen_display_enable: return bgon_.raw;
        case mosaic_control: return mzctl_.raw;
        case special_function_code_select: return sfsel_.raw;
        case special_function_code: return sfcode_.raw;
        case character_control_a: return chctla_.raw;
        case character_control_b: return chctlb_.raw;
        case bitmap_palette_number_a: return bmpna_.raw;
        case bitmap_palette_number_b: return bmpnb_.raw;
        case pattern_name_control_nbg0: return pncn0_.raw;
        case pattern_name_control_nbg1: return pncn1_.raw;
        case pattern_name_control_nbg2: return pncn2_.raw;
        case pattern_name_control_nbg3: return pncn3_.raw;
        case pattern_name_control_rbg0: return pncr_.raw;
        case plane_size: return plsz_.raw;
        case map_offset_n: return mpofn_.raw;
        case map_offset_r: return mpofr_.raw;
        case map_nbg0_plane_a_b: return mpabn0_.raw;
        case map_nbg0_plane_c_d: return mpcdn0_.raw;
        case map_nbg1_plane_a_b: return mpabn1_.raw;
        case map_nbg1_plane_c_d: return mpcdn1_.raw;
        case map_nbg2_plane_a_b: return mpabn2_.raw;
        case map_nbg2_plane_c_d: return mpcdn2_.raw;
        case map_nbg3_plane_a_b: return mpabn3_.raw;
        case map_nbg3_plane_c_d: return mpcdn3_.raw;
        case map_rotation_parameter_a_plane_a_b: return mpabra_.raw;
        case map_rotation_parameter_a_plane_c_d: return mpcdra_.raw;
        case map_rotation_parameter_a_plane_e_f: return mpefra_.raw;
        case map_rotation_parameter_a_plane_g_h: return mpghra_.raw;
        case map_rotation_parameter_a_plane_i_j: return mpijra_.raw;
        case map_rotation_parameter_a_plane_k_l: return mpklra_.raw;
        case map_rotation_parameter_a_plane_m_n: return mpmnra_.raw;
        case map_rotation_parameter_a_plane_o_p: return mpopra_.raw;
        case map_rotation_parameter_b_plane_a_b: return mpabrb_.raw;
        case map_rotation_parameter_b_plane_c_d: return mpcdrb_.raw;
        case map_rotation_parameter_b_plane_e_f: return mpefrb_.raw;
        case map_rotation_parameter_b_plane_g_h: return mpghrb_.raw;
        case map_rotation_parameter_b_plane_i_j: return mpijrb_.raw;
        case map_rotation_parameter_b_plane_k_l: return mpklrb_.raw;
        case map_rotation_parameter_b_plane_m_n: return mpmnrb_.raw;
        case map_rotation_parameter_b_plane_o_p: return mpoprb_.raw;
        case screen_scroll_value_nbg0_h_int_part: return scxin0_.raw;
        case screen_scroll_value_nbg0_h_fract_part: return scxdn0_.raw;
        case screen_scroll_value_nbg0_v_int_part: return scyin0_.raw;
        case screen_scroll_value_nbg0_v_fract_part: return scydn0_.raw;
        case coordinate_increment_nbg0_h_int_part: return zmxin0_.raw;
        case coordinate_increment_nbg0_h_fract_part: return zmxdn0_.raw;
        case coordinate_increment_nbg0_v_int_part: return zmyin0_.raw;
        case coordinate_increment_nbg0_v_fract_part: return zmydn0_.raw;
        case screen_scroll_value_nbg1_h_int_part: return scxin1_.raw;
        case screen_scroll_value_nbg1_h_fract_part: return scxdn1_.raw;
        case screen_scroll_value_nbg1_v_int_part: return scyin1_.raw;
        case screen_scroll_value_nbg1_v_fract_part: return scydn1_.raw;
        case coordinate_increment_nbg1_h_int_part: return zmxin1_.raw;
        case coordinate_increment_nbg1_h_fract_part: return zmxdn1_.raw;
        case coordinate_increment_nbg1_v_int_part: return zmyin1_.raw;
        case coordinate_increment_nbg1_v_fract_part: return zmydn1_.raw;
        case screen_scroll_value_nbg2_h: return scxn2_.raw;
        case screen_scroll_value_nbg2_v: return scyn2_.raw;
        case screen_scroll_value_nbg3_h: return scxn3_.raw;
        case screen_scroll_value_nbg3_v: return scyn3_.raw;
        case reduction_enable: return zmctl_.raw;
        case line_and_vertical_cell_scroll_control: return scrctl_.raw;
        case vertical_cell_scroll_table_address_upper: return vcstau_.raw;
        case vertical_cell_scroll_table_address_lower: return vcstal_.raw;
        case line_scroll_table_address_nbg0_upper: return lsta0u_.raw;
        case line_scroll_table_address_nbg0_lower: return lsta0l_.raw;
        case line_scroll_table_address_nbg1_upper: return lsta1u_.raw;
        case line_scroll_table_address_nbg1_lower: return lsta1l_.raw;
        case line_color_screen_table_address_upper: return lctau_.raw;
        case line_color_screen_table_address_lower: return lctal_.raw;
        case back_screen_table_address_upper: return bktau_.raw;
        case back_screen_table_address_lower: return bktal_.raw;
        case rotation_parameter_mode: return rpmd_.raw;
        case rotation_parameter_read_control: return rprctl_.raw;
        case coefficient_table_control: return ktctl_.raw;
        case coefficient_table_address_offset: return ktaof_.raw;
        case screen_over_pattern_name_a: return ovpnra_.raw;
        case screen_over_pattern_name_b: return ovpnrb_.raw;
        case rotation_parameter_table_address_upper: return rptau_.raw;
        case rotation_parameter_table_address_lower: return rptal_.raw;
        case window_position_w0_h_start_point: return wpsx0_.raw;
        case window_position_w0_v_start_point: return wpsy0_.raw;
        case window_position_w0_h_end_point: return wpex0_.raw;
        case window_position_w0_v_end_point: return wpey0_.raw;
        case window_position_w1_h_start_point: return wpsx1_.raw;
        case window_position_w1_v_start_point: return wpsy1_.raw;
        case window_position_w1_h_end_point: return wpex1_.raw;
        case window_position_w1_v_end_point: return wpey1_.raw;
        case window_control_a: return wctla_.raw;
        case window_control_b: return wctlb_.raw;
        case window_control_c: return wctlc_.raw;
        case window_control_d: return wctld_.raw;
        case line_window_table_address_w0_upper: return lwta0u_.raw;
        case line_window_table_address_w0_lower: return lwta0l_.raw;
        case line_window_table_address_w1_upper: return lwta1u_.raw;
        case line_window_table_address_w1_lower: return lwta1l_.raw;
        case sprite_control: return spctl_.raw;
        case shadow_control: return sdctl_.raw;
        case color_ram_address_offset_a: return craofa_.raw;
        case color_ram_address_offset_b: return craofb_.raw;
        case line_color_screen_enable: return lnclen_.raw;
        case special_priority_mode: return sfprmd_.raw;
        case color_calculation_control: return ccctl_.raw;
        case special_color_calculation_mode: return sfccmd_.raw;
        case priority_number_sprite_0_1: return prisa_.raw;
        case priority_number_sprite_2_3: return prisb_.raw;
        case priority_number_sprite_4_5: return prisc_.raw;
        case priority_number_sprite_6_7: return prisd_.raw;
        case priority_number_nbg0_nbg1: return prina_.raw;
        case priority_number_nbg2_nbg3: return prinb_.raw;
        case priority_number_rbg0: return prir_.raw;
        case reserve_2: return rsv2_.raw;
        case color_calculation_ratio_sprite_0_1: return ccrsa_.raw;
        case color_calculation_ratio_sprite_2_3: return ccrsb_.raw;
        case color_calculation_ratio_sprite_4_5: return ccrsc_.raw;
        case color_calculation_ratio_sprite_6_7: return ccrsd_.raw;
        case color_calculation_ratio_nbg0_nbg1: return ccrna_.raw;
        case color_calculation_ratio_nbg2_nbg3: return ccrnb_.raw;
        case color_calculation_ratio_rbg0: return ccrr_.raw;
        case color_calculation_ratio_line_color_back: return ccrlb_.raw;
        case color_offset_enable: return clofen_.raw;
        case color_offset_select: return clofsl_.raw;
        case color_offset_a_red: return coar_.raw;
        case color_offset_a_green: return coag_.raw;
        case color_offset_a_blue: return coab_.raw;
        case color_offset_b_red: return cobr_.raw;
        case color_offset_b_green: return cobg_.raw;
        case color_offset_b_blue: return cobb_.raw;

        default: core::Log::warning(Logger::vdp2, core::tr("Unimplemented register read (16) {:#010x}"), addr);
    }

    return 0;
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void Vdp2::write16(const u32 addr, const u16 data) {
    switch (addr) {
        case tv_screen_mode: tvmd_.raw = data; break;
        case external_signal_enable: exten_.raw = data; break;
        case vram_size: vrsize_.raw = data; break;
        case h_counter: hcnt_.raw = data; break;
        case v_counter: vcnt_.raw = data; break;
        case reserve_1: rsv1_.raw = data; break;
        case ram_control: ramctl_.raw = data; break;
        case vram_cycle_pattern_bank_a0_lower: cyca0l_.raw = data; break;
        case vram_cycle_pattern_bank_a0_upper: cyca0u_.raw = data; break;
        case vram_cycle_pattern_bank_a1_lower: cyca1l_.raw = data; break;
        case vram_cycle_pattern_bank_a1_upper: cyca1u_.raw = data; break;
        case vram_cycle_pattern_bank_b0_lower: cycb0l_.raw = data; break;
        case vram_cycle_pattern_bank_b0_upper: cycb0u_.raw = data; break;
        case vram_cycle_pattern_bank_b1_lower: cycb1l_.raw = data; break;
        case vram_cycle_pattern_bank_b1_upper: cycb1u_.raw = data; break;
        case screen_display_enable: bgon_.raw = data; break;
        case mosaic_control: mzctl_.raw = data; break;
        case special_function_code_select: sfsel_.raw = data; break;
        case special_function_code: sfcode_.raw = data; break;
        case character_control_a: chctla_.raw = data; break;
        case character_control_b: chctlb_.raw = data; break;
        case bitmap_palette_number_a: bmpna_.raw = data; break;
        case bitmap_palette_number_b: bmpnb_.raw = data; break;
        case pattern_name_control_nbg0: pncn0_.raw = data; break;
        case pattern_name_control_nbg1: pncn1_.raw = data; break;
        case pattern_name_control_nbg2: pncn2_.raw = data; break;
        case pattern_name_control_nbg3: pncn3_.raw = data; break;
        case pattern_name_control_rbg0: pncr_.raw = data; break;
        case plane_size: plsz_.raw = data; break;
        case map_offset_n: mpofn_.raw = data; break;
        case map_offset_r: mpofr_.raw = data; break;
        case map_nbg0_plane_a_b: mpabn0_.raw = data; break;
        case map_nbg0_plane_c_d: mpcdn0_.raw = data; break;
        case map_nbg1_plane_a_b: mpabn1_.raw = data; break;
        case map_nbg1_plane_c_d: mpcdn1_.raw = data; break;
        case map_nbg2_plane_a_b: mpabn2_.raw = data; break;
        case map_nbg2_plane_c_d: mpcdn2_.raw = data; break;
        case map_nbg3_plane_a_b: mpabn3_.raw = data; break;
        case map_nbg3_plane_c_d: mpcdn3_.raw = data; break;
        case map_rotation_parameter_a_plane_a_b: mpabra_.raw = data; break;
        case map_rotation_parameter_a_plane_c_d: mpcdra_.raw = data; break;
        case map_rotation_parameter_a_plane_e_f: mpefra_.raw = data; break;
        case map_rotation_parameter_a_plane_g_h: mpghra_.raw = data; break;
        case map_rotation_parameter_a_plane_i_j: mpijra_.raw = data; break;
        case map_rotation_parameter_a_plane_k_l: mpklra_.raw = data; break;
        case map_rotation_parameter_a_plane_m_n: mpmnra_.raw = data; break;
        case map_rotation_parameter_a_plane_o_p: mpopra_.raw = data; break;
        case map_rotation_parameter_b_plane_a_b: mpabrb_.raw = data; break;
        case map_rotation_parameter_b_plane_c_d: mpcdrb_.raw = data; break;
        case map_rotation_parameter_b_plane_e_f: mpefrb_.raw = data; break;
        case map_rotation_parameter_b_plane_g_h: mpghrb_.raw = data; break;
        case map_rotation_parameter_b_plane_i_j: mpijrb_.raw = data; break;
        case map_rotation_parameter_b_plane_k_l: mpklrb_.raw = data; break;
        case map_rotation_parameter_b_plane_m_n: mpmnrb_.raw = data; break;
        case map_rotation_parameter_b_plane_o_p: mpoprb_.raw = data; break;
        case screen_scroll_value_nbg0_h_int_part: scxin0_.raw = data; break;
        case screen_scroll_value_nbg0_h_fract_part: scxdn0_.raw = data; break;
        case screen_scroll_value_nbg0_v_int_part: scyin0_.raw = data; break;
        case screen_scroll_value_nbg0_v_fract_part: scydn0_.raw = data; break;
        case coordinate_increment_nbg0_h_int_part: zmxin0_.raw = data; break;
        case coordinate_increment_nbg0_h_fract_part: zmxdn0_.raw = data; break;
        case coordinate_increment_nbg0_v_int_part: zmyin0_.raw = data; break;
        case coordinate_increment_nbg0_v_fract_part: zmydn0_.raw = data; break;
        case screen_scroll_value_nbg1_h_int_part: scxin1_.raw = data; break;
        case screen_scroll_value_nbg1_h_fract_part: scxdn1_.raw = data; break;
        case screen_scroll_value_nbg1_v_int_part: scyin1_.raw = data; break;
        case screen_scroll_value_nbg1_v_fract_part: scydn1_.raw = data; break;
        case coordinate_increment_nbg1_h_int_part: zmxin1_.raw = data; break;
        case coordinate_increment_nbg1_h_fract_part: zmxdn1_.raw = data; break;
        case coordinate_increment_nbg1_v_int_part: zmyin1_.raw = data; break;
        case coordinate_increment_nbg1_v_fract_part: zmydn1_.raw = data; break;
        case screen_scroll_value_nbg2_h: scxn2_.raw = data; break;
        case screen_scroll_value_nbg2_v: scyn2_.raw = data; break;
        case screen_scroll_value_nbg3_h: scxn3_.raw = data; break;
        case screen_scroll_value_nbg3_v: scyn3_.raw = data; break;
        case reduction_enable: zmctl_.raw = data; break;
        case line_and_vertical_cell_scroll_control: scrctl_.raw = data; break;
        case vertical_cell_scroll_table_address_upper: vcstau_.raw = data; break;
        case vertical_cell_scroll_table_address_lower: vcstal_.raw = data; break;
        case line_scroll_table_address_nbg0_upper: lsta0u_.raw = data; break;
        case line_scroll_table_address_nbg0_lower: lsta0l_.raw = data; break;
        case line_scroll_table_address_nbg1_upper: lsta1u_.raw = data; break;
        case line_scroll_table_address_nbg1_lower: lsta1l_.raw = data; break;
        case line_color_screen_table_address_upper: lctau_.raw = data; break;
        case line_color_screen_table_address_lower: lctal_.raw = data; break;
        case back_screen_table_address_upper: bktau_.raw = data; break;
        case back_screen_table_address_lower: bktal_.raw = data; break;
        case rotation_parameter_mode: rpmd_.raw = data; break;
        case rotation_parameter_read_control: rprctl_.raw = data; break;
        case coefficient_table_control: ktctl_.raw = data; break;
        case coefficient_table_address_offset: ktaof_.raw = data; break;
        case screen_over_pattern_name_a: ovpnra_.raw = data; break;
        case screen_over_pattern_name_b: ovpnrb_.raw = data; break;
        case rotation_parameter_table_address_upper: rptau_.raw = data; break;
        case rotation_parameter_table_address_lower: rptal_.raw = data; break;
        case window_position_w0_h_start_point: wpsx0_.raw = data; break;
        case window_position_w0_v_start_point: wpsy0_.raw = data; break;
        case window_position_w0_h_end_point: wpex0_.raw = data; break;
        case window_position_w0_v_end_point: wpey0_.raw = data; break;
        case window_position_w1_h_start_point: wpsx1_.raw = data; break;
        case window_position_w1_v_start_point: wpsy1_.raw = data; break;
        case window_position_w1_h_end_point: wpex1_.raw = data; break;
        case window_position_w1_v_end_point: wpey1_.raw = data; break;
        case window_control_a: wctla_.raw = data; break;
        case window_control_b: wctlb_.raw = data; break;
        case window_control_c: wctlc_.raw = data; break;
        case window_control_d: wctld_.raw = data; break;
        case line_window_table_address_w0_upper: lwta0u_.raw = data; break;
        case line_window_table_address_w0_lower: lwta0l_.raw = data; break;
        case line_window_table_address_w1_upper: lwta1u_.raw = data; break;
        case line_window_table_address_w1_lower: lwta1l_.raw = data; break;
        case sprite_control: spctl_.raw = data; break;
        case shadow_control: sdctl_.raw = data; break;
        case color_ram_address_offset_a: craofa_.raw = data; break;
        case color_ram_address_offset_b: craofb_.raw = data; break;
        case line_color_screen_enable: lnclen_.raw = data; break;
        case special_priority_mode: sfprmd_.raw = data; break;
        case color_calculation_control: ccctl_.raw = data; break;
        case special_color_calculation_mode: sfccmd_.raw = data; break;
        case priority_number_sprite_0_1: prisa_.raw = data; break;
        case priority_number_sprite_2_3: prisb_.raw = data; break;
        case priority_number_sprite_4_5: prisc_.raw = data; break;
        case priority_number_sprite_6_7: prisd_.raw = data; break;
        case priority_number_nbg0_nbg1: prina_.raw = data; break;
        case priority_number_nbg2_nbg3: prinb_.raw = data; break;
        case priority_number_rbg0: prir_.raw = data; break;
        case reserve_2: rsv2_.raw = data; break;
        case color_calculation_ratio_sprite_0_1: ccrsa_.raw = data; break;
        case color_calculation_ratio_sprite_2_3: ccrsb_.raw = data; break;
        case color_calculation_ratio_sprite_4_5: ccrsc_.raw = data; break;
        case color_calculation_ratio_sprite_6_7: ccrsd_.raw = data; break;
        case color_calculation_ratio_nbg0_nbg1: ccrna_.raw = data; break;
        case color_calculation_ratio_nbg2_nbg3: ccrnb_.raw = data; break;
        case color_calculation_ratio_rbg0: ccrr_.raw = data; break;
        case color_calculation_ratio_line_color_back: ccrlb_.raw = data; break;
        case color_offset_enable: clofen_.raw = data; break;
        case color_offset_select: clofsl_.raw = data; break;
        case color_offset_a_red: coar_.raw = data; break;
        case color_offset_a_green: coag_.raw = data; break;
        case color_offset_a_blue: coab_.raw = data; break;
        case color_offset_b_red: cobr_.raw = data; break;
        case color_offset_b_green: cobg_.raw = data; break;
        case color_offset_b_blue: cobb_.raw = data; break;
        default: core::Log::warning(Logger::vdp2, core::tr("Unimplemented register write (16) {:#010x}"), addr);
    }
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void Vdp2::write32(const u32 addr, const u32 data) {
    const auto h = static_cast<u16>(data >> displacement_16);
    const auto l = static_cast<u16>(data & bitmask_FFFF);
    switch (addr) {
        case vram_cycle_pattern_bank_a0_lower:
            cyca0l_.raw = h;
            cyca0u_.raw = l;
            break;
        case vram_cycle_pattern_bank_a1_lower:
            cyca1l_.raw = h;
            cyca1u_.raw = l;
            break;
        case vram_cycle_pattern_bank_b0_lower:
            cycb0l_.raw = h;
            cycb0u_.raw = l;
            break;
        case vram_cycle_pattern_bank_b1_lower:
            cycb1l_.raw = h;
            cycb1u_.raw = l;
            break;
        case screen_display_enable:
            bgon_.raw  = h;
            mzctl_.raw = l;
            break;
        case special_function_code_select:
            sfsel_.raw  = h;
            sfcode_.raw = l;
            break;
        case character_control_a:
            chctla_.raw = h;
            chctlb_.raw = l;
            break;
        case bitmap_palette_number_a:
            bmpna_.raw = h;
            bmpnb_.raw = l;
            break;
        case pattern_name_control_nbg0:
            pncn0_.raw = h;
            pncn1_.raw = l;
            break;
        case pattern_name_control_nbg2:
            pncn2_.raw = h;
            pncn3_.raw = l;
            break;
        case pattern_name_control_rbg0:
            pncr_.raw = h;
            plsz_.raw = l;
            break;
        case map_offset_n:
            mpofn_.raw = h;
            mpofr_.raw = l;
            break;
        case map_nbg0_plane_a_b:
            mpabn0_.raw = h;
            mpcdn0_.raw = l;
            break;
        case map_nbg1_plane_a_b:
            mpabn1_.raw = h;
            mpcdn1_.raw = l;
            break;
        case map_nbg2_plane_a_b:
            mpabn2_.raw = h;
            mpcdn2_.raw = l;
            break;
        case map_nbg3_plane_a_b:
            mpabn3_.raw = h;
            mpcdn3_.raw = l;
            break;
        case map_rotation_parameter_a_plane_a_b:
            mpabra_.raw = h;
            mpcdra_.raw = l;
            break;
        case map_rotation_parameter_a_plane_e_f:
            mpefra_.raw = h;
            mpghra_.raw = l;
            break;
        case map_rotation_parameter_a_plane_i_j:
            mpijra_.raw = h;
            mpklra_.raw = l;
            break;
        case map_rotation_parameter_a_plane_m_n:
            mpmnra_.raw = h;
            mpopra_.raw = l;
            break;
        case map_rotation_parameter_b_plane_a_b:
            mpabrb_.raw = h;
            mpcdrb_.raw = l;
            break;
        case map_rotation_parameter_b_plane_e_f:
            mpefrb_.raw = h;
            mpghrb_.raw = l;
            break;
        case map_rotation_parameter_b_plane_i_j:
            mpijrb_.raw = h;
            mpklrb_.raw = l;
            break;
        case map_rotation_parameter_b_plane_m_n:
            mpmnrb_.raw = h;
            mpoprb_.raw = l;
            break;
        case screen_scroll_value_nbg0_h_int_part:
            scxin0_.raw = h;
            scxdn0_.raw = l;
            break;
        case screen_scroll_value_nbg0_v_int_part:
            scyin0_.raw = h;
            scydn0_.raw = l;
            break;
        case coordinate_increment_nbg0_h_int_part:
            zmxin0_.raw = h;
            zmxdn0_.raw = l;
            break;
        case coordinate_increment_nbg0_v_int_part:
            zmyin0_.raw = h;
            zmydn0_.raw = l;
            break;
        case screen_scroll_value_nbg1_h_int_part:
            scxin1_.raw = h;
            scxdn1_.raw = l;
            break;
        case screen_scroll_value_nbg1_v_int_part:
            scyin1_.raw = h;
            scydn1_.raw = l;
            break;
        case coordinate_increment_nbg1_h_int_part:
            zmxin1_.raw = h;
            zmxdn1_.raw = l;
            break;
        case coordinate_increment_nbg1_v_int_part:
            zmyin1_.raw = h;
            zmydn1_.raw = l;
            break;
        case screen_scroll_value_nbg2_h:
            scxn2_.raw = h;
            scyn2_.raw = l;
            break;
        case screen_scroll_value_nbg3_h:
            scxn3_.raw = h;
            scyn3_.raw = l;
            break;
        case reduction_enable:
            zmctl_.raw  = h;
            scrctl_.raw = l;
            break;
        case vertical_cell_scroll_table_address_upper:
            vcstau_.raw = h;
            vcstal_.raw = l;
            break;
        case line_scroll_table_address_nbg0_upper:
            lsta0u_.raw = h;
            lsta0l_.raw = l;
            break;
        case line_scroll_table_address_nbg1_upper:
            lsta1u_.raw = h;
            lsta1l_.raw = l;
            break;
        case line_color_screen_table_address_upper:
            lctau_.raw = h;
            lctal_.raw = l;
            break;
        case back_screen_table_address_upper:
            bktau_.raw = h;
            bktal_.raw = l;
            break;
        case rotation_parameter_mode:
            rpmd_.raw   = h;
            rprctl_.raw = l;
            break;
        case coefficient_table_control:
            ktctl_.raw = h;
            ktaof_.raw = l;
            break;
        case screen_over_pattern_name_a:
            ovpnra_.raw = h;
            ovpnrb_.raw = l;
            break;
        case rotation_parameter_table_address_upper:
            rptau_.raw = h;
            rptal_.raw = l;
            break;
        case window_position_w0_h_start_point:
            wpsx0_.raw = h;
            wpsy0_.raw = l;
            break;
        case window_position_w0_h_end_point:
            wpex0_.raw = h;
            wpey0_.raw = l;
            break;
        case window_position_w1_h_start_point:
            wpsx1_.raw = h;
            wpsy1_.raw = l;
            break;
        case window_position_w1_h_end_point:
            wpex1_.raw = h;
            wpey1_.raw = l;
            break;
        case window_control_a:
            wctla_.raw = h;
            wctlb_.raw = l;
            break;
        case window_control_c:
            wctlc_.raw = h;
            wctld_.raw = l;
            break;
        case line_window_table_address_w0_upper:
            lwta0u_.raw = h;
            lwta0l_.raw = l;
            break;
        case line_window_table_address_w1_upper:
            lwta1u_.raw = h;
            lwta1l_.raw = l;
            break;
        case sprite_control:
            spctl_.raw = h;
            sdctl_.raw = l;
            break;
        case color_ram_address_offset_a:
            craofa_.raw = h;
            craofb_.raw = l;
            break;
        case line_color_screen_enable:
            lnclen_.raw = h;
            sfprmd_.raw = l;
            break;
        case color_calculation_control:
            ccctl_.raw  = h;
            sfccmd_.raw = l;
            break;
        case priority_number_sprite_0_1:
            prisa_.raw = h;
            prisb_.raw = l;
            break;
        case priority_number_sprite_4_5:
            prisc_.raw = h;
            prisd_.raw = l;
            break;
        case priority_number_nbg0_nbg1:
            prina_.raw = h;
            prinb_.raw = l;
            break;
        case priority_number_rbg0:
            prir_.raw = h;
            rsv2_.raw = l;
            break;
        case color_calculation_ratio_sprite_0_1:
            ccrsa_.raw = h;
            ccrsb_.raw = l;
            break;
        case color_calculation_ratio_sprite_4_5:
            ccrsc_.raw = h;
            ccrsd_.raw = l;
            break;
        case color_calculation_ratio_nbg0_nbg1:
            ccrna_.raw = h;
            ccrnb_.raw = l;
            break;
        case color_calculation_ratio_rbg0:
            ccrr_.raw  = h;
            ccrlb_.raw = l;
            break;
        case color_offset_enable:
            clofen_.raw = h;
            clofsl_.raw = l;
            break;
        case color_offset_a_red:
            coar_.raw = h;
            coag_.raw = l;
            break;
        case color_offset_a_blue:
            coab_.raw = h;
            cobr_.raw = l;
            break;
        case color_offset_b_green:
            cobg_.raw = h;
            cobb_.raw = l;
            break;
        default: core::Log::warning(Logger::vdp2, core::tr("Unimplemented register write (16) {:#010x}"), addr);
    }
}

auto Vdp2::screenInDebug() -> ScrollScreen { return screen_in_debug_; }
void Vdp2::screenInDebug(const ScrollScreen s) { screen_in_debug_ = s; }

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
    tv_screen_status_.is_picture_displayed = (toEnum<Display>(tvmd_.display) == Display::displayed);
    tv_screen_status_.border_color_mode    = toEnum<BorderColorMode>(tvmd_.border_color_mode);
    tv_screen_status_.interlace_mode       = toEnum<InterlaceMode>(tvmd_.interlace_mode);

    switch (toEnum<HorizontalResolution>(tvmd_.horizontal_resolution)) {
        using enum HorizontalResolution;
        case normal_320:
            tv_screen_status_.horizontal_res   = horizontal_res_320;
            tv_screen_status_.screen_mode_type = ScreenModeType::normal;
            if (tv_screen_status_.interlace_mode == InterlaceMode::non_interlace) {
                switch (toEnum<VerticalResolution>(tvmd_.vertical_resolution)) {
                    using enum VerticalResolution;
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
                switch (toEnum<VerticalResolution>(tvmd_.vertical_resolution)) {
                    using enum VerticalResolution;
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
            if (tv_screen_status_.interlace_mode == InterlaceMode::non_interlace) {
                switch (toEnum<VerticalResolution>(tvmd_.vertical_resolution)) {
                    using enum VerticalResolution;
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
                switch (toEnum<VerticalResolution>(tvmd_.vertical_resolution)) {
                    using enum VerticalResolution;
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
            if (tv_screen_status_.interlace_mode == InterlaceMode::non_interlace) {
                switch (toEnum<VerticalResolution>(tvmd_.vertical_resolution)) {
                    using enum VerticalResolution;
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
                switch (toEnum<VerticalResolution>(tvmd_.vertical_resolution)) {
                    using enum VerticalResolution;
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
            if (tv_screen_status_.interlace_mode == InterlaceMode::non_interlace) {
                switch (toEnum<VerticalResolution>(tvmd_.vertical_resolution)) {
                    using enum VerticalResolution;
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
                switch (toEnum<VerticalResolution>(tvmd_.vertical_resolution)) {
                    using enum VerticalResolution;
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
    ram_status_.vram_size      = toEnum<VramSize>(static_cast<bool>(vrsize_.vram_size));
    ram_status_.vram_a_mode    = toEnum<VramMode>(ramctl_.vram_a_mode);
    ram_status_.vram_b_mode    = toEnum<VramMode>(ramctl_.vram_b_mode);
    ram_status_.color_ram_mode = toEnum<ColorRamMode>(ramctl_.color_ram_mode);
    // memory()->vdp2_cram_memory_mask_         = (ram_status_.color_ram_mode == ColorRamMode::mode_1_rgb_5_bits_2048_colors)
    //                                               ? core::vdp2_cram_32Kb_memory_mask
    //                                               : core::vdp2_cram_16Kb_memory_mask;
    ram_status_.coefficient_table_storage    = toEnum<CoefficientTableStorage>(ramctl_.coefficient_table_storage);
    ram_status_.vram_a0_rotation_bank_select = toEnum<RotationDataBankSelect>(ramctl_.vram_a0_rotation_bank_select);
    ram_status_.vram_a1_rotation_bank_select = toEnum<RotationDataBankSelect>(ramctl_.vram_a1_rotation_bank_select);
    ram_status_.vram_b0_rotation_bank_select = toEnum<RotationDataBankSelect>(ramctl_.vram_b0_rotation_bank_select);
    ram_status_.vram_b1_rotation_bank_select = toEnum<RotationDataBankSelect>(ramctl_.vram_b1_rotation_bank_select);
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
        using enum ScrollScreen;
        case nbg0: {
            if (toEnum<ScreenDisplayEnableBit>(bgon_.screen_display_enable_nbg0) == ScreenDisplayEnableBit::cannot_display) {
                return false;
            }

            // Pattern name data reads depend on the reduction setting of the screen
            const auto reduction = getReductionSetting(static_cast<ZoomQuarter>(static_cast<bool>(zmctl_.zoom_quarter_nbg0)),
                                                       static_cast<ZoomHalf>(static_cast<bool>(zmctl_.zoom_half_nbg0)));

            // Character / Bitmap pattern data reads depend on the reduction setting and the number of colors
            if (toEnum<BitmapEnable>(chctla_.bitmap_enable_nbg0) == BitmapEnable::bitmap_format) {
                // Bitmap format needs only bitmap pattern data.
                const auto color_number = toEnum<CharacterColorNumber3Bits>(chctla_.character_color_number_nbg0);
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

                const auto color_number = toEnum<CharacterColorNumber3Bits>(chctla_.character_color_number_nbg0);
                const auto required_cpd_reads
                    = util::toUnderlying(calculateRequiredVramCharacterPatternReads(reduction, color_number));
                const auto current_cpd_reads
                    = getVramAccessByCommand(VramAccessCommand::nbg0_character_pattern_data_read, reduction);
                if (current_cpd_reads < required_cpd_reads) { return false; }
            }
            break;
        }
        case nbg1: {
            if (toEnum<ScreenDisplayEnableBit>(bgon_.screen_display_enable_nbg1) == ScreenDisplayEnableBit::cannot_display) {
                return false;
            }
            // Pattern name data reads depend on the reduction setting of the screen
            const auto reduction = getReductionSetting(static_cast<ZoomQuarter>(static_cast<bool>(zmctl_.zoom_quarter_nbg1)),
                                                       static_cast<ZoomHalf>(static_cast<bool>(zmctl_.zoom_half_nbg1)));

            // Character / Bitmap pattern data reads depend on the reduction setting and the number of colors
            if (toEnum<BitmapEnable>(chctla_.bitmap_enable_nbg1) == BitmapEnable::bitmap_format) {
                // Bitmap format needs only bitmap pattern data.
                const auto color_number = toEnum<CharacterColorNumber2Bits>(chctla_.character_color_number_nbg1);
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

                const auto color_number = toEnum<CharacterColorNumber2Bits>(chctla_.character_color_number_nbg1);
                const auto required_cpd_reads
                    = util::toUnderlying(calculateRequiredVramCharacterPatternReads(reduction, color_number));
                const auto current_cpd_reads
                    = getVramAccessByCommand(VramAccessCommand::nbg1_character_pattern_data_read, reduction);
                if (current_cpd_reads < required_cpd_reads) { return false; }
            }

            break;
        }
        case nbg2: {
            if (toEnum<ScreenDisplayEnableBit>(bgon_.screen_display_enable_nbg2) == ScreenDisplayEnableBit::cannot_display) {
                return false;
            }

            if (isScreenDisplayLimitedByReduction(s)) { return false; }

            const auto reduction = ReductionSetting::none;

            // Character format needs character pattern data (cpd) and pattern name data (pnd).
            const auto required_pnd_reads = util::toUnderlying(calculateRequiredVramPatternNameReads(reduction));
            const auto current_pnd_reads  = getVramAccessByCommand(VramAccessCommand::nbg2_pattern_name_read, reduction);
            if (current_pnd_reads < required_pnd_reads) { return false; }

            const auto color_number = toEnum<CharacterColorNumber1Bit>(chctlb_.character_color_number_nbg2);
            const auto required_cpd_reads
                = util::toUnderlying(calculateRequiredVramCharacterPatternReads(reduction, color_number));
            const auto current_cpd_reads = getVramAccessByCommand(VramAccessCommand::nbg2_character_pattern_data_read, reduction);
            if (current_cpd_reads < required_cpd_reads) { return false; }

            break;
        }
        case nbg3: {
            if (toEnum<ScreenDisplayEnableBit>(bgon_.screen_display_enable_nbg3) == ScreenDisplayEnableBit::cannot_display) {
                return false;
            }
            if (isScreenDisplayLimitedByReduction(s)) { return false; }

            const auto reduction = ReductionSetting::none;

            // Character format needs character pattern data (cpd) and pattern name data (pnd).
            const auto required_pnd_reads = util::toUnderlying(calculateRequiredVramPatternNameReads(reduction));
            const auto current_pnd_reads  = getVramAccessByCommand(VramAccessCommand::nbg3_pattern_name_read, reduction);
            if (current_pnd_reads < required_pnd_reads) { return false; }

            const auto color_number = toEnum<CharacterColorNumber1Bit>(chctlb_.character_color_number_nbg3);
            const auto required_cpd_reads
                = util::toUnderlying(calculateRequiredVramCharacterPatternReads(reduction, color_number));
            const auto current_cpd_reads = getVramAccessByCommand(VramAccessCommand::nbg3_character_pattern_data_read, reduction);
            if (current_cpd_reads < required_cpd_reads) { return false; }
            break;
        }
        case rbg0: {
            if (toEnum<ScreenDisplayEnableBit>(bgon_.screen_display_enable_rbg0) == ScreenDisplayEnableBit::cannot_display) {
                return false;
            }
            core::Log::unimplemented(core::tr("VDP2 RBG0 display"));
            break;
        }
        case rbg1: {
            if (toEnum<ScreenDisplayEnableBit>(bgon_.screen_display_enable_rbg1) == ScreenDisplayEnableBit::cannot_display) {
                return false;
            }
            core::Log::unimplemented(core::tr("VDP2 RBG1 display"));
            break;
        }
        default: {
            Log::warning(Logger::vdp2, tr("Scroll scrren not set"));
        }
    }

    getScreen(s).is_display_enabled = true;

    return true;
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto Vdp2::isScreenDisplayLimitedByReduction(ScrollScreen s) -> bool {
    switch (s) {
        using enum ScrollScreen;
        case nbg2: {
            const auto reduction    = getReductionSetting(static_cast<ZoomQuarter>(static_cast<bool>(zmctl_.zoom_quarter_nbg0)),
                                                       static_cast<ZoomHalf>(static_cast<bool>(zmctl_.zoom_half_nbg0)));
            const auto color_number = toEnum<CharacterColorNumber3Bits>(chctla_.character_color_number_nbg0);

            if ((reduction == ReductionSetting::up_to_one_quarter) && (color_number == CharacterColorNumber3Bits::palette_16)) {
                return true;
            }
            if ((reduction == ReductionSetting::up_to_one_half) && (color_number == CharacterColorNumber3Bits::palette_256)) {
                return true;
            }
            break;
        }
        case nbg3: {
            const auto reduction    = getReductionSetting(static_cast<ZoomQuarter>(static_cast<bool>(zmctl_.zoom_quarter_nbg1)),
                                                       static_cast<ZoomHalf>(static_cast<bool>(zmctl_.zoom_half_nbg1)));
            const auto color_number = toEnum<CharacterColorNumber2Bits>(chctla_.character_color_number_nbg1);

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

    auto is_normal_mode = (toEnum<HorizontalResolution>(tvmd_.horizontal_resolution) == HorizontalResolution::normal_320);
    is_normal_mode |= (toEnum<HorizontalResolution>(tvmd_.horizontal_resolution) == HorizontalResolution::normal_352);

    VramTiming bank_a0 = {toEnum<VramAccessCommand>(cyca0l_.t0),
                          toEnum<VramAccessCommand>(cyca0l_.t1),
                          toEnum<VramAccessCommand>(cyca0l_.t2),
                          toEnum<VramAccessCommand>(cyca0l_.t3),
                          is_normal_mode ? toEnum<VramAccessCommand>(cyca0u_.t4) : VramAccessCommand::no_access,
                          is_normal_mode ? toEnum<VramAccessCommand>(cyca0u_.t5) : VramAccessCommand::no_access,
                          is_normal_mode ? toEnum<VramAccessCommand>(cyca0u_.t6) : VramAccessCommand::no_access,
                          is_normal_mode ? toEnum<VramAccessCommand>(cyca0u_.t7) : VramAccessCommand::no_access};

    VramTiming bank_a1 = {toEnum<VramAccessCommand>(cyca1l_.t0),
                          toEnum<VramAccessCommand>(cyca1l_.t1),
                          toEnum<VramAccessCommand>(cyca1l_.t2),
                          toEnum<VramAccessCommand>(cyca1l_.t3),
                          is_normal_mode ? toEnum<VramAccessCommand>(cyca1u_.t4) : VramAccessCommand::no_access,
                          is_normal_mode ? toEnum<VramAccessCommand>(cyca1u_.t5) : VramAccessCommand::no_access,
                          is_normal_mode ? toEnum<VramAccessCommand>(cyca1u_.t6) : VramAccessCommand::no_access,
                          is_normal_mode ? toEnum<VramAccessCommand>(cyca1u_.t7) : VramAccessCommand::no_access};

    VramTiming bank_b0 = {toEnum<VramAccessCommand>(cycb0l_.t0),
                          toEnum<VramAccessCommand>(cycb0l_.t1),
                          toEnum<VramAccessCommand>(cycb0l_.t2),
                          toEnum<VramAccessCommand>(cycb0l_.t3),
                          is_normal_mode ? toEnum<VramAccessCommand>(cycb0u_.t4) : VramAccessCommand::no_access,
                          is_normal_mode ? toEnum<VramAccessCommand>(cycb0u_.t5) : VramAccessCommand::no_access,
                          is_normal_mode ? toEnum<VramAccessCommand>(cycb0u_.t6) : VramAccessCommand::no_access,
                          is_normal_mode ? toEnum<VramAccessCommand>(cycb0u_.t7) : VramAccessCommand::no_access};

    VramTiming bank_b1 = {toEnum<VramAccessCommand>(cycb1l_.t0),
                          toEnum<VramAccessCommand>(cycb1l_.t1),
                          toEnum<VramAccessCommand>(cycb1l_.t2),
                          toEnum<VramAccessCommand>(cycb1l_.t3),
                          is_normal_mode ? toEnum<VramAccessCommand>(cycb1u_.t4) : VramAccessCommand::no_access,
                          is_normal_mode ? toEnum<VramAccessCommand>(cycb1u_.t5) : VramAccessCommand::no_access,
                          is_normal_mode ? toEnum<VramAccessCommand>(cycb1u_.t6) : VramAccessCommand::no_access,
                          is_normal_mode ? toEnum<VramAccessCommand>(cycb1u_.t7) : VramAccessCommand::no_access};

    switch (command) {
        using enum VramAccessCommand;
        case nbg0_character_pattern_data_read: {
            if (toEnum<BitmapEnable>(chctla_.bitmap_enable_nbg0) == BitmapEnable::bitmap_format) {
                return getVramBitmapReads(bank_a0, bank_a1, bank_b0, bank_b1, command);
            }
            const auto cp_size = toEnum<CharacterSize>(chctla_.character_size_nbg0);
            return getVramCharacterPatternDataReads(bank_a0,
                                                    bank_a1,
                                                    bank_b0,
                                                    bank_b1,
                                                    command,
                                                    reduction,
                                                    is_normal_mode,
                                                    (cp_size == CharacterSize::two_by_two));
        }
        case nbg1_character_pattern_data_read: {
            if (toEnum<BitmapEnable>(chctla_.bitmap_enable_nbg1) == BitmapEnable::bitmap_format) {
                return getVramBitmapReads(bank_a0, bank_a1, bank_b0, bank_b1, command);
            }
            const auto cp_size = toEnum<CharacterSize>(chctla_.character_size_nbg1);
            return getVramCharacterPatternDataReads(bank_a0,
                                                    bank_a1,
                                                    bank_b0,
                                                    bank_b1,
                                                    command,
                                                    reduction,
                                                    is_normal_mode,
                                                    (cp_size == CharacterSize::two_by_two));
        }
        case nbg2_character_pattern_data_read: {
            const auto cp_size = toEnum<CharacterSize>(chctlb_.character_size_nbg2);
            return getVramCharacterPatternDataReads(bank_a0,
                                                    bank_a1,
                                                    bank_b0,
                                                    bank_b1,
                                                    command,
                                                    reduction,
                                                    is_normal_mode,
                                                    (cp_size == CharacterSize::two_by_two));
        }
        case nbg3_character_pattern_data_read: {
            const auto cp_size = toEnum<CharacterSize>(chctlb_.character_size_nbg3);
            return getVramCharacterPatternDataReads(bank_a0,
                                                    bank_a1,
                                                    bank_b0,
                                                    bank_b1,
                                                    command,
                                                    reduction,
                                                    is_normal_mode,
                                                    (cp_size == CharacterSize::two_by_two));
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
auto Vdp2::getVramBitmapReads(const VramTiming&       bank_a0,
                              const VramTiming&       bank_a1,
                              const VramTiming&       bank_b0,
                              const VramTiming&       bank_b1,
                              const VramAccessCommand command) -> u8 {
    auto bitmap_reads = std::count(bank_a0.begin(), bank_a0.end(), command);
    if (toEnum<VramMode>(ramctl_.vram_a_mode) == VramMode::partition_in_2_banks) {
        bitmap_reads += std::count(bank_a1.begin(), bank_a1.end(), command);
    }
    bitmap_reads += std::count(bank_b0.begin(), bank_b0.end(), command);
    if (toEnum<VramMode>(ramctl_.vram_b_mode) == VramMode::partition_in_2_banks) {
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
    if (toEnum<VramMode>(ramctl_.vram_a_mode) == VramMode::partition_in_2_banks
        && (toEnum<VramMode>(ramctl_.vram_b_mode) == VramMode::partition_in_2_banks)) {
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
auto Vdp2::calculateRequiredVramCharacterPatternReads(ReductionSetting r, CharacterColorNumber3Bits ccn) -> VramAccessNumber {
    switch (ccn) {
        using enum CharacterColorNumber3Bits;
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
auto Vdp2::calculateRequiredVramCharacterPatternReads(ReductionSetting r, CharacterColorNumber2Bits ccn) -> VramAccessNumber {
    switch (ccn) {
        using enum CharacterColorNumber2Bits;
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
auto Vdp2::calculateRequiredVramCharacterPatternReads(ReductionSetting r, CharacterColorNumber1Bit ccn) -> VramAccessNumber {
    switch (ccn) {
        using enum CharacterColorNumber1Bit;
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
auto Vdp2::getPatternNameFromCharacterPattern(const VramAccessCommand character_pattern) -> VramAccessCommand {
    switch (character_pattern) {
        using enum VramAccessCommand;
        case nbg0_character_pattern_data_read: {
            return VramAccessCommand::nbg0_pattern_name_read;
        }
        case nbg1_character_pattern_data_read: {
            return VramAccessCommand::nbg1_pattern_name_read;
        }
        case nbg2_character_pattern_data_read: {
            return VramAccessCommand::nbg2_pattern_name_read;
        }
        case nbg3_character_pattern_data_read: {
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
auto Vdp2::getVramCharacterPatternDataReads(const VramTiming&       bank_a0,
                                            const VramTiming&       bank_a1,
                                            const VramTiming&       bank_b0,
                                            const VramTiming&       bank_b1,
                                            const VramAccessCommand command,
                                            const ReductionSetting  reduction,
                                            const bool              is_screen_mode_normal,
                                            const bool              is_using_2_by_2_cp) -> u8 {
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

        // Timings limitations are disabled for now (cf https://github.com/rtoumazet/saturnin/issues/255)
        // setCharacterPatternLimitations(is_screen_mode_normal, is_using_2_by_2_cp, pnd_timing_access, allowed_cpd_timing);
        if (is_screen_mode_normal) {
            allowed_cpd_timing = std::array{true, true, true, true, true, true, true, true};
        } else {
            allowed_cpd_timing = std::array{true, true, true, true, false, false, false, false};
        }

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

void Vdp2::clearRenderData(const ScrollScreen s) {
    // std::vector<std::unique_ptr<video::BaseRenderingPart>>().swap(vdp2_parts_[toUnderlying(s)]);
    std::vector<video::Vdp2Part>().swap(vdp2_parts_[toUnderlying(s)]);
    //  vdp2_parts_[toUnderlying(s)].clear();
}

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

        clearRenderData(ScrollScreen::nbg3);
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
        using enum ScrollScreen;
        case nbg1: {
            return (nbg0_color_nb != ColorCount::rgb_16m);
        }
        case nbg2: {
            const auto colors_preventing_display = std::array{ColorCount::palette_2048, ColorCount::rgb_32k, ColorCount::rgb_16m};
            return std::none_of(colors_preventing_display.begin(),
                                colors_preventing_display.end(),
                                [&nbg0_color_nb](const ColorCount cc) { return cc == nbg0_color_nb; });
        }
        case nbg3: {
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

    const auto getCharacterColorNumber3Bits = [](const CharacterColorNumber3Bits c, const ScreenModeType t) {
        switch (c) {
            using enum CharacterColorNumber3Bits;
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
    const auto getCharacterColorNumber2Bits = [](const CharacterColorNumber2Bits c, const ScreenModeType t) {
        switch (c) {
            using enum CharacterColorNumber2Bits;
            case palette_16: return ColorCount::palette_16;
            case palette_256: return ColorCount::palette_256;
            case palette_2048: return ColorCount::palette_2048;
            case rgb_32k: {
                return (t == ScreenModeType::exclusive) ? ColorCount::rgb_16m : ColorCount::rgb_32k;
            }
            default: return ColorCount::not_allowed;
        }
    };
    const auto getCharacterColorNumber1Bit = [](const CharacterColorNumber1Bit c) {
        switch (c) {
            using enum CharacterColorNumber1Bit;
            case palette_16: return ColorCount::palette_16;
            case palette_256: return ColorCount::palette_256;
            default: return ColorCount::not_allowed;
        }
    };
    const auto getCharacterColorNumberRbg0 = [](const CharacterColorNumber3Bits c, const ScreenModeType t) {
        if (t == ScreenModeType::exclusive) { return ColorCount::cannot_display; }

        switch (c) {
            using enum CharacterColorNumber3Bits;
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
            using enum BitmapSize2Bits;
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

    // const auto setColorOffset
    //     = [&](ScrollScreenStatus& s, const ColorOffsetEnableBit enable_bit, const ColorOffsetSelectBit select_bit) {
    //           s.is_color_offset_enabled = (enable_bit == ColorOffsetEnableBit::enabled);
    //           s.color_offset            = {};
    //           if (s.is_color_offset_enabled) {
    //               if (select_bit == ColorOffsetSelectBit::use_color_offset_a) {
    //                   s.color_offset.as_s16.r = static_cast<s16>(coar_.raw);
    //                   s.color_offset.as_s16.g = static_cast<s16>(coag_.raw);
    //                   s.color_offset.as_s16.b = static_cast<s16>(coab_.raw);
    //               } else {
    //                   s.color_offset.as_s16.r = static_cast<s16>(cobr_.raw);
    //                   s.color_offset.as_s16.g = static_cast<s16>(cobg_.raw);
    //                   s.color_offset.as_s16.b = static_cast<s16>(cobb_.raw);
    //               }
    //               s.color_offset.as_float.r = static_cast<float>(s.color_offset.as_s16.r) / static_cast<float>(u16_max);
    //               s.color_offset.as_float.g = static_cast<float>(s.color_offset.as_s16.g) / static_cast<float>(u16_max);
    //               s.color_offset.as_float.b = static_cast<float>(s.color_offset.as_s16.b) / static_cast<float>(u16_max);
    //           }
    //       };

    auto& screen         = getScreen(s);
    screen.scroll_screen = s;

    switch (s) {
        using enum ScrollScreen;
        case nbg0:
            // Color RAM
            screen.color_ram_address_offset = getColorRamAddressOffset(craofa_.color_ram_address_offset_nbg0);

            // Transparency
            screen.is_transparency_code_valid = (toEnum<TransparentDisplayEnable>(bgon_.transparency_display_enable_nbg0)
                                                 == TransparentDisplayEnable::transparency_code_valid);

            // Priority
            screen.priority_number = prina_.nbg0;

            // Map
            screen.map_size   = map_size_nbg;
            screen.map_offset = mpofn_.map_offset_nbg0;

            // Plane
            screen.plane_size            = toEnum<PlaneSize>(plsz_.plane_size_nbg0);
            screen.plane_a_start_address = calculatePlaneStartAddress(s, mpabn0_.plane_a);
            screen.plane_b_start_address = calculatePlaneStartAddress(s, mpabn0_.plane_b);
            screen.plane_c_start_address = calculatePlaneStartAddress(s, mpcdn0_.plane_c);
            screen.plane_d_start_address = calculatePlaneStartAddress(s, mpcdn0_.plane_d);

            // Page
            screen.page_size = getPageSize(toEnum<PatternNameDataSize>(pncn0_.pattern_name_data_size),
                                           toEnum<CharacterSize>(chctla_.character_size_nbg0));

            // Pattern name data
            screen.pattern_name_data_size           = toEnum<PatternNameDataSize>(pncn0_.pattern_name_data_size);
            screen.character_number_supplement_mode = toEnum<CharacterNumberSupplementMode>(pncn0_.character_number_mode);
            screen.special_priority                 = static_cast<u8>(static_cast<bool>(pncn0_.special_priority));
            screen.special_color_calculation        = static_cast<u8>(static_cast<bool>(pncn0_.special_color_calculation));
            screen.supplementary_palette_number     = pncn0_.supplementary_palette_number;
            screen.supplementary_character_number   = pncn0_.supplementary_character_number;

            // Character pattern
            screen.character_pattern_size = toEnum<CharacterSize>(chctla_.character_size_nbg0);
            screen.character_color_number
                = getCharacterColorNumber3Bits(toEnum<CharacterColorNumber3Bits>(chctla_.character_color_number_nbg0),
                                               tv_screen_status_.screen_mode_type);
            screen.format = getScrollScreenFormat(toEnum<BitmapEnable>(chctla_.bitmap_enable_nbg0));

            // Cell
            screen.cell_size = cell_size * getDotSize(screen.character_color_number) / bits_in_a_byte;

            // Bitmap
            screen.bitmap_size           = getBitmapSize(toEnum<BitmapSize2Bits>(chctla_.bitmap_size_nbg0));
            screen.bitmap_palette_number = bmpna_.bitmap_palette_number_nbg0;
            // screen.bitmap_special_priority = static_cast<u8>(static_cast<bool>(bmpna_.bitmap_special_priority_nbg0));
            screen.bitmap_special_priority = static_cast<u8>(static_cast<bool>(bmpna_.bitmap_special_priority_nbg0));
            screen.bitmap_special_color_calculation
                = static_cast<u8>(static_cast<bool>(bmpna_.bitmap_special_color_calculation_nbg0));
            screen.bitmap_start_address = getBitmapStartAddress(screen.map_offset);

            // Scroll screen
            screen.screen_scroll_horizontal_integer    = scxin0_.integer;
            screen.screen_scroll_horizontal_fractional = static_cast<u8>(scxdn0_.fractional);
            screen.screen_scroll_vertical_integer      = scyin0_.integer;
            screen.screen_scroll_vertical_fractional   = static_cast<u8>(scydn0_.fractional);

            // Color offset
            screen.color_offset = getColorOffset(Layer::nbg0);

            break;
        case nbg1:
            // Color RAM
            screen.color_ram_address_offset = getColorRamAddressOffset(craofa_.color_ram_address_offset_nbg1);

            // Transparency
            screen.is_transparency_code_valid = (toEnum<TransparentDisplayEnable>(bgon_.transparency_display_enable_nbg1)
                                                 == TransparentDisplayEnable::transparency_code_valid);
            // Priority
            screen.priority_number = static_cast<u8>(prina_.nbg1);

            // Map
            screen.map_size   = map_size_nbg;
            screen.map_offset = mpofn_.map_offset_nbg1;

            // Plane
            screen.plane_size            = toEnum<PlaneSize>(plsz_.plane_size_nbg1);
            screen.plane_a_start_address = calculatePlaneStartAddress(s, mpabn1_.plane_a);
            screen.plane_b_start_address = calculatePlaneStartAddress(s, mpabn1_.plane_b);
            screen.plane_c_start_address = calculatePlaneStartAddress(s, mpcdn1_.plane_c);
            screen.plane_d_start_address = calculatePlaneStartAddress(s, mpcdn1_.plane_d);

            // Page
            screen.page_size = getPageSize(toEnum<PatternNameDataSize>(pncn1_.pattern_name_data_size),
                                           toEnum<CharacterSize>(chctla_.character_size_nbg1));

            // Pattern name data
            screen.pattern_name_data_size           = toEnum<PatternNameDataSize>(pncn1_.pattern_name_data_size);
            screen.character_number_supplement_mode = toEnum<CharacterNumberSupplementMode>(pncn1_.character_number_mode);
            screen.special_priority                 = static_cast<u8>(static_cast<bool>(pncn1_.special_priority));
            screen.special_color_calculation        = static_cast<u8>(static_cast<bool>(pncn1_.special_color_calculation));
            screen.supplementary_palette_number     = pncn1_.supplementary_palette_number;
            screen.supplementary_character_number   = pncn1_.supplementary_character_number;

            // Character pattern
            screen.character_pattern_size = toEnum<CharacterSize>(chctla_.character_size_nbg1);
            screen.character_color_number
                = getCharacterColorNumber2Bits(toEnum<CharacterColorNumber2Bits>(chctla_.character_color_number_nbg1),
                                               tv_screen_status_.screen_mode_type);
            screen.format = getScrollScreenFormat(toEnum<BitmapEnable>(chctla_.bitmap_enable_nbg1));

            // Cell
            screen.cell_size = cell_size * getDotSize(screen.character_color_number) / bits_in_a_byte;

            // Bitmap
            screen.bitmap_size             = getBitmapSize(toEnum<BitmapSize2Bits>(chctla_.bitmap_size_nbg1));
            screen.bitmap_palette_number   = static_cast<u8>(bmpna_.bitmap_palette_number_nbg1);
            screen.bitmap_special_priority = static_cast<u8>(static_cast<bool>(bmpna_.bitmap_special_priority_nbg1));
            screen.bitmap_special_color_calculation
                = static_cast<u8>(static_cast<bool>(bmpna_.bitmap_special_color_calculation_nbg1));
            screen.bitmap_start_address = getBitmapStartAddress(screen.map_offset);

            // Scroll screen
            screen.screen_scroll_horizontal_integer    = scxin1_.integer;
            screen.screen_scroll_horizontal_fractional = static_cast<u8>(scxdn1_.fractional);
            screen.screen_scroll_vertical_integer      = scyin1_.integer;
            screen.screen_scroll_vertical_fractional   = static_cast<u8>(scydn1_.fractional);

            // Color offset
            screen.color_offset = getColorOffset(Layer::nbg1);
            break;
        case nbg2:
            // Color RAM
            screen.color_ram_address_offset = getColorRamAddressOffset(static_cast<u8>(craofa_.color_ram_address_offset_nbg2));

            // Transparency
            screen.is_transparency_code_valid = (toEnum<TransparentDisplayEnable>(bgon_.transparency_display_enable_nbg2)
                                                 == TransparentDisplayEnable::transparency_code_valid);

            // Priority
            screen.priority_number = prinb_.nbg2;

            // Map
            screen.map_size   = map_size_nbg;
            screen.map_offset = static_cast<u8>(mpofn_.map_offset_nbg2);

            // Plane
            screen.plane_size            = toEnum<PlaneSize>(plsz_.plane_size_nbg2);
            screen.plane_a_start_address = calculatePlaneStartAddress(s, mpabn2_.plane_a);
            screen.plane_b_start_address = calculatePlaneStartAddress(s, mpabn2_.plane_b);
            screen.plane_c_start_address = calculatePlaneStartAddress(s, mpcdn2_.plane_c);
            screen.plane_d_start_address = calculatePlaneStartAddress(s, mpcdn2_.plane_d);

            // Page
            screen.page_size = getPageSize(toEnum<PatternNameDataSize>(pncn2_.pattern_name_data_size),
                                           toEnum<CharacterSize>(chctlb_.character_size_nbg2));

            // Pattern name data
            screen.pattern_name_data_size           = toEnum<PatternNameDataSize>(pncn2_.pattern_name_data_size);
            screen.character_number_supplement_mode = toEnum<CharacterNumberSupplementMode>(pncn2_.character_number_mode);
            screen.special_priority                 = static_cast<u8>(static_cast<bool>(pncn2_.special_priority));
            screen.special_color_calculation        = static_cast<u8>(static_cast<bool>(pncn2_.special_color_calculation));
            screen.supplementary_palette_number     = pncn2_.supplementary_palette_number;
            screen.supplementary_character_number   = pncn2_.supplementary_character_number;

            // Character pattern
            screen.character_pattern_size = toEnum<CharacterSize>(chctlb_.character_size_nbg2);
            screen.character_color_number
                = getCharacterColorNumber1Bit(toEnum<CharacterColorNumber1Bit>(chctlb_.character_color_number_nbg2));
            screen.bitmap_size          = BitmapSize::not_set;
            screen.bitmap_start_address = 0;

            // Cell
            screen.cell_size = cell_size * getDotSize(screen.character_color_number) / bits_in_a_byte;

            // Scroll screen
            screen.screen_scroll_horizontal_integer = scxn2_.integer;
            // if (screen.screen_scroll_horizontal_integer & 0x400) { screen.screen_scroll_horizontal_integer |= 0xFFFFF800; }
            screen.screen_scroll_vertical_integer = scyn2_.integer;
            // if (screen.screen_scroll_vertical_integer & 0x400) { screen.screen_scroll_vertical_integer |= 0xFFFFF800; }

            // Color offset
            screen.color_offset = getColorOffset(Layer::nbg2);
            break;

        case nbg3:
            // Color RAM
            screen.color_ram_address_offset = getColorRamAddressOffset(static_cast<u8>(craofa_.color_ram_address_offset_nbg3));

            // Transparency
            screen.is_transparency_code_valid = (toEnum<TransparentDisplayEnable>(bgon_.transparency_display_enable_nbg3)
                                                 == TransparentDisplayEnable::transparency_code_valid);

            // Priority
            screen.priority_number = static_cast<u8>(prinb_.nbg3);

            // Map
            screen.map_size   = map_size_nbg;
            screen.map_offset = static_cast<u8>(mpofn_.map_offset_nbg3);

            // Plane
            screen.plane_size            = toEnum<PlaneSize>(plsz_.plane_size_nbg3);
            screen.plane_a_start_address = calculatePlaneStartAddress(s, mpabn3_.plane_a);
            screen.plane_b_start_address = calculatePlaneStartAddress(s, mpabn3_.plane_b);
            screen.plane_c_start_address = calculatePlaneStartAddress(s, mpcdn3_.plane_c);
            screen.plane_d_start_address = calculatePlaneStartAddress(s, mpcdn3_.plane_d);

            // Page
            screen.page_size = getPageSize(toEnum<PatternNameDataSize>(pncn3_.pattern_name_data_size),
                                           toEnum<CharacterSize>(chctlb_.character_size_nbg3));

            // Pattern name data
            screen.pattern_name_data_size           = toEnum<PatternNameDataSize>(pncn3_.pattern_name_data_size);
            screen.character_number_supplement_mode = toEnum<CharacterNumberSupplementMode>(pncn3_.character_number_mode);
            screen.special_priority                 = static_cast<u8>(static_cast<bool>(pncn3_.special_priority));
            screen.special_color_calculation        = static_cast<u8>(static_cast<bool>(pncn3_.special_color_calculation));
            screen.supplementary_palette_number     = pncn3_.supplementary_palette_number;
            screen.supplementary_character_number   = pncn3_.supplementary_character_number;

            // Character pattern
            screen.character_pattern_size = toEnum<CharacterSize>(chctlb_.character_size_nbg3);
            screen.character_color_number
                = getCharacterColorNumber1Bit(toEnum<CharacterColorNumber1Bit>(chctlb_.character_color_number_nbg3));
            screen.bitmap_size          = BitmapSize::not_set;
            screen.bitmap_start_address = 0;

            // Cell
            screen.cell_size = cell_size * getDotSize(screen.character_color_number) / bits_in_a_byte;

            // Scroll screen
            screen.screen_scroll_horizontal_integer = scxn3_.integer;
            // if (screen.screen_scroll_horizontal_integer & 0x400) { screen.screen_scroll_horizontal_integer |= 0xFFFFF800; }
            screen.screen_scroll_vertical_integer = scyn3_.integer;
            // if (screen.screen_scroll_vertical_integer & 0x400) { screen.screen_scroll_vertical_integer |= 0xFFFFF800; }

            // Color offset
            screen.color_offset = getColorOffset(Layer::nbg3);
            break;

        case rbg0:
            // Color RAM
            screen.color_ram_address_offset = getColorRamAddressOffset(craofb_.color_ram_address_offset_rbg0);

            // Transparency
            screen.is_transparency_code_valid = (toEnum<TransparentDisplayEnable>(bgon_.transparency_display_enable_rbg0)
                                                 == TransparentDisplayEnable::transparency_code_valid);

            // Priority
            screen.priority_number = prir_.rbg0;

            // Map
            screen.map_size   = map_size_rbg;
            screen.map_offset = mpofr_.map_offset_rpa;

            // Plane
            screen.plane_size            = toEnum<PlaneSize>(plsz_.plane_size_rpa);
            screen.plane_a_start_address = calculatePlaneStartAddress(s, mpabra_.plane_a);
            screen.plane_b_start_address = calculatePlaneStartAddress(s, mpabra_.plane_b);
            screen.plane_c_start_address = calculatePlaneStartAddress(s, mpcdra_.plane_c);
            screen.plane_d_start_address = calculatePlaneStartAddress(s, mpcdra_.plane_d);
            screen.plane_e_start_address = calculatePlaneStartAddress(s, mpefra_.plane_e);
            screen.plane_f_start_address = calculatePlaneStartAddress(s, mpefra_.plane_f);
            screen.plane_g_start_address = calculatePlaneStartAddress(s, mpghra_.plane_g);
            screen.plane_h_start_address = calculatePlaneStartAddress(s, mpghra_.plane_h);
            screen.plane_i_start_address = calculatePlaneStartAddress(s, mpijra_.plane_i);
            screen.plane_j_start_address = calculatePlaneStartAddress(s, mpijra_.plane_j);
            screen.plane_k_start_address = calculatePlaneStartAddress(s, mpklra_.plane_k);
            screen.plane_l_start_address = calculatePlaneStartAddress(s, mpklra_.plane_l);
            screen.plane_m_start_address = calculatePlaneStartAddress(s, mpmnra_.plane_m);
            screen.plane_n_start_address = calculatePlaneStartAddress(s, mpmnra_.plane_n);
            screen.plane_o_start_address = calculatePlaneStartAddress(s, mpopra_.plane_o);
            screen.plane_p_start_address = calculatePlaneStartAddress(s, mpopra_.plane_p);

            // Page
            screen.page_size = getPageSize(toEnum<PatternNameDataSize>(pncr_.pattern_name_data_size),
                                           toEnum<CharacterSize>(chctlb_.character_size_rbg0));

            // Pattern name data
            screen.pattern_name_data_size           = toEnum<PatternNameDataSize>(pncr_.pattern_name_data_size);
            screen.character_number_supplement_mode = toEnum<CharacterNumberSupplementMode>(pncr_.character_number_mode);
            screen.special_priority                 = static_cast<u8>(static_cast<bool>(pncr_.special_priority));
            screen.special_color_calculation        = static_cast<u8>(static_cast<bool>(pncr_.special_color_calculation));
            screen.supplementary_palette_number     = pncr_.supplementary_palette_number;
            screen.supplementary_character_number   = pncr_.supplementary_character_number;

            // Character pattern
            screen.character_pattern_size = toEnum<CharacterSize>(chctlb_.character_size_rbg0);
            screen.character_color_number
                = getCharacterColorNumberRbg0(toEnum<CharacterColorNumber3Bits>(chctlb_.character_color_number_rbg0),
                                              tv_screen_status_.screen_mode_type);
            screen.format = getScrollScreenFormat(toEnum<BitmapEnable>(chctlb_.bitmap_enable_rbg0));

            // Cell
            screen.cell_size = cell_size * getDotSize(screen.character_color_number) / bits_in_a_byte;

            // Bitmap
            screen.bitmap_size = getBitmapSize(static_cast<BitmapSize2Bits>(toEnum<BitmapSize1Bit>(chctlb_.bitmap_size_rbg0)));
            screen.bitmap_palette_number   = bmpnb_.bitmap_palette_number_rbg0;
            screen.bitmap_special_priority = static_cast<u8>(static_cast<bool>(bmpnb_.bitmap_special_priority_rbg0));
            screen.bitmap_special_color_calculation
                = static_cast<u8>(static_cast<bool>(bmpnb_.bitmap_special_color_calculation_rbg0));
            screen.bitmap_start_address = getBitmapStartAddress(screen.map_offset);

            // Color offset
            screen.color_offset = getColorOffset(Layer::rbg0);
            break;

        case rbg1:
            // Color RAM
            screen.color_ram_address_offset = getColorRamAddressOffset(craofa_.color_ram_address_offset_nbg0);

            // Transparency
            screen.is_transparency_code_valid = (toEnum<TransparentDisplayEnable>(bgon_.transparency_display_enable_nbg0)
                                                 == TransparentDisplayEnable::transparency_code_valid);

            // Priority
            screen.priority_number = prina_.nbg0;

            // Map
            screen.map_size   = map_size_rbg;
            screen.map_offset = mpofr_.map_offset_rpb;

            // Plane
            screen.plane_size            = toEnum<PlaneSize>(plsz_.plane_size_rpb);
            screen.plane_a_start_address = calculatePlaneStartAddress(s, mpabrb_.plane_a);
            screen.plane_b_start_address = calculatePlaneStartAddress(s, mpabrb_.plane_b);
            screen.plane_c_start_address = calculatePlaneStartAddress(s, mpcdrb_.plane_c);
            screen.plane_d_start_address = calculatePlaneStartAddress(s, mpcdrb_.plane_d);
            screen.plane_e_start_address = calculatePlaneStartAddress(s, mpefrb_.plane_e);
            screen.plane_f_start_address = calculatePlaneStartAddress(s, mpefrb_.plane_f);
            screen.plane_g_start_address = calculatePlaneStartAddress(s, mpghrb_.plane_g);
            screen.plane_h_start_address = calculatePlaneStartAddress(s, mpghrb_.plane_h);
            screen.plane_i_start_address = calculatePlaneStartAddress(s, mpijrb_.plane_i);
            screen.plane_j_start_address = calculatePlaneStartAddress(s, mpijrb_.plane_j);
            screen.plane_k_start_address = calculatePlaneStartAddress(s, mpklrb_.plane_k);
            screen.plane_l_start_address = calculatePlaneStartAddress(s, mpklrb_.plane_l);
            screen.plane_m_start_address = calculatePlaneStartAddress(s, mpmnrb_.plane_m);
            screen.plane_n_start_address = calculatePlaneStartAddress(s, mpmnrb_.plane_n);
            screen.plane_o_start_address = calculatePlaneStartAddress(s, mpoprb_.plane_o);
            screen.plane_p_start_address = calculatePlaneStartAddress(s, mpoprb_.plane_p);

            // Page
            screen.page_size = getPageSize(toEnum<PatternNameDataSize>(pncn0_.pattern_name_data_size),
                                           toEnum<CharacterSize>(chctla_.character_size_nbg0));

            // Pattern name data
            screen.pattern_name_data_size           = toEnum<PatternNameDataSize>(pncn0_.pattern_name_data_size);
            screen.character_number_supplement_mode = toEnum<CharacterNumberSupplementMode>(pncn0_.character_number_mode);
            screen.special_priority                 = static_cast<u8>(static_cast<bool>(pncn0_.special_priority));
            screen.special_color_calculation        = static_cast<u8>(static_cast<bool>(pncn0_.special_color_calculation));
            screen.supplementary_palette_number     = pncn0_.supplementary_palette_number;
            screen.supplementary_character_number   = pncn0_.supplementary_character_number;

            // Character pattern
            screen.character_pattern_size = toEnum<CharacterSize>(chctla_.character_size_nbg0);
            screen.character_color_number
                = getCharacterColorNumber3Bits(toEnum<CharacterColorNumber3Bits>(chctla_.character_color_number_nbg0),
                                               tv_screen_status_.screen_mode_type);
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
    screen.character_pattern_screen_offset = [](const CharacterSize sz) {
        return (sz == CharacterSize::one_by_one) ? ScreenOffset{1, 1} : ScreenOffset{2, 2};
    }(screen.character_pattern_size);
    screen.page_screen_offset  = {cells_nb_64, cells_nb_64};
    screen.plane_screen_offset = [&](const PlaneSize sz) {
        switch (sz) {
            using enum PlaneSize;
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
            using enum PlaneSize;
            case size_1_by_1: plane_size = 1; break;
            case size_2_by_1: plane_size = 2; break;
            case size_2_by_2: plane_size = 4; break;
        }

        return cells_per_page * plane_size * screen.map_size;
    }();

    // Calculating the total width of the scroll screen, will be used for scrolling calculation.
    screen.total_screen_scroll_width = [&screen]() {
        constexpr auto pixels_per_cell      = 8;
        constexpr auto cells_per_page_width = 64;
        auto           plane_width          = u8{};
        switch (screen.plane_size) {
            using enum PlaneSize;
            case size_1_by_1: plane_width = 1; break;
            case size_2_by_1:
            case size_2_by_2: plane_width = 2; break;
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
        }

        return pixels_per_cell * cells_per_page_width * plane_width * map_width;
    }();

    // Calculating the total height of the scroll screen, will be used for scrolling calculation.
    screen.total_screen_scroll_height = [&screen]() {
        constexpr auto pixels_per_cell       = 8;
        constexpr auto cells_per_page_height = 64;
        auto           plane_height          = u8{};
        switch (screen.plane_size) {
            using enum PlaneSize;
            case size_1_by_1:
            case size_2_by_1: {
                plane_height = 1;
                break;
            }
            case size_2_by_2: {
                plane_height = 2;
                break;
            }
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
        }

        return pixels_per_cell * cells_per_page_height * plane_height * map_height;
    }();

    screen.scroll_offset_horizontal = [&]() {
        constexpr auto plane_width  = 512;
        auto           nb_of_planes = u8{};
        switch (screen.plane_size) {
            using enum PlaneSize;
            case size_1_by_1: nb_of_planes = 1; break;
            case size_2_by_1:
            case size_2_by_2: nb_of_planes = 2; break;
        }

        return screen.screen_scroll_horizontal_integer % (plane_width * nb_of_planes);
    }();

    screen.scroll_offset_vertical = [&]() {
        constexpr auto plane_height = 512;
        auto           nb_of_planes = u8{};
        switch (screen.plane_size) {
            using enum PlaneSize;
            case size_1_by_1:
            case size_2_by_1: nb_of_planes = 1; break;
            case size_2_by_2: nb_of_planes = 2; break;
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
    constexpr auto multiplier_800   = u32{0x800};
    constexpr auto multiplier_1000  = u32{0x1000};
    constexpr auto multiplier_2000  = u32{0x2000};
    constexpr auto multiplier_4000  = u32{0x4000};
    constexpr auto multiplier_8000  = u32{0x8000};
    constexpr auto multiplier_10000 = u32{0x10000};

    auto&      screen                 = getScreen(s);
    const auto is_vram_size_4mb       = toEnum<VramSize>(vrsize_.vram_size) == VramSize::size_4_mbits;
    auto       plane_size             = PlaneSize{};
    auto       pattern_name_data_size = PatternNameDataSize{};
    auto       character_size         = CharacterSize{};
    auto       start_address          = u32{screen.map_offset << displacement_6 | map_addr};

    switch (s) {
        using enum ScrollScreen;
        case nbg0:
            plane_size             = toEnum<PlaneSize>(plsz_.plane_size_nbg0);
            pattern_name_data_size = toEnum<PatternNameDataSize>(pncn0_.pattern_name_data_size);
            character_size         = toEnum<CharacterSize>(chctla_.character_size_nbg0);
            break;
        case nbg1:
            plane_size             = toEnum<PlaneSize>(plsz_.plane_size_nbg1);
            pattern_name_data_size = toEnum<PatternNameDataSize>(pncn1_.pattern_name_data_size);
            character_size         = toEnum<CharacterSize>(chctla_.character_size_nbg1);
            break;
        case nbg2:
            plane_size             = toEnum<PlaneSize>(plsz_.plane_size_nbg2);
            pattern_name_data_size = toEnum<PatternNameDataSize>(pncn2_.pattern_name_data_size);
            character_size         = toEnum<CharacterSize>(chctlb_.character_size_nbg2);
            break;
        case nbg3:
            plane_size             = toEnum<PlaneSize>(plsz_.plane_size_nbg3);
            pattern_name_data_size = toEnum<PatternNameDataSize>(pncn3_.pattern_name_data_size);
            character_size         = toEnum<CharacterSize>(chctlb_.character_size_nbg3);
            break;
        case rbg0:
            plane_size             = toEnum<PlaneSize>(plsz_.plane_size_rpa);
            pattern_name_data_size = toEnum<PatternNameDataSize>(pncr_.pattern_name_data_size);
            character_size         = toEnum<CharacterSize>(chctlb_.character_size_rbg0);
            break;
        case rbg1:
            plane_size             = toEnum<PlaneSize>(plsz_.plane_size_rpb);
            pattern_name_data_size = toEnum<PatternNameDataSize>(pncn0_.pattern_name_data_size);
            character_size         = toEnum<CharacterSize>(chctla_.character_size_nbg0);
            break;
        default: break;
    }

    auto mask       = u32{};
    auto multiplier = u32{};
    switch (plane_size) {
        using enum PlaneSize;
        case size_1_by_1:
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
        case size_2_by_1:
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
        case size_2_by_2:
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
    // if (isCacheDirty(s)) { discardCache(s); }
    // vdp2_parts_[util::toUnderlying(s)].clear();

    if (screen.format == ScrollScreenFormat::cell) {
        std::chrono::time_point<std::chrono::steady_clock> start_time = std::chrono::steady_clock::now();
        std::chrono::duration<double>                      elapsed_time{};

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
        if (std::none_of(rotation_screens.begin(), rotation_screens.end(), [&s](const ScrollScreen rss) { return rss == s; })) {
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

        // Unique addresses are handled
        for (const auto& [addr, offset] : start_addresses) {
            readPlaneData(screen, addr, offset);
        }

        if constexpr (use_concurrent_read_for_cells) {
            // Getting all cell data values in order to parallelize their read.

            constexpr auto chunk_size = u32{0x400};
            for (u32 current = 0; current < cell_data_to_process_.size(); current += chunk_size) {
                ThreadPool::pool_.push_task(&Vdp2::concurrentMultiReadCell,
                                            this,
                                            screen,
                                            cell_data_to_process_.begin() + current,
                                            cell_data_to_process_.begin() + current + chunk_size);
            }

            // for (const auto& cell : cell_data_to_process_) {
            //     ThreadPool::pool_
            //         .push_task(&Vdp2::concurrentReadCell, this, screen, cell.pnd, cell.cell_address, cell.screen_offset,
            //         cell.key);
            // }
            ThreadPool::pool_.wait_for_tasks();
        }

        auto res     = (std::chrono::duration_cast<std::chrono::microseconds>(elapsed_time)).count();
        elapsed_time = std::chrono::steady_clock::now() - start_time;
        res          = (std::chrono::duration_cast<std::chrono::microseconds>(elapsed_time)).count();
        core::Log::warning(Logger::vdp2, core::tr("Parallel read {}µs"), res);

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
    const auto      texture_size = texture_width * texture_height * 4;
    std::vector<u8> texture_data;
    texture_data.reserve(texture_size);
    const auto key = Texture::calculateKey(VdpType::vdp2,
                                           screen.bitmap_start_address,
                                           toUnderlying(screen.character_color_number),
                                           screen.bitmap_palette_number);

    if (Texture::isTextureLoadingNeeded(key)) {
        if (ram_status_.color_ram_mode == ColorRamMode::mode_2_rgb_8_bits_1024_colors) {
            // 32 bits access to color RAM
            switch (screen.character_color_number) {
                using enum ColorCount;
                case palette_16: {
                    break;
                }
                case palette_256: {
                    read256ColorsBitmapData<u32>(texture_data, screen);
                    break;
                }
                case palette_2048: {
                    break;
                }
                case rgb_32k: {
                    break;
                }
                case rgb_16m: {
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
                    break;
                }
                case palette_256: {
                    read256ColorsBitmapData<u16>(texture_data, screen);
                    break;
                }
                case palette_2048: {
                    break;
                }
                case rgb_32k: {
                    break;
                }
                case rgb_16m: {
                    break;
                }
                default: {
                    Log::warning(Logger::vdp2, tr("Character color number invalid !"));
                }
            }
        }
        Texture::storeTexture(Texture(VdpType::vdp2,
                                      screen.bitmap_start_address,
                                      toUnderlying(screen.character_color_number),
                                      screen.bitmap_palette_number,
                                      texture_data,
                                      texture_width,
                                      texture_height));
        modules_.opengl()->addOrUpdateTexture(key);
    }
    saveBitmap(screen, texture_data, texture_width, texture_height, key);
}

void Vdp2::saveBitmap(const ScrollScreenStatus& screen,
                      std::vector<u8>&          texture_data,
                      const u16                 width,
                      const u16                 height,
                      const size_t              key) {
    // auto pos = ScreenPos{0, 0};

    // auto p = Vdp2Part(key, width, height, screen.priority_number, screen.linked_layer);
    //  auto p = std::make_unique<Vdp2Part>(key, width, height);
    //  p.priority(screen.priority_number);
    //  vdp2_parts_[util::toUnderlying(screen.scroll_screen)].push_back(
    //     std::make_unique<Vdp2Part>(key, width, height, screen.priority_number));
    vdp2_parts_[util::toUnderlying(screen.scroll_screen)].emplace_back(key,
                                                                       width,
                                                                       height,
                                                                       screen.priority_number,
                                                                       screen.color_offset.as_float);
}

void Vdp2::readPlaneData(const ScrollScreenStatus& screen, const u32 plane_address, const ScreenOffset& plane_offset) {
    auto page_start_address = plane_address;
    switch (screen.plane_size) {
        using enum PlaneSize;
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
        using enum PatternNameDataEnum;
        case two_words: readPatternNameData = reinterpret_cast<PatterNameDataFunc>(&getPatternNameData2Words); break;
        case one_word_1_cell_16_colors_10_bits:
            readPatternNameData = reinterpret_cast<PatterNameDataFunc>(&getPatternNameData1Word1Cell16Colors10Bits);
            break;
        case one_word_1_cell_16_colors_12_bits:
            readPatternNameData = reinterpret_cast<PatterNameDataFunc>(&getPatternNameData1Word1Cell16Colors12Bits);
            break;
        case one_word_1_cell_over_16_colors_10_bits:
            readPatternNameData = reinterpret_cast<PatterNameDataFunc>(&getPatternNameData1Word1CellOver16Colors10Bits);
            break;
        case one_word_1_cell_over_16_colors_12_bits:
            readPatternNameData = reinterpret_cast<PatterNameDataFunc>(&getPatternNameData1Word1CellOver16Colors12Bits);
            break;
        case one_word_4_cells_16_colors_10_bits:
            readPatternNameData = reinterpret_cast<PatterNameDataFunc>(&getPatternNameData1Word4Cells16Colors10Bits);
            break;
        case one_word_4_cells_16_colors_12_bits:
            readPatternNameData = reinterpret_cast<PatterNameDataFunc>(&getPatternNameData1Word4Cells16Colors12Bits);
            break;
        case one_word_4_cells_over_16_colors_10_bits:
            readPatternNameData = reinterpret_cast<PatterNameDataFunc>(&getPatternNameData1Word4CellsOver16Colors10Bits);
            break;
        case one_word_4_cells_over_16_colors_12_bits:
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
    // if (pnd.character_number != 0x3000 && pnd.character_number != 0x3345 && pnd.character_number != 0x3741) __debugbreak();
    const auto character_number_address = pnd.character_number * character_pattern_boundary;
    if (screen.character_pattern_size == CharacterSize::one_by_one) {
        readCell(screen, pnd, character_number_address, cp_offset);
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
    const auto key
        = Texture::calculateKey(VdpType::vdp2, cell_address, toUnderlying(screen.character_color_number), pnd.palette_number);

    if constexpr (use_concurrent_read_for_cells) {
        cell_data_to_process_.emplace_back(pnd, cell_address, cell_offset, key);
    } else {
        if (Texture::isTextureLoadingNeeded(key)) {
            constexpr auto  texture_width  = u16{8};
            constexpr auto  texture_height = u16{8};
            constexpr auto  texture_size   = texture_width * texture_height * 4;
            std::vector<u8> texture_data;
            texture_data.reserve(texture_size);

            if (ram_status_.color_ram_mode == ColorRamMode::mode_2_rgb_8_bits_1024_colors) {
                // 32 bits access to color RAM
                switch (screen.character_color_number) {
                    using enum ColorCount;
                    case palette_16: {
                        read16ColorsCellData<u32>(texture_data, screen, pnd.palette_number, cell_address);
                        break;
                    }
                    case palette_256: {
                        read256ColorsCellData<u32>(texture_data, screen, pnd.palette_number, cell_address);
                        break;
                    }
                    case palette_2048: {
                        break;
                    }
                    case rgb_32k: {
                        break;
                    }
                    case rgb_16m: {
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
                        read16ColorsCellData<u16>(texture_data, screen, pnd.palette_number, cell_address);
                        break;
                    }
                    case palette_256: {
                        read256ColorsCellData<u16>(texture_data, screen, pnd.palette_number, cell_address);
                        break;
                    }
                    case palette_2048: {
                        break;
                    }
                    case rgb_32k: {
                        break;
                    }
                    case rgb_16m: {
                        break;
                    }
                    default: {
                        Log::warning(Logger::vdp2, tr("Character color number invalid !"));
                    }
                }
            }
            Texture::storeTexture(Texture(VdpType::vdp2,
                                          cell_address,
                                          toUnderlying(screen.character_color_number),
                                          pnd.palette_number,
                                          texture_data,
                                          texture_width,
                                          texture_height));
            modules_.opengl()->addOrUpdateTexture(key);
        }
        saveCell(screen, pnd, cell_address, cell_offset, key);
    }
}

void Vdp2::concurrentReadCell(const ScrollScreenStatus screen,
                              const PatternNameData    pnd,
                              const u32                cell_address,
                              const ScreenOffset       cell_offset,
                              const size_t             key) {
    if (Texture::isTextureLoadingNeeded(key)) {
        constexpr auto  texture_width  = u16{8};
        constexpr auto  texture_height = u16{8};
        constexpr auto  texture_size   = texture_width * texture_height * 4;
        std::vector<u8> texture_data;
        texture_data.reserve(texture_size);

        if (ram_status_.color_ram_mode == ColorRamMode::mode_2_rgb_8_bits_1024_colors) {
            // 32 bits access to color RAM
            switch (screen.character_color_number) {
                using enum ColorCount;
                case palette_16: {
                    read16ColorsCellData<u32>(texture_data, screen, pnd.palette_number, cell_address);
                    break;
                }
                case palette_256: {
                    read256ColorsCellData<u32>(texture_data, screen, pnd.palette_number, cell_address);
                    break;
                }
                case palette_2048: {
                    break;
                }
                case rgb_32k: {
                    break;
                }
                case rgb_16m: {
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
                    read16ColorsCellData<u16>(texture_data, screen, pnd.palette_number, cell_address);
                    break;
                }
                case palette_256: {
                    read256ColorsCellData<u16>(texture_data, screen, pnd.palette_number, cell_address);
                    break;
                }
                case palette_2048: {
                    break;
                }
                case rgb_32k: {
                    break;
                }
                case rgb_16m: {
                    break;
                }
                default: {
                    Log::warning(Logger::vdp2, tr("Character color number invalid !"));
                }
            }
        }
        Texture::storeTexture(Texture(VdpType::vdp2,
                                      cell_address,
                                      toUnderlying(screen.character_color_number),
                                      pnd.palette_number,
                                      texture_data,
                                      texture_width,
                                      texture_height));
        modules_.opengl()->addOrUpdateTexture(key);
    }
    saveCell(screen, pnd, cell_address, cell_offset, key);
}

void Vdp2::concurrentMultiReadCell(const ScrollScreenStatus              screen,
                                   const std::vector<CellData>::iterator start,
                                   const std::vector<CellData>::iterator end) {
    std::vector<Texture> textures;
    textures.reserve(end - start);

    for (auto it = start; it != end; ++it) {
        if (Texture::isTextureLoadingNeeded(it->key)) {
            constexpr auto  texture_width  = u16{8};
            constexpr auto  texture_height = u16{8};
            constexpr auto  texture_size   = texture_width * texture_height * 4;
            std::vector<u8> texture_data;
            texture_data.reserve(texture_size);

            if (ram_status_.color_ram_mode == ColorRamMode::mode_2_rgb_8_bits_1024_colors) {
                // 32 bits access to color RAM
                switch (screen.character_color_number) {
                    using enum ColorCount;
                    case palette_16: {
                        read16ColorsCellData<u32>(texture_data, screen, it->pnd.palette_number, it->cell_address);
                        break;
                    }
                    case palette_256: {
                        read256ColorsCellData<u32>(texture_data, screen, it->pnd.palette_number, it->cell_address);
                        break;
                    }
                    case palette_2048: {
                        break;
                    }
                    case rgb_32k: {
                        break;
                    }
                    case rgb_16m: {
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
                        read16ColorsCellData<u16>(texture_data, screen, it->pnd.palette_number, it->cell_address);
                        break;
                    }
                    case palette_256: {
                        read256ColorsCellData<u16>(texture_data, screen, it->pnd.palette_number, it->cell_address);
                        break;
                    }
                    case palette_2048: {
                        break;
                    }
                    case rgb_32k: {
                        break;
                    }
                    case rgb_16m: {
                        break;
                    }
                    default: {
                        Log::warning(Logger::vdp2, tr("Character color number invalid !"));
                    }
                }
            }

            modules_.opengl()->addOrUpdateTexture(it->key);

            textures.emplace_back(Texture(VdpType::vdp2,
                                          it->cell_address,
                                          toUnderlying(screen.character_color_number),
                                          it->pnd.palette_number,
                                          texture_data,
                                          texture_width,
                                          texture_height));
        }
        saveCell(screen, it->pnd, it->cell_address, it->screen_offset, it->key);
    }

    Texture::storeTextures(textures);
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

    pos.x -= screen.scroll_offset_horizontal;
    pos.y -= screen.scroll_offset_vertical;

    LockGuard lock(vdp2_parts_mutex_);
    vdp2_parts_[util::toUnderlying(screen.scroll_screen)].emplace_back(pnd,
                                                                       pos,
                                                                       key,
                                                                       screen.priority_number,
                                                                       screen.color_offset.as_float);
}

auto Vdp2::getColorRamAddressOffset(const u8 register_offset) -> u16 {
    constexpr auto color_size_2_bytes = u8{2};
    constexpr auto color_size_4_bytes = u8{4};
    auto           color_size         = u8{};
    constexpr auto mask_2_bits        = u8{0x3};
    constexpr auto mask_3_bits        = u8{0x7};
    auto           register_mask      = u8{};
    switch (ram_status_.color_ram_mode) {
        using enum ColorRamMode;
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
    return ((register_offset & register_mask) << displacement_8) * color_size;
};

//--------------------------------------------------------------------------------------------------------------
// CACHE methods
//--------------------------------------------------------------------------------------------------------------

void Vdp2::resetCacheState() {
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

void Vdp2::discardCache(const ScrollScreen screen) {
    // 1) Textures used by the vdp2 parts of the screen are discarded
    Texture::discardCache(modules_.opengl(), VdpType::vdp2);

    // 2) Vdp2 parts are deleted
    // clearRenderData(screen);
}
//--------------------------------------------------------------------------------------------------------------
// Free functions
//--------------------------------------------------------------------------------------------------------------

auto getPatternNameData2Words(const u32 data, [[maybe_unused]] const ScrollScreenStatus& screen) -> PatternNameData {
    auto pattern_name_data                      = PatternNameData{};
    auto reg                                    = PatternNameData2Words{data};
    pattern_name_data.character_number          = reg.character_number;
    pattern_name_data.palette_number            = reg.palette_number;
    pattern_name_data.special_color_calculation = static_cast<u8>(static_cast<bool>(reg.special_color_calculation));
    pattern_name_data.special_priority          = static_cast<u8>(static_cast<bool>(reg.special_priority));
    pattern_name_data.is_horizontally_flipped   = static_cast<bool>(reg.horizontal_flip);
    pattern_name_data.is_vertically_flipped     = static_cast<bool>(reg.vertical_flip);
    return pattern_name_data;
};

auto getPatternNameData1Word1Cell16Colors10Bits(const u32 data, const ScrollScreenStatus& screen) -> PatternNameData {
    auto pattern_name_data = PatternNameData{};
    auto reg               = PatternNameData1Word1Cell16Colors10Bits{data};

    constexpr auto cn_disp             = u8{10};
    pattern_name_data.character_number = (screen.supplementary_character_number << cn_disp);
    pattern_name_data.character_number |= reg.character_number;

    constexpr auto pn_disp           = u8{4};
    pattern_name_data.palette_number = (screen.supplementary_palette_number << pn_disp);
    pattern_name_data.palette_number |= reg.palette_number;

    pattern_name_data.special_color_calculation = screen.special_color_calculation;
    pattern_name_data.special_priority          = screen.special_priority;
    pattern_name_data.is_horizontally_flipped   = static_cast<bool>(reg.horizontal_flip);
    pattern_name_data.is_vertically_flipped     = static_cast<bool>(reg.vertical_flip);

    return pattern_name_data;
};

auto getPatternNameData1Word1Cell16Colors12Bits(const u32 data, const ScrollScreenStatus& screen) -> PatternNameData {
    auto pattern_name_data = PatternNameData{};
    auto reg               = PatternNameData1Word1Cell16Colors12Bits{data};

    constexpr auto cn_disp             = u8{10};
    constexpr auto cn_mask             = u8{0x1C};
    pattern_name_data.character_number = ((screen.supplementary_character_number & cn_mask) << cn_disp);
    pattern_name_data.character_number |= reg.character_number;

    constexpr auto pn_disp           = u8{4};
    pattern_name_data.palette_number = (screen.supplementary_palette_number << pn_disp);
    pattern_name_data.palette_number |= reg.palette_number;

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
    pattern_name_data.character_number = (reg.palette_number << cn_disp);
    pattern_name_data.character_number |= reg.character_number;

    constexpr auto pn_disp           = u8{8};
    pattern_name_data.palette_number = (screen.supplementary_palette_number << pn_disp);

    pattern_name_data.special_color_calculation = screen.special_color_calculation;
    pattern_name_data.special_priority          = screen.special_priority;
    pattern_name_data.is_horizontally_flipped   = static_cast<bool>(reg.horizontal_flip);
    pattern_name_data.is_vertically_flipped     = static_cast<bool>(reg.vertical_flip);

    return pattern_name_data;
};

auto getPatternNameData1Word1CellOver16Colors12Bits(const u32 data, const ScrollScreenStatus& screen) -> PatternNameData {
    auto pattern_name_data = PatternNameData{};
    auto reg               = PatternNameData1Word1CellOver16Colors12Bits{data};

    constexpr auto cn_disp             = u8{10};
    constexpr auto cn_mask             = u8{0x1C};
    pattern_name_data.character_number = ((screen.supplementary_character_number & cn_mask) << cn_disp);
    pattern_name_data.character_number |= reg.character_number;

    constexpr auto pn_disp           = u8{8};
    pattern_name_data.palette_number = (reg.palette_number << pn_disp);

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
    pattern_name_data.character_number |= (reg.character_number << cn_disp_2);
    constexpr auto cn_mask_2 = u8{0x3};
    pattern_name_data.character_number |= (screen.supplementary_character_number & cn_mask_2);

    constexpr auto pn_disp           = u8{4};
    pattern_name_data.palette_number = (screen.supplementary_palette_number << pn_disp);
    pattern_name_data.palette_number |= reg.palette_number;

    pattern_name_data.special_color_calculation = screen.special_color_calculation;
    pattern_name_data.special_priority          = screen.special_priority;
    pattern_name_data.is_horizontally_flipped   = static_cast<bool>(reg.horizontal_flip);
    pattern_name_data.is_vertically_flipped     = static_cast<bool>(reg.vertical_flip);

    return pattern_name_data;
};

auto getPatternNameData1Word4Cells16Colors12Bits(const u32 data, const ScrollScreenStatus& screen) -> PatternNameData {
    auto pattern_name_data = PatternNameData{};
    auto reg               = PatternNameData1Word4Cells16Colors12Bits{data};

    constexpr auto cn_disp_1           = u8{10};
    constexpr auto cn_mask_1           = u8{0x10};
    pattern_name_data.character_number = (screen.supplementary_character_number & cn_mask_1) << cn_disp_1;
    constexpr auto cn_disp_2           = u8{2};
    pattern_name_data.character_number |= (reg.character_number << cn_disp_2);
    constexpr auto cn_mask_2 = u8{0x3};
    pattern_name_data.character_number |= (screen.supplementary_character_number & cn_mask_2);

    constexpr auto pn_disp           = u8{4};
    pattern_name_data.palette_number = (screen.supplementary_palette_number << pn_disp);
    pattern_name_data.palette_number |= reg.palette_number;

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
    pattern_name_data.character_number |= (reg.character_number << cn_disp_2);
    constexpr auto cn_mask_2 = u8{0x3};
    pattern_name_data.character_number |= (screen.supplementary_character_number & cn_mask_2);

    constexpr auto pn_disp           = u8{8};
    pattern_name_data.palette_number = (reg.palette_number << pn_disp);

    pattern_name_data.special_color_calculation = screen.special_color_calculation;
    pattern_name_data.special_priority          = screen.special_priority;
    pattern_name_data.is_horizontally_flipped   = static_cast<bool>(reg.horizontal_flip);
    pattern_name_data.is_vertically_flipped     = static_cast<bool>(reg.vertical_flip);

    return pattern_name_data;
};

auto getPatternNameData1Word4CellsOver16Colors12Bits(const u32 data, const ScrollScreenStatus& screen) -> PatternNameData {
    auto pattern_name_data = PatternNameData{};
    auto reg               = PatternNameData1Word4CellsOver16Colors12Bits{data};

    constexpr auto cn_disp_1           = u8{10};
    constexpr auto cn_mask_1           = u8{0x10};
    pattern_name_data.character_number = (screen.supplementary_character_number & cn_mask_1) << cn_disp_1;
    constexpr auto cn_disp_2           = u8{2};
    pattern_name_data.character_number |= (reg.character_number << cn_disp_2);
    constexpr auto cn_mask_2 = u8{0x3};
    pattern_name_data.character_number |= (screen.supplementary_character_number & cn_mask_2);

    constexpr auto pn_disp           = u8{8};
    pattern_name_data.palette_number = (reg.palette_number << pn_disp);

    pattern_name_data.special_color_calculation = screen.special_color_calculation;
    pattern_name_data.special_priority          = screen.special_priority;
    pattern_name_data.is_horizontally_flipped   = false;
    pattern_name_data.is_vertically_flipped     = false;

    return pattern_name_data;
};

} // namespace saturnin::video
