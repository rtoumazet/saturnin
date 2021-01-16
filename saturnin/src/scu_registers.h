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

#include <saturnin/src/pch.h>
#include <saturnin/src/emulator_defs.h>

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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  DmaReadAddressRegister
///
/// \brief  DMA Level 2-0 Read Address Register (D0R, D1R &amp; D2R).
///
/// \author Runik
/// \date   29/01/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class DmaReadAddressRegister : public Register {
  public:
    using Register::Register;
    inline static const auto read_address = BitRange<u32>{0, 26}; ///< Defines read address in bytes.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  DmaWriteAddressRegister
///
/// \brief  DMA Level 2-0 Write Address Register (D0W, D1W &amp; D2W).
///
/// \author Runik
/// \date   29/01/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class DmaWriteAddressRegister : public Register {
  public:
    using Register::Register;
    inline static const auto write_address = BitRange<u32>{0, 26}; ///< Defines write address in bytes.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  DmaLevel0TransferByteNumberRegister
///
/// \brief  DMA Level 0 Transfer Byte Number (D0C).
///
/// \author Runik
/// \date   29/01/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class DmaLevel0TransferByteNumberRegister : public Register {
  public:
    using Register::Register;
    inline static const auto transfer_byte_number = BitRange<u32>{0, 19}; ///< Defines transfer byte number.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  DmaLevel1TransferByteNumberRegister
///
/// \brief  DMA Level 1 Transfer Byte Number (D1C).
///
/// \author Runik
/// \date   29/01/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class DmaLevel1TransferByteNumberRegister : public Register {
  public:
    using Register::Register;
    inline static const auto transfer_byte_number = BitRange<u32>{0, 11}; ///< Defines transfer byte number.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  DmaLevel2TransferByteNumberRegister
///
/// \brief  DMA Level 2 Transfer Byte Number (D2C).
///
/// \author Runik
/// \date   29/01/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class DmaLevel2TransferByteNumberRegister : public Register {
  public:
    using Register::Register;
    inline static const auto transfer_byte_number = BitRange<u32>{0, 11}; ///< Defines transfer byte number.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   ReadAddressAddValue
///
/// \brief  Number of bytes added to the read address.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class ReadAddressAddValue : u8 {
    add_0 = 0b0, ///< Nothing is added.
    add_4 = 0b1  ///< 4 bytes are added
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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  DmaAddressAddValueRegister
///
/// \brief  DMA Address Add Value Register (D0AD, D1AD and D2AD).
///
/// \author Runik
/// \date   25/01/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class DmaAddressAddValueRegister : public Register {
  public:
    using Register::Register;
    inline static const auto read_add_value
        = BitRange<ReadAddressAddValue>{8}; ///< Defines read address add value (D0RA, D1RA and D2RA).
    inline static const auto write_add_value
        = BitRange<WriteAddressAddValue>{0, 2}; ///< Defines write address add value (D0WA, D1WA and D2WA).
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   DmaEnable
///
/// \brief  DxEN bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class DmaEnable : u8 {
    disabled = 0b0, ///< DMA is disabled.
    enabled  = 0b1  ///< DMA is enabled.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   DmaStarting
///
/// \brief  DxGO bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class DmaStarting : u8 {
    not_started = 0b0, ///< DMA isn't started.
    started     = 0b1  ///< DMA starts execution. Only relevant when start factor is DMA.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  DmaEnableRegister
///
/// \brief  DMA Enable Register (DxEN).
///
/// \author Runik
/// \date   27/01/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class DmaEnableRegister : public Register {
  public:
    using Register::Register;
    inline static const auto dma_enable   = BitRange<DmaEnable>{8};   ///< Defines DMA Enable Bit (D0EN, D1EN, D2EN)
    inline static const auto dma_starting = BitRange<DmaStarting>{0}; ///< Defines DMA Starting Bit (D0GO, D1GO, D2GO).
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   DmaMode
///
/// \brief  DxMOD bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class DmaMode : u8 {
    direct   = 0b0, ///< Direct mode.
    indirect = 0b1  ///< Indirect mode.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   ReadAddressUpdate
