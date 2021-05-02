//
// emulator_modules.cpp
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

#include <saturnin/src/pch.h>
#include <saturnin/src/emulator_modules.h>

namespace saturnin::core {

auto EmulatorModules::context() const -> EmulatorContext* { return context_; };
auto EmulatorModules::config() const -> Config* { return context_->config(); };
auto EmulatorModules::memory() const -> Memory* { return context_->memory(); };
auto EmulatorModules::masterSh2() const -> sh2::Sh2* { return context_->masterSh2(); };
auto EmulatorModules::slaveSh2() const -> sh2::Sh2* { return context_->slaveSh2(); };
auto EmulatorModules::scu() const -> Scu* { return context_->scu(); };
auto EmulatorModules::smpc() const -> Smpc* { return context_->smpc(); };
auto EmulatorModules::scsp() const -> sound::Scsp* { return context_->scsp(); };
auto EmulatorModules::cdrom() const -> cdrom::Cdrom* { return context_->cdrom(); };
auto EmulatorModules::vdp1() const -> video::Vdp1* { return context_->vdp1(); };
auto EmulatorModules::vdp2() const -> video::Vdp2* { return context_->vdp2(); };
auto EmulatorModules::opengl() const -> video::Opengl* { return context_->opengl(); };

} // namespace saturnin::core
