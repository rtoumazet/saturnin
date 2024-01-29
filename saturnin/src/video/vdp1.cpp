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
#include <Windows.h> // removes C4005 warning
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
#include <saturnin/src/video/texture.h>
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
using core::tr;

void Vdp1::initialize() {
    regs_.tvmr                        = {}; // Undefined after power on or reset.
    regs_.fbcr                        = {}; // Undefined after power on or reset.
    regs_.ptmr                        = {};
    regs_.ewdr                        = {}; // Undefined after power on or reset.
    regs_.endr                        = {}; // Write only
    regs_.edsr                        = {}; // Unknown at power on or reset.
    regs_.lopr                        = {}; // Unknown at power on or reset.
    regs_.copr                        = {}; // Unknown at power on or reset.
    constexpr auto copr_default_value = u16{1000};
    regs_.modr                        = copr_default_value;
}

auto Vdp1::intializeFramebuffer() -> bool {
    // glGenTextures(2, &framebuffer_[0]);
    // glBindTexture(GL_TEXTURE_2D, texture);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    return true;
}

void Vdp1::onVblankIn() {
    using Ptmr = Vdp1Regs::Ptmr;

    Texture::cleanCache(modules_.opengl(), VdpType::vdp1);
    Texture::setCache(VdpType::vdp1);
    updateResolution();

    switch (regs_.ptmr >> Ptmr::ptm_enum) {
        using enum Ptmr::PlotTriggerMode;
        case idle_at_frame_change: {
            Log::debug(Logger::vdp1, tr("Idle at frame change"));
            break;
        }
        case starts_drawing_at_frame_change: {
            Log::debug(Logger::vdp1, tr("Starts drawing automatically at frame change"));
            populateRenderData();
            break;
        }
        default: // Nothing
            break;
    }
}

auto Vdp1::getDebugDrawList() const -> std::vector<std::string> {
    std::vector<std::string> debug_draw_list;
    for (const auto& p : vdp1_parts_) {
        debug_draw_list.emplace_back(p.debugHeader());
    }

    return debug_draw_list;
}

void Vdp1::updateResolution() { color_ram_address_offset_ = modules_.vdp2()->getSpriteColorAddressOffset(); };

