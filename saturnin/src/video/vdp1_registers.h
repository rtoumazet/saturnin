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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	TvModeSelection
///
/// \brief	TV Mode Selection register (TVMR).
///
/// \author	Runik
/// \date	21/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union TvModeSelection {
    u16         raw;                              ///< Raw representation.
    BitField<3> vblank_erase_write_enable;        ///< Defines VBE bit.
    BitField<2> tvm_hdtv_enable;                  ///< Defines TVM bit 2.
    BitField<1> tvm_frame_buffer_rotation_enable; ///< Defines TVM bit 1.
    BitField<0> tvm_bit_depth_selection;          ///< Defines TVM bit 0.
};

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
    one_cycle_mode                  = 0b00, ///< 1-cycle mode, change every 1/60 second (when VBE = 0).
    manual_mode_erase               = 0b10, ///< Manual mode (erase), erase in next field (when VBE = 0).
    manual_mode_change_and_or_erase = 0b11  ///< Manual mode (change), change in next field (when VBE = 0) Manual mode (erase and
                                            ///< change), erase by VBlank and change in next field (when VBE = 1).
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	FrameBufferSwitchMode
///
/// \brief	Frame Buffer Switch Mode register (FBCR).
///
/// \author	Runik
/// \date	21/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union FrameBufferSwitchMode {
    u16            raw;                           ///< Raw representation.
    BitField<4>    even_odd_coordinate_select;    ///< Defines EOS bit.
    BitField<3>    double_interlace_enable;       ///< Defines DIE bit.
    BitField<2>    double_interlace_draw_line;    ///< Defines DIL bit.
    BitField<2, 2> double_interlace_both_bits;    ///< Defines DIE + DIL bits.
    BitField<1>    frame_buffer_change_mode;      ///< Defines FCM bits.
    BitField<0, 2> frame_buffer_change_both_bits; ///< Defines FCM + FCT bits.
};

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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  PlotTrigger
///
/// \brief  Plot Trigger register (PTMR).
///
/// \author Runik
/// \date   13/06/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

