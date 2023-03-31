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
#include <saturnin/src/utilities.h>

namespace saturnin::core {

namespace uti = saturnin::utilities;

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

struct SmpcRegs {

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	CommandRegister
    ///
    /// \brief	Command Register (COMREG).
    ///
    /// \author	Runik
    /// \date	31/03/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Comreg {
        using PosType    = Pos<u8, Comreg>;
        using MaskedType = Masked<u8, Comreg>;
        template<typename E>
        using EnumType = Enum<u8, Comreg, E>;

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

        static constexpr auto comreg_pos = PosType(0);

        static constexpr auto comreg_mask = 0x1F;

        static constexpr auto comreg_enum = EnumType<SmpcCommand>(comreg_mask, comreg_pos);
    };
    using ComregType = Reg<u8, Comreg>;
    ComregType comreg;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Sr
    ///
    /// \brief	Status Register (SR).
    ///
    /// \author	Runik
    /// \date	31/03/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Sr {
        using PosType     = Pos<u8, Sr>;
        using BitsType    = Bits<u8, Sr>;
        using MaskedType  = Masked<u8, Sr>;
        using ShiftedType = Shifted<u8, Sr>;

        static constexpr auto p1md_pos         = PosType(0); // Port 1 mode.
        static constexpr auto p2md_pos         = PosType(2); // Port 2 mode.
        static constexpr auto upper_nibble_pos = PosType(4);
        static constexpr auto resb_pos         = PosType(4); // Reset button status.
        static constexpr auto pde_pos          = PosType(5); // Peripheral data remaining.
        static constexpr auto pdl_pos          = PosType(6); // Peripheral data location (1: first, 0: second or above).
        static constexpr auto bit_6_pos        = PosType(6); // Bit 6
        static constexpr auto bit_7_pos        = PosType(7); // Bit 7

        static constexpr auto p1md_mask         = 0x03;
        static constexpr auto p2md_mask         = 0x03;
        static constexpr auto upper_nibble_mask = 0x0F;

        static constexpr auto resb  = BitsType(1, resb_pos);
        static constexpr auto pde   = BitsType(1, pde_pos);
        static constexpr auto pdl   = BitsType(1, pdl_pos);
        static constexpr auto bit_6 = BitsType(1, bit_6_pos);
        static constexpr auto bit_7 = BitsType(1, bit_7_pos);

        static constexpr auto port_1_mode_15_bytes  = MaskedType(p1md_mask, 0b00, p1md_pos);
        static constexpr auto port_1_mode_255_bytes = MaskedType(p1md_mask, 0b01, p1md_pos);
        static constexpr auto port_1_mode_reserved  = MaskedType(p1md_mask, 0b10, p1md_pos);
        static constexpr auto port_1_mode_0_byte    = MaskedType(p1md_mask, 0b11, p1md_pos);

        static constexpr auto port_2_mode_15_bytes  = MaskedType(p2md_mask, 0b00, p1md_pos);
        static constexpr auto port_2_mode_255_bytes = MaskedType(p2md_mask, 0b01, p1md_pos);
        static constexpr auto port_2_mode_reserved  = MaskedType(p2md_mask, 0b10, p1md_pos);
        static constexpr auto port_2_mode_0_byte    = MaskedType(p2md_mask, 0b11, p1md_pos);

        static constexpr auto upper_nibble = MaskedType(upper_nibble_mask, 0xF, upper_nibble_pos);

