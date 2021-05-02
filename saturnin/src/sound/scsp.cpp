//
// scsp.cpp
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
#include <saturnin/src/sound/scsp.h>
#include <saturnin/src/memory.h>

namespace saturnin::sound {

using saturnin::core::rawRead;

auto Scsp::read8(const u32 addr) const -> u8 {
    auto local_addr = addr & 0x1fffff;
    if (local_addr < 0x80000) return rawRead<u32>(modules_.memory()->sound_ram_, local_addr);

    return 0;
}

auto Scsp::read16(const u32 addr) const -> u16 { return 0; }

auto Scsp::read32(const u32 addr) const -> u32 { return 0; }

void Scsp::write8(const u32 addr, const u8 data) {}

void Scsp::write16(const u32 addr, const u16 data) {}

void Scsp::write32(const u32 addr, const u32 data) {}

} // namespace saturnin::sound
