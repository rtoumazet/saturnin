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
            if (io_registers_[tocr & 0x1FF] & util::toUnderlying(TocrOcrs::selects_ocrb)) {
                frt_ocrb_ = (data << 8) | 0xFF;
            }
            else {
                frt_ocra_ = (data << 8) | 0xFF;
            }
            break;
        case ocrbl:
            if (io_registers_[tocr & 0x1FF] & util::toUnderlying(TocrOcrs::selects_ocrb)) {
                frt_ocrb_ = (0xFF << 8) | data;
            }
            else {
                frt_ocra_ = (0xFF << 8) | data;
            }
            break;
        case tcr:
            switch (io_registers_[tcr & 0x1FF] & util::toUnderlying(TcrMask::cksx)) {
                case util::toUnderlying(TcrClock::internal_divided_by_8):
                    frt_clock_  = 8;
                    frt_mask_   = 0b00000111;
                    break;
                case util::toUnderlying(TcrClock::internal_divided_by_32):
                    frt_clock_  = 32;
                    frt_mask_   = 0b00011111;
                    break;
                case util::toUnderlying(TcrClock::internal_divided_by_128):
                    frt_clock_  = 128;
                    frt_mask_   = 0b01111111;
                    break;
                case util::toUnderlying(TcrClock::external):
                    Log::warning("sh2", "FRT - External clock not implemented");
                    break;
            }
            break;
        case ccr:
            Log::debug("sh2", fmt::format("CCR byte write: {}", data));
            if (data & util::toUnderlying(CcrCp::cache_purge)) {
                purgeCache();
                data ^= util::toUnderlying(CcrMask::cp); // cache purge bit is cleared after operation
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
        case icr:
            u16 old_icr = rawRead<u16>(io_registers_, addr & 0x1FF);
            if (old_icr & util::toUnderlying(IcrNmie::detection_falling_edge)) {
                if ( (old_icr & util::toUnderlying(IcrNmil::nmi_input_high)) && (data & util::toUnderlying(IcrNmil::nmi_input_low))) {
                    Log::error("sh2", "Falling edge NMI !");
                    //EmuState::pMem->nmiToDo = true;
                    //EmuState::pMem->nmiCount = 0x100;
                }
            }else {
                if ((old_icr & util::toUnderlying(IcrNmil::nmi_input_low)) && (data & util::toUnderlying(IcrNmil::nmi_input_high))) {
                    Log::error("sh2", "Rising edge NMI !");
                    //EmuState::pMem->nmiToDo = true;
                    //EmuState::pMem->nmiCount = 0x100;
                }
            }
            
            rawWrite<u16>(io_registers_, addr & 0x1FF, data);
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
//    IOReg[(Addr & 0x00000FFF) - 0xE00] = static_cast<uint8_t>((Data & 0xFF000000) >> 24);
//    IOReg[(Addr & 0x00000FFF) - 0xE00 + 0x1] = static_cast<uint8_t>((Data & 0x00FF0000) >> 16);
//    IOReg[(Addr & 0x00000FFF) - 0xE00 + 0x2] = static_cast<uint8_t>((Data & 0x0000FF00) >> 8);
//    IOReg[(Addr & 0x00000FFF) - 0xE00 + 0x3] = static_cast<uint8_t>(Data & 0x000000FF);
//
//    switch (Addr & 0x1FF) {
//        case LOCAL_TIER:
//#ifdef _LOGS
//            if (isMaster) EmuState::pLog->Scu("TIER write (master): ", Data);
//            else EmuState::pLog->Scu("TIER write (slave): ", Data);
//#endif
//            break;
//        case LOCAL_BCR1:
//            IOReg[(Addr & 0x00000FFF) - 0xE00] = static_cast<uint8_t>((Data & 0xFF000000) >> 24);
//            IOReg[(Addr & 0x00000FFF) - 0xE00 + 0x1] = static_cast<uint8_t>((Data & 0x00FF0000) >> 16);
//            IOReg[(Addr & 0x00000FFF) - 0xE00 + 0x2] |= static_cast<uint8_t>((Data & 0x00001F00) >> 8);
//            IOReg[(Addr & 0x00000FFF) - 0xE00 + 0x3] |= static_cast<uint8_t>(Data & 0x000000F7);
//            break;
//        case LOCAL_BCR2:
//            IOReg[(Addr & 0x00000FFF) - 0xE00] = static_cast<uint8_t>((Data & 0xFF000000) >> 24);
//            IOReg[(Addr & 0x00000FFF) - 0xE00 + 0x1] = static_cast<uint8_t>((Data & 0x00FF0000) >> 16);
//            IOReg[(Addr & 0x00000FFF) - 0xE00 + 0x2] = static_cast<uint8_t>((Data & 0x0000000) >> 8);
//            IOReg[(Addr & 0x00000FFF) - 0xE00 + 0x3] |= static_cast<uint8_t>(Data & 0x000000FC);
//            break;
//            // Division Unit 
//        case LOCAL_DVDNT:
//            IOReg[(Addr & 0x00000FFF) - 0xE00] = static_cast<uint8_t>((Data & 0xFF000000) >> 24);
//            IOReg[(Addr & 0x00000FFF) - 0xE00 + 0x1] = static_cast<uint8_t>((Data & 0x00FF0000) >> 16);
//            IOReg[(Addr & 0x00000FFF) - 0xE00 + 0x2] = static_cast<uint8_t>((Data & 0x0000FF00) >> 8);
//            IOReg[(Addr & 0x00000FFF) - 0xE00 + 0x3] = static_cast<uint8_t>(Data & 0x000000FF);
//
//            division32Start = true;
//
//            // Mirroring ST-V
//            IOReg[LOCAL_DVDNTL] = static_cast<uint8_t>((Data & 0xFF000000) >> 24);
//            IOReg[LOCAL_DVDNTL + 1] = static_cast<uint8_t>((Data & 0x00FF0000) >> 16);
//            IOReg[LOCAL_DVDNTL + 2] = static_cast<uint8_t>((Data & 0x0000FF00) >> 8);
//            IOReg[LOCAL_DVDNTL + 3] = static_cast<uint8_t>(Data & 0x000000FF);
//
//            IOReg[LOCAL_DVDNTL_SHADOW] = static_cast<uint8_t>((Data & 0xFF000000) >> 24);
//            IOReg[LOCAL_DVDNTL_SHADOW + 1] = static_cast<uint8_t>((Data & 0x00FF0000) >> 16);
//            IOReg[LOCAL_DVDNTL_SHADOW + 2] = static_cast<uint8_t>((Data & 0x0000FF00) >> 8);
//            IOReg[LOCAL_DVDNTL_SHADOW + 3] = static_cast<uint8_t>(Data & 0x000000FF);
//            if (Data & 0x80000000) {
//                IOReg[LOCAL_DVDNTH] = static_cast<uint8_t>(0xff);
//                IOReg[LOCAL_DVDNTH + 1] = static_cast<uint8_t>(0xff);
//                IOReg[LOCAL_DVDNTH + 2] = static_cast<uint8_t>(0xff);
//                IOReg[LOCAL_DVDNTH + 3] = static_cast<uint8_t>(0xff);
//            }
//            else {
//                IOReg[LOCAL_DVDNTH] = static_cast<uint8_t>(0x0);
//                IOReg[LOCAL_DVDNTH + 1] = static_cast<uint8_t>(0x0);
//                IOReg[LOCAL_DVDNTH + 2] = static_cast<uint8_t>(0x0);
//                IOReg[LOCAL_DVDNTH + 3] = static_cast<uint8_t>(0x0);
//            }
//            break;
//        case LOCAL_DVDNTL:
//            IOReg[(Addr & 0x00000FFF) - 0xE00] = static_cast<uint8_t>((Data & 0xFF000000) >> 24);
//            IOReg[(Addr & 0x00000FFF) - 0xE00 + 0x1] = static_cast<uint8_t>((Data & 0x00FF0000) >> 16);
//            IOReg[(Addr & 0x00000FFF) - 0xE00 + 0x2] = static_cast<uint8_t>((Data & 0x0000FF00) >> 8);
//            IOReg[(Addr & 0x00000FFF) - 0xE00 + 0x3] = static_cast<uint8_t>(Data & 0x000000FF);
//
//            division64Start = true;
//            IOReg[LOCAL_DVDNTL_SHADOW] = static_cast<uint8_t>((Data & 0xFF000000) >> 24);
//            IOReg[LOCAL_DVDNTL_SHADOW + 1] = static_cast<uint8_t>((Data & 0x00FF0000) >> 16);
//            IOReg[LOCAL_DVDNTL_SHADOW + 2] = static_cast<uint8_t>((Data & 0x0000FF00) >> 8);
//            IOReg[LOCAL_DVDNTL_SHADOW + 3] = static_cast<uint8_t>(Data & 0x000000FF);
//            break;
//            // DMA
//        case LOCAL_CHCR0:
//            IOReg[(Addr & 0x00000FFF) - 0xE00] = static_cast<uint8_t>((Data & 0xFF000000) >> 24);
//            IOReg[(Addr & 0x00000FFF) - 0xE00 + 0x1] = static_cast<uint8_t>((Data & 0x00FF0000) >> 16);
//            IOReg[(Addr & 0x00000FFF) - 0xE00 + 0x2] = static_cast<uint8_t>((Data & 0x0000FF00) >> 8);
//            IOReg[(Addr & 0x00000FFF) - 0xE00 + 0x3] = static_cast<uint8_t>(Data & 0x000000FF);
//
//            if (Data & 0x1) if (IOReg[LOCAL_DMAOR + 3] & 0x1) ExecuteDma();
//            break;
//        case LOCAL_CHCR1:
//            IOReg[(Addr & 0x00000FFF) - 0xE00] = static_cast<uint8_t>((Data & 0xFF000000) >> 24);
//            IOReg[(Addr & 0x00000FFF) - 0xE00 + 0x1] = static_cast<uint8_t>((Data & 0x00FF0000) >> 16);
//            IOReg[(Addr & 0x00000FFF) - 0xE00 + 0x2] = static_cast<uint8_t>((Data & 0x0000FF00) >> 8);
//            IOReg[(Addr & 0x00000FFF) - 0xE00 + 0x3] = static_cast<uint8_t>(Data & 0x000000FF);
//
//            if (Data & 0x1) if (IOReg[LOCAL_DMAOR + 3] & 0x1) ExecuteDma();
//            break;
//        case LOCAL_DMAOR:
//            IOReg[(Addr & 0x00000FFF) - 0xE00] = static_cast<uint8_t>((Data & 0xFF000000) >> 24);
//            IOReg[(Addr & 0x00000FFF) - 0xE00 + 0x1] = static_cast<uint8_t>((Data & 0x00FF0000) >> 16);
//            IOReg[(Addr & 0x00000FFF) - 0xE00 + 0x2] = static_cast<uint8_t>((Data & 0x0000FF00) >> 8);
//            IOReg[(Addr & 0x00000FFF) - 0xE00 + 0x3] = static_cast<uint8_t>(Data & 0x000000FF);
//
//            if (IOReg[LOCAL_DMAOR + 3] & 0x1) ExecuteDma();
//            break;
//        default:
//            if (Addr >= 0xFFFFFE00)
//            {
//                IOReg[(Addr & 0x00000FFF) - 0xE00] = static_cast<uint8_t>((Data & 0xFF000000) >> 24);
//                IOReg[(Addr & 0x00000FFF) - 0xE00 + 0x1] = static_cast<uint8_t>((Data & 0x00FF0000) >> 16);
//                IOReg[(Addr & 0x00000FFF) - 0xE00 + 0x2] = static_cast<uint8_t>((Data & 0x0000FF00) >> 8);
//                IOReg[(Addr & 0x00000FFF) - 0xE00 + 0x3] = static_cast<uint8_t>(Data & 0x000000FF);
//            }
//            break;
//    }
//
//    if (division32Start || division64Start) ExecuteDivision();
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