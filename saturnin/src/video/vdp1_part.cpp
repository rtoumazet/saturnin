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
#include <saturnin/src/video/opengl.h>
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
    cmdctrl_       = std::move(cmdctrl);
    cmdlink_       = std::move(cmdlink);
    table_address_ = table_address;
    readParameters(modules.memory(), table_address);
    calculatePriority(modules);
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
            scaledSpriteDraw(modules, *this);
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

void Vdp1Part::calculatePriority(const EmulatorModules& modules) {
    // Currently VDP1 part priority calculation is based on the first dot of the part.
    // Actually each dot has its own priority, so this calculation should be done on every dot, but
    // calculation overhead is too big for now, and you can't have a one pixel granularity using OpenGL.
    // That will have to be reworked later.

    auto           spctl                    = modules.vdp2()->getSpriteControlRegister();
    auto           tvmr                     = modules.vdp1()->getTvModeSelectionRegister();
    const auto     sprite_type              = spctl.get(SpriteControl::sprite_type);
    auto           priority_number_register = u8{};
    constexpr auto disp_priority_on_2_bits  = u8{1};
    constexpr auto disp_priority_on_1_bit   = u8{2};

    if (tvmr.get(TvModeSelection::tvm_bit_depth_selection) == BitDepthSelection::sixteen_bits_per_pixel) {
        const auto is_data_mixed = spctl.get(SpriteControl::sprite_color_mode) == SpriteColorMode::mixed;

        switch (sprite_type) {
            case SpriteType::type_0: {
                if (is_data_mixed) {
                    priority_number_register = getPriorityRegister(modules, SpriteTypeRegister::type_0_priority_mixed);
                    priority_number_register <<= disp_priority_on_1_bit;
                } else {
                    priority_number_register = getPriorityRegister(modules, SpriteTypeRegister::type_0_priority_palette);
                    priority_number_register <<= disp_priority_on_2_bits;
                }
                break;
            }
            case SpriteType::type_1: {
                if (is_data_mixed) {
                    priority_number_register = getPriorityRegister(modules, SpriteTypeRegister::type_1_priority_mixed);
                    priority_number_register <<= disp_priority_on_2_bits;
                } else {
                    priority_number_register = getPriorityRegister(modules, SpriteTypeRegister::type_1_priority_palette);
                }
                break;
            }
            case SpriteType::type_2: {
                priority_number_register = getPriorityRegister(modules, SpriteTypeRegister::type_2_priority);
                priority_number_register <<= disp_priority_on_1_bit;
                break;
            }
            case SpriteType::type_3: {
                priority_number_register = getPriorityRegister(modules, SpriteTypeRegister::type_3_priority);
                priority_number_register <<= disp_priority_on_2_bits;
                break;
            }
            case SpriteType::type_4: {
                priority_number_register = getPriorityRegister(modules, SpriteTypeRegister::type_4_priority);
                priority_number_register <<= disp_priority_on_2_bits;
                break;
            }
            case SpriteType::type_5: {
                priority_number_register = getPriorityRegister(modules, SpriteTypeRegister::type_5_priority);
                break;
            }
            case SpriteType::type_6: {
                priority_number_register = getPriorityRegister(modules, SpriteTypeRegister::type_6_priority);
                break;
            }
            case SpriteType::type_7: {
                priority_number_register = getPriorityRegister(modules, SpriteTypeRegister::type_7_priority);
                break;
            }
            default: {
                Log::warning(Logger::vdp1,
                             tr("Sprite type is not 16bits in a 16bits framebuffer configuration (Type:{:#x})"),
                             toUnderlying(sprite_type));
            }
        }

    } else {
        // 8 bits by pixel
        switch (sprite_type) {
            case SpriteType::type_8: {
                priority_number_register = getPriorityRegister(modules, SpriteTypeRegister::type_8_priority);
                priority_number_register <<= disp_priority_on_1_bit;
                break;
            }
            case SpriteType::type_9: {
                priority_number_register = getPriorityRegister(modules, SpriteTypeRegister::type_9_priority);
                priority_number_register <<= disp_priority_on_1_bit;
                break;
            }
            case SpriteType::type_a: {
                priority_number_register = getPriorityRegister(modules, SpriteTypeRegister::type_a_priority);
                priority_number_register <<= disp_priority_on_2_bits;
                break;
            }
            case SpriteType::type_b: {
                // No priority bits for this sprite type.
                priority_number_register = 0;
                break;
            }
            case SpriteType::type_c: {
                priority_number_register = getPriorityRegister(modules, SpriteTypeRegister::type_c_priority);
                priority_number_register <<= disp_priority_on_1_bit;
                break;
            }
            case SpriteType::type_d: {
                priority_number_register = getPriorityRegister(modules, SpriteTypeRegister::type_d_priority);
                priority_number_register <<= disp_priority_on_1_bit;
                break;
            }
            case SpriteType::type_e: {
                priority_number_register = getPriorityRegister(modules, SpriteTypeRegister::type_e_priority);
                priority_number_register <<= disp_priority_on_2_bits;
                break;
            }
            case SpriteType::type_f: {
                // No priority bits for this sprite type.
                priority_number_register = 0;
                break;
            }
            default: {
                Log::warning(Logger::vdp1,
                             tr("Sprite type is not 8bits in a 8bits framebuffer configuration (Type:{:#x})"),
                             toUnderlying(sprite_type));
            }
        }
    }

    priority(modules.vdp2()->getSpritePriority(priority_number_register));
}

