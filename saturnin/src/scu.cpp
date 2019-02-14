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
#include "scu_registers.h"
#include "emulator_context.h"
#include "memory.h"
#include "interrupt_sources.h"

namespace is = saturnin::core::interrupt_source;

namespace saturnin {
namespace core {

Scu::Scu(Emulator_context* ec) : emulator_context_(ec) {
    initializeRegisters();
};

auto Scu::scuMemory() const {
    return emulator_context_->memory()->scu_;
}

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
            return rawRead<u32>(scuMemory(), addr & scu_memory_mask);
    }

    return rawRead<u32>(scuMemory(), addr & scu_memory_mask);

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
            if (DmaEnableRegister(data).get(DmaEnableRegister::dmaEnable) == DmaEnable::enabled) {
                auto dma_0_config = configureDmaTransfer(DmaLevel::level_0);
                executeDma(dma_0_config);
            }
            break;
        }
        case level_1_dma_enable_register: {
            if (DmaEnableRegister(data).get(DmaEnableRegister::dmaEnable) == DmaEnable::enabled) {
                auto dma_1_config = configureDmaTransfer(DmaLevel::level_1);
                executeDma(dma_1_config);
            }
            break;
        }
        case level_2_dma_enable_register: {
            if (DmaEnableRegister(data).get(DmaEnableRegister::dmaEnable) == DmaEnable::enabled) {
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
    rawWrite<u32>(scuMemory(), addr & scu_memory_mask, data);
};

void Scu::executeDma(const DmaConfiguration& dc) {

}

void Scu::setInterruptStatusRegister(const Interrupt& i) {
    auto isr = InterruptStatusRegister(rawRead<u32>(scuMemory(), interrupt_status_register & scu_memory_mask));

    //switch (i) {
    //    case is::v_blank_in.vector:            isr.set(InterruptStatusRegister::vBlankInInterruptStatus);      break;
    //    case is::v_blank_out.vector:           isr.set(InterruptStatusRegister::vBlankOutInterruptStatus);     break;
    //    case is::h_blank_in.vector:            isr.set(InterruptStatusRegister::hBlankInInterruptStatus);      break;
    //    case is::timer_0.vector:               isr.set(InterruptStatusRegister::timer0InterruptStatus);        break;
    //    case is::timer_1.vector:               isr.set(InterruptStatusRegister::timer1InterruptStatus);        break;
    //    case is::dsp_end.vector:               isr.set(InterruptStatusRegister::dspEndInterruptStatus);        break;
    //    case is::sound_request.vector:         isr.set(InterruptStatusRegister::soundRequestInterruptStatus);  break;
    //    case is::system_manager.vector:        isr.set(InterruptStatusRegister::systemManagerInterruptStatus); break;
    //    case is::pad_interrupt.vector:         isr.set(InterruptStatusRegister::padInterruptStatus);           break;
    //    case is::level_2_dma_end.vector:       isr.set(InterruptStatusRegister::level2DmaInterruptStatus);     break;
    //    case is::level_1_dma_end.vector:       isr.set(InterruptStatusRegister::level1DmaInterruptStatus);     break;
    //    case is::level_0_dma_end.vector:       isr.set(InterruptStatusRegister::level0DmaInterruptStatus);     break;
    //    case is::dma_illegal.vector:           isr.set(InterruptStatusRegister::dmaIllegalInterruptStatus);    break;
    //    case is::sprite_draw_end.vector:       isr.set(InterruptStatusRegister::spriteDrawEndInterruptStatus); break;
    //    case is::external_interrupt_00.vector: isr.set(InterruptStatusRegister::externalInterrupt00Status);    break;
    //    case is::external_interrupt_01.vector: isr.set(InterruptStatusRegister::externalInterrupt01Status);    break;
    //    case is::external_interrupt_02.vector: isr.set(InterruptStatusRegister::externalInterrupt02Status);    break;
    //    case is::external_interrupt_03.vector: isr.set(InterruptStatusRegister::externalInterrupt03Status);    break;
    //    case is::external_interrupt_04.vector: isr.set(InterruptStatusRegister::externalInterrupt04Status);    break;
    //    case is::external_interrupt_05.vector: isr.set(InterruptStatusRegister::externalInterrupt05Status);    break;
    //    case is::external_interrupt_06.vector: isr.set(InterruptStatusRegister::externalInterrupt06Status);    break;
    //    case is::external_interrupt_07.vector: isr.set(InterruptStatusRegister::externalInterrupt07Status);    break;
    //    case is::external_interrupt_08.vector: isr.set(InterruptStatusRegister::externalInterrupt08Status);    break;
    //    case is::external_interrupt_09.vector: isr.set(InterruptStatusRegister::externalInterrupt09Status);    break;
    //    case is::external_interrupt_10.vector: isr.set(InterruptStatusRegister::externalInterrupt10Status);    break;
    //    case is::external_interrupt_11.vector: isr.set(InterruptStatusRegister::externalInterrupt11Status);    break;
    //    case is::external_interrupt_12.vector: isr.set(InterruptStatusRegister::externalInterrupt12Status);    break;
    //    case is::external_interrupt_13.vector: isr.set(InterruptStatusRegister::externalInterrupt13Status);    break;
    //    case is::external_interrupt_14.vector: isr.set(InterruptStatusRegister::externalInterrupt14Status);    break;
    //    case is::external_interrupt_15.vector: isr.set(InterruptStatusRegister::externalInterrupt15Status);    break;
    //    default: break;
    //}

    rawWrite<u32>(scuMemory(), interrupt_status_register & scu_memory_mask, isr.toUlong());
};

void Scu::resetInterruptStatusRegister(const Interrupt& i) {
    auto isr = InterruptStatusRegister(rawRead<u32>(scuMemory(), interrupt_status_register & scu_memory_mask));

    //switch (i) {
    //    case is::v_blank_in.vector:            isr.reset(InterruptStatusRegister::vBlankInInterruptStatus);      break;
    //    case is::v_blank_out.vector:           isr.reset(InterruptStatusRegister::vBlankOutInterruptStatus);     break;
    //    case is::h_blank_in.vector:            isr.reset(InterruptStatusRegister::hBlankInInterruptStatus);      break;
    //    case is::timer_0.vector:               isr.reset(InterruptStatusRegister::timer0InterruptStatus);        break;
    //    case is::timer_1.vector:               isr.reset(InterruptStatusRegister::timer1InterruptStatus);        break;
    //    case is::dsp_end.vector:               isr.reset(InterruptStatusRegister::dspEndInterruptStatus);        break;
    //    case is::sound_request.vector:         isr.reset(InterruptStatusRegister::soundRequestInterruptStatus);  break;
    //    case is::system_manager.vector:        isr.reset(InterruptStatusRegister::systemManagerInterruptStatus); break;
    //    case is::pad_interrupt.vector:         isr.reset(InterruptStatusRegister::padInterruptStatus);           break;
    //    case is::level_2_dma_end.vector:       isr.reset(InterruptStatusRegister::level2DmaInterruptStatus);     break;
    //    case is::level_1_dma_end.vector:       isr.reset(InterruptStatusRegister::level1DmaInterruptStatus);     break;
    //    case is::level_0_dma_end.vector:       isr.reset(InterruptStatusRegister::level0DmaInterruptStatus);     break;
    //    case is::dma_illegal.vector:           isr.reset(InterruptStatusRegister::dmaIllegalInterruptStatus);    break;
    //    case is::sprite_draw_end.vector:       isr.reset(InterruptStatusRegister::spriteDrawEndInterruptStatus); break;
    //    case is::external_interrupt_00.vector: isr.reset(InterruptStatusRegister::externalInterrupt00Status);    break;
    //    case is::external_interrupt_01.vector: isr.reset(InterruptStatusRegister::externalInterrupt01Status);    break;
    //    case is::external_interrupt_02.vector: isr.reset(InterruptStatusRegister::externalInterrupt02Status);    break;
    //    case is::external_interrupt_03.vector: isr.reset(InterruptStatusRegister::externalInterrupt03Status);    break;
    //    case is::external_interrupt_04.vector: isr.reset(InterruptStatusRegister::externalInterrupt04Status);    break;
    //    case is::external_interrupt_05.vector: isr.reset(InterruptStatusRegister::externalInterrupt05Status);    break;
    //    case is::external_interrupt_06.vector: isr.reset(InterruptStatusRegister::externalInterrupt06Status);    break;
    //    case is::external_interrupt_07.vector: isr.reset(InterruptStatusRegister::externalInterrupt07Status);    break;
    //    case is::external_interrupt_08.vector: isr.reset(InterruptStatusRegister::externalInterrupt08Status);    break;
    //    case is::external_interrupt_09.vector: isr.reset(InterruptStatusRegister::externalInterrupt09Status);    break;
    //    case is::external_interrupt_10.vector: isr.reset(InterruptStatusRegister::externalInterrupt10Status);    break;
    //    case is::external_interrupt_11.vector: isr.reset(InterruptStatusRegister::externalInterrupt11Status);    break;
    //    case is::external_interrupt_12.vector: isr.reset(InterruptStatusRegister::externalInterrupt12Status);    break;
    //    case is::external_interrupt_13.vector: isr.reset(InterruptStatusRegister::externalInterrupt13Status);    break;
    //    case is::external_interrupt_14.vector: isr.reset(InterruptStatusRegister::externalInterrupt14Status);    break;
    //    case is::external_interrupt_15.vector: isr.reset(InterruptStatusRegister::externalInterrupt15Status);    break;
    //    default: break;
    //}

    rawWrite<u32>(scuMemory(), interrupt_status_register & scu_memory_mask, isr.toUlong());
};

bool Scu::isInterruptMasked(const Interrupt& i, bool sentFromMasterSh2) const {
    auto imr = InterruptMaskRegister(rawRead<u32>(scuMemory(), interrupt_mask_register & scu_memory_mask));
    if (sentFromMasterSh2) {
        imr.get(i.mask);
        
        switch (i) {
            case is::vector_nmi:
            
                return true;
                break;
            default:
                //if(i.mask == InterruptMaskRegister::undefined)
                //i.mask
                break;
                
        }

    }
    else {
        // Slave SH2
        switch (i) {
            case is::vector_nmi:
            case is::vector_v_blank_in:
            case is::vector_h_blank_in:
            case is::vector_frt_input_capture:
                return true;
                break;
            default:
                return false;
        }
    }

    
}

void Scu::initializeRegisters() {
    // DMA
    rawWrite<u32>(scuMemory(), level_0_dma_add_value_register & scu_memory_mask, 0x00000101);
    rawWrite<u32>(scuMemory(), level_0_dma_enable_register    & scu_memory_mask, 0x00000000);
    rawWrite<u32>(scuMemory(), level_0_dma_mode_register      & scu_memory_mask, 0x00000007);
    rawWrite<u32>(scuMemory(), level_1_dma_add_value_register & scu_memory_mask, 0x00000101);
    rawWrite<u32>(scuMemory(), level_1_dma_enable_register    & scu_memory_mask, 0x00000000);
    rawWrite<u32>(scuMemory(), level_1_dma_mode_register      & scu_memory_mask, 0x00000007);
    rawWrite<u32>(scuMemory(), level_2_dma_add_value_register & scu_memory_mask, 0x00000101);
    rawWrite<u32>(scuMemory(), level_2_dma_enable_register    & scu_memory_mask, 0x00000000);
    rawWrite<u32>(scuMemory(), level_2_dma_mode_register      & scu_memory_mask, 0x00000007);
    rawWrite<u32>(scuMemory(), dma_forced_stop                & scu_memory_mask, 0x00000000);
    rawWrite<u32>(scuMemory(), (dma_status_register + 0x0C)   & scu_memory_mask, 0x00000000);

    // DSP
    rawWrite<u32>(scuMemory(), dsp_program_control_port  & scu_memory_mask, 0x00000000);
    rawWrite<u32>(scuMemory(), dsp_data_ram_address_port & scu_memory_mask, 0x00000000);
    rawWrite<u32>(scuMemory(), dsp_data_ram_data_port    & scu_memory_mask, 0x00000000);

    // Timer
    rawWrite<u32>(scuMemory(), timer_1_mode_register & scu_memory_mask, 0x00000000);

    // Interrupt control
    rawWrite<u32>(scuMemory(), interrupt_mask_register   & scu_memory_mask, 0x0000BFFF);
    rawWrite<u32>(scuMemory(), interrupt_status_register & scu_memory_mask, 0x00000000);

    // A-BUS control
    rawWrite<u32>(scuMemory(), a_bus_interrupt_acknowledge & scu_memory_mask, 0x00000000);
    rawWrite<u32>(scuMemory(), a_bus_set_register          & scu_memory_mask, 0x00000000);
    rawWrite<u32>(scuMemory(), (a_bus_set_register + 4)    & scu_memory_mask, 0x00000000);
    rawWrite<u32>(scuMemory(), a_bus_refresh_register      & scu_memory_mask, 0x00000000);

    // SCU control
    rawWrite<u32>(scuMemory(), scu_sdram_select_register & scu_memory_mask, 0x00000000);
    rawWrite<u32>(scuMemory(), scu_version_register      & scu_memory_mask, 0x00000000);

    //d0en = 0;
    //d1en = 0;
    //d2en = 0;
    //dsta = 0;
}

DmaConfiguration Scu::configureDmaTransfer(DmaLevel level) const {
    DmaConfiguration dc{};
    switch (level) {
        case DmaLevel::level_0:
            initializeDmaReadAddress(dc, level_0_dma_read_address);
            initializeDmaWriteAddress(dc, level_0_dma_write_address);
            initializeDmaTransferByteNumber(dc, level_0_dma_transfer_byte_number);
            initializeDmaAddressAdd(dc, level_0_dma_add_value_register);
            initializeDmaEnable(dc, level_0_dma_enable_register);
            initializeDmaMode(dc, level_0_dma_mode_register);
            break;
        case DmaLevel::level_1:
            initializeDmaReadAddress(dc, level_1_dma_read_address);
            initializeDmaWriteAddress(dc, level_1_dma_write_address);
            initializeDmaTransferByteNumber(dc, level_1_dma_transfer_byte_number);
            initializeDmaAddressAdd(dc, level_1_dma_add_value_register);
            initializeDmaEnable(dc, level_1_dma_enable_register);
            initializeDmaMode(dc, level_1_dma_mode_register);
            break;
        case DmaLevel::level_2:
            initializeDmaReadAddress(dc, level_2_dma_read_address);
            initializeDmaWriteAddress(dc, level_2_dma_write_address);
            initializeDmaTransferByteNumber(dc, level_2_dma_transfer_byte_number);
            initializeDmaAddressAdd(dc, level_2_dma_add_value_register);
            initializeDmaEnable(dc, level_2_dma_enable_register);
            initializeDmaMode(dc, level_2_dma_mode_register);
            break;
        default:
            break;
    }
    return dc;
}

void Scu::initializeDmaMode(DmaConfiguration& dc, const u32 register_address) const {
    auto dmr                  = DmaModeRegister(rawRead<u32>(scuMemory(), register_address & scu_memory_mask));
    dc.dma_mode               = dmr.get(DmaModeRegister::dmaMode);
    dc.read_address_update    = dmr.get(DmaModeRegister::readAddressUpdate);
    dc.write_address_update   = dmr.get(DmaModeRegister::writeAddressUpdate);
    dc.starting_factor_select = dmr.get(DmaModeRegister::startingFactorSelect);
}

void Scu::initializeDmaEnable(DmaConfiguration& dc, const u32 register_address) const {
    auto der        = DmaEnableRegister(rawRead<u32>(scuMemory(), register_address & scu_memory_mask));
    dc.dma_enable   = der.get(DmaEnableRegister::dmaEnable);
    dc.dma_starting = der.get(DmaEnableRegister::dmaStarting);
}

void Scu::initializeDmaAddressAdd(DmaConfiguration& dc, const u32 register_address) const {
    auto avr           = DmaAddressAddValueRegister(rawRead<u32>(scuMemory(), register_address & scu_memory_mask));
    dc.read_add_value  = avr.get(DmaAddressAddValueRegister::readAddValue);
    dc.write_add_value = avr.get(DmaAddressAddValueRegister::writeAddValue);
}

void Scu::initializeDmaTransferByteNumber(DmaConfiguration& dc, const u32 register_address) const {
    switch (register_address & scu_memory_mask) {
        case (level_0_dma_transfer_byte_number & 0xFF): {
            auto tbnr               = DmaLevel2TransferByteNumberRegister(rawRead<u32>(scuMemory(), level_0_dma_transfer_byte_number & scu_memory_mask));
            dc.transfer_byte_number = tbnr.get(DmaLevel2TransferByteNumberRegister::transferByteNumber);
            break;
        }
        case (level_1_dma_transfer_byte_number & 0xFF): {
            auto tbnr               = DmaLevel2TransferByteNumberRegister(rawRead<u32>(scuMemory(), level_1_dma_transfer_byte_number & scu_memory_mask));
            dc.transfer_byte_number = tbnr.get(DmaLevel2TransferByteNumberRegister::transferByteNumber);
            break;
        }
        case (level_2_dma_transfer_byte_number & 0xFF): {
            auto tbnr               = DmaLevel2TransferByteNumberRegister(rawRead<u32>(scuMemory(), level_2_dma_transfer_byte_number & scu_memory_mask));
            dc.transfer_byte_number = tbnr.get(DmaLevel2TransferByteNumberRegister::transferByteNumber);
            break;
        }
        default:
            Log::error("scu", "Unknown DMA Transfer Byte Number register address ! {}", register_address);
            break;
    }
}
void Scu::initializeDmaWriteAddress(DmaConfiguration& dc, const u32 register_address) const {
    auto war         = DmaWriteAddressRegister(rawRead<u32>(scuMemory(), register_address & scu_memory_mask));
    dc.write_address = war.get(DmaWriteAddressRegister::writeAddress);
}

void Scu::initializeDmaReadAddress(DmaConfiguration& dc, const u32 register_address) const {
    auto rar        = DmaReadAddressRegister(rawRead<u32>(scuMemory(), register_address & scu_memory_mask));
    dc.read_address = rar.get(DmaReadAddressRegister::readAddress);
}

}
}