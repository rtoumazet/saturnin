// 
// scu_registers.h
// Saturnin
//
// Copyright (c) 2018-2019 Renaud Toumazet
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

#include "emulator_defs.h"

namespace saturnin {
namespace core {

/// \name SCU registers addresses
//@{
constexpr u32 level_0_dma_read_address         = 0x25FE0000;
constexpr u32 level_0_dma_write_address        = 0x25FE0004;
constexpr u32 level_0_dma_transfer_byte_number = 0x25FE0008;
constexpr u32 level_0_dma_add_value_register   = 0x25FE000C;
constexpr u32 level_0_dma_enable_register      = 0x25FE0010;
constexpr u32 level_0_dma_mode_register        = 0x25FE0014;
constexpr u32 level_1_dma_read_address         = 0x25FE0020;
constexpr u32 level_1_dma_write_address        = 0x25FE0024;
constexpr u32 level_1_dma_transfer_byte_number = 0x25FE0028;
constexpr u32 level_1_dma_add_value_register   = 0x25FE002C;
constexpr u32 level_1_dma_enable_register      = 0x25FE0030;
constexpr u32 level_1_dma_mode_register        = 0x25FE0034;
constexpr u32 level_2_dma_read_address         = 0x25FE0040;
constexpr u32 level_2_dma_write_address        = 0x25FE0044;
constexpr u32 level_2_dma_transfer_byte_number = 0x25FE0048;
constexpr u32 level_2_dma_add_value_register   = 0x25FE004C;
constexpr u32 level_2_dma_enable_register      = 0x25FE0050;
constexpr u32 level_2_dma_mode_register        = 0x25FE0054;
constexpr u32 dma_forced_stop                  = 0x25FE0060; // not used anymore, according to SCU Final Specifications (No 14)
constexpr u32 dma_status_register              = 0x25FE0070;
constexpr u32 dsp_program_control_port         = 0x25FE0080;
constexpr u32 dsp_program_ram_dataport         = 0x25FE0084;
constexpr u32 dsp_data_ram_address_port        = 0x25FE0088;
constexpr u32 dsp_data_ram_data_port           = 0x25FE008C;
constexpr u32 timer_0_compare_register         = 0x25FE0090;
constexpr u32 timer_1_set_data_register        = 0x25FE0094;
constexpr u32 timer_1_mode_register            = 0x25FE0098;
constexpr u32 interrupt_mask_register          = 0x25FE00A0;
constexpr u32 interrupt_status_register        = 0x25FE00A4;
constexpr u32 a_bus_interrupt_acknowledge      = 0x25FE00A8;
constexpr u32 a_bus_set_register               = 0x25FE00B0;
constexpr u32 a_bus_refresh_register           = 0x25FE00B8;
constexpr u32 scu_sdram_select_register        = 0x25FE00C4;
constexpr u32 scu_version_register             = 0x25FE00C8;
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
        inline static const BitRange<u32> read_address{ 0, 26 };   ///< Defines read address in bytes.
        inline static const BitRange<u32> all_bits{ 0, 31 };  ///< Defines the whole register bits
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
        inline static const BitRange<u32> write_address{ 0, 26 };   ///< Defines write address in bytes.
        inline static const BitRange<u32> all_bits{ 0, 31 };  ///< Defines the whole register bits
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
        inline static const BitRange<u32> transfer_byte_number{ 0, 19 };   ///< Defines transfer byte number.
        inline static const BitRange<u32> all_bits{ 0, 31 };  ///< Defines the whole register bits
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
        inline static const BitRange<u32> transfer_byte_number{ 0, 11 };   ///< Defines transfer byte number.
        inline static const BitRange<u32> all_bits{ 0, 31 };  ///< Defines the whole register bits
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
        inline static const BitRange<u32> transfer_byte_number{ 0, 11 };   ///< Defines transfer byte number.
        inline static const BitRange<u32> all_bits{ 0, 31 };  ///< Defines the whole register bits
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
        inline static const BitRange<ReadAddressAddValue>  read_add_value { 8 };    ///< Defines read address add value (D0RA, D1RA and D2RA).
        inline static const BitRange<WriteAddressAddValue> write_add_value{ 0, 2 }; ///< Defines write address add value (D0WA, D1WA and D2WA).
        inline static const BitRange<u32> all_bits{ 0, 31 };  ///< Defines the whole register bits
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   DmaEnable
///
/// \brief  DxEN bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class DmaEnable :u8 {
    disabled = 0b0,  ///< DMA is disabled.
    enabled  = 0b1   ///< DMA is enabled.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   DmaStarting
///
/// \brief  DxGO bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class DmaStarting : u8 {
    not_started = 0b0,   ///< DMA isn't started.
    started     = 0b1    ///< DMA starts execution. Only relevant when start factor is DMA.
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
        inline static const BitRange<DmaEnable>   dma_enable{ 8 };   ///< Defines DMA Enable Bit (D0EN, D1EN, D2EN)
        inline static const BitRange<DmaStarting> dma_starting{ 0 }; ///< Defines DMA Starting Bit (D0GO, D1GO, D2GO).
        inline static const BitRange<u32> all_bits{ 0, 31 };  ///< Defines the whole register bits
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   DmaMode
///
/// \brief  DxMOD bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class DmaMode : u8 {
    direct   = 0b0,  ///< Direct mode.
    indirect = 0b1   ///< Indirect mode.
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
    v_blank_in       = 0b000,  ///< V-bLANK-IN signal receive and enable bit setting.
    v_blank_out      = 0b001,  ///< V-bLANK-OUT signal receive and enable bit setting.
    h_blank_in       = 0b010,  ///< H-bLANK-IN signal receive and enable bit setting.
    timer_0          = 0b011,  ///< Timer 0 signal receive and enable bit setting.
    timer_1          = 0b100,  ///< Timer 1 signal receive and enable bit setting.
    sound_req        = 0b101,  ///< Sound Req signal receive and enable bit setting.
    sprite_draw_end  = 0b110,  ///< Sprite draw end signal receive and enable bit setting.
    dma_start_factor = 0b111,  ///< DMA starting factor setting and enable bit setting.
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
        inline static const BitRange<DmaMode>              dma_mode             { 24 };   ///< Defines DxMOD bit.
        inline static const BitRange<ReadAddressUpdate>    read_address_update   { 16 };   ///< Defines DxRUP bit.
        inline static const BitRange<WriteAddressUpdate>   write_address_update  { 8 };    ///< Defines DxWUP bit.
        inline static const BitRange<StartingFactorSelect> starting_factor_select{ 0, 2 }; ///< Defines DxFTy bits.
        inline static const BitRange<u32> all_bits{ 0, 31 };  ///< Defines the whole register bits
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
        inline static const BitRange<DspBusAccess>       dsp_bus_access      { 22 }; ///< Defines DACSD bit. 
        inline static const BitRange<BBusAccess>         b_bus_access        { 21 }; ///< Defines DACSB bit. 
        inline static const BitRange<ABusAccess>         a_bus_access        { 20 }; ///< Defines DACSA bit. 
        inline static const BitRange<Level1DmaInterrupt> level_1_dma_interrupt{ 17 }; ///< Defines D1BK bit. 
        inline static const BitRange<Level0DmaInterrupt> level_0_dma_interrupt{ 16 }; ///< Defines D0BK bit. 
        inline static const BitRange<Level2DmaStandBy>   level_2_dma_stand_by  { 13 }; ///< Defines D2WT bit. 
        inline static const BitRange<Level2DmaOperation> level_2_dma_operation{ 12 }; ///< Defines D2MV bit. 
        inline static const BitRange<Level1DmaStandBy>   level_1_dma_stand_by  { 9 };  ///< Defines D1WT bit. 
        inline static const BitRange<Level1DmaOperation> level_1_dma_operation{ 8 };  ///< Defines D1MV bit. 
        inline static const BitRange<Level0DmaStandBy>   level_0_dma_stand_by  { 5 };  ///< Defines D0WT bit. 
        inline static const BitRange<Level0DmaOperation> level_0_dma_operation{ 4 };  ///< Defines D0MV bit. 
        inline static const BitRange<DspDmaStandBy>      dsp_dma_stand_by     { 1 };  ///< Defines DDWT bit. 
        inline static const BitRange<DspDmaOperation>    dsp_dma_operation   { 0 };  ///< Defines DDMV bit. 
        inline static const BitRange<u32> all_bits{ 0, 31 };  ///< Defines the whole register bits
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
    
