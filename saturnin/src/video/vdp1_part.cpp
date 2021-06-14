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
#include <saturnin/src/locale.h> // tr
#include <saturnin/src/memory.h>
#include <saturnin/src/utilities.h>

namespace saturnin::video {

using core::EmulatorModules;
using core::rawRead;
using core::rawWrite;
using core::tr;
using utilities::toUnderlying;

constexpr auto vdp1_vram_start_address = u32{0x25C00000};

s16 Vdp1Part::local_coordinate_x_;
s16 Vdp1Part::local_coordinate_y_;

Vdp1Part::Vdp1Part(EmulatorModules& modules,
                   const DrawType   type,
                   const u32        table_address,
                   const CmdCtrl&   cmdctrl,
                   const CmdLink&   cmdlink) {
    vdpType(VdpType::vdp1);
    drawType(type);
    cmdctrl_ = std::move(cmdctrl);
    cmdlink_ = std::move(cmdlink);
    readParameters(modules.memory(), table_address);
    generatePartData(modules);
};

void Vdp1Part::readParameters(Memory* m, const u32 address) {
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
            debug_header_ = tr("Set system clipping coordinates");
            break;
        }
        case CommandSelect::user_clipping: {
            // Not implemented
            debug_header_ = tr("Set user clipping coordinates");
            break;
        }
        case CommandSelect::local_coordinate: {
            SetLocalCoordinates(cmdxa_.twoCmp(), cmdya_.twoCmp());
            debug_header_ = tr("Set local coordinates");
            break;
        }
        case CommandSelect::normal_sprite_draw: {
            normalSpriteDraw(modules, *this);
            debug_header_ = tr("Normal sprite draw");
            break;
        }
        case CommandSelect::scaled_sprite_draw: {
            debug_header_ = tr("Scaled sprite draw");
            break;
        }
        case CommandSelect::distorted_sprite_draw: {
            distortedSpriteDraw(modules, *this);
            debug_header_ = tr("Distorted sprite draw");
            break;
        }
        case CommandSelect::polygon_draw: {
            polygonDraw(modules, *this);
            debug_header_ = tr("Polygon draw");
            break;
        }
        case CommandSelect::polyline_draw: {
            debug_header_ = tr("Polyline draw");
            break;
        }
        case CommandSelect::line_draw: {
            debug_header_ = tr("Line draw");
            break;
        }
    }
}

void Vdp1Part::SetLocalCoordinates(const s16 x, const s16 y) {
    Log::debug(Logger::vdp1, tr("Command - Local coordinate set"));
    Log::debug(Logger::vdp1, tr("Local coordinates are now ({},{})"), x, y);
    Vdp1Part::local_coordinate_x_ = x;
    Vdp1Part::local_coordinate_y_ = y;
}

auto Vdp1Part::getDebugDetail() -> std::string {
    auto part_detail = std::string{};

    const auto getZoomPoint = [](const ZoomPoint zp) {
        switch (zp) {
            case ZoomPoint::center_center:
                return R"(
Zoom point
    Center-center)";
            case ZoomPoint::center_left:
                return R"(
Zoom point
    Center-left)";
            case ZoomPoint::center_right:
                return R"(
Zoom point
    Center-right)";
            case ZoomPoint::lower_center:
                return R"(
Zoom point 
    Lower-center)";
            case ZoomPoint::lower_left:
                return R"(
Zoom point
    Lower-left)";
            case ZoomPoint::lower_right:
                return R"(
Zoom point
    Lower-right)";
            case ZoomPoint::upper_center:
                return R"(
Zoom point
    Upper-center)";
            case ZoomPoint::upper_left:
                return R"(
Zoom point
    Upper-left)";
            case ZoomPoint::upper_right:
                return R"(
Zoom point
    Upper-right)";
            case ZoomPoint::two_coordinates:
                return R"(
Zoom point
    Specifies two coordinates)";
            default:
                return R"(
