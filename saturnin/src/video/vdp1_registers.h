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

#include <saturnin/src/pch.h>

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

enum class VBlankEraseWriteEnable : u8 {
    no_erase_write_performed_during_vblank = 0, ///< Erase/Write is not performed during VBlank.
    erase_write_performed_during_vblank    = 1  ///< Erase/Write is performed during VBlank.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   HdtvEnable
///
/// \brief  TVM bit 2 values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class HdtvEnable : u8 {
    ntsc_pal  = 0, ///< NTSC, PAL.
    hdtv_31kc = 1  ///< HDTV, 31KC.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   FrameBufferRotationEnable
///
/// \brief  TVM bit 1 values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class FrameBufferRotationEnable : u8 {
    no_rotation = 0, ///< No rotation.
    rotation    = 1  ///< Rotation.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   BitDepthSelection
///
/// \brief  TVM bit 0 values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class BitDepthSelection : u8 {
    sixteen_bits_per_pixel = 0, ///< 16 bits / pixel.
    eight_bits_per_pixel   = 1  ///< 8 bits / pixel.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  TvModeSelection
///
/// \brief  TV Mode Selection register (TVMR).
///
/// \author Runik
/// \date   10/06/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class TvModeSelection : public Register {
  public:
    using Register::Register;
    inline static const BitRange<VBlankEraseWriteEnable>    vblank_erase_write_enable{3};        ///< Defines VBE bit.
    inline static const BitRange<HdtvEnable>                tvm_hdtv_enable{2};                  ///< Defines TVM bit 2.
    inline static const BitRange<FrameBufferRotationEnable> tvm_frame_buffer_rotation_enable{1}; ///< Defines TVM bit 1.
    inline static const BitRange<BitDepthSelection>         tvm_bit_depth_selection{0};          ///< Defines TVM bit 0.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   EvenOddCoordinateSelect
///
/// \brief  EOS bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class EvenOddCoordinateSelect : u8 {
    samples_pixels_at_even_coordinates = 0, ///< Samples only pixels at even coordinates.
    samples_pixels_at_odd_coordinates  = 1  ///< Samples only pixels at odd coordinates.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   DoubleInterlaceEnable
///
/// \brief  DIE bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class DoubleInterlaceEnable : u8 {
    non_interlace_or_single_interlace = 0, ///< Non-interlace/single interlace.
    double_interlace                  = 1  ///< Double interlace.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   DoubleInterlaceDrawLine
///
/// \brief  DIL bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class DoubleInterlaceDrawLine : u8 {
    not_set = 0, ///< DIL bit not set.
    set     = 1  ///< DIL bit set.
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

enum class FrameBufferChangeModeBit : u8 {
    automatic_mode = 0, ///< Automatic mode change.
    manual_mode    = 1  ///< Manual mode change.
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
/// \class  FrameBufferSwitchMode
///
/// \brief  Frame Buffer Switch Mode register (FBCR).
///
/// \author Runik
/// \date   12/06/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class FrameBufferSwitchMode : public Register {
  public:
    using Register::Register;
    inline static const BitRange<EvenOddCoordinateSelect>   even_odd_coordinate_select{4};       ///< Defines EOS bit.
    inline static const BitRange<DoubleInterlaceEnable>     double_interlace_enable{3};          ///< Defines DIE bit.
    inline static const BitRange<DoubleInterlaceDrawLine>   double_interlace_draw_line{2};       ///< Defines DIL bit.
    inline static const BitRange<DoubleInterlaceDrawLine>   double_interlace_both_bits{2, 3};    ///< Defines DIE + DIL bits.
    inline static const BitRange<FrameBufferChangeModeBit>  frame_buffer_change_mode{1};         ///< Defines FCM bits.
    inline static const BitRange<FrameBufferChangeBothBits> frame_buffer_change_both_bits{0, 1}; ///< Defines FCM + FCT bits.
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

enum class PlotTriggerMode1 : u8 {
    cleared = 0, ///< Bit is cleared.
    set     = 1  ///< Bit is set.

};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  PlotTrigger
///
/// \brief  Plot Trigger register (PTMR).
///
/// \author Runik
/// \date   13/06/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class PlotTrigger : public Register {
  public:
    using Register::Register;
    inline static const BitRange<PlotTriggerMode>  plot_trigger_mode{0, 1};    ///< Defines PTM bits.
    inline static const BitRange<PlotTriggerMode1> plot_trigger_mode_bit_1{1}; ///< Defines PTM1 bit.
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
/// \date   13/06/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class EraseWriteUpperLeftCoordinate : public Register {
  public:
    using Register::Register;
    inline static const BitRange<u8> upper_left_coordinate_x1{9, 14}; ///< Erase/Write Upper Left Coordinate X1.
    inline static const BitRange<u8> upper_left_coordinate_y1{0, 8};  ///< Erase/Write Upper Left Coordinate Y1.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  EraseWriteLowerRightCoordinate
///
/// \brief  Erase/Write Lower-Right Coordinate register (EWRR).
///
/// \author Runik
/// \date   13/06/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class EraseWriteLowerRightCoordinate : public Register {
  public:
    using Register::Register;
    inline static const BitRange<u8> lower_right_coordinate_x3{9, 15}; ///< Erase/Write Lower Right Coordinate X3.
    inline static const BitRange<u8> lower_right_coordinate_y3{0, 8};  ///< Erase/Write Lower Right Coordinate Y3.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  DrawForcedTermination
///
/// \brief  Draw Forced Termination register (ENDR).
///
/// \author Runik
/// \date   13/06/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class DrawForcedTermination : public Register {
  public:
    using Register::Register;
    inline static const BitRange<u16> draw_forced_termination{0, 15}; ///< Draw forced termination: draw end.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   CurrentEndBitFetchStatus
///
/// \brief  CEF bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class CurrentEndBitFetchStatus : u8 {
    end_bit_not_fetched = 0, ///< The end bit in current frame has not been fetched.
    end_bit_fetched     = 1  ///< The end bit in current frame has been fetched and plotting is ended.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   BeforeEndBitFetchStatus
///
/// \brief  BEF bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class BeforeEndBitFetchStatus : u8 {
    end_bit_not_fetched = 0, ///< The end bit in previous frame has not been fetched.
    end_bit_fetched     = 1  ///< The end bit in previous frame has been fetched and drawing is terminated.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  TransferEndStatus
///
/// \brief  Transfer End Status register (EDSR).
///
/// \author Runik
/// \date   13/06/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class TransferEndStatus : public Register {
  public:
    using Register::Register;
    inline static const BitRange<CurrentEndBitFetchStatus> current_end_bit_fetch_status{1}; ///< Defines CEF bit.
    inline static const BitRange<BeforeEndBitFetchStatus>  before_end_bit_fetch_status{0};  ///< Defines BEF bit.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  LastOperationCommandAddress
///
/// \brief  Last Operation Command Address register (LOPR).
///
/// \author Runik
/// \date   13/06/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class LastOperationCommandAddress : public Register {
  public:
    using Register::Register;
    inline static const BitRange<u16> last_operation_command_address{0, 15}; ///< Last operation command address.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  CurrentOperationCommandAddress
///
/// \brief  Current Operation Command Address register (COPR).
///
/// \author Runik
/// \date   13/06/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class CurrentOperationCommandAddress : public Register {
  public:
    using Register::Register;
    inline static const BitRange<u16> current_operation_command_address{0, 15}; ///< Current operation command address.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  ModeStatus
///
/// \brief  Mode Status register (MODR).
///
/// \author Runik
/// \date   13/06/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class ModeStatus : public Register {
  public:
    using Register::Register;
    inline static const BitRange<u8>                        version_number{12, 15};              ///< Version number (VER) bits.
    inline static const BitRange<PlotTriggerMode1>          plot_trigger_mode_bit_1{8};          ///< Defines PTM1 bit.
    inline static const BitRange<EvenOddCoordinateSelect>   even_odd_coordinate_select{7};       ///< Defines EOS bit.
    inline static const BitRange<DoubleInterlaceEnable>     double_interlace_enable{6};          ///< Defines DIE bit.
    inline static const BitRange<DoubleInterlaceDrawLine>   double_interlace_draw_line{5};       ///< Defines DIL bit.
    inline static const BitRange<FrameBufferChangeModeBit>  frame_buffer_change_mode{4};         ///< Defines FCM bit.
    inline static const BitRange<VBlankEraseWriteEnable>    vblank_erase_write_enable{3};        ///< Defines VBE bit.
    inline static const BitRange<HdtvEnable>                tvm_hdtv_enable{2};                  ///< Defines TVM bit 2.
    inline static const BitRange<FrameBufferRotationEnable> tvm_frame_buffer_rotation_enable{1}; ///< Defines TVM bit 1.
    inline static const BitRange<BitDepthSelection>         tvm_bit_depth_selection{0};          ///< Defines TVM bit 0.
};

}; // namespace saturnin::video
