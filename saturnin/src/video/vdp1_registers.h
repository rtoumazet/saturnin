//
// vdp1_registers.h
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
/// \file	vdp1_registers.h
///
/// \brief	Declares VDP1 registers and related enums.
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <saturnin/src/emulator_defs.h>
#include <saturnin/src/bitfield.h>
#include <saturnin/src/bit_register.h>

namespace saturnin::video {
namespace vdp1_register_address {
constexpr u32 tv_mode_selection                  = {0x25d00000};
constexpr u32 frame_buffer_change_mode           = {0x25d00002};
constexpr u32 plot_trigger                       = {0x25d00004};
constexpr u32 erase_write_data                   = {0x25d00006};
constexpr u32 erase_write_upper_left_coordinate  = {0x25d00008};
constexpr u32 erase_write_lower_right_coordinate = {0x25d0000a};
constexpr u32 draw_forced_termination            = {0x25d0000c};
constexpr u32 transfer_end_status                = {0x25d00010};
constexpr u32 last_operation_command_address     = {0x25d00012};
constexpr u32 current_operation_command_address  = {0x25d00014};
constexpr u32 mode_status                        = {0x25d00016};
}; // namespace vdp1_register_address

struct Vdp1Regs {
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Tvmr
    ///
    /// \brief	TV Mode Selection register (TVMR).
    ///
    /// \author	Runik
    /// \date	08/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Tvmr {
        GENERATE_USING(Tvmr, u16);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   VBlankEraseWriteEnable
        ///
        /// \brief  VBE bit values.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class VBlankEraseWriteEnable : bool {
            no_erase_write_performed_during_vblank = false, ///< Erase/Write is not performed during VBlank.
            erase_write_performed_during_vblank    = true   ///< Erase/Write is performed during VBlank.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   HdtvEnable
        ///
        /// \brief  TVM bit 2 values.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class HdtvEnable : bool {
            ntsc_pal  = false, ///< NTSC, PAL.
            hdtv_31kc = true   ///< HDTV, 31KC.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   FrameBufferRotationEnable
        ///
        /// \brief  TVM bit 1 values.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class FrameBufferRotationEnable : bool {
            no_rotation = false, ///< No rotation.
            rotation    = true   ///< Rotation.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   BitDepthSelection
        ///
        /// \brief  TVM bit 0 values.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class BitDepthSelection : bool {
            sixteen_bits_per_pixel = false, ///< 16 bits / pixel.
            eight_bits_per_pixel   = true   ///< 8 bits / pixel.
        };

