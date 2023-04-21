//
// scu_registers.h
// Saturnin
//
// Copyright (c) 2018 Renaud Toumazet
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
/// \file	scu_registers.h
///
/// \brief	Declares SCU registers and related enums.
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <saturnin/src/emulator_defs.h>
#include <saturnin/src/bit_register.h>

namespace saturnin::core {

/// \name SCU registers addresses
//@{
constexpr auto level_0_dma_read_address         = u32{0x25FE0000};
constexpr auto level_0_dma_write_address        = u32{0x25FE0004};
constexpr auto level_0_dma_transfer_byte_number = u32{0x25FE0008};
constexpr auto level_0_dma_add_value_register   = u32{0x25FE000C};
constexpr auto level_0_dma_enable_register      = u32{0x25FE0010};
constexpr auto level_0_dma_mode_register        = u32{0x25FE0014};
constexpr auto level_1_dma_read_address         = u32{0x25FE0020};
constexpr auto level_1_dma_write_address        = u32{0x25FE0024};
constexpr auto level_1_dma_transfer_byte_number = u32{0x25FE0028};
constexpr auto level_1_dma_add_value_register   = u32{0x25FE002C};
constexpr auto level_1_dma_enable_register      = u32{0x25FE0030};
constexpr auto level_1_dma_mode_register        = u32{0x25FE0034};
constexpr auto level_2_dma_read_address         = u32{0x25FE0040};
constexpr auto level_2_dma_write_address        = u32{0x25FE0044};
constexpr auto level_2_dma_transfer_byte_number = u32{0x25FE0048};
constexpr auto level_2_dma_add_value_register   = u32{0x25FE004C};
constexpr auto level_2_dma_enable_register      = u32{0x25FE0050};
constexpr auto level_2_dma_mode_register        = u32{0x25FE0054};
constexpr auto dma_forced_stop             = u32{0x25FE0060}; // not used anymore, according to SCU Final Specifications (No 14)
constexpr auto dma_status_register         = u32{0x25FE007C};
constexpr auto dsp_program_control_port    = u32{0x25FE0080};
constexpr auto dsp_program_ram_dataport    = u32{0x25FE0084};
constexpr auto dsp_data_ram_address_port   = u32{0x25FE0088};
constexpr auto dsp_data_ram_data_port      = u32{0x25FE008C};
constexpr auto timer_0_compare_register    = u32{0x25FE0090};
constexpr auto timer_1_set_data_register   = u32{0x25FE0094};
constexpr auto timer_1_mode_register       = u32{0x25FE0098};
constexpr auto interrupt_mask_register     = u32{0x25FE00A0};
constexpr auto interrupt_status_register   = u32{0x25FE00A4};
constexpr auto a_bus_interrupt_acknowledge = u32{0x25FE00A8};
constexpr auto a_bus_set_register          = u32{0x25FE00B0};
constexpr auto a_bus_refresh_register      = u32{0x25FE00B8};
constexpr auto scu_sdram_select_register   = u32{0x25FE00C4};
constexpr auto scu_version_register        = u32{0x25FE00C8};
//@}

struct ScuRegs {
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Dxr
    ///
    /// \brief	DMA Level 2-0 Read Address Register (D0R, D1R & D2R).
    ///
    /// \author	Runik
    /// \date	06/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Dxr {
        GENERATE_USING(Dxr, u32);

        static constexpr auto ra_pos  = PosType(0); ///< Defines read address in bytes.
        static constexpr auto ra_mask = 0x7FFFFFF;
        static constexpr auto ra_shft = ShiftedType(ra_mask, ra_pos);

        GENERATE_MASKED_RANGE("ScuRegs::Dxr", READ_ADDRESS, readAddress, ra_mask, ra_pos, ra_mask);
    };
    using DxrType = Reg<u32, Dxr>;
    DxrType d0r;
    DxrType d1r;
    DxrType d2r;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Dxw
    ///
    /// \brief	DMA Level 2-0 Write Address Register (D0W, D1W & D2W).
    ///
    /// \author	Runik
    /// \date	06/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Dxw {
        GENERATE_USING(Dxw, u32);

        static constexpr auto wa_pos  = PosType(0); ///< Defines write address in bytes.
        static constexpr auto wa_mask = 0x7FFFFFF;
        static constexpr auto wa_shft = ShiftedType(wa_mask, wa_pos);

