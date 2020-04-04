//
// sh2_debug.cpp
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

#include <fmt/format.h>
#include "sh2_debug.h"
#include "sh2_instructions.h"

namespace saturnin::sh2 {

auto add_d(const u16 opcode) -> std::string {
    return fmt::format("{:#06x} ADD R{d},R{d}", opcode, x00n0(opcode), x0n00(opcode));
};
auto addi_d(const u16 opcode) -> std::string {
    u32 imm{x00nn(opcode)};
    imm = ((imm & 0x80) == 0) ? imm & 0x000000FF : imm | 0xFFFFFF00;
    return fmt::format("{:#06x} ADD #{:#010x},R{d}", opcode, imm, x0n00(opcode));
};
auto addc_d(const u16 opcode) -> std::string {
    return fmt::format("{:#06x} ADDC R{d},R{d}", opcode, x00n0(opcode), x0n00(opcode));
};
auto addv_d(const u16 opcode) -> std::string {
    return fmt::format("{:#06x} ADDV R{d},R{d}", opcode, x00n0(opcode), x0n00(opcode));
};
auto and_op_d(const u16 opcode) -> std::string {
    return fmt::format("{:#06x} AND R{d},R{d}", opcode, x00n0(opcode), x0n00(opcode));
};
auto andi_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} AND #{:#x},R0", opcode, x000n(opcode)); };
auto andm_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} AND.B #{:#x},@(R0,GBR)", opcode, x000n(opcode)); };
auto bf_d(const u16 opcode) -> std::string {
    u32 imm{x00nn(opcode)};
    imm = ((imm & 0x80) == 0) ? imm & 0x000000FF : imm | 0xFFFFFF00;
    return fmt::format("{:#06x} BF PC+{:#x}", opcode, imm * 2 + 4);
};
auto bfs_d(const u16 opcode) -> std::string {
    u32 imm{x00nn(opcode)};
    imm = ((imm & 0x80) == 0) ? imm & 0x000000FF : imm | 0xFFFFFF00;
    return fmt::format("{:#06x} BF/S PC+{:#x}", opcode, imm * 2 + 4);
};
auto bra_d(const u16 opcode) -> std::string {
    u32 imm{x0nnn(opcode)};
    imm = ((imm & 0x800) == 0) ? imm & 0x00000FFF : imm | 0xFFFFF000;
    return fmt::format("{:#06x} BRA PC+{:#x}", opcode, imm * 2 + 4);
};
auto braf_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} BRAF R{d}", opcode, x0n00(opcode)); };
auto bsr_d(const u16 opcode) -> std::string {
    u32 imm{x0nnn(opcode)};
    imm = ((imm & 0x800) == 0) ? imm & 0x00000FFF : imm | 0xFFFFF000;
    return fmt::format("{:#06x} BSR PC+{:#x}", opcode, imm * 2 + 4);
};
auto bsrf_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} BSRF R{d}", opcode, x0n00(opcode)); };
auto bt_d(const u16 opcode) -> std::string {
    u32 imm{x00nn(opcode)};
    imm = ((imm & 0x80) == 0) ? imm & 0x000000FF : imm | 0xFFFFFF00;
    return fmt::format("{:#06x} BT PC+{:#x}", opcode, imm * 2 + 4);
};
auto bts_d(const u16 opcode) -> std::string {
    u32 imm{x00nn(opcode)};
    imm = ((imm & 0x80) == 0) ? imm & 0x000000FF : imm | 0xFFFFFF00;
    return fmt::format("{:#06x} BT/S PC+{:#x}", opcode, imm * 2 + 4);
};
auto clrmac_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} CLRMAC", opcode); };
auto clrt_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} CLRT", opcode); };
auto cmpeq_d(const u16 opcode) -> std::string {
    return fmt::format("{:#06x} CMP/EQ R{d},R{d}", opcode, x00n0(opcode), x0n00(opcode));
};
auto cmpge_d(const u16 opcode) -> std::string {
    return fmt::format("{:#06x} CMP/GE R{d},R{d}", opcode, x00n0(opcode), x0n00(opcode));
};
auto cmpgt_d(const u16 opcode) -> std::string {
    return fmt::format("{:#06x} CMP/GT R{d},R{d}", opcode, x00n0(opcode), x0n00(opcode));
};
auto cmphi_d(const u16 opcode) -> std::string {
    return fmt::format("{:#06x} CMP/HI R{d},R{d}", opcode, x00n0(opcode), x0n00(opcode));
};
auto cmphs_d(const u16 opcode) -> std::string {
    return fmt::format("{:#06x} CMP/HS R{d},R{d}", opcode, x00n0(opcode), x0n00(opcode));
};
auto cmppl_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} CMP/PL R{d}", opcode, x0n00(opcode)); };
auto cmppz_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} CMP/PZ R{d}", opcode, x0n00(opcode)); };
auto cmpstr_d(const u16 opcode) -> std::string {
    return fmt::format("{:#06x} CMP/STR R{d},R{d}", opcode, x00n0(opcode), x0n00(opcode));
};
auto cmpim_d(const u16 opcode) -> std::string {
    u32 imm{x00nn(opcode)};
    imm = ((imm & 0x80) == 0) ? imm & 0x000000FF : imm | 0xFFFFFF00;
    return fmt::format("{:#06x} CMP/EQ #{:#x},R0", opcode, imm);
};
auto div0s_d(const u16 opcode) -> std::string {
    return fmt::format("{:#06x} DIV0S R{d},R{d}", opcode, x00n0(opcode), x0n00(opcode));
};
auto div0u_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} DIV0U", opcode); };
auto div1_d(const u16 opcode) -> std::string {
    return fmt::format("{:#06x} DIV1 R{d},R{d}", opcode, x00n0(opcode), x0n00(opcode));
};
auto dmuls_d(const u16 opcode) -> std::string {
    return fmt::format("{:#06x} DMULS.L R{d},R{d}", opcode, x00n0(opcode), x0n00(opcode));
};
auto dmulu_d(const u16 opcode) -> std::string {
    return fmt::format("{:#06x} DMULU.L R{d},R{d}", opcode, x00n0(opcode), x0n00(opcode));
};
auto dt_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} DT R{d}", opcode, x0n00(opcode)); };
auto extsb_d(const u16 opcode) -> std::string {
    return fmt::format("{:#06x} EXTS.B R{d},R{d}", opcode, x00n0(opcode), x0n00(opcode));
};
auto extsw_d(const u16 opcode) -> std::string {
    return fmt::format("{:#06x} EXTS.W R{d},R{d}", opcode, x00n0(opcode), x0n00(opcode));
};
auto extub_d(const u16 opcode) -> std::string {
    return fmt::format("{:#06x} EXTU.B R{d},R{d}", opcode, x00n0(opcode), x0n00(opcode));
};
auto extuw_d(const u16 opcode) -> std::string {
    return fmt::format("{:#06x} EXTU.W R{d},R{d}", opcode, x00n0(opcode), x0n00(opcode));
};
auto jmp_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} JMP @R{d}", opcode, x0n00(opcode)); };
auto jsr_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} JSR @R{d}", opcode, x0n00(opcode)); };
auto ldcsr_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} LDC R{d},SR", opcode, x0n00(opcode)); };
auto ldcgbr_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} LDC R{d},GBR", opcode, x0n00(opcode)); };
auto ldcvbr_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} LDC R{d},VBR", opcode, x0n00(opcode)); };
auto ldcmsr_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} LDS.L @R{d}+,SR", opcode, x0n00(opcode)); };
auto ldcmgbr_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} LDS.L @R{d}+,GBR", opcode, x0n00(opcode)); };
auto ldcmvbr_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} LDS.L @R{d}+,VBR", opcode, x0n00(opcode)); };
auto ldsmach_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} LDS R{d},MACH", opcode, x0n00(opcode)); };
auto ldsmacl_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} LDS R{d},MACL", opcode, x0n00(opcode)); };
auto ldspr_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} LDS R{d},PR", opcode, x0n00(opcode)); };
auto ldsmmach_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} LDS.L @R{d}+,MACH", opcode, x0n00(opcode)); };
auto ldsmmacl_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} LDS.L @R{d}+,MACL", opcode, x0n00(opcode)); };
auto ldsmpr_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} LDS.L @R{d}+,PR", opcode, x0n00(opcode)); };
auto mac_d(const u16 opcode) -> std::string {
    return fmt::format("{:#06x} MAC.L @R{d},@R{d}", opcode, x00n0(opcode), x0n00(opcode));
};
auto macw_d(const u16 opcode) -> std::string {
    return fmt::format("{:#06x} MAC.W @R{d}+,@R{d}+", opcode, x00n0(opcode), x0n00(opcode));
};
auto mov_d(const u16 opcode) -> std::string {
    return fmt::format("{:#06x} MOV.B R{d},R{d}", opcode, x00n0(opcode), x0n00(opcode));
};
auto movbs_d(const u16 opcode) -> std::string {
    return fmt::format("{:#06x} MOV.B R{d},@R{d}", opcode, x00n0(opcode), x0n00(opcode));
};
auto movws_d(const u16 opcode) -> std::string {
    return fmt::format("{:#06x} MOV.W R{d},@R{d}", opcode, x00n0(opcode), x0n00(opcode));
};
auto movls_d(const u16 opcode) -> std::string {
    return fmt::format("{:#06x} MOV.L R{d},@R{d}", opcode, x00n0(opcode), x0n00(opcode));
};
auto movbl_d(const u16 opcode) -> std::string {
    return fmt::format("{:#06x} MOV.B @R{d},R{d}", opcode, x00n0(opcode), x0n00(opcode));
};
auto movwl_d(const u16 opcode) -> std::string {
    return fmt::format("{:#06x} MOV.W @R{d},R{d}", opcode, x00n0(opcode), x0n00(opcode));
};
auto movll_d(const u16 opcode) -> std::string {
    return fmt::format("{:#06x} MOV.L @R{d},R{d}", opcode, x00n0(opcode), x0n00(opcode));
};
auto movbm_d(const u16 opcode) -> std::string {
    return fmt::format("{:#06x} MOV.B R{d},@R-{d}", opcode, x00n0(opcode), x0n00(opcode));
};
auto movwm_d(const u16 opcode) -> std::string {
    return fmt::format("{:#06x} MOV.W R{d},@R-{d}", opcode, x00n0(opcode), x0n00(opcode));
};
auto movlm_d(const u16 opcode) -> std::string {
    return fmt::format("{:#06x} MOV.L R{d},@R-{d}", opcode, x00n0(opcode), x0n00(opcode));
};
auto movbp_d(const u16 opcode) -> std::string {
    return fmt::format("{:#06x} MOV.B @R{d}+,R{d}", opcode, x00n0(opcode), x0n00(opcode));
};
auto movwp_d(const u16 opcode) -> std::string {
    return fmt::format("{:#06x} MOV.W @R{d}+,R{d}", opcode, x00n0(opcode), x0n00(opcode));
};
auto movlp_d(const u16 opcode) -> std::string {
    return fmt::format("{:#06x} MOV.L @R{d}+,R{d}", opcode, x00n0(opcode), x0n00(opcode));
};
auto movbs0_d(const u16 opcode) -> std::string {
    return fmt::format("{:#06x} MOV.B R{d}, @(R0,R{d})", opcode, x00n0(opcode), x0n00(opcode));
};
auto movws0_d(const u16 opcode) -> std::string {
    return fmt::format("{:#06x} MOV.W R{d}, @(R0,R{d})", opcode, x00n0(opcode), x0n00(opcode));
};
auto movls0_d(const u16 opcode) -> std::string {
    return fmt::format("{:#06x} MOV.L R{d}, @(R0,R{d})", opcode, x00n0(opcode), x0n00(opcode));
};
auto movbl0_d(const u16 opcode) -> std::string {
    return fmt::format("{:#06x} MOV.B @(R0,R{d}),R{d}", opcode, x00n0(opcode), x0n00(opcode));
};
auto movwl0_d(const u16 opcode) -> std::string {
    return fmt::format("{:#06x} MOV.W @(R0,R{d}),R{d}", opcode, x00n0(opcode), x0n00(opcode));
};
auto movll0_d(const u16 opcode) -> std::string {
    return fmt::format("{:#06x} MOV.L @(R0,R{d}),R{d}", opcode, x00n0(opcode), x0n00(opcode));
};
auto movi_d(const u16 opcode) -> std::string {
    u32 imm{x00nn(opcode)};
    imm = ((imm & 0x80) == 0) ? imm & 0x000000FF : imm | 0xFFFFFF00;
    return fmt::format("{:#06x} MOV #{:#x},R{d}", opcode, imm, x0n00(opcode));
};
auto movwi_d(const u16 opcode) -> std::string {
    u32 imm{x00nn(opcode)};
    return fmt::format("{:#06x} BT PC+{:#x}", opcode, imm * 2 + 4);
};
auto movli_d(const u16 opcode) -> std::string {
    return fmt::format("{:#06x} MOV.L @(PC & 0xFFFFFFFC + {:#x}),R{d}", opcode, x000n(opcode) * 4 + 4, x0n00(opcode));
};
auto movblg_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} MOV.B @({:#x},GBR)", opcode, x000n(opcode)); };
auto movwlg_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} MOV.W @({:#x},GBR)", opcode, x000n(opcode) * 2); };
auto movllg_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} MOV.L @({:#x},GBR)", opcode, x000n(opcode) * 4); };
auto movbsg_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} MOV.B R0,@({:#x},GBR)", opcode, x000n(opcode)); };
auto movwsg_d(const u16 opcode) -> std::string {
    return fmt::format("{:#06x} MOV.W R0,@({:#x},GBR)", opcode, x000n(opcode) * 2);
};
auto movlsg_d(const u16 opcode) -> std::string {
    return fmt::format("{:#06x} MOV.L R0,@({:#x},GBR)", opcode, x000n(opcode) * 4);
};
auto movbs4_d(const u16 opcode) -> std::string {
    return fmt::format("{:#06x} MOV.B R0,@({:#x},R{d})", opcode, x000n(opcode), x00n0(opcode));
};
auto movws4_d(const u16 opcode) -> std::string {
    return fmt::format("{:#06x} MOV.W R0,@({:#x},R{d})", opcode, x000n(opcode) * 2, x00n0(opcode));
};
auto movls4_d(const u16 opcode) -> std::string {
    return fmt::format("{:#06x} MOV.L R{d},@({:#x}R{d})", opcode, x00n0(opcode), x000n(opcode) * 4, x0n00(opcode));
};
auto movbl4_d(const u16 opcode) -> std::string {
    return fmt::format("{:#06x} MOV.B @({:#x},R{d}),R0", opcode, x000n(opcode), x00n0(opcode));
};
auto movwl4_d(const u16 opcode) -> std::string {
    return fmt::format("{:#06x} MOV.W @({:#x},R{d}),R{d}", opcode, x000n(opcode) * 2, x00n0(opcode), x0n00(opcode));
};
auto movll4_d(const u16 opcode) -> std::string {
    return fmt::format("{:#06x} MOV.L @({:#x},R{d}),R{d}", opcode, x000n(opcode) * 4, x00n0(opcode), x0n00(opcode));
};
auto mova_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} MOVA @({:#x},PC),R0", opcode, x000n(opcode)); };
auto movt_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} MOVT R{d}", opcode, x0n00(opcode)); };
auto mull_d(const u16 opcode) -> std::string {
    return fmt::format("{:#06x} MUL.L R{d} R{d}*2", opcode, x00n0(opcode), x0n00(opcode));
};
auto muls_d(const u16 opcode) -> std::string {
    return fmt::format("{:#06x} MULS.W R{d},R{d}", opcode, x00n0(opcode), x0n00(opcode));
};
auto mulu_d(const u16 opcode) -> std::string {
    return fmt::format("{:#06x} MULU.W R{d},R{d}", opcode, x00n0(opcode), x0n00(opcode));
};
auto neg_d(const u16 opcode) -> std::string {
    return fmt::format("{:#06x} NEG R{d},R{d}", opcode, x00n0(opcode), x0n00(opcode));
};
auto negc_d(const u16 opcode) -> std::string {
    return fmt::format("{:#06x} NEGC R{d},R{d}", opcode, x00n0(opcode), x0n00(opcode));
};
auto nop_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} NOP", opcode); };
auto not_op_d(const u16 opcode) -> std::string {
    return fmt::format("{:#06x} NOT R{d},R{d}", opcode, x00n0(opcode), x0n00(opcode));
};
auto or_op_d(const u16 opcode) -> std::string {
    return fmt::format("{:#06x} OR R{d},R{d}", opcode, x00n0(opcode), x0n00(opcode));
};
auto ori_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} ORI #{:#x},R0", opcode, x000n(opcode)); };
auto orm_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} OR.B #{:#x},(R0,GBR)", opcode, x000n(opcode)); };
auto rotcl_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} ROTCL R{d}", opcode, x0n00(opcode)); };
auto rotcr_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} ROTCR R{d}", opcode, x0n00(opcode)); };
auto rotl_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} ROTL R{d}", opcode, x0n00(opcode)); };
auto rotr_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} ROTR R{d}", opcode, x0n00(opcode)); };
auto rte_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} RTE", opcode); };
auto rts_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} RTS", opcode); };
auto sett_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} SETT", opcode); };
auto shal_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} SHAL R{d}", opcode, x0n00(opcode)); };
auto shar_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} SHAR R{d}", opcode, x0n00(opcode)); };
auto shll_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} SHLL R{d}", opcode, x0n00(opcode)); };
auto shll2_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} SHLL2 R{d}", opcode, x0n00(opcode)); };
auto shll8_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} SHLL8 R{d}", opcode, x0n00(opcode)); };
auto shll16_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} SHLL16 R{d}", opcode, x0n00(opcode)); };
auto shlr_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} SHLR R{d}", opcode, x0n00(opcode)); };
auto shlr2_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} SHLR2 R{d}", opcode, x0n00(opcode)); };
auto shlr8_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} SHLR8 R{d}", opcode, x0n00(opcode)); };
auto shlr16_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} SHLR16 R{d}", opcode, x0n00(opcode)); };
auto sleep_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} SLEEP", opcode); };
auto stcsr_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} STC SR,R{d}", opcode, x0n00(opcode)); };
auto stcgbr_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} STC GBR,R{d}", opcode, x0n00(opcode)); };
auto stcvbr_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} STC VBR,R{d}", opcode, x0n00(opcode)); };
auto stcmsr_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} STC.L MSR,@-R{d}", opcode, x0n00(opcode)); };
auto stcmgbr_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} STC.L GBR,@-R{d}", opcode, x0n00(opcode)); };
auto stcmvbr_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} STC.L VBR,@-R{d}", opcode, x0n00(opcode)); };
auto stsmach_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} STS MACH,R{d}", opcode, x0n00(opcode)); };
auto stsmacl_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} STS MACL,R{d}", opcode, x0n00(opcode)); };
auto stspr_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} STS PR,R{d}", opcode, x0n00(opcode)); };
auto stsmmach_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} STS.L MACH,@-R{d}", opcode, x0n00(opcode)); };
auto stsmmacl_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} STS.L MACL,@-R{d}", opcode, x0n00(opcode)); };
auto stsmpr_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} STS.L PR,@-R{d}", opcode, x0n00(opcode)); };
auto sub_d(const u16 opcode) -> std::string {
    return fmt::format("{:#06x} SUB R{d},R{d}", opcode, x00n0(opcode), x0n00(opcode));
};
auto subc_d(const u16 opcode) -> std::string {
    return fmt::format("{:#06x} SUBC R{d},R{d}", opcode, x00n0(opcode), x0n00(opcode));
};
auto subv_d(const u16 opcode) -> std::string {
    return fmt::format("{:#06x} SUBV R{d},R{d}", opcode, x00n0(opcode), x0n00(opcode));
};
auto swapb_d(const u16 opcode) -> std::string {
    return fmt::format("{:#06x} SWAP.B R{d},R{d}", opcode, x00n0(opcode), x0n00(opcode));
};
auto swapw_d(const u16 opcode) -> std::string {
    return fmt::format("{:#06x} SWAP.W R{d},R{d}", opcode, x00n0(opcode), x0n00(opcode));
};
auto tas_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} TAS.B @R{d}", opcode, x0n00(opcode)); };
auto trapa_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} TRAPA #{:#x}", opcode, x000n(opcode) * 4); };
auto tst_d(const u16 opcode) -> std::string {
    return fmt::format("{:#06x} TST R{d},R{d}", opcode, x00n0(opcode), x0n00(opcode));
};
auto tsti_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} TST #{:#x},R0", opcode, x000n(opcode)); };
auto tstm_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} TST.B #{:#x},@(R0,GBR)", opcode, x000n(opcode)); };
auto xor_op_d(const u16 opcode) -> std::string {
    return fmt::format("{:#06x} XOR R{d},R{d}", opcode, x00n0(opcode), x0n00(opcode));
};
auto xori_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} XOR #{:#x},R0", opcode, x000n(opcode)); };
auto xorm_d(const u16 opcode) -> std::string { return fmt::format("{:#06x} XOR.B #{:#x},(R0,GBR)", opcode, x000n(opcode)); };
auto xtrct_d(const u16 opcode) -> std::string {
    return fmt::format("{:#06x} XTRCT R{d},R{d}", opcode, x00n0(opcode), x0n00(opcode));
};

}; // namespace saturnin::sh2
