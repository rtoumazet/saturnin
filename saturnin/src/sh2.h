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

#include <array>  // array
#include <mutex>  // mutex
#include <vector> // vector
#include <saturnin/src/emulator_defs.h>
#include <saturnin/src/emulator_modules.h>
#include <saturnin/src/log.h>
#include <saturnin/src/memory.h>
#include <saturnin/src/sh2_registers.h>

// Forward declarations
namespace saturnin::core {
class EmulatorContext;
class EmulatorModules;
class Scu;
struct Interrupt;
} // namespace saturnin::core

namespace saturnin::sh2 {

using saturnin::core::EmulatorContext;
using saturnin::core::EmulatorModules;
using saturnin::core::Interrupt;
using saturnin::core::Log;
using saturnin::core::Logger;
using saturnin::core::Memory;
using saturnin::core::Scu;

using saturnin::core::rawRead;
using saturnin::core::rawWrite;

constexpr auto max_interrupt_number     = u8{10};
constexpr auto max_interrupt_level      = u8{16};
constexpr auto general_registers_number = u8{16};
constexpr auto sp_register_index        = u8{0xF};
constexpr auto cache_address_size       = u16{0x400};
constexpr auto cache_data_size          = u16{0x1000};
constexpr auto io_registers_size        = u16{0x200};

constexpr auto sh2_memory_mask    = u32{0x1FF};
constexpr auto cache_lines_number = u8{32};

constexpr auto frt_clock_divisor_8        = u8{8};
constexpr auto frt_clock_divisor_32       = u8{32};
constexpr auto frt_clock_divisor_128      = u8{128};
constexpr auto frt_clock_divisor_mask_8   = u8{0b00000111};
constexpr auto frt_clock_divisor_mask_32  = u8{0b00011111};
constexpr auto frt_clock_divisor_mask_128 = u8{0b01111111};

constexpr auto breakpoints_number = u8{5};
////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   Sh2Type
///
/// \brief  Type of SH2 CPU.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Sh2Type {
    unknown, ///< Unknown SH2 type.
    master,  ///< Master SH2.
    slave    ///< Slave SH2.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   Sh2Register
///
/// \brief  Values that represent SH2 internal registers.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Sh2Register { pc, pr, macl, mach, vbr, gbr, sr, r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15 };

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   DmaChannel
///
/// \brief  DMA channel.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class DmaChannel {
    channel_unknown, ///< Unknown channel DMA.
    channel_0,       ///< Channel 0 DMA.
    channel_1,       ///< Channel 1 DMA.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   DmaNextTransferPriority
///
/// \brief  Indicates the priority order for the next DMA transfer.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class DmaNextTransferPriority {
    channel_0_first, ///< Channel 0 will come first, then channel 1.
    channel_1_first  ///< Channel 1 will come first, then channel 0.

};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct Sh2DmaConfiguration
///
/// \brief  SH2 DMA Configuration.
///
/// \author Runik
/// \date   01/12/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

struct Sh2DmaConfiguration {
    DmaChannel              channel;
    u32                     counter;
    u32                     source;
    u32                     destination;
    Sh2Regs::Dmac::ChcrType chcr;
    Sh2Regs::Dmac::DrcrType drcr;
    Interrupt               interrupt{is::undefined};
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct CallstackItem
///
/// \brief  A callstack item.
///
/// \author Runik
/// \date   04/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

struct CallstackItem {
    u32 call_address;
    u32 return_address;

