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
constexpr u32 dma_forced_stop                  = 0x25FE0060;
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
        
        u32 read32(const u32 addr) const;

        void write32(const u32 addr, const u32 data);

    private:
        void initializeRegisters();

        Memory* memory_;
};

}
}