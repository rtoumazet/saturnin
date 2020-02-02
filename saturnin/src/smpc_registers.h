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

#include "emulator_defs.h"

namespace saturnin {
namespace core {

/// \name SH2 interface registers addresses
//@{
constexpr u32 command_register          = 0x2010001F;
constexpr u32 status_register           = 0x20100061;
constexpr u32 status_flag               = 0x20100063;
constexpr u32 input_register_0          = 0x20100001;
constexpr u32 input_register_1          = 0x20100003;
constexpr u32 input_register_2          = 0x20100005;
constexpr u32 input_register_3          = 0x20100007;
constexpr u32 input_register_4          = 0x20100009;
constexpr u32 input_register_5          = 0x2010000B;
constexpr u32 input_register_6          = 0x2010000D;
constexpr u32 output_register_0         = 0x20100021;
constexpr u32 output_register_1         = 0x20100023;
constexpr u32 output_register_2         = 0x20100025;
constexpr u32 output_register_3         = 0x20100027;
constexpr u32 output_register_4         = 0x20100029;
constexpr u32 output_register_5         = 0x2010002B;
constexpr u32 output_register_6         = 0x2010002D;
constexpr u32 output_register_7         = 0x2010002F;
constexpr u32 output_register_8         = 0x20100031;
constexpr u32 output_register_9         = 0x20100033;
constexpr u32 output_register_10        = 0x20100035;
constexpr u32 output_register_11        = 0x20100037;
constexpr u32 output_register_12        = 0x20100039;
constexpr u32 output_register_13        = 0x2010003B;
constexpr u32 output_register_14        = 0x2010003D;
constexpr u32 output_register_15        = 0x2010003F;
constexpr u32 output_register_16        = 0x20100041;
constexpr u32 output_register_17        = 0x20100043;
constexpr u32 output_register_18        = 0x20100045;
constexpr u32 output_register_19        = 0x20100047;
constexpr u32 output_register_20        = 0x20100049;
constexpr u32 output_register_21        = 0x2010004B;
constexpr u32 output_register_22        = 0x2010004D;
constexpr u32 output_register_23        = 0x2010004F;
constexpr u32 output_register_24        = 0x20100051;
constexpr u32 output_register_25        = 0x20100053;
constexpr u32 output_register_26        = 0x20100055;
constexpr u32 output_register_27        = 0x20100057;
constexpr u32 output_register_28        = 0x20100059;
constexpr u32 output_register_29        = 0x2010005B;
constexpr u32 output_register_30        = 0x2010005D;
constexpr u32 output_register_31        = 0x2010005F;
constexpr u32 data_direction_register_1 = 0x20100079;
constexpr u32 data_direction_register_2 = 0x2010007B;
constexpr u32 port_data_register_1      = 0x20100075;
constexpr u32 port_data_register_2      = 0x20100077;
constexpr u32 io_select_register        = 0x2010007D;
constexpr u32 external_latch_register   = 0x2010007F;
//@}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   SmpcCommand
///
/// \brief  SMPC commands definition.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class SmpcCommand : u8 {
    // Resetable commands
    master_sh2_on = 0x00,       ///< MSHON command
    slave_sh2_on = 0x02,        ///< SSHON command
    slave_sh2_off = 0x03,       ///< SSHOFF command
    sound_on = 0x06,            ///< SNDON command
    sound_off = 0x07,           ///< SNDOFF command
    cd_on = 0x08,               ///< CDDN command
    cd_off = 0x09,              ///< CDOFF command
    reset_entire_system = 0x0d, ///< SYSRES command
    clock_change_352 = 0x0e,    ///< CKCHG352 command
    clock_change_320 = 0x0f,    ///< CKCHG320 command
    nmi_request = 0x18,         ///< NMIREQ command
    reset_enable = 0x19,        ///< RESENAB command
    reset_disable = 0x1a,       ///< RESDISA command

    // Non resetable commands
    interrupt_back = 0x10,      ///< INTBACK command
    smpc_memory_setting = 0x17, ///< SETSMEM command

