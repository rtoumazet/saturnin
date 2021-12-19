//
// vdp2_part.cpp
// Saturnin
//
// Copyright (c) 2021 Renaud Toumazet
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
#include <saturnin/src/video/vdp2_part.h>
#include <saturnin/src/video/vdp2.h> // PatternNameData

namespace saturnin::video {

Vdp2Part::Vdp2Part(const PatternNameData& pnd, const ScreenPos& pos, const size_t texture_key, const u8 priority) :
    BaseRenderingPart(VdpType::vdp2, DrawType::textured_polygon, texture_key, priority), scroll_screen_pos_(pos),
    character_number_(pnd.character_number), palette_number_(pnd.palette_number),
    is_horizontally_flipped_(pnd.is_horizontally_flipped), is_vertically_flipped_(pnd.is_vertically_flipped) {
    // Vdp2 parts are 8*8 pixels squares
    constexpr auto cell_width   = u8{8};
    constexpr auto cell_height  = u8{8};
    const auto     pos_x        = static_cast<s16>(pos.x);
    const auto     pos_x_width  = static_cast<s16>(pos_x + cell_width);
    const auto     pos_y        = static_cast<s16>(pos.y);
    const auto     pos_y_height = static_cast<s16>(pos.y + cell_height);

    auto s_left  = float{0.0};
    auto s_right = float{1.0};
    auto t_down  = float{0.0};
    auto t_up    = float{1.0};

    if (is_horizontally_flipped_) {
        s_left  = 1.0f - s_left;
        s_right = 1.0f - s_right;
    }
    if (is_vertically_flipped_) {
        t_down = 1.0f - t_down;
        t_up   = 1.0f - t_up;
    }

    vertexes_.emplace_back(pos_x, pos_y, s_left, t_down);             // lower left
    vertexes_.emplace_back(pos_x_width, pos_y, s_right, t_down);      // lower right
    vertexes_.emplace_back(pos_x_width, pos_y_height, s_right, t_up); // upper right
    vertexes_.emplace_back(pos_x, pos_y_height, s_left, t_up);        // upper left
};

Vdp2Part::Vdp2Part(const size_t texture_key, const u16 texture_width, const u16 texture_height, const u8 priority) :
    BaseRenderingPart(VdpType::vdp2, DrawType::textured_polygon, texture_key, priority), scroll_screen_pos_({0, 0}),
    character_number_(0), palette_number_(0), is_horizontally_flipped_(false), is_vertically_flipped_(false) {
    const auto pos_x        = static_cast<s16>(0);
    const auto pos_x_width  = static_cast<s16>(texture_width);
    const auto pos_y        = static_cast<s16>(0);
    const auto pos_y_height = static_cast<s16>(texture_height);

    vertexes_.emplace_back(pos_x, pos_y, 0.0, 0.0);              // lower left
    vertexes_.emplace_back(pos_x_width, pos_y, 1.0, 0.0);        // lower right
    vertexes_.emplace_back(pos_x_width, pos_y_height, 1.0, 1.0); // upper right
    vertexes_.emplace_back(pos_x, pos_y_height, 0.0, 1.0);       // upper left
};

// void Vdp2Part::renderPart() { displayCell(); };
//
// void Vdp2Part::displayCell() {}

} // namespace saturnin::video
