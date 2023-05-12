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
#include <saturnin/src/sh2/basic_interpreter/sh2_disasm.h>

namespace saturnin::sh2 {
// Forward declarations
class Sh2;
} // namespace saturnin::sh2

namespace saturnin::sh2::basic_interpreter {

extern void badOpcode(Sh2& s);

/// \name Opcode decoding helpers
//@{
// inline auto xn00(const u16 inst) -> u8;
// inline auto x0n0(const u16 inst) -> u8;
// inline auto xnnn(const u16 inst) -> u16;
// inline auto x0nn(const u16 inst) -> u8;
// inline auto x00n(const u16 inst) -> u8;
//@}

struct BasicInterpreter {
    /// \name SH2 instructions
    //@{
    // inline void delaySlot(Sh2& s, u32 addr);
    // inline void badOpcode(Sh2& s);
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
    static void stmsr(Sh2& s);
    static void stmgbr(Sh2& s);
    static void stmvbr(Sh2& s);
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
};

constexpr auto instructions_number = u8{142};                    ///< Total number of SH2 instructions used.
constexpr auto opcodes_lut_size    = u32{0x10000};               ///< Size of the opcodes lookup table

using ExecuteType = void (*)(Sh2&);                              ///< Type of execute functions
using DisasmType  = auto (*)(u32 pc, u16 opcode) -> std::string; ///< Type of disassembly functions

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
    bool        illegal_instruction_slot; ///< True if the instruction can't be used in a delay slot.
    bool        is_subroutine_call;       ///< True if the instruction calls a subroutine.
    ExecuteType execute;                  ///< Link to the corresponding function.
    DisasmType  disasm;                   ///< Link to the disassembly formatting function.
};

static std::array<ExecuteType, opcodes_lut_size> opcodes_lut; ///< The opcodes LUT, used for instruction fast fetching
static std::array<DisasmType, opcodes_lut_size>
    opcodes_disasm_lut;                                       ///< The opcodes disasm LUT, used for instruction fast fetching

static std::array<bool, opcodes_lut_size>
    illegal_instruction_lut; ///< The illegal instruction LUT, used for instruction fast fetching

static std::array<bool, opcodes_lut_size> calls_subroutine_lut;