///
/// \brief  DxRUP bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class ReadAddressUpdate : u8 {
    save   = 0b0, ///< Saves the value.
    update = 0b1  ///< Updates the value.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   WriteAddressUpdate
///
/// \brief  DxWUP bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class WriteAddressUpdate : u8 {
    save   = 0b0, ///< Saves the value.
    update = 0b1  ///< Updates the value.
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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  DmaEnableRegister
///
/// \brief  DMA Mode, Address Update, Start Factor Select Register (DxMD).
///
/// \author Runik
/// \date   27/01/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class DmaModeRegister : public Register {
  public:
    using Register::Register;
    inline static const auto dma_mode               = BitRange<DmaMode>{24};                ///< Defines DxMOD bit.
    inline static const auto read_address_update    = BitRange<ReadAddressUpdate>{16};      ///< Defines DxRUP bit.
    inline static const auto write_address_update   = BitRange<WriteAddressUpdate>{8};      ///< Defines DxWUP bit.
    inline static const auto starting_factor_select = BitRange<StartingFactorSelect>{0, 2}; ///< Defines DxFTy bits.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   DspBusAccess
///
/// \brief  DACSD bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class DspBusAccess : u8 {
    not_accessing = 0b0, ///< Not accessing the DSP Bus.
    accessing     = 0b1  ///< Accessing the DSP Bus.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   BBusAccess
///
/// \brief  DACSB bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class BBusAccess : u8 {
    not_accessing = 0b0, ///< Not accessing the B Bus.
    accessing     = 0b1  ///< Accessing the B Bus.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   ABusAccess
///
/// \brief  DACSA bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class ABusAccess : u8 {
    not_accessing = 0b0, ///< Not accessing the A Bus.
    accessing     = 0b1  ///< Accessing the A Bus.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   Level1DmaInterrupt
///
/// \brief  D1BK bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Level1DmaInterrupt : u8 {
    not_interrupted = 0b0, ///< Level 1 DMA isn't interrupted.
    interrupted     = 0b1  ///< Level 1 DMA is interrupted.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   Level0DmaInterrupt
///
/// \brief  D0BK bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Level0DmaInterrupt : u8 {
    not_interrupted = 0b0, ///< Level 0 DMA isn't interrupted.
    interrupted     = 0b1  ///< Level 0 DMA is interrupted.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   Level2DmaStandBy
///
/// \brief  D2WT bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Level2DmaStandBy : u8 {
    not_on_standby = 0b0, ///< Level 2 DMA isn't on standby.
    on_standby     = 0b1  ///< Level 2 DMA is on standby.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   Level2DmaOperation
///
/// \brief  D2MV bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Level2DmaOperation : u8 {
    not_in_operation = 0b0, ///< Level 2 DMA isn't in operation.
    in_operation     = 0b1  ///< Level 2 DMA is in operation.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   Level1DmaStandBy
///
/// \brief  D1WT bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Level1DmaStandBy : u8 {
    not_on_standby = 0b0, ///< Level 1 DMA isn't on standby.
    on_standby     = 0b1  ///< Level 1 DMA is on standby.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   Level1DmaOperation
///
/// \brief  D1MV bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Level1DmaOperation : u8 {
    not_in_operation = 0b0, ///< Level 1 DMA isn't in operation.
    in_operation     = 0b1  ///< Level 1 DMA is in operation.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   Level0DmaStandBy
///
/// \brief  D0WT bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Level0DmaStandBy : u8 {
    not_on_standby = 0b0, ///< Level 0 DMA isn't on standby.
    on_standby     = 0b1  ///< Level 0 DMA is on standby.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   Level0DmaOperation
///
/// \brief  D0MV bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Level0DmaOperation : u8 {
    not_in_operation = 0b0, ///< Level 0 DMA isn't in operation.
    in_operation     = 0b1  ///< Level 0 DMA is in operation.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   DspDmaStandBy
///
/// \brief  DDWT bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class DspDmaStandBy : u8 {
    not_on_standby = 0b0, ///< DSP DMA isn't on standby.
    on_standby     = 0b1  ///< DSP DMA is on standby.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   DspDmaOperation
