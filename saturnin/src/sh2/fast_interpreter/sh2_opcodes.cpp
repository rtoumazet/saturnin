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
#include <saturnin/src/emulator_context.h> // EmulatorContext
#include <saturnin/src/emulator_defs.h>
#include <saturnin/src/emulator_enums.h>   // DebugStatus
#include <saturnin/src/interrupt_sources.h>
#include <saturnin/src/log.h>
#include <saturnin/src/memory.h>
#include <saturnin/src/scu.h>
#include <saturnin/src/sh2/sh2.h> // Sh2, Sh2Type

namespace saturnin::sh2 {
void add(Sh2& s, const int m, const int n) {
    // Rm + Rn -> Rn
    s.r_[n] += s.r_[m];

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void addi(Sh2& s, const int n, const int i) {
    // Rn + imm -> Rn
    if ((i & 0x80) == 0) {
        s.r_[n] += (0xFF & static_cast<u32>(i));       // #imm positive, 32bits sign extension
    } else {
        s.r_[n] += (0xFFFFFF00 | static_cast<u32>(i)); // #imm negative, 32bits sign extension
    }
    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void addc(Sh2& s, const int m, const int n) {
    // Rn + Rm + T -> Rn, carry -> T

    const auto tmp1 = static_cast<s32>(s.r_[n] + s.r_[m]);
    const auto tmp0 = static_cast<s32>(s.r_[n]);
    s.r_[n]         = tmp1 + (s.regs_.sr >> Sh2Regs::StatusRegister::t_shft);

    (tmp0 > tmp1) ? s.regs_.sr.set(Sh2Regs::StatusRegister::t) : s.regs_.sr.clr(Sh2Regs::StatusRegister::t);

    if (tmp1 > static_cast<s32>(s.r_[n])) { s.regs_.sr.set(Sh2Regs::StatusRegister::t); }
    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void addv(Sh2& s, const int m, const int n) {
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
} // namespace saturnin::sh2
