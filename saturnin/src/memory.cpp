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
#include "locale.h"
#include "emulator_context.h"
#include "sh2.h"

namespace lzpp = libzippp;
namespace fs = std::filesystem;
namespace sh2 = saturnin::sh2;

namespace saturnin {
namespace core {

bool Memory::loadRom(const std::string& zip_name,
                     const std::string&  file_name,
                     u8*	   destination,
                     const u32 size,
                     const Rom_load rom_load,
                     const u8  times_mirrored,
                     const Rom_type rom_type) {

    std::string zip = zip_name + ".zip";
    fs::path rom_path{ config()->readValue(Access_keys::config_roms_stv).c_str() };
    rom_path /= zip;
    //rom_path += "\\" + zip_name + ".zip";

    lzpp::ZipArchive zf(rom_path.string());
    if (zf.open(lzpp::ZipArchive::READ_ONLY)) {
        if (zf.hasEntry(file_name, false, false)) {
            lzpp::ZipEntry entry = zf.getEntry(file_name, false, false);
            std::unique_ptr<u8[]> data(static_cast<u8*>(entry.readAsBinary()));

            switch (rom_type) {
                case Rom_type::bios: {
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
                case Rom_type::program:
                case Rom_type::graphic: {
                    const u32 stv_bios_region_address = 0x808;
                    u32 region_cart_address{};
                    switch (rom_load) {
                        case Rom_load::not_interleaved: {
                            const auto &src_begin = data.get();
                            std::move(src_begin, std::next(src_begin, size), destination);

                            region_cart_address = 0x40;
                            break;
                        }
                        case Rom_load::odd_interleaved: {  // Data is loaded on odd bytes only
                            for (u32 i = 0; i < size; ++i) {
                                destination[(i * 2 + 1)] = data[i];
                            }

                            region_cart_address = 0x81;
                            break;
                        }
                        case Rom_load::even_interleaved: { // Data is loaded on even bytes only
                            for (u32 i = 0; i < size; ++i) {
                                destination[i * 2] = data[i];
                            }

                            region_cart_address = 0x80;
                            break;
                        }
                    }

                    // bios region is forced for program roms
                    if (rom_type == Rom_type::program) this->cart_[region_cart_address] = this->rom_[stv_bios_region_address];

                    mirrorData(destination, size, times_mirrored, rom_load);
                    break;
                }
                default: {
                    Log::warning("memory", fmt::format(tr("Unknown rom type while loading {0}.zip"), zip_name));
                    return false;
                    break;
                }
            }
        }
        else {
            zf.close();
            std::string str = fmt::format(tr("File '{0}' not found in zip file !"), file_name);
            Log::warning("memory", str);
            return false;
        }
        zf.close();
    }
    else {
        std::string str = fmt::format(tr("Zip file '{0}' not found !"), rom_path.string());
        Log::warning("memory", str);
        return false;
    }
    return true;
}

void Memory::loadBios(const HardwareMode mode) {
    std::string bios_path{};
    switch (mode) {
        case HardwareMode::saturn: bios_path = config()->readValue(Access_keys::config_bios_saturn).c_str(); break;
        case HardwareMode::stv:    bios_path = config()->readValue(Access_keys::config_bios_stv).c_str(); break;
        default: {
            Log::error("config", tr("Unknown hardware mode"));
            Log::error("config", tr("Exiting ..."));
            exit(EXIT_FAILURE);
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

bool Memory::loadStvGame(const std::string& config_filename) {
    auto full_path = std::filesystem::current_path() / "stv" / config_filename;

    core::Config stv(full_path.string());
    stv.readFile();

    const std::string game_name = stv.readValue(core::Access_keys::stv_game_name);
    const std::string zip_name = stv.readValue(core::Access_keys::stv_zip_name);
    const std::string parent_set = stv.readValue(core::Access_keys::stv_parent_set);
    const std::string version = stv.readValue(core::Access_keys::stv_version);
    const std::string release_date = stv.readValue(core::Access_keys::stv_release_date);
    const std::string region = stv.readValue(core::Access_keys::stv_region);
    const libconfig::Setting& files = stv.readValue(core::Access_keys::stv_files);
    for (u8 i = 0; i < files.getLength(); ++i) {
        const std::string rom_name = files[i][0];
        const u32    load_address = files[i][1];
        const u32    load_size = files[i][2];
        const auto        rom_load = Config::rom_load[files[i][3]];
        const u32    times_mirrored = files[i][4];
        const auto        rom_type = Config::rom_type[files[i][5]];
        if (!this->loadRom(zip_name,
                           rom_name,
                           &this->cart_[load_address],
                           load_size,
                           rom_load,
                           times_mirrored,
                           rom_type)) {
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
    initializeHandlers<readDummy, u8, u16, u32>(0x00000000, 0xFFFFFFFF);
    initializeHandlers<writeDummy, u8, u16, u32>(0x00000000, 0xFFFFFFFF);


    // ROM access
    initializeHandlers<readRom, u8, u16, u32>(0x00000000, 0x000FFFFF);
    initializeHandlers<readRom, u8, u16, u32>(0x20000000, 0x200FFFFF);

    // SMPC access
    initializeHandlers<readSmpc, u8, u16, u32>(0x00100000, 0x0017FFFF);
    initializeHandlers<readSmpc, u8, u16, u32>(0x20100000, 0x2017FFFF);

    initializeHandlers<writeSmpc, u8, u16, u32>(0x00100000, 0x0017FFFF);
    initializeHandlers<writeSmpc, u8, u16, u32>(0x20100000, 0x2017FFFF);

    // Backup RAM access
    initializeHandlers<readBackupRam, u8, u16, u32>(0x00180000, 0x001FFFFF);
    initializeHandlers<readBackupRam, u8, u16, u32>(0x20180000, 0x201FFFFF);

    initializeHandlers<writeBackupRam, u8, u16, u32>(0x00180000, 0x001FFFFF);
    initializeHandlers<writeBackupRam, u8, u16, u32>(0x20180000, 0x201FFFFF);

    // Low workram access
    initializeHandlers<readWorkramLow, u8, u16, u32>(0x00200000, 0x002FFFFF);
    initializeHandlers<readWorkramLow, u8, u16, u32>(0x20200000, 0x202FFFFF);

    initializeHandlers<writeWorkramLow, u8, u16, u32>(0x00200000, 0x002FFFFF);
    initializeHandlers<writeWorkramLow, u8, u16, u32>(0x20200000, 0x202FFFFF);

    // STV I/O access
    initializeHandlers<readStvIo, u8, u16, u32>(0x00400000, 0x004FFFFF);
    initializeHandlers<readStvIo, u8, u16, u32>(0x20400000, 0x204FFFFF);

    initializeHandlers<writeStvIo, u8, u16, u32>(0x00400000, 0x004FFFFF);
    initializeHandlers<writeStvIo, u8, u16, u32>(0x20400000, 0x204FFFFF);

    // Cart access
    initializeHandlers<readCart, u8, u16, u32>(0x02000000, 0x04FFFFFF);
    initializeHandlers<readCart, u8, u16, u32>(0x22000000, 0x24FFFFFF);

    initializeHandlers<writeCart, u8, u16, u32>(0x02000000, 0x04FFFFFF);
    initializeHandlers<writeCart, u8, u16, u32>(0x22000000, 0x24FFFFFF);

    // CdBlock access
    initializeHandlers<readCdBlock, u8, u16, u32>(0x05800000, 0x058FFFFF);
    initializeHandlers<readCdBlock, u8, u16, u32>(0x25800000, 0x258FFFFF);

    initializeHandlers<writeCdBlock, u8, u16, u32>(0x05800000, 0x058FFFFF);
    initializeHandlers<writeCdBlock, u8, u16, u32>(0x25800000, 0x258FFFFF);

    // SCSP access
    initializeHandlers<readScsp, u8, u16, u32>(0x05A00000, 0x05BFFFFF);
    initializeHandlers<readScsp, u8, u16, u32>(0x25A00000, 0x25BFFFFF);

    initializeHandlers<writeScsp, u8, u16, u32>(0x05A00000, 0x05BFFFFF);
    initializeHandlers<writeScsp, u8, u16, u32>(0x25A00000, 0x25BFFFFF);

    // VDP1 RAM access
    initializeHandlers<readVdp1Ram, u8, u16, u32>(0x05C00000, 0x05C7FFFF);
    initializeHandlers<readVdp1Ram, u8, u16, u32>(0x25C00000, 0x25C7FFFF);

    initializeHandlers<writeVdp1Ram, u8, u16, u32>(0x05C00000, 0x05C7FFFF);
    initializeHandlers<writeVdp1Ram, u8, u16, u32>(0x25C00000, 0x25C7FFFF);

    // VDP1 framebuffer access
    initializeHandlers<readVdp1Framebuffer, u8, u16, u32>(0x05C80000, 0x05CFFFFF);
    initializeHandlers<readVdp1Framebuffer, u8, u16, u32>(0x25C80000, 0x25CFFFFF);

    initializeHandlers<writeVdp1Framebuffer, u8, u16, u32>(0x05C80000, 0x05CFFFFF);
    initializeHandlers<writeVdp1Framebuffer, u8, u16, u32>(0x25C80000, 0x25CFFFFF);

    //// VDP1 Registers access
    initializeHandlers<readVdp1Registers, u8, u16, u32>(0x05D00000, 0x05D7FFFF);
    initializeHandlers<readVdp1Registers, u8, u16, u32>(0x25D00000, 0x25D7FFFF);

    initializeHandlers<writeVdp1Registers, u8, u16, u32>(0x05D00000, 0x05D7FFFF);
    initializeHandlers<writeVdp1Registers, u8, u16, u32>(0x25D00000, 0x25D7FFFF);

    // VDP2 VRAM access
    initializeHandlers<readVdp2Vram, u8, u16, u32>(0x05E00000, 0x05EFFFFF);
    initializeHandlers<readVdp2Vram, u8, u16, u32>(0x25E00000, 0x25EFFFFF);

    initializeHandlers<writeVdp2Vram, u8, u16, u32>(0x05E00000, 0x05EFFFFF);
    initializeHandlers<writeVdp2Vram, u8, u16, u32>(0x25E00000, 0x25EFFFFF);

    // VDP2 CRAM access
    initializeHandlers<readVdp2Cram, u8, u16, u32>(0x05F00000, 0x05F7FFFF);
    initializeHandlers<readVdp2Cram, u8, u16, u32>(0x25F00000, 0x25F7FFFF);

    initializeHandlers<writeVdp2Cram, u8, u16, u32>(0x05F00000, 0x05F7FFFF);
    initializeHandlers<writeVdp2Cram, u8, u16, u32>(0x25F00000, 0x25F7FFFF);

    // VDP2 Registers access
    initializeHandlers<readVdp2Registers, u8, u16, u32>(0x05F80000, 0x05FBFFFF);
    initializeHandlers<readVdp2Registers, u8, u16, u32>(0x25F80000, 0x25FBFFFF);

    initializeHandlers<writeVdp2Registers, u8, u16, u32>(0x05F80000, 0x05FBFFFF);
    initializeHandlers<writeVdp2Registers, u8, u16, u32>(0x25F80000, 0x25FBFFFF);

    // SCU access
    initializeHandlers<readScu, u8, u16, u32>(0x05FE0000, 0x05FEFFFF);
    initializeHandlers<readScu, u8, u16, u32>(0x25FE0000, 0x25FEFFFF);

    initializeHandlers<writeScu, u8, u16, u32>(0x05FE0000, 0x05FEFFFF);
    initializeHandlers<writeScu, u8, u16, u32>(0x25FE0000, 0x25FEFFFF);

    // Workram high access
    initializeHandlers<readWorkramHigh, u8, u16, u32>(0x06000000, 0x07FFFFFF);
    initializeHandlers<readWorkramHigh, u8, u16, u32>(0x26000000, 0x27FFFFFF);

    initializeHandlers<writeWorkramHigh, u8, u16, u32>(0x06000000, 0x07FFFFFF);
    initializeHandlers<writeWorkramHigh, u8, u16, u32>(0x26000000, 0x27FFFFFF);

    // Master FRT access
    initializeHandlers<writeMasterSh2Frt, u8, u16, u32>(0x01000000, 0x017FFFFF);
    initializeHandlers<writeMasterSh2Frt, u8, u16, u32>(0x21000000, 0x217FFFFF);

    // Slave FRT access
    initializeHandlers<writeSlaveSh2Frt, u8, u16, u32>(0x01800000, 0x01FFFFFF);
    initializeHandlers<writeSlaveSh2Frt, u8, u16, u32>(0x21800000, 0x21FFFFFF);

    // SH2 register access
    initializeHandlers<readSh2Registers, u8, u16, u32>(0xFFFFFE00, 0xFFFFFFFF);
    initializeHandlers<writeSh2Registers, u8, u16, u32>(0xFFFFFE00, 0xFFFFFFFF);

    // Cache addresses access
    initializeHandlers<readCacheAddresses, u8, u16, u32>(0x60000000, 0x6FFFFFFF);
    initializeHandlers<writeCacheAddresses, u8, u16, u32>(0x60000000, 0x6FFFFFFF);

    // Cache data access
    initializeHandlers<readCacheData, u8, u16, u32>(0x80000000, 0x8FFFFFFF);
    initializeHandlers<readCacheData, u8, u16, u32>(0xC0000000, 0xCFFFFFFF);

    initializeHandlers<writeCacheData, u8, u16, u32>(0x80000000, 0x8FFFFFFF);
    initializeHandlers<writeCacheData, u8, u16, u32>(0xC0000000, 0xCFFFFFFF);
}

u32 Memory::readStvProtection(const u32 addr, u32 data) const {

    if (this->isStvProtectionEnabled()) {
        switch (data) {
            // Astra Superstars
            case 0x01230000:
                stv_protection_offset += 4;
                data = rawRead<u32>(this->cart_, stv_protection_offset);
                break;
                // Final Fight Revenge
            case 0x10da0000:
                stv_protection_offset += 4;
                data = rawRead<u32>(this->cart_, stv_protection_offset);
                break;
            case 0x10d70000:
                stv_protection_offset += 4;
                data = rawRead<u32>(this->cart_, stv_protection_offset);
                break;
                // Steep Slope Sliders
            case 0x2c5b0000:
            case 0x47F10000:
            case 0xfcda0000:
            case 0xb5e60000:
            case 0x392c0000:
            case 0x77c30000:
            case 0x8a620000:
                stv_protection_offset += 4;
                data = rawRead<u32>(this->cart_, stv_protection_offset);
                break;
                // Radiant Silvergun
            case 0x77770000:
                data = 0;
                break;
                // Elan Doreé
            case 0xff7f0000:
            case 0xf9ff0000:
            case 0xffbf0000:
                data = 0x02002000;
                break;
        }
        core::Log::debug("memory", "ST-V protection read index: {}, value: {}", stv_protection_offset, data);
    }
    else {
        stv_protection_offset = 0;
    }

    return data;
}

void Memory::writeStvProtection(const u32 addr, u32 data) {
    u32 relative_addr = calculateRelativeCartAddress(stv_protection_register_address);
    const u32 index = rawRead<u32>(this->cart_, relative_addr);
    switch (index) {
        // Astra Superstars
        case 0x01230000:
            this->stv_protection_offset_ = (0x0400000) - 4;
            break;
            // Final Fight Revenge
        case 0x10d70000:
        case 0x10da0000:
            this->stv_protection_offset_ = (0x02B994) - 4;
            break;
            // Streep Slope Sliders
        case 0x2c5b0000:
            this->stv_protection_offset_ = (0x145ffac) - 4;
            break;
        case 0x47F10000:
            this->stv_protection_offset_ = (0x145ffac + 0xbaf0) - 4;
            break;
        case 0xfcda0000:
            this->stv_protection_offset_ = (0x145ffac + 0x12fd0) - 4;
            break;
        case 0xb5e60000:
            this->stv_protection_offset_ = (0x145ffac + 0x1a4c4) - 4;
            break;
        case 0x392c0000:
            this->stv_protection_offset_ = (0x145ffac + 0x219b0) - 4;
            break;
        case 0x77c30000:
            this->stv_protection_offset_ = (0x145ffac + 0x28ea0) - 4;
            break;
        case 0x8a620000:
            this->stv_protection_offset_ = (0x145ffac + 0x30380) - 4;
            break;
            // Radiant Silvergun
        case 0x77770000:
            break;
    }
    core::Log::debug("memory", fmt::format(core::tr("ST-V offset start: {}"), this->stv_protection_offset_));
}

bool Memory::isStvProtectionEnabled() const {
    u32 relative_addr = calculateRelativeCartAddress(stv_protection_enabled);
    return cart_[relative_addr] == 0x1;
}

void Memory::sendFrtInterrupt(sh2::Sh2Type t) const {
    switch (t) {
        case sh2::Sh2Type::master: masterSh2()->sendInterrupt(interrupt_source::frt_input_capture); break;
        case sh2::Sh2Type::slave:  slaveSh2()->sendInterrupt(interrupt_source::frt_input_capture);  break;
    }
}

Config* Memory::config() const { 
    return emulator_context_->config(); 
};

sh2::Sh2* Memory::masterSh2() const {
    return emulator_context_->masterSh2();
};

sh2::Sh2* Memory::slaveSh2() const {
    return emulator_context_->slaveSh2();
};

Scu* Memory::scu() const {
    return emulator_context_->scu();
};

void mirrorData(u8* data, const u32 size, const u8 times_mirrored, const Rom_load rom_load) {
    if (times_mirrored > 0) {
        u32 multiple{};
        switch (rom_load) {
            case Rom_load::not_interleaved: multiple = 1; break;
            case Rom_load::even_interleaved:multiple = 2; break;
            case Rom_load::odd_interleaved: multiple = 2; break;
        }
        for (u8 i = 1; i <= times_mirrored; ++i) {
            std::copy(data, data + size * multiple - 1, data + (i*size*multiple));
        }
    }
}

std::vector<std::string> listStvConfigurationFiles() {
    auto full_path = fs::current_path() / "stv";
    std::vector<std::string> files;
    for (auto& p : fs::directory_iterator(full_path)) {
        if ((p.path().extension() == ".cfg") && (p.path().filename() != "dummy.cfg")) {
            files.push_back(p.path().filename().string());
        }
    }
    return files;
}

inline bool isMasterSh2InOperation(const Memory& m) {
    return (m.sh2_in_operation_ == sh2::Sh2Type::master);
}

void Memory::initialize() {
    sh2_in_operation_ = sh2::Sh2Type::unknown;

    initializeHandlers();
}

}
}