// 
// sh2_opcodes.h
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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file	sh2_opcodes.h
///
/// \brief	Opcodes information table 
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include<array>
#include "emulator_defs.h"
#include "sh2_intructions.h"

namespace saturnin {
namespace core {

// Forward declarations
class Sh2;

constexpr u8 instructions_number = 142; ///< Total number of SH2 instructions used.

using ExecuteType = void(*)(Sh2&); ///< Type of the execute

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct	Sh2Instruction
///
/// \brief	Defines a SH2 instruction.
///
/// \author	Runik
/// \date	23/09/2019
////////////////////////////////////////////////////////////////////////////////////////////////////
struct Sh2Instruction {
    u16         mask;   ///< Instruction mask.
    u16         opcode; ///< Instruction opcode.
    bool        go_next;///< True if this instruction isn't a jump or doesn't modify system registers.
    ExecuteType execute;///< Link to the corresponding function.
};


static std::array<Sh2Instruction, instructions_number> const opcode_table
{{
    { 0xFFFF, 0x0009, true,  &nop},
    { 0xF00F, 0x300C, true,  &add},
    { 0xF000, 0x7000, true,  &addi},
    { 0xF00F, 0x300E, true,  &addc},
    { 0xF00F, 0x300F, true,  &addv},
    { 0xF00F, 0x2009, true,  &and},
    { 0xFF00, 0xC900, true,  &andi}
    //{ 0xFF00, 0xCD00, false, &andm}, // R0GBR
    //{ 0xFF00, 0x8B00, false, &bf},
    //{ 0xFF00, 0x8F00, false, &bfs},
    //{ 0xFF00, 0x8900, false, &bt},
    //{ 0xFF00, 0x8D00, false, &bts},
    //{ 0xFFFF, 0x0028, true,  &clrmac},
    //{ 0xFFFF, 0x0008, true,  &clrt},
    //{ 0xFFFF, 0x0018, true,  &sett},
    //{ 0xF00F, 0x3000, true,  &cmpeq},
    //{ 0xF00F, 0x3003, true,  &cmpge},
    //{ 0xF00F, 0x3007, true,  &cmpgt},
    //{ 0xF00F, 0x3006, true,  &cmphi},
    //{ 0xF00F, 0x3002, true,  &cmphs},
    //{ 0xF0FF, 0x4015, true,  &cmppl},
    //{ 0xF0FF, 0x4011, true,  &cmppz},
    //{ 0xF00F, 0x200C, true,  &cmpstr},
    //{ 0xFF00, 0x8800, true,  &cmpim},
    //{ 0xF00F, 0x2007, true,  &div0s},
    //{ 0xFFFF, 0x0019, true,  &div0u},
    //{ 0xF00F, 0x3004, true,  &div1},
    //{ 0xF00F, 0x300D, true,  &dmuls},
    //{ 0xF00F, 0x3005, true,  &dmulu},
    //{ 0xF0FF, 0x4010, true,  &dt},
    //{ 0xF00F, 0x600E, true,  &extsb},
    //{ 0xF00F, 0x600F, true,  &extsw},
    //{ 0xF00F, 0x600C, true,  &extub},
    //{ 0xF00F, 0x600D, true,  &extuw},
    //{ 0xFFFF, 0x001B, true,  &sleep},
    //{ 0xF000, 0xA000, false, &bra},
    //{ 0xF0FF, 0x0023, false, &braf},
    //{ 0xF000, 0xB000, false, &bsr},
    //{ 0xF0FF, 0x0003, false, &bsrf},
    //{ 0xF0FF, 0x402B, false, &jmp},
    //{ 0xF0FF, 0x400B, false, &jsr},
    //{ 0xF0FF, 0x400E, true,  &ldcsr},
    //{ 0xF0FF, 0x401E, true,  &ldcgbr},
    //{ 0xF0FF, 0x402E, true,  &ldcvbr},
    //{ 0xF0FF, 0x4007, true,  &ldcmsr},
    //{ 0xF0FF, 0x4017, true,  &ldcmgbr},
    //{ 0xF0FF, 0x4027, true,  &ldcmvbr},
    //{ 0xF0FF, 0x400A, true,  &ldsmach},
    //{ 0xF0FF, 0x401A, true,  &ldsmacl},
    //{ 0xF0FF, 0x402A, true,  &ldspr},
    //{ 0xF0FF, 0x4006, true,  &ldsmmach},
    //{ 0xF0FF, 0x4016, true,  &ldsmmacl},
    //{ 0xF0FF, 0x4026, true,  &ldsmpr},
    //{ 0xF00F, 0x000F, true,  &mac},
    //{ 0xF00F, 0x400F, true,  &macw},
    //{ 0xF00F, 0x6003, true,  &mov},
    //{ 0xF00F, 0x0004, false, &movbs0}, // R0R
    //{ 0xF00F, 0x0005, false, &movws0}, // R0R
    //{ 0xF00F, 0x0006, false, &movls0}, // R0R
    //{ 0xF00F, 0x2000, false, &movbs}, // CheckRegValue
    //{ 0xF00F, 0x2001, false, &movws}, // CheckRegValue
    //{ 0xF00F, 0x2002, false, &movls}, // CheckRegValue
    //{ 0xF00F, 0x6000, true,  &movbl},
    //{ 0xF00F, 0x6001, true,  &movwl},
    //{ 0xF00F, 0x6002, true,  &movll},
    //{ 0xF00F, 0x2004, false, &movbm}, // CheckRegValueRM1
    //{ 0xF00F, 0x2005, false, &movwm}, // CheckRegValueRM2
    //{ 0xF00F, 0x2006, false, &movlm}, // CheckRegValueRM4
    //{ 0xF00F, 0x6004, true,  &movbp},
    //{ 0xF00F, 0x6005, true,  &movwp},
    //{ 0xF00F, 0x6006, true,  &movlp},
    //{ 0xF00F, 0x000C, true,  &movbl0},
    //{ 0xF00F, 0x000D, true,  &movwl0},
    //{ 0xF00F, 0x000E, true,  &movll0},
    //{ 0xF000, 0xE000, true,  &movi},
    //{ 0xF000, 0x9000, true,  &movwi},
    //{ 0xF000, 0xD000, true,  &movli},
    //{ 0xFF00, 0xC400, true,  &movblg},
    //{ 0xFF00, 0xC500, true,  &movwlg},
    //{ 0xFF00, 0xC600, true,  &movllg},
    //{ 0xFF00, 0xC000, false, &movbsg}, // DispGBR
    //{ 0xFF00, 0xC100, false, &movwsg}, // DispGBR
    //{ 0xFF00, 0xC200, false, &movlsg}, // DispGBR
    //{ 0xFF00, 0x8000, false, &movbs4}, // DispR
    //{ 0xFF00, 0x8100, false, &movws4}, // DispR
    //{ 0xF000, 0x1000, false, &movls4}, // DispR
    //{ 0xFF00, 0x8400, true,  &movbl4},
    //{ 0xFF00, 0x8500, true,  &movwl4},
    //{ 0xF000, 0x5000, true,  &movll4},
    //{ 0xFF00, 0xC700, true,  &mova},
    //{ 0xF0FF, 0x0029, true,  &movt},
    //{ 0xF00F, 0x0007, true,  &mull},
    //{ 0xF00F, 0x200F, true,  &muls},
    //{ 0xF00F, 0x200E, true,  &mulu},
    //{ 0xF00F, 0x600B, true,  &neg},
    //{ 0xF00F, 0x600A, true,  &negc},
    //{ 0xF00F, 0x6007, true,  &not},
    //{ 0xF00F, 0x200B, true,  &or},
    //{ 0xFF00, 0xCB00, true,  &ori},
    //{ 0xFF00, 0xCF00, false, &orm}, // R0GBR
    //{ 0xF0FF, 0x4024, true,  &rotcl},
    //{ 0xF0FF, 0x4025, true,  &rotcr},
    //{ 0xF0FF, 0x4004, true,  &rotl},
    //{ 0xF0FF, 0x4005, true,  &rotr},
    //{ 0xFFFF, 0x002B, false, &rte},
    //{ 0xFFFF, 0x000B, false, &rts},
    //{ 0xF0FF, 0x4020, true,  &shal},
    //{ 0xF0FF, 0x4021, true,  &shar},
    //{ 0xF0FF, 0x4000, true,  &shll},
    //{ 0xF0FF, 0x4008, true,  &shll2},
    //{ 0xF0FF, 0x4018, true,  &shll8},
    //{ 0xF0FF, 0x4028, true,  &shll16},
    //{ 0xF0FF, 0x4001, true,  &shlr},
    //{ 0xF0FF, 0x4009, true,  &shlr2},
    //{ 0xF0FF, 0x4019, true,  &shlr8},
    //{ 0xF0FF, 0x4029, true,  &shlr16},
    //{ 0xF0FF, 0x0002, true,  &stcsr},
    //{ 0xF0FF, 0x0012, true,  &stcgbr},
    //{ 0xF0FF, 0x0022, true,  &stcvbr},
    //{ 0xF0FF, 0x4003, false, &stcmsr}, // CheckRegValueRM4
    //{ 0xF0FF, 0x4013, false, &stcmgbr}, // CheckRegValueRM4
    //{ 0xF0FF, 0x4023, false, &stcmvbr}, // CheckRegValueRM4
    //{ 0xF0FF, 0x000A, true,  &stsmach},
    //{ 0xF0FF, 0x001A, true,  &stsmacl},
    //{ 0xF0FF, 0x002A, true,  &stspr},
    //{ 0xF0FF, 0x4002, false, &stsmmach}, // CheckRegValueRM4
    //{ 0xF0FF, 0x4012, false, &stsmmacl}, // CheckRegValueRM4
    //{ 0xF0FF, 0x4022, false, &stsmpr}, // CheckRegValueRM4
    //{ 0xF00F, 0x3008, true,  &sub},
    //{ 0xF00F, 0x300A, true,  &subc},
    //{ 0xF00F, 0x300B, true,  &subv},
    //{ 0xF00F, 0x6008, true,  &swapb},
    //{ 0xF00F, 0x6009, true,  &swapw},
    //{ 0xF0FF, 0x401B, false, &tas}, // CheckRegValue
    //{ 0xFF00, 0xC300, true,  &trapa},
    //{ 0xF00F, 0x2008, true,  &tst},
    //{ 0xFF00, 0xC800, true,  &tsti},
    //{ 0xFF00, 0xCC00, true,  &tstm},
    //{ 0xF00F, 0x200A, true,  &xor},
    //{ 0xFF00, 0xCA00, true,  &xori},
    //{ 0xFF00, 0xCE00, false, &orm}, // R0GBR
    //{ 0xF00F, 0x200D, true,  &xtrct}
}};

};
};