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
#include "interrupt_sources.h"

namespace saturnin {
namespace core {

// Forward declarations
class Emulator_context;

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
    Sh2(bool is_master, Emulator_context* ec);
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

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Sh2::sendInterrupt(const Interrupt& i);
    ///
    /// \brief  Sends an interrupt to the sh2.
    ///
    /// \author Runik
    /// \date   19/01/2019
    ///
    /// \param  i   Interrupt sent.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void sendInterrupt(const Interrupt& i);

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

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Sh2::executeDma();
    ///
    /// \brief  Executes the DMA operation.
    ///
    /// \author Runik
    /// \date   13/01/2019
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void executeDma();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Sh2::reset();
    ///
    /// \brief  Resets the SH2 and enters in the reset state.
    ///
    /// \author Runik
    /// \date   18/01/2019
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void reset();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Sh2::runDivisionUnit(u8 cycles_to_run);
    ///
    /// \brief  Executes the division unit for the specified number of cycles.
    ///
    /// \author Runik
    /// \date   18/01/2019
    ///
    /// \param  cycles_to_run   Number of cycles to run.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void runDivisionUnit(u8 cycles_to_run);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto Sh2::scu_memory() const;
    ///
    /// \brief  Returns SCU memory array.
    ///
    /// \author Runik
    /// \date   09/02/2019
    ///
    /// \return SCU memory array.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto scuMemory() const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto Sh2::scu() const;
    ///
    /// \brief  Returns the SCU object.
    ///
    /// \author Runik
    /// \date   10/02/2019
    ///
    /// \return The SCU object.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto scu() const;

    Emulator_context* emulator_context_;    ///< Context of the emulator
    
    std::array <u8, 0x400>  cache_addresses_;   ///< Cache addresses (1KB).
    std::array <u8, 0x1000> cache_data_;        ///< Cache data (4KB).    
    std::array <u8, 0x200>  io_registers_;      ///< I/O registers (512B).
    bool is_master_;                            ///< True if the SH2 is configured as master, false if as slave.

    /// \name Interrupt management
    //@{
    std::list<Interrupt>    pending_interrupts_;///< List of pending interrupts.
    bool                    is_interrupted_;    ///< True if this sh2 is interrupted.
    //@}

    /// \name DIVU (Division unit)
    //@{
    u8      divu_remaining_cycles_; ///< Remaining cycles to end current division
    //@}

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