//
// vdp2_registers.h
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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file	vdp2_registers.h
///
/// \brief	Declares VDP2 registers and related enums.
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../emulator_defs.h"

namespace saturnin::video {
namespace register_address {
constexpr u32 tv_screen_mode                           = {0x25f80000};
constexpr u32 external_signal_enable                   = {0x25f80002};
constexpr u32 screen_status                            = {0x25f80004};
constexpr u32 vram_size                                = {0x25f80006};
constexpr u32 h_counter                                = {0x25f80008};
constexpr u32 v_counter                                = {0x25f8000a};
constexpr u32 ram_control                              = {0x25f8000e};
constexpr u32 vram_cycle_pattern_bank_a0_lower         = {0x25f80010};
constexpr u32 vram_cycle_pattern_bank_a0_upper         = {0x25f80012};
constexpr u32 vram_cycle_pattern_bank_a1_lower         = {0x25f80014};
constexpr u32 vram_cycle_pattern_bank_a1_upper         = {0x25f80016};
constexpr u32 vram_cycle_pattern_bank_b0_lower         = {0x25f80018};
constexpr u32 vram_cycle_pattern_bank_b0_upper         = {0x25f8001a};
constexpr u32 vram_cycle_pattern_bank_b1_lower         = {0x25f8001c};
constexpr u32 vram_cycle_pattern_bank_b1_upper         = {0x25f8001e};
constexpr u32 screen_display_enable                    = {0x25f80020};
constexpr u32 mosaic_control                           = {0x25f80022};
constexpr u32 special_function_code_select             = {0x25f80024};
constexpr u32 special_function_code                    = {0x25f80026};
constexpr u32 character_control_a                      = {0x25f80028};
constexpr u32 character_control_b                      = {0x25f8002a};
constexpr u32 bitmap_palette_number_a                  = {0x25f8002c};
constexpr u32 bitmap_palette_number_b                  = {0x25f8002e};
constexpr u32 pattern_name_control_nbg0                = {0x25f80030};
constexpr u32 pattern_name_control_nbg1                = {0x25f80032};
constexpr u32 pattern_name_control_nbg2                = {0x25f80034};
constexpr u32 pattern_name_control_nbg3                = {0x25f80036};
constexpr u32 pattern_name_control_rbg0                = {0x25f80038};
constexpr u32 plane_size                               = {0x25f8003a};
constexpr u32 map_offset_n                             = {0x25f8003c};
constexpr u32 map_offset_r                             = {0x25f8003e};
constexpr u32 map_nbg0_plane_a_b                       = {0x25f80040};
constexpr u32 map_nbg0_plane_c_d                       = {0x25f80042};
constexpr u32 map_nbg1_plane_a_b                       = {0x25f80044};
constexpr u32 map_nbg1_plane_c_d                       = {0x25f80046};
constexpr u32 map_nbg2_plane_a_b                       = {0x25f80048};
constexpr u32 map_nbg2_plane_c_d                       = {0x25f8004a};
constexpr u32 map_nbg3_plane_a_b                       = {0x25f8004c};
constexpr u32 map_nbg3_plane_c_d                       = {0x25f8004e};
constexpr u32 map_rotation_parameter_a_plane_a_b       = {0x25f80050};
constexpr u32 map_rotation_parameter_a_plane_c_d       = {0x25f80052};
constexpr u32 map_rotation_parameter_a_plane_e_f       = {0x25f80054};
constexpr u32 map_rotation_parameter_a_plane_g_h       = {0x25f80056};
constexpr u32 map_rotation_parameter_a_plane_i_j       = {0x25f80058};
constexpr u32 map_rotation_parameter_a_plane_k_l       = {0x25f8005a};
constexpr u32 map_rotation_parameter_a_plane_m_n       = {0x25f8005c};
constexpr u32 map_rotation_parameter_a_plane_o_p       = {0x25f8005e};
constexpr u32 map_rotation_parameter_b_plane_a_b       = {0x25f80060};
constexpr u32 map_rotation_parameter_b_plane_c_d       = {0x25f80062};
constexpr u32 map_rotation_parameter_b_plane_e_f       = {0x25f80064};
constexpr u32 map_rotation_parameter_b_plane_g_h       = {0x25f80066};
constexpr u32 map_rotation_parameter_b_plane_i_j       = {0x25f80068};
constexpr u32 map_rotation_parameter_b_plane_k_l       = {0x25f8006a};
constexpr u32 map_rotation_parameter_b_plane_m_n       = {0x25f8006c};
constexpr u32 map_rotation_parameter_b_plane_o_p       = {0x25f8006e};
constexpr u32 screen_scroll_value_nbg0_h_int_part      = {0x25f80070};
constexpr u32 screen_scroll_value_nbg0_h_fract_part    = {0x25f80072};
constexpr u32 screen_scroll_value_nbg0_v_int_part      = {0x25f80074};
constexpr u32 screen_scroll_value_nbg0_v_fract_part    = {0x25f80076};
constexpr u32 coordinate_increment_nbg0_h_int_part     = {0x25f80078};
constexpr u32 coordinate_increment_nbg0_h_fract_part   = {0x25f8007a};
constexpr u32 coordinate_increment_nbg0_v_int_part     = {0x25f8007c};
constexpr u32 coordinate_increment_nbg0_v_fract_part   = {0x25f8007e};
constexpr u32 screen_scroll_value_nbg1_h_int_part      = {0x25f80080};
constexpr u32 screen_scroll_value_nbg1_h_fract_part    = {0x25f80082};
constexpr u32 screen_scroll_value_nbg1_v_int_part      = {0x25f80084};
constexpr u32 screen_scroll_value_nbg1_v_fract_part    = {0x25f80086};
constexpr u32 coordinate_increment_nbg1_h_int_part     = {0x25f80088};
constexpr u32 coordinate_increment_nbg1_h_fract_part   = {0x25f8008a};
constexpr u32 coordinate_increment_nbg1_v_int_part     = {0x25f8008c};
constexpr u32 coordinate_increment_nbg1_v_fract_part   = {0x25f8008e};
constexpr u32 screen_scroll_value_nbg2_h               = {0x25f80090};
constexpr u32 screen_scroll_value_nbg2_v               = {0x25f80092};
constexpr u32 screen_scroll_value_nbg3_h               = {0x25f80094};
constexpr u32 screen_scroll_value_nbg3_v               = {0x25f80096};
constexpr u32 reduction_enable                         = {0x25f80098};
constexpr u32 line_and_vertical_cell_scroll_control    = {0x25f8009a};
constexpr u32 vertical_cell_scroll_table_address_upper = {0x25f8009c};
constexpr u32 vertical_cell_scroll_table_address_lower = {0x25f8009e};
constexpr u32 line_scroll_table_address_nbg0_upper     = {0x25f800a0};
constexpr u32 line_scroll_table_address_nbg0_lower     = {0x25f800a2};
constexpr u32 line_scroll_table_address_nbg1_upper     = {0x25f800a4};
constexpr u32 line_scroll_table_address_nbg1_lower     = {0x25f800a6};
constexpr u32 line_color_screen_table_upper            = {0x25f800a8};
constexpr u32 line_color_screen_table_lower            = {0x25f800aa};
constexpr u32 back_screen_table_upper                  = {0x25f800ac};
constexpr u32 back_screen_table_lower                  = {0x25f800ae};
constexpr u32 rotation_parameter_mode                  = {0x25f800b0};
constexpr u32 rotation_parameter_read_control          = {0x25f800b2};
constexpr u32 coefficient_table_control                = {0x25f800b4};
constexpr u32 coefficient_table_address_offset         = {0x25f800b6};
constexpr u32 screen_over_pattern_name_a               = {0x25f800b8};
constexpr u32 screen_over_pattern_name_b               = {0x25f800ba};
constexpr u32 rotation_parameter_table_address_upper   = {0x25f800bc};
constexpr u32 rotation_parameter_table_address_lower   = {0x25f800be};
constexpr u32 window_position_w0_h_start_point         = {0x25f800c0};
constexpr u32 window_position_w0_v_start_point         = {0x25f800c2};
constexpr u32 window_position_w0_h_end_point           = {0x25f800c4};
constexpr u32 window_position_w0_v_end_point           = {0x25f800c6};
constexpr u32 window_position_w1_h_start_point         = {0x25f800c8};
constexpr u32 window_position_w1_v_start_point         = {0x25f800ca};
constexpr u32 window_position_w1_h_end_point           = {0x25f800cc};
constexpr u32 window_position_w1_v_end_point           = {0x25f800ce};
constexpr u32 window_control_a                         = {0x25f800d0};
constexpr u32 window_control_b                         = {0x25f800d2};
constexpr u32 window_control_c                         = {0x25f800d4};
constexpr u32 window_control_d                         = {0x25f800d6};
constexpr u32 line_window_table_address_w0_upper       = {0x25f800d8};
constexpr u32 line_window_table_address_w0_lower       = {0x25f800da};
constexpr u32 line_window_table_address_w1_upper       = {0x25f800dc};
constexpr u32 line_window_table_address_w1_lower       = {0x25f800de};
constexpr u32 sprite_control                           = {0x25f800e0};
constexpr u32 shadow_control                           = {0x25f800e2};
constexpr u32 color_ram_address_offset_a               = {0x25f800e4};
constexpr u32 color_ram_address_offset_b               = {0x25f800e6};
constexpr u32 line_color_screen_enable                 = {0x25f800e8};
constexpr u32 special_priority_mode                    = {0x25f800ea};
constexpr u32 color_calculation_control                = {0x25f800ec};
constexpr u32 special_color_calculation_mode           = {0x25f800ee};
constexpr u32 priority_number_sprite_0_1               = {0x25f800f0};
constexpr u32 priority_number_sprite_2_3               = {0x25f800f2};
constexpr u32 priority_number_sprite_4_5               = {0x25f800f4};
constexpr u32 priority_number_sprite_6_7               = {0x25f800f6};
constexpr u32 priority_number_nbg0_nbg1                = {0x25f800f8};
constexpr u32 priority_number_nbg2_nbg3                = {0x25f800fa};
constexpr u32 priority_number_rgb0                     = {0x25f800fc};
constexpr u32 color_calculation_ratio_sprite_0_1       = {0x25f80100};
constexpr u32 color_calculation_ratio_sprite_2_3       = {0x25f80102};
constexpr u32 color_calculation_ratio_sprite_4_5       = {0x25f80104};
constexpr u32 color_calculation_ratio_sprite_6_7       = {0x25f80106};
constexpr u32 color_calculation_ratio_nbg0_nbg1        = {0x25f80108};
constexpr u32 color_calculation_ratio_nbg2_nbg3        = {0x25f8010a};
constexpr u32 color_calculation_ratio_rgb0             = {0x25f8010c};
constexpr u32 color_calculation_ratio_line_color_back  = {0x25f8010e};
constexpr u32 color_offset_enable                      = {0x25f80110};
constexpr u32 color_offset_select                      = {0x25f80112};
constexpr u32 color_offset_a_red                       = {0x25f80114};
constexpr u32 color_offset_a_green                     = {0x25f80116};
constexpr u32 color_offset_a_blue                      = {0x25f80118};
constexpr u32 color_offset_b_red                       = {0x25f8011a};
constexpr u32 color_offset_b_green                     = {0x25f8011c};
constexpr u32 color_offset_b_blue                      = {0x25f8011e};

} // namespace register_address

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   Display
///
/// \brief  DISP bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Display : u8 {
    not_displayed = 0, ///< Picture is not displayed on TV screen.
    displayed     = 1  ///< Picture is displayed on TV screen.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   BorderColorMode
///
/// \brief  BDCLMD bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class BorderColorMode : u8 {
    displays_black       = 0, ///< Displays black.
    displays_back_screen = 1  ///< Displays back screen.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   InterlaceMode
///
/// \brief  LSMDx bits values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class InterlaceMode : u8 {
    non_interlace  = 0b00, ///< Non-interlace.
    not_allowed    = 0b01, ///< Not allowed.
    single_density = 0b10, ///< Single-density interlace.
    double_density = 0b11  ///< Single-density interlace.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   VerticalResolution
///
/// \brief  VRESOx bits values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class VerticalResolution : u8 {
    lines_nb_224 = 0b00, ///< 224 lines, NTSC or PAL.
    lines_nb_240 = 0b01, ///< 240 lines, NTSC or PAL.
    lines_nb_256 = 0b10, ///< 256, PAL only.
    not_allowed  = 0b11  ///< Not allowed.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   HorizontalResolution
///
/// \brief  HRESOx bits values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class HorizontalResolution : u8 {
    normal_320           = 0b000, ///< 320 pixels, normal graphic A, PAL or NTSC.
    normal_352           = 0b001, ///< 352 pixels, normal graphic B, PAL or NTSC.
    hi_res_640           = 0b010, ///< 640 pixels, hi-res graphic A, PAL or NTSC.
    hi_res_704           = 0b011, ///< 704 pixels, hi-res graphic B, PAL or NTSC.
    exclusive_normal_320 = 0b100, ///< 320 pixels, exclusive normal graphic A, 31KHz monitor.
    exclusive_normal_352 = 0b101, ///< 352 pixels, exclusive normal graphic B, Hi-vision monitor.
    exclusive_hi_res_640 = 0b110, ///< 640 pixels, exclusive normal graphic A, 31KHz monitor.
    exclusive_hi_res_704 = 0b111, ///< 704 pixels, exclusive normal graphic B, Hi-vision monitor.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  TvScreenModeRegister
///
/// \brief  TV screen mode register (TVMD).
///
/// \author Runik
/// \date   17/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class TvScreenModeRegister : public Register {
  public:
    using Register::Register;
    inline static const BitRange<Display>              display{15};                 ///< Defines DISP bit.
    inline static const BitRange<BorderColorMode>      border_color_mode{8};        ///< Defines BDCLMD bit.
    inline static const BitRange<InterlaceMode>        interlace_mode{6, 7};        ///< Defines LSMDx bit.
    inline static const BitRange<VerticalResolution>   vertical_resolution{4, 5};   ///< Defines VRESOx bit.
    inline static const BitRange<HorizontalResolution> horizontal_resolution{0, 2}; ///< Defines HRESOx bit.

    inline static const BitRange<u16> all_bits{0, 15}; ///< Defines the whole register bits
};                                                     // namespace saturnin::video

} // namespace saturnin::video
