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
#include "utilities.h" // toUnderlying

namespace util = saturnin::utilities;

namespace saturnin {
namespace core {

Sh2::Sh2(bool is_master) : is_master_(is_master) {
    initializeOnChipRegisters();
}

u32 Sh2::readRegisters(u32 addr) const {
    switch (addr) {
        
        case bcr1:
        case bcr2:
        case wcr:
        case mcr:
        case rtcsr:
        case rtcnt:
        case rtcor:
            // Bus State Controler registers
            return (rawRead<u32>(io_registers_, addr & 0x1FF) & 0x0000FFFF);
            break;
        case dvdnt:
        case dvdntl_shadow:
            return rawRead<u32>(io_registers_, dvdntl & 0x1FF);
            break;
        case dvdnth_shadow:
            return rawRead<u32>(io_registers_, dvdnth & 0x1FF);
            break;
        case dmaor:
            return (rawRead<u32>(io_registers_, addr & 0x1FF) & 0x0000000F);
            break;
        default:
            return rawRead<u32>(io_registers_, addr & 0x1FF);
    }
}

void Sh2::writeRegisters(u32 addr, u8 data) {
    switch (addr) {
        /* FRT */
        case tier:
            if(is_master_) Log::debug("sh2", "TIER byte write (master SH2)");
            else Log::debug("sh2", "TIER byte write (slave SH2)");
            break;
        case ocrbh:
            if (io_registers_[tocr & 0x1FF] & util::toUnderlying(OutputCompareRegisterSelect::ocrb)) {
                frt_ocrb_ = (data << 8) | 0xFF;
            }
            else {
                frt_ocra_ = (data << 8) | 0xFF;
            }
            break;
        case ocrbl:
            if (io_registers_[tocr & 0x1FF] & util::toUnderlying(OutputCompareRegisterSelect::ocrb)) {
                frt_ocrb_ = (0xFF << 8) | data;
            }
            else {
                frt_ocra_ = (0xFF << 8) | data;
            }
            break;
        case tcr:
            switch (io_registers_[tcr & 0x1FF] & util::toUnderlying(TimerControlRegisterMask::clock_select)) {
                case util::toUnderlying(ClockSelect::internal_divided_by_8):
                    frt_clock_  = 8;
                    frt_mask_   = 0b00000111;
                    break;
                case util::toUnderlying(ClockSelect::internal_divided_by_32):
                    frt_clock_  = 32;
                    frt_mask_   = 0b00011111;
                    break;
                case util::toUnderlying(ClockSelect::internal_divided_by_128):
                    frt_clock_  = 128;
                    frt_mask_   = 0b01111111;
                    break;
                case util::toUnderlying(ClockSelect::external):
                    Log::warning("sh2", "FRT - External clock not implemented");
                    break;
            }
            break;
        case ccr:
            Log::debug("sh2", fmt::format("CCR byte write: {}", data));
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
        case tier:
            if (is_master_) Log::debug("sh2", "TIER word write (master SH2)");
            else Log::debug("sh2", "TIER word write (slave SH2)");
            rawWrite<u16>(io_registers_, addr & 0x1FF, data);
            break;
        case icr: {
            u16 old_icr = rawRead<u16>(io_registers_, addr & 0x1FF);
            if (old_icr & util::toUnderlying(NmiEdgeDetection::falling)) {
                if ((old_icr & util::toUnderlying(NmiInputLevel::high)) && (data & util::toUnderlying(NmiInputLevel::low))) {
                    Log::error("sh2", "Falling edge NMI !");
                    //EmuState::pMem->nmiToDo = true;
                    //EmuState::pMem->nmiCount = 0x100;
                }
            }
            else {
                if ((old_icr & util::toUnderlying(NmiInputLevel::low)) && (data & util::toUnderlying(NmiInputLevel::high))) {
                    Log::error("sh2", "Rising edge NMI !");
                    //EmuState::pMem->nmiToDo = true;
                    //EmuState::pMem->nmiCount = 0x100;
                }
            }

            rawWrite<u16>(io_registers_, addr & 0x1FF, data);
        }
            break;
        case bcr1 + 2:
            rawWrite<u16>(io_registers_, addr & 0x1FF, data & 0x00F7);
            break;
        case bcr2 + 2:
            rawWrite<u16>(io_registers_, addr & 0x1FF, data & 0x00FC);
            break;
        default:
            rawWrite<u16>(io_registers_, addr & 0x1FF, data);
            break;
    }
}

void Sh2::writeRegisters(u32 addr, u32 data) {
    switch (addr) {
        case tier:
            if (is_master_) Log::debug("sh2", "TIER long write (master SH2)");
            else Log::debug("sh2", "TIER long write (slave SH2)");
            rawWrite<u32>(io_registers_, addr & 0x1FF, data);
            break;
        case bcr1:
            if ((data & 0xFFFF0000) == 0xA55A0000) {
                rawWrite<u16>(io_registers_, addr+2 & 0x1FF, data & util::toUnderlying(BusControlRegister1Mask::write_mask));
            }
            break;
        case bcr2:
            if ((data & 0xFFFF0000) == 0xA55A0000) {
                rawWrite<u16>(io_registers_, addr + 2   & 0x1FF, data & util::toUnderlying(BusControlRegister2Mask::write_mask));
            }
            break;
        case dvdnt:
            rawWrite<u32>(io_registers_, addr & 0x1FF, data);

            // ST-V needs some mirroring
            rawWrite<u32>(io_registers_, dvdntl         & 0x1FF, data);
            rawWrite<u32>(io_registers_, dvdntl_shadow  & 0x1FF, data);

            // Sign extension for the upper 32 bits if needed
            (data & 0x80000000) ? rawWrite<u32>(io_registers_, dvdnth & 0x1FF, 0xFFFFFFFF) : rawWrite<u32>(io_registers_, dvdnth & 0x1FF, 0x00000000);

            start32bitsDivision();
            break;
        case dvdntl:
            rawWrite<u32>(io_registers_, dvdntl         & 0x1FF, data);
            rawWrite<u32>(io_registers_, dvdntl_shadow  & 0x1FF, data);

            start64bitsDivision();
            break;
        case chcr0:
            rawWrite<u32>(io_registers_, chcr0 & 0x1FF, data);

            if (data & util::toUnderlying(InterruptEnable::interrupt_enabled)) {
                if(rawRead<u8>(io_registers_, (dmaor + 3) & 0x1FF) & util::toUnderlying(DmaMasterEnable::enabled)) executeDma();
            }
            break;
        case chcr1:
            rawWrite<u32>(io_registers_, chcr1 & 0x1FF, data);

            if (data & util::toUnderlying(InterruptEnable::interrupt_enabled)) {
                if (rawRead<u8>(io_registers_, (dmaor + 3) & 0x1FF) & util::toUnderlying(DmaMasterEnable::enabled)) executeDma();
            }
            break;
        case dmaor:
            rawWrite<u32>(io_registers_, dmaor & 0x1FF, data);

            if (rawRead<u8>(io_registers_, (dmaor + 3) & 0x1FF) & util::toUnderlying(DmaMasterEnable::enabled)) executeDma();
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
    rawWrite<u32>(io_registers_, bcr1 & 0x1FF, 0x000003F0);
    rawWrite<u32>(io_registers_, bcr2 & 0x1FF, 0x000000FC);
    rawWrite<u32>(io_registers_, wcr & 0x1FF, 0x0000AAFF);
    rawWrite<u32>(io_registers_, mcr & 0x1FF, 0x00000000);
    rawWrite<u32>(io_registers_, rtcsr & 0x1FF, 0x00000000);
    rawWrite<u32>(io_registers_, rtcnt & 0x1FF, 0x00000000);
    rawWrite<u32>(io_registers_, rtcor & 0x1FF, 0x00000000);

    // Direct Memory Access Controler registers
    rawWrite<u32>(io_registers_, chcr0 & 0x1FF, 0x00000000);
    rawWrite<u8>(io_registers_, drcr0 & 0x1FF, 0x00);
    rawWrite<u32>(io_registers_, chcr1 & 0x1FF, 0x00000000);
    rawWrite<u8>(io_registers_, drcr1 & 0x1FF, 0x00);
    rawWrite<u32>(io_registers_, dmaor & 0x1FF, 0x00000000);

    // Division Unit
    rawWrite<u32>(io_registers_, dvcr & 0x1FF, 0x00000000);
    rawWrite<u32>(io_registers_, vcrdiv & 0x1FF, 0x00000000); // lower 16 bits are undefined
}

void Sh2::start32bitsDivision() {

}

void Sh2::start64bitsDivision() {

}

void Sh2::executeDma() {

}

}
}