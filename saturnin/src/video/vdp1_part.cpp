//
// vdp1_part.cpp
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
#include <saturnin/src/video/vdp1_part.h>
#include <saturnin/src/video/texture.h>
#include <saturnin/src/video/vdp1.h>
#include <saturnin/src/emulator_modules.h>
#include <saturnin/src/memory.h>
#include <saturnin/src/utilities.h>

namespace saturnin::video {

using core::EmulatorModules;
using core::rawRead;
using core::rawWrite;
using utilities::toUnderlying;

constexpr auto vdp1_vram_start_address = u32{0x25C00000};

s16 Vdp1Part::local_coordinate_x_;
s16 Vdp1Part::local_coordinate_y_;

Vdp1Part::Vdp1Part(EmulatorModules& modules, const u32 table_address, const CmdCtrl& cmdctrl, const CmdLink& cmdlink) {
    setVdpType(VdpType::vdp1);
    cmdctrl_ = std::move(cmdctrl);
    cmdlink_ = std::move(cmdlink);
    readParameters(modules.memory(), table_address);
    generatePartData(modules);
};

void Vdp1Part::readParameters(Memory* m, const u32 address) {
    auto local_address = address;
    switch (cmdctrl_.get(CmdCtrl::command_select)) {
        case CommandSelect::system_clipping: {
            cmdxc_ = m->read<u16>(address + cmdxc_offset);
            cmdyc_ = m->read<u16>(address + cmdyc_offset);
            break;
        }
        case CommandSelect::user_clipping: {
            cmdxa_ = m->read<u16>(address + cmdxa_offset);
            cmdya_ = m->read<u16>(address + cmdya_offset);
            cmdxc_ = m->read<u16>(address + cmdxc_offset);
            cmdyc_ = m->read<u16>(address + cmdyc_offset);
            break;
        }
        case CommandSelect::local_coordinate: {
            cmdxa_ = m->read<u16>(address + cmdxa_offset);
            cmdya_ = m->read<u16>(address + cmdya_offset);
            SetLocalCoordinates(cmdxa_.toU16(), cmdya_.toU16());
            break;
        }
        case CommandSelect::normal_sprite_draw: {
            cmdpmod_ = m->read<u16>(address + cmdpmod_offset);
            cmdcolr_ = m->read<u16>(address + cmdcolr_offset);
            cmdsrca_ = m->read<u16>(address + cmdsrca_offset);
            cmdsize_ = m->read<u16>(address + cmdsize_offset);
            cmdxa_   = m->read<u16>(address + cmdxa_offset);
            cmdya_   = m->read<u16>(address + cmdya_offset);
            cmdgrda_ = m->read<u16>(address + cmdgrda_offset);
            break;
        }
        case CommandSelect::scaled_sprite_draw: {
            cmdpmod_ = m->read<u16>(address + cmdpmod_offset);
            cmdcolr_ = m->read<u16>(address + cmdcolr_offset);
            cmdsrca_ = m->read<u16>(address + cmdsrca_offset);
            cmdsize_ = m->read<u16>(address + cmdsize_offset);
            cmdxa_   = m->read<u16>(address + cmdxa_offset);
            cmdya_   = m->read<u16>(address + cmdya_offset);
            cmdxb_   = m->read<u16>(address + cmdxb_offset);
            cmdyb_   = m->read<u16>(address + cmdyb_offset);
            cmdxc_   = m->read<u16>(address + cmdxc_offset);
            cmdyc_   = m->read<u16>(address + cmdyc_offset);
            cmdgrda_ = m->read<u16>(address + cmdgrda_offset);
            break;
        }
        case CommandSelect::distorted_sprite_draw:
        case CommandSelect::polygon_draw:
        case CommandSelect::polyline_draw: {
            cmdpmod_ = m->read<u16>(address + cmdpmod_offset);
            cmdcolr_ = m->read<u16>(address + cmdcolr_offset);
            cmdsrca_ = m->read<u16>(address + cmdsrca_offset);
            cmdsize_ = m->read<u16>(address + cmdsize_offset);
            cmdxa_   = m->read<u16>(address + cmdxa_offset);
            cmdya_   = m->read<u16>(address + cmdya_offset);
            cmdxb_   = m->read<u16>(address + cmdxb_offset);
            cmdyb_   = m->read<u16>(address + cmdyb_offset);
            cmdxc_   = m->read<u16>(address + cmdxc_offset);
            cmdyc_   = m->read<u16>(address + cmdyc_offset);
            cmdxd_   = m->read<u16>(address + cmdxd_offset);
            cmdyd_   = m->read<u16>(address + cmdyd_offset);
            cmdgrda_ = m->read<u16>(address + cmdgrda_offset);
            break;
        }
        case CommandSelect::line_draw: {
            cmdpmod_ = m->read<u16>(address + cmdpmod_offset);
            cmdcolr_ = m->read<u16>(address + cmdcolr_offset);
            cmdxa_   = m->read<u16>(address + cmdxa_offset);
            cmdya_   = m->read<u16>(address + cmdya_offset);
            cmdxb_   = m->read<u16>(address + cmdxb_offset);
            cmdyb_   = m->read<u16>(address + cmdyb_offset);
            cmdgrda_ = m->read<u16>(address + cmdgrda_offset);
            break;
        }
    }
}

void Vdp1Part::generatePartData(const EmulatorModules& modules) {
    switch (cmdctrl_.get(CmdCtrl::command_select)) {
        case CommandSelect::system_clipping: {
            // Not implemented
            break;
        }
        case CommandSelect::user_clipping: {
            // Not implemented
            break;
        }
        case CommandSelect::local_coordinate: {
            SetLocalCoordinates(cmdxa_.toU16(), cmdya_.toU16());
            break;
        }
        case CommandSelect::normal_sprite_draw: {
            normalSpriteDraw(modules, *this);
            break;
        }
        case CommandSelect::scaled_sprite_draw: {
            break;
        }
        case CommandSelect::distorted_sprite_draw: {
            break;
        }
        case CommandSelect::polygon_draw: {
            break;
        }
        case CommandSelect::polyline_draw: {
            break;
        }
        case CommandSelect::line_draw: {
            break;
        }
    }
}

void Vdp1Part::SetLocalCoordinates(const u16 x, const u16 y) {
    Vdp1Part::local_coordinate_x_ = x;
    Vdp1Part::local_coordinate_y_ = y;
}
void normalSpriteDraw(const EmulatorModules& modules, Vdp1Part& part) { loadTextureData(modules, part); }

void loadTextureData(const EmulatorModules& modules, Vdp1Part& part) {
    const auto      color_ram_address_offset = modules.vdp1()->getColorRamAddressOffset();
    auto            start_address            = vdp1_vram_start_address + part.cmdsrca_.toU16() * address_multiplier;
    const auto      texture_width            = part.cmdsize_.get(CmdSize::character_size_x);
    const auto      texture_height           = part.cmdsize_.get(CmdSize::character_size_y);
    const auto      color_mode               = part.cmdpmod_.get(CmdPmod::color_mode);
    const auto      texture_size             = static_cast<u32>(texture_width * texture_height * 4);
    std::vector<u8> texture_data;
    texture_data.reserve(texture_size);
    const auto key = Texture::calculateKey(VdpType::vdp1, start_address, toUnderlying(color_mode));
    if (!Texture::isTextureStored(key)) {
        if (modules.vdp2()->getColorRamMode() == ColorRamMode::mode_2_rgb_8_bits_1024_colors) {
            // 32 bits access to color RAM
            switch (color_mode) {
                case ColorMode::mode_0_16_colors_bank: {
                    readColorBankMode16Colors<u32>(modules, texture_data, start_address, color_ram_address_offset, part);
                    break;
                }
                case ColorMode::mode_1_16_colors_lookup: {
                    // LUT doesn't depend on color RAM, it's always 16 bits.
                    readLookUpTable16Colors<u16>(modules, texture_data, start_address, part);
                    break;
                }
                case ColorMode::mode_2_64_colors_bank: {
                    readColorBankMode64Colors<u32>(modules, texture_data, start_address, color_ram_address_offset, part);
                    break;
                }
                case ColorMode::mode_3_128_colors_bank: {
                    readColorBankMode128Colors<u32>(modules, texture_data, start_address, color_ram_address_offset, part);
                    break;
                }
                case ColorMode::mode_4_256_colors_bank: {
                    readColorBankMode256Colors<u32>(modules, texture_data, start_address, color_ram_address_offset, part);
                    break;
                }
                case ColorMode::mode_5_32k_colors_rgb: {
                    // RGB is always 16 bits.
                    readRgb32KColors<u16>(modules, texture_data, start_address, part);
                    break;
                }
            }
        } else {
            // 16 bits access to color RAM
            switch (color_mode) {
                case ColorMode::mode_0_16_colors_bank: {
                    readColorBankMode16Colors<u16>(modules, texture_data, start_address, color_ram_address_offset, part);
                    break;
                }
                case ColorMode::mode_1_16_colors_lookup: {
                    readLookUpTable16Colors<u16>(modules, texture_data, start_address, part);
                    break;
                }
                case ColorMode::mode_2_64_colors_bank: {
                    readColorBankMode64Colors<u16>(modules, texture_data, start_address, color_ram_address_offset, part);
                    break;
                }
                case ColorMode::mode_3_128_colors_bank: {
                    readColorBankMode128Colors<u16>(modules, texture_data, start_address, color_ram_address_offset, part);
                    break;
                }
                case ColorMode::mode_4_256_colors_bank: {
                    readColorBankMode256Colors<u16>(modules, texture_data, start_address, color_ram_address_offset, part);
                    break;
                }
                case ColorMode::mode_5_32k_colors_rgb: {
                    readRgb32KColors<u16>(modules, texture_data, start_address, part);
                    break;
                }
            }
        }
    }
}

} // namespace saturnin::video
