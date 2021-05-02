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

class Scsp {
  public:
    //@{
    // Constructors / Destructors
    Scsp() = delete;
    Scsp(EmulatorContext* ec) : modules_(ec){};
    Scsp(const Scsp&) = delete;
    Scsp(Scsp&&)      = delete;
    auto operator=(const Scsp&) & -> Scsp& = delete;
    auto operator=(Scsp&&) & -> Scsp& = delete;
    ~Scsp()                           = default;
    //@}

    template<typename T>
    void write(const u32 addr, const T data) {}

    // 8 bits specialization
    template<>
    void write<u8>(const u32 addr, const u8 data) {
        write8(addr, data);
    }

    // 16 bits specialization
    template<>
    void write<u16>(const u32 addr, const u16 data) {
        write16(addr, data);
    }

    // 32 bits specialization
    template<>
    void write<u32>(const u32 addr, const u32 data) {
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

    void reset() { Log::error("scsp", "reset() not implemented"); };
    void setSound(bool isSet) { Log::error("scsp", "setSound() not implemented"); };

  private:
    EmulatorModules modules_;

    /// \name SCSP memory accessors
    //@{
    void               write8(u32 addr, u8 data);
    void               write16(u32 addr, u16 data);
    void               write32(u32 addr, u32 data);
    [[nodiscard]] auto read8(u32 addr) const -> u8;
    [[nodiscard]] auto read16(u32 addr) const -> u16;
    [[nodiscard]] auto read32(u32 addr) const -> u32;
    //@}
};

} // namespace saturnin::sound
