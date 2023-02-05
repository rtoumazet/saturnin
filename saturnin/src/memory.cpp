//
// memory.cpp
// Saturnin
//
// Copyright (c) 2018 Renaud Toumazet
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
#include <saturnin/src/memory.h>
#include <algorithm>
#include <filesystem> // filesystem
#include <fstream>    // ifstream
#include <sstream>    // stringstream
#include <GLFW/glfw3.h>
#include <libzippp/libzippp.h>
#include <saturnin/src/config.h>
#include <saturnin/src/emulator_context.h>
#include <saturnin/src/locale.h> // NOLINT(modernize-deprecated-headers)
#include <saturnin/src/sh2.h>
#include <saturnin/src/utilities.h> // format
#include <saturnin/src/cdrom/cdrom.h>
#include <saturnin/src/sound/scsp.h>
#include <saturnin/src/video/vdp1.h>
#include <saturnin/src/video/vdp2.h>

namespace lzpp = libzippp;
namespace fs   = std::filesystem;
namespace sh2  = saturnin::sh2;
namespace uti  = saturnin::utilities;

namespace saturnin::core {

using core::Log;
using core::Logger;
using std::copy;

constexpr auto region_cart_address_not_interleaved  = u8{0x40};
constexpr auto region_cart_address_odd_interleaved  = u8{0x81};
constexpr auto region_cart_address_even_interleaved = u8{0x80};

constexpr auto dummy_address        = AddressRange{0x00000000, 0xFFFFFFFF};
constexpr auto rom_address          = AddressRange{0x00000000, 0x000FFFFF};
constexpr auto smpc_address         = AddressRange{0x00100000, 0x0017FFFF};
constexpr auto backup_ram_address   = AddressRange{0x00180000, 0x001FFFFF};
constexpr auto workram_low_address  = AddressRange{0x00200000, 0x002FFFFF};
constexpr auto stv_io_address       = AddressRange{0x00400000, 0x004FFFFF};
constexpr auto cart_address         = AddressRange{0x02000000, 0x04FFFFFF};
constexpr auto cd_block_address     = AddressRange{0x05800000, 0x058FFFFF};
constexpr auto scsp_address         = AddressRange{0x05A00000, 0x05BFFFFF};
constexpr auto vdp1_ram_address     = AddressRange{0x05C00000, 0x05C7FFFF};
constexpr auto vdp1_fb_address      = AddressRange{0x05C80000, 0x05CBFFFF};
constexpr auto vdp1_regs_address    = AddressRange{0x05D00000, 0x05D7FFFF};
constexpr auto vdp2_vram_address    = AddressRange{0x05E00000, 0x05EFFFFF};
constexpr auto vdp2_cram_address    = AddressRange{0x05F00000, 0x05F7FFFF};
constexpr auto vdp2_regs_address    = AddressRange{0x05F80000, 0x05FBFFFF};
constexpr auto scu_address          = AddressRange{0x05FE0000, 0x05FEFFFF};
constexpr auto workram_high_address = AddressRange{0x06000000, 0x07FFFFFF};
constexpr auto master_frt_address   = AddressRange{0x01800000, 0x01FFFFFF};
constexpr auto slave_frt_address    = AddressRange{0x01000000, 0x017FFFFF};
constexpr auto sh2_regs_address     = AddressRange{0xFFFFFE00, 0xFFFFFFFF};
constexpr auto cache_address        = AddressRange{0x60000000, 0x6FFFFFFF};
constexpr auto cache_data_1_address = AddressRange{0x80000000, 0x8FFFFFFF};
constexpr auto cache_data_2_address = AddressRange{0xC0000000, 0xCFFFFFFF};

auto Memory::loadRom(const std::string& zip_name,
                     const std::string& file_name,
                     u8*                destination,
                     const u32          size,
                     const RomLoad      rom_load,
                     const u8           times_mirrored,
                     const RomType      rom_type) -> bool {
    const auto zip      = std::string{zip_name + ".zip"};
    auto       rom_path = fs::path{modules_.config()->readValue(AccessKeys::cfg_paths_roms_stv).c_str()};
    rom_path /= zip;
    // rom_path += "\\" + zip_name + ".zip";

    lzpp::ZipArchive zf(rom_path.string());
    if (zf.open(lzpp::ZipArchive::ReadOnly)) {
        if (zf.hasEntry(file_name, false, false)) {
            lzpp::ZipEntry        entry = zf.getEntry(file_name, false, false);
            std::unique_ptr<u8[]> data(static_cast<u8*>(entry.readAsBinary()));

            switch (rom_type) {
                using enum RomType;
                case bios: {
                    const auto counter = u32{size / 4};
                    // Needs byteswapping
                    for (u32 i = 0; i < counter; ++i) {
                        destination[i * 4 + 1] = data[i * 4 + 0];
                        destination[i * 4 + 0] = data[i * 4 + 1];
                        destination[i * 4 + 3] = data[i * 4 + 2];
                        destination[i * 4 + 2] = data[i * 4 + 3];
                    }
                    break;
                }
                case program:
                case graphic: {
                    const auto stv_bios_region_address = u32{0x808};
                    auto       region_cart_address     = u32{};
                    switch (rom_load) {
                        using enum RomLoad;
                        case not_interleaved: {
                            const auto& src_begin = data.get();
                            std::move(src_begin, std::next(src_begin, size), destination);

                            region_cart_address = region_cart_address_not_interleaved;
                            break;
                        }
                        case odd_interleaved: { // Data is loaded on odd bytes only
                            for (u32 i = 0; i < size; ++i) {
                                destination[(i * 2 + 1)] = data[i];
                            }

                            region_cart_address = region_cart_address_odd_interleaved;
                            break;
                        }
                        case even_interleaved: { // Data is loaded on even bytes only
                            for (u32 i = 0; i < size; ++i) {
                                destination[i * 2] = data[i];
                            }

                            region_cart_address = region_cart_address_even_interleaved;
                            break;
                        }
                    }

                    // bios region is forced for program roms
                    if (rom_type == RomType::program) { this->cart_[region_cart_address] = this->rom_[stv_bios_region_address]; }

                    mirrorData(destination, size, times_mirrored, rom_load);
                    break;
                }
                default: {
                    Log::warning(Logger::memory, uti::format(tr("Unknown rom type while loading {0}.zip"), zip_name));
                    return false;
                    break;
                }
            }
        } else {
            zf.close();
            const auto str = uti::format(tr("File '{0}' not found in zip file !"), file_name);
            Log::warning(Logger::memory, str);
            return false;
        }
        zf.close();
    } else {
        const auto str = uti::format(tr("Zip file '{0}' not found !"), rom_path.string());
        Log::warning(Logger::memory, str);
        return false;
    }
    return true;
}

auto Memory::loadBinaryFile(const BinaryFileConfiguration& file) -> bool {
    std::ifstream input_file(file.full_path, std::ios::binary);
    if (input_file) {
        Log::info(Logger::memory, tr("Loading binary file"));
        auto buffer = std::stringstream{};
        buffer << input_file.rdbuf();
        input_file.close();

        const auto str = buffer.str();

        std::move(str.begin(), str.end(), this->workram_high_.data() + (file.load_address & workram_high_memory_mask));

        modules_.masterSh2()->setBinaryFileStartAddress(file.start_address);

        Log::info(Logger::main, tr("Binary file loaded"));
        return true;
    } else {
        Log::warning(Logger::memory, tr("Binary file not found !"));
        return false;
    }
}

void Memory::loadBios(const HardwareMode mode) {
    Log::info(Logger::memory, tr("Loading bios"));
    auto bios_path = std::string{};
    switch (mode) {
        using enum HardwareMode;
        case saturn: bios_path = modules_.config()->readValue(AccessKeys::cfg_paths_bios_saturn).c_str(); break;
        case stv: bios_path = modules_.config()->readValue(AccessKeys::cfg_paths_bios_stv).c_str(); break;
        default: {
            Log::error(Logger::memory, tr("Unknown hardware mode"));
            throw std::runtime_error("Config error !");
            break;
        }
    }

    std::ifstream input_file(bios_path, std::ios::binary);
    if (input_file) {
        auto buffer = std::stringstream{};
        buffer << input_file.rdbuf();
        input_file.close();

        const auto str = buffer.str();

        switch (mode) {
            using enum HardwareMode;
            case saturn: {
                std::move(str.begin(), str.end(), this->rom_.data());
                break;
            }
            case stv: {
                // Needs byteswapping
                for (size_t i = 0; i < str.size(); i += 4) {
                    this->rom_[i + 1] = str[i + 0];
                    this->rom_[i + 0] = str[i + 1];
                    this->rom_[i + 3] = str[i + 2];
                    this->rom_[i + 2] = str[i + 3];
                }
                installStvBiosBypass();
                break;
            }
        }
    } else {
        Log::warning(Logger::memory, tr("Bios file not found !"));
    }
}

auto Memory::loadStvGame(const StvGameConfiguration& game) -> bool {
    for (const auto& file : game.files) {
        if (!this->loadRom(game.zip_name,
                           file.rom_name,
                           &this->cart_[file.load_address],
                           file.load_size,
                           file.rom_load,
                           file.times_mirrored,
                           file.rom_type)) {
            return false;
        }
    }

    swapCartArea();

    return true;
}

void Memory::swapCartArea() {
    const auto program_rom_size = u32{0x400000};

    // ST-V data begins with 'SEGA' string.
    // If the first byte of the string is 'E', it means the program data has to be swapped
    if (this->cart_[0] == 'E') {
        for (u32 i = 0; i < program_rom_size; i += 2) {
            std::swap(this->cart_[i], this->cart_[i + 1]);
        }
    }

    for (u32 i = program_rom_size; i < this->cart_.size(); i += 2) {
        std::swap(this->cart_[i], this->cart_[i + 1]);
    }
}

void Memory::initializeHandlers() {
    // Dummy access
    initializeHandlers<readDummy, u8, u16, u32>(dummy_address);
    initializeHandlers<writeDummy, u8, u16, u32>(dummy_address);

    // ROM access
    initializeHandlers<readRom, u8, u16, u32>(rom_address);
    initializeHandlers<readRom, u8, u16, u32>(getDirectAddress(rom_address));

    // SMPC access
    initializeHandlers<readSmpc, u8, u16, u32>(smpc_address);
    initializeHandlers<readSmpc, u8, u16, u32>(getDirectAddress(smpc_address));

    initializeHandlers<writeSmpc, u8, u16, u32>(smpc_address);
    initializeHandlers<writeSmpc, u8, u16, u32>(getDirectAddress(smpc_address));

    // Backup RAM access
    initializeHandlers<readBackupRam, u8, u16, u32>(backup_ram_address);
    initializeHandlers<readBackupRam, u8, u16, u32>(getDirectAddress(backup_ram_address));

    initializeHandlers<writeBackupRam, u8, u16, u32>(backup_ram_address);
    initializeHandlers<writeBackupRam, u8, u16, u32>(getDirectAddress(backup_ram_address));

    // Low workram access
    initializeHandlers<readWorkramLow, u8, u16, u32>(workram_low_address);
    initializeHandlers<readWorkramLow, u8, u16, u32>(getDirectAddress(workram_low_address));

    initializeHandlers<writeWorkramLow, u8, u16, u32>(workram_low_address);
    initializeHandlers<writeWorkramLow, u8, u16, u32>(getDirectAddress(workram_low_address));

    // STV I/O access
    initializeHandlers<readStvIo, u8, u16, u32>(stv_io_address);
    initializeHandlers<readStvIo, u8, u16, u32>(getDirectAddress(stv_io_address));

    initializeHandlers<writeStvIo, u8, u16, u32>(stv_io_address);
    initializeHandlers<writeStvIo, u8, u16, u32>(getDirectAddress(stv_io_address));

    // Cart access
    initializeHandlers<readCart, u8, u16, u32>(cart_address);
    initializeHandlers<readCart, u8, u16, u32>(getDirectAddress(cart_address));

    initializeHandlers<writeCart, u8, u16, u32>(cart_address);
    initializeHandlers<writeCart, u8, u16, u32>(getDirectAddress(cart_address));

    // CdBlock access
    initializeHandlers<readCdBlock, u8, u16, u32>(cd_block_address);
    initializeHandlers<readCdBlock, u8, u16, u32>(getDirectAddress(cd_block_address));

    initializeHandlers<writeCdBlock, u8, u16, u32>(cd_block_address);
    initializeHandlers<writeCdBlock, u8, u16, u32>(getDirectAddress(cd_block_address));

    // SCSP access
    initializeHandlers<readScsp, u8, u16, u32>(scsp_address);
    initializeHandlers<readScsp, u8, u16, u32>(getDirectAddress(scsp_address));

    initializeHandlers<writeScsp, u8, u16, u32>(scsp_address);
    initializeHandlers<writeScsp, u8, u16, u32>(getDirectAddress(scsp_address));

    // VDP1 RAM access
    initializeHandlers<readVdp1Ram, u8, u16, u32>(vdp1_ram_address);
    initializeHandlers<readVdp1Ram, u8, u16, u32>(getDirectAddress(vdp1_ram_address));

    initializeHandlers<writeVdp1Ram, u8, u16, u32>(vdp1_ram_address);
    initializeHandlers<writeVdp1Ram, u8, u16, u32>(getDirectAddress(vdp1_ram_address));

    // VDP1 framebuffer access
    initializeHandlers<readVdp1Framebuffer, u8, u16, u32>(vdp1_fb_address);
    initializeHandlers<readVdp1Framebuffer, u8, u16, u32>(getDirectAddress(vdp1_fb_address));

    initializeHandlers<writeVdp1Framebuffer, u8, u16, u32>(vdp1_fb_address);
    initializeHandlers<writeVdp1Framebuffer, u8, u16, u32>(getDirectAddress(vdp1_fb_address));

    // VDP1 Registers access
    initializeHandlers<readVdp1Registers, u8, u16, u32>(vdp1_regs_address);
    initializeHandlers<readVdp1Registers, u8, u16, u32>(getDirectAddress(vdp1_regs_address));

    initializeHandlers<writeVdp1Registers, u8, u16, u32>(vdp1_regs_address);
    initializeHandlers<writeVdp1Registers, u8, u16, u32>(getDirectAddress(vdp1_regs_address));

    // VDP2 VRAM access
    initializeHandlers<readVdp2Vram, u8, u16, u32>(vdp2_vram_address);
    initializeHandlers<readVdp2Vram, u8, u16, u32>(getDirectAddress(vdp2_vram_address));

    initializeHandlers<writeVdp2Vram, u8, u16, u32>(vdp2_vram_address);
    initializeHandlers<writeVdp2Vram, u8, u16, u32>(getDirectAddress(vdp2_vram_address));

    // VDP2 CRAM access
    initializeHandlers<readVdp2Cram, u8, u16, u32>(vdp2_cram_address);
    initializeHandlers<readVdp2Cram, u8, u16, u32>(getDirectAddress(vdp2_cram_address));

    initializeHandlers<writeVdp2Cram, u8, u16, u32>(vdp2_cram_address);
    initializeHandlers<writeVdp2Cram, u8, u16, u32>(getDirectAddress(vdp2_cram_address));

    // VDP2 Registers access
    initializeHandlers<readVdp2Registers, u8, u16, u32>(vdp2_regs_address);
    initializeHandlers<readVdp2Registers, u8, u16, u32>(getDirectAddress(vdp2_regs_address));

    initializeHandlers<writeVdp2Registers, u8, u16, u32>(vdp2_regs_address);
    initializeHandlers<writeVdp2Registers, u8, u16, u32>(getDirectAddress(vdp2_regs_address));

    // SCU access
    initializeHandlers<readScu, u8, u16, u32>(scu_address);
    initializeHandlers<readScu, u8, u16, u32>(getDirectAddress(scu_address));

    initializeHandlers<writeScu, u8, u16, u32>(scu_address);
    initializeHandlers<writeScu, u8, u16, u32>(getDirectAddress(scu_address));

    // Workram high access
    initializeHandlers<readWorkramHigh, u8, u16, u32>(workram_high_address);
    initializeHandlers<readWorkramHigh, u8, u16, u32>(getDirectAddress(workram_high_address));

    initializeHandlers<writeWorkramHigh, u8, u16, u32>(workram_high_address);
    initializeHandlers<writeWorkramHigh, u8, u16, u32>(getDirectAddress(workram_high_address));

    // Master FRT access
    initializeHandlers<writeMasterSh2Frt, u8, u16, u32>(master_frt_address);
    initializeHandlers<writeMasterSh2Frt, u8, u16, u32>(getDirectAddress(master_frt_address));

    // Slave FRT access
    initializeHandlers<writeSlaveSh2Frt, u8, u16, u32>(slave_frt_address);
    initializeHandlers<writeSlaveSh2Frt, u8, u16, u32>(getDirectAddress(slave_frt_address));

    // SH2 register access
    initializeHandlers<readSh2Registers, u8, u16, u32>(sh2_regs_address);
    initializeHandlers<writeSh2Registers, u8, u16, u32>(sh2_regs_address);

    // Cache addresses access
    initializeHandlers<readCacheAddresses, u8, u16, u32>(cache_address);
    initializeHandlers<writeCacheAddresses, u8, u16, u32>(cache_address);

    // Cache data access
    initializeHandlers<readCacheData, u8, u16, u32>(cache_data_1_address);
    initializeHandlers<readCacheData, u8, u16, u32>(cache_data_2_address);

    initializeHandlers<writeCacheData, u8, u16, u32>(cache_data_1_address);
    initializeHandlers<writeCacheData, u8, u16, u32>(cache_data_2_address);
}

auto Memory::readStvProtection(const u32 addr, u32 data) const -> u32 {
    if (this->isStvProtectionEnabled()) {
        switch (data) {
            // Astra Superstars
            case 0x01230000: // NOLINT(readability-magic-numbers)
                stv_protection_offset += 4;
                data = rawRead<u32>(this->cart_, stv_protection_offset);
                break;
                // Final Fight Revenge
            case 0x10da0000: // NOLINT(readability-magic-numbers)
                stv_protection_offset += 4;
                data = rawRead<u32>(this->cart_, stv_protection_offset);
                break;
            case 0x10d70000: // NOLINT(readability-magic-numbers)
                stv_protection_offset += 4;
                data = rawRead<u32>(this->cart_, stv_protection_offset);
                break;
                // Steep Slope Sliders
            case 0x2c5b0000: // NOLINT(readability-magic-numbers)
            case 0x47F10000: // NOLINT(readability-magic-numbers)
            case 0xfcda0000: // NOLINT(readability-magic-numbers)
            case 0xb5e60000: // NOLINT(readability-magic-numbers)
            case 0x392c0000: // NOLINT(readability-magic-numbers)
            case 0x77c30000: // NOLINT(readability-magic-numbers)
            case 0x8a620000: // NOLINT(readability-magic-numbers)
                stv_protection_offset += 4;
                data = rawRead<u32>(this->cart_, stv_protection_offset);
                break;
                // Radiant Silvergun
            case 0x77770000: // NOLINT(readability-magic-numbers)
                data = 0;
                break;
                // Elan Doreé
            case 0xff7f0000:                           // NOLINT(readability-magic-numbers)
            case 0xf9ff0000:                           // NOLINT(readability-magic-numbers)
            case 0xffbf0000: data = 0x02002000; break; // NOLINT(readability-magic-numbers)
        }
        Log::debug(Logger::memory, "ST-V protection read index: {}, value: {}", stv_protection_offset, data);
    } else {
        stv_protection_offset = 0;
    }

    return data;
}

void Memory::writeStvProtection(const u32 addr, u32 data) {
    const auto relative_addr = calculateRelativeCartAddress(stv_protection_register_address);
    const auto index         = rawRead<u32>(this->cart_, relative_addr);
    switch (index) {
        // Astra Superstars
        case 0x01230000:                                    // NOLINT(readability-magic-numbers)
            this->stv_protection_offset_ = (0x0400000) - 4; // NOLINT(readability-magic-numbers)
            break;
            // Final Fight Revenge
        case 0x10d70000:                                   // NOLINT(readability-magic-numbers)
        case 0x10da0000:                                   // NOLINT(readability-magic-numbers)
            this->stv_protection_offset_ = (0x02B994) - 4; // NOLINT(readability-magic-numbers)
            break;
            // Streep Slope Sliders
        case 0x2c5b0000: this->stv_protection_offset_ = (0x145ffac) - 4; break;           // NOLINT(readability-magic-numbers)
        case 0x47F10000: this->stv_protection_offset_ = (0x145ffac + 0xbaf0) - 4; break;  // NOLINT(readability-magic-numbers)
        case 0xfcda0000: this->stv_protection_offset_ = (0x145ffac + 0x12fd0) - 4; break; // NOLINT(readability-magic-numbers)
        case 0xb5e60000: this->stv_protection_offset_ = (0x145ffac + 0x1a4c4) - 4; break; // NOLINT(readability-magic-numbers)
        case 0x392c0000: this->stv_protection_offset_ = (0x145ffac + 0x219b0) - 4; break; // NOLINT(readability-magic-numbers)
        case 0x77c30000: this->stv_protection_offset_ = (0x145ffac + 0x28ea0) - 4; break; // NOLINT(readability-magic-numbers)
        case 0x8a620000:                                                                  // NOLINT(readability-magic-numbers)
            this->stv_protection_offset_ = (0x145ffac + 0x30380) - 4;                     // NOLINT(readability-magic-numbers)
            break;
            // Radiant Silvergun
        case 0x77770000: break; // NOLINT(readability-magic-numbers)
    }
    Log::debug(Logger::memory, uti::format(core::tr("ST-V offset start: {}"), this->stv_protection_offset_));
}

auto Memory::isStvProtectionEnabled() const -> bool {
    const auto relative_addr = calculateRelativeCartAddress(stv_protection_enabled);
    return cart_[relative_addr] == 0x1;
}

void Memory::sendFrtInterruptToMaster() const { modules_.masterSh2()->sendInterruptCaptureSignal(); }

void Memory::sendFrtInterruptToSlave() const { modules_.slaveSh2()->sendInterruptCaptureSignal(); }

auto Memory::getMemoryMapAreaData(const MemoryMapArea area) -> std::tuple<u8*, size_t, u32> const {
    switch (area) {
        using enum MemoryMapArea;
        case rom: return std::make_tuple(rom_.data(), rom_.size(), rom_address.start);
        case smpc: return std::make_tuple(smpc_.data(), smpc_.size(), smpc_address.start);
        case backup_ram: return std::make_tuple(backup_ram_.data(), backup_ram_.size(), backup_ram_address.start);
        case workram_low: return std::make_tuple(workram_low_.data(), workram_low_.size(), workram_low_address.start);
        case stv_io: return std::make_tuple(stv_io_.data(), stv_io_.size(), stv_io_address.start);
        case cart: return std::make_tuple(cart_.data(), cart_.size(), cart_address.start);
        // case cd_block: return std::make_tuple(.data(), .size(), cd_block_address.start);
        case scsp: return std::make_tuple(sound_ram_.data(), sound_ram_.size(), scsp_address.start);
        case vdp1_ram: return std::make_tuple(vdp1_vram_.data(), vdp1_vram_.size(), vdp1_ram_address.start);
        case vdp1_framebuffer: return std::make_tuple(vdp1_framebuffer_.data(), vdp1_framebuffer_.size(), vdp1_fb_address.start);
        case vdp1_registers: return std::make_tuple(vdp1_registers_.data(), vdp1_registers_.size(), vdp1_regs_address.start);
        case vdp2_video_ram: return std::make_tuple(vdp2_vram_.data(), vdp2_vram_.size(), vdp2_vram_address.start);
        case vdp2_color_ram: return std::make_tuple(vdp2_cram_.data(), vdp2_cram_.size(), vdp2_cram_address.start);
        case vdp2_registers: return std::make_tuple(vdp2_registers_.data(), vdp2_registers_.size(), vdp2_regs_address.start);
        case scu: return std::make_tuple(scu_.data(), scu_.size(), scu_address.start);
        case workram_high: return std::make_tuple(workram_high_.data(), workram_high_.size(), workram_high_address.start);
        default: return std::make_tuple(nullptr, 0, 0);
    }
};

void mirrorData(u8* data, const u32 size, const u8 times_mirrored, const RomLoad RomLoad) {
    if (times_mirrored > 0) {
        auto multiple = u32{};
        switch (RomLoad) {
            using enum RomLoad;
            case not_interleaved: multiple = 1; break;
            case even_interleaved: multiple = 2; break;
            case odd_interleaved: multiple = 2; break;
        }
        for (u8 i = 1; i <= times_mirrored; ++i) {
            std::copy(data, data + size * multiple - 1, data + (i * size * multiple));
        }
    }
}

auto listAvailableStvGames() -> std::vector<StvGameConfiguration> {
    auto games = std::vector<StvGameConfiguration>{};
    games.push_back(defaultStvGame());

    auto config = core::Config("");
    // Getting the files of the ST-V directory.
    const auto full_path = fs::current_path() / "stv";
    for (auto& p : fs::directory_iterator(full_path)) {
        if ((p.path().extension() == ".cfg") && (p.path().filename() != "dummy.cfg")) {
            auto file = core::Config(p.path().string());
            file.readFile();

            auto game         = StvGameConfiguration{};
            game.game_name    = file.readValue(core::AccessKeys::stv_game_name).c_str();
            game.zip_name     = file.readValue(core::AccessKeys::stv_zip_name).c_str();
            game.parent_set   = file.readValue(core::AccessKeys::stv_parent_set).c_str();
            game.version      = file.readValue(core::AccessKeys::stv_version).c_str();
            game.release_date = file.readValue(core::AccessKeys::stv_release_date).c_str();
            game.region       = file.readValue(core::AccessKeys::stv_region).c_str();

            const libconfig::Setting& files = file.readValue(core::AccessKeys::stv_files);
            for (u8 i = 0; i < files.getLength(); ++i) {
                auto rom           = StvRomFile{};
                rom.rom_name       = files[i][0].c_str();
                rom.load_address   = u32{files[i][1]};
                rom.load_size      = u32{files[i][2]};
                rom.rom_load       = config.getRomLoad(files[i][3]);
                rom.times_mirrored = u32{files[i][4]};
                rom.rom_type       = config.getRomType(files[i][5]);
                game.files.push_back(rom);
            }

            games.push_back(game);
        }
    }

    return games;
}

auto defaultStvGame() -> StvGameConfiguration { return StvGameConfiguration{.game_name = tr("No game selected")}; }
auto defaultBinaryFile() -> BinaryFileConfiguration { return BinaryFileConfiguration{}; }

inline auto isMasterSh2InOperation(const Memory& m) -> bool { return (m.sh2_in_operation_ == sh2::Sh2Type::master); }

inline auto getDirectAddress(AddressRange ar) -> AddressRange {
    constexpr auto direct_address_offset = u32{0x20000000};
    ar.start |= direct_address_offset;
    ar.end |= direct_address_offset;
    return ar;
}

void Memory::initialize(saturnin::core::HardwareMode mode) {
    sh2_in_operation_ = sh2::Sh2Type::unknown;

    initializeHandlers();
    initializeMemoryMap();

    loadBios(mode);

    if (mode == HardwareMode::stv) {
        if (selectedStvGame().game_name != defaultStvGame().game_name) { loadStvGame(selectedStvGame()); }
    } else {
        if (selectedBinaryFile().full_path != defaultBinaryFile().full_path) {
            //            if (loadBinaryFile(selectedBinaryFile())) {
            installMinimumBiosRoutines();
            if (modules_.memory()->selectedBinaryFile().is_auto_started) {
                modules_.context()->debugStatus(DebugStatus::disabled);
            } else {
                modules_.context()->debugStatus(DebugStatus::paused);
            }
            //} else {
            //    selectedBinaryFile(defaultBinaryFile());
            //}
        }
    }
}

void Memory::initializeMemoryMap() {
    memory_map_.insert(MapArea::value_type(MemoryMapArea::rom, tr("Rom")));
    memory_map_.insert(MapArea::value_type(MemoryMapArea::smpc, tr("SMPC")));
    memory_map_.insert(MapArea::value_type(MemoryMapArea::backup_ram, tr("Backup RAM")));
    memory_map_.insert(MapArea::value_type(MemoryMapArea::workram_low, tr("Workram low")));
    memory_map_.insert(MapArea::value_type(MemoryMapArea::stv_io, tr("ST-V I/O")));
    memory_map_.insert(MapArea::value_type(MemoryMapArea::cart, tr("Cartridge")));
    memory_map_.insert(MapArea::value_type(MemoryMapArea::cd_block, tr("Cd block")));
    memory_map_.insert(MapArea::value_type(MemoryMapArea::scsp, tr("Sound RAM")));
    memory_map_.insert(MapArea::value_type(MemoryMapArea::vdp1_ram, tr("VDP1 RAM")));
    memory_map_.insert(MapArea::value_type(MemoryMapArea::vdp1_framebuffer, tr("VDP1 framebuffer")));
    memory_map_.insert(MapArea::value_type(MemoryMapArea::vdp1_registers, tr("VDP1 registers")));
    memory_map_.insert(MapArea::value_type(MemoryMapArea::vdp2_video_ram, tr("VDP2 video RAM")));
    memory_map_.insert(MapArea::value_type(MemoryMapArea::vdp2_color_ram, tr("VDP2 color RAM")));
    memory_map_.insert(MapArea::value_type(MemoryMapArea::vdp2_registers, tr("VDP2 registers")));
    memory_map_.insert(MapArea::value_type(MemoryMapArea::scu, tr("SCU")));
    memory_map_.insert(MapArea::value_type(MemoryMapArea::workram_high, tr("Workram high")));
}

void Memory::installStvBiosBypass() {
    const bool is_stv_bios_bypassed = modules_.config()->readValue(AccessKeys::cfg_global_stv_bios_bypass);
    if (is_stv_bios_bypassed) {
        // Replacing these 2 opcodes by nops speed up significantly ST-V boot up time.
        constexpr auto bypass_address_1  = u32{0xd4b0};
        this->rom_[bypass_address_1]     = 0x00;
        this->rom_[bypass_address_1 + 1] = 0x09;

        constexpr auto bypass_address_2  = u32{0xd4c6};
        this->rom_[bypass_address_2]     = 0x00;
        this->rom_[bypass_address_2 + 1] = 0x09;
    }
}

void Memory::installMinimumBiosRoutines() {
    // Copying first raw block including start vectors and routines
    std::copy_n(&rom_[0] + 0x600, 0xb00, &workram_high_[0]);

    rawWrite<u16>(workram_high_, 0x236 & workram_high_memory_mask, 0x02ac);
    rawWrite<u16>(workram_high_, 0x23a & workram_high_memory_mask, 0x02bc);
    rawWrite<u16>(workram_high_, 0x23e & workram_high_memory_mask, 0x0350);

    rawWrite<u32>(workram_high_, 0x240 & workram_high_memory_mask, rawRead<u32>(rom_, 0x1384));
    rawWrite<u32>(workram_high_, 0x254 & workram_high_memory_mask, rawRead<u32>(rom_, 0x1344));
    rawWrite<u32>(workram_high_, 0x268 & workram_high_memory_mask, rawRead<u32>(rom_, 0x1344));
    rawWrite<u32>(workram_high_, 0x26c & workram_high_memory_mask, rawRead<u32>(rom_, 0x1348));
    rawWrite<u32>(workram_high_, 0x284 & workram_high_memory_mask, rawRead<u32>(rom_, 0x1358));
    rawWrite<u32>(workram_high_, 0x288 & workram_high_memory_mask, rawRead<u32>(rom_, 0x134c));
    rawWrite<u32>(workram_high_, 0x28c & workram_high_memory_mask, rawRead<u32>(rom_, 0x1350));
    rawWrite<u32>(workram_high_, 0x29c & workram_high_memory_mask, rawRead<u32>(rom_, 0x1354));
    rawWrite<u32>(workram_high_, 0x2c0 & workram_high_memory_mask, rawRead<u32>(rom_, 0x1100));
    rawWrite<u32>(workram_high_, 0x2c4 & workram_high_memory_mask, rawRead<u32>(rom_, 0x1104));
    rawWrite<u32>(workram_high_, 0x2c8 & workram_high_memory_mask, rawRead<u32>(rom_, 0x1108));
    rawWrite<u32>(workram_high_, 0x2cc & workram_high_memory_mask, rawRead<u32>(rom_, 0x110c));
    rawWrite<u32>(workram_high_, 0x2d0 & workram_high_memory_mask, rawRead<u32>(rom_, 0x1110));
    rawWrite<u32>(workram_high_, 0x2d4 & workram_high_memory_mask, rawRead<u32>(rom_, 0x1114));
    rawWrite<u32>(workram_high_, 0x2d8 & workram_high_memory_mask, rawRead<u32>(rom_, 0x1118));
    rawWrite<u32>(workram_high_, 0x2dc & workram_high_memory_mask, rawRead<u32>(rom_, 0x111c));

    rawWrite<u16>(workram_high_, 0x328 & workram_high_memory_mask, 0x04c8);
    rawWrite<u16>(workram_high_, 0x32c & workram_high_memory_mask, 0x1800);

    for (int i = 0; i < 0x80; ++i) {
        rawWrite<u32>(workram_high_, 0xa00 + i, 0x0600083c);
    }
    rawWrite<u32>(workram_high_, 0xa00, 0x06028d64);
    rawWrite<u32>(workram_high_, 0xa04, 0x06028d9e);
    rawWrite<u32>(workram_high_, 0xa1c, 0x0602bcc2);
}
} // namespace saturnin::core
