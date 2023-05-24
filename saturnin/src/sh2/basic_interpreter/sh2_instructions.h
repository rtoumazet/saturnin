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
#include <saturnin/src/sh2/sh2_shared.h>
#include <saturnin/src/sh2/sh2_disasm.h>

namespace saturnin::sh2 {
// Forward declarations
class Sh2;
} // namespace saturnin::sh2

namespace saturnin::sh2::basic_interpreter {

extern void badOpcode(Sh2& s);

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

    static void delaySlot(Sh2& s, u32 addr);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	void badOpcode(Sh2& s);
    ///
    /// \brief	Function called when an invalid opcode is reached.
    ///
    /// \author	Runik
    /// \date	13/05/2023
    ///
    /// \param [in,out]	s	Sh2 processor to process.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static void badOpcode(Sh2& s);

    static void execute(Sh2& s);
};

using ExecuteType = void (*)(Sh2&); ///< Type of execute functions

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct	InstructionToFunction
///
/// \brief	Defines a link between a SH2 instruction and its corresponding function.
///
/// \author	Runik
/// \date	24/05/2023
////////////////////////////////////////////////////////////////////////////////////////////////////
struct InstructionToFunction {
    Sh2Instruction instruction;
    ExecuteType    execute; ///< Link to the corresponding function.
};

static std::array<ExecuteType, opcodes_lut_size> opcodes_lut; ///< The opcodes LUT, used for instruction fast fetching

static std::array<bool, opcodes_lut_size>
    illegal_instruction_lut; ///< The illegal instruction LUT, used for instruction fast fetching

static std::array<bool, opcodes_lut_size> calls_subroutine_lut;

// clang-format off
using InstructionsToFunctions = std::unordered_map<Sh2Instruction, ExecuteType>;

//static const auto test = InstructionsToFunctions{
//    { Sh2Instruction::nop,      &BasicInterpreter::nop,     },
//    { Sh2Instruction::add,      &BasicInterpreter::add,     }
//};


