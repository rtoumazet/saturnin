//
// sh2_disasm.cpp
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

#include <saturnin/src/pch.h>
#include <saturnin/src/sh2/basic_interpreter/sh2_disasm.h>
#include <istream>
#include <fmt/format.h>
#include <saturnin/src/sh2/sh2_shared.h>
#include <saturnin/src/utilities.h> // format

namespace uti = saturnin::utilities;

namespace saturnin::sh2::basic_interpreter {

auto badOpcode_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} Unknown opcode", pc, opcode);
};
auto add_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} ADD R{:d},R{:d}", pc, opcode, x0n0(opcode), xn00(opcode));
};
auto addi_d(const u32 pc, const u16 opcode) -> std::string {
    auto imm = u32{x0nn(opcode)};
    imm      = ((imm & sign_bit_8_mask) == 0) ? imm & 0xFF : imm | 0xFFFFFF00u;
    return uti::format("{:#010x} {:#06x} ADD #{:#010x},R{:d}", pc, opcode, imm, xn00(opcode));
};
auto addc_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} ADDC R{:d},R{:d}", pc, opcode, x0n0(opcode), xn00(opcode));
};
auto addv_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} ADDV R{:d},R{:d}", pc, opcode, x0n0(opcode), xn00(opcode));
};
auto and_op_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} AND R{:d},R{:d}", pc, opcode, x0n0(opcode), xn00(opcode));
};
auto andi_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} AND #{:#x},R0", pc, opcode, x00n(opcode));
};
auto andm_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} AND.B #{:#x},@(R0,GBR)", pc, opcode, x00n(opcode));
};
auto bf_d(const u32 pc, const u16 opcode) -> std::string {
    auto imm = u32{x0nn(opcode)};
    imm      = ((imm & sign_bit_8_mask) == 0) ? imm & 0xFF : imm | 0xFFFFFF00u;
    return uti::format("{:#010x} {:#06x} BF {:#x}", pc, opcode, pc + imm * 2 + 4);
};
auto bfs_d(const u32 pc, const u16 opcode) -> std::string {
    auto imm = u32{x0nn(opcode)};
    imm      = ((imm & sign_bit_8_mask) == 0) ? imm & 0xFF : imm | 0xFFFFFF00u;
    return uti::format("{:#010x} {:#06x} BF/S {:#x}", pc, opcode, pc + imm * 2 + 4);
};
auto bra_d(const u32 pc, const u16 opcode) -> std::string {
    auto imm = u32{xnnn(opcode)};
    imm      = ((imm & sign_bit_12_mask) == 0) ? imm & 0xFFF : imm | 0xFFFFF000u;
    return uti::format("{:#010x} {:#06x} BRA {:#x}", pc, opcode, pc + imm * 2 + 4);
};
auto braf_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} BRAF R{:d}", pc, opcode, xn00(opcode));
};
auto bsr_d(const u32 pc, const u16 opcode) -> std::string {
    auto imm = u32{xnnn(opcode)};
    imm      = ((imm & sign_bit_12_mask) == 0) ? imm & 0xFFF : imm | 0xFFFFF000u;
    return uti::format("{:#010x} {:#06x} BSR {:#x}", pc, opcode, pc + imm * 2 + 4);
};
auto bsrf_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} BSRF R{:d}", pc, opcode, xn00(opcode));
};
auto bt_d(const u32 pc, const u16 opcode) -> std::string {
    auto imm = u32{x0nn(opcode)};
    imm      = ((imm & sign_bit_8_mask) == 0) ? imm & 0xFF : imm | 0xFFFFFF00u;
    return uti::format("{:#010x} {:#06x} BT {:#x}", pc, opcode, pc + imm * 2 + 4);
};
auto bts_d(const u32 pc, const u16 opcode) -> std::string {
    auto imm = u32{x0nn(opcode)};
    imm      = ((imm & sign_bit_8_mask) == 0) ? imm & 0xFF : imm | 0xFFFFFF00u;
    return uti::format("{:#010x} {:#06x} BT/S {:#x}", pc, opcode, pc + imm * 2 + 4);
};
auto clrmac_d(const u32 pc, const u16 opcode) -> std::string { return uti::format("{:#010x} {:#06x} CLRMAC", pc, opcode); };
auto clrt_d(const u32 pc, const u16 opcode) -> std::string { return uti::format("{:#010x} {:#06x} CLRT", pc, opcode); };
auto cmpeq_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} CMP/EQ R{:d},R{:d}", pc, opcode, x0n0(opcode), xn00(opcode));
};
auto cmpge_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} CMP/GE R{:d},R{:d}", pc, opcode, x0n0(opcode), xn00(opcode));
};
auto cmpgt_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} CMP/GT R{:d},R{:d}", pc, opcode, x0n0(opcode), xn00(opcode));
};
auto cmphi_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} CMP/HI R{:d},R{:d}", pc, opcode, x0n0(opcode), xn00(opcode));
};
auto cmphs_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} CMP/HS R{:d},R{:d}", pc, opcode, x0n0(opcode), xn00(opcode));
};
auto cmppl_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} CMP/PL R{:d}", pc, opcode, xn00(opcode));
};
auto cmppz_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} CMP/PZ R{:d}", pc, opcode, xn00(opcode));
};
auto cmpstr_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} CMP/STR R{:d},R{:d}", pc, opcode, x0n0(opcode), xn00(opcode));
};
auto cmpim_d(const u32 pc, const u16 opcode) -> std::string {
    auto imm = u32{x0nn(opcode)};
    imm      = ((imm & sign_bit_8_mask) == 0) ? imm & 0xFF : imm | 0xFFFFFF00u;
    return uti::format("{:#010x} {:#06x} CMP/EQ #{:#x},R0", pc, opcode, imm);
};
auto div0s_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} DIV0S R{:d},R{:d}", pc, opcode, x0n0(opcode), xn00(opcode));
};
auto div0u_d(const u32 pc, const u16 opcode) -> std::string { return uti::format("{:#010x} {:#06x} DIV0U", pc, opcode); };
auto div1_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} DIV1 R{:d},R{:d}", pc, opcode, x0n0(opcode), xn00(opcode));
};
auto dmuls_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} DMULS.L R{:d},R{:d}", pc, opcode, x0n0(opcode), xn00(opcode));
};
auto dmulu_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} DMULU.L R{:d},R{:d}", pc, opcode, x0n0(opcode), xn00(opcode));
};
auto dt_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} DT R{:d}", pc, opcode, xn00(opcode));
};
auto extsb_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} EXTS.B R{:d},R{:d}", pc, opcode, x0n0(opcode), xn00(opcode));
};
auto extsw_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} EXTS.W R{:d},R{:d}", pc, opcode, x0n0(opcode), xn00(opcode));
};
auto extub_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} EXTU.B R{:d},R{:d}", pc, opcode, x0n0(opcode), xn00(opcode));
};
auto extuw_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} EXTU.W R{:d},R{:d}", pc, opcode, x0n0(opcode), xn00(opcode));
};
auto jmp_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} JMP @R{:d}", pc, opcode, xn00(opcode));
};
auto jsr_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} JSR @R{:d}", pc, opcode, xn00(opcode));
};
auto ldcsr_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} LDC R{:d},SR", pc, opcode, xn00(opcode));
};
auto ldcgbr_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} LDC R{:d},GBR", pc, opcode, xn00(opcode));
};
auto ldcvbr_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} LDC R{:d},VBR", pc, opcode, xn00(opcode));
};
auto ldcmsr_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} LDS.L @R{:d}+,SR", pc, opcode, xn00(opcode));
};
auto ldcmgbr_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} LDS.L @R{:d}+,GBR", pc, opcode, xn00(opcode));
};
auto ldcmvbr_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} LDS.L @R{:d}+,VBR", pc, opcode, xn00(opcode));
};
auto ldsmach_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} LDS R{:d},MACH", pc, opcode, xn00(opcode));
};
auto ldsmacl_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} LDS R{:d},MACL", pc, opcode, xn00(opcode));
};
auto ldspr_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} LDS R{:d},PR", pc, opcode, xn00(opcode));
};
auto ldsmmach_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} LDS.L @R{:d}+,MACH", pc, opcode, xn00(opcode));
};
auto ldsmmacl_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} LDS.L @R{:d}+,MACL", pc, opcode, xn00(opcode));
};
auto ldsmpr_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} LDS.L @R{:d}+,PR", pc, opcode, xn00(opcode));
};
auto mac_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} MAC.L @R{:d},@R{:d}", pc, opcode, x0n0(opcode), xn00(opcode));
};
auto macw_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} MAC.W @R{:d}+,@R{:d}+", pc, opcode, x0n0(opcode), xn00(opcode));
};
auto mov_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} MOV.B R{:d},R{:d}", pc, opcode, x0n0(opcode), xn00(opcode));
};
auto movbs_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} MOV.B R{:d},@R{:d}", pc, opcode, x0n0(opcode), xn00(opcode));
};
auto movws_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} MOV.W R{:d},@R{:d}", pc, opcode, x0n0(opcode), xn00(opcode));
};
auto movls_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} MOV.L R{:d},@R{:d}", pc, opcode, x0n0(opcode), xn00(opcode));
};
auto movbl_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} MOV.B @R{:d},R{:d}", pc, opcode, x0n0(opcode), xn00(opcode));
};
auto movwl_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} MOV.W @R{:d},R{:d}", pc, opcode, x0n0(opcode), xn00(opcode));
};
auto movll_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} MOV.L @R{:d},R{:d}", pc, opcode, x0n0(opcode), xn00(opcode));
};
auto movbm_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} MOV.B R{:d},@R-{:d}", pc, opcode, x0n0(opcode), xn00(opcode));
};
auto movwm_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} MOV.W R{:d},@R-{:d}", pc, opcode, x0n0(opcode), xn00(opcode));
};
auto movlm_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} MOV.L R{:d},@R-{:d}", pc, opcode, x0n0(opcode), xn00(opcode));
};
auto movbp_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} MOV.B @R{:d}+,R{:d}", pc, opcode, x0n0(opcode), xn00(opcode));
};
auto movwp_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} MOV.W @R{:d}+,R{:d}", pc, opcode, x0n0(opcode), xn00(opcode));
};
auto movlp_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} MOV.L @R{:d}+,R{:d}", pc, opcode, x0n0(opcode), xn00(opcode));
};
auto movbs0_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} MOV.B R{:d}, @(R0,R{:d})", pc, opcode, x0n0(opcode), xn00(opcode));
};
auto movws0_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} MOV.W R{:d}, @(R0,R{:d})", pc, opcode, x0n0(opcode), xn00(opcode));
};
auto movls0_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} MOV.L R{:d}, @(R0,R{:d})", pc, opcode, x0n0(opcode), xn00(opcode));
};
auto movbl0_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} MOV.B @(R0,R{:d}),R{:d}", pc, opcode, x0n0(opcode), xn00(opcode));
};
auto movwl0_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} MOV.W @(R0,R{:d}),R{:d}", pc, opcode, x0n0(opcode), xn00(opcode));
};
auto movll0_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} MOV.L @(R0,R{:d}),R{:d}", pc, opcode, x0n0(opcode), xn00(opcode));
};
auto movi_d(const u32 pc, const u16 opcode) -> std::string {
    auto imm = u32{x0nn(opcode)};
    imm      = ((imm & sign_bit_8_mask) == 0) ? imm & 0xFF : imm | 0xFFFFFF00u;
    return uti::format("{:#010x} {:#06x} MOV #{:#x},R{:d}", pc, opcode, imm, xn00(opcode));
};
auto movwi_d(const u32 pc, const u16 opcode) -> std::string {
    auto imm = u32{x0nn(opcode)};
    return uti::format("{:#010x} {:#06x} BT {:#x}", pc, opcode, pc + imm * 2 + 4);
};
auto movli_d(const u32 pc, const u16 opcode) -> std::string {
    auto disp = u32{(0xFFu & x0nn(opcode))};
    return uti::format("{:#010x} {:#06x} MOV.L @({:#x}),R{:d}", pc, opcode, (pc & 0xFFFFFFFCu) + (disp << 2) + 4, xn00(opcode));
};
auto movblg_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} MOV.B @({:#x},GBR)", pc, opcode, x00n(opcode));
};
auto movwlg_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} MOV.W @({:#x},GBR)", pc, opcode, x00n(opcode) * 2);
};
auto movllg_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} MOV.L @({:#x},GBR)", pc, opcode, x00n(opcode) * 4);
};
auto movbsg_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} MOV.B R0,@({:#x},GBR)", pc, opcode, x00n(opcode));
};
auto movwsg_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} MOV.W R0,@({:#x},GBR)", pc, opcode, x00n(opcode) * 2);
};
auto movlsg_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} MOV.L R0,@({:#x},GBR)", pc, opcode, x00n(opcode) * 4);
};
auto movbs4_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} MOV.B R0,@({:#x},R{:d})", pc, opcode, x00n(opcode), x0n0(opcode));
};
auto movws4_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} MOV.W R0,@({:#x},R{:d})", pc, opcode, x00n(opcode) * 2, x0n0(opcode));
};
auto movls4_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} MOV.L R{:d},@({:#x},R{:d})", pc, opcode, x0n0(opcode), x00n(opcode) * 4, xn00(opcode));
};
auto movbl4_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} MOV.B @({:#x},R{:d}),R0", pc, opcode, x00n(opcode), x0n0(opcode));
};
auto movwl4_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} MOV.W @({:#x},R{:d}),R0", pc, opcode, x00n(opcode) * 2, x0n0(opcode));
};
auto movll4_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} MOV.L @({:#x},R{:d}),R{:d}", pc, opcode, x00n(opcode) * 4, x0n0(opcode), xn00(opcode));
};
auto mova_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} MOVA @({:#x},PC),R0", pc, opcode, x00n(opcode));
};
auto movt_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} MOVT R{:d}", pc, opcode, xn00(opcode));
};
auto mull_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} MUL.L R{:d} R{:d}*2", pc, opcode, x0n0(opcode), xn00(opcode));
};
auto muls_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} MULS.W R{:d},R{:d}", pc, opcode, x0n0(opcode), xn00(opcode));
};
auto mulu_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} MULU.W R{:d},R{:d}", pc, opcode, x0n0(opcode), xn00(opcode));
};
auto neg_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} NEG R{:d},R{:d}", pc, opcode, x0n0(opcode), xn00(opcode));
};
auto negc_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} NEGC R{:d},R{:d}", pc, opcode, x0n0(opcode), xn00(opcode));
};
auto nop_d(const u32 pc, const u16 opcode) -> std::string { return uti::format("{:#010x} {:#06x} NOP", pc, opcode); };
auto not_op_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} NOT R{:d},R{:d}", pc, opcode, x0n0(opcode), xn00(opcode));
};
auto or_op_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} OR R{:d},R{:d}", pc, opcode, x0n0(opcode), xn00(opcode));
};
auto ori_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} ORI #{:#x},R0", pc, opcode, x00n(opcode));
};
auto orm_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} OR.B #{:#x},(R0,GBR)", pc, opcode, x00n(opcode));
};
auto rotcl_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} ROTCL R{:d}", pc, opcode, xn00(opcode));
};
auto rotcr_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} ROTCR R{:d}", pc, opcode, xn00(opcode));
};
auto rotl_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} ROTL R{:d}", pc, opcode, xn00(opcode));
};
auto rotr_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} ROTR R{:d}", pc, opcode, xn00(opcode));
};
auto rte_d(const u32 pc, const u16 opcode) -> std::string { return uti::format("{:#010x} {:#06x} RTE", pc, opcode); };
auto rts_d(const u32 pc, const u16 opcode) -> std::string { return uti::format("{:#010x} {:#06x} RTS", pc, opcode); };
auto sett_d(const u32 pc, const u16 opcode) -> std::string { return uti::format("{:#010x} {:#06x} SETT", pc, opcode); };
auto shal_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} SHAL R{:d}", pc, opcode, xn00(opcode));
};
auto shar_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} SHAR R{:d}", pc, opcode, xn00(opcode));
};
auto shll_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} SHLL R{:d}", pc, opcode, xn00(opcode));
};
auto shll2_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} SHLL2 R{:d}", pc, opcode, xn00(opcode));
};
auto shll8_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} SHLL8 R{:d}", pc, opcode, xn00(opcode));
};
auto shll16_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} SHLL16 R{:d}", pc, opcode, xn00(opcode));
};
auto shlr_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} SHLR R{:d}", pc, opcode, xn00(opcode));
};
auto shlr2_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} SHLR2 R{:d}", pc, opcode, xn00(opcode));
};
auto shlr8_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} SHLR8 R{:d}", pc, opcode, xn00(opcode));
};
auto shlr16_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} SHLR16 R{:d}", pc, opcode, xn00(opcode));
};
auto sleep_d(const u32 pc, const u16 opcode) -> std::string { return uti::format("{:#010x} {:#06x} SLEEP", pc, opcode); };
auto stcsr_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} STC SR,R{:d}", pc, opcode, xn00(opcode));
};
auto stcgbr_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} STC GBR,R{:d}", pc, opcode, xn00(opcode));
};
auto stcvbr_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} STC VBR,R{:d}", pc, opcode, xn00(opcode));
};
auto stcmsr_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} STC.L MSR,@-R{:d}", pc, opcode, xn00(opcode));
};
auto stcmgbr_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} STC.L GBR,@-R{:d}", pc, opcode, xn00(opcode));
};
auto stcmvbr_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} STC.L VBR,@-R{:d}", pc, opcode, xn00(opcode));
};
auto stsmach_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} STS MACH,R{:d}", pc, opcode, xn00(opcode));
};
auto stsmacl_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} STS MACL,R{:d}", pc, opcode, xn00(opcode));
};
auto stspr_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} STS PR,R{:d}", pc, opcode, xn00(opcode));
};
auto stsmmach_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} STS.L MACH,@-R{:d}", pc, opcode, xn00(opcode));
};
auto stsmmacl_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} STS.L MACL,@-R{:d}", pc, opcode, xn00(opcode));
};
auto stsmpr_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} STS.L PR,@-R{:d}", pc, opcode, xn00(opcode));
};
auto sub_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} SUB R{:d},R{:d}", pc, opcode, x0n0(opcode), xn00(opcode));
};
auto subc_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} SUBC R{:d},R{:d}", pc, opcode, x0n0(opcode), xn00(opcode));
};
auto subv_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} SUBV R{:d},R{:d}", pc, opcode, x0n0(opcode), xn00(opcode));
};
auto swapb_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} SWAP.B R{:d},R{:d}", pc, opcode, x0n0(opcode), xn00(opcode));
};
auto swapw_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} SWAP.W R{:d},R{:d}", pc, opcode, x0n0(opcode), xn00(opcode));
};
auto tas_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} TAS.B @R{:d}", pc, opcode, xn00(opcode));
};
auto trapa_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} TRAPA #{:#x}", pc, opcode, x00n(opcode) * 4);
};
auto tst_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} TST R{:d},R{:d}", pc, opcode, x0n0(opcode), xn00(opcode));
};
auto tsti_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} TST #{:#x},R0", pc, opcode, x00n(opcode));
};
auto tstm_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} TST.B #{:#x},@(R0,GBR)", pc, opcode, x00n(opcode));
};
auto xor_op_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} XOR R{:d},R{:d}", pc, opcode, x0n0(opcode), xn00(opcode));
};
auto xori_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} XOR #{:#x},R0", pc, opcode, x00n(opcode));
};
auto xorm_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} XOR.B #{:#x},(R0,GBR)", pc, opcode, x00n(opcode));
};
auto xtrct_d(const u32 pc, const u16 opcode) -> std::string {
    return uti::format("{:#010x} {:#06x} XTRCT R{:d},R{:d}", pc, opcode, x0n0(opcode), xn00(opcode));
};

auto disasm(const u32 pc, const u16 opcode) -> std::string { return opcodes_disasm_lut[opcode](pc, opcode); }

}; // namespace saturnin::sh2::basic_interpreter
