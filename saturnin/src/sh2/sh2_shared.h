//
// sh2_shared.h
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
/// \file	sh2_shared.h
///
/// \brief	SH2 shared definitions and functions.
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <saturnin/src/emulator_defs.h>

namespace saturnin::sh2 {

// Forward declaration
class Sh2;

constexpr auto opcodes_lut_size = u32{0x10000}; ///< Size of the opcodes lookup table

/// \name Opcode decoding helpers
//@{
inline auto xn00(const u16 inst) -> u8 { return static_cast<u8>((inst & 0x0F00) >> 8); }
inline auto x0n0(const u16 inst) -> u8 { return static_cast<u8>((inst & 0x00F0) >> 4); }
inline auto xnnn(const u16 inst) -> u16 { return (inst & 0xFFF); }
inline auto x0nn(const u16 inst) -> u8 { return static_cast<u8>(inst & 0xFFu); }
inline auto x00n(const u16 inst) -> u8 { return static_cast<u8>(inst & 0xF); }
//@}

} // namespace saturnin::sh2