auto Vdp1Part::getPriorityRegister(const EmulatorModules& modules, const BitRange<u8>& range) -> const u8 {
    const auto color_mode = cmdpmod_.get(CmdPmod::color_mode);
    switch (color_mode) {
        case ColorMode::mode_5_32k_colors_rgb: {
            return 0;
        }
        case ColorMode::mode_1_16_colors_lookup: {
            const auto lut_address = u32{vdp1_ram_start_address + cmdsrca_.toU32() * address_multiplier};
            auto       reg         = SpriteTypeRegister{modules.memory()->read<u32>(lut_address)};
            return (reg.get(SpriteTypeRegister::msb) == 1) ? 0 : reg.get(range);
        }
        default: {
            auto reg = SpriteTypeRegister{cmdcolr_.toU32()};
            return reg.get(range);
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
    auto part_detail = fmt::format("Table address : {:#x}\n", table_address_);

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
        return "";
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
        return "";
    };

    const auto getColorCalculation = [](const ColorCalculation cc) {
        switch (cc) {
            case ColorCalculation::mode_0:
                return R"(
Color calculation
    Mode 0, 
    Replace
)";
            case ColorCalculation::mode_1:
                return R"(
Color calculation 
    Mode 1
    Cannot rewrite / shadow
)";
            case ColorCalculation::mode_2:
                return R"(
Color calculation 
    Mode 2
    Half-luminance
)";
            case ColorCalculation::mode_3:
                return R"(
Color calculation 
    Mode 3
    Replace / half-transparent
)";
            case ColorCalculation::mode_4:
                return R"(
Color calculation 
    Mode 4
    Gouraud shading
)";
            case ColorCalculation::mode_5:
                return R"(
Color calculation 
    Mode 5
    Setting prohibited
)";
            case ColorCalculation::mode_6:
                return R"(
Color calculation 
    Mode 6
    Gouraud shading + half-luminance
)";
            case ColorCalculation::mode_7:
                return R"(
