// 
// sh2.h
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

/////////////////////////////////////
// 5. Interrupt Controller (INTC)  //
/////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   InterruptControlRegisterMask
///
/// \brief  ICR (Interrupt Control Register) bit mask.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class InterruptControlRegisterMask : u16 {
    nmi_input_level                 = 0b1000000000000000,   ///< NMI Input Level (NMIL)
    nmi_edge_select                 = 0b0000000100000000,   ///< NMI Edge Select (NMIE)
    interrupt_vector_mode_select    = 0b0000000000000001    ///< Interrupt Vcetor Mode Select (VECMD)
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   NmiInputLevel
///
/// \brief  ICR - NMIL bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class NmiInputLevel : u16 {
    low   = 0b0000000000000000,   ///< NMI input level is low
    high  = 0b1000000000000000    ///< NMI input level is high
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   NmiEdgeDetection
///
/// \brief  ICR - NMIE bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class NmiEdgeDetection : u16 {
    falling = 0b0000000000000000,   ///< Interrupt request detected on falling edge of NMI input (initial)
    rising  = 0b0000000100000000    ///< Interrupt request detected on rising edge of NMI input
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   IcrVecmd
///
/// \brief  ICR - VECMD bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class VectorMode : u16 {
    auto_vector     = 0b0000000000000000,   ///< Auto vector mode (initial)
    external_vector = 0b0000000000000001    ///< External vector mode
};

//////////////////////////////////
// 7. Bus State Controler (BSC) //
//////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   BusControlRegister1Mask
///
/// \brief  BCR1 (Bus Control Register 1) bit mask.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class BusControlRegister1Mask : u16 {
    write_mask = 0b0001111111110111
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   BusControlRegister2Mask
///
/// \brief  BCR2 (Bus Control Register 2) bit mask.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class BusControlRegister2Mask : u16 {
    write_mask = 0b0000000011111100
};

//////////////
// 8. Cache //
//////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   CacheControlRegisterMask
///
/// \brief  CCR (Cache Control Register) bit mask.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class CacheControlRegisterMask : u8 {
    way_specification               = 0b11000000,   ///< Way Specification (wx)
    cache_purge                     = 0b00010000,   ///< Cache Purge (CP)
    two_way_mode                    = 0b00001000,   ///< Two Way Mode (Tw)
    data_replacement_disable        = 0b00000100,   ///< Data Replacement Disable (OD)
    instruction_replacement_disable = 0b00000010,   ///< Instruction Replacement Disable (ID)
    cache_enable                    = 0b00000001    ///< Cache Enable (CE)
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   CachePurge
///
/// \brief  CCR - CP bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class CachePurge : u8 {
    normal_operation    = 0b00000000,   ///< Normal operation
    cache_purge         = 0b00010000    ///< Cache purge
};

//////////////////////////////////////////////
// 9. Direct Memory Access Controler (DMAC) //
//////////////////////////////////////////////

enum class DmaChannelControlRegisterMask : u32 {

};

