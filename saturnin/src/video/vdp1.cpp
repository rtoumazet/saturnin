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

#include "vdp1.h"
//#include "../config.h"
#include "../emulator_context.h"
#include "../log.h"
//#include "../smpc.h"
#include "../interrupt_sources.h"

namespace saturnin::video {
namespace interrupt_source = core::interrupt_source;
using namespace vdp1_register_address;

using core::Config;
using core::EmulatorContext;
using core::Log;
using core::Smpc;
using core::StartingFactorSelect;
using core::tr;

void Vdp1::initialize() {}

auto Vdp1::intializeFramebuffer() -> bool {}

void Vdp1::run(const u8 cycles) {}

auto Vdp1::read16(const u32 addr) const -> u16 {
    switch (addr) {
        case transfer_end_status: return edsr_.toU16();
        case last_operation_command_address: return lopr_.toU16();
        case current_operation_command_address: return copr_.toU16();
        case mode_status: return modr_.toU16();
        default: core::Log::warning("vdp1", core::tr("Unimplemented register read {:#010x}"), addr);
    }

    return 0;
}

void Vdp1::write16(const u32 addr, const u16 data) {
    switch (addr) {
        case tv_mode_selection: tvmr_.set(bits_0_15, data); break;
        case frame_buffer_change_mode: fbcr_.set(bits_0_15, data); break;
        case plot_trigger: ptmr_.set(bits_0_15, data); break;
        case erase_write_data: ewdr_.set(bits_0_15, data); break;
        case erase_write_upper_left_coordinate: ewlr_.set(bits_0_15, data); break;
        case erase_write_lower_right_coordinate: ewlr_.set(bits_0_15, data); break;
        case draw_forced_termination: endr_.set(bits_0_15, data); break;
        default: core::Log::warning("vdp1", core::tr("Unimplemented register write {:#010x}"), addr);
    }
}

} // namespace saturnin::video
