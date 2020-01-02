// 
// imgui_custom_controls.h
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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file	imgui_custom_controls.h
///
/// \brief	Regroups various wrappers for imgui
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <imgui.h>

#include "../../src/smpc.h" // PeripheralKey

namespace ImGui {

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn bool ImageButtonWithText(ImTextureID texId, const char* label, const ImVec2& imageSize = ImVec2(0, 0), const ImVec2& uv0 = ImVec2(0, 0), 
    ///     const ImVec2& uv1 = ImVec2(1, 1), int frame_padding = -1, const ImVec4& bg_col = ImVec4(0, 0, 0, 0), const ImVec4& tint_col = ImVec4(1, 1, 1, 1));
    ///
    /// \brief  Image button with text.
    ///
    /// \author Runik
    /// \date   07/10/2019
    ///
    /// \param  texId           Texture identifier.
    /// \param  label           Button label.
    /// \param  imageSize       (Optional) Size of the image.
    /// \param  uv0             (Optional) uv0 coordinates.
    /// \param  uv1             (Optional) uv1 coordinates.
    /// \param  frame_padding   (Optional) The frame padding.
    /// \param  bg_col          (Optional) The background col.
    /// \param  tint_col        (Optional) The tint col.
    ///
    /// \return True if it succeeds, false if it fails.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ImageButtonWithText(ImTextureID texId, const char* label, const ImVec2& imageSize = ImVec2(0, 0), const ImVec2& uv0 = ImVec2(0, 0),
        const ImVec2& uv1 = ImVec2(1, 1), int frame_padding = -1, const ImVec4& bg_col = ImVec4(0, 0, 0, 0), const ImVec4& tint_col = ImVec4(1, 1, 1, 1));

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void peripheralKeyCombo(const std::vector<saturnin::core::PeripheralKey>& keys, saturnin::core::PeripheralKey& default_key, const std::string& combo_name);
    ///
    /// \brief  Peripheral key combo.
    ///
    /// \author Runik
    /// \date   01/01/2020
    ///
    /// \param          keys        List of available PeripheralKeys.
    /// \param [in,out] default_key The default key, will be updated by combo selection.
    /// \param          combo_name  Name of the combo used to display the keys.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void peripheralKeyCombo(const std::vector<saturnin::core::PeripheralKey>& keys, saturnin::core::PeripheralKey& default_key, const std::string& combo_name);
    
}