//
// vdp2.h
// Saturnin
//
// Copyright (c) 2020 Renaud Toumazet
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
/// \file	vdp1.h
///
/// \brief	Declares the Vdp2 class and related functions.
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "vdp2_registers.h"

// Forward declarations
namespace saturnin::core {
class EmulatorContext;
}

namespace saturnin::video {

using saturnin::core::EmulatorContext;

// Saturn video resolution
//  Horizontal resolution : 320 or 352 dots (PAL or NTSC)
//  Vertical resolution :
//  - PAL
//     - 312.5 lines non interlaced (50Hz / frame), 625 lines interlaced  (25Hz / frame)
//     - 288 active lines (non interlaced), 576 active lines (interlaced)
//     - 24.5 blanking lines (non interlaced), 49 blanking lines (interlaced)
//  - NTSC :
//     - 262.5 lines non interlaced (60Hz / frame), 525 lines interlaced (30Hz / frame)
//     - 240 active lines (non interlaced), 480 active lines (interlaced)
//     - 22.5 blanking lines (non interlaced), 45 blanking lines (interlaced)

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   TvStandard
///
/// \brief  Tv standard used.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class TvStandard : s8 {
    unknown = -1, ///< Unknown.
    pal     = 0,  ///< PAL.
    ntsc    = 1   ///< NTSC.
};

class Vdp2 {
  public:
    //@{
    // Constructors / Destructors
    Vdp2() = delete;
    Vdp2(EmulatorContext* ec) : emulator_context_(ec){};
    Vdp2(const Vdp2&) = delete;
    Vdp2(Vdp2&&)      = delete;
    auto operator=(const Vdp2&) & -> Vdp2& = delete;
    auto operator=(Vdp2&&) & -> Vdp2& = delete;
    ~Vdp2()                           = default;
    //@}

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Vdp2::initialize();
    ///
    /// \brief  Initializes the VDP2
    ///
    /// \author Runik
    /// \date   16/05/2020
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void initialize();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Vdp2::run(u8 sh2_cycles);
    ///
    /// \brief  Runs the VDP2 for the given number of SH2 cycles
    ///
    /// \author Runik
    /// \date   17/05/2020
    ///
    /// \param  cycles  The cycles to run.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void run(u8 cycles);

    /// \name Vdp2 registers accessors
    //@{
    void               write8(u32 addr, u8 data);
    void               write16(u32 addr, u16 data);
    void               write32(u32 addr, u32 data);
    [[nodiscard]] auto read8(u32 addr) const -> u8;
    [[nodiscard]] auto read16(u32 addr) const -> u16;
    [[nodiscard]] auto read32(u32 addr) const -> u32;
    //@}

  private:
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn   void Vdp2::reset();
    ///
    /// \brief    Resets the VPD2
    ///
    /// \author   Runik
    /// \date 16/05/2020
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void reset();

    EmulatorContext* emulator_context_; ///< Emulator context object.

    // VDP2 registers
    TvScreenModeRegister tvmd_;
};

} // namespace saturnin::video
