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

extern void delaySlot(Sh2& s, u32 addr);
} // namespace saturnin::sh2

namespace saturnin::sh2::fast_interpreter {

struct FastInterpreter {
    void add(Sh2& s, const u32 n, const u32 m) const;
    void addi(Sh2& s, const u32 n, const u32 i) const;
    void addc(Sh2& s, const u32 n, const u32 m) const;
    void addv(Sh2& s, const u32 n, const u32 m) const;
    void and_op(Sh2& s, const u32 n, const u32 m) const;
    void andi(Sh2& s, const u32 i) const;
    void andm(Sh2& s, const u32 i) const;
    void bf(Sh2& s, const u32 d) const;
    void bfs(Sh2& s, const u32 d) const;
    void bra(Sh2& s, const u32 d) const;
    void braf(Sh2& s, const u32 m) const;
    void bsr(Sh2& s, const u32 d) const;
    void bsrf(Sh2& s, const u32 m) const;
    void bt(Sh2& s, const u32 d) const;
    void bts(Sh2& s, const u32 d) const;
    void clrmac(Sh2& s) const;
    void clrt(Sh2& s) const;
    void cmpeq(Sh2& s, const u32 n, const u32 m) const;
    void cmpge(Sh2& s, const u32 n, const u32 m) const;
    void cmpgt(Sh2& s, const u32 n, const u32 m) const;
    void cmphi(Sh2& s, const u32 n, const u32 m) const;
    void cmphs(Sh2& s, const u32 n, const u32 m) const;
    void cmppl(Sh2& s, const u32 n) const;
    void cmppz(Sh2& s, const u32 n) const;
    void cmpstr(Sh2& s, const u32 n, const u32 m) const;
    void cmpim(Sh2& s, const u32 i) const;
    void div0s(Sh2& s, const u32 n, const u32 m) const;
    void div0u(Sh2& s) const;
    void div1(Sh2& s, const u32 n, const u32 m) const;
    void dmuls(Sh2& s, const u32 n, const u32 m) const;
    void dmulu(Sh2& s, const u32 n, const u32 m) const;
    void dt(Sh2& s, const u32 n) const;
    void extsb(Sh2& s, const u32 n, const u32 m) const;
    void extsw(Sh2& s, const u32 n, const u32 m) const;
    void extub(Sh2& s, const u32 n, const u32 m) const;
    void extuw(Sh2& s, const u32 n, const u32 m) const;
    void jmp(Sh2& s, const u32 m) const;
    void jsr(Sh2& s, const u32 m) const;
    void ldcsr(Sh2& s, const u32 m) const;
    void ldcgbr(Sh2& s, const u32 m) const;
    void ldcvbr(Sh2& s, const u32 m) const;
    void ldcmsr(Sh2& s, const u32 m) const;
    void ldcmgbr(Sh2& s, const u32 m) const;
    void ldcmvbr(Sh2& s, const u32 m) const;
    void ldsmach(Sh2& s, const u32 m) const;
    void ldsmacl(Sh2& s, const u32 m) const;
    void ldspr(Sh2& s, const u32 m) const;
    void ldsmmach(Sh2& s, const u32 m) const;
    void ldsmmacl(Sh2& s, const u32 m) const;
    void ldsmpr(Sh2& s, const u32 m) const;
    void mac(Sh2& s, const u32 n, const u32 m) const;
    void macw(Sh2& s, const u32 n, const u32 m) const;
    void mov(Sh2& s, const u32 n, const u32 m) const;
    void movbs(Sh2& s, const u32 n, const u32 m) const;
    void movws(Sh2& s, const u32 n, const u32 m) const;
    void movls(Sh2& s, const u32 n, const u32 m) const;
    void movbl(Sh2& s, const u32 n, const u32 m) const;
    void movwl(Sh2& s, const u32 n, const u32 m) const;
    void movll(Sh2& s, const u32 n, const u32 m) const;
    void movbm(Sh2& s, const u32 n, const u32 m) const;
    void movwm(Sh2& s, const u32 n, const u32 m) const;
    void movlm(Sh2& s, const u32 n, const u32 m) const;
    void movbp(Sh2& s, const u32 n, const u32 m) const;
    void movwp(Sh2& s, const u32 n, const u32 m) const;
    void movlp(Sh2& s, const u32 n, const u32 m) const;
    void movbs0(Sh2& s, const u32 n, const u32 m) const;
    void movws0(Sh2& s, const u32 n, const u32 m) const;
    void movls0(Sh2& s, const u32 n, const u32 m) const;
    void movbl0(Sh2& s, const u32 n, const u32 m) const;
    void movwl0(Sh2& s, const u32 n, const u32 m) const;
    void movll0(Sh2& s, const u32 n, const u32 m) const;
    void movi(Sh2& s, const u32 n, const u32 i) const;
    void movwi(Sh2& s, const u32 n, const u32 d) const;
    void movli(Sh2& s, const u32 n, const u32 d) const;
    void movblg(Sh2& s, const u32 d) const;
    void movwlg(Sh2& s, const u32 d) const;
    void movllg(Sh2& s, const u32 d) const;
    void movbsg(Sh2& s, const u32 d) const;
    void movwsg(Sh2& s, const u32 d) const;
    void movlsg(Sh2& s, const u32 d) const;
    void movbs4(Sh2& s, const u32 n, const u32 d) const;
    void movws4(Sh2& s, const u32 n, const u32 d) const;
    void movls4(Sh2& s, const u32 n, const u32 m, const u32 d) const;
    void movbl4(Sh2& s, const u32 m, const u32 d) const;
    void movwl4(Sh2& s, const u32 m, const u32 d) const;
    void movll4(Sh2& s, const u32 n, const u32 m, const u32 d) const;
    void mova(Sh2& s, const u32 d) const;
    void movt(Sh2& s, const u32 n) const;
    void mull(Sh2& s, const u32 n, const u32 m) const;
    void muls(Sh2& s, const u32 n, const u32 m) const;
    void mulu(Sh2& s, const u32 n, const u32 m) const;
    void neg(Sh2& s, const u32 n, const u32 m) const;
    void negc(Sh2& s, const u32 n, const u32 m) const;
    void nop(Sh2& s) const;
    void not_op(Sh2& s, const u32 n, const u32 m) const;
    void or_op(Sh2& s, const u32 n, const u32 m) const;
    void ori(Sh2& s, const u32 i) const;
    void orm(Sh2& s, const u32 i) const;
    void rotcl(Sh2& s, const u32 n) const;
    void rotcr(Sh2& s, const u32 n) const;
    void rotl(Sh2& s, const u32 n) const;
    void rotr(Sh2& s, const u32 n) const;
    void rte(Sh2& s) const;
    void rts(Sh2& s) const;
    void sett(Sh2& s) const;
    void shal(Sh2& s, const u32 n) const;
    void shar(Sh2& s, const u32 n) const;
    void shll(Sh2& s, const u32 n) const;
    void shll2(Sh2& s, const u32 n) const;
    void shll8(Sh2& s, const u32 n) const;
    void shll16(Sh2& s, const u32 n) const;
    void shlr(Sh2& s, const u32 n) const;
    void shlr2(Sh2& s, const u32 n) const;
    void shlr8(Sh2& s, const u32 n) const;
    void shlr16(Sh2& s, const u32 n) const;
    void sleep(Sh2& s) const;
    void stcsr(Sh2& s, const u32 n) const;
    void stcgbr(Sh2& s, const u32 n) const;
    void stcvbr(Sh2& s, const u32 n) const;
    void stcmsr(Sh2& s, const u32 n) const;
    void stcmgbr(Sh2& s, const u32 n) const;
    void stcmvbr(Sh2& s, const u32 n) const;
    void stsmach(Sh2& s, const u32 n) const;
    void stsmacl(Sh2& s, const u32 n) const;
    void stspr(Sh2& s, const u32 n) const;
    void stsmmach(Sh2& s, const u32 n) const;
    void stsmmacl(Sh2& s, const u32 n) const;
    void stsmpr(Sh2& s, const u32 n) const;
    void sub(Sh2& s, const u32 n, const u32 m) const;
    void subc(Sh2& s, const u32 n, const u32 m) const;
    void subv(Sh2& s, const u32 n, const u32 m) const;
    void swapb(Sh2& s, const u32 n, const u32 m) const;
    void swapw(Sh2& s, const u32 n, const u32 m) const;
    void tas(Sh2& s, const u32 n) const;
    void trapa(Sh2& s, const u32 i) const;
    void tst(Sh2& s, const u32 n, const u32 m) const;
    void tsti(Sh2& s, const u32 i) const;
    void tstm(Sh2& s, const u32 i) const;
    void xor_op(Sh2& s, const u32 n, const u32 m) const;
    void xori(Sh2& s, const u32 i) const;
    void xorm(Sh2& s, const u32 i) const;
    void xtrct(Sh2& s, const u32 n, const u32 m) const;

    void execute(Sh2& s) const;
};
} // namespace saturnin::sh2::fast_interpreter
