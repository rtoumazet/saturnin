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

#include <saturnin/src/pch.h>
#include <saturnin/src/video/imgui_wrapper.h>

namespace ImGui {

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn static auto vectorGetter = [](void* vec, int idx, const char** out_text)
///
/// \brief  Function lambda allowing to use vector of strings in ImGui functions.
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

auto Combo(const char* label, int* current_index, std::vector<std::string>& values) -> bool {
    if (values.empty()) { return false; }
    if (ImGui::BeginCombo(label, values[*current_index].c_str())) {
        for (int i = 0; i < values.size(); ++i) {
            const bool is_selected = (*current_index == i);
            if (ImGui::Selectable(values[i].c_str(), is_selected)) { *current_index = i; }
            if (is_selected) { ImGui::SetItemDefaultFocus(); }
        }
        ImGui::EndCombo();
    }
    return true;
}

auto ListBox(const char* label, int* current_index, std::vector<std::string>& values) -> bool {
    if (values.empty()) { return false; }
    if (ImGui::BeginListBox(label)) {
        for (int n = 0; n < values.size(); n++) {
            const bool is_selected = (*current_index == n);
            if (ImGui::Selectable(values[n].c_str(), is_selected)) { *current_index = n; }
            if (is_selected) { ImGui::SetItemDefaultFocus(); }
        }
        ImGui::EndListBox();
    }
}

} // namespace ImGui