Color calculation 
    Mode 7
    Gouraud shading / gouraud shading + half-tranparent
)";
        }
        return "";
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

    const auto getGouraudShadingData = [&]() {
        if (cmdpmod_.get(CmdPmod::gouraud_shading) == GouraudShading::enabled) {
            return fmt::format(R"(
Gouraud shading 
    Table address {:#x}
)",
                               vdp1_ram_start_address + cmdgrda_.get(CmdGrda::gouraud_shading_table) * address_multiplier);
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
            part_detail += fmt::format("Vertex A ({}, {})", cmdxa_.twoCmp(), cmdya_.twoCmp());
            part_detail += fmt::format("{}\n", getCharacterReadDirection(cmdctrl_.get(CmdCtrl::character_read_direction)));
            part_detail += fmt::format("Character size {} * {}\n",
                                       cmdsize_.get(CmdSize::character_size_x) * 8,
                                       cmdsize_.get(CmdSize::character_size_y));
            part_detail += getDrawMode(cmdpmod_);
            part_detail += getGouraudShadingData();
            part_detail += fmt::format("Texture key : {:#x}", textureKey());
            break;
        }
        case CommandSelect::scaled_sprite_draw: {
            part_detail += getZoomPoint(cmdctrl_.get(CmdCtrl::zoom_point));
            part_detail += fmt::format("Vertex A ({}, {})\n", cmdxa_.twoCmp(), cmdya_.twoCmp());
            part_detail += getDrawMode(cmdpmod_);
            part_detail += getGouraudShadingData();
            part_detail += fmt::format("Texture key : {:#x}", textureKey());
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
            part_detail += getGouraudShadingData();
            part_detail += fmt::format("Texture key : {:#x}", textureKey());
            break;
        }
        case CommandSelect::polygon_draw: {
            part_detail += fmt::format("Vertex A ({}, {})\n", cmdxa_.twoCmp(), cmdya_.twoCmp());
            part_detail += fmt::format("Vertex B ({}, {})\n", cmdxb_.twoCmp(), cmdyb_.twoCmp());
            part_detail += fmt::format("Vertex C ({}, {})\n", cmdxc_.twoCmp(), cmdyc_.twoCmp());
            part_detail += fmt::format("Vertex D ({}, {})\n", cmdxd_.twoCmp(), cmdyd_.twoCmp());
            auto color = Color(cmdcolr_.get(CmdColr::color_control));
            part_detail += fmt::format("Color ({}, {}, {}, {})\n", color.r, color.g, color.b, color.a);
            part_detail += getGouraudShadingData();
            break;
        }
        case CommandSelect::polyline_draw: {
            part_detail += fmt::format("Vertex A ({}, {})\n", cmdxa_.twoCmp(), cmdya_.twoCmp());
            part_detail += fmt::format("Vertex B ({}, {})\n", cmdxb_.twoCmp(), cmdyb_.twoCmp());
            part_detail += fmt::format("Vertex C ({}, {})\n", cmdxc_.twoCmp(), cmdyc_.twoCmp());
            part_detail += fmt::format("Vertex D ({}, {})\n", cmdxd_.twoCmp(), cmdyd_.twoCmp());
            auto color = Color(cmdcolr_.get(CmdColr::color_control));
            part_detail += fmt::format("Color ({}, {}, {}, {})\n", color.r, color.g, color.b, color.a);
            part_detail += getGouraudShadingData();
            break;
        }
        case CommandSelect::line_draw: {
            part_detail += fmt::format("Vertex A ({}, {})\n", cmdxa_.twoCmp(), cmdya_.twoCmp());
            part_detail += fmt::format("Vertex B ({}, {})\n", cmdxb_.twoCmp(), cmdyb_.twoCmp());
            auto color = Color(cmdcolr_.get(CmdColr::color_control));
            part_detail += fmt::format("Color ({}, {}, {}, {})\n", color.r, color.g, color.b, color.a);
            part_detail += getGouraudShadingData();
            break;
        }
    }

    return part_detail;
}

auto Vdp1Part::calculatedXA() -> s16 { return cmdxa_.twoCmp() + local_coordinate_x_; }
auto Vdp1Part::calculatedYA() -> s16 { return cmdya_.twoCmp() + local_coordinate_y_; }
auto Vdp1Part::calculatedXB() -> s16 { return cmdxb_.twoCmp() + local_coordinate_x_; }
auto Vdp1Part::calculatedYB() -> s16 { return cmdyb_.twoCmp() + local_coordinate_y_; }
auto Vdp1Part::calculatedXC() -> s16 { return cmdxc_.twoCmp() + local_coordinate_x_; }
auto Vdp1Part::calculatedYC() -> s16 { return cmdyc_.twoCmp() + local_coordinate_y_; }
auto Vdp1Part::calculatedXD() -> s16 { return cmdxd_.twoCmp() + local_coordinate_x_; }
auto Vdp1Part::calculatedYD() -> s16 { return cmdyd_.twoCmp() + local_coordinate_y_; }

