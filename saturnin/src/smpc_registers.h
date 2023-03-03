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
#include <saturnin/src/bitfield.h>

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
/// \union	CommandRegister
///
/// \brief	Command Register (COMREG).
///
/// \author	Runik
/// \date	20/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union CommandRegister {
    u8 raw; ///< Raw representation.
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

enum class ResetButtonStatus : bool {
    button_off = false, ///< Reset button off.
    button_on  = true   ///< Reset buton on.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   PeripheralDataRemaining
///
/// \brief  Peripheral data remaining values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class PeripheralDataRemaining : bool { no_remaining_peripheral_data = false, remaining_peripheral_data = true };

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   PeripheralDataLocation
///
/// \brief  SR - PDL bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class PeripheralDataLocation : bool { second_or_above_peripheral_data = false, first_peripheral_data = true };

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	StatusRegister
///
/// \brief	Status Register (SR).
///
/// \author	Runik
/// \date	20/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union StatusRegister {
    u8             raw;                       ///< Raw representation.
    BitField<0, 2> port_1_mode;               ///< Defines port 1 mode (P1MD).
    BitField<2, 2> port_2_mode;               ///< Defines port 2 mode (P2MD).
    BitField<4>    reset_button_status;       ///< Defines RESB bit.
    BitField<5>    peripheral_data_remaining; ///< Defines PDE bit.
    BitField<6>    peripheral_data_location;  ///< Defines PDL bit.
    BitField<6>    bit_6;                     ///< Bit 6 access.
    BitField<7>    bit_7;                     ///< Bit 7 access.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	StatusFlag
///
/// \brief	Status Flag (SF).
///
/// \author	Runik
/// \date	20/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union StatusFlag {
    u8          raw; ///< Raw representation.
    BitField<0> sf;  ///< SF value.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   IntbackContinueRequest
///
/// \brief  Intback Continue Request values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class IntbackContinueRequest : bool { not_requested = false, requested = true };

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   IntbackBreakRequest
///
/// \brief  Intback Break Request values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class IntbackBreakRequest : bool { not_requested = false, requested = true };

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   SmpcStatusAcquisition
///
/// \brief  Status acquisition values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class SmpcStatusAcquisition : bool { status_not_returned = false, status_returned = true };

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   AcquisitionTimeOptimization
///
/// \brief  Acquisition time optimizations values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class AcquisitionTimeOptimization : bool { optimized = false, not_optimized = true };

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   PeripheralDataEnable
///
/// \brief  Peripheral data enable values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class PeripheralDataEnable : bool { peripheral_data_not_returned = false, peripheral_data_returned = true };

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	InputRegister
///
/// \brief	Input Register (IREGx).
///
/// \author	Runik
/// \date	20/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union InputRegister {
    u8             raw;                                 ///< Raw representation.
    BitField<7>    ireg0_continue_request;              ///< Defines Intback continue request (IREG0).
    BitField<6>    ireg0_break_request;                 ///< Defines Intback break request (IREG0).
    BitField<0>    ireg0_status_acquisition;            ///< Defines SMPC status acquisition (IREG0).
    BitField<6, 2> ireg1_port_2_mode;                   ///< Defines port 2 mode (IREG1).
    BitField<4, 2> ireg1_port_1_mode;                   ///< Defines port 1 mode (IREG1).
    BitField<3>    ireg1_peripheral_data_enable;        ///< Defines if peripheral data is enabled (IREG1).
    BitField<1>    ireg1_acquisition_time_optimization; ///< Defines if peripheral acquisition time is optimized (IREG1).
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   ResetStatus
///
/// \brief  Defines OREG0 - RESD bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class ResetStatus : bool {
    enabled  = false, ///< Reset enabled.
    disabled = true   ///< Reset disabled.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   SetTime
///
/// \brief  Defines OREG0 - STE bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class SetTime : bool {
    not_set_time = false, ///< Not SETTIME after SMPC cold reset.
    set_time     = true   ///< SETTIME is done after SMPC cold reset.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  OutputRegister
///
/// \brief  Output Register (OREGx).
///
/// \author Runik
/// \date   14/12/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

union OutputRegister {
    u8          raw;                ///< Raw representation.
    BitField<7> oreg0_set_time;     ///< Defines STE bit (OREG0).
    BitField<6> oreg0_reset_status; ///< Defines RESD bit (OREG0).
    BitField<7> bit_7;              ///< Bit 7 access.
    BitField<6> bit_6;              ///< Bit 6 access.
    BitField<5> bit_5;              ///< Bit 5 access.
    BitField<4> bit_4;              ///< Bit 4 access.
    BitField<3> bit_3;              ///< Bit 3 access.
    BitField<2> bit_2;              ///< Bit 2 access.
    BitField<1> bit_1;              ///< Bit 1 access.
    BitField<0> bit_0;              ///< Bit 0 access.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   ResetButtonStatus
///
/// \brief  Reset button status values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class ButtonStatus : bool {
    pressed  = false, ///< Button is pressed.
    released = true   ///< Button is released.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	SaturnStandardPad1stData
///
/// \brief	First part of the standard Saturn PAD data.
///
/// \author	Runik
/// \date	20/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union SaturnStandardPad1stData {
    u8          raw;             ///< Raw representation.
    BitField<7> direction_right; ///< Right direction.
    BitField<6> direction_left;  ///< Left direction.
    BitField<5> direction_down;  ///< Down direction.
    BitField<4> direction_up;    ///< Up direction.
    BitField<3> button_start;    ///< Start button.
    BitField<2> button_a;        ///< A button.
    BitField<1> button_c;        ///< C button.
    BitField<0> button_b;        ///< B button.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	SaturnStandardPad2ndData
///
/// \brief	Second part of the standard Saturn PAD data.
///
/// \author	Runik
/// \date	20/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union SaturnStandardPad2ndData {
    u8          raw;                   ///< Raw representation.
    BitField<7> button_shoulder_right; ///< Right shoulder button.
    BitField<6> button_x;              ///< X button.
    BitField<5> button_y;              ///< Y button.
    BitField<4> button_z;              ///< Z button.
    BitField<3> button_shoulder_left;  ///< Left shoulder button.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	DataDirectionRegister
///
/// \brief	Data Direction Register (DDRx).
///
/// \author	Runik
/// \date	21/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union DataDirectionRegister {
    u8             raw; ///< Raw representation.
    BitField<0, 6> ddr; ///< Used bits in the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  PortDataRegister
///
/// \brief  Port Data Register (PDRx).
///
/// \author Runik
/// \date   14/12/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

union PortDataRegister {
    u8             raw; ///< Raw representation.
    BitField<0, 6> pdr; ///< Used bits in the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   PeripheralPortMode
///
/// \brief  IOSEL values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class PeripheralPortMode : bool {
    smpc_control_mode = false, ///< SMPC control mode. (initial)
    sh2_direct_mode   = true,  ///< SH2 direct mode
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	IOSelect
///
/// \brief	I/O Select (IOSEL).
///
/// \author	Runik
/// \date	21/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union IOSelect {
    u8          raw;                    ///< Raw representation.
    BitField<1> peripheral_port_2_mode; ///< IOSEL2 bit.
    BitField<0> peripheral_port_1_mode; ///< IOSEL1 bit.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   ExternalLatch
///
/// \brief  EXLE values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class ExternalLatch : bool {
    disabled = false, ///< Disabled. (initial)
    enabled  = true,  ///< Enabled
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  ExternalLatchEnable
///
/// \brief  External Latch Enable (EXLE).
///
/// \author Runik
/// \date   14/12/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

union ExternalLatchEnable {
    u8             raw;                     ///< Raw representation.
    BitField<1>    external_latch_2_enable; ///< EXLE2 bit.
    BitField<0>    external_latch_1_enable; ///< EXLE1 bit.
    BitField<0, 2> exle;                    ///< Used bits in the register.
};

} // namespace saturnin::core
