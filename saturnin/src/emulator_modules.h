//
// emulator_modules.h
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
/// \file	emulator_modules.h
///
/// \brief	Declares accessors to the internal components of the emulator.
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <saturnin/src/emulator_defs.h>
#include <saturnin/src/emulator_context.h>

namespace saturnin::core {

class EmulatorModules {
  public:
    ///@{
    /// Constructors / Destructors
    EmulatorModules() = delete;
    EmulatorModules(EmulatorContext* ec) : context_(ec){};
    EmulatorModules(const EmulatorModules&) = delete;
    EmulatorModules(EmulatorModules&&)      = delete;
    auto operator=(const EmulatorModules&) & -> EmulatorModules& = delete;
    auto operator=(EmulatorModules&&) & -> EmulatorModules& = delete;
    ~EmulatorModules()                                      = default;
    ///@}

    ///@{
    /// \name Internal modules accessors
    [[nodiscard]] auto context() const -> core::EmulatorContext*;
    [[nodiscard]] auto config() const -> core::Config*;
    [[nodiscard]] auto memory() const -> core::Memory*;
    [[nodiscard]] auto masterSh2() const -> sh2::Sh2*;
    [[nodiscard]] auto slaveSh2() const -> sh2::Sh2*;
    [[nodiscard]] auto scu() const -> core::Scu*;
    [[nodiscard]] auto smpc() const -> core::Smpc*;
    [[nodiscard]] auto scsp() const -> sound::Scsp*;
    [[nodiscard]] auto cdrom() const -> cdrom::Cdrom*;
    [[nodiscard]] auto vdp1() const -> video::Vdp1*;
    [[nodiscard]] auto vdp2() const -> video::Vdp2*;
    [[nodiscard]] auto opengl() const -> video::Opengl*;
    ///@}

  private:
    EmulatorContext* context_;
};
} // namespace saturnin::core
