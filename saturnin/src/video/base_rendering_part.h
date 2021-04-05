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

namespace saturnin::video {

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   VdpType
///
/// \brief  Values that represent VDP types
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class VdpType { not_set, vdp1, vdp2 };

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
    BaseRenderingPart()                         = default;
    BaseRenderingPart(const BaseRenderingPart&) = default;
    BaseRenderingPart(BaseRenderingPart&&)      = default;
    auto operator=(const BaseRenderingPart&) & -> BaseRenderingPart& = default;
    auto operator=(BaseRenderingPart&&) & -> BaseRenderingPart& = default;
    virtual ~BaseRenderingPart()                                = default;
    ///@}

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static auto BaseRenderingPart::getGlobalOrder() -> const u32
    ///
    /// \brief  Returns current global order value, which is incremented after being returned.
    ///
    /// \author Runik
    /// \date   17/02/2021
    ///
    /// \returns    A const u32.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static auto getGlobalOrder() -> const u32 { return global_order_++; }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static void BaseRenderingPart::resetGlobalOrder()
    ///
    /// \brief  Resets global order value.
    ///
    /// \author Runik
    /// \date   17/02/2021
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static void resetGlobalOrder() { global_order_ = 0; }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn virtual void BaseRenderingPart::renderPart() = 0;
    ///
    /// \brief  Renders the current part.
    ///
    /// \author Runik
    /// \date   18/03/2021
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    virtual void renderPart() = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void BaseRenderingPart::setPartType(const VdpType p);
    ///
    /// \brief  Sets the part type.
    ///
    /// \author Runik
    /// \date   18/03/2021
    ///
    /// \param  p   A VdpType.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void setVdpType(const VdpType p) { vdp_type_ = p; };

  private:
    VdpType           vdp_type_{VdpType::not_set}; ///< Type of the part.
    u8                priority_{0};                ///< Priority of the part.
    u32               order_{0};                   ///< Creation order for the same priority parts (mostly used for VDP1 parts).
    static inline u32 global_order_{0};            ///< Static variable used to get the current part order.
};
} // namespace saturnin::video