///
/// \brief  DDMV bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class DspDmaOperation : u8 {
    not_in_operation = 0b0, ///< DSP DMA isn't in operation.
    in_operation     = 0b1  ///< DSP DMA is in operation.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  DmaEnableRegister
///
/// \brief  DMA Status Register (DSTA).
///
/// \author Runik
/// \date   27/01/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class DmaStatusRegister : public Register {
  public:
    using Register::Register;
    inline static const auto dsp_bus_access        = BitRange<DspBusAccess>{22};       ///< Defines DACSD bit.
    inline static const auto b_bus_access          = BitRange<BBusAccess>{21};         ///< Defines DACSB bit.
    inline static const auto a_bus_access          = BitRange<ABusAccess>{20};         ///< Defines DACSA bit.
    inline static const auto level_1_dma_interrupt = BitRange<Level1DmaInterrupt>{17}; ///< Defines D1BK bit.
    inline static const auto level_0_dma_interrupt = BitRange<Level0DmaInterrupt>{16}; ///< Defines D0BK bit.
    inline static const auto level_2_dma_stand_by  = BitRange<Level2DmaStandBy>{13};   ///< Defines D2WT bit.
    inline static const auto level_2_dma_operation = BitRange<Level2DmaOperation>{12}; ///< Defines D2MV bit.
    inline static const auto level_1_dma_stand_by  = BitRange<Level1DmaStandBy>{9};    ///< Defines D1WT bit.
    inline static const auto level_1_dma_operation = BitRange<Level1DmaOperation>{8};  ///< Defines D1MV bit.
    inline static const auto level_0_dma_stand_by  = BitRange<Level0DmaStandBy>{5};    ///< Defines D0WT bit.
    inline static const auto level_0_dma_operation = BitRange<Level0DmaOperation>{4};  ///< Defines D0MV bit.
    inline static const auto dsp_dma_stand_by      = BitRange<DspDmaStandBy>{1};       ///< Defines DDWT bit.
    inline static const auto dsp_dma_operation     = BitRange<DspDmaOperation>{0};     ///< Defines DDMV bit.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   InterruptMask
