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
#include <saturnin/src/video/base_rendering_part.h>
#include <saturnin/src/video/vdp_common.h>
#include <saturnin/src/video/vdp1_registers.h>
#include <saturnin/src/emulator_modules.h>
#include <saturnin/src/log.h>

// Forward declarations
namespace saturnin::core {
class EmulatorModules;
class Memory;
} // namespace saturnin::core

namespace saturnin::video {

using core::EmulatorModules;
using core::Log;
using core::Memory;

constexpr auto cmdctrl_offset = u8{0x0};
constexpr auto cmdlink_offset = u8{0x2};
constexpr auto cmdpmod_offset = u8{0x4};
constexpr auto cmdcolr_offset = u8{0x6};
constexpr auto cmdsrca_offset = u8{0x8};
constexpr auto cmdsize_offset = u8{0xa};
constexpr auto cmdxa_offset   = u8{0xc};
constexpr auto cmdya_offset   = u8{0xe};
constexpr auto cmdxb_offset   = u8{0x10};
constexpr auto cmdyb_offset   = u8{0x12};
constexpr auto cmdxc_offset   = u8{0x14};
constexpr auto cmdyc_offset   = u8{0x16};
constexpr auto cmdxd_offset   = u8{0x18};
constexpr auto cmdyd_offset   = u8{0x1a};
constexpr auto cmdgrda_offset = u8{0x1c};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  Vdp1Part
///
/// \brief  Defines a VDP1 part. This class cannot be inherited.
///
/// \author Runik
/// \date   13/05/2021
////////////////////////////////////////////////////////////////////////////////////////////////////

class Vdp1Part final : public BaseRenderingPart {
  public:
    ///@{
    /// Constructors / Destructors
    Vdp1Part(EmulatorModules& modules,
             const DrawType   type,
             const u32        table_address,
             const CmdCtrl&   cmdctrl,
             const CmdLink&   cmdlink);
    Vdp1Part(const Vdp1Part&) = default;
    Vdp1Part(Vdp1Part&&)      = default;
    auto operator=(const Vdp1Part&) & -> Vdp1Part& = default;
    auto operator=(Vdp1Part&&) & -> Vdp1Part& = default;
    ~Vdp1Part()                               = default;
    ///@}

    void renderPart(){};

    static void SetLocalCoordinates(const s16 x, const s16 y);

    ///@{
    /// \name  Calculated coordinates.
    auto calculatedXA() -> const s16;
    auto calculatedYA() -> const s16;
    auto calculatedXB() -> const s16;
    auto calculatedYB() -> const s16;
    auto calculatedXC() -> const s16;
    auto calculatedYC() -> const s16;
    auto calculatedXD() -> const s16;
    auto calculatedYD() -> const s16;
    ///@}

    ///@{
    /// \name  Part registers
    CmdCtrl             cmdctrl_; ///< Control words.
    CmdLink             cmdlink_; ///< Link specification.
    CmdPmod             cmdpmod_; ///< Draw mode words.
    CmdColr             cmdcolr_; ///< Color control word.
    CmdSrca             cmdsrca_; ///< Character address.
    CmdSize             cmdsize_; ///< Character size.
    CmdVertexCoordinate cmdxa_;   ///< Vertex coordinate data.
    CmdVertexCoordinate cmdya_;   ///< Vertex coordinate data.
    CmdVertexCoordinate cmdxb_;   ///< Vertex coordinate data.
    CmdVertexCoordinate cmdyb_;   ///< Vertex coordinate data.
    CmdVertexCoordinate cmdxc_;   ///< Vertex coordinate data.
    CmdVertexCoordinate cmdyc_;   ///< Vertex coordinate data.
    CmdVertexCoordinate cmdxd_;   ///< Vertex coordinate data.
    CmdVertexCoordinate cmdyd_;   ///< Vertex coordinate data.
    CmdGrda             cmdgrda_; ///< Gouraud shading table.
                                  ///@}

    std::vector<Vertex> vertexes_; ///< Contains the geometry vertexes of the part.

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
    /// \param  modules The modules.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void generatePartData(const EmulatorModules& modules);

    EmulatorModules modules_;
    DrawType        type_{DrawType::undefined};