union PlotTrigger {
    u16            raw;                     ///< Raw representation.
    BitField<1>    plot_trigger_mode_bit_1; ///< Defines PTM1 bit.
    BitField<0, 2> plot_trigger_mode;       ///< Defines PTM bits.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  EraseWriteData
///
/// \brief  Erase/Write Data register (EWDR).
///
/// \author Runik
/// \date   13/06/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class EraseWriteData : public Register {
  public:
    using Register::Register;
    inline static const BitRange<u16> erase_write_data{0, 15};                       ///< Erase/Write data (16 bits/pixel).
    inline static const BitRange<u8> erase_write_data_for_even_x_coordinates{8, 15}; ///< Erase/Write data for even X coordinates.
    inline static const BitRange<u8> erase_write_data_for_odd_x_coordinates{0, 7};   ///< Erase/Write data for odd X coordinates.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  EraseWriteUpperLeftCoordinate
///
/// \brief  Erase/Write Upper-Left Coordinate register (EWLR).
///
/// \author Runik
/// \date
////////////////////////////////////////////////////////////////////////////////////////////////////

union EraseWriteUpperLeftCoordinate {
    u16            raw;                      ///< Raw representation.
    BitField<9, 6> upper_left_coordinate_x1; ///< Erase/Write Upper Left Coordinate X1.
    BitField<0, 9> upper_left_coordinate_y1; ///< Erase/Write Upper Left Coordinate Y1.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	EraseWriteLowerRightCoordinate
///
/// \brief	Erase/Write Lower-Right Coordinate register (EWRR).
///
/// \author	Runik
/// \date	21/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union EraseWriteLowerRightCoordinate {
    u16            raw;                       ///< Raw representation.
    BitField<9, 7> lower_right_coordinate_x3; ///< Erase/Write Upper Left Coordinate X3.
    BitField<0, 9> lower_right_coordinate_y3; ///< Erase/Write Upper Left Coordinate Y3.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	DrawForcedTermination
///
/// \brief	Draw Forced Termination register (ENDR).
///
/// \author	Runik
/// \date	21/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union DrawForcedTermination {
    u16 raw; ///< Raw representation.
};

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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  TransferEndStatus
///
/// \brief  Transfer End Status register (EDSR).
///
/// \author Runik
/// \date   13/06/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

union TransferEndStatus {
    u16         raw;                          ///< Raw representation.
    BitField<1> current_end_bit_fetch_status; ///< Defines CEF bit.
    BitField<0> before_end_bit_fetch_status;  ///< Defines BEF bit.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	LastOperationCommandAddress
///
/// \brief	Last Operation Command Address register (LOPR).
///
/// \author	Runik
/// \date	21/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union LastOperationCommandAddress {
    u16 raw; ///< Raw representation.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	CurrentOperationCommandAddress
///
/// \brief	Current Operation Command Address register (COPR).
///
/// \author	Runik
/// \date	21/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union CurrentOperationCommandAddress {
    u16 raw; ///< Raw representation.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  ModeStatus
///
/// \brief  Mode Status register (MODR).
///
/// \author Runik
/// \date   13/06/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

union ModeStatus {
    u16             raw;                              ///< Raw representation.
    BitField<12, 4> version_number;                   ///< Version number (VER) bits.
    BitField<8>     plot_trigger_mode_bit_1;          ///< Defines PTM1 bit.
    BitField<7>     even_odd_coordinate_select;       ///< Defines EOS bit.
    BitField<6>     double_interlace_enable;          ///< Defines DIE bit.
    BitField<5>     double_interlace_draw_line;       ///< Defines DIL bit.
    BitField<4>     frame_buffer_change_mode;         ///< Defines FCM bit.
    BitField<3>     vblank_erase_write_enable;        ///< Defines VBE bit.
    BitField<2>     tvm_hdtv_enable;                  ///< Defines TVM bit 2.
    BitField<1>     tvm_frame_buffer_rotation_enable; ///< Defines TVM bit 1.
    BitField<0>     tvm_bit_depth_selection;          ///< Defines TVM bit 0.
};

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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	CmdCtrl
///
/// \brief	Control Words.
///
/// \author	Runik
/// \date	21/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union CmdCtrl {
    u16             raw;                      ///< Raw representation.
    BitField<15>    end_bit;                  ///< END bit.
    BitField<12, 3> jump_select;              ///< JS bits.
    BitField<8, 4>  zoom_point;               ///< ZP bits.
    BitField<4, 2>  character_read_direction; ///< Dir bits.
    BitField<0, 4>  command_select;           ///< Comm bits.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	CmdLink
///
/// \brief	Link specification.
///
/// \author	Runik
/// \date	21/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union CmdLink {
    u16 raw; ///< Raw representation.
};

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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	CmdPmod
///
/// \brief	Draw Mode Word.
///
/// \author	Runik
/// \date	21/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union CmdPmod {
    u16            raw;                       ///< Raw representation.
    BitField<15>   msb_on;                    ///< MON bit.
    BitField<12>   high_speed_shrink;         ///< HSS bit.
    BitField<11>   pre_clipping_disable;      ///< PCLP bit.
    BitField<10>   user_clipping_enable;      ///< CLIP bit.
    BitField<9>    user_clipping_mode;        ///< CMOD bit.
    BitField<8>    mesh_enable;               ///< MESH bit.
    BitField<7>    end_code_disable;          ///< ECD bit.
    BitField<6>    transparent_pixel_disable; ///< SPD bit.
    BitField<3, 3> color_mode;                ///< Color mode bits.
    BitField<0, 3> color_calculation;         ///< Color calculation bits.
    BitField<2>    gouraud_shading;           ///< Gouraud shading bit.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	CmdColr
///
/// \brief	Color Control Word.
///
/// \author	Runik
/// \date	21/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union CmdColr {
    u16 raw; ///< Raw representation.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	CmdSrca
///
/// \brief	Character Address.
///
/// \author	Runik
/// \date	21/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union CmdSrca {
    u16 raw; ///< Raw representation.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	CmdSize
///
/// \brief	Character Size.
///
/// \author	Runik
/// \date	21/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union CmdSize {
    u16            raw;              ///< Raw representation.
    BitField<8, 6> character_size_x; ///< Character Size X divided by 8.
    BitField<0, 8> character_size_y; ///< Character Size Y.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	CmdVertexCoordinate
///
/// \brief	Vertex Coordinate data.
///
/// \author	Runik
/// \date	22/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union CmdVertexCoordinate {
    u16 raw; ///< Raw representation.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	CmdGrda
///
/// \brief	Gouraud Shading Table.
///
/// \author	Runik
/// \date	22/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union CmdGrda {
    u16 raw; ///< Raw representation.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	SpriteTypeRegister
///
/// \brief	Sprite type register.
///
/// \author	Runik
/// \date	22/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union SpriteTypeRegister {
    u16 raw; ///< Raw representation.

    BitField<14, 2> type_0_priority_palette;  ///< Priority for Type 0 sprites on palette only fb.
    BitField<14>    type_0_priority_mixed;    ///< Priority for Type 0 sprites on mixed fb.
    BitField<11, 2> type_0_color_calculation; ///< Color calculation for Type 0 sprites.
    BitField<0, 11> type_0_dot_color;         ///< Dot color for Type 0 sprites.

