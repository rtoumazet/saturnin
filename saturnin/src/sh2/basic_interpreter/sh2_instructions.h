//
// sh2_instructions.h
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
/// \file	sh2_instructions.h
///
/// \brief	Declares SH2 instructions
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <array>  // array
#include <string> // string
#include <saturnin/src/emulator_defs.h>
#include <saturnin/src/sh2/sh2_shared.h>
#include <saturnin/src/sh2/sh2_disasm.h>

namespace saturnin::sh2 {
// Forward declarations
class Sh2;
} // namespace saturnin::sh2

namespace saturnin::sh2::basic_interpreter {

extern void badOpcode(Sh2& s);

struct BasicInterpreter {
    /// \name SH2 instructions
    //@{
    static void add(Sh2& s);
    static void addi(Sh2& s);
    static void addc(Sh2& s);
    static void addv(Sh2& s);
    static void and_op(Sh2& s);
    static void andi(Sh2& s);
    static void andm(Sh2& s);
    static void bf(Sh2& s);
    static void bfs(Sh2& s);
    static void bra(Sh2& s);
    static void braf(Sh2& s);
    static void bsr(Sh2& s);
    static void bsrf(Sh2& s);
    static void bt(Sh2& s);
    static void bts(Sh2& s);
    static void clrmac(Sh2& s);
    static void clrt(Sh2& s);
    static void cmpeq(Sh2& s);
    static void cmpge(Sh2& s);
    static void cmpgt(Sh2& s);
    static void cmphi(Sh2& s);
    static void cmphs(Sh2& s);
    static void cmppl(Sh2& s);
    static void cmppz(Sh2& s);
    static void cmpstr(Sh2& s);
    static void cmpim(Sh2& s);
    static void div0s(Sh2& s);
    static void div0u(Sh2& s);
    static void div1(Sh2& s);
    static void dmuls(Sh2& s);
    static void dmulu(Sh2& s);
    static void dt(Sh2& s);
    static void extsb(Sh2& s);
    static void extsw(Sh2& s);
    static void extub(Sh2& s);
    static void extuw(Sh2& s);
    static void jmp(Sh2& s);
    static void jsr(Sh2& s);
    static void ldcsr(Sh2& s);
    static void ldcgbr(Sh2& s);
    static void ldcvbr(Sh2& s);
    static void ldcmsr(Sh2& s);
    static void ldcmgbr(Sh2& s);
    static void ldcmvbr(Sh2& s);
    static void ldsmach(Sh2& s);
    static void ldsmacl(Sh2& s);
    static void ldspr(Sh2& s);
    static void ldsmmach(Sh2& s);
    static void ldsmmacl(Sh2& s);
    static void ldsmpr(Sh2& s);
    static void mac(Sh2& s);
    static void macw(Sh2& s);
    static void mov(Sh2& s);
    static void movbs(Sh2& s);
    static void movws(Sh2& s);
    static void movls(Sh2& s);
    static void movbl(Sh2& s);
    static void movwl(Sh2& s);
    static void movll(Sh2& s);
    static void movbm(Sh2& s);
    static void movwm(Sh2& s);
    static void movlm(Sh2& s);
    static void movbp(Sh2& s);
    static void movwp(Sh2& s);
    static void movlp(Sh2& s);
    static void movbs0(Sh2& s);
    static void movws0(Sh2& s);
    static void movls0(Sh2& s);
    static void movbl0(Sh2& s);
    static void movwl0(Sh2& s);
    static void movll0(Sh2& s);
    static void movi(Sh2& s);
    static void movwi(Sh2& s);
    static void movli(Sh2& s);
    static void movblg(Sh2& s);
    static void movwlg(Sh2& s);
    static void movllg(Sh2& s);
    static void movbsg(Sh2& s);
    static void movwsg(Sh2& s);
    static void movlsg(Sh2& s);
    static void movbs4(Sh2& s);
    static void movws4(Sh2& s);
    static void movls4(Sh2& s);
    static void movbl4(Sh2& s);
    static void movwl4(Sh2& s);
    static void movll4(Sh2& s);
    static void mova(Sh2& s);
    static void movt(Sh2& s);
    static void mull(Sh2& s);
    static void muls(Sh2& s);
    static void mulu(Sh2& s);
    static void neg(Sh2& s);
    static void negc(Sh2& s);
    static void nop(Sh2& s);
    static void not_op(Sh2& s);
    static void or_op(Sh2& s);
    static void ori(Sh2& s);
    static void orm(Sh2& s);
    static void rotcl(Sh2& s);
    static void rotcr(Sh2& s);
    static void rotl(Sh2& s);
    static void rotr(Sh2& s);
    static void rte(Sh2& s);
    static void rts(Sh2& s);
    static void sett(Sh2& s);
    static void shal(Sh2& s);
    static void shar(Sh2& s);
    static void shll(Sh2& s);
    static void shll2(Sh2& s);
    static void shll8(Sh2& s);
    static void shll16(Sh2& s);
    static void shlr(Sh2& s);
    static void shlr2(Sh2& s);
    static void shlr8(Sh2& s);
    static void shlr16(Sh2& s);
    static void sleep(Sh2& s);
    static void stcsr(Sh2& s);
    static void stcgbr(Sh2& s);
    static void stcvbr(Sh2& s);
    static void stcmsr(Sh2& s);
    static void stcmgbr(Sh2& s);
    static void stcmvbr(Sh2& s);
    static void stsmach(Sh2& s);
    static void stsmacl(Sh2& s);
    static void stspr(Sh2& s);
    static void stsmmach(Sh2& s);
    static void stsmmacl(Sh2& s);
    static void stsmpr(Sh2& s);
    static void sub(Sh2& s);
    static void subc(Sh2& s);
    static void subv(Sh2& s);
    static void swapb(Sh2& s);
    static void swapw(Sh2& s);
    static void tas(Sh2& s);
    static void trapa(Sh2& s);
    static void tst(Sh2& s);
    static void tsti(Sh2& s);
    static void tstm(Sh2& s);
    static void xor_op(Sh2& s);
    static void xori(Sh2& s);
    static void xorm(Sh2& s);
    static void xtrct(Sh2& s);
    //@}

    static void delaySlot(Sh2& s, u32 addr);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	void badOpcode(Sh2& s);
    ///
    /// \brief	Function called when an invalid opcode is reached.
    ///
    /// \author	Runik
    /// \date	13/05/2023
    ///
    /// \param [in,out]	s	Sh2 processor to process.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static void badOpcode(Sh2& s);

    static void execute(Sh2& s);

    static void updateDebugStatus(Sh2& s);

    static void initializeDebugStatus(Sh2& s);
};

static std::array<bool, opcodes_lut_size>
    illegal_instruction_lut; ///< The illegal instruction LUT, used for instruction fast fetching

static std::array<bool, opcodes_lut_size> calls_subroutine_lut;

void initializeOpcodesLut();

inline auto isInstructionIllegal(u16 inst) -> bool;

} // namespace saturnin::sh2::basic_interpreter