        GENERATE_BIT_WITH_ENUM(tvm0, 0, 0b1, BitDepthSelection);         ///< Bit depth selection (TVM0).
        GENERATE_BIT_WITH_ENUM(tvm1, 1, 0b1, FrameBufferRotationEnable); ///< Frame buffer rotation enable (TVM1).
        GENERATE_BIT_WITH_ENUM(tvm2, 2, 0b1, HdtvEnable);                ///< HDTV enable (TVM2).
        GENERATE_BIT_WITH_ENUM(vbe, 3, 0b1, VBlankEraseWriteEnable);     ///< VBlank erase write enable (VBE).
    };
    using TvmrType = Reg<u16, Tvmr>;
    TvmrType tvmr;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Fbcr
    ///
    /// \brief	Frame Buffer Switch Mode register (FBCR).
    ///
    /// \author	Runik
    /// \date	08/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Fbcr {
        GENERATE_USING(Fbcr, u16);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   EvenOddCoordinateSelect
        ///
        /// \brief  EOS bit values.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class EvenOddCoordinateSelect : bool {
            samples_pixels_at_even_coordinates = false, ///< Samples only pixels at even coordinates.
            samples_pixels_at_odd_coordinates  = true   ///< Samples only pixels at odd coordinates.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   DoubleInterlaceEnable
        ///
        /// \brief  DIE bit values.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class DoubleInterlaceEnable : bool {
            non_interlace_or_single_interlace = false, ///< Non-interlace/single interlace.
            double_interlace                  = true   ///< Double interlace.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   DoubleInterlaceDrawLine
        ///
        /// \brief  DIL bit values.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class DoubleInterlaceDrawLine : bool {
            not_set = false, ///< DIL bit not set.
            set     = true   ///< DIL bit set.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   DoubleInterlaceBothBits
        ///
        /// \brief  DIE + DIL bits values.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class DoubleInterlaceBothBits : u8 {
            plot_both_even_and_odd_lines = 0b00, ///< Plot both even and odd lines.
            plot_even_numbered_lines     = 0b10, ///< Plot even numbered lines only.
            plot_odd_numbered_lines      = 0b11  ///< Plot odd numbered lines only.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   FrameBufferChangeModeBit
        ///
        /// \brief  FCM bit values.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class FrameBufferChangeModeBit : bool {
            automatic_mode = false, ///< Automatic mode change.
            manual_mode    = true   ///< Manual mode change.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   FrameBufferChangeBothBits
        ///
        /// \brief  FCM + FCT bits values.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class FrameBufferChangeBothBits : u8 {
            one_cycle_mode    = 0b00, ///< 1-cycle mode, change every 1/60 second (when VBE = 0).
            manual_mode_erase = 0b10, ///< Manual mode (erase), erase in next field (when VBE = 0).
            manual_mode_change_and_or_erase
            = 0b11                    ///< Manual mode (change), change in next field (when VBE = 0) Manual mode (erase and
                                      ///< change), erase by VBlank and change in next field (when VBE = 1).
        };

        GENERATE_BIT_WITH_ENUM(fcm_fct, 0, 0b11, FrameBufferChangeBothBits); ///< Frame buffer change both bits (FCM + FCT).
        GENERATE_BIT_WITH_ENUM(fcm, 1, 0b1, FrameBufferChangeModeBit);       ///< Frame buffer change mode bit (FCM).
        GENERATE_BIT_WITH_ENUM(die_dil, 2, 0b11, DoubleInterlaceBothBits);   ///< Double interlace both bits (DIE + DIL).
        GENERATE_BIT_WITH_ENUM(dil, 2, 0b1, DoubleInterlaceDrawLine);        ///< Double interlace draw line (DIL).
        GENERATE_BIT_WITH_ENUM(die, 3, 0b1, DoubleInterlaceEnable);          ///< Double interlaceE enable (DIE).
        GENERATE_BIT_WITH_ENUM(eos, 4, 0b1, EvenOddCoordinateSelect);        ///< Even odd coordinate select (EOS).
    };
    using FbcrType = Reg<u16, Fbcr>;
    FbcrType fbcr;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Ptmr
    ///
    /// \brief	Plot Trigger register (PTMR).
    ///
    /// \author	Runik
    /// \date	08/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Ptmr {
        GENERATE_USING(Ptmr, u16);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   PlotTriggerMode
        ///
        /// \brief  PTM bits values.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class PlotTriggerMode : u8 {
            idle_at_frame_change           = 0b00, ///< Idle at frame change.
            starts_drawing_when_written    = 0b01, ///< Starts drawing when 0b01 is written.
            starts_drawing_at_frame_change = 0b10  ///< Starts drawing automatically with frame change.

        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   PlotTriggerMode1
        ///
        /// \brief  PTM bit 1 values.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class PlotTriggerMode1 : bool {
            cleared = false, ///< Bit is cleared.
            set     = true   ///< Bit is set.

        };

        GENERATE_BIT_WITH_ENUM(ptm, 0, 0b11, PlotTriggerMode);  ///< Plot trigger mode (PTM).
        GENERATE_BIT_WITH_ENUM(ptm1, 1, 0b1, PlotTriggerMode1); ///< Plot trigger mode bit 1 (PTM1).
    };
    using PtmrType = Reg<u16, Ptmr>;
    PtmrType ptmr;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Ewdr
    ///
    /// \brief	Erase/Write Data register (EWDR).
    ///
    /// \author	Runik
    /// \date	08/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Ewdr {
        GENERATE_USING(Ewdr, u16);

        static constexpr auto ew16_pos  = PosType(0); // Erase/Write data (16 bits/pixel).
        static constexpr auto ew8xe_pos = PosType(0); // Erase/Write data for even X coordinates.
        static constexpr auto ew8xo_pos = PosType(8); // Erase/Write data for odd X coordinates.

        static constexpr auto ew16_mask = 0xFFFF;
        static constexpr auto ew8_mask  = 0xFF;

        static constexpr auto ew16_shft  = ShiftedType(ew16_mask, ew16_pos);
        static constexpr auto ew8xe_shft = ShiftedType(ew8_mask, ew8xe_pos);
        static constexpr auto ew8xo_shft = ShiftedType(ew8_mask, ew8xo_pos);
    };
    using EwdrType = Reg<u16, Ewdr>;
    EwdrType ewdr;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Ewlr
    ///
    /// \brief	Erase/Write Upper-Left Coordinate register (EWLR).
    ///
    /// \author	Runik
    /// \date	08/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Ewlr {
        GENERATE_USING(Ewlr, u16);

        static constexpr auto ulcy1_pos = PosType(0); // Erase/Write Upper Left Coordinate Y1.
        static constexpr auto ulcx1_pos = PosType(9); // Erase/Write Upper Left Coordinate X1.

        static constexpr auto ulcy1_mask = 0x1FF;
        static constexpr auto ulcx1_mask = 0x3F;

        static constexpr auto ulcy1_shft = ShiftedType(ulcy1_mask, ulcy1_pos);
        static constexpr auto ulcx1_shft = ShiftedType(ulcx1_mask, ulcx1_pos);
    };
    using EwlrType = Reg<u16, Ewlr>;
    EwlrType ewlr;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Ewrr
    ///
    /// \brief	Erase/Write Lower-Right Coordinate register (EWRR).
    ///
    /// \author	Runik
    /// \date	10/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Ewrr {
        GENERATE_USING(Ewrr, u16);

        static constexpr auto lrcy3_pos = PosType(0); // Erase/Write Lower Right Coordinate Y3.
        static constexpr auto lrcx3_pos = PosType(9); // Erase/Write Lower Right Coordinate X3.

        static constexpr auto lrcy3_mask = 0x1FF;
        static constexpr auto lrcx3_mask = 0x7F;

        static constexpr auto lrcy3_shft = ShiftedType(lrcy3_mask, lrcy3_pos);
        static constexpr auto lrcx3_shft = ShiftedType(lrcx3_mask, lrcx3_pos);
    };
    using EwrrType = Reg<u16, Ewrr>;
    EwrrType ewrr;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Endr
    ///
    /// \brief	Draw Forced Termination register (ENDR).
    ///
    /// \author	Runik
    /// \date	10/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Endr {};
    using EndrType = Reg<u16, Endr>;
    EndrType endr;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Edsr
    ///
    /// \brief	Transfer End Status register (EDSR).
    ///
    /// \author	Runik
    /// \date	10/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Edsr {
        GENERATE_USING(Edsr, u16);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   CurrentEndBitFetchStatus
        ///
        /// \brief  CEF bit values.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class CurrentEndBitFetchStatus : bool {
            end_bit_not_fetched = false, ///< The end bit in current frame has not been fetched.
            end_bit_fetched     = true   ///< The end bit in current frame has been fetched and plotting is ended.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   BeforeEndBitFetchStatus
        ///
        /// \brief  BEF bit values.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class BeforeEndBitFetchStatus : bool {
            end_bit_not_fetched = false, ///< The end bit in previous frame has not been fetched.
            end_bit_fetched     = true   ///< The end bit in previous frame has been fetched and drawing is terminated.
        };

        GENERATE_BIT_WITH_ENUM(bef, 0, 0b1, BeforeEndBitFetchStatus);  ///< Before end bit fetch status (BEF).
        GENERATE_BIT_WITH_ENUM(cef, 1, 0b1, CurrentEndBitFetchStatus); ///< Current end bit fetch status (CEF).
    };
    using EdsrType = Reg<u16, Edsr>;
    EdsrType edsr;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Lopr
    ///
    /// \brief	Last Operation Command Address register (LOPR).
    ///
    /// \author	Runik
    /// \date	10/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Lopr {};
    using LoprType = Reg<u16, Lopr>;
    LoprType lopr;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Copr
    ///
    /// \brief	Current Operation Command Address register (COPR).
    ///
    /// \author	Runik
    /// \date	10/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Copr {};
    using CoprType = Reg<u16, Copr>;
    CoprType copr;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Modr
    ///
    /// \brief	Mode Status register (MODR).
    ///
    /// \author	Runik
    /// \date	10/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Modr {
        GENERATE_USING(Modr, u16);

        GENERATE_BIT_WITHOUT_ENUM(tvm0, 0, 0b1); ///< Defines TVM bit 0.
        GENERATE_BIT_WITHOUT_ENUM(tvm1, 1, 0b1); ///< Defines TVM bit 1.
        GENERATE_BIT_WITHOUT_ENUM(tvm2, 2, 0b1); ///< Defines TVM bit 2.
        GENERATE_BIT_WITHOUT_ENUM(vbe, 3, 0b1);  ///< Defines VBE bit.
        GENERATE_BIT_WITHOUT_ENUM(fcm, 4, 0b1);  ///< Defines FCM bit.
        GENERATE_BIT_WITHOUT_ENUM(dil, 5, 0b1);  ///< Defines DIL bit.
        GENERATE_BIT_WITHOUT_ENUM(die, 6, 0b1);  ///< Defines DIE bit.
        GENERATE_BIT_WITHOUT_ENUM(eos, 7, 0b1);  ///< Defines EOS bit.
        GENERATE_BIT_WITHOUT_ENUM(ptm1, 8, 0b1); ///< Defines PTM1 bit.

        static constexpr auto ver_pos  = PosType(12);
        static constexpr auto ver_mask = 0xF;
        static constexpr auto ver_shft = ShiftedType(ver_mask, ver_pos);
        GENERATE_MASKED_RANGE("Vdp1Regs::Modr", VER, ver, ver_mask, ver_pos, ver_mask);
    };
    using ModrType = Reg<u16, Modr>;
    ModrType modr;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct	CmdCtrl
