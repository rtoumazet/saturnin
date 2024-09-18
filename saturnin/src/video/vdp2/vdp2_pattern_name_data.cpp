//
// vdp2_pattern_name_data.cpp
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

#include <saturnin/src/pch.h>
#include <saturnin/src/video/vdp2/vdp2.h>

namespace saturnin::video {

auto getPatternNameData2Words(const u32 data, [[maybe_unused]] const ScrollScreenStatus& screen) -> PatternNameData {
    auto pattern_name_data                      = PatternNameData{};
    auto reg                                    = PatternNameData2WordsType{data};
    pattern_name_data.character_number          = static_cast<u16>(reg >> PatternNameData2Words::character_number_shft);
    pattern_name_data.palette_number            = static_cast<u16>(reg >> PatternNameData2Words::palette_number_shft);
    pattern_name_data.special_color_calculation = static_cast<u8>(reg >> PatternNameData2Words::special_color_calculation_shft);
    pattern_name_data.special_priority          = static_cast<u8>(reg >> PatternNameData2Words::special_priority_shft);
    pattern_name_data.is_horizontally_flipped   = reg >> PatternNameData2Words::horizontal_flip_shft;
    pattern_name_data.is_vertically_flipped     = reg >> PatternNameData2Words::vertical_flip_shft;
    return pattern_name_data;
};

auto getPatternNameData1Word1Cell16Colors10Bits(const u32 data, const ScrollScreenStatus& screen) -> PatternNameData {
    using std::byte;
    auto pattern_name_data = PatternNameData{};
    auto reg               = PatternNameData1Word1Cell16Colors10BitsType{data};

    const auto scn                     = static_cast<byte>(screen.supplementary_character_number);
    pattern_name_data.character_number = static_cast<u16>(std::to_integer<u16>(scn) << 10);
    pattern_name_data.character_number |= reg >> PatternNameData1Word1Cell16Colors10Bits::character_number_shft;

    pattern_name_data.palette_number = static_cast<u16>(static_cast<byte>(screen.supplementary_palette_number) << 4);
    pattern_name_data.palette_number |= reg >> PatternNameData1Word1Cell16Colors10Bits::palette_number_shft;

    pattern_name_data.special_color_calculation = screen.special_color_calculation;
    pattern_name_data.special_priority          = screen.special_priority;
    pattern_name_data.is_horizontally_flipped   = reg >> PatternNameData1Word1Cell16Colors10Bits::horizontal_flip_shft;
    pattern_name_data.is_vertically_flipped     = reg >> PatternNameData1Word1Cell16Colors10Bits::vertical_flip_shft;

    return pattern_name_data;
};

auto getPatternNameData1Word1Cell16Colors12Bits(const u32 data, const ScrollScreenStatus& screen) -> PatternNameData {
    using std::byte;

    auto pattern_name_data = PatternNameData{};
    auto reg               = PatternNameData1Word1Cell16Colors12BitsType{data};

    const auto scn                     = static_cast<byte>(screen.supplementary_character_number) & byte{0x1C};
    pattern_name_data.character_number = static_cast<u16>(std::to_integer<u16>(scn) << 10);
    pattern_name_data.character_number |= reg >> PatternNameData1Word1Cell16Colors12Bits::character_number_shft;

    pattern_name_data.palette_number = static_cast<u16>(static_cast<byte>(screen.supplementary_palette_number) << 4);
    pattern_name_data.palette_number |= reg >> PatternNameData1Word1Cell16Colors12Bits::palette_number_shft;

    pattern_name_data.special_color_calculation = screen.special_color_calculation;
    pattern_name_data.special_priority          = screen.special_priority;
    pattern_name_data.is_horizontally_flipped   = false;
    pattern_name_data.is_vertically_flipped     = false;

    return pattern_name_data;
};

auto getPatternNameData1Word1CellOver16Colors10Bits(const u32 data, const ScrollScreenStatus& screen) -> PatternNameData {
    using std::byte;

    auto pattern_name_data = PatternNameData{};
    auto reg               = PatternNameData1Word1CellOver16Colors10BitsType{data};

    pattern_name_data.character_number
        = static_cast<u16>(reg >> PatternNameData1Word1CellOver16Colors10Bits::palette_number_shft << 10);
    pattern_name_data.character_number |= reg >> PatternNameData1Word1CellOver16Colors10Bits::character_number_shft;

    const auto spn                   = static_cast<u16>(screen.supplementary_palette_number) << 8;
    pattern_name_data.palette_number = static_cast<u16>(spn);

    pattern_name_data.special_color_calculation = screen.special_color_calculation;
    pattern_name_data.special_priority          = screen.special_priority;
    pattern_name_data.is_horizontally_flipped   = reg >> PatternNameData1Word1CellOver16Colors10Bits::horizontal_flip_shft;
    pattern_name_data.is_vertically_flipped     = reg >> PatternNameData1Word1CellOver16Colors10Bits::vertical_flip_shft;

    return pattern_name_data;
};

auto getPatternNameData1Word1CellOver16Colors12Bits(const u32 data, const ScrollScreenStatus& screen) -> PatternNameData {
    using std::byte;

    auto pattern_name_data = PatternNameData{};
    auto reg               = PatternNameData1Word1CellOver16Colors12BitsType{data};

    const auto scn                     = static_cast<byte>(screen.supplementary_character_number) & byte{0x1C};
    pattern_name_data.character_number = static_cast<u16>(std::to_integer<u16>(scn) << 10);
    pattern_name_data.character_number |= reg >> PatternNameData1Word1CellOver16Colors12Bits::character_number_shft;

    pattern_name_data.palette_number
        = static_cast<u16>((reg >> PatternNameData1Word1CellOver16Colors12Bits::palette_number_shft) << 8);

    pattern_name_data.special_color_calculation = screen.special_color_calculation;
    pattern_name_data.special_priority          = screen.special_priority;
    pattern_name_data.is_horizontally_flipped   = false;
    pattern_name_data.is_vertically_flipped     = false;

    return pattern_name_data;
};

auto getPatternNameData1Word4Cells16Colors10Bits(const u32 data, const ScrollScreenStatus& screen) -> PatternNameData {
    using std::byte;

    auto pattern_name_data = PatternNameData{};
    auto reg               = PatternNameData1Word4Cells16Colors10BitsType{data};

    const auto scn                     = static_cast<byte>(screen.supplementary_character_number) & byte{0x1C};
    pattern_name_data.character_number = static_cast<u16>(std::to_integer<u16>(scn) << 10);
    pattern_name_data.character_number |= (reg >> PatternNameData1Word4Cells16Colors10Bits::character_number_shft << 2);
    pattern_name_data.character_number |= static_cast<u16>(static_cast<byte>(screen.supplementary_character_number) & byte{0x3});

    const auto spn                   = byte{screen.supplementary_palette_number};
    pattern_name_data.palette_number = static_cast<u16>(std::to_integer<u16>(spn) << 4);
    pattern_name_data.palette_number |= reg >> PatternNameData1Word4Cells16Colors10Bits::palette_number_shft;

    pattern_name_data.special_color_calculation = screen.special_color_calculation;
    pattern_name_data.special_priority          = screen.special_priority;
    pattern_name_data.is_horizontally_flipped   = reg >> PatternNameData1Word4Cells16Colors10Bits::horizontal_flip_shft;
    pattern_name_data.is_vertically_flipped     = reg >> PatternNameData1Word4Cells16Colors10Bits::vertical_flip_shft;

    return pattern_name_data;
};

auto getPatternNameData1Word4Cells16Colors12Bits(const u32 data, const ScrollScreenStatus& screen) -> PatternNameData {
    using std::byte;

    auto pattern_name_data = PatternNameData{};
    auto reg               = PatternNameData1Word4Cells16Colors12BitsType{data};

    const auto scn                     = static_cast<byte>(screen.supplementary_character_number) & byte{0x10};
    pattern_name_data.character_number = static_cast<u16>(std::to_integer<u16>(scn) << 10);
    pattern_name_data.character_number |= (reg >> PatternNameData1Word4Cells16Colors12Bits::character_number_shft << 2);
    const auto scn2 = static_cast<byte>(screen.supplementary_character_number) & byte{0x3};
    pattern_name_data.character_number |= std::to_integer<u16>(scn2);

    const auto spn                   = byte{screen.supplementary_palette_number};
    pattern_name_data.palette_number = static_cast<u16>(std::to_integer<u16>(spn) << 4);
    pattern_name_data.palette_number |= reg >> PatternNameData1Word4Cells16Colors12Bits::palette_number_shft;

    pattern_name_data.special_color_calculation = screen.special_color_calculation;
    pattern_name_data.special_priority          = screen.special_priority;
    pattern_name_data.is_horizontally_flipped   = false;
    pattern_name_data.is_vertically_flipped     = false;

    return pattern_name_data;
};

auto getPatternNameData1Word4CellsOver16Colors10Bits(const u32 data, const ScrollScreenStatus& screen) -> PatternNameData {
    using std::byte;

    auto pattern_name_data = PatternNameData{};
    auto reg               = PatternNameData1Word4CellsOver16Colors10BitsType{data};

    const auto scn                     = static_cast<byte>(screen.supplementary_character_number) & byte{0x1C};
    pattern_name_data.character_number = static_cast<u16>(std::to_integer<u16>(scn) << 10);
    constexpr auto cn_disp_2           = u8{2};
    pattern_name_data.character_number
        |= (reg >> PatternNameData1Word4CellsOver16Colors10Bits::character_number_shft << cn_disp_2);
    pattern_name_data.character_number |= static_cast<u16>(static_cast<byte>(screen.supplementary_character_number) & byte{0x3});

    pattern_name_data.palette_number
        = static_cast<u16>(reg >> PatternNameData1Word4CellsOver16Colors10Bits::palette_number_shft << 8);

    pattern_name_data.special_color_calculation = screen.special_color_calculation;
    pattern_name_data.special_priority          = screen.special_priority;
    pattern_name_data.is_horizontally_flipped   = reg >> PatternNameData1Word4CellsOver16Colors10Bits::horizontal_flip_shft;
    pattern_name_data.is_vertically_flipped     = reg >> PatternNameData1Word4CellsOver16Colors10Bits::vertical_flip_shft;

    return pattern_name_data;
};

auto getPatternNameData1Word4CellsOver16Colors12Bits(const u32 data, const ScrollScreenStatus& screen) -> PatternNameData {
    using std::byte;

    auto pattern_name_data = PatternNameData{};
    auto reg               = PatternNameData1Word4CellsOver16Colors12BitsType{data};

    const auto scn                     = static_cast<byte>(screen.supplementary_character_number) & byte{0x10};
    pattern_name_data.character_number = static_cast<u16>(std::to_integer<u16>(scn) << 10);
    pattern_name_data.character_number |= (reg >> PatternNameData1Word4CellsOver16Colors12Bits::character_number_shft << 2);
    const auto scn2 = static_cast<byte>(screen.supplementary_character_number) & byte{0x3};
    pattern_name_data.character_number |= std::to_integer<u16>(scn2);

    pattern_name_data.palette_number
        = static_cast<u16>(reg >> PatternNameData1Word4CellsOver16Colors12Bits::palette_number_shft << 8);

    pattern_name_data.special_color_calculation = screen.special_color_calculation;
    pattern_name_data.special_priority          = screen.special_priority;
    pattern_name_data.is_horizontally_flipped   = false;
    pattern_name_data.is_vertically_flipped     = false;

    return pattern_name_data;
};
} // namespace saturnin::video
