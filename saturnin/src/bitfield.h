//
// bitfield.h
// Saturnin
//
// Copyright (c) 2022 Renaud Toumazet
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
/// \file	bitfield.h
///
/// \brief	Bitfield declarations.
///
/// Original code by Evan Teran : https://blog.codef00.com/2014/12/06/portable-bitfields-using-c11
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <saturnin/src/emulator_defs.h>
#include <cstddef>
#include <type_traits>

namespace saturnin {

namespace {

template<size_t LastBit>
struct MinimumTypeHelper {
    typedef typename std::conditional<
        LastBit == 0,
        void,
        typename std::conditional<
            LastBit <= 8,
            u8,
            typename std::conditional<
                LastBit <= 16,
                u16,
                typename std::conditional<LastBit <= 32, u32, typename std::conditional<LastBit <= 64, u64, void>::type>::type>::
                type>::type>::type type;
};

} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  BitField
///
/// \brief  A bit field.
///
/// \author Runik
/// \date   14/01/2022
///
/// \tparam Index   Start index.
/// \tparam Bits    Number of bits.
////////////////////////////////////////////////////////////////////////////////////////////////////

template<size_t Index, size_t Bits = 1>
class BitField {
  private:
    enum { Mask = (1u << Bits) - 1u };

    typedef typename MinimumTypeHelper<Index + Bits>::type T;

  public:
    template<class T2>
    BitField& operator=(T2 value) {
        value_ = (value_ & ~(Mask << Index)) | ((value & Mask) << Index);
        return *this;
    }

    operator T() const { return (value_ >> Index) & Mask; }

    explicit operator bool() const { return (value_ >> Index) & Mask; }

    BitField& operator++() { return *this = *this + 1; }
    T         operator++(int) {
        T r = *this;
        ++*this;
        return r;
    }
    BitField& operator--() { return *this = *this - 1; }
    T         operator--(int) {
        T r = *this;
        --*this;
        return r;
    }
    BitField& operator&=(const T& data) { return *this = *this & data; }
    BitField& operator|=(const T& data) { return *this = *this | data; }
    BitField& operator^=(const T& data) { return *this = *this ^ data; }

  private:
    T value_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  BitField<Index,1>
///
/// \brief  BitField specialization to consider 1 bit BitField as a boolean.
///
/// \author Runik
/// \date   14/01/2022
///
/// \tparam Index Start index.
////////////////////////////////////////////////////////////////////////////////////////////////////

template<size_t Index>
class BitField<Index, 1> {
  private:
    enum { Bits = 1, Mask = 0x01 };

    typedef typename MinimumTypeHelper<Index + Bits>::type T;

  public:
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	BitField& 1>::operator=(bool value)
    ///
    /// \brief	Assignment operator
    ///
    /// \author	Runik
    /// \date	17/01/2022
    ///
    /// \param 	value	True to value.
    ///
    /// \returns	A shallow copy of this object.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    BitField& operator=(bool value) {
        value_ = (value_ & ~(Mask << Index)) | (value << Index);
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn template<class T, class = typename std::enable_if<std::is_enum<T>::value>::type> BitField& 1>::operator=(T e)
    ///
    /// \brief  Assignment operator specialization for scoped enums.
    ///
    /// \author Runik
    /// \date   14/01/2022
    ///
    /// \param  e   A T to process.
    ///
    /// \returns    A shallow copy of this object.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    template<class T, class = typename std::enable_if<std::is_enum<T>::value>::type>
    BitField& operator=(T e) {
        auto value = static_cast<typename std::underlying_type<T>::type>(e);
        value_     = (value_ & ~(Mask << Index)) | (value << Index);
        return *this;
    }

    explicit operator bool() const { return value_ & (Mask << Index); }
    //         operator T() const { return static_cast<T>(value_ & (Mask << Index)); }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	template<class T2, typename = std::enable_if_t<std::is_integral_v<T2>>> bool 1>::operator==(T2 val)
    ///
    /// \brief	Equality operator for integral values.
    ///
    /// \author	Runik
    /// \date	17/01/2022
    ///
    /// \param 	val	The value.
    ///
    /// \returns	True if the parameters are considered equivalent.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    template<class T2, typename = std::enable_if_t<std::is_integral_v<T2>>>
    bool operator==(T2 val) {
        return (value_ & (Mask << Index)) == val;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	template<class T2> bool 1>::operator==(T2& b)
    ///
    /// \brief	Equality operator between BitField bools.
    ///
    /// \author	Runik
    /// \date	17/01/2022
    ///
    /// \param [in,out]	b	A T2 to process.
    ///
    /// \returns	True if the parameters are considered equivalent.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    template<class T2>
    bool operator==(T2& b) {
        return static_cast<bool>(value_ & (Mask << Index)) == static_cast<bool>(b);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	template<class T2> bool 1>::operator==(T2& b)
    ///
    /// \brief	Difference operator between BitField bools.
    ///
    /// \author	Runik
    /// \date	19/01/2022
    ///
    /// \param [in,out]	b	A T2 to process.
    ///
    /// \returns	True if the parameters are considered equivalent.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    template<class T2>
    bool operator!=(T2& b) {
        return static_cast<bool>(value_ & (Mask << Index)) != static_cast<bool>(b);
    }

  private:
    T value_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn template<typename E, size_t T, size_t N> constexpr auto toEnum(BitField<T, N> b) noexcept
///
/// \brief  BitField to scoped enum conversion helper.
///
/// \tparam E     Type of the e.
/// \tparam Index Index of the BitField.
/// \tparam Size  Size of the BitField.
/// \param  b     A BitField&lt;T,N&gt; to process.
///
/// \returns      BitField as an enum.
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename E, size_t Index, size_t Size>
constexpr auto toEnum(BitField<Index, Size> b) noexcept {
    return static_cast<E>(static_cast<std::underlying_type_t<E>>(b));
}

template<typename E, class T>
constexpr auto toEnum(T t) noexcept {
    return static_cast<E>(static_cast<std::underlying_type_t<E>>(t));
}

} // namespace saturnin
