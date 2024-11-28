//
// emulator_defs.h
// Saturnin
//
// Copyright (c) 2019 Renaud Toumazet
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
/// \file	emulator_defs.h
///
/// \brief	Emulator typedefs.
///
/// Regroups all the typedefs used by the emulator
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <bitset>
#include <cstdint>
#include <optional>
#include <map>
#include <mutex>        // mutex
#include <shared_mutex> // shared_mutex

namespace saturnin {

using u8   = std::uint8_t;
using u16  = std::uint16_t;
using u32  = std::uint32_t;
using u64  = std::uint64_t;
using uptr = std::uintptr_t;
using s8   = std::int8_t;
using s16  = std::int16_t;
using s32  = std::int32_t;
using s64  = std::int64_t;
using sptr = std::intptr_t;

using LabelValue       = std::pair<std::string, std::optional<std::string>>;
using AddressToNameMap = std::map<u32, std::string>;

using Mutex         = std::mutex;
using SharedMutex   = std::shared_mutex;
using ReadOnlyLock  = std::shared_lock<SharedMutex>;
using UpdatableLock = std::unique_lock<SharedMutex>;

constexpr auto sign_bit_48_mask = u64{0x800000000000};
constexpr auto sign_bit_32_mask = u32{0x80000000};
constexpr auto sign_bit_16_mask = u16{0x8000};
constexpr auto sign_bit_12_mask = u16{0x0800};
constexpr auto sign_bit_8_mask  = u8{0x80};

constexpr auto u31_min_64_extended = u64{0xFFFFFFFF80000000};
constexpr auto u47_min_64_extended = u64{0xFFFF800000000000};
constexpr auto u47_max             = u64{0x00007FFFFFFFFFFF};
constexpr auto u32_max             = u32{UINT32_MAX};
constexpr auto u31_max             = u32{0x7FFFFFFF};
constexpr auto u30_max             = u32{0x3FFFFFFF};
constexpr auto u16_max             = u16{UINT16_MAX};
constexpr auto u8_max              = u8{UINT8_MAX};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct Coord
///
/// \brief  A coordinate in 2D space.
///
/// \author Runik
/// \date   21/01/2021
////////////////////////////////////////////////////////////////////////////////////////////////////

struct Coord {
    float x;
    float y;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct ScreenResolution
///
/// \brief  A screen resolution.
///
/// \author Runik
/// \date   10/02/2021
////////////////////////////////////////////////////////////////////////////////////////////////////

struct ScreenResolution {
    u16 width;
    u16 height;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct ScreenPos
///
/// \brief  A screen position.
///
/// \author Runik
/// \date   19/03/2021
////////////////////////////////////////////////////////////////////////////////////////////////////

struct ScreenPos {
    u16 x;
    u16 y;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct ScreenOffset
///
/// \brief  A screen offset.
///
/// \author Runik
/// \date   19/03/2021
////////////////////////////////////////////////////////////////////////////////////////////////////

struct ScreenOffset {
    u16 x;
    u16 y;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct Size
///
/// \brief  An size.
///
/// \author Runik
/// \date   29/04/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

struct Size {
    u16 w; ///< Width
    u16 h; ///< Height
};

// Memory map address ranges
struct AddressRange {
    u32 start;
    u32 end;
};
}; // namespace saturnin