        GENERATE_MASKED_RANGE("SmpcRegs::Sr", P1MD, p1md, p1md_mask, p1md_pos, 4);
        GENERATE_MASKED_RANGE("SmpcRegs::Sr", P2MD, p2md, p2md_mask, p2md_pos, 4);
    };
    using SrType = Reg<u8, Sr>;
    SrType sr;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	StatusFlag
    ///
    /// \brief	Status Flag (SF).
    ///
    /// \author	Runik
    /// \date	31/03/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Sf {
        using PosType  = Pos<u8, Sf>;
        using BitsType = Bits<u8, Sf>;

        static constexpr auto sf_pos = PosType(0);

        static constexpr auto sf = BitsType(1, sf_pos);
    };
    using SfType = Reg<u8, Sf>;
    SfType sf;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Ireg
    ///
    /// \brief	Input Register (IREGx).
    ///
    /// \author	Runik
    /// \date	31/03/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Ireg {
        using PosType     = Pos<u8, Ireg>;
        using BitsType    = Bits<u8, Ireg>;
        using MaskedType  = Masked<u8, Ireg>;
        using ShiftedType = Shifted<u8, Ireg>;
        template<typename E>
        using EnumType = Enum<u8, Ireg, E>;

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

        static constexpr auto stac_pos = PosType(0); // Defines SMPC status acquisition (IREG0).
        static constexpr auto br_pos                       = PosType(6); // Defines Intback break request (IREG0).
        static constexpr auto cont_pos                     = PosType(7); // Defines Intback continue request (IREG0).
        static constexpr auto ope_pos  = PosType(1); // Defines if peripheral acquisition time is optimized (IREG1).
        static constexpr auto pen_pos  = PosType(3); // Defines if peripheral data is enabled (IREG1).
        static constexpr auto p1md_pos = PosType(4); // Defines port 1 mode (IREG1).
        static constexpr auto p2md_pos = PosType(6); // Defines port 2 mode (IREG1).

        static constexpr auto stac         = BitsType(1, stac_pos);
        static constexpr auto br                               = BitsType(1, br_pos);
        static constexpr auto cont                             = BitsType(1, cont_pos);
        static constexpr auto ope = BitsType(1, ope_pos);
        static constexpr auto pen   = BitsType(1, pen_pos);

        static constexpr auto stac_mask      = 0b1;
        static constexpr auto br_mask        = 0b1;
        static constexpr auto cont_mask      = 0b1;
        static constexpr auto ope_mask       = 0b1;
        static constexpr auto pen_mask       = 0b1;
        static constexpr auto port_mode_mask = 0b11;

        static constexpr auto p1md_shft = ShiftedType(port_mode_mask, p1md_pos);
        static constexpr auto p2md_shft = ShiftedType(port_mode_mask, p2md_pos);
        static constexpr auto cont_shft = ShiftedType(cont_mask, cont_pos);

        GENERATE_MASKED_RANGE("SmpcRegs::Ireg", P1MD, p1md, port_mode_mask, p1md_pos, 4);
        GENERATE_MASKED_RANGE("SmpcRegs::Ireg", P2MD, p2md, port_mode_mask, p2md_pos, 4);

        static constexpr auto stac_enum = EnumType<SmpcStatusAcquisition>(stac_mask, stac_pos);
        static constexpr auto br_enum = EnumType<IntbackBreakRequest>(br_mask, br_pos);
        static constexpr auto cont_enum = EnumType<IntbackContinueRequest>(cont_mask, cont_pos);
        static constexpr auto ope_enum = EnumType<AcquisitionTimeOptimization>(ope_mask, ope_pos);
        static constexpr auto pen_enum = EnumType<PeripheralDataEnable>(pen_mask, pen_pos);
    };
    using IregType = Reg<u8, Ireg>;
    std::array<IregType, input_registers_number> ireg;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	OutputRegister
    ///
    /// \brief	Output Register (OREGx).
    ///
    /// \author	Runik
    /// \date	31/03/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Oreg {
        using PosType     = Pos<u8, Oreg>;
        using BitsType    = Bits<u8, Oreg>;
        using MaskedType  = Masked<u8, Oreg>;
        using ShiftedType = Shifted<u8, Oreg>;
        template<typename E>
        using EnumType = Enum<u8, Oreg, E>;

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

        static constexpr auto bit_0_pos = PosType(0); // Bit 0 access.
        static constexpr auto bit_1_pos = PosType(1); // Bit 1 access.
        static constexpr auto bit_2_pos = PosType(2); // Bit 2 access.
        static constexpr auto bit_3_pos = PosType(3); // Bit 3 access.
        static constexpr auto bit_4_pos = PosType(4); // Bit 4 access.
        static constexpr auto bit_5_pos = PosType(5); // Bit 5 access.
        static constexpr auto bit_6_pos = PosType(6); // Bit 6 access.
        static constexpr auto bit_7_pos = PosType(7); // Bit 7 access.
        static constexpr auto resd_pos  = PosType(6); // Defines Reset Status (OREG0).
        static constexpr auto ste_pos   = PosType(7); // Defines Set Time (OREG0).

        static constexpr auto bit_0 = BitsType(1, bit_0_pos);
        static constexpr auto bit_1 = BitsType(1, bit_1_pos);
        static constexpr auto bit_2 = BitsType(1, bit_2_pos);
        static constexpr auto bit_3 = BitsType(1, bit_3_pos);
        static constexpr auto bit_4 = BitsType(1, bit_4_pos);
        static constexpr auto bit_5 = BitsType(1, bit_5_pos);
        static constexpr auto bit_6 = BitsType(1, bit_6_pos);
        static constexpr auto bit_7 = BitsType(1, bit_7_pos);
        static constexpr auto resd  = BitsType(1, resd_pos);
        static constexpr auto ste   = BitsType(1, ste_pos);

        static constexpr auto ste_mask            = std::byte{0x01};
        static constexpr auto sound_on_mask       = std::byte{0x01};
        static constexpr auto soft_reset_mask     = std::byte{0x01};
        static constexpr auto horizontal_res_mask = std::byte{0x01};

        static inline auto horizontalRes(const bool is_horizontal_res_352) -> MaskedType {
            return MaskedType(std::to_integer<u8>(horizontal_res_mask << bit_6_pos.pos()),
                              std::to_integer<u8>(std::byte{is_horizontal_res_352} << bit_6_pos.pos()));
        }

        static inline auto softResetAllowed(const bool is_soft_reset_allowed) -> MaskedType {
            return MaskedType(std::to_integer<u8>(soft_reset_mask << bit_3_pos.pos()),
                              std::to_integer<u8>(std::byte{is_soft_reset_allowed} << bit_3_pos.pos()));
        }

        static inline auto soundOn(const bool is_sound_on) -> MaskedType {
            return MaskedType(std::to_integer<u8>(sound_on_mask << bit_0_pos.pos()),
                              std::to_integer<u8>(std::byte{is_sound_on} << bit_0_pos.pos()));
        }

        static inline auto setTime(const bool is_time_set) -> MaskedType {
            return MaskedType(std::to_integer<u8>(ste_mask << ste_pos.pos()),
                              std::to_integer<u8>(std::byte{is_time_set} << ste_pos.pos()));
        }

        static constexpr auto comreg_pos     = PosType(0);
        static constexpr auto comreg_mask    = 0x1F;
        static constexpr auto interrupt_back = MaskedType(comreg_mask, 0x10, comreg_pos);
    };
    using OregType = Reg<u8, Oreg>;
    std::array<OregType, output_registers_number> oreg;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	DataDirectionRegister
    ///
    /// \brief	Data Direction Register (DDRx).
    ///
    /// \author	Runik
    /// \date	31/03/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Ddr {
        using PosType     = Pos<u8, Ddr>;
        using BitsType    = Bits<u8, Ddr>;
        using MaskedType  = Masked<u8, Ddr>;
        using ShiftedType = Shifted<u8, Ddr>;

        static constexpr auto ddr_pos = PosType(0);

        static constexpr auto ddr_mask = 0x3F;

        static constexpr auto ddr_shft = ShiftedType(ddr_mask, ddr_pos);

        GENERATE_MASKED_RANGE("SmpcRegs::Ddr", DDR, ddr, ddr_mask, ddr_pos, ddr_mask);
    };
    using DdrType = Reg<u8, Ddr>;
    DdrType ddr1;
    DdrType ddr2;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	PortDataRegister
    ///
    /// \brief	Port Data Register (PDRx).
    ///
    /// \author	Runik
    /// \date	31/03/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Pdr {
        using PosType     = Pos<u8, Pdr>;
        using BitsType    = Bits<u8, Pdr>;
        using MaskedType  = Masked<u8, Pdr>;
        using ShiftedType = Shifted<u8, Pdr>;

        static constexpr auto pdr_pos = PosType(0);

        static constexpr auto pdr_mask = 0x3F;

        static constexpr auto pdr_shft = ShiftedType(pdr_mask, pdr_pos);

        GENERATE_MASKED_RANGE("SmpcRegs::Pdr", PDR, pdr, pdr_mask, pdr_pos, pdr_mask);
    };
    using PdrType = Reg<u8, Pdr>;
    PdrType pdr1;
    PdrType pdr2;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	IOSelect
    ///
    /// \brief	I/O Select (IOSEL).
    ///
    /// \author	Runik
    /// \date	31/03/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct IOSelect {
        using PosType  = Pos<u8, IOSelect>;
        using BitsType = Bits<u8, IOSelect>;
        template<typename E>
        using EnumType = Enum<u8, IOSelect, E>;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   PeripheralPortMode
        ///
        /// \brief  IOSEL values.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class PeripheralPortMode : bool {
            smpc_control_mode = false, ///< SMPC control mode. (initial)
            sh2_direct_mode   = true,  ///< SH2 direct mode
        };

        static constexpr auto iosel1_pos = PosType(0); ///< (Immutable) Peripheral port 1 mode.
        static constexpr auto iosel2_pos = PosType(1); ///< (Immutable) Peripheral port 2 mode.

        // 1: SH2 direct mode, 0: SMPC control mode
        static constexpr auto iosel1 = BitsType(1, iosel1_pos);
        static constexpr auto iosel2 = BitsType(1, iosel2_pos);

        static constexpr auto iosel1_mask = 0b1;
        static constexpr auto iosel2_mask = 0b1;

        static constexpr auto iosel1_enum = EnumType<PeripheralPortMode>(iosel1_mask, iosel1_pos);
        static constexpr auto iosel2_enum = EnumType<PeripheralPortMode>(iosel2_mask, iosel2_pos);
    };
    using IOSelectType = Reg<u8, IOSelect>;
    IOSelectType iosel;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	ExternalLatchEnable
    ///
    /// \brief	External Latch Enable (EXLE).
    ///
    /// \author	Runik
    /// \date	31/03/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Exle {
        using PosType  = Pos<u8, Exle>;
        using BitsType = Bits<u8, Exle>;
        template<typename E>
        using EnumType = Enum<u8, Exle, E>;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   ExternalLatch
        ///
        /// \brief  EXLE values.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class ExternalLatch : bool {
            disabled = false, ///< Disabled. (initial)
            enabled  = true,  ///< Enabled
        };

        static constexpr auto exle1_pos = PosType(0); ///< (Immutable) External latch 1 enable.
        static constexpr auto exle2_pos = PosType(1); ///< (Immutable) External latch 2 enable.

        static constexpr auto exle1 = BitsType(1, exle1_pos);
        static constexpr auto exle2 = BitsType(1, exle2_pos);

        static constexpr auto exle1_mask = 0b1;
        static constexpr auto exle2_mask = 0b1;

        static constexpr auto exle1_enum = EnumType<ExternalLatch>(exle1_mask, exle1_pos);
        static constexpr auto exle2_enum = EnumType<ExternalLatch>(exle2_mask, exle2_pos);
    };
    using ExleType = Reg<u8, Exle>;
    ExleType exle;
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

        static constexpr PosType button_b_pos        = PosType(0);
        static constexpr PosType button_c_pos        = PosType(1);
        static constexpr PosType button_a_pos        = PosType(2);
        static constexpr PosType button_start_pos    = PosType(3);
        static constexpr PosType direction_up_pos    = PosType(4);
        static constexpr PosType direction_down_pos  = PosType(5);
        static constexpr PosType direction_left_pos  = PosType(6);
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

} // namespace saturnin::core
