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
#include <saturnin/src/bit_register.h>

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

enum class Layer {
    nbg0      = 0,
    rbg1      = 0,
    nbg1      = 1,
    exbg      = 1,
    nbg2      = 2,
    nbg3      = 3,
    rbg0      = 4,
    back      = 5,
    sprite    = 6,
    plane     = 7,
    undefined = -1
};

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
/// \struct	DataExtraction
///
/// \brief	Utility to extract data from a 32 bits register.
///
/// \author	Runik
/// \date	11/03/2023
////////////////////////////////////////////////////////////////////////////////////////////////////

struct DataExtraction {
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	As16Bits
    ///
    /// \brief	Extracts 16 bits chunks of data from a 32 bits register.
    ///
    /// \author	Runik
    /// \date	11/03/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct As16Bits {
        using PosType     = Pos<u32, As16Bits>;
        using BitsType    = Bits<u32, As16Bits>;
        using MaskedType  = Masked<u32, As16Bits>;
        using ShiftedType = Shifted<u32, As16Bits>;

        static constexpr PosType dot0_pos = PosType(16);
        static constexpr PosType dot1_pos = PosType(0);

        static constexpr u32 data_mask = 0xFFFF;

        static constexpr ShiftedType dot0_shift = ShiftedType(data_mask, dot0_pos);
        static constexpr ShiftedType dot1_shift = ShiftedType(data_mask, dot1_pos);
    };
    using As16BitsType = Reg<u32, As16Bits>;
    As16BitsType as_16bits;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	As11Bits
    ///
    /// \brief	Extracts 11 bits chunks of data from a 32 bits register.
    ///
    /// \author	Runik
    /// \date	18/07/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct As11Bits {
        using PosType     = Pos<u32, As11Bits>;
        using BitsType    = Bits<u32, As11Bits>;
        using MaskedType  = Masked<u32, As11Bits>;
        using ShiftedType = Shifted<u32, As11Bits>;

        static constexpr PosType dot0_pos = PosType(16);
        static constexpr PosType dot1_pos = PosType(0);

        static constexpr u32 data_mask = 0x7FF;

        static constexpr ShiftedType dot0_shift = ShiftedType(data_mask, dot0_pos);
        static constexpr ShiftedType dot1_shift = ShiftedType(data_mask, dot1_pos);
    };
    using As11BitsType = Reg<u32, As11Bits>;
    As11BitsType as_11bits;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	As8Bits
    ///
    /// \brief	Extracts 8 bits chunks of data from a 32 bits register.
    ///
    /// \author	Runik
    /// \date	11/03/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct As8Bits {
        using PosType     = Pos<u32, As8Bits>;
        using BitsType    = Bits<u32, As8Bits>;
        using MaskedType  = Masked<u32, As8Bits>;
        using ShiftedType = Shifted<u32, As8Bits>;

        static constexpr PosType dot0_pos = PosType(24);
        static constexpr PosType dot1_pos = PosType(16);
        static constexpr PosType dot2_pos = PosType(8);
        static constexpr PosType dot3_pos = PosType(0);

        static constexpr u32 data_mask = 0xFF;

        static constexpr ShiftedType dot0_shift = ShiftedType(data_mask, dot0_pos);
        static constexpr ShiftedType dot1_shift = ShiftedType(data_mask, dot1_pos);
        static constexpr ShiftedType dot2_shift = ShiftedType(data_mask, dot2_pos);
        static constexpr ShiftedType dot3_shift = ShiftedType(data_mask, dot3_pos);
    };
    using As8BitsType = Reg<u32, As8Bits>;
    As8BitsType as_8bits;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	As7Bits
    ///
    /// \brief	Extracts 7 bits chunks of data from a 32 bits register.
    ///
    /// \author	Runik
    /// \date	11/03/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct As7Bits {
        using PosType     = Pos<u32, As7Bits>;
        using BitsType    = Bits<u32, As7Bits>;
        using MaskedType  = Masked<u32, As7Bits>;
        using ShiftedType = Shifted<u32, As7Bits>;

        static constexpr PosType dot0_pos = PosType(24);
        static constexpr PosType dot1_pos = PosType(16);
        static constexpr PosType dot2_pos = PosType(8);
        static constexpr PosType dot3_pos = PosType(0);

        static constexpr u32 data_mask = 0x7F;

        static constexpr ShiftedType dot0_shift = ShiftedType(data_mask, dot0_pos);
        static constexpr ShiftedType dot1_shift = ShiftedType(data_mask, dot1_pos);
        static constexpr ShiftedType dot2_shift = ShiftedType(data_mask, dot2_pos);
        static constexpr ShiftedType dot3_shift = ShiftedType(data_mask, dot3_pos);
    };
    using As7BitsType = Reg<u32, As7Bits>;
    As7BitsType as_7bits;

    struct As6Bits {
        using PosType     = Pos<u32, As6Bits>;
        using BitsType    = Bits<u32, As6Bits>;
        using MaskedType  = Masked<u32, As6Bits>;
        using ShiftedType = Shifted<u32, As6Bits>;

        static constexpr PosType dot0_pos = PosType(24);
        static constexpr PosType dot1_pos = PosType(16);
        static constexpr PosType dot2_pos = PosType(8);
        static constexpr PosType dot3_pos = PosType(0);

        static constexpr u32 data_mask = 0x3F;

        static constexpr ShiftedType dot0_shift = ShiftedType(data_mask, dot0_pos);
        static constexpr ShiftedType dot1_shift = ShiftedType(data_mask, dot1_pos);
        static constexpr ShiftedType dot2_shift = ShiftedType(data_mask, dot2_pos);
        static constexpr ShiftedType dot3_shift = ShiftedType(data_mask, dot3_pos);
    };
    using As6BitsType = Reg<u32, As6Bits>;
    As6BitsType as_6bits;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	As4Bits
    ///
    /// \brief	Extracts 4 bits chunks of data from a 32 bits register.
    ///
    /// \author	Runik
    /// \date	11/03/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct As4Bits {
        using PosType     = Pos<u32, As4Bits>;
        using BitsType    = Bits<u32, As4Bits>;
        using MaskedType  = Masked<u32, As4Bits>;
        using ShiftedType = Shifted<u32, As4Bits>;

        static constexpr PosType dot0_pos = PosType(28);
        static constexpr PosType dot1_pos = PosType(24);
        static constexpr PosType dot2_pos = PosType(20);
        static constexpr PosType dot3_pos = PosType(16);
        static constexpr PosType dot4_pos = PosType(12);
        static constexpr PosType dot5_pos = PosType(8);
        static constexpr PosType dot6_pos = PosType(4);
        static constexpr PosType dot7_pos = PosType(0);

        static constexpr u32 data_mask = 0xF;

        static constexpr ShiftedType dot0_shift = ShiftedType(data_mask, dot0_pos);
        static constexpr ShiftedType dot1_shift = ShiftedType(data_mask, dot1_pos);
        static constexpr ShiftedType dot2_shift = ShiftedType(data_mask, dot2_pos);
        static constexpr ShiftedType dot3_shift = ShiftedType(data_mask, dot3_pos);
        static constexpr ShiftedType dot4_shift = ShiftedType(data_mask, dot4_pos);
        static constexpr ShiftedType dot5_shift = ShiftedType(data_mask, dot5_pos);
        static constexpr ShiftedType dot6_shift = ShiftedType(data_mask, dot6_pos);
        static constexpr ShiftedType dot7_shift = ShiftedType(data_mask, dot7_pos);
    };
    using As4BitsType = Reg<u32, As4Bits>;
    As4BitsType as_4bits;
};

} // namespace saturnin::video
