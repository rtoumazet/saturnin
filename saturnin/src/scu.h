// 
// scu.h
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
/// \file	scu.h
///
/// \brief	Declares everything related to the System Control Unit (SCU). 
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "emulator_defs.h"

namespace saturnin {
namespace core {

// Forward declarations
class Memory;
struct DmaConfiguration;

enum class DmaLevel {
    level_unknown,  ///< Unknown level.
    level_0,        ///< Level 0 DMA.
    level_1,        ///< Level 1 DMA.
    level_2         ///< Level 2 DMA.
};

class Scu {
    public:
    //@{
    // Constructors / Destructors
    Scu()                        = delete;
    Scu(Memory* m);
    Scu(const Scu&)              = delete;
    Scu(Scu&&)                   = delete;
    Scu& operator=(const Scu&) & = delete;
    Scu& operator=(Scu&&) &      = delete;
    ~Scu()                       = default;
    //@}

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn u32 Scu::read32(const u32 addr) const;
    ///
    /// \brief  Reads 32 bits of data from the specified address in the SCU memory space.
    ///
    /// \author Runik
    /// \date   24/01/2019
    ///
    /// \param  addr    Address to read data from.
    ///
    /// \return Data read.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    u32 read32(const u32 addr) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Scu::write32(const u32 addr, const u32 data);
    ///
    /// \brief  Writes 32 bits of data at the specified address in the SCU memory space.
    ///
    /// \author Runik
    /// \date   24/01/2019
    ///
    /// \param  addr    Address to write data to.
    /// \param  data    Data to write.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void write32(const u32 addr, const u32 data);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Scu::executeDma(const DmaConfiguration& dc);
    ///
    /// \brief  Executes the DMA operation.
    ///
    /// \author Runik
    /// \date   02/02/2019
    ///
    /// \param  dc  DMA configuration.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void executeDma(const DmaConfiguration& dc);

    private:

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Scu::initializeRegisters();
    ///
    /// \brief  Initializes the SCU registers.
    ///
    /// \author Runik
    /// \date   24/01/2019
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void initializeRegisters();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn DmaConfiguration Scu::configureDmaTransfer(DmaLevel dl) const;
    ///
    /// \brief  Configures the DMA transfer.
    ///
    /// \author Runik
    /// \date   24/01/2019
    ///
    /// \param  dl  DMA level to configure.
    ///
    /// \return A DmaConfiguration struct.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    DmaConfiguration configureDmaTransfer(const DmaLevel dl) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Scu::initializeDmaTransferByteNumber(DmaConfiguration& dc, const u32 register_address) const;
    ///
    /// \brief  Initializes the DMA transfer byte number part of DMA configuration.
    ///
    /// \author Runik
    /// \date   02/02/2019
    ///
    /// \param [in,out] dc                  DMA configuration.
    /// \param          register_address    Register address to read data from.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void initializeDmaTransferByteNumber(DmaConfiguration& dc, const u32 register_address) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Scu::initializeDmaMode(DmaConfiguration& dc, const u32 register_address) const;
    ///
    /// \brief  Initializes the DMA mode part of DMA configuration.
    ///
    /// \author Runik
    /// \date   02/02/2019
    ///
    /// \param [in,out] dc                  DMA configuration.
    /// \param          register_address    Register address to read data from.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void initializeDmaMode(DmaConfiguration& dc, const u32 register_address) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Scu::initializeDmaEnable(DmaConfiguration& dc, const u32 register_address) const;
    ///
    /// \brief  Initializes the DMA enable part of DMA configuration.
    ///
    /// \author Runik
    /// \date   02/02/2019
    ///
    /// \param [in,out] dc                  DMA configuration.
    /// \param          register_address    Register address to read data from.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void initializeDmaEnable(DmaConfiguration& dc, const u32 register_address) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Scu::initializeDmaAddressAdd(DmaConfiguration& dc, const u32 register_address) const;
    ///
    /// \brief  Initializes the DMA address add part of DMA configuration.
    ///
    /// \author Runik
    /// \date   02/02/2019
    ///
    /// \param [in,out] dc                  DMA configuration.
    /// \param          register_address    Register address to read data from.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void initializeDmaAddressAdd(DmaConfiguration& dc, const u32 register_address) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Scu::initializeDmaWriteAddress(DmaConfiguration& dc, const u32 register_address) const;
    ///
    /// \brief  Initializes the DMA write address part of DMA configuration.
    ///
    /// \author Runik
    /// \date   02/02/2019
    ///
    /// \param [in,out] dc                  DMA configuration.
    /// \param          register_address    Register address to read data from.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void initializeDmaWriteAddress(DmaConfiguration& dc, const u32 register_address) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Scu::initializeDmaReadAddress(DmaConfiguration& dc, const u32 register_address) const;
    ///
    /// \brief  Initializes the DMA read address part of DMA configuration.
    ///
    /// \author Runik
    /// \date   02/02/2019
    ///
    /// \param [in,out] dc                  DMA configuration.
    /// \param          register_address    Register address to read data from.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void initializeDmaReadAddress(DmaConfiguration& dc, const u32 register_address) const;

    Memory* memory_;    ///< Memory object
};

}
}