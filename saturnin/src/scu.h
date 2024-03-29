//
// scu.h
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
/// \file	scu.h
///
/// \brief	Declares everything related to the System Control Unit (SCU).
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <array>  // array
#include <queue>  // priority_queue
#include <vector> // vector
#include <saturnin/src/emulator_defs.h>
#include <saturnin/src/interrupt_sources.h>
#include <saturnin/src/emulator_modules.h>

namespace is = saturnin::core::interrupt_source;

// Forward declarations
namespace saturnin::sh2 {
class Sh2;
enum class Sh2Type;
} // namespace saturnin::sh2

namespace saturnin::core {

// Forward declarations
class Memory;
class EmulatorContext;

constexpr auto indirect_dma_end_code = u32{0x80000000};
constexpr auto program_ram_size      = u16{256 * 32 * 4}; ///< 256 words of 32 bits.
constexpr auto data_ram_size         = u16{64 * 32 * 4};  ///< 64 words of 32 bits.
constexpr auto data_ram_number       = u8{4};             ///< Number of data RAM.

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   DmaLevel
///
/// \brief  DMA level.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class DmaLevel : uint8_t {
    level_0       = 0,  ///< Level 0 DMA.
    level_1       = 1,  ///< Level 1 DMA.
    level_2       = 2,  ///< Level 2 DMA.
    level_unknown = 255 ///< Unknown level.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   DmaStatus
///
/// \brief  DMA status.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class DmaStatus : uint8_t { finished = 0, waiting_start_factor = 1, queued = 2, active = 3 };

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   DmaBus
///
/// \brief  DMA bus.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class DmaBus { a_bus, b_bus, cpu_bus, dsp_bus, unknown_bus };

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   ScuRegion
///
/// \brief  Region of the SCU.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class ScuRegion {
    unknown,
    rom,
    smpc,
    backup_ram,
    work_ram_l,
    minit,
    sinit,
    a_bus_cs0,
    a_bus_cs1,
    a_bus_dummy,
    a_bus_cs2,
    sound,
    vdp1,
    vdp2,
    scu_register,
    work_ram_h
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
    DmaLevel                            dma_level;
    DmaStatus                           dma_status;
    u32                                 read_address;
    u32                                 write_address;
    u32                                 transfer_byte_number;
    ScuRegs::Dxad::ReadAddressAddValue  read_add_value;
    ScuRegs::Dxad::WriteAddressAddValue write_add_value;
    ScuRegs::Dxen::DmaEnable            dma_enable;
    ScuRegs::Dxen::DmaStarting          dma_starting;
    ScuRegs::Dxmd::DmaMode              dma_mode;
    ScuRegs::Dxmd::ReadAddressUpdate    read_address_update;
    ScuRegs::Dxmd::WriteAddressUpdate   write_address_update;
    ScuRegs::Dxmd::StartingFactorSelect starting_factor_select;
};

class Scu {
  public:
    //@{
    // Constructors / Destructors
    Scu() = delete;
    explicit Scu(EmulatorContext* ec);
    Scu(const Scu&)                      = delete;
    Scu(Scu&&)                           = delete;
    auto operator=(const Scu&) & -> Scu& = delete;
    auto operator=(Scu&&) & -> Scu&      = delete;
    ~Scu()                               = default;
    //@}

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto Scu::read32(u32 addr) const -> u32;
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

    [[nodiscard]] auto read32(u32 addr) const -> u32;

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