///
/// \brief	Control Words.
///
/// \author	Runik
/// \date	10/04/2023
////////////////////////////////////////////////////////////////////////////////////////////////////

struct CmdCtrl {
    GENERATE_USING(CmdCtrl, u16);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \enum   EndBit
    ///
    /// \brief  END bit values.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    enum class EndBit : bool {
        command_selection_valid = false, ///< Command selection valid.
        draw_ended              = true   ///< Draw end command
    };

    enum class JumpSelect : u8 {
        jump_next   = 0b000, ///< Automatically jumps to next table after this table is processed (CMDLINK is ignored).
        jump_assign = 0b001, ///< Jumps to CMDLINK table after this table is processed.
        jump_call   = 0b010, ///< CMDLINK table receives subroutine call after this table is processed.
        jump_return = 0b011, ///< Returns to main routine after this table is processed.
        skip_next   = 0b100, ///< Jumps to next table without processing this table (CMDLINK is ignored).
        skip_assign = 0b101, ///< Jumps to CMDLINK table without processing this table.
        skip_call   = 0b110, ///< CMDLINK table receives subroutine call without processing this table.
        skip_return = 0b111, ///< Returns to main routine without processing this table.
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \enum   ZoomPoint
    ///
    /// \brief  ZP bits values.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    enum class ZoomPoint : u8 {
        two_coordinates = 0b0000, ///< Specifies two coordinates.
        upper_left      = 0b0101, ///< Upper left.
        upper_center    = 0b0110, ///< Upper center.
        upper_right     = 0b0111, ///< Upper right.
        center_left     = 0b1001, ///< Center left.
        center_center   = 0b1010, ///< Center center.
        center_right    = 0b1011, ///< Center right.
        lower_left      = 0b1101, ///< Lower left.
        lower_center    = 0b1110, ///< Lower center.
        lower_right     = 0b1111  ///< Lower right.
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \enum   CharacterReadDirection
    ///
    /// \brief  Dir bits values.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    enum class CharacterReadDirection : u8 {
        not_inverted = 0b00, ///< Not inverted.
        h_invertion  = 0b01, ///< Inverted horizontally.
        v_invertion  = 0b10, ///< Inverted vertically.
        vh_invertion = 0b11, ///< Inverted vertically and horizontally.
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \enum   CommandSelect
    ///
    /// \brief  Comm bits values.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    enum class CommandSelect : u8 {
        normal_sprite_draw    = 0b0000, ///< Normal sprite draw command.
        scaled_sprite_draw    = 0b0001, ///< Scaled sprite draw command.
        distorted_sprite_draw = 0b0010, ///< Distorted sprite draw command.
        polygon_draw          = 0b0100, ///< Polygon draw command.
        polyline_draw         = 0b0101, ///< Polyline draw command.
        line_draw             = 0b0110, ///< Line draw command.
        user_clipping         = 0b1000, ///< User clipping coordinates set command.
        system_clipping       = 0b1001, ///< System clipping coordinates set command.
        local_coordinate      = 0b1010  ///< Local coordinate set command.
    };

    GENERATE_BIT_WITH_ENUM(comm, 0, 0b1111, CommandSelect);       ///< Command select.
    GENERATE_BIT_WITH_ENUM(dir, 4, 0b11, CharacterReadDirection); ///< Character read direction.
    GENERATE_BIT_WITH_ENUM(zp, 8, 0b1111, ZoomPoint);             ///< Zoom point.
    GENERATE_BIT_WITH_ENUM(js, 12, 0b111, JumpSelect);            ///< Jump select.
    GENERATE_BIT_WITH_ENUM(end, 15, 0b1, EndBit);                 ///< End bit.
};
using CmdCtrlType = Reg<u16, CmdCtrl>;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct	CmdLink
///
/// \brief	Link specification.
///
/// \author	Runik
/// \date	10/04/2023
////////////////////////////////////////////////////////////////////////////////////////////////////

