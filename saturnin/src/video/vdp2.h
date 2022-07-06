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
#include <saturnin/src/bitfield.h>
#include <saturnin/src/emulator_modules.h> // EmulatorModules
#include <saturnin/src/locale.h>           // tr
#include <saturnin/src/log.h>              // Log
#include <saturnin/src/thread_pool.h>      // ThreadPool
#include <saturnin/src/utilities.h>        // toUnderlying
#include <saturnin/src/video/vdp_common.h>
#include <saturnin/src/video/vdp2_part.h> // ScrollScreenPos
#include <saturnin/src/video/vdp2_registers.h>

// Forward declarations
namespace saturnin::core {
class Memory;
class Scu;
} // namespace saturnin::core

namespace saturnin::video {

class Vdp1;

using saturnin::core::EmulatorContext;
using saturnin::core::EmulatorModules;
using saturnin::core::Log;
using saturnin::core::Logger;
using saturnin::core::ThreadPool;
// using AddressToNameMap = std::map<u32, std::string>;

using namespace std::chrono;
using seconds = duration<double>;
using milli   = duration<double, std::milli>;
using micro   = duration<double, std::micro>;

constexpr auto vram_banks_number  = u8{4};
constexpr auto vram_bank_a0_index = u8{0};
constexpr auto vram_bank_a1_index = u8{1};
constexpr auto vram_bank_b0_index = u8{2};
constexpr auto vram_bank_b1_index = u8{3};
constexpr auto vram_timing_size   = u8{8};
using VramTiming                  = std::array<VramAccessCommand, vram_timing_size>;

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

constexpr auto vram_start_address = u32{0x25e00000};
constexpr auto cram_start_address = u32{0x25f00000};

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
    bool            is_picture_displayed{false};
    BorderColorMode border_color_mode{BorderColorMode::displays_black};
    InterlaceMode   interlace_mode{InterlaceMode::non_interlace};
    ScreenMode      screen_mode{ScreenMode::not_set};
    ScreenModeType  screen_mode_type{ScreenModeType::not_set};
    u16             horizontal_res{};
    u16             vertical_res{};
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
    VramSize                vram_size{VramSize::size_4_mbits};
    VramMode                vram_a_mode{VramMode::no_partition};
    VramMode                vram_b_mode{VramMode::no_partition};
    ColorRamMode            color_ram_mode{ColorRamMode::not_set};
    CoefficientTableStorage coefficient_table_storage{CoefficientTableStorage::stored_in_vram};
    RotationDataBankSelect  vram_a0_rotation_bank_select{RotationDataBankSelect::not_used};
    RotationDataBankSelect  vram_a1_rotation_bank_select{RotationDataBankSelect::not_used};
    RotationDataBankSelect  vram_b0_rotation_bank_select{RotationDataBankSelect::not_used};
    RotationDataBankSelect  vram_b1_rotation_bank_select{RotationDataBankSelect::not_used};
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
    u8                            map_size{};                     ///< Size of the map (2*2 for NBG / 4*4 for RBG)
    u8                            map_offset{};                   ///< The map offset
    PlaneSize                     plane_size{PlaneSize::not_set}; ///< Size of the plane (1*1, 2*1 or 2*2 pages)
    u16                           page_size{};                    ///< Size of the page / pattern name table
    PatternNameDataSize           pattern_name_data_size{};       ///< Size of the pattern name data (1 or 2 words)
    CharacterNumberSupplementMode character_number_supplement_mode{
        CharacterNumberSupplementMode::character_number_10_bits}; ///< 10 bits/12 bits
    u8            special_priority{};                             ///< Special priority bit
    u8            special_color_calculation{};                    ///< Special color calculation bit
    u8            supplementary_palette_number{};                 ///< Supplementary palette number bit
    u8            supplementary_character_number{};               ///< Supplementary character number bit
    CharacterSize character_pattern_size{};                       ///< Size of the character pattern (1*1 or 2*2 cells)
    u16           cell_size{};                                    ///< Size of the cell (8*8 dots)

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
/// \union	PatternNameData2Words
///
/// \brief	Pattern Name Data - 2 words configuration.
///
/// \author	Runik
/// \date	22/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union PatternNameData2Words {
    u32             raw;                       ///< Raw representation.
    BitField<31>    vertical_flip;             ///< Defines the vertical flip bit.
    BitField<30>    horizontal_flip;           ///< Defines the horizontal flip bit.
    BitField<29>    special_priority;          ///< Defines the special priority bit.
    BitField<28>    special_color_calculation; ///< Defines the special color calculation bit.
    BitField<16, 7> palette_number;            ///< Defines the palette number.
    BitField<0, 15> character_number;          ///< Defines the character number.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	PatternNameData1Word1Cell16Colors10Bits
///
/// \brief	Pattern Name Data - 1 word, 1 cell, 16 colors, 10 bits configuration.
///
/// \author	Runik
/// \date	22/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union PatternNameData1Word1Cell16Colors10Bits {
    u32             raw;              ///< Raw representation.
    BitField<11>    vertical_flip;    ///< Defines the vertical flip bit.
    BitField<10>    horizontal_flip;  ///< Defines the horizontal flip bit.
    BitField<12, 4> palette_number;   ///< Defines the palette number.
    BitField<0, 10> character_number; ///< Defines the character number.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	PatternNameData1Word1Cell16Colors12Bits
///
/// \brief	Pattern Name Data - 1 word, 1 cell, 16 colors, 12 bits configuration.
///
/// \author	Runik
/// \date	22/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union PatternNameData1Word1Cell16Colors12Bits {
    u32             raw;              ///< Raw representation.
    BitField<12, 4> palette_number;   ///< Defines the palette number.
    BitField<0, 12> character_number; ///< Defines the character number.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	PatternNameData1Word1CellOver16Colors10Bits
///
/// \brief	Pattern Name Data - 1 word, 1 cell, over 16 colors, 10 bits configuration.
///
/// \author	Runik
/// \date	22/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union PatternNameData1Word1CellOver16Colors10Bits {
    u32             raw;              ///< Raw representation.
    BitField<11>    vertical_flip;    ///< Defines the vertical flip bit.
    BitField<10>    horizontal_flip;  ///< Defines the horizontal flip bit.
    BitField<12, 3> palette_number;   ///< Defines the palette number.
    BitField<0, 10> character_number; ///< Defines the character number.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	PatternNameData1Word1CellOver16Colors12Bits
///
/// \brief	Pattern Name Data - 1 word, 1 cell, over 16 colors, 10 bits configuration.
///
/// \author	Runik
/// \date	22/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union PatternNameData1Word1CellOver16Colors12Bits {
    u32             raw;              ///< Raw representation.
    BitField<12, 3> palette_number;   ///< Defines the palette number.
    BitField<0, 12> character_number; ///< Defines the character number.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	PatternNameData1Word4Cells16Colors10Bits
///
/// \brief	Pattern Name Data - 1 word, 4 cells, 16 colors, 10 bits configuration.
///
/// \author	Runik
/// \date	22/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union PatternNameData1Word4Cells16Colors10Bits {
    u32             raw;              ///< Raw representation.
    BitField<11>    vertical_flip;    ///< Defines the vertical flip bit.
    BitField<10>    horizontal_flip;  ///< Defines the horizontal flip bit.
    BitField<12, 4> palette_number;   ///< Defines the palette number.
    BitField<0, 10> character_number; ///< Defines the character number.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	PatternNameData1Word4Cells16Colors12Bits
///
/// \brief	Pattern Name Data - 1 word, 4 cells, 16 colors, 12 bits configuration.
///
/// \author	Runik
/// \date	22/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union PatternNameData1Word4Cells16Colors12Bits {
    u32             raw;              ///< Raw representation.
    BitField<12, 4> palette_number;   ///< Defines the palette number.
    BitField<0, 12> character_number; ///< Defines the character number.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  PatternNameData1Word4CellsOver16Colors10Bits
///
/// \brief  Pattern Name Data - 1 word, 4 cells, over 16 colors, 10 bits configuration.
///
/// \author Runik
/// \date   15/03/2021
////////////////////////////////////////////////////////////////////////////////////////////////////

union PatternNameData1Word4CellsOver16Colors10Bits {
    u32             raw;              ///< Raw representation.
    BitField<11>    vertical_flip;    ///< Defines the vertical flip bit.
    BitField<10>    horizontal_flip;  ///< Defines the horizontal flip bit.
    BitField<12, 3> palette_number;   ///< Defines the palette number.
    BitField<0, 10> character_number; ///< Defines the character number.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	PatternNameData1Word4CellsOver16Colors12Bits
///
/// \brief	Pattern Name Data - 1 word, 4 cells, 16 colors, 12 bits configuration.
///
/// \author	Runik
/// \date	22/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union PatternNameData1Word4CellsOver16Colors12Bits {
    u32             raw;              ///< Raw representation.
    BitField<12, 3> palette_number;   ///< Defines the palette number.
    BitField<0, 12> character_number; ///< Defines the character number.
};

class Vdp2 {
  public:
    ///@{
    /// Constructors / Destructors
    Vdp2() = delete;
    Vdp2(EmulatorContext* ec) : modules_(ec){};
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
        core::Log::warning(Logger::vdp2, core::tr("{}bits register write {:#0x}"), sizeof(T) * number_of_bits_8, addr);
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
        core::Log::warning(Logger::vdp2, core::tr("{}bits register read {:#0x}"), sizeof(T) * number_of_bits_8, addr);
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
    /// \fn auto Vdp2::vdp2Parts(const ScrollScreen s) -> const std::vector<std::unique_ptr<video::BaseRenderingPart>>
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

