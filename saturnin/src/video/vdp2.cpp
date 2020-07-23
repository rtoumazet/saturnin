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

#include "vdp2.h"
#include "../config.h"
#include "../emulator_context.h"
#include "../log.h"
#include "../smpc.h"
#include "../interrupt_sources.h"

namespace saturnin::video {
namespace interrupt_source = core::interrupt_source;
using namespace vdp2_register_address;

using core::Config;
using core::EmulatorContext;
using core::Log;
using core::Smpc;
using core::StartingFactorSelect;
using core::tr;

void Vdp2::initialize() {
    initializeRegisterNameMap();

    std::string ts = emulator_context_->config()->readValue(core::AccessKeys::cfg_rendering_tv_standard);
    switch (Config::tv_standard[ts]) {
        case video::TvStandard::pal: tvstat_.set(ScreenStatus::tv_standard_flag, TvStandardFlag::pal_standard); break;
        case video::TvStandard::ntsc: tvstat_.set(ScreenStatus::tv_standard_flag, TvStandardFlag::ntsc_standard); break;
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

            Log::debug("vdp2", tr("VBlankIn interrupt request"));
            emulator_context_->scu()->generateInterrupt(interrupt_source::v_blank_in);
            emulator_context_->scu()->sendStartFactor(StartingFactorSelect::v_blank_in);
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

        Log::debug("vdp2", tr("VBlankOut interrupt request"));
        emulator_context_->scu()->generateInterrupt(interrupt_source::v_blank_out);
        emulator_context_->scu()->sendStartFactor(StartingFactorSelect::v_blank_out);

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

            // Log::debug("vdp2", tr("HBlankIn interrupt request"));
            emulator_context_->scu()->generateInterrupt(interrupt_source::h_blank_in);
            emulator_context_->scu()->sendStartFactor(StartingFactorSelect::h_blank_in);

            timer_0_counter_++;

            if (timer_0_counter_ == emulator_context_->scu()->getTimer0CompareValue()) {
                emulator_context_->scu()->generateInterrupt(interrupt_source::timer_0);
                emulator_context_->scu()->sendStartFactor(StartingFactorSelect::timer_0);
            }
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
        case pattern_name_control_rbg0: return pncnr_.toU16();
        case plane_size: return pnsz_.toU16();
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

        default: core::Log::warning("vdp2", core::tr("Unimplemented register read {:#010x}"), addr);
    }

    return 0;
}

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
        case pattern_name_control_rbg0: pncnr_.set(bits_0_15, data); break;
        case plane_size: pnsz_.set(bits_0_15, data); break;
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
        default: core::Log::warning("vdp2", core::tr("Unimplemented register write {:#010x}"), addr);
    }
}

void Vdp2::write32(const u32 addr, const u32 data) {
    u16 h = (data >> displacement_16);
    u16 l = (data & bitmask_FFFF);
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
            pncnr_.set(bits_0_15, h);
            pnsz_.set(bits_0_15, l);
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
        default: core::Log::warning("vdp2", core::tr("Unimplemented register write {:#010x}"), addr);
    }
}

auto Vdp2::getRegisters() const -> const AddressToNameMap& { return address_to_name_; };

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

void Vdp2::onSystemClockUpdate() { calculateDisplayDuration(); }

void Vdp2::calculateDisplayDuration() {
    // - A full frame vertical resolution is :
    //      - 262.5 lines for NTSC
    //      - 312.5 for PAL

    constexpr u16 lines_nb_224{224};
    constexpr u16 lines_nb_240{240};
    constexpr u16 lines_nb_256{256};

    std::string ts = emulator_context_->config()->readValue(core::AccessKeys::cfg_rendering_tv_standard);
    switch (Config::tv_standard[ts]) {
        case video::TvStandard::pal: {
            const seconds frame_duration{1.0 / 50.0};
            cycles_per_frame_ = emulator_context_->smpc()->calculateCyclesNumber(frame_duration);

            constexpr u16 total_lines{313};
            u16           visible_lines{};
            switch (tvmd_.get(TvScreenMode::vertical_resolution)) {
                case VerticalResolution::lines_nb_224: visible_lines = lines_nb_224; break;
                case VerticalResolution::lines_nb_240: visible_lines = lines_nb_240; break;
                case VerticalResolution::lines_nb_256: visible_lines = lines_nb_256; break;
            }
            const u16 vblank_lines{static_cast<u16>(total_lines - visible_lines)};
            cycles_per_vblank_  = vblank_lines * cycles_per_frame_ / total_lines;
            cycles_per_vactive_ = cycles_per_frame_ - cycles_per_vblank_;

            const micro pal_total_line_duration{64};
            const micro pal_hblank_duration{12};
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
            const seconds frame_duration{1.0 / 60.0};
            cycles_per_frame_ = emulator_context_->smpc()->calculateCyclesNumber(frame_duration);

            constexpr u16 total_lines{263};
            u16           visible_lines{};
            switch (tvmd_.get(TvScreenMode::vertical_resolution)) {
                case VerticalResolution::lines_nb_224: visible_lines = lines_nb_224; break;
                case VerticalResolution::lines_nb_240: visible_lines = lines_nb_240; break;
            }
            const u16 vblank_lines{static_cast<u16>(total_lines - visible_lines)};
            cycles_per_vblank_  = vblank_lines * cycles_per_frame_ / total_lines;
            cycles_per_vactive_ = cycles_per_frame_ - cycles_per_vblank_;

            const micro ntsc_total_line_duration{63.5};
            const micro ntsc_hblank_duration{10.9};
            calculateLineDuration(ntsc_total_line_duration, ntsc_hblank_duration);
            break;
        }
    }
}

