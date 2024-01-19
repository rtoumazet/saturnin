//
// vdp1_part.h
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
/// \file	vdp1_part.h
///
/// \brief	VDP1 part definition.
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <saturnin/src/emulator_defs.h>
#include <saturnin/src/video/vdp_common.h>
#include <saturnin/src/video/vdp1_registers.h>
#include <saturnin/src/emulator_modules.h>
#include <saturnin/src/log.h>
#include <saturnin/src/utilities.h> // toUnderlying
#include <saturnin/src/memory.h>

// Forward declarations
namespace saturnin::core {
class EmulatorModules;
} // namespace saturnin::core

namespace saturnin::video {

using core::EmulatorModules;
using core::Log;
using core::Logger;
using core::Memory;
using utilities::toUnderlying;

constexpr auto vdp1_vram_start_address = u32{0x25C00000};
constexpr auto cmdctrl_offset          = u8{0x0};
constexpr auto cmdlink_offset          = u8{0x2};
constexpr auto cmdpmod_offset          = u8{0x4};
constexpr auto cmdcolr_offset          = u8{0x6};
constexpr auto cmdsrca_offset          = u8{0x8};
constexpr auto cmdsize_offset          = u8{0xa};
constexpr auto cmdxa_offset            = u8{0xc};
constexpr auto cmdya_offset            = u8{0xe};
constexpr auto cmdxb_offset            = u8{0x10};
constexpr auto cmdyb_offset            = u8{0x12};
constexpr auto cmdxc_offset            = u8{0x14};
constexpr auto cmdyc_offset            = u8{0x16};
constexpr auto cmdxd_offset            = u8{0x18};
constexpr auto cmdyd_offset            = u8{0x1a};
constexpr auto cmdgrda_offset          = u8{0x1c};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  Vdp1Part
///
/// \brief  Defines a VDP1 part. This class cannot be inherited.
///
/// \author Runik
/// \date   13/05/2021
////////////////////////////////////////////////////////////////////////////////////////////////////

class Vdp1Part {
  public:
    ///@{
    /// Constructors / Destructors
    Vdp1Part() = default;
    Vdp1Part(const EmulatorModules& modules,
             const DrawType         type,
             const u32              table_address,
             const CmdCtrlType&     cmdctrl,
             const CmdLinkType&     cmdlink,
             const ColorF&          color_offset);
    Vdp1Part(const Vdp1Part&)                      = default;
    Vdp1Part(Vdp1Part&&)                           = default;
    auto operator=(const Vdp1Part&) & -> Vdp1Part& = default;
    auto operator=(Vdp1Part&&) & -> Vdp1Part&      = default;
    ~Vdp1Part()                                    = default;
    ///@}

    static void SetLocalCoordinates(const s16 x, const s16 y);

    ///@{
    /// \name  Calculated coordinates.
    auto calculatedXA() const -> s16;
    auto calculatedYA() const -> s16;
    auto calculatedXB() const -> s16;
    auto calculatedYB() const -> s16;
    auto calculatedXC() const -> s16;
    auto calculatedYC() const -> s16;
    auto calculatedXD() const -> s16;
    auto calculatedYD() const -> s16;
    ///@}

    ///@{
    /// \name  Part registers
    CmdCtrlType             cmdctrl_;       ///< Control words.
    CmdLinkType             cmdlink_;       ///< Link specification.
    CmdPmodType             cmdpmod_;       ///< Draw mode words.
    CmdColrType             cmdcolr_;       ///< Color control word.
    CmdSrcaType             cmdsrca_;       ///< Character address.
    CmdSizeType             cmdsize_;       ///< Character size.
    CmdVertexCoordinateType cmdxa_;         ///< Vertex coordinate data.
    CmdVertexCoordinateType cmdya_;         ///< Vertex coordinate data.
    CmdVertexCoordinateType cmdxb_;         ///< Vertex coordinate data.
    CmdVertexCoordinateType cmdyb_;         ///< Vertex coordinate data.
    CmdVertexCoordinateType cmdxc_;         ///< Vertex coordinate data.
    CmdVertexCoordinateType cmdyc_;         ///< Vertex coordinate data.
    CmdVertexCoordinateType cmdxd_;         ///< Vertex coordinate data.
    CmdVertexCoordinateType cmdyd_;         ///< Vertex coordinate data.
    CmdGrdaType             cmdgrda_;       ///< Gouraud shading table.
    u32                     table_address_; ///< The table address.
                                            ///@}

