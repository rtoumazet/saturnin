// 
// imgui_wrapper.cpp
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
//

#pragma once

#include "imgui_wrapper.h"

namespace ImGui {

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn static auto vectorGetter = [](void* vec, int idx, const char** out_text)
///
/// \brief  Function lambda returning const char * for a string in a vector.
///         Is used by ImGui wrapper functions.
///
/// \author Runik
/// \date   06/09/2018
///
/// \param          vec         Vector of data.
/// \param [in,out] idx         Current index.
/// \param [in,out] values      Returned value.
///
/// \return True if it succeeds.
////////////////////////////////////////////////////////////////////////////////////////////////////
static auto vectorGetter = [](void* vec, int idx, const char** out_text) { 
    auto& vector = *static_cast<std::vector<std::string>*>(vec);
    if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
    *out_text = vector.at(idx).c_str();
    return true;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn bool Combo(const char* label, int* currIndex, std::vector<std::string>& values)
///
/// \brief  Wrapper for ImGui combos allowing the use of vector of strings
///
/// \author Runik
/// \date   06/09/2018
///
/// \param          label       Label of the combo.
/// \param [in,out] currIndex   Current index selected.
/// \param [in,out] values      Values to display in the combo.
///
/// \return True if it succeeds.
////////////////////////////////////////////////////////////////////////////////////////////////////

bool Combo(const char* label, int* currIndex, std::vector<std::string>& values) {
    if (values.empty()) { return false; }
    return Combo(label, 
                 currIndex, 
                 vectorGetter,
                 static_cast<void*>(&values), 
                 values.size());
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn bool ListBox(const char* label, int* currIndex, std::vector<std::string>& values)
///
/// \brief  Wrapper for ImGui listboxes allowing the use of vector of strings
///
/// \author Runik
/// \date   06/09/2018
///
/// \param          label       Label of the combo.
/// \param [in,out] currIndex   Current index selected.
/// \param [in,out] values      Values to display in the combo.
///
/// \return True if it succeeds.
////////////////////////////////////////////////////////////////////////////////////////////////////

bool ListBox(const char* label, int* currIndex, std::vector<std::string>& values) {
    if (values.empty()) { return false; }
    return ListBox(label, 
                   currIndex, 
                   vectorGetter,
                   static_cast<void*>(&values), 
                   values.size());
}

}
