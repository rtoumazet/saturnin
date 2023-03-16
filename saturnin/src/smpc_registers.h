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
#include <saturnin/src/bit_register.h>

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

constexpr u8 input_registers_number{7};
constexpr u8 output_registers_number{32};


struct SmpcRegisters {
    struct CommandRegister {
        using PosType    = Pos<u8, CommandRegister>;
        using MaskedType = Masked<u8, CommandRegister>;

        static constexpr PosType comreg_pos = PosType(0);

        static constexpr u8 comreg_mask = 0x1F;

        // Resetable commands
        static constexpr MaskedType master_sh2_on       = MaskedType(comreg_mask, 0x00, comreg_pos);
        static constexpr MaskedType slave_sh2_on        = MaskedType(comreg_mask, 0x02, comreg_pos);
        static constexpr MaskedType slave_sh2_off       = MaskedType(comreg_mask, 0x03, comreg_pos);
        static constexpr MaskedType sound_on            = MaskedType(comreg_mask, 0x06, comreg_pos);
        static constexpr MaskedType sound_off           = MaskedType(comreg_mask, 0x07, comreg_pos);
        static constexpr MaskedType cd_on               = MaskedType(comreg_mask, 0x08, comreg_pos);
        static constexpr MaskedType cd_off              = MaskedType(comreg_mask, 0x09, comreg_pos);
        static constexpr MaskedType reset_entire_system = MaskedType(comreg_mask, 0x0d, comreg_pos);
        static constexpr MaskedType clock_change_352    = MaskedType(comreg_mask, 0x0e, comreg_pos);
        static constexpr MaskedType clock_change_320    = MaskedType(comreg_mask, 0x0f, comreg_pos);
        static constexpr MaskedType nmi_request         = MaskedType(comreg_mask, 0x18, comreg_pos);
        static constexpr MaskedType reset_enable        = MaskedType(comreg_mask, 0x19, comreg_pos);
        static constexpr MaskedType reset_disable       = MaskedType(comreg_mask, 0x1a, comreg_pos);

        // Non resetable commands
        static constexpr MaskedType interrupt_back      = MaskedType(comreg_mask, 0x10, comreg_pos);
        static constexpr MaskedType smpc_memory_setting = MaskedType(comreg_mask, 0x17, comreg_pos);

        // RTC command
        static constexpr MaskedType time_setting = MaskedType(comreg_mask, 0x16, comreg_pos);
    };
    using CommandRegisterType = Reg<u8, CommandRegister>;
    CommandRegisterType comreg;

    struct StatusRegister {
        using PosType     = Pos<u8, StatusRegister>;
        using BitsType    = Bits<u8, StatusRegister>;
        using MaskedType  = Masked<u8, StatusRegister>;
        using ShiftedType = Shifted<u8, StatusRegister>;

        static constexpr PosType p1md_pos         = PosType(0); // Port 1 mode.
        static constexpr PosType p2md_pos         = PosType(2); // Port 2 mode.
        static constexpr PosType upper_nibble_pos = PosType(4);
        static constexpr PosType resb_pos         = PosType(4); // Reset button status.
        static constexpr PosType pde_pos          = PosType(5); // Peripheral data remaining.
        static constexpr PosType pdl_pos          = PosType(6); // Peripheral data location (1: first, 0: second or above).
        static constexpr PosType bit_6_pos        = PosType(6); // Bit 6
        static constexpr PosType bit_7_pos        = PosType(7); // Bit 7

        static constexpr u8 p1md_mask         = 0x03;
        static constexpr u8 p2md_mask         = 0x03;
        static constexpr u8 upper_nibble_mask = 0x0F;

        static constexpr BitsType resb  = BitsType(1, resb_pos);
        static constexpr BitsType pde   = BitsType(1, pde_pos);
        static constexpr BitsType pdl   = BitsType(1, pdl_pos);
        static constexpr BitsType bit_6 = BitsType(1, bit_6_pos);
        static constexpr BitsType bit_7 = BitsType(1, bit_7_pos);

        static constexpr MaskedType port_1_mode_15_bytes  = MaskedType(p1md_mask, 0b00, p1md_pos);
        static constexpr MaskedType port_1_mode_255_bytes = MaskedType(p1md_mask, 0b01, p1md_pos);
        static constexpr MaskedType port_1_mode_reserved  = MaskedType(p1md_mask, 0b10, p1md_pos);
        static constexpr MaskedType port_1_mode_0_byte    = MaskedType(p1md_mask, 0b11, p1md_pos);

        static constexpr MaskedType port_2_mode_15_bytes  = MaskedType(p2md_mask, 0b00, p1md_pos);
        static constexpr MaskedType port_2_mode_255_bytes = MaskedType(p2md_mask, 0b01, p1md_pos);
        static constexpr MaskedType port_2_mode_reserved  = MaskedType(p2md_mask, 0b10, p1md_pos);
        static constexpr MaskedType port_2_mode_0_byte    = MaskedType(p2md_mask, 0b11, p1md_pos);

