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

Vdp2Part::Vdp2Part(const PatternNameData& pnd,
                   const ScreenPos&       pos,
                   const size_t           texture_key,
                   const u8               priority,
                   const ColorF&          color_offset,
                   const u32              linked_plane_address) :
    scroll_screen_pos_(pos),
    linked_plane_address_(linked_plane_address) {
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

    if (pnd.is_horizontally_flipped) {
        s_left  = 1.0f - s_left;
        s_right = 1.0f - s_right;
    }
    if (pnd.is_vertically_flipped) {
        t_down = 1.0f - t_down;
        t_up   = 1.0f - t_up;
    }
    common_vdp_data_.vertexes.reserve(4);
    common_vdp_data_.vertexes.emplace_back(pos_x, pos_y, s_left, t_down);             // lower left
    common_vdp_data_.vertexes.emplace_back(pos_x_width, pos_y, s_right, t_down);      // lower right
    common_vdp_data_.vertexes.emplace_back(pos_x_width, pos_y_height, s_right, t_up); // upper right
    common_vdp_data_.vertexes.emplace_back(pos_x, pos_y_height, s_left, t_up);        // upper left

    common_vdp_data_.vdp_type     = VdpType::vdp2_cell;
    common_vdp_data_.draw_type    = DrawType::textured_polygon;
    common_vdp_data_.texture_key  = texture_key;
    common_vdp_data_.priority     = priority;
    common_vdp_data_.color_offset = color_offset;
};

Vdp2Part::Vdp2Part(const size_t  texture_key,
                   const u16     texture_width,
                   const u16     texture_height,
                   const u8      priority,
                   const ColorF& color_offset,
                   const VdpType vdp_type) :
    scroll_screen_pos_({0, 0}) {
    const auto pos_x        = static_cast<s16>(0);
    const auto pos_x_width  = static_cast<s16>(texture_width);
    const auto pos_y        = static_cast<s16>(0);
    const auto pos_y_height = static_cast<s16>(texture_height);

    common_vdp_data_.vertexes.reserve(4);
    common_vdp_data_.vertexes.emplace_back(pos_x, pos_y, 0.0f, 0.0f);              // lower left
    common_vdp_data_.vertexes.emplace_back(pos_x_width, pos_y, 1.0f, 0.0f);        // lower right
    common_vdp_data_.vertexes.emplace_back(pos_x_width, pos_y_height, 1.0f, 1.0f); // upper right
    common_vdp_data_.vertexes.emplace_back(pos_x, pos_y_height, 0.0f, 1.0f);       // upper left

    common_vdp_data_.vdp_type     = vdp_type;
    common_vdp_data_.draw_type    = DrawType::textured_polygon;
    common_vdp_data_.texture_key  = texture_key;
    common_vdp_data_.priority     = priority;
    common_vdp_data_.color_offset = color_offset;
};
} // namespace saturnin::video
