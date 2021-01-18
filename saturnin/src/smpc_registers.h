//
// smpc_registers.h
// Saturnin
//
// Copyright (c) 2019 Renaud Toumazet
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
/// \file	smpc_registers.h
///
/// \brief	Declares SMPC onchip registers and related enums.
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <saturnin/src/emulator_defs.h>

namespace saturnin::core {

/// \name SH2 interface registers addresses
//@{
constexpr auto command_register          = u32{0x2010001F};
constexpr auto status_register           = u32{0x20100061};
constexpr auto status_flag               = u32{0x20100063};
constexpr auto input_register_0          = u32{0x20100001};
constexpr auto input_register_1          = u32{0x20100003};
constexpr auto input_register_2          = u32{0x20100005};
constexpr auto input_register_3          = u32{0x20100007};
constexpr auto input_register_4          = u32{0x20100009};
constexpr auto input_register_5          = u32{0x2010000B};
constexpr auto input_register_6          = u32{0x2010000D};
constexpr auto output_register_0         = u32{0x20100021};
constexpr auto output_register_1         = u32{0x20100023};
constexpr auto output_register_2         = u32{0x20100025};
constexpr auto output_register_3         = u32{0x20100027};
constexpr auto output_register_4         = u32{0x20100029};
constexpr auto output_register_5         = u32{0x2010002B};
constexpr auto output_register_6         = u32{0x2010002D};
constexpr auto output_register_7         = u32{0x2010002F};
constexpr auto output_register_8         = u32{0x20100031};
constexpr auto output_register_9         = u32{0x20100033};
constexpr auto output_register_10        = u32{0x20100035};
constexpr auto output_register_11        = u32{0x20100037};
constexpr auto output_register_12        = u32{0x20100039};
constexpr auto output_register_13        = u32{0x2010003B};
constexpr auto output_register_14        = u32{0x2010003D};
constexpr auto output_register_15        = u32{0x2010003F};
constexpr auto output_register_16        = u32{0x20100041};
constexpr auto output_register_17        = u32{0x20100043};
constexpr auto output_register_18        = u32{0x20100045};
constexpr auto output_register_19        = u32{0x20100047};
constexpr auto output_register_20        = u32{0x20100049};
constexpr auto output_register_21        = u32{0x2010004B};
constexpr auto output_register_22        = u32{0x2010004D};
constexpr auto output_register_23        = u32{0x2010004F};
constexpr auto output_register_24        = u32{0x20100051};
constexpr auto output_register_25        = u32{0x20100053};
constexpr auto output_register_26        = u32{0x20100055};
constexpr auto output_register_27        = u32{0x20100057};
constexpr auto output_register_28        = u32{0x20100059};
constexpr auto output_register_29        = u32{0x2010005B};
constexpr auto output_register_30        = u32{0x2010005D};
constexpr auto output_register_31        = u32{0x2010005F};
constexpr auto data_direction_register_1 = u32{0x20100079};
constexpr auto data_direction_register_2 = u32{0x2010007B};
constexpr auto port_data_register_1      = u32{0x20100075};
constexpr auto port_data_register_2      = u32{0x20100077};
constexpr auto io_select_register        = u32{0x2010007D};
constexpr auto external_latch_register   = u32{0x2010007F};
//@}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   SmpcCommand
///
/// \brief  SMPC commands definition.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class SmpcCommand : u8 {
    // Resetable commands
    master_sh2_on       = 0x00, ///< MSHON command
    slave_sh2_on        = 0x02, ///< SSHON command
    slave_sh2_off       = 0x03, ///< SSHOFF command
    sound_on            = 0x06, ///< SNDON command
    sound_off           = 0x07, ///< SNDOFF command
    cd_on               = 0x08, ///< CDDN command
    cd_off              = 0x09, ///< CDOFF command
    reset_entire_system = 0x0d, ///< SYSRES command
    clock_change_352    = 0x0e, ///< CKCHG352 command
    clock_change_320    = 0x0f, ///< CKCHG320 command
    nmi_request         = 0x18, ///< NMIREQ command
    reset_enable        = 0x19, ///< RESENAB command
    reset_disable       = 0x1a, ///< RESDISA command

    // Non resetable commands
    interrupt_back      = 0x10, ///< INTBACK command
    smpc_memory_setting = 0x17, ///< SETSMEM command