struct CmdLink {};
using CmdLinkType = Reg<u16, CmdLink>;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct	CmdPmod
///
/// \brief	Draw Mode Word.
///
/// \author	Runik
/// \date	10/04/2023
////////////////////////////////////////////////////////////////////////////////////////////////////

struct CmdPmod {
    GENERATE_USING(CmdPmod, u16);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \enum   MsbOn
    ///
    /// \brief  MON bit values.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    enum class MsbOn : bool {
        off = false, ///< MSB is 0 on the framebuffer pixels that should be drawn.
        on  = true   ///< MSB is 1 on the framebuffer pixels that should be drawn.
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \enum   HighSpeedShrink
    ///
    /// \brief  HSS bit values.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    enum class HighSpeedShrink : bool {
        disabled = false, ///< High Speed Shrink is disabled.
        enabled  = true   ///< High Speed Shrink is enabled.
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \enum   PreClippingDisable
    ///
    /// \brief  PCLP bit values.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    enum class PreClippingDisable : bool {
        pre_clipping    = false, ///< Pre-clipping with horizontal inversion.
        no_pre_clipping = true   ///< No pre-clipping and no horizontal inversion.
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \enum   UserClippingEnable
    ///
    /// \brief  CLIP bit values.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    enum class UserClippingEnable : bool {
        ignored = false, ///< User clipping coordinates are ignored.
        enabled = true   ///< Part is clipped and drawn according to the user clipping coordinates and clipping mode bit.
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \enum   UserClippingMode
    ///
    /// \brief  CMOD bit values.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    enum class UserClippingMode : bool {
        drawing_inside  = false, ///< Drawing is performed inside the clipping area.
        drawing_outside = true   ///< Drawing is performed outside the clipping area.
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \enum   MeshEnable
    ///
    /// \brief  MESH bit values.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    enum class MeshEnable : bool {
        disabled = false, ///< Draw without mesh processing.
        enabled  = true   ///< Draw with mesh processing.
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \enum   EndCodeDisable
    ///
    /// \brief  ECD bit values.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    enum class EndCodeDisable : bool {
        enabled  = false, ///< End code is enabled.
        disabled = true   ///< End code is disabled.
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \enum   TransparentPixelDisable
    ///
    /// \brief  SPD bit values.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    enum class TransparentPixelDisable : bool {
        transparent_pixel_enabled  = false, ///< Transparent pixel is enabled.
        transparent_pixel_disabled = true   ///< Transparent pixel disabled.
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \enum   ColorMode
    ///
    /// \brief  Color mode bits values.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    enum class ColorMode : u8 {
        mode_0_16_colors_bank   = 0b000, ///< 16 colors, color bank mode, 4 bits per pixel.
        mode_1_16_colors_lookup = 0b001, ///< 16 colors, lookup table mode, 4 bits per pixel.
        mode_2_64_colors_bank   = 0b010, ///< 64 colors, color bank mode, 8 bits per pixel.
        mode_3_128_colors_bank  = 0b011, ///< 128 colors, color bank mode, 8 bits per pixel.
        mode_4_256_colors_bank  = 0b100, ///< 256 colors, color bank mode, 8 bits per pixel.
        mode_5_32k_colors_rgb   = 0b101, ///< 32K colors, RGB mode, 16 bits per pixel.
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \enum   ColorCalculation
    ///
    /// \brief  Color calculation bits values.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    enum class ColorCalculation : u8 {
        mode_0 = 0b000, ///< Replace.
        mode_1 = 0b001, ///< Cannot rewrite / Shadow.
        mode_2 = 0b010, ///< Half-luminance.
        mode_3 = 0b011, ///< Replace / Half-transparent.
        mode_4 = 0b100, ///< Gouraud shading.
        mode_5 = 0b101, ///< Prohibited.
        mode_6 = 0b110, ///< Gouraud shading + half-luminance.
        mode_7 = 0b111, ///< Gouraud shading + half-transparent.
    };

    enum class GouraudShading : bool { disabled = false, enabled = true };

    GENERATE_BIT_WITH_ENUM(cc, 0, 0b111, ColorCalculation);       ///< Color calculation bits.
    GENERATE_BIT_WITH_ENUM(gs, 2, 0b1, GouraudShading);           ///< Gouraud shading bit.
    GENERATE_BIT_WITH_ENUM(cm, 3, 0b111, ColorMode);              ///< Color mode bits.
    GENERATE_BIT_WITH_ENUM(spd, 6, 0b1, TransparentPixelDisable); ///< Transparent pixel disable bit.
    GENERATE_BIT_WITH_ENUM(ecd, 7, 0b1, EndCodeDisable);          ///< End code disable bit.
    GENERATE_BIT_WITH_ENUM(mesh, 8, 0b1, MeshEnable);             ///< Mesh enable bit.
    GENERATE_BIT_WITH_ENUM(cmod, 9, 0b1, UserClippingMode);       ///< User clipping mode bit.
    GENERATE_BIT_WITH_ENUM(clip, 10, 0b1, UserClippingEnable);    ///< User clipping enable bit.
    GENERATE_BIT_WITH_ENUM(pclp, 11, 0b1, PreClippingDisable);    ///< Pre clipping disable bit.
    GENERATE_BIT_WITH_ENUM(hss, 12, 0b1, HighSpeedShrink);        ///< High speed shrink bit.
    GENERATE_BIT_WITH_ENUM(mon, 15, 0b1, MsbOn);                  ///< MSB on bit.
};
using CmdPmodType = Reg<u16, CmdPmod>;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct	CmdColr
///
/// \brief	Color Control Word.
///
/// \author	Runik
/// \date	11/04/2023
////////////////////////////////////////////////////////////////////////////////////////////////////

