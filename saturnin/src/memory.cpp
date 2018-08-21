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

using namespace libzippp;
using namespace std;


namespace saturnin {
namespace core {

bool Memory::loadRom(const string& zip_name,
                    const string&  file_name,
                    uint8_t*	   destination,
                    const uint32_t size,
                    const Rom_load rom_load,
                    const uint8_t  times_mirrored,
                    const Rom_type rom_type) {
    string rom_path{ config_->readValue(Config_keys::roms_stv).c_str() };
    rom_path += "\\" + zip_name + ".zip";

    ZipArchive zf(rom_path);
    if (zf.open(ZipArchive::READ_ONLY)) {
        if (zf.hasEntry(file_name, false, false)) {
            ZipEntry entry = zf.getEntry(file_name, false, false);
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
            string str = fmt::format(tr("File '{0}' not found in zip file !"), file_name);
            Log::warning("memory", str);
            return false;
        }
        zf.close();
    }
    else {
        string str = fmt::format(tr("Zip file '{0}' not found !"), rom_path);
        Log::warning("memory", str);
        return false;
    }
    return true;
}

void Memory::loadBios(const Hardware_mode mode) {
    string bios_path{};
    switch (mode) {
        case Hardware_mode::saturn: bios_path = config_->readValue(Config_keys::bios_saturn).c_str(); break;
        case Hardware_mode::stv:    bios_path = config_->readValue(Config_keys::bios_stv).c_str(); break;
        default: {
            Log::error("config", tr("Unknown hardware mode"));
            Log::error("config", tr("Exiting ..."));
            exit(EXIT_FAILURE);
            break;
        }
    }

    ifstream input_file(bios_path, ios::binary);
    if (input_file) {
        stringstream buffer;
        buffer << input_file.rdbuf();
        input_file.close();

        string str = buffer.str();
            
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

}
}