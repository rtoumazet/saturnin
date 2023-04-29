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
#include <saturnin/src/sh2/sh2.h> // Sh2, Sh2Type

namespace saturnin::sh2::fast_interpreter {
void add(Sh2& s, const u32 n, const u32 m) {
    // Rm + Rn -> Rn
    s.r_[n] += s.r_[m];

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void addi(Sh2& s, const u32 n, const u32 i) {
    // Rn + imm -> Rn
    auto imm = static_cast<s32>(static_cast<s8>(i));
    s.r_[n] += imm;

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void addc(Sh2& s, const u32 n, const u32 m) {
    // Rn + Rm + T -> Rn, carry -> T

    const auto tmp1 = static_cast<s32>(s.r_[n] + s.r_[m]);
    const auto tmp0 = static_cast<s32>(s.r_[n]);
    s.r_[n]         = tmp1 + (s.regs_.sr >> Sh2Regs::StatusRegister::t_shft);

    (tmp0 > tmp1) ? s.regs_.sr.set(Sh2Regs::StatusRegister::t) : s.regs_.sr.clr(Sh2Regs::StatusRegister::t);

    if (tmp1 > static_cast<s32>(s.r_[n])) { s.regs_.sr.set(Sh2Regs::StatusRegister::t); }
    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void addv(Sh2& s, const u32 n, const u32 m) {
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

void and_op(Sh2& s, const u32 n, const u32 m) {
    // Rn & Rm -> Rn
    s.r_[n] &= s.r_[m];
    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void andi(Sh2& s, const u32 i) {
    // R0 & imm -> R0
    s.r_[0] &= (0xFF & i);
    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void andm(Sh2& s, const u32 i) {
    //(R0 + GBR) & imm -> (R0 + GBR)

    auto temp = u32{s.modules_.memory()->read<u8>(s.gbr_ + s.r_[0])};
    temp &= (0xFF & i);
    s.modules_.memory()->write<u8>(s.gbr_ + s.r_[0], static_cast<u8>(temp));
    s.pc_ += 2;
    s.cycles_elapsed_ = 3;
}

void bf(Sh2& s, const u32 d) {
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

void bfs(Sh2& s, const u32 d) {
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

void bra(Sh2& s, const u32 d) {
    // disp*2 + PC -> PC
    // Modified using SH4 manual

    auto disp = u32{d};
    if ((d & sign_bit_12_mask) != 0) { disp = 0xFFFFF000 | d; }

    const auto saved_pc = u32{s.pc_};
    delaySlot(s, s.pc_ + 2);
    s.pc_             = saved_pc + (disp << 1) + 4;
    s.cycles_elapsed_ = 2;
}

void braf(Sh2& s, const u32 m) {
    // Rm + PC +4 -> PC
    // Modified using SH4 manual + correction
    // Registers save for the delay slot
    const auto old_pc = u32{s.pc_};
    const auto old_r  = u32{s.r_[m]};

    delaySlot(s, s.pc_ + 2);
    s.pc_             = old_pc + old_r + 4;
    s.cycles_elapsed_ = 2;
}

void bsr(Sh2& s, const u32 d) {
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

void bsrf(Sh2& s, const u32 m) {
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

void bt(Sh2& s, const u32 d) {
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

void bts(Sh2& s, const u32 d) {
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

void clrmac(Sh2& s) {
    // 0 -> MACH,MACL
    s.mach_ = 0;
    s.macl_ = 0;

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void clrt(Sh2& s) {
    // 0 -> T
    s.regs_.sr.clr(Sh2Regs::StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void cmpeq(Sh2& s, const u32 n, const u32 m) {
    // If Rn = Rm, T=1
    (s.r_[n] == s.r_[m]) ? s.regs_.sr.set(Sh2Regs::StatusRegister::t) : s.regs_.sr.clr(Sh2Regs::StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void cmpge(Sh2& s, const u32 n, const u32 m) {
    // If Rn >= Rm with sign, T=1
    (static_cast<s32>(s.r_[n]) >= static_cast<s32>(s.r_[m])) ? s.regs_.sr.set(Sh2Regs::StatusRegister::t)
                                                             : s.regs_.sr.clr(Sh2Regs::StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void cmpgt(Sh2& s, const u32 n, const u32 m) {
    // If Rn > Rm with sign, T=1
    (static_cast<s32>(s.r_[n]) > static_cast<s32>(s.r_[m])) ? s.regs_.sr.set(Sh2Regs::StatusRegister::t)
                                                            : s.regs_.sr.clr(Sh2Regs::StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void cmphi(Sh2& s, const u32 n, const u32 m) {
    // If Rn > Rm without sign, T=1
    (s.r_[n] > s.r_[m]) ? s.regs_.sr.set(Sh2Regs::StatusRegister::t) : s.regs_.sr.clr(Sh2Regs::StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void cmphs(Sh2& s, const u32 n, const u32 m) {
    // If Rn > Rm without sign, T=1
    (s.r_[n] >= s.r_[m]) ? s.regs_.sr.set(Sh2Regs::StatusRegister::t) : s.regs_.sr.clr(Sh2Regs::StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void cmppl(Sh2& s, const u32 n) {
    // If Rn > 0, T=1
    (static_cast<s32>(s.r_[n]) > 0) ? s.regs_.sr.set(Sh2Regs::StatusRegister::t) : s.regs_.sr.clr(Sh2Regs::StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void cmppz(Sh2& s, const u32 n) {
    // If Rn >= 0, T=1
    (static_cast<s32>(s.r_[n]) >= 0) ? s.regs_.sr.set(Sh2Regs::StatusRegister::t) : s.regs_.sr.clr(Sh2Regs::StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void cmpstr(Sh2& s, const u32 n, const u32 m) {
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

void cmpim(Sh2& s, const u32 i) {
    // ex: If R0 = imm, T=1

    auto imm = static_cast<s32>(static_cast<s8>(i));
    (s.r_[0] == imm) ? s.regs_.sr.set(Sh2Regs::StatusRegister::t) : s.regs_.sr.clr(Sh2Regs::StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void div0s(Sh2& s, const u32 n, const u32 m) {
    // Rn MSB -> Q, Rm MSB -> M, M^Q -> T
    ((s.r_[n] & sign_bit_32_mask) == 0) ? s.regs_.sr.clr(Sh2Regs::StatusRegister::q) : s.regs_.sr.set(Sh2Regs::StatusRegister::q);
    ((s.r_[m] & sign_bit_32_mask) == 0) ? s.regs_.sr.clr(Sh2Regs::StatusRegister::m) : s.regs_.sr.set(Sh2Regs::StatusRegister::m);
    ((s.regs_.sr >> Sh2Regs::StatusRegister::m_shft) == (s.regs_.sr >> Sh2Regs::StatusRegister::q_shft))
        ? s.regs_.sr.clr(Sh2Regs::StatusRegister::t)
        : s.regs_.sr.set(Sh2Regs::StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void div0u(Sh2& s) {
    // 0 -> M/Q/T
    s.regs_.sr.clr(Sh2Regs::StatusRegister::m);
    s.regs_.sr.clr(Sh2Regs::StatusRegister::q);
    s.regs_.sr.clr(Sh2Regs::StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void div1(Sh2& s, const u32 n, const u32 m) {
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

void dmuls(Sh2& s, const u32 n, const u32 m) {
    // With sign, Rn * Rm -> MACH,MACL

    // Arranged using SH4 manual
    const auto result = static_cast<s64>(static_cast<s32>(s.r_[m])) * static_cast<s32>(s.r_[n]);
    s.mach_           = result >> 32;
    s.macl_           = static_cast<u32>(result);

    s.pc_ += 2;
    s.cycles_elapsed_ = 2;
}

void dmulu(Sh2& s, const u32 n, const u32 m) {
    // Without sign, Rm * Rn -> MACH, MACL

    // MIGHT BE WRONG

    // Arranged using SH4 manual
    const auto result = u64{static_cast<u64>(s.r_[m]) * static_cast<u64>(s.r_[n])};
    s.mach_           = static_cast<u32>(result >> 32);
    s.macl_           = static_cast<u32>(result & u32_max);

    s.pc_ += 2;
    s.cycles_elapsed_ = 2;
}

void dt(Sh2& s, const u32 n) {
    // Rn - 1 -> Rn;
    // Si R[n] = 0, T=1
    // Sinon T=0
    --s.r_[n];
    (s.r_[n] == 0) ? s.regs_.sr.set(Sh2Regs::StatusRegister::t) : s.regs_.sr.clr(Sh2Regs::StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void extsb(Sh2& s, const u32 n, const u32 m) {
    // Rm sign extension (byte) -> Rn
    s.r_[n] = static_cast<s32>(static_cast<s8>(s.r_[m]));
    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void extsw(Sh2& s, const u32 n, const u32 m) {
    // Rm sign extension (word) -> Rn
    s.r_[n] = static_cast<s32>(static_cast<s16>(s.r_[m]));
    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void extub(Sh2& s, const u32 n, const u32 m) {
    // Rm is 0 extended (byte) -> Rn
    s.r_[n] = static_cast<u32>(static_cast<u8>(s.r_[m]));
    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void extuw(Sh2& s, const u32 n, const u32 m) {
    // Rm is 0 extended (word) -> Rn
    s.r_[n] = static_cast<u32>(static_cast<u16>(s.r_[m]));
    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void jmp(Sh2& s, const u32 m) {
    // Rm -> PC
    // Arranged and fixed using SH4 manual

    const auto old_r = u32{s.r_[m]};
    delaySlot(s, s.pc_ + 2);

    s.pc_             = old_r;
    s.cycles_elapsed_ = 2;
}

void jsr(Sh2& s, const u32 m) {
    // PC -> PR, Rm -> PC
    // Arranged and fixed using SH4 manual

    const auto old_r = u32{s.r_[m]};
    s.pr_            = s.pc_ + 4;
    delaySlot(s, s.pc_ + 2);

    s.addToCallstack(s.pc_, s.pr_);

    s.pc_             = old_r;
    s.cycles_elapsed_ = 2;
}

void ldcsr(Sh2& s, const u32 m) {
    // Rm -> SR
    s.regs_.sr = (s.r_[m] & 0x000003f3);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void ldcgbr(Sh2& s, const u32 m) {
    // Rm -> GBR
    s.gbr_ = s.r_[m];

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void ldcvbr(Sh2& s, const u32 m) {
    // Rm -> VBR
    s.vbr_ = s.r_[m];

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void ldcmsr(Sh2& s, const u32 m) {
    // (Rm) -> SR, Rm + 4 -> Rm
    s.regs_.sr = static_cast<u16>(s.modules_.memory()->read<u32>(s.r_[m]) & 0x000003f3);
    s.r_[m] += 4;

    s.pc_ += 2;
    s.cycles_elapsed_ = 3;
}

void ldcmgbr(Sh2& s, const u32 m) {
    // (Rm) -> GBR, Rm + 4 -> Rm
    s.gbr_ = s.modules_.memory()->read<u32>(s.r_[m]);
    s.r_[m] += 4;

    s.pc_ += 2;
    s.cycles_elapsed_ = 3;
}

void ldcmvbr(Sh2& s, const u32 m) {
    // (Rm) -> VBR, Rm + 4 -> Rm
    s.vbr_ = s.modules_.memory()->read<u32>(s.r_[m]);
    s.r_[m] += 4;

    s.pc_ += 2;
    s.cycles_elapsed_ = 3;
}

void ldsmach(Sh2& s, const u32 m) {
    // Rm -> MACH
    s.mach_ = s.r_[m];

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void ldsmacl(Sh2& s, const u32 m) {
    // Rm -> MACL
    s.mach_ = s.r_[m];

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void ldspr(Sh2& s, const u32 m) {
    // Rm -> PR
    s.pr_ = s.r_[m];

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void ldsmmach(Sh2& s, const u32 m) {
    //(Rm) -> MACH, Rm + 4 -> Rm
    s.mach_ = s.modules_.memory()->read<u32>(s.r_[m]);
    s.r_[m] += 4;

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void ldsmmacl(Sh2& s, const u32 m) {
    //(Rm) -> MACL, Rm + 4 -> Rm
    s.macl_ = s.modules_.memory()->read<u32>(s.r_[m]);
    s.r_[m] += 4;

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void ldsmpr(Sh2& s, const u32 m) {
    //(Rm) -> PR, Rm + 4 -> Rm
    s.pr_ = s.modules_.memory()->read<u32>(s.r_[m]);
    s.r_[m] += 4;

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void mac(Sh2& s, const u32 n, const u32 m) {
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

void macw(Sh2& s, const u32 n, const u32 m) {
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

void mov(Sh2& s, const u32 n, const u32 m) {
    // Rm -> Rn
    s.r_[n] = s.r_[m];

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void movbs(Sh2& s, const u32 n, const u32 m) {
    // Rm -> (Rn)
    s.modules_.memory()->write<u8>(s.r_[n], static_cast<u8>(s.r_[m]));

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void movws(Sh2& s, const u32 n, const u32 m) {
    // Rm -> (Rn)
    s.modules_.memory()->write<u16>(s.r_[n], static_cast<u16>(s.r_[m]));

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void movls(Sh2& s, const u32 n, const u32 m) {
    // Rm -> (Rn)
    s.modules_.memory()->write<u32>(s.r_[n], s.r_[m]);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void movbl(Sh2& s, const u32 n, const u32 m) {
    // (Rm) -> sign extension -> Rn
    s.r_[n] = static_cast<s32>(static_cast<s8>(s.modules_.memory()->read<u8>(s.r_[m])));

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void movwl(Sh2& s, const u32 n, const u32 m) {
    // (Rm) -> sign extension -> Rn
    s.r_[n] = static_cast<s32>(static_cast<s16>(s.modules_.memory()->read<u16>(s.r_[m])));

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void movll(Sh2& s, const u32 n, const u32 m) {
    // (Rm) -> Rn
    s.r_[n] = s.modules_.memory()->read<u32>(s.r_[m]);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void movbm(Sh2& s, const u32 n, const u32 m) {
    // Rn - 1 -> Rn, Rm -> (Rn)
    s.modules_.memory()->write<u8>(s.r_[n] - 1, static_cast<u8>(s.r_[m]));
    s.r_[n] -= 1;

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void movwm(Sh2& s, const u32 n, const u32 m) {
    // Rn - 2 -> Rn, Rm -> (Rn)
    s.modules_.memory()->write<u16>(s.r_[n] - 2, static_cast<u16>(s.r_[m]));
    s.r_[n] -= 2;

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void movlm(Sh2& s, const u32 n, const u32 m) {
    // Rn - 4 -> Rn, Rm -> (Rn)
    s.modules_.memory()->write<u32>(s.r_[n] - 4, s.r_[m]);
    s.r_[n] -= 4;

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void movbp(Sh2& s, const u32 n, const u32 m) {
    // (Rm) -> sign extension -> Rn, Rm + 1 -> Rm
    s.r_[n] = static_cast<s32>(static_cast<s8>(s.modules_.memory()->read<u8>(s.r_[m])));
    if (n != m) { ++s.r_[m]; }

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void movwp(Sh2& s, const u32 n, const u32 m) {
    // (Rm) -> sign extension -> Rn, Rm + 2 -> Rm
    s.r_[n] = static_cast<s32>(static_cast<s16>(s.modules_.memory()->read<u16>(s.r_[m])));
    if (n != m) { s.r_[m] += 2; }

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void movlp(Sh2& s, const u32 n, const u32 m) {
    // (Rm) -> Rn, Rm + 4 -> Rm
    s.r_[n] = s.modules_.memory()->read<u32>(s.r_[m]);
    if (n != m) { s.r_[m] += 4; }
    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void movbs0(Sh2& s, const u32 n, const u32 m) {
    // Rm -> (R0 + Rn)
    s.modules_.memory()->write<u8>(s.r_[n] + s.r_[0], static_cast<u8>(s.r_[m]));

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void movws0(Sh2& s, const u32 n, const u32 m) {
    // Rm -> (R0 + Rn)
    s.modules_.memory()->write<u16>(s.r_[n] + s.r_[0], static_cast<u16>(s.r_[m]));

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void movls0(Sh2& s, const u32 n, const u32 m) {
    // Rm -> (R0 + Rn)
    s.modules_.memory()->write<u32>(s.r_[n] + s.r_[0], s.r_[m]);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void movbl0(Sh2& s, const u32 n, const u32 m) {
    // (R0 + Rm) -> sign extension -> Rn
    s.r_[n] = static_cast<s32>(static_cast<s8>(s.modules_.memory()->read<u8>(s.r_[m] + s.r_[0])));

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void movwl0(Sh2& s, const u32 n, const u32 m) {
    // (R0 + Rm) -> sign extension -> Rn
    s.r_[n] = static_cast<s32>(static_cast<s16>(s.modules_.memory()->read<u16>(s.r_[m] + s.r_[0])));

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void movll0(Sh2& s, const u32 n, const u32 m) {
    // (R0 + Rm) -> Rn
    s.r_[n] = s.modules_.memory()->read<u32>(s.r_[m] + s.r_[0]);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

} // namespace saturnin::sh2::fast_interpreter
