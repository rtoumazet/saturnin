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

namespace saturnin {
namespace core {

u32 Sh2::readRegisters(u32 addr) const {
    switch (addr) {
        case dvdnt:
        case dvdntl_shadow:
            return rawRead<u32>(io_registers_, dvdntl & 0x1FF);
            break;
        case dvdnth_shadow:
            return rawRead<u32>(io_registers_, dvdnth & 0x1FF);
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
            if (io_registers_[tocr & 0x1FF] & 0x10) {
                frt_ocrb_ = (data << 8) | 0xFF;
            }
            else {
                frt_ocra_ = (data << 8) | 0xFF;
            }
            break;
        case ocrbl:
            if (io_registers_[tocr & 0x1FF] & 0x10) {
                frt_ocrb_ = (0xFF << 8) | data;
            }
            else {
                frt_ocra_ = (0xFF << 8) | data;
            }
            break;
        case tcr:
            switch (io_registers_[tcr & 0x1FF] & 0x3) {
                case 0x0:
                    frt_clock_ = 8;
                    frt_mask_ = 0x7;
                    break;
                case 0x1:
                    frt_clock_ = 32;
                    frt_mask_ = 0x1F;
                    break;
                case 0x2:
                    frt_clock_ = 128;
                    frt_mask_ = 0x7F;
                    break;
                case 0x3:
                    Log::warning("sh2", "FRT - External clock not implemented");
                    break;
            }
            break;
        case ccr:
            Log::debug("sh2", fmt::format("CCR byte write: {}", data));
            if (data & ccr_cache_purge) {
                purgeCache();

                data &= 0xEF; // cache purge bit is reverted to 0 after operation
            }

            break;
        default:
            rawWrite<u8>(io_registers_, addr & 0x1FF, data);
    }
    
    io_registers_[addr & 0x1FF] = data;

}

void Sh2::writeRegisters(u32 addr, u16 data) {

}

void Sh2::writeRegisters(u32 addr, u32 data) {

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

}
}