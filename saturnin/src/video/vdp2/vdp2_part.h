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
#include <saturnin/src/video/vdp_common.h>

namespace saturnin::video {

struct PatternNameData;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  Vdp2Part
///
/// \brief  Defines a VDP2 part (8*8 pixels).
///
/// \author Runik
/// \date   18/03/2021
////////////////////////////////////////////////////////////////////////////////////////////////////

class Vdp2Part {
  public:
    ///@{
    /// Constructors / Destructors
    Vdp2Part() = default;
    Vdp2Part(const PatternNameData& pnd,
             const ScreenPos&       pos,
             const size_t           texture_key,
             const u8               priority,
             const ColorOffset&     color_offset,
             const u32              linked_plane_address);
    Vdp2Part(size_t             texture_key,
             const u16          texture_width,
             const u16          texture_height,
             const u8           priority,
             const ColorOffset& color_offset,
             const VdpType      vdp_type); // Constructor for bitmaps and planes

    Vdp2Part(const Vdp2Part&)                      = default;
    Vdp2Part(Vdp2Part&&)                           = default;
    auto operator=(const Vdp2Part&) & -> Vdp2Part& = default;
    auto operator=(Vdp2Part&&) & -> Vdp2Part&      = default;
    ~Vdp2Part()                                    = default;
    ///@}

    CommonVdpData common_vdp_data_;        ///< Data shared between different VDP parts.
    ScreenPos     scroll_screen_pos_{};    ///< Position in the scroll screen.
    u32           linked_plane_address_{}; ///< Address of the linked plane, used to generate the plane texture while rendering.
};
} // namespace saturnin::video
