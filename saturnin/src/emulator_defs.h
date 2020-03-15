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

constexpr u8 register_size{32};

constexpr u8 number_of_bits_32{32};
constexpr u8 number_of_bits_16{16};
constexpr u8 number_of_bits_8{8};

constexpr u8 displacement_4{4};
constexpr u8 displacement_8{8};
constexpr u8 displacement_12{12};
constexpr u8 displacement_16{16};
constexpr u8 displacement_24{24};
constexpr u8 displacement_32{32};

constexpr u64 bitmask_FFFFFFFF00000000{0xFFFFFFFF00000000};
constexpr u32 bitmask_0000000F{0x0000000F};
constexpr u32 bitmask_000000FF{0x000000FF};
constexpr u32 bitmask_00000FFF{0x00000FFF};
constexpr u32 bitmask_0000FFFF{0x0000FFFF};
constexpr u32 bitmask_00FFFFFF{0x00FFFFFF};
constexpr u32 bitmask_0FFFFFFF{0x0FFFFFFF};
constexpr u32 bitmask_0000FF00{0x0000FF00};
constexpr u32 bitmask_00FF0000{0x00FF0000};
constexpr u32 bitmask_FF000000{0xFF000000};
constexpr u32 bitmask_7FFFFFFF{0x7FFFFFFF};
constexpr u32 bitmask_FFFFFF00{0xFFFFFF00};
constexpr u32 bitmask_FFFFF000{0xFFFFF000};
constexpr u32 bitmask_FFFF0000{0xFFFF0000};
constexpr u32 bitmask_FFFFFFFE{0xFFFFFFFE};
constexpr u32 bitmask_FFFFFFFC{0xFFFFFFFC};
constexpr u16 bitmask_F000{0xF000};
constexpr u16 bitmask_0F00{0x0F00};
constexpr u16 bitmask_00F0{0x00F0};
constexpr u16 bitmask_000F{0x000F};
constexpr u16 bitmask_00FF{0x00FF};
constexpr u16 bitmask_03FF{0x03FF};
constexpr u16 bitmask_0FFF{0x0FFF};
constexpr u16 bitmask_FFFF{0xFFFF};
constexpr u8  bitmask_0F{0x0F};

constexpr u64 sign_bit_48_mask{0x800000000000};
constexpr u32 sign_bit_32_mask{0x80000000};
constexpr u16 sign_bit_16_mask{0x8000};
constexpr u16 sign_bit_12_mask{0x0800};
constexpr u8  sign_bit_8_mask{0x80};

constexpr u64 uint31_min_64_extended{0xFFFFFFFF80000000};
constexpr u64 uint47_min_64_extended{0xFFFF800000000000};
constexpr u64 uint47_max{0x00007FFFFFFFFFFF};
constexpr u32 uint32_max{UINT32_MAX};
constexpr u32 uint31_max{0x7FFFFFFF};
constexpr u32 uint30_max{0x3FFFFFFF};

constexpr u8 index_0{0};
constexpr u8 index_1{1};
constexpr u8 index_2{2};
constexpr u8 index_3{3};
constexpr u8 index_4{4};
constexpr u8 index_5{5};
constexpr u8 index_6{6};
constexpr u8 index_7{7};
constexpr u8 index_8{8};
constexpr u8 index_9{9};
constexpr u8 index_10{10};
constexpr u8 index_11{11};
constexpr u8 index_12{12};
constexpr u8 index_13{13};
constexpr u8 index_14{14};
constexpr u8 index_15{15};
constexpr u8 index_16{16};
constexpr u8 index_17{17};
constexpr u8 index_18{18};
constexpr u8 index_19{19};
constexpr u8 index_20{20};
constexpr u8 index_21{21};
constexpr u8 index_22{22};
constexpr u8 index_23{23};
constexpr u8 index_24{24};
constexpr u8 index_25{25};
constexpr u8 index_26{26};
constexpr u8 index_27{27};
constexpr u8 index_28{28};
constexpr u8 index_29{29};
constexpr u8 index_30{30};
constexpr u8 index_31{31};

constexpr u8 bit_0{0};
constexpr u8 bit_1{1};
constexpr u8 bit_2{2};
constexpr u8 bit_3{3};
constexpr u8 bit_4{4};
constexpr u8 bit_5{5};
constexpr u8 bit_6{6};
constexpr u8 bit_7{7};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct BitRange
///
/// \brief  A range of bits.
///
/// \author Runik
/// \date   07/02/2019
///
/// \tparam T   Generic type parameter.
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct BitRange {
    u8 first_bit_pos_;
    u8 last_bit_pos_;

    BitRange(u8 pos) : first_bit_pos_{pos}, last_bit_pos_{pos} {};
    BitRange(u8 first, u8 last) : first_bit_pos_{first}, last_bit_pos_{last} {};
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  Register
///
/// \brief  Base of any register used in the emulator. Will be specialized to return specific values depending on the register
/// used.
///
/// \author Runik
/// \date   27/01/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class Register {
  public:
    /// \name Constructors/Destructors.
    //@{
    Register(const u32 v) : register_value(v){};
    Register()                = default;
    Register(const Register&) = default;
    Register(Register&&)      = default;
    auto operator=(const Register&) & -> Register& = default;
    auto operator=(Register&&) & -> Register& = default;
    ~Register()                               = default;
    //@}

    template<typename T>
    inline auto get(const BitRange<T>& r) -> T {
        auto range = register_value;
        range >>= r.first_bit_pos_;                      // drops rightmost bits
        range <<= (register_size - r.last_bit_pos_ - 1); // drops leftmost bits
        range >>= (register_size - r.last_bit_pos_ - 1); // shifts back into place
        return static_cast<T>(range.to_ulong());
    }

    template<typename T>
    inline void set(const BitRange<T>& r, T new_value) {
        u32 nv = static_cast<u32>(new_value);
        u8  j  = 0;
        for (u8 i = r.first_bit_pos_; i <= r.last_bit_pos_; ++i) {
            register_value[i] = (nv & 1 << j) ? true : false;
            ++j;
        }
    }

    template<typename T>
    inline void set(const BitRange<T>& r) {
        for (u8 i = r.first_bit_pos_; i <= r.last_bit_pos_; ++i) {
            register_value.set(i);
        }
    }

    inline void set() { register_value.set(); }

    inline auto operator[](const u8 index) -> std::bitset<register_size>::reference { return register_value[index]; }

    template<typename T>
    inline void reset(const BitRange<T>& r) {
        for (u8 i = r.first_bit_pos_; i <= r.last_bit_pos_; ++i) {
            register_value.reset(i);
        }
    }

    inline void reset() { register_value.reset(); }

    [[nodiscard]] inline auto none() const -> bool { return register_value.none(); }
    [[nodiscard]] inline auto any() const -> bool { return register_value.any(); }
    [[nodiscard]] inline auto all() const -> bool { return register_value.all(); }

    [[nodiscard]] inline auto toU32() const -> u32 { return register_value.to_ulong(); };

  protected:
    std::bitset<register_size> register_value; ///< Internal register value.
};

}; // namespace saturnin