    void write32(u32 addr, u32 data);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Scu::executeDma(DmaConfiguration& dc);
    ///
    /// \brief  Executes the DMA operation.
    ///
    /// \author Runik
    /// \date   02/02/2019
    ///
    /// \param [in,out] dc  DMA configuration.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void executeDma(DmaConfiguration& dc);

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
    /// \fn void Scu::generateInterrupt(const Interrupt& i);
    ///
    /// \brief  Generates an interrupt and sends it to the master SH2 if conditions are set.
    ///
    /// \author Runik
    /// \date   10/02/2019
    ///
    /// \param  i   Interrupt to generate.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void generateInterrupt(const Interrupt& i);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto Scu::isInterruptMasked(const Interrupt& i) -> bool;
    ///
    /// \brief  Checks if the interrupt is masked in the SCU.
    ///
    /// \author Runik
    /// \date   10/02/2019
    ///
    /// \param  i   Interrupt to check.
    ///
    /// \return True if interrupt is masked.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto isInterruptMasked(const Interrupt& i) -> bool;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto Scu::isInterruptExecuting(const Interrupt& i) -> bool;
    ///
    /// \brief  Checks if the interrupt is currently executing.
    ///
    /// \author Runik
    /// \date   21/11/2019
    ///
    /// \param  i   Interrupt to check.
    ///
    /// \return True if interrupt is executing.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto isInterruptExecuting(const Interrupt& i) -> bool;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Scu::sendStartFactor(StartingFactorSelect sfs);
    ///
    /// \brief  Notifies the SCU that a DMA start factor has occured.
    ///
    /// \author Runik
    /// \date   22/03/2019
    ///
    /// \param  sfs Start factor sent.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void sendStartFactor(ScuRegs::Dxmd::StartingFactorSelect sfs);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn bool Scu::clearInterruptFlag(const Interrupt& i);
    ///
    /// \brief  Clears the interrupt flag in the Interrupt Status Register.
    ///
    /// \author Runik
    /// \date   04/10/2019
    ///
    /// \param  i   Interrupt to clear.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void clearInterruptFlag(const Interrupt& i);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto Scu::getTimer0CompareValue() const->u32;
    ///
    /// \brief  Gets timer 0 compare value
    ///
    /// \author Runik
    /// \date   07/06/2020
    ///
    /// \returns    The timer 0 compare value.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto getTimer0CompareValue() const -> u32;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto Scu::isTimer1Enabled() const -> bool;
    ///
    /// \brief  Queries if Timer 1 is enabled.
    ///
    /// \author Runik
    /// \date   08/06/2020
    ///
    /// \returns    True if Timer 1 is enabled.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto isTimer1Enabled() const -> bool;

    /// \name Functions handling various interrupt events
    //@{
    void onVblankIn();
    void onVblankOut();
    void onHblankIn();
    void onTimer0();
    //@}

    void dmaTest();

    /// \name Context objects accessors
    //@{
    //[[nodiscard]] auto emulatorContext() const -> EmulatorContext*;
    //[[nodiscard]] auto memory() const -> Memory*;
    //@}

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
    /// \fn auto Scu::configureDmaTransfer(DmaLevel level) -> DmaConfiguration;
    ///
    /// \brief  Configures the DMA transfer.
    ///
    /// \author Runik
    /// \date   24/01/2019
    ///
    /// \param  level   DMA level to configure.
    ///
    /// \return A DmaConfiguration struct.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto configureDmaTransfer(DmaLevel level) -> DmaConfiguration;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Scu::initializeDmaTransferByteNumber(DmaConfiguration& dc) const;
    ///
    /// \brief  Initializes the DMA transfer byte number part of DMA configuration.
    ///
    /// \author Runik
    /// \date   02/02/2019
    ///
    /// \param [in,out] dc                  DMA configuration.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void initializeDmaTransferByteNumber(DmaConfiguration& dc) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	static void Scu::initializeDmaMode(DmaConfiguration& dc, const ScuRegs::DxmdType& reg);
    ///
    /// \brief	Initializes the DMA mode part of DMA configuration.
    ///
    /// \author	Runik
    /// \date	02/02/2019
    ///
    /// \param [in,out]	dc 	DMA configuration.
    /// \param 		   	reg	Internal register to read data from.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static void initializeDmaMode(DmaConfiguration& dc, const ScuRegs::DxmdType& reg);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	static void Scu::initializeDmaEnable(DmaConfiguration& dc, const ScuRegs::DxenType& reg);
    ///
    /// \brief	Initializes the DMA enable part of DMA configuration.
    ///
    /// \author	Runik
    /// \date	02/02/2019
    ///
    /// \param [in,out]	dc 	DMA configuration.
    /// \param 		   	reg	Internal register to read data from.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static void initializeDmaEnable(DmaConfiguration& dc, const ScuRegs::DxenType& reg);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	static void Scu::initializeDmaAddressAdd(DmaConfiguration& dc, const ScuRegs::DxadType& reg);
    ///
    /// \brief	Initializes the DMA address add part of DMA configuration.
    ///
    /// \author	Runik
    /// \date	02/02/2019
    ///
    /// \param [in,out]	dc 	DMA configuration.
    /// \param 		   	reg	Internal register to read data from.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static void initializeDmaAddressAdd(DmaConfiguration& dc, const ScuRegs::DxadType& reg);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	static void Scu::initializeDmaWriteAddress(DmaConfiguration& dc, const ScuRegs::DxwType& reg);
    ///
    /// \brief	Initializes the DMA write address part of DMA configuration.
    ///
    /// \author	Runik
    /// \date	02/02/2019
    ///
    /// \param [in,out]	dc 	DMA configuration.
    /// \param 		   	reg	Internal register to read data from.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static void initializeDmaWriteAddress(DmaConfiguration& dc, const ScuRegs::DxwType& reg);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	static void Scu::initializeDmaReadAddress(DmaConfiguration& dc, const ScuRegs::DxrType& reg);
    ///
    /// \brief	Initializes the DMA read address part of DMA configuration.
    ///
    /// \author	Runik
    /// \date	02/02/2019
    ///
    /// \param [ins]	dc 	DMA configuration.
    /// \param 			reg	Internal register to read data from.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static void initializeDmaReadAddress(DmaConfiguration& dc, const ScuRegs::DxrType& reg);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Scu::addDmaToQueue(const DmaConfiguration& dc);
    ///
    /// \brief  Adds the DMA to the queue.
    ///
    /// \author Runik
    /// \date   22/03/2019
    ///
    /// \param [in] dc                  DMA configuration.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void addDmaToQueue(const DmaConfiguration& dc);