Zoom point
    Setting prohibited)";
        }
    };

    const auto getCharacterReadDirection = [](const CharacterReadDirection crd) {
        switch (crd) {
            case CharacterReadDirection::h_invertion:
                return R"(
Character read direction 
    Horizontally inverted)";
            case CharacterReadDirection::not_inverted:
                return R"(
Character read direction
    Not inverted)";
            case CharacterReadDirection::vh_invertion:
                return R"(
Character read direction 
    Horizontally and vertically inverted)";
            case CharacterReadDirection::v_invertion:
                return R"(
Character read direction
    Vertically inverted)";
        }
    };

    const auto getColorMode = [](const ColorMode cm) {
        switch (cm) {
            case ColorMode::mode_0_16_colors_bank:
                return R"(
Color mode 
    Mode 0
    16 colors
    Color bank
    4 bits/pixel)";
            case ColorMode::mode_1_16_colors_lookup:
                return R"(
Color mode
    Mode 1
    16 colors
    Lookup table
    4 bits / pixel)";
            case ColorMode::mode_2_64_colors_bank:
                return R"(
Color mode 
    Mode 2
    64 colors
    Color bank
    8 bits/pixel)";
            case ColorMode::mode_3_128_colors_bank:
                return R"(
Color mode 
    Mode 3
    128 colors
    Color bank
    8 bits/pixel)";
            case ColorMode::mode_4_256_colors_bank:
                return R"(
Color mode 
    Mode 4
    256 colors
    Color bank
    8 bits/pixel)";
            case ColorMode::mode_5_32k_colors_rgb:
                return R"(
Color mode 
    Mode 5
    32K colors
    RGB
    16 bits/pixel)";
        }
    };

    const auto getColorCalculation = [](const ColorCalculation cc) {
        switch (cc) {
            case ColorCalculation::mode_0:
                return R"(
Color calculation
    Mode 0, 
    Replace)";
            case ColorCalculation::mode_1:
                return R"(
Color calculation 
    Mode 1
    Cannot rewrite / shadow)";
            case ColorCalculation::mode_2:
                return R"(
Color calculation 
    Mode 2
    Half-luminance)";
            case ColorCalculation::mode_3:
                return R"(
Color calculation 
    Mode 3
    Replace / half-transparent)";
            case ColorCalculation::mode_4:
                return R"(
Color calculation 
    Mode 4
    Gouraud shading)";
            case ColorCalculation::mode_5:
                return R"(
Color calculation 
    Mode 5
    Setting prohibited)";
            case ColorCalculation::mode_6:
                return R"(
Color calculation 
    Mode 6
    Gouraud shading + half-luminance)";
            case ColorCalculation::mode_7:
                return R"(