//static const auto opcodes_functions=std::array<InstructionToFunction, instructions_number>
//{ {
static const auto test = InstructionsToFunctions{
    { Sh2Instruction::nop,      &BasicInterpreter::nop,     },
    { Sh2Instruction::add,      &BasicInterpreter::add,     },
    { Sh2Instruction::addi,     &BasicInterpreter::addi,    },
    { Sh2Instruction::addc,     &BasicInterpreter::addc,    },
    { Sh2Instruction::addv,     &BasicInterpreter::addv,    },
    { Sh2Instruction::and_op,   &BasicInterpreter::and_op,  },
    { Sh2Instruction::andi,     &BasicInterpreter::andi,    },
    { Sh2Instruction::andm,     &BasicInterpreter::andm,    },
    { Sh2Instruction::bf,       &BasicInterpreter::bf,      },
    { Sh2Instruction::bfs,      &BasicInterpreter::bfs,     },
    { Sh2Instruction::bt,       &BasicInterpreter::bt,      },
    { Sh2Instruction::bts,      &BasicInterpreter::bts,     },
    { Sh2Instruction::clrmac,   &BasicInterpreter::clrmac,  },
    { Sh2Instruction::clrt,     &BasicInterpreter::clrt,    },
    { Sh2Instruction::sett,     &BasicInterpreter::sett,    },
    { Sh2Instruction::cmpeq,    &BasicInterpreter::cmpeq,   },
    { Sh2Instruction::cmpge,    &BasicInterpreter::cmpge,   },
    { Sh2Instruction::cmpgt,    &BasicInterpreter::cmpgt,   },
    { Sh2Instruction::cmphi,    &BasicInterpreter::cmphi,   },
    { Sh2Instruction::cmphs,    &BasicInterpreter::cmphs,   },
    { Sh2Instruction::cmppl,    &BasicInterpreter::cmppl,   },
    { Sh2Instruction::cmppz,    &BasicInterpreter::cmppz,   },
    { Sh2Instruction::cmpstr,   &BasicInterpreter::cmpstr,  },
    { Sh2Instruction::cmpim,    &BasicInterpreter::cmpim,   },
    { Sh2Instruction::div0s,    &BasicInterpreter::div0s,   },
    { Sh2Instruction::div0u,    &BasicInterpreter::div0u,   },
    { Sh2Instruction::div1,     &BasicInterpreter::div1,    },
    { Sh2Instruction::dmuls,    &BasicInterpreter::dmuls,   },
    { Sh2Instruction::dmulu,    &BasicInterpreter::dmulu,   },
    { Sh2Instruction::dt,       &BasicInterpreter::dt,      },
    { Sh2Instruction::extsb,    &BasicInterpreter::extsb,   },
    { Sh2Instruction::extsw,    &BasicInterpreter::extsw,   },
    { Sh2Instruction::extub,    &BasicInterpreter::extub,   },
    { Sh2Instruction::extuw,    &BasicInterpreter::extuw,   },
    { Sh2Instruction::sleep,    &BasicInterpreter::sleep,   },
    { Sh2Instruction::bra,      &BasicInterpreter::bra,     },
    { Sh2Instruction::braf,     &BasicInterpreter::braf,    },
    { Sh2Instruction::bsr,      &BasicInterpreter::bsr,     },
    { Sh2Instruction::bsrf,     &BasicInterpreter::bsrf,    },
    { Sh2Instruction::jmp,      &BasicInterpreter::jmp,     },
    { Sh2Instruction::jsr,      &BasicInterpreter::jsr,     },
    { Sh2Instruction::ldcsr,    &BasicInterpreter::ldcsr,   },
    { Sh2Instruction::ldcgbr,   &BasicInterpreter::ldcgbr,  },
    { Sh2Instruction::ldcvbr,   &BasicInterpreter::ldcvbr,  },
    { Sh2Instruction::ldcmsr,   &BasicInterpreter::ldcmsr,  },
    { Sh2Instruction::ldcmgbr,  &BasicInterpreter::ldcmgbr, },
    { Sh2Instruction::ldcmvbr,  &BasicInterpreter::ldcmvbr, },
    { Sh2Instruction::ldsmach,  &BasicInterpreter::ldsmach, },
    { Sh2Instruction::ldsmacl,  &BasicInterpreter::ldsmacl, },
    { Sh2Instruction::ldspr,    &BasicInterpreter::ldspr,   },
    { Sh2Instruction::ldsmmach, &BasicInterpreter::ldsmmach,},
    { Sh2Instruction::ldsmmacl, &BasicInterpreter::ldsmmacl,},
    { Sh2Instruction::ldsmpr,   &BasicInterpreter::ldsmpr,  },
    { Sh2Instruction::mac,      &BasicInterpreter::mac,     },
    { Sh2Instruction::macw,     &BasicInterpreter::macw,    },
    { Sh2Instruction::mov,      &BasicInterpreter::mov,     },
    { Sh2Instruction::movbs0,   &BasicInterpreter::movbs0,  },
    { Sh2Instruction::movws0,   &BasicInterpreter::movws0,  },
    { Sh2Instruction::movls0,   &BasicInterpreter::movls0,  },
    { Sh2Instruction::movbs,    &BasicInterpreter::movbs,   },
    { Sh2Instruction::movws,    &BasicInterpreter::movws,   },
    { Sh2Instruction::movls,    &BasicInterpreter::movls,   },
    { Sh2Instruction::movbl,    &BasicInterpreter::movbl,   },
    { Sh2Instruction::movwl,    &BasicInterpreter::movwl,   },
    { Sh2Instruction::movll,    &BasicInterpreter::movll,   },
    { Sh2Instruction::movbm,    &BasicInterpreter::movbm,   },
    { Sh2Instruction::movwm,    &BasicInterpreter::movwm,   },
    { Sh2Instruction::movlm,    &BasicInterpreter::movlm,   },
    { Sh2Instruction::movbp,    &BasicInterpreter::movbp,   },
    { Sh2Instruction::movwp,    &BasicInterpreter::movwp,   },
    { Sh2Instruction::movlp,    &BasicInterpreter::movlp,   },
    { Sh2Instruction::movbl0,   &BasicInterpreter::movbl0,  },
    { Sh2Instruction::movwl0,   &BasicInterpreter::movwl0,  },
    { Sh2Instruction::movll0,   &BasicInterpreter::movll0,  },
    { Sh2Instruction::movi,     &BasicInterpreter::movi,    },
    { Sh2Instruction::movwi,    &BasicInterpreter::movwi,   },
    { Sh2Instruction::movli,    &BasicInterpreter::movli,   },
    { Sh2Instruction::movwlg,   &BasicInterpreter::movblg,  },
    { Sh2Instruction::movllg,   &BasicInterpreter::movwlg,  },
    { Sh2Instruction::movbsg,   &BasicInterpreter::movllg,  },
    { Sh2Instruction::movwsg,   &BasicInterpreter::movbsg,  },
    { Sh2Instruction::movlsg,   &BasicInterpreter::movwsg,  },
    { Sh2Instruction::movbs4,   &BasicInterpreter::movlsg,  },
    { Sh2Instruction::movws4,   &BasicInterpreter::movbs4,  },
    { Sh2Instruction::movls4,   &BasicInterpreter::movws4,  },
    { Sh2Instruction::movbl4,   &BasicInterpreter::movls4,  },
    { Sh2Instruction::movwl4,   &BasicInterpreter::movbl4,  },
    { Sh2Instruction::movll4,   &BasicInterpreter::movwl4,  },
    { Sh2Instruction::movblg,   &BasicInterpreter::movll4,  },
    { Sh2Instruction::mova,     &BasicInterpreter::mova,    },
    { Sh2Instruction::movt,     &BasicInterpreter::movt,    },
    { Sh2Instruction::mull,     &BasicInterpreter::mull,    },
    { Sh2Instruction::muls,     &BasicInterpreter::muls,    },
    { Sh2Instruction::mulu,     &BasicInterpreter::mulu,    },
    { Sh2Instruction::neg,      &BasicInterpreter::neg,     },
    { Sh2Instruction::negc,     &BasicInterpreter::negc,    },
    { Sh2Instruction::not_op,   &BasicInterpreter::not_op,  },
    { Sh2Instruction::or_op,    &BasicInterpreter::or_op,   },
    { Sh2Instruction::ori,      &BasicInterpreter::ori,     },
    { Sh2Instruction::orm,      &BasicInterpreter::orm,     },
    { Sh2Instruction::rotcl,    &BasicInterpreter::rotcl,   },
    { Sh2Instruction::rotcr,    &BasicInterpreter::rotcr,   },
    { Sh2Instruction::rotl,     &BasicInterpreter::rotl,    },
    { Sh2Instruction::rotr,     &BasicInterpreter::rotr,    },
    { Sh2Instruction::rte,      &BasicInterpreter::rte,     },
    { Sh2Instruction::rts,      &BasicInterpreter::rts,     },
    { Sh2Instruction::shal,     &BasicInterpreter::shal,    },
    { Sh2Instruction::shar,     &BasicInterpreter::shar,    },
    { Sh2Instruction::shll,     &BasicInterpreter::shll,    },
    { Sh2Instruction::shll2,    &BasicInterpreter::shll2,   },
    { Sh2Instruction::shll8,    &BasicInterpreter::shll8,   },
    { Sh2Instruction::shll16,   &BasicInterpreter::shll16,  },
    { Sh2Instruction::shlr,     &BasicInterpreter::shlr,    },
    { Sh2Instruction::shlr2,    &BasicInterpreter::shlr2,   },
    { Sh2Instruction::shlr8,    &BasicInterpreter::shlr8,   },
    { Sh2Instruction::shlr16,   &BasicInterpreter::shlr16,  },
    { Sh2Instruction::stcsr,    &BasicInterpreter::stcsr,   },
    { Sh2Instruction::stcgbr,   &BasicInterpreter::stcgbr,  },
    { Sh2Instruction::stcvbr,   &BasicInterpreter::stcvbr,  },
    { Sh2Instruction::stcmsr,   &BasicInterpreter::stcmsr,  },
    { Sh2Instruction::stcmgbr,  &BasicInterpreter::stcmgbr, },
    { Sh2Instruction::stcmvbr,  &BasicInterpreter::stcmvbr, },
    { Sh2Instruction::stsmach,  &BasicInterpreter::stsmach, },
    { Sh2Instruction::stsmacl,  &BasicInterpreter::stsmacl, },
    { Sh2Instruction::stspr,    &BasicInterpreter::stspr,   },
    { Sh2Instruction::stsmmach, &BasicInterpreter::stsmmach,},
    { Sh2Instruction::stsmmacl, &BasicInterpreter::stsmmacl,},
    { Sh2Instruction::stsmpr,   &BasicInterpreter::stsmpr,  },
    { Sh2Instruction::sub,      &BasicInterpreter::sub,     },
    { Sh2Instruction::subc,     &BasicInterpreter::subc,    },
    { Sh2Instruction::subv,     &BasicInterpreter::subv,    },
    { Sh2Instruction::swapb,    &BasicInterpreter::swapb,   },
    { Sh2Instruction::swapw,    &BasicInterpreter::swapw,   },
    { Sh2Instruction::tas,      &BasicInterpreter::tas,     },
    { Sh2Instruction::trapa,    &BasicInterpreter::trapa,   },
    { Sh2Instruction::tst,      &BasicInterpreter::tst,     },
    { Sh2Instruction::tsti,     &BasicInterpreter::tsti,    },
    { Sh2Instruction::tstm,     &BasicInterpreter::tstm,    },
    { Sh2Instruction::xor_op,   &BasicInterpreter::xor_op,  },
    { Sh2Instruction::xori,     &BasicInterpreter::xori,    },
    { Sh2Instruction::xorm,     &BasicInterpreter::xorm,    },
    { Sh2Instruction::xtrct,    &BasicInterpreter::xtrct,   }
//}
 };
// clang-format on

void initializeOpcodesLut();

inline auto isInstructionIllegal(u16 inst) -> bool;

} // namespace saturnin::sh2::basic_interpreter
