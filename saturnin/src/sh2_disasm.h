//
// sh2_debug.h
// Saturnin
//
// Copyright (c) 2020 Renaud Toumazet
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
/// \file	sh2_debug.h
///
/// \brief	Declares SH2 disasm instructions
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include "emulator_defs.h"

namespace saturnin::sh2 {

/// \name SH2 debug instructions
//@{
auto badOpcode_d(u32 pc, u16 opcode) -> std::string;
auto add_d(u32 pc, u16 opcode) -> std::string;
auto addi_d(u32 pc, u16 opcode) -> std::string;
auto addc_d(u32 pc, u16 opcode) -> std::string;
auto addv_d(u32 pc, u16 opcode) -> std::string;
auto and_op_d(u32 pc, u16 opcode) -> std::string;
auto andi_d(u32 pc, u16 opcode) -> std::string;
auto andm_d(u32 pc, u16 opcode) -> std::string;
auto bf_d(u32 pc, u16 opcode) -> std::string;
auto bfs_d(u32 pc, u16 opcode) -> std::string;
auto bra_d(u32 pc, u16 opcode) -> std::string;
auto braf_d(u32 pc, u16 opcode) -> std::string;
auto bsr_d(u32 pc, u16 opcode) -> std::string;
auto bsrf_d(u32 pc, u16 opcode) -> std::string;
auto bt_d(u32 pc, u16 opcode) -> std::string;
auto bts_d(u32 pc, u16 opcode) -> std::string;
auto clrmac_d(u32 pc, u16 opcode) -> std::string;
auto clrt_d(u32 pc, u16 opcode) -> std::string;
auto cmpeq_d(u32 pc, u16 opcode) -> std::string;
auto cmpge_d(u32 pc, u16 opcode) -> std::string;
auto cmpgt_d(u32 pc, u16 opcode) -> std::string;
auto cmphi_d(u32 pc, u16 opcode) -> std::string;
auto cmphs_d(u32 pc, u16 opcode) -> std::string;
auto cmppl_d(u32 pc, u16 opcode) -> std::string;
auto cmppz_d(u32 pc, u16 opcode) -> std::string;
auto cmpstr_d(u32 pc, u16 opcode) -> std::string;
auto cmpim_d(u32 pc, u16 opcode) -> std::string;
auto div0s_d(u32 pc, u16 opcode) -> std::string;
auto div0u_d(u32 pc, u16 opcode) -> std::string;
auto div1_d(u32 pc, u16 opcode) -> std::string;
auto dmuls_d(u32 pc, u16 opcode) -> std::string;
auto dmulu_d(u32 pc, u16 opcode) -> std::string;
auto dt_d(u32 pc, u16 opcode) -> std::string;
auto extsb_d(u32 pc, u16 opcode) -> std::string;
auto extsw_d(u32 pc, u16 opcode) -> std::string;
auto extub_d(u32 pc, u16 opcode) -> std::string;
auto extuw_d(u32 pc, u16 opcode) -> std::string;
auto jmp_d(u32 pc, u16 opcode) -> std::string;
auto jsr_d(u32 pc, u16 opcode) -> std::string;
auto ldcsr_d(u32 pc, u16 opcode) -> std::string;
auto ldcgbr_d(u32 pc, u16 opcode) -> std::string;
auto ldcvbr_d(u32 pc, u16 opcode) -> std::string;
auto ldcmsr_d(u32 pc, u16 opcode) -> std::string;
auto ldcmgbr_d(u32 pc, u16 opcode) -> std::string;
auto ldcmvbr_d(u32 pc, u16 opcode) -> std::string;
auto ldsmach_d(u32 pc, u16 opcode) -> std::string;
auto ldsmacl_d(u32 pc, u16 opcode) -> std::string;
auto ldspr_d(u32 pc, u16 opcode) -> std::string;
auto ldsmmach_d(u32 pc, u16 opcode) -> std::string;
auto ldsmmacl_d(u32 pc, u16 opcode) -> std::string;
auto ldsmpr_d(u32 pc, u16 opcode) -> std::string;
auto mac_d(u32 pc, u16 opcode) -> std::string;
auto macw_d(u32 pc, u16 opcode) -> std::string;
auto mov_d(u32 pc, u16 opcode) -> std::string;
auto movbs_d(u32 pc, u16 opcode) -> std::string;
auto movws_d(u32 pc, u16 opcode) -> std::string;
auto movls_d(u32 pc, u16 opcode) -> std::string;
auto movbl_d(u32 pc, u16 opcode) -> std::string;
auto movwl_d(u32 pc, u16 opcode) -> std::string;
auto movll_d(u32 pc, u16 opcode) -> std::string;
auto movbm_d(u32 pc, u16 opcode) -> std::string;
auto movwm_d(u32 pc, u16 opcode) -> std::string;
auto movlm_d(u32 pc, u16 opcode) -> std::string;
auto movbp_d(u32 pc, u16 opcode) -> std::string;
auto movwp_d(u32 pc, u16 opcode) -> std::string;
auto movlp_d(u32 pc, u16 opcode) -> std::string;
auto movbs0_d(u32 pc, u16 opcode) -> std::string;
auto movws0_d(u32 pc, u16 opcode) -> std::string;
auto movls0_d(u32 pc, u16 opcode) -> std::string;
auto movbl0_d(u32 pc, u16 opcode) -> std::string;
auto movwl0_d(u32 pc, u16 opcode) -> std::string;
auto movll0_d(u32 pc, u16 opcode) -> std::string;
auto movi_d(u32 pc, u16 opcode) -> std::string;
auto movwi_d(u32 pc, u16 opcode) -> std::string;
auto movli_d(u32 pc, u16 opcode) -> std::string;
auto movblg_d(u32 pc, u16 opcode) -> std::string;
auto movwlg_d(u32 pc, u16 opcode) -> std::string;
auto movllg_d(u32 pc, u16 opcode) -> std::string;
auto movbsg_d(u32 pc, u16 opcode) -> std::string;
auto movwsg_d(u32 pc, u16 opcode) -> std::string;
auto movlsg_d(u32 pc, u16 opcode) -> std::string;
auto movbs4_d(u32 pc, u16 opcode) -> std::string;
auto movws4_d(u32 pc, u16 opcode) -> std::string;
auto movls4_d(u32 pc, u16 opcode) -> std::string;
auto movbl4_d(u32 pc, u16 opcode) -> std::string;
auto movwl4_d(u32 pc, u16 opcode) -> std::string;
auto movll4_d(u32 pc, u16 opcode) -> std::string;
auto mova_d(u32 pc, u16 opcode) -> std::string;
auto movt_d(u32 pc, u16 opcode) -> std::string;
auto mull_d(u32 pc, u16 opcode) -> std::string;
auto muls_d(u32 pc, u16 opcode) -> std::string;
auto mulu_d(u32 pc, u16 opcode) -> std::string;
auto neg_d(u32 pc, u16 opcode) -> std::string;
auto negc_d(u32 pc, u16 opcode) -> std::string;
auto nop_d(u32 pc, u16 opcode) -> std::string;
auto not_op_d(u32 pc, u16 opcode) -> std::string;
auto or_op_d(u32 pc, u16 opcode) -> std::string;
auto ori_d(u32 pc, u16 opcode) -> std::string;
auto orm_d(u32 pc, u16 opcode) -> std::string;
auto rotcl_d(u32 pc, u16 opcode) -> std::string;
auto rotcr_d(u32 pc, u16 opcode) -> std::string;
auto rotl_d(u32 pc, u16 opcode) -> std::string;
auto rotr_d(u32 pc, u16 opcode) -> std::string;
auto rte_d(u32 pc, u16 opcode) -> std::string;
auto rts_d(u32 pc, u16 opcode) -> std::string;
auto sett_d(u32 pc, u16 opcode) -> std::string;
auto shal_d(u32 pc, u16 opcode) -> std::string;
auto shar_d(u32 pc, u16 opcode) -> std::string;
auto shll_d(u32 pc, u16 opcode) -> std::string;
auto shll2_d(u32 pc, u16 opcode) -> std::string;
auto shll8_d(u32 pc, u16 opcode) -> std::string;
auto shll16_d(u32 pc, u16 opcode) -> std::string;
auto shlr_d(u32 pc, u16 opcode) -> std::string;
auto shlr2_d(u32 pc, u16 opcode) -> std::string;
auto shlr8_d(u32 pc, u16 opcode) -> std::string;
auto shlr16_d(u32 pc, u16 opcode) -> std::string;
auto sleep_d(u32 pc, u16 opcode) -> std::string;
auto stcsr_d(u32 pc, u16 opcode) -> std::string;
auto stcgbr_d(u32 pc, u16 opcode) -> std::string;
auto stcvbr_d(u32 pc, u16 opcode) -> std::string;
auto stcmsr_d(u32 pc, u16 opcode) -> std::string;
auto stcmgbr_d(u32 pc, u16 opcode) -> std::string;
auto stcmvbr_d(u32 pc, u16 opcode) -> std::string;
auto stsmach_d(u32 pc, u16 opcode) -> std::string;
auto stsmacl_d(u32 pc, u16 opcode) -> std::string;
auto stspr_d(u32 pc, u16 opcode) -> std::string;
auto stsmmach_d(u32 pc, u16 opcode) -> std::string;
auto stsmmacl_d(u32 pc, u16 opcode) -> std::string;
auto stsmpr_d(u32 pc, u16 opcode) -> std::string;
auto sub_d(u32 pc, u16 opcode) -> std::string;
auto subc_d(u32 pc, u16 opcode) -> std::string;
auto subv_d(u32 pc, u16 opcode) -> std::string;
auto swapb_d(u32 pc, u16 opcode) -> std::string;
auto swapw_d(u32 pc, u16 opcode) -> std::string;
auto tas_d(u32 pc, u16 opcode) -> std::string;
auto trapa_d(u32 pc, u16 opcode) -> std::string;
auto tst_d(u32 pc, u16 opcode) -> std::string;
auto tsti_d(u32 pc, u16 opcode) -> std::string;
auto tstm_d(u32 pc, u16 opcode) -> std::string;
auto xor_op_d(u32 pc, u16 opcode) -> std::string;
auto xori_d(u32 pc, u16 opcode) -> std::string;
auto xorm_d(u32 pc, u16 opcode) -> std::string;
auto xtrct_d(u32 pc, u16 opcode) -> std::string;
//@}

} // namespace saturnin::sh2