        static constexpr MaskedType upper_nibble = MaskedType(upper_nibble_mask, 0xF, upper_nibble_pos);
    };
    using StatusRegisterType = Reg<u8, StatusRegister>;
    StatusRegisterType sr;

    struct StatusFlag {
        using PosType  = Pos<u8, StatusFlag>;
        using BitsType = Bits<u8, StatusFlag>;

        static constexpr PosType sf_pos = PosType(0);

        static constexpr BitsType sf = BitsType(1, sf_pos);
    };
    using StatusFlagType = Reg<u8, StatusFlag>;
    StatusFlagType sf;

    // /////////////////////////////////////////////
  
    struct InputRegister {
        using PosType     = Pos<u8, InputRegister>;
        using BitsType    = Bits<u8, InputRegister>;
        using MaskedType  = Masked<u8, InputRegister>;
        using ShiftedType = Shifted<u8, InputRegister>;

        static constexpr PosType ireg0_status_acquisition_pos   = PosType(0); // Defines SMPC status acquisition (IREG0).
        static constexpr PosType ireg0_br_pos                   = PosType(6); // Defines Intback break request (IREG0).
        static constexpr PosType ireg0_cont_pos                 = PosType(7); // Defines Intback continue request (IREG0).
        static constexpr PosType ireg1_ope_pos                  = PosType(1); // Defines if peripheral acquisition time is optimized (IREG1).
        static constexpr PosType ireg1_pen_pos                  = PosType(3); // Defines if peripheral data is enabled (IREG1).
        static constexpr PosType ireg1_p1md_pos                 = PosType(4); // Defines port 1 mode (IREG1).
        static constexpr PosType ireg1_p2md_pos                 = PosType(6); // Defines port 2 mode (IREG1).

        static constexpr u8 p1md_mask         = 0x03;
        static constexpr u8 p2md_mask         = 0x03;

        static constexpr BitsType ireg0_status_acquisition  = BitsType(1, ireg0_status_acquisition_pos);
        static constexpr BitsType ireg0_br                  = BitsType(1, ireg0_br_pos);
        static constexpr BitsType ireg0_cont                = BitsType(1, ireg0_cont_pos);
        static constexpr BitsType ireg1_ope                 = BitsType(1, ireg1_ope_pos);
        static constexpr BitsType ireg1_pen                 = BitsType(1, ireg1_pen_pos);

        static constexpr MaskedType port_1_mode_15_bytes  = MaskedType(p1md_mask, 0b00, ireg1_p1md_pos);
        static constexpr MaskedType port_1_mode_255_bytes = MaskedType(p1md_mask, 0b01, ireg1_p1md_pos);
        static constexpr MaskedType port_1_mode_reserved  = MaskedType(p1md_mask, 0b10, ireg1_p1md_pos);
        static constexpr MaskedType port_1_mode_0_byte    = MaskedType(p1md_mask, 0b11, ireg1_p1md_pos);

        static constexpr MaskedType port_2_mode_15_bytes  = MaskedType(p2md_mask, 0b00, ireg1_p2md_pos);
        static constexpr MaskedType port_2_mode_255_bytes = MaskedType(p2md_mask, 0b01, ireg1_p2md_pos);
        static constexpr MaskedType port_2_mode_reserved  = MaskedType(p2md_mask, 0b10, ireg1_p2md_pos);
        static constexpr MaskedType port_2_mode_0_byte    = MaskedType(p2md_mask, 0b11, ireg1_p2md_pos);

    };
    using InputRegisterType = Reg<u8, InputRegister>;
    std::array<InputRegisterType, input_registers_number> ireg;

    struct DataDirectionRegister {
        using PosType     = Pos<u8, DataDirectionRegister>;
        using BitsType    = Bits<u8, DataDirectionRegister>;
        using MaskedType  = Masked<u8, DataDirectionRegister>;
        using ShiftedType = Shifted<u8, DataDirectionRegister>;

        static constexpr PosType ddr_pos = PosType(0);

        static constexpr u8 ddr_mask = 0x3F;

        static constexpr ShiftedType ddr_shft = ShiftedType(ddr_mask, ddr_pos);

        GENERATE_MASKED_RANGE("SmpcRegisters::DataDirectionRegister ", DDR, ddr, ddr_mask, ddr_pos, ddr_mask);
    };
    using DataDirectionRegisterType = Reg<u8, DataDirectionRegister>;
    DataDirectionRegisterType ddr1;
    DataDirectionRegisterType ddr2;

    struct PortDataRegister {
        using PosType     = Pos<u8, PortDataRegister>;
        using BitsType    = Bits<u8, PortDataRegister>;
        using MaskedType  = Masked<u8, PortDataRegister>;
        using ShiftedType = Shifted<u8, PortDataRegister>;

