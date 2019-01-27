// 
// scu.h
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
/// \file	scu.h
///
/// \brief	Declares everything related to the System Control Unit (SCU). 
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "emulator_defs.h"
#include "utilities.h" // toUnderlying

namespace saturnin {
namespace core {

// Forward declaration
class Memory;

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
/// \enum   DmaReadAddressRegisterMask
///
/// \brief  DMA Level 2-0 Read Address Register (D0R, D1R &amp; D2R) bit mask.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class DmaReadAddressRegisterMask : u32 {
    data_mask = 0x07FFFFFF ///< Mask to access register data.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   DmaWriteAddressRegisterMask
///
/// \brief  DMA Level 2-0 Write Address Register (D0W, D1W &amp; D2W) bit mask.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class DmaWriteAddressRegisterMask : u32 {
    data_mask = 0x07FFFFFF ///< Mask to access register data.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   DmaLevel0TransferByteNumberMask
///
/// \brief  DMA Level 0 Transfer Byte Number (D0C) bit mask.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class DmaLevel0TransferByteNumberMask : u32 {
    data_mask = 0x000FFFFF ///< Mask to access register data.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   DmaLevel1TransferByteNumberMask
///
/// \brief  DMA Level 1 Transfer Byte Number (D1C) bit mask.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class DmaLevel1TransferByteNumberMask : u32 {
    data_mask = 0x00000FFF ///< Mask to access register data.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   DmaLevel2TransferByteNumberMask
///
/// \brief  DMA Level 2 Transfer Byte Number (D2C) bit mask.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class DmaLevel2TransferByteNumberMask : u32 {
    data_mask = 0x00000FFF ///< Mask to access register data.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  DmaAddressAddValueRegister
///
/// \brief  DMA Address Add Value Register (D0AD, D1AD and D2AD).
///
/// \author Runik
/// \date   25/01/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class DmaAddressAddValueRegister : Register {
    
    auto readAddValue() { return static_cast<ReadAddressAddValue>(extract(8,8)); };   ///< Returns read address add value (D0RA, D1RA and D2RA).
    auto writeAddValue() { return static_cast<WriteAddressAddValue>(extract(0,2)); }; ///< Returns write address add value mask (D0WA, D1WA and D2WA).
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   ReadAddressAddValue
///
/// \brief  Number of bytes added to the read address.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class ReadAddressAddValue : u8 {
    add_0 = 0b0000'0000, ///< Nothing is added.
    add_4 = 0b0000'0001  ///< 4 bytes are added
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   WriteAddressAddValue
///
/// \brief  Number of bytes added to the write address.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class WriteAddressAddValue : u8 {
    add_0   = 0b0000'0000, ///< Nothing is added.
    add_2   = 0b0000'0001, ///< 2 bytes are added.
    add_4   = 0b0000'0010, ///< 4 bytes are added.
    add_8   = 0b0000'0011, ///< 8 bytes are added.
    add_16  = 0b0000'0100, ///< 16 bytes are added.
    add_32  = 0b0000'0101, ///< 32 bytes are added.
    add_64  = 0b0000'0110, ///< 64 bytes are added.
    add_128 = 0b0000'0111  ///< 128 bytes are added.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  DmaEnableRegister
///
/// \brief  DMA Enable Register (DxEN).
///
/// \author Runik
/// \date   27/01/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class DmaEnableRegister : Register {
    auto dmaEnable() { return static_cast<DmaEnable>((register_value & util::toUnderlying(enable_mask)) >> 8); };
    auto dmaStarting() { return static_cast<DmaStarting>(register_value & util::toUnderlying(dma_starting_mask)); };