///
/// \brief  Values that represent interrupt masks.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class InterruptMask : u8 {
    not_masked = 0, ///< Interupt is not masked
    masked     = 1  ///< Interrupt is masked
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  InterruptMaskRegister
///
/// \brief  Interrupt Mask Register (IMS).
///
/// \author Runik
/// \date   05/02/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class InterruptMaskRegister : public Register {
  public:
    using Register::Register;

    inline static const auto external_15     = BitRange<InterruptMask>{31}; ///< Defines External interrupt 15 mask bit.
    inline static const auto external_14     = BitRange<InterruptMask>{30}; ///< Defines External interrupt 14 mask bit.
    inline static const auto external_13     = BitRange<InterruptMask>{29}; ///< Defines External interrupt 13 mask bit.
    inline static const auto external_12     = BitRange<InterruptMask>{28}; ///< Defines External interrupt 12 mask bit.
    inline static const auto external_11     = BitRange<InterruptMask>{27}; ///< Defines External interrupt 11 mask bit.
    inline static const auto external_10     = BitRange<InterruptMask>{26}; ///< Defines External interrupt 10 mask bit.
    inline static const auto external_09     = BitRange<InterruptMask>{25}; ///< Defines External interrupt 09 mask bit.
    inline static const auto external_08     = BitRange<InterruptMask>{24}; ///< Defines External interrupt 08 mask bit.
    inline static const auto external_07     = BitRange<InterruptMask>{23}; ///< Defines External interrupt 07 mask bit.
    inline static const auto external_06     = BitRange<InterruptMask>{22}; ///< Defines External interrupt 06 mask bit.
    inline static const auto external_05     = BitRange<InterruptMask>{21}; ///< Defines External interrupt 05 mask bit.
    inline static const auto external_04     = BitRange<InterruptMask>{20}; ///< Defines External interrupt 04 mask bit.
    inline static const auto external_03     = BitRange<InterruptMask>{19}; ///< Defines External interrupt 03 mask bit.
    inline static const auto external_02     = BitRange<InterruptMask>{18}; ///< Defines External interrupt 02 mask bit.
    inline static const auto external_01     = BitRange<InterruptMask>{17}; ///< Defines External interrupt 01 mask bit.
    inline static const auto external_00     = BitRange<InterruptMask>{16}; ///< Defines External interrupt 00 mask bit.
    inline static const auto a_bus           = BitRange<InterruptMask>{15}; ///< Defines A-Bus interrupt mask bit.
    inline static const auto sprite_draw_end = BitRange<InterruptMask>{13}; ///< Defines Sprite Draw End interrupt mask bit.
    inline static const auto dma_illegal     = BitRange<InterruptMask>{12}; ///< Defines DMA Illegal interrupt mask bit.
    inline static const auto level_0_dma     = BitRange<InterruptMask>{11}; ///< Defines Level 0 DMA interrupt mask bit.
    inline static const auto level_1_dma     = BitRange<InterruptMask>{10}; ///< Defines Level 1 DMA interrupt mask bit.
    inline static const auto level_2_dma     = BitRange<InterruptMask>{9};  ///< Defines Level 2 DMA interrupt mask bit.
    inline static const auto pad             = BitRange<InterruptMask>{8};  ///< Defines PAD interrupt mask bit.
    inline static const auto system_manager  = BitRange<InterruptMask>{7};  ///< Defines System Manager interrupt mask bit.
    inline static const auto sound_request   = BitRange<InterruptMask>{6};  ///< Defines Sound Request interrupt mask bit.
    inline static const auto dsp_end         = BitRange<InterruptMask>{5};  ///< Defines DSP End interrupt mask bit.
    inline static const auto timer_1         = BitRange<InterruptMask>{4};  ///< Defines Timer 1 interrupt mask bit.
    inline static const auto timer_0         = BitRange<InterruptMask>{3};  ///< Defines Timer 0 interrupt mask bit.
    inline static const auto h_blank_in      = BitRange<InterruptMask>{2};  ///< Defines H-Blank-In interrupt mask bit.
    inline static const auto v_blank_out     = BitRange<InterruptMask>{1};  ///< Defines V-Blank-Out interrupt mask bit.
    inline static const auto v_blank_in      = BitRange<InterruptMask>{0};  ///< Defines V-Blank-In interrupt mask bit.
    inline static const auto undefined       = BitRange<InterruptMask>{0};  ///< Undefined value.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   InterruptEnable
