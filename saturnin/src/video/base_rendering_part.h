//
// base_rendering_part.h
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
/// \file	base_rendering_part.h
///
/// \brief	Declares the base class for rendering parts.
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <saturnin/src/emulator_defs.h>
#include <saturnin/src/video/vdp_common.h>

namespace saturnin::video {

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  BaseRenderingPart
///
/// \brief  A base rendering part that will be specialized as Vdp1 or Vpd2 part.
///
/// \author Runik
/// \date   17/02/2021
////////////////////////////////////////////////////////////////////////////////////////////////////

class BaseRenderingPart {
  public:
    ///@{
    /// Constructors / Destructors
    BaseRenderingPart()                                              = default;
    BaseRenderingPart(const BaseRenderingPart&)                      = default;
    BaseRenderingPart(BaseRenderingPart&&)                           = default;
    auto operator=(const BaseRenderingPart&) & -> BaseRenderingPart& = default;
    auto operator=(BaseRenderingPart&&) & -> BaseRenderingPart&      = default;
    virtual ~BaseRenderingPart()                                     = default;
    ///@}

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	static auto BaseRenderingPart::getGlobalOrder() -> u32
    ///
    /// \brief	Returns current global order value, which is incremented after being returned.
    ///
    /// \author	Runik
    /// \date	17/02/2021
    ///
    /// \returns	A const u32.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static auto getGlobalOrder() -> u32 { return global_order_++; }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static void BaseRenderingPart::resetGlobalOrder()
    ///
    /// \brief  Resets global order value.
    ///
    /// \author Runik
    /// \date   17/02/2021
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static void resetGlobalOrder() { global_order_ = 0; }

    ///@{
    /// Accessors / Mutators
    void               vdpType(const VdpType p) { vdp_type_ = p; };
    [[nodiscard]] auto vdpType() const -> VdpType { return vdp_type_; };
    void               drawType(const DrawType d) { draw_type_ = d; };
    [[nodiscard]] auto drawType() const -> DrawType { return draw_type_; };
    void               priority(const u8 p) { priority_ = p; };
    [[nodiscard]] auto priority() const -> u8 { return priority_; };
    void               textureKey(const size_t k) { texture_key_ = k; };
    [[nodiscard]] auto textureKey() const -> size_t { return texture_key_; };
    [[nodiscard]] auto colorOffset() const -> std::array<float, 3> {
        return std::array<float, 3>{color_offset_.r, color_offset_.g, color_offset_.b};
    };
    ///@}

    std::vector<Vertex> vertexes_; ///< Contains the geometry vertexes of the part.

  protected:
    BaseRenderingPart(const VdpType  vdp_type,
                      const DrawType draw_type,
                      const size_t   texture_key  = {},
                      const u8       priority     = {},
                      const ColorF   color_offset = {}) :
        vdp_type_(vdp_type),
        draw_type_(draw_type),
        priority_(priority),
        texture_key_(texture_key),
        color_offset_(color_offset){};

  private:
    VdpType           vdp_type_{VdpType::not_set};     ///< Type of the part.
    DrawType          draw_type_{DrawType::undefined}; ///< Type of the draw
    u8                priority_{0};                    ///< Priority of the part.
    static inline u32 global_order_{0};                ///< Static variable used to get the current part order.
    size_t            texture_key_{};                  ///< Link to the texture.
    ColorF            color_offset_{};                 ///< Color offset for the part.
};
} // namespace saturnin::video