void normalSpriteDraw(const EmulatorModules& modules, Vdp1Part& part) {
    Log::debug(Logger::vdp1, tr("Command - Normal sprite draw"));
    const auto size_x = static_cast<s16>(part.cmdsize_.get(CmdSize::character_size_x) * 8);
    const auto size_y = static_cast<s16>(part.cmdsize_.get(CmdSize::character_size_y));

    loadTextureData(modules, part);

    const auto     color          = Color{u16{}};
    const auto     gouraud_values = readGouraudData(modules, part);
    VertexPosition a{part.calculatedXA(), part.calculatedYA()};
    VertexPosition b{static_cast<s16>(part.calculatedXA() + size_x), part.calculatedYA()};
    VertexPosition c{static_cast<s16>(part.calculatedXA() + size_x), static_cast<s16>(part.calculatedYA() + size_y)};
    VertexPosition d{part.calculatedXA(), static_cast<s16>(part.calculatedYA() + size_y)};

    const auto coords = getTextureCoordinates(part.cmdctrl_.get(CmdCtrl::character_read_direction));
    if (coords.size() != 4) {
        Log::error(Logger::vdp1, tr("VDP1 normal sprite draw coordinates error"));
        throw std::runtime_error("VDP1 normal sprite draw coordinates error !");
    }
    part.vertexes_.emplace_back(Vertex{a, coords[0], {color.r, color.g, color.b, color.a}, gouraud_values[0]}); // lower left
    part.vertexes_.emplace_back(Vertex{b, coords[1], {color.r, color.g, color.b, color.a}, gouraud_values[1]}); // lower right
    part.vertexes_.emplace_back(Vertex{c, coords[2], {color.r, color.g, color.b, color.a}, gouraud_values[2]}); // upper right
    part.vertexes_.emplace_back(Vertex{d, coords[3], {color.r, color.g, color.b, color.a}, gouraud_values[3]}); // upper left
}

