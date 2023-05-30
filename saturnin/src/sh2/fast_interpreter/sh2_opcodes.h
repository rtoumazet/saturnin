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

namespace saturnin::sh2 {
class Sh2;

using OpcodeFunc = void (*)(Sh2&);

} // namespace saturnin::sh2

namespace saturnin::sh2::fast_interpreter {

struct FastInterpreter {
    static void add(Sh2& s, const u32 n, const u32 m);
    static void addi(Sh2& s, const u32 n, const u32 i);
    static void addc(Sh2& s, const u32 n, const u32 m);
    static void addv(Sh2& s, const u32 n, const u32 m);
    static void and_op(Sh2& s, const u32 n, const u32 m);
    static void andi(Sh2& s, const u32 i);
    static void andm(Sh2& s, const u32 i);
    static void bf(Sh2& s, const u32 d);
    static void bfs(Sh2& s, const u32 d);
    static void bra(Sh2& s, const u32 d);
    static void braf(Sh2& s, const u32 m);
    static void bsr(Sh2& s, const u32 d);
    static void bsrf(Sh2& s, const u32 m);
    static void bt(Sh2& s, const u32 d);
    static void bts(Sh2& s, const u32 d);
    static void clrmac(Sh2& s);
    static void clrt(Sh2& s);
    static void cmpeq(Sh2& s, const u32 n, const u32 m);
    static void cmpge(Sh2& s, const u32 n, const u32 m);
    static void cmpgt(Sh2& s, const u32 n, const u32 m);
    static void cmphi(Sh2& s, const u32 n, const u32 m);
    static void cmphs(Sh2& s, const u32 n, const u32 m);
    static void cmppl(Sh2& s, const u32 n);
    static void cmppz(Sh2& s, const u32 n);
    static void cmpstr(Sh2& s, const u32 n, const u32 m);
    static void cmpim(Sh2& s, const u32 i);
    static void div0s(Sh2& s, const u32 n, const u32 m);
    static void div0u(Sh2& s);
    static void div1(Sh2& s, const u32 n, const u32 m);
    static void dmuls(Sh2& s, const u32 n, const u32 m);
    static void dmulu(Sh2& s, const u32 n, const u32 m);
    static void dt(Sh2& s, const u32 n);
    static void extsb(Sh2& s, const u32 n, const u32 m);
    static void extsw(Sh2& s, const u32 n, const u32 m);
    static void extub(Sh2& s, const u32 n, const u32 m);
    static void extuw(Sh2& s, const u32 n, const u32 m);
    static void jmp(Sh2& s, const u32 m);
    static void jsr(Sh2& s, const u32 m);
    static void ldcsr(Sh2& s, const u32 m);
    static void ldcgbr(Sh2& s, const u32 m);
    static void ldcvbr(Sh2& s, const u32 m);
    static void ldcmsr(Sh2& s, const u32 m);
    static void ldcmgbr(Sh2& s, const u32 m);
    static void ldcmvbr(Sh2& s, const u32 m);
    static void ldsmach(Sh2& s, const u32 m);
    static void ldsmacl(Sh2& s, const u32 m);
    static void ldspr(Sh2& s, const u32 m);
    static void ldsmmach(Sh2& s, const u32 m);
    static void ldsmmacl(Sh2& s, const u32 m);
    static void ldsmpr(Sh2& s, const u32 m);
    static void mac(Sh2& s, const u32 n, const u32 m);
    static void macw(Sh2& s, const u32 n, const u32 m);
    static void mov(Sh2& s, const u32 n, const u32 m);
    static void movbs(Sh2& s, const u32 n, const u32 m);
    static void movws(Sh2& s, const u32 n, const u32 m);
    static void movls(Sh2& s, const u32 n, const u32 m);
    static void movbl(Sh2& s, const u32 n, const u32 m);
    static void movwl(Sh2& s, const u32 n, const u32 m);
    static void movll(Sh2& s, const u32 n, const u32 m);
    static void movbm(Sh2& s, const u32 n, const u32 m);
    static void movwm(Sh2& s, const u32 n, const u32 m);
    static void movlm(Sh2& s, const u32 n, const u32 m);
    static void movbp(Sh2& s, const u32 n, const u32 m);
    static void movwp(Sh2& s, const u32 n, const u32 m);
    static void movlp(Sh2& s, const u32 n, const u32 m);
    static void movbs0(Sh2& s, const u32 n, const u32 m);
    static void movws0(Sh2& s, const u32 n, const u32 m);
    static void movls0(Sh2& s, const u32 n, const u32 m);
    static void movbl0(Sh2& s, const u32 n, const u32 m);
    static void movwl0(Sh2& s, const u32 n, const u32 m);
    static void movll0(Sh2& s, const u32 n, const u32 m);
    static void movi(Sh2& s, const u32 n, const u32 i);
    static void movwi(Sh2& s, const u32 n, const u32 d);
    static void movli(Sh2& s, const u32 n, const u32 d);
    static void movblg(Sh2& s, const u32 d);
    static void movwlg(Sh2& s, const u32 d);
    static void movllg(Sh2& s, const u32 d);
    static void movbsg(Sh2& s, const u32 d);
    static void movwsg(Sh2& s, const u32 d);
    static void movlsg(Sh2& s, const u32 d);
    static void movbs4(Sh2& s, const u32 n, const u32 d);
    static void movws4(Sh2& s, const u32 n, const u32 d);
    static void movls4(Sh2& s, const u32 n, const u32 m, const u32 d);
    static void movbl4(Sh2& s, const u32 m, const u32 d);
    static void movwl4(Sh2& s, const u32 m, const u32 d);
    static void movll4(Sh2& s, const u32 n, const u32 m, const u32 d);
    static void mova(Sh2& s, const u32 d);
    static void movt(Sh2& s, const u32 n);
    static void mull(Sh2& s, const u32 n, const u32 m);
    static void muls(Sh2& s, const u32 n, const u32 m);
    static void mulu(Sh2& s, const u32 n, const u32 m);
    static void neg(Sh2& s, const u32 n, const u32 m);
    static void negc(Sh2& s, const u32 n, const u32 m);
    static void nop(Sh2& s);
    static void not_op(Sh2& s, const u32 n, const u32 m);
    static void or_op(Sh2& s, const u32 n, const u32 m);
    static void ori(Sh2& s, const u32 i);
    static void orm(Sh2& s, const u32 i);
    static void rotcl(Sh2& s, const u32 n);
    static void rotcr(Sh2& s, const u32 n);
    static void rotl(Sh2& s, const u32 n);
    static void rotr(Sh2& s, const u32 n);
    static void rte(Sh2& s);
    static void rts(Sh2& s);
    static void sett(Sh2& s);
    static void shal(Sh2& s, const u32 n);
    static void shar(Sh2& s, const u32 n);
    static void shll(Sh2& s, const u32 n);
    static void shll2(Sh2& s, const u32 n);
    static void shll8(Sh2& s, const u32 n);
    static void shll16(Sh2& s, const u32 n);
    static void shlr(Sh2& s, const u32 n);
    static void shlr2(Sh2& s, const u32 n);
    static void shlr8(Sh2& s, const u32 n);
    static void shlr16(Sh2& s, const u32 n);
    static void sleep(Sh2& s);
    static void stcsr(Sh2& s, const u32 n);
    static void stcgbr(Sh2& s, const u32 n);
    static void stcvbr(Sh2& s, const u32 n);
    static void stcmsr(Sh2& s, const u32 n);
    static void stcmgbr(Sh2& s, const u32 n);
    static void stcmvbr(Sh2& s, const u32 n);
    static void stsmach(Sh2& s, const u32 n);
    static void stsmacl(Sh2& s, const u32 n);
    static void stspr(Sh2& s, const u32 n);
    static void stsmmach(Sh2& s, const u32 n);
    static void stsmmacl(Sh2& s, const u32 n);
    static void stsmpr(Sh2& s, const u32 n);
    static void sub(Sh2& s, const u32 n, const u32 m);
    static void subc(Sh2& s, const u32 n, const u32 m);
    static void subv(Sh2& s, const u32 n, const u32 m);
    static void swapb(Sh2& s, const u32 n, const u32 m);
    static void swapw(Sh2& s, const u32 n, const u32 m);
    static void tas(Sh2& s, const u32 n);
    static void trapa(Sh2& s, const u32 i);
    static void tst(Sh2& s, const u32 n, const u32 m);
    static void tsti(Sh2& s, const u32 i);
    static void tstm(Sh2& s, const u32 i);
    static void xor_op(Sh2& s, const u32 n, const u32 m);
    static void xori(Sh2& s, const u32 i);
    static void xorm(Sh2& s, const u32 i);
    static void xtrct(Sh2& s, const u32 n, const u32 m);

    static void execute(Sh2& s);

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
};

inline auto isInstructionIllegal(u16 inst) -> bool;

} // namespace saturnin::sh2::fast_interpreter
