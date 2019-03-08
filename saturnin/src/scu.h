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

#include <vector>
#include "emulator_defs.h"
#include "emulator_enums.h"
#include "interrupt_sources.h"

namespace saturnin {
namespace core {

// Forward declarations
class Memory;
class Emulator_context;

enum class DmaLevel {
    level_unknown,  ///< Unknown level.
    level_0,        ///< Level 0 DMA.
    level_1,        ///< Level 1 DMA.
    level_2         ///< Level 2 DMA.
};

enum class DmaStatus : uint8_t {
    finished                = 0,
    waiting_start_factor    = 1,
    queued                  = 2,
    active                  = 3
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct DmaConfiguration
///
/// \brief  Will hold configuration data for level 0-2 DMA transfers.
///
/// \author Runik
/// \date   24/01/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

struct DmaConfiguration {
    DmaLevel             dma_level;
    DmaStatus            dma_status;
    u32                  read_address;
    u32                  write_address;
    u32                  transfer_byte_number;
    ReadAddressAddValue  read_add_value;
    WriteAddressAddValue write_add_value;
    DmaEnable            dma_enable;
    DmaStarting          dma_starting;
    DmaMode              dma_mode;
    ReadAddressUpdate    read_address_update;
    WriteAddressUpdate   write_address_update;
    StartingFactorSelect starting_factor_select;
};


class Scu {
public:
    //@{
    // Constructors / Destructors
    Scu()                        = delete;
    Scu(Emulator_context*);
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

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Scu::setInterruptStatusRegister(const Interrupt& i);
    ///
    /// \brief  Sets the interrupt in the Interrupt Status Register.
    ///
    /// \author Runik
    /// \date   10/02/2019
    ///
    /// \param  i   The interrupt to set.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void setInterruptStatusRegister(const Interrupt& i);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Scu::resetInterruptStatusRegister(const Interrupt& i);
    ///
    /// \brief  Resets the interrupt in the Interrupt Status Register.
    ///
    /// \author Runik
    /// \date   10/02/2019
    ///
    /// \param  i   The interrupt to reset.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void resetInterruptStatusRegister(const Interrupt& i);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn bool Scu::isInterruptMasked(const Interrupt& i, Sh2Type t ) const;
    ///
    /// \brief  Checks if the interrupt is masked in the SCU.
    ///
    /// \author Runik
    /// \date   10/02/2019
    ///
    /// \param  i   Interrupt to check.
    /// \param  t   Type of SH2 to check.
    ///
    /// \return True if interrupt is masked.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    bool isInterruptMasked(const Interrupt& i, Sh2Type t) const;

private:

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto Scu::scu_memory() const;
    ///
    /// \brief  Returns SCU memory array.
    ///
    /// \author Runik
    /// \date   08/02/2019
    ///
    /// \return SCU memory array.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto scuMemory() const;

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

    void addDmaToQueue(const DmaConfiguration& dc);

    void sortDma();

    Emulator_context* emulator_context_; ///< Pointer to the emulator context object.
    
    std::vector<DmaConfiguration> dma_queue_;
};

}
}