// clang-format off
static const auto opcodes_table=std::array<Sh2Instruction, instructions_number>
{ {
    { 0xFFFF, 0x0009, true,  false, false, &BasicInterpreter::nop, &nop_d},
    { 0xF00F, 0x300C, true,  false, false, &BasicInterpreter::add, &add_d},
    { 0xF000, 0x7000, true,  false, false, &BasicInterpreter::addi, &addi_d},
    { 0xF00F, 0x300E, true,  false, false, &BasicInterpreter::addc, &addc_d},
    { 0xF00F, 0x300F, true,  false, false, &BasicInterpreter::addv, &addv_d},
    { 0xF00F, 0x2009, true,  false, false, &BasicInterpreter::and_op, &and_op_d},
    { 0xFF00, 0xC900, true,  false, false, &BasicInterpreter::andi, &andi_d},
    { 0xFF00, 0xCD00, false, false, false, &BasicInterpreter::andm, &andm_d}, // R0GBR
    { 0xFF00, 0x8B00, false, true,  false, &BasicInterpreter::bf, &bf_d},
    { 0xFF00, 0x8F00, false, true,  false, &BasicInterpreter::bfs, &bfs_d},
    { 0xFF00, 0x8900, false, true,  false, &BasicInterpreter::bt, &bt_d},
    { 0xFF00, 0x8D00, false, true,  false, &BasicInterpreter::bts, &bts_d},
    { 0xFFFF, 0x0028, true,  false, false, &BasicInterpreter::clrmac, &clrmac_d},
    { 0xFFFF, 0x0008, true,  false, false, &BasicInterpreter::clrt, &clrt_d},
    { 0xFFFF, 0x0018, true,  false, false, &BasicInterpreter::sett, &sett_d},
    { 0xF00F, 0x3000, true,  false, false, &BasicInterpreter::cmpeq, &cmpeq_d},
    { 0xF00F, 0x3003, true,  false, false, &BasicInterpreter::cmpge, &cmpge_d},
    { 0xF00F, 0x3007, true,  false, false, &BasicInterpreter::cmpgt, &cmpgt_d},
    { 0xF00F, 0x3006, true,  false, false, &BasicInterpreter::cmphi, &cmphi_d},
    { 0xF00F, 0x3002, true,  false, false, &BasicInterpreter::cmphs, &cmphs_d},
    { 0xF0FF, 0x4015, true,  false, false, &BasicInterpreter::cmppl, &cmppl_d},
    { 0xF0FF, 0x4011, true,  false, false, &BasicInterpreter::cmppz, &cmppz_d},
    { 0xF00F, 0x200C, true,  false, false, &BasicInterpreter::cmpstr, &cmpstr_d},
    { 0xFF00, 0x8800, true,  false, false, &BasicInterpreter::cmpim, &cmpim_d},
    { 0xF00F, 0x2007, true,  false, false, &BasicInterpreter::div0s, &div0s_d},
    { 0xFFFF, 0x0019, true,  false, false, &BasicInterpreter::div0u, &div0u_d},
    { 0xF00F, 0x3004, true,  false, false, &BasicInterpreter::div1, &div1_d},
    { 0xF00F, 0x300D, true,  false, false, &BasicInterpreter::dmuls, &dmuls_d},
    { 0xF00F, 0x3005, true,  false, false, &BasicInterpreter::dmulu, &dmulu_d},
    { 0xF0FF, 0x4010, true,  false, false, &BasicInterpreter::dt, &dt_d},
    { 0xF00F, 0x600E, true,  false, false, &BasicInterpreter::extsb, &extsb_d},
    { 0xF00F, 0x600F, true,  false, false, &BasicInterpreter::extsw, &extsw_d},
    { 0xF00F, 0x600C, true,  false, false, &BasicInterpreter::extub, &extub_d},
    { 0xF00F, 0x600D, true,  false, false, &BasicInterpreter::extuw, &extuw_d},
    { 0xFFFF, 0x001B, true,  false, false, &BasicInterpreter::sleep, &sleep_d},
    { 0xF000, 0xA000, false, true,  false, &BasicInterpreter::bra, &bra_d},
    { 0xF0FF, 0x0023, false, true,  false, &BasicInterpreter::braf, &braf_d},
    { 0xF000, 0xB000, false, true,  true,  &BasicInterpreter::bsr, &bsr_d},
    { 0xF0FF, 0x0003, false, true,  true,  &BasicInterpreter::bsrf, &bsrf_d},
    { 0xF0FF, 0x402B, false, true,  false, &BasicInterpreter::jmp, &jmp_d},
    { 0xF0FF, 0x400B, false, true,  true,  &BasicInterpreter::jsr, &jsr_d},
    { 0xF0FF, 0x400E, true,  false, false, &BasicInterpreter::ldcsr, &ldcsr_d},
    { 0xF0FF, 0x401E, true,  false, false, &BasicInterpreter::ldcgbr, &ldcgbr_d},
    { 0xF0FF, 0x402E, true,  false, false, &BasicInterpreter::ldcvbr, &ldcvbr_d},
    { 0xF0FF, 0x4007, true,  false, false, &BasicInterpreter::ldcmsr, &ldcmsr_d},
    { 0xF0FF, 0x4017, true,  false, false, &BasicInterpreter::ldcmgbr, &ldcmgbr_d},
    { 0xF0FF, 0x4027, true,  false, false, &BasicInterpreter::ldcmvbr, &ldcmvbr_d},
    { 0xF0FF, 0x400A, true,  false, false, &BasicInterpreter::ldsmach, &ldsmach_d},
    { 0xF0FF, 0x401A, true,  false, false, &BasicInterpreter::ldsmacl, &ldsmacl_d},
    { 0xF0FF, 0x402A, true,  false, false, &BasicInterpreter::ldspr, &ldspr_d},
    { 0xF0FF, 0x4006, true,  false, false, &BasicInterpreter::ldsmmach, &ldsmmach_d},
    { 0xF0FF, 0x4016, true,  false, false, &BasicInterpreter::ldsmmacl, &ldsmmacl_d},
    { 0xF0FF, 0x4026, true,  false, false, &BasicInterpreter::ldsmpr, &ldsmpr_d},
    { 0xF00F, 0x000F, true,  false, false, &BasicInterpreter::mac, &mac_d},
    { 0xF00F, 0x400F, true,  false, false, &BasicInterpreter::macw, &macw_d},
    { 0xF00F, 0x6003, true,  false, false, &BasicInterpreter::mov, &mov_d},
    { 0xF00F, 0x0004, false, false, false, &BasicInterpreter::movbs0, &movbs0_d}, // R0R
    { 0xF00F, 0x0005, false, false, false, &BasicInterpreter::movws0, &movws0_d}, // R0R
    { 0xF00F, 0x0006, false, false, false, &BasicInterpreter::movls0, &movls0_d}, // R0R
    { 0xF00F, 0x2000, false, false, false, &BasicInterpreter::movbs, &movbs_d}, // CheckRegValue
    { 0xF00F, 0x2001, false, false, false, &BasicInterpreter::movws, &movws_d}, // CheckRegValue
    { 0xF00F, 0x2002, false, false, false, &BasicInterpreter::movls, &movls_d}, // CheckRegValue
    { 0xF00F, 0x6000, true,  false, false, &BasicInterpreter::movbl, &movbl_d},
    { 0xF00F, 0x6001, true,  false, false, &BasicInterpreter::movwl, &movwl_d},
    { 0xF00F, 0x6002, true,  false, false, &BasicInterpreter::movll, &movll_d},
    { 0xF00F, 0x2004, false, false, false, &BasicInterpreter::movbm, &movbm_d}, // CheckRegValueRM1
    { 0xF00F, 0x2005, false, false, false, &BasicInterpreter::movwm, &movwm_d}, // CheckRegValueRM2
    { 0xF00F, 0x2006, false, false, false, &BasicInterpreter::movlm, &movlm_d}, // CheckRegValueRM4
    { 0xF00F, 0x6004, true,  false, false, &BasicInterpreter::movbp, &movbp_d},
    { 0xF00F, 0x6005, true,  false, false, &BasicInterpreter::movwp, &movwp_d},
    { 0xF00F, 0x6006, true,  false, false, &BasicInterpreter::movlp, &movlp_d},
    { 0xF00F, 0x000C, true,  false, false, &BasicInterpreter::movbl0, &movbl0_d},
    { 0xF00F, 0x000D, true,  false, false, &BasicInterpreter::movwl0, &movwl0_d},
    { 0xF00F, 0x000E, true,  false, false, &BasicInterpreter::movll0, &movll0_d},
    { 0xF000, 0xE000, true,  false, false, &BasicInterpreter::movi, &movi_d},
    { 0xF000, 0x9000, true,  false, false, &BasicInterpreter::movwi, &movwi_d},
    { 0xF000, 0xD000, true,  false, false, &BasicInterpreter::movli, &movli_d},
    { 0xFF00, 0xC400, true,  false, false, &BasicInterpreter::movblg, &movblg_d},
    { 0xFF00, 0xC500, true,  false, false, &BasicInterpreter::movwlg, &movwlg_d},
    { 0xFF00, 0xC600, true,  false, false, &BasicInterpreter::movllg, &movllg_d},
    { 0xFF00, 0xC000, false, false, false, &BasicInterpreter::movbsg, &movbsg_d}, // DispGBR
    { 0xFF00, 0xC100, false, false, false, &BasicInterpreter::movwsg, &movwsg_d}, // DispGBR
    { 0xFF00, 0xC200, false, false, false, &BasicInterpreter::movlsg, &movlsg_d}, // DispGBR
    { 0xFF00, 0x8000, false, false, false, &BasicInterpreter::movbs4, &movbs4_d}, // DispR
    { 0xFF00, 0x8100, false, false, false, &BasicInterpreter::movws4, &movws4_d}, // DispR
    { 0xF000, 0x1000, false, false, false, &BasicInterpreter::movls4, &movls4_d}, // DispR
    { 0xFF00, 0x8400, true,  false, false, &BasicInterpreter::movbl4, &movbl4_d},
    { 0xFF00, 0x8500, true,  false, false, &BasicInterpreter::movwl4, &movwl4_d},
    { 0xF000, 0x5000, true,  false, false, &BasicInterpreter::movll4, &movll4_d},
    { 0xFF00, 0xC700, true,  false, false, &BasicInterpreter::mova, &mova_d},
    { 0xF0FF, 0x0029, true,  false, false, &BasicInterpreter::movt, &movt_d},
    { 0xF00F, 0x0007, true,  false, false, &BasicInterpreter::mull, &mull_d},
    { 0xF00F, 0x200F, true,  false, false, &BasicInterpreter::muls, &muls_d},
    { 0xF00F, 0x200E, true,  false, false, &BasicInterpreter::mulu, &mulu_d},
    { 0xF00F, 0x600B, true,  false, false, &BasicInterpreter::neg, &neg_d},
    { 0xF00F, 0x600A, true,  false, false, &BasicInterpreter::negc, &negc_d},
    { 0xF00F, 0x6007, true,  false, false, &BasicInterpreter::not_op, &not_op_d},
    { 0xF00F, 0x200B, true,  false, false, &BasicInterpreter::or_op, &or_op_d},
    { 0xFF00, 0xCB00, true,  false, false, &BasicInterpreter::ori, &ori_d},
    { 0xFF00, 0xCF00, false, false, false, &BasicInterpreter::orm, &orm_d}, // R0GBR
    { 0xF0FF, 0x4024, true,  false, false, &BasicInterpreter::rotcl, &rotcl_d},
    { 0xF0FF, 0x4025, true,  false, false, &BasicInterpreter::rotcr, &rotcr_d},
    { 0xF0FF, 0x4004, true,  false, false, &BasicInterpreter::rotl, &rotl_d},
    { 0xF0FF, 0x4005, true,  false, false, &BasicInterpreter::rotr, &rotr_d},
    { 0xFFFF, 0x002B, false, true,  false, &BasicInterpreter::rte, &rte_d},
    { 0xFFFF, 0x000B, false, true,  false, &BasicInterpreter::rts, &rts_d},
    { 0xF0FF, 0x4020, true,  false, false, &BasicInterpreter::shal, &shal_d},
    { 0xF0FF, 0x4021, true,  false, false, &BasicInterpreter::shar, &shar_d},
    { 0xF0FF, 0x4000, true,  false, false, &BasicInterpreter::shll, &shll_d},
    { 0xF0FF, 0x4008, true,  false, false, &BasicInterpreter::shll2, &shll2_d},
    { 0xF0FF, 0x4018, true,  false, false, &BasicInterpreter::shll8, &shll8_d},
    { 0xF0FF, 0x4028, true,  false, false, &BasicInterpreter::shll16, &shll16_d},
    { 0xF0FF, 0x4001, true,  false, false, &BasicInterpreter::shlr, &shlr_d},
    { 0xF0FF, 0x4009, true,  false, false, &BasicInterpreter::shlr2, &shlr2_d},
    { 0xF0FF, 0x4019, true,  false, false, &BasicInterpreter::shlr8, &shlr8_d},
    { 0xF0FF, 0x4029, true,  false, false, &BasicInterpreter::shlr16, &shlr16_d},
    { 0xF0FF, 0x0002, true,  false, false, &BasicInterpreter::stcsr, &stcsr_d},
    { 0xF0FF, 0x0012, true,  false, false, &BasicInterpreter::stcgbr, &stcgbr_d},
    { 0xF0FF, 0x0022, true,  false, false, &BasicInterpreter::stcvbr, &stcvbr_d},
    { 0xF0FF, 0x4003, false, false, false, &BasicInterpreter::stcmsr, &stcmsr_d}, // CheckRegValueRM4
    { 0xF0FF, 0x4013, false, false, false, &BasicInterpreter::stcmgbr, &stcmgbr_d}, // CheckRegValueRM4
    { 0xF0FF, 0x4023, false, false, false, &BasicInterpreter::stcmvbr, &stcmvbr_d}, // CheckRegValueRM4
    { 0xF0FF, 0x000A, true,  false, false, &BasicInterpreter::stsmach, &stsmach_d},
    { 0xF0FF, 0x001A, true,  false, false, &BasicInterpreter::stsmacl, &stsmacl_d},
    { 0xF0FF, 0x002A, true,  false, false, &BasicInterpreter::stspr, &stspr_d},
    { 0xF0FF, 0x4002, false, false, false, &BasicInterpreter::stsmmach, &stsmmach_d}, // CheckRegValueRM4
    { 0xF0FF, 0x4012, false, false, false, &BasicInterpreter::stsmmacl, &stsmmacl_d}, // CheckRegValueRM4
    { 0xF0FF, 0x4022, false, false, false, &BasicInterpreter::stsmpr, &stsmpr_d}, // CheckRegValueRM4
    { 0xF00F, 0x3008, true,  false, false, &BasicInterpreter::sub, &sub_d},
    { 0xF00F, 0x300A, true,  false, false, &BasicInterpreter::subc, &subc_d},
    { 0xF00F, 0x300B, true,  false, false, &BasicInterpreter::subv, &subv_d},
    { 0xF00F, 0x6008, true,  false, false, &BasicInterpreter::swapb, &swapb_d},
    { 0xF00F, 0x6009, true,  false, false, &BasicInterpreter::swapw, &swapw_d},
    { 0xF0FF, 0x401B, false, false, false, &BasicInterpreter::tas, &tas_d}, // CheckRegValue
    { 0xFF00, 0xC300, true,  true,  false, &BasicInterpreter::trapa, &trapa_d},
    { 0xF00F, 0x2008, true,  false, false, &BasicInterpreter::tst, &tst_d},
    { 0xFF00, 0xC800, true,  false, false, &BasicInterpreter::tsti, &tsti_d},
    { 0xFF00, 0xCC00, true,  false, false, &BasicInterpreter::tstm, &tstm_d},
    { 0xF00F, 0x200A, true,  false, false, &BasicInterpreter::xor_op, &xor_op_d},
    { 0xFF00, 0xCA00, true,  false, false, &BasicInterpreter::xori, &xori_d},
    { 0xFF00, 0xCE00, false, false, false, &BasicInterpreter::xorm, &xorm_d}, // R0GBR
    { 0xF00F, 0x200D, true,  false, false, &BasicInterpreter::xtrct, &xtrct_d}
} };
// clang-format on

void initializeOpcodesLut();

void execute(Sh2& s);

auto disasm(u32 pc, u16 opcode) -> std::string;

} // namespace saturnin::sh2::basic_interpreter
