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
#include <saturnin/src/bit_register.h>

namespace saturnin::video {
namespace vdp2_register_address {
// Real address base is 0x25f80000
constexpr auto tv_screen_mode                           = u16{0x0000};
constexpr auto external_signal_enable                   = u16{0x0002};
constexpr auto screen_status                            = u16{0x0004};
constexpr auto vram_size                                = u16{0x0006};
constexpr auto h_counter                                = u16{0x0008};
constexpr auto v_counter                                = u16{0x000a};
constexpr auto reserve_1                                = u16{0x000c};
constexpr auto ram_control                              = u16{0x000e};
constexpr auto vram_cycle_pattern_bank_a0_lower         = u16{0x0010};
constexpr auto vram_cycle_pattern_bank_a0_upper         = u16{0x0012};
constexpr auto vram_cycle_pattern_bank_a1_lower         = u16{0x0014};
constexpr auto vram_cycle_pattern_bank_a1_upper         = u16{0x0016};
constexpr auto vram_cycle_pattern_bank_b0_lower         = u16{0x0018};
constexpr auto vram_cycle_pattern_bank_b0_upper         = u16{0x001a};
constexpr auto vram_cycle_pattern_bank_b1_lower         = u16{0x001c};
constexpr auto vram_cycle_pattern_bank_b1_upper         = u16{0x001e};
constexpr auto screen_display_enable                    = u16{0x0020};
constexpr auto mosaic_control                           = u16{0x0022};
constexpr auto special_function_code_select             = u16{0x0024};
constexpr auto special_function_code                    = u16{0x0026};
constexpr auto character_control_a                      = u16{0x0028};
constexpr auto character_control_b                      = u16{0x002a};
constexpr auto bitmap_palette_number_a                  = u16{0x002c};
constexpr auto bitmap_palette_number_b                  = u16{0x002e};
constexpr auto pattern_name_control_nbg0                = u16{0x0030};
constexpr auto pattern_name_control_nbg1                = u16{0x0032};
constexpr auto pattern_name_control_nbg2                = u16{0x0034};
constexpr auto pattern_name_control_nbg3                = u16{0x0036};
constexpr auto pattern_name_control_rbg0                = u16{0x0038};
constexpr auto plane_size                               = u16{0x003a};
constexpr auto map_offset_n                             = u16{0x003c};
constexpr auto map_offset_r                             = u16{0x003e};
constexpr auto map_nbg0_plane_a_b                       = u16{0x0040};
constexpr auto map_nbg0_plane_c_d                       = u16{0x0042};
constexpr auto map_nbg1_plane_a_b                       = u16{0x0044};
constexpr auto map_nbg1_plane_c_d                       = u16{0x0046};
constexpr auto map_nbg2_plane_a_b                       = u16{0x0048};
constexpr auto map_nbg2_plane_c_d                       = u16{0x004a};
constexpr auto map_nbg3_plane_a_b                       = u16{0x004c};
constexpr auto map_nbg3_plane_c_d                       = u16{0x004e};
constexpr auto map_rotation_parameter_a_plane_a_b       = u16{0x0050};
constexpr auto map_rotation_parameter_a_plane_c_d       = u16{0x0052};
constexpr auto map_rotation_parameter_a_plane_e_f       = u16{0x0054};
constexpr auto map_rotation_parameter_a_plane_g_h       = u16{0x0056};
constexpr auto map_rotation_parameter_a_plane_i_j       = u16{0x0058};
constexpr auto map_rotation_parameter_a_plane_k_l       = u16{0x005a};
constexpr auto map_rotation_parameter_a_plane_m_n       = u16{0x005c};
constexpr auto map_rotation_parameter_a_plane_o_p       = u16{0x005e};
constexpr auto map_rotation_parameter_b_plane_a_b       = u16{0x0060};
constexpr auto map_rotation_parameter_b_plane_c_d       = u16{0x0062};
constexpr auto map_rotation_parameter_b_plane_e_f       = u16{0x0064};
constexpr auto map_rotation_parameter_b_plane_g_h       = u16{0x0066};
constexpr auto map_rotation_parameter_b_plane_i_j       = u16{0x0068};
constexpr auto map_rotation_parameter_b_plane_k_l       = u16{0x006a};
constexpr auto map_rotation_parameter_b_plane_m_n       = u16{0x006c};
constexpr auto map_rotation_parameter_b_plane_o_p       = u16{0x006e};
constexpr auto screen_scroll_value_nbg0_h_int_part      = u16{0x0070};
constexpr auto screen_scroll_value_nbg0_h_fract_part    = u16{0x0072};
constexpr auto screen_scroll_value_nbg0_v_int_part      = u16{0x0074};
constexpr auto screen_scroll_value_nbg0_v_fract_part    = u16{0x0076};
constexpr auto coordinate_increment_nbg0_h_int_part     = u16{0x0078};
constexpr auto coordinate_increment_nbg0_h_fract_part   = u16{0x007a};
constexpr auto coordinate_increment_nbg0_v_int_part     = u16{0x007c};
constexpr auto coordinate_increment_nbg0_v_fract_part   = u16{0x007e};
constexpr auto screen_scroll_value_nbg1_h_int_part      = u16{0x0080};
constexpr auto screen_scroll_value_nbg1_h_fract_part    = u16{0x0082};
constexpr auto screen_scroll_value_nbg1_v_int_part      = u16{0x0084};
constexpr auto screen_scroll_value_nbg1_v_fract_part    = u16{0x0086};
constexpr auto coordinate_increment_nbg1_h_int_part     = u16{0x0088};
constexpr auto coordinate_increment_nbg1_h_fract_part   = u16{0x008a};
constexpr auto coordinate_increment_nbg1_v_int_part     = u16{0x008c};
constexpr auto coordinate_increment_nbg1_v_fract_part   = u16{0x008e};
constexpr auto screen_scroll_value_nbg2_h               = u16{0x0090};
constexpr auto screen_scroll_value_nbg2_v               = u16{0x0092};
constexpr auto screen_scroll_value_nbg3_h               = u16{0x0094};
constexpr auto screen_scroll_value_nbg3_v               = u16{0x0096};
constexpr auto reduction_enable                         = u16{0x0098};
constexpr auto line_and_vertical_cell_scroll_control    = u16{0x009a};
constexpr auto vertical_cell_scroll_table_address_upper = u16{0x009c};
constexpr auto vertical_cell_scroll_table_address_lower = u16{0x009e};
constexpr auto line_scroll_table_address_nbg0_upper     = u16{0x00a0};
constexpr auto line_scroll_table_address_nbg0_lower     = u16{0x00a2};
constexpr auto line_scroll_table_address_nbg1_upper     = u16{0x00a4};
constexpr auto line_scroll_table_address_nbg1_lower     = u16{0x00a6};
constexpr auto line_color_screen_table_address_upper    = u16{0x00a8};
constexpr auto line_color_screen_table_address_lower    = u16{0x00aa};
constexpr auto back_screen_table_address_upper          = u16{0x00ac};
constexpr auto back_screen_table_address_lower          = u16{0x00ae};
constexpr auto rotation_parameter_mode                  = u16{0x00b0};
constexpr auto rotation_parameter_read_control          = u16{0x00b2};
constexpr auto coefficient_table_control                = u16{0x00b4};
constexpr auto coefficient_table_address_offset         = u16{0x00b6};
constexpr auto screen_over_pattern_name_a               = u16{0x00b8};
constexpr auto screen_over_pattern_name_b               = u16{0x00ba};
constexpr auto rotation_parameter_table_address_upper   = u16{0x00bc};
constexpr auto rotation_parameter_table_address_lower   = u16{0x00be};
constexpr auto window_position_w0_h_start_point         = u16{0x00c0};
constexpr auto window_position_w0_v_start_point         = u16{0x00c2};
constexpr auto window_position_w0_h_end_point           = u16{0x00c4};
constexpr auto window_position_w0_v_end_point           = u16{0x00c6};
constexpr auto window_position_w1_h_start_point         = u16{0x00c8};
constexpr auto window_position_w1_v_start_point         = u16{0x00ca};
constexpr auto window_position_w1_h_end_point           = u16{0x00cc};
constexpr auto window_position_w1_v_end_point           = u16{0x00ce};
constexpr auto window_control_a                         = u16{0x00d0};
constexpr auto window_control_b                         = u16{0x00d2};
constexpr auto window_control_c                         = u16{0x00d4};
constexpr auto window_control_d                         = u16{0x00d6};
constexpr auto line_window_table_address_w0_upper       = u16{0x00d8};
constexpr auto line_window_table_address_w0_lower       = u16{0x00da};
constexpr auto line_window_table_address_w1_upper       = u16{0x00dc};
constexpr auto line_window_table_address_w1_lower       = u16{0x00de};
constexpr auto sprite_control                           = u16{0x00e0};
constexpr auto shadow_control                           = u16{0x00e2};
constexpr auto color_ram_address_offset_a               = u16{0x00e4};
constexpr auto color_ram_address_offset_b               = u16{0x00e6};
constexpr auto line_color_screen_enable                 = u16{0x00e8};
constexpr auto special_priority_mode                    = u16{0x00ea};
constexpr auto color_calculation_control                = u16{0x00ec};
constexpr auto special_color_calculation_mode           = u16{0x00ee};
constexpr auto priority_number_sprite_0_1               = u16{0x00f0};
constexpr auto priority_number_sprite_2_3               = u16{0x00f2};
constexpr auto priority_number_sprite_4_5               = u16{0x00f4};
constexpr auto priority_number_sprite_6_7               = u16{0x00f6};
constexpr auto priority_number_nbg0_nbg1                = u16{0x00f8};
constexpr auto priority_number_nbg2_nbg3                = u16{0x00fa};
constexpr auto priority_number_rbg0                     = u16{0x00fc};
constexpr auto reserve_2                                = u16{0x00fe};
constexpr auto color_calculation_ratio_sprite_0_1       = u16{0x0100};
constexpr auto color_calculation_ratio_sprite_2_3       = u16{0x0102};
constexpr auto color_calculation_ratio_sprite_4_5       = u16{0x0104};
constexpr auto color_calculation_ratio_sprite_6_7       = u16{0x0106};
constexpr auto color_calculation_ratio_nbg0_nbg1        = u16{0x0108};
constexpr auto color_calculation_ratio_nbg2_nbg3        = u16{0x010a};
constexpr auto color_calculation_ratio_rbg0             = u16{0x010c};
constexpr auto color_calculation_ratio_line_color_back  = u16{0x010e};
constexpr auto color_offset_enable                      = u16{0x0110};
constexpr auto color_offset_select                      = u16{0x0112};
constexpr auto color_offset_a_red                       = u16{0x0114};
constexpr auto color_offset_a_green                     = u16{0x0116};
constexpr auto color_offset_a_blue                      = u16{0x0118};
constexpr auto color_offset_b_red                       = u16{0x011a};
constexpr auto color_offset_b_green                     = u16{0x011c};
constexpr auto color_offset_b_blue                      = u16{0x011e};

} // namespace vdp2_register_address

struct Vdp2Regs {
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Tvmd
    ///
    /// \brief	TV screen mode register (TVMD).
    ///
    /// \author	Runik
    /// \date	13/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Tvmd {
        GENERATE_USING(Tvmd, u16);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   Display
        ///
        /// \brief  DISP bit values.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class Display : bool {
            not_displayed = false, ///< Picture is not displayed on TV screen.
            displayed     = true   ///< Picture is displayed on TV screen.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   BorderColorMode
        ///
        /// \brief  BDCLMD bit values.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class BorderColorMode : bool {
            displays_black       = false, ///< Displays black.
            displays_back_screen = true   ///< Displays back screen.
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