        inline static const BitRange<InterruptMask> external_15   { 31 }; ///< Defines External interrupt 15 mask bit.
        inline static const BitRange<InterruptMask> external_14   { 30 }; ///< Defines External interrupt 14 mask bit.
        inline static const BitRange<InterruptMask> external_13   { 29 }; ///< Defines External interrupt 13 mask bit.
        inline static const BitRange<InterruptMask> external_12   { 28 }; ///< Defines External interrupt 12 mask bit.
        inline static const BitRange<InterruptMask> external_11   { 27 }; ///< Defines External interrupt 11 mask bit.
        inline static const BitRange<InterruptMask> external_10   { 26 }; ///< Defines External interrupt 10 mask bit.
        inline static const BitRange<InterruptMask> external_09   { 25 }; ///< Defines External interrupt 09 mask bit.
        inline static const BitRange<InterruptMask> external_08   { 24 }; ///< Defines External interrupt 08 mask bit.
        inline static const BitRange<InterruptMask> external_07   { 23 }; ///< Defines External interrupt 07 mask bit.
        inline static const BitRange<InterruptMask> external_06   { 22 }; ///< Defines External interrupt 06 mask bit.
        inline static const BitRange<InterruptMask> external_05   { 21 }; ///< Defines External interrupt 05 mask bit.
        inline static const BitRange<InterruptMask> external_04   { 20 }; ///< Defines External interrupt 04 mask bit.
        inline static const BitRange<InterruptMask> external_03   { 19 }; ///< Defines External interrupt 03 mask bit.
        inline static const BitRange<InterruptMask> external_02   { 18 }; ///< Defines External interrupt 02 mask bit.
        inline static const BitRange<InterruptMask> external_01   { 17 }; ///< Defines External interrupt 01 mask bit.
        inline static const BitRange<InterruptMask> external_00   { 16 }; ///< Defines External interrupt 00 mask bit.
        inline static const BitRange<InterruptMask> a_bus         { 15 }; ///< Defines A-Bus interrupt mask bit.
        inline static const BitRange<InterruptMask> sprite_draw_end{ 13 }; ///< Defines Sprite Draw End interrupt mask bit.
        inline static const BitRange<InterruptMask> dma_illegal   { 12 }; ///< Defines DMA Illegal interrupt mask bit.
        inline static const BitRange<InterruptMask> level_0_dma    { 11 }; ///< Defines Level 0 DMA interrupt mask bit.
        inline static const BitRange<InterruptMask> level_1_dma    { 10 }; ///< Defines Level 1 DMA interrupt mask bit.
        inline static const BitRange<InterruptMask> level_2_dma    { 9 };  ///< Defines Level 2 DMA interrupt mask bit.
        inline static const BitRange<InterruptMask> pad          { 8 };  ///< Defines PAD interrupt mask bit.
        inline static const BitRange<InterruptMask> system_manager{ 7 };  ///< Defines System Manager interrupt mask bit.
        inline static const BitRange<InterruptMask> sound_request { 6 };  ///< Defines Sound Request interrupt mask bit.
        inline static const BitRange<InterruptMask> dsp_end       { 5 };  ///< Defines DSP End interrupt mask bit.
        inline static const BitRange<InterruptMask> timer_1       { 4 };  ///< Defines Timer 1 interrupt mask bit.
        inline static const BitRange<InterruptMask> timer_0       { 3 };  ///< Defines Timer 0 interrupt mask bit.
        inline static const BitRange<InterruptMask> h_blank_in     { 2 };  ///< Defines H-Blank-In interrupt mask bit.
        inline static const BitRange<InterruptMask> v_blank_out    { 1 };  ///< Defines V-Blank-Out interrupt mask bit. 
        inline static const BitRange<InterruptMask> v_blank_in     { 0 };  ///< Defines V-Blank-In interrupt mask bit.
        inline static const BitRange<InterruptMask> undefined    { 0 }; ///< Undefined value.
        inline static const BitRange<u32>             all_bits{ 0, 31 };  ///< Defines the whole register bits

};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   InterruptEnable
///
/// \brief  Values that represent if an interrupt is enabled or not.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class InterruptEnable : u8 {
    disabled = 0,   ///< Interrupt is disabled
    enabled  = 1    ///< Interrupt is enabled
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