///
/// \brief  Values that represent if an interrupt is enabled or not.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class InterruptEnable : u8 {
    disabled = 0, ///< Interrupt is disabled
    enabled  = 1  ///< Interrupt is enabled
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  InterruptStatusRegister
///
/// \brief  Interrupt Status Register (IST).
///
/// \author Runik
/// \date   05/02/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class InterruptStatusRegister : public Register {
  public:
    using Register::Register;

    inline static const auto external_15     = BitRange<InterruptEnable>{31}; ///< Defines External interrupt 15 status.
    inline static const auto external_14     = BitRange<InterruptEnable>{30}; ///< Defines External interrupt 14 status.
    inline static const auto external_13     = BitRange<InterruptEnable>{29}; ///< Defines External interrupt 13 status.
    inline static const auto external_12     = BitRange<InterruptEnable>{28}; ///< Defines External interrupt 12 status.
    inline static const auto external_11     = BitRange<InterruptEnable>{27}; ///< Defines External interrupt 11 status.
    inline static const auto external_10     = BitRange<InterruptEnable>{26}; ///< Defines External interrupt 10 status.
    inline static const auto external_09     = BitRange<InterruptEnable>{25}; ///< Defines External interrupt 09 status.
    inline static const auto external_08     = BitRange<InterruptEnable>{24}; ///< Defines External interrupt 08 status.
    inline static const auto external_07     = BitRange<InterruptEnable>{23}; ///< Defines External interrupt 07 status.
    inline static const auto external_06     = BitRange<InterruptEnable>{22}; ///< Defines External interrupt 06 status.
    inline static const auto external_05     = BitRange<InterruptEnable>{21}; ///< Defines External interrupt 05 status.
    inline static const auto external_04     = BitRange<InterruptEnable>{20}; ///< Defines External interrupt 04 status.
    inline static const auto external_03     = BitRange<InterruptEnable>{19}; ///< Defines External interrupt 03 status.
    inline static const auto external_02     = BitRange<InterruptEnable>{18}; ///< Defines External interrupt 02 status.
    inline static const auto external_01     = BitRange<InterruptEnable>{17}; ///< Defines External interrupt 01 status.
    inline static const auto external_00     = BitRange<InterruptEnable>{16}; ///< Defines External interrupt 00 status.
    inline static const auto a_bus           = BitRange<InterruptEnable>{15}; ///< Defines A-Bus interrupt status.
    inline static const auto sprite_draw_end = BitRange<InterruptEnable>{13}; ///< Defines Sprite Draw End interrupt status.
    inline static const auto dma_illegal     = BitRange<InterruptEnable>{12}; ///< Defines DMA Illegal interrupt status.
    inline static const auto level_0_dma     = BitRange<InterruptEnable>{11}; ///< Defines Level 0 DMA interrupt status.
    inline static const auto level_1_dma     = BitRange<InterruptEnable>{10}; ///< Defines Level 1 DMA interrupt status.
    inline static const auto level_2_dma     = BitRange<InterruptEnable>{9};  ///< Defines Level 2 DMA interrupt status.
    inline static const auto pad             = BitRange<InterruptEnable>{8};  ///< Defines PAD interrupt status.
    inline static const auto system_manager  = BitRange<InterruptEnable>{7};  ///< Defines System Manager interrupt status.
    inline static const auto sound_request   = BitRange<InterruptEnable>{6};  ///< Defines Sound Request interrupt status.
    inline static const auto dsp_end         = BitRange<InterruptEnable>{5};  ///< Defines DSP End interrupt status.
    inline static const auto timer_1         = BitRange<InterruptEnable>{4};  ///< Defines Timer 1 interrupt status.
    inline static const auto timer_0         = BitRange<InterruptEnable>{3};  ///< Defines Timer 0 interrupt status.
    inline static const auto h_blank_in      = BitRange<InterruptEnable>{2};  ///< Defines H-Blank-In interrupt status.
    inline static const auto v_blank_out     = BitRange<InterruptEnable>{1};  ///< Defines V-Blank-Out interrupt status.
    inline static const auto v_blank_in      = BitRange<InterruptEnable>{0};  ///< Defines V-Blank-In interrupt status.
    inline static const auto undefined       = BitRange<InterruptEnable>{0};  ///< Undefined value.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  Timer0CompareRegister
///
/// \brief  Timer 0 compare register (T0C).
///
/// \author Runik
/// \date   07/06/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class Timer0CompareRegister : public Register {
  public:
    using Register::Register;
    inline static const auto timer_0_compare_data = BitRange<u32>{0, 9}; ///< Defines timer 0 compare data bits.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  Timer1SetDataRegister
///
/// \brief  Timer 1 Set Data Register (T1S).
///
/// \author Runik
/// \date   07/06/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class Timer1SetDataRegister : public Register {
  public:
    using Register::Register;
    inline static const auto timer_1_set_data = BitRange<u32>{0, 8}; ///< Defines timer 1 set data bits.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   Timer1Mode
///
/// \brief  T1MD bit value.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Timer1Mode : u8 {
    interrupt_occurs_each_line     = 0, ///< Interrupt occurs at each line.
    interrupt_occurs_only_at_timer = 1  ///< Interrupt occurs only at lines indicated by Timer 0.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   TimerEnable
///
/// \brief  TENB bit value.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class TimerEnable : u8 {
    timer_operation_off = 0, ///< Timer operation off.
    timer_operation_on  = 1  ///< Timer operation on.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  Timer1ModeRegister
///
/// \brief  Timer 1 Mode Register (T1MD).
///
/// \author Runik
/// \date   07/06/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class Timer1ModeRegister : public Register {
  public:
    using Register::Register;
    inline static const auto timer_1_mode = BitRange<Timer1Mode>{8};  ///< Defines T1MD bit.
    inline static const auto timer_enable = BitRange<TimerEnable>{0}; ///< Defines TENB bit.
};

} // namespace saturnin::core
