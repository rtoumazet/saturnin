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
#include <saturnin/src/bitfield.h>

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
/// \union  DmaReadAddressRegister
///
/// \brief  DMA Level 2-0 Read Address Register (D0R, D1R &amp; D2R).
///
/// \author Runik
/// \date   14/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union DmaReadAddressRegister {
    u32             raw;          ///< Raw representation.
    BitField<0, 27> read_address; ///< Defines read address in bytes.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union DmaWriteAddressRegister
///
/// \brief  DMA Level 2-0 Write Address Register (D0W, D1W &amp; D2W).
///
/// \author Runik
/// \date   14/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union DmaWriteAddressRegister {
    u32             raw;           ///< Raw representation.
    BitField<0, 27> write_address; ///< Defines write address in bytes.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union  DmaLevel0TransferByteNumberRegister
///
/// \brief  DMA Level 0 Transfer Byte Number (D0C).
///
/// \author Runik
/// \date   14/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union DmaLevel0TransferByteNumberRegister {
    u32             raw;                  ///< Raw representation.
    BitField<0, 20> transfer_byte_number; ///< Defines transfer byte number.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union  DmaLevel1TransferByteNumberRegister
///
/// \brief  DMA Level 1 Transfer Byte Number (D1C).
///
/// \author Runik
/// \date   14/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union DmaLevel1TransferByteNumberRegister {
    u32             raw;                  ///< Raw representation.
    BitField<0, 12> transfer_byte_number; ///< Defines transfer byte number.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union  DmaLevel2TransferByteNumberRegister
///
/// \brief  DMA Level 2 Transfer Byte Number (D2C).
///
/// \author Runik
/// \date   14/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union DmaLevel2TransferByteNumberRegister {
    u32             raw;                  ///< Raw representation.
    BitField<0, 12> transfer_byte_number; ///< Defines transfer byte number.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   ReadAddressAddValue
///
/// \brief  Number of bytes added to the read address.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class ReadAddressAddValue : bool {
    add_0 = 0, ///< Nothing is added.
    add_4 = 1  ///< 4 bytes are added
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
/// \union  DmaAddressAddValueRegister
///
/// \brief  DMA Address Add Value Register (D0AD, D1AD and D2AD).
///
/// \author Runik
/// \date   15/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union DmaAddressAddValueRegister {
    u32            raw;             ///< Raw representation.
    BitField<8>    read_add_value;  ///< Defines read address add value (D0RA, D1RA and D2RA).
    BitField<0, 3> write_add_value; ///< Defines write address add value (D0WA, D1WA and D2WA).
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   DmaEnable
///
/// \brief  DxEN bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class DmaEnable : bool {
    disabled = 0, ///< DMA is disabled.
    enabled  = 1  ///< DMA is enabled.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   DmaStarting
///
/// \brief  DxGO bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class DmaStarting : bool {
    not_started = 0, ///< DMA isn't started.
    started     = 1  ///< DMA starts execution. Only relevant when start factor is DMA.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union  DmaEnableRegister
///
/// \brief  DMA Enable Register (DxEN).
///
/// \author Runik
/// \date   15/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union DmaEnableRegister {
    u32         raw;          ///< Raw representation.
    BitField<8> dma_enable;   ///< Defines DMA Enable Bit (D0EN, D1EN, D2EN)
    BitField<0> dma_starting; ///< Defines DMA Starting Bit (D0GO, D1GO, D2GO).
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   DmaMode
///
/// \brief  DxMOD bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class DmaMode : bool {
    direct   = 0, ///< Direct mode.
    indirect = 1  ///< Indirect mode.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   ReadAddressUpdate
///
/// \brief  DxRUP bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class ReadAddressUpdate : bool {
    save   = 0, ///< Saves the value.
    update = 1  ///< Updates the value.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   WriteAddressUpdate