    CommonVdpData common_vdp_data_; ///< Data shared between different VDP parts.

    ///@{
    /// Accessors / mutators
    void debugHeader(std::string_view s) { debug_header_ = s; }
    auto debugHeader() const -> std::string { return debug_header_; }
    ///@}

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto final::getDebugDetail() -> std::string;
    ///
    /// \brief  Returns the debug detail of the current part .
    ///
    /// \author Runik
    /// \date   11/06/2021
    ///
    /// \returns    The debug detail of the part.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto getDebugDetail() -> std::string;

  private:
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void final::readParameters(Memory* m, const u32 address);
    ///
    /// \brief  Reads the parameters of the part, depending on its type.
    ///
    /// \author Runik
    /// \date   13/05/2021
    ///
    /// \param [in,out] m       Pointer to the memory object, used for memory access.
    /// \param          address The address of the table.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void readParameters(Memory* m, const u32 address);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void final::generatePartData(const EmulatorModules& modules);
    ///
    /// \brief  Generates the current part data.
    ///
    /// \author Runik
    /// \date   14/05/2021
    ///
    /// \param  modules The emulator modules.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void generatePartData(const EmulatorModules& modules);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void final::calculatePriority(const EmulatorModules& modules);
    ///
    /// \brief  Calculates the priority of the part.
    ///
    /// \author Runik
    /// \date   03/09/2021
    ///
    /// \param  modules The emulator modules.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void calculatePriority(const EmulatorModules& modules);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	auto final::getPriorityRegister(const EmulatorModules& modules, const u8 priority) const -> u8;
    ///
    /// \brief	Gets the priority register depending on dot data.
    ///
    /// \author	Runik
    /// \date	16/09/2021
    ///
    /// \param 	modules 	The color mode.
    /// \param 	priority	The priority.
    ///
    /// \returns	The priority register.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto getPriorityRegister(const EmulatorModules& modules, const u8 priority) const -> u8;

    EmulatorModules modules_;

    ///@{
    /// Local coordinates
    static s16 local_coordinate_x_;
    static s16 local_coordinate_y_;
    ///@}

