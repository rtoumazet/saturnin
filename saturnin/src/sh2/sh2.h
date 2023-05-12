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
#include <saturnin/src/sh2/sh2_registers.h>
#include <saturnin/src/sh2/fast_interpreter/sh2_opcodes.h>
#include <saturnin/src/sh2/basic_interpreter/sh2_instructions.h>

// Forward declarations
namespace saturnin::core {
class EmulatorContext;
class EmulatorModules;
class Scu;
struct Interrupt;
} // namespace saturnin::core

namespace saturnin::sh2 {

using ExecuteFunc = void(Sh2&);

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

constexpr auto ignored_delay_slot_address = u32{0x20000202};

enum class Sh2Core { basic_interpreter, fast_interpreter };

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

    friend auto basic_interpreter::xn00(const Sh2& s) -> u16;
    friend auto basic_interpreter::x0n0(const Sh2& s) -> u16;
    friend auto basic_interpreter::x00n(const Sh2& s) -> u16;
    friend auto basic_interpreter::xnnn(const Sh2& s) -> u16;
    friend auto basic_interpreter::x0nn(const Sh2& s) -> u16;

    // friend void basic_interpreter::add(Sh2& s);
    // friend void basic_interpreter::addi(Sh2& s);
    // friend void basic_interpreter::addc(Sh2& s);
    // friend void basic_interpreter::addv(Sh2& s);
    // friend void basic_interpreter::and_op(Sh2& s);
    // friend void basic_interpreter::andi(Sh2& s);
    // friend void basic_interpreter::andm(Sh2& s);
    // friend void basic_interpreter::bf(Sh2& s);
    // friend void basic_interpreter::bfs(Sh2& s);
    // friend void basic_interpreter::bra(Sh2& s);
    // friend void basic_interpreter::braf(Sh2& s);
    // friend void basic_interpreter::bsr(Sh2& s);
    // friend void basic_interpreter::bsrf(Sh2& s);
    // friend void basic_interpreter::bt(Sh2& s);
    // friend void basic_interpreter::bts(Sh2& s);
    // friend void basic_interpreter::clrmac(Sh2& s);
    // friend void basic_interpreter::clrt(Sh2& s);
    // friend void basic_interpreter::cmpeq(Sh2& s);
    // friend void basic_interpreter::cmpge(Sh2& s);
    // friend void basic_interpreter::cmpgt(Sh2& s);
    // friend void basic_interpreter::cmphi(Sh2& s);
    // friend void basic_interpreter::cmphs(Sh2& s);
    // friend void basic_interpreter::cmppl(Sh2& s);
    // friend void basic_interpreter::cmppz(Sh2& s);
    // friend void basic_interpreter::cmpstr(Sh2& s);
    // friend void basic_interpreter::cmpim(Sh2& s);
    // friend void basic_interpreter::div0s(Sh2& s);
    // friend void basic_interpreter::div0u(Sh2& s);
    // friend void basic_interpreter::div1(Sh2& s);
    // friend void basic_interpreter::dmuls(Sh2& s);
    // friend void basic_interpreter::dmulu(Sh2& s);
    // friend void basic_interpreter::dt(Sh2& s);
    // friend void basic_interpreter::extsb(Sh2& s);
    // friend void basic_interpreter::extsw(Sh2& s);
    // friend void basic_interpreter::extub(Sh2& s);
    // friend void basic_interpreter::extuw(Sh2& s);
    // friend void basic_interpreter::jmp(Sh2& s);
    // friend void basic_interpreter::jsr(Sh2& s);
    // friend void basic_interpreter::ldcsr(Sh2& s);
    // friend void basic_interpreter::ldcgbr(Sh2& s);
    // friend void basic_interpreter::ldcvbr(Sh2& s);
    // friend void basic_interpreter::ldcmsr(Sh2& s);
    // friend void basic_interpreter::ldcmgbr(Sh2& s);
    // friend void basic_interpreter::ldcmvbr(Sh2& s);
    // friend void basic_interpreter::ldsmach(Sh2& s);
    // friend void basic_interpreter::ldsmacl(Sh2& s);
    // friend void basic_interpreter::ldspr(Sh2& s);
    // friend void basic_interpreter::ldsmmach(Sh2& s);
    // friend void basic_interpreter::ldsmmacl(Sh2& s);
    // friend void basic_interpreter::ldsmpr(Sh2& s);
    // friend void basic_interpreter::mac(Sh2& s);
    // friend void basic_interpreter::macw(Sh2& s);
    // friend void basic_interpreter::mov(Sh2& s);
    // friend void basic_interpreter::movbs(Sh2& s);
    // friend void basic_interpreter::movws(Sh2& s);
    // friend void basic_interpreter::movls(Sh2& s);
    // friend void basic_interpreter::movbl(Sh2& s);
    // friend void basic_interpreter::movwl(Sh2& s);
    // friend void basic_interpreter::movll(Sh2& s);
    // friend void basic_interpreter::movbm(Sh2& s);
    // friend void basic_interpreter::movwm(Sh2& s);
    // friend void basic_interpreter::movlm(Sh2& s);
    // friend void basic_interpreter::movbp(Sh2& s);
    // friend void basic_interpreter::movwp(Sh2& s);
    // friend void basic_interpreter::movlp(Sh2& s);
    // friend void basic_interpreter::movbs0(Sh2& s);
    // friend void basic_interpreter::movws0(Sh2& s);
    // friend void basic_interpreter::movls0(Sh2& s);
    // friend void basic_interpreter::movbl0(Sh2& s);
    // friend void basic_interpreter::movwl0(Sh2& s);
    // friend void basic_interpreter::movll0(Sh2& s);
    // friend void basic_interpreter::movi(Sh2& s);
    // friend void basic_interpreter::movwi(Sh2& s);
    // friend void basic_interpreter::movli(Sh2& s);
    // friend void basic_interpreter::movblg(Sh2& s);
    // friend void basic_interpreter::movwlg(Sh2& s);
    // friend void basic_interpreter::movllg(Sh2& s);
    // friend void basic_interpreter::movbsg(Sh2& s);
    // friend void basic_interpreter::movwsg(Sh2& s);
    // friend void basic_interpreter::movlsg(Sh2& s);
    // friend void basic_interpreter::movbs4(Sh2& s);
    // friend void basic_interpreter::movws4(Sh2& s);
    // friend void basic_interpreter::movls4(Sh2& s);
    // friend void basic_interpreter::movbl4(Sh2& s);
    // friend void basic_interpreter::movwl4(Sh2& s);
    // friend void basic_interpreter::movll4(Sh2& s);
    // friend void basic_interpreter::mova(Sh2& s);
    // friend void basic_interpreter::movt(Sh2& s);
    // friend void basic_interpreter::mull(Sh2& s);
    // friend void basic_interpreter::muls(Sh2& s);
    // friend void basic_interpreter::mulu(Sh2& s);
    // friend void basic_interpreter::neg(Sh2& s);
    // friend void basic_interpreter::negc(Sh2& s);
    // friend void basic_interpreter::nop(Sh2& s);
    // friend void basic_interpreter::not_op(Sh2& s);
    // friend void basic_interpreter::or_op(Sh2& s);
    // friend void basic_interpreter::ori(Sh2& s);
    // friend void basic_interpreter::orm(Sh2& s);
    // friend void basic_interpreter::rotcl(Sh2& s);
    // friend void basic_interpreter::rotcr(Sh2& s);
    // friend void basic_interpreter::rotl(Sh2& s);
    // friend void basic_interpreter::rotr(Sh2& s);
    // friend void basic_interpreter::rte(Sh2& s);
    // friend void basic_interpreter::rts(Sh2& s);
    // friend void basic_interpreter::sett(Sh2& s);
    // friend void basic_interpreter::shal(Sh2& s);
    // friend void basic_interpreter::shar(Sh2& s);
    // friend void basic_interpreter::shll(Sh2& s);
    // friend void basic_interpreter::shll2(Sh2& s);
    // friend void basic_interpreter::shll8(Sh2& s);
    // friend void basic_interpreter::shll16(Sh2& s);
    // friend void basic_interpreter::shlr(Sh2& s);
    // friend void basic_interpreter::shlr2(Sh2& s);
    // friend void basic_interpreter::shlr8(Sh2& s);
    // friend void basic_interpreter::shlr16(Sh2& s);
    // friend void basic_interpreter::sleep(Sh2& s);
    // friend void basic_interpreter::stcsr(Sh2& s);
    // friend void basic_interpreter::stcgbr(Sh2& s);
    // friend void basic_interpreter::stcvbr(Sh2& s);
    // friend void basic_interpreter::stcmsr(Sh2& s);
    // friend void basic_interpreter::stcmgbr(Sh2& s);
    // friend void basic_interpreter::stcmvbr(Sh2& s);
    // friend void basic_interpreter::stsmach(Sh2& s);
    // friend void basic_interpreter::stsmacl(Sh2& s);
    // friend void basic_interpreter::stspr(Sh2& s);
    // friend void basic_interpreter::stsmmach(Sh2& s);
    // friend void basic_interpreter::stsmmacl(Sh2& s);
    // friend void basic_interpreter::stsmpr(Sh2& s);
    // friend void basic_interpreter::sub(Sh2& s);
    // friend void basic_interpreter::subc(Sh2& s);
    // friend void basic_interpreter::subv(Sh2& s);
    // friend void basic_interpreter::swapb(Sh2& s);
    // friend void basic_interpreter::swapw(Sh2& s);
    // friend void basic_interpreter::tas(Sh2& s);
    // friend void basic_interpreter::trapa(Sh2& s);
    // friend void basic_interpreter::tst(Sh2& s);
    // friend void basic_interpreter::tsti(Sh2& s);
    // friend void basic_interpreter::tstm(Sh2& s);
    // friend void basic_interpreter::xor_op(Sh2& s);
    // friend void basic_interpreter::xori(Sh2& s);
    // friend void basic_interpreter::xorm(Sh2& s);
    // friend void basic_interpreter::xtrct(Sh2& s);
    friend struct basic_interpreter::BasicInterpreter;
    friend struct fast_interpreter::FastInterpreter;

