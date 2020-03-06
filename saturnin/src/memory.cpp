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

#include <sstream> // stringstream
#include <fstream> // ifstream
#include <libzippp/libzippp.h>
#include "locale.h" // NOLINT(modernize-deprecated-headers)
#include "emulator_context.h"
#include "config.h"
#include "sh2.h"

namespace lzpp = libzippp;
namespace fs   = std::filesystem;
namespace sh2  = saturnin::sh2;

namespace saturnin::core {

using core::Log;

constexpr u8 region_cart_address_not_interleaved  = 0x40;
constexpr u8 region_cart_address_odd_interleaved  = 0x81;
constexpr u8 region_cart_address_even_interleaved = 0x80;

// constexpr u32 direct_address_offset     = 0x20000000;

auto Memory::loadRom(const std::string& zip_name,
                     const std::string& file_name,
                     u8*                destination,
                     const u32          size,
                     const RomLoad      RomLoad,
                     const u8           times_mirrored,
                     const RomType      RomType) -> bool {
    std::string zip = zip_name + ".zip";
    fs::path    rom_path{config()->readValue(AccessKeys::cfg_paths_roms_stv).c_str()};
    rom_path /= zip;
    // rom_path += "\\" + zip_name + ".zip";

    lzpp::ZipArchive zf(rom_path.string());
    if (zf.open(lzpp::ZipArchive::READ_ONLY)) {
        if (zf.hasEntry(file_name, false, false)) {
            lzpp::ZipEntry        entry = zf.getEntry(file_name, false, false);
            std::unique_ptr<u8[]> data(static_cast<u8*>(entry.readAsBinary()));

            switch (RomType) {
                case RomType::bios: {
                    u32 counter = size / 4;
                    // Needs byteswapping
                    for (u32 i = 0; i < counter; ++i) {
                        destination[i * 4 + 1] = data[i * 4 + 0];
                        destination[i * 4 + 0] = data[i * 4 + 1];
                        destination[i * 4 + 3] = data[i * 4 + 2];
                        destination[i * 4 + 2] = data[i * 4 + 3];
                    }
                    break;
                }
                case RomType::program:
                case RomType::graphic: {
                    const u32 stv_bios_region_address = 0x808;
                    u32       region_cart_address{};
                    switch (RomLoad) {
                        case RomLoad::not_interleaved: {
                            const auto& src_begin = data.get();
                            std::move(src_begin, std::next(src_begin, size), destination);

                            region_cart_address = region_cart_address_not_interleaved;
                            break;
                        }
                        case RomLoad::odd_interleaved: { // Data is loaded on odd bytes only
                            for (u32 i = 0; i < size; ++i) {
                                destination[(i * 2 + 1)] = data[i];
                            }

                            region_cart_address = region_cart_address_odd_interleaved;
                            break;
                        }
                        case RomLoad::even_interleaved: { // Data is loaded on even bytes only
                            for (u32 i = 0; i < size; ++i) {
                                destination[i * 2] = data[i];
                            }

                            region_cart_address = region_cart_address_even_interleaved;
                            break;
                        }
                    }

                    // bios region is forced for program roms
                    if (RomType == RomType::program) {
                        this->cart_[region_cart_address] = this->rom_[stv_bios_region_address];
                    }

                    mirrorData(destination, size, times_mirrored, RomLoad);
                    break;
                }
                default: {
                    Log::warning("memory", fmt::format(tr("Unknown rom type while loading {0}.zip"), zip_name));
                    return false;
                    break;
                }
            }
        } else {
            zf.close();
            std::string str = fmt::format(tr("File '{0}' not found in zip file !"), file_name);
            Log::warning("memory", str);
            return false;
        }
        zf.close();
    } else {
        std::string str = fmt::format(tr("Zip file '{0}' not found !"), rom_path.string());
        Log::warning("memory", str);
        return false;
    }
    return true;
}

void Memory::loadBios(const HardwareMode mode) {
    std::string bios_path{};
    switch (mode) {
        case HardwareMode::saturn: bios_path = config()->readValue(AccessKeys::cfg_paths_bios_saturn).c_str(); break;
        case HardwareMode::stv: bios_path = config()->readValue(AccessKeys::cfg_paths_bios_stv).c_str(); break;
        default: {
            Log::error("config", tr("Unknown hardware mode"));
            throw std::runtime_error("Config error !");
            break;
        }
    }

    std::ifstream input_file(bios_path, std::ios::binary);
    if (input_file) {
        std::stringstream buffer;
        buffer << input_file.rdbuf();
        input_file.close();

        std::string str = buffer.str();

        switch (mode) {
            case HardwareMode::saturn: {
                std::move(str.begin(), str.end(), this->rom_.data());
                break;
            }
            case HardwareMode::stv: {
                // Needs byteswapping
                for (size_t i = 0; i < str.size(); i += 4) {
                    this->rom_[i + 1] = str[i + 0];
                    this->rom_[i + 0] = str[i + 1];
                    this->rom_[i + 3] = str[i + 2];
                    this->rom_[i + 2] = str[i + 3];
                }
                break;
            }
        }
    }
}

auto Memory::loadStvGame(const std::string& config_filename) -> bool {
    auto full_path = std::filesystem::current_path() / "stv" / config_filename;

    core::Config stv(full_path.string());
    stv.readFile();

    const std::string         game_name    = stv.readValue(core::AccessKeys::stv_game_name);
    const std::string         zip_name     = stv.readValue(core::AccessKeys::stv_zip_name);
    const std::string         parent_set   = stv.readValue(core::AccessKeys::stv_parent_set);
    const std::string         version      = stv.readValue(core::AccessKeys::stv_version);
    const std::string         release_date = stv.readValue(core::AccessKeys::stv_release_date);
    const std::string         region       = stv.readValue(core::AccessKeys::stv_region);
    const libconfig::Setting& files        = stv.readValue(core::AccessKeys::stv_files);
    for (u8 i = 0; i < files.getLength(); ++i) {
        const std::string rom_name       = files[i][0];
        const u32         load_address   = files[i][1];
        const u32         load_size      = files[i][2];
        const auto        rom_load       = Config::rom_load[files[i][3]];
        const u32         times_mirrored = files[i][4];
        const auto        rom_type       = Config::rom_type[files[i][5]];
        if (!this->loadRom(zip_name, rom_name, &this->cart_[load_address], load_size, rom_load, times_mirrored, rom_type)) {
            return false;
        }
    }

    swapCartArea();

    return true;
}

void Memory::swapCartArea() {
    const u32 program_rom_size = 0x400000;

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
    constexpr u32 dummy_start_address = 0x00000000;
    constexpr u32 dummy_end_address   = 0xFFFFFFFF;
    initializeHandlers<readDummy, u8, u16, u32>(dummy_start_address, dummy_end_address);
    initializeHandlers<writeDummy, u8, u16, u32>(dummy_start_address, dummy_end_address);

    // ROM access
    constexpr u32 rom_start_address = 0x00000000;
    constexpr u32 rom_end_address   = 0x000FFFFF;

    initializeHandlers<readRom, u8, u16, u32>(rom_start_address, rom_end_address);
    initializeHandlers<readRom, u8, u16, u32>(getDirectAddress(rom_start_address), getDirectAddress(rom_end_address));

    // SMPC access
    constexpr u32 smpc_start_address = 0x00100000;
    constexpr u32 smpc_end_address   = 0x0017FFFF;

    initializeHandlers<readSmpc, u8, u16, u32>(smpc_start_address, smpc_end_address);
    initializeHandlers<readSmpc, u8, u16, u32>(getDirectAddress(smpc_start_address), getDirectAddress(smpc_end_address));

    initializeHandlers<writeSmpc, u8, u16, u32>(smpc_start_address, smpc_end_address);
    initializeHandlers<writeSmpc, u8, u16, u32>(getDirectAddress(smpc_start_address), getDirectAddress(smpc_end_address));

    // Backup RAM access
    constexpr u32 backup_ram_start_address = 0x00180000;
    constexpr u32 backup_ram_end_address   = 0x001FFFFF;

    initializeHandlers<readBackupRam, u8, u16, u32>(backup_ram_start_address, backup_ram_end_address);
    initializeHandlers<readBackupRam, u8, u16, u32>(getDirectAddress(backup_ram_start_address),
                                                    getDirectAddress(backup_ram_end_address));

    initializeHandlers<writeBackupRam, u8, u16, u32>(backup_ram_start_address, backup_ram_end_address);
    initializeHandlers<writeBackupRam, u8, u16, u32>(getDirectAddress(backup_ram_start_address),
                                                     getDirectAddress(backup_ram_end_address));

    // Low workram access
    constexpr u32 workram_low_start_address = 0x00200000;
    constexpr u32 workram_low_end_address   = 0x002FFFFF;

    initializeHandlers<readWorkramLow, u8, u16, u32>(workram_low_start_address, workram_low_end_address);
    initializeHandlers<readWorkramLow, u8, u16, u32>(getDirectAddress(workram_low_start_address),
                                                     getDirectAddress(workram_low_end_address));

    initializeHandlers<writeWorkramLow, u8, u16, u32>(workram_low_start_address, workram_low_end_address);
    initializeHandlers<writeWorkramLow, u8, u16, u32>(getDirectAddress(workram_low_start_address),
                                                      getDirectAddress(workram_low_end_address));

    // STV I/O access
    constexpr u32 stv_io_start_address = 0x00400000;
    constexpr u32 stv_io_end_address   = 0x004FFFFF;

    initializeHandlers<readStvIo, u8, u16, u32>(stv_io_start_address, stv_io_end_address);
    initializeHandlers<readStvIo, u8, u16, u32>(getDirectAddress(stv_io_start_address), getDirectAddress(stv_io_end_address));

    initializeHandlers<writeStvIo, u8, u16, u32>(stv_io_start_address, stv_io_end_address);
    initializeHandlers<writeStvIo, u8, u16, u32>(getDirectAddress(stv_io_start_address), getDirectAddress(stv_io_end_address));

    // Cart access
    constexpr u32 cart_start_address = 0x02000000;
    constexpr u32 cart_end_address   = 0x04FFFFFF;

    initializeHandlers<readCart, u8, u16, u32>(cart_start_address, cart_end_address);
    initializeHandlers<readCart, u8, u16, u32>(getDirectAddress(cart_start_address), getDirectAddress(cart_end_address));

    initializeHandlers<writeCart, u8, u16, u32>(cart_start_address, cart_end_address);
    initializeHandlers<writeCart, u8, u16, u32>(getDirectAddress(cart_start_address), getDirectAddress(cart_end_address));

    // CdBlock access
    constexpr u32 cd_block_start_address = 0x05800000;
    constexpr u32 cd_block_end_address   = 0x058FFFFF;

    initializeHandlers<readCdBlock, u8, u16, u32>(cd_block_start_address, cd_block_end_address);
    initializeHandlers<readCdBlock, u8, u16, u32>(getDirectAddress(cd_block_start_address),
                                                  getDirectAddress(cd_block_end_address));

    initializeHandlers<writeCdBlock, u8, u16, u32>(cd_block_start_address, cd_block_end_address);
    initializeHandlers<writeCdBlock, u8, u16, u32>(getDirectAddress(cd_block_start_address),
                                                   getDirectAddress(cd_block_end_address));

    // SCSP access
    constexpr u32 scsp_start_address = 0x05A00000;
    constexpr u32 scsp_end_address   = 0x05BFFFFF;

    initializeHandlers<readScsp, u8, u16, u32>(scsp_start_address, scsp_end_address);
    initializeHandlers<readScsp, u8, u16, u32>(getDirectAddress(scsp_start_address), getDirectAddress(scsp_end_address));

    initializeHandlers<writeScsp, u8, u16, u32>(scsp_start_address, scsp_end_address);
    initializeHandlers<writeScsp, u8, u16, u32>(getDirectAddress(scsp_start_address), getDirectAddress(scsp_end_address));

    // VDP1 RAM access
    constexpr u32 vdp1_ram_start_address = 0x05C00000;
    constexpr u32 vdp1_ram_end_address   = 0x05C7FFFF;

    initializeHandlers<readVdp1Ram, u8, u16, u32>(vdp1_ram_start_address, vdp1_ram_end_address);
    initializeHandlers<readVdp1Ram, u8, u16, u32>(getDirectAddress(vdp1_ram_start_address),
                                                  getDirectAddress(vdp1_ram_end_address));

    initializeHandlers<writeVdp1Ram, u8, u16, u32>(vdp1_ram_start_address, vdp1_ram_end_address);
    initializeHandlers<writeVdp1Ram, u8, u16, u32>(getDirectAddress(vdp1_ram_start_address),
                                                   getDirectAddress(vdp1_ram_end_address));

    // VDP1 framebuffer access
    constexpr u32 vdp1_fb_start_address = 0x05C80000;
    constexpr u32 vdp1_fb_end_address   = 0x05CFFFFF;

    initializeHandlers<readVdp1Framebuffer, u8, u16, u32>(vdp1_fb_start_address, vdp1_fb_end_address);
    initializeHandlers<readVdp1Framebuffer, u8, u16, u32>(getDirectAddress(vdp1_fb_start_address),
                                                          getDirectAddress(vdp1_fb_end_address));

    initializeHandlers<writeVdp1Framebuffer, u8, u16, u32>(vdp1_fb_start_address, vdp1_fb_end_address);
    initializeHandlers<writeVdp1Framebuffer, u8, u16, u32>(getDirectAddress(vdp1_fb_start_address),
                                                           getDirectAddress(vdp1_fb_end_address));

    // VDP1 Registers access
    constexpr u32 vdp1_regs_start_address = 0x05D00000;
    constexpr u32 vdp1_regs_end_address   = 0x05D7FFFF;

    initializeHandlers<readVdp1Registers, u8, u16, u32>(vdp1_regs_start_address, vdp1_regs_end_address);
    initializeHandlers<readVdp1Registers, u8, u16, u32>(getDirectAddress(vdp1_regs_start_address),
                                                        getDirectAddress(vdp1_regs_end_address));

    initializeHandlers<writeVdp1Registers, u8, u16, u32>(vdp1_regs_start_address, vdp1_regs_end_address);
    initializeHandlers<writeVdp1Registers, u8, u16, u32>(getDirectAddress(vdp1_regs_start_address),
                                                         getDirectAddress(vdp1_regs_end_address));

    // VDP2 VRAM access
    constexpr u32 vdp2_vram_start_address = 0x05E00000;
    constexpr u32 vdp2_vram_end_address   = 0x05EFFFFF;

    initializeHandlers<readVdp2Vram, u8, u16, u32>(vdp2_vram_start_address, vdp2_vram_end_address);
    initializeHandlers<readVdp2Vram, u8, u16, u32>(getDirectAddress(vdp2_vram_start_address),
                                                   getDirectAddress(vdp2_vram_end_address));

    initializeHandlers<writeVdp2Vram, u8, u16, u32>(vdp2_vram_start_address, vdp2_vram_end_address);
    initializeHandlers<writeVdp2Vram, u8, u16, u32>(getDirectAddress(vdp2_vram_start_address),
                                                    getDirectAddress(vdp2_vram_end_address));

    // VDP2 CRAM access
    constexpr u32 vdp2_cram_start_address = 0x05F00000;
    constexpr u32 vdp2_cram_end_address   = 0x05F7FFFF;

    initializeHandlers<readVdp2Cram, u8, u16, u32>(vdp2_cram_start_address, vdp2_cram_end_address);
    initializeHandlers<readVdp2Cram, u8, u16, u32>(getDirectAddress(vdp2_cram_start_address),
                                                   getDirectAddress(vdp2_cram_end_address));

    initializeHandlers<writeVdp2Cram, u8, u16, u32>(vdp2_cram_start_address, vdp2_cram_end_address);
    initializeHandlers<writeVdp2Cram, u8, u16, u32>(getDirectAddress(vdp2_cram_start_address),
                                                    getDirectAddress(vdp2_cram_end_address));

    // VDP2 Registers access
    constexpr u32 vdp2_regs_start_address = 0x05F80000;
    constexpr u32 vdp2_regs_end_address   = 0x05FBFFFF;

    initializeHandlers<readVdp2Registers, u8, u16, u32>(vdp2_regs_start_address, vdp2_regs_end_address);
    initializeHandlers<readVdp2Registers, u8, u16, u32>(getDirectAddress(vdp2_regs_start_address),
                                                        getDirectAddress(vdp2_regs_end_address));

    initializeHandlers<writeVdp2Registers, u8, u16, u32>(vdp2_regs_start_address, vdp2_regs_end_address);
    initializeHandlers<writeVdp2Registers, u8, u16, u32>(getDirectAddress(vdp2_regs_start_address),
                                                         getDirectAddress(vdp2_regs_end_address));

    // SCU access
    constexpr u32 scu_start_address = 0x05FE0000;
    constexpr u32 scu_end_address   = 0x05FEFFFF;

    initializeHandlers<readScu, u8, u16, u32>(scu_start_address, scu_end_address);
    initializeHandlers<readScu, u8, u16, u32>(getDirectAddress(scu_start_address), getDirectAddress(scu_end_address));

    initializeHandlers<writeScu, u8, u16, u32>(scu_start_address, scu_end_address);
    initializeHandlers<writeScu, u8, u16, u32>(getDirectAddress(scu_start_address), getDirectAddress(scu_end_address));

    // Workram high access
    constexpr u32 workram_high_start_address = 0x06000000;
    constexpr u32 workram_high_end_address   = 0x07FFFFFF;

    initializeHandlers<readWorkramHigh, u8, u16, u32>(workram_high_start_address, workram_high_end_address);
    initializeHandlers<readWorkramHigh, u8, u16, u32>(getDirectAddress(workram_high_start_address),
                                                      getDirectAddress(workram_high_end_address));

    initializeHandlers<writeWorkramHigh, u8, u16, u32>(workram_high_start_address, workram_high_end_address);
    initializeHandlers<writeWorkramHigh, u8, u16, u32>(getDirectAddress(workram_high_start_address),
                                                       getDirectAddress(workram_high_end_address));

    // Master FRT access
    constexpr u32 master_frt_start_address = 0x01800000;
    constexpr u32 master_frt_end_address   = 0x01FFFFFF;

    initializeHandlers<writeMasterSh2Frt, u8, u16, u32>(master_frt_start_address, master_frt_end_address);
    initializeHandlers<writeMasterSh2Frt, u8, u16, u32>(getDirectAddress(master_frt_start_address),
                                                        getDirectAddress(master_frt_end_address));

    // Slave FRT access
    constexpr u32 slave_frt_start_address = 0x01000000;
    constexpr u32 slave_frt_end_address   = 0x017FFFFF;

    initializeHandlers<writeSlaveSh2Frt, u8, u16, u32>(slave_frt_start_address, slave_frt_end_address);
    initializeHandlers<writeSlaveSh2Frt, u8, u16, u32>(getDirectAddress(slave_frt_start_address),
                                                       getDirectAddress(slave_frt_end_address));

    // SH2 register access
    constexpr u32 sh2_regs_start_address = 0xFFFFFE00;
    constexpr u32 sh2_regs_end_address   = 0xFFFFFFFF;

    initializeHandlers<readSh2Registers, u8, u16, u32>(sh2_regs_start_address, sh2_regs_end_address);
    initializeHandlers<writeSh2Registers, u8, u16, u32>(sh2_regs_start_address, sh2_regs_end_address);

    // Cache addresses access
    constexpr u32 cache_addresses_start_address = 0x60000000;
    constexpr u32 cache_addresses_end_address   = 0x6FFFFFFF;

    initializeHandlers<readCacheAddresses, u8, u16, u32>(cache_addresses_start_address, cache_addresses_end_address);
    initializeHandlers<writeCacheAddresses, u8, u16, u32>(cache_addresses_start_address, cache_addresses_end_address);

    // Cache data access
    constexpr u32 cache_data_1_start_address = 0x80000000;
    constexpr u32 cache_data_1_end_address   = 0x8FFFFFFF;
    constexpr u32 cache_data_2_start_address = 0xC0000000;
    constexpr u32 cache_data_2_end_address   = 0xCFFFFFFF;

    initializeHandlers<readCacheData, u8, u16, u32>(cache_data_1_start_address, cache_data_1_end_address);
    initializeHandlers<readCacheData, u8, u16, u32>(cache_data_2_start_address, cache_data_2_end_address);

    initializeHandlers<writeCacheData, u8, u16, u32>(cache_data_1_start_address, cache_data_1_end_address);
    initializeHandlers<writeCacheData, u8, u16, u32>(cache_data_2_start_address, cache_data_2_end_address);
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
        Log::debug("memory", "ST-V protection read index: {}, value: {}", stv_protection_offset, data);
    } else {
        stv_protection_offset = 0;
    }