    // RTC command
    time_setting = 0x16         ///< SETTIME command
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
    inline static const BitRange<SmpcCommand> smpc_command{ 0, 7 }; ///< Defines the smpc command.
    inline static const BitRange<u8> all_bits{ 0, 7 };   ///< Defines the range of all the bits of the register.
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

enum class PeripheralDataRemaining : u8 {
    no_remaining_peripheral_data = 0,
    remaining_peripheral_data    = 1
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   PeripheralDataLocation
///
/// \brief  SR - PDL bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class PeripheralDataLocation: u8 {
    second_or_above_peripheral_data = 0,
    first_peripheral_data = 1
};

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
    inline static const BitRange<ResetButtonStatus> reset_button_status{ 4 }; ///< Defines RESB bit.
    inline static const BitRange<PeripheralDataRemaining> peripheral_data_remaining{ 5 }; ///< Defines PDE bit.
    inline static const BitRange<PeripheralDataLocation> peripheral_data_location{ 6 }; ///< Defines PDL bit.
    inline static const BitRange<u8> all_bits{ 0, 7 };   ///< Defines the range of all the bits of the register.
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
    inline static const BitRange<u8> sf{ 0 };  ///< SF value.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   IntbackContinueRequest
///
/// \brief  Intback Continue Request values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class IntbackContinueRequest : u8 {
    not_requested = 0,
    requested     = 1
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   IntbackBreakRequest
///
/// \brief  Intback Break Request values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class IntbackBreakRequest : u8 {
    not_requested = 0,
    requested     = 1
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   SmpcStatusAcquisition
///
/// \brief  Status acquisition values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class SmpcStatusAcquisition : u8 {
    status_not_returned = 0b00000000,
    status_returned     = 0b00000001
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   AcquisitionTimeOptimization
///
/// \brief  Acquisition time optimizations values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class AcquisitionTimeOptimization : u8 {
    optimized     = 0,
    not_optimized = 1
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   PeripheralDataEnable
///
/// \brief  Peripheral data enable values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class PeripheralDataEnable : u8 {
    peripheral_data_not_returned = 0,
    peripheral_data_returned     = 1
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
    inline static const BitRange<IntbackContinueRequest> ireg0_continue_request { 7 }; ///< Defines Intback continue request (IREG0).
    inline static const BitRange<IntbackBreakRequest> ireg0_break_request { 6 }; ///< Defines Intback break request (IREG0).
    inline static const BitRange<SmpcStatusAcquisition> ireg0_status_acquisition { 0, 7 }; ///< Defines SMPC status acquisition (IREG0).
    inline static const BitRange<AcquisitionTimeOptimization> ireg1_acquisition_time_optimization{ 1 }; ///< Defines if peripheral acquisition time is optimized (IREG1).
    inline static const BitRange<PeripheralDataEnable> ireg1_peripheral_data_enable{ 3 }; ///< Defines if peripheral data is enabled (IREG1).
    inline static const BitRange<PortMode> ireg1_port_1_mode{ 4, 5 }; ///< Defines port 1 mode (IREG1).
    inline static const BitRange<PortMode> ireg1_port_2_mode{ 6, 7 }; ///< Defines port 2 mode (IREG1).
    inline static const BitRange<u8> all_bits{ 0, 7 };          ///< Defines the range of all the bits of the register.
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
    inline static const BitRange<ResetStatus> oreg0_reset_status { 6 }; ///< Defines RESD bit (OREG0).
    inline static const BitRange<SetTime>     oreg0_set_time{ 7 };      ///< Defines STE bit (OREG0).
    inline static const BitRange<u8>          all_bits{ 0, 7 };         ///< Defines the range of all the bits of the register.
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
    inline static const BitRange<u8> all_bits{ 0, 6 };         ///< Defines the range of all the bits of the register.
    inline static const u32 accessMask() { return 0b1111111; } ///< Returns access mask;
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
    inline static const BitRange<u8> all_bits{ 0, 6 };         ///< Defines the range of all the bits of the register.
    inline static const u32 accessMask() { return 0b1111111; } ///< Returns access mask;
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
    inline static const BitRange<PeripheralPortMode> peripheral_port_2_mode { 1 }; ///< IOSEL2 bit.
    inline static const BitRange<PeripheralPortMode> peripheral_port_1_mode { 0 }; ///< IOSEL1 bit.
    inline static const BitRange<u8> all_bits{ 0, 1 };                             ///< Defines the range of all the bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   ExternalLatch
///
/// \brief  EXLE values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class ExternalLatch : u8 {
    disabled = 0, ///< Disabled. (initial)
    enabled = 1, ///< Enabled 
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
    inline static const BitRange<ExternalLatch> external_latch_2_enable { 1 }; ///< EXLE2 bit.
    inline static const BitRange<ExternalLatch> external_latch_1_enable { 0 }; ///< EXLE1 bit.
    inline static const BitRange<u8> all_bits{ 0, 1 };                         ///< Defines the range of all the bits of the register.
};

}
}
