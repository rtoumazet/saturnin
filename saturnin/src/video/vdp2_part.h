//
// vdp2_part.h
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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file	vdp2_part.h
///
/// \brief	VDP2 part (cell) definition.
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <saturnin/src/emulator_defs.h>
#include <saturnin/src/video/base_rendering_part.h>

namespace saturnin::video {

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  Vdp2Part
///
/// \brief  Defines a VDP2 part (8*8 pixels).
///
/// \author Runik
/// \date   18/03/2021
////////////////////////////////////////////////////////////////////////////////////////////////////

class Vdp2Part : public BaseRenderingPart {
  public:
    //@{
    // Constructors / Destructors
    Vdp2Part() { setPartType(PartType::vdp2); };
    Vdp2Part(const Vdp2Part&) = delete;
    Vdp2Part(Vdp2Part&&)      = delete;
    auto operator=(const Vdp2Part&) & -> Vdp2Part& = delete;
    auto operator=(Vdp2Part&&) & -> Vdp2Part& = delete;
    virtual ~Vdp2Part()                       = default;
    //@}

  private:
    ScreenPos scroll_screen_pos_{};       ///< Position in the scroll screen.
    u32       character_number_{};        ///< The character number.
    u32       palette_number_{};          ///< The palette number.
    u32       color_address_{};           ///< The color address.
    bool      is_horizontally_flipped_{}; ///< True if the part is horizontally flipped.
    bool      is_vertically_flipped_{};   ///< True if the part is vertically flipped.
};
} // namespace saturnin::video