///
/// \brief  DxWUP bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class WriteAddressUpdate : bool {
    save   = 0, ///< Saves the value.
    update = 1  ///< Updates the value.
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
/// \union  DmaEnableRegister
///
/// \brief  DMA Mode, Address Update, Start Factor Select Register (DxMD).
///
/// \author Runik
/// \date   15/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union DmaModeRegister {
    u32            raw;                    ///< Raw representation.
    BitField<24>   dma_mode;               ///< Defines DxMOD bit.
    BitField<16>   read_address_update;    ///< Defines DxRUP bit.
    BitField<8>    write_address_update;   ///< Defines DxWUP bit.
    BitField<0, 3> starting_factor_select; ///< Defines DxFTy bits.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   DspBusAccess
///
/// \brief  DACSD bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class DspBusAccess : bool {
    not_accessing = 0, ///< Not accessing the DSP Bus.
    accessing     = 1  ///< Accessing the DSP Bus.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   BBusAccess
///
/// \brief  DACSB bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class BBusAccess : bool {
    not_accessing = 0, ///< Not accessing the B Bus.
    accessing     = 1  ///< Accessing the B Bus.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   ABusAccess
///
/// \brief  DACSA bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class ABusAccess : bool {
    not_accessing = 0, ///< Not accessing the A Bus.
    accessing     = 1  ///< Accessing the A Bus.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   Level1DmaInterrupt
///
/// \brief  D1BK bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Level1DmaInterrupt : bool {
    not_interrupted = 0, ///< Level 1 DMA isn't interrupted.
    interrupted     = 1  ///< Level 1 DMA is interrupted.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   Level0DmaInterrupt
///
/// \brief  D0BK bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Level0DmaInterrupt : bool {
    not_interrupted = 0, ///< Level 0 DMA isn't interrupted.
    interrupted     = 1  ///< Level 0 DMA is interrupted.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   Level2DmaStandBy
///
/// \brief  D2WT bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Level2DmaStandBy : bool {
    not_on_standby = 0, ///< Level 2 DMA isn't on standby.
    on_standby     = 1  ///< Level 2 DMA is on standby.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   Level2DmaOperation
///
/// \brief  D2MV bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Level2DmaOperation : bool {
    not_in_operation = 0, ///< Level 2 DMA isn't in operation.
    in_operation     = 1  ///< Level 2 DMA is in operation.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   Level1DmaStandBy
///
/// \brief  D1WT bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Level1DmaStandBy : bool {
    not_on_standby = 0, ///< Level 1 DMA isn't on standby.
    on_standby     = 1  ///< Level 1 DMA is on standby.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   Level1DmaOperation
///
/// \brief  D1MV bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Level1DmaOperation : bool {
    not_in_operation = 0, ///< Level 1 DMA isn't in operation.
    in_operation     = 1  ///< Level 1 DMA is in operation.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   Level0DmaStandBy
///
/// \brief  D0WT bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Level0DmaStandBy : bool {
    not_on_standby = 0, ///< Level 0 DMA isn't on standby.
    on_standby     = 1  ///< Level 0 DMA is on standby.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   Level0DmaOperation
///
/// \brief  D0MV bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Level0DmaOperation : bool {
    not_in_operation = 0, ///< Level 0 DMA isn't in operation.
    in_operation     = 1  ///< Level 0 DMA is in operation.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   DspDmaStandBy
///
/// \brief  DDWT bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class DspDmaStandBy : bool {
    not_on_standby = 0, ///< DSP DMA isn't on standby.
    on_standby     = 1  ///< DSP DMA is on standby.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   DspDmaOperation
///
/// \brief  DDMV bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class DspDmaOperation : bool {
    not_in_operation = 0, ///< DSP DMA isn't in operation.
    in_operation     = 1  ///< DSP DMA is in operation.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union  DmaEnableRegister