struct CmdColr {};
using CmdColrType = Reg<u16, CmdColr>;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct	CmdSrca
///
/// \brief	Character Address.
///
/// \author	Runik
/// \date	11/04/2023
////////////////////////////////////////////////////////////////////////////////////////////////////

struct CmdSrca {};
using CmdSrcaType = Reg<u16, CmdSrca>;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct	CmdSize
///
/// \brief	Character Size.
///
/// \author	Runik
/// \date	11/04/2023
////////////////////////////////////////////////////////////////////////////////////////////////////

struct CmdSize {
    GENERATE_USING(CmdSize, u16);

    GENERATE_BIT_WITHOUT_ENUM(chszy, 0, 0xFF); ///< Character Size Y.
    GENERATE_BIT_WITHOUT_ENUM(chszx, 8, 0x3F); ///< Character Size X divided by 8.

    // static constexpr auto chszy_pos = PosType(0);
    // static constexpr auto chszx_pos = PosType(8); ///< Character Size X divided by 8.

    // static constexpr auto chszy_mask = 0xFF;
    // static constexpr auto chszx_mask = 0x3F;

    // static constexpr auto chszy_shft = ShiftedType(chszy_mask, chszy_pos);
    // static constexpr auto chszx_shft = ShiftedType(chszx_mask, chszx_pos);
};
using CmdSizeType = Reg<u16, CmdSize>;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct	CmdVertexCoordinate
///
/// \brief	Vertex Coordinate data.
///
/// \author	Runik
/// \date	11/04/2023
////////////////////////////////////////////////////////////////////////////////////////////////////

