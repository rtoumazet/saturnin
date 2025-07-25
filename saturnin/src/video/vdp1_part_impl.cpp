//
// vdp1_part_impl.cpp
// Saturnin
//
// Copyright (c) 2025 Renaud Toumazet
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

// vdp1_part.h template implementations

#pragma once

#include <saturnin/src/pch.h>
#include <saturnin/src/video/vdp1_part.h>
#include <saturnin/src/memory.h>

namespace saturnin::video {

// readDotColorBank16 - template definition
template<typename T>
void readDotColorBank16(const EmulatorModules& modules,
                        std::vector<u8>&       texture_data,
                        const u16              color_ram_offset,
                        Vdp1Part&              part,
                        const u8               dot) {
    constexpr auto color_bank_mask = u16{0x0FF0};
    const auto     color_address
        = static_cast<u32>(cram_start_address + color_ram_offset + ((part.cmdcolr_.data() & color_bank_mask) | dot) * sizeof(T));

    auto color = Color(modules.memory()->read<T>(color_address));

    // Checking transparency.
    if ((part.cmdpmod_ >> CmdPmod::spd_enum) == CmdPmod::TransparentPixelDisable::transparent_pixel_enabled) {
        if (!dot) color.a = 0;
    }

    texture_data.insert(texture_data.end(), {color.r, color.g, color.b, color.a});
};

// readDotColorBank16 - explicit instanciations
template void readDotColorBank16<u16>(const EmulatorModules& modules,
                                      std::vector<u8>&       texture_data,
                                      const u16              color_ram_offset,
                                      Vdp1Part&              part,
                                      const u8               dot);

template void readDotColorBank16<u32>(const EmulatorModules& modules,
                                      std::vector<u8>&       texture_data,
                                      const u16              color_ram_offset,
                                      Vdp1Part&              part,
                                      const u8               dot);

// readDotLookUpTable16 - template definition
template<typename T>
void readDotLookUpTable16(const EmulatorModules& modules, std::vector<u8>& texture_data, Vdp1Part& part, const u8 dot) {
    const auto color_address
        = static_cast<u32>(vdp1_vram_start_address + part.cmdcolr_.data() * vdp1_address_multiplier + dot * sizeof(T));

    auto color = Color(modules.memory()->read<T>(color_address));

    // Checking transparency.
    if ((part.cmdpmod_ >> CmdPmod::spd_enum) == CmdPmod::TransparentPixelDisable::transparent_pixel_enabled) {
        if (!dot) color.a = 0;
    }

    texture_data.insert(texture_data.end(), {color.r, color.g, color.b, color.a});
};
// readDotLookUpTable16 - explicit instanciations
template void
    readDotLookUpTable16<u16>(const EmulatorModules& modules, std::vector<u8>& texture_data, Vdp1Part& part, const u8 dot);
template void
    readDotLookUpTable16<u32>(const EmulatorModules& modules, std::vector<u8>& texture_data, Vdp1Part& part, const u8 dot);

// readDotColorBank64 - template definition
template<typename T>
void readDotColorBank64(const EmulatorModules& modules,
                        std::vector<u8>&       texture_data,
                        const u16              color_ram_offset,
                        Vdp1Part&              part,
                        const u8               dot) {
    constexpr auto color_bank_mask = u16{0x0FC0};
    const auto     color_address
        = static_cast<u32>(cram_start_address + color_ram_offset + ((part.cmdcolr_.data() & color_bank_mask) | dot) * sizeof(T));

    auto color = Color(modules.memory()->read<T>(color_address));

    // Checking transparency.
    if ((part.cmdpmod_ >> CmdPmod::spd_enum) == CmdPmod::TransparentPixelDisable::transparent_pixel_enabled) {
        if (!dot) color.a = 0;
    }

    texture_data.insert(texture_data.end(), {color.r, color.g, color.b, color.a});
};
// readDotColorBank64 - explicit instanciations
template void readDotColorBank64<u16>(const EmulatorModules& modules,
                                      std::vector<u8>&       texture_data,
                                      const u16              color_ram_offset,
                                      Vdp1Part&              part,
                                      const u8               dot);
template void readDotColorBank64<u32>(const EmulatorModules& modules,
                                      std::vector<u8>&       texture_data,
                                      const u16              color_ram_offset,
                                      Vdp1Part&              part,
                                      const u8               dot);

// readDotColorBank128 - template definition
template<typename T>
void readDotColorBank128(const EmulatorModules& modules,
                         std::vector<u8>&       texture_data,
                         const u16              color_ram_offset,
                         Vdp1Part&              part,
                         const u8               dot) {
    constexpr auto color_bank_mask = u16{0x0F80};
    const auto     color_address
        = static_cast<u32>(cram_start_address + color_ram_offset + ((part.cmdcolr_.data() & color_bank_mask) | dot) * sizeof(T));

    auto color = Color(modules.memory()->read<T>(color_address));

    // Checking transparency.
    if ((part.cmdpmod_ >> CmdPmod::spd_enum) == CmdPmod::TransparentPixelDisable::transparent_pixel_enabled) {
        if (!dot) color.a = 0;
    }

    texture_data.insert(texture_data.end(), {color.r, color.g, color.b, color.a});
};
// readDotColorBank128 - explicit instanciations
template void readDotColorBank128<u16>(const EmulatorModules& modules,
                                       std::vector<u8>&       texture_data,
                                       const u16              color_ram_offset,
                                       Vdp1Part&              part,
                                       const u8               dot);
template void readDotColorBank128<u32>(const EmulatorModules& modules,
                                       std::vector<u8>&       texture_data,
                                       const u16              color_ram_offset,
                                       Vdp1Part&              part,
                                       const u8               dot);

// readDotColorBank256 - template definition
template<typename T>
void readDotColorBank256(const EmulatorModules& modules,
                         std::vector<u8>&       texture_data,
                         const u16              color_ram_offset,
                         Vdp1Part&              part,
                         const u8               dot) {
    constexpr auto color_bank_mask = u16{0xFF00};
    const auto     color_address
        = static_cast<u32>(cram_start_address + color_ram_offset + ((part.cmdcolr_.data() & color_bank_mask) | dot) * sizeof(T));

    auto color = Color(modules.memory()->read<T>(color_address));

    // Checking transparency.
    if ((part.cmdpmod_ >> CmdPmod::spd_enum) == CmdPmod::TransparentPixelDisable::transparent_pixel_enabled) {
        if (!dot) color.a = 0;
    }

    texture_data.insert(texture_data.end(), {color.r, color.g, color.b, color.a});
};
// readDotColorBank256 - explicit instanciations
template void readDotColorBank256<u16>(const EmulatorModules& modules,
                                       std::vector<u8>&       texture_data,
                                       const u16              color_ram_offset,
                                       Vdp1Part&              part,
                                       const u8               dot);
template void readDotColorBank256<u32>(const EmulatorModules& modules,
                                       std::vector<u8>&       texture_data,
                                       const u16              color_ram_offset,
                                       Vdp1Part&              part,
                                       const u8               dot);

// readColorBankMode16Colors - template definition
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
// readColorBankMode16Colors - explicit instanciations
template void readColorBankMode16Colors<u16>(const EmulatorModules& modules,
                                             std::vector<u8>&       texture_data,
                                             const u32              start_address,
                                             const u16              color_ram_address_offset,
                                             Vdp1Part&              part);
template void readColorBankMode16Colors<u32>(const EmulatorModules& modules,
                                             std::vector<u8>&       texture_data,
                                             const u32              start_address,
                                             const u16              color_ram_address_offset,
                                             Vdp1Part&              part);

// readLookUpTable16Colors - template definition
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
// readLookUpTable16Colors - explicit instanciations
template void readLookUpTable16Colors<u16>(const EmulatorModules& modules,
                                           std::vector<u8>&       texture_data,
                                           const u32              start_address,
                                           Vdp1Part&              part);
template void readLookUpTable16Colors<u32>(const EmulatorModules& modules,
                                           std::vector<u8>&       texture_data,
                                           const u32              start_address,
                                           Vdp1Part&              part);

// readLookUpTable16Colors - template definition
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
// readColorBankMode64Colors - explicit instanciations
template void readColorBankMode64Colors<u16>(const EmulatorModules& modules,
                                             std::vector<u8>&       texture_data,
                                             const u32              start_address,
                                             const u16              color_ram_address_offset,
                                             Vdp1Part&              part);
template void readColorBankMode64Colors<u32>(const EmulatorModules& modules,
                                             std::vector<u8>&       texture_data,
                                             const u32              start_address,
                                             const u16              color_ram_address_offset,
                                             Vdp1Part&              part);

// readColorBankMode128Colors - template definition
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
// readColorBankMode128Colors - explicit instanciations
template void readColorBankMode128Colors<u16>(const EmulatorModules& modules,
                                              std::vector<u8>&       texture_data,
                                              const u32              start_address,
                                              const u16              color_ram_address_offset,
                                              Vdp1Part&              part);
template void readColorBankMode128Colors<u32>(const EmulatorModules& modules,
                                              std::vector<u8>&       texture_data,
                                              const u32              start_address,
                                              const u16              color_ram_address_offset,
                                              Vdp1Part&              part);

// readColorBankMode256Colors - template definition
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
// readColorBankMode256Colors - explicit instanciations
template void readColorBankMode256Colors<u16>(const EmulatorModules& modules,
                                              std::vector<u8>&       texture_data,
                                              const u32              start_address,
                                              const u16              color_ram_address_offset,
                                              Vdp1Part&              part);
template void readColorBankMode256Colors<u32>(const EmulatorModules& modules,
                                              std::vector<u8>&       texture_data,
                                              const u32              start_address,
                                              const u16              color_ram_address_offset,
                                              Vdp1Part&              part);

// readRgb32KColors - template definition
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
// readRgb32KColors - explicit instanciations
template void
    readRgb32KColors<u16>(const EmulatorModules& modules, std::vector<u8>& texture_data, const u32 start_address, Vdp1Part& part);
template void
    readRgb32KColors<u32>(const EmulatorModules& modules, std::vector<u8>& texture_data, const u32 start_address, Vdp1Part& part);
} // namespace saturnin::video
