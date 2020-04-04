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
#include "sh2_debug.h"

namespace saturnin::sh2 {

// Forward declarations
// class Sh2;

/// \name Instruction split functions.
//@{
inline auto xn000(const u16 inst) -> u8 { return static_cast<u8>((inst & bitmask_F000) >> displacement_12); }
inline auto x0n00(const u16 inst) -> u8 { return static_cast<u8>((inst & bitmask_0F00) >> displacement_8); }
inline auto x00n0(const u16 inst) -> u8 { return static_cast<u8>((inst & bitmask_00F0) >> displacement_4); }
inline auto x0nnn(const u16 inst) -> u16 { return (inst & bitmask_0FFF); }
inline auto x00nn(const u16 inst) -> u8 { return static_cast<u8>(inst & bitmask_00FF); }
inline auto x000n(const u16 inst) -> u8 { return static_cast<u8>(inst & bitmask_000F); }
//@}

/// \name Opcode decoding helpers
//@{
inline auto xn00(Sh2& s) -> u16;
inline auto x0n0(Sh2& s) -> u16;
inline auto x00n(Sh2& s) -> u16;
inline auto xnnn(Sh2& s) -> u16;
inline auto x0nn(Sh2& s) -> u16;
//@}

/// \name SH2 instructions
//@{
inline void delaySlot(Sh2& s, u32 addr);
inline auto isInstructionIllegal(u16 inst) -> bool;
inline void badOpcode(Sh2& s);
inline void add(Sh2& s);
inline void addi(Sh2& s);
inline void addc(Sh2& s);
inline void addv(Sh2& s);
inline void and_op(Sh2& s);
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
inline void not_op(Sh2& s);
inline void or_op(Sh2& s);
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
inline void xor_op(Sh2& s);
inline void xori(Sh2& s);
inline void xorm(Sh2& s);
inline void xtrct(Sh2& s);
//@}

constexpr u8  instructions_number = 142;     ///< Total number of SH2 instructions used.
constexpr u32 opcodes_lut_size    = 0x10000; ///< Size of the opcodes lookup table

using ExecuteType = void (*)(Sh2&);                      ///< Type of execute functions
using DebugType   = auto (*)(u16 opcode) -> std::string; ///< Type of debug functions

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct	Sh2Instruction
///
/// \brief	Defines a SH2 instruction.
///
/// \author	Runik
/// \date	23/09/2019
////////////////////////////////////////////////////////////////////////////////////////////////////
struct Sh2Instruction {
    u16         mask;                     ///< Instruction mask.
    u16         opcode;                   ///< Instruction opcode.
    bool        is_simple;                ///< True if this instruction isn't a jump or doesn't modify system registers.
    bool        illegal_instruction_slot; ///< True if the instruction can't be used in a delay slot
    ExecuteType execute;                  ///< Link to the corresponding function.
    DebugType   debug;                    ///< Link to the debug formatting function.
};

static std::array<ExecuteType, opcodes_lut_size> opcodes_lut; ///< The opcodes LUT, used for instruction fast fetching

static std::array<bool, opcodes_lut_size>
    illegal_instruction_lut; ///< The illegal instruction LUT, used for instruction fast fetching

// clang-format off
static std::array<Sh2Instruction, instructions_number> const opcodes_table
{ {
    { 0xFFFF, 0x0009, true,  false, &nop, &nop_d},
    { 0xF00F, 0x300C, true,  false, &add, &add_d},
    { 0xF000, 0x7000, true,  false, &addi, &addi_d},
    { 0xF00F, 0x300E, true,  false, &addc, &addc_d},
    { 0xF00F, 0x300F, true,  false, &addv, &addv_d},
    { 0xF00F, 0x2009, true,  false, &and_op, &and_op_d},
    { 0xFF00, 0xC900, true,  false, &andi, &andi_d},
    { 0xFF00, 0xCD00, false, false, &andm, &andm_d}, // R0GBR
    { 0xFF00, 0x8B00, false, true, &bf, &bf_d},
    { 0xFF00, 0x8F00, false, true, &bfs, &bfs_d},
    { 0xFF00, 0x8900, false, true, &bt, &bt_d},
    { 0xFF00, 0x8D00, false, true, &bts, &bts_d},
    { 0xFFFF, 0x0028, true,  false, &clrmac, &clrmac_d},
    { 0xFFFF, 0x0008, true,  false, &clrt, &clrt_d},
    { 0xFFFF, 0x0018, true,  false, &sett, &sett_d},
    { 0xF00F, 0x3000, true,  false, &cmpeq, &cmpeq_d},
    { 0xF00F, 0x3003, true,  false, &cmpge, &cmpge_d},
    { 0xF00F, 0x3007, true,  false, &cmpgt, &cmpgt_d},
    { 0xF00F, 0x3006, true,  false, &cmphi, &cmphi_d},
    { 0xF00F, 0x3002, true,  false, &cmphs, &cmphs_d},
    { 0xF0FF, 0x4015, true,  false, &cmppl, &cmppl_d},
    { 0xF0FF, 0x4011, true,  false, &cmppz, &cmppz_d},
    { 0xF00F, 0x200C, true,  false, &cmpstr, &cmpstr_d},
    { 0xFF00, 0x8800, true,  false, &cmpim, &cmpim_d},
    { 0xF00F, 0x2007, true,  false, &div0s, &div0s_d},
    { 0xFFFF, 0x0019, true,  false, &div0u, &div0u_d},
    { 0xF00F, 0x3004, true,  false, &div1, &div1_d},
    { 0xF00F, 0x300D, true,  false, &dmuls, &dmuls_d},
    { 0xF00F, 0x3005, true,  false, &dmulu, &dmulu_d},
    { 0xF0FF, 0x4010, true,  false, &dt, &dt_d},
    { 0xF00F, 0x600E, true,  false, &extsb, &extsb_d},
    { 0xF00F, 0x600F, true,  false, &extsw, &extsw_d},
    { 0xF00F, 0x600C, true,  false, &extub, &extub_d},
    { 0xF00F, 0x600D, true,  false, &extuw, &extuw_d},
    { 0xFFFF, 0x001B, true,  false, &sleep, &sleep_d},
    { 0xF000, 0xA000, false, true, &bra, &bra_d},
    { 0xF0FF, 0x0023, false, true, &braf, &braf_d},
    { 0xF000, 0xB000, false, true, &bsr, &bsr_d},
    { 0xF0FF, 0x0003, false, true, &bsrf, &bsrf_d},
    { 0xF0FF, 0x402B, false, true, &jmp, &jmp_d},
    { 0xF0FF, 0x400B, false, true, &jsr, &jsr_d},
    { 0xF0FF, 0x400E, true,  false, &ldcsr, &ldcsr_d},
    { 0xF0FF, 0x401E, true,  false, &ldcgbr, &ldcgbr_d},
    { 0xF0FF, 0x402E, true,  false, &ldcvbr, &ldcvbr_d},
    { 0xF0FF, 0x4007, true,  false, &ldcmsr, &ldcmsr_d},
    { 0xF0FF, 0x4017, true,  false, &ldcmgbr, &ldcmgbr_d},
    { 0xF0FF, 0x4027, true,  false, &ldcmvbr, &ldcmvbr_d},
    { 0xF0FF, 0x400A, true,  false, &ldsmach, &ldsmach_d},
    { 0xF0FF, 0x401A, true,  false, &ldsmacl, &ldsmacl_d},
    { 0xF0FF, 0x402A, true,  false, &ldspr, &ldspr_d},
    { 0xF0FF, 0x4006, true,  false, &ldsmmach, &ldsmmach_d},
    { 0xF0FF, 0x4016, true,  false, &ldsmmacl, &ldsmmacl_d},
    { 0xF0FF, 0x4026, true,  false, &ldsmpr, &ldsmpr_d},
    { 0xF00F, 0x000F, true,  false, &mac, &mac_d},
    { 0xF00F, 0x400F, true,  false, &macw, &macw_d},
    { 0xF00F, 0x6003, true,  false, &mov, &mov_d},
    { 0xF00F, 0x0004, false, false, &movbs0, &movbs0_d}, // R0R
    { 0xF00F, 0x0005, false, false, &movws0, &movws0_d}, // R0R
    { 0xF00F, 0x0006, false, false, &movls0, &movls0_d}, // R0R
    { 0xF00F, 0x2000, false, false, &movbs, &movbs_d}, // CheckRegValue
    { 0xF00F, 0x2001, false, false, &movws, &movws_d}, // CheckRegValue
    { 0xF00F, 0x2002, false, false, &movls, &movls_d}, // CheckRegValue
    { 0xF00F, 0x6000, true,  false, &movbl, &movbl_d},
    { 0xF00F, 0x6001, true,  false, &movwl, &movwl_d},
    { 0xF00F, 0x6002, true,  false, &movll, &movll_d},
    { 0xF00F, 0x2004, false, false, &movbm, &movbm_d}, // CheckRegValueRM1
    { 0xF00F, 0x2005, false, false, &movwm, &movwm_d}, // CheckRegValueRM2
    { 0xF00F, 0x2006, false, false, &movlm, &movlm_d}, // CheckRegValueRM4
    { 0xF00F, 0x6004, true,  false, &movbp, &movbp_d},
    { 0xF00F, 0x6005, true,  false, &movwp, &movwp_d},
    { 0xF00F, 0x6006, true,  false, &movlp, &movlp_d},
    { 0xF00F, 0x000C, true,  false, &movbl0, &movbl0_d},
    { 0xF00F, 0x000D, true,  false, &movwl0, &movwl0_d},
    { 0xF00F, 0x000E, true,  false, &movll0, &movll0_d},
    { 0xF000, 0xE000, true,  false, &movi, &movi_d},
    { 0xF000, 0x9000, true,  false, &movwi, &movwi_d},
    { 0xF000, 0xD000, true,  false, &movli, &movli_d},
    { 0xFF00, 0xC400, true,  false, &movblg, &movblg_d},
    { 0xFF00, 0xC500, true,  false, &movwlg, &movwlg_d},
    { 0xFF00, 0xC600, true,  false, &movllg, &movllg_d},
    { 0xFF00, 0xC000, false, false, &movbsg, &movbsg_d}, // DispGBR
    { 0xFF00, 0xC100, false, false, &movwsg, &movwsg_d}, // DispGBR
    { 0xFF00, 0xC200, false, false, &movlsg, &movlsg_d}, // DispGBR
    { 0xFF00, 0x8000, false, false, &movbs4, &movbs4_d}, // DispR
    { 0xFF00, 0x8100, false, false, &movws4, &movws4_d}, // DispR
    { 0xF000, 0x1000, false, false, &movls4, &movls4_d}, // DispR
    { 0xFF00, 0x8400, true,  false, &movbl4, &movbl4_d},
    { 0xFF00, 0x8500, true,  false, &movwl4, &movwl4_d},
    { 0xF000, 0x5000, true,  false, &movll4, &movll4_d},
    { 0xFF00, 0xC700, true,  false, &mova, &mova_d},
    { 0xF0FF, 0x0029, true,  false, &movt, &movt_d},
    { 0xF00F, 0x0007, true,  false, &mull, &mull_d},
    { 0xF00F, 0x200F, true,  false, &muls, &muls_d},
    { 0xF00F, 0x200E, true,  false, &mulu, &mulu_d},
    { 0xF00F, 0x600B, true,  false, &neg, &neg_d},
    { 0xF00F, 0x600A, true,  false, &negc, &negc_d},
    { 0xF00F, 0x6007, true,  false, &not_op, &not_op_d},
    { 0xF00F, 0x200B, true,  false, &or_op, &or_op_d},
    { 0xFF00, 0xCB00, true,  false, &ori, &ori_d},
    { 0xFF00, 0xCF00, false, false, &orm, &orm_d}, // R0GBR
    { 0xF0FF, 0x4024, true,  false, &rotcl, &rotcl_d},
    { 0xF0FF, 0x4025, true,  false, &rotcr, &rotcr_d},
    { 0xF0FF, 0x4004, true,  false, &rotl, &rotl_d},
    { 0xF0FF, 0x4005, true,  false, &rotr, &rotr_d},
    { 0xFFFF, 0x002B, false, true, &rte, &rte_d},
    { 0xFFFF, 0x000B, false, true, &rts, &rts_d},
    { 0xF0FF, 0x4020, true,  false, &shal, &shal_d},
    { 0xF0FF, 0x4021, true,  false, &shar, &shar_d},
    { 0xF0FF, 0x4000, true,  false, &shll, &shll_d},
    { 0xF0FF, 0x4008, true,  false, &shll2, &shll2_d},
    { 0xF0FF, 0x4018, true,  false, &shll8, &shll8_d},
    { 0xF0FF, 0x4028, true,  false, &shll16, &shll16_d},
    { 0xF0FF, 0x4001, true,  false, &shlr, &shlr_d},
    { 0xF0FF, 0x4009, true,  false, &shlr2, &shlr2_d},
    { 0xF0FF, 0x4019, true,  false, &shlr8, &shlr8_d},
    { 0xF0FF, 0x4029, true,  false, &shlr16, &shlr16_d},
    { 0xF0FF, 0x0002, true,  false, &stcsr, &stcsr_d},
    { 0xF0FF, 0x0012, true,  false, &stcgbr, &stcgbr_d},
    { 0xF0FF, 0x0022, true,  false, &stcvbr, &stcvbr_d},
    { 0xF0FF, 0x4003, false, false, &stcmsr, &stcmsr_d}, // CheckRegValueRM4
    { 0xF0FF, 0x4013, false, false, &stcmgbr, &stcmgbr_d}, // CheckRegValueRM4
    { 0xF0FF, 0x4023, false, false, &stcmvbr, &stcmvbr_d}, // CheckRegValueRM4
    { 0xF0FF, 0x000A, true,  false, &stsmach, &stsmach_d},
    { 0xF0FF, 0x001A, true,  false, &stsmacl, &stsmacl_d},
    { 0xF0FF, 0x002A, true,  false, &stspr, &stspr_d},
    { 0xF0FF, 0x4002, false, false, &stsmmach, &stsmmach_d}, // CheckRegValueRM4
    { 0xF0FF, 0x4012, false, false, &stsmmacl, &stsmmacl_d}, // CheckRegValueRM4
    { 0xF0FF, 0x4022, false, false, &stsmpr, &stsmpr_d}, // CheckRegValueRM4
    { 0xF00F, 0x3008, true,  false, &sub, &sub_d},
    { 0xF00F, 0x300A, true,  false, &subc, &subc_d},
    { 0xF00F, 0x300B, true,  false, &subv, &subv_d},
    { 0xF00F, 0x6008, true,  false, &swapb, &swapb_d},
    { 0xF00F, 0x6009, true,  false, &swapw, &swapw_d},
    { 0xF0FF, 0x401B, false, false, &tas, &tas_d}, // CheckRegValue
    { 0xFF00, 0xC300, true,  true, &trapa, &trapa_d},
    { 0xF00F, 0x2008, true,  false, &tst, &tst_d},
    { 0xFF00, 0xC800, true,  false, &tsti, &tsti_d},
    { 0xFF00, 0xCC00, true,  false, &tstm, &tstm_d},
    { 0xF00F, 0x200A, true,  false, &xor_op, &xor_op_d},
    { 0xFF00, 0xCA00, true,  false, &xori, &xori_d},
    { 0xFF00, 0xCE00, false, false, &xorm, &xorm_d}, // R0GBR
    { 0xF00F, 0x200D, true,  false, &xtrct, &xtrct_d}
} };
// clang-format on

void initializeOpcodesLut();

void execute(Sh2& s);

} // namespace saturnin::sh2
