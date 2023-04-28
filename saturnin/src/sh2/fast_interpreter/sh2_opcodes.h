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

namespace saturnin::sh2 {
class Sh2;
}

namespace saturnin::sh2::fast_interpreter {
void add(Sh2& s, const u32 n, const u32 m);
void addi(Sh2& s, const u32 n, const u32 i);
void addc(Sh2& s, const u32 n, const u32 m);
void addv(Sh2& s, const u32 n, const u32 m);
void and_op(Sh2& s, const u32 n, const u32 m);
void andi(Sh2& s, const u32 i);
void andm(Sh2& s, const u32 i);
void bf(Sh2& s, const u32 d);
void bfs(Sh2& s, const u32 d);
void bra(Sh2& s, const u32 d);
void braf(Sh2& s, const u32 m);
void bsr(Sh2& s, const u32 d);
void bsrf(Sh2& s, const u32 m);
void bt(Sh2& s, const u32 d);
void bts(Sh2& s, const u32 d);
void clrmac(Sh2& s);
void clrt(Sh2& s);
void cmpeq(Sh2& s, const u32 n, const u32 m);
void cmpge(Sh2& s, const u32 n, const u32 m);
void cmpgt(Sh2& s, const u32 n, const u32 m);
void cmphi(Sh2& s, const u32 n, const u32 m);
void cmphs(Sh2& s, const u32 n, const u32 m);
void cmppl(Sh2& s, const u32 n);
void cmppz(Sh2& s, const u32 n);
void cmpstr(Sh2& s, const u32 n, const u32 m);
void cmpim(Sh2& s, const u32 i);
void div0s(Sh2& s, const u32 n, const u32 m);
void div0u(Sh2& s);
void div1(Sh2& s, const u32 n, const u32 m);
void dmuls(Sh2& s, const u32 n, const u32 m);
void dmulu(Sh2& s, const u32 n, const u32 m);
void dt(Sh2& s, const u32 n);
void extsb(Sh2& s, const u32 n, const u32 m);
void extsw(Sh2& s, const u32 n, const u32 m);
void extub(Sh2& s, const u32 n, const u32 m);
void extuw(Sh2& s, const u32 n, const u32 m);
void jmp(Sh2& s, const u32 m);
void jsr(Sh2& s, const u32 m);
void ldcsr(Sh2& s, const u32 m);
void ldcgbr(Sh2& s, const u32 m);
void ldcvbr(Sh2& s, const u32 m);
void ldcmsr(Sh2& s, const u32 m);
void ldcmgbr(Sh2& s, const u32 m);
void ldcmvbr(Sh2& s, const u32 m);
void ldsmach(Sh2& s, const u32 m);
void ldsmacl(Sh2& s, const u32 m);
void ldspr(Sh2& s, const u32 m);
void ldsmmach(Sh2& s, const u32 m);
void ldsmmacl(Sh2& s, const u32 m);
void ldsmpr(Sh2& s, const u32 m);
void mac(Sh2& s, const u32 n, const u32 m);
void macw(Sh2& s, const u32 n, const u32 m);
} // namespace saturnin::sh2::fast_interpreter