struct CmdVertexCoordinate {};
using CmdVertexCoordinateType = Reg<u16, CmdVertexCoordinate>;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct	CmdGrda
///
/// \brief	Gouraud Shading Table.
///
/// \author	Runik
/// \date	11/04/2023
////////////////////////////////////////////////////////////////////////////////////////////////////

struct CmdGrda {};
using CmdGrdaType = Reg<u16, CmdGrda>;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct	SpriteTypeRegister
///
/// \brief	Sprite type register.
///
/// \author	Runik
/// \date	11/04/2023
////////////////////////////////////////////////////////////////////////////////////////////////////

struct SpriteTypeRegister {
    GENERATE_USING(SpriteTypeRegister, u16);

    GENERATE_BIT_WITHOUT_ENUM(type_0_priority_palette, 14, 0b11);   ///< Priority for Type 0 sprites on palette only fb.
    GENERATE_BIT_WITHOUT_ENUM(type_0_priority_mixed, 14, 0b1);      ///< Priority for Type 0 sprites on mixed fb.
    GENERATE_BIT_WITHOUT_ENUM(type_0_color_calculation, 11, 0b11);  ///< Color calculation for Type 0 sprites.
    GENERATE_BIT_WITHOUT_ENUM(type_0_dot_color, 0, 0b11111111111);  ///< Dot color for Type 0 sprites.