    auto vdp2Parts(const ScrollScreen s) -> const std::vector<video::Vdp2Part>& {
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

    auto getColorRamMode() const -> ColorRamMode { return ram_status_.color_ram_mode; }

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

    auto getSpriteControlRegister() const -> SpriteControl { return spctl_; }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto Vdp2::getSpritePriority(const u8 register_number) -> u8;
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

    auto getSpritePriority(const u8 register_number) -> u8;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn template<typename T> auto Vdp2::readColor(const u32 color_address) -> const Color
    ///
    /// \brief  Reads a color
    ///
    /// \tparam T   u32 for 32 bits color, u16 for 16 bits colors.
    /// \param  color_address   The color address.
    ///
    /// \returns    The color read.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    template<typename T>
    auto readColor(const u32 color_address) -> const Color {
        return Color(modules_.memory()->read<T>(color_address));
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

    //--------------------------------------------------------------------------------------------------------------
    // DEBUG methods, defined in vdp2_debug.cpp
    //--------------------------------------------------------------------------------------------------------------

    ///@{
    /// \name Various Vdp2 debug functions
    auto        getDebugGlobalMainData() const -> std::vector<LabelValue>;
    auto        getDebugRamMainData() -> std::vector<LabelValue>;
    auto        getDebugVramAccessMainData() -> std::vector<LabelValue>;
    auto        getDebugVramAccessBanks() -> std::vector<VramTiming>;
    auto        getDebugVramAccessBanksUsed() -> std::array<bool, vram_banks_number>;
    auto        getDebugVramAccessBanksName() -> std::vector<std::string>;
    static auto getDebugVramAccessCommandDescription(const VramAccessCommand command) -> LabelValue;
    auto        getDebugScrollScreenData(const ScrollScreen s) -> std::optional<std::vector<LabelValue>>;
    auto        screenInDebug() -> ScrollScreen;
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
    void               write16(u32 addr, u16 data);
    void               write32(u32 addr, u32 data);
    ///@}

    //--------------------------------------------------------------------------------------------------------------
    // MISC methods
    //--------------------------------------------------------------------------------------------------------------

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn   void Vdp2::reset();
    ///
    /// \brief    Resets the VPD2
    ///
    /// \author   Runik
    /// \date 16/05/2020
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void reset();

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
    /// \fn auto Vdp2::isScreenDisplayLimitedByReduction(ScrollScreen s) -> bool;
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

    auto isScreenDisplayLimitedByReduction(ScrollScreen s) -> bool;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto Vdp2::getVramAccessByCommand(const VramAccessCommand command, const ReductionSetting reduction) -> u8;
    ///
    /// \brief  Gets VRAM access corresponding to the command taking into account limitations.
    ///
    /// \author Runik
    /// \date   21/12/2020
    ///
    /// \param  command     The access command.
    /// \param  reduction   The current screen reduction.
    ///
    /// \returns    Number of VRAM access for the command.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto getVramAccessByCommand(const VramAccessCommand command, const ReductionSetting reduction) -> u8;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto Vdp2::getVramBitmapReads(const VramTiming& bank_a0, const VramTiming& bank_a1,
    ///     const VramTiming& bank_b0, const VramTiming& bank_b1, const VramAccessCommand command) -> u8;
    ///
    /// \brief  Gets VRAM bitmap reads
    ///
    /// \author Runik
    /// \date   22/12/2020
    ///
    /// \param  bank_a0 VRAM bank a0.
    /// \param  bank_a1 VRAM bank a1.
    /// \param  bank_b0 VRAM bank b0.
    /// \param  bank_b1 VRAM bank b1.
    /// \param  command The VRAM access command.
    ///
    /// \returns    The VRAM bitmap reads.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto getVramBitmapReads(const VramTiming&       bank_a0,
                            const VramTiming&       bank_a1,
                            const VramTiming&       bank_b0,
                            const VramTiming&       bank_b1,
                            const VramAccessCommand command) -> u8;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto Vdp2::getVramPatternNameDataReads(const VramTiming& bank_a0, const VramTiming& bank_a1,
    ///     const VramTiming& bank_b0, const VramTiming& bank_b1, const VramAccessCommand command) -> u8;
    ///
    /// \brief  Gets VRAM pattern name data reads
    ///
    /// \author Runik
    /// \date   22/12/2020
    ///
    /// \param  bank_a0 VRAM bank a0.
    /// \param  bank_a1 VRAM bank a1.
    /// \param  bank_b0 VRAM bank b0.
    /// \param  bank_b1 VRAM bank b1.
    /// \param  command The VRAM access command.
    ///
    /// \returns    The VRAM pattern name data reads.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto getVramPatternNameDataReads(const VramTiming&       bank_a0,
                                     const VramTiming&       bank_a1,
                                     const VramTiming&       bank_b0,
                                     const VramTiming&       bank_b1,
                                     const VramAccessCommand command) -> u8;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static auto Vdp2::getReductionSetting(ZoomQuarter zq, ZoomHalf zh) -> ReductionSetting;
    ///
    /// \brief  Returns the reduction setting for the specified parameters
    ///
    /// \author Runik
    /// \date   11/08/2020
    ///
    /// \param  zq  Zoom quarter reduction setting of the plane.
    /// \param  zh  Zoom half reduction setting of the plane.
    ///
    /// \returns    The reduction setting.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static auto getReductionSetting(ZoomQuarter zq, ZoomHalf zh) -> ReductionSetting;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	static auto Vdp2::calculateRequiredVramCharacterPatternReads(ReductionSetting r, CharacterColorNumber3Bits ccn) ->
    /// VramAccessNumber;
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

    static auto calculateRequiredVramCharacterPatternReads(ReductionSetting r, CharacterColorNumber3Bits ccn) -> VramAccessNumber;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static auto Vdp2::calculateRequiredVramCharacterPatternReads(ReductionSetting r, CharacterColorNumber2Bits ccn) ->
    /// VramAccessNumber;
    ///
    /// \brief  Calculates the required VRAM character/bitmap pattern reads to display a screen.
    ///
    /// \author Runik
    /// \date   12/01/2021
    ///
    /// \param  r   The reduction setting.
    /// \param  ccn The character color number.
    ///
    /// \returns    The required VRAM character pattern reads.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static auto calculateRequiredVramCharacterPatternReads(ReductionSetting r, CharacterColorNumber2Bits ccn) -> VramAccessNumber;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static auto Vdp2::calculateRequiredVramCharacterPatternReads(ReductionSetting r, CharacterColorNumber1Bit ccn) ->
    /// VramAccessNumber;
    ///
    /// \brief  Calculates the required VRAM character/bitmap pattern reads to display a screen.
    ///
    /// \author Runik
    /// \date   12/01/2021
    ///
    /// \param  r   The reduction setting.
    /// \param  ccn The character color number.
    ///
    /// \returns    The required VRAM character pattern reads.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static auto calculateRequiredVramCharacterPatternReads(ReductionSetting r, CharacterColorNumber1Bit ccn) -> VramAccessNumber;

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
    /// \fn static auto Vdp2::getPatternNameFromCharacterPattern(const VramAccessCommand character_pattern) -> VramAccessCommand;
    ///
    /// \brief  Gets the Pattern Name command corresponding to the Character Pattern command
    ///
    /// \author Runik
    /// \date   23/12/2020
    ///
    /// \param  character_pattern   Character pattern command.
    ///
    /// \returns    Pattern name command.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static auto getPatternNameFromCharacterPattern(const VramAccessCommand character_pattern) -> VramAccessCommand;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static void Vdp2::setPatternNameAccess(const VramTiming& bank, const VramAccessCommand pattern, std::array<bool,
    /// vram_timing_size>& pnd_access);
    ///
    /// \brief  Sets the pattern name accesses for the given bank.
    ///
    /// \author Runik
    /// \date   23/12/2020
    ///
    /// \param          bank        The bank to check.
    /// \param          pattern     Pattern name data used.
    /// \param [in,out] pnd_access  Sets true for the index of the pattern name data found in the
    ///                             bank.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static void setPatternNameAccess(const VramTiming&                   bank,
                                     const VramAccessCommand             pattern,
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
    /// \fn static auto Vdp2::getVramCharacterPatternDataReads(const VramTiming& bank_a0, const VramTiming& bank_a1, const
    /// VramTiming& bank_b0, const VramTiming& bank_b1, const VramAccessCommand command, const ReductionSetting reduction, const
    /// bool is_screen_mode_normal) -> u8;
    ///
    /// \brief  Gets VRAM character pattern data reads
    ///
    /// \author Runik
    /// \date   22/12/2020
    ///
    /// \param  bank_a0                 VRAM bank a0.
    /// \param  bank_a1                 VRAM bank a1.
    /// \param  bank_b0                 VRAM bank b0.
    /// \param  bank_b1                 VRAM bank b1.
    /// \param  command                 The VRAM access command.
    /// \param  reduction               Reduction setting of current screen.
    /// \param  is_screen_mode_normal   True if current screen is in normal mode.
    /// \param  is_using_2_by_2_cp      True if current screen uses 2 by 2 character patterns.
    ///
    /// \returns    The VRAM character pattern data reads.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static auto getVramCharacterPatternDataReads(const VramTiming&       bank_a0,
                                                 const VramTiming&       bank_a1,
                                                 const VramTiming&       bank_b0,
                                                 const VramTiming&       bank_b1,
                                                 const VramAccessCommand command,
                                                 const ReductionSetting  reduction,
                                                 const bool              is_screen_mode_normal,
                                                 const bool              is_using_2_by_2_cp) -> u8;

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
    /// \fn void Vdp2::readCell(const ScrollScreenStatus& screen, const PatternNameData& pnd, const u32 cell_address, const
    /// ScreenOffset& cell_offset);
    ///
    /// \brief  Reads a cell
    ///
    /// \author Runik
    /// \date   14/03/2021
    ///
    /// \param  screen          Current scroll screen status.
    /// \param  pnd             The pattern name data.
    /// \param  cell_address    The cell address.
    /// \param  cell_offset     The cell offset, in cell units.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void readCell(const ScrollScreenStatus& screen,
                  const PatternNameData&    pnd,
                  const u32                 cell_address,
                  const ScreenOffset&       cell_offset);

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
        const auto color_address = u32{cram_start_address + screen.color_ram_address_offset | (palette_number + dot) * sizeof(T)};
        auto       color         = readColor<T>(color_address);
        if (!dot && screen.is_transparency_code_valid) color.a = 0;

