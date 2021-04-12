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

Vdp2Part::Vdp2Part(const PatternNameData& pnd, ScreenPos&& pos, size_t texture_key) {
    setVdpType(VdpType::vdp2);
    scroll_screen_pos_       = std::move(pos);
    character_number_        = pnd.character_number;
    palette_number_          = pnd.palette_number;
    is_horizontally_flipped_ = pnd.is_horizontally_flipped;
    is_vertically_flipped_   = pnd.is_vertically_flipped;
    texture_key_             = texture_key;

    // Vdp2 parts are 8*8 pixels squares
    constexpr auto cell_width  = u8{8};
    constexpr auto cell_height = u8{8};
    const auto     pos_x       = static_cast<s16>(pos.x);
    const auto     pos_y       = static_cast<s16>(pos.y);
    part_vertexes_.emplace_back(Vertex{pos_x, pos_y, 0, 0, 0, 0, 0.0, 1.0});                            // lower left
    part_vertexes_.emplace_back(Vertex{pos_x + cell_width, pos_y, 0, 0, 0, 0, 1.0, 1.0});               // lower right
    part_vertexes_.emplace_back(Vertex{pos_x + cell_width, pos_y + cell_height, 0, 0, 0, 0, 1.0, 0.0}); // upper right
    part_vertexes_.emplace_back(Vertex{pos_x, pos_y + cell_height, 0, 0, 0, 0, 0.0, 0.0});              // upper left
};

void Vdp2Part::renderPart() { displayCell(); };

void Vdp2Part::displayCell() {}

} // namespace saturnin::video
