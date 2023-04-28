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
    if ((i & 0x80) == 0) {
        s.r_[n] += (0xFF & i);       // #imm positive, 32bits sign extension
    } else {
        s.r_[n] += (0xFFFFFF00 | i); // #imm negative, 32bits sign extension
    }
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
        auto disp = static_cast<s32>(static_cast<u8>(d));

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
        auto disp = static_cast<s32>(static_cast<u8>(d));

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
    const auto old_r  = u32{s.r_[xn00(s)]};
    delaySlot(s, s.pc_ + 2);
    s.pc_             = old_pc + 4 + old_r;
    s.cycles_elapsed_ = 2;

    s.addToCallstack(old_pc, s.pr_);
}

void bt(Sh2& s, const u32 d) {
    // If T=1, disp*2 + PC -> PC;
    // If T=0=, nop

    if (s.regs_.sr.any(Sh2Regs::StatusRegister::t)) {
        auto disp         = static_cast<s32>(static_cast<u8>(d));
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
        auto       disp   = static_cast<s32>(static_cast<u8>(d));
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
} // namespace saturnin::sh2::fast_interpreter