    GENERATE_BIT_WITHOUT_ENUM(type_1_priority_palette, 13, 0b111);  ///< Priority for Type 1 sprites on palette only fb.
    GENERATE_BIT_WITHOUT_ENUM(type_1_priority_mixed, 13, 0b11);     ///< Priority for Type 1 sprites on mixed fb.
    GENERATE_BIT_WITHOUT_ENUM(type_1_color_calculation, 11, 0b11);  ///< Color calculation for Type 1 sprites.
    GENERATE_BIT_WITHOUT_ENUM(type_1_dot_color, 0, 0b11111111111);  ///< Dot color for Type 1 sprites.

    GENERATE_BIT_WITHOUT_ENUM(type_2_priority, 14, 0b1);            ///< Priority for Type 2 sprites.
    GENERATE_BIT_WITHOUT_ENUM(type_2_shadow_palette, 15, 0b1);      ///< Shadow for Type 2 sprites on palette only fb.
    GENERATE_BIT_WITHOUT_ENUM(type_2_color_calculation, 11, 0b111); ///< Color calculation for Type 2 sprites.
    GENERATE_BIT_WITHOUT_ENUM(type_2_dot_color, 0, 0b11111111111);  ///< Dot color for Type 2 sprites.

    GENERATE_BIT_WITHOUT_ENUM(type_3_shadow_palette, 15, 0b1);      ///< Shadow for Type 3 sprites on palette only fb.
    GENERATE_BIT_WITHOUT_ENUM(type_3_priority, 13, 0b11);           ///< Priority for Type 3 sprites.
    GENERATE_BIT_WITHOUT_ENUM(type_3_color_calculation, 11, 0b11);  ///< Color calculation for Type 3 sprites.
    GENERATE_BIT_WITHOUT_ENUM(type_3_dot_color, 0, 0b11111111111);  ///< Dot color for Type 3 sprites.

    GENERATE_BIT_WITHOUT_ENUM(type_4_shadow_palette, 15, 0b1);      ///< Shadow for Type 4 sprites on palette only fb.
    GENERATE_BIT_WITHOUT_ENUM(type_4_priority, 13, 0b11);           ///< Priority for Type 4 sprites.
    GENERATE_BIT_WITHOUT_ENUM(type_4_color_calculation, 10, 0b111); ///< Color calculation for Type 4 sprites.
    GENERATE_BIT_WITHOUT_ENUM(type_4_dot_color, 0, 0b1111111111);   ///< Dot color for Type 4 sprites.

    GENERATE_BIT_WITHOUT_ENUM(type_5_shadow_palette, 15, 0b1);      ///< Shadow for Type 5 sprites on palette only fb.
    GENERATE_BIT_WITHOUT_ENUM(type_5_priority, 12, 0b111);          ///< Priority for Type 5 sprites.
    GENERATE_BIT_WITHOUT_ENUM(type_5_color_calculation, 11, 0b1);   ///< Color calculation for Type 5 sprites.
    GENERATE_BIT_WITHOUT_ENUM(type_5_dot_color, 0, 0b11111111111);  ///< Dot color for Type 5 sprites.

    GENERATE_BIT_WITHOUT_ENUM(type_6_shadow_palette, 15, 0b1);      ///< Shadow for Type 6 sprites on palette only fb.
    GENERATE_BIT_WITHOUT_ENUM(type_6_priority, 12, 0b111);          ///< Priority for Type 6 sprites.
    GENERATE_BIT_WITHOUT_ENUM(type_6_color_calculation, 10, 0b11);  ///< Color calculation for Type 6 sprites.
    GENERATE_BIT_WITHOUT_ENUM(type_6_dot_color, 0, 0b1111111111);   ///< Dot color for Type 6 sprites.

