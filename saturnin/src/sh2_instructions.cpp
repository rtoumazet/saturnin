//
// sh2_instructions.cpp
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

#include "sh2.h"
#include "sh2_instructions.h"
#include "emulator_context.h"
#include "emulator_defs.h"
#include "interrupt_sources.h"
#include "locale.h"
#include "memory.h"
#include "scu.h"

namespace is = saturnin::core::interrupt_source;

namespace saturnin::sh2 {

using core::Log;

constexpr u32 ignored_delay_slot_address{0x20000202};
constexpr u32 sr_bitmask{0x3f3};

auto xn00(Sh2& s) -> u16 { return (s.current_opcode_ & bitmask_0F00) >> displacement_8; }
auto x0n0(Sh2& s) -> u16 { return (s.current_opcode_ & bitmask_00F0) >> displacement_4; }
auto x00n(Sh2& s) -> u16 { return s.current_opcode_ & bitmask_000F; }
auto xnnn(Sh2& s) -> u16 { return s.current_opcode_ & bitmask_0FFF; }
auto x0nn(Sh2& s) -> u16 { return s.current_opcode_ & bitmask_00FF; }

void delaySlot(Sh2& s, const u32 addr) {
    // Algorithm :
    // Addr read and intruction fetch
    // if the instruction is BF,BT,BRA,BSR,JMP,JSR,RTS,RTE,TRAPA,BF/S,BT/S,BRAF or BSRF then
    //		-> illegal instruction slot
    // else
    //		Slot instruction execution
    // end

    u32 current_inst_cycles = s.cycles_elapsed_; // We musn't forget the DS instruction count
    if (addr != ignored_delay_slot_address) {    // Delay slot isn't detected after the Power On Reset (to prevent the "illegal
                                                 // instruction slot")

        s.current_opcode_ = s.memory()->read<u16>(addr);

        if (isInstructionIllegal(s.current_opcode_)) {
            Log::error("sh2", "Illegal instruction slot");
            s.emulatorContext()->emulationStatus(core::EmulationStatus::stopped);
        } else {
            // Delay slot instruction execution
            execute(s);
            s.cycles_elapsed_ += current_inst_cycles;
        }
    }
}

auto isInstructionIllegal(const u16 inst) -> bool {
    // 'Illegal Slot' detection
    // Returns true if an ISI (illegal slot instruction) is detected
    return illegal_instruction_lut[inst];
    // switch (xn000(inst)) {
    //    case 0b0000:
    //        switch (x000n(inst)) {
    //            case 0b0011:
    //                switch (x00n0(inst)) {
    //                    case 0b0000: return true; // BSRF Rm*2
    //                    case 0b0010: return true; // BRAF Rm*2
    //                }
    //                break;
    //            case 0xb1011:
    //                switch (x00n0(inst)) {
    //                    case 0b0000: return true; // RTS
    //                    case 0b0010: return true; // RTE
    //                }
    //                break;
    //        }
    //        break;
    //    case 0b0100:
    //        switch (x000n(inst)) {
    //            case 0b1011:
    //                switch (x00n0(inst)) {
    //                    case 0b0000: return true; // JSR @Rm
    //                    case 0b0010: return true; // JMP @Rm
    //                }
    //                break;
    //        }
    //        break;
    //    case 0b1000:
    //        switch (x0n00(inst)) {
    //            case 0b1001: return true; // BT label
    //            case 0b1011: return true; // BF label
    //            case 0x1101: return true; // BT/S label*2
    //            case 0x1111: return true; // BF/S label*2
    //        }
    //        break;
    //    case 0b1010: return true; // BRA label
    //    case 0b1011: return true; // BSR label
    //    case 0b1100:
    //        switch (x0n00(inst)) {
    //            case 0x0011: return true; // TRAPA #imm
    //        }
    //        break;
    //}
    // return false;
}

void badOpcode(Sh2& s) {
    std::string type = (s.sh2_type_ == Sh2Type::master) ? "Master" : "Slave";
    Log::error("Unexpected opcode({} SH2). Opcode = {:#06X}. PC = {:#010X}", type, s.current_opcode_, s.pc_);

    s.emulatorContext()->emulationStatus(core::EmulationStatus::stopped);
}

void add(Sh2& s) {
    // Rm + Rn -> Rn
    s.r_[xn00(s)] += s.r_[x0n0(s)];

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void addi(Sh2& s) {
    // Rn + imm -> Rn
    if ((x0nn(s) & sign_bit_8_mask) == 0) {
        s.r_[xn00(s)] += (bitmask_000000FF & static_cast<u32>(x0nn(s))); // #imm positive, 32bits sign extension
    } else {
        s.r_[xn00(s)] += (bitmask_FFFFFF00 | static_cast<u32>(x0nn(s))); // #imm negative, 32bits sign extension
    }
    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void addc(Sh2& s) {
    // Rn + Rm + T -> Rn, carry -> T

    s32 tmp1      = s.r_[xn00(s)] + s.r_[x0n0(s)];
    s32 tmp0      = s.r_[xn00(s)];
    s.r_[xn00(s)] = tmp1 + s.sr_.get(StatusRegister::t);

    (tmp0 > tmp1) ? s.sr_.set(StatusRegister::t) : s.sr_.reset(StatusRegister::t);

    if (tmp1 > static_cast<s32>(s.r_[xn00(s)])) { s.sr_.set(StatusRegister::t); }
    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void addv(Sh2& s) {
    // Rn + Rm -> Rn, overflow -> T

    s32 dest = (s.r_[xn00(s)] >= 0) ? 0 : 1;
    s32 src  = (s.r_[x0n0(s)] >= 0) ? 0 : 1;

    src += dest;
    s.r_[xn00(s)] += s.r_[x0n0(s)];

    s32 ans = (s.r_[xn00(s)] >= 0) ? 0 : 1;

    ans += dest;
    if (src == 0 || src == 2) {
        (ans == 1) ? s.sr_.set(StatusRegister::t) : s.sr_.reset(StatusRegister::t);
    } else {
        s.sr_.reset(StatusRegister::t);
    }
    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void and_op(Sh2& s) {
    // Rn & Rm -> Rn
    s.r_[xn00(s)] &= s.r_[x0n0(s)];
    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void andi(Sh2& s) {
    // R0 & imm -> R0
    s.r_[0] &= (bitmask_000000FF & x0nn(s));
    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void andm(Sh2& s) {
    //(R0 + GBR) & imm -> (R0 + GBR)

    u32 temp = s.memory()->read<u8>(s.gbr_ + s.r_[0]);
    temp &= (bitmask_000000FF & x0nn(s));
    s.memory()->write<u8>(s.gbr_ + s.r_[0], static_cast<u8>(temp));
    s.pc_ += 2;
    s.cycles_elapsed_ = 3;
}

void bf(Sh2& s) {
    // If T = 0, disp*2 + PC -> PC
    // Si T = 1, nop

    if (s.sr_.get(StatusRegister::t) == 0) {
        u32 disp{};
        if ((x0nn(s) & sign_bit_8_mask) == 0) {
            disp = (bitmask_000000FF & x0nn(s));
        } else {
            disp = (bitmask_FFFFFF00 | x0nn(s));
        }
        s.pc_             = s.pc_ + (disp << 1) + 4;
        s.cycles_elapsed_ = 3;
    } else {
        s.pc_ += 2;
        s.cycles_elapsed_ = 1;
    }
}

void bfs(Sh2& s) {
    // If T=0, disp*2 + PC -> PC
    // If T=1, nop
    // Modified using SH4 manual

    if (s.sr_.get(StatusRegister::t) == 0) {
        u32 disp{};
        if ((x0nn(s) & sign_bit_8_mask) == 0) {
            disp = (bitmask_000000FF & x0nn(s));
        } else {
            disp = (bitmask_FFFFFF00 | x0nn(s));
        }
        u32 saved_pc{s.pc_};
        delaySlot(s, s.pc_ + 2);
        s.pc_             = saved_pc + (disp << 1) + 4;
        s.cycles_elapsed_ = 2;
    } else {
        s.pc_ += 2;
        s.cycles_elapsed_ = 1;
    }
}

void bra(Sh2& s) {
    // disp*2 + PC -> PC
    // Modified using SH4 manual

    u32 disp{};
    if ((xnnn(s) & sign_bit_12_mask) == 0) {
        disp = (bitmask_00000FFF & xnnn(s));
    } else {
        disp = (bitmask_FFFFF000 | xnnn(s));
    }
    u32 saved_pc{s.pc_};
    delaySlot(s, s.pc_ + 2);
    s.pc_             = saved_pc + (disp << 1) + 4;
    s.cycles_elapsed_ = 2;
}

void braf(Sh2& s) {
    // Rm + PC +4 -> PC
    // Modified using SH4 manual + correction
    // Registers save for the delay slot
    u32 old_pc{s.pc_};
    u32 old_r{s.r_[xn00(s)]};

    delaySlot(s, s.pc_ + 2);
    s.pc_             = old_pc + old_r + 4;
    s.cycles_elapsed_ = 2;
}

void bsr(Sh2& s) {
    // PC -> PR, disp*2 + PC -> PC
    // Modified using SH4 manual + correction

    u32 disp{};
    if ((xnnn(s) & sign_bit_12_mask) == 0) {
        disp = (bitmask_00000FFF & xnnn(s));
    } else {
        disp = (bitmask_FFFFF000 | xnnn(s));
    }
    s.pr_ = s.pc_ + 4;
    u32 old_pc{s.pc_};
    delaySlot(s, s.pc_ + 2);
    s.pc_             = old_pc + (disp << 1) + 4;
    s.cycles_elapsed_ = 2;

    s.addToCallstack(old_pc, s.pr_);
}

void bsrf(Sh2& s) {
    // PC -> PR, Rm + PC -> PC
    // Modified using SH4 manual + correction
    // Registers save for the delay slot

    s.pr_ = s.pc_ + 4;

    u32 old_pc{s.pc_};
    u32 old_r{s.r_[xn00(s)]};
    delaySlot(s, s.pc_ + 2);
    s.pc_             = old_pc + 4 + old_r;
    s.cycles_elapsed_ = 2;

    s.addToCallstack(old_pc, s.pr_);
}

void bt(Sh2& s) {
    // If T=1, disp*2 + PC -> PC;
    // If T=0=, nop

    if (s.sr_.get(StatusRegister::t) == 1) {
        u32 disp{};
        if ((x0nn(s) & sign_bit_8_mask) == 0) {
            disp = (bitmask_000000FF & x0nn(s));
        } else {
            disp = (bitmask_FFFFFF00 | x0nn(s));
        }
        s.pc_             = s.pc_ + (disp << 1) + 4;
        s.cycles_elapsed_ = 3;
    } else {
        s.pc_ += 2;
        s.cycles_elapsed_ = 1;
    }
}

void bts(Sh2& s) {
    // If T=1, disp*2 + PC -> PC
    // If T=0, nop
    // Modified using SH4 manual
    if (s.sr_.get(StatusRegister::t) == 0x1) {
        u32 disp{};
        if ((x0nn(s) & sign_bit_8_mask) == 0) {
            disp = (bitmask_000000FF & x0nn(s));
        } else {
            disp = (bitmask_FFFFFF00 | x0nn(s));
        }
        u32 old_pc{s.pc_};
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
    s.sr_.reset(StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void cmpeq(Sh2& s) {
    // If Rn = Rm, T=1
    (s.r_[xn00(s)] == s.r_[x0n0(s)]) ? s.sr_.set(StatusRegister::t) : s.sr_.reset(StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void cmpge(Sh2& s) {
    // If Rn >= Rm with sign, T=1
    (static_cast<s32>(s.r_[xn00(s)]) >= static_cast<s32>(s.r_[x0n0(s)])) ? s.sr_.set(StatusRegister::t)
                                                                         : s.sr_.reset(StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void cmpgt(Sh2& s) {
    // If Rn > Rm with sign, T=1
    (static_cast<s32>(s.r_[xn00(s)]) > static_cast<s32>(s.r_[x0n0(s)])) ? s.sr_.set(StatusRegister::t)
                                                                        : s.sr_.reset(StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void cmphi(Sh2& s) {
    // If Rn > Rm without sign, T=1
    (static_cast<u32>(s.r_[xn00(s)]) > static_cast<u32>(s.r_[x0n0(s)])) ? s.sr_.set(StatusRegister::t)
                                                                        : s.sr_.reset(StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void cmphs(Sh2& s) {
    // If Rn > Rm without sign, T=1
    (static_cast<u32>(s.r_[xn00(s)]) >= static_cast<u32>(s.r_[x0n0(s)])) ? s.sr_.set(StatusRegister::t)
                                                                         : s.sr_.reset(StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void cmppl(Sh2& s) {
    // If Rn > 0, T=1
    (static_cast<s32>(s.r_[xn00(s)]) > 0) ? s.sr_.set(StatusRegister::t) : s.sr_.reset(StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void cmppz(Sh2& s) {
    // If Rn >= 0, T=1
    (static_cast<s32>(s.r_[xn00(s)]) >= 0) ? s.sr_.set(StatusRegister::t) : s.sr_.reset(StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void cmpstr(Sh2& s) {
    // If one byte of Rn = one byte of Rm then T=1

    u32 rm{s.r_[xn00(s)]};
    u32 rn{s.r_[x0n0(s)]};

    ((rm & bitmask_FF000000) == (rn & bitmask_FF000000) || (rm & bitmask_00FF0000) == (rn & bitmask_00FF0000)
     || (rm & bitmask_0000FF00) == (rn & bitmask_0000FF00) || (rm & bitmask_000000FF) == (rn & bitmask_000000FF))
        ? s.sr_.set(StatusRegister::t)
        : s.sr_.reset(StatusRegister::t);

    // u32 temp{s.r_[xn00(s)] ^ s.r_[x0n0(s)]};
    // s32 hh{(temp >> 12) & 0x000000FF};
    // s32 hl{(temp >> 8) & 0x000000FF};
    // s32 lh{(temp >> 4) & 0x000000FF};
    // s32 ll{temp & 0x000000FF};
    // hh = hh && hl && lh && ll;
    //(hh == 0) ? s.sr_.set(StatusRegister::t) : s.sr_.reset(StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void cmpim(Sh2& s) {
    // If R0 = imm, T=1
    u32 imm{};
    if ((x0nn(s) & sign_bit_8_mask) == 0) {
        imm = (bitmask_000000FF & x0nn(s));
    } else {
        imm = (bitmask_FFFFFF00 | x0nn(s));
    }
    (s.r_[0] == imm) ? s.sr_.set(StatusRegister::t) : s.sr_.reset(StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void div0s(Sh2& s) {
    // Rn MSB -> Q, Rm MSB -> M, M^Q -> T
    ((s.r_[xn00(s)] & sign_bit_32_mask) == 0) ? s.sr_.reset(StatusRegister::q) : s.sr_.set(StatusRegister::q);
    ((s.r_[x0n0(s)] & sign_bit_32_mask) == 0) ? s.sr_.reset(StatusRegister::m) : s.sr_.set(StatusRegister::m);
    (s.sr_.get(StatusRegister::m) == s.sr_.get(StatusRegister::q)) ? s.sr_.reset(StatusRegister::t)
                                                                   : s.sr_.set(StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void div0u(Sh2& s) {
    // 0 -> M/Q/T
    s.sr_.reset(StatusRegister::m);
    s.sr_.reset(StatusRegister::q);
    s.sr_.reset(StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void div1(Sh2& s) {
    // Division done in one pass (Rn + Rm)
    u32  tmp0{};
    bool tmp1{};

    u8 old_q{s.sr_.get(StatusRegister::q)};
    ((sign_bit_32_mask & s.r_[xn00(s)]) != 0) ? s.sr_.set(StatusRegister::q) : s.sr_.reset(StatusRegister::q);

    s.r_[xn00(s)] <<= 1;
    s.r_[xn00(s)] |= s.sr_.get(StatusRegister::t);
    switch (old_q) {
        case 0:
            switch (s.sr_.get(StatusRegister::m)) {
                case 0:
                    tmp0 = s.r_[xn00(s)];
                    s.r_[xn00(s)] -= s.r_[x0n0(s)];
                    tmp1 = (static_cast<u32>(s.r_[xn00(s)]) > tmp0);
                    switch (s.sr_.get(StatusRegister::q)) {
                        case 0: (tmp1) ? s.sr_.set(StatusRegister::q) : s.sr_.reset(StatusRegister::q); break;
                        case 1: (!tmp1) ? s.sr_.set(StatusRegister::q) : s.sr_.reset(StatusRegister::q); break;
                    }
                    break;
                case 1:
                    tmp0 = s.r_[xn00(s)];
                    s.r_[xn00(s)] += s.r_[x0n0(s)];
                    tmp1 = (static_cast<u32>(s.r_[xn00(s)]) < tmp0);
                    switch (s.sr_.get(StatusRegister::q)) {
                        case 0: (!tmp1) ? s.sr_.set(StatusRegister::q) : s.sr_.reset(StatusRegister::q); break;
                        case 1: (tmp1) ? s.sr_.set(StatusRegister::q) : s.sr_.reset(StatusRegister::q); break;
                    }
                    break;
            }
            break;
        case 1:
            switch (s.sr_.get(StatusRegister::m)) {
                case 0:
                    tmp0 = s.r_[xn00(s)];
                    s.r_[xn00(s)] += s.r_[x0n0(s)];
                    tmp1 = (static_cast<uint32_t>(s.r_[xn00(s)]) < tmp0);
                    switch (s.sr_.get(StatusRegister::q)) {
                        case 0: (tmp1) ? s.sr_.set(StatusRegister::q) : s.sr_.reset(StatusRegister::q); break;
                        case 1: (!tmp1) ? s.sr_.set(StatusRegister::q) : s.sr_.reset(StatusRegister::q); break;
                    }
                    break;
                case 1:
                    tmp0 = s.r_[xn00(s)];
                    s.r_[xn00(s)] -= s.r_[x0n0(s)];
                    tmp1 = (static_cast<u32>(s.r_[xn00(s)]) > tmp0);
                    switch (s.sr_.get(StatusRegister::q)) {
                        case 0: (!tmp1) ? s.sr_.set(StatusRegister::q) : s.sr_.reset(StatusRegister::q); break;
                        case 1: (tmp1) ? s.sr_.set(StatusRegister::q) : s.sr_.reset(StatusRegister::q); break;
                    }
                    break;
            }
            break;
    }
    (s.sr_.get(StatusRegister::q) == s.sr_.get(StatusRegister::m)) ? s.sr_.set(StatusRegister::t)
                                                                   : s.sr_.reset(StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void dmuls(Sh2& s) {
    // With sign, Rn * Rm -> MACH,MACL

    // Arranged using SH4 manual
    s64 result{static_cast<s64>(s.r_[x0n0(s)]) * static_cast<s64>(s.r_[xn00(s)])};
    s.mach_ = static_cast<s32>(result >> displacement_32);
    s.macl_ = static_cast<u32>(result & u32_max);

    s.pc_ += 2;
    s.cycles_elapsed_ = 2;
}

void dmulu(Sh2& s) {
    // Without sign, Rm * Rn -> MACH, MACL

    // MIGHT BE WRONG

    // Arranged using SH4 manual
    u64 result{static_cast<u64>(s.r_[x0n0(s)]) * static_cast<u64>(s.r_[xn00(s)])};
    s.mach_ = static_cast<u32>(result >> displacement_32);
    s.macl_ = static_cast<u32>(result & u32_max);

    s.pc_ += 2;
    s.cycles_elapsed_ = 2;
}

void dt(Sh2& s) {
    // Rn - 1 -> Rn;
    // Si R[n] = 0, T=1
    // Sinon T=0
    --s.r_[xn00(s)];
    (s.r_[xn00(s)] == 0) ? s.sr_.set(StatusRegister::t) : s.sr_.reset(StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void extsb(Sh2& s) {
    // Rm sign extension (byte) -> Rn
    s.r_[xn00(s)] = s.r_[x0n0(s)];
    if ((s.r_[x0n0(s)] & sign_bit_8_mask) == 0) {
        s.r_[xn00(s)] &= bitmask_000000FF;
    } else {
        s.r_[xn00(s)] |= bitmask_FFFFFF00;
    }
    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void extsw(Sh2& s) {
    // Rm sign extension (word) -> Rn
    s.r_[xn00(s)] = s.r_[x0n0(s)];
    if ((s.r_[x0n0(s)] & sign_bit_16_mask) == 0) {
        s.r_[xn00(s)] &= bitmask_0000FFFF;
    } else {
        s.r_[xn00(s)] |= bitmask_FFFF0000;
    }
    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void extub(Sh2& s) {
    // Rm is 0 extended (byte) -> Rn
    s.r_[xn00(s)] = s.r_[x0n0(s)];
    s.r_[xn00(s)] &= bitmask_000000FF;

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void extuw(Sh2& s) {
    // Rm is 0 extended (word) -> Rn
    s.r_[xn00(s)] = s.r_[x0n0(s)];
    s.r_[xn00(s)] &= bitmask_0000FFFF;

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void jmp(Sh2& s) {
    // Rm -> PC
    // Arranged and fixed using SH4 manual

    u32 old_r{s.r_[xn00(s)]};
    delaySlot(s, s.pc_ + 2);

    s.pc_             = old_r;
    s.cycles_elapsed_ = 2;
}

void jsr(Sh2& s) {
    // PC -> PR, Rm -> PC
    // Arranged and fixed using SH4 manual

    u32 old_r{s.r_[xn00(s)]};
    s.pr_ = s.pc_ + 4;
    delaySlot(s, s.pc_ + 2);

    s.addToCallstack(s.pc_, s.pr_);

    s.pc_             = old_r;
    s.cycles_elapsed_ = 2;
}

void ldcsr(Sh2& s) {
    // Rm -> SR
    s.sr_.set(StatusRegister::all_bits, static_cast<u16>(s.r_[xn00(s)] & sr_bitmask));

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void ldcgbr(Sh2& s) {
    // Rm -> GBR
    s.gbr_ = s.r_[xn00(s)];

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void ldcvbr(Sh2& s) {
    // Rm -> VBR
    s.vbr_ = s.r_[xn00(s)];

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void ldcmsr(Sh2& s) {
    // (Rm) -> SR, Rm + 4 -> Rm
    s.sr_.set(StatusRegister::all_bits, static_cast<u16>(s.memory()->read<u32>(s.r_[xn00(s)]) & sr_bitmask));
    s.r_[xn00(s)] += 4;

    s.pc_ += 2;
    s.cycles_elapsed_ = 3;
}

void ldcmgbr(Sh2& s) {
    // (Rm) -> GBR, Rm + 4 -> Rm
    s.gbr_ = s.memory()->read<u32>(s.r_[xn00(s)]);
    s.r_[xn00(s)] += 4;

    s.pc_ += 2;
    s.cycles_elapsed_ = 3;
}

void ldcmvbr(Sh2& s) {
    // (Rm) -> VBR, Rm + 4 -> Rm
    s.vbr_ = s.memory()->read<u32>(s.r_[xn00(s)]);
    s.r_[xn00(s)] += 4;

    s.pc_ += 2;
    s.cycles_elapsed_ = 3;
}

void ldsmach(Sh2& s) {
    // Rm -> MACH
    s.mach_ = s.r_[xn00(s)];

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void ldsmacl(Sh2& s) {
    // Rm -> MACL
    s.mach_ = s.r_[xn00(s)];

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void ldspr(Sh2& s) {
    // Rm -> PR
    s.pr_ = s.r_[xn00(s)];

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void ldsmmach(Sh2& s) {
    //(Rm) -> MACH, Rm + 4 -> Rm
    s.mach_ = s.memory()->read<u32>(s.r_[xn00(s)]);
    s.r_[xn00(s)] += 4;

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void ldsmmacl(Sh2& s) {
    //(Rm) -> MACL, Rm + 4 -> Rm
    s.macl_ = s.memory()->read<u32>(s.r_[xn00(s)]);
    s.r_[xn00(s)] += 4;

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void ldsmpr(Sh2& s) {
    //(Rm) -> PR, Rm + 4 -> Rm
    s.pr_ = s.memory()->read<u32>(s.r_[xn00(s)]);
    s.r_[xn00(s)] += 4;

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void mac(Sh2& s) {
    // Signed operation, (Rn)*(Rm) + MAC -> MAC
    // Arranged using SH4 manual

    s64 src_n{static_cast<s64>(static_cast<s32>(s.memory()->read<u32>(s.r_[xn00(s)])))};
    s.r_[xn00(s)] += 4;
    s64 src_m{static_cast<s64>(static_cast<s32>(s.memory()->read<u32>(s.r_[x0n0(s)])))};
    s.r_[x0n0(s)] += 4;

    s64 mul{src_m * src_n};

    s64 mac{s.mach_};
    mac <<= displacement_32;
    mac |= s.macl_;
    mac += mul;

    if (s.sr_.get(StatusRegister::s) == 1) {
        if (mac < u47_min_64_extended) { mac = u47_min_64_extended; }
        if (mac > sign_bit_48_mask) { mac = u47_max; }
    }
    s.mach_ = static_cast<u32>(mac >> displacement_32);
    s.macl_ = static_cast<u32>(mac & u32_max);

    s.pc_ += 2;
    s.cycles_elapsed_ = 3;
}

void macw(Sh2& s) {
    // Signed operation, (Rn) * (Rm) + MAC -> MAC
    // Arranged using SH4 manual

    s64 src_n{static_cast<s64>(static_cast<s32>(s.memory()->read<u32>(s.r_[xn00(s)])))};
    s.r_[xn00(s)] += 2;
    s64 src_m{static_cast<s64>(static_cast<s32>(s.memory()->read<u32>(s.r_[x0n0(s)])))};
    s.r_[x0n0(s)] += 2;

    s64 mul = src_m * src_n;
    s64 mac{};
    if (s.sr_.get(StatusRegister::s) == 0) {
        mac = s.mach_;
        mac <<= displacement_32;
        mac |= s.macl_;
        mac += mul;
        s.mach_ = static_cast<u32>(mac >> displacement_32);
        s.macl_ = static_cast<u32>(mac & u32_max);
    } else {
        if ((s.macl_ & sign_bit_32_mask) > 0) {
            mac = static_cast<s64>(s.macl_ | bitmask_FFFFFFFF00000000);
        } else {
            mac = static_cast<s64>(s.macl_ & u32_max);
        }
        mac += mul;
        if (mac > u31_max) {
            s.mach_ |= 0x00000001;
            s.macl_ = u31_max;
        } else if (mac < u31_min_64_extended) {
            s.mach_ |= 0x00000001;
            s.macl_ = sign_bit_32_mask;
        } else {
            s.mach_ &= bitmask_FFFFFFFE;
            s.macl_ = static_cast<u32>(mac & u32_max);
        }
    }
    s.pc_ += 2;
    s.cycles_elapsed_ = 3;
} // namespace saturnin::sh2

void mov(Sh2& s) {
    // Rm -> Rn
    s.r_[xn00(s)] = s.r_[x0n0(s)];

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void movbs(Sh2& s) {
    // Rm -> (Rn)
    s.memory()->write<u8>(s.r_[xn00(s)], static_cast<u8>(s.r_[x0n0(s)]));

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void movws(Sh2& s) {
    // Rm -> (Rn)
    s.memory()->write<u16>(s.r_[xn00(s)], static_cast<u16>(s.r_[x0n0(s)]));

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void movls(Sh2& s) {
    // Rm -> (Rn)
    s.memory()->write<u32>(s.r_[xn00(s)], s.r_[x0n0(s)]);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void movbl(Sh2& s) {
    // (Rm) -> sign extension -> Rn
    s.r_[xn00(s)] = s.memory()->read<u8>(s.r_[x0n0(s)]);
    if ((s.r_[xn00(s)] & sign_bit_8_mask) == 0) {
        s.r_[xn00(s)] &= bitmask_000000FF;
    } else {
        s.r_[xn00(s)] |= bitmask_FFFFFF00;
    }
    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void movwl(Sh2& s) {
    // (Rm) -> sign extension -> Rn
    s.r_[xn00(s)] = s.memory()->read<u16>(s.r_[x0n0(s)]);
    if ((s.r_[xn00(s)] & sign_bit_16_mask) == 0) {
        s.r_[xn00(s)] &= bitmask_0000FFFF;
    } else {
        s.r_[xn00(s)] |= bitmask_FFFF0000;
    }
    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void movll(Sh2& s) {
    // (Rm) -> Rn
    s.r_[xn00(s)] = s.memory()->read<u32>(s.r_[x0n0(s)]);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void movbm(Sh2& s) {
    // Rn - 1 -> Rn, Rm -> (Rn)
    s.memory()->write<u8>(s.r_[xn00(s)] - 1, static_cast<u8>(s.r_[x0n0(s)]));
    s.r_[xn00(s)] -= 1;

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void movwm(Sh2& s) {
    // Rn - 2 -> Rn, Rm -> (Rn)
    s.memory()->write<u16>(s.r_[xn00(s)] - 2, static_cast<u16>(s.r_[x0n0(s)]));
    s.r_[xn00(s)] -= 2;

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void movlm(Sh2& s) {
    // Rn - 4 -> Rn, Rm -> (Rn)
    s.memory()->write<u32>(s.r_[xn00(s)] - 4, s.r_[x0n0(s)]);
    s.r_[xn00(s)] -= 4;

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void movbp(Sh2& s) {
    // (Rm) -> sign extension -> Rn, Rm + 1 -> Rm
    s.r_[xn00(s)] = s.memory()->read<u8>(s.r_[x0n0(s)]);
    if ((s.r_[xn00(s)] & sign_bit_8_mask) == 0) {
        s.r_[xn00(s)] &= bitmask_000000FF;
    } else {
        s.r_[xn00(s)] |= bitmask_FFFFFF00;
    }
    if (xn00(s) != x0n0(s)) { ++s.r_[x0n0(s)]; }
    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void movwp(Sh2& s) {
    // (Rm) -> sign extension -> Rn, Rm + 2 -> Rm
    s.r_[xn00(s)] = s.memory()->read<u16>(s.r_[x0n0(s)]);
    if ((s.r_[xn00(s)] & sign_bit_16_mask) == 0) {
        s.r_[xn00(s)] &= bitmask_0000FFFF;
    } else {
        s.r_[xn00(s)] |= bitmask_FFFF0000;
    }
    if (xn00(s) != x0n0(s)) { s.r_[x0n0(s)] += 2; }
    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void movlp(Sh2& s) {
    // (Rm) -> Rn, Rm + 4 -> Rm
    s.r_[xn00(s)] = s.memory()->read<u32>(s.r_[x0n0(s)]);
    if (xn00(s) != x0n0(s)) { s.r_[x0n0(s)] += 4; }
    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void movbs0(Sh2& s) {
    // Rm -> (R0 + Rn)
    s.memory()->write<u8>(s.r_[xn00(s)] + s.r_[0], static_cast<u8>(s.r_[x0n0(s)]));

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void movws0(Sh2& s) {
    // Rm -> (R0 + Rn)
    s.memory()->write<u16>(s.r_[xn00(s)] + s.r_[0], static_cast<uint16_t>(s.r_[x0n0(s)]));

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void movls0(Sh2& s) {
    // Rm -> (R0 + Rn)
    s.memory()->write<u32>(s.r_[xn00(s)] + s.r_[0], s.r_[x0n0(s)]);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void movbl0(Sh2& s) {
    // (R0 + Rm) -> sign extension -> Rn
    s.r_[xn00(s)] = s.memory()->read<u8>(s.r_[x0n0(s)] + s.r_[0]);
    if ((s.r_[xn00(s)] & sign_bit_8_mask) == 0) {
        s.r_[xn00(s)] &= bitmask_000000FF;
    } else {
        s.r_[xn00(s)] |= bitmask_FFFFFF00;
    }
    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void movwl0(Sh2& s) {
    // (R0 + Rm) -> sign extension -> Rn
    s.r_[xn00(s)] = s.memory()->read<u16>(s.r_[x0n0(s)] + s.r_[0]);
    if ((s.r_[xn00(s)] & sign_bit_16_mask) == 0) {
        s.r_[xn00(s)] &= bitmask_0000FFFF;
    } else {
        s.r_[xn00(s)] |= bitmask_FFFF0000;
    }
    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void movll0(Sh2& s) {
    // (R0 + Rm) -> Rn
    s.r_[xn00(s)] = s.memory()->read<u32>(s.r_[x0n0(s)] + s.r_[0]);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void movi(Sh2& s) {
    // imm -> sign extension -> Rn
    if ((x0nn(s) & sign_bit_8_mask) == 0) {
        s.r_[xn00(s)] = (bitmask_000000FF & x0nn(s));
    } else {
        s.r_[xn00(s)] = (bitmask_FFFFFF00 | x0nn(s));
    }
    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void movwi(Sh2& s) {
    //(disp * 2 + PC) -> sign extension -> Rn
    u32 disp{(bitmask_000000FF & x0nn(s))};
    s.r_[xn00(s)] = s.memory()->read<u16>(s.pc_ + (disp << 1) + 4); // + 4 added
    if ((s.r_[xn00(s)] & sign_bit_16_mask) == 0) {
        s.r_[xn00(s)] &= bitmask_0000FFFF;
    } else {
        s.r_[xn00(s)] |= bitmask_FFFF0000;
    }
    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void movli(Sh2& s) {
    //(disp * 4 + PC) -> Rn
    u32 disp{(bitmask_000000FF & x0nn(s))};
    s.r_[xn00(s)] = s.memory()->read<u32>((s.pc_ & bitmask_FFFFFFFC) + (disp << 2) + 4); // + 4 added

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void movblg(Sh2& s) {
    //(disp + GBR) -> sign extension -> R0
    u32 disp{(bitmask_000000FF & x0nn(s))};
    s.r_[0] = s.memory()->read<u8>(s.gbr_ + disp);
    if ((s.r_[0] & sign_bit_8_mask) == 0) {
        s.r_[0] &= bitmask_000000FF;
    } else {
        s.r_[0] |= bitmask_FFFFFF00;
    }
    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void movwlg(Sh2& s) {
    // (disp *2 + BGR) -> sign extension -> R0
    u32 disp{(bitmask_000000FF & x0nn(s))};
    s.r_[0] = s.memory()->read<u16>(s.gbr_ + (disp << 1));
    if ((s.r_[0] & sign_bit_16_mask) == 0) {
        s.r_[0] &= bitmask_0000FFFF;
    } else {
        s.r_[0] |= bitmask_FFFF0000;
    }
    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void movllg(Sh2& s) {
    // (disp *4 + GBR) -> R0
    u32 disp{(bitmask_000000FF & x0nn(s))};
    s.r_[0] = s.memory()->read<u32>(s.gbr_ + (disp << 2));

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void movbsg(Sh2& s) {
    // R0 -> (disp + GBR)
    u32 disp{(bitmask_000000FF & x0nn(s))};
    s.memory()->write<u8>(s.gbr_ + disp, static_cast<u8>(s.r_[0]));

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void movwsg(Sh2& s) {
    // R0 -> (disp *2 + GBR)
    u32 disp{(bitmask_000000FF & x0nn(s))};
    s.memory()->write<u16>(s.gbr_ + (disp << 1), static_cast<u16>(s.r_[0]));

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void movlsg(Sh2& s) {
    // R0 -> (disp *4 + GBR)
    u32 disp{(bitmask_000000FF & x0nn(s))};
    s.memory()->write<u32>(s.gbr_ + (disp << 2), s.r_[0]);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

inline void movbs4(Sh2& s) {
    // R0 -> (disp + Rn)
    u32 disp{(bitmask_0000000F & x00n(s))};
    s.memory()->write<u8>(s.r_[x0n0(s)] + disp, static_cast<u8>(s.r_[0]));

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void movws4(Sh2& s) {
    // R0 -> (disp *2 + Rn)
    u32 disp{(bitmask_0000000F & x00n(s))};
    s.memory()->write<u16>(s.r_[x0n0(s)] + (disp << 1), static_cast<u16>(s.r_[0]));

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void movls4(Sh2& s) {
    // Rm -> (disp *4 + Rn)
    u32 disp{(bitmask_0000000F & x00n(s))};
    s.memory()->write<u32>(s.r_[xn00(s)] + (disp << 2), s.r_[x0n0(s)]);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void movbl4(Sh2& s) {
    // (disp + Rm)-> sign extension ->R0
    u32 disp = (bitmask_0000000F & x00n(s));
    s.r_[0]  = s.memory()->read<u8>(s.r_[x0n0(s)] + disp);
    if ((s.r_[0] & sign_bit_8_mask) == 0) {
        s.r_[0] &= bitmask_000000FF;
    } else {
        s.r_[0] |= bitmask_FFFFFF00;
    }
    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void movwl4(Sh2& s) {
    // (disp *2 + Rm)-> sign extension ->R0
    u32 disp{(bitmask_0000000F & x00n(s))};
    s.r_[0] = s.memory()->read<u16>(s.r_[x0n0(s)] + (disp << 1));
    if ((s.r_[0] & sign_bit_16_mask) == 0) {
        s.r_[0] &= bitmask_0000FFFF;
    } else {
        s.r_[0] |= bitmask_FFFF0000;
    }
    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void movll4(Sh2& s) {
    // (disp *4 +Rm) -> Rn
    u32 disp{(bitmask_0000000F & x00n(s))};
    s.r_[xn00(s)] = s.memory()->read<u32>(s.r_[x0n0(s)] + (disp << 2));

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void mova(Sh2& s) {
    // disp *4 + PC -> R0
    u32 disp{(bitmask_000000FF & x0nn(s))};
    s.r_[0] = (s.pc_ & bitmask_FFFFFFFC) + (disp << 2) + 4; // + 4 added

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void movt(Sh2& s) {
    // T -> Rn
    s.r_[xn00(s)] = s.sr_.get(StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void mull(Sh2& s) {
    // Rn * Rm -> MACL
    s.macl_ = s.r_[xn00(s)] * s.r_[x0n0(s)];

    s.pc_ += 2;
    s.cycles_elapsed_ = 2; // 2 to 4
}

void muls(Sh2& s) {
    // signed operation, Rn*Rm -> MACL
    s.macl_ = (static_cast<s32>(static_cast<s16>(s.r_[xn00(s)])) * static_cast<s32>(static_cast<s16>(s.r_[x0n0(s)])));

    s.pc_ += 2;
    s.cycles_elapsed_ = 1; // 1 to 3
}

void mulu(Sh2& s) {
    // No sign, Rn+Rm -> MAC
    s.macl_ = (static_cast<u32>(static_cast<u16>(s.r_[xn00(s)])) * static_cast<u32>(static_cast<u16>(s.r_[x0n0(s)])));

    s.pc_ += 2;
    s.cycles_elapsed_ = 1; // 1 to 3
}

void neg(Sh2& s) {
    // 0-Rm -> Rn
    s.r_[xn00(s)] = 0 - s.r_[x0n0(s)];

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void negc(Sh2& s) {
    u32 temp{0 - s.r_[x0n0(s)]};
    s.r_[xn00(s)] = temp - s.sr_.get(StatusRegister::t);
    (0 < temp) ? s.sr_.set(StatusRegister::t) : s.sr_.reset(StatusRegister::t);
    if (temp < static_cast<u32>(s.r_[xn00(s)])) { s.sr_.set(StatusRegister::t); }
    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void nop(Sh2& s) {
    // No operation
    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void not_op(Sh2& s) {
    // -Rm -> Rn
    s.r_[xn00(s)] = ~s.r_[x0n0(s)];

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void or_op(Sh2& s) {
    // Rn | Rm -> Rn
    s.r_[xn00(s)] |= s.r_[x0n0(s)];

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void ori(Sh2& s) {
    // R0 | imm -> R0
    s.r_[0] |= (bitmask_000000FF & x0nn(s));

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void orm(Sh2& s) {
    // (R0 + GBR) | imm -> (R0 + GBR)
    u32 temp{s.memory()->read<u8>(s.gbr_ + s.r_[0])};
    temp |= (bitmask_000000FF & x0nn(s));
    s.memory()->write<u8>(s.gbr_ + s.r_[0], static_cast<u8>(temp));

    s.pc_ += 2;
    s.cycles_elapsed_ = 3;
}

void rotcl(Sh2& s) {
    // T <- Rn <- T
    s32 temp{((s.r_[xn00(s)] & sign_bit_32_mask) == 0) ? 0 : 1};
    s.r_[xn00(s)] <<= 1;
    if (s.sr_.get(StatusRegister::t) == 1) {
        s.r_[xn00(s)] |= 0x00000001;
    } else {
        s.r_[xn00(s)] &= bitmask_FFFFFFFE;
    }
    (temp == 1) ? s.sr_.set(StatusRegister::t) : s.sr_.reset(StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void rotcr(Sh2& s) {
    // T -> Rn -> T
    s32 temp{((s.r_[xn00(s)] & 0x00000001) == 0) ? 0 : 1};
    s.r_[xn00(s)] >>= 1;
    if (s.sr_.get(StatusRegister::t) == 1) {
        s.r_[xn00(s)] |= sign_bit_32_mask;
    } else {
        s.r_[xn00(s)] &= u31_max;
    }
    (temp == 1) ? s.sr_.set(StatusRegister::t) : s.sr_.reset(StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void rotl(Sh2& s) {
    // T <- Rn <- MSB
    // NOLINTNEXTLINE(readability-magic-numbers)
    ((s.r_[xn00(s)] & sign_bit_32_mask) == 0) ? s.sr_.reset(StatusRegister::t) : s.sr_.set(StatusRegister::t);
    s.r_[xn00(s)] <<= 1;
    if (s.sr_.get(StatusRegister::t) == 1) {
        s.r_[xn00(s)] |= 0x00000001;
    } else {
        s.r_[xn00(s)] &= bitmask_FFFFFFFE;
    }
    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void rotr(Sh2& s) {
    // LSB -> Rn -> T
    ((s.r_[xn00(s)] & 0x00000001) == 0) ? s.sr_.reset(StatusRegister::t) : s.sr_.set(StatusRegister::t);
    s.r_[xn00(s)] >>= 1;
    if (s.sr_.get(StatusRegister::t) == 1) {
        s.r_[xn00(s)] |= sign_bit_32_mask;
    } else {
        s.r_[xn00(s)] &= u31_max;
    }
    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void rte(Sh2& s) {
    // Stack -> PC/SR
    // Fixed
    delaySlot(s, s.pc_ + 2);
    s.pc_ = s.memory()->read<u32>(s.r_[sp_register_index]);
    s.r_[sp_register_index] += 4;
    s.sr_.set(StatusRegister::all_bits, static_cast<u16>(s.memory()->read<u16>(s.r_[sp_register_index] + 2) & sr_bitmask));
    s.r_[sp_register_index] += 4;
    s.cycles_elapsed_ = 4;

    if (s.is_interrupted_) {
        // Current sh2 is interrupted, we get back to regular flow
        if (s.sh2_type_ == Sh2Type::master) {
            s.scu()->clearInterruptFlag(s.current_interrupt_);
            Log::debug("sh2", "*** Back from interrupt ***");
            switch (s.current_interrupt_.vector) {
                case is::vector_v_blank_in: Log::debug("sh2", "VBlank-In interrupt routine finished"); break;
                case is::vector_v_blank_out: Log::debug("sh2", "VBlank-Out interrupt routine finished"); break;
                case is::vector_h_blank_in: Log::debug("sh2", "HBlank-In interrupt routine finished"); break;
                case is::vector_timer_0: Log::debug("sh2", "Timer 0 interrupt routine finished"); break;
                case is::vector_timer_1: Log::debug("sh2", "Timer 1 interrupt routine finished"); break;
                case is::vector_dsp_end: Log::debug("sh2", "DSP End interrupt routine finished"); break;
                case is::vector_sound_request: Log::debug("sh2", "Sound Request interrupt routine finished"); break;
                case is::vector_system_manager: Log::debug("sh2", "System Manager interrupt routine finished"); break;
                case is::vector_pad_interrupt: Log::debug("sh2", "Pad interrupt routine finished"); break;
                case is::vector_level_2_dma_end: Log::debug("sh2", "Level 2 DMA End interrupt routine finished"); break;
                case is::vector_level_1_dma_end: Log::debug("sh2", "Level 1 DMA End interrupt routine finished"); break;
                case is::vector_level_0_dma_end: Log::debug("sh2", "Level 0 DMA End interrupt routine finished"); break;
                case is::vector_dma_illegal: Log::debug("sh2", "DMA Illegal interrupt routine finished"); break;
                case is::vector_sprite_draw_end: Log::debug("sh2", "Sprite Draw End interrupt routine finished"); break;
            }

            Log::debug("sh2", "Level:{:#0x}", s.current_interrupt_.level);
        }

        s.is_interrupted_                                   = false;
        s.current_interrupt_                                = is::undefined;
        s.is_level_interrupted_[s.current_interrupt_.level] = false;
    }
}

void rts(Sh2& s) {
    // PR -> PC
    // Arranged and fixed using SH4 manual
    delaySlot(s, s.pc_ + 2);

    s.popFromCallstack();
    switch (s.emulatorContext()->debugStatus()) {
        case core::DebugStatus::step_out:
        case core::DebugStatus::wait_end_of_routine: {
            if (s.subroutineDepth() == s.callstack().size()) { s.emulatorContext()->debugStatus(core::DebugStatus::paused); }
            break;
        }
    }

    s.pc_             = s.pr_;
    s.cycles_elapsed_ = 2;
}

void sett(Sh2& s) {
    // 1 -> T
    s.sr_.set(StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void shal(Sh2& s) {
    // T <- Rn <- 0
    ((s.r_[xn00(s)] & sign_bit_32_mask) == 0) ? s.sr_.reset(StatusRegister::t) : s.sr_.set(StatusRegister::t);
    s.r_[xn00(s)] <<= 1;

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void shar(Sh2& s) {
    // MSB -> Rn -> T
    ((s.r_[xn00(s)] & 0x0000001) == 0) ? s.sr_.reset(StatusRegister::t) : s.sr_.set(StatusRegister::t);
    s32 temp{((s.r_[xn00(s)] & sign_bit_32_mask) == 0) ? 0 : 1};
    s.r_[xn00(s)] >>= 1;
    if (temp == 1) {
        s.r_[xn00(s)] |= sign_bit_32_mask;
    } else {
        s.r_[xn00(s)] &= u31_max;
    }
    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void shll(Sh2& s) {
    // T <- Rn <- 0
    ((s.r_[xn00(s)] & sign_bit_32_mask) == 0) ? s.sr_.reset(StatusRegister::t) : s.sr_.set(StatusRegister::t);
    s.r_[xn00(s)] <<= 1;

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void shll2(Sh2& s) {
    // Rn << 2 -> Rn
    s.r_[xn00(s)] <<= 2;

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void shll8(Sh2& s) {
    // Rn << 8 -> Rn
    s.r_[xn00(s)] <<= displacement_8;

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void shll16(Sh2& s) {
    // Rn << 16 -> Rn
    s.r_[xn00(s)] <<= displacement_16;

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void shlr(Sh2& s) {
    // 0 -> Rn -> T
    ((s.r_[xn00(s)] & 0x00000001) == 0) ? s.sr_.reset(StatusRegister::t) : s.sr_.set(StatusRegister::t);
    s.r_[xn00(s)] >>= 1;
    s.r_[xn00(s)] &= u31_max;

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void shlr2(Sh2& s) {
    // Rn >> 2 -> Rn
    s.r_[xn00(s)] >>= 2;
    s.r_[xn00(s)] &= u30_max;

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void shlr8(Sh2& s) {
    // Rn >> 8 -> Rn
    s.r_[xn00(s)] >>= displacement_8;
    s.r_[xn00(s)] &= bitmask_00FFFFFF;

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void shlr16(Sh2& s) {
    // Rn >> 16 -> Rn
    s.r_[xn00(s)] >>= displacement_16;
    s.r_[xn00(s)] &= bitmask_0000FFFF;

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void sleep(Sh2& s) {
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

void stcsr(Sh2& s) {
    // SR -> Rn
    s.r_[xn00(s)] = s.sr_.get(StatusRegister::all_bits);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void stcgbr(Sh2& s) {
    // GBR -> Rn
    s.r_[xn00(s)] = s.gbr_;

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void stcvbr(Sh2& s) {
    // VBR -> Rn
    s.r_[xn00(s)] = s.vbr_;

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void stcmsr(Sh2& s) {
    // Rn-4 -> Rn, SR -> (Rn)
    s.r_[xn00(s)] -= 4;
    s.memory()->write<u32>(s.r_[xn00(s)], s.sr_.get(StatusRegister::all_bits));

    s.pc_ += 2;
    s.cycles_elapsed_ = 2;
}

void stcmgbr(Sh2& s) {
    // Rn-4 -> Rn, GBR -> (Rn)
    s.r_[xn00(s)] -= 4;
    s.memory()->write<u32>(s.r_[xn00(s)], s.gbr_);

    s.pc_ += 2;
    s.cycles_elapsed_ = 2;
}

void stcmvbr(Sh2& s) {
    // Rn-4 -> Rn, VBR -> (Rn)
    s.r_[xn00(s)] -= 4;
    s.memory()->write<u32>(s.r_[xn00(s)], s.vbr_);

    s.pc_ += 2;
    s.cycles_elapsed_ = 2;
}

void stsmach(Sh2& s) {
    // MACH -> Rn
    s.r_[xn00(s)] = s.mach_;

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void stsmacl(Sh2& s) {
    // MACL -> Rn
    s.r_[xn00(s)] = s.macl_;

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void stspr(Sh2& s) {
    // PR -> Rn
    s.r_[xn00(s)] = s.pr_;

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void stsmmach(Sh2& s) {
    // Rn - :4 -> Rn, MACH -> (Rn)
    s.r_[xn00(s)] -= 4;
    s.memory()->write<u32>(s.r_[xn00(s)], s.mach_);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void stsmmacl(Sh2& s) {
    // Rn - :4 -> Rn, MACL -> (Rn)
    s.r_[xn00(s)] -= 4;
    s.memory()->write<u32>(s.r_[xn00(s)], s.macl_);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void stsmpr(Sh2& s) {
    // Rn - :4 -> Rn, PR -> (Rn)
    s.r_[xn00(s)] -= 4;
    s.memory()->write<u32>(s.r_[xn00(s)], s.pr_);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void sub(Sh2& s) {
    // Rn - Rm -> Rn
    s.r_[xn00(s)] -= s.r_[x0n0(s)];

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void subc(Sh2& s) {
    // Rn - Rm - T -> Rn, Carry -> T
    u32 tmp1{s.r_[xn00(s)] - s.r_[x0n0(s)]};
    u32 tmp0{s.r_[xn00(s)]};
    s.r_[xn00(s)] = tmp1 - s.sr_.get(StatusRegister::t);
    (tmp0 < tmp1) ? s.sr_.set(StatusRegister::t) : s.sr_.reset(StatusRegister::t);
    if (tmp1 < static_cast<u32>(s.r_[xn00(s)])) { s.sr_.set(StatusRegister::t); }
    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void subv(Sh2& s) {
    // Rn - Rm -> Rn, underflow -> T
    s32 dest{(static_cast<s32>(s.r_[xn00(s)]) >= 0) ? 0 : 1};
    s32 src{(static_cast<s32>(s.r_[x0n0(s)]) >= 0) ? 0 : 1};

    s.r_[xn00(s)] -= s.r_[x0n0(s)];
    s32 ans{(static_cast<s32>(s.r_[xn00(s)]) >= 0) ? 0 : 1};
    ans += dest;

    if (src == 1) {
        (ans == 1) ? s.sr_.set(StatusRegister::t) : s.sr_.reset(StatusRegister::t);
    } else {
        s.sr_.reset(StatusRegister::t);
    }
    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void swapb(Sh2& s) {
    // Rm -> bytes swap -> Rn
    u32 temp0{s.r_[x0n0(s)] & bitmask_FFFF0000};
    u32 temp1{(s.r_[x0n0(s)] & bitmask_000000FF) << displacement_8};
    s.r_[xn00(s)] = (s.r_[x0n0(s)] >> displacement_8) & bitmask_000000FF;
    s.r_[xn00(s)] = s.r_[xn00(s)] | temp1 | temp0;

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void swapw(Sh2& s) {
    // Rm -> words swap -> Rn
    u32 temp{(s.r_[x0n0(s)] >> displacement_16) & bitmask_0000FFFF};
    s.r_[xn00(s)] = s.r_[x0n0(s)] << displacement_16;
    s.r_[xn00(s)] |= temp;

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void tas(Sh2& s) {
    // If (Rn) = 0, 1 -> T, 1 -> MSB of (Rn)
    u32 temp{s.memory()->read<u8>(s.r_[xn00(s)])};
    (temp == 0) ? s.sr_.set(StatusRegister::t) : s.sr_.reset(StatusRegister::t);
    temp |= sign_bit_8_mask;
    s.memory()->write<u8>(s.r_[xn00(s)], static_cast<u8>(temp));

    s.pc_ += 2;
    s.cycles_elapsed_ = 4;
}

void trapa(Sh2& s) {
    // PC/SR -> stack, (imm*4 + VBR) -> PC
    u32 imm{(bitmask_000000FF & x0nn(s))};
    s.r_[sp_register_index] -= 4;
    s.memory()->write<u32>(s.r_[sp_register_index], s.sr_.get(StatusRegister::all_bits));
    s.r_[sp_register_index] -= 4;
    s.memory()->write<u32>(s.r_[sp_register_index], s.pc_ + 2);

    s.pc_             = s.memory()->read<u32>(s.vbr_ + (imm << 2));
    s.cycles_elapsed_ = 8; // NOLINT(readability-magic-numbers)
}

void tst(Sh2& s) {
    // Rn & Rm, if result = 0, 1 -> T
    ((s.r_[xn00(s)] & s.r_[x0n0(s)]) == 0) ? s.sr_.set(StatusRegister::t) : s.sr_.reset(StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void tsti(Sh2& s) {
    // R0 & imm, if result is 0, 1 -> T
    ((s.r_[0] & (bitmask_000000FF & x0nn(s))) == 0) ? s.sr_.set(StatusRegister::t) : s.sr_.reset(StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void tstm(Sh2& s) {
    // (R0 + GBR) & imm, if result is 0, 1 -> T
    u32 temp{s.memory()->read<u8>(s.gbr_ + s.r_[0])};
    temp &= (bitmask_000000FF & x0nn(s));
    (temp == 0) ? s.sr_.set(StatusRegister::t) : s.sr_.reset(StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 3;
}

void xor_op(Sh2& s) {
    // Rn^Rm -> Rn
    s.r_[xn00(s)] ^= s.r_[x0n0(s)];

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void xori(Sh2& s) {
    // R0 ^imm -> R0
    s.r_[0] ^= (bitmask_000000FF & x0nn(s));

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void xorm(Sh2& s) {
    // (R0 + GBR)^imm -> (R0 + GBR)
    u32 temp{s.memory()->read<u8>(s.gbr_ + s.r_[0])};
    temp ^= (bitmask_000000FF & x0nn(s));
    s.memory()->write<u8>(s.gbr_ + s.r_[0], static_cast<u8>(temp));

    s.pc_ += 2;
    s.cycles_elapsed_ = 3;
}

void xtrct(Sh2& s) {
    // Middle 32 bits of Rm and Rn -> Rn
    u32 temp      = (s.r_[x0n0(s)] << displacement_16) & bitmask_FFFF0000;
    s.r_[xn00(s)] = (s.r_[xn00(s)] >> displacement_16) & bitmask_0000FFFF;
    s.r_[xn00(s)] |= temp;

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void initializeOpcodesLut() {
    u32 counter{};
    while (counter < opcodes_lut_size) {
        for (u32 i = 0; i < instructions_number; ++i) {
            if ((opcodes_table[i].opcode & opcodes_table[i].mask) == (counter & opcodes_table[i].mask)) {
                //                nextInstructionLut[counter] = OpcodeInfoTable[i].goNext;
                opcodes_lut[counter]             = opcodes_table[i].execute;
                opcodes_disasm_lut[counter]      = opcodes_table[i].disasm;
                illegal_instruction_lut[counter] = opcodes_table[i].illegal_instruction_slot;
                calls_subroutine_lut[counter]    = opcodes_table[i].is_subroutine_call;
                break;
            }
            //            nextInstructionLut[counter] = false;
            opcodes_lut[counter]             = &badOpcode;
            opcodes_disasm_lut[counter]      = &badOpcode_d;
            illegal_instruction_lut[counter] = false;
            calls_subroutine_lut[counter]    = false;
        }
        ++counter;
    }
}

void execute(Sh2& s) {
    switch (s.emulatorContext()->debugStatus()) {
        case core::DebugStatus::step_over: {
            if (!calls_subroutine_lut[s.current_opcode_]) {
                //
                s.emulatorContext()->debugStatus(core::DebugStatus::paused);
            } else {
                s.emulatorContext()->debugStatus(core::DebugStatus::wait_end_of_routine);
                s.initializeSubroutineDepth();
            }
            break;
        }
        case core::DebugStatus::step_into: {
            s.emulatorContext()->debugStatus(core::DebugStatus::paused);
            break;
        }
    }

    opcodes_lut[s.current_opcode_](s);
}

auto disasm(const u32 pc, const u16 opcode) -> std::string { return opcodes_disasm_lut[opcode](pc, opcode); }

} // namespace saturnin::sh2
