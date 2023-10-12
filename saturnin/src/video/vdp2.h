//
// vdp2.h
// Saturnin
//
// Copyright (c) 2020 Renaud Toumazet
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
/// \file	vdp2.h
///
/// \brief	Declares the Vdp2 class and related functions.
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <array>                           // array
#include <chrono>                          // duration
#include <saturnin/src/emulator_context.h> // EmulatorContext
#include <saturnin/src/emulator_defs.h>    // u8, u16, u32
#include <saturnin/src/emulator_modules.h> // EmulatorModules
#include <saturnin/src/locale.h>           // tr
#include <saturnin/src/log.h>              // Log
#include <saturnin/src/memory.h>
#include <saturnin/src/thread_pool.h> // ThreadPool
#include <saturnin/src/utilities.h>   // toUnderlying
#include <saturnin/src/video/vdp_common.h>
#include <saturnin/src/video/vdp2_part.h> // ScrollScreenPos
#include <saturnin/src/video/vdp2_registers.h>

// Forward declarations
namespace saturnin::core {
class Scu;
} // namespace saturnin::core

namespace saturnin::video {

class Vdp1;
class Texture;

using saturnin::core::EmulatorContext;
using saturnin::core::EmulatorModules;
using saturnin::core::Log;
using saturnin::core::Logger;
using saturnin::core::ThreadPool;

using seconds = std::chrono::duration<double>;
using milli   = std::chrono::duration<double, std::milli>;
using micro   = std::chrono::duration<double, std::micro>;

using AddressToPlaneData = std::unordered_map<u32, std::vector<Vdp2PartPosition>>;

constexpr auto vram_banks_number  = u8{4};
constexpr auto vram_bank_a0_index = u8{0};
constexpr auto vram_bank_a1_index = u8{1};
constexpr auto vram_bank_b0_index = u8{2};
constexpr auto vram_bank_b1_index = u8{3};
constexpr auto vram_timing_size   = u8{8};
using VramTiming                  = std::array<Vdp2Regs::VramAccessCommand, vram_timing_size>;

constexpr auto saturn_framebuffer_width  = u16{2048};
constexpr auto saturn_framebuffer_height = u16{2048};

constexpr auto horizontal_res_320 = u16{320};
constexpr auto horizontal_res_352 = u16{352};
constexpr auto horizontal_res_640 = u16{640};
constexpr auto horizontal_res_704 = u16{704};
constexpr auto vertical_res_224   = u16{224};
constexpr auto vertical_res_240   = u16{240};
constexpr auto vertical_res_256   = u16{256};
constexpr auto vertical_res_448   = u16{448};
constexpr auto vertical_res_480   = u16{480};
constexpr auto vertical_res_512   = u16{512};

// Saturn video resolution
//  Horizontal resolution : 320 or 352 dots (PAL or NTSC)
//  Vertical resolution :
//  - PAL
//     - Total of 313 lines
//     - 224, 240 or 256 active lines
//     - 89, 73 or 57 lines corresponding blanking lines
//  - NTSC :
//     - Total of 262 lines
//     - 224 or 240 active lines
//     - 39 or 23 corresponding blanking lines

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   TvStandard
///
/// \brief  Tv standard used.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class TvStandard : s8 {
    unknown = -1, ///< Unknown.
    pal     = 0,  ///< PAL.
    ntsc    = 1   ///< NTSC.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   ScrollScreen
///
/// \brief  Scroll screens.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class ScrollScreen { nbg0 = 0, nbg1 = 1, nbg2 = 2, nbg3 = 3, rbg0 = 4, rbg1 = 5, none = -1 };

using DisabledScrollScreen = std::unordered_map<ScrollScreen, bool>;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   ColorCount
///
/// \brief  Number of colors.
////////////////////////////////////////////////////////////////////////////////////////////////////
enum class ColorCount {
    palette_16,    ///< 16 colors
    palette_256,   ///< 256 colors
    palette_2048,  ///< 2048 colors
    rgb_32k,       ///< 32768 colors
    rgb_16m,       ///< 16770000 colors
    not_allowed,   ///< An enum constant representing the not allowed option
    not_set,       ///< An enum constant representing the not set option
    cannot_display ///< An enum constant representing the cannot display option

};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   ScrollScreenFormat
///
/// \brief  Values that represent scroll screen formats
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class ScrollScreenFormat {
    cell,  ///< An enum constant representing the cell format.
    bitmap ///< An enum constant representing the bitmap format.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   BitmapSize
///
/// \brief  Values that represent bitmap sizes
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class BitmapSize { size_512_by_256, size_512_by_512, size_1024_by_256, size_1024_by_512, not_set };

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   ScreenMode
///
/// \brief  Values that represent TV screen modes.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class ScreenMode {
    not_set,
    normal_320_224,
    normal_320_240,
    normal_320_256,
    normal_320_448,
    normal_320_480,
    normal_320_512,
    normal_352_224,
    normal_352_240,
    normal_352_256,
    normal_352_448,
    normal_352_480,
    normal_352_512,
    hi_res_640_224,
    hi_res_640_240,
    hi_res_640_256,
    hi_res_640_448,
    hi_res_640_480,
    hi_res_640_512,
    hi_res_704_224,
    hi_res_704_240,
    hi_res_704_256,
    hi_res_704_448,
    hi_res_704_480,
    hi_res_704_512,
    exclusive_320_480,
    exclusive_352_480,
    exclusive_640_480,
    exclusive_704_480,
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   ScreenModeType
///
/// \brief  Values that represent TV screen mode types
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class ScreenModeType { not_set, normal, hi_res, exclusive };

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   ReductionSetting
///
/// \brief  Values that represent normal scroll screen reduction.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class ReductionSetting { none, up_to_one_half, up_to_one_quarter };

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   VramAccessNumber
///
/// \brief  Number of VRAM access.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class VramAccessNumber : u8 { none = 0, one = 1, two = 2, four = 4, eight = 8 };

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   PatternNameDataEnum
///
/// \brief  Values that represent the different pattern name data configurations.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class PatternNameDataEnum {
    two_words,
    one_word_1_cell_16_colors_10_bits,
    one_word_1_cell_16_colors_12_bits,
    one_word_1_cell_over_16_colors_10_bits,
    one_word_1_cell_over_16_colors_12_bits,
    one_word_4_cells_16_colors_10_bits,
    one_word_4_cells_16_colors_12_bits,
    one_word_4_cells_over_16_colors_10_bits,
    one_word_4_cells_over_16_colors_12_bits
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct TvScreenStatus
///
/// \brief  The TV screen status.
///
/// \author Runik
/// \date   21/02/2021
////////////////////////////////////////////////////////////////////////////////////////////////////

struct TvScreenStatus {
    bool                            is_picture_displayed{false};
    Vdp2Regs::Tvmd::BorderColorMode border_color_mode{Vdp2Regs::Tvmd::BorderColorMode::displays_black};
    Vdp2Regs::Tvmd::InterlaceMode   interlace_mode{Vdp2Regs::Tvmd::InterlaceMode::non_interlace};
    ScreenMode                      screen_mode{ScreenMode::not_set};
    ScreenModeType                  screen_mode_type{ScreenModeType::not_set};
    u16                             horizontal_res{};
    u16                             vertical_res{};
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct RamStatus
///
/// \brief  The RAM status.
///
/// \author Runik
/// \date   12/03/2021
////////////////////////////////////////////////////////////////////////////////////////////////////

struct RamStatus {
    using Vrsize = Vdp2Regs::Vrsize;
    using Ramctl = Vdp2Regs::Ramctl;
    Vrsize::VramSize                vram_size{Vrsize::VramSize::size_4_mbits};
    Ramctl::VramMode                vram_a_mode{Ramctl::VramMode::no_partition};
    Ramctl::VramMode                vram_b_mode{Ramctl::VramMode::no_partition};
    Ramctl::ColorRamMode            color_ram_mode{Ramctl::ColorRamMode::not_set};
    Ramctl::CoefficientTableStorage coefficient_table_storage{Ramctl::CoefficientTableStorage::stored_in_vram};
    Ramctl::RotationDataBankSelect  vram_a0_rotation_bank_select{Ramctl::RotationDataBankSelect::not_used};
    Ramctl::RotationDataBankSelect  vram_a1_rotation_bank_select{Ramctl::RotationDataBankSelect::not_used};
    Ramctl::RotationDataBankSelect  vram_b0_rotation_bank_select{Ramctl::RotationDataBankSelect::not_used};
    Ramctl::RotationDataBankSelect  vram_b1_rotation_bank_select{Ramctl::RotationDataBankSelect::not_used};
};

struct ColorOffset {
    ColorS16 as_s16;
    ColorF   as_float;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct ScrollScreenStatus
///
/// \brief  Status of a scroll screen, base for NBG and RBG.
///
/// \author Runik
/// \date   23/02/2021
////////////////////////////////////////////////////////////////////////////////////////////////////
struct ScrollScreenStatus {
    ScrollScreen scroll_screen{};                    ///< The scroll screen value
    bool         is_display_enabled{};               ///< True when displayed.
    bool         is_transparency_code_valid{};       ///< True when transparency code is valid.
    bool         is_transparency_code_valid_dirty{}; ///< True when transparency code was changed.

    ScrollScreenFormat format{ScrollScreenFormat::cell};            ///< Cell or bitmap.
    ColorCount         character_color_number{ColorCount::not_set}; ///< Color number.
    u16                color_ram_address_offset{};                  ///< The color ram address offset.
    u8                 priority_number{};                           ///< The priority number.

    // Bitmap
    BitmapSize bitmap_size{BitmapSize::not_set};   ///< Size of the bitmap.
    u8         bitmap_palette_number{0};           ///< The bitmap palette number.
    u8         bitmap_special_priority{};          ///< Bitmap special priority bit
    u8         bitmap_special_color_calculation{}; ///< Bitmap special color calculation bit
    u32        bitmap_start_address{0};            ///< The bitmap start address.

    // Cell
    u8                        map_size{};                                     ///< Size of the map (2*2 for NBG / 4*4 for RBG)
    u8                        map_offset{};                                   ///< The map offset
    Vdp2Regs::Plsz::PlaneSize plane_size{Vdp2Regs::Plsz::PlaneSize::not_set}; ///< Size of the plane (1*1, 2*1 or 2*2 pages)
    u16                       page_size{};                                    ///< Size of the page / pattern name table
    u32                       cells_number{};                                 ///< Total number of cells
    Vdp2Regs::Pcnxx::PatternNameDataSize           pattern_name_data_size{};  ///< Size of the pattern name data (1 or 2 words)
    Vdp2Regs::Pcnxx::CharacterNumberSupplementMode character_number_supplement_mode{
        Vdp2Regs::Pcnxx::CharacterNumberSupplementMode::character_number_10_bits}; ///< 10 bits/12 bits
    u8                      special_priority{};                                    ///< Special priority bit
    u8                      special_color_calculation{};                           ///< Special color calculation bit
    u8                      supplementary_palette_number{};                        ///< Supplementary palette number bit
    u8                      supplementary_character_number{};                      ///< Supplementary character number bit
    Vdp2Regs::CharacterSize character_pattern_size{}; ///< Size of the character pattern (1*1 or 2*2 cells)
    u16                     cell_size{};              ///< Size of the cell (8*8 dots)

    // Positioning variables
    ScreenOffset plane_screen_offset{};             ///< Offset of one plane in cell units.
    ScreenOffset page_screen_offset{};              ///< Offset of one page in cell units.
    ScreenOffset character_pattern_screen_offset{}; ///< Offset of one character pattern in cell units.
    ScreenOffset cell_screen_offset{8, 8};          ///< Offset of one cell in cell units.

    u32 plane_a_start_address{}; ///< The plane A start address
    u32 plane_b_start_address{}; ///< The plane B start address
    u32 plane_c_start_address{}; ///< The plane C start address
    u32 plane_d_start_address{}; ///< The plane D start address

    // Rotation specifics
    u32 plane_e_start_address{}; ///< The plane E start address
    u32 plane_f_start_address{}; ///< The plane F start address
    u32 plane_g_start_address{}; ///< The plane G start address
    u32 plane_h_start_address{}; ///< The plane H start address
    u32 plane_i_start_address{}; ///< The plane I start address
    u32 plane_j_start_address{}; ///< The plane J start address
    u32 plane_k_start_address{}; ///< The plane K start address
    u32 plane_l_start_address{}; ///< The plane L start address
    u32 plane_m_start_address{}; ///< The plane M start address
    u32 plane_n_start_address{}; ///< The plane N start address
    u32 plane_o_start_address{}; ///< The plane O start address
    u32 plane_p_start_address{}; ///< The plane P start address

    // Srolling data
    u16 screen_scroll_horizontal_integer{};    ///< Horizontal screen scroll integer part.
    u8  screen_scroll_horizontal_fractional{}; ///< Horizontal screen scroll fractional part.
    u16 screen_scroll_vertical_integer{};      ///< Vertical screen scroll vertical integer part.
    u8  screen_scroll_vertical_fractional{};   ///< Vertical screen scroll fractional part.
    u16 scroll_offset_horizontal{};            ///< Horizontal scroll offset.
    u16 scroll_offset_vertical{};              ///< Vertical scroll offset.

    // Color offset data
    bool        is_color_offset_enabled{}; ///< True when color offset is enabled.
    ColorOffset color_offset;              ///< Color offset data.

    u16 total_screen_scroll_width{};  ///< Width of the screen scroll in pixels.
    u16 total_screen_scroll_height{}; ///< Height of the screen scroll in pixels.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct PatternNameData
///
/// \brief  Pattern name data.
///
/// \author Runik
/// \date   15/03/2021
////////////////////////////////////////////////////////////////////////////////////////////////////

struct PatternNameData {
    u16  character_number;
    u16  palette_number;
    u8   special_priority;
    u8   special_color_calculation;
    bool is_vertically_flipped;
    bool is_horizontally_flipped;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct CellData
///
/// \brief  Stores cell data needed for parallel read.
///
/// \author Runik
/// \date   19/08/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

struct CellData {
    PatternNameData pnd;
    u32             cell_address;
    ScreenOffset    screen_offset;
    size_t          key;
    CellData(const PatternNameData pnd, const u32 cell_address, const ScreenOffset screen_offset, const size_t key) :
        pnd(pnd),
        cell_address(cell_address),
        screen_offset(screen_offset),
        key(key){};
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct PlaneDetail
///
/// \brief  Stores details of a plane, used for rendering.
///
/// \author Runik
/// \date   06/10/2023
////////////////////////////////////////////////////////////////////////////////////////////////////

struct PlaneDetail {
    u32          start_address;
    ScreenOffset screen_offset;
    PlaneDetail(const u32 sa, const ScreenOffset so) : start_address(sa), screen_offset(so){};
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct	PatternNameData2Words
///
/// \brief	Pattern Name Data - 2 words configuration.
///
/// \author	Runik
/// \date	12/04/2023
////////////////////////////////////////////////////////////////////////////////////////////////////

struct PatternNameData2Words {
    GENERATE_USING(PatternNameData2Words, u32);

    GENERATE_BIT_WITHOUT_ENUM(vertical_flip, 31, 0x1);             ///< Defines the vertical flip bit.
    GENERATE_BIT_WITHOUT_ENUM(horizontal_flip, 30, 0x1);           ///< Defines the horizontal flip bit.
    GENERATE_BIT_WITHOUT_ENUM(special_priority, 29, 0x1);          ///< Defines the special priority bit.
    GENERATE_BIT_WITHOUT_ENUM(special_color_calculation, 28, 0x1); ///< Defines the special color calculation bit.
    GENERATE_BIT_WITHOUT_ENUM(palette_number, 16, 0x7F);           ///< Defines the palette number.
    GENERATE_BIT_WITHOUT_ENUM(character_number, 0, 0x7FFF);        ///< Defines the character number.
};
using PatternNameData2WordsType = Reg<u32, PatternNameData2Words>;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct	PatternNameData1Word1Cell16Colors10Bits
///
/// \brief	Pattern Name Data - 1 word, 1 cell, 16 colors, 10 bits configuration.
///
/// \author	Runik
/// \date	12/04/2023
////////////////////////////////////////////////////////////////////////////////////////////////////

struct PatternNameData1Word1Cell16Colors10Bits {
    GENERATE_USING(PatternNameData1Word1Cell16Colors10Bits, u32);

    GENERATE_BIT_WITHOUT_ENUM(palette_number, 12, 0b1111); ///< Defines the palette number.
    GENERATE_BIT_WITHOUT_ENUM(vertical_flip, 11, 0b1);     ///< Defines the vertical flip bit.
    GENERATE_BIT_WITHOUT_ENUM(horizontal_flip, 10, 0b1);   ///< Defines the horizontal flip bit.
    GENERATE_BIT_WITHOUT_ENUM(character_number, 0, 0x3FF); ///< Defines the character number.
};
using PatternNameData1Word1Cell16Colors10BitsType = Reg<u32, PatternNameData1Word1Cell16Colors10Bits>;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct	PatternNameData1Word1Cell16Colors12Bits
///
/// \brief	Pattern Name Data - 1 word, 1 cell, 16 colors, 12 bits configuration.
///
/// \author	Runik
/// \date	13/04/2023
////////////////////////////////////////////////////////////////////////////////////////////////////

struct PatternNameData1Word1Cell16Colors12Bits {
    GENERATE_USING(PatternNameData1Word1Cell16Colors12Bits, u32);

    GENERATE_BIT_WITHOUT_ENUM(palette_number, 12, 0b1111); ///< Defines the palette number.
    GENERATE_BIT_WITHOUT_ENUM(character_number, 0, 0xFFF); ///< Defines the character number.
};
using PatternNameData1Word1Cell16Colors12BitsType = Reg<u32, PatternNameData1Word1Cell16Colors12Bits>;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct	PatternNameData1Word1CellOver16Colors10Bits
///
/// \brief	Pattern Name Data - 1 word, 1 cell, over 16 colors, 10 bits configuration.
///
/// \author	Runik
/// \date	13/04/2023
////////////////////////////////////////////////////////////////////////////////////////////////////

struct PatternNameData1Word1CellOver16Colors10Bits {
    GENERATE_USING(PatternNameData1Word1CellOver16Colors10Bits, u32);

    GENERATE_BIT_WITHOUT_ENUM(palette_number, 12, 0b111);  ///< Defines the palette number.
    GENERATE_BIT_WITHOUT_ENUM(vertical_flip, 11, 0b1);     ///< Defines the vertical flip bit.
    GENERATE_BIT_WITHOUT_ENUM(horizontal_flip, 10, 0b1);   ///< Defines the horizontal flip bit.
    GENERATE_BIT_WITHOUT_ENUM(character_number, 0, 0x3FF); ///< Defines the character number.
};
using PatternNameData1Word1CellOver16Colors10BitsType = Reg<u32, PatternNameData1Word1CellOver16Colors10Bits>;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct	PatternNameData1Word1CellOver16Colors12Bits
///
/// \brief	Pattern Name Data - 1 word, 1 cell, over 16 colors, 10 bits configuration.
///
/// \author	Runik
/// \date	13/04/2023
////////////////////////////////////////////////////////////////////////////////////////////////////

struct PatternNameData1Word1CellOver16Colors12Bits {
    GENERATE_USING(PatternNameData1Word1CellOver16Colors12Bits, u32);

    GENERATE_BIT_WITHOUT_ENUM(palette_number, 12, 0b111);  ///< Defines the palette number.
    GENERATE_BIT_WITHOUT_ENUM(character_number, 0, 0xFFF); ///< Defines the character number.
};
using PatternNameData1Word1CellOver16Colors12BitsType = Reg<u32, PatternNameData1Word1CellOver16Colors12Bits>;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct	PatternNameData1Word4Cells16Colors10Bits
///
/// \brief	Pattern Name Data - 1 word, 4 cells, 16 colors, 10 bits configuration.
///
/// \author	Runik
/// \date	13/04/2023
////////////////////////////////////////////////////////////////////////////////////////////////////

struct PatternNameData1Word4Cells16Colors10Bits {
    GENERATE_USING(PatternNameData1Word4Cells16Colors10Bits, u32);

    GENERATE_BIT_WITHOUT_ENUM(palette_number, 12, 0b1111); ///< Defines the palette number.
    GENERATE_BIT_WITHOUT_ENUM(vertical_flip, 11, 0b1);     ///< Defines the vertical flip bit.
    GENERATE_BIT_WITHOUT_ENUM(horizontal_flip, 10, 0b1);   ///< Defines the horizontal flip bit.
    GENERATE_BIT_WITHOUT_ENUM(character_number, 0, 0x3FF); ///< Defines the character number.
};
using PatternNameData1Word4Cells16Colors10BitsType = Reg<u32, PatternNameData1Word4Cells16Colors10Bits>;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct	PatternNameData1Word4Cells16Colors12Bits
///
/// \brief	Pattern Name Data - 1 word, 4 cells, 16 colors, 12 bits configuration.
///
/// \author	Runik
/// \date	13/04/2023
////////////////////////////////////////////////////////////////////////////////////////////////////

struct PatternNameData1Word4Cells16Colors12Bits {
    GENERATE_USING(PatternNameData1Word4Cells16Colors12Bits, u32);

    GENERATE_BIT_WITHOUT_ENUM(palette_number, 12, 0b1111); ///< Defines the palette number.
    GENERATE_BIT_WITHOUT_ENUM(character_number, 0, 0xFFF); ///< Defines the character number.
};
using PatternNameData1Word4Cells16Colors12BitsType = Reg<u32, PatternNameData1Word4Cells16Colors12Bits>;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct	PatternNameData1Word4CellsOver16Colors10Bits
///
/// \brief	Pattern Name Data - 1 word, 4 cells, over 16 colors, 10 bits configuration.
///
/// \author	Runik
/// \date	13/04/2023
////////////////////////////////////////////////////////////////////////////////////////////////////

struct PatternNameData1Word4CellsOver16Colors10Bits {
    GENERATE_USING(PatternNameData1Word4CellsOver16Colors10Bits, u32);

    GENERATE_BIT_WITHOUT_ENUM(palette_number, 12, 0b111);  ///< Defines the palette number.
    GENERATE_BIT_WITHOUT_ENUM(vertical_flip, 11, 0b1);     ///< Defines the vertical flip bit.
    GENERATE_BIT_WITHOUT_ENUM(horizontal_flip, 10, 0b1);   ///< Defines the horizontal flip bit.
    GENERATE_BIT_WITHOUT_ENUM(character_number, 0, 0x3FF); ///< Defines the character number.
};
using PatternNameData1Word4CellsOver16Colors10BitsType = Reg<u32, PatternNameData1Word4CellsOver16Colors10Bits>;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct	PatternNameData1Word4CellsOver16Colors12Bits
///
/// \brief	Pattern Name Data - 1 word, 4 cells, 16 colors, 12 bits configuration.
///
/// \author	Runik
/// \date	13/04/2023
////////////////////////////////////////////////////////////////////////////////////////////////////

struct PatternNameData1Word4CellsOver16Colors12Bits {
    GENERATE_USING(PatternNameData1Word4CellsOver16Colors12Bits, u32);

    GENERATE_BIT_WITHOUT_ENUM(palette_number, 12, 0b111);  ///< Defines the palette number.
    GENERATE_BIT_WITHOUT_ENUM(character_number, 0, 0xFFF); ///< Defines the character number.
};
using PatternNameData1Word4CellsOver16Colors12BitsType = Reg<u32, PatternNameData1Word4CellsOver16Colors12Bits>;

class Vdp2 {
  public:
    ///@{
    /// Constructors / Destructors
    Vdp2() = delete;
    explicit Vdp2(EmulatorContext* ec) : modules_(ec){};
    Vdp2(const Vdp2&)                      = delete;
    Vdp2(Vdp2&&)                           = delete;
    auto operator=(const Vdp2&) & -> Vdp2& = delete;
    auto operator=(Vdp2&&) & -> Vdp2&      = delete;
    ~Vdp2()                                = default;
    ///@}

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Vdp2::initialize();
    ///
    /// \brief  Initializes the VDP2
    ///
    /// \author Runik
    /// \date   16/05/2020
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void initialize();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Vdp2::run(u8 sh2_cycles);
    ///
    /// \brief  Runs the VDP2 for the given number of SH2 cycles
    ///
    /// \author Runik
    /// \date   17/05/2020
    ///
    /// \param  cycles  The cycles to run.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void run(u8 cycles);

    template<typename T>
    void writeRegisters(const u32 addr, const T data) {
        core::Log::warning(Logger::vdp2, core::tr("{}bits register write {:#0x}"), sizeof(T) * 8, addr);
    }

    // 8 bits specialization
    template<>
    void writeRegisters<u8>(const u32 addr, const u8 data) {
        write8(addr, data);
    }

    // 16 bits specialization
    template<>
    void writeRegisters<u16>(const u32 addr, const u16 data) {
        write16(addr, data);
    }
    // 32 bits specialization
    template<>
    void writeRegisters<u32>(const u32 addr, const u32 data) {
        write32(addr, data);
    }
    template<typename T>
    auto readRegisters(const u32 addr) -> T {
        core::Log::warning(Logger::vdp2, core::tr("{}bits register read {:#0x}"), sizeof(T) * 8, addr);
        return 0;
    }
    // 16 bits specialization
    template<>
    auto readRegisters<u16>(const u32 addr) -> u16 {
        return read16(addr);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto Vdp2::getRegisters() const -> const AddressToNameMap&;
    ///
    /// \brief  Gets registers content for debug purpose.
    ///
    /// \author Runik
    /// \date   20/05/2020
    ///
    /// \returns    The registers content.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    [[nodiscard]] auto getRegisters() const -> const AddressToNameMap&;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Vdp2::onSystemClockUpdate();
    ///
    /// \brief  Executes the actions needed after a system clock change.
    ///
    /// \author Runik
    /// \date   29/05/2020
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void onSystemClockUpdate();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Vdp2::calculateDisplayDuration();
    ///
    /// \brief  Calculates everything related to the display duration (frame, vblank, hblank length ...)
    ///
    /// \author Runik
    /// \date   01/06/2020
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void calculateDisplayDuration();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Vdp2::onVblankIn();
    ///
    /// \brief  Handles everything that needs to be done on VBlankIn.
    ///
    /// \author Runik
    /// \date   19/02/2021
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void onVblankIn();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto Vdp2::vdp2Parts(const ScrollScreen s)  const -> const std::vector<std::unique_ptr<video::BaseRenderingPart>>
    ///
    /// \brief  Returns the VDP2 parts of a scroll screen.
    ///
    /// \author Runik
    /// \date   15/04/2021
    ///
    /// \param  s   A ScrollScreen.
    ///
    /// \returns    A reference to a const std::vector&lt;Vdp2Part&gt;
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto vdp2Parts(const ScrollScreen s) const -> const std::vector<video::Vdp2Part>& {
        return vdp2_parts_[utilities::toUnderlying(s)];
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto Vdp2::getSpriteColorAddressOffset() const -> u16;
    ///
    /// \brief  Gets the color address offset of sprites.
    ///
    /// \author Runik
    /// \date   14/05/2021
    ///
    /// \returns    The sprite color address offset.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto getSpriteColorAddressOffset() -> u16;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto Vdp2::getColorRamMode() const -> ColorRamMode
    ///
    /// \brief  Gets current color RAM mode.
    ///
    /// \author Runik
    /// \date   14/05/2021
    ///
    /// \returns    The color ram mode.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto getColorRamMode() const -> Vdp2Regs::Ramctl::ColorRamMode { return ram_status_.color_ram_mode; }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto Vdp2::getSpriteControlRegister() const -> SpriteControl
    ///
    /// \brief  Gets the Sprite Control register.
    ///
    /// \author Runik
    /// \date   03/09/2021
    ///
    /// \returns    The sprite control register.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto getSpriteControlRegister() const -> Vdp2Regs::SpctlType { return regs_.spctl; }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto Vdp2::getSpritePriority(const u8 register_number) const -> u8;
    ///
    /// \brief  Gets sprite priority from the register number.
    ///
    /// \author Runik
    /// \date   16/09/2021
    ///
    /// \param  register_number The register number.
    ///
    /// \returns    The sprite priority.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto getSpritePriority(const u8 register_number) const -> u8;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	auto Vdp2::getColorOffset(const Layer layer) -> ColorOffset;
    ///
    /// \brief	Returns the color offset for the specified layer.
    ///
    /// \author	Runik
    /// \date	11/08/2022
    ///
    /// \param 	layer	The layer.
    ///
    /// \returns	The color offset.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto getColorOffset(const Layer layer) -> ColorOffset;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	void Vdp2::refreshRegisters();
    ///
    /// \brief	Refresh registers from memory, useful after a SCU DMA transfer.
    ///
    /// \author	Runik
    /// \date	12/02/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void refreshRegisters();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn template<typename T> auto Vdp2::readColor(const u32 color_address) -> Color
    ///
    /// \brief  Reads a color
    ///
    /// \tparam T   u32 for 32 bits color, u16 for 16 bits colors.
    /// \param  color_address   The color address.
    ///
    /// \returns    The color read.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    template<typename T>
    auto readColor(const u32 color_address) -> Color {
        return Color(modules_.memory()->read<T>(color_address));
    };

    template<typename T>
    auto readColor(const u32 color_address, std::span<const u8> cram) -> Color {
        return Color(0);
    };
    template<>
    auto readColor<u16>(const u32 color_address, std::span<const u8> cram) -> Color {
        return Color(utilities::readAs16(cram.subspan(color_address & core::vdp2_cram_memory_mask, 2)));
    };
    template<>
    auto readColor<u32>(const u32 color_address, std::span<const u8> cram) -> Color {
        return Color(utilities::readAs32(cram.subspan(color_address & core::vdp2_cram_memory_mask, 4)));
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Vdp2::clearRenderData(const ScrollScreen s);
    ///
    /// \brief  Clears data from the VDP2 memory for the selected scroll screen.
    ///
    /// \author Runik
    /// \date   01/07/2021
    ///
    /// \param  s   A ScrollScreen to process.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void clearRenderData(const ScrollScreen s);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	auto Vdp2::fps() -> std::string;
    ///
    /// \brief	Gets the calculated FPS.
    ///
    /// \author	Runik
    /// \date	02/06/2023
    ///
    /// \returns	A std::string.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto fps() -> std::string;

    //--------------------------------------------------------------------------------------------------------------
    // DEBUG methods, defined in vdp2_debug.cpp
    //--------------------------------------------------------------------------------------------------------------

    ///@{
    /// \name Various Vdp2 debug functions
    auto        getDebugGlobalMainData() const -> std::vector<LabelValue>;
    auto        getDebugRamMainData() const -> std::vector<LabelValue>;
    auto        getDebugVramAccessMainData() const -> std::vector<LabelValue>;
    auto        getDebugVramAccessBanks() const -> std::vector<VramTiming>;
    auto        getDebugVramAccessBanksUsed() const -> std::array<bool, vram_banks_number>;
    auto        getDebugVramAccessBanksName() const -> std::vector<std::string>;
    static auto getDebugVramAccessCommandDescription(const Vdp2Regs::VramAccessCommand command) -> LabelValue;
    auto        getDebugScrollScreenData(const ScrollScreen s) -> std::optional<std::vector<LabelValue>>;
    auto        screenInDebug() const -> ScrollScreen;
    void        screenInDebug(const ScrollScreen s);
    auto        isLayerDisabled(const ScrollScreen s) -> bool;
    void        disableLayer(const ScrollScreen s, const bool is_disabled);

    ///@}

  private:
    //--------------------------------------------------------------------------------------------------------------
    // MEMORY ACCESS methods
    //--------------------------------------------------------------------------------------------------------------

    ///@{
    ///  \name Vdp2 registers accessors
    [[nodiscard]] auto read16(u32 addr) const -> u16;
    void               write8(u32 addr, u8 data);
    void               write16(u32 addr, u16 data);
    void               write32(u32 addr, u32 data);
    ///@}

    //--------------------------------------------------------------------------------------------------------------
    // MISC methods
    //--------------------------------------------------------------------------------------------------------------

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Vdp2::addToRegisterNameMap(u32 addr, const std::string& name);
    ///
    /// \brief  Adds an entry to the RegisterName map.
    ///
    /// \author Runik
    /// \date   04/06/2020
    ///
    /// \param  addr    The address.
    /// \param  name    The name.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void addToRegisterNameMap(u32 addr, const std::string& name);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Vdp2::initializeRegisterNameMap();
    ///
    /// \brief  Initializes the map that links the register address to its name.
    ///
    /// \author Runik
    /// \date   21/05/2020
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void initializeRegisterNameMap();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Vdp2::computePrecalculatedData();
    ///
    /// \brief  Computes the precalculated data
    ///
    /// \author Runik
    /// \date   21/03/2021
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void computePrecalculatedData();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Vdp2::calculateLineDuration(const micro& total_line_duration, const micro& hblank_duration);
    ///
    /// \brief  Calculates the number of cycles per line.
    ///
    /// \author Runik
    /// \date   10/03/2021
    ///
    /// \param  total_line_duration Duration of the total line.
    /// \param  hblank_duration     Duration of the hblank.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void calculateLineDuration(const micro& total_line_duration, const micro& hblank_duration);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Vdp2::updateResolution();
    ///
    /// \brief  Updates VDP2 resolution by reading internal registers.
    ///
    /// \author Runik
    /// \date   20/02/2021
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void updateResolution();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Vdp2::updateRamStatus();
    ///
    /// \brief  Updates the RAM status
    ///
    /// \author Runik
    /// \date   12/03/2021
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void updateRamStatus();

    //--------------------------------------------------------------------------------------------------------------
    // VRAM CYCLES methods
    //--------------------------------------------------------------------------------------------------------------

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto Vdp2::isScreenDisplayed(ScrollScreen s) -> bool;
    ///
    /// \brief  Query if scroll screen 's' is displayed.
    ///
    /// \author Runik
    /// \date   21/08/2020
    ///
    /// \param  s   A ScrollScreen to process.
    ///
    /// \returns    True if the scroll screen is displayed.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto isScreenDisplayed(ScrollScreen s) -> bool;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto Vdp2::isScreenDisplayLimitedByReduction(ScrollScreen s) const -> bool;
    ///
    /// \brief  Query if reduction setting prevents scroll screen 's' to be displayed
    ///
    /// \author Runik
    /// \date   13/01/2021
    ///
    /// \param  s   A ScrollScreen to process.
    ///
    /// \returns    True if reduction prevents the scroll screen to be displayed.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto isScreenDisplayLimitedByReduction(ScrollScreen s) const -> bool;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	auto Vdp2::getVramAccessByCommand(const Vdp2Regs::VramAccessCommand command, const ReductionSetting reduction) const
    /// -> u8;
    ///
    /// \brief	Gets VRAM access corresponding to the command taking into account limitations.
    ///
    /// \author	Runik
    /// \date	21/12/2020
    ///
    /// \param 	command  	The access command.
    /// \param 	reduction	The current screen reduction.
    ///
    /// \returns	Number of VRAM access for the command.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto getVramAccessByCommand(const Vdp2Regs::VramAccessCommand command, const ReductionSetting reduction) const -> u8;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	auto Vdp2::getVramBitmapReads(const VramTiming& bank_a0, const VramTiming& bank_a1, const VramTiming& bank_b0, const
    /// VramTiming& bank_b1, const Vdp2Regs::VramAccessCommand command) const -> u8;
    ///
    /// \brief	Gets VRAM bitmap reads
    ///
    /// \author	Runik
    /// \date	22/12/2020
    ///
    /// \param 	bank_a0	VRAM bank a0.
    /// \param 	bank_a1	VRAM bank a1.
    /// \param 	bank_b0	VRAM bank b0.
    /// \param 	bank_b1	VRAM bank b1.
    /// \param 	command	The VRAM access command.
    ///
    /// \returns	The VRAM bitmap reads.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto getVramBitmapReads(const VramTiming&                 bank_a0,
                            const VramTiming&                 bank_a1,
                            const VramTiming&                 bank_b0,
                            const VramTiming&                 bank_b1,
                            const Vdp2Regs::VramAccessCommand command) const -> u8;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	auto Vdp2::getVramPatternNameDataReads(const VramTiming& bank_a0, const VramTiming& bank_a1, const VramTiming&
    /// bank_b0, const VramTiming& bank_b1, const Vdp2Regs::VramAccessCommand command) const -> u8;
    ///
    /// \brief	Gets VRAM pattern name data reads
    ///
    /// \author	Runik
    /// \date	22/12/2020
    ///
    /// \param 	bank_a0	VRAM bank a0.
    /// \param 	bank_a1	VRAM bank a1.
    /// \param 	bank_b0	VRAM bank b0.
    /// \param 	bank_b1	VRAM bank b1.
    /// \param 	command	The VRAM access command.
    ///
    /// \returns	The VRAM pattern name data reads.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto getVramPatternNameDataReads(const VramTiming&                 bank_a0,
                                     const VramTiming&                 bank_a1,
                                     const VramTiming&                 bank_b0,
                                     const VramTiming&                 bank_b1,
                                     const Vdp2Regs::VramAccessCommand command) const -> u8;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	static auto Vdp2::getReductionSetting(Vdp2Regs::Zmctl::ZoomQuarter zq, Vdp2Regs::Zmctl::ZoomHalf zh) ->
    /// ReductionSetting;
    ///
    /// \brief	Returns the reduction setting for the specified parameters
    ///
    /// \author	Runik
    /// \date	11/08/2020
    ///
    /// \param 	zq	Zoom quarter reduction setting of the plane.
    /// \param 	zh	Zoom half reduction setting of the plane.
    ///
    /// \returns	The reduction setting.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static auto getReductionSetting(Vdp2Regs::Zmctl::ZoomQuarter zq, Vdp2Regs::Zmctl::ZoomHalf zh) -> ReductionSetting;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	static auto Vdp2::calculateRequiredVramCharacterPatternReads(ReductionSetting r, Vdp2Regs::CharacterColorNumber3Bits
    /// ccn) -> VramAccessNumber;
    ///
    /// \brief	Calculates the required VRAM character/bitmap pattern reads to display a screen.
    ///
    /// \author	Runik
    /// \date	10/08/2020
    ///
    /// \param 	r  	The reduction setting.
    /// \param 	ccn	The character color number.
    ///
    /// \returns	The required VRAM character pattern reads.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static auto calculateRequiredVramCharacterPatternReads(ReductionSetting r, Vdp2Regs::CharacterColorNumber3Bits ccn)
        -> VramAccessNumber;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	static auto Vdp2::calculateRequiredVramCharacterPatternReads(ReductionSetting r, Vdp2Regs::CharacterColorNumber2Bits
    /// ccn) -> VramAccessNumber;
    ///
    /// \brief	Calculates the required VRAM character/bitmap pattern reads to display a screen.
    ///
    /// \author	Runik
    /// \date	12/01/2021
    ///
    /// \param 	r  	The reduction setting.
    /// \param 	ccn	The character color number.
    ///
    /// \returns	The required VRAM character pattern reads.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static auto calculateRequiredVramCharacterPatternReads(ReductionSetting r, Vdp2Regs::CharacterColorNumber2Bits ccn)
        -> VramAccessNumber;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	static auto Vdp2::calculateRequiredVramCharacterPatternReads(ReductionSetting r, Vdp2Regs::CharacterColorNumber1Bit
    /// ccn) -> VramAccessNumber;
    ///
    /// \brief	Calculates the required VRAM character/bitmap pattern reads to display a screen.
    ///
    /// \author	Runik
    /// \date	12/01/2021
    ///
    /// \param 	r  	The reduction setting.
    /// \param 	ccn	The character color number.
    ///
    /// \returns	The required VRAM character pattern reads.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static auto calculateRequiredVramCharacterPatternReads(ReductionSetting r, Vdp2Regs::CharacterColorNumber1Bit ccn)
        -> VramAccessNumber;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static auto Vdp2::calculateRequiredVramPatternNameReads(ReductionSetting r) -> VramAccessNumber;
    ///
    /// \brief  Calculates the required VRAM pattern name reads to display a screen.
    ///
    /// \author Runik
    /// \date   11/08/2020
    ///
    /// \param  r   Reduction setting of the plane.
    ///
    /// \returns    The required VRAM pattern name reads.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static auto calculateRequiredVramPatternNameReads(ReductionSetting r) -> VramAccessNumber;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	static auto Vdp2::getPatternNameFromCharacterPattern(const Vdp2Regs::VramAccessCommand character_pattern) ->
    /// Vdp2Regs::VramAccessCommand;
    ///
    /// \brief	Gets the Pattern Name command corresponding to the Character Pattern command
    ///
    /// \author	Runik
    /// \date	23/12/2020
    ///
    /// \param 	character_pattern	Character pattern command.
    ///
    /// \returns	Pattern name command.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static auto getPatternNameFromCharacterPattern(const Vdp2Regs::VramAccessCommand character_pattern)
        -> Vdp2Regs::VramAccessCommand;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	static void Vdp2::setPatternNameAccess(const VramTiming& bank, const Vdp2Regs::VramAccessCommand pattern,
    /// std::array<bool, vram_timing_size>& pnd_access);
    ///
    /// \brief	Sets the pattern name accesses for the given bank.
    ///
    /// \author	Runik
    /// \date	23/12/2020
    ///
    /// \param 		   	bank	  	The bank to check.
    /// \param 		   	pattern   	Pattern name data used.
    /// \param [in,out]	pnd_access	Sets true for the index of the pattern name data found in the
    /// 							bank.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static void setPatternNameAccess(const VramTiming&                   bank,
                                     const Vdp2Regs::VramAccessCommand   pattern,
                                     std::array<bool, vram_timing_size>& pnd_access);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static void Vdp2::setCharacterPatternLimitations(const bool is_screen_mode_normal, const std::array<bool,
    /// vram_timing_size>& pnd_access, std::array<bool, vram_timing_size>& allowed_cpd_timing);
    ///
    /// \brief  Sets character pattern limitations.
    ///
    /// \author Runik
    /// \date   26/12/2020
    ///
    /// \param          is_screen_mode_normal       True when the TV screen mode is normal.
    /// \param          is_character_pattern_2_by_2 True when the character pattern is 2 by 2.
    /// \param          pnd_access                  True when the pattern name data is used.
    /// \param [in,out] allowed_cpd_timing          Sets true for the allowed character pattern timings.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static void setCharacterPatternLimitations(const bool                                is_screen_mode_normal,
                                               const bool                                is_character_pattern_2_by_2,
                                               const std::array<bool, vram_timing_size>& pnd_access,
                                               std::array<bool, vram_timing_size>&       allowed_cpd_timing);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	static auto Vdp2::getVramCharacterPatternDataReads(const VramTiming& bank_a0, const VramTiming& bank_a1, const
    /// VramTiming& bank_b0, const VramTiming& bank_b1, const Vdp2Regs::VramAccessCommand command, const ReductionSetting
    /// reduction, const bool is_screen_mode_normal, const bool is_using_2_by_2_cp) -> u8;
    ///
    /// \brief	Gets VRAM character pattern data reads
    ///
    /// \author	Runik
    /// \date	22/12/2020
    ///
    /// \param 	bank_a0				 	VRAM bank a0.
    /// \param 	bank_a1				 	VRAM bank a1.
    /// \param 	bank_b0				 	VRAM bank b0.
    /// \param 	bank_b1				 	VRAM bank b1.
    /// \param 	command				 	The VRAM access command.
    /// \param 	reduction			 	Reduction setting of current screen.
    /// \param 	is_screen_mode_normal	True if current screen is in normal mode.
    /// \param 	is_using_2_by_2_cp   	True if current screen uses 2 by 2 character patterns.
    ///
    /// \returns	The VRAM character pattern data reads.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static auto getVramCharacterPatternDataReads(const VramTiming&                 bank_a0,
                                                 const VramTiming&                 bank_a1,
                                                 const VramTiming&                 bank_b0,
                                                 const VramTiming&                 bank_b1,
                                                 const Vdp2Regs::VramAccessCommand command,
                                                 const ReductionSetting            reduction,
                                                 const bool                        is_screen_mode_normal,
                                                 const bool                        is_using_2_by_2_cp) -> u8;

    //--------------------------------------------------------------------------------------------------------------
    // DISPLAY methods
    //--------------------------------------------------------------------------------------------------------------

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Vdp2::populateRenderData();
    ///
    /// \brief  Populates data from the VDP2 memory before backend rendering.
    ///
    /// \author Runik
    /// \date   29/01/2021
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void populateRenderData();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto Vdp2::canScrollScreenBeDisplayed(const ScrollScreen s) const -> bool;
    ///
    /// \brief  Determine if the scroll screen can be displayed, based on others scroll screens
    ///         configuration
    ///
    /// \author Runik
    /// \date   11/03/2021
    ///
    /// \param  s   A ScrollScreen to process.
    ///
    /// \returns    True the scroll screen can be displayed, false if not.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto canScrollScreenBeDisplayed(const ScrollScreen s) const -> bool;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Vdp2::updateScrollScreenStatus(const ScrollScreen s);
    ///
    /// \brief  Updates the scroll screen data.
    ///
    /// \author Runik
    /// \date   28/02/2021
    ///
    /// \param  s   A ScrollScreen to process.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void updateScrollScreenStatus(const ScrollScreen s);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto Vdp2::calculatePlaneStartAddress(const ScrollScreen s, const u32 map_addr) -> u32;
    ///
    /// \brief  Calculates the plane start address
    ///
    /// \author Runik
    /// \date   28/02/2021
    ///
    /// \param  s           The ScrollScreen to process.
    /// \param  map_addr    The map address.
    ///
    /// \returns    The calculated plane start address.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto calculatePlaneStartAddress(const ScrollScreen s, const u32 map_addr) -> u32;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto Vdp2::getScreen(const ScrollScreen s) -> ScrollScreenStatus*;
    ///
    /// \brief  Gets the screen status of the screen.
    ///
    /// \author Runik
    /// \date   09/03/2021
    ///
    /// \param  s   A ScrollScreen to process.
    ///
    /// \returns    The screen status.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto getScreen(const ScrollScreen s) -> ScrollScreenStatus&;
    auto getScreen(const ScrollScreen s) const -> const ScrollScreenStatus&;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Vdp2::readScrollScreenData(const ScrollScreen s);
    ///
    /// \brief  Reads scroll screen data in VRAM
    ///
    /// \author Runik
    /// \date   13/03/2021
    ///
    /// \param  s   A ScrollScreen to process.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void readScrollScreenData(const ScrollScreen s);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Vdp2::readBitmapData(const ScrollScreenStatus& screen);
    ///
    /// \brief  Reads bitmap data in VRAM
    ///
    /// \author Runik
    /// \date   15/08/2021
    ///
    /// \param  screen  Current scroll screen status.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void readBitmapData(const ScrollScreenStatus& screen);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Vdp2::readPlaneData(const ScrollScreenStatus& screen, const u32 plane_address, const ScreenOffset& plane_offset);
    ///
    /// \brief  Reads plane data.
    ///
    /// \author Runik
    /// \date   13/03/2021
    ///
    /// \param  screen          Current scroll screen status.
    /// \param  plane_address   The plane address.
    /// \param  plane_offset    The plane offset in cell units.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void readPlaneData(const ScrollScreenStatus& screen, const u32 plane_address, const ScreenOffset& plane_offset);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Vdp2::readPageData(const ScrollScreenStatus& screen, const u32 page_address, const ScreenOffset& page_offset);
    ///
    /// \brief  Reads page data.
    ///
    /// \author Runik
    /// \date   14/03/2021
    ///
    /// \param  screen          Current scroll screen status.
    /// \param  page_address    The page address.
    /// \param  page_offset     The page offset in cell units.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void readPageData(const ScrollScreenStatus& screen, const u32 page_address, const ScreenOffset& page_offset);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Vdp2::readCharacterPattern(const ScrollScreenStatus& screen, const PatternNameData& pnd, const ScreenOffset&
    /// cp_offset);
    ///
    /// \brief  Reads a character pattern
    ///
    /// \author Runik
    /// \date   14/03/2021
    ///
    /// \param  screen      Current scroll screen status.
    /// \param  pnd         The pattern name data.
    /// \param  cp_offset   The character pattern offset in cell units.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void readCharacterPattern(const ScrollScreenStatus& screen, const PatternNameData& pnd, const ScreenOffset& cp_offset);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	void Vdp2::readCellDispatch(const ScrollScreenStatus& screen, const PatternNameData& pnd, const u32 cell_address,
    /// const ScreenOffset& cell_offset);
    ///
    /// \brief	Reads a cell
    ///
    /// \author	Runik
    /// \date	14/03/2021
    ///
    /// \param 	screen			Current scroll screen status.
    /// \param 	pnd				The pattern name data.
    /// \param 	cell_address	The cell address.
    /// \param 	cell_offset 	The cell offset, in cell units.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void readCellDispatch(const ScrollScreenStatus& screen,
                          const PatternNameData&    pnd,
                          const u32                 cell_address,
                          const ScreenOffset&       cell_offset);

    void readCell(const ScrollScreenStatus& screen, const PatternNameData& pnd, const u32 cell_address, const size_t key);

    void readCellMT(const ScrollScreenStatus& screen,
                    const u16                 palette_number,
                    const u32                 cell_address,
                    const size_t              key,
                    const std::span<const u8> vram,
                    const std::span<const u8> cram);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Vdp2::saveCell(const ScrollScreenStatus& screen, const PatternNameData& pnd, const u32 cell_address, const
    /// ScreenOffset& cell_offset, const size_t key);
    ///
    /// \brief  Saves a cell to the parts vector
    ///
    /// \author Runik
    /// \date   14/04/2021
    ///
    /// \param  screen          Current scroll screen status.
    /// \param  pnd             The pattern name data.
    /// \param  cell_address    The cell address.
    /// \param  cell_offset     The cell offset in cell units.
    /// \param  key             Texture key.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void saveCell(const ScrollScreenStatus& screen,
                  const PatternNameData&    pnd,
                  const u32                 cell_address,
                  const ScreenOffset&       cell_offset,
                  const size_t              key);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn template<typename T> void Vdp2::readPalette16Dot(std::vector<u8>& texture_data, const ScrollScreenStatus& screen,
    /// const u8 palette_number, const u8 dot)
    ///
    /// \brief  Reads one dot in 16 colors palette format.
    ///
    /// \tparam T   Generic type parameter.
    /// \param [in,out] texture_data    Raw texture data.
    /// \param          screen          Current scroll screen status.
    /// \param          palette_number  The palette number.
    /// \param          dot             The dot to read.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    template<typename T>
    void readPalette16Dot(std::vector<u8>&          texture_data,
                          const ScrollScreenStatus& screen,
                          const u16                 palette_number,
                          const u8                  dot) {
        const auto local_palette = palette_number << 4;
        const auto color_address = u32{cram_start_address + screen.color_ram_address_offset | (local_palette | dot) * sizeof(T)};
        auto       color         = readColor<T>(color_address);
        if (!dot && screen.is_transparency_code_valid) color.a = 0;

        texture_data.insert(texture_data.end(), {color.r, color.g, color.b, color.a});
    };

    template<typename T>
    void readPalette256Dot(std::vector<u8>&          texture_data,
                           const ScrollScreenStatus& screen,
                           const u16                 palette_number,
                           const u8                  dot) {
        const auto local_palette = (palette_number & 0xf0) << 4;
        const auto color_address = u32{cram_start_address + screen.color_ram_address_offset | (local_palette | dot) * sizeof(T)};
        auto       color         = readColor<T>(color_address);

        if (!dot && screen.is_transparency_code_valid) color.a = 0;

        texture_data.insert(texture_data.end(), {color.r, color.g, color.b, color.a});
    };

    template<typename T>
    void readPalette256Dot(std::vector<u8>&          texture_data,
                           const ScrollScreenStatus& screen,
                           const u16                 palette_number,
                           const u8                  dot,
                           const std::span<const u8> cram) {
        const auto local_palette = (palette_number & 0xf0) << 4;
        const auto color_address = u32{cram_start_address + screen.color_ram_address_offset | (local_palette | dot) * sizeof(T)};
        auto       color         = readColor<T>(color_address, cram);

        if (!dot && screen.is_transparency_code_valid) color.a = 0;

        texture_data.insert(texture_data.end(), {color.r, color.g, color.b, color.a});
    };

    template<typename T>
    void readPalette256DotBitmap(std::vector<u8>&          texture_data,
                                 const ScrollScreenStatus& screen,
                                 const u16                 palette_number,
                                 const u8                  dot) {
        const auto color_address = u32{cram_start_address + screen.color_ram_address_offset | (palette_number + dot) * sizeof(T)};
        auto       color         = readColor<T>(color_address);
        if (!dot && screen.is_transparency_code_valid) color.a = 0;

        texture_data.insert(texture_data.end(), {color.r, color.g, color.b, color.a});
    };

    template<typename T>
    void readPalette2048Dot(std::vector<u8>& texture_data, const ScrollScreenStatus& screen, const u8 dot) {
        const auto color_address = u32{cram_start_address + screen.color_ram_address_offset | dot * sizeof(T)};
        auto       color         = readColor<T>(color_address);

        if (!dot && screen.is_transparency_code_valid) color.a = 0;

        texture_data.insert(texture_data.end(), {color.r, color.g, color.b, color.a});
    };

    void read32KDot(std::vector<u8>& texture_data, const ScrollScreenStatus& screen, const u16 dot) const {
        auto color = Color(dot);
        if (!(dot & 0x8000) && screen.is_transparency_code_valid) color.a = 0;

        texture_data.insert(texture_data.end(), {color.r, color.g, color.b, color.a});
    };

    void read16MDot(std::vector<u8>& texture_data, const ScrollScreenStatus& screen, const u32 dot) const {
        auto color = Color(dot);
        if (!(dot & 0x80000000) && screen.is_transparency_code_valid) color.a = 0;

        texture_data.insert(texture_data.end(), {color.r, color.g, color.b, color.a});
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn template<typename T> void saturnin::video::Vdp2::read16ColorsCellData(std::vector<u8>& texture_data, const
    /// ScrollScreenStatus& screen, const u8 palette_number, const u32 cell_address)
    ///
    /// \brief  Reads one cell data in 16 colors palette format.
    ///
    /// \tparam T   Generic type parameter.
    /// \param [in,out] texture_data    Raw texture data.
    /// \param          screen          Current scroll screen status.
    /// \param          palette_number  The palette number.
    /// \param          cell_address    The cell address.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    template<typename T>
    void read16ColorsCellData(std::vector<u8>&          texture_data,
                              const ScrollScreenStatus& screen,
                              const u16                 palette_number,
                              const u32                 cell_address) {
        constexpr auto row_offset      = u8{4};
        auto           current_address = vram_start_address + cell_address;
        auto           row             = DataExtraction{};
        for (u32 i = 0; i < 8; ++i) {
            row.as_4bits = modules_.memory()->read<u32>(current_address);
            readPalette16Dot<T>(texture_data, screen, palette_number, row.as_4bits >> DataExtraction::As4Bits::dot0_shift);
            readPalette16Dot<T>(texture_data, screen, palette_number, row.as_4bits >> DataExtraction::As4Bits::dot1_shift);
            readPalette16Dot<T>(texture_data, screen, palette_number, row.as_4bits >> DataExtraction::As4Bits::dot2_shift);
            readPalette16Dot<T>(texture_data, screen, palette_number, row.as_4bits >> DataExtraction::As4Bits::dot3_shift);
            readPalette16Dot<T>(texture_data, screen, palette_number, row.as_4bits >> DataExtraction::As4Bits::dot4_shift);
            readPalette16Dot<T>(texture_data, screen, palette_number, row.as_4bits >> DataExtraction::As4Bits::dot5_shift);
            readPalette16Dot<T>(texture_data, screen, palette_number, row.as_4bits >> DataExtraction::As4Bits::dot6_shift);
            readPalette16Dot<T>(texture_data, screen, palette_number, row.as_4bits >> DataExtraction::As4Bits::dot7_shift);
            current_address += row_offset;
        }
    }

    template<typename T>
    void read256ColorsCellData(std::vector<u8>&          texture_data,
                               const ScrollScreenStatus& screen,
                               const u16                 palette_number,
                               const u32                 cell_address) {
        constexpr auto row_offset      = u8{4};
        auto           current_address = vram_start_address + cell_address;
        auto           row             = DataExtraction{};
        // const auto     data            = modules_.memory()->read(core::MemoryMapArea::vdp2_video_ram, current_address, 0x40);
        for (u32 i = 0; i < 8; ++i) {
            row.as_8bits = modules_.memory()->read<u32>(current_address);
            readPalette256Dot<T>(texture_data, screen, palette_number, row.as_8bits >> DataExtraction::As8Bits::dot0_shift);
            readPalette256Dot<T>(texture_data, screen, palette_number, row.as_8bits >> DataExtraction::As8Bits::dot1_shift);
            readPalette256Dot<T>(texture_data, screen, palette_number, row.as_8bits >> DataExtraction::As8Bits::dot2_shift);
            readPalette256Dot<T>(texture_data, screen, palette_number, row.as_8bits >> DataExtraction::As8Bits::dot3_shift);
            current_address += row_offset;
            row.as_8bits = modules_.memory()->read<u32>(current_address);
            readPalette256Dot<T>(texture_data, screen, palette_number, row.as_8bits >> DataExtraction::As8Bits::dot0_shift);
            readPalette256Dot<T>(texture_data, screen, palette_number, row.as_8bits >> DataExtraction::As8Bits::dot1_shift);
            readPalette256Dot<T>(texture_data, screen, palette_number, row.as_8bits >> DataExtraction::As8Bits::dot2_shift);
            readPalette256Dot<T>(texture_data, screen, palette_number, row.as_8bits >> DataExtraction::As8Bits::dot3_shift);
            current_address += row_offset;
        }
    }

    template<typename T>
    void read256ColorsCellDataMT(std::vector<u8>&          texture_data,
                                 const ScrollScreenStatus& screen,
                                 const u16                 palette_number,
                                 const u32                 cell_address,
                                 const std::span<const u8> vram,
                                 const std::span<const u8> cram) {
        constexpr auto row_offset      = u8{4};
        auto           current_address = vram_start_address + cell_address;
        auto           row             = DataExtraction{};
        for (u32 i = 0; i < 8; ++i) {
            // auto addr    = current_address & core::vdp2_vram_memory_mask;
            row.as_8bits = utilities::readAs32(vram.subspan(current_address & core::vdp2_vram_memory_mask, 4));
            readPalette256Dot<T>(texture_data, screen, palette_number, row.as_8bits >> DataExtraction::As8Bits::dot0_shift, cram);
            readPalette256Dot<T>(texture_data, screen, palette_number, row.as_8bits >> DataExtraction::As8Bits::dot1_shift, cram);
            readPalette256Dot<T>(texture_data, screen, palette_number, row.as_8bits >> DataExtraction::As8Bits::dot2_shift, cram);
            readPalette256Dot<T>(texture_data, screen, palette_number, row.as_8bits >> DataExtraction::As8Bits::dot3_shift, cram);
            current_address += row_offset;
            row.as_8bits = utilities::readAs32(vram.subspan(current_address & core::vdp2_vram_memory_mask, 4));
            readPalette256Dot<T>(texture_data, screen, palette_number, row.as_8bits >> DataExtraction::As8Bits::dot0_shift, cram);
            readPalette256Dot<T>(texture_data, screen, palette_number, row.as_8bits >> DataExtraction::As8Bits::dot1_shift, cram);
            readPalette256Dot<T>(texture_data, screen, palette_number, row.as_8bits >> DataExtraction::As8Bits::dot2_shift, cram);
            readPalette256Dot<T>(texture_data, screen, palette_number, row.as_8bits >> DataExtraction::As8Bits::dot3_shift, cram);
            current_address += row_offset;
        }
    }

    template<typename T>
    void read2048ColorsCellData(std::vector<u8>& texture_data, const ScrollScreenStatus& screen, const u32 cell_address) {
        constexpr auto row_offset      = u8{4};
        auto           current_address = vram_start_address + cell_address;
        auto           row             = DataExtraction{};
        for (u32 i = 0; i < 8; ++i) {
            row.as_11bits = modules_.memory()->read<u32>(current_address);
            readPalette2048Dot<T>(texture_data, screen, row.as_11bits >> DataExtraction::As11Bits::dot0_shift);
            readPalette2048Dot<T>(texture_data, screen, row.as_11bits >> DataExtraction::As11Bits::dot1_shift);
            current_address += row_offset;
            row.as_11bits = modules_.memory()->read<u32>(current_address);
            readPalette2048Dot<T>(texture_data, screen, row.as_11bits >> DataExtraction::As11Bits::dot0_shift);
            readPalette2048Dot<T>(texture_data, screen, row.as_11bits >> DataExtraction::As11Bits::dot1_shift);
            current_address += row_offset;
            row.as_11bits = modules_.memory()->read<u32>(current_address);
            readPalette2048Dot<T>(texture_data, screen, row.as_11bits >> DataExtraction::As11Bits::dot0_shift);
            readPalette2048Dot<T>(texture_data, screen, row.as_11bits >> DataExtraction::As11Bits::dot1_shift);
            current_address += row_offset;
            row.as_11bits = modules_.memory()->read<u32>(current_address);
            readPalette2048Dot<T>(texture_data, screen, row.as_11bits >> DataExtraction::As11Bits::dot0_shift);
            readPalette2048Dot<T>(texture_data, screen, row.as_11bits >> DataExtraction::As11Bits::dot1_shift);
            current_address += row_offset;
        }
    }

    void read32KColorsCellData(std::vector<u8>& texture_data, const ScrollScreenStatus& screen, const u32 cell_address) {
        constexpr auto row_offset      = u8{4};
        auto           current_address = vram_start_address + cell_address;
        auto           row             = DataExtraction{};
        for (u32 i = 0; i < 8; ++i) {
            row.as_16bits = modules_.memory()->read<u32>(current_address);
            read32KDot(texture_data, screen, row.as_16bits >> DataExtraction::As16Bits::dot0_shift);
            read32KDot(texture_data, screen, row.as_16bits >> DataExtraction::As16Bits::dot1_shift);
            current_address += row_offset;
            row.as_16bits = modules_.memory()->read<u32>(current_address);
            read32KDot(texture_data, screen, row.as_16bits >> DataExtraction::As16Bits::dot0_shift);
            read32KDot(texture_data, screen, row.as_16bits >> DataExtraction::As16Bits::dot1_shift);
            current_address += row_offset;
            row.as_16bits = modules_.memory()->read<u32>(current_address);
            read32KDot(texture_data, screen, row.as_16bits >> DataExtraction::As16Bits::dot0_shift);
            read32KDot(texture_data, screen, row.as_16bits >> DataExtraction::As16Bits::dot1_shift);
            current_address += row_offset;
            row.as_16bits = modules_.memory()->read<u32>(current_address);
            read32KDot(texture_data, screen, row.as_16bits >> DataExtraction::As16Bits::dot0_shift);
            read32KDot(texture_data, screen, row.as_16bits >> DataExtraction::As16Bits::dot1_shift);
            current_address += row_offset;
        }
    }
    void read16MColorsCellData(std::vector<u8>& texture_data, const ScrollScreenStatus& screen, const u32 cell_address) {
        constexpr auto row_offset      = u8{4};
        auto           current_address = vram_start_address + cell_address;
        for (u32 i = 0; i < 8; ++i) {
            read16MDot(texture_data, screen, modules_.memory()->read<u32>(current_address));
            current_address += row_offset;
            read16MDot(texture_data, screen, modules_.memory()->read<u32>(current_address));
            current_address += row_offset;
            read16MDot(texture_data, screen, modules_.memory()->read<u32>(current_address));
            current_address += row_offset;
            read16MDot(texture_data, screen, modules_.memory()->read<u32>(current_address));
            current_address += row_offset;
            read16MDot(texture_data, screen, modules_.memory()->read<u32>(current_address));
            current_address += row_offset;
            read16MDot(texture_data, screen, modules_.memory()->read<u32>(current_address));
            current_address += row_offset;
            read16MDot(texture_data, screen, modules_.memory()->read<u32>(current_address));
            current_address += row_offset;
            read16MDot(texture_data, screen, modules_.memory()->read<u32>(current_address));
            current_address += row_offset;
        }
    }

    template<typename T>
    void read16ColorsBitmapData(std::vector<u8>& texture_data, const ScrollScreenStatus& screen) {
        constexpr auto offset          = u8{4};
        auto           current_address = screen.bitmap_start_address;
        const auto     end_address     = current_address + static_cast<u32>(texture_data.capacity()) / 4;
        constexpr auto palette_disp    = u8{4};
        const auto     palette         = screen.bitmap_palette_number << palette_disp;
        auto           row             = DataExtraction{};
        for (u32 i = screen.bitmap_start_address; i < end_address; i += offset) {
            row.as_4bits = modules_.memory()->read<u32>(current_address);
            readPalette16Dot<T>(texture_data, screen, palette, row.as_4bits >> DataExtraction::As4Bits::dot0_shift);
            readPalette16Dot<T>(texture_data, screen, palette, row.as_4bits >> DataExtraction::As4Bits::dot1_shift);
            readPalette16Dot<T>(texture_data, screen, palette, row.as_4bits >> DataExtraction::As4Bits::dot2_shift);
            readPalette16Dot<T>(texture_data, screen, palette, row.as_4bits >> DataExtraction::As4Bits::dot3_shift);
            readPalette16Dot<T>(texture_data, screen, palette, row.as_4bits >> DataExtraction::As4Bits::dot4_shift);
            readPalette16Dot<T>(texture_data, screen, palette, row.as_4bits >> DataExtraction::As4Bits::dot5_shift);
            readPalette16Dot<T>(texture_data, screen, palette, row.as_4bits >> DataExtraction::As4Bits::dot6_shift);
            readPalette16Dot<T>(texture_data, screen, palette, row.as_4bits >> DataExtraction::As4Bits::dot7_shift);
            current_address += offset;
        }
    }

    template<typename T>
    void read256ColorsBitmapData(std::vector<u8>& texture_data, const ScrollScreenStatus& screen) {
        constexpr auto offset          = u8{4};
        auto           current_address = screen.bitmap_start_address;
        const auto     end_address     = current_address + static_cast<u32>(texture_data.capacity()) / 4;
        constexpr auto palette_disp    = u8{8};
        const auto     palette         = screen.bitmap_palette_number << palette_disp;

        BS::timer                                          tmr;
        std::chrono::time_point<std::chrono::steady_clock> start_time = std::chrono::steady_clock::now();
        std::chrono::duration<double>                      elapsed_time{};

        // start_time = std::chrono::steady_clock::now();

        // for (u32 i = screen.bitmap_start_address; i < end_address; i += (offset * 2)) {
        //     // if (screen.format == ScrollScreenFormat::bitmap) {
        //     //    if (current_address == 0x25E26830) DebugBreak();
        //     //}
        //     auto row = Dots8Bits{modules_.memory()->read<u32>(current_address)};
        //     readPalette256Dot<T>(texture_data, screen, palette, row.dot_0);
        //     readPalette256Dot<T>(texture_data, screen, palette, row.dot_1);
        //     readPalette256Dot<T>(texture_data, screen, palette, static_cast<u8>(row.dot_2));
        //     readPalette256Dot<T>(texture_data, screen, palette, row.dot_3);
        //     current_address += offset;
        //     row = Dots8Bits{modules_.memory()->read<u32>(current_address)};
        //     readPalette256Dot<T>(texture_data, screen, palette, row.dot_0);
        //     readPalette256Dot<T>(texture_data, screen, palette, row.dot_1);
        //     readPalette256Dot<T>(texture_data, screen, palette, static_cast<u8>(row.dot_2));
        //     readPalette256Dot<T>(texture_data, screen, palette, row.dot_3);
        //     current_address += offset;
        // }
        // elapsed_time = std::chrono::steady_clock::now() - start_time;
        auto res = (std::chrono::duration_cast<std::chrono::microseconds>(elapsed_time)).count();
        // core::Log::warning(Logger::vdp2, core::tr(u8"Sequential read {}µs"), res);

        //---------------------------------------------------------//

        // Data to read is divided in chucks to facilitate parallelization
        // At first data is read from memory to a local vector, then this local vector is copied to the main vector.
        // The main vector can be updated by the various threads concurrently without synchronisation as the data don't
        // overlap.
        texture_data.assign(texture_data.capacity(), 0);
        constexpr auto chunk_size     = u32{0x1000};
        auto           readBitmapPart = [&](u32 start_address, u32 texture_offset) {
            auto       local_texture      = std::vector<u8>{};
            const auto local_texture_size = chunk_size / offset * 0x10;
            local_texture.reserve(local_texture_size);
            auto local_offset = texture_offset;
            auto row          = DataExtraction{};
            for (u32 i = start_address; i < (start_address + chunk_size); i += offset) {
                row.as_8bits = modules_.memory()->read<u32>(i);
                readPalette256DotBitmap<T>(local_texture, screen, palette, row.as_8bits >> DataExtraction::As8Bits::dot0_shift);
                local_offset += 4;
                readPalette256DotBitmap<T>(local_texture, screen, palette, row.as_8bits >> DataExtraction::As8Bits::dot1_shift);
                local_offset += 4;
                readPalette256DotBitmap<T>(local_texture, screen, palette, row.as_8bits >> DataExtraction::As8Bits::dot2_shift);
                local_offset += 4;
                readPalette256DotBitmap<T>(local_texture, screen, palette, row.as_8bits >> DataExtraction::As8Bits::dot3_shift);
                local_offset += 4;
            }
            std::copy(local_texture.begin(), local_texture.end(), &texture_data[0] + texture_offset * 4);
        };
        // start_time          = std::chrono::steady_clock::now();
        auto texture_offset = u32{};

        // core::Log::warning(Logger::vdp2, u8"Bitmap size {:#x}", end_address - screen.bitmap_start_address);
        for (u32 i = screen.bitmap_start_address; i < end_address; i += chunk_size) {
            ThreadPool::pool_.push_task(readBitmapPart, current_address, texture_offset);
            current_address += chunk_size;
            texture_offset += chunk_size;
        }
        ThreadPool::pool_.wait_for_tasks();

        // elapsed_time = std::chrono::steady_clock::now() - start_time;
        // res          = (std::chrono::duration_cast<std::chrono::microseconds>(elapsed_time)).count();
        // core::Log::warning(Logger::vdp2, core::tr(u8"Parallel read {}µs"), res);

        //---------------------

        // if (!texture_data.empty()) {
        //     std::ofstream outfile("bitmap_seq.dat", std::ofstream::binary);
        //     // outfile.write(reinterpret_cast<const char*>(texture_data.data(), sizeof(u8) * texture_data.size());
        //     outfile.write(reinterpret_cast<const char*>(texture_data.data()), sizeof(u8) * texture_data.size());
        //     outfile.close();
        // }
    }

    template<typename T>
    void read2048ColorsBitmapData(std::vector<u8>& texture_data, const ScrollScreenStatus& screen) {
        constexpr auto offset          = u8{4};
        auto           current_address = screen.bitmap_start_address;
        const auto     end_address     = current_address + static_cast<u32>(texture_data.capacity()) / 4;
        auto           row             = DataExtraction{};
        for (u32 i = screen.bitmap_start_address; i < end_address; i += offset) {
            row.as_11bits = modules_.memory()->read<u32>(current_address);
            readPalette2048Dot<T>(texture_data, screen, row.as_11bits >> DataExtraction::As11Bits::dot0_shift);
            readPalette2048Dot<T>(texture_data, screen, row.as_11bits >> DataExtraction::As11Bits::dot1_shift);
            current_address += offset;
        }
    }

    void read32KColorsBitmapData(std::vector<u8>& texture_data, const ScrollScreenStatus& screen) const {
        constexpr auto offset          = u8{4};
        auto           current_address = screen.bitmap_start_address;
        const auto     end_address     = current_address + static_cast<u32>(texture_data.capacity()) / 4;
        auto           row             = DataExtraction{};
        for (u32 i = screen.bitmap_start_address; i < end_address; i += (offset * 2)) {
            // auto row = Dots16Bits{modules_.memory()->read<u32>(current_address)};
            row.as_16bits = modules_.memory()->read<u32>(current_address);
            read32KDot(texture_data, screen, static_cast<u16>(row.as_16bits >> DataExtraction::As16Bits::dot0_shift));
            read32KDot(texture_data, screen, static_cast<u16>(row.as_16bits >> DataExtraction::As16Bits::dot1_shift));
            current_address += offset;
            // row = Dots16Bits{modules_.memory()->read<u32>(current_address)};
            row.as_16bits = modules_.memory()->read<u32>(current_address);
            read32KDot(texture_data, screen, static_cast<u16>(row.as_16bits >> DataExtraction::As16Bits::dot0_shift));
            read32KDot(texture_data, screen, static_cast<u16>(row.as_16bits >> DataExtraction::As16Bits::dot1_shift));
            current_address += offset;
        }
    }

    void read16MColorsBitmapData(std::vector<u8>& texture_data, const ScrollScreenStatus& screen) const {
        constexpr auto offset          = u8{4};
        auto           current_address = screen.bitmap_start_address;
        const auto     end_address     = current_address + static_cast<u32>(texture_data.capacity()) / 4;

        for (u32 i = screen.bitmap_start_address; i < end_address; i += offset) {
            auto dot = modules_.memory()->read<u32>(i);
            read16MDot(texture_data, screen, dot);
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	void Vdp2::saveBitmap(const ScrollScreenStatus& screen, const u16 width, const u16 height, const size_t key);
    ///
    /// \brief	Saves a bitmap to the parts vector.
    ///
    /// \author	Runik
    /// \date	17/08/2021
    ///
    /// \param 	screen	Current scroll screen status.
    /// \param 	width 	Bitmap width.
    /// \param 	height	Bitmap height.
    /// \param 	key   	Texture key.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void saveBitmap(const ScrollScreenStatus& screen, const u16 width, const u16 height, const size_t key);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn const auto Vdp2::getColorRamAddressOffset(const u8 register_offset);
    ///
    /// \brief  Gets color ram address offset
    ///
    /// \author Runik
    /// \date   14/05/2021
    ///
    /// \param  register_offset The register offset.
    ///
    /// \returns    The color ram address offset.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto getColorRamAddressOffset(const u8 register_offset) const -> u16;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Vdp2::resetCacheState() const;
    ///
    /// \brief  Resets the cache state.
    ///
    /// \author Runik
    /// \date   07/10/2021
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void resetCacheState() const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto Vdp2::isCacheDirty(const ScrollScreen screen) -> bool;
    ///
    /// \brief  Checks if a scroll screen has to be reloaded.
    ///
    /// \author Runik
    /// \date   07/10/2021
    ///
    /// \param  screen  The scroll screen to check.
    ///
    /// \returns    True if the cache has to be reloaded.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto isCacheDirty(const ScrollScreen screen) -> bool;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Vdp2::discardCache(const ScrollScreen screen) const;
    ///
    /// \brief  Discards cache data for the scroll screen.
    ///
    /// \author Runik
    /// \date   08/10/2021
    ///
    /// \param  screen  The scroll screen to manage.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void discardCache(const ScrollScreen screen) const;

    void calculateFps();

    auto getVram() const -> std::span<const u8> { return std::span<const u8>{modules_.memory()->vdp2_vram_}; };
    auto getCram() const -> std::span<const u8> { return std::span<const u8>{modules_.memory()->vdp2_cram_}; };

    EmulatorModules modules_;

    AddressToNameMap address_to_name_; ///< Link between a register address and its name.

    u32 elapsed_frame_cycles_{}; ///< Elapsed cycles for the current frame.
    u32 elapsed_line_cycles_{};  ///< Elapsed cycles for the current line.

    u32 cycles_per_frame_{};   ///< Number of SH2 cycles needed to display one frame (active + blanking).
    u32 cycles_per_vblank_{};  ///< Number of SH2 cycles needed for VBlank duration.
    u32 cycles_per_vactive_{}; ///< Number of SH2 cycles needed to display the visible part of the frame

    u32 cycles_per_line_{};    ///< Number of SH2 cycles needed to display one line (active + blanking).
    u32 cycles_per_hblank_{};  ///< Number of SH2 cycles needed for HBlank duration.
    u32 cycles_per_hactive_{}; ///< Number of SH2 cycles needed to display the visible part of a line.

    bool is_vblank_current_{}; ///< True if VBlank is current.
    bool is_hblank_current_{}; ///< True if HBlank is current.

    u16 timer_0_counter_{}; ///< Timer 0 counter.
    u16 timer_1_counter_{}; ///< Timer 1 counter.

    TvScreenStatus tv_screen_status_; ///< The TV screen status.
    RamStatus      ram_status_;       ///< The RAM status

    std::array<ScrollScreenStatus, 6> bg_;       ///< The backgrounds status.
    std::array<ScrollScreenStatus, 6> saved_bg_; /// \brief  The backgrounds status from the previous frame.

    // Pre calculated modulo values used for character patterns positionning
    std::vector<u32> pre_calculated_modulo_64_{}; ///< The pre calculated modulo 64
    std::vector<u32> pre_calculated_modulo_32_{}; ///< The pre calculated modulo 32

    std::array<std::vector<Vdp2Part>, 6> vdp2_parts_;           ///< Storage of rendering parts for each scroll cell.
    std::vector<CellData>                cell_data_to_process_; ///< Will store cell data before parallelized read for one scroll.
    std::mutex                           plane_data_mutex_;     ///< Mutex protecting address_to_plane_data_.
    AddressToPlaneData address_to_plane_data_; ///< Stores plane data to improve performance when a same address is used multiple
    u32                current_plane_address_; ///< The current plane address.
                                               ///< times in the same NBG / RBG.
    std::array<std::vector<PlaneDetail>, 6> planes_details_; ///< Stores planes details for every scroll.

    ScrollScreen         screen_in_debug_{ScrollScreen::none}; ///< Scroll screen currently viewed in debug.
    DisabledScrollScreen disabled_scroll_screens_;             ///< Disabling state of scroll screens.

    std::mutex  fps_mutex_; ///< Mutex protecting fps_.
    std::string fps_;       ///< The FPS

    Vdp2Regs regs_; ///< VDP2 registers
};

///@{
/// \name Pattern Name Data access functions.
auto getPatternNameData2Words(const u32 data, [[maybe_unused]] const ScrollScreenStatus& screen) -> PatternNameData;
auto getPatternNameData1Word1Cell16Colors10Bits(const u32 data, const ScrollScreenStatus& screen) -> PatternNameData;
auto getPatternNameData1Word1Cell16Colors12Bits(const u32 data, const ScrollScreenStatus& screen) -> PatternNameData;
auto getPatternNameData1Word1CellOver16Colors10Bits(const u32 data, const ScrollScreenStatus& screen) -> PatternNameData;
auto getPatternNameData1Word1CellOver16Colors12Bits(const u32 data, const ScrollScreenStatus& screen) -> PatternNameData;
auto getPatternNameData1Word4Cells16Colors10Bits(const u32 data, const ScrollScreenStatus& screen) -> PatternNameData;
auto getPatternNameData1Word4Cells16Colors12Bits(const u32 data, const ScrollScreenStatus& screen) -> PatternNameData;
auto getPatternNameData1Word4CellsOver16Colors10Bits(const u32 data, const ScrollScreenStatus& screen) -> PatternNameData;
auto getPatternNameData1Word4CellsOver16Colors12Bits(const u32 data, const ScrollScreenStatus& screen) -> PatternNameData;
///@}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn	auto screenName(const ScrollScreen& ss) -> std::string;
///
/// \brief	Gets the name of a ScrollScreen as a string.
///
/// \author	Runik
/// \date	16/04/2023
///
/// \param 	ss	The ScrollScreen
///
/// \returns	A std::string.
////////////////////////////////////////////////////////////////////////////////////////////////////

auto screenName(const ScrollScreen& ss) -> std::string;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn	auto scrollScreenToLayer(const ScrollScreen& ss) -> Layer;
///
/// \brief	Gets the layer linked to the ScrollScreen.
///
/// \author	Runik
/// \date	08/09/2023
///
/// \param 	ss	The ScrollScreen
///
/// \returns	A Layer.
////////////////////////////////////////////////////////////////////////////////////////////////////

auto scrollScreenToLayer(const ScrollScreen& ss) -> Layer;
} // namespace saturnin::video
