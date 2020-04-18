//
// imgui_wrapper.h
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
/// \file	imgui_wrapper.h
///
/// \brief	Regroups various wrappers for imgui
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <imgui.h>
#include <vector>
#include <string>

namespace ImGui {

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn auto Combo(const char* label, int* current_index, std::vector<std::string>& values) -> bool;
///
/// \brief  Wrapper for ImGui combos allowing the use of vector of strings
///
/// \author Runik
/// \date   06/09/2018
///
/// \param          label       Label of the combo.
/// \param [in,out] current_index   Current index selected.
/// \param [in,out] values      Values to display in the combo.
///
/// \returns    True if it succeeds.
////////////////////////////////////////////////////////////////////////////////////////////////////

auto Combo(const char* label, int* current_index, std::vector<std::string>& values) -> bool;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn auto ListBox(const char* label, int* current_index, std::vector<std::string>& values) -> bool;
///
/// \brief  Wrapper for ImGui listboxes allowing the use of vector of strings
///
/// \author Runik
/// \date   06/09/2018
///
/// \param          label       Label of the combo.
/// \param [in,out] current_index   Current index selected.
/// \param [in,out] values      Values to display in the combo.
///
/// \returns    True if it succeeds.
////////////////////////////////////////////////////////////////////////////////////////////////////

auto ListBox(const char* label, int* current_index, std::vector<std::string>& values) -> bool;

} // namespace ImGui
