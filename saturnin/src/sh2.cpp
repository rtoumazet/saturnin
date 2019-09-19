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
#include "sh2_registers.h"
#include "emulator_context.h"
#include "scu_registers.h"

namespace is = saturnin::core::interrupt_source;

namespace saturnin {
namespace core {

Sh2::Sh2(Sh2Type st, Emulator_context* ec) : sh2_type_(st), emulator_context_(ec) {
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
            return (rawRead<u32>(io_registers_, addr & sh2_memory_mask) & 0x0000FFFF);
            break;
        case dividend_register_l_32_bits:
        case dividend_register_l_shadow:
            return rawRead<u32>(io_registers_, dividend_register_l & sh2_memory_mask);
            break;
        case dividend_register_h_shadow:
            return rawRead<u32>(io_registers_, dividend_register_h & sh2_memory_mask);
            break;
        case dma_operation_register:
            return (rawRead<u32>(io_registers_, addr & sh2_memory_mask) & 0x0000000F);
            break;
        default:
            return rawRead<u32>(io_registers_, addr & sh2_memory_mask);
    }
}

void Sh2::writeRegisters(u32 addr, u8 data) {
    switch (addr) {
        /* FRT */
        case timer_interrupt_enable_register:
            if (sh2_type_ == Sh2Type::master) Log::debug("sh2", "TIER byte write (master SH2)");
            else Log::debug("sh2", "TIER byte write (slave SH2)");
            break;
        case output_compare_register_b_h: {
                auto tocr = TimerOutputCompareControlRegister(io_registers_[timer_output_compare_control_register & sh2_memory_mask]);
                switch (tocr.get(TimerOutputCompareControlRegister::outputCompareRegisterSelect)) {
                    case OutputCompareRegisterSelect::ocra: frt_ocra_ = (data << 8) | 0xFF; break;
                    case OutputCompareRegisterSelect::ocrb: frt_ocrb_ = (data << 8) | 0xFF; break;
                }
            }
            break;
        case output_compare_register_b_l: {
            auto tocr = TimerOutputCompareControlRegister(io_registers_[timer_output_compare_control_register & sh2_memory_mask]);
            switch (tocr.get(TimerOutputCompareControlRegister::outputCompareRegisterSelect)) {
                case OutputCompareRegisterSelect::ocra: frt_ocra_ = (0xFF << 8) | data; break;
                case OutputCompareRegisterSelect::ocrb: frt_ocrb_ = (0xFF << 8) | data; break;
            }
        }
            break;
        case timer_control_register:
            switch (TimerControlRegister(io_registers_[timer_control_register & 0x1FF]).get(TimerControlRegister::clockSelect)) {
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
        case cache_control_register: {
                Log::debug("sh2", "CCR byte write: {}", data);

                auto ccr = CacheControlRegister(data);
                if (ccr.get(CacheControlRegister::cachePurge) == CachePurge::cache_purge) {
                    purgeCache();

                    // cache purge bit is cleared after operation
                    ccr.reset(CacheControlRegister::cachePurge);
                    data = ccr.toUlong();
                }
            }
            break;
        default:
            break;
    }
    rawWrite<u8>(io_registers_, addr & sh2_memory_mask, data);
}

void Sh2::writeRegisters(u32 addr, u16 data) {
    switch (addr) {
        case timer_interrupt_enable_register:
            if (sh2_type_ == Sh2Type::master) Log::debug("sh2", "TIER word write (master SH2)");
            else Log::debug("sh2", "TIER word write (slave SH2)");
            rawWrite<u16>(io_registers_, addr & 0x1FF, data);
            break;
        case interrupt_control_register: {
            auto old_icr = InterruptControlRegister(rawRead<u16>(io_registers_, addr & sh2_memory_mask));
            auto new_icr = InterruptControlRegister(data);
            switch (old_icr.get(InterruptControlRegister::nmiEdgeDetection)) {
                case NmiEdgeDetection::falling:
                    if ((old_icr.get(InterruptControlRegister::nmiInputLevel) == NmiInputLevel::high) && 
                            (new_icr.get(InterruptControlRegister::nmiInputLevel) == NmiInputLevel::low)) {
                        Log::error("sh2", "Falling edge NMI !");
                    }
                    break;
                case NmiEdgeDetection::rising:
                    if ((old_icr.get(InterruptControlRegister::nmiInputLevel) == NmiInputLevel::low) && 
                            (new_icr.get(InterruptControlRegister::nmiInputLevel) == NmiInputLevel::high)) {
                        Log::error("sh2", "Rising edge NMI !");
                    }
                    break;
            }
            rawWrite<u16>(io_registers_, addr & sh2_memory_mask, data);
        }
                                         break;
        case bus_control_register1 + 2:
            rawWrite<u16>(io_registers_, addr & sh2_memory_mask, data & 0x00F7);
            break;
        case bus_control_register2 + 2:
            rawWrite<u16>(io_registers_, addr & sh2_memory_mask, data & 0x00FC);
            break;
        default:
            rawWrite<u16>(io_registers_, addr & sh2_memory_mask, data);
            break;
    }
}

void Sh2::writeRegisters(u32 addr, u32 data) {
    switch (addr) {
        case timer_interrupt_enable_register:
            if (sh2_type_ == Sh2Type::master) Log::debug("sh2", "TIER long write (master SH2)");
            else Log::debug("sh2", "TIER long write (slave SH2)");
            rawWrite<u32>(io_registers_, addr & sh2_memory_mask, data);
            break;
        case bus_control_register1:
            if ((data & 0xFFFF0000) == 0xA55A0000) {
                rawWrite<u16>(io_registers_, addr + 2 & sh2_memory_mask, data & BusControlRegister1::writeMask());
            }
            break;
        case bus_control_register2:
            if ((data & 0xFFFF0000) == 0xA55A0000) {
                rawWrite<u16>(io_registers_, addr + 2 & sh2_memory_mask, data & BusControlRegister2::writeMask());
            }
            break;
        case dividend_register_l_32_bits:
            rawWrite<u32>(io_registers_, addr & sh2_memory_mask, data);

            // ST-V needs some mirroring
            rawWrite<u32>(io_registers_, dividend_register_l & sh2_memory_mask, data);
            rawWrite<u32>(io_registers_, dividend_register_l_shadow & sh2_memory_mask, data);

            // Sign extension for the upper 32 bits if needed
            (data & 0x80000000) ? rawWrite<u32>(io_registers_, dividend_register_h & sh2_memory_mask, 0xFFFFFFFF)
                : rawWrite<u32>(io_registers_, dividend_register_h & sh2_memory_mask, 0x00000000);

            start32bitsDivision();
            break;
        case dividend_register_l:
            rawWrite<u32>(io_registers_, dividend_register_l & sh2_memory_mask, data);
            rawWrite<u32>(io_registers_, dividend_register_l_shadow & sh2_memory_mask, data);

            start64bitsDivision();
            break;
        case dma_channel_control_register_0:
            rawWrite<u32>(io_registers_, dma_channel_control_register_0 & sh2_memory_mask, data);

            if (DmaChannelControlRegister(data).get(DmaChannelControlRegister::interruptEnable) == Sh2DmaInterruptEnable::enabled) {
                auto dor = DmaOperationRegister(rawRead<u32>(io_registers_, dma_operation_register & sh2_memory_mask));
                if (dor.get(DmaOperationRegister::dmaMasterEnable) == DmaMasterEnable::enabled) {
                    executeDma();
                }
            }
            break;
        case dma_channel_control_register_1:
            rawWrite<u32>(io_registers_, dma_channel_control_register_1 & 0x1FF, data);

            if (DmaChannelControlRegister(data).get(DmaChannelControlRegister::interruptEnable) == Sh2DmaInterruptEnable::enabled) {
                auto dor = DmaOperationRegister(rawRead<u32>(io_registers_, dma_operation_register & sh2_memory_mask));
                if (dor.get(DmaOperationRegister::dmaMasterEnable) == DmaMasterEnable::enabled) {
                    executeDma();
                }
            }
            break;
        case dma_operation_register:
            rawWrite<u32>(io_registers_, dma_operation_register & sh2_memory_mask, data);
            if (DmaOperationRegister(data).get(DmaOperationRegister::dmaMasterEnable) == DmaMasterEnable::enabled) {
                executeDma();
            }
            break;
        default:
            rawWrite<u32>(io_registers_, addr & sh2_memory_mask, data);
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
    switch (sh2_type_) {
        case Sh2Type::master: rawWrite<u32>(io_registers_, bus_control_register1 & sh2_memory_mask, 0x000003F0); break;
        case Sh2Type::slave:  rawWrite<u32>(io_registers_, bus_control_register1 & sh2_memory_mask, 0x000083F0); break;
        default: Log::error("sh2", "Unknown SH2 type");
    }
   
    rawWrite<u32>(io_registers_, bus_control_register2                 & sh2_memory_mask, 0x000000FC);
    rawWrite<u32>(io_registers_, wait_state_control_register           & sh2_memory_mask, 0x0000AAFF);
    rawWrite<u32>(io_registers_, individual_memory_control_register    & sh2_memory_mask, 0x00000000);
    rawWrite<u32>(io_registers_, refresh_timer_control_status_register & sh2_memory_mask, 0x00000000);
    rawWrite<u32>(io_registers_, refresh_timer_counter                 & sh2_memory_mask, 0x00000000);
    rawWrite<u32>(io_registers_, refresh_time_constant_register        & sh2_memory_mask, 0x00000000);

    // Direct Memory Access Controler registers
    rawWrite<u32>(io_registers_, dma_channel_control_register_0                   & sh2_memory_mask, 0x00000000);
    rawWrite<u8>(io_registers_, dma_request_response_selection_control_register_0 & sh2_memory_mask, 0x00);
    rawWrite<u32>(io_registers_, dma_channel_control_register_1                   & sh2_memory_mask, 0x00000000);
    rawWrite<u8>(io_registers_, dma_request_response_selection_control_register_1 & sh2_memory_mask, 0x00);
    rawWrite<u32>(io_registers_, dma_operation_register                           & sh2_memory_mask, 0x00000000);

    // Division Unit
    rawWrite<u32>(io_registers_, division_control_register          & sh2_memory_mask, 0x00000000);
    rawWrite<u32>(io_registers_, vector_number_setting_register_div & sh2_memory_mask, 0x00000000); // lower 16 bits are undefined

    // Serial Communication Interface
    rawWrite<u8>(io_registers_, serial_mode_register    & sh2_memory_mask, 0x00);
    rawWrite<u8>(io_registers_, bit_rate_register       & sh2_memory_mask, 0xFF);
    rawWrite<u8>(io_registers_, serial_control_register & sh2_memory_mask, 0x00);
    rawWrite<u8>(io_registers_, transmit_data_register  & sh2_memory_mask, 0xFF);
    rawWrite<u8>(io_registers_, serial_status_register  & sh2_memory_mask, 0x84);
    rawWrite<u8>(io_registers_, receive_data_register   & sh2_memory_mask, 0x00);

    // Free Running timer
    rawWrite<u8>(io_registers_, timer_interrupt_enable_register            & sh2_memory_mask, 0x01);
    rawWrite<u8>(io_registers_, free_running_timer_control_status_register & sh2_memory_mask, 0x00);
    rawWrite<u8>(io_registers_, free_running_counter_h                     & sh2_memory_mask, 0x00);
    rawWrite<u8>(io_registers_, free_running_counter_l                     & sh2_memory_mask, 0x00);
    rawWrite<u8>(io_registers_, output_compare_register_a_h                & sh2_memory_mask, 0xff);
    rawWrite<u8>(io_registers_, output_compare_register_a_l                & sh2_memory_mask, 0xff);
    rawWrite<u8>(io_registers_, output_compare_register_b_h                & sh2_memory_mask, 0xff);
    rawWrite<u8>(io_registers_, output_compare_register_b_l                & sh2_memory_mask, 0xff);
    rawWrite<u8>(io_registers_, timer_control_register                     & sh2_memory_mask, 0x00);
    rawWrite<u8>(io_registers_, timer_output_compare_control_register      & sh2_memory_mask, 0xe0);
    rawWrite<u8>(io_registers_, input_capture_register_h                   & sh2_memory_mask, 0x00);
    rawWrite<u8>(io_registers_, input_capture_register_l                   & sh2_memory_mask, 0x00);

    // Watch Dog Timer
    rawWrite<u8>(io_registers_, watchdog_timer_control_status_register     & sh2_memory_mask, 0x18);
    rawWrite<u8>(io_registers_, watchdog_timer_counter                     & sh2_memory_mask, 0x00);
    rawWrite<u8>(io_registers_, reset_control_status_register              & sh2_memory_mask, 0x1F);
}

void Sh2::start32bitsDivision() {
    // 32/32 division
    Log::debug("sh2", "32/32 division");

    // DVDNT is copied in DVDNTL and DVDNTH
    s32 dvdnt = rawRead<u32>(io_registers_, dividend_register_l_32_bits & sh2_memory_mask);

    rawWrite<u32>(io_registers_, dividend_register_l & sh2_memory_mask, dvdnt);

    if (dvdnt < 0) rawWrite<u32>(io_registers_, dividend_register_h & sh2_memory_mask, 0xffffffff);
    else           rawWrite<u32>(io_registers_, dividend_register_h & sh2_memory_mask, 0x00000000);

    s32 dvsr = rawRead<u32>(io_registers_, divisor_register & sh2_memory_mask);
    
    Log::debug("sh2", "Dividend : {}, divisor : {}", dvdnt, dvsr);
    
    auto dvcr = DivisionControlRegister(io_registers_[division_control_register & sh2_memory_mask]);
    ldiv_t result{};
    if (dvsr != 0) result = ldiv(dvdnt, dvsr);
    else           dvcr.set(DivisionControlRegister::overflowFlag);
    
    // Overflow check
    if ((dvdnt & 0x80000000) && (dvsr & 0x80000000)) {
        if ((result.quot == 0x7FFFFFFF) && (result.rem & 0x80000000)) dvcr.set(DivisionControlRegister::overflowFlag);
    }

    // 39 cycles for regular division, 6 cycles when overflow is detected
    divu_remaining_cycles_ = (dvcr.get(DivisionControlRegister::overflowFlag) == OverflowFlag::overflow) ? 6 : 39;
    divu_quot_             = result.quot;
    divu_rem_              = result.rem;

    if (dvcr.get(DivisionControlRegister::overflowFlag) == OverflowFlag::overflow) {
        rawWrite<u32>(io_registers_, division_control_register & sh2_memory_mask, dvcr.toUlong()); // Updating the register
    }

    divu_is_running_ = true;
}

void Sh2::start64bitsDivision() {
    Log::debug("sh2", "64/32 division");

    s32 dvdntl = rawRead<u32>(io_registers_, dividend_register_l & sh2_memory_mask);
    s32 dvdnth = rawRead<u32>(io_registers_, dividend_register_h & sh2_memory_mask);
    s32 dvsr   = rawRead<u32>(io_registers_, divisor_register & sh2_memory_mask);
    
    s64 dividend = (static_cast<s64>(dvdnth) << 32) | (dvdntl & 0xffffffff);

    Log::debug("sh2", "Dividend : {}, divisor : {}", dividend, dvsr);

    auto dvcr = DivisionControlRegister(io_registers_[division_control_register & sh2_memory_mask]);
    s64 quotient{};
    s64 remainder{};
    if (dvsr != 0) {
        quotient  = dividend / dvsr;
        remainder = dividend % dvsr;
    } else 
        dvcr.set(DivisionControlRegister::overflowFlag);

    // Overflow check
    if ((dvdnth & 0x80000000) && (dvsr & 0x80000000)) {
        if ((quotient == 0x7FFFFFFF) && (remainder & 0x80000000)) dvcr.set(DivisionControlRegister::overflowFlag);
    }

    // 39 cycles for regular division, 6 cycles when overflow is detected
    divu_remaining_cycles_ = (dvcr.get(DivisionControlRegister::overflowFlag) == OverflowFlag::overflow) ? 6 : 39;
    divu_quot_             = static_cast<s32>(quotient);
    divu_rem_              = static_cast<s32>(remainder);

    if (dvcr.get(DivisionControlRegister::overflowFlag) == OverflowFlag::overflow) {
        rawWrite<u32>(io_registers_, division_control_register & sh2_memory_mask, dvcr.toUlong()); // Updating the register
    }

    divu_is_running_ = true;
}

void Sh2::runDivisionUnit(const u8 cycles_to_run) {
    divu_remaining_cycles_ -= cycles_to_run;
    if (divu_remaining_cycles_ == 0) {
        auto dvcr = DivisionControlRegister(io_registers_[division_control_register & sh2_memory_mask]);
        if (dvcr.get(DivisionControlRegister::overflowFlag) == OverflowFlag::overflow) {
            if (dvcr.get(DivisionControlRegister::interruptEnable) == InterruptEnable::enabled) {
                Log::debug("sh2", "DIVU - Sending division overflow interrupt");
                is::sh2_division_overflow.vector = rawRead<u8>(io_registers_, vector_number_setting_register_div & sh2_memory_mask);
                is::sh2_division_overflow.level  = rawRead<u8>(io_registers_, interrupt_priority_level_setting_register_a & sh2_memory_mask) >> 4;
                sendInterrupt(is::sh2_division_overflow);
            }
        } else {
            // Copy in DVDNTL and DVDNTH + ST-V mirroring
            rawWrite<u32>(io_registers_, dividend_register_l_32_bits & sh2_memory_mask, divu_quot_);
            rawWrite<u32>(io_registers_, dividend_register_l         & sh2_memory_mask, divu_quot_);
            rawWrite<u32>(io_registers_, dividend_register_l_shadow  & sh2_memory_mask, divu_quot_);

            rawWrite<u32>(io_registers_, dividend_register_h         & sh2_memory_mask, divu_rem_);
            rawWrite<u32>(io_registers_, dividend_register_h_shadow  & sh2_memory_mask, divu_rem_);
        }
        divu_is_running_ = false;
    }
}

void Sh2::executeDma() {

}

void Sh2::reset() {
    initializeOnChipRegisters();
}

auto Sh2::scuMemory() const {
    return emulator_context_->memory()->scu_;
}

auto Sh2::scu() const {
    return emulator_context_->scu();
}

void Sh2::sendInterrupt(const Interrupt& i) {
    if (i.level != 0) {
        scu()->setInterruptStatusRegister(i);
        if (!scu()->isInterruptMasked(i, sh2_type_)) {
            if (pending_interrupts_.size() <= max_interrupt_number) {
                if (!is_level_interrupted_[i.level]) {
                    is_level_interrupted_[i.level] = true;
                    pending_interrupts_.push_front(i);
                    
                    // Sorting (greatest priority first)
                    pending_interrupts_.sort();
                    pending_interrupts_.reverse();

                    Log::debug("sh2", "{} SH2 interrupt pending : {:#0x}", (sh2_type_== Sh2Type::master) ? "Master" : "Slave", i.vector);
                }
            }
            else {
                // Max number of pending interrupts reached, nothing is added
                Log::debug("sh2", "Maximum number of pending interrupts reached");
                
                // When the interrupt is NMI, the lower priority interrupt is removed
                if (i.vector == is::vector_nmi) {
                    pending_interrupts_.pop_back();
                    pending_interrupts_.push_front(is::nmi);

                    // Sorting (greatest priority first)
                    pending_interrupts_.sort();
                    pending_interrupts_.reverse();

                    Log::debug("sh2", "NMI interrupt forced");

                }
            }
        }
    }
}

}
}