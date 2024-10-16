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
        GENERATE_MASKED_RANGE("Vdp2Regs::Exten", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vdp2Regs::Exten", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
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
        GENERATE_MASKED_RANGE("Vdp2Regs::Exten", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vdp2Regs::Exten", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
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
        GENERATE_MASKED_RANGE("Vdp2Regs::Vrsize", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vdp2Regs::Vrsize", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
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
        GENERATE_MASKED_RANGE("Vdp2Regs::Hcnt", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vdp2Regs::Hcnt", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
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
        GENERATE_MASKED_RANGE("Vdp2Regs::Vcnt", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vdp2Regs::Vcnt", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
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
        GENERATE_MASKED_RANGE("Vdp2Regs::Reserve", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vdp2Regs::Reserve", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
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
        GENERATE_MASKED_RANGE("Vdp2Regs::Ramctl", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vdp2Regs::Ramctl", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
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
        GENERATE_MASKED_RANGE("Vdp2Regs::Cycxxl", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vdp2Regs::Cycxxl", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
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
        GENERATE_MASKED_RANGE("Vdp2Regs::Cycxxu", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vdp2Regs::Cycxxu", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
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
        GENERATE_MASKED_RANGE("Vdp2Regs::Bgon", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vdp2Regs::Bgon", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
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
        GENERATE_MASKED_RANGE("Vdp2Regs::Mzctl", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vdp2Regs::Mzctl", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
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
        GENERATE_MASKED_RANGE("Vdp2Regs::Sfsel", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vdp2Regs::Sfsel", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
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
        GENERATE_MASKED_RANGE("Vdp2Regs::Sfsel", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vdp2Regs::Sfsel", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
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
        GENERATE_MASKED_RANGE("Vdp2Regs::Chctla", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vdp2Regs::Chctla", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
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
        GENERATE_MASKED_RANGE("Vdp2Regs::Chctlb", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vdp2Regs::Chctlb", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
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
        GENERATE_MASKED_RANGE("Vdp2Regs::Bmpna", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vdp2Regs::Bmpna", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
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
        GENERATE_MASKED_RANGE("Vdp2Regs::Bmpnb", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vdp2Regs::Bmpnb", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using BmpnbType = Reg<u16, Bmpnb>;
    BmpnbType bmpnb;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Pcnxx
    ///
    /// \brief	Pattern Name Control (NBGx, RBG0) register (PNCxx).
    ///
    /// \author	Runik
    /// \date	15/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Pcnxx {
        GENERATE_USING(Pcnxx, u16);

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

        GENERATE_BIT_WITH_ENUM(pnb, 15, 0b1, PatternNameDataSize);            ///< Pattern name data size.
        GENERATE_BIT_WITH_ENUM(cnsm, 14, 0b1, CharacterNumberSupplementMode); ///< Character number mode.
        GENERATE_BIT_WITHOUT_ENUM(spr, 9, 0b1);                               ///< Special priority.
        GENERATE_BIT_WITHOUT_ENUM(scc, 8, 0b1);                               ///< Special color calculation.
        GENERATE_BIT_WITHOUT_ENUM(splt, 5, 0b111);                            ///< Supplementary palette number.
        GENERATE_BIT_WITHOUT_ENUM(scn, 0, 0b11111);                           ///< Supplementary character number.

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Pcnxx", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vdp2Regs::Pcnxx", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using PcnxxType = Reg<u16, Pcnxx>;
    PcnxxType pncn0;
    PcnxxType pncn1;
    PcnxxType pncn2;
    PcnxxType pncn3;
    PcnxxType pncr;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Plsz
    ///
    /// \brief	Plane Size register (PLSZ).
    ///
    /// \author	Runik
    /// \date	16/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Plsz {
        GENERATE_USING(Plsz, u16);

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
            = 0b01, ///< Outside the display area,the character pattern specified by screen over pattern name register is repeated
                    ///< (Only when the rotation scroll surface is in cell format).
            scroll_screen_is_transparent       = 0b10, ///< Outside the display area, the scroll screen is transparent.
            force_display_and_make_transparent = 0b11  ///< Set the display area as 0<=X<=512, 0<=Y<=512 regardless of plane or
                                                       ///< bitmap size and makes that area transparent.
        };

        GENERATE_BIT_WITH_ENUM(rbovr, 14, 0b11, ScreenOverProcess); ///< RPB screen over.
        GENERATE_BIT_WITH_ENUM(rbplsz, 12, 0b11, PlaneSize);        ///< RPB plane size.
        GENERATE_BIT_WITH_ENUM(raovr, 10, 0b11, ScreenOverProcess); ///< RPA screen over.
        GENERATE_BIT_WITH_ENUM(raplsz, 8, 0b11, PlaneSize);         ///< RPA plane size.
        GENERATE_BIT_WITH_ENUM(n3plsz, 6, 0b11, PlaneSize);         ///< NBG3 plane size.
        GENERATE_BIT_WITH_ENUM(n2plsz, 4, 0b11, PlaneSize);         ///< NBG2  plane size.
        GENERATE_BIT_WITH_ENUM(n1plsz, 2, 0b11, PlaneSize);         ///< NBG1 plane size.
        GENERATE_BIT_WITH_ENUM(n0plsz, 0, 0b11, PlaneSize);         ///< NBG0 plane size.

        static constexpr auto lo_byte_pos = PosType(0);             ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8);             ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Plsz", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vdp2Regs::Plsz", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using PlszType = Reg<u16, Plsz>;
    PlszType plsz;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Mpofn
    ///
    /// \brief	Map Offset (NBG0 - NBG3) register (MPOFN).
    ///
    /// \author	Runik
    /// \date	16/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Mpofn {
        GENERATE_USING(Mpofn, u16);

        GENERATE_BIT_WITHOUT_ENUM(n3mp, 12, 0b111);     ///< NBG3 map offset.
        GENERATE_BIT_WITHOUT_ENUM(n2mp, 8, 0b111);      ///< NBG2 map offset.
        GENERATE_BIT_WITHOUT_ENUM(n1mp, 4, 0b111);      ///< NBG1 map offset.
        GENERATE_BIT_WITHOUT_ENUM(n0mp, 0, 0b111);      ///< NBG0 map offset.

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Bmpnb", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vdp2Regs::Bmpnb", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using MpofnType = Reg<u16, Mpofn>;
    MpofnType mpofn;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Mpofr
    ///
    /// \brief	Map Offset (Rotation Parameter A,B) register (MPOFR).
    ///
    /// \author	Runik
    /// \date	16/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Mpofr {
        GENERATE_USING(Mpofr, u16);

        GENERATE_BIT_WITHOUT_ENUM(rbmp, 4, 0b111);      ///< RPB map offset.
        GENERATE_BIT_WITHOUT_ENUM(ramp, 0, 0b111);      ///< RPA map offset.

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Bmpnb", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vdp2Regs::Bmpnb", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using MpofrType = Reg<u16, Mpofr>;
    MpofrType mpofr;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Mpab
    ///
    /// \brief	Map (NBGx, RBGx, Plane A,B) register (MPABxx).
    ///
    /// \author	Runik
    /// \date	16/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Mpab {
        GENERATE_USING(Mpab, u16);

        GENERATE_BIT_WITHOUT_ENUM(mpa, 0, 0b111111);    ///< Plane A.
        GENERATE_BIT_WITHOUT_ENUM(mpb, 0, 0b111111);    ///< Plane B.

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Mpab", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Mpab", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using MpabType = Reg<u16, Mpab>;
    MpabType mpabn0;
    MpabType mpabn1;
    MpabType mpabn2;
    MpabType mpabn3;
    MpabType mpabra;
    MpabType mpabrb;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Mpcd
    ///
    /// \brief	Map (NBGx, RBGx, Plane C,D) register (MPCDxx).
    ///
    /// \author	Runik
    /// \date	16/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Mpcd {
        GENERATE_USING(Mpcd, u16);

        GENERATE_BIT_WITHOUT_ENUM(mpc, 0, 0b111111);    ///< Plane C.
        GENERATE_BIT_WITHOUT_ENUM(mpd, 0, 0b111111);    ///< Plane D.

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Mpcd", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Mpcd", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using MpcdType = Reg<u16, Mpcd>;
    MpcdType mpcdn0;
    MpcdType mpcdn1;
    MpcdType mpcdn2;
    MpcdType mpcdn3;
    MpcdType mpcdra;
    MpcdType mpcdrb;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Mpef
    ///
    /// \brief	Map (Rotation Parameter x, Plane E,F) register (MPEFRx).
    ///
    /// \author	Runik
    /// \date	16/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Mpef {
        GENERATE_USING(Mpef, u16);

        GENERATE_BIT_WITHOUT_ENUM(mpe, 0, 0b111111);    ///< Plane E.
        GENERATE_BIT_WITHOUT_ENUM(mpf, 0, 0b111111);    ///< Plane F.

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Mpef", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Mpef", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using MpefType = Reg<u16, Mpef>;
    MpefType mpefra;
    MpefType mpefrb;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Mpgh
    ///
    /// \brief	Map (Rotation Parameter x, Plane G,H) register (MPGHRx).
    ///
    /// \author	Runik
    /// \date	16/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Mpgh {
        GENERATE_USING(Mpgh, u16);

        GENERATE_BIT_WITHOUT_ENUM(mpg, 0, 0b111111);    ///< Plane G.
        GENERATE_BIT_WITHOUT_ENUM(mph, 0, 0b111111);    ///< Plane H.

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Mpgh", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Mpgh", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using MpghType = Reg<u16, Mpgh>;
    MpghType mpghra;
    MpghType mpghrb;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Mpij
    ///
    /// \brief	Map (Rotation Parameter x, Plane I,J) register (MPIJRx).
    ///
    /// \author	Runik
    /// \date	16/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Mpij {
        GENERATE_USING(Mpij, u16);

        GENERATE_BIT_WITHOUT_ENUM(mpi, 0, 0b111111);    ///< Plane I.
        GENERATE_BIT_WITHOUT_ENUM(mpj, 0, 0b111111);    ///< Plane J.

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Mpij", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Mpij", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using MpijType = Reg<u16, Mpij>;
    MpijType mpijra;
    MpijType mpijrb;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Mpkl
    ///
    /// \brief	Map (Rotation Parameter x, Plane K,L) register (MPKLRx).
    ///
    /// \author	Runik
    /// \date	16/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Mpkl {
        GENERATE_USING(Mpkl, u16);

        GENERATE_BIT_WITHOUT_ENUM(mpk, 0, 0b111111);    ///< Plane K.
        GENERATE_BIT_WITHOUT_ENUM(mpl, 0, 0b111111);    ///< Plane L.

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Mpij", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Mpij", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using MpklType = Reg<u16, Mpkl>;
    MpklType mpklra;
    MpklType mpklrb;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Mpmn
    ///
    /// \brief	Map (Rotation Parameter x, Plane M,N) register (MPMNRx).
    ///
    /// \author	Runik
    /// \date	16/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Mpmn {
        GENERATE_USING(Mpmn, u16);

        GENERATE_BIT_WITHOUT_ENUM(mpm, 0, 0b111111);    ///< Plane M.
        GENERATE_BIT_WITHOUT_ENUM(mpn, 0, 0b111111);    ///< Plane N.

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Mpmn", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Mpmn", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using MpmnType = Reg<u16, Mpmn>;
    MpmnType mpmnra;
    MpmnType mpmnrb;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Mpop
    ///
    /// \brief	Map (Rotation Parameter x, Plane O,P) register (MPOPRx).
    ///
    /// \author	Runik
    /// \date	16/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Mpop {
        GENERATE_USING(Mpop, u16);

        GENERATE_BIT_WITHOUT_ENUM(mpo, 0, 0b111111);    ///< Plane O.
        GENERATE_BIT_WITHOUT_ENUM(mpp, 0, 0b111111);    ///< Plane P.

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Mpmn", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Mpmn", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using MpopType = Reg<u16, Mpop>;
    MpopType mpopra;
    MpopType mpoprb;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Scxin
    ///
    /// \brief	Screen Scroll Value (Integer Part) (SCxINx).
    ///
    /// \author	Runik
    /// \date	16/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Scin {
        GENERATE_USING(Scin, u16);

        GENERATE_BIT_WITHOUT_ENUM(nsci, 0, 0x7FF);      ///< Screen scroll integer part  (NxSCIx).

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Scin", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Scin", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using ScinType = Reg<u16, Scin>;
    ScinType scxin0;
    ScinType scyin0;
    ScinType scxin1;
    ScinType scyin1;
    ScinType scxn2;
    ScinType scyn2;
    ScinType scxn3;
    ScinType scyn3;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Scdn
    ///
    /// \brief	Screen Scroll Value (Fractional Part) (SCXDNx).
    ///
    /// \author	Runik
    /// \date	16/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Scdn {
        GENERATE_USING(Scdn, u16);

        GENERATE_BIT_WITHOUT_ENUM(nscd, 8, 0xFF);       ///< Screen scroll integer part  (NxSCIx).

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Scdn", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Scdn", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using ScdnType = Reg<u16, Scdn>;
    ScdnType scxdn0;
    ScdnType scydn0;
    ScdnType scxdn1;
    ScdnType scydn1;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Zmin
    ///
    /// \brief	Coordinate Increment Integer Part.
    ///
    /// \author	Runik
    /// \date	16/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Zmin {
        GENERATE_USING(Zmin, u16);

        GENERATE_BIT_WITHOUT_ENUM(zmin, 0, 0b111);      ///< Coordinate increment (NxZMxIx).

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Zmin", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Zmin", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using ZminType = Reg<u16, Zmin>;
    ZminType zmxin0;
    ZminType zmyin0;
    ZminType zmxin1;
    ZminType zmyin1;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Zmdn
    ///
    /// \brief	Coordinate Increment Fractional Part.
    ///
    /// \author	Runik
    /// \date	16/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Zmdn {
        GENERATE_USING(Zmdn, u16);

        GENERATE_BIT_WITHOUT_ENUM(zmdn, 8, 0xFF);       ///< Coordinate increment (NxZMxDx).

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Zmdn", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Zmdn", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using ZmdnType = Reg<u16, Zmdn>;
    ZmdnType zmxdn0;
    ZmdnType zmydn0;
    ZmdnType zmxdn1;
    ZmdnType zmydn1;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Zmctl
    ///
    /// \brief	Reduction Enable (ZMCTL).
    ///
    /// \author	Runik
    /// \date	16/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Zmctl {
        GENERATE_USING(Zmctl, u16);

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

        GENERATE_BIT_WITH_ENUM(n1zmqt, 9, 0b1, ZoomQuarter); ///< NBG1 zoom quarter.
        GENERATE_BIT_WITH_ENUM(n1zmhf, 8, 0b1, ZoomHalf);    ///< NBG1 zoom half.
        GENERATE_BIT_WITH_ENUM(n0zmqt, 1, 0b1, ZoomQuarter); ///< NBG0 zoom quarter.
        GENERATE_BIT_WITH_ENUM(n0zmhf, 0, 0b1, ZoomHalf);    ///< NBG0 zoom half.

        static constexpr auto lo_byte_pos = PosType(0);      ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8);      ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Zmctl", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Zmctl", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using ZmctlType = Reg<u16, Zmctl>;
    ZmctlType zmctl;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Scrctl
    ///
    /// \brief	Line And Vertical Cell Scroll Control (NBG0, NBG1) (SRCCTL).
    ///
    /// \author	Runik
    /// \date	16/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Scrctl {
        GENERATE_USING(Scrctl, u16);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum	LineScrollInterval
        ///
        /// \brief	NxLSSx bits values. Depends of the interlace setting (ni = non-interlace, sdi = single-density interlace,
        /// 		ddi = double-density interlace.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class LineScrollInterval : u8 {
            each_line_or_every_2_lines      = 0b00, ///< Each line for ni or ddi, 2 lines for sdi.
            every_2_lines_or_every_4_lines  = 0b01, ///< Every 2 lines for ni or ddi, 4 lines for sdi.
            every_4_lines_or_every_8_lines  = 0b10, ///< Every 4 lines for ni or ddi, 8 lines for sdi.
            every_8_lines_or_every_16_lines = 0b11  ///< Every 8 lines for ni or ddi, 16 lines for sdi.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum	LineZoomEnable
        ///
        /// \brief	NxLZMX bit values.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class LineZoomEnable : bool { does_not_scale_horizontally = false, scales_horizontally = true };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum	LineScrollEnable
        ///
        /// \brief	NxLSCY bit values.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class LineScrollYEnable : bool { does_not_scroll_vertically = false, scrolls_vertically = true };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum	LineScrollXEnable
        ///
        /// \brief	NxLSCX bit values.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class LineScrollXEnable : bool { does_not_scroll_horizontally = false, scrolls_vertically = true };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum	VerticalCellScrollEnable
        ///
        /// \brief	NxVCSC bit values.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class VerticalCellScrollEnable : bool { does_not_cell_scroll_vertically = false, cell_scrolls_vertically = true };

        GENERATE_BIT_WITH_ENUM(n1lss, 12, 0b11, LineScrollInterval);      ///< NBG1 line scroll interval.
        GENERATE_BIT_WITH_ENUM(n1lzmx, 11, 0b1, LineZoomEnable);          ///< NBG1 line zoom enable.
        GENERATE_BIT_WITH_ENUM(n1lscy, 10, 0b1, LineScrollYEnable);       ///< NBG1 vertical line scroll enable.
        GENERATE_BIT_WITH_ENUM(n1lscx, 9, 0b1, LineScrollXEnable);        ///< NBG1 horizontal line scroll enable.
        GENERATE_BIT_WITH_ENUM(n1vcsc, 8, 0b1, VerticalCellScrollEnable); ///< NBG1 vertical cell scroll enable.
        GENERATE_BIT_WITH_ENUM(n0lss, 4, 0b11, LineScrollInterval);       ///< NBG0 line scroll interval.
        GENERATE_BIT_WITH_ENUM(n0lzmx, 3, 0b1, LineZoomEnable);           ///< NBG0 line zoom enable.
        GENERATE_BIT_WITH_ENUM(n0lscy, 2, 0b1, LineScrollYEnable);        ///< NBG0 vertical line scroll enable.
        GENERATE_BIT_WITH_ENUM(n0lscx, 1, 0b1, LineScrollXEnable);        ///< NBG0 horizontal line scroll enable.
        GENERATE_BIT_WITH_ENUM(n0vcsc, 0, 0b1, VerticalCellScrollEnable); ///< NBG1 vertical cell scroll enable.

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Scrctl", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Scrctl", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using ScrctlType = Reg<u16, Scrctl>;
    ScrctlType scrctl;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Vcstau
    ///
    /// \brief	Vertical Cell Scroll Table Address Upper (NBG0, NBG1) (VCSTAU).
    ///
    /// \author	Runik
    /// \date	16/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Vcstau {
        GENERATE_USING(Vcstau, u16);

        GENERATE_BIT_WITHOUT_ENUM(vstau, 0, 0b111);     ///< Upper vertical cell scroll table address register(NxZMxDx).

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Vcstau", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Vcstau", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using VcstauType = Reg<u16, Vcstau>;
    VcstauType vcstau;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Vcstal
    ///
    /// \brief	Vertical Cell Scroll Table Address Lower (NBG0, NBG1) (VCSTAL).
    ///
    /// \author	Runik
    /// \date	16/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Vcstal {
        GENERATE_USING(Vcstal, u16);

        GENERATE_BIT_WITHOUT_ENUM(vstal, 1, 0x7FFF);    ///< Upper vertical cell scroll table address register(NxZMxDx).

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Vcstau", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Vcstau", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using VcstalType = Reg<u16, Vcstal>;
    VcstalType vcstal;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Lstau
    ///
    /// \brief	Line Scroll Table Address Upper (NBGx) (LSTAxU).
    ///
    /// \author	Runik
    /// \date	17/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Lstau {
        GENERATE_USING(Lstau, u16);

        GENERATE_BIT_WITHOUT_ENUM(lstau, 0, 0b111);     ///< Upper vertical cell scroll table address register(NxZMxDx).

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Lstau", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Lstau", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using LstauType = Reg<u16, Lstau>;
    LstauType lsta0u;
    LstauType lsta1u;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Lstal
    ///
    /// \brief	Line Scroll Table Address Lower (NBGx) (LSTAxL).
    ///
    /// \author	Runik
    /// \date	17/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Lstal {
        GENERATE_USING(Lstal, u16);

        GENERATE_BIT_WITHOUT_ENUM(lstal, 1, 0x7FFF);    ///< Upper vertical cell scroll table address register(NxZMxDx).

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Lstal", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Lstal", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using LstalType = Reg<u16, Lstal>;
    LstalType lsta0l;
    LstalType lsta1l;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Lctau
    ///
    /// \brief	Line Color Screen Table Address Upper (LCTAU).
    ///
    /// \author	Runik
    /// \date	17/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Lctau {
        GENERATE_USING(Lctau, u16);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum	LineColorScreenMode
        ///
        /// \brief	LNCL color mode bit (LCCLMD)
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class LineColorScreenMode : bool { single_color = false, select_per_line = true };

        GENERATE_BIT_WITH_ENUM(lcclmd, 15, 0b1, LineColorScreenMode); ///< Line color screen mode (LCCLMD).
        GENERATE_BIT_WITHOUT_ENUM(lctau, 0, 0b111);                   ///< Line color screen table address (upper part).

        static constexpr auto lo_byte_pos = PosType(0);               ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8);               ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Lctau", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Lctau", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using LctauType = Reg<u16, Lctau>;
    LctauType lctau;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Lctal
    ///
    /// \brief	Line Color Screen Table Address Lower (LCTAL).
    ///
    /// \author	Runik
    /// \date	17/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Lctal {
        GENERATE_USING(Lctal, u16);

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Lctal", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Lctal", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using LctalType = Reg<u16, Lctal>;
    LctalType lctal;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Bktau
    ///
    /// \brief	Back Screen Table Address Upper (BKTAU).
    ///
    /// \author	Runik
    /// \date	17/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Bktau {
        GENERATE_USING(Bktau, u16);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum	BackScreenColorMode
        ///
        /// \brief	BACK color mode bit (BKCLMD)
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class BackScreenColorMode : bool { single_color = false, select_per_line = true };

        GENERATE_BIT_WITH_ENUM(bkclmd, 15, 0b1, BackScreenColorMode); ///< Back screen color mode (BKCLMD).
        GENERATE_BIT_WITHOUT_ENUM(bktau, 0, 0b111);                   ///< Line color screen table address (upper part).

        static constexpr auto lo_byte_pos = PosType(0);               ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8);               ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Bktau", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Bktau", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using BktauType = Reg<u16, Bktau>;
    BktauType bktau;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Bktal
    ///
    /// \brief	Back Screen Table Address Lower (BKTAL).
    ///
    /// \author	Runik
    /// \date	17/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Bktal {
        GENERATE_USING(Bktal, u16);

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Lctal", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Lctal", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using BktalType = Reg<u16, Bktal>;
    BktalType bktal;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Rpmd
    ///
    /// \brief	Rotation Parameter Mode (RPMD).
    ///
    /// \author	Runik
    /// \date	17/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Rpmd {
        GENERATE_USING(Rpmd, u16);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum	RotationParametersMode
        ///
        /// \brief	Rotation parameters mode bits values (RPMDx)
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class RotationParametersMode : u8 {
            rotation_parameter_a                       = 0b00,
            rotation_parameter_b                       = 0b01,
            screens_are_switched_via_coefficient_table = 0b10,
            screens_are_switched_via_window            = 0b11
        };

        GENERATE_BIT_WITH_ENUM(rpmd, 0, 0b11, RotationParametersMode); ///< Rotation parameters mode (RPMDx).

        static constexpr auto lo_byte_pos = PosType(0);                ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8);                ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Rpmd", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Rpmd", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using RpmdType = Reg<u16, Rpmd>;
    RpmdType rpmd;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Rprctl
    ///
    /// \brief	Rotation Parameter Read Control (RPRCTL).
    ///
    /// \author	Runik
    /// \date	17/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Rprctl {
        GENERATE_USING(Rprctl, u16);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum	ParameterReadEnable
        ///
        /// \brief	Parameter read enable bit values (RxSTRE)
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class ParameterReadEnable : bool { rotation_parameter_a = false, rotation_parameter_b = true };

        GENERATE_BIT_WITH_ENUM(raxstre, 0, 0b1, ParameterReadEnable);   ///< For Xst of Rotation Parameter A (RAXSTRE).
        GENERATE_BIT_WITH_ENUM(raystre, 1, 0b1, ParameterReadEnable);   ///< For Yst of Rotation Parameter A (RAYSTRE).
        GENERATE_BIT_WITH_ENUM(rakastre, 2, 0b1, ParameterReadEnable);  ///< For KAst of Rotation Parameter A (RAKASTRE).
        GENERATE_BIT_WITH_ENUM(rbxstre, 8, 0b1, ParameterReadEnable);   ///< For Xst of Rotation Parameter B (RBXSTRE).
        GENERATE_BIT_WITH_ENUM(rbystre, 9, 0b1, ParameterReadEnable);   ///< For Yst of Rotation Parameter B (RBYSTRE).
        GENERATE_BIT_WITH_ENUM(rbkastre, 10, 0b1, ParameterReadEnable); ///< For KAst of Rotation Parameter B (RBKASTRE).

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Rprctl", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Rprctl", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using RprctlType = Reg<u16, Rprctl>;
    RprctlType rprctl;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Ktctl
    ///
    /// \brief	Coefficient Table Control (KTCTL).
    ///
    /// \author	Runik
    /// \date	17/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Ktctl {
        GENERATE_USING(Ktctl, u16);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum	ParameterReadEnable
        ///
        /// \brief	Parameter read enable bit values (RxKLCE)
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class CoefficientLineColorEnable : bool {
            line_color_screen_data_is_not_used = false, ///< Line color screen data within coefficient data is not used.
            line_color_screen_data_is_used     = true   ///< Line color screen data within coefficient data is used.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum	Coefficient mode
        ///
        /// \brief	Designates what parameters the coefficient data is used as (RxKMD)
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class CoefficientMode : u8 {
            use_as_scale_coeff_kx_ky                      = 0b00, ///< Use as scale coefficient kx, ky.
            use_as_scale_coeff_kx                         = 0b01, ///< Use as scale coefficient kx.
            use_as_scale_coeff_ky                         = 0b10, ///< Use as scale coefficient ky.
            use_as_viewpoint_xp_after_rotation_conversion = 0b11  ///< Use as viewpoint Xp after rotation conversion.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum	CoefficientDataSize
        ///
        /// \brief	Designates the size of the coefficient data (RxKDBS).
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class CoefficientDataSize : bool {
            two_words = false, ///< 2 words.
            one_word  = true   ///< 1 word.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum	CoefficientTableEnable
        ///
        /// \brief	Designates whether the coefficient table is used (RxKTE).
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class CoefficientTableEnable : bool {
            do_not_use_coeff_table = false,                            ///< Do not use coefficient table.
            use_coeff_table        = true                              ///< Use coefficient table.
        };
        GENERATE_BIT_WITH_ENUM(rakte, 0, 0b1, CoefficientTableEnable); /**< Coefficient table enable for rotation parameter A.
                                                                          (RAKTE).*/
        GENERATE_BIT_WITH_ENUM(rakdbs, 1, 0b1, CoefficientDataSize);   /**< Coefficient data size for rotation parameter A.
                                                                            (RAKDBS).*/
        GENERATE_BIT_WITH_ENUM(rakmd, 2, 0b11, CoefficientMode); /**< Coefficient data mode for rotation parameter A.(RAKMD).*/
        GENERATE_BIT_WITH_ENUM(raklce, 4, 0b1, CoefficientLineColorEnable); /**< Coefficient line color enable for rotation
                                                                                 parameter A (RAKLCE).*/
        GENERATE_BIT_WITH_ENUM(rbkte, 0, 0b1, CoefficientTableEnable); /**< Coefficient table enable for rotation parameter B.
                                                                          (RBKTE).*/
        GENERATE_BIT_WITH_ENUM(rbkdbs, 9, 0b1, CoefficientDataSize);   /**< Coefficient data size for rotation parameter B.
                                                                            (RBKDBS).*/
        GENERATE_BIT_WITH_ENUM(rbkmd, 10, 0b11, CoefficientMode); /**< Coefficient data mode for rotation parameter B.(RBKMD).*/
        GENERATE_BIT_WITH_ENUM(rbklce, 12, 0b1, CoefficientLineColorEnable); /**< Coefficient line color enable for rotation
                                                                                  parameter B (RBKLCE).*/

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Ktctl", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Ktctl", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using KtctlType = Reg<u16, Ktctl>;
    KtctlType ktctl;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Ktaof
    ///
    /// \brief	Coefficient Table Address Offset (Rotation Parameter A,B) (KTAOF).
    ///
    /// \author	Runik
    /// \date	17/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Ktaof {
        GENERATE_USING(Ktaof, u16);

        GENERATE_BIT_WITHOUT_ENUM(raktaos, 0, 0b111);   /**< Designates the lead address offset value of the coefficient table
                                                             stored in the rotation parameter A table. (RAKTAOS) */
        GENERATE_BIT_WITHOUT_ENUM(rbktaos, 8, 0b111);   /**< Designates the lead address offset value of the coefficient table
                                                             stored in the rotation parameter B table. (RBKTAOS) */

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Ktaof", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Ktaof", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using KtaofType = Reg<u16, Ktaof>;
    KtaofType ktaof;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Ovprn
    ///
    /// \brief	Screen Over Pattern Name (OVPRNx)
    ///
    /// \author	Runik
    /// \date	17/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Ovprn {
        GENERATE_USING(Ovprn, u16);

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Ovprn", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Ovprn", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using OvprnType = Reg<u16, Ovprn>;
    OvprnType ovpnra;
    OvprnType ovpnrb;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Rptau
    ///
    /// \brief	Rotation Parameter Table Address Upper (RPTAU).
    ///
    /// \author	Runik
    /// \date	17/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Rptau {
        GENERATE_USING(Rptau, u16);

        GENERATE_BIT_WITHOUT_ENUM(rptau, 0, 0b111);     ///< Upper rotation parameter table address (RPTA).

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Rptau", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Rptau", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using RptauType = Reg<u16, Rptau>;
    RptauType rptau;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Rptal
    ///
    /// \brief	Rotation Parameter Table Address Lower (RPTAL).
    ///
    /// \author	Runik
    /// \date	17/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Rptal {
        GENERATE_USING(Rptal, u16);

        GENERATE_BIT_WITHOUT_ENUM(rptal, 1, 0x7FFF);    ///< Lower rotation parameter table address (RPTAL).

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Rptal", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Rptal", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using RptalType = Reg<u16, Rptal>;
    RptalType rptal;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Wpsx
    ///
    /// \brief	Window position (for horizontal coordinates).
    ///
    /// \author	Runik
    /// \date	17/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Wpsx {
        GENERATE_USING(Wpsx, u16);

        GENERATE_BIT_WITHOUT_ENUM(rptau, 0, 0x3FF);     ///< Window start/end X bits (WxxXx).

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Wpsx", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Wpsx", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using WpsxType = Reg<u16, Wpsx>;
    WpsxType wpsx0;
    WpsxType wpex0;
    WpsxType wpsx1;
    WpsxType wpex1;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Wpsy
    ///
    /// \brief	Window position (for vertical coordinates).
    ///
    /// \author	Runik
    /// \date	18/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Wpsy {
        GENERATE_USING(Wpsy, u16);

        GENERATE_BIT_WITHOUT_ENUM(rptau, 0, 0x1FF);     ///< Window start/end Y bits (WxxYx).

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Wpsy", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Wpsy", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using WpsyType = Reg<u16, Wpsy>;
    WpsyType wpsy0;
    WpsyType wpey0;
    WpsyType wpsy1;
    WpsyType wpey1;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Wctla
    ///
    /// \brief	Window Control (NBG0, NBG1) (WCTLA).
    ///
    /// \author	Runik
    /// \date	18/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Wctla {
        GENERATE_USING(Wctla, u16);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum	WindowLogic
        ///
        /// \brief	Designates the method of overlapping windows used in each screen. (NxLOG)
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class WindowLogic : bool {
            overlaid_logic_or  = false, ///< Or.
            overlaid_logic_and = true   ///< And.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum	WindowEnable
        ///
        /// \brief	Designates whether to use the Normal window W0, W1 or sprite window SW in each screen.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class WindowEnable : bool {
            does_not_use_window = false, ///< Does not use window.
            uses_window         = true   ///< Uses window.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum	WindowArea
        ///
        /// \brief	Designates the valid area of the Normal window W0, W1 or sprite window SW used in each screen.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class WindowArea : bool {
            enables_inside_of_window = false, ///< Enables the inside of W0,W1 or SW window.
            enables_ouside_of_window = true   ///< Enables the outside of W0,W1 or SW window.
        };

        GENERATE_BIT_WITH_ENUM(n0w0a, 0, 0b1, WindowArea);    ///< NBG0 window area (N0W0A).
        GENERATE_BIT_WITH_ENUM(n0w0e, 1, 0b1, WindowEnable);  ///< NBG0 window W0 enable (N0W0E).
        GENERATE_BIT_WITH_ENUM(n0w1a, 2, 0b1, WindowArea);    ///< NBG0 window area (N0W1A).
        GENERATE_BIT_WITH_ENUM(n0w1e, 3, 0b1, WindowEnable);  ///< NBG0 window W1 enable (N0W1E).
        GENERATE_BIT_WITH_ENUM(n0swa, 4, 0b1, WindowArea);    ///< NBG0 window area (N0SWA).
        GENERATE_BIT_WITH_ENUM(n0swe, 5, 0b1, WindowEnable);  ///< NBG0 window SW enable (N0SWE).
        GENERATE_BIT_WITH_ENUM(n0log, 7, 0b1, WindowLogic);   ///< NBG0 overlapping window method (N0LOG).

        GENERATE_BIT_WITH_ENUM(n1w0a, 8, 0b1, WindowArea);    ///< NBG1 window area (N1W0A).
        GENERATE_BIT_WITH_ENUM(n1w0e, 9, 0b1, WindowEnable);  ///< NBG1 window W0 enable (N1W0E).
        GENERATE_BIT_WITH_ENUM(n1w1a, 10, 0b1, WindowArea);   ///< NBG1 window area (N1W1A).
        GENERATE_BIT_WITH_ENUM(n1w1e, 11, 0b1, WindowEnable); ///< NBG1 window W1 enable (N1W1E).
        GENERATE_BIT_WITH_ENUM(n1swa, 12, 0b1, WindowArea);   ///< NBG1 window area (N1SWA).
        GENERATE_BIT_WITH_ENUM(n1swe, 13, 0b1, WindowEnable); ///< NBG1 window SW enable (N1SWE).
        GENERATE_BIT_WITH_ENUM(n1log, 15, 0b1, WindowLogic);  ///< NBG1 overlapping window method (N1LOG).

        static constexpr auto lo_byte_pos = PosType(0);       ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8);       ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Wctla", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Wctla", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using WctlaType = Reg<u16, Wctla>;
    WctlaType wctla;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Wctlb
    ///
    /// \brief	Window Control (NBG2, NBG3) (WCTLB).
    ///
    /// \author	Runik
    /// \date	18/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Wctlb {
        GENERATE_USING(Wctlb, u16);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum	WindowLogic
        ///
        /// \brief	Designates the method of overlapping windows used in each screen. (NxLOG)
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class WindowLogic : bool {
            overlaid_logic_or  = false, ///< Or.
            overlaid_logic_and = true   ///< And.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum	WindowEnable
        ///
        /// \brief	Designates whether to use the Normal window W0, W1 or sprite window SW in each screen.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class WindowEnable : bool {
            does_not_use_window = false, ///< Does not use window.
            uses_window         = true   ///< Uses window.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum	WindowArea
        ///
        /// \brief	Designates the valid area of the Normal window W0, W1 or sprite window SW used in each screen.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class WindowArea : bool {
            enables_inside_of_window = false, ///< Enables the inside of W0,W1 or SW window.
            enables_ouside_of_window = true   ///< Enables the outside of W0,W1 or SW window.
        };

        GENERATE_BIT_WITH_ENUM(n2w0a, 0, 0b1, WindowArea);    ///< NBG2 window area (N2W0A).
        GENERATE_BIT_WITH_ENUM(n2w0e, 1, 0b1, WindowEnable);  ///< NBG2 window W0 enable (N2W0E).
        GENERATE_BIT_WITH_ENUM(n2w1a, 2, 0b1, WindowArea);    ///< NBG2 window area (N2W1A).
        GENERATE_BIT_WITH_ENUM(n2w1e, 3, 0b1, WindowEnable);  ///< NBG2 window W1 enable (N2W1E).
        GENERATE_BIT_WITH_ENUM(n2swa, 4, 0b1, WindowArea);    ///< NBG2 window area (N2SWA).
        GENERATE_BIT_WITH_ENUM(n2swe, 5, 0b1, WindowEnable);  ///< NBG2 window SW enable (N2SWE).
        GENERATE_BIT_WITH_ENUM(n2log, 7, 0b1, WindowLogic);   ///< NBG2 overlapping window method (N2LOG).

        GENERATE_BIT_WITH_ENUM(n3w0a, 8, 0b1, WindowArea);    ///< NBG3 window area (N3W0A).
        GENERATE_BIT_WITH_ENUM(n3w0e, 9, 0b1, WindowEnable);  ///< NBG3 window W0 enable (N3W0E).
        GENERATE_BIT_WITH_ENUM(n3w1a, 10, 0b1, WindowArea);   ///< NBG3 window area (N3W1A).
        GENERATE_BIT_WITH_ENUM(n3w1e, 11, 0b1, WindowEnable); ///< NBG3 window W1 enable (N3W1E).
        GENERATE_BIT_WITH_ENUM(n3swa, 12, 0b1, WindowArea);   ///< NBG3 window area (N3SWA).
        GENERATE_BIT_WITH_ENUM(n3swe, 13, 0b1, WindowEnable); ///< NBG3 window SW enable (N3SWE).
        GENERATE_BIT_WITH_ENUM(n3log, 15, 0b1, WindowLogic);  ///< NBG3 overlapping window method (N3LOG).

        static constexpr auto lo_byte_pos = PosType(0);       ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8);       ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Wctlb", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Wctlb", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using WctlbType = Reg<u16, Wctlb>;
    WctlbType wctlb;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Wctlc
    ///
    /// \brief	Window Control (RBG0, SPRITE) (WCTLC).
    ///
    /// \author	Runik
    /// \date	18/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Wctlc {
        GENERATE_USING(Wctlc, u16);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum	WindowLogic
        ///
        /// \brief	Designates the method of overlapping windows used in each screen.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class WindowLogic : bool {
            overlaid_logic_or  = false, ///< Or.
            overlaid_logic_and = true   ///< And.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum	WindowEnable
        ///
        /// \brief	Designates whether to use the Normal window W0, W1 or sprite window SW in each screen.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class WindowEnable : bool {
            does_not_use_window = false, ///< Does not use window.
            uses_window         = true   ///< Uses window.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum	WindowArea
        ///
        /// \brief	Designates the valid area of the Normal window W0, W1 or sprite window SW used in each screen.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class WindowArea : bool {
            enables_inside_of_window = false, ///< Enables the inside of W0,W1 or SW window.
            enables_ouside_of_window = true   ///< Enables the outside of W0,W1 or SW window.
        };

        GENERATE_BIT_WITH_ENUM(r0w0a, 0, 0b1, WindowArea);    ///< RBG0 window area (R0W0A).
        GENERATE_BIT_WITH_ENUM(r0w0e, 1, 0b1, WindowEnable);  ///< RBG0 window W0 enable (R0W0E).
        GENERATE_BIT_WITH_ENUM(r0w1a, 2, 0b1, WindowArea);    ///< RBG0 window area (R0W1A).
        GENERATE_BIT_WITH_ENUM(r0w1e, 3, 0b1, WindowEnable);  ///< RBG0 window W1 enable (R0W1E).
        GENERATE_BIT_WITH_ENUM(r0swa, 4, 0b1, WindowArea);    ///< RBG0 window area (R0SWA).
        GENERATE_BIT_WITH_ENUM(r0swe, 5, 0b1, WindowEnable);  ///< RBG0 window SW enable (R0SWE).
        GENERATE_BIT_WITH_ENUM(r0log, 7, 0b1, WindowLogic);   ///< RBG0 overlapping window method (R0LOG).

        GENERATE_BIT_WITH_ENUM(spw0a, 8, 0b1, WindowArea);    ///< Sprite window area (SPW0A).
        GENERATE_BIT_WITH_ENUM(spw0e, 9, 0b1, WindowEnable);  ///< Sprite window W0 enable (SPW0E).
        GENERATE_BIT_WITH_ENUM(spw1a, 10, 0b1, WindowArea);   ///< Sprite window area (SPW1A).
        GENERATE_BIT_WITH_ENUM(spw1e, 11, 0b1, WindowEnable); ///< Sprite window W1 enable (SPW1E).
        GENERATE_BIT_WITH_ENUM(spswa, 12, 0b1, WindowArea);   ///< Sprite window area (SPSWA).
        GENERATE_BIT_WITH_ENUM(spswe, 13, 0b1, WindowEnable); ///< Sprite window SW enable (SPSWE).
        GENERATE_BIT_WITH_ENUM(splog, 15, 0b1, WindowLogic);  ///< Sprite overlapping window method (SPLOG).

        static constexpr auto lo_byte_pos = PosType(0);       ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8);       ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Wctlc", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Wctlc", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using WctlcType = Reg<u16, Wctlc>;
    WctlcType wctlc;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Wctld
    ///
    /// \brief	Window Control (Parameter Window, Color Calc. Window) (WCTLD).
    ///
    /// \author	Runik
    /// \date	18/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Wctld {
        GENERATE_USING(Wctld, u16);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum	WindowLogic
        ///
        /// \brief	Designates the method of overlapping windows used in each screen.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class WindowLogic : bool {
            overlaid_logic_or  = false, ///< Or.
            overlaid_logic_and = true   ///< And.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum	WindowEnable
        ///
        /// \brief	Designates whether to use the Normal window W0, W1 or sprite window SW in each screen.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class WindowEnable : bool {
            does_not_use_window = false, ///< Does not use window.
            uses_window         = true   ///< Uses window.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum	WindowArea
        ///
        /// \brief	Designates the valid area of the Normal window W0, W1 or sprite window SW used in each screen.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class WindowArea : bool {
            enables_inside_of_window = false, ///< Enables the inside of W0,W1 or SW window.
            enables_ouside_of_window = true   ///< Enables the outside of W0,W1 or SW window.
        };

        GENERATE_BIT_WITH_ENUM(rpw0a, 0, 0b1, WindowArea);    ///< Rotation parameter window area (RPW0A).
        GENERATE_BIT_WITH_ENUM(rpw0e, 1, 0b1, WindowEnable);  ///< Rotation parameter window W0 enable (RPW0E).
        GENERATE_BIT_WITH_ENUM(rpw1a, 2, 0b1, WindowArea);    ///< Rotation parameter window area (RPW1A).
        GENERATE_BIT_WITH_ENUM(rpw1e, 3, 0b1, WindowEnable);  ///< Rotation parameter window W1 enable (RPW1E).
        GENERATE_BIT_WITH_ENUM(rplog, 7, 0b1, WindowLogic);   ///< Rotation parameter overlapping window method (RPLOG).

        GENERATE_BIT_WITH_ENUM(ccw0a, 8, 0b1, WindowArea);    ///< Color calculation window area (CCW0A).
        GENERATE_BIT_WITH_ENUM(ccw0e, 9, 0b1, WindowEnable);  ///< Color calculation window W0 enable (CCW0E).
        GENERATE_BIT_WITH_ENUM(ccw1a, 10, 0b1, WindowArea);   ///< Color calculation window area (CCW1A).
        GENERATE_BIT_WITH_ENUM(ccw1e, 11, 0b1, WindowEnable); ///< Color calculation window W1 enable (CCW1E).
        GENERATE_BIT_WITH_ENUM(ccswa, 12, 0b1, WindowArea);   ///< Color calculation window area (CCSWA).
        GENERATE_BIT_WITH_ENUM(ccswe, 13, 0b1, WindowEnable); ///< Color calculation window SW enable (CCSWE).
        GENERATE_BIT_WITH_ENUM(cclog, 15, 0b1, WindowLogic);  ///< Color calculation overlapping window method (CCLOG).

        static constexpr auto lo_byte_pos = PosType(0);       ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8);       ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Wctld", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Wctld", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using WctldType = Reg<u16, Wctld>;
    WctldType wctld;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Lwtau
    ///
    /// \brief	Line Window Table Upper Address for W0, W1. (LWTA0U, LWTA1U)
    ///
    /// \author	Runik
    /// \date	18/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Lwtau {
        GENERATE_USING(Lwtau, u16);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum	LineWindowEnable
        ///
        /// \brief	Designates whether to make the Normal window a line window. (W0LVE, W1LVE)
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class LineWindowEnable : bool {
            does_not_process = false, ///< Does not process Normal Window to Line Window
            processes        = true   ///< Processes Normal Window to Line Window

        };
        GENERATE_BIT_WITHOUT_ENUM(lwtau, 0, 0b111);               ///< Line window table lead address (upper).
        GENERATE_BIT_WITH_ENUM(wxlwe, 15, 0b1, LineWindowEnable); ///< Line window enable.

        static constexpr auto lo_byte_pos = PosType(0);           ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8);           ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Lwtau", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Lwtau", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using LwtauType = Reg<u16, Lwtau>;
    LwtauType lwta0u;
    LwtauType lwta1u;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Lwtal
    ///
    /// \brief	Line Window Lower Table Address for W0, W1. (LWTA0L, LWTA1L)
    ///
    /// \author	Runik
    /// \date	18/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Lwtal {
        GENERATE_USING(Lwtal, u16);

        GENERATE_BIT_WITHOUT_ENUM(lwtal, 1, 0x7FFF);    ///< Line window table lead address (lower).

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Lwtal", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Lwtal", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using LwtalType = Reg<u16, Lwtal>;
    LwtalType lwta0l;
    LwtalType lwta1l;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Spctl
    ///
    /// \brief	Sprite Control (SPCTL).
    ///
    /// \author	Runik
    /// \date	18/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Spctl {
        GENERATE_USING(Spctl, u16);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   SpriteColorCalculationCondition
        ///
        /// \brief  SPCCCSx bits values.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class SpriteColorCalculationCondition : u8 {
            only_when_priority_is_less_or_equals_condition
            = 0b00,                                     ///< (Priority number) <= (Color calculation condition number) only.
            only_when_priority_equals_condition = 0b01, ///< (Priority number) = (Color calculation condition number) only.
            only_when_priority_is_higher_or_equals_condition
            = 0b10,                                     ///< (Priority number) >= (Color calculation condition number) only.
            only_when_color_data_msb_is_1 = 0b11        ///< Only when Color Data MSB is 1.
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

        GENERATE_BIT_WITH_ENUM(spcccs, 12, 0b11, SpriteColorCalculationCondition); ///< Sprite color calculation condition.
        GENERATE_BIT_WITHOUT_ENUM(spccn, 8, 0b111);                                ///< Sprite color calculation number.
        GENERATE_BIT_WITH_ENUM(spclmd, 5, 0b1, SpriteColorMode);                   ///< Sprite color mode.
        GENERATE_BIT_WITH_ENUM(spwinen, 4, 0b1, SpriteWindowEnable);               ///< Sprite window enable.
        GENERATE_BIT_WITH_ENUM(sptype, 0, 0b1111, SpriteType);                     ///< Sprite type.

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Spctl", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Spctl", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using SpctlType = Reg<u16, Spctl>;
    SpctlType spctl;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Sdctl
    ///
    /// \brief	Shadow Control (SDCTL).
    ///
    /// \author	Runik
    /// \date	18/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Sdctl {
        GENERATE_USING(Sdctl, u16);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum	ShadowEnable
        ///
        /// \brief	Determines whether to use the shadow function for the scroll screen and back screen.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class ShadowEnable : bool {
            does_not_use_shadow_function = false, ///< Does not use shadow function (shadow not added).
            uses_shadow_function         = true   ///< Uses shadow function (shadow added).
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum	TransparentShadowSelect
        ///
        /// \brief	Determines whether to activate the sprite of the transparent shadow.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class TransparentShadowSelect : bool {
            disables_transparent_shadow_sprite = false, ///< Disables transparent shadow sprite.
            enables_transparent_shadow_sprite  = true   ///< Enables transparent shadow sprite.
        };

        GENERATE_BIT_WITH_ENUM(n0sden, 0, 0b1, ShadowEnable);            ///< NBG0 (or RBG1) shadow enable (N0SDEN).
        GENERATE_BIT_WITH_ENUM(n1sden, 1, 0b1, ShadowEnable);            ///< NBG1 (or EXBG) shadow enable (N1SDEN).
        GENERATE_BIT_WITH_ENUM(n2sden, 2, 0b1, ShadowEnable);            ///< NBG2 shadow enable (N2SDEN).
        GENERATE_BIT_WITH_ENUM(n3sden, 3, 0b1, ShadowEnable);            ///< NBG3 shadow enable (N3SDEN).
        GENERATE_BIT_WITH_ENUM(r0sden, 4, 0b1, ShadowEnable);            ///< RBG0 shadow enable (R0SDEN).
        GENERATE_BIT_WITH_ENUM(bksden, 5, 0b1, ShadowEnable);            ///< Back screen shadow enable (BKSDEN).
        GENERATE_BIT_WITH_ENUM(tpsdsl, 8, 0b1, TransparentShadowSelect); ///< Transparent shadow select.

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Sdctl", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Sdctl", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using SdctlType = Reg<u16, Sdctl>;
    SdctlType sdctl;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Craofa
    ///
    /// \brief	Color RAM Address Offset (NBGx) (CRAOFA).
    ///
    /// \author	Runik
    /// \date	18/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Craofa {
        GENERATE_USING(Craofa, u16);

        GENERATE_BIT_WITHOUT_ENUM(n0caos, 0, 0b111);    ///< NBG0 color RAM address offset.
        GENERATE_BIT_WITHOUT_ENUM(n1caos, 4, 0b111);    ///< NBG1 color RAM address offset.
        GENERATE_BIT_WITHOUT_ENUM(n2caos, 8, 0b111);    ///< NBG2 color RAM address offset.
        GENERATE_BIT_WITHOUT_ENUM(n3caos, 12, 0b111);   ///< NBG3 color RAM address offset.

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Craofa", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Craofa", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using CraofaType = Reg<u16, Craofa>;
    CraofaType craofa;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Craofb
    ///
    /// \brief	Color RAM Address Offset (RBG0, SPRITE) (CRAOFB).
    ///
    /// \author	Runik
    /// \date	18/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Craofb {
        GENERATE_USING(Craofb, u16);

        GENERATE_BIT_WITHOUT_ENUM(r0caos, 0, 0b111);    ///< RBG0 color RAM address offset.
        GENERATE_BIT_WITHOUT_ENUM(spcaos, 4, 0b111);    ///< Sprite color RAM address offset.

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Craofb", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Craofb", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using CraofbType = Reg<u16, Craofb>;
    CraofbType craofb;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Lnclen
    ///
    /// \brief	Line Color Screen Enable (LNCLEN).
    ///
    /// \author	Runik
    /// \date	19/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Lnclen {
        GENERATE_USING(Lnclen, u16);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum	LineColorEnable
        ///
        /// \brief	Designates whether to insert the line color screen when each screen is a top image.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class LineColorEnable : bool {
            does_not_insert = false, ///< Does not insert the line color screen when corresponding screen is top image.
            inserts         = true   ///< Inserts the line color screen when corresponding screen is top image.
        };

        GENERATE_BIT_WITH_ENUM(n0lcen, 0, 0b1, LineColorEnable); ///< NBG0 (or RBG1) line color enable (N0LCEN).
        GENERATE_BIT_WITH_ENUM(n1lcen, 1, 0b1, LineColorEnable); ///< NBG1 (or EXBG) line color enable (N1LCEN).
        GENERATE_BIT_WITH_ENUM(n2lcen, 2, 0b1, LineColorEnable); ///< NBG2 line color enable (N2LCEN).
        GENERATE_BIT_WITH_ENUM(n3lcen, 3, 0b1, LineColorEnable); ///< NBG3 line color enable (N3LCEN).
        GENERATE_BIT_WITH_ENUM(r0lcen, 4, 0b1, LineColorEnable); ///< RBG0 line color enable (R0LCEN).
        GENERATE_BIT_WITH_ENUM(splcen, 5, 0b1, LineColorEnable); ///< Sprite line color enable (SPLCEN).

        static constexpr auto lo_byte_pos = PosType(0);          ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8);          ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Lnclen", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Lnclen", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using LnclenType = Reg<u16, Lnclen>;
    LnclenType lnclen;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Sfprmd
    ///
    /// \brief	Special Priority Mode (SFPRMD).
    ///
    /// \author	Runik
    /// \date	19/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Sfprmd {
        GENERATE_USING(Sfprmd, u16);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum	SpecialPriorityMode
        ///
        /// \brief	Designates the special priority function mode of each screen scroll.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class SpecialPriorityMode : u8 {
            mode_0      = 0b00, ///< Select the priority number LSB per each screen.
            mode_1      = 0b01, ///< Select the priority number LSB per each character.
            mode_2      = 0b10, ///< Select the priority number LSB per each dot.
            not_allowed = 0b11  ///< Selection not allowed.
        };

        GENERATE_BIT_WITH_ENUM(n0lcen, 0, 0b11, SpecialPriorityMode); ///< NBG0 (or RBG1) special priority mode (N0SPRM).
        GENERATE_BIT_WITH_ENUM(n1lcen, 1, 0b11, SpecialPriorityMode); ///< NBG1 (or EXBG) special priority mode (N1SPRM).
        GENERATE_BIT_WITH_ENUM(n2lcen, 2, 0b11, SpecialPriorityMode); ///< NBG2 special priority mode (N2SPRM).
        GENERATE_BIT_WITH_ENUM(n3lcen, 3, 0b11, SpecialPriorityMode); ///< NBG3 special priority mode (N3SPRM).
        GENERATE_BIT_WITH_ENUM(r0lcen, 4, 0b11, SpecialPriorityMode); ///< RBG0 special priority mode (R0SPRM).

        static constexpr auto lo_byte_pos = PosType(0);               ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8);               ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Sfprmd", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Sfprmd", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using SfprmdType = Reg<u16, Sfprmd>;
    SfprmdType sfprmd;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Ccctl
    ///
    /// \brief	Color Calculation Control (CCCTL).
    ///
    /// \author	Runik
    /// \date	19/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Ccctl {
        GENERATE_USING(Ccctl, u16);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   GradationEnable
        ///
        /// \brief  Determines whether to use the gradation function.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class GradationEnable : bool {
            do_not_use = false, ///< Do not use gradation calculation function.
            use        = true   ///< Use gradation calculation function.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   GradationScreenNumber
        ///
        /// \brief  Designates the screen using the gradation (shading) calculation function.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class GradationScreenNumber : u8 {
            sprite   = 0b000, ///< Sprite.
            rbg0     = 0b001, ///< RBG0
            nbg0     = 0b010, ///< NBG0 or RBG1
            invalid1 = 0b011, ///< Invalid
            nbg1     = 0b100, ///< NBG1 or EXBG
            nbg2     = 0b101, ///< NBG2
            nbg3     = 0b110, ///< NBG3
            invalid2 = 0b111  ///< Invalid
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   ExtendedColorCalculationEnable
        ///
        /// \brief  Determines whether to use the extended color calculation function.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class ExtendedColorCalculationEnable : bool {
            do_not_use = false, ///< Do not use extended color calculation.
            use        = true   ///< Use extended color calculation.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   ColorCalculationRatioMode
        ///
        /// \brief  Designates the color calculation ratio mode.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class ColorCalculationRatioMode : bool {
            select_per_top_screen_side    = false, ///< For color calculation ratio, select per top screen side.
            select_per_second_screen_side = true   ///< For color calculation ratio, select per second screen side.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   ColorCalculationMode
        ///
        /// \brief  Designates the color calculation ratio mode.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class ColorCalculationMode : bool {
            add_according_to_register = false, ///< Add according to the color calculation register value.
            add_as_is                 = true   ///< Add as is.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   ColorCalculationEnable
        ///
        /// \brief  Designates whether to perform color calculation (color calculation enable).
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class ColorCalculationEnable : bool {
            does_not_color_calculate = false, ///< Does not color-calculate.
            color_calculates         = true   ///< Color-calculates.
        };

        GENERATE_BIT_WITH_ENUM(boken, 15, 0b1, GradationEnable);                 ///< Gradation enable (BOKEN).
        GENERATE_BIT_WITH_ENUM(bokn, 12, 0b111, GradationScreenNumber);          ///< Gradation screen number (BOKNx).
        GENERATE_BIT_WITH_ENUM(exccen, 10, 0b1, ExtendedColorCalculationEnable); ///< Extended color calculation enable (EXCCEN).
        GENERATE_BIT_WITH_ENUM(ccrtmd, 9, 0b1, ColorCalculationRatioMode);       ///< Color calculation ratio mode (CCRTMD).
        GENERATE_BIT_WITH_ENUM(ccmd, 8, 0b1, ColorCalculationMode);              ///< Color calculation mode (CCMD).
        GENERATE_BIT_WITH_ENUM(spccen, 6, 0b1, ColorCalculationEnable);          ///< Sprite color calculation enable (SPCCEN).
        GENERATE_BIT_WITH_ENUM(lcccen, 5, 0b1, ColorCalculationEnable);          ///< LNCL color calculation enable (LCCCEN).
        GENERATE_BIT_WITH_ENUM(r0ccen, 4, 0b1, ColorCalculationEnable);          ///< RBG0 color calculation enable (R0CCEN).
        GENERATE_BIT_WITH_ENUM(n3ccen, 3, 0b1, ColorCalculationEnable);          ///< NBG3 color calculation enable (N3CCEN).
        GENERATE_BIT_WITH_ENUM(n2ccen, 2, 0b1, ColorCalculationEnable);          ///< NBG2 color calculation enable (N2CCEN).
        GENERATE_BIT_WITH_ENUM(n1ccen, 1, 0b1, ColorCalculationEnable); ///< NBG1 (or EXBG) color calculation enable (N1CCEN).
        GENERATE_BIT_WITH_ENUM(n0ccen, 0, 0b1, ColorCalculationEnable); ///< NBG0 (or RBG1) color calculation enable (N0CCEN).

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Ccctl", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Ccctl", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using CcctlType = Reg<u16, Ccctl>;
    CcctlType ccctl;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Sfccmd
    ///
    /// \brief	Special Color Calculation Mode (SFCCMD).
    ///
    /// \author	Runik
    /// \date	19/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Sfccmd {
        GENERATE_USING(Sfccmd, u16);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum	SpecialColorCalculationMode
        ///
        /// \brief	Designates the special color calculation function mode of each scroll screen.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class SpecialColorCalculationMode : u8 {
            per_screen          = 0b00, ///< Select color calculation enable per screen.
            per_character       = 0b01, ///< Select color calculation enable per character.
            per_dot             = 0b10, ///< Select color calculation enable per dot.
            with_color_data_msb = 0b11, ///< Select color calculation enable with color data MSB.
        };

        GENERATE_BIT_WITH_ENUM(r0sccm, 8, 0b11, SpecialColorCalculationMode); ///< RBG0 color calculation enable (R0SCCMx).
        GENERATE_BIT_WITH_ENUM(n3sccm, 6, 0b11, SpecialColorCalculationMode); ///< NBG3 color calculation enable (N3SCCMx).
        GENERATE_BIT_WITH_ENUM(n2sccm, 4, 0b11, SpecialColorCalculationMode); ///< NBG2 color calculation enable (N2SCCMx).
        GENERATE_BIT_WITH_ENUM(n1sccm,
                               2,
                               0b11,
                               SpecialColorCalculationMode); ///< NBG1 (or EXBG) color calculation enable (N1SCCMx).
        GENERATE_BIT_WITH_ENUM(n0sccm,
                               0,
                               0b11,
                               SpecialColorCalculationMode); ///< NBG0 (or RBG1) special color calculation enable (N0SCCMx).

        static constexpr auto lo_byte_pos = PosType(0);      ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8);      ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Sfccmd", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Sfccmd", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using SfccmdType = Reg<u16, Sfccmd>;
    SfccmdType sfccmd;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Prisa
    ///
    /// \brief	Priority Number (SPRITE 0,1) (PRISA).
    ///
    /// \author	Runik
    /// \date	20/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Prisa {
        GENERATE_USING(Prisa, u16);

        GENERATE_BIT_WITHOUT_ENUM(s0prin, 0, 0b111);    ///< Sprite register 0 (S0PRINx).
        GENERATE_BIT_WITHOUT_ENUM(s1prin, 8, 0b111);    ///< Sprite register 1 (S1PRINx).

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Prisa", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Prisa", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using PrisaType = Reg<u16, Prisa>;
    PrisaType prisa;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Prisb
    ///
    /// \brief	Priority Number (SPRITE 2,3) (PRISB).
    ///
    /// \author	Runik
    /// \date	20/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Prisb {
        GENERATE_USING(Prisb, u16);

        GENERATE_BIT_WITHOUT_ENUM(s2prin, 0, 0b111);    ///< Sprite register 2 (S2PRINx).
        GENERATE_BIT_WITHOUT_ENUM(s3prin, 8, 0b111);    ///< Sprite register 3 (S3PRINx).

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Prisb", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Prisb", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using PrisbType = Reg<u16, Prisb>;
    PrisbType prisb;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Prisc
    ///
    /// \brief	Priority Number (SPRITE 4,5) (PRISC).
    ///
    /// \author	Runik
    /// \date	20/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Prisc {
        GENERATE_USING(Prisc, u16);

        GENERATE_BIT_WITHOUT_ENUM(s4prin, 0, 0b111);    ///< Sprite register 4 (S4PRINx).
        GENERATE_BIT_WITHOUT_ENUM(s5prin, 8, 0b111);    ///< Sprite register 5 (S5PRINx).

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Prisc", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Prisc", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using PriscType = Reg<u16, Prisc>;
    PriscType prisc;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Prisd
    ///
    /// \brief	Priority Number (SPRITE 6,7) (PRISD).
    ///
    /// \author	Runik
    /// \date	20/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Prisd {
        GENERATE_USING(Prisd, u16);

        GENERATE_BIT_WITHOUT_ENUM(s6prin, 0, 0b111);    ///< Sprite register 6 (S6PRINx).
        GENERATE_BIT_WITHOUT_ENUM(s7prin, 8, 0b111);    ///< Sprite register 7 (S7PRINx).

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Prisd", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Prisd", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using PrisdType = Reg<u16, Prisd>;
    PrisdType prisd;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Prina
    ///
    /// \brief	Priority Number A (NBG0, NBG1) (PRINA).
    ///
    /// \author	Runik
    /// \date	20/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Prina {
        GENERATE_USING(Prina, u16);

        GENERATE_BIT_WITHOUT_ENUM(n0prin, 0, 0b111);    ///< NBG0 priority number (N0PRINx).
        GENERATE_BIT_WITHOUT_ENUM(n1prin, 8, 0b111);    ///< NBG1 priority number (N1PRINx).

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Prina", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Prina", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using PrinaType = Reg<u16, Prina>;
    PrinaType prina;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Prinb
    ///
    /// \brief	Priority Number B (NBG2, NBG3) (PRINB).
    ///
    /// \author	Runik
    /// \date	20/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Prinb {
        GENERATE_USING(Prinb, u16);

        GENERATE_BIT_WITHOUT_ENUM(n2prin, 0, 0b111);    ///< NBG2 priority number (N2PRINx).
        GENERATE_BIT_WITHOUT_ENUM(n3prin, 8, 0b111);    ///< NBG3 priority number (N3PRINx).

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Prinb", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Prinb", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using PrinbType = Reg<u16, Prinb>;
    PrinbType prinb;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Prir
    ///
    /// \brief	Priority Number R (Rbg0) (PRIR).
    ///
    /// \author	Runik
    /// \date	20/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Prir {
        GENERATE_USING(Prir, u16);

        GENERATE_BIT_WITHOUT_ENUM(r0prin, 0, 0b111);    ///< RBG0 priority number (R0PRINx).

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Prir", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Prir", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using PrirType = Reg<u16, Prir>;
    PrirType prir;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Ccrsa
    ///
    /// \brief	Color Calculation Ratio (Sprite 0,1) (CCRSA).
    ///
    /// \author	Runik
    /// \date	20/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Ccrsa {
        GENERATE_USING(Ccrsa, u16);

        GENERATE_BIT_WITHOUT_ENUM(s0ccrt, 0, 0b11111);  ///< Sprite register 0 color calculation ratio (S0CCRTx).
        GENERATE_BIT_WITHOUT_ENUM(s1ccrt, 8, 0b11111);  ///< Sprite register 1 color calculation ratio (S1CCRTx).

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Ccrsa", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Ccrsa", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using CcrsaType = Reg<u16, Ccrsa>;
    CcrsaType ccrsa;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Ccrsb
    ///
    /// \brief	Color Calculation Ratio (Sprite 2,3) (CCRSB).
    ///
    /// \author	Runik
    /// \date	20/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Ccrsb {
        GENERATE_USING(Ccrsb, u16);

        GENERATE_BIT_WITHOUT_ENUM(s2ccrt, 0, 0b11111);  ///< Sprite register 2 color calculation ratio (S2CCRTx).
        GENERATE_BIT_WITHOUT_ENUM(s3ccrt, 8, 0b11111);  ///< Sprite register 3 color calculation ratio (S3CCRTx).

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Ccrsb", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Ccrsb", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using CcrsbType = Reg<u16, Ccrsb>;
    CcrsbType ccrsb;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Ccrsc
    ///
    /// \brief	Color Calculation Ratio (Sprite 4,5) (CCRSC).
    ///
    /// \author	Runik
    /// \date	20/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Ccrsc {
        GENERATE_USING(Ccrsc, u16);

        GENERATE_BIT_WITHOUT_ENUM(s4ccrt, 0, 0b11111);  ///< Sprite register 4 color calculation ratio (S4CCRTx).
        GENERATE_BIT_WITHOUT_ENUM(s5ccrt, 8, 0b11111);  ///< Sprite register 5 color calculation ratio (S5CCRTx).

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Ccrsc", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Ccrsc", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using CcrscType = Reg<u16, Ccrsc>;
    CcrscType ccrsc;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Ccrsd
    ///
    /// \brief	Color Calculation Ratio (Sprite 6,7) (CCRSD).
    ///
    /// \author	Runik
    /// \date	20/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Ccrsd {
        GENERATE_USING(Ccrsd, u16);

        GENERATE_BIT_WITHOUT_ENUM(s6ccrt, 0, 0b11111);  ///< Sprite register 6 color calculation ratio (S6CCRTx).
        GENERATE_BIT_WITHOUT_ENUM(s7ccrt, 8, 0b11111);  ///< Sprite register 7 color calculation ratio (S7CCRTx).

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Ccrsd", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Ccrsd", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using CcrsdType = Reg<u16, Ccrsd>;
    CcrsdType ccrsd;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Ccrna
    ///
    /// \brief	Color Calculation Ratio (NBG0, NBG1) (CCRNA).
    ///
    /// \author	Runik
    /// \date	20/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Ccrna {
        GENERATE_USING(Ccrna, u16);

        GENERATE_BIT_WITHOUT_ENUM(n0ccrt, 0, 0b11111);  ///< NBG0 color calculation ratio (N0CCRTx).
        GENERATE_BIT_WITHOUT_ENUM(n1ccrt, 8, 0b11111);  ///< NBG1 color calculation ratio (N1CCRTx).

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Ccrna", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Ccrna", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using CcrnaType = Reg<u16, Ccrna>;
    CcrnaType ccrna;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Ccrnb
    ///
    /// \brief	Color Calculation Ratio (NBG2, NBG3) (CCRNB).
    ///
    /// \author	Runik
    /// \date	20/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Ccrnb {
        GENERATE_USING(Ccrnb, u16);

        GENERATE_BIT_WITHOUT_ENUM(n2ccrt, 0, 0b11111);  ///< NBG2 color calculation ratio (N2CCRTx).
        GENERATE_BIT_WITHOUT_ENUM(n3ccrt, 8, 0b11111);  ///< NBG3 color calculation ratio (N3CCRTx).

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Ccrnb", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Ccrnb", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using CcrnbType = Reg<u16, Ccrnb>;
    CcrnbType ccrnb;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Ccrr
    ///
    /// \brief	Color Calculation Ratio (RBG0) (CCRR).
    ///
    /// \author	Runik
    /// \date	20/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Ccrr {
        GENERATE_USING(Ccrr, u16);

        GENERATE_BIT_WITHOUT_ENUM(r0ccrt, 0, 0b11111);  ///< RBG0 color calculation ratio (R0CCRTx).

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Ccrr", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Ccrr", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using CcrrType = Reg<u16, Ccrr>;
    CcrrType ccrr;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Ccrlb
    ///
    /// \brief	Color Calculation Ratio (Line Color Screen, Back Screen) (CCRLB).
    ///
    /// \author	Runik
    /// \date	20/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Ccrlb {
        GENERATE_USING(Ccrlb, u16);

        GENERATE_BIT_WITHOUT_ENUM(lcccrt, 0, 0b11111);  ///< LNCL color calculation ratio (LCCCRTx).
        GENERATE_BIT_WITHOUT_ENUM(bkccrt, 8, 0b11111);  ///< Back color calculation ratio (BKCCRTx).

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Ccrlb", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Ccrlb", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using CcrlbType = Reg<u16, Ccrlb>;
    CcrlbType ccrlb;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Clofen
    ///
    /// \brief	Color Offset Enable (CLOFEN).
    ///
    /// \author	Runik
    /// \date	20/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Clofen {
        GENERATE_USING(Clofen, u16);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   ColorOffsetEnableBit
        ///
        /// \brief  Designates whether to use the color offset function.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class ColorOffsetEnable : bool {
            disabled = false, ///< Do not use color offset function.
            enabled  = true   ///< Use color offset function.
        };

        GENERATE_BIT_WITH_ENUM(n0coen, 0, 0b1, ColorOffsetEnable); ///< NBG0 (or RBG1) color offset enable (N0COEN).
        GENERATE_BIT_WITH_ENUM(n1coen, 1, 0b1, ColorOffsetEnable); ///< NBG1 (or EXBG) color offset enable (N1COEN).
        GENERATE_BIT_WITH_ENUM(n2coen, 2, 0b1, ColorOffsetEnable); ///< NBG2 color offset enable (N2COEN).
        GENERATE_BIT_WITH_ENUM(n3coen, 3, 0b1, ColorOffsetEnable); ///< NBG3 color offset enable (N3COEN).
        GENERATE_BIT_WITH_ENUM(r0coen, 4, 0b1, ColorOffsetEnable); ///< RBG0 color offset enable (R0COEN).
        GENERATE_BIT_WITH_ENUM(bkcoen, 5, 0b1, ColorOffsetEnable); ///< Back color offset enable (BKCOEN).
        GENERATE_BIT_WITH_ENUM(spcoen, 6, 0b1, ColorOffsetEnable); ///< Sprite color offset enable (SPCOEN).

        static constexpr auto lo_byte_pos = PosType(0);            ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8);            ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Clofen", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Clofen", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using ClofenType = Reg<u16, Clofen>;
    ClofenType clofen;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Clofsl
    ///
    /// \brief	Color Offset Select (CLOFSL).
    ///
    /// \author	Runik
    /// \date	20/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Clofsl {
        GENERATE_USING(Clofsl, u16);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   ColorOffsetSelect
        ///
        /// \brief  Designates the color offset register to use when using the color offset function.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class ColorOffsetSelect : bool {
            use_color_offset_a = false, ///< Use color offset A.
            use_color_offset_b = true   ///< Use color offset B.
        };

        GENERATE_BIT_WITH_ENUM(n0cosl, 0, 0b1, ColorOffsetSelect); ///< NBG0 (or RBG1) color offset select (N0COSL).
        GENERATE_BIT_WITH_ENUM(n1cosl, 1, 0b1, ColorOffsetSelect); ///< NBG1 (or EXBG) color offset select (N1COSL).
        GENERATE_BIT_WITH_ENUM(n2cosl, 2, 0b1, ColorOffsetSelect); ///< NBG2 color offset select (N2COSL).
        GENERATE_BIT_WITH_ENUM(n3cosl, 3, 0b1, ColorOffsetSelect); ///< NBG3 color offset select (N3COSL).
        GENERATE_BIT_WITH_ENUM(r0cosl, 4, 0b1, ColorOffsetSelect); ///< RBG0 color offset select (R0COSL).
        GENERATE_BIT_WITH_ENUM(bkcosl, 5, 0b1, ColorOffsetSelect); ///< Back color offset select (BKCOSL).
        GENERATE_BIT_WITH_ENUM(spcosl, 6, 0b1, ColorOffsetSelect); ///< Sprite color offset select (SPCOSL).

        static constexpr auto lo_byte_pos = PosType(0);            ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8);            ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Clofsl", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Clofsl", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using ClofslType = Reg<u16, Clofsl>;
    ClofslType clofsl;

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
    /// \struct	Coar
    ///
    /// \brief	Color Offset A (Red) (COAR).
    ///
    /// \author	Runik
    /// \date	20/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Coar {
        GENERATE_USING(Coar, u16);

        GENERATE_BIT_WITH_ENUM(sign, 8, 0b1, Sign);     ///< Sign of the data..
        GENERATE_BIT_WITHOUT_ENUM(coard, 0, 0xFF);      ///< Defines the color offset A red data (COARDx).

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Coar", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Coar", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using CoarType = Reg<u16, Coar>;
    CoarType coar;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Coag
    ///
    /// \brief	Color Offset A (Green) (COAG).
    ///
    /// \author	Runik
    /// \date	20/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Coag {
        GENERATE_USING(Coag, u16);

        GENERATE_BIT_WITH_ENUM(sign, 8, 0b1, Sign);     ///< Sign of the data..
        GENERATE_BIT_WITHOUT_ENUM(coagd, 0, 0xFF);      ///< Defines the color offset A green data (COAGDx).

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Coag", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Coag", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using CoagType = Reg<u16, Coag>;
    CoagType coag;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Coab
    ///
    /// \brief	Color Offset A (Blue)  (COAB).
    ///
    /// \author	Runik
    /// \date	20/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Coab {
        GENERATE_USING(Coab, u16);

        GENERATE_BIT_WITH_ENUM(sign, 8, 0b1, Sign);     ///< Sign of the data..
        GENERATE_BIT_WITHOUT_ENUM(coarb, 0, 0xFF);      ///< Defines the color offset A blue data (COABDx).

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Coab", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Coab", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using CoabType = Reg<u16, Coab>;
    CoabType coab;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Cobr
    ///
    /// \brief	Color Offset B (Red) (COBR).
    ///
    /// \author	Runik
    /// \date	20/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Cobr {
        GENERATE_USING(Cobr, u16);

        GENERATE_BIT_WITH_ENUM(sign, 8, 0b1, Sign);     ///< Sign of the data..
        GENERATE_BIT_WITHOUT_ENUM(cobrd, 0, 0xFF);      ///< Defines the color offset B red data (COBRDx).

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Cobr", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Cobr", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using CobrType = Reg<u16, Cobr>;
    CobrType cobr;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Cobg
    ///
    /// \brief	Color Offset B (Green) (COBG).
    ///
    /// \author	Runik
    /// \date	20/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Cobg {
        GENERATE_USING(Cobg, u16);

        GENERATE_BIT_WITH_ENUM(sign, 8, 0b1, Sign);     ///< Sign of the data..
        GENERATE_BIT_WITHOUT_ENUM(cobgr, 0, 0xFF);      ///< Defines the color offset B green data (COBGRx).

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Cobg", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Cobg", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using CobgType = Reg<u16, Cobg>;
    CobgType cobg;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Cobb
    ///
    /// \brief	Color Offset B (Blue)  (COBB).
    ///
    /// \author	Runik
    /// \date	20/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Cobb {
        GENERATE_USING(Cobb, u16);

        GENERATE_BIT_WITH_ENUM(sign, 8, 0b1, Sign);     ///< Sign of the data..
        GENERATE_BIT_WITHOUT_ENUM(cobbl, 0, 0xFF);      ///< Defines the color offset B blue data (COBBLx).

        static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("Vdp2Regs::Cobb", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("Vd2pRegs::Cobb", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
    };
    using CobbType = Reg<u16, Cobb>;
    CobbType cobb;
};
} // namespace saturnin::video
