//
// imgui_custom_controls.cpp
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

#include <saturnin/src/pch.h>
#include <saturnin/lib/imgui/imgui_custom_controls.h>

namespace ImGui {

using saturnin::core::getKeyName;
using saturnin::core::PeripheralKey;

auto ImageButtonWithText(ImTextureID   texId,
                         const char*   label,
                         const ImVec2& imageSize,
                         const ImVec2& uv0,
                         const ImVec2& uv1,
                         int           frame_padding,
                         const ImVec4& bg_col,
                         const ImVec4& tint_col) -> bool {
    // ImGuiWindow* window = GetCurrentWindow();
    // if (window->SkipItems)
    //    return false;

    // ImVec2 size = imageSize;
    // if (size.x <= 0 && size.y <= 0) { size.x = size.y = ImGui::GetTextLineHeightWithSpacing(); } else {
    //    if (size.x <= 0)          size.x = size.y;
    //    else if (size.y <= 0)     size.y = size.x;
    //    size *= window->FontWindowScale*ImGui::GetIO().FontGlobalScale;
    //}

    // ImGuiContext& g = *GImGui;
    // const ImGuiStyle& style = g.Style;

    // const ImGuiID id = window->GetID(label);
    // const ImVec2 textSize = ImGui::CalcTextSize(label, NULL, true);
    // const bool hasText = textSize.x > 0;

    // const float innerSpacing = hasText ? ((frame_padding >= 0) ? (float)frame_padding : (style.ItemInnerSpacing.x)) : 0.f;
    // const ImVec2 padding = (frame_padding >= 0) ? ImVec2((float)frame_padding, (float)frame_padding) : style.FramePadding;
    // const ImVec2 totalSizeWithoutPadding(size.x + innerSpacing + textSize.x, size.y > textSize.y ? size.y : textSize.y);
    // const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + totalSizeWithoutPadding + padding * 2);
    // ImVec2 start(0, 0);
    // start = window->DC.CursorPos + padding; if (size.y < textSize.y) start.y += (textSize.y - size.y)*.5f;
    // const ImRect image_bb(start, start + size);
    // start = window->DC.CursorPos + padding; start.x += size.x + innerSpacing; if (size.y > textSize.y) start.y += (size.y -
    // textSize.y)*.5f; ItemSize(bb); if (!ItemAdd(bb, &id))
    //    return false;

    // bool hovered = false, held = false;
    // bool pressed = ButtonBehavior(bb, id, &hovered, &held);

    //// Render
    // const ImU32 col = GetColorU32((hovered && held) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered :
    // ImGuiCol_Button); RenderFrame(bb.Min, bb.Max, col, true, ImClamp((float)ImMin(padding.x, padding.y), 0.0f,
    // style.FrameRounding)); if (bg_col.w > 0.0f)
    //    window->DrawList->AddRectFilled(image_bb.Min, image_bb.Max, GetColorU32(bg_col));

    // window->DrawList->AddImage(texId, image_bb.Min, image_bb.Max, uv0, uv1, GetColorU32(tint_col));

    // if (textSize.x > 0) ImGui::RenderText(start, label);
    // return pressed;
    return true;
}

void peripheralKeyCombo(const std::vector<PeripheralKey>& keys, PeripheralKey& default_key, const std::string& combo_name) {
    const auto default_key_name = std::string{getKeyName(default_key)};
    const auto combo            = std::string{"##combo_" + combo_name};
    if (ImGui::BeginCombo(combo.c_str(), default_key_name.c_str())) {
        for (auto key : keys) {
            bool is_selected = (default_key_name.c_str() == getKeyName(key).c_str());
            if (ImGui::Selectable(getKeyName(key).c_str(), is_selected)) {
                default_key = key;
                if (is_selected) { ImGui::SetItemDefaultFocus(); }
            }
        }
        ImGui::EndCombo();
    }
}

void CenteredText(const std::string& text) {
    auto           size  = ImGui::CalcTextSize(text.c_str());
    constexpr auto ratio = float{0.5f};
    ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth() * ratio - size.x / 2);
    ImGui::TextUnformatted(text.c_str());
}

// Helper to display a little (?) mark which shows a tooltip when hovered.
// In your own code you may want to display an actual icon if you are using a merged icon fonts (see docs/FONTS.md)
void HelpMarker(const std::string& desc, const bool with_question_mark) {
    if (with_question_mark) { ImGui::TextDisabled("(?)"); }
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc.c_str());
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

} // namespace ImGui