void Vdp2::calculateLineDuration(const micro& total_line_duration, const micro& hblank_duration) {
    const auto active_line_duration{total_line_duration - hblank_duration};
    cycles_per_hblank_  = emulator_context_->smpc()->calculateCyclesNumber(hblank_duration);
    cycles_per_hactive_ = emulator_context_->smpc()->calculateCyclesNumber(active_line_duration);
    cycles_per_line_    = cycles_per_hactive_ + cycles_per_hblank_;
}

auto Vdp2::isScreenDisplayed(ScrollScreen s) -> bool {
    u8 pattern_data_reads_required{};
    u8 character_pattern_data_reads_required{};
    u8 bitmap_pattern_data_reads_required{};

    // First check to ensure scroll screen must be displayed. If the screen cannot display, no vram access will be performed.
    switch (s) {
        case ScrollScreen::nbg0:
            if (bgon_.get(ScreenDisplayEnable::screen_display_enable_nbg0) == ScreenDisplayEnableBit::cannot_display)
                return false;

            // Pattern name data reads depend on the reduction setting of the screen
            pattern_data_reads_required = 1;
            if (zmctl_.get(ReductionEnable::zoom_quarter_nbg0) == ZoomQuarter::up_to_one_quarter) {
                pattern_data_reads_required = 4;
            } else {
                if (zmctl_.get(ReductionEnable::zoom_half_nbg0) == ZoomHalf::up_to_one_half) { pattern_data_reads_required = 2; }
            }

            // Character / Bitmap pattern data reads depend on the reduction setting and the number of colors
            if (chctla_.get(CharacterControlA::bitmap_enable_nbg0) == BitmapEnable::bitmap_format) {
                // Needs only bitmap pattern data.
                bitmap_pattern_data_reads_required;
                switch (chctla_.get(CharacterControlA::character_color_number_nbg0)) {
                    case CharacterColorNumber3bits::palette_16: break;
                    case CharacterColorNumber3bits::palette_256: break;
                    case CharacterColorNumber3bits::palette_2048: break;
                    case CharacterColorNumber3bits::rgb_32k: break;
                    case CharacterColorNumber3bits::rgb_16m: break;
                }
            } else {
                // Needs character pattern data and pattern name data.
                character_pattern_data_reads_required;

                // Character pattern data
            }

            break;
        case ScrollScreen::nbg1:
            if (bgon_.get(ScreenDisplayEnable::screen_display_enable_nbg1) == ScreenDisplayEnableBit::cannot_display)
                return false;
            break;
        case ScrollScreen::nbg2:
            if (bgon_.get(ScreenDisplayEnable::screen_display_enable_nbg2) == ScreenDisplayEnableBit::cannot_display)
                return false;
            break;
        case ScrollScreen::nbg3:
            if (bgon_.get(ScreenDisplayEnable::screen_display_enable_nbg3) == ScreenDisplayEnableBit::cannot_display)
                return false;
            break;
        case ScrollScreen::rbg0:
            if (bgon_.get(ScreenDisplayEnable::screen_display_enable_rbg0) == ScreenDisplayEnableBit::cannot_display)
                return false;
            break;
        case ScrollScreen::rbg1:
            if (bgon_.get(ScreenDisplayEnable::screen_display_enable_rbg1) == ScreenDisplayEnableBit::cannot_display)
                return false;
            break;
    }

    if (ramctl_.get(RamControl::vram_a_mode_) == VramMode::no_partition) {}
    if (ramctl_.get(RamControl::vram_b_mode_) == VramMode::no_partition) {}

    // 8 timings (T0 to T7) are available for each bank in normal mode during 1 display cycle, only 4 (T0 to T3) in hires
    // or exclusive monitor mode.

    // Pattern name data read during 1 cycle can access 2 banks max, one being A0 or B0, the other being A1 or B1.

    // ReductionEnable zmctl_

    return false;
}

} // namespace saturnin::video
