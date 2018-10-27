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
#include "../lib/libzippp/libzippp.h"
#include "config.h"
#include "locale.h"
#include "log.h"
#include "memory.h"

namespace lzpp = libzippp;
namespace fs = std::filesystem;

namespace saturnin {
namespace core {

bool Memory::loadRom(const std::string& zip_name,
                    const std::string&  file_name,
                    uint8_t*	   destination,
                    const uint32_t size,
                    const Rom_load rom_load,
                    const uint8_t  times_mirrored,
                    const Rom_type rom_type) {

    std::string zip = zip_name + ".zip";
    fs::path rom_path{ config_->readValue(Access_keys::config_roms_stv).c_str() };
    rom_path /= zip;
    //rom_path += "\\" + zip_name + ".zip";

    lzpp::ZipArchive zf(rom_path.string());
    if (zf.open(lzpp::ZipArchive::READ_ONLY)) {
        if (zf.hasEntry(file_name, false, false)) {
            lzpp::ZipEntry entry = zf.getEntry(file_name, false, false);
            std::unique_ptr<uint8_t[]> data(static_cast<uint8_t*>(entry.readAsBinary()));

            switch (rom_type) {
                case Rom_type::bios: {
                    uint32_t counter = size / 4;
                    // Needs byteswapping
                    for (uint32_t i = 0; i < counter; ++i) {
                        destination[i * 4 + 1] = data[i * 4 + 0];
                        destination[i * 4 + 0] = data[i * 4 + 1];
                        destination[i * 4 + 3] = data[i * 4 + 2];
                        destination[i * 4 + 2] = data[i * 4 + 3];
                    }
                    break;
                }
                case Rom_type::program:
                case Rom_type::graphic: {
                    const uint32_t stv_bios_region_address = 0x808;
                    uint32_t region_cart_address{};
                    switch (rom_load) {
                        case Rom_load::not_interleaved: {
                            const auto &src_begin = data.get();
                            std::move(src_begin, std::next(src_begin, size), destination);

                            region_cart_address = 0x40;
                            break;
                        }
                        case Rom_load::odd_interleaved: {  // Data is loaded on odd bytes only
                            for (uint32_t i = 0; i < size; ++i) {
                                destination[(i * 2 + 1)] = data[i];
                            }

                            region_cart_address = 0x81;
                            break;
                        }
                        case Rom_load::even_interleaved: { // Data is loaded on even bytes only
                            for (uint32_t i = 0; i < size; ++i) {
                                destination[i * 2] = data[i];
                            }

                            region_cart_address = 0x80;
                            break;
                        }
                    }

                    // bios region is forced for program roms
                    if (rom_type == Rom_type::program) this->cart[region_cart_address] = this->rom[stv_bios_region_address];

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

void Memory::loadBios(const Hardware_mode mode) {
    std::string bios_path{};
    switch (mode) {
        case Hardware_mode::saturn: bios_path = config_->readValue(Access_keys::config_bios_saturn).c_str(); break;
        case Hardware_mode::stv:    bios_path = config_->readValue(Access_keys::config_bios_stv).c_str(); break;
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
            case Hardware_mode::saturn: {
                std::move(str.begin(), str.end(), this->rom.data());
                break;
            }
            case Hardware_mode::stv: {
                // Needs byteswapping
                for (size_t i = 0; i < str.size(); i += 4) {
                    this->rom[i + 1] = str[i + 0];
                    this->rom[i + 0] = str[i + 1];
                    this->rom[i + 3] = str[i + 2];
                    this->rom[i + 2] = str[i + 3];
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

    const std::string game_name       = stv.readValue(core::Access_keys::stv_game_name);
    const std::string zip_name        = stv.readValue(core::Access_keys::stv_zip_name);
    const std::string parent_set      = stv.readValue(core::Access_keys::stv_parent_set);
    const std::string version         = stv.readValue(core::Access_keys::stv_version);
    const std::string release_date    = stv.readValue(core::Access_keys::stv_release_date);
    const std::string region          = stv.readValue(core::Access_keys::stv_region);
    const libconfig::Setting& files   = stv.readValue(core::Access_keys::stv_files);
    for (uint8_t i=0; i < files.getLength(); ++i) {
        const std::string rom_name      = files[i][0];
        const uint32_t    load_address  = files[i][1];
        const uint32_t    load_size     = files[i][2];
        const auto        rom_load      = Config::rom_load[files[i][3]];
        const uint32_t    times_mirrored= files[i][4];
        const auto        rom_type      = Config::rom_type[files[i][5]];
        if (!this->loadRom(zip_name,
                           rom_name,
                           &this->cart[load_address],
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
    const uint32_t program_rom_size = 0x400000;

    // ST-V data begins with 'SEGA' string.
    // If the first byte of the string is 'E', it means the program data has to be swapped
    if (this->cart[0] == 'E') {
        for (uint32_t i = 0; i < program_rom_size; i += 2) {
            std::swap(this->cart[i], this->cart[i + 1]);
        }
    }

    for (uint32_t i = program_rom_size; i < this->cart.size(); i += 2) {
        std::swap(this->cart[i], this->cart[i + 1]);
    }
}

void Memory::initializeHandlers() {
    initializeReadHandler<8>(0x00000000, 0xFFFFFFFF, &readDummy<8>);
    //initializeReadHandler<16>(0x00000000, 0xFFFFFFFF, readDummy<16>);
    //initializeReadHandler<32>(0x00000000, 0xFFFFFFFF, readDummy<32>);
}

void mirrorData(uint8_t* data, const uint32_t size, const uint8_t times_mirrored, const Rom_load rom_load) {
    if (times_mirrored > 0) {
        uint32_t multiple{};
        switch (rom_load) {
            case Rom_load::not_interleaved: multiple = 1; break;
            case Rom_load::even_interleaved:multiple = 2; break;
            case Rom_load::odd_interleaved: multiple = 2; break;
        }
        for (uint8_t i = 1; i <= times_mirrored; ++i) {
            std::copy(data, data + size * multiple - 1, data + (i*size*multiple));
        }
    }
}

std::vector<std::string> listStvConfigurationFiles() {
    auto full_path = fs::current_path() / "stv";
    std::vector<std::string> files;
    for (auto& p : fs::directory_iterator(full_path)) {
        if ((p.path().extension() == ".cfg") && (p.path().filename() != "dummy.cfg")){
              files.push_back(p.path().filename().string());
        }
    }
    return files;
}

// Handlers


}
}