///
/// \brief  DMA Status Register (DSTA).
///
/// \author Runik
/// \date   16/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union DmaStatusRegister {
    u32          raw;                   ///< Raw representation.
    BitField<22> dsp_bus_access;        ///< Defines DACSD bit.
    BitField<21> b_bus_access;          ///< Defines DACSB bit.
    BitField<20> a_bus_access;          ///< Defines DACSA bit.
    BitField<17> level_1_dma_interrupt; ///< Defines D1BK bit.
    BitField<16> level_0_dma_interrupt; ///< Defines D0BK bit.
    BitField<13> level_2_dma_stand_by;  ///< Defines D2WT bit.
    BitField<12> level_2_dma_operation; ///< Defines D2MV bit.
    BitField<9>  level_1_dma_stand_by;  ///< Defines D1WT bit.
    BitField<8>  level_1_dma_operation; ///< Defines D1MV bit.
    BitField<5>  level_0_dma_stand_by;  ///< Defines D0WT bit.
    BitField<4>  level_0_dma_operation; ///< Defines D0MV bit.
    BitField<1>  dsp_dma_stand_by;      ///< Defines DDWT bit.
    BitField<0>  dsp_dma_operation;     ///< Defines DDMV bit.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   InterruptMask
///
/// \brief  Values that represent interrupt masks.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class InterruptMask : bool {
    not_masked = 0, ///< Interupt is not masked
    masked     = 1  ///< Interrupt is masked
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  InterruptMaskRegister
///
/// \brief  Interrupt Mask Register (IMS).
///
/// \author Runik
/// \date   16/01/2022
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
    inline static const auto undefined       = BitRange<InterruptMask>{14}; ///< Undefined value.
};

// union InterruptMaskRegister {
//     u32          raw;             ///< Raw representation.
//     BitField<31> external_15;     ///< Defines External interrupt 15 mask bit.
//     BitField<30> external_14;     ///< Defines External interrupt 14 mask bit.
//     BitField<29> external_13;     ///< Defines External interrupt 13 mask bit.
//     BitField<28> external_12;     ///< Defines External interrupt 12 mask bit.
//     BitField<27> external_11;     ///< Defines External interrupt 11 mask bit.
//     BitField<26> external_10;     ///< Defines External interrupt 10 mask bit.
//     BitField<25> external_09;     ///< Defines External interrupt 09 mask bit.
//     BitField<24> external_08;     ///< Defines External interrupt 08 mask bit.
//     BitField<23> external_07;     ///< Defines External interrupt 07 mask bit.
//     BitField<22> external_06;     ///< Defines External interrupt 06 mask bit.
//     BitField<21> external_05;     ///< Defines External interrupt 05 mask bit.
//     BitField<20> external_04;     ///< Defines External interrupt 04 mask bit.
//     BitField<19> external_03;     ///< Defines External interrupt 03 mask bit.
//     BitField<18> external_02;     ///< Defines External interrupt 02 mask bit.
//     BitField<17> external_01;     ///< Defines External interrupt 01 mask bit.
//     BitField<16> external_00;     ///< Defines External interrupt 00 mask bit.
//     BitField<15> a_bus;           ///< Defines A-Bus interrupt mask bit.
//     BitField<13> sprite_draw_end; ///< Defines Sprite Draw End interrupt mask bit.
//     BitField<12> dma_illegal;     ///< Defines DMA Illegal interrupt mask bit.
//     BitField<11> level_0_dma;     ///< Defines Level 0 DMA interrupt mask bit.
//     BitField<10> level_1_dma;     ///< Defines Level 1 DMA interrupt mask bit.
//     BitField<9>  level_2_dma;     ///< Defines Level 2 DMA interrupt mask bit.
//     BitField<8>  pad;             ///< Defines PAD interrupt mask bit.
//     BitField<7>  system_manager;  ///< Defines System Manager interrupt mask bit.
//     BitField<6>  sound_request;   ///< Defines Sound Request interrupt mask bit.
//     BitField<5>  dsp_end;         ///< Defines DSP End interrupt mask bit.
//     BitField<4>  timer_1;         ///< Defines Timer 1 interrupt mask bit.
//     BitField<3>  timer_0;         ///< Defines Timer 0 interrupt mask bit.
//     BitField<2>  h_blank_in;      ///< Defines H-Blank-In interrupt mask bit.
//     BitField<1>  v_blank_out;     ///< Defines V-Blank-Out interrupt mask bit.
//     BitField<0>  v_blank_in;      ///< Defines V-Blank-In interrupt mask bit.
//     BitField<14> undefined;       ///< Undefined value.
// };

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
/// \union  Timer0CompareRegister
///
/// \brief  Timer 0 compare register (T0C).
///
/// \author Runik
/// \date   16/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union Timer0CompareRegister {
    u32             raw;                  ///< Raw representation.
    BitField<0, 10> timer_0_compare_data; ///< Defines timer 0 compare data bits.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union  Timer1SetDataRegister
