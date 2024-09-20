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
#include <saturnin/src/memory.h>
#include <saturnin/src/video/vdp1_part.h>
#include <saturnin/src/video/opengl/opengl.h>
#include <saturnin/src/video/texture.h>
#include <saturnin/src/video/vdp1.h>
#include <saturnin/src/emulator_modules.h>
#include <saturnin/src/locale.h>    // tr
#include <saturnin/src/utilities.h> // format, toUnderlying

namespace uti   = saturnin::utilities;
namespace excpt = saturnin::exception;

namespace saturnin::video {

using core::EmulatorModules;
using core::rawRead;
using core::rawWrite;
using core::tr;
using utilities::readAs16;
using utilities::toUnderlying;

constexpr auto horizontal_multiplier = u8{8};

s16 Vdp1Part::local_coordinate_x_;
s16 Vdp1Part::local_coordinate_y_;

Vdp1Part::Vdp1Part(const EmulatorModules& modules,
                   const DrawType         type,
                   const u32              table_address,
                   const CmdCtrlType&     cmdctrl,
                   const CmdLinkType&     cmdlink,
                   const ColorOffset&     color_offset) {
    common_vdp_data_.vdp_type     = VdpType::vdp1;
    common_vdp_data_.draw_type    = type;
    common_vdp_data_.color_offset = color_offset;
    cmdctrl_                      = cmdctrl;
    cmdlink_                      = cmdlink;
    table_address_                = table_address;
    readParameters(modules.memory(), table_address);
    calculatePriority(modules);
    generatePartData(modules);
};

void Vdp1Part::readParameters(Memory* m, const u32 address) {
    const auto ram_chunk = m->read(address, 0x20);
    switch (cmdctrl_ >> CmdCtrl::comm_enum) {
        using enum CmdCtrl::CommandSelect;
        case system_clipping: {
            cmdxc_ = readAs16(ram_chunk.subspan(cmdxc_offset, 2));
            cmdyc_ = readAs16(ram_chunk.subspan(cmdyc_offset, 2));
            break;
        }
        case user_clipping: {
            cmdxa_ = readAs16(ram_chunk.subspan(cmdxa_offset, 2));
            cmdya_ = readAs16(ram_chunk.subspan(cmdya_offset, 2));
            cmdxc_ = readAs16(ram_chunk.subspan(cmdxc_offset, 2));
            cmdyc_ = readAs16(ram_chunk.subspan(cmdyc_offset, 2));
            break;
        }
        case local_coordinate: {
            cmdxa_ = readAs16(ram_chunk.subspan(cmdxa_offset, 2));
            cmdya_ = readAs16(ram_chunk.subspan(cmdya_offset, 2));
            break;
        }
        case normal_sprite_draw: {
            cmdpmod_ = readAs16(ram_chunk.subspan(cmdpmod_offset, 2));
            cmdcolr_ = readAs16(ram_chunk.subspan(cmdcolr_offset, 2));
            cmdsrca_ = readAs16(ram_chunk.subspan(cmdsrca_offset, 2));
            cmdsize_ = readAs16(ram_chunk.subspan(cmdsize_offset, 2));
            cmdxa_   = readAs16(ram_chunk.subspan(cmdxa_offset, 2));
            cmdya_   = readAs16(ram_chunk.subspan(cmdya_offset, 2));
            cmdgrda_ = readAs16(ram_chunk.subspan(cmdgrda_offset, 2));
            break;
        }
        case distorted_sprite_draw:
        case scaled_sprite_draw:
        case polygon_draw:
        case polyline_draw: {
            cmdpmod_ = readAs16(ram_chunk.subspan(cmdpmod_offset, 2));
            cmdcolr_ = readAs16(ram_chunk.subspan(cmdcolr_offset, 2));
            cmdsrca_ = readAs16(ram_chunk.subspan(cmdsrca_offset, 2));
            cmdsize_ = readAs16(ram_chunk.subspan(cmdsize_offset, 2));
            cmdxa_   = readAs16(ram_chunk.subspan(cmdxa_offset, 2));
            cmdya_   = readAs16(ram_chunk.subspan(cmdya_offset, 2));
            cmdxb_   = readAs16(ram_chunk.subspan(cmdxb_offset, 2));
            cmdyb_   = readAs16(ram_chunk.subspan(cmdyb_offset, 2));
            cmdxc_   = readAs16(ram_chunk.subspan(cmdxc_offset, 2));
            cmdyc_   = readAs16(ram_chunk.subspan(cmdyc_offset, 2));
            cmdxd_   = readAs16(ram_chunk.subspan(cmdxd_offset, 2)); // not used for scaled sprites
            cmdyd_   = readAs16(ram_chunk.subspan(cmdyd_offset, 2)); // not used for scaled sprites
            cmdgrda_ = readAs16(ram_chunk.subspan(cmdgrda_offset, 2));
            break;
        }
        case line_draw: {
            cmdpmod_ = readAs16(ram_chunk.subspan(cmdpmod_offset, 2));
            cmdcolr_ = readAs16(ram_chunk.subspan(cmdcolr_offset, 2));
            cmdxa_   = readAs16(ram_chunk.subspan(cmdxa_offset, 2));
            cmdya_   = readAs16(ram_chunk.subspan(cmdya_offset, 2));
            cmdxb_   = readAs16(ram_chunk.subspan(cmdxb_offset, 2));
            cmdyb_   = readAs16(ram_chunk.subspan(cmdyb_offset, 2));
            cmdgrda_ = readAs16(ram_chunk.subspan(cmdgrda_offset, 2));
            break;
        }
    }
}

void Vdp1Part::generatePartData(const EmulatorModules& modules) {
    switch (cmdctrl_ >> CmdCtrl::comm_enum) {
        using enum CmdCtrl::CommandSelect;
        case system_clipping: {
            // Not implemented
            debug_header_ = tr("Set system clipping coordinates");
            break;
        }
        case user_clipping: {
            // Not implemented
            debug_header_ = tr("Set user clipping coordinates");
            break;
        }
        case local_coordinate: {
            SetLocalCoordinates(twosComplement(cmdxa_.data()), twosComplement(cmdya_.data()));
            debug_header_ = tr("Set local coordinates");
            break;
        }
        case normal_sprite_draw: {
            normalSpriteDraw(modules, *this);
            debug_header_ = tr("Normal sprite draw");
            break;
        }
        case scaled_sprite_draw: {
            scaledSpriteDraw(modules, *this);
            debug_header_ = tr("Scaled sprite draw");
            break;
        }
        case distorted_sprite_draw: {
            distortedSpriteDraw(modules, *this);
            debug_header_ = tr("Distorted sprite draw");
            break;
        }
        case polygon_draw: {
            polyDraw(modules, *this, polygon_draw);
            debug_header_ = tr("Polygon draw");
            break;
        }
        case polyline_draw: {
            polyDraw(modules, *this, polyline_draw);
            debug_header_ = tr("Polyline draw");
            break;
        }
        case line_draw: {
            lineDraw(modules, *this);
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

    using Tvmr  = Vdp1Regs::Tvmr;
    using Spctl = Vdp2Regs::Spctl;

    auto spctl = modules.vdp2()->getSpriteControlRegister();
    auto tvmr  = modules.vdp1()->getTvModeSelectionRegister();
    // const auto sprite_type     = static_cast<SpriteType>(static_cast<u8>(spctl.sprite_type));
    const auto sprite_type     = spctl >> Spctl::sptype_enum;
    auto       sprite_register = SpriteTypeRegisterType{cmdcolr_.data()};
    // sprite_type.
    auto           priority_number_register = byte{};
    constexpr auto disp_priority_on_2_bits  = byte{1};
    constexpr auto disp_priority_on_1_bit   = byte{2};

    if ((tvmr >> Tvmr::tvm0_enum) == Tvmr::BitDepthSelection::sixteen_bits_per_pixel) {
        const auto is_data_mixed = (spctl >> Spctl::spclmd_enum) == Spctl::SpriteColorMode::mixed;

        switch (sprite_type) {
            using enum Spctl::SpriteType;
            case type_0: {
                if (is_data_mixed) {
                    priority_number_register
                        = getPriorityRegister(modules,
                                              static_cast<u8>(sprite_register >> SpriteTypeRegister::type_0_priority_mixed_shft));
                    priority_number_register <<= disp_priority_on_1_bit;
                } else {
                    priority_number_register = getPriorityRegister(
                        modules,
                        static_cast<u8>(sprite_register >> SpriteTypeRegister::type_0_priority_palette_shft));
                    priority_number_register <<= disp_priority_on_2_bits;
                }
                break;
            }
            case type_1: {
                if (is_data_mixed) {
                    priority_number_register
                        = getPriorityRegister(modules,
                                              static_cast<u8>(sprite_register >> SpriteTypeRegister::type_1_priority_mixed_shft));
                    priority_number_register <<= disp_priority_on_2_bits;
                } else {
                    priority_number_register = getPriorityRegister(
                        modules,
                        static_cast<u8>(sprite_register >> SpriteTypeRegister::type_1_priority_palette_shft));
                }
                break;
            }
            case type_2: {
                priority_number_register = getPriorityRegister(
                    modules,
                    static_cast<u8>(static_cast<bool>(sprite_register >> SpriteTypeRegister::type_2_priority_shft)));
                priority_number_register <<= disp_priority_on_1_bit;
                break;
            }
            case type_3: {
                priority_number_register
                    = getPriorityRegister(modules, static_cast<u8>(sprite_register >> SpriteTypeRegister::type_3_priority_shft));
                priority_number_register <<= disp_priority_on_2_bits;
                break;
            }
            case type_4: {
                priority_number_register
                    = getPriorityRegister(modules, static_cast<u8>(sprite_register >> SpriteTypeRegister::type_4_priority_shft));
                priority_number_register <<= disp_priority_on_2_bits;
                break;
            }
            case type_5: {
                priority_number_register
                    = getPriorityRegister(modules, static_cast<u8>(sprite_register >> SpriteTypeRegister::type_5_priority_shft));
                break;
            }
            case type_6: {
                priority_number_register
                    = getPriorityRegister(modules, static_cast<u8>(sprite_register >> SpriteTypeRegister::type_6_priority_shft));
                break;
            }
            case type_7: {
                priority_number_register
                    = getPriorityRegister(modules, static_cast<u8>(sprite_register >> SpriteTypeRegister::type_7_priority_shft));
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
            using enum Spctl::SpriteType;
            case type_8: {
                priority_number_register = getPriorityRegister(
                    modules,
                    static_cast<u8>(static_cast<bool>(sprite_register >> SpriteTypeRegister::type_8_priority_shft)));
                priority_number_register <<= disp_priority_on_1_bit;
                break;
            }
            case type_9: {
                priority_number_register = getPriorityRegister(
                    modules,
                    static_cast<u8>(static_cast<bool>(sprite_register >> SpriteTypeRegister::type_9_priority_shft)));
                priority_number_register <<= disp_priority_on_1_bit;
                break;
            }
            case type_a: {
                priority_number_register
                    = getPriorityRegister(modules, static_cast<u8>(sprite_register >> SpriteTypeRegister::type_a_priority_shft));
                priority_number_register <<= disp_priority_on_2_bits;
                break;
            }
            case type_b: {
                // No priority bits for this sprite type.
                priority_number_register = 0;
                break;
            }
            case type_c: {
                priority_number_register = getPriorityRegister(
                    modules,
                    static_cast<u8>(static_cast<bool>(sprite_register >> SpriteTypeRegister::type_c_priority_shft)));
                priority_number_register <<= disp_priority_on_1_bit;
                break;
            }
            case type_d: {
                priority_number_register = getPriorityRegister(
                    modules,
                    static_cast<u8>(static_cast<bool>(sprite_register >> SpriteTypeRegister::type_d_priority_shft)));
                priority_number_register <<= disp_priority_on_1_bit;
                break;
            }
            case type_e: {
                priority_number_register
                    = getPriorityRegister(modules, static_cast<u8>(sprite_register >> SpriteTypeRegister::type_e_priority_shft));
                priority_number_register <<= disp_priority_on_2_bits;
                break;
            }
            case type_f: {
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

    common_vdp_data_.priority = modules.vdp2()->getSpritePriority(priority_number_register);
}

auto Vdp1Part::getPriorityRegister(const EmulatorModules& modules, const u8 priority) const -> u8 {
    switch (cmdpmod_ >> CmdPmod::cm_enum) {
        using enum CmdPmod::ColorMode;
        case mode_5_32k_colors_rgb: {
            return 0;
        }
        case mode_1_16_colors_lookup: {
            const auto lut_address = u32{vdp1_ram_start_address + cmdsrca_.data() * vdp1_address_multiplier};
            auto       reg         = SpriteTypeRegisterType{modules.memory()->read<u16>(lut_address)};
            return (reg >> SpriteTypeRegister::msb_shft == 1) ? 0 : priority;
        }
        default: {
            return priority;
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
    auto part_detail = uti::format("Table address: [cccccc]{:#x}[]\n", table_address_);

    const auto getZoomPoint = [](const CmdCtrl::ZoomPoint zp) {
        switch (zp) {
            using enum CmdCtrl::ZoomPoint;
            case center_center: return "Zoom point: [cccccc]center-center[]\n";
            case center_left: return "Zoom point: [cccccc]center-left[]\n";
            case center_right: return "Zoom point: [cccccc]center-right[]\n";
            case lower_center: return "Zoom point: [cccccc]lower-center[]\n";
            case lower_left: return "Zoom point: [cccccc]lower-left[]\n";
            case lower_right: return "Zoom point: [cccccc]lower-right[]\n";
            case upper_center: return "Zoom point: [cccccc]upper-center[]\n";
            case upper_left: return "Zoom point: [cccccc]upper-left[]\n";
            case upper_right: return "Zoom point: [cccccc]upper-right[]\n";
            case two_coordinates: return "Zoom point: [cccccc]specifies two coordinates[]\n";
            default: return "Zoom point: [ff0000]setting prohibited[]\n";
        }
    };

    const auto getCharacterReadDirection = [](const CmdCtrl::CharacterReadDirection crd) {
        switch (crd) {
            using enum CmdCtrl::CharacterReadDirection;
            case h_invertion: return "Read direction: [cccccc]horizontally inverted[]\n";
            case not_inverted: return "Read direction: [cccccc]not inverted[]\n";
            case vh_invertion: return "Read direction: [cccccc]horizontally and vertically inverted[]\n";
            case v_invertion: return "Read direction: [cccccc]vertically inverted[]\n";
        }
        return "";
    };

    const auto getColorMode = [](const CmdPmod::ColorMode cm) {
        switch (cm) {
            using enum CmdPmod::ColorMode;
            case mode_0_16_colors_bank: return "Color mode: [cccccc]mode 0, 16 colors, color bank, 4 bits/pixel[]\n";
            case mode_1_16_colors_lookup: return "Color mode: [cccccc]mode 1, 16 colors, lookup table, 4 bits / pixel[]\n";
            case mode_2_64_colors_bank: return "Color mode: [cccccc]mode 2, 64 colors, color bank, 8 bits/pixel[]\n";
            case mode_3_128_colors_bank: return "Color mode: [cccccc]mode 3, 128 colors, color bank, 8 bits/pixel[]\n";
            case mode_4_256_colors_bank: return "Color mode: [cccccc]mode 4, 256 colors, color bank, 8 bits/pixel[]\n";
            case mode_5_32k_colors_rgb: return "Color mode: [cccccc]mode 5, 32K colors, RGB, 16 bits/pixel[]\n";
        }
        return "";
    };

    const auto getColorCalculation = [](const CmdPmod::ColorCalculation cc) {
        switch (cc) {
            using enum CmdPmod::ColorCalculation;
            case mode_0: return "Color calculation: [cccccc]mode 0, replace[]\n";
            case mode_1: return "Color calculation: [cccccc]mode 1, cannot rewrite / shadow[]\n";
            case mode_2: return "Color calculation: [cccccc]mode 2, half-luminance[]\n";
            case mode_3: return "Color calculation: [cccccc]mode 3, replace / half-transparent[]\n";
            case mode_4: return "Color calculation: [cccccc]mode 4, gouraud shading[]\n";
            case mode_5: return "Color calculation: [ff0000]mode 5, setting prohibited[]\n";
            case mode_6: return "Color calculation: [cccccc]mode 6, gouraud shading + half-luminance[]\n";
            case mode_7: return "Color calculation: [cccccc]mode 7, gouraud shading / gouraud shading + half-tranparent[]\n";
        }
        return "";
    };

    const auto getDrawMode = [&getColorMode, &getColorCalculation](CmdPmodType& cp) {
        auto s = std::string{};
        s += uti::format("MSB: {}[]\n", ((cp >> CmdPmod::mon_enum) == CmdPmod::MsbOn::on) ? "[00ff00]on" : "[ff0000]off");

        s += uti::format("High speed shrink: {}[]\n",
                         ((cp >> CmdPmod::hss_enum) == CmdPmod::HighSpeedShrink::enabled) ? "[00ff00]enabled"
                                                                                          : "[ff0000]disabled");
        s += uti::format("Pre-clipping: {}[]\n",
                         ((cp >> CmdPmod::pclp_enum) == CmdPmod::PreClippingDisable::pre_clipping) ? "[00ff00]enabled"
                                                                                                   : "[ff0000]disabled");

        s += "User clipping ";
        if ((cp >> CmdPmod::clip_enum) == CmdPmod::UserClippingEnable::enabled) {
            s += "[00ff00]enabled[]\n";
            s += "\tUser clipping: ";
            s += ((cp >> CmdPmod::cmod_enum) == CmdPmod::UserClippingMode::drawing_inside) ? "[cccccc]drawing inside[]\n"
                                                                                           : "[cccccc]drawing outside[]\n";
        } else {
            s += "[ff0000]ignored[]\n";
        }
        s += uti::format("Draw {}[] mesh processing[]\n",
                         ((cp >> CmdPmod::mesh_enum) == CmdPmod::MeshEnable::enabled) ? "[00ff00]with" : "[ff0000]without");
        s += uti::format("End code: {}[]\n",
                         ((cp >> CmdPmod::ecd_enum) == CmdPmod::EndCodeDisable::enabled) ? "[00ff00]enabled"
                                                                                         : "[ff0000]disabled");
        s += uti::format("Transparent pixel: {}[]\n",
                         ((cp >> CmdPmod::spd_enum) == CmdPmod::TransparentPixelDisable::transparent_pixel_enabled)
                             ? "[00ff00]enabled"
                             : "[ff0000]disabled");

        s += getColorMode(cp >> CmdPmod::cm_enum);
        s += getColorCalculation(cp >> CmdPmod::cc_enum);

        return s;
    };

    const auto getGouraudShadingData = [&]() {
        if ((cmdpmod_ >> CmdPmod::gs_enum) == CmdPmod::GouraudShading::enabled) {
            return uti::format(R"(
Gouraud shading 
    Table address [cccccc]{:#x}[]
)",
                               vdp1_ram_start_address + cmdgrda_.data() * vdp1_address_multiplier);
        }
        return std::string{};
    };

    switch (cmdctrl_ >> CmdCtrl::comm_enum) {
        using enum CmdCtrl::CommandSelect;
        case system_clipping: {
            // cmdxc_ = m->read<u16>(address + cmdxc_offset);
            // cmdyc_ = m->read<u16>(address + cmdyc_offset);
            break;
        }
        case user_clipping: {
            // cmdxa_ = m->read<u16>(address + cmdxa_offset);
            // cmdya_ = m->read<u16>(address + cmdya_offset);
            // cmdxc_ = m->read<u16>(address + cmdxc_offset);
            // cmdyc_ = m->read<u16>(address + cmdyc_offset);
            break;
        }
        case local_coordinate: {
            part_detail
                += uti::format("[cccccc]x = {}, y = {}[]\n", twosComplement(cmdxa_.data()), twosComplement(cmdya_.data()));
            break;
        }
        case normal_sprite_draw: {
            part_detail
                += uti::format("Vertex A [cccccc]({}, {})[]", twosComplement(cmdxa_.data()), twosComplement(cmdya_.data()));
            part_detail += uti::format("{}\n", getCharacterReadDirection(cmdctrl_ >> CmdCtrl::dir_enum));
            part_detail += uti::format("Character size: [cccccc]{} * {}[]\n",
                                       (cmdsize_ >> CmdSize::chszx_shft) * horizontal_multiplier,
                                       (cmdsize_ >> CmdSize::chszy_shft));
            part_detail += getDrawMode(cmdpmod_);
            part_detail += getGouraudShadingData();
            part_detail += uti::format("Texture key: [cccccc]{:#x}[]", common_vdp_data_.texture_key);
            break;
        }
        case scaled_sprite_draw: {
            part_detail += getZoomPoint(cmdctrl_ >> CmdCtrl::zp_enum);
            part_detail
                += uti::format("Vertex A [cccccc]({}, {})[]\n", twosComplement(cmdxa_.data()), twosComplement(cmdya_.data()));
            part_detail += uti::format("{}\n", getCharacterReadDirection(cmdctrl_ >> CmdCtrl::dir_enum));
            part_detail += getDrawMode(cmdpmod_);
            part_detail += getGouraudShadingData();
            part_detail += uti::format("Texture key: [cccccc]{:#x}[]", common_vdp_data_.texture_key);
            break;
        }
        case distorted_sprite_draw: {
            part_detail
                += uti::format("Vertex A [cccccc]({}, {})[]\n", twosComplement(cmdxa_.data()), twosComplement(cmdya_.data()));
            part_detail
                += uti::format("Vertex B [cccccc]({}, {})[]\n", twosComplement(cmdxb_.data()), twosComplement(cmdyb_.data()));
            part_detail
                += uti::format("Vertex C [cccccc]({}, {})[]\n", twosComplement(cmdxc_.data()), twosComplement(cmdyc_.data()));
            part_detail
                += uti::format("Vertex D [cccccc]({}, {})[]\n", twosComplement(cmdxd_.data()), twosComplement(cmdyd_.data()));
            part_detail += uti::format("{}", getCharacterReadDirection(cmdctrl_ >> CmdCtrl::dir_enum));
            part_detail += getDrawMode(cmdpmod_);
            part_detail += getGouraudShadingData();
            part_detail += uti::format("Texture key: [cccccc]{:#x}[]", common_vdp_data_.texture_key);
            break;
        }
        case polygon_draw:
        case polyline_draw: {
            part_detail
                += uti::format("Vertex A [cccccc]({}, {})[]\n", twosComplement(cmdxa_.data()), twosComplement(cmdya_.data()));
            part_detail
                += uti::format("Vertex B [cccccc]({}, {})[]\n", twosComplement(cmdxb_.data()), twosComplement(cmdyb_.data()));
            part_detail
                += uti::format("Vertex C [cccccc]({}, {})[]\n", twosComplement(cmdxc_.data()), twosComplement(cmdyc_.data()));
            part_detail
                += uti::format("Vertex D [cccccc]({}, {})[]\n", twosComplement(cmdxd_.data()), twosComplement(cmdyd_.data()));
            auto color = Color(cmdcolr_.data());
            part_detail += uti::format("Color [cccccc]({}, {}, {}, {})[]\n", color.r, color.g, color.b, color.a);
            part_detail += getGouraudShadingData();
            break;
        }
        case line_draw: {
            part_detail
                += uti::format("Vertex A [cccccc]({}, {})[]\n", twosComplement(cmdxa_.data()), twosComplement(cmdya_.data()));
            part_detail
                += uti::format("Vertex B [cccccc]({}, {})[]\n", twosComplement(cmdxb_.data()), twosComplement(cmdyb_.data()));
            auto color = Color(cmdcolr_.data());
            part_detail += uti::format("Color [cccccc]({}, {}, {}, {})[]\n", color.r, color.g, color.b, color.a);
            part_detail += getGouraudShadingData();
            break;
        }
    }

    return part_detail;
}

auto Vdp1Part::calculatedXA() const -> s16 { return twosComplement(cmdxa_.data()) + local_coordinate_x_; }
auto Vdp1Part::calculatedYA() const -> s16 { return twosComplement(cmdya_.data()) + local_coordinate_y_; }
auto Vdp1Part::calculatedXB() const -> s16 { return twosComplement(cmdxb_.data()) + local_coordinate_x_; }
auto Vdp1Part::calculatedYB() const -> s16 { return twosComplement(cmdyb_.data()) + local_coordinate_y_; }
auto Vdp1Part::calculatedXC() const -> s16 { return twosComplement(cmdxc_.data()) + local_coordinate_x_; }
auto Vdp1Part::calculatedYC() const -> s16 { return twosComplement(cmdyc_.data()) + local_coordinate_y_; }
auto Vdp1Part::calculatedXD() const -> s16 { return twosComplement(cmdxd_.data()) + local_coordinate_x_; }
auto Vdp1Part::calculatedYD() const -> s16 { return twosComplement(cmdyd_.data()) + local_coordinate_y_; }

void normalSpriteDraw(const EmulatorModules& modules, Vdp1Part& part) {
    Log::debug(Logger::vdp1, tr("Command - Normal sprite draw"));
    const auto size_x = static_cast<s16>((part.cmdsize_ >> CmdSize::chszx_shft) * horizontal_multiplier);
    const auto size_y = static_cast<s16>(part.cmdsize_ >> CmdSize::chszy_shft);

    loadTextureData(modules, part);

    part.common_vdp_data_.draw_type = DrawType::textured_polygon;

    const auto     color          = Color{u16{}};
    const auto     gouraud_values = readGouraudData(modules, part);
    VertexPosition a{part.calculatedXA(), part.calculatedYA()};
    VertexPosition b{static_cast<s16>(part.calculatedXA() + size_x), part.calculatedYA()};
    VertexPosition c{static_cast<s16>(part.calculatedXA() + size_x), static_cast<s16>(part.calculatedYA() + size_y)};
    VertexPosition d{part.calculatedXA(), static_cast<s16>(part.calculatedYA() + size_y)};

    const auto coords = getTextureCoordinates(part.cmdctrl_ >> CmdCtrl::dir_enum);
    if (coords.size() != 4) {
        Log::error(Logger::vdp1, tr("VDP1 normal sprite draw coordinates error"));
        throw excpt::Vdp1Error("VDP1 normal sprite draw coordinates error !");
    }

    part.common_vdp_data_.vertexes.reserve(4);
    part.common_vdp_data_.vertexes
        .emplace_back(a.x, a.y, coords[0].s, coords[0].t, 0.0f, color.r, color.g, color.b, color.a, gouraud_values[0]);
    part.common_vdp_data_.vertexes
        .emplace_back(b.x, b.y, coords[1].s, coords[1].t, 0.0f, color.r, color.g, color.b, color.a, gouraud_values[1]);
    part.common_vdp_data_.vertexes
        .emplace_back(c.x, c.y, coords[2].s, coords[2].t, 0.0f, color.r, color.g, color.b, color.a, gouraud_values[2]);
    part.common_vdp_data_.vertexes
        .emplace_back(d.x, d.y, coords[3].s, coords[3].t, 0.0f, color.r, color.g, color.b, color.a, gouraud_values[3]);
}

void scaledSpriteDraw(const EmulatorModules& modules, Vdp1Part& part) {
    Log::debug(Logger::vdp1, tr("Command - Scaled sprite draw"));

    loadTextureData(modules, part);

    part.common_vdp_data_.draw_type = DrawType::textured_polygon;

    std::vector<VertexPosition> vertexes_pos;

    const auto width  = twosComplement(part.cmdxb_.data());
    const auto height = twosComplement(part.cmdyb_.data());
    vertexes_pos.reserve(4);
    switch (part.cmdctrl_ >> CmdCtrl::zp_enum) {
        using enum CmdCtrl::ZoomPoint;
        case two_coordinates: {
            const auto size_x = static_cast<s16>((part.cmdsize_ >> CmdSize::chszx_shft) * 8);
            const auto size_y = static_cast<s16>(part.cmdsize_ >> CmdSize::chszy_shft);
            Log::debug(Logger::vdp1, "Character size {} * {}", size_x, size_y);
            vertexes_pos.emplace_back(part.calculatedXA(), part.calculatedYA());
            vertexes_pos.emplace_back(part.calculatedXA() + size_x, part.calculatedYA());
            vertexes_pos.emplace_back(part.calculatedXA() + size_x, part.calculatedYA() + size_y);
            vertexes_pos.emplace_back(part.calculatedXA(), part.calculatedYA() + size_y);
            break;
        }
        case upper_left: {
            vertexes_pos.emplace_back(part.calculatedXA(), part.calculatedYA() + height);
            vertexes_pos.emplace_back(part.calculatedXA() + width, part.calculatedYA() + height);
            vertexes_pos.emplace_back(part.calculatedXA() + width, part.calculatedYA());
            vertexes_pos.emplace_back(part.calculatedXA(), part.calculatedYA());
            break;
        }
        case upper_center: {
            vertexes_pos.emplace_back(part.calculatedXA() - width / 2, part.calculatedYA() + height);
            vertexes_pos.emplace_back(part.calculatedXA() + width / 2, part.calculatedYA() + height);
            vertexes_pos.emplace_back(part.calculatedXA() + width / 2, part.calculatedYA());
            vertexes_pos.emplace_back(part.calculatedXA() - width / 2, part.calculatedYA());
            break;
        }
        case upper_right: {
            vertexes_pos.emplace_back(part.calculatedXA() - width, part.calculatedYA() + height);
            vertexes_pos.emplace_back(part.calculatedXA(), part.calculatedYA() + height);
            vertexes_pos.emplace_back(part.calculatedXA(), part.calculatedYA());
            vertexes_pos.emplace_back(part.calculatedXA() - width, part.calculatedYA());
            break;
        }
        case center_left: {
            vertexes_pos.emplace_back(part.calculatedXA(), part.calculatedYA() - height / 2);
            vertexes_pos.emplace_back(part.calculatedXA() + width, part.calculatedYA() - height / 2);
            vertexes_pos.emplace_back(part.calculatedXA() + width, part.calculatedYA() + height / 2);
            vertexes_pos.emplace_back(part.calculatedXA(), part.calculatedYA() + height / 2);
            break;
        }
        case center_center: {
            vertexes_pos.emplace_back(part.calculatedXA() - width / 2, part.calculatedYA() - height / 2);
            vertexes_pos.emplace_back(part.calculatedXA() + width / 2, part.calculatedYA() - height / 2);
            vertexes_pos.emplace_back(part.calculatedXA() + width / 2, part.calculatedYA() + height / 2);
            vertexes_pos.emplace_back(part.calculatedXA() - width / 2, part.calculatedYA() + height / 2);
            break;
        }
        case center_right: {
            vertexes_pos.emplace_back(part.calculatedXA() - width / 2, part.calculatedYA() - height / 2);
            vertexes_pos.emplace_back(part.calculatedXA(), part.calculatedYA() - height / 2);
            vertexes_pos.emplace_back(part.calculatedXA(), part.calculatedYA() + height / 2);
            vertexes_pos.emplace_back(part.calculatedXA() - width / 2, part.calculatedYA() + height / 2);
            break;
        }
        case lower_left: {
            vertexes_pos.emplace_back(part.calculatedXA(), part.calculatedYA());
            vertexes_pos.emplace_back(part.calculatedXA() + width, part.calculatedYA());
            vertexes_pos.emplace_back(part.calculatedXA() + width, part.calculatedYA() - height);
            vertexes_pos.emplace_back(part.calculatedXA(), part.calculatedYA() - height);
            break;
        }
        case lower_center: {
            vertexes_pos.emplace_back(part.calculatedXA() - width / 2, part.calculatedYA());
            vertexes_pos.emplace_back(part.calculatedXA() + width / 2, part.calculatedYA());
            vertexes_pos.emplace_back(part.calculatedXA() + width / 2, part.calculatedYA() - height);
            vertexes_pos.emplace_back(part.calculatedXA() - width / 2, part.calculatedYA() - height);
            break;
        }
        case lower_right: {
            vertexes_pos.emplace_back(part.calculatedXA() - width, part.calculatedYA());
            vertexes_pos.emplace_back(part.calculatedXA(), part.calculatedYA());
            vertexes_pos.emplace_back(part.calculatedXA(), part.calculatedYA() - height);
            vertexes_pos.emplace_back(part.calculatedXA() - width, part.calculatedYA() - height);
            break;
        }
    }

    const auto coords = getTextureCoordinates(part.cmdctrl_ >> CmdCtrl::dir_enum);
    if (coords.size() != 4) {
        Log::error(Logger::vdp1, tr("VDP1 scaled sprite draw coordinates error"));
        throw excpt::Vdp1Error("VDP1 scaled sprite draw coordinates error !");
    }

    auto       color          = Color{u16{}};
    const auto gouraud_values = readGouraudData(modules, part);
    part.common_vdp_data_.vertexes.reserve(4);
    part.common_vdp_data_.vertexes.emplace_back(vertexes_pos[0].x,
                                                vertexes_pos[0].y,
                                                coords[0].s,
                                                coords[0].t,
                                                0.0f,
                                                color.r,
                                                color.g,
                                                color.b,
                                                color.a,
                                                gouraud_values[0]); // lower left
    part.common_vdp_data_.vertexes.emplace_back(vertexes_pos[1].x,
                                                vertexes_pos[1].y,
                                                coords[1].s,
                                                coords[1].t,
                                                0.0f,
                                                color.r,
                                                color.g,
                                                color.b,
                                                color.a,
                                                gouraud_values[1]); // lower right
    part.common_vdp_data_.vertexes.emplace_back(vertexes_pos[2].x,
                                                vertexes_pos[2].y,
                                                coords[2].s,
                                                coords[2].t,
                                                0.0f,
                                                color.r,
                                                color.g,
                                                color.b,
                                                color.a,
                                                gouraud_values[2]); // upper right
    part.common_vdp_data_.vertexes.emplace_back(vertexes_pos[3].x,
                                                vertexes_pos[3].y,
                                                coords[3].s,
                                                coords[3].t,
                                                0.0f,
                                                color.r,
                                                color.g,
                                                color.b,
                                                color.a,
                                                gouraud_values[3]); // upper left
}

void distortedSpriteDraw(const EmulatorModules& modules, Vdp1Part& part) {
    Log::debug(Logger::vdp1, tr("Command - Distorted sprite draw"));

    loadTextureData(modules, part);

    part.common_vdp_data_.draw_type = DrawType::textured_polygon;

    auto color = Color{u16{}};

    const auto coords = getTextureCoordinates(part.cmdctrl_ >> CmdCtrl::dir_enum);
    if (coords.size() != 4) {
        Log::error(Logger::vdp1, tr("VDP1 distorted sprite draw coordinates error"));
        throw excpt::Vdp1Error("VDP1 distorted sprite draw coordinates error !");
    }

    const auto gouraud_values = readGouraudData(modules, part);

    part.common_vdp_data_.vertexes.reserve(4);
    part.common_vdp_data_.vertexes.emplace_back(part.calculatedXA(),
                                                part.calculatedYA(),
                                                coords[0].s,
                                                coords[0].t,
                                                0.0f,
                                                color.r,
                                                color.g,
                                                color.b,
                                                color.a,
                                                gouraud_values[0]); // lower left

    part.common_vdp_data_.vertexes.emplace_back(part.calculatedXB() + 1,
                                                part.calculatedYB(),
                                                coords[1].s,
                                                coords[1].t,
                                                0.0f,
                                                color.r,
                                                color.g,
                                                color.b,
                                                color.a,
                                                gouraud_values[1]); // lower right
    part.common_vdp_data_.vertexes.emplace_back(part.calculatedXC() + 1,
                                                part.calculatedYC() + 1,
                                                coords[2].s,
                                                coords[2].t,
                                                0.0f,
                                                color.r,
                                                color.g,
                                                color.b,
                                                color.a,
                                                gouraud_values[2]); // upper right
    part.common_vdp_data_.vertexes.emplace_back(part.calculatedXD(),
                                                part.calculatedYD() + 1,
                                                coords[3].s,
                                                coords[3].t,
                                                0.0f,
                                                color.r,
                                                color.g,
                                                color.b,
                                                color.a,
                                                gouraud_values[3]); // upper left
}

void polyDraw(const EmulatorModules& modules, Vdp1Part& part, CmdCtrl::CommandSelect command) {
    switch (command) {
        using enum CmdCtrl::CommandSelect;
        case polygon_draw: {
            Log::debug(Logger::vdp1, tr("Command - Polygon draw"));
            part.common_vdp_data_.draw_type = DrawType::non_textured_polygon;
            break;
        }
        case polyline_draw: {
            Log::debug(Logger::vdp1, tr("Command - Polyline draw"));
            part.common_vdp_data_.draw_type = DrawType::polyline;
            break;
        }
        default: {
            Log::error(Logger::vdp1, tr("Command - Unknown poly draw"));
            return;
        }
    }

    auto color = Color(part.cmdcolr_.data());
    if (part.cmdcolr_.is(0)) { color.a = 0; }
    const auto gouraud_values = readGouraudData(modules, part);

    part.common_vdp_data_.vertexes.reserve(4);
    part.common_vdp_data_.vertexes.emplace_back(part.calculatedXA(),
                                                part.calculatedYA(),
                                                0.0f,
                                                0.0f,
                                                0.0f,
                                                color.r,
                                                color.g,
                                                color.b,
                                                color.a,
                                                gouraud_values[0]); // lower left
    part.common_vdp_data_.vertexes.emplace_back(part.calculatedXB(),
                                                part.calculatedYB(),
                                                1.0f,
                                                0.0f,
                                                0.0f,
                                                color.r,
                                                color.g,
                                                color.b,
                                                color.a,
                                                gouraud_values[1]); // lower right
    part.common_vdp_data_.vertexes.emplace_back(part.calculatedXC(),
                                                part.calculatedYC(),
                                                1.0f,
                                                1.0f,
                                                0.0f,
                                                color.r,
                                                color.g,
                                                color.b,
                                                color.a,
                                                gouraud_values[2]); // upper right
    part.common_vdp_data_.vertexes.emplace_back(part.calculatedXD(),
                                                part.calculatedYD(),
                                                0.0f,
                                                1.0f,
                                                0.0f,
                                                color.r,
                                                color.g,
                                                color.b,
                                                color.a,
                                                gouraud_values[3]); // upper left
}

void lineDraw(const EmulatorModules& modules, Vdp1Part& part) {
    Log::debug(Logger::vdp1, tr("Command - Line draw"));

    part.common_vdp_data_.draw_type = DrawType::line;

    auto color = Color(part.cmdcolr_.data());
    if (part.cmdcolr_.is(0)) { color.a = 0; }

    const auto gouraud_values = readGouraudData(modules, part);

    part.common_vdp_data_.vertexes.reserve(2);
    part.common_vdp_data_.vertexes.emplace_back(part.calculatedXA(),
                                                part.calculatedYA(),
                                                0.0f,
                                                0.0f,
                                                0.0f,
                                                color.r,
                                                color.g,
                                                color.b,
                                                color.a,
                                                gouraud_values[0]); // lower left
    part.common_vdp_data_.vertexes.emplace_back(part.calculatedXB(),
                                                part.calculatedYB(),
                                                1.0f,
                                                0.0f,
                                                0.0f,
                                                color.r,
                                                color.g,
                                                color.b,
                                                color.a,
                                                gouraud_values[1]); // lower right
}

void loadTextureData(const EmulatorModules& modules, Vdp1Part& part) {
    const auto      color_ram_address_offset = modules.vdp1()->getColorRamAddressOffset();
    auto            start_address            = vdp1_vram_start_address + part.cmdsrca_.data() * vdp1_address_multiplier;
    const auto      texture_width            = (part.cmdsize_ >> CmdSize::chszx_shft) * 8;
    const auto      texture_height           = part.cmdsize_ >> CmdSize::chszy_shft;
    const auto      color_mode               = part.cmdpmod_ >> CmdPmod::cm_enum;
    const auto      texture_size             = static_cast<u32>(texture_width * texture_height * 4);
    std::vector<u8> texture_data;
    texture_data.reserve(texture_size);
    const auto key = Texture::calculateKey(VdpType::vdp1, start_address, toUnderlying(color_mode));

    if (Texture::isTextureLoadingNeeded(key)) {
        if (modules.vdp2()->getColorRamMode() == Vdp2Regs::Ramctl::ColorRamMode::mode_2_rgb_8_bits_1024_colors) {
            // 32 bits access to color RAM
            switch (color_mode) {
                using enum CmdPmod::ColorMode;
                case mode_0_16_colors_bank: {
                    readColorBankMode16Colors<u32>(modules, texture_data, start_address, color_ram_address_offset, part);
                    break;
                }
                case mode_1_16_colors_lookup: {
                    // LUT doesn't depend on color RAM, it's always 16 bits.
                    readLookUpTable16Colors<u16>(modules, texture_data, start_address, part);
                    break;
                }
                case mode_2_64_colors_bank: {
                    readColorBankMode64Colors<u32>(modules, texture_data, start_address, color_ram_address_offset, part);
                    break;
                }
                case mode_3_128_colors_bank: {
                    readColorBankMode128Colors<u32>(modules, texture_data, start_address, color_ram_address_offset, part);
                    break;
                }
                case mode_4_256_colors_bank: {
                    readColorBankMode256Colors<u32>(modules, texture_data, start_address, color_ram_address_offset, part);
                    break;
                }
                case mode_5_32k_colors_rgb: {
                    // RGB is always 16 bits.
                    readRgb32KColors<u16>(modules, texture_data, start_address, part);
                    break;
                }
            }
        } else {
            // 16 bits access to color RAM
            switch (color_mode) {
                using enum CmdPmod::ColorMode;
                case mode_0_16_colors_bank: {
                    readColorBankMode16Colors<u16>(modules, texture_data, start_address, color_ram_address_offset, part);
                    break;
                }
                case mode_1_16_colors_lookup: {
                    readLookUpTable16Colors<u16>(modules, texture_data, start_address, part);
                    break;
                }
                case mode_2_64_colors_bank: {
                    readColorBankMode64Colors<u16>(modules, texture_data, start_address, color_ram_address_offset, part);
                    break;
                }
                case mode_3_128_colors_bank: {
                    readColorBankMode128Colors<u16>(modules, texture_data, start_address, color_ram_address_offset, part);
                    break;
                }
                case mode_4_256_colors_bank: {
                    readColorBankMode256Colors<u16>(modules, texture_data, start_address, color_ram_address_offset, part);
                    break;
                }
                case mode_5_32k_colors_rgb: {
                    readRgb32KColors<u16>(modules, texture_data, start_address, part);
                    break;
                }
            }
        }

        Texture::storeTexture(Texture(VdpType::vdp1,
                                      VdpLayer::sprite,
                                      start_address,
                                      toUnderlying(color_mode),
                                      0,
                                      texture_data,
                                      texture_width,
                                      texture_height));
        modules.opengl()->addOrUpdateTexture(key, VdpLayer::sprite);
    }
    part.common_vdp_data_.texture_key = key;
}

auto readGouraudData(const EmulatorModules& modules, const Vdp1Part& part) -> std::vector<Gouraud> {
    auto gouraud_values = std::vector<Gouraud>{};
    if ((part.cmdpmod_ >> CmdPmod::gs_enum) == CmdPmod::GouraudShading::enabled) {
        const auto grd_table_address = vdp1_ram_start_address + part.cmdgrda_.data() * 8;

        constexpr auto max_values = u8{4};
        for (u8 i = 0; i < max_values; i++) {
            gouraud_values.emplace_back(Gouraud(modules.memory()->read<u16>(grd_table_address + i * 2)));
        }
    } else {
        gouraud_values = {Gouraud(), Gouraud(), Gouraud(), Gouraud()};
    }
    return gouraud_values;
}

auto getTextureCoordinates(const CmdCtrl::CharacterReadDirection crd) -> std::vector<TextureCoordinates> {
    auto coords = std::vector<TextureCoordinates>{};
    switch (crd) {
        using enum CmdCtrl::CharacterReadDirection;
        case h_invertion: {
            coords.emplace_back(TextureCoordinates{1.0, 0.0}); // lower left
            coords.emplace_back(TextureCoordinates{0.0, 0.0}); // lower right
            coords.emplace_back(TextureCoordinates{0.0, 1.0}); // upper right
            coords.emplace_back(TextureCoordinates{1.0, 1.0}); // upper left
            break;
        }
        case v_invertion: {
            coords.emplace_back(TextureCoordinates{0.0, 1.0}); // lower left
            coords.emplace_back(TextureCoordinates{1.0, 1.0}); // lower right
            coords.emplace_back(TextureCoordinates{1.0, 0.0}); // upper right
            coords.emplace_back(TextureCoordinates{0.0, 0.0}); // upper left
            break;
        }
        case vh_invertion: {
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

void checkColorCalculation(const Vdp1Part& part) {
    switch (part.cmdpmod_ >> CmdPmod::cc_enum) {
        using enum CmdPmod::ColorCalculation;
        case mode_0: break;
        case mode_4: break;
        default: {
            Log::unimplemented("Vdp1 - Color calculation {}", part.cmdpmod_ >> CmdPmod::cc_shft);
        }
    }
}

} // namespace saturnin::video
