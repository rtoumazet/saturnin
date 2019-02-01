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
#include "memory.h"

namespace util = saturnin::utilities;

namespace saturnin {
namespace core {

Scu::Scu(Memory* m) : memory_(m) {
    initializeRegisters();
};

u32 Scu::read32(const u32 addr) const {
    switch (addr) {
        case dsp_program_control_port:
            Log::debug("scu", "DSP - Program Control Port read");
            //data = current_DSP_program_control_port_;
            break;
        case dsp_program_ram_dataport:
            Log::debug("scu", "DSP - Program Dataport read");
            //data = current_DSP_program_dataport_;
            break;
        case dsp_data_ram_address_port:
            Log::debug("scu", "DSP - Data RAM Address Port read");
            //data = current_DSP_data_ram_address_port_;
            break;
        case dsp_data_ram_data_port:
            Log::debug("scu", "DSP - Data RAM Dataport read");
            //data = current_DSP_data_ram_dataport_;
            break;
        default:
            return rawRead<u32>(memory_->scu_, addr & 0xFF);
    }
    
    return rawRead<u32>(memory_->scu_, addr & 0xFF);

};

void Scu::write32(const u32 addr, const u32 data) {
//    uint8_t index = 0;
//    uint32_t temp = 0;
//
    switch (addr) {
//        case dsp_program_control_port:
//            current_DSP_program_control_port_ = data;
//            if (current_DSP_program_control_port_&DSP_T0) {
//                // D0 Bus DMA execution
//
//            }
//            if (current_DSP_program_control_port_&DSP_EX) {
//#ifdef _DEBUGEMU
//                std::ostringstream oss;
//                oss << "DSP program execution started at address : " << showbase << hex << static_cast<uint32_t>((current_DSP_program_control_port_ & DSP_P));
//                EmuState::pLog->ScuDsp(oss.str());
//#endif
//                PC_ = static_cast<uint8_t>((current_DSP_program_control_port_ & DSP_P));
//                DSPProgramDisasm(PC_);
//                current_DSP_program_control_port_ &= ~DSP_EX; // DSP interception
//                SetISTFlag(0x45); // DSP interrupt flag
//            }
//            break;
//        case dsp_program_ram_dataport:
//            index = static_cast<uint8_t>((current_DSP_program_control_port_ & DSP_P));
//
//            RawWrite32(pProgramRam_, index, data);
//            index++;
//            current_DSP_program_control_port_ &= 0xFFFFFF00;
//            current_DSP_program_control_port_ |= static_cast<uint8_t>(index);
//
//            current_DSP_program_dataport_ = data;
//            break;
//        case dsp_data_ram_address_port:
//            current_DSP_data_ram_address_port_ = static_cast<uint8_t>(data);
//            break;
//        case dsp_data_ram_data_port:
//            current_DSP_data_ram_dataport_ = data;
//            break;
        case level_0_dma_enable_register: {
            auto der = DmaEnableRegister(data);
            if (der.dmaEnable() == DmaEnable::enabled) {
                auto dma_0_config = configureDmaTransfer(DmaLevel::level_0);
                executeDma(dma_0_config);
            }
            break;
        }
        case level_1_dma_enable_register: {
            auto der = DmaEnableRegister(data);
            if (der.dmaEnable() == DmaEnable::enabled) {
                auto dma_1_config = configureDmaTransfer(DmaLevel::level_1);
                executeDma(dma_1_config);
            }
            break;
        }
        case level_2_dma_enable_register: {
            auto der = DmaEnableRegister(data);
            if (der.dmaEnable() == DmaEnable::enabled) {
                auto dma_2_config = configureDmaTransfer(DmaLevel::level_2);
                executeDma(dma_2_config);
            }
            break;
        }
//        case dma_status_register + 0x0C:
//            // DMA registers write
//            dmaUpdate_ = true;
//            break;
        default:

            break;
    }

    //if(address==0x25FE00A0) DebugBreak();
    rawWrite<u32>(memory_->scu_, addr & 0xFF, data);
};

void Scu::executeDma(const DmaConfiguration& dc) {

}


void Scu::initializeRegisters() {
    // DMA
    rawWrite<u32>(memory_->scu_, level_0_dma_add_value_register & 0xFF, 0x00000101);
    rawWrite<u32>(memory_->scu_, level_0_dma_enable_register & 0xFF,    0x00000000);
    rawWrite<u32>(memory_->scu_, level_0_dma_mode_register & 0xFF,      0x00000007);
    rawWrite<u32>(memory_->scu_, level_1_dma_add_value_register & 0xFF, 0x00000101);
    rawWrite<u32>(memory_->scu_, level_1_dma_enable_register & 0xFF,    0x00000000);
    rawWrite<u32>(memory_->scu_, level_1_dma_mode_register & 0xFF,      0x00000007);
    rawWrite<u32>(memory_->scu_, level_2_dma_add_value_register & 0xFF, 0x00000101);
    rawWrite<u32>(memory_->scu_, level_2_dma_enable_register & 0xFF,    0x00000000);
    rawWrite<u32>(memory_->scu_, level_2_dma_mode_register & 0xFF,      0x00000007);
    rawWrite<u32>(memory_->scu_, dma_forced_stop & 0xFF,                0x00000000);
    rawWrite<u32>(memory_->scu_, (dma_status_register + 0x0C) & 0xFF,   0x00000000);
    
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

DmaConfiguration Scu::configureDmaTransfer(DmaLevel level) const {
    DmaConfiguration dc{};
    switch (level) {
        case DmaLevel::level_0:
            initializeDmaReadAddress(       dc, level_0_dma_read_address);
            initializeDmaWriteAddress(      dc, level_0_dma_write_address);
            initializeDmaTransferByteNumber(dc, level_0_dma_transfer_byte_number);
            initializeDmaAddressAdd(        dc, level_0_dma_add_value_register);
            initializeDmaEnable(            dc, level_0_dma_enable_register);
            initializeDmaMode(              dc, level_0_dma_mode_register);
            break;
        case DmaLevel::level_1: 
            initializeDmaReadAddress(       dc, level_1_dma_read_address);
            initializeDmaWriteAddress(      dc, level_1_dma_write_address);
            initializeDmaTransferByteNumber(dc, level_1_dma_transfer_byte_number);
            initializeDmaAddressAdd(        dc, level_1_dma_add_value_register);
            initializeDmaEnable(            dc, level_1_dma_enable_register);
            initializeDmaMode(              dc, level_1_dma_mode_register);
            break;
        case DmaLevel::level_2: 
            initializeDmaReadAddress(       dc, level_2_dma_read_address);
            initializeDmaWriteAddress(      dc, level_2_dma_write_address);
            initializeDmaTransferByteNumber(dc, level_2_dma_transfer_byte_number);
            initializeDmaAddressAdd(        dc, level_2_dma_add_value_register);
            initializeDmaEnable(            dc, level_2_dma_enable_register);
            initializeDmaMode(              dc, level_2_dma_mode_register);
            break;
        default:
            break;
    }
    return dc;
}

void Scu::initializeDmaMode(DmaConfiguration& dc, const u32 register_address) const {
    auto dmr                  = DmaModeRegister(rawRead<u32>(memory_->scu_, register_address));
    dc.dma_mode               = dmr.dmaMode();
    dc.read_address_update    = dmr.readAddressUpdate();
    dc.write_address_update   = dmr.writeAddressUpdate();
    dc.starting_factor_select = dmr.startingFactorSelect();
}

void Scu::initializeDmaEnable(DmaConfiguration& dc, const u32 register_address) const {
    auto der        = DmaEnableRegister(rawRead<u32>(memory_->scu_, register_address));
    dc.dma_enable   = der.dmaEnable();
    dc.dma_starting = der.dmaStarting();
}

void Scu::initializeDmaAddressAdd(DmaConfiguration& dc, const u32 register_address) const {
    auto avr           = DmaAddressAddValueRegister(rawRead<u32>(memory_->scu_, register_address));
    dc.read_add_value  = avr.readAddValue();
    dc.write_add_value = avr.writeAddValue();
}

void Scu::initializeDmaTransferByteNumber(DmaConfiguration& dc, const u32 register_address) const {
    switch (register_address & 0xFF) {
        case (level_0_dma_transfer_byte_number & 0xFF): {
            auto tbnr = DmaLevel2TransferByteNumberRegister(rawRead<u32>(memory_->scu_, level_0_dma_transfer_byte_number));
            dc.transfer_byte_number = tbnr.transferByteNumber();
            break;
        }
        case (level_1_dma_transfer_byte_number & 0xFF): {
            auto tbnr = DmaLevel2TransferByteNumberRegister(rawRead<u32>(memory_->scu_, level_1_dma_transfer_byte_number));
            dc.transfer_byte_number = tbnr.transferByteNumber();
            break;
        }
        case (level_2_dma_transfer_byte_number & 0xFF): {
            auto tbnr               = DmaLevel2TransferByteNumberRegister(rawRead<u32>(memory_->scu_, level_2_dma_transfer_byte_number));
            dc.transfer_byte_number = tbnr.transferByteNumber();
            break;
        }
        default:
            Log::error("scu", "Unknown DMA Transfer Byte Number register address ! {}", register_address);
            break;
    }
}
void Scu::initializeDmaWriteAddress(DmaConfiguration& dc, const u32 register_address) const {
    auto war = DmaWriteAddressRegister(rawRead<u32>(memory_->scu_, register_address));
    dc.write_address = war.writeAddress();
}

void Scu::initializeDmaReadAddress(DmaConfiguration& dc, const u32 register_address) const {
    auto rar = DmaReadAddressRegister(rawRead<u32>(memory_->scu_, register_address));
    dc.read_address = rar.readAddress();
}

}
}