    struct DmaCompare {
        auto operator()(const DmaConfiguration& dc1, const DmaConfiguration& dc2) const -> bool {
            return dc1.dma_status < dc2.dma_status;
        }
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static auto Scu::getDmaBus(u32 address) -> DmaBus;
    ///
    /// \brief  Returns the DMA bus of the specified address.
    ///
    /// \author Runik
    /// \date   26/06/2019
    ///
    /// \param  address Memory map address.
    ///
    /// \return The dma bus.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static auto getDmaBus(u32 address) -> DmaBus;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static auto Scu::getScuRegion(u32 address) -> ScuRegion;
    ///
    /// \brief  Returns the SCU region of the specified address.
    ///
    /// \author Runik
    /// \date   26/06/2019
    ///
    /// \param  address Memory map address.
    ///
    /// \return The scu region.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static auto getScuRegion(u32 address) -> ScuRegion;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Scu::sendDmaEndInterrupt(DmaLevel l);
    ///
    /// \brief  Sends dma end interrupt of the current dma level.
    ///
    /// \author Runik
    /// \date   05/08/2019
    ///
    /// \param  l   DMA level to send interrupt for.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void sendDmaEndInterrupt(DmaLevel l);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	void Scu::resetDmaEnable( DmaConfiguration& dc) const;
    ///
    /// \brief	Reset dma enable value for the current dma level.
    ///
    /// \author	Runik
    /// \date	05/08/2019
    ///
    /// \param [in]	dc	DMA configuration.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void resetDmaEnable(DmaConfiguration& dc) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Scu::dmaUpdateWriteAddress(DmaLevel l, u32 data);
    ///
    /// \brief  Updates write address of the current dma level.
    ///
    /// \author Runik
    /// \date   12/08/2019
    ///
    /// \param  l       DMA level.
    /// \param  data    Data to write.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void dmaUpdateWriteAddress(DmaLevel l, u32 data);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Scu::dmaSort();
    ///
    /// \brief  Sorts the DMA queue
    ///
    /// \author Runik
    /// \date   01/01/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void dmaSort();

    // using DmaConfigurations = std::vector<DmaConfiguration>;
    // using DmaQueue          = std::priority_queue<DmaConfiguration, DmaConfigurations, DmaCompare>;
    using DmaQueue = std::vector<DmaConfiguration>;
    DmaQueue dma_queue_;

    void activateDma();

    EmulatorModules modules_;

    std::array<u8, program_ram_size>                           program_ram_; ///< DSP Program RAM.
    std::array<std::array<u8, data_ram_size>, data_ram_number> data_ram_;    ///< DSP Data RAM.

    ScuRegs regs_;                                                           ///< Scu memory registers.
};

} // namespace saturnin::core
