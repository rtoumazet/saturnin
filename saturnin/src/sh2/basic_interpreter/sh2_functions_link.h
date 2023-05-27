//
// sh2_functions_link.h
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
/// \file	sh2_functions_link.h
///
/// \brief	Declares the link between a SH2 instruction and its related function.
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <unordered_map>
#include <saturnin/src/emulator_defs.h>
#include <saturnin/src/sh2/sh2_shared.h>

namespace saturnin::sh2::basic_interpreter {

using ExecuteType             = void (*)(Sh2&); ///< Type of execute functions
using InstructionsToFunctions = std::unordered_map<Sh2Instruction, ExecuteType>;

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

// clang-format off

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \var inst_to_func
/// 	   
/// \brief (Immutable) Link between a SH2 instruction and the related function.
/// 	   
////////////////////////////////////////////////////////////////////////////////////////////////
static const auto inst_to_func = InstructionsToFunctions{   
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
    { Sh2Instruction::movwlg,   &BasicInterpreter::movwlg,  },
    { Sh2Instruction::movllg,   &BasicInterpreter::movllg,  },
    { Sh2Instruction::movbsg,   &BasicInterpreter::movbsg,  },
    { Sh2Instruction::movwsg,   &BasicInterpreter::movwsg,  },
    { Sh2Instruction::movlsg,   &BasicInterpreter::movlsg,  },
    { Sh2Instruction::movbs4,   &BasicInterpreter::movbs4,  },
    { Sh2Instruction::movws4,   &BasicInterpreter::movws4,  },
    { Sh2Instruction::movls4,   &BasicInterpreter::movls4,  },
    { Sh2Instruction::movbl4,   &BasicInterpreter::movbl4,  },
    { Sh2Instruction::movwl4,   &BasicInterpreter::movwl4,  },
    { Sh2Instruction::movll4,   &BasicInterpreter::movll4,  },
    { Sh2Instruction::movblg,   &BasicInterpreter::movblg,  },
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
    { Sh2Instruction::xtrct,    &BasicInterpreter::xtrct   }
 };
// clang-format on

static std::array<ExecuteType, opcodes_lut_size> opcodes_lut; ///< The opcodes LUT, used for instruction fast fetching

} // namespace saturnin::sh2::basic_interpreter
