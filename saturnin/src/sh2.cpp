// 
// sh2.cpp
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

#include "sh2.h"
#include "interrupt_sources.h"
#include "utilities.h" // toUnderlying

namespace util = saturnin::utilities;

namespace saturnin {
namespace core {

Sh2::Sh2(bool is_master) : is_master_(is_master) {
    is_interrupted_ = false;
    pending_interrupts_.clear();

    reset();
}

u32 Sh2::readRegisters(u32 addr) const {
    switch (addr) {

        case bus_control_register1:
        case bus_control_register2:
        case wait_state_control_register:
        case individual_memory_control_register:
        case refresh_timer_control_status_register:
        case refresh_timer_counter:
        case refresh_time_constant_register:
            // Bus State Controler registers
            return (rawRead<u32>(io_registers_, addr & 0x1FF) & 0x0000FFFF);
            break;
        case dividend_register_l_32_bits:
        case dividend_register_l_shadow:
            return rawRead<u32>(io_registers_, dividend_register_l & 0x1FF);
            break;
        case dividend_register_h_shadow:
            return rawRead<u32>(io_registers_, dividend_register_h & 0x1FF);
            break;
        case dma_operation_register:
            return (rawRead<u32>(io_registers_, addr & 0x1FF) & 0x0000000F);
            break;
        default:
            return rawRead<u32>(io_registers_, addr & 0x1FF);
    }
}

void Sh2::writeRegisters(u32 addr, u8 data) {
    switch (addr) {
        /* FRT */
        case timer_interrupt_enable_register:
            if (is_master_) Log::debug("sh2", "TIER byte write (master SH2)");
            else Log::debug("sh2", "TIER byte write (slave SH2)");
            break;
        case output_compare_register_b_h:
            switch (TimerOutputCompareControlRegister(io_registers_[timer_output_compare_control_register & 0x1FF]).outputCompareRegisterSelect()) {
                case OutputCompareRegisterSelect::ocra: frt_ocra_ = (data << 8) | 0xFF; break;
                case OutputCompareRegisterSelect::ocrb: frt_ocrb_ = (data << 8) | 0xFF; break;
            }
            break;
        case output_compare_register_b_l:
            switch (TimerOutputCompareControlRegister(io_registers_[timer_output_compare_control_register & 0x1FF]).outputCompareRegisterSelect()) {
                case OutputCompareRegisterSelect::ocra: frt_ocra_ = (0xFF << 8) | data; break;
                case OutputCompareRegisterSelect::ocrb: frt_ocrb_ = (0xFF << 8) | data; break;
            }
            break;
        case timer_control_register:
            switch (TimerControlRegister(io_registers_[timer_control_register & 0x1FF]).clockSelect()) {
                case ClockSelect::internal_divided_by_8:
                    frt_clock_ = 8;
                    frt_mask_ = 0b00000111;
                    break;
                case ClockSelect::internal_divided_by_32:
                    frt_clock_ = 32;
                    frt_mask_ = 0b00011111;
                    break;
                case ClockSelect::internal_divided_by_128:
                    frt_clock_ = 128;
                    frt_mask_ = 0b01111111;
                    break;
                case ClockSelect::external:
                    Log::warning("sh2", "FRT - External clock not implemented");
                    break;
            }
            break;
        case cache_control_register:
            //Log::debug("sh2", fmt::format("CCR byte write: {}", data));
            Log::debug("sh2", "CCR byte write: {}", data);
            if (data & util::toUnderlying(CachePurge::cache_purge)) {
                purgeCache();
                data ^= util::toUnderlying(CachePurge::cache_purge); // cache purge bit is cleared after operation
            }
            break;
        default:
            rawWrite<u8>(io_registers_, addr & 0x1FF, data);
    }

    io_registers_[addr & 0x1FF] = data;

}

void Sh2::writeRegisters(u32 addr, u16 data) {
    switch (addr) {
        case timer_interrupt_enable_register:
            if (is_master_) Log::debug("sh2", "TIER word write (master SH2)");
            else Log::debug("sh2", "TIER word write (slave SH2)");
            rawWrite<u16>(io_registers_, addr & 0x1FF, data);
            break;
        case interrupt_control_register: {
            auto old_icr = InterruptControlRegister(rawRead<u16>(io_registers_, addr & 0x1FF));
            auto new_icr = InterruptControlRegister(data);
            switch (old_icr.nmiEdgeDetection()) {
                case NmiEdgeDetection::falling:
                    if ((old_icr.nmiInputLevel() == NmiInputLevel::high) && (new_icr.nmiInputLevel() == NmiInputLevel::low)) {
                        Log::error("sh2", "Falling edge NMI !");
                    }
                    break;
                case NmiEdgeDetection::rising:
                    if ((old_icr.nmiInputLevel() == NmiInputLevel::low) && (new_icr.nmiInputLevel() == NmiInputLevel::high)) {
                        Log::error("sh2", "Rising edge NMI !");
                    }
                    break;
            }
            rawWrite<u16>(io_registers_, addr & 0x1FF, data);
        }
                                         break;
        case bus_control_register1 + 2:
            rawWrite<u16>(io_registers_, addr & 0x1FF, data & 0x00F7);
            break;
        case bus_control_register2 + 2:
            rawWrite<u16>(io_registers_, addr & 0x1FF, data & 0x00FC);
            break;
        default:
            rawWrite<u16>(io_registers_, addr & 0x1FF, data);
            break;
    }
}

void Sh2::writeRegisters(u32 addr, u32 data) {
    switch (addr) {
        case timer_interrupt_enable_register:
            if (is_master_) Log::debug("sh2", "TIER long write (master SH2)");
            else Log::debug("sh2", "TIER long write (slave SH2)");
            rawWrite<u32>(io_registers_, addr & 0x1FF, data);
            break;
        case bus_control_register1:
            if ((data & 0xFFFF0000) == 0xA55A0000) {
                rawWrite<u16>(io_registers_, addr + 2 & 0x1FF, data & BusControlRegister1::writeMask());
            }
            break;
        case bus_control_register2:
            if ((data & 0xFFFF0000) == 0xA55A0000) {
                rawWrite<u16>(io_registers_, addr + 2 & 0x1FF, data & BusControlRegister2::writeMask());
            }
            break;
        case dividend_register_l_32_bits:
            rawWrite<u32>(io_registers_, addr & 0x1FF, data);

            // ST-V needs some mirroring
            rawWrite<u32>(io_registers_, dividend_register_l & 0x1FF, data);
            rawWrite<u32>(io_registers_, dividend_register_l_shadow & 0x1FF, data);

            // Sign extension for the upper 32 bits if needed
            (data & 0x80000000) ? rawWrite<u32>(io_registers_, dividend_register_h & 0x1FF, 0xFFFFFFFF)
                : rawWrite<u32>(io_registers_, dividend_register_h & 0x1FF, 0x00000000);

            start32bitsDivision();
            break;
        case dividend_register_l:
            rawWrite<u32>(io_registers_, dividend_register_l & 0x1FF, data);
            rawWrite<u32>(io_registers_, dividend_register_l_shadow & 0x1FF, data);

            start64bitsDivision();
            break;
        case dma_channel_control_register_0:
            rawWrite<u32>(io_registers_, dma_channel_control_register_0 & 0x1FF, data);

            if (DmaChannelControlRegister(data).interruptEnable() == InterruptEnable::enabled) {
                if (DmaOperationRegister(rawRead<u32>(io_registers_, dma_operation_register & 0x1FF)).dmaMasterEnable() == DmaMasterEnable::enabled) {
                    executeDma();
                }
            }
            break;
        case dma_channel_control_register_1:
            rawWrite<u32>(io_registers_, dma_channel_control_register_1 & 0x1FF, data);

            if (DmaChannelControlRegister(data).interruptEnable() == InterruptEnable::enabled) {
                if (DmaOperationRegister(rawRead<u32>(io_registers_, dma_operation_register & 0x1FF)).dmaMasterEnable() == DmaMasterEnable::enabled) {
                    executeDma();
                }
            }
            break;
        case dma_operation_register:
            rawWrite<u32>(io_registers_, dma_operation_register & 0x1FF, data);
            if (DmaOperationRegister(data).dmaMasterEnable() == DmaMasterEnable::enabled) {
                executeDma();
            }
            break;
        default:
            rawWrite<u32>(io_registers_, addr & 0x1FF, data);
            break;
    }

}

void Sh2::purgeCache() {
    // All the valid bits and LRU bits are initialized to 0
    for (u8 i = 0; i < 32; ++i) {
        // :WARNING: the following code is untested
        u32 data = rawRead<u32>(cache_addresses_, i);
        data &= 0xFFFFFC0B;
        rawWrite<u32>(cache_addresses_, i, data);
    }
}

void Sh2::initializeOnChipRegisters() {
    // Bus State Controler registers
    rawWrite<u32>(io_registers_, bus_control_register1 & 0x1FF, 0x000003F0);
    rawWrite<u32>(io_registers_, bus_control_register2 & 0x1FF, 0x000000FC);
    rawWrite<u32>(io_registers_, wait_state_control_register & 0x1FF, 0x0000AAFF);
    rawWrite<u32>(io_registers_, individual_memory_control_register & 0x1FF, 0x00000000);
    rawWrite<u32>(io_registers_, refresh_timer_control_status_register & 0x1FF, 0x00000000);
    rawWrite<u32>(io_registers_, refresh_timer_counter & 0x1FF, 0x00000000);
    rawWrite<u32>(io_registers_, refresh_time_constant_register & 0x1FF, 0x00000000);

    // Direct Memory Access Controler registers
    rawWrite<u32>(io_registers_, dma_channel_control_register_0 & 0x1FF, 0x00000000);
    rawWrite<u8>(io_registers_, dma_request_response_selection_control_register_0 & 0x1FF, 0x00);
    rawWrite<u32>(io_registers_, dma_channel_control_register_1 & 0x1FF, 0x00000000);
    rawWrite<u8>(io_registers_, dma_request_response_selection_control_register_1 & 0x1FF, 0x00);
    rawWrite<u32>(io_registers_, dma_operation_register & 0x1FF, 0x00000000);

    // Division Unit
    rawWrite<u32>(io_registers_, division_control_register & 0x1FF, 0x00000000);
    rawWrite<u32>(io_registers_, vector_number_setting_register_div & 0x1FF, 0x00000000); // lower 16 bits are undefined
}

void Sh2::start32bitsDivision() {

}

void Sh2::start64bitsDivision() {

}

void Sh2::executeDma() {

}

void Sh2::reset() {
    initializeOnChipRegisters();
}

void Sh2::sendInterrupt(const Interrupt& i) {
    if (i.level != 0) {

    }
}

}
}