    friend void delaySlot(Sh2& s, u32 addr);
    friend void badOpcode(Sh2& s);
    friend void basic_interpreter::execute(Sh2& s);

    std::array<u8, cache_address_size>   cache_addresses_;          ///< Cache addresses (1KB).
    std::array<u8, cache_data_size>      cache_data_;               ///< Cache data (4KB).
    std::array<u8, io_registers_size>    io_registers_;             ///< I/O registers (512B).
    Sh2Type                              sh2_type_;                 ///< Type of the SH2.
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

    u8  cycles_elapsed_;                ///< CPU cycles used by the last instruction.
    u16 current_opcode_;                ///< Opcode to be executed.

    bool is_binary_file_loaded_{false}; ///< True if a binary file has been loaded.
    u32  binary_file_start_address_{};  ///< Start address of the binary file if any.

    std::mutex sh2_mutex_;              ///< Handles class data when accessed from another thread.

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
    //@}

    /// \name FRT (Free Running Timer)
    //@{
    u32  frt_elapsed_cycles_{}; ///< Elapsed FRT cycles.
    u8   frt_clock_divisor_{};  ///< FRT clock divisor.
    u8   frt_mask_{};           ///< FRT mask.
    bool frt_current_ocr_{};    ///< Current Output Compare Register.
    //@}

    std::vector<CallstackItem>          callstack_;                    ///< Callstack of the processor
    size_t                              step_over_subroutine_depth_{}; ///< Subroutine depth, used with DebugStatus::step_over
    std::array<u32, breakpoints_number> breakpoints_;                  ///< Breakpoints on current CPU program counter.

    bool is_nmi_registered_{false};                                    ///< True if a Non Maskable Interrupt is registered
};

inline auto isInstructionIllegal(u16 inst) -> bool;

inline void delaySlot(Sh2& s, u32 addr);

void badOpcode(Sh2& s);

// std::function<ExecuteFunc> execute;

} // namespace saturnin::sh2