    // RTC command
    time_setting = 0x16 ///< SETTIME command
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  CommandRegister
///
/// \brief  Command Register (COMREG).
///
/// \author Runik
/// \date   14/12/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class CommandRegister : public Register {
  public:
    using Register::Register;
    inline static const auto smpc_command = BitRange<SmpcCommand>{0, 7}; ///< Defines the smpc command.
    inline static const auto all_bits     = BitRange<u8>{0, 7};          ///< Defines the range of all the bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   PortMode
///
/// \brief  Port mode values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class PortMode : u8 {
    mode_15_byte  = 0b00, ///< 15-byte mode.
    mode_255_byte = 0b01, ///< 255-byte mode.
    mode_reserved = 0b10, ///< Sega reserved.
    mode_0_byte   = 0b11  ///< 0-byte mode.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   ResetButtonStatus
///
/// \brief  Reset button status values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class ResetButtonStatus : u8 {
    button_off = 0, ///< Reset button off.
    button_on  = 1  ///< Reset buton on.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   PeripheralDataRemaining
///
/// \brief  Peripheral data remaining values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class PeripheralDataRemaining : u8 { no_remaining_peripheral_data = 0, remaining_peripheral_data = 1 };

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   PeripheralDataLocation
///
/// \brief  SR - PDL bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class PeripheralDataLocation : u8 { second_or_above_peripheral_data = 0, first_peripheral_data = 1 };

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  StatusRegister
///
/// \brief  Status Register (SR).
///
/// \author Runik
/// \date   14/12/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class StatusRegister : public Register {
  public:
    using Register::Register;
    inline static const auto port_1_mode               = BitRange<PortMode>{0, 1};             ///< Defines port 1 mode (P1MD).
    inline static const auto port_2_mode               = BitRange<PortMode>{2, 3};             ///< Defines port 2 mode (P2MD).
    inline static const auto reset_button_status       = BitRange<ResetButtonStatus>{4};       ///< Defines RESB bit.
    inline static const auto peripheral_data_remaining = BitRange<PeripheralDataRemaining>{5}; ///< Defines PDE bit.
    inline static const auto peripheral_data_location  = BitRange<PeripheralDataLocation>{6};  ///< Defines PDL bit.
    inline static const auto all_bits = BitRange<u8>{0, 7}; ///< Defines the range of all the bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  StatusFlag
///
/// \brief  Status Flag (SF).
///
/// \author Runik
/// \date   14/12/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class StatusFlag : public Register {
  public:
    using Register::Register;
    inline static const BitRange<u8> sf{0}; ///< SF value.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   IntbackContinueRequest
///
/// \brief  Intback Continue Request values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class IntbackContinueRequest : u8 { not_requested = 0, requested = 1 };

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   IntbackBreakRequest
///
/// \brief  Intback Break Request values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class IntbackBreakRequest : u8 { not_requested = 0, requested = 1 };

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   SmpcStatusAcquisition
///
/// \brief  Status acquisition values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class SmpcStatusAcquisition : u8 { status_not_returned = 0x00, status_returned = 0x01 };

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   AcquisitionTimeOptimization
///
/// \brief  Acquisition time optimizations values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class AcquisitionTimeOptimization : u8 { optimized = 0, not_optimized = 1 };

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   PeripheralDataEnable
///
/// \brief  Peripheral data enable values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class PeripheralDataEnable : u8 { peripheral_data_not_returned = 0, peripheral_data_returned = 1 };

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  InputRegister
///
/// \brief  Input Register (IREGx).
///
/// \author Runik
/// \date   14/12/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class InputRegister : public Register {
  public:
    using Register::Register;
    inline static const auto ireg0_continue_request
        = BitRange<IntbackContinueRequest>{7}; ///< Defines Intback continue request (IREG0).
    inline static const auto ireg0_break_request = BitRange<IntbackBreakRequest>{6}; ///< Defines Intback break request (IREG0).
    inline static const auto ireg0_status_acquisition
        = BitRange<SmpcStatusAcquisition>{0, 7}; ///< Defines SMPC status acquisition (IREG0).
    inline static const auto ireg1_acquisition_time_optimization
        = BitRange<AcquisitionTimeOptimization>{1}; ///< Defines if peripheral acquisition time is optimized (IREG1).
    inline static const auto ireg1_peripheral_data_enable
        = BitRange<PeripheralDataEnable>{3};                               ///< Defines if peripheral data is enabled (IREG1).
    inline static const auto ireg1_port_1_mode = BitRange<PortMode>{4, 5}; ///< Defines port 1 mode (IREG1).
    inline static const auto ireg1_port_2_mode = BitRange<PortMode>{6, 7}; ///< Defines port 2 mode (IREG1).
    inline static const auto all_bits          = BitRange<u8>{0, 7};       ///< Defines the range of all the bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   ResetStatus
///
/// \brief  Defines OREG0 - RESD bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class ResetStatus : u8 {
    enabled  = 0, ///< Reset enabled.
    disabled = 1  ///< Reset disabled.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   SetTime
///
/// \brief  Defines OREG0 - STE bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class SetTime : u8 {
    not_set_time = 0, ///< Not SETTIME after SMPC cold reset.
    set_time     = 1  ///< SETTIME is done after SMPC cold reset.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  OutputRegister
///
/// \brief  Output Register (OREGx).
///
/// \author Runik
/// \date   14/12/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class OutputRegister : public Register {
  public:
    using Register::Register;
    inline static const auto oreg0_reset_status = BitRange<ResetStatus>{6}; ///< Defines RESD bit (OREG0).
    inline static const auto oreg0_set_time     = BitRange<SetTime>{7};     ///< Defines STE bit (OREG0).
    inline static const auto all_bits           = BitRange<u8>{0, 7}; ///< Defines the range of all the bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   ResetButtonStatus
///
/// \brief  Reset button status values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class ButtonStatus : u8 {
    pressed  = 0, ///< Button is pressed.
    released = 1  ///< Button is released.
};