    private:
        const u32 enable_mask       = 0b0000'0000'0000'0000'0000'0001'0000'0000; ///< DMA Enable Bit (D0EN, D1EN, D2EN).
        const u32 dma_starting_mask = 0b0000'0000'0000'0000'0000'0000'0000'0001; ///< DMA Starting Bit (D0GO, D1GO, D2GO).
};


////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   DmaEnable
///
/// \brief  DxEN bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class DmaEnable :u8 {
    disabled = 0b0000'0000,  ///< DMA is disabled.
    enabled  = 0b0000'0001   ///< DMA is enabled.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   DmaStarting
///
/// \brief  DxGO bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class DmaStarting : u8 {
    not_started = 0b0000'0000,   ///< DMA isn't started.
    started     = 0b0000'0001    ///< DMA starts execution. Only relevant when start factor is DMA.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   DmaModeRegisterMask
///
/// \brief  DMA Mode, Address Update, Start Factor Select Register (DxMD) bit mask
////////////////////////////////////////////////////////////////////////////////////////////////////

//enum class DmaModeRegisterMask : u32 {
//    mode                 = 0b00000001000000000000000000000000,
//    read_address_update  = 0b00000000000000010000000000000000,
//    write_address_update = 0b00000000000000000000000100000000,
//    starting_factor      = 0b00000000000000000000000000000111,
//};

//class DmaModeRegister : Register {
//    auto dmaMode() { return static_cast<DmaMode>((register_value & util::toUnderlying(enable_mask)) >> 8); };
//
//    private :
//        const u32 mode_mask                 = 0b00000001000000000000000000000000;
//        const u32 read_address_update_mask  = 0b00000000000000010000000000000000;
//        const u32 write_address_update_mask = 0b00000000000000000000000100000000;
//        const u32 starting_factor_mask      = 0b00000000000000000000000000000111;
//
//};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   DmaMode
///
/// \brief  DxMOD bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class DmaMode : u32 {
    direct   = 0b00000000000000000000000000000000,  ///< Direct mode.
    indirect = 0b00000001000000000000000000000000   ///< Indirect mode.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   ReadAddressUpdate
///
/// \brief  DxRUP bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class ReadAddressUpdate : u32 {
    save   = 0b00000000000000000000000000000000,    ///< Saves the value.
    update = 0b00000000000000010000000000000000     ///< Updates the value.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   WriteAddressUpdate
///
/// \brief  DxWUP bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class WriteAddressUpdate : u32 {
    save = 0b00000000000000000000000000000000,  ///< Saves the value.
    update = 0b00000000000000000000000100000000 ///< Updates the value.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   StartingFactorSelect
///
/// \brief  DxFTy bits values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class StartingFactorSelect : u32 {
    v_blank_in       = 0b00000000000000000000000000000000,  ///< V-bLANK-IN signal receive and enable bit setting.
    v_blank_out      = 0b00000000000000000000000000000001,  ///< V-bLANK-OUT signal receive and enable bit setting.
    h_blank_in       = 0b00000000000000000000000000000010,  ///< H-bLANK-IN signal receive and enable bit setting.
    timer_0          = 0b00000000000000000000000000000011,  ///< Timer 0 signal receive and enable bit setting.
    timer_1          = 0b00000000000000000000000000000100,  ///< Timer 1 signal receive and enable bit setting.
    sound_req        = 0b00000000000000000000000000000101,  ///< Sound Req signal receive and enable bit setting.
    sprite_draw_end  = 0b00000000000000000000000000000110,  ///< Sprite draw end signal receive and enable bit setting.
    dma_start_factor = 0b00000000000000000000000000000111,  ///< DMA starting factor setting and enable bit setting.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   DmaStatusRegisterMask
///
/// \brief  DMA Status Register (DSTA) bit mask.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class DmaStatusRegisterMask : u32 {
    dsp_bus_access        = 0b00000000010000000000000000000000,    ///< DSP bus access.
    b_bus_access          = 0b00000000001000000000000000000000,    ///< B bus access.
    a_bus_access          = 0b00000000000100000000000000000000,    ///< A bus access.
    level_1_dma_interrupt = 0b00000000000000100000000000000000,    ///< Level 1 DMA interrupt.
    level_0_dma_interrupt = 0b00000000000000010000000000000000,    ///< Level 0 DMA interrupt.
    level_2_dma_stand_by  = 0b00000000000000000010000000000000,    ///< Level 2 DMA in stand by.
    level_2_dma_operation = 0b00000000000000000001000000000000,    ///< Level 2 DMA in operation.
    level_1_dma_stand_by  = 0b00000000000000000000001000000000,    ///< Level 1 DMA in stand by.
    level_1_dma_operation = 0b00000000000000000000000100000000,    ///< Level 1 DMA in operation.
    level_0_dma_stand_by  = 0b00000000000000000000000000100000,    ///< Level 0 DMA in stand by.
    level_0_dma_operation = 0b00000000000000000000000000010000,    ///< Level 0 DMA in operation.
    dsp_dma_stand_by      = 0b00000000000000000000000000000010,    ///< DSP DMA in stand by.
    dsp_dma_operation     = 0b00000000000000000000000000000001     ///< DSP DMA in operation.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   DspBusAccess
///
/// \brief  DACSD bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class DspBusAccess : u32 {
    not_accessing = 0b00000000000000000000000000000000, ///< Not accessing the DSP Bus.
    accessing     = 0b00000000010000000000000000000000  ///< Accessing the DSP Bus.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   BBusAccess
///
/// \brief  DACSB bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class BBusAccess : u32 {
    not_accessing = 0b00000000000000000000000000000000, ///< Not accessing the B Bus.
    accessing     = 0b00000000001000000000000000000000  ///< Accessing the B Bus.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   ABusAccess
///
/// \brief  DACSA bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class ABusAccess : u32 {
    not_accessing = 0b00000000000000000000000000000000, ///< Not accessing the A Bus.
    accessing     = 0b00000000000100000000000000000000  ///< Accessing the A Bus.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   Level1DmaInterrupt
///
/// \brief  D1BK bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Level1DmaInterrupt : u32 {
    not_interrupted = 0b00000000000000000000000000000000, ///< Level 1 DMA isn't interrupted.
    interrupted     = 0b00000000000000100000000000000000  ///< Level 1 DMA is interrupted.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   Level0DmaInterrupt
///
/// \brief  D0BK bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Level0DmaInterrupt : u32 {
    not_interrupted = 0b00000000000000000000000000000000, ///< Level 0 DMA isn't interrupted.
    interrupted     = 0b00000000000000010000000000000000  ///< Level 0 DMA is interrupted.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   Level2DmaStandBy
///
/// \brief  D2WT bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Level2DmaStandBy: u32 {
    not_on_standby = 0b00000000000000000000000000000000, ///< Level 2 DMA isn't on standby.
    on_standby     = 0b00000000000000000010000000000000  ///< Level 2 DMA is on standby.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   Level2DmaOperation
///
/// \brief  D2MV bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Level2DmaOperation : u32 {
    not_in_operation = 0b00000000000000000000000000000000, ///< Level 2 DMA isn't in operation.
    in_operation     = 0b00000000000000000001000000000000  ///< Level 2 DMA is in operation.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   Level2DmaStandBy
///
/// \brief  D1WT bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Level1DmaStandBy : u32 {
    not_on_standby = 0b00000000000000000000000000000000, ///< Level 1 DMA isn't on standby.
    on_standby     = 0b00000000000000000000001000000000  ///< Level 1 DMA is on standby.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   Level1DmaOperation
///
/// \brief  D1MV bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Level1DmaOperation : u32 {
    not_in_operation = 0b00000000000000000000000000000000, ///< Level 1 DMA isn't in operation.
    in_operation     = 0b00000000000000000000000100000000  ///< Level 1 DMA is in operation.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   Level0DmaStandBy
///
/// \brief  D0WT bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Level0DmaStandBy : u32 {
    not_on_standby = 0b00000000000000000000000000000000, ///< Level 0 DMA isn't on standby.
    on_standby     = 0b00000000000000000000000000100000  ///< Level 0 DMA is on standby.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   Level0DmaOperation
///
/// \brief  D0MV bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Level0DmaOperation : u32 {
    not_in_operation = 0b00000000000000000000000000000000, ///< Level 0 DMA isn't in operation.
    in_operation     = 0b00000000000000000000000000010000  ///< Level 0 DMA is in operation.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   DspDmaStandBy
///
/// \brief  DDWT bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class DspDmaStandBy : u32 {
    not_on_standby = 0b00000000000000000000000000000000, ///< DSP DMA isn't on standby.
    on_standby     = 0b00000000000000000000000000000010  ///< DSP DMA is on standby.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   DspDmaOperation
///
/// \brief  DDMV bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class DspDmaOperation : u32 {
    not_in_operation = 0b00000000000000000000000000000000, ///< DSP DMA isn't in operation.
    in_operation     = 0b00000000000000000000000000000001  ///< DSP DMA is in operation.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct DmaConfiguration
///
/// \brief  Will hold configuration data for level 0-2 DMA transfers.
///
/// \author Runik
/// \date   24/01/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

struct DmaConfiguration  {
    u32 read_address;
    u32 write_address;
    u32 transfer_byte_number;
    ReadAddressAddValue  read_add_value;
    WriteAddressAddValue write_add_value;
    DmaEnable            dma_enable;
    DmaStarting          dma_starting;
    bool is_in_direct_mode;
    bool is_read_address_updated;
    bool is_write_address_updated;

};

enum class DmaLevel {
    level_unknown,  ///< Unknown level.
    level_0,        ///< Level 0 DMA.
    level_1,        ///< Level 1 DMA.
    level_2         ///< Level 2 DMA.
};

class Scu {
    public:
        //@{
        // Constructors / Destructors
        Scu()                        = delete;
        Scu(Memory* m);
        Scu(const Scu&)              = delete;
        Scu(Scu&&)                   = delete;
        Scu& operator=(const Scu&) & = delete;
        Scu& operator=(Scu&&) &      = delete;
        ~Scu()                       = default;
        //@}

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn u32 Scu::read32(const u32 addr) const;
        ///
        /// \brief  Reads 32 bits of data from the specified address in the SCU memory space.
        ///
        /// \author Runik
        /// \date   24/01/2019
        ///
        /// \param  addr    Address to read data from.
        ///
        /// \return Data read.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        u32 read32(const u32 addr) const;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn void Scu::write32(const u32 addr, const u32 data);
        ///
        /// \brief  Writes 32 bits of data at the specified address in the SCU memory space.
        ///
        /// \author Runik
        /// \date   24/01/2019
        ///
        /// \param  addr    Address to write data to.
        /// \param  data    Data to write.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        void write32(const u32 addr, const u32 data);

        void executeDma(const DmaConfiguration& dc);

    private:

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn void Scu::initializeRegisters();
        ///
        /// \brief  Initializes the SCU registers.
        ///
        /// \author Runik
        /// \date   24/01/2019
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        void initializeRegisters();

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn DmaConfiguration Scu::configureDmaTransfer(DmaLevel dl) const;
        ///
        /// \brief  Configures the DMA transfer.
        ///
        /// \author Runik
        /// \date   24/01/2019
        ///
        /// \param  dl  DMA level to configure.
        ///
        /// \return A DmaConfiguration struct.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        DmaConfiguration configureDmaTransfer(const DmaLevel dl) const;

        Memory* memory_;    ///< Memory object
};

}
}