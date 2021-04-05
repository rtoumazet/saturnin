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
};

void Vdp2Part::renderPart(){};

void Vdp2Part::displayCell(const u32 x, const u32 y) {}

} // namespace saturnin::video
