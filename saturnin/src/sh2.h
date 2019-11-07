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
#include "sh2_registers.h"

//namespace core = saturnin::core;

// Forward declarations
namespace saturnin::core { 
    class Emulator_context; 
    class Scu;
    struct Interrupt;
}

namespace saturnin {
namespace sh2 {

using saturnin::core::Emulator_context;
using saturnin::core::Memory;
using saturnin::core::Scu;
using saturnin::core::Interrupt;
using saturnin::core::Log;

using saturnin::core::rawRead;
using saturnin::core::rawWrite;

constexpr u8 max_interrupt_number = 10;
constexpr u8 max_interrupt_level  = 10;

constexpr u32 sh2_memory_mask = 0x1FF;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   Sh2Type
///
/// \brief  Type of SH2 CPU.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Sh2Type {
    unknown,
    master,
    slave
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
    Sh2(Sh2Type st, Emulator_context* ec);
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
    T readRegisters(const u32 addr) {
        return rawRead<T>(this->io_registers_, addr & sh2_memory_mask);
    }
    // 8 bits specialization
    template<>
    u8 readRegisters<u8>(const u32 addr) {
        return readRegisters8(addr);
    }
    // 16 bits specialization
    template<>
    u16 readRegisters<u16>(const u32 addr) {
        return readRegisters16(addr);
    }
    // 32 bits specialization
    template<>
    u32 readRegisters<u32>(const u32 addr) {
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

    void sendInterrupt(const core::Interrupt& i);

    template<typename T>
    void unmappedAccess(const u32 addr, const T data) const {
        Log::warning("sh2", "Unmapped write access : address :{:#0x} data:{:#0x}", addr, data);
    }

    void unmappedAccess(const u32 addr) const {
        Log::warning("sh2", "Unmapped read access : address :{:#0x}", addr);
    }

private:

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn u8 Sh2::readRegisters8(const u32 addr);
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

    u8 readRegisters8(const u32 addr);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn u16 Sh2::readRegisters16(const u32 addr);
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

    u16 readRegisters16(const u32 addr);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn u32 Sh2::readRegisters32(const u32 addr);
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

    u32 readRegisters32(const u32 addr);

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
    /// \fn void Sh2::powerOnReset();
    ///
    /// \brief  Initializes the values of general registers.
    ///
    /// \author Runik
    /// \date   21/09/2019
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void powerOnReset();

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
    /// \fn void Sh2::runDivisionUnit(const u8 cycles_to_run);
    ///
    /// \brief  Executes the division unit for the specified number of cycles.
    ///
    /// \author Runik
    /// \date   18/01/2019
    ///
    /// \param  cycles_to_run   Number of cycles to run.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void runDivisionUnit(const u8 cycles_to_run);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn Memory* Sh2::memory() const;
    ///
    /// \brief  Returns the memory object.
    ///
    /// \author Runik
    /// \date   09/02/2019
    ///
    /// \return SCU memory array.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    Memory* memory() const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn Scu* Sh2::scu() const;
    ///
    /// \brief  Returns the SCU object.
    ///
    /// \author Runik
    /// \date   10/02/2019
    ///
    /// \return The SCU object.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    Scu* scu() const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn  Emulator_context* Sh2::emultaorContext() const;
    ///
    /// \brief  Returns the emulator_context object.
    ///
    /// \author Runik
    /// \date   23/09/2019
    ///
    /// \return The Emulator_context object.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    Emulator_context* emulatorContext();

    friend u16 xn00(Sh2&);
    friend u16 x0n0(Sh2&);
    friend u16 x00n(Sh2&);
    friend u16 xnnn(Sh2&);
    friend u16 x0nn(Sh2&);

    friend void add(Sh2&);
    friend void addi(Sh2& s);
    friend void addc(Sh2& s);
    friend void addv(Sh2& s);
    friend void and(Sh2& s);
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
    friend void not(Sh2& s);
    friend void or (Sh2& s);
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
    //friend void stmsr(Sh2& s);
    //friend void stmgbr(Sh2& s);
    //friend void stmvbr(Sh2& s);
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
    friend void xor(Sh2& s);
    friend void xori(Sh2& s);
    friend void xorm(Sh2& s);
    friend void xtrct(Sh2& s);
    friend void delaySlot(Sh2& s, u32 addr);
    friend void badOpcode(Sh2& s);
    friend void execute(Sh2& s);

    Emulator_context* emulator_context_;    ///< Context of the emulator
    
    std::array <u8, 0x400>  cache_addresses_;   ///< Cache addresses (1KB).
    std::array <u8, 0x1000> cache_data_;        ///< Cache data (4KB).    
    std::array <u8, 0x200>  io_registers_;      ///< I/O registers (512B).
    Sh2Type sh2_type_;                          ///< Type of the SH2.

    /// \name Processor registers
    //@{
    u32 pc_;    ///< Progream Counter
    u32 pr_;    ///< Procedure Register
    u32 macl_;  ///< Multiply and ACummulate register Low (0x48)
    u32 mach_;  ///< Multiply and ACummulate register High (0x4C)
    u32 vbr_;   ///< Vector Base Register (0x50)
    u32 gbr_;   ///< Global Base Register (0x54)
    StatusRegister sr_{0};    ///< Status Register (0x58)
    u32 r_[16]; ///< General registers, last one is the stack pointer (SP) (0x5C)
    //@}

    u8  cycles_elapsed_; ///< CPU cycles used by the last instruction.
    u16 current_opcode_; ///< Opcode to be executed.
                                                
    /// \name Interrupt management
    //@{
    std::list<Interrupt>                    pending_interrupts_ = {};       ///< List of pending interrupts.
    bool                                    is_interrupted_{};        ///< True if this sh2 is interrupted.
    std::array<bool, max_interrupt_level>   is_level_interrupted_{};            ///< Determines if any given level is already interrupted.
    Interrupt                               current_interrupt_{ is::undefined };///< Interrupt in execution
    //@}

    /// \name INTC (Interrupt Controller)
    //@{
    InterruptPriorityLevelSettingRegisterA intc_ipra_;
    InterruptPriorityLevelSettingRegisterB intc_iprb_;

    //@}

    /// \name DIVU (Division unit)
    //@{
    bool    divu_is_running_{};       ///< True when division unit is in operation
    u8      divu_remaining_cycles_{}; ///< Remaining cycles to end current division
    s32     divu_quot_{};             ///< Quotient of the division
    s32     divu_rem_{};              ///< Remainder of the division
    
    DivisorRegister                divu_dvsr_;
    DividendRegister32Bits         divu_dvdnt_;
    DivisionControlRegister        divu_dvcr_;
    VectorNumberSettingRegisterDiv divu_vcrdiv_;
    DividendRegisterH              divu_dvdnth_;
    DividendRegisterL              divu_dvdntl_;
    DividendRegisterL              divu_dvdntl_shadow_;
    //@}

    /// \name FRT (Free Running Timer)
    //@                         {
    u32	    frt_elapsed_cycles_{}; ///< Elapsed FRT cycles. 
    u8	    frt_clock_divisor_{};  ///< FRT clock divisor. 
    u8	    frt_mask_{};           ///< FRT mask. 
    bool    frt_icie_{};           ///< Input Capture Interrupt Enable. 
    bool    frt_ociae_{};          ///< Output Compare Interrupt A Enable. 
    bool    frt_ocibe_{};          ///< Output Compare Interrupt B Enable.
    bool    frt_ovie_{};           ///< Timer Overflow Interrupt Enable. 
    bool    frt_current_ocr_{};    ///< Current Output Compare Register. 
    
    TimerInterruptEnableRegister          frt_tier_;
    FreeRunningTimerControlStatusRegister frt_ftcsr_;
    FreeRunningCounter                    frt_frc_;
    OutputCompareRegister                 frt_ocra_;
    OutputCompareRegister                 frt_ocrb_;
    TimerControlRegister                  frt_tcr_;
    TimerOutputCompareControlRegister     frt_tocr_;
    InputCaptureRegister                  frt_icr_;
    //@}
    
};  


}
}