Color calculation 
    Mode 7
    Gouraud shading / gouraud shading + half-tranparent)";
        }
    };

    const auto getDrawMode = [&getColorMode, &getColorCalculation](CmdPmod& cp) {
        auto s = std::string{};

        s += "MSB ";
        s += (cp.get(CmdPmod::msb_on) == MsbOn::on) ? "on" : "off";
        s += "\nHigh speed shrink ";
        s += (cp.get(CmdPmod::high_speed_shrink) == HighSpeedShrink::enabled) ? "enabled" : "disabled";
        s += "\nPre-clipping ";
        s += (cp.get(CmdPmod::pre_clipping_disable) == PreClippingDisable::pre_clipping) ? "enabled" : "disabled";
        s += "\nUser clipping ";
        if (cp.get(CmdPmod::user_clipping_enable) == UserClippingEnable::enabled) {
            s += "enabled";
            s += "\nUser clipping ";
            s += (cp.get(CmdPmod::user_clipping_mode) == UserClippingMode::drawing_inside) ? "drawing inside" : "drawing outside";
        } else {
            s += "ignored";
        }
        s += "\n";
        s += (cp.get(CmdPmod::mesh_enable) == MeshEnable::enabled) ? "Draw with mesh processing" : "Draw without mesh processing";
        s += "\nEnd code ";
        s += (cp.get(CmdPmod::end_code_disable) == EndCodeDisable::enabled) ? "enabled" : "disabled";
        s += "\nTransparent pixel ";
        s += (cp.get(CmdPmod::transparent_pixel_disable) == TransparentPixelDisable::transparent_pixel_enabled) ? "enabled"
                                                                                                                : "disabled";
        s += getColorMode(cp.get(CmdPmod::color_mode));
        s += getColorCalculation(cp.get(CmdPmod::color_calculation));

        return s;
    };

    const auto getGouraudShadingTableAddress = [&]() {
        if (cmdpmod_.get(CmdPmod::gouraud_shading) == GouraudShading::enabled) {
            constexpr auto addr_disp = u8{3};
            return fmt::format(R"(
Gouraud shading table address 
    {:#x})",
                               vdp1_ram_start_address + cmdgrda_.toU16() * addr_disp);
        }
        return std::string{};
    };

    switch (cmdctrl_.get(CmdCtrl::command_select)) {
        case CommandSelect::system_clipping: {
            // cmdxc_ = m->read<u16>(address + cmdxc_offset);
            // cmdyc_ = m->read<u16>(address + cmdyc_offset);
            break;
        }
        case CommandSelect::user_clipping: {
            // cmdxa_ = m->read<u16>(address + cmdxa_offset);
            // cmdya_ = m->read<u16>(address + cmdya_offset);
            // cmdxc_ = m->read<u16>(address + cmdxc_offset);
            // cmdyc_ = m->read<u16>(address + cmdyc_offset);
            break;
        }
        case CommandSelect::local_coordinate: {
            part_detail += fmt::format("x = {}, y = {}\n", cmdxa_.twoCmp(), cmdya_.twoCmp());
            break;
        }
        case CommandSelect::normal_sprite_draw: {
            // cmdpmod_ = m->read<u16>(address + cmdpmod_offset);
            // cmdcolr_ = m->read<u16>(address + cmdcolr_offset);
            // cmdsrca_ = m->read<u16>(address + cmdsrca_offset);
            // cmdsize_ = m->read<u16>(address + cmdsize_offset);
            // cmdxa_   = m->read<u16>(address + cmdxa_offset);
            // cmdya_   = m->read<u16>(address + cmdya_offset);
            // cmdgrda_ = m->read<u16>(address + cmdgrda_offset);
            break;
        }
        case CommandSelect::scaled_sprite_draw: {
            part_detail += getDrawMode(cmdpmod_);
            // cmdpmod_ = m->read<u16>(address + cmdpmod_offset);
            // cmdcolr_ = m->read<u16>(address + cmdcolr_offset);
            // cmdsrca_ = m->read<u16>(address + cmdsrca_offset);
            // cmdsize_ = m->read<u16>(address + cmdsize_offset);
            // cmdxa_   = m->read<u16>(address + cmdxa_offset);
            // cmdya_   = m->read<u16>(address + cmdya_offset);
            // cmdxb_   = m->read<u16>(address + cmdxb_offset);
            // cmdyb_   = m->read<u16>(address + cmdyb_offset);
            // cmdxc_   = m->read<u16>(address + cmdxc_offset);
            // cmdyc_   = m->read<u16>(address + cmdyc_offset);
            // cmdgrda_ = m->read<u16>(address + cmdgrda_offset);
            break;
        }
        case CommandSelect::distorted_sprite_draw: {
            part_detail += fmt::format("Vertex A ({}, {})\n", cmdxa_.twoCmp(), cmdya_.twoCmp());
            part_detail += fmt::format("Vertex B ({}, {})\n", cmdxb_.twoCmp(), cmdyb_.twoCmp());
            part_detail += fmt::format("Vertex C ({}, {})\n", cmdxc_.twoCmp(), cmdyc_.twoCmp());
            part_detail += fmt::format("Vertex D ({}, {})\n", cmdxd_.twoCmp(), cmdyd_.twoCmp());
            part_detail += getDrawMode(cmdpmod_);
            part_detail += getGouraudShadingTableAddress();
            break;
        }
        case CommandSelect::polygon_draw: {
            part_detail += fmt::format("Vertex A ({}, {})\n", cmdxa_.twoCmp(), cmdya_.twoCmp());
            part_detail += fmt::format("Vertex B ({}, {})\n", cmdxb_.twoCmp(), cmdyb_.twoCmp());
            part_detail += fmt::format("Vertex C ({}, {})\n", cmdxc_.twoCmp(), cmdyc_.twoCmp());
            part_detail += fmt::format("Vertex D ({}, {})\n", cmdxd_.twoCmp(), cmdyd_.twoCmp());
            part_detail += getGouraudShadingTableAddress();
            break;
        }
        case CommandSelect::polyline_draw: {
            // cmdpmod_ = m->read<u16>(address + cmdpmod_offset);
            // cmdcolr_ = m->read<u16>(address + cmdcolr_offset);
            // cmdsrca_ = m->read<u16>(address + cmdsrca_offset);
            // cmdsize_ = m->read<u16>(address + cmdsize_offset);
            // cmdxa_   = m->read<u16>(address + cmdxa_offset);
            // cmdya_   = m->read<u16>(address + cmdya_offset);
            // cmdxb_   = m->read<u16>(address + cmdxb_offset);
            // cmdyb_   = m->read<u16>(address + cmdyb_offset);
            // cmdxc_   = m->read<u16>(address + cmdxc_offset);
            // cmdyc_   = m->read<u16>(address + cmdyc_offset);
            // cmdxd_   = m->read<u16>(address + cmdxd_offset);
            // cmdyd_   = m->read<u16>(address + cmdyd_offset);
            // cmdgrda_ = m->read<u16>(address + cmdgrda_offset);
            break;
        }
        case CommandSelect::line_draw: {
            // cmdpmod_ = m->read<u16>(address + cmdpmod_offset);
            // cmdcolr_ = m->read<u16>(address + cmdcolr_offset);
            // cmdxa_   = m->read<u16>(address + cmdxa_offset);
            // cmdya_   = m->read<u16>(address + cmdya_offset);
            // cmdxb_   = m->read<u16>(address + cmdxb_offset);
            // cmdyb_   = m->read<u16>(address + cmdyb_offset);
            // cmdgrda_ = m->read<u16>(address + cmdgrda_offset);
            break;
        }
    }

    return part_detail;
}

