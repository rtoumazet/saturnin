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

#include <array> // array
#include "emulator_defs.h"
#include "sh2.h"

namespace saturnin::sh2 {

// Forward declarations
// class Sh2;

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
inline void and (Sh2 & s);
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
inline void stcmsr(Sh2& s);
inline void stcmgbr(Sh2& s);
inline void stcmvbr(Sh2& s);
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

constexpr u8 instructions_number = 142; ///< Total number of SH2 instructions used.

using ExecuteType = void (*)(Sh2&); ///< Type of the execute

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct	Sh2Instruction
///
/// \brief	Defines a SH2 instruction.
///
/// \author	Runik
/// \date	23/09/2019
////////////////////////////////////////////////////////////////////////////////////////////////////
struct Sh2Instruction {
    u16         mask;    ///< Instruction mask.
    u16         opcode;  ///< Instruction opcode.
    bool        go_next; ///< True if this instruction isn't a jump or doesn't modify system registers.
    ExecuteType execute; ///< Link to the corresponding function.
};

static std::array<ExecuteType, 0x10000> opcodes_lut; ///< The opcodes LUT, used for instruction fast fetching

// clang-format off
static std::array<Sh2Instruction, instructions_number> const opcodes_table
{ {
    { 0xFFFF, 0x0009, true,  &nop},
    { 0xF00F, 0x300C, true,  &add},
    { 0xF000, 0x7000, true,  &addi},
    { 0xF00F, 0x300E, true,  &addc},
    { 0xF00F, 0x300F, true,  &addv},
    { 0xF00F, 0x2009, true,  &and},
    { 0xFF00, 0xC900, true,  &andi},
    { 0xFF00, 0xCD00, false, &andm}, // R0GBR
    { 0xFF00, 0x8B00, false, &bf},
    { 0xFF00, 0x8F00, false, &bfs},
    { 0xFF00, 0x8900, false, &bt},
    { 0xFF00, 0x8D00, false, &bts},
    { 0xFFFF, 0x0028, true,  &clrmac},
    { 0xFFFF, 0x0008, true,  &clrt},
    { 0xFFFF, 0x0018, true,  &sett},
    { 0xF00F, 0x3000, true,  &cmpeq},
    { 0xF00F, 0x3003, true,  &cmpge},
    { 0xF00F, 0x3007, true,  &cmpgt},
    { 0xF00F, 0x3006, true,  &cmphi},
    { 0xF00F, 0x3002, true,  &cmphs},
    { 0xF0FF, 0x4015, true,  &cmppl},
    { 0xF0FF, 0x4011, true,  &cmppz},
    { 0xF00F, 0x200C, true,  &cmpstr},
    { 0xFF00, 0x8800, true,  &cmpim},
    { 0xF00F, 0x2007, true,  &div0s},
    { 0xFFFF, 0x0019, true,  &div0u},
    { 0xF00F, 0x3004, true,  &div1},
    { 0xF00F, 0x300D, true,  &dmuls},
    { 0xF00F, 0x3005, true,  &dmulu},
    { 0xF0FF, 0x4010, true,  &dt},
    { 0xF00F, 0x600E, true,  &extsb},
    { 0xF00F, 0x600F, true,  &extsw},
    { 0xF00F, 0x600C, true,  &extub},
    { 0xF00F, 0x600D, true,  &extuw},
    { 0xFFFF, 0x001B, true,  &sleep},
    { 0xF000, 0xA000, false, &bra},
    { 0xF0FF, 0x0023, false, &braf},
    { 0xF000, 0xB000, false, &bsr},
    { 0xF0FF, 0x0003, false, &bsrf},
    { 0xF0FF, 0x402B, false, &jmp},
    { 0xF0FF, 0x400B, false, &jsr},
    { 0xF0FF, 0x400E, true,  &ldcsr},
    { 0xF0FF, 0x401E, true,  &ldcgbr},
    { 0xF0FF, 0x402E, true,  &ldcvbr},
    { 0xF0FF, 0x4007, true,  &ldcmsr},
    { 0xF0FF, 0x4017, true,  &ldcmgbr},
    { 0xF0FF, 0x4027, true,  &ldcmvbr},
    { 0xF0FF, 0x400A, true,  &ldsmach},
    { 0xF0FF, 0x401A, true,  &ldsmacl},
    { 0xF0FF, 0x402A, true,  &ldspr},
    { 0xF0FF, 0x4006, true,  &ldsmmach},
    { 0xF0FF, 0x4016, true,  &ldsmmacl},
    { 0xF0FF, 0x4026, true,  &ldsmpr},
    { 0xF00F, 0x000F, true,  &mac},
    { 0xF00F, 0x400F, true,  &macw},
    { 0xF00F, 0x6003, true,  &mov},
    { 0xF00F, 0x0004, false, &movbs0}, // R0R
    { 0xF00F, 0x0005, false, &movws0}, // R0R
    { 0xF00F, 0x0006, false, &movls0}, // R0R
    { 0xF00F, 0x2000, false, &movbs}, // CheckRegValue
    { 0xF00F, 0x2001, false, &movws}, // CheckRegValue
    { 0xF00F, 0x2002, false, &movls}, // CheckRegValue
    { 0xF00F, 0x6000, true,  &movbl},
    { 0xF00F, 0x6001, true,  &movwl},
    { 0xF00F, 0x6002, true,  &movll},
    { 0xF00F, 0x2004, false, &movbm}, // CheckRegValueRM1
    { 0xF00F, 0x2005, false, &movwm}, // CheckRegValueRM2
    { 0xF00F, 0x2006, false, &movlm}, // CheckRegValueRM4
    { 0xF00F, 0x6004, true,  &movbp},
    { 0xF00F, 0x6005, true,  &movwp},
    { 0xF00F, 0x6006, true,  &movlp},
    { 0xF00F, 0x000C, true,  &movbl0},
    { 0xF00F, 0x000D, true,  &movwl0},
    { 0xF00F, 0x000E, true,  &movll0},
    { 0xF000, 0xE000, true,  &movi},
    { 0xF000, 0x9000, true,  &movwi},
    { 0xF000, 0xD000, true,  &movli},
    { 0xFF00, 0xC400, true,  &movblg},
    { 0xFF00, 0xC500, true,  &movwlg},
    { 0xFF00, 0xC600, true,  &movllg},
    { 0xFF00, 0xC000, false, &movbsg}, // DispGBR
    { 0xFF00, 0xC100, false, &movwsg}, // DispGBR
    { 0xFF00, 0xC200, false, &movlsg}, // DispGBR
    { 0xFF00, 0x8000, false, &movbs4}, // DispR
    { 0xFF00, 0x8100, false, &movws4}, // DispR
    { 0xF000, 0x1000, false, &movls4}, // DispR
    { 0xFF00, 0x8400, true,  &movbl4},
    { 0xFF00, 0x8500, true,  &movwl4},
    { 0xF000, 0x5000, true,  &movll4},
    { 0xFF00, 0xC700, true,  &mova},
    { 0xF0FF, 0x0029, true,  &movt},
    { 0xF00F, 0x0007, true,  &mull},
    { 0xF00F, 0x200F, true,  &muls},
    { 0xF00F, 0x200E, true,  &mulu},
    { 0xF00F, 0x600B, true,  &neg},
    { 0xF00F, 0x600A, true,  &negc},
    { 0xF00F, 0x6007, true,  &not},
    { 0xF00F, 0x200B, true,  &or},
    { 0xFF00, 0xCB00, true,  &ori},
    { 0xFF00, 0xCF00, false, &orm}, // R0GBR
    { 0xF0FF, 0x4024, true,  &rotcl},
    { 0xF0FF, 0x4025, true,  &rotcr},
    { 0xF0FF, 0x4004, true,  &rotl},
    { 0xF0FF, 0x4005, true,  &rotr},
    { 0xFFFF, 0x002B, false, &rte},
    { 0xFFFF, 0x000B, false, &rts},
    { 0xF0FF, 0x4020, true,  &shal},
    { 0xF0FF, 0x4021, true,  &shar},
    { 0xF0FF, 0x4000, true,  &shll},
    { 0xF0FF, 0x4008, true,  &shll2},
    { 0xF0FF, 0x4018, true,  &shll8},
    { 0xF0FF, 0x4028, true,  &shll16},
    { 0xF0FF, 0x4001, true,  &shlr},
    { 0xF0FF, 0x4009, true,  &shlr2},
    { 0xF0FF, 0x4019, true,  &shlr8},
    { 0xF0FF, 0x4029, true,  &shlr16},
    { 0xF0FF, 0x0002, true,  &stcsr},
    { 0xF0FF, 0x0012, true,  &stcgbr},
    { 0xF0FF, 0x0022, true,  &stcvbr},
    { 0xF0FF, 0x4003, false, &stcmsr}, // CheckRegValueRM4
    { 0xF0FF, 0x4013, false, &stcmgbr}, // CheckRegValueRM4
    { 0xF0FF, 0x4023, false, &stcmvbr}, // CheckRegValueRM4
    { 0xF0FF, 0x000A, true,  &stsmach},
    { 0xF0FF, 0x001A, true,  &stsmacl},
    { 0xF0FF, 0x002A, true,  &stspr},
    { 0xF0FF, 0x4002, false, &stsmmach}, // CheckRegValueRM4
    { 0xF0FF, 0x4012, false, &stsmmacl}, // CheckRegValueRM4
    { 0xF0FF, 0x4022, false, &stsmpr}, // CheckRegValueRM4
    { 0xF00F, 0x3008, true,  &sub},
    { 0xF00F, 0x300A, true,  &subc},
    { 0xF00F, 0x300B, true,  &subv},
    { 0xF00F, 0x6008, true,  &swapb},
    { 0xF00F, 0x6009, true,  &swapw},
    { 0xF0FF, 0x401B, false, &tas}, // CheckRegValue
    { 0xFF00, 0xC300, true,  &trapa},
    { 0xF00F, 0x2008, true,  &tst},
    { 0xFF00, 0xC800, true,  &tsti},
    { 0xFF00, 0xCC00, true,  &tstm},
    { 0xF00F, 0x200A, true,  &xor},
    { 0xFF00, 0xCA00, true,  &xori},
    { 0xFF00, 0xCE00, false, &orm}, // R0GBR
    { 0xF00F, 0x200D, true,  &xtrct}
} };
// clang-format on

void initializeOpcodesLut();

void execute(Sh2& s);

} // namespace saturnin::sh2
