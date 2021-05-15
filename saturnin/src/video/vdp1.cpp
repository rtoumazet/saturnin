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

#include <saturnin/src/pch.h>
#define GLFW_INCLUDE_NONE
#include <windows.h> // removes C4005 warning
#include <istream>
#include <glbinding/gl21/gl.h>
#include <glbinding/gl21ext/gl.h>
#include <glbinding/glbinding.h>
#include <GLFW/glfw3.h>
#include <saturnin/src/emulator_context.h>
#include <saturnin/src/emulator_modules.h>
#include <saturnin/src/interrupt_sources.h>
#include <saturnin/src/memory.h>        // rawRead
#include <saturnin/src/scu_registers.h> // StartingFactorSelect
#include <saturnin/src/video/vdp1.h>
#include <saturnin/src/video/vdp1_part.h>
#include <saturnin/src/video/vdp2.h>

using namespace gl21;
using namespace gl21ext;

namespace saturnin::video {
namespace interrupt_source = core::interrupt_source;
using namespace vdp1_register_address;

using core::Config;
using core::EmulatorContext;
using core::Log;
using core::Logger;
using core::rawRead;
using core::Smpc;
using core::StartingFactorSelect;
using core::tr;

void Vdp1::initialize() {
    tvmr_.reset(); // Undefined after power on or reset.
    fbcr_.reset(); // Undefined after power on or reset.
    ptmr_.reset();
    ewdr_.reset(); // Undefined after power on or reset.
    endr_.reset(); // Write only
    edsr_.reset(); // Unknown at power on or reset.
    lopr_.reset(); // Unknown at power on or reset.
    copr_.reset(); // Unknown at power on or reset.
    constexpr auto copr_default_value = u16{1000};
    modr_                             = copr_default_value;
}

auto Vdp1::intializeFramebuffer() -> bool {
    // glGenTextures(2, &framebuffer_[0]);
    // glBindTexture(GL_TEXTURE_2D, texture);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    return true;
}

void Vdp1::run(const u8 cycles) {}

void Vdp1::onVblankIn() {
    updateResolution();

    switch (ptmr_.get(PlotTrigger::plot_trigger_mode)) {
        case PlotTriggerMode::idle_at_frame_change: {
            Log::debug(Logger::vdp1, tr("Idle at frame change"));
            break;
        }
        case PlotTriggerMode::starts_drawing_at_frame_change: {
            Log::debug(Logger::vdp1, tr("Starts drawing automatically at frame change"));
            populateRenderData();
            break;
        }
        default: // Nothing
            break;
    }
}

void Vdp1::updateResolution() { color_ram_address_offset_ = modules_.vdp2()->getSpriteColorAddressOffset(); };

void Vdp1::populateRenderData() {
    constexpr auto top_address     = u32{}; // Using relative address, is 0x25c00000 in the address map.
    auto           current_address = top_address;
    auto           return_address  = u32{};
    auto           cmdctrl         = CmdCtrl{rawRead<u16>(modules_.memory()->vdp1_vram_, current_address + cmdctrl_offset)};
    auto           cmdlink         = CmdLink{rawRead<u16>(modules_.memory()->vdp1_vram_, current_address + cmdlink_offset)};

    Vdp1Part::SetLocalCoordinates(0, 0); // Local coordinates are reset.

    while (cmdctrl.get(CmdCtrl::end_bit) == EndBit::command_selection_valid) {
        switch (cmdctrl.get(CmdCtrl::jump_select)) {
            case JumpSelect::jump_next: {
                Log::debug(Logger::vdp1, tr("Jump next"));
                current_address += table_size;
                break;
            }
            case JumpSelect::jump_assign: {
                Log::debug(Logger::vdp1, tr("Jump assign"));
                current_address = cmdlink.toU16() * address_multiplier;
                break;
            }
            case JumpSelect::jump_call: {
                Log::debug(Logger::vdp1, tr("Jump call"));
                current_address = cmdlink.toU16() * address_multiplier;
                return_address  = current_address + table_size;
                break;
            }
            case JumpSelect::jump_return: {
                Log::debug(Logger::vdp1, tr("Jump return"));
                current_address = return_address;
                return_address  = 0;
                break;
            }
            case JumpSelect::skip_next: {
                Log::debug(Logger::vdp1, tr("Skip next"));
                current_address += table_size;
                continue;
            }
            case JumpSelect::skip_assign: {
                Log::debug(Logger::vdp1, tr("Skip assign"));
                current_address = cmdlink.toU16() * address_multiplier;
                continue;
            }
            case JumpSelect::skip_call: {
                Log::debug(Logger::vdp1, tr("Skip call"));
                current_address = cmdlink.toU16() * address_multiplier;
                return_address  = current_address + table_size;
                continue;
            }
            case JumpSelect::skip_return: {
                Log::debug(Logger::vdp1, tr("Skip return"));
                current_address = return_address;
                return_address  = 0;
                continue;
            }
        }

        switch (cmdctrl.get(CmdCtrl::command_select)) {
            case CommandSelect::system_clipping: {
                Log::unimplemented(tr("Vdp1 command - System clipping coordinate set"));
                break;
            }
            case CommandSelect::user_clipping: {
                Log::unimplemented(tr("Vpd1 command - User clipping coordinate set"));
                break;
            }
            case CommandSelect::local_coordinate: {
                Log::debug(Logger::vdp1, tr("Command - Local coordinate set"));
                vdp1_parts_.emplace_back(Vdp1Part(modules_, current_address, cmdctrl, cmdlink));
                break;
            }
            case CommandSelect::normal_sprite_draw: {
                Log::debug(Logger::vdp1, tr("Command - Normal sprite draw"));
                vdp1_parts_.emplace_back(Vdp1Part(modules_, current_address, cmdctrl, cmdlink));
                break;
            }
            case CommandSelect::scaled_sprite_draw: {
                Log::debug(Logger::vdp1, tr("Command - Scaled sprite draw"));
                vdp1_parts_.emplace_back(Vdp1Part(modules_, current_address, cmdctrl, cmdlink));
                break;
            }
            case CommandSelect::distorted_sprite_draw: {
                Log::debug(Logger::vdp1, tr("Command - Distorted sprite draw"));
                vdp1_parts_.emplace_back(Vdp1Part(modules_, current_address, cmdctrl, cmdlink));
                break;
            }
            case CommandSelect::polygon_draw: {
                Log::debug(Logger::vdp1, tr("Command - Polygon draw"));
                vdp1_parts_.emplace_back(Vdp1Part(modules_, current_address, cmdctrl, cmdlink));
                break;
            }
            case CommandSelect::polyline_draw: {
                Log::debug(Logger::vdp1, tr("Command - Polyline draw"));
                vdp1_parts_.emplace_back(Vdp1Part(modules_, current_address, cmdctrl, cmdlink));
                break;
            }
            case CommandSelect::line_draw: {
                Log::debug(Logger::vdp1, tr("Command - Line draw"));
                vdp1_parts_.emplace_back(Vdp1Part(modules_, current_address, cmdctrl, cmdlink));
                break;
            }
            default: {
                Log::warning(Logger::vdp1, tr("Unknown command"));
            }
        }

        cmdctrl = CmdCtrl{rawRead<u16>(modules_.memory()->vdp1_vram_, current_address + cmdctrl_offset)};
        cmdlink = CmdLink{rawRead<u16>(modules_.memory()->vdp1_vram_, current_address + cmdlink_offset)};
    }

    Log::debug(Logger::vdp1, tr("-= Draw End command =-"));

    edsr_.set(TransferEndStatus::current_end_bit_fetch_status, CurrentEndBitFetchStatus::end_bit_fetched);
    edsr_.set(TransferEndStatus::before_end_bit_fetch_status, BeforeEndBitFetchStatus::end_bit_fetched); // Needs rework

    using namespace saturnin::core::interrupt_source;
    using saturnin::core::StartingFactorSelect;
    Log::debug(Logger::vdp1, tr("Interrupt request"));
    modules_.scu()->generateInterrupt(sprite_draw_end);
    modules_.scu()->sendStartFactor(StartingFactorSelect::sprite_draw_end);
}

auto Vdp1::read16(const u32 addr) const -> u16 {
    switch (addr) {
        case transfer_end_status: return edsr_.toU16();
        case last_operation_command_address: return lopr_.toU16();
        case current_operation_command_address: return copr_.toU16();
        case mode_status: return modr_.toU16();
        default: Log::warning(Logger::vdp1, core::tr("Unimplemented register read {:#010x}"), addr);
    }

    return 0;
}

void Vdp1::write16(const u32 addr, const u16 data) {
    switch (addr) {
        case tv_mode_selection: tvmr_.set(bits_0_15, data); break;
        case frame_buffer_change_mode: fbcr_.set(bits_0_15, data); break;
        case plot_trigger:
            ptmr_.set(bits_0_15, data);
            if (ptmr_.get(PlotTrigger::plot_trigger_mode) == PlotTriggerMode::starts_drawing_when_written) {
                // Drawing starts from the beginning of the table
                Log::debug(Logger::vdp1, tr("Drawing started at register write"));
                populateRenderData();
            }
            break;
        case erase_write_data: ewdr_.set(bits_0_15, data); break;
        case erase_write_upper_left_coordinate: ewlr_.set(bits_0_15, data); break;
        case erase_write_lower_right_coordinate: ewlr_.set(bits_0_15, data); break;
        case draw_forced_termination: endr_.set(bits_0_15, data); break;
        default: Log::warning(Logger::vdp1, core::tr("Unimplemented register write {:#010x}"), addr);
    }
}

} // namespace saturnin::video
