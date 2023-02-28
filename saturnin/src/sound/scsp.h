//
// scsp.h
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
/// \file	scsp.h
///
/// \brief	Declares the SCSP class.
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <saturnin/src/emulator_defs.h>
#include <saturnin/src/emulator_modules.h>
#include <saturnin/src/log.h>
#include <saturnin/src/memory.h>

// Forward declarations
namespace saturnin::core {
class EmulatorContext;
class EmulatorModules;
class Memory;
} // namespace saturnin::core

namespace saturnin::sound {

using saturnin::core::EmulatorContext;
using saturnin::core::EmulatorModules;
using saturnin::core::Log;
using saturnin::core::Logger;

class Scsp {
  public:
    //@{
    // Constructors / Destructors
    Scsp() = delete;
    explicit Scsp(EmulatorContext* ec) : modules_(ec) { external_access_modules_ = ec; };
    Scsp(const Scsp&)                      = delete;
    Scsp(Scsp&&)                           = delete;
    auto operator=(const Scsp&) & -> Scsp& = delete;
    auto operator=(Scsp&&) & -> Scsp&      = delete;
    ~Scsp()                                = default;
    //@}

    template<typename T>
    void write(const u32 addr, const T data) {}

    // 8 bits specialization
    template<>
    void write<u8>(const u32 addr, const u8 data) {
        // if ((addr & 0x1FFFFF) == 0x700) __debugbreak();
        write8(addr, data);
    }

    // 16 bits specialization
    template<>
    void write<u16>(const u32 addr, const u16 data) {
        // if ((addr & 0x1FFFFF) == 0x700) __debugbreak();
        write16(addr, data);
    }

    // 32 bits specialization
    template<>
    void write<u32>(const u32 addr, const u32 data) {
        // if ((addr & 0x1FFFFF) == 0x700) __debugbreak();
        write32(addr, data);
    }

    template<typename T>
    auto read(const u32 addr) -> T {}
    // 8 bits specialization
    template<>
    auto read<u8>(const u32 addr) -> u8 {
        return read8(addr);
    }
    // 16 bits specialization
    template<>
    auto read<u16>(const u32 addr) -> u16 {
        return read16(addr);
    }
    // 32 bits specialization
    template<>
    auto read<u32>(const u32 addr) -> u32 {
        return read32(addr);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Scsp::initialize();
    ///
    /// \brief  Initializes the SCSP module (including the 68K).
    ///
    /// \author Runik
    /// \date   05/05/2021
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void initialize();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Scsp::reset();
    ///
    /// \brief  Resets the sound system.
    ///
    /// \author Runik
    /// \date   05/05/2021
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void reset();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Scsp::run(const u32 cycles);
    ///
    /// \brief  Runs the SCSP for the number of cycles specified.
    ///
    /// \author Runik
    /// \date   05/05/2021
    ///
    /// \param  cycles  Number of cycles to execute.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void run(const u32 cycles);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static void Scsp::scspHostInterruptHandler();
    ///
    /// \brief  Interrupt sent to the host system by the SCSP.
    ///
    /// \author Runik
    /// \date   05/05/2021
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static void scspHostInterruptHandler();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static void Scsp::scsp68kInterruptHandler(const u32 level);
    ///
    /// \brief  Interrupt sent to the 68K sound CPU by the SCSP.
    ///
    /// \author Runik
    /// \date   05/05/2021
    ///
    /// \param  level   The interrupt level.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static void scsp68kInterruptHandler(const u32 level);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static auto Scsp::ram() -> std::array<u8, core::sound_ram_size>&
    ///
    /// \brief  Reference to the SCSP RAM area to allow access from external functions (like
    ///         Musashi's)
    ///
    /// \author Runik
    /// \date   05/05/2021
    ///
    /// \returns    A reference to the SCSP RAM.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static auto ram() -> std::array<u8, core::sound_ram_size>& { return external_access_modules_.memory()->sound_ram_; }

  private:
    /// \name SCSP memory accessors
    ///@{
    void               write8(u32 addr, u8 data);
    void               write16(u32 addr, u16 data);
    void               write32(u32 addr, u32 data);
    [[nodiscard]] auto read8(u32 addr) const -> u8;
    [[nodiscard]] auto read16(u32 addr) const -> u16;
    [[nodiscard]] auto read32(u32 addr) const -> u32;
    ///@}

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Scsp::calculateSamplesPerFrame();
    ///
    /// \brief  Calculates the number of samples per frame.
    ///
    /// \author Runik
    /// \date   07/05/2021
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void calculateSamplesPerFrame();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Scsp::calculate68KCyclesRatio();
    ///
    /// \brief  Calculates the 68K cycles ratio relative to the SH2 cycles
    ///
    /// \author Runik
    /// \date   07/05/2021
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void calculate68KCyclesRatio();

    static EmulatorModules external_access_modules_; ///< Used to get access to the soundram data from Musashi's functions
    EmulatorModules        modules_;

    u16   samples_per_frame_{};  ///< Number of samples to be played in one frame. Depends on the frame duration
    float m68k_cycles_ratio_{};  ///< The 68k cycles ratio relative to the sh2 cycles.
    u32   elapsed_68k_cycles_{}; ///< The elapsed 68k cycles.
};

} // namespace saturnin::sound