void scaledSpriteDraw(const EmulatorModules& modules, Vdp1Part& part) {
    Log::debug(Logger::vdp1, tr("Command - Scaled sprite draw"));

    loadTextureData(modules, part);

    std::vector<VertexPosition> vertexes_pos;

    const auto width  = part.cmdxb_.twoCmp();
    const auto height = part.cmdyb_.twoCmp();

    switch (part.cmdctrl_.get(CmdCtrl::zoom_point)) {
        case ZoomPoint::two_coordinates: {
            const auto size_x = static_cast<s16>(part.cmdsize_.get(CmdSize::character_size_x) * 8);
            const auto size_y = static_cast<s16>(part.cmdsize_.get(CmdSize::character_size_y));
            Log::debug(Logger::vdp1, "Character size {} * {}", size_x, size_y);
            vertexes_pos.emplace_back(part.calculatedXA(), part.calculatedYA());
            vertexes_pos.emplace_back(part.calculatedXA() + size_x, part.calculatedYA());
            vertexes_pos.emplace_back(part.calculatedXA() + size_x, part.calculatedYA() + size_y);
            vertexes_pos.emplace_back(part.calculatedXA(), part.calculatedYA() + size_y);
            break;
        }
        case ZoomPoint::upper_left: {
            vertexes_pos.emplace_back(part.calculatedXA(), part.calculatedYA() + height);
            vertexes_pos.emplace_back(part.calculatedXA() + width, part.calculatedYA() + height);
            vertexes_pos.emplace_back(part.calculatedXA() + width, part.calculatedYA());
            vertexes_pos.emplace_back(part.calculatedXA(), part.calculatedYA());
            break;
        }
        case ZoomPoint::upper_center: {
            vertexes_pos.emplace_back(part.calculatedXA() - width / 2, part.calculatedYA() + height);
            vertexes_pos.emplace_back(part.calculatedXA() + width / 2, part.calculatedYA() + height);
            vertexes_pos.emplace_back(part.calculatedXA() + width / 2, part.calculatedYA());
            vertexes_pos.emplace_back(part.calculatedXA() - width / 2, part.calculatedYA());
            break;
        }
        case ZoomPoint::upper_right: {
            vertexes_pos.emplace_back(part.calculatedXA() - width, part.calculatedYA() + height);
            vertexes_pos.emplace_back(part.calculatedXA(), part.calculatedYA() + height);
            vertexes_pos.emplace_back(part.calculatedXA(), part.calculatedYA());
            vertexes_pos.emplace_back(part.calculatedXA() - width, part.calculatedYA());
            break;
        }
        case ZoomPoint::center_left: {
            vertexes_pos.emplace_back(part.calculatedXA(), part.calculatedYA() - height / 2);
            vertexes_pos.emplace_back(part.calculatedXA() + width, part.calculatedYA() - height / 2);
            vertexes_pos.emplace_back(part.calculatedXA() + width, part.calculatedYA() + height / 2);
            vertexes_pos.emplace_back(part.calculatedXA(), part.calculatedYA() + height / 2);
            break;
        }
        case ZoomPoint::center_center: {
            vertexes_pos.emplace_back(part.calculatedXA() - width / 2, part.calculatedYA() - height / 2);
            vertexes_pos.emplace_back(part.calculatedXA() + width / 2, part.calculatedYA() - height / 2);
            vertexes_pos.emplace_back(part.calculatedXA() + width / 2, part.calculatedYA() + height / 2);
            vertexes_pos.emplace_back(part.calculatedXA() - width / 2, part.calculatedYA() + height / 2);
            break;
        }
        case ZoomPoint::center_right: {
            vertexes_pos.emplace_back(part.calculatedXA() - width / 2, part.calculatedYA() - height / 2);
            vertexes_pos.emplace_back(part.calculatedXA(), part.calculatedYA() - height / 2);
            vertexes_pos.emplace_back(part.calculatedXA(), part.calculatedYA() + height / 2);
            vertexes_pos.emplace_back(part.calculatedXA() - width / 2, part.calculatedYA() + height / 2);
            break;
        }
        case ZoomPoint::lower_left: {
            vertexes_pos.emplace_back(part.calculatedXA(), part.calculatedYA());
            vertexes_pos.emplace_back(part.calculatedXA() + width, part.calculatedYA());
            vertexes_pos.emplace_back(part.calculatedXA() + width, part.calculatedYA() - height);
            vertexes_pos.emplace_back(part.calculatedXA(), part.calculatedYA() - height);
            break;
        }
        case ZoomPoint::lower_center: {
            vertexes_pos.emplace_back(part.calculatedXA() - width / 2, part.calculatedYA());
            vertexes_pos.emplace_back(part.calculatedXA() + width / 2, part.calculatedYA());
            vertexes_pos.emplace_back(part.calculatedXA() + width / 2, part.calculatedYA() - height);
            vertexes_pos.emplace_back(part.calculatedXA() - width / 2, part.calculatedYA() - height);
            break;
        }
        case ZoomPoint::lower_right: {
            vertexes_pos.emplace_back(part.calculatedXA() - width, part.calculatedYA());
            vertexes_pos.emplace_back(part.calculatedXA(), part.calculatedYA());
            vertexes_pos.emplace_back(part.calculatedXA(), part.calculatedYA() - height);
            vertexes_pos.emplace_back(part.calculatedXA() - width, part.calculatedYA() - height);
            break;
        }
    }

    const auto coords = getTextureCoordinates(part.cmdctrl_.get(CmdCtrl::character_read_direction));
    if (coords.size() != 4) {
        Log::error(Logger::vdp1, tr("VDP1 scaled sprite draw coordinates error"));
        throw std::runtime_error("VDP1 scaled sprite draw coordinates error !");
    }

    auto       color          = Color{u16{}};
    const auto gouraud_values = readGouraudData(modules, part);
    part.vertexes_.emplace_back(
        Vertex{vertexes_pos[0], coords[0], {color.r, color.g, color.b, color.a}, gouraud_values[0]}); // lower left
    part.vertexes_.emplace_back(
        Vertex{vertexes_pos[1], coords[1], {color.r, color.g, color.b, color.a}, gouraud_values[1]}); // lower right
    part.vertexes_.emplace_back(
        Vertex{vertexes_pos[2], coords[2], {color.r, color.g, color.b, color.a}, gouraud_values[2]}); // upper right
    part.vertexes_.emplace_back(
        Vertex{vertexes_pos[3], coords[3], {color.r, color.g, color.b, color.a}, gouraud_values[3]}); // upper left
}