        static constexpr PosType pdr_pos = PosType(0);

        static constexpr u8 pdr_mask = 0x3F;

        static constexpr ShiftedType pdr_shft = ShiftedType(pdr_mask, pdr_pos);

        GENERATE_MASKED_RANGE("SmpcRegisters::PortDataRegister", PDR, pdr, pdr_mask, pdr_pos, pdr_mask);
    };
    using PortDataRegisterType = Reg<u8, PortDataRegister>;
    PortDataRegisterType pdr1;
    PortDataRegisterType pdr2;

    struct IOSelect {
        using PosType  = Pos<u8, IOSelect>;
        using BitsType = Bits<u8, IOSelect>;

        static constexpr PosType iosel1_pos = PosType(0);
        static constexpr PosType iosel2_pos = PosType(1);

        // 1: SH2 direct mode, 0: SMPC control mode
        static constexpr BitsType iosel1 = BitsType(1, iosel1_pos); // Peripheral port 1 mode.
        static constexpr BitsType iosel2 = BitsType(1, iosel2_pos); // Peripheral port 2 mode.
    };
    using IOSelectType = Reg<u8, IOSelect>;
    IOSelectType iosel;

    struct ExternalLatchEnable {
        using PosType  = Pos<u8, ExternalLatchEnable>;
        using BitsType = Bits<u8, ExternalLatchEnable>;

        static constexpr PosType exle1_pos = PosType(0);
        static constexpr PosType exle2_pos = PosType(1);

        static constexpr BitsType exle1 = BitsType(1, exle1_pos); // External latch 1 enable.
        static constexpr BitsType exle2 = BitsType(1, exle2_pos); // External latch 2 enable.
    };
    using ExternalLatchEnableType = Reg<u8, ExternalLatchEnable>;
    ExternalLatchEnableType exle;
};

struct SaturnPadData {

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	SaturnStandardPad1stData
    ///
    /// \brief	First part of the standard Saturn PAD data.
    ///
    /// \author	Runik
    /// \date	16/03/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct SaturnStandardPad1stData {
        using PosType  = Pos<u8, SaturnStandardPad1stData>;
        using BitsType = Bits<u8, SaturnStandardPad1stData>;

        static constexpr PosType button_b_pos  = PosType(0);
        static constexpr PosType button_c_pos  = PosType(1);
        static constexpr PosType button_a_pos       = PosType(2);
        static constexpr PosType button_start_pos  = PosType(3);
        static constexpr PosType direction_up_pos              = PosType(4);
        static constexpr PosType direction_down_pos              = PosType(5);
        static constexpr PosType direction_left_pos              = PosType(6);
        static constexpr PosType direction_right_pos = PosType(7);

        static constexpr BitsType button_b        = BitsType(1, button_b_pos);
        static constexpr BitsType button_c        = BitsType(1, button_c_pos);
        static constexpr BitsType button_a        = BitsType(1, button_a_pos);
        static constexpr BitsType button_start    = BitsType(1, button_start_pos);
        static constexpr BitsType direction_up    = BitsType(1, direction_up_pos);
        static constexpr BitsType direction_down  = BitsType(1, direction_down_pos);
        static constexpr BitsType direction_left  = BitsType(1, direction_left_pos);
        static constexpr BitsType direction_right = BitsType(1, direction_right_pos);
    };
    using SaturnStandardPad1stDataType = Reg<u8, SaturnStandardPad1stData>;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	SaturnStandardPad2ndData
    ///
    /// \brief	Second part of the standard Saturn PAD data.
    ///
    /// \author	Runik
    /// \date	16/03/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct SaturnStandardPad2ndData {
        using PosType  = Pos<u8, SaturnStandardPad2ndData>;
        using BitsType = Bits<u8, SaturnStandardPad2ndData>;

        static constexpr PosType button_shoulder_left_pos  = PosType(3);
        static constexpr PosType button_z_pos              = PosType(4);
        static constexpr PosType button_y_pos              = PosType(5);
        static constexpr PosType button_x_pos              = PosType(6);
        static constexpr PosType button_shoulder_right_pos = PosType(7);

        static constexpr BitsType button_shoulder_left  = BitsType(1, button_shoulder_left_pos);
        static constexpr BitsType button_z              = BitsType(1, button_z_pos);
        static constexpr BitsType button_y              = BitsType(1, button_y_pos);
        static constexpr BitsType button_x              = BitsType(1, button_x_pos);
        static constexpr BitsType button_shoulder_right = BitsType(1, button_shoulder_right_pos);
    };
    using SaturnStandardPad2ndDataType = Reg<u8, SaturnStandardPad2ndData>;
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

