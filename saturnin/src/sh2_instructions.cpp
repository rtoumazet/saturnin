// 
// sh2_instructions.cpp
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

#include "sh2.h"
#include "sh2_instructions.h"
#include "emulator_context.h"
#include "memory.h"

namespace saturnin {
namespace sh2 {

using core::Log;

u16 xn00(Sh2& s) { return (s.current_opcode_ & 0x0f00) >> 8; }
u16 x0n0(Sh2& s) { return (s.current_opcode_ & 0x00f0) >> 4; }
u16 x00n(Sh2& s) { return s.current_opcode_ & 0x000f; }
u16 xnnn(Sh2& s) { return s.current_opcode_ & 0x0fff; }
u16 x0nn(Sh2& s) { return s.current_opcode_ & 0x00ff; }

void delaySlot(Sh2& s, const u32 addr) {
    //Algorithm :
    // Addr read and intruction fetch
    // if the instruction is BF,BT,BRA,BSR,JMP,JSR,RTS,RTE,TRAPA,BF/S,BT/S,BRAF or BSRF then
    //		-> illegal instruction slot
    // else
    //		Slot instruction execution
    // end

    uint32_t current_inst_cycles = s.cycles_elapsed_; // We musn't forget the DS instruction count
    if (addr != 0x20000202) { // Delay slot isn't detected after the Power On Reset (to prevent the "illegal instruction slot")

        s.current_opcode_ = s.memory()->read<u16>(addr);

        if (isInstructionIllegal(s.current_opcode_)) {
            Log::error("sh2", "Illegal instruction slot");
            s.emulatorContext()->emulationStatus_ = core::EmulationStatus::stopped;
        } else {

            // Delay slot instruction execution
            execute(s);
            s.cycles_elapsed_ += current_inst_cycles;
        }
    }
}

bool isInstructionIllegal(const u16 inst) {
    // 'Illegal Slot' detection
    // Returns true if an ISI (illegal slot instruction) is detected
    // 0 otherwise
    bool returnValue = false;

    switch (xn000(inst)) {
        case 0b0000:
            switch (x000n(inst)) {
                case 0b0011:
                    switch (x00n0(inst)) {
                        case 0b0000: return true; //BSRF Rm*2
                        case 0b0010: return true; //BRAF Rm*2
                    }
                    break;
                case 0xb1011:
                    switch (x00n0(inst)) {
                        case 0b0000: return true; //RTS
                        case 0b0010: return true; //RTE
                    }
                    break;
            }
            break;
        case 0b0100:
            switch (x000n(inst)) {
                case 0b1011:
                    switch (x00n0(inst)) {
                        case 0b0000: return true; //JSR @Rm
                        case 0b0010: return true; //JMP @Rm
                    }
                    break;
            }
            break;
        case 0b1000:
            switch (x0n00(inst)) {
                case 0b1001: return true; //BT label
                case 0b1011: return true; //BF label
                case 0x1101: return true; //BT/S label*2
                case 0x1111: return true; //BF/S label*2
            }
            break;
        case 0b1010: return true; //BRA label
        case 0b1011: return true; //BSR label
        case 0b1100:
            switch (x0n00(inst)) {
                case 0x0011: return true; //TRAPA #imm
            }
            break;
    }
    return false;
}

void badOpcode(Sh2& s) {
    std::string type = (s.sh2_type_ == Sh2Type::master) ? "Master" : "Slave";
    Log::error("Unexpected opcode({} SH2). Opcode = {:#06X}. PC = {:#010X}", type, s.current_opcode_, s.pc_);

    s.emulatorContext()->emulationStatus_ = core::EmulationStatus::stopped;
}

void add(Sh2& s) {
    // Rm + Rn -> Rn
    s.r_[xn00(s)] += s.r_[x0n0(s)];

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void addi(Sh2& s) {
    // Rn + imm -> Rn 
    if ((x0nn(s) & 0x80) == 0) s.r_[xn00(s)] += (0x000000FF & static_cast<u32>(x0nn(s))); // #imm positive, 32bits sign extension
    else                       s.r_[xn00(s)] += (0xFFFFFF00 | static_cast<u32>(x0nn(s))); // #imm negative, 32bits sign extension

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void addc(Sh2& s) {
    // Rn + Rm + T -> Rn, carry -> T

    s32 tmp1 = s.r_[xn00(s)] + s.r_[x0n0(s)];
    s32 tmp0 = s.r_[xn00(s)];
    s.r_[xn00(s)] = tmp1 + s.sr_.get(StatusRegister::t);

    (tmp0 > tmp1) ? s.sr_.set(StatusRegister::t) : s.sr_.reset(StatusRegister::t);

    if (tmp1 > static_cast<s32>(s.r_[xn00(s)])) s.sr_.set(StatusRegister::t);

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
    } else s.sr_.reset(StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void and(Sh2& s) {
    // Rn & Rm -> Rn
    s.r_[xn00(s)] &= s.r_[x0n0(s)];
    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void andi(Sh2& s) {
    // R0 & imm -> R0
    s.r_[0] &= (0x000000FF & x0nn(s));
    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void andm(Sh2& s) {
    //(R0 + GBR) & imm -> (R0 + GBR)

    s32 temp = s.memory()->read<u8>(s.gbr_ + s.r_[0]);
    temp &= (0x000000FF & static_cast<s32>(x0nn(s)));
    s.memory()->write<u8>(s.gbr_ + s.r_[0], static_cast<u8>(temp));
    s.pc_ += 2;
    s.cycles_elapsed_ = 3;
}

void bf(Sh2& s) {
    // If T = 0, disp*2 + PC -> PC
    // Si T = 1, nop

    if (s.sr_.get(StatusRegister::t) == 0) {
        s32 disp{};
        if ((static_cast<s32>(x0nn(s)) & 0x80) == 0) disp = (0x000000FF & static_cast<s32>(x0nn(s)));
        else disp = (0xFFFFFF00 | static_cast<s32>(x0nn(s)));

        s.pc_ = s.pc_ + (disp << 1) + 4;
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
        s32 disp{};
        if ((x0nn(s) & 0x80) == 0) disp = (0x000000FF & static_cast<s32>(x0nn(s)));
        else disp = (0xFFFFFF00 | static_cast<s32>(x0nn(s)));

        u32 saved_pc { s.pc_ };
        delaySlot(s, s.pc_ + 2);
        s.pc_ = saved_pc + (disp << 1) + 4;
        s.cycles_elapsed_ = 2;
    } else {
        s.pc_ += 2;
        s.cycles_elapsed_ = 1;
    }
}

void bra(Sh2& s) {
    // disp*2 + PC -> PC
    // Modified using SH4 manual

    s32 disp{};
    if ((xnnn(s) & 0x800) == 0) disp = (0x00000FFF & xnnn(s));
    else disp = (0xFFFFF000 | xnnn(s));
    
    u32 saved_pc { s.pc_ };
    delaySlot(s, s.pc_ + 2);
    s.pc_ = saved_pc + (disp << 1) + 4;
    s.cycles_elapsed_ = 2;
}

void braf(Sh2& s) {
    // Rm + PC +4 -> PC
    // Modified using SH4 manual + correction
    // Registers save for the delay slot
    u32 old_pc {s.pc_};
    u32 old_r{ s.r_[xn00(s)] };

    delaySlot(s, s.pc_ + 2);
    s.pc_ = old_pc + old_r + 4;
    s.cycles_elapsed_ = 2;
}

void bsr(Sh2& s) {
    // PC -> PR, disp*2 + PC -> PC
    // Modified using SH4 manual + correction
    s32 disp{};
    if ((xnnn(s) & 0x800) == 0) disp = (0x00000FFF & xnnn(s));
    else disp = (0xFFFFF000 | xnnn(s));
    s.pr_ = s.pc_ + 4;
    u32 old_pc{ s.pc_ };
    delaySlot(s, s.pc_ + 2);
    s.pc_ = old_pc + (disp << 1) + 4;
    s.cycles_elapsed_ = 2;
}

void bsrf(Sh2& s) {
    // PC -> PR, Rm + PC -> PC
    // Modified using SH4 manual + correction
    // Registers save for the delay slot
    s.pr_ = s.pc_ + 4;

    u32 old_pc{ s.pc_ };
    u32 old_r{ s.r_[xn00(s)] };
    delaySlot(s, s.pc_ + 2);
    s.pc_ = old_pc + 4 + old_r;
    s.cycles_elapsed_ = 2;
}

void bt(Sh2& s) {
    // If T=1, disp*2 + PC -> PC;
    // If T=0=, nop

    if (s.sr_.get(StatusRegister::t) == 1) {
        s32 disp{};
        if ((x0nn(s) & 0x80) == 0) disp = (0x000000FF & static_cast<s32>(x0nn(s)));
        else disp = (0xFFFFFF00 | static_cast<s32>(x0nn(s)));

        s.pc_ = s.pc_ + (disp << 1) + 4;
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
        s32 disp{};
        if ((x0nn(s) & 0x80) == 0) disp = (0x000000FF & static_cast<s32>(x0nn(s)));
        else disp = (0xFFFFFF00 | static_cast<s32>(x0nn(s)));

        u32 old_pc{ s.pc_ };
        delaySlot(s, s.pc_ + 2);
        s.pc_ = old_pc + (disp << 1) + 4;
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
    //0 -> T
    s.sr_.reset(StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void cmpeq(Sh2& s) {
    // If Rn = Rm, T=1
    (s.r_[xn00(s)] == s.r_[x0n0(s)]) ? 
        s.sr_.set(StatusRegister::t) : s.sr_.reset(StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void cmpge(Sh2& s) {
    // If Rn >= Rm with sign, T=1
    (static_cast<s32>(s.r_[xn00(s)]) >= static_cast<s32>(s.r_[x0n0(s)])) ? 
        s.sr_.set(StatusRegister::t) : s.sr_.reset(StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void cmpgt(Sh2& s) {
    // If Rn > Rm with sign, T=1
    (static_cast<s32>(s.r_[xn00(s)]) > static_cast<s32>(s.r_[x0n0(s)])) ?
        s.sr_.set(StatusRegister::t) : s.sr_.reset(StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void cmphi(Sh2& s) {
    // If Rn > Rm without sign, T=1
    (static_cast<u32>(s.r_[xn00(s)]) > static_cast<u32>(s.r_[x0n0(s)])) ?
        s.sr_.set(StatusRegister::t) : s.sr_.reset(StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void cmphs(Sh2& s) {
    // If Rn > Rm without sign, T=1
    (static_cast<u32>(s.r_[xn00(s)]) >= static_cast<u32>(s.r_[x0n0(s)])) ?
        s.sr_.set(StatusRegister::t) : s.sr_.reset(StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void cmppl(Sh2& s) {
    //If Rn > 0, T=1
    (static_cast<s32>(s.r_[xn00(s)]) > 0) ?
        s.sr_.set(StatusRegister::t) : s.sr_.reset(StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void cmppz(Sh2& s) {
    //If Rn >= 0, T=1
    (static_cast<s32>(s.r_[xn00(s)]) >= 0) ?
        s.sr_.set(StatusRegister::t) : s.sr_.reset(StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void cmpstr(Sh2& s) {
    //If one byte of Rn = one byte of Rm then T=1

    u32 temp { s.r_[xn00(s)] ^ s.r_[x0n0(s)] };
    s32 hh { (temp >> 12) & 0x000000FF };
    s32 hl { (temp >> 8) & 0x000000FF };
    s32 lh { (temp >> 4) & 0x000000FF };
    s32 ll { temp & 0x000000FF };
    hh = hh && hl && lh && ll;
    (hh == 0) ? s.sr_.set(StatusRegister::t) : s.sr_.reset(StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void cmpim(Sh2& s) {
    //If R0 = imm, T=1
    s32 imm{};
    if ((static_cast<s32>(x0nn(s)) & 0x80) == 0) imm = (0x000000FF & static_cast<s32>(x0nn(s)));
    else imm = (0xFFFFFF00 | static_cast<s32>(x0nn(s)));
    
    (s.r_[0] == imm) ? s.sr_.set(StatusRegister::t) : s.sr_.reset(StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void div0s(Sh2& s) {
    // Rn MSB -> Q, Rm MSB -> M, M^Q -> T
    ((s.r_[xn00(s)] & 0x80000000) == 0) ? s.sr_.reset(StatusRegister::q) : s.sr_.set(StatusRegister::q);
    ((s.r_[x0n0(s)] & 0x80000000) == 0) ? s.sr_.reset(StatusRegister::m) : s.sr_.set(StatusRegister::m);
    (s.sr_.get(StatusRegister::m) == s.sr_.get(StatusRegister::q)) ? 
        s.sr_.reset(StatusRegister::t) : s.sr_.set(StatusRegister::t);
    
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
    u32 tmp0{};
    u8 tmp1{};

    u8 old_q{ s.sr_.get(StatusRegister::q) };
    ( (0x80000000 & s.r_[xn00(s)]) != 0 ) ? s.sr_.set(StatusRegister::q) : s.sr_.reset(StatusRegister::q);

    s.r_[xn00(s)] <<= 1;
    s.r_[xn00(s)] |= s.sr_.get(StatusRegister::t);
    switch (old_q) {
        case 0: switch (s.sr_.get(StatusRegister::m)) {
            case 0: 
                tmp0 = s.r_[xn00(s)];
                s.r_[xn00(s)] -= s.r_[x0n0(s)];
                tmp1 = (static_cast<u32>(s.r_[xn00(s)]) > tmp0);
                switch (s.sr_.get(StatusRegister::q)) {
                    case 0: 
                        (tmp1) ? s.sr_.set(StatusRegister::q) : s.sr_.reset(StatusRegister::q);
                        break;
                    case 1: 
                        (tmp1 == 0) ? s.sr_.set(StatusRegister::q) : s.sr_.reset(StatusRegister::q);
                        break;
                }
                break;
            case 1: 
                tmp0 = s.r_[xn00(s)];
                s.r_[xn00(s)] += s.r_[x0n0(s)];
                tmp1 = (static_cast<u32>(s.r_[xn00(s)]) < tmp0);
                switch (s.sr_.get(StatusRegister::q)) {
                    case 0: 
                        (tmp1 == 0) ? s.sr_.set(StatusRegister::q) : s.sr_.reset(StatusRegister::q);
                        break;
                    case 1: 
                        (tmp1) ? s.sr_.set(StatusRegister::q) : s.sr_.reset(StatusRegister::q);
                        break;
                }
                break;
        }
                break;
        case 1: switch (s.sr_.get(StatusRegister::m)) {
            case 0: 
                tmp0 = s.r_[xn00(s)];
                s.r_[xn00(s)] += s.r_[x0n0(s)];
                tmp1 = (static_cast<uint32_t>(s.r_[xn00(s)]) < tmp0);
                switch (s.sr_.get(StatusRegister::q)) {
                    case 0: 
                        (tmp1) ? s.sr_.set(StatusRegister::q) : s.sr_.reset(StatusRegister::q);
                        break;
                    case 1: 
                        (tmp1 == 0) ? s.sr_.set(StatusRegister::q) : s.sr_.reset(StatusRegister::q);
                        break;
                }
                break;
            case 1: tmp0 = s.r_[xn00(s)];
                s.r_[xn00(s)] -= s.r_[x0n0(s)];
                tmp1 = (static_cast<u32>(s.r_[xn00(s)]) > tmp0);
                switch (s.sr_.get(StatusRegister::q)) {
                    case 0: 
                        (tmp1 == 0) ? s.sr_.set(StatusRegister::q) : s.sr_.reset(StatusRegister::q);
                        break;
                    case 1: 
                        (tmp1) ? s.sr_.set(StatusRegister::q) : s.sr_.reset(StatusRegister::q);
                        break;
                }
                break;
        }
        break;
    }
    (s.sr_.get(StatusRegister::q) == s.sr_.get(StatusRegister::m)) ? 
        s.sr_.set(StatusRegister::t) : s.sr_.reset(StatusRegister::t);

    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void dmuls(Sh2& s) {
    // With sign, Rn * Rm -> MACH,MACL

    // Arranged using SH4 manual
    s64 result { static_cast<s64>(s.r_[x0n0(s)]) * static_cast<s64>(s.r_[xn00(s)]) };
    s.mach_ = static_cast<s32>(result >> 32);
    s.macl_ = static_cast<u32>(result & 0x00000000FFFFFFFF);

    s.pc_ += 2;
    s.cycles_elapsed_ = 2;
}

void dmulu(Sh2& s) {
    // Without sign, Rm * Rn -> MACH, MACL

    // MIGHT BE WRONG

    // Arranged using SH4 manual
    u64 result { static_cast<u64>(s.r_[x0n0(s)]) * static_cast<u64>(s.r_[xn00(s)]) };
    s.mach_ = static_cast<u32>(result >> 32);
    s.macl_ = static_cast<u32>(result & 0x00000000FFFFFFFF);

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
    if ((s.r_[x0n0(s)] & 0x00000080) == 0) s.r_[xn00(s)] &= 0x000000FF;
    else s.r_[xn00(s)] |= 0xFFFFFF00;
    
    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void extsw(Sh2& s) {
    // Rm sign extension (word) -> Rn
    s.r_[xn00(s)] = s.r_[x0n0(s)];
    if ((s.r_[x0n0(s)] & 0x00008000) == 0) s.r_[xn00(s)] &= 0x0000FFFF;
    else s.r_[xn00(s)] |= 0xFFFF0000;
    
    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void extub(Sh2& s) {
    // Rm is 0 extended (byte) -> Rn
    s.r_[xn00(s)] = s.r_[x0n0(s)];
    s.r_[xn00(s)] &= 0x000000FF;
    
    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void extuw(Sh2& s) {
    // Rm is 0 extended (word) -> Rn
    s.r_[xn00(s)] = s.r_[x0n0(s)];
    s.r_[xn00(s)] &= 0x0000FFFF;
    
    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void jmp(Sh2& s) {
    // Rm -> PC
    // Arranged and fixed using SH4 manual

    u32 old_r{ s.r_[xn00(s)] };
    delaySlot(s, s.pc_ + 2);
    
    s.pc_ = old_r;
    s.cycles_elapsed_ = 2;
}

void jsr(Sh2& s) {
    //PC -> PR, Rm -> PC
    // Arranged and fixed using SH4 manual

    u32 old_r{ s.r_[xn00(s)] };
    s.pr_ = s.pc_ + 4;
    delaySlot(s, s.pc_ + 2);
    
    s.pc_ = old_r;
    s.cycles_elapsed_ = 2;
}

//void ldcsr(Sh2& s) {
//    // Rm -> SR
//    s.sr_ = s.r_[xn00(s)] & 0x000003F3;
//    
//    s.pc_ += 2;
//    s.cycles_elapsed_ = 1;
//}

void nop(Sh2& s) {
    // Mo operation
    s.pc_ += 2;
    s.cycles_elapsed_ = 1;
}

void initializeOpcodesLut() {
    u32 counter{};
    while (counter < 0x10000) {
        for (u32 i = 0; i < instructions_number; ++i) {
            if ((opcodes_table[i].opcode & opcodes_table[i].mask) == (counter & opcodes_table[i].mask)) {
//                nextInstructionLut[counter] = OpcodeInfoTable[i].goNext;
                opcodes_lut[counter] = opcodes_table[i].execute;
                break;
            }
//            nextInstructionLut[counter] = false;
            opcodes_lut[counter] = &badOpcode;
        }
        ++counter;
    }
}

void execute(Sh2& s) {
    opcodes_lut[s.current_opcode_](s);
}

}
}