auto Vdp1Part::calculatedXA() -> const s16 { return cmdxa_.twoCmp() + local_coordinate_x_; }
auto Vdp1Part::calculatedYA() -> const s16 { return cmdya_.twoCmp() + local_coordinate_y_; }
auto Vdp1Part::calculatedXB() -> const s16 { return cmdxb_.twoCmp() + local_coordinate_x_; }
auto Vdp1Part::calculatedYB() -> const s16 { return cmdyb_.twoCmp() + local_coordinate_y_; }
auto Vdp1Part::calculatedXC() -> const s16 { return cmdxc_.twoCmp() + local_coordinate_x_; }
auto Vdp1Part::calculatedYC() -> const s16 { return cmdyc_.twoCmp() + local_coordinate_y_; }
auto Vdp1Part::calculatedXD() -> const s16 { return cmdxd_.twoCmp() + local_coordinate_x_; }
auto Vdp1Part::calculatedYD() -> const s16 { return cmdyd_.twoCmp() + local_coordinate_y_; }

void normalSpriteDraw(const EmulatorModules& modules, Vdp1Part& part) { loadTextureData(modules, part); }

void distortedSpriteDraw(const EmulatorModules& modules, Vdp1Part& part) {
    Log::debug(Logger::vdp1, tr("Command - Distorted sprite draw"));
    Log::debug(Logger::vdp1, "Vertex A ({},{})", part.calculatedXA(), part.calculatedYA());
    Log::debug(Logger::vdp1, "Vertex B ({},{})", part.calculatedXB(), part.calculatedYB());
    Log::debug(Logger::vdp1, "Vertex C ({},{})", part.calculatedXC(), part.calculatedYC());
    Log::debug(Logger::vdp1, "Vertex D ({},{})", part.calculatedXD(), part.calculatedYD());

    loadTextureData(modules, part);

    auto color = Color{u16{}};

    part.vertexes_.emplace_back(
        Vertex{{part.calculatedXA(), part.calculatedYA()}, {color.r, color.g, color.b, color.a}, {0.0, 0.0}}); // lower left
    part.vertexes_.emplace_back(
        Vertex{{part.calculatedXB(), part.calculatedYB()}, {color.r, color.g, color.b, color.a}, {1.0, 0.0}}); // lower right
    part.vertexes_.emplace_back(
        Vertex{{part.calculatedXC(), part.calculatedYC()}, {color.r, color.g, color.b, color.a}, {1.0, 1.0}}); // upper right
    part.vertexes_.emplace_back(
        Vertex{{part.calculatedXD(), part.calculatedYD()}, {color.r, color.g, color.b, color.a}, {0.0, 1.0}}); // upper left
}

