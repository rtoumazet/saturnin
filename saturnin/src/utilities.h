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

#include <string> // string
#include <type_traits> // underlying_type_t
#include <vector> // vector

namespace saturnin {
namespace utilities {

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn std::vector<char> stringToVector(const std::string& source, const uint32_t reserved_size);
///
/// \brief  String to vector.
///
/// \author Runik
/// \date   15/09/2018
///
/// \param  source          Source string to change in vector.
/// \param  reserved_size   Reserved size for the new vector.
///
/// \return A std::vector&lt;char&gt;
////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<char> stringToVector(const std::string& source, const uint32_t reserved_size);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn std::string getLastErrorMessage();
///
/// \brief  Returns a std::string corresponding to GetLastError() value.
///
/// \author Runik
/// \date   22/09/2018
///
/// \return System message linked to GetLastError().
////////////////////////////////////////////////////////////////////////////////////////////////////

std::string getLastErrorMessage();

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
template <typename E>
constexpr auto toUnderlying(E e) noexcept {
    return static_cast<std::underlying_type_t<E>>(e);
}

template <typename M, typename V>
const auto getKeyFromValue(const M& map, const V find_value) {
    auto it = std::find_if(map.begin(),
                           map.end(),
                           [find_value](const M::value_type & p) { return p.second == find_value; });
    return it;
    //if (it != map.end()) {
    //    auto v = *it;
    //    return v.first;
    ////    config->writeValue(core::Access_keys::config_access_method, v.first);
    //}
    //else {
    //    core::Log::error("config", core::tr("Value not found ..."));
    //    std::exit(EXIT_FAILURE);
    //}
}

//auto find_val = cdrom::Cdrom_access_method::aspi;
//auto it = std::find_if(core::Config::cdrom_access.begin(),
//                       core::Config::cdrom_access.end(),
//                       [find_val](const core::Config::Map_cdrom_access::value_type & p) {
//    return p.second == find_val;
//});
//if (it != core::Config::cdrom_access.end()) {
//    auto v = *it;
//    config->writeValue(core::Access_keys::config_access_method, v.first);
//}
}
}