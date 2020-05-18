//
// vdp2.cpp
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

#include "vdp2.h"

namespace saturnin::video {

void Vdp2::initialize() {}

void Vdp2::run(const u8 cycles) {}

void Vdp2::write8(const u32 addr, const u8 data) {}
void Vdp2::write16(const u32 addr, const u16 data) {}
void Vdp2::write32(const u32 addr, const u32 data) {}
auto Vdp2::read8(const u32 addr) const -> u8 { return 0; }
auto Vdp2::read16(const u32 addr) const -> u16 { return 0; }
auto Vdp2::read32(const u32 addr) const -> u32 { return 0; }

} // namespace saturnin::video