        texture_data.insert(texture_data.end(), {color.r, color.g, color.b, color.a});
    };

    template<typename T>
    void readPalette256Dot(std::vector<u8>&          texture_data,
                           const ScrollScreenStatus& screen,
                           const u16                 palette_number,
                           const u8                  dot) {
        const auto color_address = u32{cram_start_address + screen.color_ram_address_offset | (palette_number + dot) * sizeof(T)};
        auto       color         = readColor<T>(color_address);
        // if (screen.format == ScrollScreenFormat::bitmap) {
        //    if (dot) DebugBreak();
        //}
        if (!dot && screen.is_transparency_code_valid) color.a = 0;

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
        // constexpr auto palette_disp    = u8{4};
        // const auto     palette         = palette_number << palette_disp;
        for (u32 i = 0; i < 8; ++i) {
            auto row = Dots4Bits{modules_.memory()->read<u32>(current_address)};
            readPalette16Dot<T>(texture_data, screen, palette_number, row.dot_0);
            readPalette16Dot<T>(texture_data, screen, palette_number, row.dot_1);
            readPalette16Dot<T>(texture_data, screen, palette_number, row.dot_2);
            readPalette16Dot<T>(texture_data, screen, palette_number, row.dot_3);
            readPalette16Dot<T>(texture_data, screen, palette_number, static_cast<u8>(row.dot_4));
            readPalette16Dot<T>(texture_data, screen, palette_number, static_cast<u8>(row.dot_5));
            readPalette16Dot<T>(texture_data, screen, palette_number, static_cast<u8>(row.dot_6));
            readPalette16Dot<T>(texture_data, screen, palette_number, static_cast<u8>(row.dot_7));
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
        constexpr auto palette_disp    = u8{8};
        // const auto     palette         = palette_number << palette_disp;
        // const auto palette = palette_number;

        for (u32 i = 0; i < 8; ++i) {
            auto row = Dots8Bits{modules_.memory()->read<u32>(current_address)};
            readPalette256Dot<T>(texture_data, screen, palette_number, row.dot_0);
            readPalette256Dot<T>(texture_data, screen, palette_number, row.dot_1);
            readPalette256Dot<T>(texture_data, screen, palette_number, static_cast<u8>(row.dot_2));
            readPalette256Dot<T>(texture_data, screen, palette_number, row.dot_3);
            current_address += row_offset;
            row = Dots8Bits{modules_.memory()->read<u32>(current_address)};
            readPalette256Dot<T>(texture_data, screen, palette_number, row.dot_0);
            readPalette256Dot<T>(texture_data, screen, palette_number, row.dot_1);
            readPalette256Dot<T>(texture_data, screen, palette_number, static_cast<u8>(row.dot_2));
            readPalette256Dot<T>(texture_data, screen, palette_number, row.dot_3);
            current_address += row_offset;
        }
    }

    Color read2048ColorsData();
    Color read32KColorsData();
    Color read16MColorsData();

    template<typename T>
    void read256ColorsBitmapData(std::vector<u8>& texture_data, const ScrollScreenStatus& screen) {
        constexpr auto offset          = u8{4};
        auto           current_address = screen.bitmap_start_address;
        const auto     end_address     = current_address + static_cast<u32>(texture_data.capacity()) / 4;
        constexpr auto palette_disp    = u8{8};
        const auto     palette         = screen.bitmap_palette_number << palette_disp;
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

        auto mf = BS::multi_future<std::vector<u8>>{};
        for (u32 i = screen.bitmap_start_address; i < end_address; i += (offset * 2)) {
            mf.f.push_back(ThreadPool::pool_.submit([&]() {
                auto           local_texture      = std::vector<u8>{};
                auto           local_address      = current_address;
                constexpr auto local_texture_size = 8 * 4;
                local_texture.reserve(local_texture_size);
                auto row = Dots8Bits{modules_.memory()->read<u32>(current_address)};
                readPalette256Dot<T>(local_texture, screen, palette, row.dot_0);
                readPalette256Dot<T>(local_texture, screen, palette, row.dot_1);
                readPalette256Dot<T>(local_texture, screen, palette, static_cast<u8>(row.dot_2));
                readPalette256Dot<T>(local_texture, screen, palette, row.dot_3);
                local_address += offset;
                row = Dots8Bits{modules_.memory()->read<u32>(current_address)};
                readPalette256Dot<T>(local_texture, screen, palette, row.dot_0);
                readPalette256Dot<T>(local_texture, screen, palette, row.dot_1);
                readPalette256Dot<T>(local_texture, screen, palette, static_cast<u8>(row.dot_2));
                readPalette256Dot<T>(local_texture, screen, palette, row.dot_3);
                local_address += offset;
                return local_texture;
            }));
            current_address += offset * 2;
        }

        auto futures = mf.get();
        for (auto& v : futures) {
            std::move(std::begin(v), std::end(v), std::back_inserter(texture_data));
        }

        // auto bitmap_parts = std::vector<std::pair<u32, std::vector<u8>>>{};
        // auto order        = u32{};
        // for (u32 i = screen.bitmap_start_address; i < end_address; i += (offset * 2)) {
        //     ThreadPool::pool_.push_task([&]() {
        //         auto           local_texture      = std::vector<u8>{};
        //         constexpr auto local_texture_size = 8 * 4;
        //         local_texture.reserve(local_texture_size);
        //         auto row = Dots8Bits{modules_.memory()->read<u32>(current_address)};
        //         readPalette256Dot<T>(local_texture, screen, palette, row.dot_0);
        //         readPalette256Dot<T>(local_texture, screen, palette, row.dot_1);
        //         readPalette256Dot<T>(local_texture, screen, palette, static_cast<u8>(row.dot_2));
        //         readPalette256Dot<T>(local_texture, screen, palette, row.dot_3);
        //         current_address += offset;
        //         row = Dots8Bits{modules_.memory()->read<u32>(current_address)};
        //         readPalette256Dot<T>(local_texture, screen, palette, row.dot_0);
        //         readPalette256Dot<T>(local_texture, screen, palette, row.dot_1);
        //         readPalette256Dot<T>(local_texture, screen, palette, static_cast<u8>(row.dot_2));
        //         readPalette256Dot<T>(local_texture, screen, palette, row.dot_3);
        //         current_address += offset;
        //         bitmap_parts.push_back(std::pair(order, local_texture));
        //     });
        //     ++order;
        // }

        // auto futures = mf.get();
        // for (auto& v : futures) {
        //     std::move(std::begin(v), std::end(v), std::back_inserter(texture_data));
        // }

        // if (!texture_data.empty()) {
        //     std::ofstream outfile("bitmap_seq.dat", std::ofstream::binary);
        //     // outfile.write(reinterpret_cast<const char*>(texture_data.data(), sizeof(u8) * texture_data.size());
        //     outfile.write(reinterpret_cast<const char*>(texture_data.data()), sizeof(u8) * texture_data.size());
        //     outfile.close();
        // }

        // std::future<std::vector<u8>> my_future = core::ThreadPool::pool_.submit([] { return std::vector<u8>{0x01, 0x02, 0x04};
        // });
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Vdp2::saveBitmap(const ScrollScreenStatus& screen, std::vector<u8>& texture_data, const u16 width, const u16
    /// height, const size_t key);
    ///
    /// \brief  Saves a bitmap to the parts vector.
    ///
    /// \author Runik
    /// \date   17/08/2021
    ///
    /// \param          screen          Current scroll screen status.
    /// \param [in,out] texture_data    Raw texture data.
    /// \param          width           Bitmap width.
    /// \param          height          Bitmap height.
    /// \param          key             Texture key.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void saveBitmap(const ScrollScreenStatus& screen,
                    std::vector<u8>&          texture_data,
                    const u16                 width,
                    const u16                 height,
                    const size_t              key);

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

    auto getColorRamAddressOffset(const u8 register_offset) -> u16;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Vdp2::resetCacheState();
    ///
    /// \brief  Resets the cache state.
    ///
    /// \author Runik
    /// \date   07/10/2021
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void resetCacheState();

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
    /// \fn void Vdp2::discardCache(const ScrollScreen screen);
    ///
    /// \brief  Discards cache data for the scroll screen.
    ///
    /// \author Runik
    /// \date   08/10/2021
    ///
    /// \param  screen  The scroll screen to manage.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void discardCache(const ScrollScreen screen);

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

    std::vector<Vdp2Part> vdp2_parts_[6]; ///< Storage of rendering parts for each scroll cell.
    // std::vector<std::unique_ptr<video::BaseRenderingPart>> vdp2_parts_[6]; ///< Storage of rendering parts for each scroll
    //   cell.
    ScrollScreen         screen_in_debug_{ScrollScreen::none}; ///< Scroll screen currently viewed in debug.
    DisabledScrollScreen disabled_scroll_screens_;             ///< Disabling state of scroll screens.

    std::chrono::time_point<std::chrono::steady_clock> current_time_;
    std::chrono::time_point<std::chrono::steady_clock> new_time_;

    ///@{
    /// \name VDP2 registers
    TvScreenMode                                    tvmd_;
    ExternalSignalEnable                            exten_;
    ScreenStatus                                    tvstat_;
    VramSizeRegister                                vrsize_;
    HCounter                                        hcnt_;
    VCounter                                        vcnt_;
    RamControl                                      ramctl_;
    Reserve                                         rsv1_;
    VramCyclePatternBankLower                       cyca0l_;
    VramCyclePatternBankUpper                       cyca0u_;
    VramCyclePatternBankLower                       cyca1l_;
    VramCyclePatternBankUpper                       cyca1u_;
    VramCyclePatternBankLower                       cycb0l_;
    VramCyclePatternBankUpper                       cycb0u_;
    VramCyclePatternBankLower                       cycb1l_;
    VramCyclePatternBankUpper                       cycb1u_;
    ScreenDisplayEnable                             bgon_;
    MosaicControl                                   mzctl_;
    SpecialFunctionCodeSelect                       sfsel_;
    SpecialFunctionCode                             sfcode_;
    CharacterControlA                               chctla_;
    CharacterControlB                               chctlb_;
    BitmapPaletteNumberA                            bmpna_;
    BitmapPaletteNumberB                            bmpnb_;
    PatternNameControl                              pncn0_;
    PatternNameControl                              pncn1_;
    PatternNameControl                              pncn2_;
    PatternNameControl                              pncn3_;
    PatternNameControl                              pncr_;
    PlaneSizeRegister                               plsz_;
    MapOffsetNbg                                    mpofn_;
    MapOffsetRbg                                    mpofr_;
    MapPlaneAB                                      mpabn0_;
    MapPlaneCD                                      mpcdn0_;
    MapPlaneAB                                      mpabn1_;
    MapPlaneCD                                      mpcdn1_;
    MapPlaneAB                                      mpabn2_;
    MapPlaneCD                                      mpcdn2_;
    MapPlaneAB                                      mpabn3_;
    MapPlaneCD                                      mpcdn3_;
    MapPlaneAB                                      mpabra_;
    MapPlaneCD                                      mpcdra_;
    MapPlaneEF                                      mpefra_;
    MapPlaneGH                                      mpghra_;
    MapPlaneIJ                                      mpijra_;
    MapPlaneKL                                      mpklra_;
    MapPlaneMN                                      mpmnra_;
    MapPlaneOP                                      mpopra_;
    MapPlaneAB                                      mpabrb_;
    MapPlaneCD                                      mpcdrb_;
    MapPlaneEF                                      mpefrb_;
    MapPlaneGH                                      mpghrb_;
    MapPlaneIJ                                      mpijrb_;
    MapPlaneKL                                      mpklrb_;
    MapPlaneMN                                      mpmnrb_;
    MapPlaneOP                                      mpoprb_;
    ScreenScrollValueIntegerPart                    scxin0_;
    ScreenScrollValueFractionalPart                 scxdn0_;
    ScreenScrollValueIntegerPart                    scyin0_;
    ScreenScrollValueFractionalPart                 scydn0_;
    CoordinateIncrementNbg0HorizontalIntegerPart    zmxin0_;
    CoordinateIncrementNbg0HorizontalFractionalPart zmxdn0_;
    CoordinateIncrementNbg0VerticalIntegerPart      zmyin0_;
    CoordinateIncrementNbg0VerticalFractionalPart   zmydn0_;
    ScreenScrollValueIntegerPart                    scxin1_;
    ScreenScrollValueFractionalPart                 scxdn1_;
    ScreenScrollValueIntegerPart                    scyin1_;
    ScreenScrollValueFractionalPart                 scydn1_;
    CoordinateIncrementNbg1HorizontalIntegerPart    zmxin1_;
    CoordinateIncrementNbg1HorizontalFractionalPart zmxdn1_;
    CoordinateIncrementNbg1VerticalIntegerPart      zmyin1_;
    CoordinateIncrementNbg1VerticalFractionalPart   zmydn1_;
    ScreenScrollValueIntegerPart                    scxn2_;
    ScreenScrollValueIntegerPart                    scyn2_;
    ScreenScrollValueIntegerPart                    scxn3_;
    ScreenScrollValueIntegerPart                    scyn3_;
    ReductionEnable                                 zmctl_;
    LineAndVerticalCellScrollControl                scrctl_;
    VerticalCellScrollTableAddressUpper             vcstau_;
    VerticalCellScrollTableAddressLower             vcstal_;
    LineScrollTableAddressNbg0Upper                 lsta0u_;
    LineScrollTableAddressNbg0Lower                 lsta0l_;
    LineScrollTableAddressNbg1Upper                 lsta1u_;
    LineScrollTableAddressNbg1Lower                 lsta1l_;
    LineColorScreenTableAddressUpper                lctau_;
    LineColorScreenTableAddressLower                lctal_;
    BackScreenTableAddressUpper                     bktau_;
    BackScreenTableAddressLower                     bktal_;
    RotationParameterMode                           rpmd_;
    RotationParameterReadControl                    rprctl_;
    CoefficientTableControl                         ktctl_;
    CoefficientTableAddressOffset                   ktaof_;
    ScreenOverPatternNameA                          ovpnra_;
    ScreenOverPatternNameB                          ovpnrb_;
    RotationParameterTableAddressUpper              rptau_;
    RotationParameterTableAddressLower              rptal_;
    WindowPositionW0HorizontalStart                 wpsx0_;
    WindowPositionW0VerticalStart                   wpsy0_;
    WindowPositionW0HorizontalEnd                   wpex0_;
    WindowPositionW0VerticalEnd                     wpey0_;
    WindowPositionW1HorizontalStart                 wpsx1_;
    WindowPositionW1VerticalStart                   wpsy1_;
    WindowPositionW1HorizontalEnd                   wpex1_;
    WindowPositionW1VerticalEnd                     wpey1_;
    WindowControlA                                  wctla_;
    WindowControlB                                  wctlb_;
    WindowControlC                                  wctlc_;
    WindowControlD                                  wctld_;
    LineWindowTableAddressW0Upper                   lwta0u_;
    LineWindowTableAddressW0Lower                   lwta0l_;
    LineWindowTableAddressW1Upper                   lwta1u_;
    LineWindowTableAddressW1Lower                   lwta1l_;
    SpriteControl                                   spctl_;
    ShadowControl                                   sdctl_;
    ColorRamAddressOffsetA                          craofa_;
    ColorRamAddressOffsetB                          craofb_;
    LineColorScreenEnable                           lnclen_;
    SpecialPriorityMode                             sfprmd_;
    ColorCalculationControl                         ccctl_;
    SpecialColorCalculationMode                     sfccmd_;
    PriorityNumberSpriteA                           prisa_;
    PriorityNumberSpriteB                           prisb_;
    PriorityNumberSpriteC                           prisc_;
    PriorityNumberSpriteD                           prisd_;
    PriorityNumberA                                 prina_;
    PriorityNumberB                                 prinb_;
    PriorityNumberR                                 prir_;
    Reserve                                         rsv2_;
    ColorCalculationRatioSpriteA                    ccrsa_;
    ColorCalculationRatioSpriteB                    ccrsb_;
    ColorCalculationRatioSpriteC                    ccrsc_;
    ColorCalculationRatioSpriteD                    ccrsd_;
    ColorCalculationRatioNbg0Nbg1                   ccrna_;
    ColorCalculationRatioNbg2Nbg3                   ccrnb_;
    ColorCalculationRatioRbg0                       ccrr_;
    ColorCalculationRatioLineColorBack              ccrlb_;
    ColorOffsetEnable                               clofen_;
    ColorOffsetSelect                               clofsl_;
    ColorOffsetARed                                 coar_;
    ColorOffsetAGreen                               coag_;
    ColorOffsetABlue                                coab_;
    ColorOffsetBRed                                 cobr_;
    ColorOffsetBGreen                               cobg_;
    ColorOffsetBBlue                                cobb_;
    ///@}
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

} // namespace saturnin::video