///
/// \brief  Timer 1 Set Data Register (T1S).
///
/// \author Runik
/// \date   16/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union Timer1SetDataRegister {
    u32            raw;                  ///< Raw representation.
    BitField<0, 9> timer_0_compare_data; ///< Defines timer 1 set data bits.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   Timer1Mode
///
/// \brief  T1MD bit value.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Timer1Mode : bool {
    interrupt_occurs_each_line     = 0, ///< Interrupt occurs at each line.
    interrupt_occurs_only_at_timer = 1  ///< Interrupt occurs only at lines indicated by Timer 0.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   TimerEnable
///
/// \brief  TENB bit value.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class TimerEnable : bool {
    timer_operation_off = 0, ///< Timer operation off.
    timer_operation_on  = 1  ///< Timer operation on.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union  Timer1ModeRegister
///
/// \brief  Timer 1 Mode Register (T1MD).
///
/// \author Runik
/// \date   16/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union Timer1ModeRegister {
    u32         raw;          ///< Raw representation.
    BitField<8> timer_1_mode; ///< Defines T1MD bit.
    BitField<0> timer_enable; ///< Defines TENB bit.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   ExecutePauseReset
///
/// \brief  PR bit value.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class ExecutePauseReset : bool {
    no_change              = 0, ///< No condition change.
    program_pause_is_reset = 1  ///< Program pause is reset when program execution begins.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   ExecutePause
///
/// \brief  EP bit value.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class ExecutePause : bool {
    no_change      = 0, ///< No condition change.
    program_pauses = 1  ///< Program pauses when program execution begins.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   D0BusDmaExecution
///
/// \brief  T0 bit value.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class D0BusDmaExecution : bool {
    no_dma           = 0, ///< No DMA is executing.
    dma_is_executing = 1  ///< Executing DMA using the D0-Bus.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   Sign
///
/// \brief  S bit value.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Sign : bool {
    operation_result_is_not_negative = 0, ///< Operation result isn't negative.
    operation_result_is_negative     = 1  ///< Operation result is negative.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   Zero
///
/// \brief  Z bit value.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Zero : bool {
    operation_result_is_not_zero = 0, ///< Operation result isn't 0.
    operation_result_is_zero     = 1  ///< Operation result is 0.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   Carry
///
/// \brief  C bit value.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Carry : bool {
    operation_result_has_no_carry = 0, ///< Carry does not occurs in operation result.
    operation_result_has_carry    = 1  ///< Carry occurs in operation result.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   Overflow
///
/// \brief  V bit value.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Overflow : bool {
    operation_does_not_cause_overflow = 0, ///< Operation result does not cause overflow or underflow.
    operation_causes_overflow         = 1  ///< Operation result causes overflow or underflow.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   ProgramEndInterrupt
///
/// \brief  E bit value.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class ProgramEndInterrupt : bool {
    program_not_ended_by_endi = 0, ///< Program not ended by ENDI command.
    program_ended_by_endi     = 1  ///< Program ended by ENDI command detected.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   StepExecuteControl
///
/// \brief  ES bit value.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class StepExecuteControl : bool {
    program_does_not_execute_one_step = 0, ///< Program does not execute one step.
    program_executes_one_step         = 1  ///< Program executes one step if program is paused.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   ProgramExecuteControl
///
/// \brief  EX bit value.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class ProgramExecuteControl : bool {
    program_execution_stops  = 0, ///< Program execution stops when writing 0.
    program_execution_begins = 1  ///< Program execution begins when writing 1.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   ProgramCounterTransferEnable
///
/// \brief  LE bit value.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class ProgramCounterTransferEnable : bool {
    program_ram_address_not_loaded_to_program_counter = 0, ///< The program RAM address is not loaded to the program counter.
    program_ram_address_loaded_to_program_counter     = 1  ///< The program RAM address is loaded to the program counter.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union  DspProgramControlPort