void polygonDraw(const EmulatorModules& modules, Vdp1Part& part) {
    Log::debug(Logger::vdp1, tr("Command - Polygon draw"));
    Log::debug(Logger::vdp1, "Vertex A ({},{})", part.calculatedXA(), part.calculatedYA());
    // Log::debug(Logger::vdp1, "XA ({},{})", part.cmdxa_.toU16(), part.cmdxa_.twoCmp());

    Log::debug(Logger::vdp1, "Vertex B ({},{})", part.calculatedXB(), part.calculatedYB());
    Log::debug(Logger::vdp1, "Vertex C ({},{})", part.calculatedXC(), part.calculatedYC());
    Log::debug(Logger::vdp1, "Vertex D ({},{})", part.calculatedXD(), part.calculatedYD());

    auto color = Color(part.cmdcolr_.get(CmdColr::color_control));

    part.vertexes_.emplace_back(
        Vertex{{part.calculatedXA(), part.calculatedYA()}, {color.r, color.g, color.b, color.a}, {0.0, 0.0}}); // lower left
    part.vertexes_.emplace_back(
        Vertex{{part.calculatedXB(), part.calculatedYB()}, {color.r, color.g, color.b, color.a}, {1.0, 0.0}}); // lower right
    part.vertexes_.emplace_back(
        Vertex{{part.calculatedXC(), part.calculatedYC()}, {color.r, color.g, color.b, color.a}, {1.0, 1.0}}); // upper right
    part.vertexes_.emplace_back(
        Vertex{{part.calculatedXD(), part.calculatedYD()}, {color.r, color.g, color.b, color.a}, {0.0, 1.0}}); // upper left
    // part.vertexes_.emplace_back(Vertex{{50, 50}, {color.r, color.g, color.b, color.a}, {0.0, 0.0}});   // lower left
    // part.vertexes_.emplace_back(Vertex{{100, 50}, {color.r, color.g, color.b, color.a}, {1.0, 0.0}});  // lower right
    // part.vertexes_.emplace_back(Vertex{{100, 100}, {color.r, color.g, color.b, color.a}, {1.0, 1.0}}); // upper right
    // part.vertexes_.emplace_back(Vertex{{50, 100}, {color.r, color.g, color.b, color.a}, {0.0, 1.0}});  // upper left
}

void loadTextureData(const EmulatorModules& modules, Vdp1Part& part) {
    const auto      color_ram_address_offset = modules.vdp1()->getColorRamAddressOffset();
    auto            start_address            = vdp1_vram_start_address + part.cmdsrca_.toU16() * address_multiplier;
    const auto      texture_width            = part.cmdsize_.get(CmdSize::character_size_x) * 8;
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

        Texture::storeTexture(
            Texture(VdpType::vdp1, start_address, toUnderlying(color_mode), 0, texture_data, texture_width, texture_height));
    }
    part.textureKey(key);
}

} // namespace saturnin::video
