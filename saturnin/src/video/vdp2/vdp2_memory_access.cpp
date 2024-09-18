//
// vdp2_memory_access.cpp
// Saturnin
//
// Copyright (c) 2024 Renaud Toumazet
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
#include <saturnin/src/video/vdp2/vdp2_registers.h>

namespace saturnin::video {

using namespace vdp2_register_address;
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

} // namespace saturnin::video