        inline static const BitRange<InterruptEnable> external_15   { 31 }; ///< Defines External interrupt 15 status.
        inline static const BitRange<InterruptEnable> external_14   { 30 }; ///< Defines External interrupt 14 status.
        inline static const BitRange<InterruptEnable> external_13   { 29 }; ///< Defines External interrupt 13 status.
        inline static const BitRange<InterruptEnable> external_12   { 28 }; ///< Defines External interrupt 12 status.
        inline static const BitRange<InterruptEnable> external_11   { 27 }; ///< Defines External interrupt 11 status.
        inline static const BitRange<InterruptEnable> external_10   { 26 }; ///< Defines External interrupt 10 status.
        inline static const BitRange<InterruptEnable> external_09   { 25 }; ///< Defines External interrupt 09 status.
        inline static const BitRange<InterruptEnable> external_08   { 24 }; ///< Defines External interrupt 08 status.
        inline static const BitRange<InterruptEnable> external_07   { 23 }; ///< Defines External interrupt 07 status.
        inline static const BitRange<InterruptEnable> external_06   { 22 }; ///< Defines External interrupt 06 status.
        inline static const BitRange<InterruptEnable> external_05   { 21 }; ///< Defines External interrupt 05 status.
        inline static const BitRange<InterruptEnable> external_04   { 20 }; ///< Defines External interrupt 04 status.
        inline static const BitRange<InterruptEnable> external_03   { 19 }; ///< Defines External interrupt 03 status.
        inline static const BitRange<InterruptEnable> external_02   { 18 }; ///< Defines External interrupt 02 status.
        inline static const BitRange<InterruptEnable> external_01   { 17 }; ///< Defines External interrupt 01 status.
        inline static const BitRange<InterruptEnable> external_00   { 16 }; ///< Defines External interrupt 00 status.
        inline static const BitRange<InterruptEnable> a_bus         { 15 }; ///< Defines A-Bus interrupt status. 
        inline static const BitRange<InterruptEnable> sprite_draw_end{ 13 }; ///< Defines Sprite Draw End interrupt status. 
        inline static const BitRange<InterruptEnable> dma_illegal   { 12 }; ///< Defines DMA Illegal interrupt status. 
        inline static const BitRange<InterruptEnable> level_0_dma    { 11};  ///< Defines Level 0 DMA interrupt status. 
        inline static const BitRange<InterruptEnable> level_1_dma    { 10 }; ///< Defines Level 1 DMA interrupt status. 
        inline static const BitRange<InterruptEnable> level_2_dma    { 9 };  ///< Defines Level 2 DMA interrupt status. 
        inline static const BitRange<InterruptEnable> pad          { 8 };  ///< Defines PAD interrupt status. 
        inline static const BitRange<InterruptEnable> system_manager{ 7 };  ///< Defines System Manager interrupt status. 
        inline static const BitRange<InterruptEnable> sound_request { 6 };  ///< Defines Sound Request interrupt status. 
        inline static const BitRange<InterruptEnable> dsp_end       { 5 };  ///< Defines DSP End interrupt status. 
        inline static const BitRange<InterruptEnable> timer_1       { 4 };  ///< Defines Timer 1 interrupt status. 
        inline static const BitRange<InterruptEnable> timer_0       { 3 };  ///< Defines Timer 0 interrupt status.  
        inline static const BitRange<InterruptEnable> h_blank_in     { 2 };  ///< Defines H-Blank-In interrupt status. 
        inline static const BitRange<InterruptEnable> v_blank_out    { 1 };  ///< Defines V-Blank-Out interrupt status. 
        inline static const BitRange<InterruptEnable> v_blank_in     { 0 };  ///< Defines V-Blank-In interrupt status. 
        inline static const BitRange<InterruptEnable> undefined    { 0 };  ///< Undefined value.
        inline static const BitRange<u32>             all_bits{ 0, 31 };  ///< Defines the whole register bits.
};

}
}