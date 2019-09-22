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

#include "sh2_intructions.h"
#include "sh2.h"
//#include "emulator_context.h"

namespace saturnin {
namespace core {

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

        //if (sh2->IllegalSlotInstructionDetected(sh2->currentOpcode)) MessageBox(NULL, "Illegal instruction slot", "Error", NULL);
        //else {
        //    if (sh2->GetExecuteInstruction()) {
        //        sh2->SetDelaySlotInstruction(true);
        //        while (sh2->GetDelaySlotInstruction()) {
        //            Sleep(1); // Waiting until the delay slot is done
        //        }
        //        SendMessage(sh2->GetWindowHandle(), WM_UPDATE, Addr, 0);
        //    }

        //    // Delay slot instruction execution
        //    sh2->Execute(sh2->currentOpcode);

        //    s.cycles_elapsed_ += current_inst_cycles;
        //}
    }
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



}
}