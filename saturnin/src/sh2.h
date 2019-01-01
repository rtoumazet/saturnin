// 
// sh2.h
// Saturnin
//
// Copyright (c) 2018 Renaud Toumazet
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
/// \file	sh2.h
///
/// \brief	Declares the sh2 class. 
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <array> // array 
#include "emulator_defs.h"
#include "memory.h"

namespace saturnin {
namespace core {

/// \name Onboard i/o registers addresses
//@{
const u32 smr           = 0xFFFFFE00;
const u32 brr           = 0xFFFFFE01;
const u32 scr           = 0xFFFFFE02;
const u32 tdr           = 0xFFFFFE03;
const u32 ssr           = 0xFFFFFE04;
const u32 rdr           = 0xFFFFFE05;
const u32 tier          = 0xFFFFFE10;
const u32 ftcsr         = 0xFFFFFE11;
const u32 frc           = 0xFFFFFE12;
const u32 frch          = 0xFFFFFE12;
const u32 frcl          = 0xFFFFFE13;
const u32 ocrab         = 0xFFFFFE14;
const u32 ocrbh         = 0xFFFFFE14;
const u32 ocrbl         = 0xFFFFFE15;
const u32 ocrah         = 0xFFFFFE14;
const u32 ocral         = 0xFFFFFE15;
const u32 tcr           = 0xFFFFFE16;
const u32 tocr          = 0xFFFFFE17;
const u32 ficr          = 0xFFFFFE18;
const u32 icrh          = 0xFFFFFE18;
const u32 icrl          = 0xFFFFFE19;
const u32 ipra          = 0xFFFFFEE2;
const u32 iprb          = 0xFFFFFE60;
const u32 vcra          = 0xFFFFFE62;
const u32 vcrb          = 0xFFFFFE64;
const u32 vcrc          = 0xFFFFFE66;
const u32 vcrd          = 0xFFFFFE68;
const u32 drcr0         = 0xFFFFFE71;
const u32 drcr1         = 0xFFFFFE72;
const u32 wtcsr         = 0xFFFFFE80;
const u32 wtcnt         = 0xFFFFFE81;
const u32 rstcsr        = 0xFFFFFE82;
const u32 sbycr         = 0xFFFFFE91;
const u32 ccr           = 0xFFFFFE92;
const u32 icr           = 0xFFFFFEE0;
const u32 vcrwdt        = 0xFFFFFEE4;
const u32 dvsr          = 0xFFFFFF00;
const u32 dvdnt         = 0xFFFFFF04;
const u32 dvcr          = 0xFFFFFF08;
const u32 vcrdiv        = 0xFFFFFF0C;
const u32 dvdnth        = 0xFFFFFF10;
const u32 dvdntl        = 0xFFFFFF14;
const u32 dvdnth_shadow = 0xFFFFFF18;
const u32 dvdntl_shadow = 0xFFFFFF1C;
const u32 barah         = 0xFFFFFF40;
const u32 baral         = 0xFFFFFF42;
const u32 bamrah        = 0xFFFFFF44;
const u32 bamral        = 0xFFFFFF46;
const u32 bbra          = 0xFFFFFF48;
const u32 barbh         = 0xFFFFFF60;
const u32 barbl         = 0xFFFFFF62;
const u32 bamrbh        = 0xFFFFFF64;
const u32 bamrbl        = 0xFFFFFF66;
const u32 bbrb          = 0xFFFFFF68;
const u32 bdrbh         = 0xFFFFFF70;
const u32 bdrbl         = 0xFFFFFF72;
const u32 bdmrbh        = 0xFFFFFF74;
const u32 bdmrbl        = 0xFFFFFF76;
const u32 brcr          = 0xFFFFFF78;
const u32 sar0          = 0xFFFFFF80;
const u32 dar0          = 0xFFFFFF84;
const u32 tcr0          = 0xFFFFFF88;
const u32 chcr0         = 0xFFFFFF8C;
const u32 sar1          = 0xFFFFFF90;
const u32 dar1          = 0xFFFFFF94;
const u32 tcr1          = 0xFFFFFF98;
const u32 chcr1         = 0xFFFFFF9C;
const u32 vcrdma0       = 0xFFFFFFA0;
const u32 vcrdma1       = 0xFFFFFFA8;
const u32 dmaor         = 0xFFFFFFB0;
const u32 bcr1          = 0xFFFFFFE0;
const u32 bcr2          = 0xFFFFFFE4;
const u32 wcr           = 0xFFFFFFE8;
const u32 mcr           = 0xFFFFFFEC;
const u32 rtcsr         = 0xFFFFFFF0;
const u32 rtcnt         = 0xFFFFFFF4;
const u32 rtcor         = 0xFFFFFFF8;
//@}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  Sh2
///
/// \brief  Encapsulates Sh2 processor and related methods.
///
/// \author Runik
/// \date   31/12/2018
////////////////////////////////////////////////////////////////////////////////////////////////////

class Sh2 {
    public:
        //@{
        // Constructors / Destructors
        Sh2()                        = delete;
        Sh2(bool is_master) : is_master_(is_master) {};
        Sh2(const Sh2&)              = delete;
        Sh2(Sh2&&)                   = delete;
        Sh2& operator=(const Sh2&) & = delete;
        Sh2& operator=(Sh2&&) &      = delete;
        ~Sh2()                       = default;
        //@}

        template<typename T>
        T read(const u32 addr) const {
            return rawRead<T>(this->io_registers_, addr & 0x1FF);
        }

        // 32 bits specialization
        template<>
        u32 read<u32>(const u32 addr) const {
            return read(addr);
        }

        template<typename T>
        void write(const u32 addr, const T data) {
            rawWrite<T>(io_registers_, addr & 0x1FF, data);
        }

        template<>
        void write<u8>(const u32 addr, const u8 data) {
            write(addr, data);
        }

        template<>
        void write<u16>(const u32 addr, const u16 data) {
            write(addr, data);
        }

        template<>
        void write<u32>(const u32 addr, const u32 data) {
            write(addr, data);
        }

    private:
        std::array <u8, 0x200> io_registers_; ///< I/O registers (512B).
        bool is_master_;

        u32 read(const u32 addr) const;

        void write(u32 addr, u8 data);
        void write(u32 addr, u16 data);
        void write(u32 addr, u32 data);
};



}
}