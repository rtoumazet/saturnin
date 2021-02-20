//
// vdp1.h
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
/// \brief	Declares the Vdp1 class and related functions.
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <array>                        // array
#include <saturnin/src/emulator_defs.h> // u8, u16, u32
#include <saturnin/src/locale.h>        // tr
#include <saturnin/src/log.h>           // Log
#include <saturnin/src/video/vdp1_registers.h>

// Forward declarations
namespace saturnin::core {
class EmulatorContext;
}

namespace saturnin::video {

using saturnin::core::EmulatorContext;

class Vdp1 {
  public:
    //@{
    // Constructors / Destructors
    Vdp1() = delete;
    Vdp1(EmulatorContext* ec) : emulator_context_(ec){};
    Vdp1(const Vdp1&) = delete;
    Vdp1(Vdp1&&)      = delete;
    auto operator=(const Vdp1&) & -> Vdp1& = delete;
    auto operator=(Vdp1&&) & -> Vdp1& = delete;
    ~Vdp1()                           = default;
    //@}

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Vdp2::initialize();
    ///
    /// \brief  Initializes the VDP1
    ///
    /// \author Runik
    /// \date   15/06/2020
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

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Vdp1::onVblankIn();
    ///
    /// \brief  Handles everything that needs to be done on VBlankIn.
    ///
    /// \author Runik
    /// \date   19/02/2021
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void onVblankIn();

    template<typename T>
    void writeRegisters(const u32 addr, const T data) {
        core::Log::warning("vdp1", core::tr("{}bits register write {:#0x}"), sizeof(T) * number_of_bits_8, addr);
    }

    // 16 bits specialization
    template<>
    void writeRegisters<u16>(const u32 addr, const u16 data) {
        write16(addr, data);
    }

    template<typename T>
    auto readRegisters(const u32 addr) -> T {
        core::Log::warning("vdp1", core::tr("{}bits register read {:#0x}"), sizeof(T) * number_of_bits_8, addr);
        return 0;
    }
    // 16 bits specialization
    template<>
    auto readRegisters<u16>(const u32 addr) -> u16 {
        return read16(addr);
    }

  private:
    /// \name Vdp2 registers accessors
    //@{
    void               write16(u32 addr, u16 data);
    [[nodiscard]] auto read16(u32 addr) const -> u16;
    //@}

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn   void Vdp2::reset();
    ///
    /// \brief    Resets the VPD2
    ///
    /// \author   Runik
    /// \date 16/05/2020
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void reset();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto Vdp1::intializeFramebuffer() -> bool;
    ///
    /// \brief  Initializes the OpenGL framebuffer textures.
    ///
    /// \author Runik
    /// \date   24/06/2020
    ///
    /// \returns    True if it succeeds.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto intializeFramebuffer() -> bool;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Vdp1::populateRenderData();
    ///
    /// \brief  Populates data from the VDP1 memory before backend rendering.
    ///
    /// \author Runik
    /// \date   29/01/2021
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void populateRenderData();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Vdp1::updateResolution();
    ///
    /// \brief  Updates VDP1 resolution by reading internal registers.
    ///
    /// \author Runik
    /// \date   20/02/2021
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void updateResolution();

    EmulatorContext* emulator_context_; ///< Emulator context object.

    // VDP1 registers
    TvModeSelection                tvmr_;
    FrameBufferSwitchMode          fbcr_;
    PlotTrigger                    ptmr_;
    EraseWriteData                 ewdr_;
    EraseWriteUpperLeftCoordinate  ewlr_;
    EraseWriteLowerRightCoordinate ewrr_;
    DrawForcedTermination          endr_;
    TransferEndStatus              edsr_;
    LastOperationCommandAddress    lopr_;
    CurrentOperationCommandAddress copr_;
    ModeStatus                     modr_;

    std::array<u32, 2> framebuffer_; ///< Framebuffers texture id.
};

} // namespace saturnin::video
