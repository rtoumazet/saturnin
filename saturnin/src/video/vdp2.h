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
/// \file	vdp1.h
///
/// \brief	Declares the Vdp2 class and related functions.
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <array>                           // array
#include <chrono>                          // duration
#include <saturnin/src/emulator_context.h> // EmulatorContext
#include <saturnin/src/emulator_defs.h>    // u8, u16, u32
#include <saturnin/src/locale.h>           // tr
#include <saturnin/src/log.h>              // Log
#include <saturnin/src/video/vdp2_registers.h>

// Forward declarations
namespace saturnin::core {
// class EmulatorContext;
class Scu;
} // namespace saturnin::core

namespace saturnin::video {

class Vdp1;

using saturnin::core::EmulatorContext;
using AddressToNameMap = std::map<u32, std::string>;

using seconds = std::chrono::duration<double>;
using micro   = std::chrono::duration<double, std::micro>;

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

enum class ScrollScreen { nbg0 = 0, nbg1 = 1, nbg2 = 2, nbg3 = 3, rbg0 = 4, rbg1 = 5 };

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
    VramSize                vram_size{VramSize::not_set};
    VramMode                vram_a_mode{VramMode::no_partition};
    VramMode                vram_b_mode{VramMode::no_partition};
    ColorRamMode            color_ram_mode{ColorRamMode::not_set};
    CoefficientTableStorage coefficient_table_storage{CoefficientTableStorage::not_set};
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
    bool is_display_enabled{};               ///< True when displayed.
    bool is_display_enabled_dirty{};         ///< True when displayed state was changed.
    bool is_transparency_code_valid{};       ///< True when transparency code is valid.
    bool is_transparency_code_valid_dirty{}; ///< True when transparency code was changed.

    ScrollScreenFormat format{ScrollScreenFormat::cell};            ///< Cell or bitmap.
    BitmapSize         bitmap_size{BitmapSize::not_set};            ///< Size of the bitmap
    ColorCount         character_color_number{ColorCount::not_set}; ///< Color number

    u8  map_size{};               ///< Size of the map (2*2 for NBG / 4*4 for RBG)
    u8  map_offset{};             ///< The map offset
    u8  plane_size{};             ///< Size of the plane (1*1, 2*1 or 2*2 pages)
    u16 page_size{};              ///< Size of the page / pattern name table
    u8  pattern_name_data_size{}; ///< Size of the pattern name data (1 or 2 words)
    u8  character_pattern_size{}; ///< Size of the character pattern (1*1 or 2*2 cells)
    u8  cell_size{};              ///< Size of the cell (8*8 dots)

    u32 bitmap_start_address{0}; ///< The bitmap start address

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
};

class Vdp2 {
  public:
    //@{
    // Constructors / Destructors
    Vdp2() = delete;
    Vdp2(EmulatorContext* ec) : emulator_context_(ec){};
    Vdp2(const Vdp2&) = delete;
    Vdp2(Vdp2&&)      = delete;
    auto operator=(const Vdp2&) & -> Vdp2& = delete;
    auto operator=(Vdp2&&) & -> Vdp2& = delete;
    ~Vdp2()                           = default;
    //@}

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
        core::Log::warning("vdp2", core::tr("{}bits register write {:#0x}"), sizeof(T) * number_of_bits_8, addr);
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
        core::Log::warning("vdp2", core::tr("{}bits register read {:#0x}"), sizeof(T) * number_of_bits_8, addr);
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
    /// \fn auto Vdp2::getRenderVertexes() const -> const std::vector<Vertex>&;
    ///
    /// \brief  Returns the vertexes to render.
    ///
    /// \author Runik
    /// \date   03/02/2021
    ///
    /// \returns    A reference to a std::vector&lt;Vertex&gt;
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto getRenderVertexes() const -> const std::vector<Vertex>&;

    //--------------------------------------------------------------------------------------------------------------
    // DEBUG methods
    //--------------------------------------------------------------------------------------------------------------

