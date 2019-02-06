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

using u8    = std::uint8_t;
using u16   = std::uint16_t;
using u32   = std::uint32_t;
using s8    = std::int8_t;
using s16   = std::int16_t;
using s32   = std::int32_t;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  Register
///
/// \brief  Base of any register used in the emulator. Will be specialized to return specific values depending on the register used.
///
/// \author Runik
/// \date   27/01/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class Register {
    public:
        /// \name Constructors/Destructors.
        //@{
    Register(const u32 v) : register_value(v) { val = register_value; };
        Register()                             = delete;
        Register(const Register&)              = delete;
        Register(Register&&)                   = delete;
        Register& operator=(const Register&) & = delete;
        Register& operator=(Register&&) &      = delete;
        ~Register()                            = default;
        //@}
    

    protected:
        inline auto extract(u8 first_bit_index, u8 last_bit_index = 0) {
            if (last_bit_index == 0) last_bit_index = first_bit_index;
            return (register_value >> first_bit_index) & ~(~0 << (last_bit_index - first_bit_index + 1));
        }

        inline auto get(u8 bit_index) { return val[bit_index]; }
        
        // drop bits outside the range [R, L) == [R, L - 1]
        std::bitset<32> get(u8 first_bit_index, u8 last_bit_index) {
            val >>= first_bit_index;            // drop R rightmost bits
            val <<= (32 - last_bit_index - 1);  // drop L leftmost bits
            val >>= (32 - last_bit_index - 1);      // shift back into place
            return val;
        }


        //inline auto get(u8 first_bit_index, u8 last_bit_index) {
        //    u32 mask = 1;
        //    u32 result = 0;
        //    for (size_t i = first_bit_index; i < last_bit_index; ++i) {
        //        if (val.test(i))
        //            result |= mask;
        //        mask <<= 1;
        //    }
        //    return result;
        //}

        u32 register_value; ///< Internal register value.
        std::bitset<32> val;
};

};