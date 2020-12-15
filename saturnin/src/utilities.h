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
#include <string>                       // string
#include <type_traits>                  // underlying_type_t
#include <vector>                       // vector
#include <saturnin/src/emulator_defs.h> // u8, u16, u32

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
    return std::find_if(map.begin(), map.end(), [find_value](const typename M::value_type& p) { return p.second == find_value; });
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

} // namespace saturnin::utilities