void Vdp1::populateRenderData() {
    auto current_table_address = vdp1_ram_start_address;
    auto next_table_address    = current_table_address;
    auto return_address        = u32{};
    auto cmdctrl               = CmdCtrlType{modules_.memory()->read<u16>(current_table_address + cmdctrl_offset)};
    auto cmdlink               = CmdLinkType{modules_.memory()->read<u16>(current_table_address + cmdlink_offset)};
    vdp1_parts_.clear();
    // Texture::discardCache(modules_.opengl(), VdpType::vdp1);

    color_offset_ = modules_.vdp2()->getColorOffset(Layer::sprite);

    while ((cmdctrl >> CmdCtrl::end_enum) == CmdCtrl::EndBit::command_selection_valid) {
        auto skip_table = false;
        switch (cmdctrl >> CmdCtrl::js_enum) {
            using enum CmdCtrl::JumpSelect;
            case jump_next: {
                Log::debug(Logger::vdp1, tr("Jump next"));
                next_table_address += table_size;
                break;
            }
            case jump_assign: {
                Log::debug(Logger::vdp1, tr("Jump assign"));
                next_table_address = vdp1_ram_start_address + cmdlink.data() * vdp1_address_multiplier;
                break;
            }
            case jump_call: {
                Log::debug(Logger::vdp1, tr("Jump call"));
                next_table_address = vdp1_ram_start_address + cmdlink.data() * vdp1_address_multiplier;
                return_address     = current_table_address + table_size;
                break;
            }
            case jump_return: {
                Log::debug(Logger::vdp1, tr("Jump return"));
                next_table_address = return_address;
                return_address     = 0;
                break;
            }
            case skip_next: {
                Log::debug(Logger::vdp1, tr("Skip next"));
                next_table_address += table_size;
                skip_table = true;
                break;
            }
            case skip_assign: {
                Log::debug(Logger::vdp1, tr("Skip assign"));
                next_table_address = vdp1_ram_start_address + cmdlink.data() * vdp1_address_multiplier;
                skip_table         = true;
                break;
            }
            case skip_call: {
                Log::debug(Logger::vdp1, tr("Skip call"));
                next_table_address = vdp1_ram_start_address + cmdlink.data() * vdp1_address_multiplier;
                return_address     = current_table_address + table_size;
                skip_table         = true;
                break;
            }
            case skip_return: {
                Log::debug(Logger::vdp1, tr("Skip return"));
                next_table_address = return_address;
                return_address     = 0;
                skip_table         = true;
                break;
            }
        }

        if (!skip_table) {
            switch (cmdctrl >> CmdCtrl::comm_enum) {
                using enum CmdCtrl::CommandSelect;
                case system_clipping: {
                    Log::unimplemented(tr("VDP1 command - System clipping coordinate set"));
                    break;
                }
                case user_clipping: {
                    Log::unimplemented(tr("VDP1 command - User clipping coordinate set"));
                    break;
                }
                case local_coordinate: {
                    vdp1_parts_
                        .emplace_back(modules_, DrawType::not_drawable, current_table_address, cmdctrl, cmdlink, color_offset_);
                    break;
                }
                case normal_sprite_draw: {
                    vdp1_parts_.emplace_back(modules_,
                                             DrawType::textured_polygon,
                                             current_table_address,
                                             cmdctrl,
                                             cmdlink,
                                             color_offset_);
                    break;
                }
                case scaled_sprite_draw: {
                    vdp1_parts_.emplace_back(modules_,
                                             DrawType::textured_polygon,
                                             current_table_address,
                                             cmdctrl,
                                             cmdlink,
                                             color_offset_);
                    break;
                }
                case distorted_sprite_draw: {
                    vdp1_parts_.emplace_back(modules_,
                                             DrawType::textured_polygon,
                                             current_table_address,
                                             cmdctrl,
                                             cmdlink,
                                             color_offset_);
                    break;
                }
                case polygon_draw: {
                    vdp1_parts_.emplace_back(modules_,
                                             DrawType::non_textured_polygon,
                                             current_table_address,
                                             cmdctrl,
                                             cmdlink,
                                             color_offset_);
                    break;
                }
                case polyline_draw: {
                    vdp1_parts_
                        .emplace_back(modules_, DrawType::polyline, current_table_address, cmdctrl, cmdlink, color_offset_);
                    break;
                }
                case line_draw: {
                    vdp1_parts_.emplace_back(modules_, DrawType::line, current_table_address, cmdctrl, cmdlink, color_offset_);
                    break;
                }
                default: {
                    Log::warning(Logger::vdp1, tr("Unknown command"));
                }
            }
        }
        cmdctrl               = modules_.memory()->read<u16>(next_table_address + cmdctrl_offset);
        cmdlink               = CmdLinkType{modules_.memory()->read<u16>(next_table_address + cmdlink_offset)};
        current_table_address = next_table_address;
    }

    Log::debug(Logger::vdp1, tr("-= Draw End command =-"));

    using Edsr = Vdp1Regs::Edsr;
    regs_.edsr.upd(Edsr::cef_enum, Edsr::CurrentEndBitFetchStatus::end_bit_fetched);
    regs_.edsr.upd(Edsr::bef_enum, Edsr::BeforeEndBitFetchStatus::end_bit_fetched); // Needs rework

    using namespace saturnin::core::interrupt_source;
    Log::debug(Logger::vdp1, tr("Interrupt request"));
    modules_.scu()->generateInterrupt(sprite_draw_end);
    modules_.scu()->sendStartFactor(core::ScuRegs::Dxmd::StartingFactorSelect::sprite_draw_end);
}

auto Vdp1::read16(const u32 addr) const -> u16 {
    switch (addr) {
        case transfer_end_status: return regs_.edsr.data();
        case last_operation_command_address: return regs_.lopr.data();
        case current_operation_command_address: return regs_.copr.data();
        case mode_status: return regs_.modr.data();
        default: Log::warning(Logger::vdp1, core::tr("Unimplemented register read {:#010x}"), addr);
    }

    return 0;
}

void Vdp1::write16(const u32 addr, const u16 data) {
    switch (addr) {
        case tv_mode_selection: regs_.tvmr = data; break;
        case frame_buffer_change_mode: regs_.fbcr = data; break;
        case plot_trigger:
            using Ptmr = Vdp1Regs::Ptmr;
            regs_.ptmr = data;
            if ((regs_.ptmr >> Ptmr::ptm_enum) == Ptmr::PlotTriggerMode::starts_drawing_when_written) {
                // Drawing starts from the beginning of the table
                Log::debug(Logger::vdp1, tr("Drawing started at register write"));
                populateRenderData();
            }
            break;
        case erase_write_data: regs_.ewdr = data; break;
        case erase_write_upper_left_coordinate: regs_.ewlr = data; break;
        case erase_write_lower_right_coordinate: regs_.ewrr = data; break;
        case draw_forced_termination: regs_.endr = data; break;
        default: Log::warning(Logger::vdp1, core::tr("Unimplemented register write {:#010x}"), addr);
    }
}

} // namespace saturnin::video
