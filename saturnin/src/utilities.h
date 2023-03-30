//
// utilities.h
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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file	utilities.h
///
/// \brief	Regroups various free functions utilities.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <algorithm>                    // count
#include <functional>                   // hash
#include <string>                       // string
#include <type_traits>                  // underlying_type_t
#include <vector>                       // vector
#include <saturnin/src/emulator_defs.h> // u8, u16, u32
#include <fmt/core.h>

namespace saturnin::utilities {

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn auto stringToVector(const std::string& source, u32 reserved_size) -> std::vector<char>;
///
/// \brief  String to vector.
///
/// \author Runik
/// \date   15/09/2018
///
/// \param  source          Source string to change in vector.
/// \param  reserved_size   Reserved size for the new vector.
///
/// \returns    A std::vector&lt;char&gt;
////////////////////////////////////////////////////////////////////////////////////////////////////

auto stringToVector(const std::string& source, u32 reserved_size) -> std::vector<char>;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn auto getLastErrorMessage() -> std::string;
///
/// \brief  Returns a std::string corresponding to GetLastError() value.
///
/// \author Runik
/// \date   22/09/2018
///
/// \return System message linked to GetLastError().
////////////////////////////////////////////////////////////////////////////////////////////////////

auto getLastErrorMessage() -> std::string;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn constexpr auto toUnderlying(E e) noexcept
///
/// \brief  Converts an enumeration to the underlying value.
///
/// \author Runik
/// \date   17/09/2018
///
/// \param  e   The enumeration to process.
///
/// \return The enumeration underlying value.
////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename E>
constexpr auto toUnderlying(E e) noexcept {
    return static_cast<std::underlying_type_t<E>>(e);
}

template<typename E, class T>
constexpr auto toEnum(T t) noexcept {
    return static_cast<E>(static_cast<std::underlying_type_t<E>>(t));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn template <typename M, typename V> auto getKeyFromValue(const M& map, const V find_value)
///
/// \brief  Gets the key from its value in a 1<->1 map.
///
/// \author Runik
/// \date   02/10/2018
///
/// \tparam M   Map type.
/// \tparam V   Value type.
/// \param  map         The map to search.
/// \param  find_value  The value to find.
///
/// \return An iterator corresponding to the found key, or end().
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename M, typename V>
auto getKeyFromValue(const M& map, const V find_value) {
    return std::ranges::find_if(map, [find_value](const typename M::value_type& p) { return p.second == find_value; });
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn auto explode(std::string const& s, char delim) -> std::vector<std::string>;
///
/// \brief  Explodes the string based on the delimiter.
///
/// \author Runik
/// \date   04/10/2018
///
/// \param  s       String to process.
/// \param  delim   The delimiter.
///
/// \return A vector of strings.
////////////////////////////////////////////////////////////////////////////////////////////////////

auto explode(std::string const& s, char delim) -> std::vector<std::string>;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn auto dec2bcd(u16 dec) -> u32;
///
/// \brief  Converts a number to BCD.
///
/// \author Runik
/// \date   19/01/2020
///
/// \param  dec The number to convert to bcd.
///
/// \returns    The number in bcd representation.
////////////////////////////////////////////////////////////////////////////////////////////////////

auto dec2bcd(u16 dec) -> u32;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn template<typename T, typename... Rest> void hashCombine(std::size_t& seed, const T& v, Rest... rest)
///
/// \brief  Hash combine
///          Usage :
///         std::size_t h=0;
///         hashCombine(h, obj1, obj2, obj3);
///
/// \tparam T       Generic type parameter.
/// \tparam Rest    Type of the REST.
/// \param [in,out] seed    The seed.
/// \param          v       A T to process.
/// \param          rest    Variable arguments providing the REST.
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T, typename... Rest>
void hashCombine(std::size_t& seed, const T& v, Rest... rest) {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    (hashCombine(seed, rest), ...);
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn	template<typename T> inline T swapEndianness(T value);
///
/// \brief	Swap endianness
///
/// \tparam	T	Generic type parameter.
/// \param 	value	The value.
///
/// \returns	A T.
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
inline auto swapEndianness(T value) -> T;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn	template<> inline auto swapEndianness<u16>(u16 value) -> u16
///
/// \brief	Swaps endianness, 16 bits specialization.
///
/// \tparam	u16	Type of the value to swap.
/// \param 	value	Value to swap.
///
/// \returns	Swapped value.
////////////////////////////////////////////////////////////////////////////////////////////////////

template<>
inline auto swapEndianness<u16>(u16 value) -> u16 {
    return static_cast<u16>((value >> 8) | (value << 8));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn	template<> inline u32 swapEndianness<u32>(u32 value)
///
/// \brief	Swaps endianness, 32 bits specialization.
///
/// \tparam	u32	Type of the value to swap.
/// \param 	value	Value to swap.
///
/// \returns	The swapped value.
////////////////////////////////////////////////////////////////////////////////////////////////////

template<>
inline u32 swapEndianness<u32>(u32 value) {
    return static_cast<u32>(swapEndianness<u16>(static_cast<u16>(value)) << 16) | swapEndianness<u16>(value >> 16);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn	inline u32 swapWords(const u32 param)
///
/// \brief	Swaps words in a long.
///
/// \author	Runik
/// \date	07/05/2021
///
/// \param 	param	The value to swap.
///
/// \returns	The swapped value.
////////////////////////////////////////////////////////////////////////////////////////////////////

inline u32 swapWords(const u32 param) { return (param & 0xFFFF0000) >> 16 | (param & 0x0000FFFF) << 16; }

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn template<typename... T> auto format(std::string_view str, T&&... args) -> std::string
///
/// \brief  Formats args according to specifications in str and returns the result as a string.
///         Raw wrapper over fmt::format.
///
/// \tparam T       Generic type parameter.
/// \param          str     String to format.
/// \param          args    Variable formats arguments.
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename... T>
auto format(std::string_view str, T&&... args) -> std::string {
    return fmt::format(fmt::runtime(str), std::forward<T>(args)...);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class	Range
///
/// \brief	Checks if a value is inside a range.
/// 		Ex: if (range<0x10000, 0x1FFFFF>::contains(age)) { ... }
///
/// \author	Runik
/// \date	12/02/2023
///
/// \tparam	min	Range minimum.
/// \tparam	max	Range maximum.
////////////////////////////////////////////////////////////////////////////////////////////////////

template<AddressRange addr>
struct Range {
    static bool contains(u32 i) { return addr.start <= i && i <= addr.end; }
};

} // namespace saturnin::utilities
