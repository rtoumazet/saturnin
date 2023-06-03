//
// sh2_disasm_link.h
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
/// \file	sh2_disasm_link.h
///
/// \brief	Declares the link between a SH2 instruction and its related disassembly function.
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <unordered_map>
#include <saturnin/src/emulator_defs.h>
#include <saturnin/src/sh2/sh2_shared.h>
#include <saturnin/src/sh2/sh2_disasm.h>

namespace saturnin::sh2 {

using DisasmType               = auto (*)(u32 pc, u16 opcode) -> std::string; ///< Type of disassembly functions
using InstructionsToFuncDisasm = std::unordered_map<Sh2Instruction, DisasmType>;

// clang-format off

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \var inst_to_func
/// 	   
/// \brief (Immutable) Link between a SH2 instruction and the related function.
/// 	   
////////////////////////////////////////////////////////////////////////////////////////////////
static const auto inst_to_disasm = InstructionsToFuncDisasm{   
    { Sh2Instruction::nop,      &nop_d,     },
    { Sh2Instruction::add,      &add_d,     },
    { Sh2Instruction::addi,     &addi_d,    },
    { Sh2Instruction::addc,     &addc_d,    },
    { Sh2Instruction::addv,     &addv_d,    },
    { Sh2Instruction::and_op,   &and_op_d,  },
    { Sh2Instruction::andi,     &andi_d,    },
    { Sh2Instruction::andm,     &andm_d,    },
    { Sh2Instruction::bf,       &bf_d,      },
    { Sh2Instruction::bfs,      &bfs_d,     },
    { Sh2Instruction::bt,       &bt_d,      },
    { Sh2Instruction::bts,      &bts_d,     },
    { Sh2Instruction::clrmac,   &clrmac_d,  },
    { Sh2Instruction::clrt,     &clrt_d,    },
    { Sh2Instruction::sett,     &sett_d,    },
    { Sh2Instruction::cmpeq,    &cmpeq_d,   },
    { Sh2Instruction::cmpge,    &cmpge_d,   },
    { Sh2Instruction::cmpgt,    &cmpgt_d,   },
    { Sh2Instruction::cmphi,    &cmphi_d,   },
    { Sh2Instruction::cmphs,    &cmphs_d,   },
    { Sh2Instruction::cmppl,    &cmppl_d,   },
    { Sh2Instruction::cmppz,    &cmppz_d,   },
    { Sh2Instruction::cmpstr,   &cmpstr_d,  },
    { Sh2Instruction::cmpim,    &cmpim_d,   },
    { Sh2Instruction::div0s,    &div0s_d,   },
    { Sh2Instruction::div0u,    &div0u_d,   },
    { Sh2Instruction::div1,     &div1_d,    },
    { Sh2Instruction::dmuls,    &dmuls_d,   },
    { Sh2Instruction::dmulu,    &dmulu_d,   },
    { Sh2Instruction::dt,       &dt_d,      },
    { Sh2Instruction::extsb,    &extsb_d,   },
    { Sh2Instruction::extsw,    &extsw_d,   },
    { Sh2Instruction::extub,    &extub_d,   },
    { Sh2Instruction::extuw,    &extuw_d,   },
    { Sh2Instruction::sleep,    &sleep_d,   },
    { Sh2Instruction::bra,      &bra_d,     },
    { Sh2Instruction::braf,     &braf_d,    },
    { Sh2Instruction::bsr,      &bsr_d,     },
    { Sh2Instruction::bsrf,     &bsrf_d,    },
    { Sh2Instruction::jmp,      &jmp_d,     },
    { Sh2Instruction::jsr,      &jsr_d,     },
    { Sh2Instruction::ldcsr,    &ldcsr_d,   },
    { Sh2Instruction::ldcgbr,   &ldcgbr_d,  },
    { Sh2Instruction::ldcvbr,   &ldcvbr_d,  },
    { Sh2Instruction::ldcmsr,   &ldcmsr_d,  },
    { Sh2Instruction::ldcmgbr,  &ldcmgbr_d, },
    { Sh2Instruction::ldcmvbr,  &ldcmvbr_d, },
    { Sh2Instruction::ldsmach,  &ldsmach_d, },
    { Sh2Instruction::ldsmacl,  &ldsmacl_d, },
    { Sh2Instruction::ldspr,    &ldspr_d,   },
    { Sh2Instruction::ldsmmach, &ldsmmach_d,},
    { Sh2Instruction::ldsmmacl, &ldsmmacl_d,},
    { Sh2Instruction::ldsmpr,   &ldsmpr_d,  },
    { Sh2Instruction::mac,      &mac_d,     },
    { Sh2Instruction::macw,     &macw_d,    },
    { Sh2Instruction::mov,      &mov_d,     },
    { Sh2Instruction::movbs0,   &movbs0_d,  },
    { Sh2Instruction::movws0,   &movws0_d,  },
    { Sh2Instruction::movls0,   &movls0_d,  },
    { Sh2Instruction::movbs,    &movbs_d,   },
    { Sh2Instruction::movws,    &movws_d,   },
    { Sh2Instruction::movls,    &movls_d,   },
    { Sh2Instruction::movbl,    &movbl_d,   },
    { Sh2Instruction::movwl,    &movwl_d,   },
    { Sh2Instruction::movll,    &movll_d,   },
    { Sh2Instruction::movbm,    &movbm_d,   },
    { Sh2Instruction::movwm,    &movwm_d,   },
    { Sh2Instruction::movlm,    &movlm_d,   },
    { Sh2Instruction::movbp,    &movbp_d,   },
    { Sh2Instruction::movwp,    &movwp_d,   },
    { Sh2Instruction::movlp,    &movlp_d,   },
    { Sh2Instruction::movbl0,   &movbl0_d,  },
    { Sh2Instruction::movwl0,   &movwl0_d,  },
    { Sh2Instruction::movll0,   &movll0_d,  },
    { Sh2Instruction::movi,     &movi_d,    },
    { Sh2Instruction::movwi,    &movwi_d,   },
    { Sh2Instruction::movli,    &movli_d,   },
    { Sh2Instruction::movwlg,   &movwlg_d,  },
    { Sh2Instruction::movllg,   &movllg_d,  },
    { Sh2Instruction::movbsg,   &movbsg_d,  },
    { Sh2Instruction::movwsg,   &movwsg_d,  },
    { Sh2Instruction::movlsg,   &movlsg_d,  },
    { Sh2Instruction::movbs4,   &movbs4_d,  },
    { Sh2Instruction::movws4,   &movws4_d,  },
    { Sh2Instruction::movls4,   &movls4_d,  },
    { Sh2Instruction::movbl4,   &movbl4_d,  },
    { Sh2Instruction::movwl4,   &movwl4_d,  },
    { Sh2Instruction::movll4,   &movll4_d,  },
    { Sh2Instruction::movblg,   &movblg_d,  },
    { Sh2Instruction::mova,     &mova_d,    },
    { Sh2Instruction::movt,     &movt_d,    },
    { Sh2Instruction::mull,     &mull_d,    },
    { Sh2Instruction::muls,     &muls_d,    },
    { Sh2Instruction::mulu,     &mulu_d,    },
    { Sh2Instruction::neg,      &neg_d,     },
    { Sh2Instruction::negc,     &negc_d,    },
    { Sh2Instruction::not_op,   &not_op_d,  },
    { Sh2Instruction::or_op,    &or_op_d,   },
    { Sh2Instruction::ori,      &ori_d,     },
    { Sh2Instruction::orm,      &orm_d,     },
    { Sh2Instruction::rotcl,    &rotcl_d,   },
    { Sh2Instruction::rotcr,    &rotcr_d,   },
    { Sh2Instruction::rotl,     &rotl_d,    },
    { Sh2Instruction::rotr,     &rotr_d,    },
    { Sh2Instruction::rte,      &rte_d,     },
    { Sh2Instruction::rts,      &rts_d,     },
    { Sh2Instruction::shal,     &shal_d,    },
    { Sh2Instruction::shar,     &shar_d,    },
    { Sh2Instruction::shll,     &shll_d,    },
    { Sh2Instruction::shll2,    &shll2_d,   },
    { Sh2Instruction::shll8,    &shll8_d,   },
    { Sh2Instruction::shll16,   &shll16_d,  },
    { Sh2Instruction::shlr,     &shlr_d,    },
    { Sh2Instruction::shlr2,    &shlr2_d,   },
    { Sh2Instruction::shlr8,    &shlr8_d,   },
    { Sh2Instruction::shlr16,   &shlr16_d,  },
    { Sh2Instruction::stcsr,    &stcsr_d,   },
    { Sh2Instruction::stcgbr,   &stcgbr_d,  },
    { Sh2Instruction::stcvbr,   &stcvbr_d,  },
    { Sh2Instruction::stcmsr,   &stcmsr_d,  },
    { Sh2Instruction::stcmgbr,  &stcmgbr_d, },
    { Sh2Instruction::stcmvbr,  &stcmvbr_d, },
    { Sh2Instruction::stsmach,  &stsmach_d, },
    { Sh2Instruction::stsmacl,  &stsmacl_d, },
    { Sh2Instruction::stspr,    &stspr_d,   },
    { Sh2Instruction::stsmmach, &stsmmach_d,},
    { Sh2Instruction::stsmmacl, &stsmmacl_d,},
    { Sh2Instruction::stsmpr,   &stsmpr_d,  },
    { Sh2Instruction::sub,      &sub_d,     },
    { Sh2Instruction::subc,     &subc_d,    },
    { Sh2Instruction::subv,     &subv_d,    },
    { Sh2Instruction::swapb,    &swapb_d,   },
    { Sh2Instruction::swapw,    &swapw_d,   },
    { Sh2Instruction::tas,      &tas_d,     },
    { Sh2Instruction::trapa,    &trapa_d,   },
    { Sh2Instruction::tst,      &tst_d,     },
    { Sh2Instruction::tsti,     &tsti_d,    },
    { Sh2Instruction::tstm,     &tstm_d,    },
    { Sh2Instruction::xor_op,   &xor_op_d,  },
    { Sh2Instruction::xori,     &xori_d,    },
    { Sh2Instruction::xorm,     &xorm_d,    },
    { Sh2Instruction::xtrct,    &xtrct_d   }
 };
// clang-format on

} // namespace saturnin::sh2