    std::string debug_header_{}; ///< Debug header of the part
};

void normalSpriteDraw(const EmulatorModules& modules, Vdp1Part& part);
void scaledSpriteDraw(const EmulatorModules& modules, Vdp1Part& part);
void distortedSpriteDraw(const EmulatorModules& modules, Vdp1Part& part);
void polygonDraw(const EmulatorModules& modules, Vdp1Part& part);
void polylineDraw(const EmulatorModules& modules, Vdp1Part& part);
void lineDraw(const EmulatorModules& modules, Vdp1Part& part);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn	void loadTextureData(const EmulatorModules& modules, Vdp1Part& part);
///
/// \brief	Loads texture data in a texture for the part.
///
/// \author	Runik
/// \date	19/11/2022
///
/// \param 		   	modules	Emulator modules.
/// \param [in,out]	part   	The Vdp1Part being processed.
////////////////////////////////////////////////////////////////////////////////////////////////////

void loadTextureData(const EmulatorModules& modules, Vdp1Part& part);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn	auto readGouraudData(const EmulatorModules& modules, const Vdp1Part& part) -> std::vector<Gouraud>;
///
/// \brief	Reads gouraud data for the part.
///
/// \author	Runik
/// \date	19/11/2022
///
/// \param 	modules	Emulator modules.
/// \param 	part   	The Vdp1Part being processed.
///
/// \returns	The gouraud data.
////////////////////////////////////////////////////////////////////////////////////////////////////

auto readGouraudData(const EmulatorModules& modules, const Vdp1Part& part) -> std::vector<Gouraud>;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn template<typename T> void readDotColorBank16(const EmulatorModules& modules, std::vector<u8>& texture_data, const u16
/// color_ram_offset, Vdp1Part& vp, const u8 dot)
///
/// \brief  Reads one dot in color bank 16 colors format.
///
/// \tparam T   Generic type parameter.
/// \param          modules             Emulator modules.
/// \param [in,out] texture_data        Raw texture data.
/// \param          color_ram_offset    The color ram offset.
/// \param [in,out] vp                  The Vdp1Part being processed.
/// \param          dot                 The current dot.
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
void readDotColorBank16(const EmulatorModules& modules,
                        std::vector<u8>&       texture_data,
                        const u16              color_ram_offset,
                        Vdp1Part&              part,
                        const u8               dot) {
    constexpr auto color_bank_mask = u16{0x0FF0};
    const auto     color_address
        = u32{cram_start_address + color_ram_offset + ((part.cmdcolr_.data() & color_bank_mask) | dot) * sizeof(T)};

    auto color = modules.vdp2()->readColor<T>(color_address);

    // Checking transparency.
    if ((part.cmdpmod_ >> CmdPmod::spd_enum) == CmdPmod::TransparentPixelDisable::transparent_pixel_enabled) {
        if (!dot) color.a = 0;
    }

    texture_data.insert(texture_data.end(), {color.r, color.g, color.b, color.a});
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn template<typename T> void readDotLookUpTable16(const EmulatorModules& modules, std::vector<u8>& texture_data, Vdp1Part&
/// vp, const u8 dot)
///
/// \brief  Reads dot look up table 16
///
/// \tparam T   Generic type parameter.
/// \param          modules             Emulator modules.
/// \param [in,out] texture_data        Raw texture data.
/// \param [in,out] vp                  The Vdp1Part being processed.
/// \param          dot                 The current dot.
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
void readDotLookUpTable16(const EmulatorModules& modules, std::vector<u8>& texture_data, Vdp1Part& part, const u8 dot) {
    const auto color_address = u32{vdp1_vram_start_address + part.cmdcolr_.data() * vdp1_address_multiplier + dot * sizeof(T)};

    auto color = modules.vdp2()->readColor<T>(color_address);

    // Checking transparency.
    if ((part.cmdpmod_ >> CmdPmod::spd_enum) == CmdPmod::TransparentPixelDisable::transparent_pixel_enabled) {
        if (!dot) color.a = 0;
    }

    texture_data.insert(texture_data.end(), {color.r, color.g, color.b, color.a});
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn template<typename T> void readDotColorBank64(const EmulatorModules& modules, std::vector<u8>& texture_data, const u16
/// color_ram_offset, Vdp1Part& part, const u8 dot)
///
/// \brief  Reads one dot in color bank 64 colors format.
///
/// \tparam T   Generic type parameter.
/// \param          modules             Emulator modules.
/// \param [in,out] texture_data        Raw texture data.
/// \param          color_ram_offset    The color ram offset.
/// \param [in,out] part                  The Vdp1Part being processed.
/// \param          dot                 The current dot.
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
void readDotColorBank64(const EmulatorModules& modules,
                        std::vector<u8>&       texture_data,
                        const u16              color_ram_offset,
                        Vdp1Part&              part,
                        const u8               dot) {
    constexpr auto color_bank_mask = u16{0x0FC0};
    const auto     color_address
        = u32{cram_start_address + color_ram_offset + ((part.cmdcolr_.data() & color_bank_mask) | dot) * sizeof(T)};

    auto color = modules.vdp2()->readColor<T>(color_address);

    // Checking transparency.
    if ((part.cmdpmod_ >> CmdPmod::spd_enum) == CmdPmod::TransparentPixelDisable::transparent_pixel_enabled) {
        if (!dot) color.a = 0;
    }

    texture_data.insert(texture_data.end(), {color.r, color.g, color.b, color.a});
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn template<typename T> void readDotColorBank128(const EmulatorModules& modules, std::vector<u8>& texture_data, const u16
/// color_ram_offset, Vdp1Part& part, const u8 dot)
///
/// \brief  Reads one dot in color bank 128 colors format.
///
/// \tparam T   Generic type parameter.
/// \param          modules             Emulator modules.
/// \param [in,out] texture_data        Raw texture data.
/// \param          color_ram_offset    The color ram offset.
/// \param [in,out] part                The Vdp1Part being processed.
/// \param          dot                 The current dot.
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
void readDotColorBank128(const EmulatorModules& modules,
                         std::vector<u8>&       texture_data,
                         const u16              color_ram_offset,
                         Vdp1Part&              part,
                         const u8               dot) {
    constexpr auto color_bank_mask = u16{0x0F80};
    const auto     color_address
        = u32{cram_start_address + color_ram_offset + ((part.cmdcolr_.data() & color_bank_mask) | dot) * sizeof(T)};

    auto color = modules.vdp2()->readColor<T>(color_address);

    // Checking transparency.
    if ((part.cmdpmod_ >> CmdPmod::spd_enum) == CmdPmod::TransparentPixelDisable::transparent_pixel_enabled) {
        if (!dot) color.a = 0;
    }

    texture_data.insert(texture_data.end(), {color.r, color.g, color.b, color.a});
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn template<typename T> void readDotColorBank256(const EmulatorModules& modules, std::vector<u8>& texture_data, const u16
/// color_ram_offset, Vdp1Part& part, const u8 dot)
///
/// \brief  Reads one dot in color bank 256 colors format.
///
/// \tparam T   Generic type parameter.
/// \param          modules             Emulator modules.
/// \param [in,out] texture_data        Raw texture data.
/// \param          color_ram_offset    The color ram offset.
/// \param [in,out] part                The Vdp1Part being processed.
/// \param          dot                 The current dot.
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
void readDotColorBank256(const EmulatorModules& modules,
                         std::vector<u8>&       texture_data,
                         const u16              color_ram_offset,
                         Vdp1Part&              part,
                         const u8               dot) {
    constexpr auto color_bank_mask = u16{0xFF00};
    const auto     color_address
        = u32{cram_start_address + color_ram_offset + ((part.cmdcolr_.data() & color_bank_mask) | dot) * sizeof(T)};

    auto color = modules.vdp2()->readColor<T>(color_address);

    // Checking transparency.
    if ((part.cmdpmod_ >> CmdPmod::spd_enum) == CmdPmod::TransparentPixelDisable::transparent_pixel_enabled) {
        if (!dot) color.a = 0;
    }

    texture_data.insert(texture_data.end(), {color.r, color.g, color.b, color.a});
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn template<typename T> void readDotRgb(std::vector<u8>& texture_data, const u16
/// color_ram_offset, const Vdp1Part& part, const u8 dot)
///
/// \brief  Reads one dot in RGB 32K colors format.
///
/// \tparam T   Generic type parameter.
/// \param [in,out] texture_data        Raw texture data.
/// \param          color_ram_offset    The color ram offset.
/// \param [in,out] part                The Vdp1Part being processed.
/// \param          dot                 The current dot.
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
void readDotRgb(std::vector<u8>& texture_data, const Vdp1Part& part, const u16 dot) {
    auto color = Color(dot);

    // Checking transparency.
    if ((part.cmdpmod_ >> CmdPmod::spd_enum) == CmdPmod::TransparentPixelDisable::transparent_pixel_enabled && !dot) {
        color.a = 0;
    }

    texture_data.insert(texture_data.end(), {color.r, color.g, color.b, color.a});
};

void checkColorCalculation(const Vdp1Part& part);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn template<typename T> void readColorBankMode16Colors(const Memory* m, std::vector<u8>& texture_data, const u32
/// start_address, const u8 color_ram_address_offset)
///
/// \brief  Reads pixel data as color bank mode 16 colors (4 bits per pixel).
///
/// \tparam T   Generic type parameter.
/// \param          modules                     Reference to the emulator modules.
/// \param [in,out] texture_data                Raw texture data.
/// \param          start_address               Texture start address.
/// \param          color_ram_address_offset    The color ram address offset.
/// \param          part                        The vdp1 part currently processed.
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
void readColorBankMode16Colors(const EmulatorModules& modules,
                               std::vector<u8>&       texture_data,
                               const u32              start_address,
                               const u16              color_ram_address_offset,
                               Vdp1Part&              part) {
    constexpr auto one_read_offset = u8{4}; // in bytes
    const auto     texture_size    = texture_data.capacity() / 4;
    if ((texture_size % 4) != 0) {
        // As we're reading 32 bits of data at a time (8 dots of 4 bits) we must ensure the data read is on 32 bits boundary.
        Log::warning(Logger::vdp1, "Texture size isn't a multiple of 4 !");
        return;
    }
    checkColorCalculation(part);
    auto row = DataExtraction{};
    for (u32 i = start_address; i < (start_address + texture_size); i += one_read_offset) {
        row.as_4bits = modules.memory()->read<u32>(i);
        readDotColorBank16<T>(modules,
                              texture_data,
                              color_ram_address_offset,
                              part,
                              row.as_4bits >> DataExtraction::As4Bits::dot0_shift);
        readDotColorBank16<T>(modules,
                              texture_data,
                              color_ram_address_offset,
                              part,
                              row.as_4bits >> DataExtraction::As4Bits::dot1_shift);
        readDotColorBank16<T>(modules,
                              texture_data,
                              color_ram_address_offset,
                              part,
                              row.as_4bits >> DataExtraction::As4Bits::dot2_shift);
        readDotColorBank16<T>(modules,
                              texture_data,
                              color_ram_address_offset,
                              part,
                              row.as_4bits >> DataExtraction::As4Bits::dot3_shift);
        readDotColorBank16<T>(modules,
                              texture_data,
                              color_ram_address_offset,
                              part,
                              row.as_4bits >> DataExtraction::As4Bits::dot4_shift);
        readDotColorBank16<T>(modules,
                              texture_data,
                              color_ram_address_offset,
                              part,
                              row.as_4bits >> DataExtraction::As4Bits::dot5_shift);
        readDotColorBank16<T>(modules,
                              texture_data,
                              color_ram_address_offset,
                              part,
                              row.as_4bits >> DataExtraction::As4Bits::dot6_shift);
        readDotColorBank16<T>(modules,
                              texture_data,
                              color_ram_address_offset,
                              part,
                              row.as_4bits >> DataExtraction::As4Bits::dot7_shift);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn template<typename T> void readLookUpTable16Colors(const EmulatorModules& modules, std::vector<u8>& texture_data, const u32
/// start_address, Vdp1Part& vp)
///
/// \brief  Reads look up table 16 colors
///
/// \tparam T   Generic type parameter.
/// \param          modules                     Reference to the emulator modules.
/// \param [in,out] texture_data                Raw texture data.
/// \param          start_address               Texture start address.
/// \param          part                        The vdp1 part currently processed.
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
void readLookUpTable16Colors(const EmulatorModules& modules,
                             std::vector<u8>&       texture_data,
                             const u32              start_address,
                             Vdp1Part&              part) {
    constexpr auto one_read_offset = u8{4};
    const auto     texture_size    = texture_data.capacity() / 4;
    if ((texture_size % 4) != 0) {
        // As we're reading 32 bits of data at a time (8 dots of 4 bits) we must ensure the data read is on 32 bits boundary.
        Log::warning(Logger::vdp1, "Texture size isn't a multiple of 4 !");
        return;
    }
    checkColorCalculation(part);
    auto row = DataExtraction{};
    for (u32 i = start_address; i < (start_address + texture_size / one_read_offset * 2); i += one_read_offset) {
        row.as_4bits = modules.memory()->read<u32>(i);
        readDotLookUpTable16<T>(modules, texture_data, part, row.as_4bits >> DataExtraction::As4Bits::dot0_shift);
        readDotLookUpTable16<T>(modules, texture_data, part, row.as_4bits >> DataExtraction::As4Bits::dot1_shift);
        readDotLookUpTable16<T>(modules, texture_data, part, row.as_4bits >> DataExtraction::As4Bits::dot2_shift);
        readDotLookUpTable16<T>(modules, texture_data, part, row.as_4bits >> DataExtraction::As4Bits::dot3_shift);
        readDotLookUpTable16<T>(modules, texture_data, part, row.as_4bits >> DataExtraction::As4Bits::dot4_shift);
        readDotLookUpTable16<T>(modules, texture_data, part, row.as_4bits >> DataExtraction::As4Bits::dot5_shift);
        readDotLookUpTable16<T>(modules, texture_data, part, row.as_4bits >> DataExtraction::As4Bits::dot6_shift);
        readDotLookUpTable16<T>(modules, texture_data, part, row.as_4bits >> DataExtraction::As4Bits::dot7_shift);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn template<typename T> void readColorBankMode64Colors(const EmulatorModules& modules, std::vector<u8>& texture_data, const
/// u32 start_address, const u16 color_ram_address_offset, Vdp1Part& part)
///
/// \brief  Reads pixel data as color bank mode 64 colors (6 bits per pixel).
///
/// \tparam T   Generic type parameter.
/// \param          modules                     Reference to the emulator modules.
/// \param [in,out] texture_data                Raw texture data.
/// \param          start_address               Texture start address.
/// \param          color_ram_address_offset    The color ram address offset.
/// \param          part                          The vdp1 part currently processed.
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
void readColorBankMode64Colors(const EmulatorModules& modules,
                               std::vector<u8>&       texture_data,
                               const u32              start_address,
                               const u16              color_ram_address_offset,
                               Vdp1Part&              part) {
    constexpr auto one_read_offset = u8{4}; // in bytes
    const auto     texture_size    = texture_data.capacity() / 4;
    if ((texture_size % 4) != 0) {
        // As we're reading 32 bits of data at a time (4 dots of 6 bits) we must ensure the data read is on 32 bits boundary.
        Log::warning(Logger::vdp1, "Texture size isn't a multiple of 4 !");
        return;
    }
    checkColorCalculation(part);
    auto row = DataExtraction{};
    for (u32 i = start_address; i < (start_address + texture_size / one_read_offset); i += one_read_offset) {
        row.as_6bits = modules.memory()->read<u32>(i);
        readDotColorBank64<T>(modules,
                              texture_data,
                              color_ram_address_offset,
                              part,
                              row.as_6bits >> DataExtraction::As6Bits::dot0_shift);
        readDotColorBank64<T>(modules,
                              texture_data,
                              color_ram_address_offset,
                              part,
                              row.as_6bits >> DataExtraction::As6Bits::dot1_shift);
        readDotColorBank64<T>(modules,
                              texture_data,
                              color_ram_address_offset,
                              part,
                              row.as_6bits >> DataExtraction::As6Bits::dot2_shift);
        readDotColorBank64<T>(modules,
                              texture_data,
                              color_ram_address_offset,
                              part,
                              row.as_6bits >> DataExtraction::As6Bits::dot3_shift);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn template<typename T> void readColorBankMode128Colors(const EmulatorModules& modules, std::vector<u8>& texture_data, const
/// u32 start_address, const u16 color_ram_address_offset, Vdp1Part& part)
///
/// \brief  Reads pixel data as color bank mode 128 colors (7 bits per pixel).
///
/// \tparam T   Generic type parameter.
/// \param          modules                     Reference to the emulator modules.
/// \param [in,out] texture_data                Raw texture data.
/// \param          start_address               Texture start address.
/// \param          color_ram_address_offset    The color ram address offset.
/// \param          part                        The vdp1 part currently processed.
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
void readColorBankMode128Colors(const EmulatorModules& modules,
                                std::vector<u8>&       texture_data,
                                const u32              start_address,
                                const u16              color_ram_address_offset,
                                Vdp1Part&              part) {
    constexpr auto one_read_offset = u8{4}; // in bytes
    const auto     texture_size    = texture_data.capacity() / 4;
    if ((texture_size % 4) != 0) {
        // As we're reading 32 bits of data at a time (4 dots of 7 bits) we must ensure the data read is on 32 bits boundary.
        Log::warning(Logger::vdp1, "Texture size isn't a multiple of 4 !");
        return;
    }
    checkColorCalculation(part);
    auto row = DataExtraction{};
    for (u32 i = start_address; i < (start_address + texture_size / one_read_offset); i += one_read_offset) {
        row.as_7bits = modules.memory()->read<u32>(i);
        readDotColorBank128<T>(modules,
                               texture_data,
                               color_ram_address_offset,
                               part,
                               row.as_7bits >> DataExtraction::As7Bits::dot0_shift);
        readDotColorBank128<T>(modules,
                               texture_data,
                               color_ram_address_offset,
                               part,
                               row.as_7bits >> DataExtraction::As7Bits::dot1_shift);
        readDotColorBank128<T>(modules,
                               texture_data,
                               color_ram_address_offset,
                               part,
                               row.as_7bits >> DataExtraction::As7Bits::dot2_shift);
        readDotColorBank128<T>(modules,
                               texture_data,
                               color_ram_address_offset,
                               part,
                               row.as_7bits >> DataExtraction::As7Bits::dot3_shift);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn template<typename T> void readColorBankMode256Colors(const EmulatorModules& modules, std::vector<u8>& texture_data, const
/// u32 start_address, const u16 color_ram_address_offset, Vdp1Part& part)
///
/// \brief  Reads pixel data as color bank mode 256 colors (8 bits per pixel).
///
/// \tparam T   Generic type parameter.
/// \param          modules                     Reference to the emulator modules.
/// \param [in,out] texture_data                Raw texture data.
/// \param          start_address               Texture start address.
/// \param          color_ram_address_offset    The color ram address offset.
/// \param          part                        The vdp1 part currently processed.
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
void readColorBankMode256Colors(const EmulatorModules& modules,
                                std::vector<u8>&       texture_data,
                                const u32              start_address,
                                const u16              color_ram_address_offset,
                                Vdp1Part&              part) {
    constexpr auto one_read_offset = u8{4}; // in bytes
    const auto     texture_size    = texture_data.capacity() / 4;
    if ((texture_size % 4) != 0) {
        // As we're reading 32 bits of data at a time (4 dots of 8 bits) we must ensure the data read is on 32 bits boundary.
        Log::warning(Logger::vdp1, "Texture size isn't a multiple of 4 !");
        return;
    }
    checkColorCalculation(part);
    auto row = DataExtraction{};
    for (u32 i = start_address; i < (start_address + texture_size); i += one_read_offset) {
        row.as_8bits = modules.memory()->read<u32>(i);
        readDotColorBank256<T>(modules,
                               texture_data,
                               color_ram_address_offset,
                               part,
                               row.as_8bits >> DataExtraction::As8Bits::dot0_shift);
        readDotColorBank256<T>(modules,
                               texture_data,
                               color_ram_address_offset,
                               part,
                               row.as_8bits >> DataExtraction::As8Bits::dot1_shift);
        readDotColorBank256<T>(modules,
                               texture_data,
                               color_ram_address_offset,
                               part,
                               row.as_8bits >> DataExtraction::As8Bits::dot2_shift);
        readDotColorBank256<T>(modules,
                               texture_data,
                               color_ram_address_offset,
                               part,
                               row.as_8bits >> DataExtraction::As8Bits::dot3_shift);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn template<typename T> void readRgb32KColors(const EmulatorModules& modules, std::vector<u8>& texture_data, const u32
/// start_address, Vdp1Part& part)
///
/// \brief  Reads pixel data as RGB 32K colors (16 bits per pixel).
///
/// \tparam T   Generic type parameter.
/// \param          modules         Reference to the emulator modules.
/// \param [in,out] texture_data    Raw texture data.
/// \param          start_address   Texture start address.
/// \param          part            The vdp1 part currently processed.
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
void readRgb32KColors(const EmulatorModules& modules, std::vector<u8>& texture_data, const u32 start_address, Vdp1Part& part) {
    constexpr auto one_read_offset_in_bytes = u8{4};
    const auto     texture_size             = texture_data.capacity();
    if ((texture_size % 4) != 0) {
        // As we're reading 32 bits of data at a time (2 dots of 16 bits) we must ensure the data read is on 32 bits boundary.
        Log::warning(Logger::vdp1, "Texture size isn't a multiple of 4 !");
        return;
    }
    checkColorCalculation(part);

    const auto max_size = static_cast<u32>(start_address + texture_size);
    auto       row      = DataExtraction{};
    for (u32 i = start_address; i < max_size; i += one_read_offset_in_bytes) {
        row.as_16bits = modules.memory()->read<u32>(i);
        readDotRgb<T>(texture_data, part, row.as_16bits >> DataExtraction::As16Bits::dot0_shift);
        readDotRgb<T>(texture_data, part, row.as_16bits >> DataExtraction::As16Bits::dot1_shift);
    }
}

auto getTextureCoordinates(const CmdCtrl::CharacterReadDirection crd) -> std::vector<TextureCoordinates>;

} // namespace saturnin::video
