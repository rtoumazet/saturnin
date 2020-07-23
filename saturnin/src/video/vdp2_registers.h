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
namespace vdp2_register_address {
constexpr u32 tv_screen_mode                           = {0x25f80000};
constexpr u32 external_signal_enable                   = {0x25f80002};
constexpr u32 screen_status                            = {0x25f80004};
constexpr u32 vram_size                                = {0x25f80006};
constexpr u32 h_counter                                = {0x25f80008};
constexpr u32 v_counter                                = {0x25f8000a};
constexpr u32 reserve_1                                = {0x25f8000c};
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
constexpr u32 line_color_screen_table_address_upper    = {0x25f800a8};
constexpr u32 line_color_screen_table_address_lower    = {0x25f800aa};
constexpr u32 back_screen_table_address_upper          = {0x25f800ac};
constexpr u32 back_screen_table_address_lower          = {0x25f800ae};
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
constexpr u32 priority_number_rbg0                     = {0x25f800fc};
constexpr u32 reserve_2                                = {0x25f800fe};
constexpr u32 color_calculation_ratio_sprite_0_1       = {0x25f80100};
constexpr u32 color_calculation_ratio_sprite_2_3       = {0x25f80102};
constexpr u32 color_calculation_ratio_sprite_4_5       = {0x25f80104};
constexpr u32 color_calculation_ratio_sprite_6_7       = {0x25f80106};
constexpr u32 color_calculation_ratio_nbg0_nbg1        = {0x25f80108};
constexpr u32 color_calculation_ratio_nbg2_nbg3        = {0x25f8010a};
constexpr u32 color_calculation_ratio_rbg0             = {0x25f8010c};
constexpr u32 color_calculation_ratio_line_color_back  = {0x25f8010e};
constexpr u32 color_offset_enable                      = {0x25f80110};
constexpr u32 color_offset_select                      = {0x25f80112};
constexpr u32 color_offset_a_red                       = {0x25f80114};
constexpr u32 color_offset_a_green                     = {0x25f80116};
constexpr u32 color_offset_a_blue                      = {0x25f80118};
constexpr u32 color_offset_b_red                       = {0x25f8011a};
constexpr u32 color_offset_b_green                     = {0x25f8011c};
constexpr u32 color_offset_b_blue                      = {0x25f8011e};

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
    inline static const BitRange<Display>              display{15};                 ///< Defines DISP bit.
    inline static const BitRange<BorderColorMode>      border_color_mode{8};        ///< Defines BDCLMD bit.
    inline static const BitRange<InterlaceMode>        interlace_mode{6, 7};        ///< Defines LSMDx bit.
    inline static const BitRange<VerticalResolution>   vertical_resolution{4, 5};   ///< Defines VRESOx bit.
    inline static const BitRange<HorizontalResolution> horizontal_resolution{0, 2}; ///< Defines HRESOx bit.
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
    inline static const BitRange<ExternalLatchFlag>   external_latch_flag{9};   ///< Defines EXLTFG bit.
    inline static const BitRange<ExternalSyncFlag>    external_sync_flag{8};    ///< Defines EXSYFG bit.
    inline static const BitRange<VerticalBlankFlag>   vertical_blank_flag{3};   ///< Defines VBLANK bit.
    inline static const BitRange<HorizontalBlankFlag> horizontal_blank_flag{2}; ///< Defines HBLANK bit.
    inline static const BitRange<ScanFieldFlag>       scan_field_flag{1};       ///< Defines ODD bit.
    inline static const BitRange<TvStandardFlag>      tv_standard_flag{0};      ///< Defines PAL bit.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   TvStandardFlag
///
/// \brief  PAL bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class VramSize : u8 {
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
    inline static const BitRange<VramSize> vram_size{15};        ///< Defines VRAMSZ bit.
    inline static const BitRange<u8>       version_number{0, 3}; ///< Defines VERx bits.
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
    mode_2_rgb_8_bits_1024_colors = 0b10  ///< RGB each 8 bits, 1024 colors settings.
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
    inline static const BitRange<CoefficientTableStorage> coefficient_table_storage{15}; ///< Defines CRKTE bit.
    inline static const BitRange<ColorRamMode>            color_ram_mode{12, 13};        ///< Defines CRMDx bits.
    inline static const BitRange<VramMode>                vram_b_mode_{9};               ///< Defines VRBMD bit.
    inline static const BitRange<VramMode>                vram_a_mode_{8};               ///< Defines VRAMD bit.
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
    inline static const BitRange<VramAccessCommand> vram_a0_timing_for_t0{12, 15}; ///< Defines VCP0A0x bits.
    inline static const BitRange<VramAccessCommand> vram_a0_timing_for_t1{8, 11};  ///< Defines VCP1A0x bits.
    inline static const BitRange<VramAccessCommand> vram_a0_timing_for_t2{4, 7};   ///< Defines VCP2A0x bits.
    inline static const BitRange<VramAccessCommand> vram_a0_timing_for_t3{0, 3};   ///< Defines VCP3A0x bits.
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
    inline static const BitRange<VramAccessCommand> vram_a0_timing_for_t4{12, 15}; ///< Defines VCP4A0x bits.
    inline static const BitRange<VramAccessCommand> vram_a0_timing_for_t5{8, 11};  ///< Defines VCP5A0x bits.
    inline static const BitRange<VramAccessCommand> vram_a0_timing_for_t6{4, 7};   ///< Defines VCP6A0x bits.
    inline static const BitRange<VramAccessCommand> vram_a0_timing_for_t7{0, 3};   ///< Defines VCP7A0x bits.
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
    inline static const BitRange<VramAccessCommand> vram_a1_timing_for_t0{12, 15}; ///< Defines VCP0A1x bits.
    inline static const BitRange<VramAccessCommand> vram_a1_timing_for_t1{8, 11};  ///< Defines VCP1A1x bits.
    inline static const BitRange<VramAccessCommand> vram_a1_timing_for_t2{4, 7};   ///< Defines VCP2A1x bits.
    inline static const BitRange<VramAccessCommand> vram_a1_timing_for_t3{0, 3};   ///< Defines VCP3A1x bits.
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
    inline static const BitRange<VramAccessCommand> vram_a1_timing_for_t4{12, 15}; ///< Defines VCP4A1x bits.
    inline static const BitRange<VramAccessCommand> vram_a1_timing_for_t5{8, 11};  ///< Defines VCP5A1x bits.
    inline static const BitRange<VramAccessCommand> vram_a1_timing_for_t6{4, 7};   ///< Defines VCP6A1x bits.
    inline static const BitRange<VramAccessCommand> vram_a1_timing_for_t7{0, 3};   ///< Defines VCP7A1x bits.
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
    inline static const BitRange<VramAccessCommand> vram_b0_timing_for_t0{12, 15}; ///< Defines VCP0B0x bits.
    inline static const BitRange<VramAccessCommand> vram_b0_timing_for_t1{8, 11};  ///< Defines VCP1B0x bits.
    inline static const BitRange<VramAccessCommand> vram_b0_timing_for_t2{4, 7};   ///< Defines VCP2B0x bits.
    inline static const BitRange<VramAccessCommand> vram_b0_timing_for_t3{0, 3};   ///< Defines VCP3B0x bits.
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
    inline static const BitRange<VramAccessCommand> vram_b0_timing_for_t4{12, 15}; ///< Defines VCP4B0x bits.
    inline static const BitRange<VramAccessCommand> vram_b0_timing_for_t5{8, 11};  ///< Defines VCP5B0x bits.
    inline static const BitRange<VramAccessCommand> vram_b0_timing_for_t6{4, 7};   ///< Defines VCP6B0x bits.
    inline static const BitRange<VramAccessCommand> vram_b0_timing_for_t7{0, 3};   ///< Defines VCP7B0x bits.
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
    inline static const BitRange<VramAccessCommand> vram_b1_timing_for_t0{12, 15}; ///< Defines VCP0B1x bits.
    inline static const BitRange<VramAccessCommand> vram_b1_timing_for_t1{8, 11};  ///< Defines VCP1B1x bits.
    inline static const BitRange<VramAccessCommand> vram_b1_timing_for_t2{4, 7};   ///< Defines VCP2B1x bits.
    inline static const BitRange<VramAccessCommand> vram_b1_timing_for_t3{0, 3};   ///< Defines VCP3B1x bits.
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
    inline static const BitRange<VramAccessCommand> vram_b1_timing_for_t4{12, 15}; ///< Defines VCP4B1x bits.
    inline static const BitRange<VramAccessCommand> vram_b1_timing_for_t5{8, 11};  ///< Defines VCP5B1x bits.
    inline static const BitRange<VramAccessCommand> vram_b1_timing_for_t6{4, 7};   ///< Defines VCP6B1x bits.
    inline static const BitRange<VramAccessCommand> vram_b1_timing_for_t7{0, 3};   ///< Defines VCP7B1x bits.
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
    inline static const BitRange<TransparentDisplayEnable> transparency_display_enable_rbg0{12}; ///< Defines R0TPON bit.
    inline static const BitRange<TransparentDisplayEnable> transparency_display_enable_nbg3{11}; ///< Defines N3TPON bit.
    inline static const BitRange<TransparentDisplayEnable> transparency_display_enable_nbg2{10}; ///< Defines N2TPON bit.
    inline static const BitRange<TransparentDisplayEnable> transparency_display_enable_nbg1{9};  ///< Defines N1TPON bit.
    inline static const BitRange<TransparentDisplayEnable> transparency_display_enable_nbg0{8};  ///< Defines N0TPON bit.
    inline static const BitRange<ScreenDisplayEnableBit>   screen_display_enable_rbg1{5};        ///< Defines R1ON bit.
    inline static const BitRange<ScreenDisplayEnableBit>   screen_display_enable_rbg0{4};        ///< Defines R0ON bit.
    inline static const BitRange<ScreenDisplayEnableBit>   screen_display_enable_nbg3{3};        ///< Defines N3ON bit.
    inline static const BitRange<ScreenDisplayEnableBit>   screen_display_enable_nbg2{2};        ///< Defines N2ON bit.
    inline static const BitRange<ScreenDisplayEnableBit>   screen_display_enable_nbg1{1};        ///< Defines N1ON bit.
    inline static const BitRange<ScreenDisplayEnableBit>   screen_display_enable_nbg0{0};        ///< Defines N0ON bit.
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
    inline static const BitRange<CharacterColorNumber2Bits> character_color_number_nbg1{12, 13}; ///< Defines N1CHCNx bits.
    inline static const BitRange<CharacterColorNumber3bits> character_color_number_nbg0{4, 6};   ///< Defines N0CHCNx bits.
    inline static const BitRange<BitmapSize2Bits>           bitmap_size_nbg1{10, 11};            ///< Defines N1BMSZx bits.
    inline static const BitRange<BitmapSize2Bits>           bitmap_size_nbg0{2, 3};              ///< Defines N0BMSZx bits.
    inline static const BitRange<BitmapEnable>              bitmap_enable_nbg0{1};               ///< Defines N0BMEN bit.
    inline static const BitRange<BitmapEnable>              bitmap_enable_nbg1{9};               ///< Defines N1BMEN bit.
    inline static const BitRange<CharacterSize>             character_size_nbg0{0};              ///< Defines N0CHSZ bit.
    inline static const BitRange<CharacterSize>             character_size_nbg1{8};              ///< Defines N1CHSZ bit.
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
    inline static const BitRange<CharacterColorNumber3bits> character_color_number_rbg0{12, 14}; ///< Defines R0CHCNx bits.
    inline static const BitRange<CharacterColorNumber1Bit>  character_color_number_nbg3{5};      ///< Defines N3CHCNx bit.
    inline static const BitRange<CharacterColorNumber1Bit>  character_color_number_nbg2{1};      ///< Defines N2CHCNx bit.
    inline static const BitRange<BitmapSize1Bit>            bitmap_size_rbg0{2};                 ///< Defines R0BMSZ bit.
    inline static const BitRange<BitmapEnable>              bitmap_enable_rbg0{9};               ///< Defines R0BMEN bit.
    inline static const BitRange<CharacterSize>             character_size_nbg2{0};              ///< Defines N2CHSZ bit.
    inline static const BitRange<CharacterSize>             character_size_nbg3{4};              ///< Defines N3CHSZ bit.
    inline static const BitRange<CharacterSize>             character_size_rbg0{8};              ///< Defines R0CHSZ bit.
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
    inline static const BitRange<u8> bitmap_special_priority_nbg0{5};           ///< Defines N0BMPR bit.
    inline static const BitRange<u8> bitmap_special_priority_nbg1{13};          ///< Defines N1BMPR bit.
    inline static const BitRange<u8> bitmap_special_color_calculation_nbg0{4};  ///< Defines N0BMCC bit.
    inline static const BitRange<u8> bitmap_special_color_calculation_nbg1{12}; ///< Defines N1BMCC bit.
    inline static const BitRange<u8> bitmap_palette_number_nbg0{0, 2};          ///< Defines N0BMPx bit.
    inline static const BitRange<u8> bitmap_palette_number_nbg1{8, 10};         ///< Defines N1BMPx bit.
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
    inline static const BitRange<u8> bitmap_special_priority_rbg0{5};          ///< Defines N0BMPR bit.
    inline static const BitRange<u8> bitmap_special_color_calculation_rbg0{4}; ///< Defines R0BMCC bit.
    inline static const BitRange<u8> bitmap_palette_number_rbg0{0, 2};         ///< Defines R0BMPx bit.
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
    inline static const BitRange<PatternNameDataSize>           pattern_name_data_size{15};           ///< Defines N0PNB bit.
    inline static const BitRange<CharacterNumberSupplementMode> character_number_mode{14};            ///< Defines N0CNSM bit.
    inline static const BitRange<u8>                            special_color_calculation{8};         ///< Defines N0SCC bit.
    inline static const BitRange<u8>                            supplementary_palette_number{5, 7};   ///< Defines N0SPLTx bits.
    inline static const BitRange<u8>                            supplementary_character_number{0, 4}; ///< Defines N0SCNx bits.
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
    inline static const BitRange<PatternNameDataSize>           pattern_name_data_size{15};           ///< Defines N1PNB bit.
    inline static const BitRange<CharacterNumberSupplementMode> character_number_mode{14};            ///< Defines N1CNSM bit.
    inline static const BitRange<u8>                            special_color_calculation{8};         ///< Defines N1SCC bit.
    inline static const BitRange<u8>                            supplementary_palette_number{5, 7};   ///< Defines N1SPLTx bits.
    inline static const BitRange<u8>                            supplementary_character_number{0, 4}; ///< Defines N1SCNx bits.
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
    inline static const BitRange<PatternNameDataSize>           pattern_name_data_size{15};           ///< Defines N2PNB bit.
    inline static const BitRange<CharacterNumberSupplementMode> character_number_mode{14};            ///< Defines N2CNSM bit.
    inline static const BitRange<u8>                            special_color_calculation{8};         ///< Defines N2SCC bit.
    inline static const BitRange<u8>                            supplementary_palette_number{5, 7};   ///< Defines N2SPLTx bits.
    inline static const BitRange<u8>                            supplementary_character_number{0, 4}; ///< Defines N2SCNx bits.
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
    inline static const BitRange<PatternNameDataSize>           pattern_name_data_size{15};           ///< Defines N3PNB bit.
    inline static const BitRange<CharacterNumberSupplementMode> character_number_mode{14};            ///< Defines N3CNSM bit.
    inline static const BitRange<u8>                            special_color_calculation{8};         ///< Defines N3SCC bit.
    inline static const BitRange<u8>                            supplementary_palette_number{5, 7};   ///< Defines N3SPLTx bits.
    inline static const BitRange<u8>                            supplementary_character_number{0, 4}; ///< Defines N3SCNx bits.
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
    inline static const BitRange<PatternNameDataSize>           pattern_name_data_size{15};           ///< Defines R0PNB bit.
    inline static const BitRange<CharacterNumberSupplementMode> character_number_mode{14};            ///< Defines R0CNSM bit.
    inline static const BitRange<u8>                            special_color_calculation{8};         ///< Defines R0SCC bit.
    inline static const BitRange<u8>                            supplementary_palette_number{5, 7};   ///< Defines R0SPLTx bits.
    inline static const BitRange<u8>                            supplementary_character_number{0, 4}; ///< Defines R0SCNx bits.
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
    inline static const BitRange<PlaneSize>         plane_size_nbg0{0, 1};   ///< Defines N0PLSZx bits.
    inline static const BitRange<PlaneSize>         plane_size_nbg1{2, 3};   ///< Defines N1PLSZx bits.
    inline static const BitRange<PlaneSize>         plane_size_nbg2{4, 5};   ///< Defines N2PLSZx bits.
    inline static const BitRange<PlaneSize>         plane_size_nbg3{6, 7};   ///< Defines N3PLSZx bits.
    inline static const BitRange<PlaneSize>         plane_size_rpa{8, 9};    ///< Defines RAPLSZx bits.
    inline static const BitRange<PlaneSize>         plane_size_rpb{12, 13};  ///< Defines RBPLSZx bits.
    inline static const BitRange<ScreenOverProcess> screen_over_rpa{10, 11}; ///< Defines RAOVRx bits.
    inline static const BitRange<ScreenOverProcess> screen_over_rpb{14, 15}; ///< Defines RBOVRx bits.
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
    inline static const BitRange<u8> map_offset_nbg0{0, 2};   ///< Defines N0MPx bits.
    inline static const BitRange<u8> map_offset_nbg1{4, 6};   ///< Defines N1MPx bits.
    inline static const BitRange<u8> map_offset_nbg2{8, 10};  ///< Defines N2MPx bits.
    inline static const BitRange<u8> map_offset_nbg3{12, 14}; ///< Defines N3MPx bits.
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
    inline static const BitRange<u8> map_offset_rpa{0, 2}; ///< Defines RAMPx bits.
    inline static const BitRange<u8> map_offset_rpb{4, 6}; ///< Defines RBMPx bits.
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
    inline static const BitRange<u8> plane_a{0, 5};  ///< Defines N0MPAx bits.
    inline static const BitRange<u8> plane_b{8, 13}; ///< Defines N0MPBx bits.
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
    inline static const BitRange<u8> plane_c{0, 5};  ///< Defines N0MPCx bits.
    inline static const BitRange<u8> plane_d{8, 13}; ///< Defines N0MPDx bits.
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
    inline static const BitRange<u8> plane_a{0, 5};  ///< Defines N1MPAx bits.
    inline static const BitRange<u8> plane_b{8, 13}; ///< Defines N1MPBx bits.
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
    inline static const BitRange<u8> plane_c{0, 5};  ///< Defines N1MPCx bits.
    inline static const BitRange<u8> plane_d{8, 13}; ///< Defines N1MPDx bits.
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
    inline static const BitRange<u8> plane_a{0, 5};  ///< Defines N2MPAx bits.
    inline static const BitRange<u8> plane_b{8, 13}; ///< Defines N2MPBx bits.
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
    inline static const BitRange<u8> plane_c{0, 5};  ///< Defines N2MPCx bits.
    inline static const BitRange<u8> plane_d{8, 13}; ///< Defines N2MPDx bits.
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
    inline static const BitRange<u8> plane_a{0, 5};  ///< Defines N3MPAx bits.
    inline static const BitRange<u8> plane_b{8, 13}; ///< Defines N3MPBx bits.
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
    inline static const BitRange<u8> plane_c{0, 5};  ///< Defines N3MPCx bits.
    inline static const BitRange<u8> plane_d{8, 13}; ///< Defines N3MPDx bits.
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
    inline static const BitRange<u8> plane_a{0, 5};  ///< Defines RAMPAx bits.
    inline static const BitRange<u8> plane_b{8, 13}; ///< Defines RAMPBx bits.
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
    inline static const BitRange<u8> plane_c{0, 5};  ///< Defines RAMPCx bits.
    inline static const BitRange<u8> plane_d{8, 13}; ///< Defines RAMPDx bits.
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
    inline static const BitRange<u8> plane_e{0, 5};  ///< Defines RAMPEx bits.
    inline static const BitRange<u8> plane_f{8, 13}; ///< Defines RAMPFx bits.
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
    inline static const BitRange<u8> plane_g{0, 5};  ///< Defines RAMPGx bits.
    inline static const BitRange<u8> plane_h{8, 13}; ///< Defines RAMPHx bits.
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
    inline static const BitRange<u8> plane_i{0, 5};  ///< Defines RAMPIx bits.
    inline static const BitRange<u8> plane_j{8, 13}; ///< Defines RAMPJx bits.
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
    inline static const BitRange<u8> plane_k{0, 5};  ///< Defines RAMPKx bits.
    inline static const BitRange<u8> plane_l{8, 13}; ///< Defines RAMPLx bits.
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
    inline static const BitRange<u8> plane_m{0, 5};  ///< Defines RAMPMx bits.
    inline static const BitRange<u8> plane_n{8, 13}; ///< Defines RAMPNx bits.
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
    inline static const BitRange<u8> plane_o{0, 5};  ///< Defines RAMPOx bits.
    inline static const BitRange<u8> plane_p{8, 13}; ///< Defines RAMPPx bits.
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
    inline static const BitRange<u8> plane_a{0, 5};  ///< Defines RBMPAx bits.
    inline static const BitRange<u8> plane_b{8, 13}; ///< Defines RBMPBx bits.
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
    inline static const BitRange<u8> plane_c{0, 5};  ///< Defines RBMPCx bits.
    inline static const BitRange<u8> plane_d{8, 13}; ///< Defines RBMPDx bits.
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
    inline static const BitRange<u8> plane_e{0, 5};  ///< Defines RBMPEx bits.
    inline static const BitRange<u8> plane_f{8, 13}; ///< Defines RBMPFx bits.
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
    inline static const BitRange<u8> plane_g{0, 5};  ///< Defines RBMPGx bits.
    inline static const BitRange<u8> plane_h{8, 13}; ///< Defines RBMPHx bits.
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
    inline static const BitRange<u8> plane_i{0, 5};  ///< Defines RBMPIx bits.
    inline static const BitRange<u8> plane_j{8, 13}; ///< Defines RBMPJx bits.
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
    inline static const BitRange<u8> plane_k{0, 5};  ///< Defines RBMPKx bits.
    inline static const BitRange<u8> plane_l{8, 13}; ///< Defines RBMPLx bits.
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
    inline static const BitRange<u8> plane_m{0, 5};  ///< Defines RBMPMx bits.
    inline static const BitRange<u8> plane_n{8, 13}; ///< Defines RBMPNx bits.
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
    inline static const BitRange<u8> plane_o{0, 5};  ///< Defines RBMPOx bits.
    inline static const BitRange<u8> plane_p{8, 13}; ///< Defines RBMPPx bits.
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
    inline static const BitRange<ZoomHalf>    zoom_half_nbg0{0};    ///< Defines N0ZMHF bits.
    inline static const BitRange<ZoomQuarter> zoom_quarter_nbg0{1}; ///< Defines N0ZMQT bits.
    inline static const BitRange<ZoomHalf>    zoom_half_nbg1{8};    ///< Defines N1ZMHF bits.
    inline static const BitRange<ZoomQuarter> zoom_quarter_nbg1{9}; ///< Defines N1ZMQT bits.
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
    inline static const BitRange<u8> nbg1{8, 10}; ///< Defines the priority number of NBG1 (N1PRINx).
    inline static const BitRange<u8> nbg0{0, 2};  ///< Defines the priority number of NBG0 (N0PRINx).
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
    inline static const BitRange<u8> nbg3{8, 10}; ///< Defines the priority number of NBG3 (N3PRINx).
    inline static const BitRange<u8> nbg2{0, 2};  ///< Defines the priority number of NBG2 (N2PRINx).
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
    inline static const BitRange<u8> rbg0{0, 2}; ///< Defines the priority number of RBG0 (R0PRINx).
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
