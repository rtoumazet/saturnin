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
#include <String.>
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

// void CenteredText(const std::string& text) {
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

void ChildWindowHeader(const std::string& label) {
    const auto pos  = ImGui::GetWindowPos();
    const auto size = ImGui::GetWindowSize();
    ImGui::GetWindowDrawList()->AddRectFilled(pos,
                                              ImVec2(pos.x + size.x, pos.y + 15.f),
                                              ImGui::GetColorU32(ImGuiCol_TableHeaderBg, 255),
                                              5.f,
                                              ImDrawCornerFlags_Top);
    ImGui::AlignTextToFramePadding();
    ImGui::SameLine(3.0f);
    ImGui::Text("%s", label.c_str());
}

void TextPadding(float amount) {
    ImGui::AlignTextToFramePadding();
    auto cursor_pos = ImGui::GetCursorPos();
    cursor_pos.x += 3.f;
    ImGui::SetCursorPos(cursor_pos);
}

const char ColorMarkerStart = '[';
const char ColorMarkerEnd   = ']';

bool ProcessInlineHexColor(const char* start, const char* end, ImVec4& color) {
    const int hexCount = (int)(end - start);
    if (hexCount == 6 || hexCount == 8) {
        char hex[9];
        // std::strncpy(hex, start, hexCount);
        strncpy_s(hex, start, hexCount);
        hex[hexCount] = 0;

        unsigned int hexColor = 0;
        if (sscanf_s(hex, "%x", &hexColor) > 0) {
            color.x = static_cast<float>((hexColor & 0x00FF0000) >> 16) / 255.0f;
            color.y = static_cast<float>((hexColor & 0x0000FF00) >> 8) / 255.0f;
            color.z = static_cast<float>((hexColor & 0x000000FF)) / 255.0f;
            color.w = 1.0f;

            if (hexCount == 8) { color.w = static_cast<float>((hexColor & 0xFF000000) >> 24) / 255.0f; }

            return true;
        }
    }

    return false;
}

void TextWithColors(const char* fmt, ...) {
    char tempStr[4096];

    va_list argPtr;
    va_start(argPtr, fmt);
    _vsnprintf_s(tempStr, sizeof(tempStr), fmt, argPtr);
    va_end(argPtr);
    tempStr[sizeof(tempStr) - 1] = '\0';

    bool        pushedColorStyle = false;
    const char* textStart        = tempStr;
    const char* textCur          = tempStr;
    while (textCur < (tempStr + sizeof(tempStr)) && *textCur != '\0') {
        if (*textCur == ColorMarkerStart) {
            // Print accumulated text
            if (textCur != textStart) {
                ImGui::TextUnformatted(textStart, textCur);
                ImGui::SameLine(0.0f, 0.0f);
            }

            // Process color code
            const char* colorStart = textCur + 1;
            do {
                ++textCur;
            } while (*textCur != '\0' && *textCur != ColorMarkerEnd);

            // Change color
            if (pushedColorStyle) {
                ImGui::PopStyleColor();
                pushedColorStyle = false;
            }

            ImVec4 textColor;
            if (ProcessInlineHexColor(colorStart, textCur, textColor)) {
                ImGui::PushStyleColor(ImGuiCol_Text, textColor);
                pushedColorStyle = true;
            }

            textStart = textCur + 1;
        } else if (*textCur == '\n') {
            // Print accumulated text an go to next line
            ImGui::TextUnformatted(textStart, textCur);
            textStart = textCur + 1;
        }

        ++textCur;
    }

    if (textCur != textStart) {
        ImGui::TextUnformatted(textStart, textCur);
    } else {
        ImGui::NewLine();
    }

    if (pushedColorStyle) { ImGui::PopStyleColor(); }
}

} // namespace ImGui