class SaturnStandardPad1stData : public OutputRegister {
  public:
    using OutputRegister::OutputRegister;
    inline static const BitRange<ButtonStatus> direction_right{7};
    inline static const BitRange<ButtonStatus> direction_left{6};
    inline static const BitRange<ButtonStatus> direction_down{5};
    inline static const BitRange<ButtonStatus> direction_up{4};
    inline static const BitRange<ButtonStatus> button_start{3};
    inline static const BitRange<ButtonStatus> button_a{2};
    inline static const BitRange<ButtonStatus> button_c{1};
    inline static const BitRange<ButtonStatus> button_b{2};
};

class SaturnStandardPad2ndData : public OutputRegister {
  public:
    using OutputRegister::OutputRegister;
    inline static const BitRange<ButtonStatus> button_shoulder_right{7};
    inline static const BitRange<ButtonStatus> button_x{6};
    inline static const BitRange<ButtonStatus> button_y{5};
    inline static const BitRange<ButtonStatus> button_z{4};
    inline static const BitRange<ButtonStatus> button_shoulder_left{3};
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  DataDirectionRegister
///
/// \brief  Data Direction Register (DDRx).
///
/// \author Runik
/// \date   14/12/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class DataDirectionRegister : public Register {
  public:
    using Register::Register;
    inline static const BitRange<u8> all_bits{0, 6};         ///< Defines the range of all the bits of the register.
    static constexpr u8              access_mask{0b1111111}; ///< The access mask
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  PortDataRegister
///
/// \brief  Port Data Register (PDRx).
///
/// \author Runik
/// \date   14/12/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class PortDataRegister : public Register {
  public:
    using Register::Register;
    inline static const BitRange<u8> all_bits{0, 6};         ///< Defines the range of all the bits of the register.
    static constexpr u8              access_mask{0b1111111}; ///< The access mask
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   PeripheralPortMode
///
/// \brief  IOSEL values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class PeripheralPortMode : u8 {
    smpc_control_mode = 0, ///< SMPC control mode. (initial)
    sh2_direct_mode   = 1, ///< SH2 direct mode
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  IOSelect
///
/// \brief  I/O Select (IOSEL).
///
/// \author Runik
/// \date   14/12/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class IOSelect : public Register {
  public:
    using Register::Register;
    inline static const auto peripheral_port_2_mode = BitRange<PeripheralPortMode>{1}; ///< IOSEL2 bit.
    inline static const auto peripheral_port_1_mode = BitRange<PeripheralPortMode>{0}; ///< IOSEL1 bit.
    inline static const auto all_bits               = BitRange<u8>{0, 1}; ///< Defines the range of all the bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   ExternalLatch
///
/// \brief  EXLE values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class ExternalLatch : u8 {
    disabled = 0, ///< Disabled. (initial)
    enabled  = 1, ///< Enabled
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  ExternalLatchEnable
///
/// \brief  External Latch Enable (EXLE).
///
/// \author Runik
/// \date   14/12/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class ExternalLatchEnable : public Register {
  public:
    using Register::Register;
    inline static const auto external_latch_2_enable = BitRange<ExternalLatch>{1}; ///< EXLE2 bit.
    inline static const auto external_latch_1_enable = BitRange<ExternalLatch>{0}; ///< EXLE1 bit.
    inline static const auto all_bits                = BitRange<u8>{0, 1}; ///< Defines the range of all the bits of the register.
};

} // namespace saturnin::core
