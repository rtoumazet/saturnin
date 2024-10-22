//
// opengl_common.h
// Saturnin
//
// Copyright (c) 2024 Renaud Toumazet
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

#pragma once

#include <unordered_map>
#include <saturnin/src/emulator_defs.h> // u8, u16, u32

namespace saturnin::video {

struct string_hash {
    using is_transparent = void;
    [[nodiscard]] size_t operator()(const char* txt) const { return std::hash<std::string_view>{}(txt); }
    [[nodiscard]] size_t operator()(std::string_view txt) const { return std::hash<std::string_view>{}(txt); }
    [[nodiscard]] size_t operator()(const std::string& txt) const { return std::hash<std::string>{}(txt); }
};
using GraphicShaders
    = std::unordered_map<std::string, std::string, string_hash, std::equal_to<>>; ///< Shader name + shader content

enum class ProgramShader : u8 { main };
using ProgramShaders = std::unordered_map<ProgramShader, u32>; ///< Program shader name + its OpenGL id.

// Holds the various shaders used in the program
struct Shaders {
    GraphicShaders graphics;
    ProgramShaders programs;
};

// Rendering part to be used in the various renderers.
struct RenderPart {
    std::vector<Vertex> vertexes;     ///< The vertexes used for rendering.
    ColorOffset         color_offset; ///< Color offset.
    DrawType            draw_type;    ///< Type of the draw.
    u8                  priority{0};  ///< Priority (used for sorting).
    size_t              texture_key;  ///< Link to the texture.
    explicit RenderPart(const Vdp1Part& p) :
        vertexes(p.common_vdp_data_.vertexes),
        color_offset(p.common_vdp_data_.color_offset),
        draw_type(p.common_vdp_data_.draw_type),
        priority(p.common_vdp_data_.priority),
        texture_key(p.common_vdp_data_.texture_key) {};
    explicit RenderPart(const Vdp2Part& p) :
        vertexes(p.common_vdp_data_.vertexes),
        color_offset(p.common_vdp_data_.color_offset),
        draw_type(p.common_vdp_data_.draw_type),
        priority(p.common_vdp_data_.priority),
        texture_key(p.common_vdp_data_.texture_key) {};
};

using PartsList = std::vector<RenderPart>;

using FboKey = std::pair<u8, VdpLayer>; // First is priority number (1 to 7, last one being the highest), second is linked layer.
using FboKeyToFbo = std::map<FboKey, u8>; // Link between a priority to display and its relative FBO index in the FBO pool.

constexpr auto mixed_parts_key = FboKey{-1, VdpLayer::undefined};

const std::unordered_map<ScrollScreen, VdpLayer> screen_to_layer = {
    {ScrollScreen::nbg3, VdpLayer::nbg3},
    {ScrollScreen::nbg2, VdpLayer::nbg2},
    {ScrollScreen::nbg1, VdpLayer::nbg1},
    {ScrollScreen::nbg0, VdpLayer::nbg0},
    {ScrollScreen::rbg1, VdpLayer::rbg1},
    {ScrollScreen::rbg0, VdpLayer::rbg0}
};
} // namespace saturnin::video