    GENERATE_BIT_WITHOUT_ENUM(type_7_shadow_palette, 15, 0b1);      ///< Shadow for Type 7 sprites on palette only fb.
    GENERATE_BIT_WITHOUT_ENUM(type_7_priority, 12, 0b111);          ///< Priority for Type 7 sprites.
    GENERATE_BIT_WITHOUT_ENUM(type_7_color_calculation, 9, 0b111);  ///< Color calculation for Type 7 sprites.
    GENERATE_BIT_WITHOUT_ENUM(type_7_dot_color, 0, 0b111111111);    ///< Dot color for Type 7 sprites.

    GENERATE_BIT_WITHOUT_ENUM(type_8_priority, 7, 0b1);             ///< Priority for Type 8 sprites.
    GENERATE_BIT_WITHOUT_ENUM(type_8_dot_color, 0, 0b1111111);      ///< Dot color for Type 8 sprites.

    GENERATE_BIT_WITHOUT_ENUM(type_9_priority, 7, 0b1);             ///< Priority for Type 9 sprites.
    GENERATE_BIT_WITHOUT_ENUM(type_9_color_calculation, 6, 0b1);    ///< Color calculation for Type 9 sprites.
    GENERATE_BIT_WITHOUT_ENUM(type_9_dot_color, 0, 0b111111);       ///< Dot color for Type 9 sprites.

    GENERATE_BIT_WITHOUT_ENUM(type_a_priority, 6, 0b11);            ///< Priority for Type A sprites.
    GENERATE_BIT_WITHOUT_ENUM(type_a_dot_color, 0, 0b111111);       ///< Dot color for Type A sprites.

    GENERATE_BIT_WITHOUT_ENUM(type_b_color_calculation, 6, 0b11);   ///< Color calculation for Type B sprites.
    GENERATE_BIT_WITHOUT_ENUM(type_b_dot_color, 0, 0b111111);       ///< Dot color for Type B sprites.

    GENERATE_BIT_WITHOUT_ENUM(type_c_priority, 7, 0b1);             ///< Priority for Type C sprites.
    GENERATE_BIT_WITHOUT_ENUM(type_c_dot_color, 0, 0b11111111);     ///< Dot color for Type C sprites.

    GENERATE_BIT_WITHOUT_ENUM(type_d_priority, 7, 0b1);             ///< Priority for Type D sprites.
    GENERATE_BIT_WITHOUT_ENUM(type_d_color_calculation, 6, 0b1);    ///< Color calculation for Type D sprites.
    GENERATE_BIT_WITHOUT_ENUM(type_d_dot_color, 0, 0b11111111);     ///< Dot color for Type D sprites.

    GENERATE_BIT_WITHOUT_ENUM(type_e_priority, 6, 0b11);            ///< Priority for Type E sprites.
    GENERATE_BIT_WITHOUT_ENUM(type_e_dot_color, 0, 0b11111111);     ///< Dot color for Type E sprites.

    GENERATE_BIT_WITHOUT_ENUM(type_f_color_calculation, 6, 0b11);   ///< Color calculation for Type F sprites.
    GENERATE_BIT_WITHOUT_ENUM(type_f_dot_color, 0, 0b11111111);     ///< Dot color for Type F sprites.

    GENERATE_BIT_WITHOUT_ENUM(msb, 15, 0b1);                        ///< MSB, RGB or palette discriminant.
};
using SpriteTypeRegisterType = Reg<u16, SpriteTypeRegister>;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn	inline auto twosComplement(u16 value) -> s16
///
/// \brief	Calculates the complement of 2 for a VDP1 coordinate.
///
/// \author	Runik
/// \date	22/01/2022
///
/// \param 	value	The value to get the complement of 2 from.
///
/// \returns	A s16.
////////////////////////////////////////////////////////////////////////////////////////////////////

[[nodiscard]] inline auto twosComplement(u16 value) -> s16 {
    constexpr auto coordinate_mask = u16{0x03FF};
    constexpr auto bit_10_set_mask = u16{0x0400};
    constexpr auto sign_extension  = u16{0xf800};
    if (value & bit_10_set_mask) {
        value |= sign_extension;
        return -static_cast<s16>((~value + 1) & coordinate_mask);
    } else {
        return static_cast<s16>(value & coordinate_mask);
    }
};

}; // namespace saturnin::video