void distortedSpriteDraw(const EmulatorModules& modules, Vdp1Part& part) {
    Log::debug(Logger::vdp1, tr("Command - Distorted sprite draw"));

    loadTextureData(modules, part);

    auto color = Color{u16{}};

    const auto gouraud_values = readGouraudData(modules, part);

    part.vertexes_.emplace_back(Vertex{{part.calculatedXA(), part.calculatedYA()},
                                       {0.0, 0.0}, // lower left
                                       {color.r, color.g, color.b, color.a},
                                       gouraud_values[0]});
    part.vertexes_.emplace_back(Vertex{{part.calculatedXB(), part.calculatedYB()},
                                       {1.0, 0.0}, // lower right
                                       {color.r, color.g, color.b, color.a},
                                       gouraud_values[1]});
    part.vertexes_.emplace_back(Vertex{{part.calculatedXC(), part.calculatedYC()},
                                       {1.0, 1.0}, // upper right
                                       {color.r, color.g, color.b, color.a},
                                       gouraud_values[2]});
    part.vertexes_.emplace_back(Vertex{{part.calculatedXD(), part.calculatedYD()},
                                       {0.0, 1.0}, // upper left
                                       {color.r, color.g, color.b, color.a},
                                       gouraud_values[3]});
}

void polygonDraw(const EmulatorModules& modules, Vdp1Part& part) {
    Log::debug(Logger::vdp1, tr("Command - Polygon draw"));

    const auto cmdcolr = part.cmdcolr_.get(CmdColr::color_control);
    auto       color   = Color(cmdcolr);
    if (!cmdcolr) { color.a = 0; }
    const auto gouraud_values = readGouraudData(modules, part);

    part.vertexes_.emplace_back(Vertex{{part.calculatedXA(), part.calculatedYA()},
                                       {0.0, 0.0},
                                       {color.r, color.g, color.b, color.a},
                                       gouraud_values[0]}); // lower left
    part.vertexes_.emplace_back(Vertex{{part.calculatedXB(), part.calculatedYB()},
                                       {1.0, 0.0},
                                       {color.r, color.g, color.b, color.a},
                                       gouraud_values[1]}); // lower right
    part.vertexes_.emplace_back(Vertex{{part.calculatedXC(), part.calculatedYC()},
                                       {1.0, 1.0},
                                       {color.r, color.g, color.b, color.a},
                                       gouraud_values[2]}); // upper right
    part.vertexes_.emplace_back(Vertex{{part.calculatedXD(), part.calculatedYD()},
                                       {0.0, 1.0},
                                       {color.r, color.g, color.b, color.a},
                                       gouraud_values[3]}); // upper left
}

void polylineDraw(const EmulatorModules& modules, Vdp1Part& part) {
    Log::debug(Logger::vdp1, tr("Command - Polyline draw"));

    const auto cmdcolr = part.cmdcolr_.get(CmdColr::color_control);
    auto       color   = Color(cmdcolr);
    if (!cmdcolr) { color.a = 0; }

    const auto gouraud_values = readGouraudData(modules, part);

    part.vertexes_.emplace_back(Vertex{{part.calculatedXA(), part.calculatedYA()},
                                       {0.0, 0.0},
                                       {color.r, color.g, color.b, color.a},
                                       gouraud_values[0]}); // lower left
    part.vertexes_.emplace_back(Vertex{{part.calculatedXB(), part.calculatedYB()},
                                       {1.0, 0.0},
                                       {color.r, color.g, color.b, color.a},
                                       gouraud_values[1]}); // lower right
    part.vertexes_.emplace_back(Vertex{{part.calculatedXC(), part.calculatedYC()},
                                       {1.0, 1.0},
                                       {color.r, color.g, color.b, color.a},
                                       gouraud_values[2]}); // upper right
    part.vertexes_.emplace_back(Vertex{{part.calculatedXD(), part.calculatedYD()},
                                       {0.0, 1.0},
                                       {color.r, color.g, color.b, color.a},
                                       gouraud_values[3]}); // upper left
}

