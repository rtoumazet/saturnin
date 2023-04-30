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

extern void delaySlot(Sh2& s, u32 addr);
} // namespace saturnin::sh2

namespace saturnin::sh2::fast_interpreter {
void add(Sh2& s, const u32 n, const u32 m);
void addi(Sh2& s, const u32 n, const u32 i);
void addc(Sh2& s, const u32 n, const u32 m);
void addv(Sh2& s, const u32 n, const u32 m);
void and_op(Sh2& s, const u32 n, const u32 m);
void andi(Sh2& s, const u32 i);
void andm(Sh2& s, const u32 i);
void bf(Sh2& s, const u32 d);
void bfs(Sh2& s, const u32 d);
void bra(Sh2& s, const u32 d);
void braf(Sh2& s, const u32 m);
void bsr(Sh2& s, const u32 d);
void bsrf(Sh2& s, const u32 m);
void bt(Sh2& s, const u32 d);
void bts(Sh2& s, const u32 d);
void clrmac(Sh2& s);
void clrt(Sh2& s);
void cmpeq(Sh2& s, const u32 n, const u32 m);
void cmpge(Sh2& s, const u32 n, const u32 m);
void cmpgt(Sh2& s, const u32 n, const u32 m);
void cmphi(Sh2& s, const u32 n, const u32 m);
void cmphs(Sh2& s, const u32 n, const u32 m);
void cmppl(Sh2& s, const u32 n);
void cmppz(Sh2& s, const u32 n);
void cmpstr(Sh2& s, const u32 n, const u32 m);
void cmpim(Sh2& s, const u32 i);
void div0s(Sh2& s, const u32 n, const u32 m);
void div0u(Sh2& s);
void div1(Sh2& s, const u32 n, const u32 m);
void dmuls(Sh2& s, const u32 n, const u32 m);
void dmulu(Sh2& s, const u32 n, const u32 m);
void dt(Sh2& s, const u32 n);
void extsb(Sh2& s, const u32 n, const u32 m);
void extsw(Sh2& s, const u32 n, const u32 m);
void extub(Sh2& s, const u32 n, const u32 m);
void extuw(Sh2& s, const u32 n, const u32 m);
void jmp(Sh2& s, const u32 m);
void jsr(Sh2& s, const u32 m);
void ldcsr(Sh2& s, const u32 m);
void ldcgbr(Sh2& s, const u32 m);
void ldcvbr(Sh2& s, const u32 m);
void ldcmsr(Sh2& s, const u32 m);
void ldcmgbr(Sh2& s, const u32 m);
void ldcmvbr(Sh2& s, const u32 m);
void ldsmach(Sh2& s, const u32 m);
void ldsmacl(Sh2& s, const u32 m);
void ldspr(Sh2& s, const u32 m);
void ldsmmach(Sh2& s, const u32 m);
void ldsmmacl(Sh2& s, const u32 m);
void ldsmpr(Sh2& s, const u32 m);
void mac(Sh2& s, const u32 n, const u32 m);
void macw(Sh2& s, const u32 n, const u32 m);
void mov(Sh2& s, const u32 n, const u32 m);
void movbs(Sh2& s, const u32 n, const u32 m);
void movws(Sh2& s, const u32 n, const u32 m);
void movls(Sh2& s, const u32 n, const u32 m);
void movbl(Sh2& s, const u32 n, const u32 m);
void movwl(Sh2& s, const u32 n, const u32 m);
void movll(Sh2& s, const u32 n, const u32 m);
void movbm(Sh2& s, const u32 n, const u32 m);
void movwm(Sh2& s, const u32 n, const u32 m);
void movlm(Sh2& s, const u32 n, const u32 m);
void movbp(Sh2& s, const u32 n, const u32 m);
void movwp(Sh2& s, const u32 n, const u32 m);
void movlp(Sh2& s, const u32 n, const u32 m);
void movbs0(Sh2& s, const u32 n, const u32 m);
void movws0(Sh2& s, const u32 n, const u32 m);
void movls0(Sh2& s, const u32 n, const u32 m);
void movbl0(Sh2& s, const u32 n, const u32 m);
void movwl0(Sh2& s, const u32 n, const u32 m);
void movll0(Sh2& s, const u32 n, const u32 m);
void movi(Sh2& s, const u32 n, const u32 i);
void movwi(Sh2& s, const u32 n, const u32 d);
void movli(Sh2& s, const u32 n, const u32 d);
void movblg(Sh2& s, const u32 d);
void movwlg(Sh2& s, const u32 d);
void movllg(Sh2& s, const u32 d);
void movbsg(Sh2& s, const u32 d);
void movwsg(Sh2& s, const u32 d);
void movlsg(Sh2& s, const u32 d);
void movbs4(Sh2& s, const u32 n, const u32 d);
void movws4(Sh2& s, const u32 n, const u32 d);
void movls4(Sh2& s, const u32 n, const u32 m, const u32 d);
void movbl4(Sh2& s, const u32 m, const u32 d);
void movwl4(Sh2& s, const u32 m, const u32 d);
void movll4(Sh2& s, const u32 n, const u32 m, const u32 d);
void mova(Sh2& s, const u32 d);
void movt(Sh2& s, const u32 n);
void mull(Sh2& s, const u32 n, const u32 m);
void muls(Sh2& s, const u32 n, const u32 m);
void mulu(Sh2& s, const u32 n, const u32 m);
void neg(Sh2& s, const u32 n, const u32 m);
void negc(Sh2& s, const u32 n, const u32 m);
void nop(Sh2& s);
void not_op(Sh2& s, const u32 n, const u32 m);
void or_op(Sh2& s, const u32 n, const u32 m);
void ori(Sh2& s, const u32 i);
void orm(Sh2& s, const u32 i);
void rotcl(Sh2& s, const u32 n);
void rotcr(Sh2& s, const u32 n);
void rotl(Sh2& s, const u32 n);
void rotr(Sh2& s, const u32 n);
void rte(Sh2& s);
void rts(Sh2& s);
void sett(Sh2& s);
void shal(Sh2& s, const u32 n);
void shar(Sh2& s, const u32 n);
void shll(Sh2& s, const u32 n);
void shll2(Sh2& s, const u32 n);
void shll8(Sh2& s, const u32 n);
void shll16(Sh2& s, const u32 n);
void shlr(Sh2& s, const u32 n);
void shlr2(Sh2& s, const u32 n);
void shlr8(Sh2& s, const u32 n);
void shlr16(Sh2& s, const u32 n);
void sleep(Sh2& s);
void stcsr(Sh2& s, const u32 n);
void stcgbr(Sh2& s, const u32 n);
void stcvbr(Sh2& s, const u32 n);
void stcmsr(Sh2& s, const u32 n);
void stcmgbr(Sh2& s, const u32 n);
void stcmvbr(Sh2& s, const u32 n);
void stsmach(Sh2& s, const u32 n);
void stsmacl(Sh2& s, const u32 n);
void stspr(Sh2& s, const u32 n);
void stsmmach(Sh2& s, const u32 n);
void stsmmacl(Sh2& s, const u32 n);
void stsmpr(Sh2& s, const u32 n);
void sub(Sh2& s, const u32 n, const u32 m);
void subc(Sh2& s, const u32 n, const u32 m);
void subv(Sh2& s, const u32 n, const u32 m);
void swapb(Sh2& s, const u32 n, const u32 m);
void swapw(Sh2& s, const u32 n, const u32 m);
void tas(Sh2& s, const u32 n);
void trapa(Sh2& s, const u32 i);
void tst(Sh2& s, const u32 n, const u32 m);
void tsti(Sh2& s, const u32 i);
void tstm(Sh2& s, const u32 i);
void xor_op(Sh2& s, const u32 n, const u32 m);
void xori(Sh2& s, const u32 i);
void xorm(Sh2& s, const u32 i);
void xtrct(Sh2& s, const u32 n, const u32 m);
} // namespace saturnin::sh2::fast_interpreter