//////////////////////////////////////////
// 11. 16 Bits Free Running Timer (FRT) //
//////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   TimerOutputCompareControlRegisterMask
///
/// \brief  TOCR (Timer Output Compare Control Register) bit mask.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class TimerOutputCompareControlRegisterMask : u8 {
    output_compare_register_select  = 0b00010000,   ///< Output Compare Register Select (OCRS)
    output_level_a                  = 0b00000010,   ///< Output Level A (OLVLA)
    output_level_b                  = 0b00000001    ///< Output Level B (OLVLB)
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   OutputCompareRegisterSelect
///
/// \brief  TOCR - OCRS bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class OutputCompareRegisterSelect : u8 {
    ocra = 0b00000000,  ///< Selects register OCRA
    ocrb = 0b00010000   ///< Selects register OCRB
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   OutputLevelA
///
/// \brief  TOCR - OLVLA bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class OutputLevelA : u8 {
    outputs_0 = 0b00000000,  ///< Outputs 0 on compare match A
    outputs_1 = 0b00000010   ///< Outputs 1 on compare match A
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   OutputLevelB
///
/// \brief  TOCR - OLVLB bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class OutputLevelB : u8 {
    outputs_0 = 0b00000000,  ///< Outputs 0 on compare match B
    outputs_1 = 0b00000001   ///< Outputs 1 on compare match B
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   TimerControlRegisterMask
///
/// \brief  TCR (Timer Control Register) bit mask.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class TimerControlRegisterMask : u8 {
    input_edge_select   = 0b10000000,   ///< Input Edge Select (IEDG)
    clock_select        = 0b00000011    ///< Clock Select (CKSX)
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   ClockSelect
///
/// \brief  TCR - CKSx bits values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class ClockSelect : u8 {
    internal_divided_by_8   = 0b00, ///< Internal clock /8 (initial)
    internal_divided_by_32  = 0b01, ///< Internal clock /32
    internal_divided_by_128 = 0b10, ///< Internal clock /128
    external                = 0b11  ///< External clock
};


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
        Sh2(bool is_master);
        Sh2(const Sh2&)              = delete;
        Sh2(Sh2&&)                   = delete;
        Sh2& operator=(const Sh2&) & = delete;
        Sh2& operator=(Sh2&&) &      = delete;
        ~Sh2()                       = default;
        //@}

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn template<typename T> T Sh2::readRegisters(const u32 addr) const
        ///
        /// \brief  Read interface for the registers area.
        ///
        /// \author Runik
        /// \date   03/01/2019
        ///
        /// \tparam T   type of data to read (u8, u16 or u32).
        /// \param  addr    Address to read data from.
        ///
        /// \return Data read.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        template<typename T>
        T readRegisters(const u32 addr) const {
            return rawRead<T>(this->io_registers_, addr & 0x1FF);
        }

        // 32 bits specialization
        template<>
        u32 readRegisters<u32>(const u32 addr) const {
            return readRegisters(addr);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn template<typename T> void Sh2::writeRegisters(const u32 addr, const T data)
        ///
        /// \brief  Write interface for the registers area.
        ///
        /// \author Runik
        /// \date   03/01/2019
        ///
        /// \tparam T       Type of data to write (u8, u16 or u32).
        /// \param  addr    Address to write data to.
        /// \param  data    Data to write.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        template<typename T>
        void writeRegisters(const u32 addr, const T data) {
            rawWrite<T>(io_registers_, addr & 0x1FF, data);
        }

        // 8 bits specialization
        template<>
        void writeRegisters<u8>(const u32 addr, const u8 data) {
            writeRegisters(addr, data);
        }

        // 16 bits specialization
        template<>
        void writeRegisters<u16>(const u32 addr, const u16 data) {
            writeRegisters(addr, data);
        }

        // 32 bits specialization
        template<>
        void writeRegisters<u32>(const u32 addr, const u32 data) {
            writeRegisters(addr, data);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn template<typename T> T Sh2::readCacheAddresses(const u32 addr) const
        ///
        /// \brief  Read interface for cache addresses.
        ///
        /// \author Runik
        /// \date   03/01/2018
        ///
        /// \tparam T       Type of data to read (u8, u16 or u32).
        /// \param  addr    Address to read data from.
        ///
        /// \return Data read.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        template<typename T>
        T readCacheAddresses(const u32 addr) const {
            return rawRead<T>(cache_addresses_, addr & 0x3FF);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn template<typename T> void Sh2::writeCacheAddresses(const u32 addr, const T data)
        ///
        /// \brief  Write  interface for cache addresses.
        ///
        /// \author Runik
        /// \date   03/01/2019
        ///
        /// \tparam T       Type of data to write (u8, u16 or u32).
        /// \param  addr    Address to write data to.
        /// \param  data    Data to write.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        template<typename T>
        void writeCacheAddresses(const u32 addr, const T data) {
            rawWrite<T>(cache_addresses_, addr & 0x3FF, data);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn template<typename T> T Sh2::readCacheData(const u32 addr) const
        ///
        /// \brief  Read interface for cache data.
        ///
        /// \author Runik
        /// \date   03/01/2018
        ///
        /// \tparam T       Type of data to read (u8, u16 or u32).
        /// \param  addr    Address to read data from.
        ///
        /// \return Data read.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        template<typename T>
        T readCacheData(const u32 addr) const {
            return rawRead<T>(cache_data_, addr & 0xFFF);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn template<typename T> void Sh2::writeCacheData(const u32 addr, const T data)
        ///
        /// \brief  Write interface for cache data.
        ///
        /// \author Runik
        /// \date   03/01/2019
        ///
        /// \tparam T       Type of data to write (u8, u16 or u32).
        /// \param  addr    Address to write data to.
        /// \param  data    Data to write.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        template<typename T>
        void writeCacheData(const u32 addr, const T data) {
            rawWrite<T>(cache_data_, addr & 0xFFF, data);
        }

    private:

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn u32 Sh2::readRegisters(const u32 addr) const;
        ///
        /// \brief  Reads from the registers area.
        ///
        /// \author Runik
        /// \date   03/01/2019
        ///
        /// \param  addr    Address to read data from.
        ///
        /// \return Data read.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        u32 readRegisters(const u32 addr) const;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn void Sh2::writeRegisters(u32 addr, u8 data);
        ///
        /// \brief  Writes to the registers area.
        ///
        /// \author Runik
        /// \date   03/01/2019
        ///
        /// \param  addr    Address to write data to.
        /// \param  data    Data to write.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        void writeRegisters(u32 addr, u8 data);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn void Sh2::writeRegisters(u32 addr, u16 data);
        ///
        /// \brief  Writes to the registers area.
        ///
        /// \author Runik
        /// \date   03/01/2019
        ///
        /// \param  addr    Address to write data to.
        /// \param  data    Data to write.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        void writeRegisters(u32 addr, u16 data);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn void Sh2::writeRegisters(u32 addr, u32 data);
        ///
        /// \brief  Writes to the registers area.
        ///
        /// \author Runik
        /// \date   03/01/2019
        ///
        /// \param  addr    Address to write data to.
        /// \param  data    Data to write.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        void writeRegisters(u32 addr, u32 data);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn void Sh2::purgeCache();
        ///
        /// \brief  cache purge : all the cache address valid bits and LRU information are initialized to 0 (in 2 ways and 4 ways mode)
        ///
        /// \author Runik
        /// \date   02/01/2018
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        void purgeCache();

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn void Sh2::initializeOnChipRegisters();
        ///
        /// \brief  Initializes the values of onchip registers.
        ///
        /// \author Runik
        /// \date   10/01/2019
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        void initializeOnChipRegisters();

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn void Sh2::start32bitsDivision();
        ///
        /// \brief  Starts the execution of the 32 bits by 32bits division operation.
        ///
        /// \author Runik
        /// \date   10/01/2019
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        void start32bitsDivision();

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn void Sh2::start64bitsDivision();
        ///
        /// \brief  Starts the execution of the 64bits by 32 bits division operation.
        ///
        /// \author Runik
        /// \date   10/01/2019
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        void start64bitsDivision();

        std::array <u8, 0x400>  cache_addresses_;   ///< Cache addresses (1KB).
        std::array <u8, 0x1000> cache_data_;        ///< Cache data (4KB).    
        std::array <u8, 0x200>  io_registers_;      ///< I/O registers (512B).
        bool is_master_;                            ///< True if the SH2 is configured as master, false if as slave.

        /// \name FRT (Free Running Timer)
        //@{
        s32	    frt_elapsed_cycles_;///< Elapsed FRT cycles. 
        u8	    frt_clock_;	        ///< FRT clock. 
        u8	    frt_mask_;	        ///< FRT mask. 
        u16     frt_ocra_;	        ///< Output Compare Register A. 
        u16     frt_ocrb_;	        ///< Output Compare Register B.
        bool    frt_icie_;	        ///< Input Capture Interrupt Enable. 
        bool    frt_ociae_;	        ///< Output Compare Interrupt A Enable. 
        bool    frt_ocibe_;	        ///< Output Compare Interrupt B Enable.
        bool    frt_ovie_;	        ///< Timer Overflow Interrupt Enable. 
        bool    frt_current_ocr_;	///< Current Output Compare Register. 
        //@}
};

}
}