    /// \name Various Vdp2 debug functions
    //@{
    auto        getDebugGlobalMainData() const -> std::vector<LabelValue>;
    auto        getDebugRamMainData() -> std::vector<LabelValue>;
    auto        getDebugVramAccessMainData() -> std::vector<LabelValue>;
    auto        getDebugVramAccessBanks() -> std::vector<VramTiming>;
    auto        getDebugVramAccessBanksUsed() -> std::array<bool, vram_banks_number>;
    auto        getDebugVramAccessBanksName() -> std::vector<std::string>;
    static auto getDebugVramAccessCommandDescription(const VramAccessCommand command) -> LabelValue;
    auto        getDebugScrollScreenData(const ScrollScreen s) -> std::optional<std::vector<LabelValue>>;

    //@}
  private:
    //--------------------------------------------------------------------------------------------------------------
    // MEMORY ACCESS methods
    //--------------------------------------------------------------------------------------------------------------

    /// \name Vdp2 registers accessors
    //@{
    [[nodiscard]] auto read16(u32 addr) const -> u16;
    void               write16(u32 addr, u16 data);
    void               write32(u32 addr, u32 data);
    //@}

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
    /// \fn static auto Vdp2::calculateRequiredVramCharacterPatternReads(ReductionSetting r, CharacterColorNumber3bits ccn) ->
    /// VramAccessNumber;
    ///
    /// \brief  Calculates the required VRAM character/bitmap pattern reads to display a screen.
    ///
    /// \author Runik
    /// \date   10/08/2020
    ///
    /// \param  r   The reduction setting.
    /// \param  ccn The character color number.
    ///
    /// \returns    The required VRAM character pattern reads.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static auto calculateRequiredVramCharacterPatternReads(ReductionSetting r, CharacterColorNumber3bits ccn) -> VramAccessNumber;

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
    /// \fn auto calculateRequiredVramPatternNameReads(ReductionSetting r) -> VramAccessNumber;
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