    ///@{
    static s16 local_coordinate_x_; ///< Horizontal local coordinate.
    static s16 local_coordinate_y_; ///< Vertical local coordinate.
    ///@}
};

void normalSpriteDraw(const EmulatorModules& modules, Vdp1Part& part);
void scaledSpriteDraw(const EmulatorModules& modules, Vdp1Part& part);
void distortedSpriteDraw(const EmulatorModules& modules, Vdp1Part& part);
void polygonDraw(const EmulatorModules& modules, Vdp1Part& part);
void polylineDraw(const EmulatorModules& modules, Vdp1Part& part);
void lineDraw(const EmulatorModules& modules, Vdp1Part& part);

void loadTextureData(const EmulatorModules& modules, Vdp1Part& part);

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
    constexpr auto color_bank_mask = u16{0xFFF0};
    const auto     color_address
        = u32{cram_start_address + color_ram_offset + (part.cmdcolr_.toU16() & color_bank_mask + dot) * sizeof(T)};

    auto color = modules.vdp2()->readColor<T>(color_address);

    // Checking transparency.
    if (part.cmdpmod_.get(CmdPmod::transparent_pixel_disable) == TransparentPixelDisable::transparent_pixel_enabled) {
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
    const auto color_address = u32{vdp1_vram_start_address + (part.cmdcolr_.toU16() * address_multiplier + dot) * sizeof(T)};

    auto color = modules.vdp2()->readColor<T>(color_address);

    // Checking transparency.
    if (part.cmdpmod_.get(CmdPmod::transparent_pixel_disable) == TransparentPixelDisable::transparent_pixel_enabled) {
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
    constexpr auto color_bank_mask = u16{0xFFC0};
    const auto     color_address
        = u32{cram_start_address + color_ram_offset + (part.cmdcolr_.toU16() & color_bank_mask + dot) * sizeof(T)};

    auto color = modules.vdp2()->readColor<T>(color_address);

    // Checking transparency.
    if (part.cmdpmod_.get(CmdPmod::transparent_pixel_disable) == TransparentPixelDisable::transparent_pixel_enabled) {
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
    constexpr auto color_bank_mask = u16{0xFF80};
    const auto     color_address
        = u32{cram_start_address + color_ram_offset + (part.cmdcolr_.toU16() & color_bank_mask + dot) * sizeof(T)};

    auto color = modules.vdp2()->readColor<T>(color_address);

    // Checking transparency.
    if (part.cmdpmod_.get(CmdPmod::transparent_pixel_disable) == TransparentPixelDisable::transparent_pixel_enabled) {
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
        = u32{cram_start_address + color_ram_offset + (part.cmdcolr_.toU16() & color_bank_mask + dot) * sizeof(T)};

    auto color = modules.vdp2()->readColor<T>(color_address);

    // Checking transparency.
    if (part.cmdpmod_.get(CmdPmod::transparent_pixel_disable) == TransparentPixelDisable::transparent_pixel_enabled) {
        if (!dot) color.a = 0;
    }

    texture_data.insert(texture_data.end(), {color.r, color.g, color.b, color.a});
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn template<typename T> void readDotRgb(const EmulatorModules& modules, std::vector<u8>& texture_data, const u16
/// color_ram_offset, Vdp1Part& part, const u8 dot)
///
/// \brief  Reads one dot in RGB 32K colors format.
///
/// \tparam T   Generic type parameter.
/// \param          modules             Emulator modules.
/// \param [in,out] texture_data        Raw texture data.
/// \param          color_ram_offset    The color ram offset.
/// \param [in,out] part                The Vdp1Part being processed.
/// \param          dot                 The current dot.
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
void readDotRgb(const EmulatorModules& modules, std::vector<u8>& texture_data, Vdp1Part& part, const u16 dot) {
    auto color = Color(dot);

    // Checking transparency.
    if (part.cmdpmod_.get(CmdPmod::transparent_pixel_disable) == TransparentPixelDisable::transparent_pixel_enabled) {
        if (!dot) color.a = 0;
    }

    texture_data.insert(texture_data.end(), {color.r, color.g, color.b, color.a});
};

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
    auto           current_address = vdp1_ram_start_address + start_address;
    const auto     texture_size    = texture_data.capacity() / 4;
    if ((texture_size % 4) == 0) {
        // As we're reading 32 bits of data at a time (8 dots of 4 bits) we must ensure the data read is on 32 bits boundary.
        Log::warning(Logger::vdp1, "Texture size isn't a multiple of 4 !");
        return;
    }
    if (part.cmdpmod_.get(CmdPmod::color_calculation) != ColorCalculation::mode_0) {
        Log::unimplemented("Vdp1 - Color calculation {}", toUnderlying(part.cmdpmod_.get(CmdPmod::color_calculation)));
    }
    for (u32 i = current_address; i < (current_address + texture_size / one_read_offset); i += one_read_offset) {
        auto row = Dots4Bits(modules.memory()->read<u32>(current_address));
        readDotColorBank16<T>(modules, texture_data, color_ram_address_offset, part, row.get(Dots4Bits::dot_0));
        readDotColorBank16<T>(modules, texture_data, color_ram_address_offset, part, row.get(Dots4Bits::dot_1));
        readDotColorBank16<T>(modules, texture_data, color_ram_address_offset, part, row.get(Dots4Bits::dot_2));
        readDotColorBank16<T>(modules, texture_data, color_ram_address_offset, part, row.get(Dots4Bits::dot_3));
        readDotColorBank16<T>(modules, texture_data, color_ram_address_offset, part, row.get(Dots4Bits::dot_4));
        readDotColorBank16<T>(modules, texture_data, color_ram_address_offset, part, row.get(Dots4Bits::dot_5));
        readDotColorBank16<T>(modules, texture_data, color_ram_address_offset, part, row.get(Dots4Bits::dot_6));
        readDotColorBank16<T>(modules, texture_data, color_ram_address_offset, part, row.get(Dots4Bits::dot_7));
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
    auto           current_address = vdp1_ram_start_address + start_address;
    const auto     texture_size    = texture_data.capacity() / 4;
    if ((texture_size % 4) == 0) {
        // As we're reading 32 bits of data at a time (8 dots of 4 bits) we must ensure the data read is on 32 bits boundary.
        Log::warning(Logger::vdp1, "Texture size isn't a multiple of 4 !");
        return;
    }
    if (part.cmdpmod_.get(CmdPmod::color_calculation) != ColorCalculation::mode_0) {
        Log::unimplemented("Vdp1 - Color calculation {}", toUnderlying(part.cmdpmod_.get(CmdPmod::color_calculation)));
    }
    for (u32 i = current_address; i < (current_address + texture_size / one_read_offset); i += one_read_offset) {
        auto row = Dots4Bits(modules.memory()->read<u32>(current_address));
        readDotLookUpTable16<T>(modules, texture_data, part, row.get(Dots4Bits::dot_0));
        readDotLookUpTable16<T>(modules, texture_data, part, row.get(Dots4Bits::dot_1));
        readDotLookUpTable16<T>(modules, texture_data, part, row.get(Dots4Bits::dot_2));
        readDotLookUpTable16<T>(modules, texture_data, part, row.get(Dots4Bits::dot_3));
        readDotLookUpTable16<T>(modules, texture_data, part, row.get(Dots4Bits::dot_4));
        readDotLookUpTable16<T>(modules, texture_data, part, row.get(Dots4Bits::dot_5));
        readDotLookUpTable16<T>(modules, texture_data, part, row.get(Dots4Bits::dot_6));
        readDotLookUpTable16<T>(modules, texture_data, part, row.get(Dots4Bits::dot_7));
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
    auto           current_address = vdp1_ram_start_address + start_address;
    const auto     texture_size    = texture_data.capacity() / 4;
    if ((texture_size % 4) == 0) {
        // As we're reading 32 bits of data at a time (4 dots of 6 bits) we must ensure the data read is on 32 bits boundary.
        Log::warning(Logger::vdp1, "Texture size isn't a multiple of 4 !");
        return;
    }
    if (part.cmdpmod_.get(CmdPmod::color_calculation) != ColorCalculation::mode_0) {
        Log::unimplemented("Vdp1 - Color calculation {}", toUnderlying(part.cmdpmod_.get(CmdPmod::color_calculation)));
    }
    for (u32 i = current_address; i < (current_address + texture_size / one_read_offset); i += one_read_offset) {
        auto row = Dots6Bits(modules.memory()->read<u32>(current_address));
        readDotColorBank64<T>(modules, texture_data, color_ram_address_offset, part, row.get(Dots6Bits::dot_0));
        readDotColorBank64<T>(modules, texture_data, color_ram_address_offset, part, row.get(Dots6Bits::dot_1));
        readDotColorBank64<T>(modules, texture_data, color_ram_address_offset, part, row.get(Dots6Bits::dot_2));
        readDotColorBank64<T>(modules, texture_data, color_ram_address_offset, part, row.get(Dots6Bits::dot_3));
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
    auto           current_address = vdp1_ram_start_address + start_address;
    const auto     texture_size    = texture_data.capacity() / 4;
    if ((texture_size % 4) == 0) {
        // As we're reading 32 bits of data at a time (4 dots of 7 bits) we must ensure the data read is on 32 bits boundary.
        Log::warning(Logger::vdp1, "Texture size isn't a multiple of 4 !");
        return;
    }
    if (part.cmdpmod_.get(CmdPmod::color_calculation) != ColorCalculation::mode_0) {
        Log::unimplemented("Vdp1 - Color calculation {}", toUnderlying(part.cmdpmod_.get(CmdPmod::color_calculation)));
    }
    for (u32 i = current_address; i < (current_address + texture_size / one_read_offset); i += one_read_offset) {
        auto row = Dots7Bits(modules.memory()->read<u32>(current_address));
        readDotColorBank128<T>(modules, texture_data, color_ram_address_offset, part, row.get(Dots7Bits::dot_0));
        readDotColorBank128<T>(modules, texture_data, color_ram_address_offset, part, row.get(Dots7Bits::dot_1));
        readDotColorBank128<T>(modules, texture_data, color_ram_address_offset, part, row.get(Dots7Bits::dot_2));
        readDotColorBank128<T>(modules, texture_data, color_ram_address_offset, part, row.get(Dots7Bits::dot_3));
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
    auto           current_address = vdp1_ram_start_address + start_address;
    const auto     texture_size    = texture_data.capacity() / 4;
    if ((texture_size % 4) == 0) {
        // As we're reading 32 bits of data at a time (4 dots of 8 bits) we must ensure the data read is on 32 bits boundary.
        Log::warning(Logger::vdp1, "Texture size isn't a multiple of 4 !");
        return;
    }
    if (part.cmdpmod_.get(CmdPmod::color_calculation) != ColorCalculation::mode_0) {
        Log::unimplemented("Vdp1 - Color calculation {}", toUnderlying(part.cmdpmod_.get(CmdPmod::color_calculation)));
    }
    for (u32 i = current_address; i < (current_address + texture_size / one_read_offset); i += one_read_offset) {
        auto row = Dots8Bits(modules.memory()->read<u32>(current_address));
        readDotColorBank256<T>(modules, texture_data, color_ram_address_offset, part, row.get(Dots8Bits::dot_0));
        readDotColorBank256<T>(modules, texture_data, color_ram_address_offset, part, row.get(Dots8Bits::dot_1));
        readDotColorBank256<T>(modules, texture_data, color_ram_address_offset, part, row.get(Dots8Bits::dot_2));
        readDotColorBank256<T>(modules, texture_data, color_ram_address_offset, part, row.get(Dots8Bits::dot_3));
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
    constexpr auto one_read_offset = u8{4}; // in bytes
    auto           current_address = vdp1_ram_start_address + start_address;
    const auto     texture_size    = texture_data.capacity() / 4;
    if ((texture_size % 4) == 0) {
        // As we're reading 32 bits of data at a time (4 dots of 8 bits) we must ensure the data read is on 32 bits boundary.
        Log::warning(Logger::vdp1, "Texture size isn't a multiple of 4 !");
        return;
    }
    if (part.cmdpmod_.get(CmdPmod::color_calculation) != ColorCalculation::mode_0) {
        Log::unimplemented("Vdp1 - Color calculation {}", toUnderlying(part.cmdpmod_.get(CmdPmod::color_calculation)));
    }
    for (u32 i = current_address; i < (current_address + texture_size / one_read_offset); i += one_read_offset) {
        auto row = Dots16Bits(modules.memory()->read<u32>(current_address));
        readDotRgb<T>(modules, texture_data, part, row.get(Dots16Bits::dot_0));
        readDotRgb<T>(modules, texture_data, part, row.get(Dots16Bits::dot_1));
    }
}
} // namespace saturnin::video
