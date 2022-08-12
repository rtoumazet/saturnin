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

struct PatternNameData;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  Vdp2Part
///
/// \brief  Defines a VDP2 part (8*8 pixels).
///
/// \author Runik
/// \date   18/03/2021
////////////////////////////////////////////////////////////////////////////////////////////////////

class Vdp2Part final : public BaseRenderingPart {
  public:
    ///@{
    /// Constructors / Destructors
    Vdp2Part() = default;
    Vdp2Part(const PatternNameData& pnd,
             const ScreenPos&       pos,
             const size_t           texture_key,
             const u8               priority,
             const ColorF&          color_offset);
    Vdp2Part(size_t        texture_key,
             const u16     texture_width,
             const u16     texture_height,
             const u8      priority,
             const ColorF& color_offset); // Constructor for bitmaps
    Vdp2Part(const Vdp2Part&)                      = default;
    Vdp2Part(Vdp2Part&&)                           = default;
    auto operator=(const Vdp2Part&) & -> Vdp2Part& = default;
    auto operator=(Vdp2Part&&) & -> Vdp2Part&      = default;
    ~Vdp2Part()                                    = default;
    ///@}

    // void renderPart() override;

    // void displayCell();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto Vdp2Part::getTextureKey() const -> const size_t
    ///
    /// \brief  Gets texture key
    ///
    /// \author Runik
    /// \date   04/04/2021
    ///
    /// \returns    The texture key.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    // auto getTextureKey() const -> const size_t { return texture_key_; };

  private:
    ScreenPos scroll_screen_pos_{};       ///< Position in the scroll screen.
    u16       character_number_{};        ///< The character number.
    u16       palette_number_{};          ///< The palette number.
    bool      is_horizontally_flipped_{}; ///< True if the part is horizontally flipped.
    bool      is_vertically_flipped_{};   ///< True if the part is vertically flipped.
    // size_t    texture_key_{};             ///< Link to the texture.
};
} // namespace saturnin::video