    BitField<13, 3> type_1_priority_palette;  ///< Priority for Type 1 sprites on palette only fb.
    BitField<13, 2> type_1_priority_mixed;    ///< Priority for Type 1 sprites on mixed fb.
    BitField<11, 2> type_1_color_calculation; ///< Color calculation for Type 1 sprites.
    BitField<0, 11> type_1_dot_color;         ///< Dot color for Type 1 sprites.

    BitField<14>    type_2_priority;          ///< Priority for Type 2 sprites.
    BitField<15>    type_2_shadow_palette;    ///< Shadow for Type 2 sprites on palette only fb.
    BitField<11, 3> type_2_color_calculation; ///< Color calculation for Type 2 sprites.
    BitField<0, 11> type_2_dot_color;         ///< Dot color for Type 2 sprites.

    BitField<15>    type_3_shadow_palette;    ///< Shadow for Type 3 sprites on palette only fb.
    BitField<13, 2> type_3_priority;          ///< Priority for Type 3 sprites.
    BitField<11, 2> type_3_color_calculation; ///< Color calculation for Type 3 sprites.
    BitField<0, 11> type_3_dot_color;         ///< Dot color for Type 3 sprites.

    BitField<15>    type_4_shadow_palette;    ///< Shadow for Type 4 sprites on palette only fb.
    BitField<13, 2> type_4_priority;          ///< Priority for Type 4 sprites.
    BitField<10, 3> type_4_color_calculation; ///< Color calculation for Type 4 sprites.
    BitField<0, 10> type_4_dot_color;         ///< Dot color for Type 4 sprites.

    BitField<15>    type_5_shadow_palette;    ///< Shadow for Type 5 sprites on palette only fb.
    BitField<12, 3> type_5_priority;          ///< Priority for Type 5 sprites.
    BitField<11>    type_5_color_calculation; ///< Color calculation for Type 5 sprites.
    BitField<0, 11> type_5_dot_color;         ///< Dot color for Type 5 sprites.

    BitField<15>    type_6_shadow_palette;    ///< Shadow for Type 6 sprites on palette only fb.
    BitField<12, 3> type_6_priority;          ///< Priority for Type 6 sprites.
    BitField<10, 2> type_6_color_calculation; ///< Color calculation for Type 6 sprites.
    BitField<0, 10> type_6_dot_color;         ///< Dot color for Type 6 sprites.

    BitField<15>    type_7_shadow_palette;    ///< Shadow for Type 7 sprites on palette only fb.
    BitField<12, 3> type_7_priority;          ///< Priority for Type 7 sprites.
    BitField<9, 3>  type_7_color_calculation; ///< Color calculation for Type 7 sprites.
    BitField<0, 9>  type_7_dot_color;         ///< Dot color for Type 7 sprites.

    BitField<7>    type_8_priority;  ///< Priority for Type 8 sprites.
    BitField<0, 7> type_8_dot_color; ///< Dot color for Type 8 sprites.

    BitField<7>    type_9_priority;          ///< Priority for Type 9 sprites.
    BitField<6>    type_9_color_calculation; ///< Color calculation for Type 9 sprites.
    BitField<0, 6> type_9_dot_color;         ///< Dot color for Type 9 sprites.

    BitField<6, 2> type_a_priority;  ///< Priority for Type A sprites.
    BitField<0, 6> type_a_dot_color; ///< Dot color for Type A sprites.

    BitField<6, 2> type_b_color_calculation; ///< Color calculation for Type B sprites.
    BitField<0, 6> type_b_dot_color;         ///< Dot color for Type B sprites.

    BitField<7>    type_c_priority;  ///< Priority for Type C sprites.
    BitField<0, 8> type_c_dot_color; ///< Dot color for Type C sprites.

    BitField<7>    type_d_priority;          ///< Priority for Type D sprites.
    BitField<6>    type_d_color_calculation; ///< Color calculation for Type D sprites.
    BitField<0, 8> type_d_dot_color;         ///< Dot color for Type D sprites.

    BitField<6, 2> type_e_priority;  ///< Priority for Type E sprites.
    BitField<0, 8> type_e_dot_color; ///< Dot color for Type E sprites.

    BitField<6, 2> type_f_color_calculation; ///< Color calculation for Type F sprites.
    BitField<0, 8> type_f_dot_color;         ///< Dot color for Type F sprites.

    BitField<15> msb; ///< MSB, RGB or palette discriminant.
};

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