    auto calculateRequiredVramPatternNameReads(ReductionSetting r) -> VramAccessNumber;

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
    /// \param          is_screen_mode_normal   True when the TV screen mode is normal.
    /// \param          pnd_access              True when the pattern name data is used.
    /// \param [in,out] allowed_cpd_timing      Sets true for the allowed character pattern timings.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static void setCharacterPatternLimitations(const bool                                is_screen_mode_normal,
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
    ///
    /// \returns    The VRAM character pattern data reads.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static auto getVramCharacterPatternDataReads(const VramTiming&       bank_a0,
                                                 const VramTiming&       bank_a1,
                                                 const VramTiming&       bank_b0,
                                                 const VramTiming&       bank_b1,
                                                 const VramAccessCommand command,
                                                 const ReductionSetting  reduction,
                                                 const bool              is_screen_mode_normal) -> u8;

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

    //@{
    // Modules accessors
    [[nodiscard]] auto scu() const -> core::Scu* { return emulator_context_->scu(); };
    [[nodiscard]] auto vdp1() const -> Vdp1* { return emulator_context_->vdp1(); };
    //@}

    EmulatorContext* emulator_context_; ///< Emulator context object.

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

    std::vector<Vertex> render_vertexes_; ///< Contains all the geometry vertexes (VDP1 & VDP2).

    std::array<ScrollScreenStatus, 6> bg_; ///< The Normal Backgrounds.

    // VDP2 registers
    TvScreenMode                                    tvmd_;
    ExternalSignalEnable                            exten_;
    ScreenStatus                                    tvstat_;
    VramSizeRegister                                vrsize_;
    HCounter                                        hcnt_;
    VCounter                                        vcnt_;
    RamControl                                      ramctl_;
    Reserve                                         rsv1_;
    VramCyclePatternBankA0Lower                     cyca0l_;
    VramCyclePatternBankA0Upper                     cyca0u_;
    VramCyclePatternBankA1Lower                     cyca1l_;
    VramCyclePatternBankA1Upper                     cyca1u_;
    VramCyclePatternBankB0Lower                     cycb0l_;
    VramCyclePatternBankB0Upper                     cycb0u_;
    VramCyclePatternBankB1Lower                     cycb1l_;
    VramCyclePatternBankB1Upper                     cycb1u_;
    ScreenDisplayEnable                             bgon_;
    MosaicControl                                   mzctl_;
    SpecialFunctionCodeSelect                       sfsel_;
    SpecialFunctionCode                             sfcode_;
    CharacterControlA                               chctla_;
    CharacterControlB                               chctlb_;
    BitmapPaletteNumberA                            bmpna_;
    BitmapPaletteNumberB                            bmpnb_;
    PatternNameControlNbg0                          pncn0_;
    PatternNameControlNbg1                          pncn1_;
    PatternNameControlNbg2                          pncn2_;
    PatternNameControlNbg3                          pncn3_;
    PatternNameControlRbg0                          pncr_;
    PlaneSizeRegister                               plsz_;
    MapOffsetNbg                                    mpofn_;
    MapOffsetRbg                                    mpofr_;
    MapNbg0PlaneAB                                  mpabn0_;
    MapNbg0PlaneCD                                  mpcdn0_;
    MapNbg1PlaneAB                                  mpabn1_;
    MapNbg1PlaneCD                                  mpcdn1_;
    MapNbg2PlaneAB                                  mpabn2_;
    MapNbg2PlaneCD                                  mpcdn2_;
    MapNbg3PlaneAB                                  mpabn3_;
    MapNbg3PlaneCD                                  mpcdn3_;
    MapRotationParameterAPlaneAB                    mpabra_;
    MapRotationParameterAPlaneCD                    mpcdra_;
    MapRotationParameterAPlaneEF                    mpefra_;
    MapRotationParameterAPlaneGH                    mpghra_;
    MapRotationParameterAPlaneIJ                    mpijra_;
    MapRotationParameterAPlaneKL                    mpklra_;
    MapRotationParameterAPlaneMN                    mpmnra_;
    MapRotationParameterAPlaneOP                    mpopra_;
    MapRotationParameterBPlaneAB                    mpabrb_;
    MapRotationParameterBPlaneCD                    mpcdrb_;
    MapRotationParameterBPlaneEF                    mpefrb_;
    MapRotationParameterBPlaneGH                    mpghrb_;
    MapRotationParameterBPlaneIJ                    mpijrb_;
    MapRotationParameterBPlaneKL                    mpklrb_;
    MapRotationParameterBPlaneMN                    mpmnrb_;
    MapRotationParameterBPlaneOP                    mpoprb_;
    ScreenScrollValueNbg0HorizontalIntegerPart      scxin0_;
    ScreenScrollValueNbg0HorizontalFractionalPart   scxdn0_;
    ScreenScrollValueNbg0VerticalIntegerPart        scyin0_;
    ScreenScrollValueNbg0VerticalFractionalPart     scydn0_;
    CoordinateIncrementNbg0HorizontalIntegerPart    zmxin0_;
    CoordinateIncrementNbg0HorizontalFractionalPart zmxdn0_;
    CoordinateIncrementNbg0VerticalIntegerPart      zmyin0_;
    CoordinateIncrementNbg0VerticalFractionalPart   zmydn0_;
    ScreenScrollValueNbg1HorizontalIntegerPart      scxin1_;
    ScreenScrollValueNbg1HorizontalFractionalPart   scxdn1_;
    ScreenScrollValueNbg1VerticalIntegerPart        scyin1_;
    ScreenScrollValueNbg1VerticalFractionalPart     scydn1_;
    CoordinateIncrementNbg1HorizontalIntegerPart    zmxin1_;
    CoordinateIncrementNbg1HorizontalFractionalPart zmxdn1_;
    CoordinateIncrementNbg1VerticalIntegerPart      zmyin1_;
    CoordinateIncrementNbg1VerticalFractionalPart   zmydn1_;
    ScreenScrollValueNbg2Horizontal                 scxn2_;
    ScreenScrollValueNbg2Vertical                   scyn2_;
    ScreenScrollValueNbg3Horizontal                 scxn3_;
    ScreenScrollValueNbg3Vertical                   scyn3_;
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
};

} // namespace saturnin::video