        GENERATE_MASKED_RANGE("ScuRegs::Dxw", WRITE_ADDRESS, writeAddress, wa_mask, wa_pos, wa_mask);
    };
    using DxwType = Reg<u32, Dxw>;
    DxwType d0w;
    DxwType d1w;
    DxwType d2w;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	D0c
    ///
    /// \brief	DMA Level 0 Transfer Byte Number (D0C).
    ///
    /// \author	Runik
    /// \date	06/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct D0c {
        GENERATE_USING(D0c, u32);

        static constexpr auto tbn_pos  = PosType(0); ///< Defines transfer byte number.
        static constexpr auto tbn_mask = 0xFFFFF;
        static constexpr auto tbn_shft = ShiftedType(tbn_mask, tbn_pos);

        GENERATE_MASKED_RANGE("ScuRegs::D0c", TRANSFER_BYTE_NUMBER, transferByteNumber, tbn_mask, tbn_pos, tbn_mask);
    };
    using D0cType = Reg<u32, D0c>;
    D0cType d0c;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	D1c
    ///
    /// \brief	DMA Level 1 Transfer Byte Number (D1C).
    ///
    /// \author	Runik
    /// \date	06/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct D1c {
        GENERATE_USING(D1c, u32);

        static constexpr auto tbn_pos  = PosType(0); ///< Defines transfer byte number.
        static constexpr auto tbn_mask = 0xFFF;
        static constexpr auto tbn_shft = ShiftedType(tbn_mask, tbn_pos);

        GENERATE_MASKED_RANGE("ScuRegs::D1c", TRANSFER_BYTE_NUMBER, transferByteNumber, tbn_mask, tbn_pos, tbn_mask);
    };
    using D1cType = Reg<u32, D1c>;
    D1cType d1c;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	D2c
    ///
    /// \brief	DMA Level 2 Transfer Byte Number (D2C).
    ///
    /// \author	Runik
    /// \date	06/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct D2c {
        GENERATE_USING(D2c, u32);

        static constexpr auto tbn_pos  = PosType(0); ///< Defines transfer byte number.
        static constexpr auto tbn_mask = 0xFFF;
        static constexpr auto tbn_shft = ShiftedType(tbn_mask, tbn_pos);

        GENERATE_MASKED_RANGE("ScuRegs::D2c", TRANSFER_BYTE_NUMBER, transferByteNumber, tbn_mask, tbn_pos, tbn_mask);
    };
    using D2cType = Reg<u32, D2c>;
    D2cType d2c;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Dxad
    ///
    /// \brief	DMA Address Add Value Register (D0AD, D1AD and D2AD).
    ///
    /// \author	Runik
    /// \date	06/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Dxad {
        GENERATE_USING(Dxad, u32);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   ReadAddressAddValue
        ///
        /// \brief  Number of bytes added to the read address.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class ReadAddressAddValue : bool {
            add_0 = false, ///< Nothing is added.
            add_4 = true   ///< 4 bytes are added
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   WriteAddressAddValue
        ///
        /// \brief  Number of bytes added to the write address.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class WriteAddressAddValue : u8 {
            add_0   = 0b000, ///< Nothing is added.
            add_2   = 0b001, ///< 2 bytes are added.
            add_4   = 0b010, ///< 4 bytes are added.
            add_8   = 0b011, ///< 8 bytes are added.
            add_16  = 0b100, ///< 16 bytes are added.
            add_32  = 0b101, ///< 32 bytes are added.
            add_64  = 0b110, ///< 64 bytes are added.
            add_128 = 0b111  ///< 128 bytes are added.
        };

        GENERATE_BIT_WITH_ENUM(dxwa, 0, 0b111, WriteAddressAddValue); ///< Defines read address add value (D0RA, D1RA and D2RA).
        GENERATE_BIT_WITH_ENUM(dxra, 8, 0b1, ReadAddressAddValue);    ///< Defines read address add value (D0RA, D1RA and D2RA).
    };
    using DxadType = Reg<u32, Dxad>;
    DxadType d0ad;
    DxadType d1ad;
    DxadType d2ad;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Dxen
    ///
    /// \brief	DMA Enable Register (DxEN).
    ///
    /// \author	Runik
    /// \date	06/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Dxen {
        GENERATE_USING(Dxen, u32);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   DmaEnable
        ///
        /// \brief  DxEN bit values.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class DmaEnable : bool {
            disabled = false, ///< DMA is disabled.
            enabled  = true   ///< DMA is enabled.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   DmaStarting
        ///
        /// \brief  DxGO bit values.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class DmaStarting : bool {
            not_started = false, ///< DMA isn't started.
            started     = true   ///< DMA starts execution. Only relevant when start factor is DMA.
        };

        GENERATE_BIT_WITH_ENUM(dxen, 0, 0b1, DmaEnable);   ///< Defines DMA Enable Bit (D0EN, D1EN, D2EN)
        GENERATE_BIT_WITH_ENUM(dxgo, 8, 0b1, DmaStarting); ///< Defines DMA Starting Bit (D0GO, D1GO, D2GO).
    };
    using DxenType = Reg<u32, Dxen>;
    DxenType d0en;
    DxenType d1en;
    DxenType d2en;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Dxmd
    ///
    /// \brief	DMA Mode, Address Update, Start Factor Select Register (DxMD).
    ///
    /// \author	Runik
    /// \date	06/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Dxmd {
        GENERATE_USING(Dxmd, u32);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   DmaMode
        ///
        /// \brief  DxMOD bit values.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class DmaMode : bool {
            direct   = false, ///< Direct mode.
            indirect = true   ///< Indirect mode.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   ReadAddressUpdate
        ///
        /// \brief  DxRUP bit values.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class ReadAddressUpdate : bool {
            save   = false, ///< Saves the value.
            update = true   ///< Updates the value.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   WriteAddressUpdate
        ///
        /// \brief  DxWUP bit values.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class WriteAddressUpdate : bool {
            save   = false, ///< Saves the value.
            update = true   ///< Updates the value.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   StartingFactorSelect
        ///
        /// \brief  DxFTy bits values.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class StartingFactorSelect : u8 {
            v_blank_in       = 0b000, ///< V-bLANK-IN signal receive and enable bit setting.
            v_blank_out      = 0b001, ///< V-bLANK-OUT signal receive and enable bit setting.
            h_blank_in       = 0b010, ///< H-bLANK-IN signal receive and enable bit setting.
            timer_0          = 0b011, ///< Timer 0 signal receive and enable bit setting.
            timer_1          = 0b100, ///< Timer 1 signal receive and enable bit setting.
            sound_req        = 0b101, ///< Sound Req signal receive and enable bit setting.
            sprite_draw_end  = 0b110, ///< Sprite draw end signal receive and enable bit setting.
            dma_start_factor = 0b111, ///< DMA starting factor setting and enable bit setting.
            none             = 0b11111111
        };

        GENERATE_BIT_WITH_ENUM(dxft, 0, 0b111, StartingFactorSelect); ///< Defines Starting Factor Select (DxFTy)
        GENERATE_BIT_WITH_ENUM(dxwup, 8, 0b1, WriteAddressUpdate);    ///< Defines Write Address Update (DxWUP).
        GENERATE_BIT_WITH_ENUM(dxrup, 16, 0b1, ReadAddressUpdate);    ///< Defines Read Address Update (DxRUP).
        GENERATE_BIT_WITH_ENUM(dxmod, 16, 0b1, DmaMode);              ///< Defines DMA Mode (DxMOD).
    };
    using DxmdType = Reg<u32, Dxmd>;
    DxmdType d0md;
    DxmdType d1md;
    DxmdType d2md;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Dsta
    ///
    /// \brief	DMA Status Register (DSTA).
    ///
    /// \author	Runik
    /// \date	06/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Dsta {
        GENERATE_USING(Dsta, u32);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   DspBusAccess
        ///
        /// \brief  DACSD bit values.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class DspBusAccess : bool {
            not_accessing = false, ///< Not accessing the DSP Bus.
            accessing     = true   ///< Accessing the DSP Bus.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   BBusAccess
        ///
        /// \brief  DACSB bit values.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class BBusAccess : bool {
            not_accessing = false, ///< Not accessing the B Bus.
            accessing     = true   ///< Accessing the B Bus.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   ABusAccess
        ///
        /// \brief  DACSA bit values.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class ABusAccess : bool {
            not_accessing = false, ///< Not accessing the A Bus.
            accessing     = true   ///< Accessing the A Bus.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   Level1DmaInterrupt
        ///
        /// \brief  D1BK bit values.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class Level1DmaInterrupt : bool {
            not_interrupted = false, ///< Level 1 DMA isn't interrupted.
            interrupted     = true   ///< Level 1 DMA is interrupted.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   Level0DmaInterrupt
        ///
        /// \brief  D0BK bit values.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class Level0DmaInterrupt : bool {
            not_interrupted = false, ///< Level 0 DMA isn't interrupted.
            interrupted     = true   ///< Level 0 DMA is interrupted.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   Level2DmaStandBy
        ///
        /// \brief  D2WT bit values.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class Level2DmaStandBy : bool {
            not_on_standby = false, ///< Level 2 DMA isn't on standby.
            on_standby     = true   ///< Level 2 DMA is on standby.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   Level2DmaOperation
        ///
        /// \brief  D2MV bit values.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class Level2DmaOperation : bool {
            not_in_operation = false, ///< Level 2 DMA isn't in operation.
            in_operation     = true   ///< Level 2 DMA is in operation.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   Level1DmaStandBy
        ///
        /// \brief  D1WT bit values.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class Level1DmaStandBy : bool {
            not_on_standby = false, ///< Level 1 DMA isn't on standby.
            on_standby     = true   ///< Level 1 DMA is on standby.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   Level1DmaOperation
        ///
        /// \brief  D1MV bit values.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class Level1DmaOperation : bool {
            not_in_operation = false, ///< Level 1 DMA isn't in operation.
            in_operation     = true   ///< Level 1 DMA is in operation.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   Level0DmaStandBy
        ///
        /// \brief  D0WT bit values.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class Level0DmaStandBy : bool {
            not_on_standby = false, ///< Level 0 DMA isn't on standby.
            on_standby     = true   ///< Level 0 DMA is on standby.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   Level0DmaOperation
        ///
        /// \brief  D0MV bit values.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class Level0DmaOperation : bool {
            not_in_operation = false, ///< Level 0 DMA isn't in operation.
            in_operation     = true   ///< Level 0 DMA is in operation.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   DspDmaStandBy
        ///
        /// \brief  DDWT bit values.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class DspDmaStandBy : bool {
            not_on_standby = false, ///< DSP DMA isn't on standby.
            on_standby     = true   ///< DSP DMA is on standby.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   DspDmaOperation
        ///
        /// \brief  DDMV bit values.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class DspDmaOperation : bool {
            not_in_operation = false, ///< DSP DMA isn't in operation.
            in_operation     = true   ///< DSP DMA is in operation.
        };

        GENERATE_BIT_WITH_ENUM(ddmv, 0, 0b1, DspDmaOperation);     ///< DSP DMA Operation (DDMV).
        GENERATE_BIT_WITH_ENUM(ddwt, 1, 0b1, DspDmaStandBy);       ///< DSP DMA Stand By (DDWT).
        GENERATE_BIT_WITH_ENUM(d0mv, 4, 0b1, Level0DmaOperation);  ///< Level 0 DMA Operation (D0MV).
        GENERATE_BIT_WITH_ENUM(d0wt, 5, 0b1, Level0DmaStandBy);    ///< Level 0 DMA Stand By (D0WT).
        GENERATE_BIT_WITH_ENUM(d1mv, 8, 0b1, Level1DmaOperation);  ///< Level 1 DMA Operation (D1MV).
        GENERATE_BIT_WITH_ENUM(d1wt, 9, 0b1, Level1DmaStandBy);    ///< Level 1 DMA Stand By (D1WT).
        GENERATE_BIT_WITH_ENUM(d2mv, 12, 0b1, Level2DmaOperation); ///< Level 2 DMA Operation (D2MV).
        GENERATE_BIT_WITH_ENUM(d2wt, 13, 0b1, Level2DmaStandBy);   ///< Level 2 DMA Stand By (D2WT).
        GENERATE_BIT_WITH_ENUM(d0bk, 16, 0b1, Level0DmaInterrupt); ///< Level 0 DMA Interrupt (D0BK).
        GENERATE_BIT_WITH_ENUM(d1bk, 17, 0b1, Level1DmaInterrupt); ///< Level 1 DMA Interrupt (D1BK).
        GENERATE_BIT_WITH_ENUM(dacsa, 20, 0b1, ABusAccess);        ///< A Bus access (DACSA).
        GENERATE_BIT_WITH_ENUM(dacsb, 21, 0b1, BBusAccess);        ///< B Bus access (DACSB).
        GENERATE_BIT_WITH_ENUM(dacsd, 22, 0b1, DspBusAccess);      ///< DSP Bus access (DACSD).
    };
    using DstaType = Reg<u32, Dsta>;
    DstaType dsta;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	T0c
    ///
    /// \brief	 Timer 0 compare register (T0C).
    ///
    /// \author	Runik
    /// \date	07/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct T0c {
        GENERATE_USING(T0c, u32);

        static constexpr auto t0c_pos  = PosType(0); ///< Timer 0 compare data bits.
        static constexpr auto t0c_mask = 0x3FF;
        static constexpr auto t0c_shft = ShiftedType(t0c_mask, t0c_pos);

        GENERATE_MASKED_RANGE("ScuRegs::T0c", TIMER_0_COMPARE, timer0Compare, t0c_mask, t0c_pos, t0c_mask);
    };
    using T0cType = Reg<u32, T0c>;
    T0cType t0c;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	T1s
    ///
    /// \brief	Timer 1 Set Data Register (T1S).
    ///
    /// \author	Runik
    /// \date	07/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct T1s {
        GENERATE_USING(T1s, u32);

        static constexpr auto t1s_pos  = PosType(0); ///< Timer 1 set data bits.
        static constexpr auto t1s_mask = 0x1FF;
        static constexpr auto t1s_shft = ShiftedType(t1s_mask, t1s_pos);

        GENERATE_MASKED_RANGE("ScuRegs::T1s", TIMER_0_COMPARE, timer0Compare, t1s_mask, t1s_pos, t1s_mask);
    };
    using T1sType = Reg<u32, T1s>;
    T1sType t1s;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	T1md
    ///
    /// \brief	Timer 1 Mode Register (T1MD).
    ///
    /// \author	Runik
    /// \date	07/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct T1md {
        GENERATE_USING(T1md, u32);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   Timer1Mode
        ///
        /// \brief  T1MD bit value.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class Timer1Mode : bool {
            interrupt_occurs_each_line     = false, ///< Interrupt occurs at each line.
            interrupt_occurs_only_at_timer = true   ///< Interrupt occurs only at lines indicated by Timer 0.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   TimerEnable
        ///
        /// \brief  TENB bit value.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class TimerEnable : bool {
            timer_operation_off = false, ///< Timer operation off.
            timer_operation_on  = true   ///< Timer operation on.
        };

        GENERATE_BIT_WITH_ENUM(tenb, 0, 0b1, TimerEnable); ///< Timer Enable (TENB).
        GENERATE_BIT_WITH_ENUM(t1md, 8, 0b1, Timer1Mode);  ///< Timer 1 Mode (T1MD).
    };
    using T1mdType = Reg<u32, T1md>;
    T1mdType t1md;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Ppaf
    ///
    /// \brief	DSP Program Control Port (PPAF).
    ///
    /// \author	Runik
    /// \date	07/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Ppaf {
        GENERATE_USING(Ppaf, u32);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   ExecutePauseReset
        ///
        /// \brief  PR bit value.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class ExecutePauseReset : bool {
            no_change              = false, ///< No condition change.
            program_pause_is_reset = true   ///< Program pause is reset when program execution begins.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   ExecutePause
        ///
        /// \brief  EP bit value.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class ExecutePause : bool {
            no_change      = false, ///< No condition change.
            program_pauses = true   ///< Program pauses when program execution begins.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   D0BusDmaExecution
        ///
        /// \brief  T0 bit value.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class D0BusDmaExecution : bool {
            no_dma           = false, ///< No DMA is executing.
            dma_is_executing = true   ///< Executing DMA using the D0-Bus.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   Sign
        ///
        /// \brief  S bit value.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class Sign : bool {
            operation_result_is_not_negative = false, ///< Operation result isn't negative.
            operation_result_is_negative     = true   ///< Operation result is negative.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   Zero
        ///
        /// \brief  Z bit value.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class Zero : bool {
            operation_result_is_not_zero = false, ///< Operation result isn't 0.
            operation_result_is_zero     = true   ///< Operation result is 0.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   Carry
        ///
        /// \brief  C bit value.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class Carry : bool {
            operation_result_has_no_carry = false, ///< Carry does not occurs in operation result.
            operation_result_has_carry    = true   ///< Carry occurs in operation result.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   Overflow
        ///
        /// \brief  V bit value.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class Overflow : bool {
            operation_does_not_cause_overflow = false, ///< Operation result does not cause overflow or underflow.
            operation_causes_overflow         = true   ///< Operation result causes overflow or underflow.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   ProgramEndInterrupt
        ///
        /// \brief  E bit value.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class ProgramEndInterrupt : bool {
            program_not_ended_by_endi = false, ///< Program not ended by ENDI command.
            program_ended_by_endi     = true   ///< Program ended by ENDI command detected.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   StepExecuteControl
        ///
        /// \brief  ES bit value.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class StepExecuteControl : bool {
            program_does_not_execute_one_step = false, ///< Program does not execute one step.
            program_executes_one_step         = true   ///< Program executes one step if program is paused.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   ProgramExecuteControl
        ///
        /// \brief  EX bit value.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class ProgramExecuteControl : bool {
            program_execution_stops  = false, ///< Program execution stops when writing 0.
            program_execution_begins = true   ///< Program execution begins when writing 1.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   ProgramCounterTransferEnable
        ///
        /// \brief  LE bit value.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class ProgramCounterTransferEnable : bool {
            program_ram_address_not_loaded_to_program_counter
            = false, ///< The program RAM address is not loaded to the program counter.
            program_ram_address_loaded_to_program_counter = true ///< The program RAM address is loaded to the program counter.
        };

        GENERATE_BIT_WITH_ENUM(le, 15, 0b1, ProgramCounterTransferEnable); ///< Program Counter Transfer Enable (LE).
        GENERATE_BIT_WITH_ENUM(ex, 16, 0b1, ProgramExecuteControl);        ///< Program Execute Control (EX).
        GENERATE_BIT_WITH_ENUM(es, 17, 0b1, StepExecuteControl);           ///< Step Execute Control (ES).
        GENERATE_BIT_WITH_ENUM(e, 18, 0b1, ProgramEndInterrupt);           ///< Program End Interrupt (E).
        GENERATE_BIT_WITH_ENUM(v, 19, 0b1, Overflow);                      ///< Overflow (V).
        GENERATE_BIT_WITH_ENUM(c, 20, 0b1, Carry);                         ///< Carry (C).
        GENERATE_BIT_WITH_ENUM(z, 21, 0b1, Zero);                          ///< Zero (Z).
        GENERATE_BIT_WITH_ENUM(s, 22, 0b1, Sign);                          ///< Sign (S).
        GENERATE_BIT_WITH_ENUM(t0, 23, 0b1, D0BusDmaExecution);            ///< D0 Bus DMA Execution (T0).
        GENERATE_BIT_WITH_ENUM(ep, 25, 0b1, ExecutePause);                 ///< Execute Pause (EP).
        GENERATE_BIT_WITH_ENUM(pp, 26, 0b1, ExecutePauseReset);            ///< Execute Pause Reset (PP).

        static constexpr auto p_pos  = PosType(0);                         ///< Program RAM Address (P7-0).
        static constexpr auto p_mask = 0xFF;
        static constexpr auto p_shft = ShiftedType(p_mask, p_pos);

        GENERATE_MASKED_RANGE("ScuRegs::Ppaf", PROGRAM_RAM_ADDRESS, programRamAddress, p_mask, p_pos, p_mask);
    };
    using PpafType = Reg<u32, Ppaf>;
    PpafType ppaf;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Ppd
    ///
    /// \brief	DSP Program Data Port (PPD).
    ///
    /// \author	Runik
    /// \date	07/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Ppd {};
    using PpdType = Reg<u32, Ppd>;
    PpdType ppd;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Ppa
    ///
    /// \brief	DSP Data RAM Address Port (PPA).
    ///
    /// \author	Runik
    /// \date	07/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Pda {
        GENERATE_USING(Pda, u32);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   DataRamSelect
        ///
        /// \brief  RA7-6 value bits
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class DataRamSelect : u8 {
            ram0 = 0b00, ///< Selects RAM0.
            ram1 = 0b01, ///< Selects RAM1.
            ram2 = 0b10, ///< Selects RAM2.
            ram3 = 0b11, ///< Selects RAM3.
        };

        GENERATE_BIT_WITH_ENUM(drs, 6, 0b11, DataRamSelect); ///< Data RAM Select (RA6-7).

        static constexpr auto dra_pos  = PosType(0);         ///< Data RAM Address (RA6-0).
        static constexpr auto dra_mask = 0x3F;
        static constexpr auto dra_shft = ShiftedType(dra_mask, dra_pos);

        GENERATE_MASKED_RANGE("ScuRegs::Pda", DATA_RAM_ADDRESS, dataRamAddress, dra_mask, dra_pos, dra_mask);
    };
    using PdaType = Reg<u32, Pda>;
    PdaType pda;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Pdd
    ///
    /// \brief	DSP Data RAM Data Port (PDD).
    ///
    /// \author	Runik
    /// \date	07/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Pdd {};
    using PddType = Reg<u32, Pdd>;
    PddType pdd;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \class  Ims
    ///
    /// \brief  Interrupt Mask Register (IMS).
    ///
    /// \author Runik
    /// \date   21/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Ims {
        GENERATE_USING(Ims, u32);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   InterruptMask
        ///
        /// \brief  Values that represent interrupt masks.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class InterruptMask : bool {
            not_masked = false, ///< Interupt is not masked
            masked     = true   ///< Interrupt is masked
        };

        GENERATE_BIT_WITH_ENUM(external_15, 31, 0b1, InterruptMask);     ///< Defines External interrupt 15 mask bit.
        GENERATE_BIT_WITH_ENUM(external_14, 30, 0b1, InterruptMask);     ///< Defines External interrupt 14 mask bit.
        GENERATE_BIT_WITH_ENUM(external_13, 29, 0b1, InterruptMask);     ///< Defines External interrupt 13 mask bit.
        GENERATE_BIT_WITH_ENUM(external_12, 28, 0b1, InterruptMask);     ///< Defines External interrupt 12 mask bit.
        GENERATE_BIT_WITH_ENUM(external_11, 27, 0b1, InterruptMask);     ///< Defines External interrupt 11 mask bit.
        GENERATE_BIT_WITH_ENUM(external_10, 26, 0b1, InterruptMask);     ///< Defines External interrupt 10 mask bit.
        GENERATE_BIT_WITH_ENUM(external_09, 25, 0b1, InterruptMask);     ///< Defines External interrupt 09 mask bit.
        GENERATE_BIT_WITH_ENUM(external_08, 24, 0b1, InterruptMask);     ///< Defines External interrupt 08 mask bit.
        GENERATE_BIT_WITH_ENUM(external_07, 23, 0b1, InterruptMask);     ///< Defines External interrupt 07 mask bit.
        GENERATE_BIT_WITH_ENUM(external_06, 22, 0b1, InterruptMask);     ///< Defines External interrupt 06 mask bit.
        GENERATE_BIT_WITH_ENUM(external_05, 21, 0b1, InterruptMask);     ///< Defines External interrupt 05 mask bit.
        GENERATE_BIT_WITH_ENUM(external_04, 20, 0b1, InterruptMask);     ///< Defines External interrupt 04 mask bit.
        GENERATE_BIT_WITH_ENUM(external_03, 19, 0b1, InterruptMask);     ///< Defines External interrupt 03 mask bit.
        GENERATE_BIT_WITH_ENUM(external_02, 18, 0b1, InterruptMask);     ///< Defines External interrupt 02 mask bit.
        GENERATE_BIT_WITH_ENUM(external_01, 17, 0b1, InterruptMask);     ///< Defines External interrupt 01 mask bit.
        GENERATE_BIT_WITH_ENUM(external_00, 16, 0b1, InterruptMask);     ///< Defines External interrupt 00 mask bit.
        GENERATE_BIT_WITH_ENUM(a_bus, 15, 0b1, InterruptMask);           ///< Defines A-Bus interrupt mask bit.
        GENERATE_BIT_WITH_ENUM(sprite_draw_end, 13, 0b1, InterruptMask); ///< Defines Sprite Draw End interrupt mask bit.
        GENERATE_BIT_WITH_ENUM(dma_illegal, 12, 0b1, InterruptMask);     ///< Defines DMA Illegal interrupt mask bit.
        GENERATE_BIT_WITH_ENUM(level_0_dma, 11, 0b1, InterruptMask);     ///< Defines Level 0 DMA interrupt mask bit.
        GENERATE_BIT_WITH_ENUM(level_1_dma, 10, 0b1, InterruptMask);     ///< Defines Level 1 DMA interrupt mask bit.
        GENERATE_BIT_WITH_ENUM(level_2_dma, 9, 0b1, InterruptMask);      ///< Defines Level 2 DMA interrupt mask bit.
        GENERATE_BIT_WITH_ENUM(pad, 8, 0b1, InterruptMask);              ///< Defines PAD interrupt mask bit.
        GENERATE_BIT_WITH_ENUM(system_manager, 7, 0b1, InterruptMask);   ///< Defines System Manager interrupt mask bit.
        GENERATE_BIT_WITH_ENUM(sound_request, 6, 0b1, InterruptMask);    ///< Defines Sound Request interrupt mask bit.
        GENERATE_BIT_WITH_ENUM(dsp_end, 5, 0b1, InterruptMask);          ///< Defines DSP End interrupt mask bit.
        GENERATE_BIT_WITH_ENUM(timer_1, 4, 0b1, InterruptMask);          ///< Defines Timer 1 interrupt mask bit.
        GENERATE_BIT_WITH_ENUM(timer_0, 3, 0b1, InterruptMask);          ///< Defines Timer 0 interrupt mask bit.
        GENERATE_BIT_WITH_ENUM(h_blank_in, 2, 0b1, InterruptMask);       ///< Defines H-Blank-In interrupt mask bit.
        GENERATE_BIT_WITH_ENUM(v_blank_out, 1, 0b1, InterruptMask);      ///< Defines V-Blank-Out interrupt mask bit.
        GENERATE_BIT_WITH_ENUM(v_blank_in, 0, 0b1, InterruptMask);       ///< Defines V-Blank-In interrupt mask bit.
        GENERATE_BIT_WITH_ENUM(undefined, 0, 0b1, InterruptMask);        ///< Undefined value.
    };
    using ImsType = Reg<u32, Ims>;
    ImsType ims;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \class  Ist
    ///
    /// \brief  Interrupt Status Register (IST).
    ///
    /// \author Runik
    /// \date   21/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Ist {
        GENERATE_USING(Ist, u32);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   InterruptEnable
        ///
        /// \brief  Values that represent if an interrupt is enabled or not.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class InterruptEnable : bool {
            disabled = false, ///< Interrupt is disabled
            enabled  = true   ///< Interrupt is enabled
        };

        GENERATE_BIT_WITH_ENUM(external_15, 31, 0b1, InterruptEnable);     ///< Defines External interrupt 15 status.
        GENERATE_BIT_WITH_ENUM(external_14, 30, 0b1, InterruptEnable);     ///< Defines External interrupt 14 status.
        GENERATE_BIT_WITH_ENUM(external_13, 29, 0b1, InterruptEnable);     ///< Defines External interrupt 13 status.
        GENERATE_BIT_WITH_ENUM(external_12, 28, 0b1, InterruptEnable);     ///< Defines External interrupt 12 status.
        GENERATE_BIT_WITH_ENUM(external_11, 27, 0b1, InterruptEnable);     ///< Defines External interrupt 11 status.
        GENERATE_BIT_WITH_ENUM(external_10, 26, 0b1, InterruptEnable);     ///< Defines External interrupt 10 status.
        GENERATE_BIT_WITH_ENUM(external_09, 25, 0b1, InterruptEnable);     ///< Defines External interrupt 09 status.
        GENERATE_BIT_WITH_ENUM(external_08, 24, 0b1, InterruptEnable);     ///< Defines External interrupt 08 status.
        GENERATE_BIT_WITH_ENUM(external_07, 23, 0b1, InterruptEnable);     ///< Defines External interrupt 07 status.
        GENERATE_BIT_WITH_ENUM(external_06, 22, 0b1, InterruptEnable);     ///< Defines External interrupt 06 status.
        GENERATE_BIT_WITH_ENUM(external_05, 21, 0b1, InterruptEnable);     ///< Defines External interrupt 05 status.
        GENERATE_BIT_WITH_ENUM(external_04, 20, 0b1, InterruptEnable);     ///< Defines External interrupt 04 status.
        GENERATE_BIT_WITH_ENUM(external_03, 19, 0b1, InterruptEnable);     ///< Defines External interrupt 03 status.
        GENERATE_BIT_WITH_ENUM(external_02, 18, 0b1, InterruptEnable);     ///< Defines External interrupt 02 status.
        GENERATE_BIT_WITH_ENUM(external_01, 17, 0b1, InterruptEnable);     ///< Defines External interrupt 01 status.
        GENERATE_BIT_WITH_ENUM(external_00, 16, 0b1, InterruptEnable);     ///< Defines External interrupt 00 status.
        GENERATE_BIT_WITH_ENUM(a_bus, 15, 0b1, InterruptEnable);           ///< Defines A-Bus interrupt status.
        GENERATE_BIT_WITH_ENUM(sprite_draw_end, 13, 0b1, InterruptEnable); ///< Defines Sprite Draw End interrupt status.
        GENERATE_BIT_WITH_ENUM(dma_illegal, 12, 0b1, InterruptEnable);     ///< Defines DMA Illegal interrupt status.
        GENERATE_BIT_WITH_ENUM(level_0_dma, 11, 0b1, InterruptEnable);     ///< Defines Level 0 DMA interrupt status.
        GENERATE_BIT_WITH_ENUM(level_1_dma, 10, 0b1, InterruptEnable);     ///< Defines Level 1 DMA interrupt status.
        GENERATE_BIT_WITH_ENUM(level_2_dma, 9, 0b1, InterruptEnable);      ///< Defines Level 2 DMA interrupt status.
        GENERATE_BIT_WITH_ENUM(pad, 8, 0b1, InterruptEnable);              ///< Defines PAD interrupt status.
        GENERATE_BIT_WITH_ENUM(system_manager, 7, 0b1, InterruptEnable);   ///< Defines System Manager interrupt status.
        GENERATE_BIT_WITH_ENUM(sound_request, 6, 0b1, InterruptEnable);    ///< Defines Sound Request interrupt status.
        GENERATE_BIT_WITH_ENUM(dsp_end, 5, 0b1, InterruptEnable);          ///< Defines DSP End interrupt status.
        GENERATE_BIT_WITH_ENUM(timer_1, 4, 0b1, InterruptEnable);          ///< Defines Timer 1 interrupt status.
        GENERATE_BIT_WITH_ENUM(timer_0, 3, 0b1, InterruptEnable);          ///< Defines Timer 0 interrupt status.
        GENERATE_BIT_WITH_ENUM(h_blank_in, 2, 0b1, InterruptEnable);       ///< Defines H-Blank-In interrupt status.
        GENERATE_BIT_WITH_ENUM(v_blank_out, 1, 0b1, InterruptEnable);      ///< Defines V-Blank-Out interrupt status.
        GENERATE_BIT_WITH_ENUM(v_blank_in, 0, 0b1, InterruptEnable);       ///< Defines V-Blank-In interrupt status.
        GENERATE_BIT_WITH_ENUM(undefined, 0, 0b1, InterruptEnable);        ///< Undefined value.
    };
    using IstType = Reg<u32, Ist>;
    IstType ist;
};
} // namespace saturnin::core
