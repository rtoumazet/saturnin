//
// sh2_opcodes.cpp
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

#include <saturnin/src/pch.h>
#include <saturnin/src/sh2/fast_interpreter/sh2_opcodes.h>
#include <saturnin/src/emulator_context.h> // EmulatorContext
#include <saturnin/src/emulator_defs.h>
#include <saturnin/src/emulator_enums.h>   // DebugStatus
#include <saturnin/src/interrupt_sources.h>
#include <saturnin/src/log.h>
#include <saturnin/src/memory.h>
#include <saturnin/src/scu.h>
#include <saturnin/src/sh2/sh2.h>                            // Sh2, Sh2Type
#include <saturnin/src/sh2/fast_interpreter/sh2_opcodes.inc> // generated functions

namespace saturnin::sh2::fast_interpreter {

void FastInterpreter::add(Sh2& s, const u32 n, const u32 m) {
    // Rm + Rn -> Rn
    s.r_[n] += s.r_[m];

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::addi(Sh2& s, const u32 n, const u32 i) {
    // Rn + imm -> Rn
    auto imm = static_cast<s32>(static_cast<s8>(i));
    s.r_[n] += imm;

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::addc(Sh2& s, const u32 n, const u32 m) {
    // Rn + Rm + T -> Rn, carry -> T

    const auto tmp1 = static_cast<s32>(s.r_[n] + s.r_[m]);
    const auto tmp0 = static_cast<s32>(s.r_[n]);
    s.r_[n]         = tmp1 + (s.regs_.sr >> Sh2Regs::StatusRegister::t_shft);

    (tmp0 > tmp1) ? s.regs_.sr.set(Sh2Regs::StatusRegister::t) : s.regs_.sr.clr(Sh2Regs::StatusRegister::t);

    if (tmp1 > static_cast<s32>(s.r_[n])) { s.regs_.sr.set(Sh2Regs::StatusRegister::t); }
    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::addv(Sh2& s, const u32 n, const u32 m) {
    // Rn + Rm -> Rn, overflow -> T

    const auto dest = s32{(static_cast<s32>(s.r_[n]) >= 0) ? 0 : 1};
    auto       src  = s32{(static_cast<s32>(s.r_[m]) >= 0) ? 0 : 1};

    src += dest;
    s.r_[n] += s.r_[m];

    auto ans = s32{(static_cast<s32>(s.r_[n]) >= 0) ? 0 : 1};

    ans += dest;
    if (src == 0 || src == 2) {
        (ans == 1) ? s.regs_.sr.set(Sh2Regs::StatusRegister::t) : s.regs_.sr.clr(Sh2Regs::StatusRegister::t);
    } else {
        s.regs_.sr.clr(Sh2Regs::StatusRegister::t);
    }
    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::and_op(Sh2& s, const u32 n, const u32 m) {
    // Rn & Rm -> Rn
    s.r_[n] &= s.r_[m];
    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::andi(Sh2& s, const u32 i) {
    // R0 & imm -> R0
    s.r_[0] &= (0xFF & i);
    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::andm(Sh2& s, const u32 i) {
    //(R0 + GBR) & imm -> (R0 + GBR)

    auto temp = u32{s.modules_.memory()->read<u8>(s.gbr_ + s.r_[0])};
    temp &= (0xFF & i);
    s.modules_.memory()->write<u8>(s.gbr_ + s.r_[0], static_cast<u8>(temp));
    s.pc_ += 2;
    s.cycles_elapsed_ = 3;
}

void FastInterpreter::bf(Sh2& s, const u32 d) {
    // If T = 0, disp*2 + PC -> PC
    // If T = 1, nop

    if (!s.regs_.sr.any(Sh2Regs::StatusRegister::t)) {
        auto disp = static_cast<s32>(static_cast<s8>(d));

        s.pc_             = s.pc_ + (disp << 1) + 4;
        s.cycles_elapsed_ = 3;
    } else {
        s.pc_ += 2;
        s.cycles_elapsed_ = 1;
    }
}

void FastInterpreter::bfs(Sh2& s, const u32 d) {
    // If T=0, disp*2 + PC -> PC
    // If T=1, nop
    // Modified using SH4 manual

    if (!s.regs_.sr.any(Sh2Regs::StatusRegister::t)) {
        auto disp = static_cast<s32>(static_cast<s8>(d));

        const auto saved_pc = u32{s.pc_};
        delaySlot(s, s.pc_ + 2);
        s.pc_             = saved_pc + (disp << 1) + 4;
        s.cycles_elapsed_ = 2;
    } else {
        s.pc_ += 2;
        s.cycles_elapsed_ = 1;
    }
}

void FastInterpreter::bra(Sh2& s, const u32 d) {
    // disp*2 + PC -> PC
    // Modified using SH4 manual

    auto disp = u32{d};
    if ((d & sign_bit_12_mask) != 0) { disp = 0xFFFFF000 | d; }

    const auto saved_pc = u32{s.pc_};
    delaySlot(s, s.pc_ + 2);
    s.pc_             = saved_pc + (disp << 1) + 4;
    s.cycles_elapsed_ = 2;
}

void FastInterpreter::braf(Sh2& s, const u32 m) {
    // Rm + PC +4 -> PC
    // Modified using SH4 manual + correction
    // Registers save for the delay slot
    const auto old_pc = u32{s.pc_};
    const auto old_r  = u32{s.r_[m]};

    delaySlot(s, s.pc_ + 2);
    s.pc_             = old_pc + old_r + 4;
    s.cycles_elapsed_ = 2;
}

void FastInterpreter::bsr(Sh2& s, const u32 d) {
    // PC -> PR, disp*2 + PC -> PC
    // Modified using SH4 manual + correction

    auto disp = u32{d};
    if ((d & sign_bit_12_mask) != 0) { disp = 0xFFFFF000 | d; }

    s.pr_             = s.pc_ + 4;
    const auto old_pc = u32{s.pc_};
    delaySlot(s, s.pc_ + 2);
    s.pc_             = old_pc + (disp << 1) + 4;
    s.cycles_elapsed_ = 2;

    s.addToCallstack(old_pc, s.pr_);
}

void FastInterpreter::bsrf(Sh2& s, const u32 m) {
    // PC -> PR, Rm + PC -> PC
    // Modified using SH4 manual + correction
    // Registers save for the delay slot

    s.pr_ = s.pc_ + 4;

    const auto old_pc = u32{s.pc_};
    const auto old_r  = u32{s.r_[m]};
    delaySlot(s, s.pc_ + 2);
    s.pc_             = old_pc + 4 + old_r;
    s.cycles_elapsed_ = 2;

    s.addToCallstack(old_pc, s.pr_);
}

void FastInterpreter::bt(Sh2& s, const u32 d) {
    // If T=1, disp*2 + PC -> PC;
    // If T=0=, nop

    if (s.regs_.sr.any(Sh2Regs::StatusRegister::t)) {
        auto disp         = static_cast<s32>(static_cast<s8>(d));
        s.pc_             = s.pc_ + (disp << 1) + 4;
        s.cycles_elapsed_ = 3;
    } else {
        s.pc_ += 2;
        s.cycles_elapsed_ = 1;
    }
}

void FastInterpreter::bts(Sh2& s, const u32 d) {
    // If T=1, disp*2 + PC -> PC
    // If T=0, nop
    // Modified using SH4 manual
    if (s.regs_.sr.any(Sh2Regs::StatusRegister::t)) {
        auto       disp   = static_cast<s32>(static_cast<s8>(d));
        const auto old_pc = u32{s.pc_};
        delaySlot(s, s.pc_ + 2);
        s.pc_             = old_pc + (disp << 1) + 4;
        s.cycles_elapsed_ = 2;
    } else {
        s.pc_ += 2;
        s.cycles_elapsed_ = 1;
    }
}

void FastInterpreter::clrmac(Sh2& s) {
    // 0 -> MACH,MACL
    s.mach_ = 0;
    s.macl_ = 0;

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::clrt(Sh2& s) {
    // 0 -> T
    s.regs_.sr.clr(Sh2Regs::StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::cmpeq(Sh2& s, const u32 n, const u32 m) {
    // If Rn = Rm, T=1
    (s.r_[n] == s.r_[m]) ? s.regs_.sr.set(Sh2Regs::StatusRegister::t) : s.regs_.sr.clr(Sh2Regs::StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::cmpge(Sh2& s, const u32 n, const u32 m) {
    // If Rn >= Rm with sign, T=1
    (static_cast<s32>(s.r_[n]) >= static_cast<s32>(s.r_[m])) ? s.regs_.sr.set(Sh2Regs::StatusRegister::t)
                                                             : s.regs_.sr.clr(Sh2Regs::StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::cmpgt(Sh2& s, const u32 n, const u32 m) {
    // If Rn > Rm with sign, T=1
    (static_cast<s32>(s.r_[n]) > static_cast<s32>(s.r_[m])) ? s.regs_.sr.set(Sh2Regs::StatusRegister::t)
                                                            : s.regs_.sr.clr(Sh2Regs::StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::cmphi(Sh2& s, const u32 n, const u32 m) {
    // If Rn > Rm without sign, T=1
    (s.r_[n] > s.r_[m]) ? s.regs_.sr.set(Sh2Regs::StatusRegister::t) : s.regs_.sr.clr(Sh2Regs::StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::cmphs(Sh2& s, const u32 n, const u32 m) {
    // If Rn > Rm without sign, T=1
    (s.r_[n] >= s.r_[m]) ? s.regs_.sr.set(Sh2Regs::StatusRegister::t) : s.regs_.sr.clr(Sh2Regs::StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::cmppl(Sh2& s, const u32 n) {
    // If Rn > 0, T=1
    (static_cast<s32>(s.r_[n]) > 0) ? s.regs_.sr.set(Sh2Regs::StatusRegister::t) : s.regs_.sr.clr(Sh2Regs::StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::cmppz(Sh2& s, const u32 n) {
    // If Rn >= 0, T=1
    (static_cast<s32>(s.r_[n]) >= 0) ? s.regs_.sr.set(Sh2Regs::StatusRegister::t) : s.regs_.sr.clr(Sh2Regs::StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::cmpstr(Sh2& s, const u32 n, const u32 m) {
    // If one byte of Rn = one byte of Rm then T=1

    auto rm = u32{s.r_[n]};
    auto rn = u32{s.r_[m]};

    ((rm & 0xFF000000) == (rn & 0xFF000000) || (rm & 0x00FF0000) == (rn & 0x00FF0000) || (rm & 0xFF00u) == (rn & 0xFF00u)
     || (rm & 0xFF) == (rn & 0xFF))
        ? s.regs_.sr.set(Sh2Regs::StatusRegister::t)
        : s.regs_.sr.clr(Sh2Regs::StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::cmpim(Sh2& s, const u32 i) {
    // ex: If R0 = imm, T=1

    auto imm = static_cast<s32>(static_cast<s8>(i));
    (s.r_[0] == imm) ? s.regs_.sr.set(Sh2Regs::StatusRegister::t) : s.regs_.sr.clr(Sh2Regs::StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::div0s(Sh2& s, const u32 n, const u32 m) {
    // Rn MSB -> Q, Rm MSB -> M, M^Q -> T
    ((s.r_[n] & sign_bit_32_mask) == 0) ? s.regs_.sr.clr(Sh2Regs::StatusRegister::q) : s.regs_.sr.set(Sh2Regs::StatusRegister::q);
    ((s.r_[m] & sign_bit_32_mask) == 0) ? s.regs_.sr.clr(Sh2Regs::StatusRegister::m) : s.regs_.sr.set(Sh2Regs::StatusRegister::m);
    ((s.regs_.sr >> Sh2Regs::StatusRegister::m_shft) == (s.regs_.sr >> Sh2Regs::StatusRegister::q_shft))
        ? s.regs_.sr.clr(Sh2Regs::StatusRegister::t)
        : s.regs_.sr.set(Sh2Regs::StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::div0u(Sh2& s) {
    // 0 -> M/Q/T
    s.regs_.sr.clr(Sh2Regs::StatusRegister::m);
    s.regs_.sr.clr(Sh2Regs::StatusRegister::q);
    s.regs_.sr.clr(Sh2Regs::StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::div1(Sh2& s, const u32 n, const u32 m) {
    // Division done in one pass (Rn + Rm)
    auto tmp0 = u32{};
    auto tmp1 = bool{};

    const auto old_q = s.regs_.sr.any(Sh2Regs::StatusRegister::q);
    ((sign_bit_32_mask & s.r_[n]) != 0) ? s.regs_.sr.set(Sh2Regs::StatusRegister::q) : s.regs_.sr.clr(Sh2Regs::StatusRegister::q);

    s.r_[n] <<= 1;
    s.r_[n] |= (s.regs_.sr >> Sh2Regs::StatusRegister::t_shft);

    if (old_q) {
        if (s.regs_.sr.any(Sh2Regs::StatusRegister::m)) {
            tmp0 = s.r_[n];
            s.r_[n] -= s.r_[m];
            tmp1 = (s.r_[n] > tmp0);

            if (s.regs_.sr.any(Sh2Regs::StatusRegister::q)) {
                tmp1 ? s.regs_.sr.set(Sh2Regs::StatusRegister::q) : s.regs_.sr.clr(Sh2Regs::StatusRegister::q);
            } else {
                (!tmp1) ? s.regs_.sr.set(Sh2Regs::StatusRegister::q) : s.regs_.sr.clr(Sh2Regs::StatusRegister::q);
            }

        } else {
            tmp0 = s.r_[n];
            s.r_[n] += s.r_[m];
            tmp1 = s.r_[n] < tmp0;

            if (s.regs_.sr.any(Sh2Regs::StatusRegister::q)) {
                (!tmp1) ? s.regs_.sr.set(Sh2Regs::StatusRegister::q) : s.regs_.sr.clr(Sh2Regs::StatusRegister::q);
            } else {
                tmp1 ? s.regs_.sr.set(Sh2Regs::StatusRegister::q) : s.regs_.sr.clr(Sh2Regs::StatusRegister::q);
            }
        }
    } else {
        if (s.regs_.sr.any(Sh2Regs::StatusRegister::m)) {
            tmp0 = s.r_[n];
            s.r_[n] += s.r_[m];
            tmp1 = (s.r_[n]) < tmp0;

            if (s.regs_.sr.any(Sh2Regs::StatusRegister::q)) {
                tmp1 ? s.regs_.sr.set(Sh2Regs::StatusRegister::q) : s.regs_.sr.clr(Sh2Regs::StatusRegister::q);
            } else {
                (!tmp1) ? s.regs_.sr.set(Sh2Regs::StatusRegister::q) : s.regs_.sr.clr(Sh2Regs::StatusRegister::q);
            }

        } else {
            tmp0 = s.r_[n];
            s.r_[n] -= s.r_[m];
            tmp1 = (s.r_[m]) > tmp0;

            if (s.regs_.sr.any(Sh2Regs::StatusRegister::q)) {
                (!tmp1) ? s.regs_.sr.set(Sh2Regs::StatusRegister::q) : s.regs_.sr.clr(Sh2Regs::StatusRegister::q);
            } else {
                tmp1 ? s.regs_.sr.set(Sh2Regs::StatusRegister::q) : s.regs_.sr.clr(Sh2Regs::StatusRegister::q);
            }
        }
    }

    ((s.regs_.sr >> Sh2Regs::StatusRegister::m_shft) == (s.regs_.sr >> Sh2Regs::StatusRegister::q_shft))
        ? s.regs_.sr.set(Sh2Regs::StatusRegister::t)
        : s.regs_.sr.clr(Sh2Regs::StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::dmuls(Sh2& s, const u32 n, const u32 m) {
    // With sign, Rn * Rm -> MACH,MACL

    // Arranged using SH4 manual
    const auto result = static_cast<s64>(static_cast<s32>(s.r_[m])) * static_cast<s32>(s.r_[n]);
    s.mach_           = result >> 32;
    s.macl_           = static_cast<u32>(result);

    s.pc_ += 2;
    s.cycles_elapsed_ = 2;
}

void FastInterpreter::dmulu(Sh2& s, const u32 n, const u32 m) {
    // Without sign, Rm * Rn -> MACH, MACL

    // MIGHT BE WRONG

    // Arranged using SH4 manual
    const auto result = u64{static_cast<u64>(s.r_[m]) * static_cast<u64>(s.r_[n])};
    s.mach_           = static_cast<u32>(result >> 32);
    s.macl_           = static_cast<u32>(result & u32_max);

    s.pc_ += 2;
    s.cycles_elapsed_ = 2;
}

void FastInterpreter::dt(Sh2& s, const u32 n) {
    // Rn - 1 -> Rn;
    // Si R[n] = 0, T=1
    // Sinon T=0
    --s.r_[n];
    (s.r_[n] == 0) ? s.regs_.sr.set(Sh2Regs::StatusRegister::t) : s.regs_.sr.clr(Sh2Regs::StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::extsb(Sh2& s, const u32 n, const u32 m) {
    // Rm sign extension (byte) -> Rn
    s.r_[n] = static_cast<s32>(static_cast<s8>(s.r_[m]));
    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::extsw(Sh2& s, const u32 n, const u32 m) {
    // Rm sign extension (word) -> Rn
    s.r_[n] = static_cast<s32>(static_cast<s16>(s.r_[m]));
    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::extub(Sh2& s, const u32 n, const u32 m) {
    // Rm is 0 extended (byte) -> Rn
    s.r_[n] = static_cast<u32>(static_cast<u8>(s.r_[m]));
    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::extuw(Sh2& s, const u32 n, const u32 m) {
    // Rm is 0 extended (word) -> Rn
    s.r_[n] = static_cast<u32>(static_cast<u16>(s.r_[m]));
    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::jmp(Sh2& s, const u32 m) {
    // Rm -> PC
    // Arranged and fixed using SH4 manual

    const auto old_r = u32{s.r_[m]};
    delaySlot(s, s.pc_ + 2);

    s.pc_             = old_r;
    s.cycles_elapsed_ = 2;
}

void FastInterpreter::jsr(Sh2& s, const u32 m) {
    // PC -> PR, Rm -> PC
    // Arranged and fixed using SH4 manual

    const auto old_r = u32{s.r_[m]};
    s.pr_            = s.pc_ + 4;
    delaySlot(s, s.pc_ + 2);

    s.addToCallstack(s.pc_, s.pr_);

    s.pc_             = old_r;
    s.cycles_elapsed_ = 2;
}

void FastInterpreter::ldcsr(Sh2& s, const u32 m) {
    // Rm -> SR
    s.regs_.sr = (s.r_[m] & 0x000003f3);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::ldcgbr(Sh2& s, const u32 m) {
    // Rm -> GBR
    s.gbr_ = s.r_[m];

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::ldcvbr(Sh2& s, const u32 m) {
    // Rm -> VBR
    s.vbr_ = s.r_[m];

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::ldcmsr(Sh2& s, const u32 m) {
    // (Rm) -> SR, Rm + 4 -> Rm
    s.regs_.sr = static_cast<u16>(s.modules_.memory()->read<u32>(s.r_[m]) & 0x000003f3);
    s.r_[m] += 4;

    s.pc_ += 2;
    s.cycles_elapsed_ = 3;
}

void FastInterpreter::ldcmgbr(Sh2& s, const u32 m) {
    // (Rm) -> GBR, Rm + 4 -> Rm
    s.gbr_ = s.modules_.memory()->read<u32>(s.r_[m]);
    s.r_[m] += 4;

    s.pc_ += 2;
    s.cycles_elapsed_ = 3;
}

void FastInterpreter::ldcmvbr(Sh2& s, const u32 m) {
    // (Rm) -> VBR, Rm + 4 -> Rm
    s.vbr_ = s.modules_.memory()->read<u32>(s.r_[m]);
    s.r_[m] += 4;

    s.pc_ += 2;
    s.cycles_elapsed_ = 3;
}

void FastInterpreter::ldsmach(Sh2& s, const u32 m) {
    // Rm -> MACH
    s.mach_ = s.r_[m];

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::ldsmacl(Sh2& s, const u32 m) {
    // Rm -> MACL
    s.mach_ = s.r_[m];

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::ldspr(Sh2& s, const u32 m) {
    // Rm -> PR
    s.pr_ = s.r_[m];

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::ldsmmach(Sh2& s, const u32 m) {
    //(Rm) -> MACH, Rm + 4 -> Rm
    s.mach_ = s.modules_.memory()->read<u32>(s.r_[m]);
    s.r_[m] += 4;

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::ldsmmacl(Sh2& s, const u32 m) {
    //(Rm) -> MACL, Rm + 4 -> Rm
    s.macl_ = s.modules_.memory()->read<u32>(s.r_[m]);
    s.r_[m] += 4;

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::ldsmpr(Sh2& s, const u32 m) {
    //(Rm) -> PR, Rm + 4 -> Rm
    s.pr_ = s.modules_.memory()->read<u32>(s.r_[m]);
    s.r_[m] += 4;

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::mac(Sh2& s, const u32 n, const u32 m) {
    // Signed operation, (Rn)*(Rm) + MAC -> MAC
    // Arranged using SH4 manual

    const auto src_n = static_cast<s64>(static_cast<s32>(s.modules_.memory()->read<u32>(s.r_[n])));
    s.r_[n] += 4;
    const auto src_m = static_cast<s64>(static_cast<s32>(s.modules_.memory()->read<u32>(s.r_[m])));
    s.r_[m] += 4;

    const auto mul = s64{src_m * src_n};

    auto mac = s64{s.mach_};
    mac <<= 32;
    mac |= s.macl_;
    mac += mul;

    if (s.regs_.sr.any(Sh2Regs::StatusRegister::s)) {
        if (mac < u47_min_64_extended) { mac = u47_min_64_extended; }
        if (mac > 0x800000000000) { mac = u47_max; }
    }
    s.mach_ = static_cast<u32>(mac >> 32);
    s.macl_ = static_cast<u32>(mac & u32_max);

    s.pc_ += 2;
    s.cycles_elapsed_ = 3;
}

void FastInterpreter::macw(Sh2& s, const u32 n, const u32 m) {
    // Signed operation, (Rn) * (Rm) + MAC -> MAC
    // Arranged using SH4 manual

    const auto src_n = static_cast<s64>(static_cast<s32>(s.modules_.memory()->read<u32>(s.r_[n])));
    s.r_[n] += 2;
    const auto src_m = static_cast<s64>(static_cast<s32>(s.modules_.memory()->read<u32>(s.r_[m])));
    s.r_[m] += 2;

    const auto mul = s64{src_m * src_n};
    auto       mac = s64{};
    if (!s.regs_.sr.any(Sh2Regs::StatusRegister::s)) {
        mac = s.mach_;
        mac <<= 32;
        mac |= s.macl_;
        mac += mul;
        s.mach_ = static_cast<u32>(mac >> 32);
        s.macl_ = static_cast<u32>(mac & u32_max);
    } else {
        if ((s.macl_ & 0x80000000) > 0) {
            mac = static_cast<s64>(s.macl_ | 0xFFFFFFFF00000000);
        } else {
            mac = static_cast<s64>(s.macl_ & u32_max);
        }
        mac += mul;
        if (mac > u31_max) {
            s.mach_ |= 0x00000001;
            s.macl_ = u31_max;
        } else if (mac < u31_min_64_extended) {
            s.mach_ |= 0x00000001;
            s.macl_ = 0x80000000;
        } else {
            s.mach_ &= 0xFFFFFFFE;
            s.macl_ = static_cast<u32>(mac & u32_max);
        }
    }

    s.pc_ += 2;
    s.cycles_elapsed_ = 3;
}

void FastInterpreter::mov(Sh2& s, const u32 n, const u32 m) {
    // Rm -> Rn
    s.r_[n] = s.r_[m];

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::movbs(Sh2& s, const u32 n, const u32 m) {
    // Rm -> (Rn)
    s.modules_.memory()->write<u8>(s.r_[n], static_cast<u8>(s.r_[m]));

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::movws(Sh2& s, const u32 n, const u32 m) {
    // Rm -> (Rn)
    s.modules_.memory()->write<u16>(s.r_[n], static_cast<u16>(s.r_[m]));

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::movls(Sh2& s, const u32 n, const u32 m) {
    // Rm -> (Rn)
    s.modules_.memory()->write<u32>(s.r_[n], s.r_[m]);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::movbl(Sh2& s, const u32 n, const u32 m) {
    // (Rm) -> sign extension -> Rn
    s.r_[n] = static_cast<s32>(static_cast<s8>(s.modules_.memory()->read<u8>(s.r_[m])));

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::movwl(Sh2& s, const u32 n, const u32 m) {
    // (Rm) -> sign extension -> Rn
    s.r_[n] = static_cast<s32>(static_cast<s16>(s.modules_.memory()->read<u16>(s.r_[m])));

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::movll(Sh2& s, const u32 n, const u32 m) {
    // (Rm) -> Rn
    s.r_[n] = s.modules_.memory()->read<u32>(s.r_[m]);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::movbm(Sh2& s, const u32 n, const u32 m) {
    // Rn - 1 -> Rn, Rm -> (Rn)
    s.modules_.memory()->write<u8>(s.r_[n] - 1, static_cast<u8>(s.r_[m]));
    s.r_[n] -= 1;

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::movwm(Sh2& s, const u32 n, const u32 m) {
    // Rn - 2 -> Rn, Rm -> (Rn)
    s.modules_.memory()->write<u16>(s.r_[n] - 2, static_cast<u16>(s.r_[m]));
    s.r_[n] -= 2;

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::movlm(Sh2& s, const u32 n, const u32 m) {
    // Rn - 4 -> Rn, Rm -> (Rn)
    s.modules_.memory()->write<u32>(s.r_[n] - 4, s.r_[m]);
    s.r_[n] -= 4;

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::movbp(Sh2& s, const u32 n, const u32 m) {
    // (Rm) -> sign extension -> Rn, Rm + 1 -> Rm
    s.r_[n] = static_cast<s32>(static_cast<s8>(s.modules_.memory()->read<u8>(s.r_[m])));
    if (n != m) { ++s.r_[m]; }

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::movwp(Sh2& s, const u32 n, const u32 m) {
    // (Rm) -> sign extension -> Rn, Rm + 2 -> Rm
    s.r_[n] = static_cast<s32>(static_cast<s16>(s.modules_.memory()->read<u16>(s.r_[m])));
    if (n != m) { s.r_[m] += 2; }

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::movlp(Sh2& s, const u32 n, const u32 m) {
    // (Rm) -> Rn, Rm + 4 -> Rm
    s.r_[n] = s.modules_.memory()->read<u32>(s.r_[m]);
    if (n != m) { s.r_[m] += 4; }
    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::movbs0(Sh2& s, const u32 n, const u32 m) {
    // Rm -> (R0 + Rn)
    s.modules_.memory()->write<u8>(s.r_[n] + s.r_[0], static_cast<u8>(s.r_[m]));

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::movws0(Sh2& s, const u32 n, const u32 m) {
    // Rm -> (R0 + Rn)
    s.modules_.memory()->write<u16>(s.r_[n] + s.r_[0], static_cast<u16>(s.r_[m]));

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::movls0(Sh2& s, const u32 n, const u32 m) {
    // Rm -> (R0 + Rn)
    s.modules_.memory()->write<u32>(s.r_[n] + s.r_[0], s.r_[m]);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::movbl0(Sh2& s, const u32 n, const u32 m) {
    // (R0 + Rm) -> sign extension -> Rn
    s.r_[n] = static_cast<s32>(static_cast<s8>(s.modules_.memory()->read<u8>(s.r_[m] + s.r_[0])));

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::movwl0(Sh2& s, const u32 n, const u32 m) {
    // (R0 + Rm) -> sign extension -> Rn
    s.r_[n] = static_cast<s32>(static_cast<s16>(s.modules_.memory()->read<u16>(s.r_[m] + s.r_[0])));

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::movll0(Sh2& s, const u32 n, const u32 m) {
    // (R0 + Rm) -> Rn
    s.r_[n] = s.modules_.memory()->read<u32>(s.r_[m] + s.r_[0]);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::movi(Sh2& s, const u32 n, const u32 i) {
    // imm -> sign extension -> Rn
    s.r_[n] = static_cast<s32>(static_cast<s8>(i));

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::movwi(Sh2& s, const u32 n, const u32 d) {
    //(disp * 2 + PC) -> sign extension -> Rn
    auto disp = u32{d};
    s.r_[n]   = static_cast<s32>(static_cast<s16>(s.modules_.memory()->read<u16>(s.pc_ + (disp << 1) + 4))); // +4 added

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::movli(Sh2& s, const u32 n, const u32 d) {
    //(disp * 4 + PC) -> Rn
    auto disp = u32{d};
    s.r_[n]   = s.modules_.memory()->read<u32>((s.pc_ & 0xFFFFFFFC) + (disp << 2) + 4); // + 4 added

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::movblg(Sh2& s, const u32 d) {
    //(disp + GBR) -> sign extension -> R0
    s.r_[0] = static_cast<s32>(static_cast<s8>(s.modules_.memory()->read<u8>(s.gbr_ + d)));

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::movwlg(Sh2& s, const u32 d) {
    // (disp *2 + GBR) -> sign extension -> R0
    s.r_[0] = static_cast<s32>(static_cast<s16>(s.modules_.memory()->read<u16>(s.gbr_ + (d << 1))));

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::movllg(Sh2& s, const u32 d) {
    // (disp *4 + GBR) -> R0
    s.r_[0] = s.modules_.memory()->read<u32>(s.gbr_ + (d << 2));

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::movbsg(Sh2& s, const u32 d) {
    // R0 -> (disp + GBR)
    s.modules_.memory()->write<u8>(s.gbr_ + d, static_cast<u8>(s.r_[0]));

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::movwsg(Sh2& s, const u32 d) {
    // R0 -> (disp *2 + GBR)
    s.modules_.memory()->write<u16>(s.gbr_ + (d << 1), static_cast<u16>(s.r_[0]));

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::movlsg(Sh2& s, const u32 d) {
    // R0 -> (disp *4 + GBR)
    s.modules_.memory()->write<u32>(s.gbr_ + (d << 2), s.r_[0]);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

inline void FastInterpreter::movbs4(Sh2& s, const u32 n, const u32 d) {
    // R0 -> (disp + Rn)
    s.modules_.memory()->write<u8>(s.r_[n] + d, static_cast<u8>(s.r_[0]));

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::movws4(Sh2& s, const u32 n, const u32 d) {
    // R0 -> (disp *2 + Rn)
    s.modules_.memory()->write<u16>(s.r_[n] + (d << 1), static_cast<u16>(s.r_[0]));

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::movls4(Sh2& s, const u32 n, const u32 m, const u32 d) {
    // Rm -> (disp *4 + Rn)
    s.modules_.memory()->write<u32>(s.r_[n] + (d << 2), s.r_[m]);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::movbl4(Sh2& s, const u32 m, const u32 d) {
    // (disp + Rm)-> sign extension ->R0
    s.r_[0] = static_cast<s32>(static_cast<s8>(s.modules_.memory()->read<u8>(s.r_[m] + d)));

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::movwl4(Sh2& s, const u32 m, const u32 d) {
    // (disp *2 + Rm)-> sign extension ->R0
    s.r_[0] = static_cast<s32>(static_cast<s16>(s.modules_.memory()->read<u16>(s.r_[m] + (d << 1))));

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::movll4(Sh2& s, const u32 n, const u32 m, const u32 d) {
    // (disp *4 +Rm) -> Rn
    s.r_[n] = s.modules_.memory()->read<u32>(s.r_[m] + (d << 2));

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::mova(Sh2& s, const u32 d) {
    // disp *4 + PC -> R0
    s.r_[0] = (s.pc_ & 0xFFFFFFFC) + (d << 2) + 4; // + 4 added

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::movt(Sh2& s, const u32 n) {
    // T -> Rn
    s.r_[n] = s.regs_.sr >> Sh2Regs::StatusRegister::t_shft;

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::mull(Sh2& s, const u32 n, const u32 m) {
    // Rn * Rm -> MACL
    s.macl_ = s.r_[n] * s.r_[m];

    s.pc_ += 2;
    s.cycles_elapsed_ = 2; // 2 to 4
}

void FastInterpreter::muls(Sh2& s, const u32 n, const u32 m) {
    // signed operation, Rn*Rm -> MACL
    s.macl_ = (static_cast<s32>(static_cast<s16>(s.r_[n])) * static_cast<s32>(static_cast<s16>(s.r_[m])));

    s.pc_ += 2;
    s.cycles_elapsed_ = 1; // 1 to 3
}

void FastInterpreter::mulu(Sh2& s, const u32 n, const u32 m) {
    // No sign, Rn+Rm -> MAC
    s.macl_ = (static_cast<u32>(static_cast<u16>(s.r_[n])) * static_cast<u32>(static_cast<u16>(s.r_[m])));

    s.pc_ += 2;
    s.cycles_elapsed_ = 1; // 1 to 3
}

void FastInterpreter::neg(Sh2& s, const u32 n, const u32 m) {
    // 0-Rm -> Rn
    s.r_[n] = 0 - s.r_[m];

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::negc(Sh2& s, const u32 n, const u32 m) {
    auto temp = u32{0 - s.r_[m]};
    s.r_[n]   = temp - (s.regs_.sr >> Sh2Regs::StatusRegister::t_shft);
    (0 < temp) ? s.regs_.sr.set(Sh2Regs::StatusRegister::t) : s.regs_.sr.clr(Sh2Regs::StatusRegister::t);
    if (temp < s.r_[n]) { s.regs_.sr.set(Sh2Regs::StatusRegister::t); }

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::nop(Sh2& s) {
    // No operation
    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::not_op(Sh2& s, const u32 n, const u32 m) {
    // -Rm -> Rn
    s.r_[n] = ~s.r_[m];

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::or_op(Sh2& s, const u32 n, const u32 m) {
    // Rn | Rm -> Rn
    s.r_[n] |= s.r_[m];

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::ori(Sh2& s, const u32 i) {
    // R0 | imm -> R0
    s.r_[0] |= i;

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::orm(Sh2& s, const u32 i) {
    // (R0 + GBR) | imm -> (R0 + GBR)
    auto temp = u32{s.modules_.memory()->read<u8>(s.gbr_ + s.r_[0])};
    temp |= i;
    s.modules_.memory()->write<u8>(s.gbr_ + s.r_[0], static_cast<u8>(temp));

    s.pc_ += 2;
    s.cycles_elapsed_ = 3;
}

void FastInterpreter::rotcl(Sh2& s, const u32 n) {
    // T <- Rn <- T
    auto temp = s32{((s.r_[n] & 0x80000000) == 0) ? 0 : 1};
    s.r_[n] <<= 1;
    if (s.regs_.sr.any(Sh2Regs::StatusRegister::t)) {
        s.r_[n] |= 0x00000001;
    } else {
        s.r_[n] &= 0xFFFFFFFE;
    }
    (temp == 1) ? s.regs_.sr.set(Sh2Regs::StatusRegister::t) : s.regs_.sr.clr(Sh2Regs::StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::rotcr(Sh2& s, const u32 n) {
    // T -> Rn -> T
    auto temp = s32{((s.r_[n] & 0x00000001) == 0) ? 0 : 1};
    s.r_[n] >>= 1;
    if (s.regs_.sr.any(Sh2Regs::StatusRegister::t)) {
        s.r_[n] |= 0x80000000;
    } else {
        s.r_[n] &= u31_max;
    }
    (temp == 1) ? s.regs_.sr.set(Sh2Regs::StatusRegister::t) : s.regs_.sr.clr(Sh2Regs::StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::rotl(Sh2& s, const u32 n) {
    // T <- Rn <- MSB
    ((s.r_[n] & 0x80000000) == 0) ? s.regs_.sr.clr(Sh2Regs::StatusRegister::t) : s.regs_.sr.set(Sh2Regs::StatusRegister::t);
    s.r_[n] <<= 1;
    if (s.regs_.sr.any(Sh2Regs::StatusRegister::t)) {
        s.r_[n] |= 0x00000001;
    } else {
        s.r_[n] &= 0xFFFFFFFE;
    }
    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::rotr(Sh2& s, const u32 n) {
    // LSB -> Rn -> T
    ((s.r_[n] & 0x00000001) == 0) ? s.regs_.sr.clr(Sh2Regs::StatusRegister::t) : s.regs_.sr.set(Sh2Regs::StatusRegister::t);
    s.r_[n] >>= 1;
    if (s.regs_.sr.any(Sh2Regs::StatusRegister::t)) {
        s.r_[n] |= 0x80000000;
    } else {
        s.r_[n] &= u31_max;
    }
    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::rte(Sh2& s) {
    // Stack -> PC/SR
    // Fixed
    delaySlot(s, s.pc_ + 2);
    s.pc_ = s.modules_.memory()->read<u32>(s.r_[sp_register_index]);
    s.r_[sp_register_index] += 4;
    s.regs_.sr = static_cast<u16>(s.modules_.memory()->read<u16>(s.r_[sp_register_index] + 2) & 0x000003f3);
    s.r_[sp_register_index] += 4;
    s.cycles_elapsed_ = 4;

    if (s.is_interrupted_) {
        // Current sh2 is interrupted, we get back to regular flow
        if (s.sh2_type_ == Sh2Type::master) {
            using enum Logger;
            s.modules_.scu()->clearInterruptFlag(s.current_interrupt_);
            Log::debug(Logger::sh2, "*** Back from interrupt ***");
            switch (s.current_interrupt_.vector) {
                case is::vector_v_blank_in: Log::debug(sh2, "VBlank-In interrupt routine finished"); break;
                case is::vector_v_blank_out: Log::debug(sh2, "VBlank-Out interrupt routine finished"); break;
                case is::vector_h_blank_in: Log::debug(sh2, "HBlank-In interrupt routine finished"); break;
                case is::vector_timer_0: Log::debug(sh2, "Timer 0 interrupt routine finished"); break;
                case is::vector_timer_1: Log::debug(sh2, "Timer 1 interrupt routine finished"); break;
                case is::vector_dsp_end: Log::debug(sh2, "DSP End interrupt routine finished"); break;
                case is::vector_sound_request: Log::debug(sh2, "Sound Request interrupt routine finished"); break;
                case is::vector_system_manager: Log::debug(sh2, "System Manager interrupt routine finished"); break;
                case is::vector_pad_interrupt: Log::debug(sh2, "Pad interrupt routine finished"); break;
                case is::vector_level_2_dma_end: Log::debug(sh2, "Level 2 DMA End interrupt routine finished"); break;
                case is::vector_level_1_dma_end: Log::debug(sh2, "Level 1 DMA End interrupt routine finished"); break;
                case is::vector_level_0_dma_end: Log::debug(sh2, "Level 0 DMA End interrupt routine finished"); break;
                case is::vector_dma_illegal: Log::debug(sh2, "DMA Illegal interrupt routine finished"); break;
                case is::vector_sprite_draw_end: Log::debug(sh2, "Sprite Draw End interrupt routine finished"); break;
                default: Log::warning(sh2, "Unknow interrupt vector:{:#0x}", s.current_interrupt_.level);
            }

            Log::debug(sh2, "Level:{:#0x}", s.current_interrupt_.level);
        }

        s.is_interrupted_                                   = false;
        s.current_interrupt_                                = is::undefined;
        s.is_level_interrupted_[s.current_interrupt_.level] = false;
    }
}

void FastInterpreter::rts(Sh2& s) {
    // PR -> PC
    // Arranged and fixed using SH4 manual
    delaySlot(s, s.pc_ + 2);

    s.popFromCallstack();
    switch (s.modules_.context()->debugStatus()) {
        using enum core::DebugStatus;
        case step_out:
        case wait_end_of_routine: {
            if (s.subroutineDepth() == s.callstack().size()) { s.modules_.context()->debugStatus(paused); }
            break;
        }
        default: break;
    }

    s.pc_             = s.pr_;
    s.cycles_elapsed_ = 2;
}

void FastInterpreter::sett(Sh2& s) {
    // 1 -> T
    s.regs_.sr.set(Sh2Regs::StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::shal(Sh2& s, const u32 n) {
    // T <- Rn <- 0
    ((s.r_[n] & 0x80000000) == 0) ? s.regs_.sr.clr(Sh2Regs::StatusRegister::t) : s.regs_.sr.set(Sh2Regs::StatusRegister::t);
    s.r_[n] <<= 1;

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::shar(Sh2& s, const u32 n) {
    // MSB -> Rn -> T
    ((s.r_[n] & 0x0000001) == 0) ? s.regs_.sr.clr(Sh2Regs::StatusRegister::t) : s.regs_.sr.set(Sh2Regs::StatusRegister::t);
    auto temp = s32{((s.r_[n] & 0x80000000) == 0) ? 0 : 1};
    s.r_[n] >>= 1;
    if (temp == 1) {
        s.r_[n] |= 0x80000000;
    } else {
        s.r_[n] &= u31_max;
    }
    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::shll(Sh2& s, const u32 n) {
    // T <- Rn <- 0
    ((s.r_[n] & 0x80000000) == 0) ? s.regs_.sr.clr(Sh2Regs::StatusRegister::t) : s.regs_.sr.set(Sh2Regs::StatusRegister::t);
    s.r_[n] <<= 1;

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::shll2(Sh2& s, const u32 n) {
    // Rn << 2 -> Rn
    s.r_[n] <<= 2;

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::shll8(Sh2& s, const u32 n) {
    // Rn << 8 -> Rn
    s.r_[n] <<= 8;

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::shll16(Sh2& s, const u32 n) {
    // Rn << 16 -> Rn
    s.r_[n] <<= 16;

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::shlr(Sh2& s, const u32 n) {
    // 0 -> Rn -> T
    ((s.r_[n] & 0x00000001) == 0) ? s.regs_.sr.clr(Sh2Regs::StatusRegister::t) : s.regs_.sr.set(Sh2Regs::StatusRegister::t);
    s.r_[n] >>= 1;
    s.r_[n] &= u31_max;

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::shlr2(Sh2& s, const u32 n) {
    // Rn >> 2 -> Rn
    s.r_[n] >>= 2;
    s.r_[n] &= u30_max;

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::shlr8(Sh2& s, const u32 n) {
    // Rn >> 8 -> Rn
    s.r_[n] >>= 8;
    s.r_[n] &= 0x00FFFFFF;

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::shlr16(Sh2& s, const u32 n) {
    // Rn >> 16 -> Rn
    s.r_[n] >>= 16;
    s.r_[n] &= 0x0000FFFF;

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::sleep(Sh2& s) {
    // Sleep
    // We'll see later how to implement this operation.
    // It'll involve waiting until an interrupt is fired up
    // Fixing needs some more researches on the Power Down Mode
    // Maybe a SH2 boolean to update ?

    if (s.is_nmi_registered_) {
        s.sendInterrupt(is::nmi);
        s.is_nmi_registered_ = false;
    }

    s.cycles_elapsed_ = 3;
}

void FastInterpreter::stcsr(Sh2& s, const u32 n) {
    // SR -> Rn
    s.r_[n] = s.regs_.sr.data();

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::stcgbr(Sh2& s, const u32 n) {
    // GBR -> Rn
    s.r_[n] = s.gbr_;

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::stcvbr(Sh2& s, const u32 n) {
    // VBR -> Rn
    s.r_[n] = s.vbr_;

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::stcmsr(Sh2& s, const u32 n) {
    // Rn-4 -> Rn, SR -> (Rn)
    s.r_[n] -= 4;
    s.modules_.memory()->write<u32>(s.r_[n], s.regs_.sr.data());

    s.pc_ += 2;
    s.cycles_elapsed_ = 2;
}

void FastInterpreter::stcmgbr(Sh2& s, const u32 n) {
    // Rn-4 -> Rn, GBR -> (Rn)
    s.r_[n] -= 4;
    s.modules_.memory()->write<u32>(s.r_[n], s.gbr_);

    s.pc_ += 2;
    s.cycles_elapsed_ = 2;
}

void FastInterpreter::stcmvbr(Sh2& s, const u32 n) {
    // Rn-4 -> Rn, VBR -> (Rn)
    s.r_[n] -= 4;
    s.modules_.memory()->write<u32>(s.r_[n], s.vbr_);

    s.pc_ += 2;
    s.cycles_elapsed_ = 2;
}

void FastInterpreter::stsmach(Sh2& s, const u32 n) {
    // MACH -> Rn
    s.r_[n] = s.mach_;

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::stsmacl(Sh2& s, const u32 n) {
    // MACL -> Rn
    s.r_[n] = s.macl_;

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::stspr(Sh2& s, const u32 n) {
    // PR -> Rn
    s.r_[n] = s.pr_;

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::stsmmach(Sh2& s, const u32 n) {
    // Rn - :4 -> Rn, MACH -> (Rn)
    s.r_[n] -= 4;
    s.modules_.memory()->write<u32>(s.r_[n], s.mach_);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::stsmmacl(Sh2& s, const u32 n) {
    // Rn - :4 -> Rn, MACL -> (Rn)
    s.r_[n] -= 4;
    s.modules_.memory()->write<u32>(s.r_[n], s.macl_);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::stsmpr(Sh2& s, const u32 n) {
    // Rn - :4 -> Rn, PR -> (Rn)
    s.r_[n] -= 4;
    s.modules_.memory()->write<u32>(s.r_[n], s.pr_);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::sub(Sh2& s, const u32 n, const u32 m) {
    // Rn - Rm -> Rn
    s.r_[n] -= s.r_[m];

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::subc(Sh2& s, const u32 n, const u32 m) {
    // Rn - Rm - T -> Rn, Carry -> T
    const auto tmp1 = u32{s.r_[n] - s.r_[m]};
    const auto tmp0 = u32{s.r_[n]};
    s.r_[n]         = tmp1 - (s.regs_.sr >> Sh2Regs::StatusRegister::t_shft);
    (tmp0 < tmp1) ? s.regs_.sr.set(Sh2Regs::StatusRegister::t) : s.regs_.sr.clr(Sh2Regs::StatusRegister::t);
    if (tmp1 < s.r_[n]) { s.regs_.sr.set(Sh2Regs::StatusRegister::t); }
    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::subv(Sh2& s, const u32 n, const u32 m) {
    // Rn - Rm -> Rn, underflow -> T
    const auto dest = s32{(static_cast<s32>(s.r_[n]) >= 0) ? 0 : 1};
    const auto src  = s32{(static_cast<s32>(s.r_[m]) >= 0) ? 0 : 1};

    s.r_[n] -= s.r_[m];
    auto ans = s32{(static_cast<s32>(s.r_[n]) >= 0) ? 0 : 1};
    ans += dest;

    if (src == 1) {
        (ans == 1) ? s.regs_.sr.set(Sh2Regs::StatusRegister::t) : s.regs_.sr.clr(Sh2Regs::StatusRegister::t);
    } else {
        s.regs_.sr.clr(Sh2Regs::StatusRegister::t);
    }
    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::swapb(Sh2& s, const u32 n, const u32 m) {
    // Rm -> bytes swap -> Rn
    const auto temp0 = u32{s.r_[m] & 0xFFFF0000};
    const auto temp1 = u32{(s.r_[m] & 0xFF) << 8};
    s.r_[n]          = (s.r_[m] >> 8) & 0xFF;
    s.r_[n]          = s.r_[n] | temp1 | temp0;

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::swapw(Sh2& s, const u32 n, const u32 m) {
    // Rm -> words swap -> Rn
    const auto temp = u32{(s.r_[m] >> 16) & 0x0000FFFF};
    s.r_[n]         = s.r_[m] << 16;
    s.r_[n] |= temp;

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::tas(Sh2& s, const u32 n) {
    // If (Rn) = 0, 1 -> T, 1 -> MSB of (Rn)
    auto temp = u32{s.modules_.memory()->read<u8>(s.r_[n])};
    (temp == 0) ? s.regs_.sr.set(Sh2Regs::StatusRegister::t) : s.regs_.sr.clr(Sh2Regs::StatusRegister::t);
    temp |= 0x80;
    s.modules_.memory()->write<u8>(s.r_[n], static_cast<u8>(temp));

    s.pc_ += 2;
    s.cycles_elapsed_ = 4;
}

void FastInterpreter::trapa(Sh2& s, const u32 i) {
    // PC/SR -> stack, (imm*4 + VBR) -> PC
    s.r_[sp_register_index] -= 4;
    s.modules_.memory()->write<u32>(s.r_[sp_register_index], s.regs_.sr.data());
    s.r_[sp_register_index] -= 4;
    s.modules_.memory()->write<u32>(s.r_[sp_register_index], s.pc_ + 2);

    s.pc_             = s.modules_.memory()->read<u32>(s.vbr_ + (i << 2));
    s.cycles_elapsed_ = 8; // NOLINT(readability-magic-numbers)
}

void FastInterpreter::tst(Sh2& s, const u32 n, const u32 m) {
    // Rn & Rm, if result = 0, 1 -> T
    ((s.r_[n] & s.r_[m]) == 0) ? s.regs_.sr.set(Sh2Regs::StatusRegister::t) : s.regs_.sr.clr(Sh2Regs::StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::tsti(Sh2& s, const u32 i) {
    // R0 & imm, if result is 0, 1 -> T
    ((s.r_[0] & i) == 0) ? s.regs_.sr.set(Sh2Regs::StatusRegister::t) : s.regs_.sr.clr(Sh2Regs::StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::tstm(Sh2& s, const u32 i) {
    // (R0 + GBR) & imm, if result is 0, 1 -> T
    auto temp = u32{s.modules_.memory()->read<u8>(s.gbr_ + s.r_[0])};
    temp &= i;
    (temp == 0) ? s.regs_.sr.set(Sh2Regs::StatusRegister::t) : s.regs_.sr.clr(Sh2Regs::StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 3;
}

void FastInterpreter::xor_op(Sh2& s, const u32 n, const u32 m) {
    // Rn^Rm -> Rn
    s.r_[n] ^= s.r_[m];

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::xori(Sh2& s, const u32 i) {
    // R0 ^imm -> R0
    s.r_[0] ^= i;

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::xorm(Sh2& s, const u32 i) {
    // (R0 + GBR)^imm -> (R0 + GBR)
    auto temp = u32{s.modules_.memory()->read<u8>(s.gbr_ + s.r_[0])};
    temp ^= i;
    s.modules_.memory()->write<u8>(s.gbr_ + s.r_[0], static_cast<u8>(temp));

    s.pc_ += 2;
    s.cycles_elapsed_ = 3;
}

void FastInterpreter::xtrct(Sh2& s, const u32 n, const u32 m) {
    // Middle 32 bits of Rm and Rn -> Rn
    const auto temp = u32{(s.r_[m] << 16) & 0xFFFF0000};
    s.r_[n]         = (s.r_[n] >> 16) & 0x0000FFFF;
    s.r_[n] |= temp;

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void FastInterpreter::execute(Sh2& s) {
    // switch (s.modules_.context()->debugStatus()) {
    //     using enum core::DebugStatus;
    //     case step_over: {
    //         if (!calls_subroutine_lut[s.current_opcode_]) {
    //             //
    //             s.modules_.context()->debugStatus(core::DebugStatus::paused);
    //         } else {
    //             s.modules_.context()->debugStatus(core::DebugStatus::wait_end_of_routine);
    //             s.initializeSubroutineDepth();
    //         }
    //         break;
    //     }
    //     case step_into: {
    //         s.modules_.context()->debugStatus(core::DebugStatus::paused);
    //         break;
    //     }
    //     default: break;
    // }

    // opcodes_lut[s.current_opcode_](s);

    // if (std::ranges::any_of(s.breakpoints_, [&s](const u32 bp) { return s.getRegister(Sh2Register::pc) == bp; })) {
    //     s.modules_.context()->debugStatus(core::DebugStatus::paused);
    //     Log::info(Logger::sh2, core::tr("Breakpoint reached !"));
    // }
}

} // namespace saturnin::sh2::fast_interpreter
