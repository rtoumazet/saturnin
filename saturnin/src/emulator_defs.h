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

using LabelValue       = std::pair<std::string, std::optional<std::string>>;
using AddressToNameMap = std::map<u32, std::string>;

using Mutex       = std::mutex;
using SharedMutex = std::shared_mutex;

constexpr auto register_size = u8{32};

constexpr auto number_of_bits_32 = u8{32};
constexpr auto number_of_bits_16 = u8{16};
constexpr auto number_of_bits_8  = u8{8};

constexpr auto displacement_4  = u8{4};
constexpr auto displacement_6  = u8{6};
constexpr auto displacement_8  = u8{8};
constexpr auto displacement_12 = u8{12};
constexpr auto displacement_16 = u8{16};
constexpr auto displacement_24 = u8{24};
constexpr auto displacement_32 = u8{32};

constexpr auto bitmask_FFFFFFFF00000000 = u64{0xFFFFFFFF00000000};
constexpr auto bitmask_0000000F         = u32{0x0000000F};
constexpr auto bitmask_000000FF         = u32{0x000000FF};
constexpr auto bitmask_00000FFF         = u32{0x00000FFF};
constexpr auto bitmask_0000FFFF         = u32{0x0000FFFF};
constexpr auto bitmask_00FFFFFF         = u32{0x00FFFFFF};
constexpr auto bitmask_0FFFFFFF         = u32{0x0FFFFFFF};
constexpr auto bitmask_0000FF00         = u32{0x0000FF00};
constexpr auto bitmask_00FF0000         = u32{0x00FF0000};
constexpr auto bitmask_FF000000         = u32{0xFF000000};
constexpr auto bitmask_7FFFFFFF         = u32{0x7FFFFFFF};
constexpr auto bitmask_FFFFFF00         = u32{0xFFFFFF00};
constexpr auto bitmask_FFFFF000         = u32{0xFFFFF000};
constexpr auto bitmask_FFFF0000         = u32{0xFFFF0000};
constexpr auto bitmask_FFFFFFFE         = u32{0xFFFFFFFE};
constexpr auto bitmask_FFFFFFFC         = u32{0xFFFFFFFC};
constexpr auto bitmask_F000             = u16{0xF000};
constexpr auto bitmask_0F00             = u16{0x0F00};
constexpr auto bitmask_00F0             = u16{0x00F0};
constexpr auto bitmask_000F             = u16{0x000F};
constexpr auto bitmask_00FF             = u16{0x00FF};
constexpr auto bitmask_03FF             = u16{0x03FF};
constexpr auto bitmask_0FFF             = u16{0x0FFF};
constexpr auto bitmask_FFFF             = u16{0xFFFF};
constexpr auto bitmask_7F               = u8{0x7F};
constexpr auto bitmask_0F               = u8{0x0F};
constexpr auto bitmask_vdp2_vram_4mb    = u16{0x3FFF};
constexpr auto bitmask_vdp2_vram_8mb    = u16{0x7FFF};

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

constexpr auto index_0  = u8{0};
constexpr auto index_1  = u8{1};
constexpr auto index_2  = u8{2};
constexpr auto index_3  = u8{3};
constexpr auto index_4  = u8{4};
constexpr auto index_5  = u8{5};
constexpr auto index_6  = u8{6};
constexpr auto index_7  = u8{7};
constexpr auto index_8  = u8{8};
constexpr auto index_9  = u8{9};
constexpr auto index_10 = u8{10};
constexpr auto index_11 = u8{11};
constexpr auto index_12 = u8{12};
constexpr auto index_13 = u8{13};
constexpr auto index_14 = u8{14};
constexpr auto index_15 = u8{15};
constexpr auto index_16 = u8{16};
constexpr auto index_17 = u8{17};
constexpr auto index_18 = u8{18};
constexpr auto index_19 = u8{19};
constexpr auto index_20 = u8{20};
constexpr auto index_21 = u8{21};
constexpr auto index_22 = u8{22};
constexpr auto index_23 = u8{23};
constexpr auto index_24 = u8{24};
constexpr auto index_25 = u8{25};
constexpr auto index_26 = u8{26};
constexpr auto index_27 = u8{27};
constexpr auto index_28 = u8{28};
constexpr auto index_29 = u8{29};
constexpr auto index_30 = u8{30};
constexpr auto index_31 = u8{31};

constexpr auto bit_0 = u8{0};
constexpr auto bit_1 = u8{1};
constexpr auto bit_2 = u8{2};
constexpr auto bit_3 = u8{3};
constexpr auto bit_4 = u8{4};
constexpr auto bit_5 = u8{5};
constexpr auto bit_6 = u8{6};
constexpr auto bit_7 = u8{7};

constexpr auto bits_in_a_byte = u8{8};

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