void lineDraw(const EmulatorModules& modules, Vdp1Part& part) {
    Log::debug(Logger::vdp1, tr("Command - Line draw"));

    const auto cmdcolr = part.cmdcolr_.get(CmdColr::color_control);
    auto       color   = Color(cmdcolr);
    if (!cmdcolr) { color.a = 0; }

    const auto gouraud_values = readGouraudData(modules, part);

    part.vertexes_.emplace_back(Vertex{{part.calculatedXA(), part.calculatedYA()},
                                       {0.0, 0.0},
                                       {color.r, color.g, color.b, color.a},
                                       gouraud_values[0]}); // lower left
    part.vertexes_.emplace_back(Vertex{{part.calculatedXB(), part.calculatedYB()},
                                       {1.0, 0.0},
                                       {color.r, color.g, color.b, color.a},
                                       gouraud_values[1]}); // lower right
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
    // if (key == 0xa57b381a6e5b28d0) DebugBreak();

    if (Texture::isTextureLoadingNeeded(key)) {
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
        modules.opengl()->addOrUpdateTexture(key);
    }
    part.textureKey(key);
}

auto readGouraudData(const EmulatorModules& modules, Vdp1Part& part) -> std::vector<Gouraud> {
    auto gouraud_values = std::vector<Gouraud>{};
    if (part.cmdpmod_.get(CmdPmod::gouraud_shading) == GouraudShading::enabled) {
        const auto grd_table_address = vdp1_ram_start_address + part.cmdgrda_.get(CmdGrda::gouraud_shading_table) * 8;

        constexpr auto max_values = u8{4};
        for (u8 i = 0; i < max_values; i++) {
            gouraud_values.emplace_back(Gouraud(modules.memory()->read<u16>(grd_table_address + i * 2)));
        }
    } else {
        gouraud_values = {Gouraud(), Gouraud(), Gouraud(), Gouraud()};
    }
    return gouraud_values;
}

auto getTextureCoordinates(const CharacterReadDirection crd) -> std::vector<TextureCoordinates> {
    auto coords = std::vector<TextureCoordinates>{};
    switch (crd) {
        case CharacterReadDirection::h_invertion: {
            coords.emplace_back(TextureCoordinates{1.0, 0.0}); // lower left
            coords.emplace_back(TextureCoordinates{0.0, 0.0}); // lower right
            coords.emplace_back(TextureCoordinates{0.0, 1.0}); // upper right
            coords.emplace_back(TextureCoordinates{1.0, 1.0}); // upper left
            break;
        }
        case CharacterReadDirection::v_invertion: {
            coords.emplace_back(TextureCoordinates{0.0, 1.0}); // lower left
            coords.emplace_back(TextureCoordinates{1.0, 1.0}); // lower right
            coords.emplace_back(TextureCoordinates{1.0, 0.0}); // upper right
            coords.emplace_back(TextureCoordinates{0.0, 0.0}); // upper left
            break;
        }
        case CharacterReadDirection::vh_invertion: {
            coords.emplace_back(TextureCoordinates{1.0, 1.0}); // lower left
            coords.emplace_back(TextureCoordinates{0.0, 1.0}); // lower right
            coords.emplace_back(TextureCoordinates{0.0, 0.0}); // upper right
            coords.emplace_back(TextureCoordinates{0.0, 1.0}); // upper left
            break;
        }
        default: {
            coords.emplace_back(TextureCoordinates{0.0, 0.0}); // lower left
            coords.emplace_back(TextureCoordinates{1.0, 0.0}); // lower right
            coords.emplace_back(TextureCoordinates{1.0, 1.0}); // upper right
            coords.emplace_back(TextureCoordinates{0.0, 1.0}); // upper left
        }
    }
    return coords;
}

void checkColorCalculation(Vdp1Part& part) {
    switch (part.cmdpmod_.get(CmdPmod::color_calculation)) {
        case ColorCalculation::mode_0: break;
        case ColorCalculation::mode_4: break;
        default: {
            Log::unimplemented("Vdp1 - Color calculation {}", toUnderlying(part.cmdpmod_.get(CmdPmod::color_calculation)));
        }
    }
}

} // namespace saturnin::video
