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
    //bool Memory::loadRom(const string& zip_name,
    //                    const string& file_name,
    //                    uint8_t*	   destination,
    //                    const uint32_t size,
    //                    const Rom_load rom_load,
    //                    const uint8_t  times_mirrored,
    //                    const Rom_type rom_type) {
    //    string rom_path{ config_->readValue(Config_keys::roms_stv).c_str() };
    //    rom_path += "\\" + zip_name + ".zip";

    //    ZipArchive zf(rom_path);
    //    if (zf.open(ZipArchive::READ_ONLY)) {
    //        if (zf.hasEntry(file_name, false, false)) {
    //            ZipEntry entry = zf.getEntry(file_name, false, false);
    //            std::unique_ptr<uint8_t[]> data(static_cast<uint8_t*>(entry.readAsBinary()));
    //            //auto e = data[2];

    //            if (rom_type == Rom_type::bios) {
    //                uint32_t counter = size / 4;
    //                // filling the rom data
    //                // Needs byteswapping
    //                for (uint32_t i = 0; i < counter; ++i) {
    //                    destination[i * 4 + 1] = data[i * 4 + 0];
    //                    destination[i * 4 + 0] = data[i * 4 + 1];
    //                    destination[i * 4 + 3] = data[i * 4 + 2];
    //                    destination[i * 4 + 2] = data[i * 4 + 3];
    //                }
    //            }
    //            else {

    //            }
    //        }
    //        else {
    //            string str = fmt::format(tr("File '{0}' not found in zip file !"), file_name);
    //            Log::warning("memory", str);
    //        }
    //        

    //        zf.close();
    //    }
    //    
    //    return true;
    //}


    //bool Memory::StvLoadRomToMemory(const string &		zipname,
    //    const string & rom_name,
    //    uint8_t *	   destination,
    //    const uint32_t size,
    //    const Rom_load rom_load,
    //    const uint8_t  times_mirrored,
    //    const Rom_type rom_type)

        
    //    //HZIP zipFileHandle;

    //    //zipFileHandle = OpenZip((void*)zipname.c_str(), 0, ZIP_FILENAME);
    //    //if (!zipFileHandle) {
    //    //    std::cout << "Error opening zip file" << endl;
    //    //}
    //    //else {
    //    //    std::cout << "Zip file opened" << endl;

    //    //    ZIPENTRY ze;
    //    //    //int32_t index = 0;
    //    //    int index = 0;
    //    //    if (FindZipItem(zipFileHandle, romname.c_str(), true, &index, &ze) != ZR_OK) {
    //    //        MessageBox(NULL, "Couldn't find the file in the zip", NULL, MB_OK);
    //    //    }
    //    //    else {
    //            if (rom_type == Rom_type::bios) {
    //                //int8_t* content = new int8_t[size];
    //                //UnzipItem(zipFileHandle, index, (void *)content, size, ZIP_MEMORY);

    //                //uint32_t counter = size / 4;
    //                //// filling the rom data
    //                //// Needs byteswapping
    //                //for (uint32_t i = 0; i < counter; i++) {
    //                //    destination[i * 4 + 1] = content[i * 4 + 0];
    //                //    destination[i * 4 + 0] = content[i * 4 + 1];
    //                //    destination[i * 4 + 3] = content[i * 4 + 2];
    //                //    destination[i * 4 + 2] = content[i * 4 + 3];
    //                //}

    //                //delete[] content;

    //            }
    //            else {

    //                switch (rom_load) {
    //                case Rom_load::not_interleaved:
    //                    //// Data is loaded normally
    //                    //UnzipItem(zipFileHandle, index, (void *)destination, size, ZIP_MEMORY);

    //                    //if (romType == PROGRAM) {
    //                    //    // bios region is forced for program roms
    //                    //    pCart[0x40] = pRom[0x808];
    //                    //}
    //                    //// ROM was loaded, checking if it needs to be mirrored
    //                    //if (mirroringNumber != 0) {
    //                    //    // ROM has to be mirrored
    //                    //    for (uint8_t i = 1; i <= mirroringNumber; i++) {
    //                    //        std::copy(destination, destination + size - 1, destination + (i*size));
    //                    //    }
    //                    //}
    //                    break;
    //                case Rom_load::odd_interleaved:
    //                {
    //                    //// Data is loaded on odd bytes only
    //                    //int8_t* content = new int8_t[size * 2];
    //                    //UnzipItem(zipFileHandle, index, (void *)content, size * 2, ZIP_MEMORY);

    //                    //for (uint32_t i = 0; i < size; i++)
    //                    //{
    //                    //    destination[(i * 2 + 1)] = content[i];
    //                    //}
    //                    //delete[] content;

    //                    //if (romType == PROGRAM) {
    //                    //    // bios region is forced for program roms
    //                    //    //pCart[0x80/4].val8[1] = pRom[0x808];
    //                    //    pCart[0x81] = pRom[0x808];
    //                    //}

    //                    //// ROM was loaded, checking if it needs to be mirrored
    //                    //if (mirroringNumber != 0) {
    //                    //    // ROM has to be mirrored
    //                    //    for (uint8_t i = 1; i <= mirroringNumber; i++) {
    //                    //        std::copy(destination, destination + size * 2 - 1, destination + (i*size * 2));
    //                    //    }
    //                    //}
    //                }
    //                break;
    //                case Rom_load::even_interleaved:
    //                {
    //                    //// Data is loaded on even bytes only
    //                    //int8_t* content = new int8_t[size];
    //                    //UnzipItem(zipFileHandle, index, (void *)content, size, ZIP_MEMORY);

    //                    //for (uint32_t i = 0; i < size; i++)
    //                    //{
    //                    //    destination[i * 2] = content[i];
    //                    //}
    //                    //delete[] content;

    //                    //if (romType == PROGRAM) {
    //                    //    // bios region is forced for program roms
    //                    //    //pCart[0x80/4].val8[1] = pRom[0x808];
    //                    //    pCart[0x81] = pRom[0x808];
    //                    //}

    //                    //// ROM was loaded, checking if it needs to be mirrored
    //                    //if (mirroringNumber != 0) {
    //                    //    // ROM has to be mirrored
    //                    //    for (uint8_t i = 1; i <= mirroringNumber; i++) {
    //                    //        std::copy(destination, destination + size * 2 - 1, destination + (i*size * 2));
    //                    //    }
    //                    //}
    //                }
    //                break;
    //                }
    //            }
    //    //    }
    //    //}

    //    return false;
    //}

    void Memory::loadBios(const Hardware_mode mode) {
        string bios_path{};
        switch (mode) {
        case Hardware_mode::saturn:
            bios_path = config_->readValue(Config_keys::bios_saturn).c_str();
            break;
        case Hardware_mode::stv:
            bios_path = config_->readValue(Config_keys::bios_stv).c_str();
            break;
        default:
            Log::error("config", tr("Unknown hardware mode"));
            Log::error("config", tr("Exiting ..."));
            exit(EXIT_FAILURE);
            break;
        }

        ifstream input_file(bios_path, ios::binary);
        if (input_file) {
            stringstream buffer;
            buffer << input_file.rdbuf();
            input_file.close();

            size_t counter{ buffer.str().size() };
            string str = buffer.str();
            switch (mode) {
            case Hardware_mode::saturn:
                for (size_t i = 0; i<counter; i += 4) {
                    this->rom[i+0] = str[i+0];
                    this->rom[i+1] = str[i+1];
                    this->rom[i+2] = str[i+2];
                    this->rom[i+3] = str[i+3];
                }
                break;
            case Hardware_mode::stv:
                // Needs byteswapping
                for (size_t i = 0; i<counter; i += 4) {
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
}