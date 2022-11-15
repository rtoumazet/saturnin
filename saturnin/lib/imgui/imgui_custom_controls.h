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

#include <string>
#include <vector>
#include <imgui.h>
#include <saturnin/src/smpc.h> // PeripheralKey

namespace ImGui {

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct	ColorTag
///
/// \brief	A color tag.
///
/// \author	Runik
/// \date	13/11/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

struct ColorTag {
    std::string open_tag;
    std::string end_tag;
    ImVec4      color;
};

const auto none  = ColorTag{"", "", ImVec4(0.0f, 0.0f, 0.0f, 0.0f)};
const auto red   = ColorTag{"[red]", "[/red]", ImVec4(1.0f, 0.0f, 0.0f, 1.0f)};
const auto green = ColorTag{"[green]", "[/green]", ImVec4(0.0f, 1.0f, 0.0f, 1.0f)};
const auto blue  = ColorTag{"[blue]", "[/blue]", ImVec4(0.0f, 0.0f, 1.0f, 1.0f)};

static const std::array<ColorTag, 3> color_tags = {red, green, blue};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn auto ImageButtonWithText(ImTextureID texId, const char* label, const ImVec2& imageSize = ImVec2(0, 0), const ImVec2& uv0 =
/// ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1), int frame_padding = -1, const ImVec4& bg_col = ImVec4(0, 0, 0, 0), const
/// ImVec4& tint_col = ImVec4(1, 1, 1, 1)) -> bool;
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
/// \returns    True if it succeeds, false if it fails.
////////////////////////////////////////////////////////////////////////////////////////////////////

auto ImageButtonWithText(ImTextureID   texId,
                         const char*   label,
                         const ImVec2& imageSize     = ImVec2(0, 0),
                         const ImVec2& uv0           = ImVec2(0, 0),
                         const ImVec2& uv1           = ImVec2(1, 1),
                         int           frame_padding = -1,
                         const ImVec4& bg_col        = ImVec4(0, 0, 0, 0),
                         const ImVec4& tint_col      = ImVec4(1, 1, 1, 1)) -> bool;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn void peripheralKeyCombo(const std::vector<saturnin::core::PeripheralKey>& keys, saturnin::core::PeripheralKey&
/// default_key, const std::string& combo_name);
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

void peripheralKeyCombo(const std::vector<saturnin::core::PeripheralKey>& keys,
                        saturnin::core::PeripheralKey&                    default_key,
                        const std::string&                                combo_name);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn void CenteredText(const std::string& text);
///
/// \brief  Centers the text, wrapper for ImGui::Text().
///
/// \author Runik
/// \date   02/01/2020
///
/// \param  text    Text to display.
////////////////////////////////////////////////////////////////////////////////////////////////////

// void CenteredText(const std::string& text);
void CenteredText(const std::string& text);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn void HelpMarker(const std::string& desc, const bool with_question_mark = false);
///
/// \brief  Helper to display a tooltip when hovered.
///
/// \author Runik
/// \date   03/03/2021
///
/// \param  desc                The description.
/// \param  with_question_mark  (Optional) When true the tooltip will be displayed over a question mark.
////////////////////////////////////////////////////////////////////////////////////////////////////

void HelpMarker(const std::string& desc, const bool with_question_mark = false);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn void ChildWindowHeader(const std::string& label);
///
/// \brief  Adds a header to a child window.
///
/// \author Runik
/// \date   01/06/2021
///
/// \param  label   The header label.
////////////////////////////////////////////////////////////////////////////////////////////////////

void ChildWindowHeader(const std::string& label);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn void TextPadding(float amount);
///
/// \brief  Adds padding to the following text display.
///
/// \author Runik
/// \date   01/06/2021
///
/// \param  amount  The amount of padding.
////////////////////////////////////////////////////////////////////////////////////////////////////

void TextPadding(float amount);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn	void ImGui::TextMultiColored(const std::string text);
///
/// \brief	Displays a text taking into account colored tags.
///
/// \author	Runik
/// \date	13/11/2022
///
/// \param 	text	The text to display.
////////////////////////////////////////////////////////////////////////////////////////////////////

void TextMultiColored(const std::string text);

} // namespace ImGui