///
/// \brief  DSP Program Control Port (PPAF).
///
/// \author Runik
/// \date   16/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

// class DspProgramControlPort : public Register {
//   public:
//     using Register::Register;
//     inline static const auto execute_pause_reset             = BitRange<ExecutePauseReset>{26};            ///< Defines PP bit.
//     inline static const auto execute_pause                   = BitRange<ExecutePause>{25};                 ///< Defines EP bit.
//     inline static const auto d0_bus_dma_execution            = BitRange<D0BusDmaExecution>{23};            ///< Defines T0 bit.
//     inline static const auto sign                            = BitRange<Sign>{22};                         ///< Defines S bit.
//     inline static const auto zero                            = BitRange<Zero>{21};                         ///< Defines Z bit.
//     inline static const auto carry                           = BitRange<Carry>{20};                        ///< Defines C bit.
//     inline static const auto overflow                        = BitRange<Overflow>{19};                     ///< Defines V bit.
//     inline static const auto program_end_interrupt           = BitRange<ProgramEndInterrupt>{18};          ///< Defines E bit.
//     inline static const auto step_execute_control            = BitRange<StepExecuteControl>{17};           ///< Defines ES bit.
//     inline static const auto program_execute_control         = BitRange<ProgramExecuteControl>{16};        ///< Defines EX bit.
//     inline static const auto program_counter_transfer_enable = BitRange<ProgramCounterTransferEnable>{15}; ///< Defines LE bit.
//     inline static const auto program_ram_address             = BitRange<u8>{0, 7};                         ///< Defines P7-0
//     bits.
// };

union DspProgramControlPort {
    u32            raw;                             ///< Raw representation.
    BitField<26>   execute_pause_reset;             ///< Defines PP bit.
    BitField<25>   execute_pause;                   ///< Defines EP bit.
    BitField<23>   d0_bus_dma_execution;            ///< Defines T0 bit.
    BitField<22>   sign;                            ///< Defines S bit.
    BitField<21>   zero;                            ///< Defines Z bit.
    BitField<20>   carry;                           ///< Defines C bit.
    BitField<19>   overflow;                        ///< Defines V bit.
    BitField<18>   program_end_interrupt;           ///< Defines E bit.
    BitField<17>   step_execute_control;            ///< Defines ES bit.
    BitField<16>   program_execute_control;         ///< Defines EX bit.
    BitField<15>   program_counter_transfer_enable; ///< Defines LE bit.
    BitField<0, 8> program_ram_address;             ///< Defines P7-0 bits.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union  DspProgramDataPort
///
/// \brief  DSP Program Data Port (PPD).
///
/// \author Runik
/// \date   16/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

// class DspProgramDataPort : public Register {
//   public:
//     using Register::Register;
//     inline static const auto dsp_program_data_port = BitRange<u32>{0, 31}; ///< Defines DSP program data port.
// };

union DspProgramDataPort {
    u32 raw; ///< Defines DSP program data port.
};

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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union  DspDataRamAddressPort
///
/// \brief  DSP Data RAM Address Port (PPA).
///
/// \author Runik
/// \date   16/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

// class DspDataRamAddressPort : public Register {
//   public:
//     using Register::Register;
//     inline static const auto data_ram_select  = BitRange<DataRamSelect>{6, 7}; ///< Defines RA7-6.
//     inline static const auto data_ram_address = BitRange<u8>{0, 5};            ///< Defines RA5-0.
// };

union DspDataRamAddressPort {
    u32            raw;              ///< Raw representation.
    BitField<6, 2> data_ram_select;  ///< Defines RA7-6 bits.
    BitField<0, 6> data_ram_address; ///< Defines RA5-0 bits.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union DspDataRamDataPort
///
/// \brief  DSP Data RAM Data Port (PDD).
///
/// \author Runik
/// \date   16/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

// class DspDataRamDataPort : public Register {
//   public:
//     using Register::Register;
//     inline static const auto dsp_data_ram_data_port = BitRange<u32>{0, 31}; ///< Defines DSP data ram port.
// };

union DspDataRamDataPort {
    u32 raw; ///< Defines DSP data ram port.
};

} // namespace saturnin::core
