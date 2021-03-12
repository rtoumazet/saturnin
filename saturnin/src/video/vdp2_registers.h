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

#include <saturnin/src/emulator_defs.h>

namespace saturnin::video {
namespace vdp2_register_address {
constexpr auto tv_screen_mode                           = u32{0x25f80000};
constexpr auto external_signal_enable                   = u32{0x25f80002};
constexpr auto screen_status                            = u32{0x25f80004};
constexpr auto vram_size                                = u32{0x25f80006};
constexpr auto h_counter                                = u32{0x25f80008};
constexpr auto v_counter                                = u32{0x25f8000a};
constexpr auto reserve_1                                = u32{0x25f8000c};
constexpr auto ram_control                              = u32{0x25f8000e};
constexpr auto vram_cycle_pattern_bank_a0_lower         = u32{0x25f80010};
constexpr auto vram_cycle_pattern_bank_a0_upper         = u32{0x25f80012};
constexpr auto vram_cycle_pattern_bank_a1_lower         = u32{0x25f80014};
constexpr auto vram_cycle_pattern_bank_a1_upper         = u32{0x25f80016};
constexpr auto vram_cycle_pattern_bank_b0_lower         = u32{0x25f80018};
constexpr auto vram_cycle_pattern_bank_b0_upper         = u32{0x25f8001a};
constexpr auto vram_cycle_pattern_bank_b1_lower         = u32{0x25f8001c};
constexpr auto vram_cycle_pattern_bank_b1_upper         = u32{0x25f8001e};
constexpr auto screen_display_enable                    = u32{0x25f80020};
constexpr auto mosaic_control                           = u32{0x25f80022};
constexpr auto special_function_code_select             = u32{0x25f80024};
constexpr auto special_function_code                    = u32{0x25f80026};
constexpr auto character_control_a                      = u32{0x25f80028};
constexpr auto character_control_b                      = u32{0x25f8002a};
constexpr auto bitmap_palette_number_a                  = u32{0x25f8002c};
constexpr auto bitmap_palette_number_b                  = u32{0x25f8002e};
constexpr auto pattern_name_control_nbg0                = u32{0x25f80030};
constexpr auto pattern_name_control_nbg1                = u32{0x25f80032};
constexpr auto pattern_name_control_nbg2                = u32{0x25f80034};
constexpr auto pattern_name_control_nbg3                = u32{0x25f80036};
constexpr auto pattern_name_control_rbg0                = u32{0x25f80038};
constexpr auto plane_size                               = u32{0x25f8003a};
constexpr auto map_offset_n                             = u32{0x25f8003c};
constexpr auto map_offset_r                             = u32{0x25f8003e};
constexpr auto map_nbg0_plane_a_b                       = u32{0x25f80040};
constexpr auto map_nbg0_plane_c_d                       = u32{0x25f80042};
constexpr auto map_nbg1_plane_a_b                       = u32{0x25f80044};
constexpr auto map_nbg1_plane_c_d                       = u32{0x25f80046};
constexpr auto map_nbg2_plane_a_b                       = u32{0x25f80048};
constexpr auto map_nbg2_plane_c_d                       = u32{0x25f8004a};
constexpr auto map_nbg3_plane_a_b                       = u32{0x25f8004c};
constexpr auto map_nbg3_plane_c_d                       = u32{0x25f8004e};
constexpr auto map_rotation_parameter_a_plane_a_b       = u32{0x25f80050};
constexpr auto map_rotation_parameter_a_plane_c_d       = u32{0x25f80052};
constexpr auto map_rotation_parameter_a_plane_e_f       = u32{0x25f80054};
constexpr auto map_rotation_parameter_a_plane_g_h       = u32{0x25f80056};
constexpr auto map_rotation_parameter_a_plane_i_j       = u32{0x25f80058};
constexpr auto map_rotation_parameter_a_plane_k_l       = u32{0x25f8005a};
constexpr auto map_rotation_parameter_a_plane_m_n       = u32{0x25f8005c};
constexpr auto map_rotation_parameter_a_plane_o_p       = u32{0x25f8005e};
constexpr auto map_rotation_parameter_b_plane_a_b       = u32{0x25f80060};
constexpr auto map_rotation_parameter_b_plane_c_d       = u32{0x25f80062};
constexpr auto map_rotation_parameter_b_plane_e_f       = u32{0x25f80064};
constexpr auto map_rotation_parameter_b_plane_g_h       = u32{0x25f80066};
constexpr auto map_rotation_parameter_b_plane_i_j       = u32{0x25f80068};
constexpr auto map_rotation_parameter_b_plane_k_l       = u32{0x25f8006a};
constexpr auto map_rotation_parameter_b_plane_m_n       = u32{0x25f8006c};
constexpr auto map_rotation_parameter_b_plane_o_p       = u32{0x25f8006e};
constexpr auto screen_scroll_value_nbg0_h_int_part      = u32{0x25f80070};
constexpr auto screen_scroll_value_nbg0_h_fract_part    = u32{0x25f80072};
constexpr auto screen_scroll_value_nbg0_v_int_part      = u32{0x25f80074};
constexpr auto screen_scroll_value_nbg0_v_fract_part    = u32{0x25f80076};
constexpr auto coordinate_increment_nbg0_h_int_part     = u32{0x25f80078};
constexpr auto coordinate_increment_nbg0_h_fract_part   = u32{0x25f8007a};
constexpr auto coordinate_increment_nbg0_v_int_part     = u32{0x25f8007c};
constexpr auto coordinate_increment_nbg0_v_fract_part   = u32{0x25f8007e};
constexpr auto screen_scroll_value_nbg1_h_int_part      = u32{0x25f80080};
constexpr auto screen_scroll_value_nbg1_h_fract_part    = u32{0x25f80082};
constexpr auto screen_scroll_value_nbg1_v_int_part      = u32{0x25f80084};
constexpr auto screen_scroll_value_nbg1_v_fract_part    = u32{0x25f80086};
constexpr auto coordinate_increment_nbg1_h_int_part     = u32{0x25f80088};
constexpr auto coordinate_increment_nbg1_h_fract_part   = u32{0x25f8008a};
constexpr auto coordinate_increment_nbg1_v_int_part     = u32{0x25f8008c};
constexpr auto coordinate_increment_nbg1_v_fract_part   = u32{0x25f8008e};
constexpr auto screen_scroll_value_nbg2_h               = u32{0x25f80090};
constexpr auto screen_scroll_value_nbg2_v               = u32{0x25f80092};
constexpr auto screen_scroll_value_nbg3_h               = u32{0x25f80094};
constexpr auto screen_scroll_value_nbg3_v               = u32{0x25f80096};
constexpr auto reduction_enable                         = u32{0x25f80098};
constexpr auto line_and_vertical_cell_scroll_control    = u32{0x25f8009a};
constexpr auto vertical_cell_scroll_table_address_upper = u32{0x25f8009c};
constexpr auto vertical_cell_scroll_table_address_lower = u32{0x25f8009e};
constexpr auto line_scroll_table_address_nbg0_upper     = u32{0x25f800a0};
constexpr auto line_scroll_table_address_nbg0_lower     = u32{0x25f800a2};
constexpr auto line_scroll_table_address_nbg1_upper     = u32{0x25f800a4};
constexpr auto line_scroll_table_address_nbg1_lower     = u32{0x25f800a6};
constexpr auto line_color_screen_table_address_upper    = u32{0x25f800a8};
constexpr auto line_color_screen_table_address_lower    = u32{0x25f800aa};
constexpr auto back_screen_table_address_upper          = u32{0x25f800ac};
constexpr auto back_screen_table_address_lower          = u32{0x25f800ae};
constexpr auto rotation_parameter_mode                  = u32{0x25f800b0};
constexpr auto rotation_parameter_read_control          = u32{0x25f800b2};
constexpr auto coefficient_table_control                = u32{0x25f800b4};
constexpr auto coefficient_table_address_offset         = u32{0x25f800b6};
constexpr auto screen_over_pattern_name_a               = u32{0x25f800b8};
constexpr auto screen_over_pattern_name_b               = u32{0x25f800ba};
constexpr auto rotation_parameter_table_address_upper   = u32{0x25f800bc};
constexpr auto rotation_parameter_table_address_lower   = u32{0x25f800be};
constexpr auto window_position_w0_h_start_point         = u32{0x25f800c0};
constexpr auto window_position_w0_v_start_point         = u32{0x25f800c2};
constexpr auto window_position_w0_h_end_point           = u32{0x25f800c4};
constexpr auto window_position_w0_v_end_point           = u32{0x25f800c6};
constexpr auto window_position_w1_h_start_point         = u32{0x25f800c8};
constexpr auto window_position_w1_v_start_point         = u32{0x25f800ca};
constexpr auto window_position_w1_h_end_point           = u32{0x25f800cc};
constexpr auto window_position_w1_v_end_point           = u32{0x25f800ce};
constexpr auto window_control_a                         = u32{0x25f800d0};
constexpr auto window_control_b                         = u32{0x25f800d2};
constexpr auto window_control_c                         = u32{0x25f800d4};
constexpr auto window_control_d                         = u32{0x25f800d6};
constexpr auto line_window_table_address_w0_upper       = u32{0x25f800d8};
constexpr auto line_window_table_address_w0_lower       = u32{0x25f800da};
constexpr auto line_window_table_address_w1_upper       = u32{0x25f800dc};
constexpr auto line_window_table_address_w1_lower       = u32{0x25f800de};
constexpr auto sprite_control                           = u32{0x25f800e0};
constexpr auto shadow_control                           = u32{0x25f800e2};
constexpr auto color_ram_address_offset_a               = u32{0x25f800e4};
constexpr auto color_ram_address_offset_b               = u32{0x25f800e6};
constexpr auto line_color_screen_enable                 = u32{0x25f800e8};
constexpr auto special_priority_mode                    = u32{0x25f800ea};
constexpr auto color_calculation_control                = u32{0x25f800ec};
constexpr auto special_color_calculation_mode           = u32{0x25f800ee};
constexpr auto priority_number_sprite_0_1               = u32{0x25f800f0};
constexpr auto priority_number_sprite_2_3               = u32{0x25f800f2};
constexpr auto priority_number_sprite_4_5               = u32{0x25f800f4};
constexpr auto priority_number_sprite_6_7               = u32{0x25f800f6};
constexpr auto priority_number_nbg0_nbg1                = u32{0x25f800f8};
constexpr auto priority_number_nbg2_nbg3                = u32{0x25f800fa};
constexpr auto priority_number_rbg0                     = u32{0x25f800fc};
constexpr auto reserve_2                                = u32{0x25f800fe};
constexpr auto color_calculation_ratio_sprite_0_1       = u32{0x25f80100};
constexpr auto color_calculation_ratio_sprite_2_3       = u32{0x25f80102};
constexpr auto color_calculation_ratio_sprite_4_5       = u32{0x25f80104};
constexpr auto color_calculation_ratio_sprite_6_7       = u32{0x25f80106};
constexpr auto color_calculation_ratio_nbg0_nbg1        = u32{0x25f80108};
constexpr auto color_calculation_ratio_nbg2_nbg3        = u32{0x25f8010a};
constexpr auto color_calculation_ratio_rbg0             = u32{0x25f8010c};
constexpr auto color_calculation_ratio_line_color_back  = u32{0x25f8010e};
constexpr auto color_offset_enable                      = u32{0x25f80110};
constexpr auto color_offset_select                      = u32{0x25f80112};
constexpr auto color_offset_a_red                       = u32{0x25f80114};
constexpr auto color_offset_a_green                     = u32{0x25f80116};
constexpr auto color_offset_a_blue                      = u32{0x25f80118};
constexpr auto color_offset_b_red                       = u32{0x25f8011a};
constexpr auto color_offset_b_green                     = u32{0x25f8011c};
constexpr auto color_offset_b_blue                      = u32{0x25f8011e};

} // namespace vdp2_register_address

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
/// \class  TvScreenMode
///
/// \brief  TV screen mode register (TVMD).
///
/// \author Runik
/// \date   17/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class TvScreenMode : public Register {
  public:
    using Register::Register;
    inline static const auto display               = BitRange<Display>{15};                ///< Defines DISP bit.
    inline static const auto border_color_mode     = BitRange<BorderColorMode>{8};         ///< Defines BDCLMD bit.
    inline static const auto interlace_mode        = BitRange<InterlaceMode>{6, 7};        ///< Defines LSMDx bit.
    inline static const auto vertical_resolution   = BitRange<VerticalResolution>{4, 5};   ///< Defines VRESOx bit.
    inline static const auto horizontal_resolution = BitRange<HorizontalResolution>{0, 2}; ///< Defines HRESOx bit.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  ExternalSignalEnable
///
/// \brief  External Signal Enable register (EXTEN).
///
/// \author Runik
/// \date   19/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class ExternalSignalEnable : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   ExternalLatchFlag
///
/// \brief  EXLTFG bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class ExternalLatchFlag : u8 {
    not_latched_in_register = 0, ///< Not latched in register.
    latched_in_register     = 1  ///< Latched in register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   ExternalSyncFlag
///
/// \brief  EXSYFG bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class ExternalSyncFlag : u8 {
    no_sync       = 0, ///< Not synchronized.
    internal_sync = 1  ///< Internal circuit synchronized.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   VerticalBlankFlag
///
/// \brief  VBLANK bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class VerticalBlankFlag : u8 {
    during_vertical_scan    = 0, ///< During vertical scan.
    during_vertical_retrace = 1  ///< During vertical retrace (vblank).
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   HorizontalBlankFlag
///
/// \brief  HBLANK bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class HorizontalBlankFlag : u8 {
    during_horizontal_scan    = 0, ///< During horizontal scan.
    during_horizontal_retrace = 1  ///< During horizontal retrace (hblank).
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   ScanFieldFlag
///
/// \brief  ODD bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class ScanFieldFlag : u8 {
    during_even_field_scan = 0, ///< During even field scan.
    during_odd_field_scan  = 1  ///< During odd field scan.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   TvStandardFlag
///
/// \brief  PAL bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class TvStandardFlag : u8 {
    ntsc_standard = 0, ///< NTSC standard.
    pal_standard  = 1  ///< PAL standard.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  ScreenStatus
///
/// \brief  Screen Status register (TVSTAT).
///
/// \author Runik
/// \date   20/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class ScreenStatus : public Register {
  public:
    using Register::Register;
    inline static const auto external_latch_flag   = BitRange<ExternalLatchFlag>{9};   ///< Defines EXLTFG bit.
    inline static const auto external_sync_flag    = BitRange<ExternalSyncFlag>{8};    ///< Defines EXSYFG bit.
    inline static const auto vertical_blank_flag   = BitRange<VerticalBlankFlag>{3};   ///< Defines VBLANK bit.
    inline static const auto horizontal_blank_flag = BitRange<HorizontalBlankFlag>{2}; ///< Defines HBLANK bit.
    inline static const auto scan_field_flag       = BitRange<ScanFieldFlag>{1};       ///< Defines ODD bit.
    inline static const auto tv_standard_flag      = BitRange<TvStandardFlag>{0};      ///< Defines PAL bit.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   TvStandardFlag
///
/// \brief  PAL bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class VramSize : u8 {
    not_set,          ///< Not set.
    size_4_mbits = 0, ///< 4 Mbit.
    size_8_mbits = 1  ///< 8 Mbit.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  VramSize
///
/// \brief  VRAM Size register (VRSIZE).
///
/// \author Runik
/// \date   20/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class VramSizeRegister : public Register {
  public:
    using Register::Register;
    inline static const auto vram_size      = BitRange<VramSize>{15}; ///< Defines VRAMSZ bit.
    inline static const auto version_number = BitRange<u8>{0, 3};     ///< Defines VERx bits.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  HCounter
///
/// \brief  H-Counter register.
///
/// \author Runik
/// \date   20/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class HCounter : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  VCounter
///
/// \brief  V-Counter register (VCNT).
///
/// \author Runik
/// \date   20/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class VCounter : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  Reserve
///
/// \brief  Reserve register.
///
/// \author Runik
/// \date   21/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class Reserve : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   CoefficientTableStorage
///
/// \brief  Selects whether to store the coefficient table in the color RAM (CRKTE bit).
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class CoefficientTableStorage : u8 {
    not_set,                 ///< Not set.
    stored_in_vram      = 0, ///< Coefficient table is stored in VRAM.
    stored_in_color_ram = 1  ///< Coefficient table is stored in color RAM.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   ColorRamMode
///
/// \brief  Selects the color RAM mode (CRMDx bits).
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class ColorRamMode : u8 {
    mode_0_rgb_5_bits_1024_colors = 0b00, ///< RGB each 5 bits, 1024 colors settings.
    mode_1_rgb_5_bits_2048_colors = 0b01, ///< RGB each 5 bits, 2048 colors settings.
    mode_2_rgb_8_bits_1024_colors = 0b10, ///< RGB each 8 bits, 1024 colors settings.
    setting_not_allowed           = 0b11, ///< Setting not allowed.
    not_set                               ///< Not set.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   VramMode
///
/// \brief  VRAM mode bit (VRxMD bit).
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class VramMode : u8 {
    no_partition         = 0, ///< Do not partition in 2 banks.
    partition_in_2_banks = 1  ///< Partition in 2 banks.
};

enum class RotationDataBankSelect : u8 {
    not_used                             = 0b00, ///< VRAM not used as RBG0 RAM.
    used_as_rbg0_coefficient_table       = 0b01, ///< VRAM used as RBG0 coefficient table.
    used_as_rbg0_pattern_name_table      = 0b10, ///< VRAM used as RBG0 Pattern Name table.
    used_as_rbg0_character_pattern_table = 0b11  ///< VRAM used as RBG0 Character Pattern table (or Bitmap Pattern)
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  RamControl
///
/// \brief  RAM Control register (RAMCTL).
///
/// \author Runik
/// \date   20/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class RamControl : public Register {
  public:
    using Register::Register;
    inline static const auto coefficient_table_storage    = BitRange<CoefficientTableStorage>{15};  ///< Defines CRKTE bit.
    inline static const auto color_ram_mode               = BitRange<ColorRamMode>{12, 13};         ///< Defines CRMDx bits.
    inline static const auto vram_b_mode                  = BitRange<VramMode>{9};                  ///< Defines VRBMD bit.
    inline static const auto vram_a_mode                  = BitRange<VramMode>{8};                  ///< Defines VRAMD bit.
    inline static const auto vram_a0_rotation_bank_select = BitRange<RotationDataBankSelect>{0, 1}; ///< Defines RDBSA0x bits.
    inline static const auto vram_a1_rotation_bank_select = BitRange<RotationDataBankSelect>{2, 3}; ///< Defines RDBSA1x bits.
    inline static const auto vram_b0_rotation_bank_select = BitRange<RotationDataBankSelect>{4, 5}; ///< Defines RDBSB0x bits.
    inline static const auto vram_b1_rotation_bank_select = BitRange<RotationDataBankSelect>{6, 7}; ///< Defines RDBSB1x bits.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   VramAccessCommand
///
/// \brief  VRAM access command (VCP0A0x, VCP0A1x, VCP0B0x, VCP0B1x bits).
////////////////////////////////////////////////////////////////////////////////////////////////////
enum class VramAccessCommand : u8 {
    nbg0_pattern_name_read                    = 0b0000,
    nbg1_pattern_name_read                    = 0b0001,
    nbg2_pattern_name_read                    = 0b0010,
    nbg3_pattern_name_read                    = 0b0011,
    nbg0_character_pattern_data_read          = 0b0100,
    nbg1_character_pattern_data_read          = 0b0101,
    nbg2_character_pattern_data_read          = 0b0110,
    nbg3_character_pattern_data_read          = 0b0111,
    setting_not_allowed_1                     = 0b1000,
    setting_not_allowed_2                     = 0b1001,
    setting_not_allowed_3                     = 0b1010,
    setting_not_allowed_4                     = 0b1011,
    nbg0_vertical_cell_scroll_table_data_read = 0b1100,
    nbg1_vertical_cell_scroll_table_data_read = 0b1101,
    cpu_read_write                            = 0b1110,
    no_access                                 = 0b1111
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  VramCyclePatternBankA0Lower
///
/// \brief  VRAM Cycle Pattern (Bank A0) lower register (CYCA0L).
///
/// \author Runik
/// \date   21/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class VramCyclePatternBankA0Lower : public Register {
  public:
    using Register::Register;
    inline static const auto t0 = BitRange<VramAccessCommand>{12, 15}; ///< Defines VCP0A0x bits.
    inline static const auto t1 = BitRange<VramAccessCommand>{8, 11};  ///< Defines VCP1A0x bits.
    inline static const auto t2 = BitRange<VramAccessCommand>{4, 7};   ///< Defines VCP2A0x bits.
    inline static const auto t3 = BitRange<VramAccessCommand>{0, 3};   ///< Defines VCP3A0x bits.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  VramCyclePatternBankA0Upper
///
/// \brief  VRAM Cycle Pattern (Bank A0) upper register (CYCA0U).
///
/// \author Runik
/// \date   21/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class VramCyclePatternBankA0Upper : public Register {
  public:
    using Register::Register;
    inline static const auto t4 = BitRange<VramAccessCommand>{12, 15}; ///< Defines VCP4A0x bits.
    inline static const auto t5 = BitRange<VramAccessCommand>{8, 11};  ///< Defines VCP5A0x bits.
    inline static const auto t6 = BitRange<VramAccessCommand>{4, 7};   ///< Defines VCP6A0x bits.
    inline static const auto t7 = BitRange<VramAccessCommand>{0, 3};   ///< Defines VCP7A0x bits.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  VramCyclePatternBankA1Lower
///
/// \brief  VRAM Cycle Pattern (Bank A1) lower register (CYCA1L).
///
/// \author Runik
/// \date   21/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class VramCyclePatternBankA1Lower : public Register {
  public:
    using Register::Register;
    inline static const auto t0 = BitRange<VramAccessCommand>{12, 15}; ///< Defines VCP0A1x bits.
    inline static const auto t1 = BitRange<VramAccessCommand>{8, 11};  ///< Defines VCP1A1x bits.
    inline static const auto t2 = BitRange<VramAccessCommand>{4, 7};   ///< Defines VCP2A1x bits.
    inline static const auto t3 = BitRange<VramAccessCommand>{0, 3};   ///< Defines VCP3A1x bits.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  VramCyclePatternBankA1Upper
///
/// \brief  VRAM Cycle Pattern (Bank A1) upper register (CYCA1U).
///
/// \author Runik
/// \date   21/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class VramCyclePatternBankA1Upper : public Register {
  public:
    using Register::Register;
    inline static const auto t4 = BitRange<VramAccessCommand>{12, 15}; ///< Defines VCP4A1x bits.
    inline static const auto t5 = BitRange<VramAccessCommand>{8, 11};  ///< Defines VCP5A1x bits.
    inline static const auto t6 = BitRange<VramAccessCommand>{4, 7};   ///< Defines VCP6A1x bits.
    inline static const auto t7 = BitRange<VramAccessCommand>{0, 3};   ///< Defines VCP7A1x bits.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  VramCyclePatternBankB0Lower
///
/// \brief  VRAM Cycle Pattern (Bank B0) lower register (CYCB0L).
///
/// \author Runik
/// \date   21/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class VramCyclePatternBankB0Lower : public Register {
  public:
    using Register::Register;
    inline static const auto t0 = BitRange<VramAccessCommand>{12, 15}; ///< Defines VCP0B0x bits.
    inline static const auto t1 = BitRange<VramAccessCommand>{8, 11};  ///< Defines VCP1B0x bits.
    inline static const auto t2 = BitRange<VramAccessCommand>{4, 7};   ///< Defines VCP2B0x bits.
    inline static const auto t3 = BitRange<VramAccessCommand>{0, 3};   ///< Defines VCP3B0x bits.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  VramCyclePatternBankB0Upper
///
/// \brief  VRAM Cycle Pattern (Bank B0) upper register (CYCB0U).
///
/// \author Runik
/// \date   21/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class VramCyclePatternBankB0Upper : public Register {
  public:
    using Register::Register;
    inline static const auto t4 = BitRange<VramAccessCommand>{12, 15}; ///< Defines VCP4B0x bits.
    inline static const auto t5 = BitRange<VramAccessCommand>{8, 11};  ///< Defines VCP5B0x bits.
    inline static const auto t6 = BitRange<VramAccessCommand>{4, 7};   ///< Defines VCP6B0x bits.
    inline static const auto t7 = BitRange<VramAccessCommand>{0, 3};   ///< Defines VCP7B0x bits.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  VramCyclePatternBankB1Lower
///
/// \brief  VRAM Cycle Pattern (Bank B1) lower register (CYCB1L).
///
/// \author Runik
/// \date   21/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class VramCyclePatternBankB1Lower : public Register {
  public:
    using Register::Register;
    inline static const auto t0 = BitRange<VramAccessCommand>{12, 15}; ///< Defines VCP0B1x bits.
    inline static const auto t1 = BitRange<VramAccessCommand>{8, 11};  ///< Defines VCP1B1x bits.
    inline static const auto t2 = BitRange<VramAccessCommand>{4, 7};   ///< Defines VCP2B1x bits.
    inline static const auto t3 = BitRange<VramAccessCommand>{0, 3};   ///< Defines VCP3B1x bits.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  VramCyclePatternBankB1Upper
///
/// \brief  VRAM Cycle Pattern (Bank B1) upper register (CYCB1U).
///
/// \author Runik
/// \date   21/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class VramCyclePatternBankB1Upper : public Register {
  public:
    using Register::Register;
    inline static const auto t4 = BitRange<VramAccessCommand>{12, 15}; ///< Defines VCP4B1x bits.
    inline static const auto t5 = BitRange<VramAccessCommand>{8, 11};  ///< Defines VCP5B1x bits.
    inline static const auto t6 = BitRange<VramAccessCommand>{4, 7};   ///< Defines VCP6B1x bits.
    inline static const auto t7 = BitRange<VramAccessCommand>{0, 3};   ///< Defines VCP7B1x bits.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   TransparentDisplayEnable
///
/// \brief  xxTPON bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class TransparentDisplayEnable : u8 {
    transparency_code_valid = 0, ///< Validates transparency code (transparency code becomes valid).
    transparency_code_invalid
    = 1 ///< Invalidates transparency code (transparency code dots are displayed according to data values).
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   ScreenDisplayEnableBit
///
/// \brief  xxON bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class ScreenDisplayEnableBit : u8 {
    cannot_display = 0, ///< Cannot display (Does not execute VRAM access for display).
    can_display    = 1  ///< Can display.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  ScreenDisplayEnable
///
/// \brief  Screen Display Enable register (BGON).
///
/// \author Runik
/// \date   22/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class ScreenDisplayEnable : public Register {
  public:
    using Register::Register;
    inline static const auto transparency_display_enable_rbg0 = BitRange<TransparentDisplayEnable>{12}; ///< Defines R0TPON bit.
    inline static const auto transparency_display_enable_nbg3 = BitRange<TransparentDisplayEnable>{11}; ///< Defines N3TPON bit.
    inline static const auto transparency_display_enable_nbg2 = BitRange<TransparentDisplayEnable>{10}; ///< Defines N2TPON bit.
    inline static const auto transparency_display_enable_nbg1 = BitRange<TransparentDisplayEnable>{9};  ///< Defines N1TPON bit.
    inline static const auto transparency_display_enable_nbg0 = BitRange<TransparentDisplayEnable>{8};  ///< Defines N0TPON bit.
    inline static const auto screen_display_enable_rbg1       = BitRange<ScreenDisplayEnableBit>{5};    ///< Defines R1ON bit.
    inline static const auto screen_display_enable_rbg0       = BitRange<ScreenDisplayEnableBit>{4};    ///< Defines R0ON bit.
    inline static const auto screen_display_enable_nbg3       = BitRange<ScreenDisplayEnableBit>{3};    ///< Defines N3ON bit.
    inline static const auto screen_display_enable_nbg2       = BitRange<ScreenDisplayEnableBit>{2};    ///< Defines N2ON bit.
    inline static const auto screen_display_enable_nbg1       = BitRange<ScreenDisplayEnableBit>{1};    ///< Defines N1ON bit.
    inline static const auto screen_display_enable_nbg0       = BitRange<ScreenDisplayEnableBit>{0};    ///< Defines N0ON bit.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  MosaicControl
///
/// \brief  Mosaic Control register (MZCTL).
///
/// \author Runik
/// \date   22/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class MosaicControl : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  SpecialFunctionCodeSelect
///
/// \brief  Special Function Code Select register (SFSEL).
///
/// \author Runik
/// \date   22/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class SpecialFunctionCodeSelect : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  SpecialFunctionCode
///
/// \brief  Special Function Code register (SFCODE).
///
/// \author Runik
/// \date   22/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class SpecialFunctionCode : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   CharacterColorNumber3Bits
///
/// \brief  N0CHCNx / R0CHCNx bits values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class CharacterColorNumber3bits : u8 {
    palette_16   = 0b000, ///< Palette format, 16 colors.
    palette_256  = 0b001, ///< Palette format, 256 colors.
    palette_2048 = 0b010, ///< Palette format, 2048 colors.
    rgb_32k      = 0b011, ///< RGB format, 32768 colors.
    rgb_16m      = 0b100  ///< RGB format, 16770000 colors.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   CharacterColorNumber2Bits
///
/// \brief  N1CHCNx bits values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class CharacterColorNumber2Bits : u8 {
    palette_16   = 0b00, ///< Palette format, 16 colors.
    palette_256  = 0b01, ///< Palette format, 256 colors.
    palette_2048 = 0b10, ///< Palette format, 2048 colors.
    rgb_32k      = 0b11, ///< RGB format, 32768 colors.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   CharacterColorNumberNbg1
///
/// \brief  N2CHCNx / N3CHCNx bits values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class CharacterColorNumber1Bit : u8 {
    palette_16  = 0b0, ///< Palette format, 16 colors.
    palette_256 = 0b1, ///< Palette format, 256 colors.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   BitmapSize2Bits
///
/// \brief  N0BMSZx / N1BMSZx bits values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class BitmapSize2Bits : u8 {
    size_512_by_256  = 0b00, ///< 512 H dots* 256 V dots.
    size_512_by_512  = 0b01, ///< 512 H dots* 512 V dots.
    size_1024_by_256 = 0b10, ///< 1024 H dots* 256 V dots.
    size_1024_by_512 = 0b11  ///< 1024 H dots* 512 V dots.

};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   BitmapSize1Bit
///
/// \brief  R0BMSZ bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class BitmapSize1Bit : u8 {
    size_512_by_256 = 0b0, ///< 512 H dots* 256 V dots.
    size_512_by_512 = 0b1  ///< 512 H dots* 512 V dots.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   BitmapEnable
///
/// \brief  xxBMEN bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class BitmapEnable : u8 {
    cell_format   = 0b0, ///< Cell Format.
    bitmap_format = 0b1  ///< Bitmap Format.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   CharacterSize
///
/// \brief  xxCHSZ bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class CharacterSize : u8 {
    one_by_one = 0b0, ///< 1 H Cell x 1 V Cell.
    two_by_two = 0b1  ///< 2 H Cells x 2 V Cells.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  CharacterControlA
///
/// \brief  Character Control (NBG0, NBG1) register (CHCTLA).
///
/// \author Runik
/// \date   22/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class CharacterControlA : public Register {
  public:
    using Register::Register;
    inline static const auto character_color_number_nbg1 = BitRange<CharacterColorNumber2Bits>{12, 13}; ///< Defines N1CHCNx bits.
    inline static const auto character_color_number_nbg0 = BitRange<CharacterColorNumber3bits>{4, 6};   ///< Defines N0CHCNx bits.
    inline static const auto bitmap_size_nbg1            = BitRange<BitmapSize2Bits>{10, 11};           ///< Defines N1BMSZx bits.
    inline static const auto bitmap_size_nbg0            = BitRange<BitmapSize2Bits>{2, 3};             ///< Defines N0BMSZx bits.
    inline static const auto bitmap_enable_nbg0          = BitRange<BitmapEnable>{1};                   ///< Defines N0BMEN bit.
    inline static const auto bitmap_enable_nbg1          = BitRange<BitmapEnable>{9};                   ///< Defines N1BMEN bit.
    inline static const auto character_size_nbg0         = BitRange<CharacterSize>{0};                  ///< Defines N0CHSZ bit.
    inline static const auto character_size_nbg1         = BitRange<CharacterSize>{8};                  ///< Defines N1CHSZ bit.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  CharacterControlB
///
/// \brief  Character Control (NBG2, NBG3, RBG0) register (CHCTLB).
///
/// \author Runik
/// \date   22/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class CharacterControlB : public Register {
  public:
    using Register::Register;
    inline static const auto character_color_number_rbg0 = BitRange<CharacterColorNumber3bits>{12, 14}; ///< Defines R0CHCNx bits.
    inline static const auto character_color_number_nbg3 = BitRange<CharacterColorNumber1Bit>{5};       ///< Defines N3CHCNx bit.
    inline static const auto character_color_number_nbg2 = BitRange<CharacterColorNumber1Bit>{1};       ///< Defines N2CHCNx bit.
    inline static const auto bitmap_size_rbg0            = BitRange<BitmapSize1Bit>{2};                 ///< Defines R0BMSZ bit.
    inline static const auto bitmap_enable_rbg0          = BitRange<BitmapEnable>{9};                   ///< Defines R0BMEN bit.
    inline static const auto character_size_nbg2         = BitRange<CharacterSize>{0};                  ///< Defines N2CHSZ bit.
    inline static const auto character_size_nbg3         = BitRange<CharacterSize>{4};                  ///< Defines N3CHSZ bit.
    inline static const auto character_size_rbg0         = BitRange<CharacterSize>{8};                  ///< Defines R0CHSZ bit.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  BitmapPaletteNumberA
///
/// \brief  Bitmap Palette Number (NBG0, NBG1) register (BMPNA).
///
/// \author Runik
/// \date   22/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class BitmapPaletteNumberA : public Register {
  public:
    using Register::Register;
    inline static const auto bitmap_special_priority_nbg0          = BitRange<u8>{5};     ///< Defines N0BMPR bit.
    inline static const auto bitmap_special_priority_nbg1          = BitRange<u8>{13};    ///< Defines N1BMPR bit.
    inline static const auto bitmap_special_color_calculation_nbg0 = BitRange<u8>{4};     ///< Defines N0BMCC bit.
    inline static const auto bitmap_special_color_calculation_nbg1 = BitRange<u8>{12};    ///< Defines N1BMCC bit.
    inline static const auto bitmap_palette_number_nbg0            = BitRange<u8>{0, 2};  ///< Defines N0BMPx bit.
    inline static const auto bitmap_palette_number_nbg1            = BitRange<u8>{8, 10}; ///< Defines N1BMPx bit.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  BitmapPaletteNumberB
///
/// \brief  Bitmap Palette Number (RBG0) register (BMPNB).
///
/// \author Runik
/// \date   22/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class BitmapPaletteNumberB : public Register {
  public:
    using Register::Register;
    inline static const auto bitmap_special_priority_rbg0          = BitRange<u8>{5};    ///< Defines N0BMPR bit.
    inline static const auto bitmap_special_color_calculation_rbg0 = BitRange<u8>{4};    ///< Defines R0BMCC bit.
    inline static const auto bitmap_palette_number_rbg0            = BitRange<u8>{0, 2}; ///< Defines R0BMPx bit.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   PatternNameDataSize
///
/// \brief  PNCNx / PNCR bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class PatternNameDataSize : u8 {
    two_words = 0b0, ///< 2 Words.
    one_word  = 0b1  ///< 1 Word.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   CharacterNumberSupplementMode
///
/// \brief  NxCNSM / R0CNSM bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class CharacterNumberSupplementMode : u8 {
    character_number_10_bits
    = 0b0, ///< Character number in pattern name data is 10 bits. Flip function can be selected in character units.
    character_number_12_bits = 0b1 ///< Character number in pattern name data is 12 bits. Flip function cannot be used.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  PatternNameControlNbg0
///
/// \brief  Pattern Name Control (NBG0) register (PNCN0).
///
/// \author Runik
/// \date   23/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class PatternNameControlNbg0 : public Register {
  public:
    using Register::Register;
    inline static const auto pattern_name_data_size       = BitRange<PatternNameDataSize>{15};           ///< Defines N0PNB bit.
    inline static const auto character_number_mode        = BitRange<CharacterNumberSupplementMode>{14}; ///< Defines N0CNSM bit.
    inline static const auto special_color_calculation    = BitRange<u8>{8};                             ///< Defines N0SCC bit.
    inline static const auto supplementary_palette_number = BitRange<u8>{5, 7};   ///< Defines N0SPLTx bits.
    inline static const auto supplementary_character_number = BitRange<u8>{0, 4}; ///< Defines N0SCNx bits.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  PatternNameControlNbg1
///
/// \brief  Pattern Name Control (NBG1) register (PNCN1).
///
/// \author Runik
/// \date   23/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class PatternNameControlNbg1 : public Register {
  public:
    using Register::Register;
    inline static const auto pattern_name_data_size       = BitRange<PatternNameDataSize>{15};           ///< Defines N1PNB bit.
    inline static const auto character_number_mode        = BitRange<CharacterNumberSupplementMode>{14}; ///< Defines N1CNSM bit.
    inline static const auto special_color_calculation    = BitRange<u8>{8};                             ///< Defines N1SCC bit.
    inline static const auto supplementary_palette_number = BitRange<u8>{5, 7};   ///< Defines N1SPLTx bits.
    inline static const auto supplementary_character_number = BitRange<u8>{0, 4}; ///< Defines N1SCNx bits.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  PatternNameControlNbg2
///
/// \brief  Pattern Name Control (NBG2) register (PNCN2).
///
/// \author Runik
/// \date   23/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class PatternNameControlNbg2 : public Register {
  public:
    using Register::Register;
    inline static const auto pattern_name_data_size       = BitRange<PatternNameDataSize>{15};           ///< Defines N2PNB bit.
    inline static const auto character_number_mode        = BitRange<CharacterNumberSupplementMode>{14}; ///< Defines N2CNSM bit.
    inline static const auto special_color_calculation    = BitRange<u8>{8};                             ///< Defines N2SCC bit.
    inline static const auto supplementary_palette_number = BitRange<u8>{5, 7};   ///< Defines N2SPLTx bits.
    inline static const auto supplementary_character_number = BitRange<u8>{0, 4}; ///< Defines N2SCNx bits.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  PatternNameControlNbg3
///
/// \brief  Pattern Name Control (NBG3) register (PNCN3).
///
/// \author Runik
/// \date   23/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class PatternNameControlNbg3 : public Register {
  public:
    using Register::Register;
    inline static const auto pattern_name_data_size       = BitRange<PatternNameDataSize>{15};           ///< Defines N3PNB bit.
    inline static const auto character_number_mode        = BitRange<CharacterNumberSupplementMode>{14}; ///< Defines N3CNSM bit.
    inline static const auto special_color_calculation    = BitRange<u8>{8};                             ///< Defines N3SCC bit.
    inline static const auto supplementary_palette_number = BitRange<u8>{5, 7};   ///< Defines N3SPLTx bits.
    inline static const auto supplementary_character_number = BitRange<u8>{0, 4}; ///< Defines N3SCNx bits.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  PatternNameControlRbg0
///
/// \brief  Pattern Name Control (RBG0) register (PNCNR).
///
/// \author Runik
/// \date   23/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class PatternNameControlRbg0 : public Register {
  public:
    using Register::Register;
    inline static const auto pattern_name_data_size       = BitRange<PatternNameDataSize>{15};           ///< Defines R0PNB bit.
    inline static const auto character_number_mode        = BitRange<CharacterNumberSupplementMode>{14}; ///< Defines R0CNSM bit.
    inline static const auto special_color_calculation    = BitRange<u8>{8};                             ///< Defines R0SCC bit.
    inline static const auto supplementary_palette_number = BitRange<u8>{5, 7};   ///< Defines R0SPLTx bits.
    inline static const auto supplementary_character_number = BitRange<u8>{0, 4}; ///< Defines R0SCNx bits.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   PlaneSize
///
/// \brief  xxPLSZx bits values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class PlaneSize : u8 {
    size_1_by_1 = 0b00, ///< 1 H Page x 1 V Page.
    size_2_by_1 = 0b01, ///< 2 H Page x 1 V Page.
    invalid     = 0b10, ///< Invalid (Do not set).
    size_2_by_2 = 0b11  ///< 2 H Page x 2 V Page.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   ScreenOverProcess
///
/// \brief  RxOVRx bits values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class ScreenOverProcess : u8 {
    image_is_repeated = 0b00, ///< Outside the display area, the image set in the display area is repeated.
    character_pattern_is_repeated
    = 0b01, ///< Outside the display area,the character pattern specified by screen over pattern name register is repeated (Only
            ///< when the rotation scroll surface is in cell format).
    scroll_screen_is_transparent = 0b10, ///< Outside the display area, the scroll screen is transparent.
    force_display_and_make_transparent
    = 0b11 ///< Set the display area as 0<=X<=512, 0<=Y<=512 regardless of plane or bitmap size and makes that area transparent.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  PlaneSize
///
/// \brief  Plane Size register (PLSZ).
///
/// \author Runik
/// \date   23/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class PlaneSizeRegister : public Register {
  public:
    using Register::Register;
    inline static const auto plane_size_nbg0 = BitRange<PlaneSize>{0, 1};           ///< Defines N0PLSZx bits.
    inline static const auto plane_size_nbg1 = BitRange<PlaneSize>{2, 3};           ///< Defines N1PLSZx bits.
    inline static const auto plane_size_nbg2 = BitRange<PlaneSize>{4, 5};           ///< Defines N2PLSZx bits.
    inline static const auto plane_size_nbg3 = BitRange<PlaneSize>{6, 7};           ///< Defines N3PLSZx bits.
    inline static const auto plane_size_rpa  = BitRange<PlaneSize>{8, 9};           ///< Defines RAPLSZx bits.
    inline static const auto plane_size_rpb  = BitRange<PlaneSize>{12, 13};         ///< Defines RBPLSZx bits.
    inline static const auto screen_over_rpa = BitRange<ScreenOverProcess>{10, 11}; ///< Defines RAOVRx bits.
    inline static const auto screen_over_rpb = BitRange<ScreenOverProcess>{14, 15}; ///< Defines RBOVRx bits.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  MapOffsetNbg
///
/// \brief  Map Offset (NBG0 - NBG3) register (MPOFN).
///
/// \author Runik
/// \date   23/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class MapOffsetNbg : public Register {
  public:
    using Register::Register;
    inline static const auto map_offset_nbg0 = BitRange<u8>{0, 2};   ///< Defines N0MPx bits.
    inline static const auto map_offset_nbg1 = BitRange<u8>{4, 6};   ///< Defines N1MPx bits.
    inline static const auto map_offset_nbg2 = BitRange<u8>{8, 10};  ///< Defines N2MPx bits.
    inline static const auto map_offset_nbg3 = BitRange<u8>{12, 14}; ///< Defines N3MPx bits.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  MapOffsetRbg
///
/// \brief  Map Offset (Rotation Parameter A,B) register (MPOFR).
///
/// \author Runik
/// \date   23/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class MapOffsetRbg : public Register {
  public:
    using Register::Register;
    inline static const auto map_offset_rpa = BitRange<u8>{0, 2}; ///< Defines RAMPx bits.
    inline static const auto map_offset_rpb = BitRange<u8>{4, 6}; ///< Defines RBMPx bits.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  MapNbg0PlaneAB
///
/// \brief  Map (NBG0, Plane A,B) register (MPABN0).
///
/// \author Runik
/// \date   23/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class MapNbg0PlaneAB : public Register {
  public:
    using Register::Register;
    inline static const auto plane_a = BitRange<u8>{0, 5};  ///< Defines N0MPAx bits.
    inline static const auto plane_b = BitRange<u8>{8, 13}; ///< Defines N0MPBx bits.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  MapNbg0PlaneCD
///
/// \brief  Map (NBG0, Plane C,D) register (MPCDN0).
///
/// \author Runik
/// \date   23/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class MapNbg0PlaneCD : public Register {
  public:
    using Register::Register;
    inline static const auto plane_c = BitRange<u8>{0, 5};  ///< Defines N0MPCx bits.
    inline static const auto plane_d = BitRange<u8>{8, 13}; ///< Defines N0MPDx bits.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  MapNbg1PlaneAB
///
/// \brief  Map (NBG1, Plane A,B) register (MPABN1).
///
/// \author Runik
/// \date   23/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class MapNbg1PlaneAB : public Register {
  public:
    using Register::Register;
    inline static const auto plane_a = BitRange<u8>{0, 5};  ///< Defines N1MPAx bits.
    inline static const auto plane_b = BitRange<u8>{8, 13}; ///< Defines N1MPBx bits.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  MapNbg1PlaneCD
///
/// \brief  Map (NBG1, Plane C,D) register (MPCDN1).
///
/// \author Runik
/// \date   23/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class MapNbg1PlaneCD : public Register {
  public:
    using Register::Register;
    inline static const auto plane_c = BitRange<u8>{0, 5};  ///< Defines N1MPCx bits.
    inline static const auto plane_d = BitRange<u8>{8, 13}; ///< Defines N1MPDx bits.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  MapNbg2PlaneAB
///
/// \brief  Map (NBG2, Plane A,B) register (MPABN2).
///
/// \author Runik
/// \date   23/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class MapNbg2PlaneAB : public Register {
  public:
    using Register::Register;
    inline static const auto plane_a = BitRange<u8>{0, 5};  ///< Defines N2MPAx bits.
    inline static const auto plane_b = BitRange<u8>{8, 13}; ///< Defines N2MPBx bits.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  MapNbg2PlaneCD
///
/// \brief  Map (NBG2, Plane C,D) register (MPCDN2).
///
/// \author Runik
/// \date   23/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class MapNbg2PlaneCD : public Register {
  public:
    using Register::Register;
    inline static const auto plane_c = BitRange<u8>{0, 5};  ///< Defines N2MPCx bits.
    inline static const auto plane_d = BitRange<u8>{8, 13}; ///< Defines N2MPDx bits.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  MapNbg3PlaneAB
///
/// \brief  Map (NBG3, Plane A,B) register (MPABN3).
///
/// \author Runik
/// \date   23/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class MapNbg3PlaneAB : public Register {
  public:
    using Register::Register;
    inline static const auto plane_a = BitRange<u8>{0, 5};  ///< Defines N3MPAx bits.
    inline static const auto plane_b = BitRange<u8>{8, 13}; ///< Defines N3MPBx bits.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  MapNbg3PlaneCD
///
/// \brief  Map (NBG3, Plane C,D) register (MPCDN3).
///
/// \author Runik
/// \date   23/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class MapNbg3PlaneCD : public Register {
  public:
    using Register::Register;
    inline static const auto plane_c = BitRange<u8>{0, 5};  ///< Defines N3MPCx bits.
    inline static const auto plane_d = BitRange<u8>{8, 13}; ///< Defines N3MPDx bits.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  MapRotationParameterAPlaneAB
///
/// \brief  Map (Rotation Parameter A, Plane A,B) register (MPABRA).
///
/// \author Runik
/// \date   23/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class MapRotationParameterAPlaneAB : public Register {
  public:
    using Register::Register;
    inline static const auto plane_a = BitRange<u8>{0, 5};  ///< Defines RAMPAx bits.
    inline static const auto plane_b = BitRange<u8>{8, 13}; ///< Defines RAMPBx bits.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  MapRotationParameterAPlaneCD
///
/// \brief  Map (Rotation Parameter A, Plane C,D) register (MPCDRA).
///
/// \author Runik
/// \date   23/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class MapRotationParameterAPlaneCD : public Register {
  public:
    using Register::Register;
    inline static const auto plane_c = BitRange<u8>{0, 5};  ///< Defines RAMPCx bits.
    inline static const auto plane_d = BitRange<u8>{8, 13}; ///< Defines RAMPDx bits.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  MapRotationParameterAPlaneEF
///
/// \brief  Map (Rotation Parameter A, Plane E,F) register (MPEFRA).
///
/// \author Runik
/// \date   23/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class MapRotationParameterAPlaneEF : public Register {
  public:
    using Register::Register;
    inline static const auto plane_e = BitRange<u8>{0, 5};  ///< Defines RAMPEx bits.
    inline static const auto plane_f = BitRange<u8>{8, 13}; ///< Defines RAMPFx bits.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  MapRotationParameterAPlaneGH
///
/// \brief  Map (Rotation Parameter A, Plane G,H) register (MPGHRA).
///
/// \author Runik
/// \date   23/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class MapRotationParameterAPlaneGH : public Register {
  public:
    using Register::Register;
    inline static const auto plane_g = BitRange<u8>{0, 5};  ///< Defines RAMPGx bits.
    inline static const auto plane_h = BitRange<u8>{8, 13}; ///< Defines RAMPHx bits.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  MapRotationParameterAPlaneIJ
///
/// \brief  Map (Rotation Parameter A, Plane I,J) register (MPIJRA).
///
/// \author Runik
/// \date   23/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class MapRotationParameterAPlaneIJ : public Register {
  public:
    using Register::Register;
    inline static const auto plane_i = BitRange<u8>{0, 5};  ///< Defines RAMPIx bits.
    inline static const auto plane_j = BitRange<u8>{8, 13}; ///< Defines RAMPJx bits.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  MapRotationParameterAPlaneKL
///
/// \brief  Map (Rotation Parameter A, Plane K,L) register (MPKLRA).
///
/// \author Runik
/// \date   23/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class MapRotationParameterAPlaneKL : public Register {
  public:
    using Register::Register;
    inline static const auto plane_k = BitRange<u8>{0, 5};  ///< Defines RAMPKx bits.
    inline static const auto plane_l = BitRange<u8>{8, 13}; ///< Defines RAMPLx bits.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  MapRotationParameterAPlaneMN
///
/// \brief  Map (Rotation Parameter A, Plane M,N) register (MPMNRA).
///
/// \author Runik
/// \date   23/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class MapRotationParameterAPlaneMN : public Register {
  public:
    using Register::Register;
    inline static const auto plane_m = BitRange<u8>{0, 5};  ///< Defines RAMPMx bits.
    inline static const auto plane_n = BitRange<u8>{8, 13}; ///< Defines RAMPNx bits.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  MapRotationParameterAPlaneOP
///
/// \brief  Map (Rotation Parameter A, Plane O,P) register (MPOPRA).
///
/// \author Runik
/// \date   23/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class MapRotationParameterAPlaneOP : public Register {
  public:
    using Register::Register;
    inline static const auto plane_o = BitRange<u8>{0, 5};  ///< Defines RAMPOx bits.
    inline static const auto plane_p = BitRange<u8>{8, 13}; ///< Defines RAMPPx bits.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  MapRotationParameterBPlaneAB
///
/// \brief  Map (Rotation Parameter B, Plane A,B) register (MPABRB).
///
/// \author Runik
/// \date   23/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class MapRotationParameterBPlaneAB : public Register {
  public:
    using Register::Register;
    inline static const auto plane_a = BitRange<u8>{0, 5};  ///< Defines RBMPAx bits.
    inline static const auto plane_b = BitRange<u8>{8, 13}; ///< Defines RBMPBx bits.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  MapRotationParameterBPlaneCD
///
/// \brief  Map (Rotation Parameter B, Plane C,D) register (MPCDRB).
///
/// \author Runik
/// \date   23/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class MapRotationParameterBPlaneCD : public Register {
  public:
    using Register::Register;
    inline static const auto plane_c = BitRange<u8>{0, 5};  ///< Defines RBMPCx bits.
    inline static const auto plane_d = BitRange<u8>{8, 13}; ///< Defines RBMPDx bits.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  MapRotationParameterBPlaneEF
///
/// \brief  Map (Rotation Parameter B, Plane E,F) register (MPEFRB).
///
/// \author Runik
/// \date   23/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class MapRotationParameterBPlaneEF : public Register {
  public:
    using Register::Register;
    inline static const auto plane_e = BitRange<u8>{0, 5};  ///< Defines RBMPEx bits.
    inline static const auto plane_f = BitRange<u8>{8, 13}; ///< Defines RBMPFx bits.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  MapRotationParameterBPlaneGH
///
/// \brief  Map (Rotation Parameter B, Plane G,H) register (MPGHRB).
///
/// \author Runik
/// \date   23/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class MapRotationParameterBPlaneGH : public Register {
  public:
    using Register::Register;
    inline static const auto plane_g = BitRange<u8>{0, 5};  ///< Defines RBMPGx bits.
    inline static const auto plane_h = BitRange<u8>{8, 13}; ///< Defines RBMPHx bits.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  MapRotationParameterBPlaneIJ
///
/// \brief  Map (Rotation Parameter B, Plane I,J) register (MPIJRB).
///
/// \author Runik
/// \date   23/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class MapRotationParameterBPlaneIJ : public Register {
  public:
    using Register::Register;
    inline static const auto plane_i = BitRange<u8>{0, 5};  ///< Defines RBMPIx bits.
    inline static const auto plane_j = BitRange<u8>{8, 13}; ///< Defines RBMPJx bits.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  MapRotationParameterBPlaneKL
///
/// \brief  Map (Rotation Parameter B, Plane K,L) register (MPKLRB).
///
/// \author Runik
/// \date   23/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class MapRotationParameterBPlaneKL : public Register {
  public:
    using Register::Register;
    inline static const auto plane_k = BitRange<u8>{0, 5};  ///< Defines RBMPKx bits.
    inline static const auto plane_l = BitRange<u8>{8, 13}; ///< Defines RBMPLx bits.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  MapRotationParameterBPlaneMN
///
/// \brief  Map (Rotation Parameter B, Plane M,N) register (MPMNRB).
///
/// \author Runik
/// \date   23/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class MapRotationParameterBPlaneMN : public Register {
  public:
    using Register::Register;
    inline static const auto plane_m = BitRange<u8>{0, 5};  ///< Defines RBMPMx bits.
    inline static const auto plane_n = BitRange<u8>{8, 13}; ///< Defines RBMPNx bits.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  MapRotationParameterBPlaneOP
///
/// \brief  Map (Rotation Parameter B, Plane O,P) register (MPOPRB).
///
/// \author Runik
/// \date   23/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class MapRotationParameterBPlaneOP : public Register {
  public:
    using Register::Register;
    inline static const auto plane_o = BitRange<u8>{0, 5};  ///< Defines RBMPOx bits.
    inline static const auto plane_p = BitRange<u8>{8, 13}; ///< Defines RBMPPx bits.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  ScreenScrollValueNbg0HorizontalIntegerPart
///
/// \brief  Screen Scroll Value (NBG0, Horizontal Integer Part) (SCXIN0).
///
/// \author Runik
/// \date   25/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class ScreenScrollValueNbg0HorizontalIntegerPart : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  ScreenScrollValueNbg0HorizontalFractionalPart
///
/// \brief  Screen Scroll Value (NBG0, Horizontal Fractional Part) (SCXDN0).
///
/// \author Runik
/// \date   25/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class ScreenScrollValueNbg0HorizontalFractionalPart : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  ScreenScrollValueNbg0VerticalIntegerPart
///
/// \brief  Screen Scroll Value (NBG0, Vertical Integer Part) (SCYIN0).
///
/// \author Runik
/// \date   25/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class ScreenScrollValueNbg0VerticalIntegerPart : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  ScreenScrollValueNbg0VerticalFractionalPart
///
/// \brief  Screen Scroll Value (NBG0, Vertical Fractional Part) (SCYDN0).
///
/// \author Runik
/// \date   25/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class ScreenScrollValueNbg0VerticalFractionalPart : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  CoordinateIncrementNbg0HorizontalIntegerPart
///
/// \brief  Coordinate Increment (NBG0, Horizontal Integer Part) (ZMXIN0).
///
/// \author Runik
/// \date   25/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class CoordinateIncrementNbg0HorizontalIntegerPart : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  CoordinateIncrementNbg0HorizontalFractionalPart
///
/// \brief  Coordinate Increment (NBG0, Horizontal Fractional Part) (ZMXDN0).
///
/// \author Runik
/// \date   25/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class CoordinateIncrementNbg0HorizontalFractionalPart : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  CoordinateIncrementNbg0VerticalIntegerPart
///
/// \brief  CoordinateIncrement (NBG0, Vertical Integer Part) (ZMYIN0).
///
/// \author Runik
/// \date   25/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class CoordinateIncrementNbg0VerticalIntegerPart : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  CoordinateIncrementNbg0VerticalFractionalPart
///
/// \brief  Coordinate Increment (NBG0, Vertical Fractional Part) (ZMYDN0).
///
/// \author Runik
/// \date   25/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class CoordinateIncrementNbg0VerticalFractionalPart : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  ScreenScrollValueNbg1HorizontalIntegerPart
///
/// \brief  Screen Scroll Value (NBG1, Horizontal Integer Part) (SCXIN1).
///
/// \author Runik
/// \date   25/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class ScreenScrollValueNbg1HorizontalIntegerPart : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  ScreenScrollValueNbg1HorizontalFractionalPart
///
/// \brief  Screen Scroll Value (NBG1, Horizontal Fractional Part) (SCXDN1).
///
/// \author Runik
/// \date   25/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class ScreenScrollValueNbg1HorizontalFractionalPart : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  ScreenScrollValueNbg1VerticalIntegerPart
///
/// \brief  Screen Scroll Value (NBG1, Vertical Integer Part) (SCYIN1).
///
/// \author Runik
/// \date   25/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class ScreenScrollValueNbg1VerticalIntegerPart : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  ScreenScrollValueNbg1VerticalFractionalPart
///
/// \brief  Screen Scroll Value (NBG1, Vertical Fractional Part) (SCYDN1).
///
/// \author Runik
/// \date   25/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class ScreenScrollValueNbg1VerticalFractionalPart : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  CoordinateIncrementNbg1HorizontalIntegerPart
///
/// \brief  Coordinate Increment (NBG1, Horizontal Integer Part) (ZMXIN1).
///
/// \author Runik
/// \date   25/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class CoordinateIncrementNbg1HorizontalIntegerPart : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  CoordinateIncrementNbg1HorizontalFractionalPart
///
/// \brief  Coordinate Increment (NBG1, Horizontal Fractional Part) (ZMXDN1).
///
/// \author Runik
/// \date   25/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class CoordinateIncrementNbg1HorizontalFractionalPart : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  CoordinateIncrementNbg1VerticalIntegerPart
///
/// \brief  CoordinateIncrement (NBG1, Vertical Integer Part) (ZMYIN1).
///
/// \author Runik
/// \date   25/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class CoordinateIncrementNbg1VerticalIntegerPart : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  CoordinateIncrementNbg1VerticalFractionalPart
///
/// \brief  Coordinate Increment (NBG1, Vertical Fractional Part) (ZMYDN1).
///
/// \author Runik
/// \date   25/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class CoordinateIncrementNbg1VerticalFractionalPart : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  ScreenScrollValueNbg2Horizontal
///
/// \brief  Screen Scroll Value (NBG2, Horizontal) (SCXN2).
///
/// \author Runik
/// \date   25/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class ScreenScrollValueNbg2Horizontal : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  ScreenScrollValueNbg2Vertical
///
/// \brief  Screen Scroll Value (NBG2, Vertical) (SCYN2).
///
/// \author Runik
/// \date   25/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class ScreenScrollValueNbg2Vertical : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  ScreenScrollValueNbg3Horizontal
///
/// \brief  Screen Scroll Value (NBG3, Horizontal) (SCXN3).
///
/// \author Runik
/// \date   25/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class ScreenScrollValueNbg3Horizontal : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  ScreenScrollValueNbg3Vertical
///
/// \brief  Screen Scroll Value (NBG3, Vertical) (SCYN3).
///
/// \author Runik
/// \date   25/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class ScreenScrollValueNbg3Vertical : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   ZoomQuarter
///
/// \brief  NxZMQT bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class ZoomQuarter : u8 { none = 0, up_to_one_quarter = 1 };

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   ZoomHalf
///
/// \brief  NxZMHF bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class ZoomHalf : u8 { none = 0, up_to_one_half = 1 };

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  ReductionEnable
///
/// \brief  Reduction Enable (ZMCTL).
///
/// \author Runik
/// \date   26/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class ReductionEnable : public Register {
  public:
    using Register::Register;
    inline static const auto zoom_half_nbg0    = BitRange<ZoomHalf>{0};    ///< Defines N0ZMHF bits.
    inline static const auto zoom_quarter_nbg0 = BitRange<ZoomQuarter>{1}; ///< Defines N0ZMQT bits.
    inline static const auto zoom_half_nbg1    = BitRange<ZoomHalf>{8};    ///< Defines N1ZMHF bits.
    inline static const auto zoom_quarter_nbg1 = BitRange<ZoomQuarter>{9}; ///< Defines N1ZMQT bits.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  LineAndVerticalCellScrollControl
///
/// \brief  Line And Vertical Cell Scroll Control (NBG0, NBG1) (SRCCTL).
///
/// \author Runik
/// \date   26/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class LineAndVerticalCellScrollControl : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  VerticalCellScrollTableAddress
///
/// \brief  Vertical Cell Scroll Table Address Upper (NBG0, NBG1) (VCSTAU).
///
/// \author Runik
/// \date   26/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class VerticalCellScrollTableAddressUpper : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  VerticalCellScrollTableAddressLower
///
/// \brief  Vertical Cell Scroll Table Address Lower (NBG0, NBG1) (VCSTAL).
///
/// \author Runik
/// \date   26/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class VerticalCellScrollTableAddressLower : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  LineScrollTableAddressNbg0Upper
///
/// \brief  Line Scroll Table Address Upper (NBG0) (LSTA0U).
///
/// \author Runik
/// \date   26/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class LineScrollTableAddressNbg0Upper : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  LineScrollTableAddressNbg0Lower
///
/// \brief  Line Scroll Table Address Lower (NBG0) (LSTA0L).
///
/// \author Runik
/// \date   26/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class LineScrollTableAddressNbg0Lower : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  LineScrollTableAddressNbg1Upper
///
/// \brief  Line Scroll Table Address Upper (NBG1) (LSTA1U).
///
/// \author Runik
/// \date   26/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class LineScrollTableAddressNbg1Upper : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  LineScrollTableAddressNbg1Lower
///
/// \brief  Line Scroll Table Address Lower (NBG1) (LSTA1L).
///
/// \author Runik
/// \date   26/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class LineScrollTableAddressNbg1Lower : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  LineColorScreenTableAddressUpper
///
/// \brief  Line Color Screen Table Address Upper (LCTAU).
///
/// \author Runik
/// \date   26/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class LineColorScreenTableAddressUpper : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  LineColorScreenTableAddressLower
///
/// \brief  Line Color Screen Table Address Lower (LCTAL).
///
/// \author Runik
/// \date   26/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class LineColorScreenTableAddressLower : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  BackScreenTableAddressUpper
///
/// \brief  Back Screen Table Address Upper (BKTAU).
///
/// \author Runik
/// \date   26/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class BackScreenTableAddressUpper : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  BackScreenTableAddressLower
///
/// \brief  Back Screen Table Address Lower (BKTAL).
///
/// \author Runik
/// \date   26/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class BackScreenTableAddressLower : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  RotationParameterMode
///
/// \brief  Rotation Parameter Mode (RPMD).
///
/// \author Runik
/// \date   27/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class RotationParameterMode : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  RotationParameterReadControl
///
/// \brief  Rotation Parameter Read Control (RPRCTL).
///
/// \author Runik
/// \date   27/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class RotationParameterReadControl : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  CoefficientTableControl
///
/// \brief  Coefficient Table Control (KTCTL).
///
/// \author Runik
/// \date   27/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class CoefficientTableControl : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  CoefficientTableAddressOffset
///
/// \brief  Coefficient Table Address Offset (Rotation Parameter A,B) (KTAOF).
///
/// \author Runik
/// \date   27/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class CoefficientTableAddressOffset : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  ScreenOverPatternNameA
///
/// \brief  Screen Over Pattern Name (Rotation Parameter A) (OVPNRA).
///
/// \author Runik
/// \date   27/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class ScreenOverPatternNameA : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  ScreenOverPatternNameB
///
/// \brief  Screen Over Pattern Name (Rotation Parameter B) (OVPNRB).
///
/// \author Runik
/// \date   27/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class ScreenOverPatternNameB : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  RotationParameterTableAddressUpper
///
/// \brief  Rotation Parameter Table Address Upper (RPTAU).
///
/// \author Runik
/// \date   27/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class RotationParameterTableAddressUpper : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  RotationParameterTableAddressLower
///
/// \brief  Rotation Parameter Table Address Lower (RPTAL).
///
/// \author Runik
/// \date   27/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class RotationParameterTableAddressLower : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  WindowPositionW0HorizontalStart
///
/// \brief  Window Position (W0 Horizontal Start Point) (WPSX0).
///
/// \author Runik
/// \date   27/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class WindowPositionW0HorizontalStart : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  WindowPositionW0VerticalStart
///
/// \brief  Window Position (W0 Vertical Start Point) (WPSY0).
///
/// \author Runik
/// \date   27/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class WindowPositionW0VerticalStart : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  WindowPositionW0HorizontalEnd
///
/// \brief  Window Position (W0 Horizontal End Point) (WPEX0).
///
/// \author Runik
/// \date   27/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class WindowPositionW0HorizontalEnd : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  WindowPositionW0VerticalEnd
///
/// \brief  Window Position (W0 Vertical End Point) (WPEY0).
///
/// \author Runik
/// \date   27/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class WindowPositionW0VerticalEnd : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  WindowPositionW1HorizontalStart
///
/// \brief  Window Position (W1 Horizontal Start Point) (WPSX1).
///
/// \author Runik
/// \date   27/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class WindowPositionW1HorizontalStart : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  WindowPositionW1VerticalStart
///
/// \brief  Window Position (W1 Vertical Start Point) (WPSY1).
///
/// \author Runik
/// \date   27/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class WindowPositionW1VerticalStart : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  WindowPositionW1HorizontalEnd
///
/// \brief  Window Position (W1 Horizontal End Point) (WPEX1).
///
/// \author Runik
/// \date   27/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class WindowPositionW1HorizontalEnd : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  WindowPositionW1VerticalEnd
///
/// \brief  Window Position (W1 Vertical End Point) (WPEY1).
///
/// \author Runik
/// \date   27/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class WindowPositionW1VerticalEnd : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  WindowControlA
///
/// \brief  Window Control (NBG0, NBG1) (WCTLA).
///
/// \author Runik
/// \date   27/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class WindowControlA : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  WindowControlB
///
/// \brief  Window Control (NBG2, NBG3) (WCTLB).
///
/// \author Runik
/// \date   27/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class WindowControlB : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  WindowControlC
///
/// \brief  Window Control (RBG0, SPRITE) (WCTLC).
///
/// \author Runik
/// \date   27/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class WindowControlC : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  WindowControlD
///
/// \brief  Window Control (Parameter Window, Color Calc. Window) (WCTLD).
///
/// \author Runik
/// \date   27/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class WindowControlD : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  LineWindowTableAddressW0Upper
///
/// \brief  Line Window Table Address Upper (W0) (LWTA0U).
///
/// \author Runik
/// \date   27/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class LineWindowTableAddressW0Upper : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  LineWindowTableAddressW0Lower
///
/// \brief  Line Window Table Address Lower (W0) (LWTA0L).
///
/// \author Runik
/// \date   27/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class LineWindowTableAddressW0Lower : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  LineWindowTableAddressW1Upper
///
/// \brief  Line Window Table Address Upper (W1) (LWTA1U).
///
/// \author Runik
/// \date   27/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class LineWindowTableAddressW1Upper : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  LineWindowTableAddressW1Lower
///
/// \brief  Line Window Table Address Lower (W1) (LWTA1L).
///
/// \author Runik
/// \date   27/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class LineWindowTableAddressW1Lower : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  SpriteControl
///
/// \brief  Sprite Control (SPCTL).
///
/// \author Runik
/// \date   27/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class SpriteControl : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  ShadowControl
///
/// \brief  Shadow Control (SDCTL).
///
/// \author Runik
/// \date   27/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class ShadowControl : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  ColorRamAddressOffsetA
///
/// \brief  Color RAM Address Offset (NBG0 - NBG3) (CRAOFA).
///
/// \author Runik
/// \date   27/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class ColorRamAddressOffsetA : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  ColorRamAddressOffsetB
///
/// \brief  Color RAM Address Offset (RBG0, SPRITE) (CRAOFB).
///
/// \author Runik
/// \date   27/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class ColorRamAddressOffsetB : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  LineColorScreenEnable
///
/// \brief  Line Color Screen Enable (LNCLEN).
///
/// \author Runik
/// \date   27/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class LineColorScreenEnable : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  SpecialPriorityMode
///
/// \brief  Special Priority Mode (SFPRMD).
///
/// \author Runik
/// \date   27/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class SpecialPriorityMode : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  ColorCalculationControl
///
/// \brief  Color Calculation Control (CCCTL).
///
/// \author Runik
/// \date   27/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class ColorCalculationControl : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  SpecialColorCalculationMode
///
/// \brief  Special Color Calculation Mode (SFCCMD).
///
/// \author Runik
/// \date   27/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class SpecialColorCalculationMode : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  PriorityNumberSpriteA
///
/// \brief  Priority Number (SPRITE 0,1) (PRISA).
///
/// \author Runik
/// \date   27/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class PriorityNumberSpriteA : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  PriorityNumberSpriteB
///
/// \brief  Priority Number (SPRITE 2,3) (PRISB).
///
/// \author Runik
/// \date   27/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class PriorityNumberSpriteB : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  PriorityNumberSpriteC
///
/// \brief  Priority Number (SPRITE 4,5) (PRISC).
///
/// \author Runik
/// \date   27/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class PriorityNumberSpriteC : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  PriorityNumberSpriteD
///
/// \brief  Priority Number (SPRITE 6,7) (PRISD).
///
/// \author Runik
/// \date   27/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class PriorityNumberSpriteD : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  PriorityNumberA
///
/// \brief  Priority Number A (NBG0, NBG1) (PRINA).
///
/// \author Runik
/// \date   27/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class PriorityNumberA : public Register {
  public:
    using Register::Register;
    inline static const auto nbg1 = BitRange<u8>{8, 10}; ///< Defines the priority number of NBG1 (N1PRINx).
    inline static const auto nbg0 = BitRange<u8>{0, 2};  ///< Defines the priority number of NBG0 (N0PRINx).
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  PriorityNumberB
///
/// \brief  Priority Number B (NBG2, NBG3) (PRINB).
///
/// \author Runik
/// \date   27/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class PriorityNumberB : public Register {
  public:
    using Register::Register;
    inline static const auto nbg3 = BitRange<u8>{8, 10}; ///< Defines the priority number of NBG3 (N3PRINx).
    inline static const auto nbg2 = BitRange<u8>{0, 2};  ///< Defines the priority number of NBG2 (N2PRINx).
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  PriorityNumberR
///
/// \brief  Priority Number R (Rbg0) (PRIR).
///
/// \author Runik
/// \date   27/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class PriorityNumberR : public Register {
  public:
    using Register::Register;
    inline static const auto rbg0 = BitRange<u8>{0, 2}; ///< Defines the priority number of RBG0 (R0PRINx).
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  ColorCalculationRatioSpriteA
///
/// \brief  Color Calculation Ratio (Sprite 0,1) (CCRSA).
///
/// \author Runik
/// \date   27/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class ColorCalculationRatioSpriteA : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  ColorCalculationRatioSpriteB
///
/// \brief  Color Calculation Ratio (Sprite 2,3) (CCRSB).
///
/// \author Runik
/// \date   27/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class ColorCalculationRatioSpriteB : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  ColorCalculationRatioSpriteC
///
/// \brief  Color Calculation Ratio (Sprite 4,5) (CCRSC).
///
/// \author Runik
/// \date   27/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class ColorCalculationRatioSpriteC : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  ColorCalculationRatioSpriteD
///
/// \brief  Color Calculation Ratio (Sprite 6,7) (CCRSD).
///
/// \author Runik
/// \date   27/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class ColorCalculationRatioSpriteD : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  ColorCalculationRatioNbg0Nbg1
///
/// \brief  Color Calculation Ratio (NBG0, NBG1) (CCRNA).
///
/// \author Runik
/// \date   27/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class ColorCalculationRatioNbg0Nbg1 : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  ColorCalculationRatioNbg2Nbg3
///
/// \brief  Color Calculation Ratio (NBG2, NBG3) (CCRNB).
///
/// \author Runik
/// \date   27/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class ColorCalculationRatioNbg2Nbg3 : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  ColorCalculationRatioRbg0
///
/// \brief  Color Calculation Ratio (RBG0) (CCRR).
///
/// \author Runik
/// \date   27/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class ColorCalculationRatioRbg0 : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  ColorCalculationRatioLineColorBack
///
/// \brief  Color Calculation Ratio (Line Color Screen, Back Screen) (CCRLB).
///
/// \author Runik
/// \date   27/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class ColorCalculationRatioLineColorBack : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  ColorOffsetEnable
///
/// \brief  Color Offset Enable (CLOFEN).
///
/// \author Runik
/// \date   27/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class ColorOffsetEnable : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  ColorOffsetSelect
///
/// \brief  Color Offset Select (CLOFSL).
///
/// \author Runik
/// \date   27/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class ColorOffsetSelect : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  ColorOffsetARed
///
/// \brief  Color Offset A (Red) (COAR).
///
/// \author Runik
/// \date   27/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class ColorOffsetARed : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  ColorOffsetAGreen
///
/// \brief  Color Offset A (Green) (COAG).
///
/// \author Runik
/// \date   27/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class ColorOffsetAGreen : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  ColorOffsetABlue
///
/// \brief  Color Offset A (Blue)  (COAB).
///
/// \author Runik
/// \date   27/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class ColorOffsetABlue : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  ColorOffsetBRed
///
/// \brief  Color Offset B (Red) (COBR).
///
/// \author Runik
/// \date   27/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class ColorOffsetBRed : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  ColorOffsetBGreen
///
/// \brief  Color Offset B (Green) (COBG).
///
/// \author Runik
/// \date   27/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class ColorOffsetBGreen : public Register {
  public:
    using Register::Register;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  ColorOffsetBBlue
///
/// \brief  Color Offset A (Blue) (COBB).
///
/// \author Runik
/// \date   27/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class ColorOffsetBBlue : public Register {
  public:
    using Register::Register;
};

} // namespace saturnin::video
