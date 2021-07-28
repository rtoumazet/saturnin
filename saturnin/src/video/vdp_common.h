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

namespace saturnin::video {

constexpr s8 gouraud_offset = 0x10;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   VdpType
///
/// \brief  Values that represent VDP types
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class VdpType { not_set, vdp1, vdp2 };

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
    Color(const u16 raw_data) {
        r = (raw_data & 0x1F) << 3;
        g = (raw_data & 0x3E0) >> 2;
        b = (raw_data & 0x7C00) >> 7;
    };
    Color(const u32 raw_data) {
        r = (raw_data & 0x0000FF);
        g = (raw_data & 0x00FF00) >> 8;
        b = (raw_data & 0xFF0000) >> 16;
    };
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
    s8 r;
    s8 g;
    s8 b;
    Gouraud(const u16 raw_data) {
        r = (raw_data & 0x1F) - 0x10;
        g = ((raw_data & 0x3E0) >> 5) - 0x10;
        b = ((raw_data & 0x7C00) >> 10) - 0x10;
    }
    Gouraud() {
        r = 0;
        g = 0;
        b = 0;
    }
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
    s16 x, y;
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
    u8 r, g, b, a; ///< Color.
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
    float s, t; ///< Texture coordinates.
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
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  Dots4Bits
///
/// \brief  32 bits register splitted in 4 bits dots components.
///
/// \author Runik
/// \date   26/03/2021
////////////////////////////////////////////////////////////////////////////////////////////////////

class Dots4Bits : public Register {
  public:
    using Register::Register;
    inline static const auto dot_0 = BitRange<u8>{28, 31};
    inline static const auto dot_1 = BitRange<u8>{24, 27};
    inline static const auto dot_2 = BitRange<u8>{20, 23};
    inline static const auto dot_3 = BitRange<u8>{16, 19};
    inline static const auto dot_4 = BitRange<u8>{12, 15};
    inline static const auto dot_5 = BitRange<u8>{8, 11};
    inline static const auto dot_6 = BitRange<u8>{4, 7};
    inline static const auto dot_7 = BitRange<u8>{0, 3};
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  Dots6Bits
///
/// \brief  32 bits register splitted in 6 bits dots components.
///
/// \author Runik
/// \date   15/05/2021
////////////////////////////////////////////////////////////////////////////////////////////////////

class Dots6Bits : public Register {
  public:
    using Register::Register;
    inline static const auto dot_0 = BitRange<u8>{24, 29};
    inline static const auto dot_1 = BitRange<u8>{16, 21};
    inline static const auto dot_2 = BitRange<u8>{8, 13};
    inline static const auto dot_3 = BitRange<u8>{0, 5};
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  Dots7Bits
///
/// \brief  32 bits register splitted in 7 bits dots components.
///
/// \author Runik
/// \date   15/05/2021
////////////////////////////////////////////////////////////////////////////////////////////////////

class Dots7Bits : public Register {
  public:
    using Register::Register;
    inline static const auto dot_0 = BitRange<u8>{24, 30};
    inline static const auto dot_1 = BitRange<u8>{16, 22};
    inline static const auto dot_2 = BitRange<u8>{8, 14};
    inline static const auto dot_3 = BitRange<u8>{0, 6};
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  Dots8Bits
///
/// \brief  32 bits register splitted in 8 bits dots components.
///
/// \author Runik
/// \date   15/05/2021
////////////////////////////////////////////////////////////////////////////////////////////////////

class Dots8Bits : public Register {
  public:
    using Register::Register;
    inline static const auto dot_0 = BitRange<u8>{24, 31};
    inline static const auto dot_1 = BitRange<u8>{16, 23};
    inline static const auto dot_2 = BitRange<u8>{8, 15};
    inline static const auto dot_3 = BitRange<u8>{0, 7};
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  Dots16Bits
///
/// \brief  32 bits register splitted in 16 bits dots components..
///
/// \author Runik
/// \date   15/05/2021
////////////////////////////////////////////////////////////////////////////////////////////////////

class Dots16Bits : public Register {
  public:
    using Register::Register;
    inline static const auto dot_0 = BitRange<u16>{16, 31};
    inline static const auto dot_1 = BitRange<u16>{0, 15};
};
} // namespace saturnin::video