    return data;
}

void Memory::writeStvProtection(const u32 addr, u32 data) {
    u32       relative_addr = calculateRelativeCartAddress(stv_protection_register_address);
    const u32 index         = rawRead<u32>(this->cart_, relative_addr);
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
    Log::debug("memory", fmt::format(core::tr("ST-V offset start: {}"), this->stv_protection_offset_));
}

auto Memory::isStvProtectionEnabled() const -> bool {
    u32 relative_addr = calculateRelativeCartAddress(stv_protection_enabled);
    return cart_[relative_addr] == 0x1;
}

void Memory::sendFrtInterruptToMaster() const { masterSh2()->sendInterruptCaptureSignal(); }

void Memory::sendFrtInterruptToSlave() const { slaveSh2()->sendInterruptCaptureSignal(); }

auto Memory::config() const -> Config* { return emulator_context_->config(); };

auto Memory::masterSh2() const -> sh2::Sh2* { return emulator_context_->masterSh2(); };

auto Memory::slaveSh2() const -> sh2::Sh2* { return emulator_context_->slaveSh2(); };

auto Memory::scu() const -> Scu* { return emulator_context_->scu(); };

auto Memory::smpc() const -> Smpc* { return emulator_context_->smpc(); };

void mirrorData(u8* data, const u32 size, const u8 times_mirrored, const RomLoad RomLoad) {
    if (times_mirrored > 0) {
        u32 multiple{};
        switch (RomLoad) {
            case RomLoad::not_interleaved: multiple = 1; break;
            case RomLoad::even_interleaved: multiple = 2; break;
            case RomLoad::odd_interleaved: multiple = 2; break;
        }
        for (u8 i = 1; i <= times_mirrored; ++i) {
            std::copy(data, data + size * multiple - 1, data + (i * size * multiple));
        }
    }
}

auto listStvConfigurationFiles() -> std::vector<std::string> {
    auto                     full_path = fs::current_path() / "stv";
    std::vector<std::string> files;
    for (auto& p : fs::directory_iterator(full_path)) {
        if ((p.path().extension() == ".cfg") && (p.path().filename() != "dummy.cfg")) {
            files.push_back(p.path().filename().string());
        }
    }
    return files;
}

inline auto isMasterSh2InOperation(const Memory& m) -> bool { return (m.sh2_in_operation_ == sh2::Sh2Type::master); }

inline auto getDirectAddress(u32 cached_address) -> u32 {
    constexpr u32 direct_address_offset = 0x20000000;
    return cached_address + direct_address_offset;
}

void Memory::initialize() {
    sh2_in_operation_ = sh2::Sh2Type::unknown;

    initializeHandlers();
}

} // namespace saturnin::core
