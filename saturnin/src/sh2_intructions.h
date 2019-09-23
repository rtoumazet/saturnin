// 
// sh2_instructions.h
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
/// \file	sh2_instructions.h
///
/// \brief	Declares SH2 instructions 
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "emulator_defs.h"

namespace saturnin {
namespace core {

// Forward declarations
class Sh2;

/// \name Instruction split functions.
//@{
inline u8  xn000(const u16 inst) { return static_cast<u8>((inst & 0xF000) >> 12); }
inline u8  x0n00(const u16 inst) { return static_cast<u8>((inst & 0xF00) >> 8); }
inline u8  x00n0(const u16 inst) { return static_cast<u8>((inst & 0xF0) >> 4); }
inline u16 x0nnn(const u16 inst) { return (inst & 0xFFF); }
inline u8  x00nn(const u16 inst) { return static_cast<u8>(inst & 0xFF); }
inline u8  x000n(const u16 inst) { return static_cast<u8>(inst & 0xF); }
//@}

/// \name Opcode decoding helpers
//@{
inline u16 xn00(Sh2& s);
inline u16 x0n0(Sh2& s);
inline u16 x00n(Sh2& s);
inline u16 xnnn(Sh2& s);
inline u16 x0nn(Sh2& s);
//@}

/// \name SH2 instructions
//@{
inline void delaySlot(Sh2& s, u32 addr);
inline bool isInstructionIllegal(const u16 inst);
inline void badOpcode(Sh2& s);
inline void add(Sh2& s);
inline void addi(Sh2& s);
inline void addc(Sh2& s);
inline void addv(Sh2& s);
inline void and(Sh2& s);
inline void andi(Sh2& s);
inline void andm(Sh2& s);
inline void bf(Sh2& s);
inline void bfs(Sh2& s);
inline void bra(Sh2& s);
inline void braf(Sh2& s);
inline void bsr(Sh2& s);
inline void bsrf(Sh2& s);
inline void bt(Sh2& s);
inline void bts(Sh2& s);
inline void clrmac(Sh2& s);
inline void clrt(Sh2& s);
inline void cmpeq(Sh2& s);
inline void cmpge(Sh2& s);
inline void cmpgt(Sh2& s);
inline void cmphi(Sh2& s);
inline void cmphs(Sh2& s);
inline void cmppl(Sh2& s);
inline void cmppz(Sh2& s);
inline void cmpstr(Sh2& s);
inline void cmpim(Sh2& s);
inline void div0s(Sh2& s);
inline void div0u(Sh2& s);
inline void div1(Sh2& s);
inline void dmuls(Sh2& s);
inline void dmulu(Sh2& s);
inline void dt(Sh2& s);
inline void extsb(Sh2& s);
inline void extsw(Sh2& s);
inline void extub(Sh2& s);
inline void extuw(Sh2& s);
inline void jmp(Sh2& s);
inline void jsr(Sh2& s);
inline void ldcsr(Sh2& s);
inline void ldcgbr(Sh2& s);
inline void ldcvbr(Sh2& s);
inline void ldcmsr(Sh2& s);
inline void ldcmgbr(Sh2& s);
inline void ldcmvbr(Sh2& s);
inline void ldsmach(Sh2& s);
inline void ldsmacl(Sh2& s);
inline void ldspr(Sh2& s);
inline void ldsmmach(Sh2& s);
inline void ldsmmacl(Sh2& s);
inline void ldsmpr(Sh2& s);
inline void mac(Sh2& s);
inline void macw(Sh2& s);
inline void mov(Sh2& s);
inline void movbs(Sh2& s);
inline void movws(Sh2& s);
inline void movls(Sh2& s);
inline void movbl(Sh2& s);
inline void movwl(Sh2& s);
inline void movll(Sh2& s);
inline void movbm(Sh2& s);
inline void movwm(Sh2& s);
inline void movlm(Sh2& s);
inline void movbp(Sh2& s);
inline void movwp(Sh2& s);
inline void movlp(Sh2& s);
inline void movbs0(Sh2& s);
inline void movws0(Sh2& s);
inline void movls0(Sh2& s);
inline void movbl0(Sh2& s);
inline void movwl0(Sh2& s);
inline void movll0(Sh2& s);
inline void movi(Sh2& s);
inline void movwi(Sh2& s);
inline void movli(Sh2& s);
inline void movblg(Sh2& s);
inline void movwlg(Sh2& s);
inline void movllg(Sh2& s);
inline void movbsg(Sh2& s);
inline void movwsg(Sh2& s);
inline void movlsg(Sh2& s);
inline void movbs4(Sh2& s);
inline void movws4(Sh2& s);
inline void movls4(Sh2& s);
inline void movbl4(Sh2& s);
inline void movwl4(Sh2& s);
inline void movll4(Sh2& s);
inline void mova(Sh2& s);
inline void movt(Sh2& s);
inline void mull(Sh2& s);
inline void muls(Sh2& s);
inline void mulu(Sh2& s);
inline void neg(Sh2& s);
inline void negc(Sh2& s);
inline void nop(Sh2& s);
inline void not(Sh2& s);
inline void or(Sh2& s);
inline void ori(Sh2& s);
inline void orm(Sh2& s);
inline void rotcl(Sh2& s);
inline void rotcr(Sh2& s);
inline void rotl(Sh2& s);
inline void rotr(Sh2& s);
inline void rte(Sh2& s);
inline void rts(Sh2& s);
inline void sett(Sh2& s);
inline void shal(Sh2& s);
inline void shar(Sh2& s);
inline void shll(Sh2& s);
inline void shll2(Sh2& s);
inline void shll8(Sh2& s);
inline void shll16(Sh2& s);
inline void shlr(Sh2& s);
inline void shlr2(Sh2& s);
inline void shlr8(Sh2& s);
inline void shlr16(Sh2& s);
inline void sleep(Sh2& s);
inline void stcsr(Sh2& s);
inline void stcgbr(Sh2& s);
inline void stcvbr(Sh2& s);
inline void stmsr(Sh2& s);
inline void stmgbr(Sh2& s);
inline void stmvbr(Sh2& s);
inline void stsmach(Sh2& s);
inline void stsmacl(Sh2& s);
inline void stspr(Sh2& s);
inline void stsmmach(Sh2& s);
inline void stsmmacl(Sh2& s);
inline void stsmpr(Sh2& s);
inline void sub(Sh2& s);
inline void subc(Sh2& s);
inline void subv(Sh2& s);
inline void swapb(Sh2& s);
inline void swapw(Sh2& s);
inline void tas(Sh2& s);
inline void trapa(Sh2& s);
inline void tst(Sh2& s);
inline void tsti(Sh2& s);
inline void tstm(Sh2& s);
inline void xor(Sh2& s);
inline void xori(Sh2& s);
inline void xorm(Sh2& s);
inline void xtrct(Sh2& s);


//@}

}
}
