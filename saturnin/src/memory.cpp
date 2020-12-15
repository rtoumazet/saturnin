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

#include <saturnin/src/memory.h>
#include <filesystem> // filesystem
#include <fstream>    // ifstream
#include <sstream>    // stringstream
#include <GLFW/glfw3.h>
#include <libzippp/libzippp.h>
#include <saturnin/src/config.h>
#include <saturnin/src/emulator_context.h>
#include <saturnin/src/locale.h> // NOLINT(modernize-deprecated-headers)
#include <saturnin/src/sh2.h>
#include <saturnin/src/cdrom/cdrom.h>
#include <saturnin/src/video/vdp2.h>

namespace lzpp = libzippp;
namespace fs   = std::filesystem;
namespace sh2  = saturnin::sh2;

namespace saturnin::core {

using core::Log;
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
constexpr auto vdp1_fb_address      = AddressRange{0x05C80000, 0x05CFFFFF};
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
    auto       rom_path = fs::path{config()->readValue(AccessKeys::cfg_paths_roms_stv).c_str()};
    rom_path /= zip;
    // rom_path += "\\" + zip_name + ".zip";

    lzpp::ZipArchive zf(rom_path.string());
    if (zf.open(lzpp::ZipArchive::ReadOnly)) {
        if (zf.hasEntry(file_name, false, false)) {
            lzpp::ZipEntry        entry = zf.getEntry(file_name, false, false);
            std::unique_ptr<u8[]> data(static_cast<u8*>(entry.readAsBinary()));

            switch (rom_type) {
                case RomType::bios: {
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
                case RomType::program:
                case RomType::graphic: {
                    const auto stv_bios_region_address = u32{0x808};
                    auto       region_cart_address     = u32{};
                    switch (rom_load) {
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
                    if (rom_type == RomType::program) { this->cart_[region_cart_address] = this->rom_[stv_bios_region_address]; }

                    mirrorData(destination, size, times_mirrored, rom_load);
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
            const auto str = fmt::format(tr("File '{0}' not found in zip file !"), file_name);
            Log::warning("memory", str);
            return false;
        }
        zf.close();
    } else {
        const auto str = fmt::format(tr("Zip file '{0}' not found !"), rom_path.string());
        Log::warning("memory", str);
        return false;
    }
    return true;
}

void Memory::loadBios(const HardwareMode mode) {
    Log::info("memory", tr("Loading bios"));
    auto bios_path = std::string{};
    switch (mode) {
        case HardwareMode::saturn: bios_path = config()->readValue(AccessKeys::cfg_paths_bios_saturn).c_str(); break;
        case HardwareMode::stv: bios_path = config()->readValue(AccessKeys::cfg_paths_bios_stv).c_str(); break;
        default: {
            Log::error("memory", tr("Unknown hardware mode"));
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
    } else {
        Log::warning("memory", tr("Bios file not found !"));
    }
}

auto Memory::loadStvGame(const std::string& config_filename) -> bool {
    const auto full_path = std::filesystem::current_path() / "stv" / config_filename;

    auto stv = core::Config(full_path.string());
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
        const auto        load_address   = u32{files[i][1]};
        const auto        load_size      = u32{files[i][2]};
        const auto        rom_load       = config()->getRomLoad(files[i][3]);
        const auto        times_mirrored = u32{files[i][4]};
        const auto        rom_type       = config()->getRomType(files[i][5]);
        if (!this->loadRom(zip_name, rom_name, &this->cart_[load_address], load_size, rom_load, times_mirrored, rom_type)) {
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
        Log::debug("memory", "ST-V protection read index: {}, value: {}", stv_protection_offset, data);
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
    Log::debug("memory", fmt::format(core::tr("ST-V offset start: {}"), this->stv_protection_offset_));
}

auto Memory::isStvProtectionEnabled() const -> bool {
    const auto relative_addr = calculateRelativeCartAddress(stv_protection_enabled);
    return cart_[relative_addr] == 0x1;
}

void Memory::sendFrtInterruptToMaster() const { masterSh2()->sendInterruptCaptureSignal(); }

void Memory::sendFrtInterruptToSlave() const { slaveSh2()->sendInterruptCaptureSignal(); }

auto Memory::getMemoryMapAreaData(const MemoryMapArea area) -> std::tuple<u8*, size_t, u32> const {
    switch (area) {
        case MemoryMapArea::rom: return std::make_tuple(rom_.data(), rom_.size(), rom_address.start);
        case MemoryMapArea::smpc: return std::make_tuple(smpc_.data(), smpc_.size(), smpc_address.start);
        case MemoryMapArea::backup_ram: return std::make_tuple(backup_ram_.data(), backup_ram_.size(), backup_ram_address.start);
        case MemoryMapArea::workram_low:
            return std::make_tuple(workram_low_.data(), workram_low_.size(), workram_low_address.start);
        case MemoryMapArea::stv_io: return std::make_tuple(stv_io_.data(), stv_io_.size(), stv_io_address.start);
        case MemoryMapArea::cart: return std::make_tuple(cart_.data(), cart_.size(), cart_address.start);
        // case MemoryMapArea::cd_block: return std::make_tuple(.data(), .size(), cd_block_address.start);
        case MemoryMapArea::scsp: return std::make_tuple(sound_ram_.data(), sound_ram_.size(), scsp_address.start);
        case MemoryMapArea::vdp1_ram: return std::make_tuple(vdp1_vram_.data(), vdp1_vram_.size(), vdp1_ram_address.start);
        case MemoryMapArea::vdp1_framebuffer:
            return std::make_tuple(vdp1_framebuffer_.data(), vdp1_framebuffer_.size(), vdp1_fb_address.start);
        case MemoryMapArea::vdp1_registers:
            return std::make_tuple(vdp1_registers_.data(), vdp1_registers_.size(), vdp1_regs_address.start);
        case MemoryMapArea::vdp2_video_ram: return std::make_tuple(vdp2_vram_.data(), vdp2_vram_.size(), vdp2_vram_address.start);
        case MemoryMapArea::vdp2_color_ram: return std::make_tuple(vdp2_cram_.data(), vdp2_cram_.size(), vdp2_cram_address.start);
        case MemoryMapArea::vdp2_registers:
            return std::make_tuple(vdp2_registers_.data(), vdp2_registers_.size(), vdp2_regs_address.start);
        case MemoryMapArea::scu: return std::make_tuple(scu_.data(), scu_.size(), scu_address.start);
        case MemoryMapArea::workram_high:
            return std::make_tuple(workram_high_.data(), workram_high_.size(), workram_high_address.start);
    }
    return std::make_tuple(nullptr, 0, 0);
};

auto Memory::config() const -> Config* { return emulator_context_->config(); };

auto Memory::masterSh2() const -> sh2::Sh2* { return emulator_context_->masterSh2(); };

auto Memory::slaveSh2() const -> sh2::Sh2* { return emulator_context_->slaveSh2(); };

auto Memory::scu() const -> Scu* { return emulator_context_->scu(); };

auto Memory::smpc() const -> Smpc* { return emulator_context_->smpc(); };

auto Memory::openglWindow() const -> GLFWwindow* { return emulator_context_->openglWindow(); };

auto Memory::cdrom() const -> cdrom::Cdrom* { return emulator_context_->cdrom(); };

auto Memory::vdp2() const -> video::Vdp2* { return emulator_context_->vdp2(); };

void mirrorData(u8* data, const u32 size, const u8 times_mirrored, const RomLoad RomLoad) {
    if (times_mirrored > 0) {
        auto multiple = u32{};
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
    const auto full_path = fs::current_path() / "stv";
    auto       files     = std::vector<std::string>{};
    for (auto& p : fs::directory_iterator(full_path)) {
        if ((p.path().extension() == ".cfg") && (p.path().filename() != "dummy.cfg")) {
            files.push_back(p.path().filename().string());
        }
    }
    return files;
}

inline auto isMasterSh2InOperation(const Memory& m) -> bool { return (m.sh2_in_operation_ == sh2::Sh2Type::master); }

inline auto getDirectAddress(AddressRange ar) -> AddressRange {
    constexpr auto direct_address_offset = u32{0x20000000};
    ar.start |= direct_address_offset;
    ar.end |= direct_address_offset;
    return ar;
}

void Memory::initialize() {
    sh2_in_operation_ = sh2::Sh2Type::unknown;

    initializeHandlers();
    initializeMemoryMap();
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
} // namespace saturnin::core