        GENERATE_BIT_WITH_ENUM(disp, 15, 0b1, Display);                ///< Defines DISP bit.
        GENERATE_BIT_WITH_ENUM(bdclmd, 8, 0b1, BorderColorMode);       ///< Defines BDCLMD bit.
        GENERATE_BIT_WITH_ENUM(lsmd, 6, 0b11, InterlaceMode);          ///< Defines LSMDx bit.
        GENERATE_BIT_WITH_ENUM(vreso, 4, 0b11, VerticalResolution);    ///< Defines VRESOx bit.
        GENERATE_BIT_WITH_ENUM(hreso, 0, 0b111, HorizontalResolution); ///< Defines HRESOx bit.

        static constexpr auto lo_byte_pos = PosType(0);                ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8);                ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Tvmd", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("VdpRegs::Tvmd", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using TvmdType = Reg<u16, Tvmd>;
    TvmdType tvmd;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Exten
    ///
    /// \brief	External Signal Enable register (EXTEN).
    ///
    /// \author	Runik
    /// \date	14/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Exten {
        GENERATE_USING(Exten, u16);

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vd2Regs::Exten", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2Regs::Exten", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using ExtenType = Reg<u16, Exten>;
    ExtenType exten;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Tvstat
    ///
    /// \brief	Screen Status register (TVSTAT).
    ///
    /// \author	Runik
    /// \date	14/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Tvstat {
        GENERATE_USING(Tvstat, u16);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   ExternalLatchFlag
        ///
        /// \brief  EXLTFG bit values.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class ExternalLatchFlag : bool {
            not_latched_in_register = false, ///< Not latched in register.
            latched_in_register     = true   ///< Latched in register.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   ExternalSyncFlag
        ///
        /// \brief  EXSYFG bit values.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class ExternalSyncFlag : bool {
            no_sync       = false, ///< Not synchronized.
            internal_sync = true   ///< Internal circuit synchronized.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   VerticalBlankFlag
        ///
        /// \brief  VBLANK bit values.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class VerticalBlankFlag : bool {
            during_vertical_scan    = false, ///< During vertical scan.
            during_vertical_retrace = true   ///< During vertical retrace (vblank).
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   HorizontalBlankFlag
        ///
        /// \brief  HBLANK bit values.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class HorizontalBlankFlag : bool {
            during_horizontal_scan    = false, ///< During horizontal scan.
            during_horizontal_retrace = true   ///< During horizontal retrace (hblank).
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   ScanFieldFlag
        ///
        /// \brief  ODD bit values.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class ScanFieldFlag : bool {
            during_even_field_scan = false, ///< During even field scan.
            during_odd_field_scan  = true   ///< During odd field scan.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   TvStandardFlag
        ///
        /// \brief  PAL bit values.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class TvStandardFlag : bool {
            ntsc_standard = false, ///< NTSC standard.
            pal_standard  = true   ///< PAL standard.
        };

        GENERATE_BIT_WITH_ENUM(exltfg, 9, 0b1, ExternalLatchFlag);   ///< External latch flag.
        GENERATE_BIT_WITH_ENUM(exsyfg, 8, 0b1, ExternalSyncFlag);    ///< External sync flag.
        GENERATE_BIT_WITH_ENUM(vblank, 3, 0b1, VerticalBlankFlag);   ///< Vertical blank flag.
        GENERATE_BIT_WITH_ENUM(hblank, 2, 0b1, HorizontalBlankFlag); ///< Horizontal blank flag.
        GENERATE_BIT_WITH_ENUM(odd, 1, 0b1, ScanFieldFlag);          ///< Scan field flag.
        GENERATE_BIT_WITH_ENUM(pal, 0, 0b1, TvStandardFlag);         ///< TV standard flag.

        static constexpr auto lo_byte_pos = PosType(0);              ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8);              ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vd2Regs::Exten", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2Regs::Exten", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using TvstatType = Reg<u16, Tvstat>;
    TvstatType tvstat;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Vrsize
    ///
    /// \brief	VRAM Size register (VRSIZE).
    ///
    /// \author	Runik
    /// \date	14/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Vrsize {
        GENERATE_USING(Vrsize, u16);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   TvStandardFlag
        ///
        /// \brief  PAL bit values.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class VramSize : bool {
            size_4_mbits = false, ///< 4 Mbit.
            size_8_mbits = true   ///< 8 Mbit.
        };

        GENERATE_BIT_WITH_ENUM(vramsz, 15, 0b1, VramSize); ///< VRAM size.

        GENERATE_BIT_WITHOUT_ENUM(ver, 0, 0b1111);         ///< Version number.

        static constexpr auto lo_byte_pos = PosType(0);    ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8);    ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vd2Regs::Vrsize", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2Regs::Vrsize", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using VrsizeType = Reg<u16, Vrsize>;
    VrsizeType vrsize;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Hcnt
    ///
    /// \brief	H-Counter register.
    ///
    /// \author	Runik
    /// \date	14/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Hcnt {
        GENERATE_USING(Hcnt, u16);

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vd2Regs::Hcnt", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2Regs::Hcnt", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using HcntType = Reg<u16, Hcnt>;
    HcntType hcnt;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Vcnt
    ///
    /// \brief	V-Counter register.
    ///
    /// \author	Runik
    /// \date	14/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Vcnt {
        GENERATE_USING(Vcnt, u16);

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vd2Regs::Vcnt", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2Regs::Vcnt", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using VcntType = Reg<u16, Vcnt>;
    VcntType vcnt;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Reserve
    ///
    /// \brief	Reserve register.
    ///
    /// \author	Runik
    /// \date	14/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Reserve {
        GENERATE_USING(Reserve, u16);

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vd2Regs::Reserve", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2Regs::Reserve", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using ReserveType = Reg<u16, Reserve>;
    ReserveType rsv1;
    ReserveType rsv2;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Ramctl
    ///
    /// \brief	RAM Control register (RAMCTL).
    ///
    /// \author	Runik
    /// \date	14/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Ramctl {
        GENERATE_USING(Ramctl, u16);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   CoefficientTableStorage
        ///
        /// \brief  Selects whether to store the coefficient table in the color RAM (CRKTE bit).
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class CoefficientTableStorage : bool {
            stored_in_vram      = false, ///< Coefficient table is stored in VRAM.
            stored_in_color_ram = true   ///< Coefficient table is stored in color RAM.
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
            not_set                       = 0xFF  ///< Not set.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   VramMode
        ///
        /// \brief  VRAM mode bit (VRxMD bit).
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class VramMode : bool {
            no_partition         = false, ///< Do not partition in 2 banks.
            partition_in_2_banks = true   ///< Partition in 2 banks.
        };

        enum class RotationDataBankSelect : u8 {
            not_used                             = 0b00, ///< VRAM not used as RBG0 RAM.
            used_as_rbg0_coefficient_table       = 0b01, ///< VRAM used as RBG0 coefficient table.
            used_as_rbg0_pattern_name_table      = 0b10, ///< VRAM used as RBG0 Pattern Name table.
            used_as_rbg0_character_pattern_table = 0b11  ///< VRAM used as RBG0 Character Pattern table (or Bitmap Pattern)
        };

        GENERATE_BIT_WITH_ENUM(crkte, 15, 0b1, CoefficientTableStorage); ///< Coefficient table storage.
        GENERATE_BIT_WITH_ENUM(crmd, 12, 0b11, ColorRamMode);            ///< Color RAM mode.
        GENERATE_BIT_WITH_ENUM(vrbmd, 9, 0b1, VramMode);                 ///< VRAM B mode.
        GENERATE_BIT_WITH_ENUM(vramd, 8, 0b1, VramMode);                 ///< VRAM A mode.
        GENERATE_BIT_WITH_ENUM(rdbsa0, 0, 0b11, RotationDataBankSelect); ///< VRAM A0 rotation bank select.
        GENERATE_BIT_WITH_ENUM(rdbsa1, 2, 0b11, RotationDataBankSelect); ///< VRAM A1 rotation bank select.
        GENERATE_BIT_WITH_ENUM(rdbsb0, 4, 0b11, RotationDataBankSelect); ///< VRAM B0 rotation bank select.
        GENERATE_BIT_WITH_ENUM(rdbsb1, 6, 0b11, RotationDataBankSelect); ///< VRAM B1 rotation bank select.

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vd2Regs::Ramctl", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2Regs::Ramctl", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using RamctlType = Reg<u16, Ramctl>;
    RamctlType ramctl;

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
    /// \struct	Cycxxl
    ///
    /// \brief	VRAM Cycle Pattern lower register (CYCxxL).
    ///
    /// \author	Runik
    /// \date	14/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Cycxxl {
        GENERATE_USING(Cycxxl, u16);

        GENERATE_BIT_WITH_ENUM(t0, 12, 0b1111, VramAccessCommand); ///< VCP0xxx bits.
        GENERATE_BIT_WITH_ENUM(t1, 8, 0b1111, VramAccessCommand);  ///< VCP1xxx bits.
        GENERATE_BIT_WITH_ENUM(t2, 4, 0b1111, VramAccessCommand);  ///< VCP2xxx bits.
        GENERATE_BIT_WITH_ENUM(t3, 0, 0b1111, VramAccessCommand);  ///< VCP3xxx bits.

        static constexpr auto lo_byte_pos = PosType(0);            ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8);            ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vd2Regs::Cycxxl", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2Regs::Cycxxl", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using CycxxlType = Reg<u16, Cycxxl>;
    CycxxlType cyca0l;
    CycxxlType cyca1l;
    CycxxlType cycb0l;
    CycxxlType cycb1l;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Cycxxu
    ///
    /// \brief	VRAM Cycle Pattern upper register (CYCxxU).
    ///
    /// \author	Runik
    /// \date	14/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Cycxxu {
        GENERATE_USING(Cycxxu, u16);

        GENERATE_BIT_WITH_ENUM(t4, 12, 0b1111, VramAccessCommand); ///< VCP4xxx bits.
        GENERATE_BIT_WITH_ENUM(t5, 8, 0b1111, VramAccessCommand);  ///< VCP5xxx bits.
        GENERATE_BIT_WITH_ENUM(t6, 4, 0b1111, VramAccessCommand);  ///< VCP6xxx bits.
        GENERATE_BIT_WITH_ENUM(t7, 0, 0b1111, VramAccessCommand);  ///< VCP7xxx bits.

        static constexpr auto lo_byte_pos = PosType(0);            ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8);            ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vd2Regs::Cycxxu", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2Regs::Cycxxu", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using CycxxuType = Reg<u16, Cycxxu>;
    CycxxuType cyca0u;
    CycxxuType cyca1u;
    CycxxuType cycb0u;
    CycxxuType cycb1u;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Bgon
    ///
    /// \brief	Screen Display Enable register (BGON).
    ///
    /// \author	Runik
    /// \date	15/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Bgon {
        GENERATE_USING(Bgon, u16);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   TransparentDisplayEnable
        ///
        /// \brief  xxTPON bit values.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class TransparentDisplayEnable : bool {
            transparency_code_valid = false, ///< Validates transparency code (transparency code becomes valid).
            transparency_code_invalid
            = true ///< Invalidates transparency code (transparency code dots are displayed according to data values).
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   ScreenDisplayEnableBit
        ///
        /// \brief  xxON bit values.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class ScreenDisplayEnableBit : bool {
            cannot_display = false, ///< Cannot display (Does not execute VRAM access for display).
            can_display    = true   ///< Can display.
        };

        GENERATE_BIT_WITH_ENUM(r0tpon, 12, 0b1, TransparentDisplayEnable); ///< RBG0 transparency display enable.
        GENERATE_BIT_WITH_ENUM(n3tpon, 11, 0b1, TransparentDisplayEnable); ///< NBG3 transparency display enable.
        GENERATE_BIT_WITH_ENUM(n2tpon, 10, 0b1, TransparentDisplayEnable); ///< NBG2 transparency display enable.
        GENERATE_BIT_WITH_ENUM(n1tpon, 9, 0b1, TransparentDisplayEnable);  ///< NBG1 transparency display enable.
        GENERATE_BIT_WITH_ENUM(n0tpon, 8, 0b1, TransparentDisplayEnable);  ///< NBG0 transparency display enable.
        GENERATE_BIT_WITH_ENUM(r1on, 5, 0b1, ScreenDisplayEnableBit);      ///< RBG1 screen display enable.
        GENERATE_BIT_WITH_ENUM(r0on, 4, 0b1, ScreenDisplayEnableBit);      ///< RBG0 screen display enable.
        GENERATE_BIT_WITH_ENUM(n3on, 3, 0b1, ScreenDisplayEnableBit);      ///< NBG3 screen display enable.
        GENERATE_BIT_WITH_ENUM(n2on, 2, 0b1, ScreenDisplayEnableBit);      ///< NBG2 screen display enable.
        GENERATE_BIT_WITH_ENUM(n1on, 1, 0b1, ScreenDisplayEnableBit);      ///< NBG1 screen display enable.
        GENERATE_BIT_WITH_ENUM(n0on, 0, 0b1, ScreenDisplayEnableBit);      ///< NBG0 screen display enable.

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vd2Regs::Bgon", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2Regs::Bgon", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using BgonType = Reg<u16, Bgon>;
    BgonType bgon;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Mzctl
    ///
    /// \brief	Mosaic Control register (MZCTL).
    ///
    /// \author	Runik
    /// \date	15/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Mzctl {
        GENERATE_USING(Mzctl, u16);

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vd2Regs::Mzctl", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2Regs::Mzctl", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using MzctlType = Reg<u16, Mzctl>;
    MzctlType mzctl;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	sfsel
    ///
    /// \brief	Special Function Code Select register (SFSEL).
    ///
    /// \author	Runik
    /// \date	15/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Sfsel {
        GENERATE_USING(Sfsel, u16);

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vd2Regs::Sfsel", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2Regs::Sfsel", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using SfselType = Reg<u16, Sfsel>;
    SfselType sfsel;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Sfcode
    ///
    /// \brief	Special Function Code register (SFCODE).
    ///
    /// \author	Runik
    /// \date	15/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Sfcode {
        GENERATE_USING(Sfcode, u16);

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vd2Regs::Sfsel", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2Regs::Sfsel", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using SfcodeType = Reg<u16, Sfcode>;
    SfcodeType sfcode;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \enum   CharacterColorNumber3Bits
    ///
    /// \brief  N0CHCNx / R0CHCNx bits values.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    enum class CharacterColorNumber3Bits : u8 {
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

    enum class CharacterColorNumber1Bit : bool {
        palette_16  = false, ///< Palette format, 16 colors.
        palette_256 = true   ///< Palette format, 256 colors.
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

    enum class BitmapSize1Bit : bool {
        size_512_by_256 = false, ///< 512 H dots* 256 V dots.
        size_512_by_512 = true   ///< 512 H dots* 512 V dots.
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \enum   BitmapEnable
    ///
    /// \brief  xxBMEN bit values.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    enum class BitmapEnable : bool {
        cell_format   = false, ///< Cell Format.
        bitmap_format = true   ///< Bitmap Format.
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \enum   CharacterSize
    ///
    /// \brief  xxCHSZ bit values.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    enum class CharacterSize : bool {
        one_by_one = false, ///< 1 H Cell x 1 V Cell.
        two_by_two = true   ///< 2 H Cells x 2 V Cells.
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Chctla
    ///
    /// \brief	Character Control (NBG0, NBG1) register (CHCTLA).
    ///
    /// \author	Runik
    /// \date	15/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Chctla {
        GENERATE_USING(Chctla, u16);

        GENERATE_BIT_WITH_ENUM(n1chcn, 12, 0b11, CharacterColorNumber2Bits); ///< NBG1 character color number.
        GENERATE_BIT_WITH_ENUM(n1bmsz, 10, 0b11, BitmapSize2Bits);           ///< NBG1 bitmap size.
        GENERATE_BIT_WITH_ENUM(n1bmen, 9, 0b1, BitmapEnable);                ///< NBG1 bitmap enable.
        GENERATE_BIT_WITH_ENUM(n1chsz, 8, 0b1, CharacterSize);               ///< NBG1 character size.
        GENERATE_BIT_WITH_ENUM(n0chcn, 4, 0b111, CharacterColorNumber3Bits); ///< NBG0 character color number.
        GENERATE_BIT_WITH_ENUM(n0bmsz, 2, 0b11, BitmapSize2Bits);            ///< NBG0 bitmap size.
        GENERATE_BIT_WITH_ENUM(n0bmen, 1, 0b1, BitmapEnable);                ///< NBG0 bitmap enable.
        GENERATE_BIT_WITH_ENUM(n0chsz, 0, 0b1, CharacterSize);               ///< NBG0 character size.

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vd2Regs::Chctla", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2Regs::Chctla", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using ChctlaType = Reg<u16, Chctla>;
    ChctlaType chctla;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Chctlb
    ///
    /// \brief	Character Control (NBG2, NBG3, RBG0) register (CHCTLB).
    ///
    /// \author	Runik
    /// \date	15/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Chctlb {
        GENERATE_USING(Chctlb, u16);

        GENERATE_BIT_WITH_ENUM(r0chcn, 12, 0b111, CharacterColorNumber3Bits); ///< RBG0 character color number.
        GENERATE_BIT_WITH_ENUM(r0bmsz, 10, 0b1, BitmapSize1Bit);              ///< RBG0 bitmap size.
        GENERATE_BIT_WITH_ENUM(r0bmen, 9, 0b1, BitmapEnable);                 ///< RBG0 bitmap enable.
        GENERATE_BIT_WITH_ENUM(r0chsz, 8, 0b1, CharacterSize);                ///< RBG0 character size.
        GENERATE_BIT_WITH_ENUM(n3chcn, 5, 0b1, CharacterColorNumber1Bit);     ///< NBG3 character color number.
        GENERATE_BIT_WITH_ENUM(n3chsz, 4, 0b1, CharacterSize);                ///< NBG3 bitmap size.
        GENERATE_BIT_WITH_ENUM(n2chcn, 1, 0b1, CharacterColorNumber1Bit);     ///< NBG2 bitmap enable.
        GENERATE_BIT_WITH_ENUM(n2chsz, 0, 0b1, CharacterSize);                ///< NBG2 character size.

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vd2Regs::Chctlb", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2Regs::Chctlb", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using ChctlbType = Reg<u16, Chctlb>;
    ChctlbType chctlb;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Bmpna
    ///
    /// \brief	Bitmap Palette Number (NBG0, NBG1) register (BMPNA).
    ///
    /// \author	Runik
    /// \date	15/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Bmpna {
        GENERATE_USING(Bmpna, u16);

        GENERATE_BIT_WITHOUT_ENUM(n1bmpr, 13, 0b1);     ///< NBG1 bitmap special priority.
        GENERATE_BIT_WITHOUT_ENUM(n1bmcc, 12, 0b1);     ///< NBG1 bitmap special color calculation.
        GENERATE_BIT_WITHOUT_ENUM(n1bmpx, 8, 0b111);    ///< NBG1 bitmap palette number.
        GENERATE_BIT_WITHOUT_ENUM(n0bmpr, 5, 0b1);      ///< NBG0 bitmap special priority.
        GENERATE_BIT_WITHOUT_ENUM(n0bmcc, 4, 0b1);      ///< NBG0 bitmap special color calculation.
        GENERATE_BIT_WITHOUT_ENUM(n0bmpx, 0, 0b111);    ///< NBG0 bitmap palette number.

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vd2Regs::Bmpna", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2Regs::Bmpna", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using BmpnaType = Reg<u16, Bmpna>;
    BmpnaType bmpna;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Bmpnb
    ///
    /// \brief	Bitmap Palette Number (RBG0) register (BMPNB).
    ///
    /// \author	Runik
    /// \date	15/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Bmpnb {
        GENERATE_USING(Bmpnb, u16);

        GENERATE_BIT_WITHOUT_ENUM(r0bmpr, 5, 0b1);      ///< RBG0 bitmap special priority.
        GENERATE_BIT_WITHOUT_ENUM(r0bmcc, 4, 0b1);      ///< RBG0 bitmap special color calculation.
        GENERATE_BIT_WITHOUT_ENUM(r0bmpx, 0, 0b111);    ///< RBG0 bitmap palette number.

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vd2Regs::Bmpnb", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2Regs::Bmpnb", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using BmpnbType = Reg<u16, Bmpnb>;
    BmpnbType bmpnb;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   PatternNameDataSize
///
/// \brief  PNCNx / PNCR bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class PatternNameDataSize : bool {
    two_words = false, ///< 2 Words.
    one_word  = true   ///< 1 Word.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   CharacterNumberSupplementMode
///
/// \brief  NxCNSM / R0CNSM bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class CharacterNumberSupplementMode : bool {
    character_number_10_bits
    = false, ///< Character number in pattern name data is 10 bits. Flip function can be selected in character units.
    character_number_12_bits = true ///< Character number in pattern name data is 12 bits. Flip function cannot be used.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	PatternNameControlNbg0
///
/// \brief	Pattern Name Control (NBGx, RBG0) register (PNCxx).
///
/// \author	Runik
/// \date	24/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union PatternNameControl {
    u16            raw;                            ///< Raw representation.
    BitField<15>   pattern_name_data_size;         ///< Defines N0PNB bit.
    BitField<14>   character_number_mode;          ///< Defines N0CNSM bit.
    BitField<9>    special_priority;               ///< Defines N0SPR bit.
    BitField<8>    special_color_calculation;      ///< Defines N0SCC bit.
    BitField<5, 3> supplementary_palette_number;   ///< Defines N0SPLTx bits.
    BitField<0, 5> supplementary_character_number; ///< Defines N0SCNx bits.
    BitField<8, 8> upper_8_bits;                   ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits;                   ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   PlaneSize
///
/// \brief  xxPLSZx bits values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class PlaneSize : u8 {
    not_set     = 0xFF, ///< Not set.
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

union PlaneSizeRegister {
    u16             raw;             ///< Raw representation.
    BitField<14, 2> screen_over_rpb; ///< Defines RBOVRx bits.
    BitField<12, 2> plane_size_rpb;  ///< Defines RBPLSZx bits.
    BitField<10, 2> screen_over_rpa; ///< Defines RAOVRx bits.
    BitField<8, 2>  plane_size_rpa;  ///< Defines RAPLSZx bits.
    BitField<6, 2>  plane_size_nbg3; ///< Defines N3PLSZx bits.
    BitField<4, 2>  plane_size_nbg2; ///< Defines N2PLSZx bits.
    BitField<2, 2>  plane_size_nbg1; ///< Defines N1PLSZx bits.
    BitField<0, 2>  plane_size_nbg0; ///< Defines N0PLSZx bits.
    BitField<8, 8>  upper_8_bits;    ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8>  lower_8_bits;    ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	MapOffsetNbg
///
/// \brief	Map Offset (NBG0 - NBG3) register (MPOFN).
///
/// \author	Runik
/// \date	25/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union MapOffsetNbg {
    u16             raw;             ///< Raw representation.
    BitField<12, 3> map_offset_nbg3; ///< Defines N3MPx bits.
    BitField<8, 3>  map_offset_nbg2; ///< Defines N2MPx bits.
    BitField<4, 3>  map_offset_nbg1; ///< Defines N1MPx bits.
    BitField<0, 3>  map_offset_nbg0; ///< Defines N0MPx bits.
    BitField<8, 8>  upper_8_bits;    ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8>  lower_8_bits;    ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	MapOffsetRbg
///
/// \brief	Map Offset (Rotation Parameter A,B) register (MPOFR).
///
/// \author	Runik
/// \date	25/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union MapOffsetRbg {
    u16            raw;            ///< Raw representation.
    BitField<4, 3> map_offset_rpb; ///< Defines RBMPx bits.
    BitField<0, 3> map_offset_rpa; ///< Defines RAMPx bits.
    BitField<8, 8> upper_8_bits;   ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits;   ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	MapPlaneAB
///
/// \brief	Map (NBGx, RBGx, Plane A,B) register (MPABxx).
///
/// \author	Runik
/// \date	25/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union MapPlaneAB {
    u16            raw;          ///< Raw representation.
    BitField<0, 6> plane_a;      ///< Defines N0MPAx bits.
    BitField<8, 6> plane_b;      ///< Defines N0MPBx bits.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	MapPlaneCD
///
/// \brief	Map (NBGx, RBGx, Plane C,D) register (MPCDxx).
///
/// \author	Runik
/// \date	25/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union MapPlaneCD {
    u16            raw;          ///< Raw representation.
    BitField<0, 6> plane_c;      ///< Defines N0MPAx bits.
    BitField<8, 6> plane_d;      ///< Defines N0MPBx bits.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	MapPlaneEF
///
/// \brief	Map (Rotation Parameter x, Plane E,F) register (MPEFRx).
///
/// \author	Runik
/// \date	25/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union MapPlaneEF {
    u16            raw;          ///< Raw representation.
    BitField<0, 6> plane_e;      ///< Defines RxMPEx bits.
    BitField<8, 6> plane_f;      ///< Defines RxMPFx bits.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	MapPlaneGH
///
/// \brief	Map (Rotation Parameter x, Plane G,H) register (MPGHRx).
///
/// \author	Runik
/// \date	25/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union MapPlaneGH {
    u16            raw;          ///< Raw representation.
    BitField<0, 6> plane_g;      ///< Defines RxMPGx bits.
    BitField<8, 6> plane_h;      ///< Defines RxMPHx bits.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	MapPlaneIJ
///
/// \brief	Map (Rotation Parameter x, Plane I,J) register (MPIJRx).
///
/// \author	Runik
/// \date	25/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union MapPlaneIJ {
    u16            raw;          ///< Raw representation.
    BitField<0, 6> plane_i;      ///< Defines RxMPIx bits.
    BitField<8, 6> plane_j;      ///< Defines RxMPJx bits.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	MapPlaneKL
///
/// \brief	Map (Rotation Parameter x, Plane K,L) register (MPKLRx).
///
/// \author	Runik
/// \date	25/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union MapPlaneKL {
    u16            raw;          ///< Raw representation.
    BitField<0, 6> plane_k;      ///< Defines RxMPIx bits.
    BitField<8, 6> plane_l;      ///< Defines RxMPJx bits.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	MapPlaneMN
///
/// \brief	Map (Rotation Parameter x, Plane M,N) register (MPMNRx).
///
/// \author	Runik
/// \date	25/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union MapPlaneMN {
    u16            raw;          ///< Raw representation.
    BitField<0, 6> plane_m;      ///< Defines RxMPIx bits.
    BitField<8, 6> plane_n;      ///< Defines RxMPJx bits.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	MapPlaneOP
///
/// \brief	Map (Rotation Parameter x, Plane O,P) register (MPOPRx).
///
/// \author	Runik
/// \date	25/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union MapPlaneOP {
    u16            raw;          ///< Raw representation.
    BitField<0, 6> plane_o;      ///< Defines RxMPIx bits.
    BitField<8, 6> plane_p;      ///< Defines RxMPJx bits.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	ScreenScrollValueIntegerPart
///
/// \brief	Screen Scroll Value (Integer Part) (SCXINx).
///
/// \author	Runik
/// \date	25/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union ScreenScrollValueIntegerPart {
    u16             raw;          ///< Raw representation.
    BitField<0, 11> integer;      ///< Defines  NxSCxIx bits.
    BitField<8, 8>  upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8>  lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	ScreenScrollValueFractionalPart
///
/// \brief	Screen Scroll Value (Fractional Part) (SCXDNx).
///
/// \author	Runik
/// \date	25/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union ScreenScrollValueFractionalPart {
    u16            raw;          ///< Raw representation.
    BitField<8, 8> fractional;   ///< Defines  NxSCxDx bits.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	CoordinateIncrementNbg0HorizontalIntegerPart
///
/// \brief	Coordinate Increment (NBG0, Horizontal Integer Part) (ZMXIN0).
///
/// \author	Runik
/// \date	23/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union CoordinateIncrementNbg0HorizontalIntegerPart {
    u16            raw;          ///< Raw representation.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	CoordinateIncrementNbg0HorizontalFractionalPart
///
/// \brief	Coordinate Increment (NBG0, Horizontal Fractional Part) (ZMXDN0).
///
/// \author	Runik
/// \date	23/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union CoordinateIncrementNbg0HorizontalFractionalPart {
    u16            raw;          ///< Raw representation.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	CoordinateIncrementNbg0VerticalIntegerPart
///
/// \brief	CoordinateIncrement (NBG0, Vertical Integer Part) (ZMYIN0).
///
/// \author	Runik
/// \date	23/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union CoordinateIncrementNbg0VerticalIntegerPart {
    u16            raw;          ///< Raw representation.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	CoordinateIncrementNbg0VerticalFractionalPart
///
/// \brief	Coordinate Increment (NBG0, Vertical Fractional Part) (ZMYDN0).
///
/// \author	Runik
/// \date	23/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union CoordinateIncrementNbg0VerticalFractionalPart {
    u16            raw;          ///< Raw representation.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	CoordinateIncrementNbg1HorizontalIntegerPart
///
/// \brief	Coordinate Increment (NBG1, Horizontal Integer Part) (ZMXIN1).
///
/// \author	Runik
/// \date	23/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union CoordinateIncrementNbg1HorizontalIntegerPart {
    u16            raw;          ///< Raw representation.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	CoordinateIncrementNbg1HorizontalFractionalPart
///
/// \brief	Coordinate Increment (NBG1, Horizontal Fractional Part) (ZMXDN1).
///
/// \author	Runik
/// \date	23/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union CoordinateIncrementNbg1HorizontalFractionalPart {
    u16            raw;          ///< Raw representation.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	CoordinateIncrementNbg1VerticalIntegerPart
///
/// \brief	CoordinateIncrement (NBG1, Vertical Integer Part) (ZMYIN1).
///
/// \author	Runik
/// \date	23/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union CoordinateIncrementNbg1VerticalIntegerPart {
    u16            raw;          ///< Raw representation.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	CoordinateIncrementNbg1VerticalFractionalPart
///
/// \brief	Coordinate Increment (NBG1, Vertical Fractional Part) (ZMYDN1).
///
/// \author	Runik
/// \date	23/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union CoordinateIncrementNbg1VerticalFractionalPart {
    u16            raw;          ///< Raw representation.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   ZoomQuarter
///
/// \brief  NxZMQT bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class ZoomQuarter : bool { none = false, up_to_one_quarter = true };

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   ZoomHalf
///
/// \brief  NxZMHF bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class ZoomHalf : bool { none = false, up_to_one_half = true };

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	ReductionEnable
///
/// \brief	Reduction Enable (ZMCTL).
///
/// \author	Runik
/// \date	25/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union ReductionEnable {
    u16            raw;               ///< Raw representation.
    BitField<9>    zoom_quarter_nbg1; ///< Defines N1ZMQT bits.
    BitField<8>    zoom_half_nbg1;    ///< Defines N1ZMHF bits.
    BitField<1>    zoom_quarter_nbg0; ///< Defines N0ZMQT bits.
    BitField<0>    zoom_half_nbg0;    ///< Defines N0ZMHF bits.
    BitField<8, 8> upper_8_bits;      ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits;      ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	LineAndVerticalCellScrollControl
///
/// \brief	Line And Vertical Cell Scroll Control (NBG0, NBG1) (SRCCTL).
///
/// \author	Runik
/// \date	23/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union LineAndVerticalCellScrollControl {
    u16            raw;          ///< Raw representation.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	VerticalCellScrollTableAddressUpper
///
/// \brief	Vertical Cell Scroll Table Address Upper (NBG0, NBG1) (VCSTAU).
///
/// \author	Runik
/// \date	23/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union VerticalCellScrollTableAddressUpper {
    u16            raw;          ///< Raw representation.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	VerticalCellScrollTableAddressLower
///
/// \brief	Vertical Cell Scroll Table Address Lower (NBG0, NBG1) (VCSTAL).
///
/// \author	Runik
/// \date	23/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union VerticalCellScrollTableAddressLower {
    u16            raw;          ///< Raw representation.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	LineScrollTableAddressNbg0Upper
///
/// \brief	Line Scroll Table Address Upper (NBG0) (LSTA0U).
///
/// \author	Runik
/// \date	23/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union LineScrollTableAddressNbg0Upper {
    u16            raw;          ///< Raw representation.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	LineScrollTableAddressNbg0Lower
///
/// \brief	Line Scroll Table Address Lower (NBG0) (LSTA0L).
///
/// \author	Runik
/// \date	23/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union LineScrollTableAddressNbg0Lower {
    u16            raw;          ///< Raw representation.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	LineScrollTableAddressNbg1Upper
///
/// \brief	Line Scroll Table Address Upper (NBG1) (LSTA1U).
///
/// \author	Runik
/// \date	23/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union LineScrollTableAddressNbg1Upper {
    u16            raw;          ///< Raw representation.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	LineScrollTableAddressNbg1Lower
///
/// \brief	Line Scroll Table Address Lower (NBG1) (LSTA1L).
///
/// \author	Runik
/// \date	23/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union LineScrollTableAddressNbg1Lower {
    u16            raw;          ///< Raw representation.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	LineColorScreenTableAddressUpper
///
/// \brief	Line Color Screen Table Address Upper (LCTAU).
///
/// \author	Runik
/// \date	23/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union LineColorScreenTableAddressUpper {
    u16            raw;          ///< Raw representation.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	LineColorScreenTableAddressLower
///
/// \brief	Line Color Screen Table Address Lower (LCTAL).
///
/// \author	Runik
/// \date	23/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union LineColorScreenTableAddressLower {
    u16            raw;          ///< Raw representation.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	BackScreenTableAddressUpper
///
/// \brief	Back Screen Table Address Upper (BKTAU).
///
/// \author	Runik
/// \date	23/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union BackScreenTableAddressUpper {
    u16            raw;          ///< Raw representation.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	BackScreenTableAddressLower
///
/// \brief	Back Screen Table Address Lower (BKTAL).
///
/// \author	Runik
/// \date	23/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union BackScreenTableAddressLower {
    u16            raw;          ///< Raw representation.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	RotationParameterMode
///
/// \brief	Rotation Parameter Mode (RPMD).
///
/// \author	Runik
/// \date	23/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union RotationParameterMode {
    u16            raw;          ///< Raw representation.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	RotationParameterReadControl
///
/// \brief	Rotation Parameter Read Control (RPRCTL).
///
/// \author	Runik
/// \date	23/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union RotationParameterReadControl {
    u16            raw;          ///< Raw representation.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	CoefficientTableControl
///
/// \brief	Coefficient Table Control (KTCTL).
///
/// \author	Runik
/// \date	23/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union CoefficientTableControl {
    u16            raw;          ///< Raw representation.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	CoefficientTableAddressOffset
///
/// \brief	Coefficient Table Address Offset (Rotation Parameter A,B) (KTAOF).
///
/// \author	Runik
/// \date	23/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union CoefficientTableAddressOffset {
    u16            raw;          ///< Raw representation.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	ScreenOverPatternNameA
///
/// \brief	Screen Over Pattern Name (Rotation Parameter A) (OVPNRA).
///
/// \author	Runik
/// \date	23/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union ScreenOverPatternNameA {
    u16            raw;          ///< Raw representation.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	ScreenOverPatternNameB
///
/// \brief	Screen Over Pattern Name (Rotation Parameter B) (OVPNRB).
///
/// \author	Runik
/// \date	23/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union ScreenOverPatternNameB {
    u16            raw;          ///< Raw representation.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	RotationParameterTableAddressUpper
///
/// \brief	Rotation Parameter Table Address Upper (RPTAU).
///
/// \author	Runik
/// \date	23/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union RotationParameterTableAddressUpper {
    u16            raw;          ///< Raw representation.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	RotationParameterTableAddressLower
///
/// \brief	Rotation Parameter Table Address Lower (RPTAL).
///
/// \author	Runik
/// \date	23/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union RotationParameterTableAddressLower {
    u16            raw;          ///< Raw representation.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	WindowPositionW0HorizontalStart
///
/// \brief	Window Position (W0 Horizontal Start Point) (WPSX0).
///
/// \author	Runik
/// \date	23/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union WindowPositionW0HorizontalStart {
    u16            raw;          ///< Raw representation.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	WindowPositionW0VerticalStart
///
/// \brief	Window Position (W0 Vertical Start Point) (WPSY0).
///
/// \author	Runik
/// \date	23/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union WindowPositionW0VerticalStart {
    u16            raw;          ///< Raw representation.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	WindowPositionW0HorizontalEnd
///
/// \brief	Window Position (W0 Horizontal End Point) (WPEX0).
///
/// \author	Runik
/// \date	23/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union WindowPositionW0HorizontalEnd {
    u16            raw;          ///< Raw representation.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	WindowPositionW0VerticalEnd
///
/// \brief	Window Position (W0 Vertical End Point) (WPEY0).
///
/// \author	Runik
/// \date	23/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union WindowPositionW0VerticalEnd {
    u16            raw;          ///< Raw representation.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	WindowPositionW1HorizontalStart
///
/// \brief	Window Position (W1 Horizontal Start Point) (WPSX1).
///
/// \author	Runik
/// \date	23/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union WindowPositionW1HorizontalStart {
    u16            raw;          ///< Raw representation.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	WindowPositionW1VerticalStart
///
/// \brief	Window Position (W1 Vertical Start Point) (WPSY1).
///
/// \author	Runik
/// \date	23/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union WindowPositionW1VerticalStart {
    u16            raw;          ///< Raw representation.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	WindowPositionW1HorizontalEnd
///
/// \brief	Window Position (W1 Horizontal End Point) (WPEX1).
///
/// \author	Runik
/// \date	23/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union WindowPositionW1HorizontalEnd {
    u16            raw;          ///< Raw representation.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	WindowPositionW1VerticalEnd
///
/// \brief	Window Position (W1 Vertical End Point) (WPEY1).
///
/// \author	Runik
/// \date	23/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union WindowPositionW1VerticalEnd {
    u16            raw;          ///< Raw representation.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	WindowControlA
///
/// \brief	Window Control (NBG0, NBG1) (WCTLA).
///
/// \author	Runik
/// \date	23/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union WindowControlA {
    u16            raw;          ///< Raw representation.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	WindowControlB
///
/// \brief	Window Control (NBG2, NBG3) (WCTLB).
///
/// \author	Runik
/// \date	23/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union WindowControlB {
    u16            raw;          ///< Raw representation.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	WindowControlC
///
/// \brief	Window Control (RBG0, SPRITE) (WCTLC).
///
/// \author	Runik
/// \date	23/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union WindowControlC {
    u16            raw;          ///< Raw representation.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	WindowControlD
///
/// \brief	Window Control (Parameter Window, Color Calc. Window) (WCTLD).
///
/// \author	Runik
/// \date	23/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union WindowControlD {
    u16            raw;          ///< Raw representation.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	LineWindowTableAddressW0Upper
///
/// \brief	Line Window Table Address Upper (W0) (LWTA0U).
///
/// \author	Runik
/// \date	23/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union LineWindowTableAddressW0Upper {
    u16            raw;          ///< Raw representation.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	LineWindowTableAddressW0Lower
///
/// \brief	Line Window Table Address Lower (W0) (LWTA0L).
///
/// \author	Runik
/// \date	23/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union LineWindowTableAddressW0Lower {
    u16            raw;          ///< Raw representation.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	LineWindowTableAddressW1Upper
///
/// \brief	Line Window Table Address Upper (W1) (LWTA1U).
///
/// \author	Runik
/// \date	23/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union LineWindowTableAddressW1Upper {
    u16            raw;          ///< Raw representation.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	LineWindowTableAddressW1Lower
///
/// \brief	Line Window Table Address Lower (W1) (LWTA1L).
///
/// \author	Runik
/// \date	23/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union LineWindowTableAddressW1Lower {
    u16            raw;          ///< Raw representation.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   SpriteColorCalculationCondition
///
/// \brief  SPCCCSx bits values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class SpriteColorCalculationCondition : u8 {
    only_when_priority_is_less_or_equals_condition   = 0b00, ///< (Priority number) <= (Color calculation condition number) only.
    only_when_priority_equals_condition              = 0b01, ///< (Priority number) = (Color calculation condition number) only.
    only_when_priority_is_higher_or_equals_condition = 0b10, ///< (Priority number) >= (Color calculation condition number) only.
    only_when_color_data_msb_is_1                    = 0b11  ///< Only when Color Data MSB is 1.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   SpriteColorMode
///
/// \brief  SPCLMD bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class SpriteColorMode : bool {
    only_palette = false, ///< Sprite data is all in palette format.
    mixed        = true   ///< Sprite data is in palette format and RGB format.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   SpriteWindowEnable
///
/// \brief  SPWINEN bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class SpriteWindowEnable : bool {
    does_not_use_sprite_window = false, ///< Does not use sprite window.
    uses_sprite_window         = true   ///< Uses sprite window.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   SpriteType
///
/// \brief  SPTYPEx bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class SpriteType : u8 {
    type_0 = 0b0000, ///< Type 0.
    type_1 = 0b0001, ///< Type 1.
    type_2 = 0b0010, ///< Type 2.
    type_3 = 0b0011, ///< Type 3.
    type_4 = 0b0100, ///< Type 4.
    type_5 = 0b0101, ///< Type 5.
    type_6 = 0b0110, ///< Type 6.
    type_7 = 0b0111, ///< Type 7.
    type_8 = 0b1000, ///< Type 8.
    type_9 = 0b1001, ///< Type 9.
    type_a = 0b1010, ///< Type A.
    type_b = 0b1011, ///< Type B.
    type_c = 0b1100, ///< Type C.
    type_d = 0b1101, ///< Type D.
    type_e = 0b1110, ///< Type E.
    type_f = 0b1111  ///< Type F.

};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	SpriteControl
///
/// \brief	Sprite Control (SPCTL).
///
/// \author	Runik
/// \date	25/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union SpriteControl {
    u16             raw;                                ///< Raw representation.
    BitField<12, 2> sprite_color_calculation_condition; ///< Defines SPCCCSx bits.
    BitField<8, 3>  sprite_color_calculation_number;    ///< Defines SPCCNx bits.
    BitField<5>     sprite_color_mode;                  ///< Defines SPCLMD bit.
    BitField<4>     sprite_window_enable;               ///< Defines SPWINEN bit.
    BitField<0, 4>  sprite_type;                        ///< Defines SPTYPEx bits.
    BitField<8, 8>  upper_8_bits;                       ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8>  lower_8_bits;                       ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	ShadowControl
///
/// \brief	Shadow Control (SDCTL).
///
/// \author	Runik
/// \date	23/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union ShadowControl {
    u16            raw;          ///< Raw representation.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	ColorRamAddressOffsetA
///
/// \brief	Color RAM Address Offset (NBG0 - NBG3) (CRAOFA).
///
/// \author	Runik
/// \date	25/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union ColorRamAddressOffsetA {
    u16             raw;                           ///< Raw representation.
    BitField<12, 3> color_ram_address_offset_nbg3; ///< Defines N3CAOSx bits.
    BitField<8, 3>  color_ram_address_offset_nbg2; ///< Defines N2CAOSx bits.
    BitField<4, 3>  color_ram_address_offset_nbg1; ///< Defines N1CAOSx bits.
    BitField<0, 3>  color_ram_address_offset_nbg0; ///< Defines N0CAOSx bits.
    BitField<8, 8>  upper_8_bits;                  ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8>  lower_8_bits;                  ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	ColorRamAddressOffsetB
///
/// \brief	Color RAM Address Offset (RBG0, SPRITE) (CRAOFB).
///
/// \author	Runik
/// \date	25/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union ColorRamAddressOffsetB {
    u16            raw;                             ///< Raw representation.
    BitField<4, 3> color_ram_address_offset_sprite; ///< Defines SPCAOSx bits.
    BitField<0, 3> color_ram_address_offset_rbg0;   ///< Defines R0CAOSx bits.
    BitField<8, 8> upper_8_bits;                    ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits;                    ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	LineColorScreenEnable
///
/// \brief	Line Color Screen Enable (LNCLEN).
///
/// \author	Runik
/// \date	23/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union LineColorScreenEnable {
    u16            raw;          ///< Raw representation.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	SpecialPriorityMode
///
/// \brief	Special Priority Mode (SFPRMD).
///
/// \author	Runik
/// \date	23/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union SpecialPriorityMode {
    u16            raw;          ///< Raw representation.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	ColorCalculationControl
///
/// \brief	Color Calculation Control (CCCTL).
///
/// \author	Runik
/// \date	23/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union ColorCalculationControl {
    u16            raw;          ///< Raw representation.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	SpecialColorCalculationMode
///
/// \brief	Special Color Calculation Mode (SFCCMD).
///
/// \author	Runik
/// \date	23/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union SpecialColorCalculationMode {
    u16            raw;          ///< Raw representation.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	PriorityNumberSpriteA
///
/// \brief	Priority Number (SPRITE 0,1) (PRISA).
///
/// \author	Runik
/// \date	25/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union PriorityNumberSpriteA {
    u16            raw;               ///< Raw representation.
    BitField<0, 3> sprite_register_0; ///< Defines S0PRINx bits.
    BitField<8, 3> sprite_register_1; ///< Defines S1PRINx bits.
    BitField<8, 8> upper_8_bits;      ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits;      ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	PriorityNumber
///
/// \brief	Priority Number (SPRITE 2,3) (PRISB).
///
/// \author	Runik
/// \date	25/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union PriorityNumberSpriteB {
    u16            raw;               ///< Raw representation.
    BitField<0, 3> sprite_register_2; ///< Defines S2PRINx bits.
    BitField<8, 3> sprite_register_3; ///< Defines S3PRINx bits.
    BitField<8, 8> upper_8_bits;      ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits;      ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	PriorityNumberSpriteC
///
/// \brief	Priority Number (SPRITE 4,5) (PRISC).
///
/// \author	Runik
/// \date	25/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union PriorityNumberSpriteC {
    u16            raw;               ///< Raw representation.
    BitField<0, 3> sprite_register_4; ///< Defines S4PRINx bits.
    BitField<8, 3> sprite_register_5; ///< Defines S5PRINx bits.
    BitField<8, 8> upper_8_bits;      ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits;      ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	PriorityNumberSpriteD
///
/// \brief	Priority Number (SPRITE 6,7) (PRISD).
///
/// \author	Runik
/// \date	25/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union PriorityNumberSpriteD {
    u16            raw;               ///< Raw representation.
    BitField<0, 3> sprite_register_6; ///< Defines S6PRINx bits.
    BitField<8, 3> sprite_register_7; ///< Defines S7PRINx bits.
    BitField<8, 8> upper_8_bits;      ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits;      ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	PriorityNumberA
///
/// \brief	Priority Number A (NBG0, NBG1) (PRINA).
///
/// \author	Runik
/// \date	25/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union PriorityNumberA {
    u16            raw;          ///< Raw representation.
    BitField<8, 3> nbg1;         ///< Defines the priority number of NBG1 (N1PRINx).
    BitField<0, 3> nbg0;         ///< Defines the priority number of NBG0 (N0PRINx).
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	PriorityNumberB
///
/// \brief	Priority Number B (NBG2, NBG3) (PRINB).
///
/// \author	Runik
/// \date	25/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union PriorityNumberB {
    u16            raw;          ///< Raw representation.
    BitField<8, 3> nbg3;         ///< Defines the priority number of NBG1 (N1PRINx).
    BitField<0, 3> nbg2;         ///< Defines the priority number of NBG0 (N0PRINx).
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	PriorityNumberR
///
/// \brief	Priority Number R (Rbg0) (PRIR).
///
/// \author	Runik
/// \date	25/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union PriorityNumberR {
    u16            raw;          ///< Raw representation.
    BitField<0, 3> rbg0;         ///< Defines the priority number of RBG0 (R0PRINx).
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	ColorCalculationRatioSpriteA
///
/// \brief	Color Calculation Ratio (Sprite 0,1) (CCRSA).
///
/// \author	Runik
/// \date	23/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union ColorCalculationRatioSpriteA {
    u16            raw;          ///< Raw representation.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	ColorCalculationRatioSpriteB
///
/// \brief	Color Calculation Ratio (Sprite 2,3) (CCRSB).
///
/// \author	Runik
/// \date	23/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union ColorCalculationRatioSpriteB {
    u16            raw;          ///< Raw representation.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	ColorCalculationRatioSpriteC
///
/// \brief	Color Calculation Ratio (Sprite 4,5) (CCRSC).
///
/// \author	Runik
/// \date	23/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union ColorCalculationRatioSpriteC {
    u16            raw;          ///< Raw representation.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	ColorCalculationRatioSpriteD
///
/// \brief	Color Calculation Ratio (Sprite 6,7) (CCRSD).
///
/// \author	Runik
/// \date	23/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union ColorCalculationRatioSpriteD {
    u16            raw;          ///< Raw representation.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	ColorCalculationRatioNbg0Nbg1
///
/// \brief	Color Calculation Ratio (NBG0, NBG1) (CCRNA).
///
/// \author	Runik
/// \date	23/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union ColorCalculationRatioNbg0Nbg1 {
    u16            raw;          ///< Raw representation.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	ColorCalculationRatioNbg2Nbg3
///
/// \brief	Color Calculation Ratio (NBG2, NBG3) (CCRNB).
///
/// \author	Runik
/// \date	23/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union ColorCalculationRatioNbg2Nbg3 {
    u16            raw;          ///< Raw representation.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	ColorCalculationRatioRbg0
///
/// \brief	Color Calculation Ratio (RBG0) (CCRR).
///
/// \author	Runik
/// \date	23/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union ColorCalculationRatioRbg0 {
    u16            raw;          ///< Raw representation.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	ColorCalculationRatioLineColorBack
///
/// \brief	Color Calculation Ratio (Line Color Screen, Back Screen) (CCRLB).
///
/// \author	Runik
/// \date	23/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union ColorCalculationRatioLineColorBack {
    u16            raw;          ///< Raw representation.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   ColorOffsetEnableBit
///
/// \brief  xxCOEN bits values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class ColorOffsetEnableBit : bool {
    disabled = false, ///< Do not use color offset function.
    enabled  = true   ///< Use color offset function.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	ColorOffsetEnable
///
/// \brief	Color Offset Enable (CLOFEN).
///
/// \author	Runik
/// \date	23/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union ColorOffsetEnable {
    u16            raw;          ///< Raw representation.
    BitField<0>    nbg0;         ///< Defines N0COEN bit.
    BitField<1>    nbg1;         ///< Defines N1COEN bit.
    BitField<2>    nbg2;         ///< Defines N2COEN bit.
    BitField<3>    nbg3;         ///< Defines N3COEN bit.
    BitField<4>    rbg0;         ///< Defines R0COEN bit.
    BitField<5>    back;         ///< Defines BKCOEN bit.
    BitField<6>    sprite;       ///< Defines SPCOEN bit.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   ColorOffsetSelectBit
///
/// \brief  xxCOSL bits values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class ColorOffsetSelectBit : bool {
    use_color_offset_a = false, ///< Use color offset A.
    use_color_offset_b = true   ///< Use color offset B.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	ColorOffsetSelect
///
/// \brief	Color Offset Select (CLOFSL).
///
/// \author	Runik
/// \date	23/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union ColorOffsetSelect {
    u16            raw;          ///< Raw representation.
    BitField<0>    nbg0;         ///< Defines N0COSL bit.
    BitField<1>    nbg1;         ///< Defines N1COSL bit.
    BitField<2>    nbg2;         ///< Defines N2COSL bit.
    BitField<3>    nbg3;         ///< Defines N3COSL bit.
    BitField<4>    rbg0;         ///< Defines R0COSL bit.
    BitField<5>    back;         ///< Defines BKCOSL bit.
    BitField<6>    sprite;       ///< Defines SPCOSL bit.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum	Sign
///
/// \brief	Sign bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Sign : bool {
    positive = false, ///< Sign is positive.
    negative = true   ///< Sign is negative.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	ColorOffsetARed
///
/// \brief	Color Offset A (Red) (COAR).
///
/// \author	Runik
/// \date	23/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union ColorOffsetARed {
    u16            raw;          ///< Raw representation.
    BitField<0, 8> red_data;     ///< Defines the color offset A red data (COARDx).
    BitField<8>    sign;         ///< Sign of the data.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	ColorOffsetAGreen
///
/// \brief	Color Offset A (Green) (COAG).
///
/// \author	Runik
/// \date	23/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union ColorOffsetAGreen {
    u16            raw;          ///< Raw representation.
    BitField<0, 8> green_data;   ///< Defines the color offset A green data (COAGRx).
    BitField<8>    sign;         ///< Sign of the data.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	ColorOffsetABlue
///
/// \brief	Color Offset A (Blue)  (COAB).
///
/// \author	Runik
/// \date	23/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union ColorOffsetABlue {
    u16            raw;          ///< Raw representation.
    BitField<0, 8> blue_data;    ///< Defines the color offset A red data (COABLx).
    BitField<8>    sign;         ///< Sign of the data.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	ColorOffsetBRed
///
/// \brief	Color Offset B (Red) (COBR).
///
/// \author	Runik
/// \date	23/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union ColorOffsetBRed {
    u16            raw;          ///< Raw representation.
    BitField<0, 8> red_data;     ///< Defines the color offset B red data (COBRDx).
    BitField<8>    sign;         ///< Sign of the data.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	ColorOffsetBGreen
///
/// \brief	Color Offset B (Green) (COBG).
///
/// \author	Runik
/// \date	23/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union ColorOffsetBGreen {
    u16            raw;          ///< Raw representation.
    BitField<0, 8> green_data;   ///< Defines the color offset B green data (COBGRx).
    BitField<8>    sign;         ///< Sign of the data.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	ColorOffsetBBlue
///
/// \brief	Color Offset A (Blue) (COBB).
///
/// \author	Runik
/// \date	23/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union ColorOffsetBBlue {
    u16            raw;          ///< Raw representation.
    BitField<0, 8> blue_data;    ///< Defines the color offset B red data (COBBLx).
    BitField<8>    sign;         ///< Sign of the data.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

} // namespace saturnin::video
