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
#include <saturnin/src/video/opengl.h>
#include <saturnin/src/video/texture.h>
#include <saturnin/src/video/vdp1.h>
#include <saturnin/src/emulator_modules.h>
#include <saturnin/src/locale.h>    // tr
#include <saturnin/src/utilities.h> // format, toUnderlying

namespace uti = saturnin::utilities;

namespace saturnin::video {

using core::EmulatorModules;
using core::rawRead;
using core::rawWrite;
using core::tr;
using utilities::toUnderlying;

constexpr auto horizontal_multiplier = u8{8};

s16 Vdp1Part::local_coordinate_x_;
s16 Vdp1Part::local_coordinate_y_;

Vdp1Part::Vdp1Part(EmulatorModules& modules,
                   const DrawType   type,
                   const u32        table_address,
                   const CmdCtrl&   cmdctrl,
                   const CmdLink&   cmdlink,
                   const ColorF&    color_offset) :
    BaseRenderingPart(VdpType::vdp1, type, 0, 0, color_offset) {
    cmdctrl_       = std::move(cmdctrl);
    cmdlink_       = std::move(cmdlink);
    table_address_ = table_address;
    readParameters(modules.memory(), table_address);
    calculatePriority(modules);
    generatePartData(modules);
};

void Vdp1Part::readParameters(Memory* m, const u32 address) {
    switch (toEnum<CommandSelect>(cmdctrl_.command_select)) {
        using enum CommandSelect;
        case system_clipping: {
            cmdxc_.raw = m->read<u16>(address + cmdxc_offset);
            cmdyc_.raw = m->read<u16>(address + cmdyc_offset);
            break;
        }
        case user_clipping: {
            cmdxa_.raw = m->read<u16>(address + cmdxa_offset);
            cmdya_.raw = m->read<u16>(address + cmdya_offset);
            cmdxc_.raw = m->read<u16>(address + cmdxc_offset);
            cmdyc_.raw = m->read<u16>(address + cmdyc_offset);
            break;
        }
        case local_coordinate: {
            cmdxa_.raw = m->read<u16>(address + cmdxa_offset);
            cmdya_.raw = m->read<u16>(address + cmdya_offset);
            break;
        }
        case normal_sprite_draw: {
            cmdpmod_.raw = m->read<u16>(address + cmdpmod_offset);
            cmdcolr_.raw = m->read<u16>(address + cmdcolr_offset);
            cmdsrca_.raw = m->read<u16>(address + cmdsrca_offset);
            cmdsize_.raw = m->read<u16>(address + cmdsize_offset);
            cmdxa_.raw   = m->read<u16>(address + cmdxa_offset);
            cmdya_.raw   = m->read<u16>(address + cmdya_offset);
            cmdgrda_.raw = m->read<u16>(address + cmdgrda_offset);
            break;
        }
        case scaled_sprite_draw: {
            cmdpmod_.raw = m->read<u16>(address + cmdpmod_offset);
            cmdcolr_.raw = m->read<u16>(address + cmdcolr_offset);
            cmdsrca_.raw = m->read<u16>(address + cmdsrca_offset);
            cmdsize_.raw = m->read<u16>(address + cmdsize_offset);
            cmdxa_.raw   = m->read<u16>(address + cmdxa_offset);
            cmdya_.raw   = m->read<u16>(address + cmdya_offset);
            cmdxb_.raw   = m->read<u16>(address + cmdxb_offset);
            cmdyb_.raw   = m->read<u16>(address + cmdyb_offset);
            cmdxc_.raw   = m->read<u16>(address + cmdxc_offset);
            cmdyc_.raw   = m->read<u16>(address + cmdyc_offset);
            cmdgrda_.raw = m->read<u16>(address + cmdgrda_offset);
            break;
        }
        case distorted_sprite_draw:
        case polygon_draw:
        case polyline_draw: {
            cmdpmod_.raw = m->read<u16>(address + cmdpmod_offset);
            cmdcolr_.raw = m->read<u16>(address + cmdcolr_offset);
            cmdsrca_.raw = m->read<u16>(address + cmdsrca_offset);
            cmdsize_.raw = m->read<u16>(address + cmdsize_offset);
            cmdxa_.raw   = m->read<u16>(address + cmdxa_offset);
            cmdya_.raw   = m->read<u16>(address + cmdya_offset);
            cmdxb_.raw   = m->read<u16>(address + cmdxb_offset);
            cmdyb_.raw   = m->read<u16>(address + cmdyb_offset);
            cmdxc_.raw   = m->read<u16>(address + cmdxc_offset);
            cmdyc_.raw   = m->read<u16>(address + cmdyc_offset);
            cmdxd_.raw   = m->read<u16>(address + cmdxd_offset);
            cmdyd_.raw   = m->read<u16>(address + cmdyd_offset);
            cmdgrda_.raw = m->read<u16>(address + cmdgrda_offset);
            break;
        }
        case line_draw: {
            cmdpmod_.raw = m->read<u16>(address + cmdpmod_offset);
            cmdcolr_.raw = m->read<u16>(address + cmdcolr_offset);
            cmdxa_.raw   = m->read<u16>(address + cmdxa_offset);
            cmdya_.raw   = m->read<u16>(address + cmdya_offset);
            cmdxb_.raw   = m->read<u16>(address + cmdxb_offset);
            cmdyb_.raw   = m->read<u16>(address + cmdyb_offset);
            cmdgrda_.raw = m->read<u16>(address + cmdgrda_offset);
            break;
        }
    }
}

void Vdp1Part::generatePartData(const EmulatorModules& modules) {
    switch (toEnum<CommandSelect>(cmdctrl_.command_select)) {
        using enum CommandSelect;
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
            SetLocalCoordinates(twosComplement(cmdxa_.raw), twosComplement(cmdya_.raw));
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
            polygonDraw(modules, *this);
            debug_header_ = tr("Polygon draw");
            break;
        }
        case polyline_draw: {
            polylineDraw(modules, *this);
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

    auto       spctl           = modules.vdp2()->getSpriteControlRegister();
    auto       tvmr            = modules.vdp1()->getTvModeSelectionRegister();
    const auto sprite_type     = static_cast<SpriteType>(static_cast<u8>(spctl.sprite_type));
    auto       sprite_register = SpriteTypeRegister{cmdcolr_.raw};
    // sprite_type.
    auto           priority_number_register = u8{};
    constexpr auto disp_priority_on_2_bits  = u8{1};
    constexpr auto disp_priority_on_1_bit   = u8{2};

    if (toEnum<BitDepthSelection>(tvmr.tvm_bit_depth_selection) == BitDepthSelection::sixteen_bits_per_pixel) {
        const auto is_data_mixed
            = static_cast<SpriteColorMode>(static_cast<bool>(spctl.sprite_color_mode)) == SpriteColorMode::mixed;

        switch (sprite_type) {
            using enum SpriteType;
            case type_0: {
                if (is_data_mixed) {
                    // priority_number_register = getPriorityRegister(modules, SpriteTypeRegister::type_0_priority_mixed);
                    priority_number_register
                        = getPriorityRegister(modules, static_cast<u8>(static_cast<bool>(sprite_register.type_0_priority_mixed)));
                    priority_number_register <<= disp_priority_on_1_bit;
                } else {
                    // priority_number_register = getPriorityRegister(modules, SpriteTypeRegister::type_0_priority_palette);
                    priority_number_register
                        = getPriorityRegister(modules, static_cast<u8>(sprite_register.type_0_priority_palette));
                    priority_number_register <<= disp_priority_on_2_bits;
                }
                break;
            }
            case type_1: {
                if (is_data_mixed) {
                    priority_number_register
                        = getPriorityRegister(modules, static_cast<u8>(sprite_register.type_1_priority_mixed));
                    priority_number_register <<= disp_priority_on_2_bits;
                } else {
                    priority_number_register
                        = getPriorityRegister(modules, static_cast<u8>(sprite_register.type_1_priority_palette));
                }
                break;
            }
            case type_2: {
                priority_number_register
                    = getPriorityRegister(modules, static_cast<u8>(static_cast<bool>(sprite_register.type_2_priority)));
                priority_number_register <<= disp_priority_on_1_bit;
                break;
            }
            case type_3: {
                priority_number_register = getPriorityRegister(modules, static_cast<u8>(sprite_register.type_3_priority));
                priority_number_register <<= disp_priority_on_2_bits;
                break;
            }
            case type_4: {
                priority_number_register = getPriorityRegister(modules, static_cast<u8>(sprite_register.type_4_priority));
                priority_number_register <<= disp_priority_on_2_bits;
                break;
            }
            case type_5: {
                priority_number_register = getPriorityRegister(modules, static_cast<u8>(sprite_register.type_5_priority));
                break;
            }
            case type_6: {
                priority_number_register = getPriorityRegister(modules, static_cast<u8>(sprite_register.type_6_priority));
                break;
            }
            case type_7: {
                priority_number_register = getPriorityRegister(modules, static_cast<u8>(sprite_register.type_7_priority));
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
            using enum SpriteType;
            case type_8: {
                priority_number_register
                    = getPriorityRegister(modules, static_cast<u8>(static_cast<bool>(sprite_register.type_8_priority)));
                priority_number_register <<= disp_priority_on_1_bit;
                break;
            }
            case type_9: {
                priority_number_register
                    = getPriorityRegister(modules, static_cast<u8>(static_cast<bool>(sprite_register.type_9_priority)));
                priority_number_register <<= disp_priority_on_1_bit;
                break;
            }
            case type_a: {
                priority_number_register = getPriorityRegister(modules, static_cast<u8>(sprite_register.type_a_priority));
                priority_number_register <<= disp_priority_on_2_bits;
                break;
            }
            case type_b: {
                // No priority bits for this sprite type.
                priority_number_register = 0;
                break;
            }
            case type_c: {
                priority_number_register
                    = getPriorityRegister(modules, static_cast<u8>(static_cast<bool>(sprite_register.type_c_priority)));
                priority_number_register <<= disp_priority_on_1_bit;
                break;
            }
            case type_d: {
                priority_number_register
                    = getPriorityRegister(modules, static_cast<u8>(static_cast<bool>(sprite_register.type_d_priority)));
                priority_number_register <<= disp_priority_on_1_bit;
                break;
            }
            case type_e: {
                priority_number_register = getPriorityRegister(modules, static_cast<u8>(sprite_register.type_e_priority));
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

    priority(modules.vdp2()->getSpritePriority(priority_number_register));
}

auto Vdp1Part::getPriorityRegister(const EmulatorModules& modules, const u8 priority) -> u8 {
    const auto color_mode = toEnum<ColorMode>(cmdpmod_.color_mode);
    switch (color_mode) {
        using enum ColorMode;
        case mode_5_32k_colors_rgb: {
            return 0;
        }
        case mode_1_16_colors_lookup: {
            const auto lut_address = u32{vdp1_ram_start_address + cmdsrca_.raw * vdp1_address_multiplier};
            auto       reg         = SpriteTypeRegister{modules.memory()->read<u16>(lut_address)};
            return (reg.msb == 1) ? 0 : priority;
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

    const auto getZoomPoint = [](const ZoomPoint zp) {
        switch (zp) {
            using enum ZoomPoint;
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

    const auto getCharacterReadDirection = [](const CharacterReadDirection crd) {
        switch (crd) {
            using enum CharacterReadDirection;
            case h_invertion: return "Read direction: [cccccc]horizontally inverted[]\n";
            case not_inverted: return "Read direction: [cccccc]not inverted[]\n";
            case vh_invertion: return "Read direction: [cccccc]horizontally and vertically inverted[]\n";
            case v_invertion: return "Read direction: [cccccc]vertically inverted[]\n";
        }
        return "";
    };

    const auto getColorMode = [](const ColorMode cm) {
        switch (cm) {
            using enum ColorMode;
            case mode_0_16_colors_bank: return "Color mode: [cccccc]mode 0, 16 colors, color bank, 4 bits/pixel[]\n";
            case mode_1_16_colors_lookup: return "Color mode: [cccccc]mode 1, 16 colors, lookup table, 4 bits / pixel[]\n";
            case mode_2_64_colors_bank: return "Color mode: [cccccc]mode 2, 64 colors, color bank, 8 bits/pixel[]\n";
            case mode_3_128_colors_bank: return "Color mode: [cccccc]mode 3, 128 colors, color bank, 8 bits/pixel[]\n";
            case mode_4_256_colors_bank: return "Color mode: [cccccc]mode 4, 256 colors, color bank, 8 bits/pixel[]\n";
            case mode_5_32k_colors_rgb: return "Color mode: [cccccc]mode 5, 32K colors, RGB, 16 bits/pixel[]\n";
        }
        return "";
    };

    const auto getColorCalculation = [](const ColorCalculation cc) {
        switch (cc) {
            using enum ColorCalculation;
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

    const auto getDrawMode = [&getColorMode, &getColorCalculation](CmdPmod& cp) {
        auto s = std::string{};
        s += uti::format("MSB: {}[]\n", (toEnum<MsbOn>(cp.msb_on) == MsbOn::on) ? "[00ff00]on" : "[ff0000]off");

        s += uti::format("High speed shrink: {}[]\n",
                         (toEnum<HighSpeedShrink>(cp.high_speed_shrink) == HighSpeedShrink::enabled) ? "[00ff00]enabled"
                                                                                                     : "[ff0000]disabled");
        s += uti::format("Pre-clipping: {}[]\n",
                         (toEnum<PreClippingDisable>(cp.pre_clipping_disable) == PreClippingDisable::pre_clipping)
                             ? "[00ff00]enabled"
                             : "[ff0000]disabled");

        s += "User clipping ";
        if (toEnum<UserClippingEnable>(cp.user_clipping_enable) == UserClippingEnable::enabled) {
            s += "[00ff00]enabled[]\n";
            s += "\tUser clipping: ";
            s += (toEnum<UserClippingMode>(cp.user_clipping_mode) == UserClippingMode::drawing_inside)
                     ? "[cccccc]drawing inside[]\n"
                     : "[cccccc]drawing outside[]\n";
        } else {
            s += "[ff0000]ignored[]\n";
        }
        s += uti::format("Draw {}[] mesh processing[]\n",
                         (toEnum<MeshEnable>(cp.mesh_enable) == MeshEnable::enabled) ? "[00ff00]with" : "[ff0000]without");
        s += uti::format("End code: {}[]\n",
                         (toEnum<EndCodeDisable>(cp.end_code_disable) == EndCodeDisable::enabled) ? "[00ff00]enabled"
                                                                                                  : "[ff0000]disabled");
        s += uti::format(
            "Transparent pixel: {}[]\n",
            (toEnum<TransparentPixelDisable>(cp.transparent_pixel_disable) == TransparentPixelDisable::transparent_pixel_enabled)
                ? "[00ff00]enabled"
                : "[ff0000]disabled");

        s += getColorMode(toEnum<ColorMode>(cp.color_mode));
        s += getColorCalculation(toEnum<ColorCalculation>(cp.color_calculation));

        return s;
    };

    const auto getGouraudShadingData = [&]() {
        if (toEnum<GouraudShading>(cmdpmod_.gouraud_shading) == GouraudShading::enabled) {
            return uti::format(R"(
Gouraud shading 
    Table address [cccccc]{:#x}[]
)",
                               vdp1_ram_start_address + cmdgrda_.raw * vdp1_address_multiplier);
        }
        return std::string{};
    };

    switch (toEnum<CommandSelect>(cmdctrl_.command_select)) {
        using enum CommandSelect;
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
            part_detail += uti::format("[cccccc]x = {}, y = {}[]\n", twosComplement(cmdxa_.raw), twosComplement(cmdya_.raw));
            break;
        }
        case normal_sprite_draw: {
            part_detail += uti::format("Vertex A [cccccc]({}, {})[]", twosComplement(cmdxa_.raw), twosComplement(cmdya_.raw));
            part_detail
                += uti::format("{}\n",
                               getCharacterReadDirection(toEnum<CharacterReadDirection>(cmdctrl_.character_read_direction)));
            part_detail += uti::format("Character size: [cccccc]{} * {}[]\n",
                                       cmdsize_.character_size_x * horizontal_multiplier,
                                       cmdsize_.character_size_y);
            part_detail += getDrawMode(cmdpmod_);
            part_detail += getGouraudShadingData();
            part_detail += uti::format("Texture key: [cccccc]{:#x}[]", textureKey());
            break;
        }
        case scaled_sprite_draw: {
            part_detail += getZoomPoint(toEnum<ZoomPoint>(cmdctrl_.zoom_point));
            part_detail += uti::format("Vertex A [cccccc]({}, {})[]\n", twosComplement(cmdxa_.raw), twosComplement(cmdya_.raw));
            part_detail
                += uti::format("{}\n",
                               getCharacterReadDirection(toEnum<CharacterReadDirection>(cmdctrl_.character_read_direction)));
            part_detail += getDrawMode(cmdpmod_);
            part_detail += getGouraudShadingData();
            part_detail += uti::format("Texture key: [cccccc]{:#x}[]", textureKey());
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
        case distorted_sprite_draw: {
            part_detail += uti::format("Vertex A [cccccc]({}, {})[]\n", twosComplement(cmdxa_.raw), twosComplement(cmdya_.raw));
            part_detail += uti::format("Vertex B [cccccc]({}, {})[]\n", twosComplement(cmdxb_.raw), twosComplement(cmdyb_.raw));
            part_detail += uti::format("Vertex C [cccccc]({}, {})[]\n", twosComplement(cmdxc_.raw), twosComplement(cmdyc_.raw));
            part_detail += uti::format("Vertex D [cccccc]({}, {})[]\n", twosComplement(cmdxd_.raw), twosComplement(cmdyd_.raw));
            part_detail
                += uti::format("{}",
                               getCharacterReadDirection(toEnum<CharacterReadDirection>(cmdctrl_.character_read_direction)));
            part_detail += getDrawMode(cmdpmod_);
            part_detail += getGouraudShadingData();
            part_detail += uti::format("Texture key: [cccccc]{:#x}[]", textureKey());
            break;
        }
        case polygon_draw: {
            part_detail += uti::format("Vertex A [cccccc]({}, {})[]\n", twosComplement(cmdxa_.raw), twosComplement(cmdya_.raw));
            part_detail += uti::format("Vertex B [cccccc]({}, {})[]\n", twosComplement(cmdxb_.raw), twosComplement(cmdyb_.raw));
            part_detail += uti::format("Vertex C [cccccc]({}, {})[]\n", twosComplement(cmdxc_.raw), twosComplement(cmdyc_.raw));
            part_detail += uti::format("Vertex D [cccccc]({}, {})[]\n", twosComplement(cmdxd_.raw), twosComplement(cmdyd_.raw));
            auto color = Color(cmdcolr_.raw);
            part_detail += uti::format("Color [cccccc]({}, {}, {}, {})[]\n", color.r, color.g, color.b, color.a);
            part_detail += getGouraudShadingData();
            break;
        }
        case polyline_draw: {
            part_detail += uti::format("Vertex A [cccccc]({}, {})[]\n", twosComplement(cmdxa_.raw), twosComplement(cmdya_.raw));
            part_detail += uti::format("Vertex B [cccccc]({}, {})[]\n", twosComplement(cmdxb_.raw), twosComplement(cmdyb_.raw));
            part_detail += uti::format("Vertex C [cccccc]({}, {})[]\n", twosComplement(cmdxc_.raw), twosComplement(cmdyc_.raw));
            part_detail += uti::format("Vertex D [cccccc]({}, {})[]\n", twosComplement(cmdxd_.raw), twosComplement(cmdyd_.raw));
            auto color = Color(cmdcolr_.raw);
            part_detail += uti::format("Color [cccccc]({}, {}, {}, {})[]\n", color.r, color.g, color.b, color.a);
            part_detail += getGouraudShadingData();
            break;
        }
        case line_draw: {
            part_detail += uti::format("Vertex A [cccccc]({}, {})[]\n", twosComplement(cmdxa_.raw), twosComplement(cmdya_.raw));
            part_detail += uti::format("Vertex B [cccccc]({}, {})[]\n", twosComplement(cmdxb_.raw), twosComplement(cmdyb_.raw));
            auto color = Color(cmdcolr_.raw);
            part_detail += uti::format("Color [cccccc]({}, {}, {}, {})[]\n", color.r, color.g, color.b, color.a);
            part_detail += getGouraudShadingData();
            break;
        }
    }

    return part_detail;
}

auto Vdp1Part::calculatedXA() const -> s16 { return twosComplement(cmdxa_.raw) + local_coordinate_x_; }
auto Vdp1Part::calculatedYA() const -> s16 { return twosComplement(cmdya_.raw) + local_coordinate_y_; }
auto Vdp1Part::calculatedXB() const -> s16 { return twosComplement(cmdxb_.raw) + local_coordinate_x_; }
auto Vdp1Part::calculatedYB() const -> s16 { return twosComplement(cmdyb_.raw) + local_coordinate_y_; }
auto Vdp1Part::calculatedXC() const -> s16 { return twosComplement(cmdxc_.raw) + local_coordinate_x_; }
auto Vdp1Part::calculatedYC() const -> s16 { return twosComplement(cmdyc_.raw) + local_coordinate_y_; }
auto Vdp1Part::calculatedXD() const -> s16 { return twosComplement(cmdxd_.raw) + local_coordinate_x_; }
auto Vdp1Part::calculatedYD() const -> s16 { return twosComplement(cmdyd_.raw) + local_coordinate_y_; }

void normalSpriteDraw(const EmulatorModules& modules, Vdp1Part& part) {
    Log::debug(Logger::vdp1, tr("Command - Normal sprite draw"));
    const auto size_x = static_cast<s16>(part.cmdsize_.character_size_x * horizontal_multiplier);
    const auto size_y = static_cast<s16>(part.cmdsize_.character_size_y);

    loadTextureData(modules, part);

    const auto     color          = Color{u16{}};
    const auto     gouraud_values = readGouraudData(modules, part);
    VertexPosition a{part.calculatedXA(), part.calculatedYA()};
    VertexPosition b{static_cast<s16>(part.calculatedXA() + size_x), part.calculatedYA()};
    VertexPosition c{static_cast<s16>(part.calculatedXA() + size_x), static_cast<s16>(part.calculatedYA() + size_y)};
    VertexPosition d{part.calculatedXA(), static_cast<s16>(part.calculatedYA() + size_y)};

    const auto coords = getTextureCoordinates(toEnum<CharacterReadDirection>(part.cmdctrl_.character_read_direction));
    if (coords.size() != 4) {
        Log::error(Logger::vdp1, tr("VDP1 normal sprite draw coordinates error"));
        throw std::runtime_error("VDP1 normal sprite draw coordinates error !");
    }
    // part.vertexes_.emplace_back(Vertex{a, coords[0], {color.r, color.g, color.b, color.a}, gouraud_values[0]}); // lower left
    // part.vertexes_.emplace_back(Vertex{b, coords[1], {color.r, color.g, color.b, color.a}, gouraud_values[1]}); // lower right
    // part.vertexes_.emplace_back(Vertex{c, coords[2], {color.r, color.g, color.b, color.a}, gouraud_values[2]}); // upper right
    // part.vertexes_.emplace_back(Vertex{d, coords[3], {color.r, color.g, color.b, color.a}, gouraud_values[3]}); // upper left
    part.vertexes_.reserve(4);
    // part.vertexes_
    //     .emplace_back(a.x, a.y, coords[0].s, coords[0].t, color.r, color.g, color.b, color.a, gouraud_values[0]); // lower left
    // part.vertexes_
    //    .emplace_back(b.x, b.y, coords[1].s, coords[1].t, color.r, color.g, color.b, color.a, gouraud_values[1]); // lower right
    // part.vertexes_
    //    .emplace_back(c.x, c.y, coords[2].s, coords[2].t, color.r, color.g, color.b, color.a, gouraud_values[2]); // upper right
    // part.vertexes_
    //    .emplace_back(d.x, d.y, coords[3].s, coords[3].t, color.r, color.g, color.b, color.a, gouraud_values[3]); // upper left

    // part.vertexes_.push_back(Vertex(a.x, a.y, coords[0].s, coords[0].t, color.r, color.g, color.b, color.a,
    // gouraud_values[0])); part.vertexes_.push_back(Vertex(b.x, b.y, coords[1].s, coords[1].t, color.r, color.g, color.b,
    // color.a, gouraud_values[1])); part.vertexes_.push_back(Vertex(c.x, c.y, coords[2].s, coords[2].t, color.r, color.g,
    // color.b, color.a, gouraud_values[2])); part.vertexes_.push_back(Vertex(d.x, d.y, coords[3].s, coords[3].t, color.r,
    // color.g, color.b, color.a, gouraud_values[3]));
    part.vertexes_.emplace_back(a.x, a.y, coords[0].s, coords[0].t, 0.0f, color.r, color.g, color.b, color.a, gouraud_values[0]);
    part.vertexes_.emplace_back(b.x, b.y, coords[1].s, coords[1].t, 0.0f, color.r, color.g, color.b, color.a, gouraud_values[1]);
    part.vertexes_.emplace_back(c.x, c.y, coords[2].s, coords[2].t, 0.0f, color.r, color.g, color.b, color.a, gouraud_values[2]);
    part.vertexes_.emplace_back(d.x, d.y, coords[3].s, coords[3].t, 0.0f, color.r, color.g, color.b, color.a, gouraud_values[3]);
}

void scaledSpriteDraw(const EmulatorModules& modules, Vdp1Part& part) {
    Log::debug(Logger::vdp1, tr("Command - Scaled sprite draw"));

    loadTextureData(modules, part);

    std::vector<VertexPosition> vertexes_pos;

    const auto width  = twosComplement(part.cmdxb_.raw);
    const auto height = twosComplement(part.cmdyb_.raw);
    vertexes_pos.reserve(4);
    switch (toEnum<ZoomPoint>(part.cmdctrl_.zoom_point)) {
        using enum ZoomPoint;
        case two_coordinates: {
            const auto size_x = static_cast<s16>(part.cmdsize_.character_size_x * 8);
            const auto size_y = static_cast<s16>(part.cmdsize_.character_size_y);
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

    const auto coords = getTextureCoordinates(toEnum<CharacterReadDirection>(part.cmdctrl_.character_read_direction));
    if (coords.size() != 4) {
        Log::error(Logger::vdp1, tr("VDP1 scaled sprite draw coordinates error"));
        throw std::runtime_error("VDP1 scaled sprite draw coordinates error !");
    }

    auto       color          = Color{u16{}};
    const auto gouraud_values = readGouraudData(modules, part);
    // part.vertexes_.emplace_back(
    //     Vertex{vertexes_pos[0], coords[0], {color.r, color.g, color.b, color.a}, gouraud_values[0]}); // lower left
    // part.vertexes_.emplace_back(
    //     Vertex{vertexes_pos[1], coords[1], {color.r, color.g, color.b, color.a}, gouraud_values[1]}); // lower right
    // part.vertexes_.emplace_back(
    //     Vertex{vertexes_pos[2], coords[2], {color.r, color.g, color.b, color.a}, gouraud_values[2]}); // upper right
    // part.vertexes_.emplace_back(
    //     Vertex{vertexes_pos[3], coords[3], {color.r, color.g, color.b, color.a}, gouraud_values[3]}); // upper left
    part.vertexes_.reserve(4);
    part.vertexes_.emplace_back(vertexes_pos[0].x,
                                vertexes_pos[0].y,
                                coords[0].s,
                                coords[0].t,
                                0.0f,
                                color.r,
                                color.g,
                                color.b,
                                color.a,
                                gouraud_values[0]); // lower left
    part.vertexes_.emplace_back(vertexes_pos[1].x,
                                vertexes_pos[1].y,
                                coords[1].s,
                                coords[1].t,
                                0.0f,
                                color.r,
                                color.g,
                                color.b,
                                color.a,
                                gouraud_values[1]); // lower right
    part.vertexes_.emplace_back(vertexes_pos[2].x,
                                vertexes_pos[2].y,
                                coords[2].s,
                                coords[2].t,
                                0.0f,
                                color.r,
                                color.g,
                                color.b,
                                color.a,
                                gouraud_values[2]); // upper right
    part.vertexes_.emplace_back(vertexes_pos[3].x,
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

    auto color = Color{u16{}};

    const auto coords = getTextureCoordinates(toEnum<CharacterReadDirection>(part.cmdctrl_.character_read_direction));
    if (coords.size() != 4) {
        Log::error(Logger::vdp1, tr("VDP1 distorted sprite draw coordinates error"));
        throw std::runtime_error("VDP1 distorted sprite draw coordinates error !");
    }

    const auto gouraud_values = readGouraudData(modules, part);

    part.vertexes_.reserve(4);
    part.vertexes_.emplace_back(part.calculatedXA(),
                                part.calculatedYA(),
                                0.0f,
                                0.0f,
                                0.0f,
                                color.r,
                                color.g,
                                color.b,
                                color.a,
                                gouraud_values[0]); // lower left

    part.vertexes_.emplace_back(part.calculatedXB() + 1,
                                part.calculatedYB(),
                                1.0f,
                                0.0f,
                                0.0f,
                                color.r,
                                color.g,
                                color.b,
                                color.a,
                                gouraud_values[1]); // lower right
    part.vertexes_.emplace_back(part.calculatedXC(),
                                part.calculatedYC(),
                                1.0f,
                                1.0f,
                                0.0f,
                                color.r,
                                color.g,
                                color.b,
                                color.a,
                                gouraud_values[2]); // upper right
    part.vertexes_.emplace_back(part.calculatedXD(),
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

void polygonDraw(const EmulatorModules& modules, Vdp1Part& part) {
    Log::debug(Logger::vdp1, tr("Command - Polygon draw"));

    auto color = Color(part.cmdcolr_.raw);
    if (part.cmdcolr_.raw == 0) { color.a = 0; }
    const auto gouraud_values = readGouraudData(modules, part);

    part.vertexes_.reserve(4);
    part.vertexes_.emplace_back(part.calculatedXA(),
                                part.calculatedYA(),
                                0.0f,
                                0.0f,
                                0.0f,
                                color.r,
                                color.g,
                                color.b,
                                color.a,
                                gouraud_values[0]); // lower left
    part.vertexes_.emplace_back(part.calculatedXB(),
                                part.calculatedYB(),
                                1.0f,
                                0.0f,
                                0.0f,
                                color.r,
                                color.g,
                                color.b,
                                color.a,
                                gouraud_values[1]); // lower right
    part.vertexes_.emplace_back(part.calculatedXC(),
                                part.calculatedYC(),
                                1.0f,
                                1.0f,
                                0.0f,
                                color.r,
                                color.g,
                                color.b,
                                color.a,
                                gouraud_values[2]); // upper right
    part.vertexes_.emplace_back(part.calculatedXD(),
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

void polylineDraw(const EmulatorModules& modules, Vdp1Part& part) {
    Log::debug(Logger::vdp1, tr("Command - Polyline draw"));

    auto color = Color(part.cmdcolr_.raw);
    if (part.cmdcolr_.raw == 0) { color.a = 0; }

    const auto gouraud_values = readGouraudData(modules, part);

    // part.vertexes_.emplace_back(Vertex{{part.calculatedXA(), part.calculatedYA()},
    //                                    {0.0, 0.0},
    //                                    {color.r, color.g, color.b, color.a},
    //                                    gouraud_values[0]}); // lower left
    // part.vertexes_.emplace_back(Vertex{{part.calculatedXB(), part.calculatedYB()},
    //                                    {1.0, 0.0},
    //                                    {color.r, color.g, color.b, color.a},
    //                                    gouraud_values[1]}); // lower right
    // part.vertexes_.emplace_back(Vertex{{part.calculatedXC(), part.calculatedYC()},
    //                                    {1.0, 1.0},
    //                                    {color.r, color.g, color.b, color.a},
    //                                    gouraud_values[2]}); // upper right
    // part.vertexes_.emplace_back(Vertex{{part.calculatedXD(), part.calculatedYD()},
    //                                    {0.0, 1.0},
    //                                    {color.r, color.g, color.b, color.a},
    //                                    gouraud_values[3]}); // upper left
    part.vertexes_.reserve(4);
    part.vertexes_.emplace_back(part.calculatedXA(),
                                part.calculatedYA(),
                                0.0f,
                                0.0f,
                                0.0f,
                                color.r,
                                color.g,
                                color.b,
                                color.a,
                                gouraud_values[0]); // lower left
    part.vertexes_.emplace_back(part.calculatedXB(),
                                part.calculatedYB(),
                                1.0f,
                                0.0f,
                                0.0f,
                                color.r,
                                color.g,
                                color.b,
                                color.a,
                                gouraud_values[1]); // lower right
    part.vertexes_.emplace_back(part.calculatedXC(),
                                part.calculatedYC(),
                                1.0f,
                                1.0f,
                                0.0f,
                                color.r,
                                color.g,
                                color.b,
                                color.a,
                                gouraud_values[2]); // upper right
    part.vertexes_.emplace_back(part.calculatedXD(),
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

    auto color = Color(part.cmdcolr_.raw);
    if (part.cmdcolr_.raw == 0) { color.a = 0; }

    const auto gouraud_values = readGouraudData(modules, part);

    // part.vertexes_.emplace_back(Vertex{{part.calculatedXA(), part.calculatedYA()},
    //                                    {0.0, 0.0},
    //                                    {color.r, color.g, color.b, color.a},
    //                                    gouraud_values[0]}); // lower left
    // part.vertexes_.emplace_back(Vertex{{part.calculatedXB(), part.calculatedYB()},
    //                                    {1.0, 0.0},
    //                                    {color.r, color.g, color.b, color.a},
    //                                    gouraud_values[1]}); // lower right
    part.vertexes_.reserve(2);
    part.vertexes_.emplace_back(part.calculatedXA(),
                                part.calculatedYA(),
                                0.0f,
                                0.0f,
                                0.0f,
                                color.r,
                                color.g,
                                color.b,
                                color.a,
                                gouraud_values[0]); // lower left
    part.vertexes_.emplace_back(part.calculatedXB(),
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
    auto            start_address            = vdp1_vram_start_address + part.cmdsrca_.raw * vdp1_address_multiplier;
    const auto      texture_width            = part.cmdsize_.character_size_x * 8;
    const auto      texture_height           = part.cmdsize_.character_size_y;
    const auto      color_mode               = toEnum<ColorMode>(part.cmdpmod_.color_mode);
    const auto      texture_size             = static_cast<u32>(texture_width * texture_height * 4);
    std::vector<u8> texture_data;
    texture_data.reserve(texture_size);
    const auto key = Texture::calculateKey(VdpType::vdp1, start_address, toUnderlying(color_mode));
    // if (key == 0xa57b381a6e5b28d0) DebugBreak();

    if (Texture::isTextureLoadingNeeded(key)) {
        if (modules.vdp2()->getColorRamMode() == ColorRamMode::mode_2_rgb_8_bits_1024_colors) {
            // 32 bits access to color RAM
            switch (color_mode) {
                using enum ColorMode;
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
                using enum ColorMode;
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

        Texture::storeTexture(
            Texture(VdpType::vdp1, start_address, toUnderlying(color_mode), 0, texture_data, texture_width, texture_height));
        modules.opengl()->addOrUpdateTexture(key);
    }
    part.textureKey(key);
}

auto readGouraudData(const EmulatorModules& modules, const Vdp1Part& part) -> std::vector<Gouraud> {
    auto gouraud_values = std::vector<Gouraud>{};
    if (toEnum<GouraudShading>(part.cmdpmod_.gouraud_shading) == GouraudShading::enabled) {
        const auto grd_table_address = vdp1_ram_start_address + part.cmdgrda_.raw * 8;

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
        using enum CharacterReadDirection;
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

void checkColorCalculation(Vdp1Part& part) {
    switch (toEnum<ColorCalculation>(part.cmdpmod_.color_calculation)) {
        using enum ColorCalculation;
        case mode_0: break;
        case mode_4: break;
        default: {
            Log::unimplemented("Vdp1 - Color calculation {}", part.cmdpmod_.color_calculation);
        }
    }
}

} // namespace saturnin::video
