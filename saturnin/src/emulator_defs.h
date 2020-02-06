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
    Register& operator=(const Register&) & = default;
    Register& operator=(Register&&) & = default;
    ~Register()                       = default;
    //@}

    template<typename T>
    inline T get(const BitRange<T>& r) {
        auto range = register_value;
        range >>= r.first_bit_pos_;           // drops rightmost bits
        range <<= (32 - r.last_bit_pos_ - 1); // drops leftmost bits
        range >>= (32 - r.last_bit_pos_ - 1); // shifts back into place
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

    inline std::bitset<32>::reference operator[](const u8 index) { return register_value[index]; }

    template<typename T>
    inline void reset(const BitRange<T>& r) {
        for (u8 i = r.first_bit_pos_; i <= r.last_bit_pos_; ++i) {
            register_value.reset(i);
        }
    }

    inline void reset() { register_value.reset(); }

    inline bool none() const { return register_value.none(); }
    inline bool any() const { return register_value.any(); }
    inline bool all() const { return register_value.all(); }

    inline u32 toU32() const { return register_value.to_ulong(); };

  protected:
    std::bitset<32> register_value; ///< Internal register value.
};

}; // namespace saturnin
