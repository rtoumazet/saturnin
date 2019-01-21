// 
// scu.cpp
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

#include "scu.h"
#include "utilities.h" // toUnderlying

namespace util = saturnin::utilities;

namespace saturnin {
namespace core {

Scu::Scu(Memory* m) : memory_(m) {
    initializeRegisters();
};

void Scu::initializeRegisters() {
    // DMA
    rawWrite<u32>(memory_->scu_, (level_0_dma_set_register + 0x0C) & 0xFF, 0x00000101);
    rawWrite<u32>(memory_->scu_, (level_0_dma_set_register + 0x10) & 0xFF, 0x00000000);
    rawWrite<u32>(memory_->scu_, (level_0_dma_set_register + 0x14) & 0xFF, 0x00000007);
    rawWrite<u32>(memory_->scu_, (level_1_dma_set_register + 0x2C) & 0xFF, 0x00000101);
    rawWrite<u32>(memory_->scu_, (level_1_dma_set_register + 0x30) & 0xFF, 0x00000000);
    rawWrite<u32>(memory_->scu_, (level_1_dma_set_register + 0x34) & 0xFF, 0x00000007);
    rawWrite<u32>(memory_->scu_, (level_2_dma_set_register + 0x4C) & 0xFF, 0x00000101);
    rawWrite<u32>(memory_->scu_, (level_2_dma_set_register + 0x50) & 0xFF, 0x00000000);
    rawWrite<u32>(memory_->scu_, (level_2_dma_set_register + 0x54) & 0xFF, 0x00000007);
    rawWrite<u32>(memory_->scu_, dma_forced_stop & 0xFF, 0x00000000);
    rawWrite<u32>(memory_->scu_, (dma_status_register + 0x0C) & 0xFF, 0x00000000);
    
    // DSP
    rawWrite<u32>(memory_->scu_, dsp_program_control_port & 0xFF,  0x00000000);
    rawWrite<u32>(memory_->scu_, dsp_data_ram_address_port & 0xFF, 0x00000000);
    rawWrite<u32>(memory_->scu_, dsp_data_ram_data_port & 0xFF,    0x00000000);
    
    // Timer
    rawWrite<u32>(memory_->scu_, timer_1_mode_register & 0xFF, 0x00000000);

    // Interrupt control
    rawWrite<u32>(memory_->scu_, interrupt_mask_register & 0xFF,   0x0000BFFF);
    rawWrite<u32>(memory_->scu_, interrupt_status_register & 0xFF, 0x00000000);

    // A-BUS control
    rawWrite<u32>(memory_->scu_, a_bus_interrupt_acknowledge & 0xFF, 0x00000000);
    rawWrite<u32>(memory_->scu_, a_bus_set_register & 0xFF,          0x00000000);
    rawWrite<u32>(memory_->scu_, (a_bus_set_register + 4) & 0xFF,    0x00000000);
    rawWrite<u32>(memory_->scu_, a_bus_refresh_register & 0xFF,      0x00000000);
    
    // SCU control
    rawWrite<u32>(memory_->scu_, scu_sdram_select_register & 0xFF, 0x00000000);
    rawWrite<u32>(memory_->scu_, scu_version_register & 0xFF,      0x00000000);

    //d0en = 0;
    //d1en = 0;
    //d2en = 0;
    //dsta = 0;
}

}
}