    CallstackItem(u32 c, u32 r) : call_address(c), return_address(r) {}
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
    Sh2() = delete;
    Sh2(Sh2Type st, EmulatorContext* ec);
    Sh2(const Sh2&)                      = delete;
    Sh2(Sh2&&)                           = delete;
    auto operator=(const Sh2&) & -> Sh2& = delete;
    auto operator=(Sh2&&) & -> Sh2&      = delete;
    ~Sh2()                               = default;
    //@}

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn template<typename T> auto Sh2::readRegisters(const u32 addr) -> T
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
    auto readRegisters(const u32 addr) -> T {
        return rawRead<T>(this->io_registers_, addr & sh2_memory_mask);
    }
    // 8 bits specialization
    template<>
    auto readRegisters<u8>(const u32 addr) -> u8 {
        return readRegisters8(addr);
    }
    // 16 bits specialization
    template<>
    auto readRegisters<u16>(const u32 addr) -> u16 {
        return readRegisters16(addr);
    }
    // 32 bits specialization
    template<>
    auto readRegisters<u32>(const u32 addr) -> u32 {
        return readRegisters32(addr);
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
        rawWrite<T>(io_registers_, addr & sh2_memory_mask, data);
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
    /// \fn	template<typename T> void Sh2::writeCachePurgeArea(const u32 addr, const T data)
    ///
    /// \brief	Cache purge area generic write.
    ///
    /// \tparam	T	Generic type parameter.
    /// \param 	addr	Address to write to.
    /// \param 	data	Data to write.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    template<typename T>
    void writeCachePurgeArea([[maybe_unused]] const u32 addr, [[maybe_unused]] const T data) {
        constexpr auto bits_number = u8{8};
        Log::warning(Logger::sh2, core::tr("{}bits write to the cache purge area !"), sizeof(T) * bits_number);
    }

    // 32 bits specialization
    template<>
    void writeCachePurgeArea<u32>(const u32 addr, [[maybe_unused]] const u32 data) {
        write32CachePurgeArea(addr, data);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	void Sh2::write32CachePurgeArea(const u32 addr, const u32 data) const;
    ///
    /// \brief	Cache purge area write
    ///
    /// \author	Runik
    /// \date	15/02/2023
    ///
    /// \param 	addr	Address to write to.
    /// \param 	data	Data to write.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void write32CachePurgeArea(const u32 addr, const u32 data) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn template<typename T> [[nodiscard]] auto Sh2::readCacheAddresses(const u32 addr) const -> T
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
    [[nodiscard]] auto readCacheAddresses(const u32 addr) const -> T {
        return rawRead<T>(cache_addresses_, addr & bitmask_03FF);
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
        rawWrite<T>(cache_addresses_, addr & bitmask_03FF, data);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn template<typename T> [[nodiscard]] auto Sh2::readCacheData(const u32 addr) const -> T
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
    [[nodiscard]] auto readCacheData(const u32 addr) const -> T {
        return rawRead<T>(cache_data_, addr & bitmask_0FFF);
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
        rawWrite<T>(cache_data_, addr & bitmask_0FFF, data);
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

    void sendInterrupt(const core::Interrupt& i);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Sh2::sendInterruptCaptureSignal();
    ///
    /// \brief  Sends the interrupt capture signal.
    ///
    /// \author Runik
    /// \date   22/11/2019
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void sendInterruptCaptureSignal();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto Sh2::run() -> u8;
    ///
    /// \brief  Runs the SH2.
    ///
    /// \author Runik
    /// \date   10/11/2019
    ///
    /// \return Elapsed cycles.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto run() -> u8;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Sh2::powerOnReset();
    ///
    /// \brief  Initializes the values of general registers.
    ///
    /// \author Runik
    /// \date   21/09/2019
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void powerOnReset();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn [[nodiscard]] auto Sh2::getRegister(Sh2Register reg) -> u32;
    ///
    /// \brief  Gets the register
    ///
    /// \author Runik
    /// \date   08/04/2020
    ///
    /// \param  reg Register to get.
    ///
    /// \returns    The register value.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    [[nodiscard]] auto getRegister(Sh2Register reg) const -> u32;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn template<typename T> static void Sh2::unmappedAccess(const u32 addr, const T data)
    ///
    /// \brief  Unmapped access.
    ///
    /// \author Runik
    /// \date   27/02/2020
    ///
    /// \tparam T   Generic type parameter.
    /// \param  addr    The address.
    /// \param  data    The data.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    template<typename T>
    static void unmappedAccess(const u32 addr, const T data) {
        Log::warning(Logger::sh2, "Unmapped write access : address :{:#0x} data:{:#0x}", addr, data);
    }

    static void unmappedAccess(const u32 addr) { Log::warning(Logger::sh2, "Unmapped read access : address :{:#0x}", addr); }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Sh2::addToCallstack(u32 call_addr, u32 return_addr);
    ///
    /// \brief  Adds an item to the callstack.
    ///
    /// \author Runik
    /// \date   06/05/2020
    ///
    /// \param  call_addr   The call address.
    /// \param  return_addr The return address.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void addToCallstack(u32 call_addr, u32 return_addr);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Sh2::popFromCallstack();
    ///
    /// \brief  Pops the last added item from callstack.
    ///
    /// \author Runik
    /// \date   06/05/2020
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void popFromCallstack();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto Sh2::callstack() -> std::vector<CallstackItem>
    ///
    /// \brief  Returns the callstack of the CPU
    ///
    /// \author Runik
    /// \date   06/05/2020
    ///
    /// \returns    A std::vector&lt;CallstackItem&gt;
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    [[nodiscard]] auto callstack() -> std::vector<CallstackItem>;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Sh2::initializeSubroutineDepth()
    ///
    /// \brief  Initializes the subroutine depth
    ///
    /// \author Runik
    /// \date   06/05/2020
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void initializeSubroutineDepth() { step_over_subroutine_depth_ = callstack_.size(); };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto Sh2::subroutineDepth() const -> size_t
    ///
    /// \brief  The saved subroutine depth.
    ///
    /// \author Runik
    /// \date   06/05/2020
    ///
    /// \returns    An u8.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto subroutineDepth() const -> size_t { return step_over_subroutine_depth_; };

    ///@{
    /// Breakpoints accessors
    void               breakpoint(const u8 index, const u32 addr) { breakpoints_[index] = addr; };
    [[nodiscard]] auto breakpoint(const u8 index) const -> u32 { return breakpoints_[index]; };
    ///@}

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Sh2::setBinaryFileStartAddress(const u32 val);
    ///
    /// \brief  Sets the start address of the binary file.
    ///
    /// \author Runik
    /// \date   12/08/2021
    ///
    /// \param  val The new start address.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void setBinaryFileStartAddress(const u32 val);

    EmulatorModules modules_; ///< Modules of the emulator

  private:
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto Sh2::readRegisters8(u32 addr) const -> u8;
    ///
    /// \brief  Reads from the registers area.
    ///
    /// \author Runik
    /// \date   30/10/2019
    ///
    /// \param  addr    Address to read data from.
    ///
    /// \return Data read.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto readRegisters8(u32 addr) const -> u8;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto Sh2::readRegisters16(u32 addr) const -> u16;
    ///
    /// \brief  Reads from the registers area.
    ///
    /// \author Runik
    /// \date   30/10/2019
    ///
    /// \param  addr    Address to read data from.
    ///
    /// \return Data read.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto readRegisters16(u32 addr) const -> u16;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto Sh2::readRegisters32(u32 addr) -> u32;
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

    auto readRegisters32(u32 addr) const -> u32;

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
    /// \brief  cache purge : all the cache address valid bits and LRU information are initialized to 0 (in 2 ways and 4 ways
    /// mode)
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
    /// \fn auto Sh2::dmaStartConditionsAreSatisfied(DmaChannel dc) const -> bool;
    ///
    /// \brief  Checks if the DMA channel start conditions are satisfied.
    ///
    /// \author Runik
    /// \date   13/01/2019
    ///
    /// \param  dc  DMA channel to check.
    ///
    /// \return True if it succeeds, false if it fails.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto dmaStartConditionsAreSatisfied(DmaChannel dc) const -> bool;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto Sh2::configureDmaTransfer(DmaChannel dc) const -> Sh2DmaConfiguration;
    ///
    /// \brief  Returns a struct configured for the DMA channel passed as parameter.
    ///
    /// \author Runik
    /// \date   01/12/2019
    ///
    /// \param  dc  The DMA channel to configure.
    ///
    /// \return A Sh2DmaConfiguration.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto configureDmaTransfer(DmaChannel dc) const -> Sh2DmaConfiguration;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Sh2::executeDmaOnChannel(Sh2DmaConfiguration& conf);
    ///
    /// \brief  Executes the DMA on the channel specified by the configuration.
    ///
    /// \author Runik
    /// \date   07/12/2019
    ///
    /// \param [in,out] conf    The DMA configuration.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void executeDmaOnChannel(Sh2DmaConfiguration& conf);

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
    /// \fn void Sh2::runInterruptController();
    ///
    /// \brief  Runs the interrupt controller.
    ///
    /// \author Runik
    /// \date   23/11/2019
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void runInterruptController();

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
    /// \fn void Sh2::runFreeRunningTimer(u8 cycles_to_run);
    ///
    /// \brief  Executes the free running timer for the specified number of cycles.
    ///
    /// \author Runik
    /// \date   13/11/2019
    ///
    /// \param  cycles_to_run   Number of cycles to run.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void runFreeRunningTimer(u8 cycles_to_run);

    friend auto xn00(Sh2& s) -> u16;
    friend auto x0n0(Sh2& s) -> u16;
    friend auto x00n(Sh2& s) -> u16;
    friend auto xnnn(Sh2& s) -> u16;
    friend auto x0nn(Sh2& s) -> u16;

    friend void add(Sh2& s);
    friend void addi(Sh2& s);
    friend void addc(Sh2& s);
    friend void addv(Sh2& s);
    friend void and_op(Sh2& s);
    friend void andi(Sh2& s);
    friend void andm(Sh2& s);
    friend void bf(Sh2& s);
    friend void bfs(Sh2& s);
    friend void bra(Sh2& s);
    friend void braf(Sh2& s);
    friend void bsr(Sh2& s);
    friend void bsrf(Sh2& s);
    friend void bt(Sh2& s);
    friend void bts(Sh2& s);
    friend void clrmac(Sh2& s);
    friend void clrt(Sh2& s);
    friend void cmpeq(Sh2& s);
    friend void cmpge(Sh2& s);
    friend void cmpgt(Sh2& s);
    friend void cmphi(Sh2& s);
    friend void cmphs(Sh2& s);
    friend void cmppl(Sh2& s);
    friend void cmppz(Sh2& s);
    friend void cmpstr(Sh2& s);
    friend void cmpim(Sh2& s);
    friend void div0s(Sh2& s);
    friend void div0u(Sh2& s);
    friend void div1(Sh2& s);
    friend void dmuls(Sh2& s);
    friend void dmulu(Sh2& s);
    friend void dt(Sh2& s);
    friend void extsb(Sh2& s);
    friend void extsw(Sh2& s);
    friend void extub(Sh2& s);
    friend void extuw(Sh2& s);
    friend void jmp(Sh2& s);
    friend void jsr(Sh2& s);
    friend void ldcsr(Sh2& s);
    friend void ldcgbr(Sh2& s);
    friend void ldcvbr(Sh2& s);
    friend void ldcmsr(Sh2& s);
    friend void ldcmgbr(Sh2& s);
    friend void ldcmvbr(Sh2& s);
    friend void ldsmach(Sh2& s);
    friend void ldsmacl(Sh2& s);
    friend void ldspr(Sh2& s);
    friend void ldsmmach(Sh2& s);
    friend void ldsmmacl(Sh2& s);
    friend void ldsmpr(Sh2& s);
    friend void mac(Sh2& s);
    friend void macw(Sh2& s);
    friend void mov(Sh2& s);
    friend void movbs(Sh2& s);
    friend void movws(Sh2& s);
    friend void movls(Sh2& s);
    friend void movbl(Sh2& s);
    friend void movwl(Sh2& s);
    friend void movll(Sh2& s);
    friend void movbm(Sh2& s);
    friend void movwm(Sh2& s);
    friend void movlm(Sh2& s);
    friend void movbp(Sh2& s);
    friend void movwp(Sh2& s);
    friend void movlp(Sh2& s);
    friend void movbs0(Sh2& s);
    friend void movws0(Sh2& s);
    friend void movls0(Sh2& s);
    friend void movbl0(Sh2& s);
    friend void movwl0(Sh2& s);
    friend void movll0(Sh2& s);
    friend void movi(Sh2& s);
    friend void movwi(Sh2& s);
    friend void movli(Sh2& s);
    friend void movblg(Sh2& s);
    friend void movwlg(Sh2& s);
    friend void movllg(Sh2& s);
    friend void movbsg(Sh2& s);
    friend void movwsg(Sh2& s);
    friend void movlsg(Sh2& s);
    friend void movbs4(Sh2& s);
    friend void movws4(Sh2& s);
    friend void movls4(Sh2& s);
    friend void movbl4(Sh2& s);
    friend void movwl4(Sh2& s);
    friend void movll4(Sh2& s);
    friend void mova(Sh2& s);
    friend void movt(Sh2& s);
    friend void mull(Sh2& s);
    friend void muls(Sh2& s);
    friend void mulu(Sh2& s);
    friend void neg(Sh2& s);
    friend void negc(Sh2& s);
    friend void nop(Sh2& s);
    friend void not_op(Sh2& s);
    friend void or_op(Sh2& s);
    friend void ori(Sh2& s);
    friend void orm(Sh2& s);
    friend void rotcl(Sh2& s);
    friend void rotcr(Sh2& s);
    friend void rotl(Sh2& s);
    friend void rotr(Sh2& s);
    friend void rte(Sh2& s);
    friend void rts(Sh2& s);
    friend void sett(Sh2& s);
    friend void shal(Sh2& s);
    friend void shar(Sh2& s);
    friend void shll(Sh2& s);
    friend void shll2(Sh2& s);
    friend void shll8(Sh2& s);
    friend void shll16(Sh2& s);
    friend void shlr(Sh2& s);
    friend void shlr2(Sh2& s);
    friend void shlr8(Sh2& s);
    friend void shlr16(Sh2& s);
    friend void sleep(Sh2& s);
    friend void stcsr(Sh2& s);
    friend void stcgbr(Sh2& s);
    friend void stcvbr(Sh2& s);
    friend void stcmsr(Sh2& s);
    friend void stcmgbr(Sh2& s);
    friend void stcmvbr(Sh2& s);
    friend void stsmach(Sh2& s);
    friend void stsmacl(Sh2& s);
    friend void stspr(Sh2& s);
    friend void stsmmach(Sh2& s);
    friend void stsmmacl(Sh2& s);
    friend void stsmpr(Sh2& s);
    friend void sub(Sh2& s);
    friend void subc(Sh2& s);
    friend void subv(Sh2& s);
    friend void swapb(Sh2& s);
    friend void swapw(Sh2& s);
    friend void tas(Sh2& s);
    friend void trapa(Sh2& s);
    friend void tst(Sh2& s);
    friend void tsti(Sh2& s);
    friend void tstm(Sh2& s);
    friend void xor_op(Sh2& s);
    friend void xori(Sh2& s);
    friend void xorm(Sh2& s);
    friend void xtrct(Sh2& s);
    friend void delaySlot(Sh2& s, u32 addr);
    friend void badOpcode(Sh2& s);
    friend void execute(Sh2& s);

    std::array<u8, cache_address_size>   cache_addresses_; ///< Cache addresses (1KB).
    std::array<u8, cache_data_size>      cache_data_;      ///< Cache data (4KB).
    std::array<u8, io_registers_size>    io_registers_;    ///< I/O registers (512B).
    Sh2Type                              sh2_type_;        ///< Type of the SH2.
    std::map<const Sh2Type, const char*> sh2_type_name_
        = {{Sh2Type::master, "Master"}, {Sh2Type::slave, "Slave"}}; ///< Name of the SH2 type, used for logging.

    /// \name Processor registers
    //@{
    Sh2Regs                                   regs_;
    u32                                       pc_;   ///< Program Counter
    u32                                       pr_;   ///< Procedure Register
    u32                                       macl_; ///< Multiply and ACummulate register Low (0x48)
    u32                                       mach_; ///< Multiply and ACummulate register High (0x4C)
    u32                                       vbr_;  ///< Vector Base Register (0x50)
    u32                                       gbr_;  ///< Global Base Register (0x54)
    std::array<u32, general_registers_number> r_;    ///< General registers, last one is the stack pointer (SP) (0x5C)
    //@}

    u8  cycles_elapsed_; ///< CPU cycles used by the last instruction.
    u16 current_opcode_; ///< Opcode to be executed.

    bool is_binary_file_loaded_{false}; ///< True if a binary file has been loaded.
    u32  binary_file_start_address_{};  ///< Start address of the binary file if any.

    std::mutex sh2_mutex_; ///< Handles class data when accessed from another thread.

    /// \name Interrupt management
    //@{
    std::list<Interrupt>                  pending_interrupts_ = {}; ///< List of pending interrupts.
    bool                                  is_interrupted_{};        ///< True if this sh2 is interrupted.
    std::array<bool, max_interrupt_level> is_level_interrupted_{};  ///< Determines if any given level is already interrupted.
    Interrupt                             current_interrupt_{is::undefined}; ///< Interrupt in execution
    //@}

    /// \name DMAC (Direct Memory Access Controller)
    //@{
    DmaNextTransferPriority dmac_next_transfer_priority_{DmaNextTransferPriority::channel_0_first};

    // DmaSourceAddressRegister                   dmac_sar0_;
    // DmaSourceAddressRegister                   dmac_sar1_;
    // DmaDestinationAddressRegister              dmac_dar0_;
    // DmaDestinationAddressRegister              dmac_dar1_;
    // DmaTransferCountRegister                   dmac_tcr0_;
    // DmaTransferCountRegister                   dmac_tcr1_;
    // DmaChannelControlRegister                  dmac_chcr0_;
    // DmaChannelControlRegister                  dmac_chcr1_;
    // DmaRequestResponseSelectionControlRegister dmac_drcr0_;
    // DmaRequestResponseSelectionControlRegister dmac_drcr1_;
    // DmaOperationRegister                       dmac_dmaor_;
    //@}

    /// \name DIVU (Division unit)
    //@{
    bool divu_is_running_{};        ///< True when division unit is in operation
    bool divu_opcode_is_stalled_{}; ///< True when a division unit register is accessed while the division unit is running ... in
                                    ///< that case the access postponed until the needed cycles to complete the operation are
                                    ///< spent.
    u8  divu_remaining_cycles_{};   ///< Remaining cycles to end current division
    s32 divu_quot_{};               ///< Quotient of the division
    s32 divu_rem_{};                ///< Remainder of the division

    // DivisorRegister         divu_dvsr_;
    // DividendRegister32Bits  divu_dvdnt_;
    // DivisionControlRegister divu_dvcr_;
    // DividendRegisterH divu_dvdnth_;
    // DividendRegisterH divu_dvdnth_shadow_;
    // DividendRegisterL divu_dvdntl_;
    // DividendRegisterL divu_dvdntl_shadow_;
    //@}

    /// \name FRT (Free Running Timer)
    //@{
    u32  frt_elapsed_cycles_{}; ///< Elapsed FRT cycles.
    u8   frt_clock_divisor_{};  ///< FRT clock divisor.
    u8   frt_mask_{};           ///< FRT mask.
    bool frt_current_ocr_{};    ///< Current Output Compare Register.

    TimerInterruptEnableRegister          frt_tier_;
    FreeRunningTimerControlStatusRegister frt_ftcsr_;
    FreeRunningCounter                    frt_frc_;
    OutputCompareRegister                 frt_ocra_;
    OutputCompareRegister                 frt_ocrb_;
    TimerControlRegister                  frt_tcr_;
    TimerOutputCompareControlRegister     frt_tocr_;
    InputCaptureRegister                  frt_icr_;
    //@}

    /// \name WDT (Watchdog Timer)
    //@{
    WatchdogTimerControlStatusRegister wdt_wtcsr_;
    WatchdogTimerCounter               wdt_wtcnt_;
    ResetControlStatusRegister         wdt_rstcsr_;
    //@}

    /// \name SCI (Serial Communication Interface)
    //@{
    ReceiveShiftRegister  sci_rsr_;
    ReceiveDataRegister   sci_rdr_;
    TransmitShiftRegister sci_tsr_;
    TransmitDataRegister  sci_tdr_;
    SerialModeRegister    sci_smr_;
    SerialControlRegister sci_scr_;
    SerialStatusRegister  sci_ssr_;
    BitRateRegister       sci_brr_;
    //@}

    /// \name Power-Down modes
    //@{
    StandbyControlRegister sbycr_;
    //@}

    std::vector<CallstackItem>          callstack_;                    ///< Callstack of the processor
    size_t                              step_over_subroutine_depth_{}; ///< Subroutine depth, used with DebugStatus::step_over
    std::array<u32, breakpoints_number> breakpoints_;                  ///< Breakpoints on current CPU program counter.

    bool is_nmi_registered_{false}; ///< True if a Non Maskable Interrupt is registered
};

} // namespace saturnin::sh2