// enum class ResetButtonStatus : bool {
//     button_off = false, ///< Reset button off.
//     button_on  = true   ///< Reset buton on.
// };

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   PeripheralDataRemaining
///
/// \brief  Peripheral data remaining values.
////////////////////////////////////////////////////////////////////////////////////////////////////

// enum class PeripheralDataRemaining : bool { no_remaining_peripheral_data = false, remaining_peripheral_data = true };

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

// union StatusRegister {
//     u8             raw;                       ///< Raw representation.
//     BitField<0, 2> port_1_mode;               ///< Defines port 1 mode (P1MD).
//     BitField<2, 2> port_2_mode;               ///< Defines port 2 mode (P2MD).
//     BitField<4>    reset_button_status;       ///< Defines RESB bit.
//     BitField<5>    peripheral_data_remaining; ///< Defines PDE bit.
//     BitField<6>    peripheral_data_location;  ///< Defines PDL bit.
//     BitField<6>    bit_6;                     ///< Bit 6 access.
//     BitField<7>    bit_7;                     ///< Bit 7 access.
// };

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	StatusFlag
///
/// \brief	Status Flag (SF).
///
/// \author	Runik
/// \date	20/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

// union StatusFlag {
//     u8          raw; ///< Raw representation.
//     BitField<0> sf;  ///< SF value.
// };

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

// union InputRegister {
//     u8             raw;                                 ///< Raw representation.
//     BitField<7>    ireg0_continue_request;              ///< Defines Intback continue request (IREG0).
//     BitField<6>    ireg0_break_request;                 ///< Defines Intback break request (IREG0).
//     BitField<0>    ireg0_status_acquisition;            ///< Defines SMPC status acquisition (IREG0).
//     BitField<6, 2> ireg1_port_2_mode;                   ///< Defines port 2 mode (IREG1).
//     BitField<4, 2> ireg1_port_1_mode;                   ///< Defines port 1 mode (IREG1).
//     BitField<3>    ireg1_peripheral_data_enable;        ///< Defines if peripheral data is enabled (IREG1).
//     BitField<1>    ireg1_acquisition_time_optimization; ///< Defines if peripheral acquisition time is optimized (IREG1).
// };

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

// enum class ButtonStatus : bool {
//     pressed  = false, ///< Button is pressed.
//     released = true   ///< Button is released.
// };

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	SaturnStandardPad1stData
///
/// \brief	First part of the standard Saturn PAD data.
///
/// \author	Runik
/// \date	20/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

// union SaturnStandardPad1stData {
//     u8          raw;             ///< Raw representation.
//     BitField<7> direction_right; ///< Right direction.
//     BitField<6> direction_left;  ///< Left direction.
//     BitField<5> direction_down;  ///< Down direction.
//     BitField<4> direction_up;    ///< Up direction.
//     BitField<3> button_start;    ///< Start button.
//     BitField<2> button_a;        ///< A button.
//     BitField<1> button_c;        ///< C button.
//     BitField<0> button_b;        ///< B button.
// };

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	SaturnStandardPad2ndData
///
/// \brief	Second part of the standard Saturn PAD data.
///
/// \author	Runik
/// \date	20/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

// union SaturnStandardPad2ndData {
//     u8          raw;                   ///< Raw representation.
//     BitField<7> button_shoulder_right; ///< Right shoulder button.
//     BitField<6> button_x;              ///< X button.
//     BitField<5> button_y;              ///< Y button.
//     BitField<4> button_z;              ///< Z button.
//     BitField<3> button_shoulder_left;  ///< Left shoulder button.
// };

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	DataDirectionRegister
///
/// \brief	Data Direction Register (DDRx).
///
/// \author	Runik
/// \date	21/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

// union DataDirectionRegister {
//     u8             raw; ///< Raw representation.
//     BitField<0, 6> ddr; ///< Used bits in the register.
// };

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  PortDataRegister
///
/// \brief  Port Data Register (PDRx).
///
/// \author Runik
/// \date   14/12/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

// union PortDataRegister {
//     u8             raw; ///< Raw representation.
//     BitField<0, 6> pdr; ///< Used bits in the register.
// };

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   PeripheralPortMode
///
/// \brief  IOSEL values.
////////////////////////////////////////////////////////////////////////////////////////////////////

// enum class PeripheralPortMode : bool {
//     smpc_control_mode = false, ///< SMPC control mode. (initial)
//     sh2_direct_mode   = true,  ///< SH2 direct mode
// };

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	IOSelect
///
/// \brief	I/O Select (IOSEL).
///
/// \author	Runik
/// \date	21/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

// union IOSelect {
//     u8          raw;                    ///< Raw representation.
//     BitField<1> peripheral_port_2_mode; ///< IOSEL2 bit.
//     BitField<0> peripheral_port_1_mode; ///< IOSEL1 bit.
// };

} // namespace saturnin::core
