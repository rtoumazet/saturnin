//
// vdp_common.h
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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file	vdp_common.h
///
/// \brief	Declares functions / structs used by both VDPs.
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <saturnin/src/emulator_defs.h> // u8, u16, u32
#include <saturnin/src/bitfield.h>

namespace saturnin::video {

constexpr auto vram_start_address      = u32{0x25e00000};
constexpr auto cram_start_address      = u32{0x25f00000};
constexpr auto vdp1_address_multiplier = u8{8};

constexpr auto gouraud_offset = s8{0x10};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   VdpType
///
/// \brief  Values that represent VDP types
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class VdpType { not_set, vdp1, vdp2 };

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum	Layer
///
/// \brief	Values that represent rendered layers.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Layer { nbg0 = 0, rbg1 = 0, nbg1 = 1, exbg = 1, nbg2 = 2, nbg3 = 3, rbg0 = 4, back = 5, sprite = 6, undefined = -1 };

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   DrawType
///
/// \brief  Values that represent draw types.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class DrawType {
    textured_polygon, // VDP2 cells, normal/scaled/distorted sprites
    non_textured_polygon,
    polyline,
    line,
    not_drawable, // Setting local coordinates, clipping commands, etc ..
    undefined
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct Color
///
/// \brief  A color defined by its components.
///
/// \author Runik
/// \date   25/03/2021
////////////////////////////////////////////////////////////////////////////////////////////////////

struct Color {
    u8 r;
    u8 g;
    u8 b;
    u8 a{0xFF};
    explicit Color(const u16 raw_data) :
        r(static_cast<u8>((raw_data & 0x1F) << 3)),
        g((raw_data & 0x3E0) >> 2),
        b((raw_data & 0x7C00) >> 7){};
    explicit Color(const u32 raw_data) : r(raw_data & 0x0000FF), g((raw_data & 0x00FF00) >> 8), b((raw_data & 0xFF0000) >> 16){};
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct Gouraud
///
/// \brief  A gouraud value.
///
/// \author Runik
/// \date   28/07/2021
////////////////////////////////////////////////////////////////////////////////////////////////////

struct Gouraud {
    s8 r = 0;
    s8 g = 0;
    s8 b = 0;
    explicit Gouraud(const u16 raw_data) :
        r((raw_data & 0x1F) - 0x10),
        g(((raw_data & 0x3E0) >> 5) - 0x10),
        b(((raw_data & 0x7C00) >> 10) - 0x10) {}
    Gouraud() = default;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct	ColorS16
///
/// \brief	A color with internal components as s16.
///
/// \author	Runik
/// \date	10/08/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

struct ColorS16 {
    s16 r;
    s16 g;
    s16 b;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct	ColorF
///
/// \brief	A color with internal components as floats.
///
/// \author	Runik
/// \date	10/08/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

struct ColorF {
    float r;
    float g;
    float b;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct VertexPosition
///
/// \brief  A vertex position in Saturn space.
///
/// \author Runik
/// \date   13/04/2021
////////////////////////////////////////////////////////////////////////////////////////////////////

struct VertexPosition {
    s16 x;
    s16 y;
    VertexPosition(const s16 x, const s16 y) : x(x), y(y){};
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct VertexColor
///
/// \brief  A vertex color.
///
/// \author Runik
/// \date   13/04/2021
////////////////////////////////////////////////////////////////////////////////////////////////////

struct VertexColor {
    u8 r; // red
    u8 g; // green
    u8 b; // blue
    u8 a; // alpha
    VertexColor(const u8 r, const u8 g, const u8 b, const u8 a) : r(r), g(g), b(b), a(a){};
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct TextureCoordinates
///
/// \brief  A texture coordinates.
///
/// \author Runik
/// \date   13/04/2021
////////////////////////////////////////////////////////////////////////////////////////////////////

struct TextureCoordinates {
    float s;
    float t;
    float p;
    TextureCoordinates(const float s, const float t, const float p) : s(s), t(t), p(p){};
    TextureCoordinates(const float s, const float t) : s(s), t(t), p(0.0f){};
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct Vertex
///
/// \brief  A vertex.
///
/// \author Runik
/// \date   01/02/2021
////////////////////////////////////////////////////////////////////////////////////////////////////

struct Vertex {
    VertexPosition     pos;        ///< Position in Saturn space.
    TextureCoordinates tex_coords; ///< Texture coordinates.
    VertexColor        color;      ///< Color.
    Gouraud            gouraud;    ///< Gouraud color.

    Vertex(const s16 x, const s16 y, const float s, const float t) :
        pos(VertexPosition(x, y)),
        tex_coords(TextureCoordinates(s, t, 0.0f)),
        color(VertexColor(0, 0, 0, 0)),
        gouraud(Gouraud()){};

    Vertex(const s16     x,
           const s16     y,
           const float   s,
           const float   t,
           const float   p,
           const u8      r,
           const u8      g,
           const u8      b,
           const u8      a,
           const Gouraud grd) :
        pos(VertexPosition(x, y)),
        tex_coords(TextureCoordinates(s, t, p)),
        color(VertexColor(r, g, b, a)),
        gouraud(grd){};
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	Dots4Bits
///
/// \brief	32 bits register splitted in 4 bits dots components.
///
/// \author	Runik
/// \date	25/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union Dots4Bits {
    u32             raw; ///< Raw representation.
    BitField<28, 4> dot_0;
    BitField<24, 4> dot_1;
    BitField<20, 4> dot_2;
    BitField<16, 4> dot_3;
    BitField<12, 4> dot_4;
    BitField<8, 4>  dot_5;
    BitField<4, 4>  dot_6;
    BitField<0, 4>  dot_7;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	Dots6Bits
///
/// \brief	32 bits register splitted in 6 bits dots components.
///
/// \author	Runik
/// \date	26/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union Dots6Bits {
    u32             raw; ///< Raw representation.
    BitField<24, 6> dot_0;
    BitField<16, 6> dot_1;
    BitField<8, 6>  dot_2;
    BitField<0, 6>  dot_3;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	Dots7Bits
///
/// \brief	32 bits register splitted in 7 bits dots components.
///
/// \author	Runik
/// \date	26/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union Dots7Bits {
    u32             raw; ///< Raw representation.
    BitField<24, 7> dot_0;
    BitField<16, 7> dot_1;
    BitField<8, 7>  dot_2;
    BitField<0, 7>  dot_3;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	Dots8Bits
///
/// \brief	32 bits register splitted in 8 bits dots components.
///
/// \author	Runik
/// \date	26/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union Dots8Bits {
    u32             raw; ///< Raw representation.
    BitField<24, 8> dot_0;
    BitField<16, 8> dot_1;
    BitField<8, 8>  dot_2;
    BitField<0, 8>  dot_3;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	Dots16Bits
///
/// \brief	32 bits register splitted in 16 bits dots components..
///
/// \author	Runik
/// \date	26/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union Dots16Bits {
    u32              raw; ///< Raw representation.
    BitField<16, 16> dot_0;
    BitField<0, 16>  dot_1;
};

// Some testing
class Dots16BitsRegister : public Register {
  public:
    using Register::Register;
    inline static const BitRange<u32> raw{0, 31};
    inline static const BitRange<u16> dot_0{16, 31};
    inline static const BitRange<u16> dot_1{0, 15};
};

} // namespace saturnin::video
