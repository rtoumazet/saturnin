//
// sh2_shared.h
// Saturnin
//
// Copyright (c) 2023 Renaud Toumazet
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
/// \file	sh2_shared.h
///
/// \brief	SH2 shared definitions and functions.
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <saturnin/src/emulator_defs.h>

namespace saturnin::sh2 {

// Forward declaration
class Sh2;

constexpr auto instructions_number = u8{142};      ///< Total number of SH2 instructions used.
constexpr auto opcodes_lut_size    = u32{0x10000}; ///< Size of the opcodes lookup table

enum class Sh2Instruction {
    nop,
    add,
    addi,
    addc,
    addv,
    and_op,
    andi,
    andm,
    bf,
    bfs,
    bt,
    bts,
    clrmac,
    clrt,
    sett,
    cmpeq,
    cmpge,
    cmpgt,
    cmphi,
    cmphs,
    cmppl,
    cmppz,
    cmpstr,
    cmpim,
    div0s,
    div0u,
    div1,
    dmuls,
    dmulu,
    dt,
    extsb,
    extsw,
    extub,
    extuw,
    sleep,
    bra,
    braf,
    bsr,
    bsrf,
    jmp,
    jsr,
    ldcsr,
    ldcgbr,
    ldcvbr,
    ldcmsr,
    ldcmgbr,
    ldcmvbr,
    ldsmach,
    ldsmacl,
    ldspr,
    ldsmmach,
    ldsmmacl,
    ldsmpr,
    mac,
    macw,
    mov,
    movbs0,
    movws0,
    movls0,
    movbs,
    movws,
    movls,
    movbl,
    movwl,
    movll,
    movbm,
    movwm,
    movlm,
    movbp,
    movwp,
    movlp,
    movbl0,
    movwl0,
    movll0,
    movi,
    movwi,
    movli,
    movblg,
    movwlg,
    movllg,
    movbsg,
    movwsg,
    movlsg,
    movbs4,
    movws4,
    movls4,
    movbl4,
    movwl4,
    movll4,
    mova,
    movt,
    mull,
    muls,
    mulu,
    neg,
    negc,
    not_op,
    or_op,
    ori,
    orm,
    rotcl,
    rotcr,
    rotl,
    rotr,
    rte,
    rts,
    shal,
    shar,
    shll,
    shll2,
    shll8,
    shll16,
    shlr,
    shlr2,
    shlr8,
    shlr16,
    stcsr,
    stcgbr,
    stcvbr,
    stcmsr,
    stcmgbr,
    stcmvbr,
    stsmach,
    stsmacl,
    stspr,
    stsmmach,
    stsmmacl,
    stsmpr,
    sub,
    subc,
    subv,
    swapb,
    swapw,
    tas,
    trapa,
    tst,
    tsti,
    tstm,
    xor_op,
    xori,
    xorm,
    xtrct
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct	Sh2InstructionDetail
///
/// \brief	Defines the details of a SH2 instruction.
///
/// \author	Runik
/// \date	23/05/2023
////////////////////////////////////////////////////////////////////////////////////////////////////

struct Sh2InstructionDetail {
    Sh2Instruction instruction;              ///< The instruction.
    u16            mask;                     ///< Instruction mask.
    u16            opcode;                   ///< Instruction opcode.
    bool           is_simple;                ///< True if this instruction isn't a jump or doesn't modify system registers.
    bool           illegal_instruction_slot; ///< True if the instruction can't be used in a delay slot.
    bool           is_subroutine_call;       ///< True if the instruction calls a subroutine.
};

static const auto opcodes_table = std::array<Sh2InstructionDetail, instructions_number>{

    {{Sh2Instruction::nop, 0xFFFF, 0x0009, true, false, false},
     {Sh2Instruction::add, 0xF00F, 0x300C, true, false, false},
     {Sh2Instruction::addi, 0xF000, 0x7000, true, false, false},
     {Sh2Instruction::addc, 0xF00F, 0x300E, true, false, false},
     {Sh2Instruction::addv, 0xF00F, 0x300F, true, false, false},
     {Sh2Instruction::and_op, 0xF00F, 0x2009, true, false, false},
     {Sh2Instruction::andi, 0xFF00, 0xC900, true, false, false},
     {Sh2Instruction::andm, 0xFF00, 0xCD00, false, false, false},
     {Sh2Instruction::bf, 0xFF00, 0x8B00, false, true, false},
     {Sh2Instruction::bfs, 0xFF00, 0x8F00, false, true, false},
     {Sh2Instruction::bt, 0xFF00, 0x8900, false, true, false},
     {Sh2Instruction::bts, 0xFF00, 0x8D00, false, true, false},
     {Sh2Instruction::clrmac, 0xFFFF, 0x0028, true, false, false},
     {Sh2Instruction::clrt, 0xFFFF, 0x0008, true, false, false},
     {Sh2Instruction::sett, 0xFFFF, 0x0018, true, false, false},
     {Sh2Instruction::cmpeq, 0xF00F, 0x3000, true, false, false},
     {Sh2Instruction::cmpge, 0xF00F, 0x3003, true, false, false},
     {Sh2Instruction::cmpgt, 0xF00F, 0x3007, true, false, false},
     {Sh2Instruction::cmphi, 0xF00F, 0x3006, true, false, false},
     {Sh2Instruction::cmphs, 0xF00F, 0x3002, true, false, false},
     {Sh2Instruction::cmppl, 0xF0FF, 0x4015, true, false, false},
     {Sh2Instruction::cmppz, 0xF0FF, 0x4011, true, false, false},
     {Sh2Instruction::cmpstr, 0xF00F, 0x200C, true, false, false},
     {Sh2Instruction::cmpim, 0xFF00, 0x8800, true, false, false},
     {Sh2Instruction::div0s, 0xF00F, 0x2007, true, false, false},
     {Sh2Instruction::div0u, 0xFFFF, 0x0019, true, false, false},
     {Sh2Instruction::div1, 0xF00F, 0x3004, true, false, false},
     {Sh2Instruction::dmuls, 0xF00F, 0x300D, true, false, false},
     {Sh2Instruction::dmulu, 0xF00F, 0x3005, true, false, false},
     {Sh2Instruction::dt, 0xF0FF, 0x4010, true, false, false},
     {Sh2Instruction::extsb, 0xF00F, 0x600E, true, false, false},
     {Sh2Instruction::extsw, 0xF00F, 0x600F, true, false, false},
     {Sh2Instruction::extub, 0xF00F, 0x600C, true, false, false},
     {Sh2Instruction::extuw, 0xF00F, 0x600D, true, false, false},
     {Sh2Instruction::sleep, 0xFFFF, 0x001B, true, false, false},
     {Sh2Instruction::bra, 0xF000, 0xA000, false, true, false},
     {Sh2Instruction::braf, 0xF0FF, 0x0023, false, true, false},
     {Sh2Instruction::bsr, 0xF000, 0xB000, false, true, true},
     {Sh2Instruction::bsrf, 0xF0FF, 0x0003, false, true, true},
     {Sh2Instruction::jmp, 0xF0FF, 0x402B, false, true, false},
     {Sh2Instruction::jsr, 0xF0FF, 0x400B, false, true, true},
     {Sh2Instruction::ldcsr, 0xF0FF, 0x400E, true, false, false},
     {Sh2Instruction::ldcgbr, 0xF0FF, 0x401E, true, false, false},
     {Sh2Instruction::ldcvbr, 0xF0FF, 0x402E, true, false, false},
     {Sh2Instruction::ldcmsr, 0xF0FF, 0x4007, true, false, false},
     {Sh2Instruction::ldcmgbr, 0xF0FF, 0x4017, true, false, false},
     {Sh2Instruction::ldcmvbr, 0xF0FF, 0x4027, true, false, false},
     {Sh2Instruction::ldsmach, 0xF0FF, 0x400A, true, false, false},
     {Sh2Instruction::ldsmacl, 0xF0FF, 0x401A, true, false, false},
     {Sh2Instruction::ldspr, 0xF0FF, 0x402A, true, false, false},
     {Sh2Instruction::ldsmmach, 0xF0FF, 0x4006, true, false, false},
     {Sh2Instruction::ldsmmacl, 0xF0FF, 0x4016, true, false, false},
     {Sh2Instruction::ldsmpr, 0xF0FF, 0x4026, true, false, false},
     {Sh2Instruction::mac, 0xF00F, 0x000F, true, false, false},
     {Sh2Instruction::macw, 0xF00F, 0x400F, true, false, false},
     {Sh2Instruction::mov, 0xF00F, 0x6003, true, false, false},
     {Sh2Instruction::movbs0, 0xF00F, 0x0004, false, false, false},
     {Sh2Instruction::movws0, 0xF00F, 0x0005, false, false, false},
     {Sh2Instruction::movls0, 0xF00F, 0x0006, false, false, false},
     {Sh2Instruction::movbs, 0xF00F, 0x2000, false, false, false},
     {Sh2Instruction::movws, 0xF00F, 0x2001, false, false, false},
     {Sh2Instruction::movls, 0xF00F, 0x2002, false, false, false},
     {Sh2Instruction::movbl, 0xF00F, 0x6000, true, false, false},
     {Sh2Instruction::movwl, 0xF00F, 0x6001, true, false, false},
     {Sh2Instruction::movll, 0xF00F, 0x6002, true, false, false},
     {Sh2Instruction::movbm, 0xF00F, 0x2004, false, false, false},
     {Sh2Instruction::movwm, 0xF00F, 0x2005, false, false, false},
     {Sh2Instruction::movlm, 0xF00F, 0x2006, false, false, false},
     {Sh2Instruction::movbp, 0xF00F, 0x6004, true, false, false},
     {Sh2Instruction::movwp, 0xF00F, 0x6005, true, false, false},
     {Sh2Instruction::movlp, 0xF00F, 0x6006, true, false, false},
     {Sh2Instruction::movbl0, 0xF00F, 0x000C, true, false, false},
     {Sh2Instruction::movwl0, 0xF00F, 0x000D, true, false, false},
     {Sh2Instruction::movll0, 0xF00F, 0x000E, true, false, false},
     {Sh2Instruction::movi, 0xF000, 0xE000, true, false, false},
     {Sh2Instruction::movwi, 0xF000, 0x9000, true, false, false},
     {Sh2Instruction::movli, 0xF000, 0xD000, true, false, false},
     {Sh2Instruction::movblg, 0xFF00, 0xC400, true, false, false},
     {Sh2Instruction::movwlg, 0xFF00, 0xC500, true, false, false},
     {Sh2Instruction::movllg, 0xFF00, 0xC600, true, false, false},
     {Sh2Instruction::movbsg, 0xFF00, 0xC000, false, false, false},
     {Sh2Instruction::movwsg, 0xFF00, 0xC100, false, false, false},
     {Sh2Instruction::movlsg, 0xFF00, 0xC200, false, false, false},
     {Sh2Instruction::movbs4, 0xFF00, 0x8000, false, false, false},
     {Sh2Instruction::movws4, 0xFF00, 0x8100, false, false, false},
     {Sh2Instruction::movls4, 0xF000, 0x1000, false, false, false},
     {Sh2Instruction::movbl4, 0xFF00, 0x8400, true, false, false},
     {Sh2Instruction::movwl4, 0xFF00, 0x8500, true, false, false},
     {Sh2Instruction::movll4, 0xF000, 0x5000, true, false, false},
     {Sh2Instruction::mova, 0xFF00, 0xC700, true, false, false},
     {Sh2Instruction::movt, 0xF0FF, 0x0029, true, false, false},
     {Sh2Instruction::mull, 0xF00F, 0x0007, true, false, false},
     {Sh2Instruction::muls, 0xF00F, 0x200F, true, false, false},
     {Sh2Instruction::mulu, 0xF00F, 0x200E, true, false, false},
     {Sh2Instruction::neg, 0xF00F, 0x600B, true, false, false},
     {Sh2Instruction::negc, 0xF00F, 0x600A, true, false, false},
     {Sh2Instruction::not_op, 0xF00F, 0x6007, true, false, false},
     {Sh2Instruction::or_op, 0xF00F, 0x200B, true, false, false},
     {Sh2Instruction::ori, 0xFF00, 0xCB00, true, false, false},
     {Sh2Instruction::orm, 0xFF00, 0xCF00, false, false, false},
     {Sh2Instruction::rotcl, 0xF0FF, 0x4024, true, false, false},
     {Sh2Instruction::rotcr, 0xF0FF, 0x4025, true, false, false},
     {Sh2Instruction::rotl, 0xF0FF, 0x4004, true, false, false},
     {Sh2Instruction::rotr, 0xF0FF, 0x4005, true, false, false},
     {Sh2Instruction::rte, 0xFFFF, 0x002B, false, true, false},
     {Sh2Instruction::rts, 0xFFFF, 0x000B, false, true, false},
     {Sh2Instruction::shal, 0xF0FF, 0x4020, true, false, false},
     {Sh2Instruction::shar, 0xF0FF, 0x4021, true, false, false},
     {Sh2Instruction::shll, 0xF0FF, 0x4000, true, false, false},
     {Sh2Instruction::shll2, 0xF0FF, 0x4008, true, false, false},
     {Sh2Instruction::shll8, 0xF0FF, 0x4018, true, false, false},
     {Sh2Instruction::shll16, 0xF0FF, 0x4028, true, false, false},
     {Sh2Instruction::shlr, 0xF0FF, 0x4001, true, false, false},
     {Sh2Instruction::shlr2, 0xF0FF, 0x4009, true, false, false},
     {Sh2Instruction::shlr8, 0xF0FF, 0x4019, true, false, false},
     {Sh2Instruction::shlr16, 0xF0FF, 0x4029, true, false, false},
     {Sh2Instruction::stcsr, 0xF0FF, 0x0002, true, false, false},
     {Sh2Instruction::stcgbr, 0xF0FF, 0x0012, true, false, false},
     {Sh2Instruction::stcvbr, 0xF0FF, 0x0022, true, false, false},
     {Sh2Instruction::stcmsr, 0xF0FF, 0x4003, false, false, false},
     {Sh2Instruction::stcmgbr, 0xF0FF, 0x4013, false, false, false},
     {Sh2Instruction::stcmvbr, 0xF0FF, 0x4023, false, false, false},
     {Sh2Instruction::stsmach, 0xF0FF, 0x000A, true, false, false},
     {Sh2Instruction::stsmacl, 0xF0FF, 0x001A, true, false, false},
     {Sh2Instruction::stspr, 0xF0FF, 0x002A, true, false, false},
     {Sh2Instruction::stsmmach, 0xF0FF, 0x4002, false, false, false},
     {Sh2Instruction::stsmmacl, 0xF0FF, 0x4012, false, false, false},
     {Sh2Instruction::stsmpr, 0xF0FF, 0x4022, false, false, false},
     {Sh2Instruction::sub, 0xF00F, 0x3008, true, false, false},
     {Sh2Instruction::subc, 0xF00F, 0x300A, true, false, false},
     {Sh2Instruction::subv, 0xF00F, 0x300B, true, false, false},
     {Sh2Instruction::swapb, 0xF00F, 0x6008, true, false, false},
     {Sh2Instruction::swapw, 0xF00F, 0x6009, true, false, false},
     {Sh2Instruction::tas, 0xF0FF, 0x401B, false, false, false},
     {Sh2Instruction::trapa, 0xFF00, 0xC300, true, true, false},
     {Sh2Instruction::tst, 0xF00F, 0x2008, true, false, false},
     {Sh2Instruction::tsti, 0xFF00, 0xC800, true, false, false},
     {Sh2Instruction::tstm, 0xFF00, 0xCC00, true, false, false},
     {Sh2Instruction::xor_op, 0xF00F, 0x200A, true, false, false},
     {Sh2Instruction::xori, 0xFF00, 0xCA00, true, false, false},
     {Sh2Instruction::xorm, 0xFF00, 0xCE00, false, false, false},
     {Sh2Instruction::xtrct, 0xF00F, 0x200D, true, false, false}}};

/// \name Opcode decoding helpers
//@{
inline auto xn00(const u16 inst) -> u8 { return static_cast<u8>((inst & 0x0F00) >> 8); }
inline auto x0n0(const u16 inst) -> u8 { return static_cast<u8>((inst & 0x00F0) >> 4); }
inline auto xnnn(const u16 inst) -> u16 { return (inst & 0xFFF); }
inline auto x0nn(const u16 inst) -> u8 { return static_cast<u8>(inst & 0xFFu); }
inline auto x00n(const u16 inst) -> u8 { return static_cast<u8>(inst & 0xF); }
//@}

} // namespace saturnin::sh2
