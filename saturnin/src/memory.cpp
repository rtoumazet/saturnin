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
                    u8*	   destination,
                    const u32 size,
                    const Rom_load rom_load,
                    const u8  times_mirrored,
                    const Rom_type rom_type) {

    std::string zip = zip_name + ".zip";
    fs::path rom_path{ config_->readValue(Access_keys::config_roms_stv).c_str() };
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
    for (u8 i=0; i < files.getLength(); ++i) {
        const std::string rom_name      = files[i][0];
        const u32    load_address  = files[i][1];
        const u32    load_size     = files[i][2];
        const auto        rom_load      = Config::rom_load[files[i][3]];
        const u32    times_mirrored= files[i][4];
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
    const u32 program_rom_size = 0x400000;

    // ST-V data begins with 'SEGA' string.
    // If the first byte of the string is 'E', it means the program data has to be swapped
    if (this->cart[0] == 'E') {
        for (u32 i = 0; i < program_rom_size; i += 2) {
            std::swap(this->cart[i], this->cart[i + 1]);
        }
    }

    for (u32 i = program_rom_size; i < this->cart.size(); i += 2) {
        std::swap(this->cart[i], this->cart[i + 1]);
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
    initializeHandlers<readBackupRam, u8, u16, u32>(0x00200000, 0x002FFFFF);
    initializeHandlers<readBackupRam, u8, u16, u32>(0x20200000, 0x202FFFFF);
    
    initializeHandlers<writeBackupRam, u8, u16, u32>(0x00200000, 0x002FFFFF);
    initializeHandlers<writeBackupRam, u8, u16, u32>(0x20200000, 0x202FFFFF);

    //// STV I/O access
    //MapMemoryTableWriteByte(0x00400000, 0x004FFFFF, &STVIOWriteByte);
    //MapMemoryTableWriteWord(0x00400000, 0x004FFFFF, &STVIOWriteWord);
    //MapMemoryTableWriteLong(0x00400000, 0x004FFFFF, &STVIOWriteLong);
    //MapMemoryTableReadByte(0x00400000, 0x004FFFFF, &STVIOReadByte);
    //MapMemoryTableReadWord(0x00400000, 0x004FFFFF, &STVIOReadWord);
    //MapMemoryTableReadLong(0x00400000, 0x004FFFFF, &STVIOReadLong);

    //MapMemoryTableWriteByte(0x20400000, 0x204FFFFF, &STVIOWriteByte);
    //MapMemoryTableWriteWord(0x20400000, 0x204FFFFF, &STVIOWriteWord);
    //MapMemoryTableWriteLong(0x20400000, 0x204FFFFF, &STVIOWriteLong);
    //MapMemoryTableReadByte(0x20400000, 0x204FFFFF, &STVIOReadByte);
    //MapMemoryTableReadWord(0x20400000, 0x204FFFFF, &STVIOReadWord);
    //MapMemoryTableReadLong(0x20400000, 0x204FFFFF, &STVIOReadLong);

    //// Cart access
    //MapMemoryTableWriteByte(0x02000000, 0x04FFFFFF, &CartWriteByte);
    //MapMemoryTableWriteWord(0x02000000, 0x04FFFFFF, &CartWriteWord);
    //MapMemoryTableWriteLong(0x02000000, 0x04FFFFFF, &CartWriteLong);
    //MapMemoryTableReadByte(0x02000000, 0x04FFFFFF, &CartReadByte);
    //MapMemoryTableReadWord(0x02000000, 0x04FFFFFF, &CartReadWord);
    //MapMemoryTableReadLong(0x02000000, 0x04FFFFFF, &CartReadLong);

    //MapMemoryTableWriteByte(0x22000000, 0x24FFFFFF, &CartWriteByte);
    //MapMemoryTableWriteWord(0x22000000, 0x24FFFFFF, &CartWriteWord);
    //MapMemoryTableWriteLong(0x22000000, 0x24FFFFFF, &CartWriteLong);
    //MapMemoryTableReadByte(0x22000000, 0x24FFFFFF, &CartReadByte);
    //MapMemoryTableReadWord(0x22000000, 0x24FFFFFF, &CartReadWord);
    //MapMemoryTableReadLong(0x22000000, 0x24FFFFFF, &CartReadLong);

    //// CdBlock access
    //MapMemoryTableWriteByte(0x05800000, 0x058FFFFF, &CdBlockWriteByte);
    //MapMemoryTableWriteWord(0x05800000, 0x058FFFFF, &CdBlockWriteWord);
    //MapMemoryTableWriteLong(0x05800000, 0x058FFFFF, &CdBlockWriteLong);
    //MapMemoryTableReadByte(0x05800000, 0x058FFFFF, &CdBlockReadByte);
    //MapMemoryTableReadWord(0x05800000, 0x058FFFFF, &CdBlockReadWord);
    //MapMemoryTableReadLong(0x05800000, 0x058FFFFF, &CdBlockReadLong);

    //MapMemoryTableWriteByte(0x25800000, 0x258FFFFF, &CdBlockWriteByte);
    //MapMemoryTableWriteWord(0x25800000, 0x258FFFFF, &CdBlockWriteWord);
    //MapMemoryTableWriteLong(0x25800000, 0x258FFFFF, &CdBlockWriteLong);
    //MapMemoryTableReadByte(0x25800000, 0x258FFFFF, &CdBlockReadByte);
    //MapMemoryTableReadWord(0x25800000, 0x258FFFFF, &CdBlockReadWord);
    //MapMemoryTableReadLong(0x25800000, 0x258FFFFF, &CdBlockReadLong);

    //// SCSP access
    //MapMemoryTableWriteByte(0x05A00000, 0x05BFFFFF, &SCSPWriteByte);
    //MapMemoryTableWriteWord(0x05A00000, 0x05BFFFFF, &SCSPWriteWord);
    //MapMemoryTableWriteLong(0x05A00000, 0x05BFFFFF, &SCSPWriteLong);
    //MapMemoryTableReadByte(0x05A00000, 0x05BFFFFF, &SCSPReadByte);
    //MapMemoryTableReadWord(0x05A00000, 0x05BFFFFF, &SCSPReadWord);
    //MapMemoryTableReadLong(0x05A00000, 0x05BFFFFF, &SCSPReadLong);

    //MapMemoryTableWriteByte(0x25A00000, 0x25BFFFFF, &SCSPWriteByte);
    //MapMemoryTableWriteWord(0x25A00000, 0x25BFFFFF, &SCSPWriteWord);
    //MapMemoryTableWriteLong(0x25A00000, 0x25BFFFFF, &SCSPWriteLong);
    //MapMemoryTableReadByte(0x25A00000, 0x25BFFFFF, &SCSPReadByte);
    //MapMemoryTableReadWord(0x25A00000, 0x25BFFFFF, &SCSPReadWord);
    //MapMemoryTableReadLong(0x25A00000, 0x25BFFFFF, &SCSPReadLong);

    //// VDP1 RAM access
    //MapMemoryTableWriteByte(0x05C00000, 0x05C7FFFF, &VDP1RAMWriteByte);
    //MapMemoryTableWriteWord(0x05C00000, 0x05C7FFFF, &VDP1RAMWriteWord);
    //MapMemoryTableWriteLong(0x05C00000, 0x05C7FFFF, &VDP1RAMWriteLong);
    //MapMemoryTableReadByte(0x05C00000, 0x05C7FFFF, &VDP1RAMReadByte);
    //MapMemoryTableReadWord(0x05C00000, 0x05C7FFFF, &VDP1RAMReadWord);
    //MapMemoryTableReadLong(0x05C00000, 0x05C7FFFF, &VDP1RAMReadLong);

    //MapMemoryTableWriteByte(0x25C00000, 0x25C7FFFF, &VDP1RAMWriteByte);
    //MapMemoryTableWriteWord(0x25C00000, 0x25C7FFFF, &VDP1RAMWriteWord);
    //MapMemoryTableWriteLong(0x25C00000, 0x25C7FFFF, &VDP1RAMWriteLong);
    //MapMemoryTableReadByte(0x25C00000, 0x25C7FFFF, &VDP1RAMReadByte);
    //MapMemoryTableReadWord(0x25C00000, 0x25C7FFFF, &VDP1RAMReadWord);
    //MapMemoryTableReadLong(0x25C00000, 0x25C7FFFF, &VDP1RAMReadLong);

    //// VDP1 framebuffer access
    //MapMemoryTableWriteByte(0x05C80000, 0x05CFFFFF, &VDP1FBWriteByte);
    //MapMemoryTableWriteWord(0x05C80000, 0x05CFFFFF, &VDP1FBWriteWord);
    //MapMemoryTableWriteLong(0x05C80000, 0x05CFFFFF, &VDP1FBWriteLong);
    //MapMemoryTableReadByte(0x05C80000, 0x05CFFFFF, &VDP1FBReadByte);
    //MapMemoryTableReadWord(0x05C80000, 0x05CFFFFF, &VDP1FBReadWord);
    //MapMemoryTableReadLong(0x05C80000, 0x05CFFFFF, &VDP1FBReadLong);

    //MapMemoryTableWriteByte(0x25C80000, 0x25CFFFFF, &VDP1FBWriteByte);
    //MapMemoryTableWriteWord(0x25C80000, 0x25CFFFFF, &VDP1FBWriteWord);
    //MapMemoryTableWriteLong(0x25C80000, 0x25CFFFFF, &VDP1FBWriteLong);
    //MapMemoryTableReadByte(0x25C80000, 0x25CFFFFF, &VDP1FBReadByte);
    //MapMemoryTableReadWord(0x25C80000, 0x25CFFFFF, &VDP1FBReadWord);
    //MapMemoryTableReadLong(0x25C80000, 0x25CFFFFF, &VDP1FBReadLong);

    //// VDP1 Registers access
    //MapMemoryTableWriteByte(0x05D00000, 0x05D7FFFF, &VDP1RegWriteByte);
    //MapMemoryTableWriteWord(0x05D00000, 0x05D7FFFF, &VDP1RegWriteWord);
    //MapMemoryTableWriteLong(0x05D00000, 0x05D7FFFF, &VDP1RegWriteLong);
    //MapMemoryTableReadByte(0x05D00000, 0x05D7FFFF, &VDP1RegReadByte);
    //MapMemoryTableReadWord(0x05D00000, 0x05D7FFFF, &VDP1RegReadWord);
    //MapMemoryTableReadLong(0x05D00000, 0x05D7FFFF, &VDP1RegReadLong);

    //MapMemoryTableWriteByte(0x25D00000, 0x25D7FFFF, &VDP1RegWriteByte);
    //MapMemoryTableWriteWord(0x25D00000, 0x25D7FFFF, &VDP1RegWriteWord);
    //MapMemoryTableWriteLong(0x25D00000, 0x25D7FFFF, &VDP1RegWriteLong);
    //MapMemoryTableReadByte(0x25D00000, 0x25D7FFFF, &VDP1RegReadByte);
    //MapMemoryTableReadWord(0x25D00000, 0x25D7FFFF, &VDP1RegReadWord);
    //MapMemoryTableReadLong(0x25D00000, 0x25D7FFFF, &VDP1RegReadLong);

    //// VDP2 VRAM access
    //MapMemoryTableWriteByte(0x05E00000, 0x05EFFFFF, &VDP2VRAMWriteByte);
    //MapMemoryTableWriteWord(0x05E00000, 0x05EFFFFF, &VDP2VRAMWriteWord);
    //MapMemoryTableWriteLong(0x05E00000, 0x05EFFFFF, &VDP2VRAMWriteLong);
    //MapMemoryTableReadByte(0x05E00000, 0x05EFFFFF, &VDP2VRAMReadByte);
    //MapMemoryTableReadWord(0x05E00000, 0x05EFFFFF, &VDP2VRAMReadWord);
    //MapMemoryTableReadLong(0x05E00000, 0x05EFFFFF, &VDP2VRAMReadLong);

    //MapMemoryTableWriteByte(0x25E00000, 0x25EFFFFF, &VDP2VRAMWriteByte);
    //MapMemoryTableWriteWord(0x25E00000, 0x25EFFFFF, &VDP2VRAMWriteWord);
    //MapMemoryTableWriteLong(0x25E00000, 0x25EFFFFF, &VDP2VRAMWriteLong);
    //MapMemoryTableReadByte(0x25E00000, 0x25EFFFFF, &VDP2VRAMReadByte);
    //MapMemoryTableReadWord(0x25E00000, 0x25EFFFFF, &VDP2VRAMReadWord);
    //MapMemoryTableReadLong(0x25E00000, 0x25EFFFFF, &VDP2VRAMReadLong);

    //// VDP2 CRAM access
    //MapMemoryTableWriteByte(0x05F00000, 0x05F7FFFF, &VDP2CRAMWriteByte);
    //MapMemoryTableWriteWord(0x05F00000, 0x05F7FFFF, &VDP2CRAMWriteWord);
    //MapMemoryTableWriteLong(0x05F00000, 0x05F7FFFF, &VDP2CRAMWriteLong);
    //MapMemoryTableReadByte(0x05F00000, 0x05F7FFFF, &VDP2CRAMReadByte);
    //MapMemoryTableReadWord(0x05F00000, 0x05F7FFFF, &VDP2CRAMReadWord);
    //MapMemoryTableReadLong(0x05F00000, 0x05F7FFFF, &VDP2CRAMReadLong);

    //MapMemoryTableWriteByte(0x25F00000, 0x25F7FFFF, &VDP2CRAMWriteByte);
    //MapMemoryTableWriteWord(0x25F00000, 0x25F7FFFF, &VDP2CRAMWriteWord);
    //MapMemoryTableWriteLong(0x25F00000, 0x25F7FFFF, &VDP2CRAMWriteLong);
    //MapMemoryTableReadByte(0x25F00000, 0x25F7FFFF, &VDP2CRAMReadByte);
    //MapMemoryTableReadWord(0x25F00000, 0x25F7FFFF, &VDP2CRAMReadWord);
    //MapMemoryTableReadLong(0x25F00000, 0x25F7FFFF, &VDP2CRAMReadLong);

    //// VDP2 Registers access
    //MapMemoryTableWriteByte(0x05F80000, 0x05FBFFFF, &VDP2RegWriteByte);
    //MapMemoryTableWriteWord(0x05F80000, 0x05FBFFFF, &VDP2RegWriteWord);
    //MapMemoryTableWriteLong(0x05F80000, 0x05FBFFFF, &VDP2RegWriteLong);
    //MapMemoryTableReadByte(0x05F80000, 0x05FBFFFF, &VDP2RegReadByte);
    //MapMemoryTableReadWord(0x05F80000, 0x05FBFFFF, &VDP2RegReadWord);
    //MapMemoryTableReadLong(0x05F80000, 0x05FBFFFF, &VDP2RegReadLong);

    //MapMemoryTableWriteByte(0x25F80000, 0x25FBFFFF, &VDP2RegWriteByte);
    //MapMemoryTableWriteWord(0x25F80000, 0x25FBFFFF, &VDP2RegWriteWord);
    //MapMemoryTableWriteLong(0x25F80000, 0x25FBFFFF, &VDP2RegWriteLong);
    //MapMemoryTableReadByte(0x25F80000, 0x25FBFFFF, &VDP2RegReadByte);
    //MapMemoryTableReadWord(0x25F80000, 0x25FBFFFF, &VDP2RegReadWord);
    //MapMemoryTableReadLong(0x25F80000, 0x25FBFFFF, &VDP2RegReadLong);

    //// SCU access
    //MapMemoryTableWriteByte(0x05FE0000, 0x05FEFFFF, &SCUWriteByte);
    //MapMemoryTableWriteWord(0x05FE0000, 0x05FEFFFF, &SCUWriteWord);
    //MapMemoryTableWriteLong(0x05FE0000, 0x05FEFFFF, &SCUWriteLong);
    //MapMemoryTableReadByte(0x05FE0000, 0x05FEFFFF, &SCUReadByte);
    //MapMemoryTableReadWord(0x05FE0000, 0x05FEFFFF, &SCUReadWord);
    //MapMemoryTableReadLong(0x05FE0000, 0x05FEFFFF, &SCUReadLong);

    //MapMemoryTableWriteByte(0x25FE0000, 0x25FEFFFF, &SCUWriteByte);
    //MapMemoryTableWriteWord(0x25FE0000, 0x25FEFFFF, &SCUWriteWord);
    //MapMemoryTableWriteLong(0x25FE0000, 0x25FEFFFF, &SCUWriteLong);
    //MapMemoryTableReadByte(0x25FE0000, 0x25FEFFFF, &SCUReadByte);
    //MapMemoryTableReadWord(0x25FE0000, 0x25FEFFFF, &SCUReadWord);
    //MapMemoryTableReadLong(0x25FE0000, 0x25FEFFFF, &SCUReadLong);

    //// Workram high access
    //MapMemoryTableWriteByte(0x06000000, 0x07FFFFFF, &RAMHWriteByte);
    //MapMemoryTableWriteWord(0x06000000, 0x07FFFFFF, &RAMHWriteWord);
    //MapMemoryTableWriteLong(0x06000000, 0x07FFFFFF, &RAMHWriteLong);
    //MapMemoryTableReadByte(0x06000000, 0x07FFFFFF, &RAMHReadByte);
    //MapMemoryTableReadWord(0x06000000, 0x07FFFFFF, &RAMHReadWord);
    //MapMemoryTableReadLong(0x06000000, 0x07FFFFFF, &RAMHReadLong);

    //MapMemoryTableWriteByte(0x26000000, 0x27FFFFFF, &RAMHWriteByte);
    //MapMemoryTableWriteWord(0x26000000, 0x27FFFFFF, &RAMHWriteWord);
    //MapMemoryTableWriteLong(0x26000000, 0x27FFFFFF, &RAMHWriteLong);
    //MapMemoryTableReadByte(0x26000000, 0x27FFFFFF, &RAMHReadByte);
    //MapMemoryTableReadWord(0x26000000, 0x27FFFFFF, &RAMHReadWord);
    //MapMemoryTableReadLong(0x26000000, 0x27FFFFFF, &RAMHReadLong);

    //// Master FRT access
    //MapMemoryTableWriteByte(0x01000000, 0x017FFFFF, &MasterFRTWriteByte);
    //MapMemoryTableWriteWord(0x01000000, 0x017FFFFF, &MasterFRTWriteWord);
    //MapMemoryTableWriteLong(0x01000000, 0x017FFFFF, &MasterFRTWriteLong);

    //MapMemoryTableWriteByte(0x21000000, 0x217FFFFF, &MasterFRTWriteByte);
    //MapMemoryTableWriteWord(0x21000000, 0x217FFFFF, &MasterFRTWriteWord);
    //MapMemoryTableWriteLong(0x21000000, 0x217FFFFF, &MasterFRTWriteLong);

    //// Slave FRT access
    //MapMemoryTableWriteByte(0x01800000, 0x01FFFFFF, &SlaveFRTWriteByte);
    //MapMemoryTableWriteWord(0x01800000, 0x01FFFFFF, &SlaveFRTWriteWord);
    //MapMemoryTableWriteLong(0x01800000, 0x01FFFFFF, &SlaveFRTWriteLong);

    //MapMemoryTableWriteByte(0x21800000, 0x21FFFFFF, &SlaveFRTWriteByte);
    //MapMemoryTableWriteWord(0x21800000, 0x21FFFFFF, &SlaveFRTWriteWord);
    //MapMemoryTableWriteLong(0x21800000, 0x21FFFFFF, &SlaveFRTWriteLong);

    //// SH2 register access
    //MapMemoryTableWriteByte(0xFFFFFE00, 0xFFFFFFFF, &SH2WriteByte);
    //MapMemoryTableWriteWord(0xFFFFFE00, 0xFFFFFFFF, &SH2WriteWord);
    //MapMemoryTableWriteLong(0xFFFFFE00, 0xFFFFFFFF, &SH2WriteLong);
    //MapMemoryTableReadByte(0xFFFFFE00, 0xFFFFFFFF, &SH2ReadByte);
    //MapMemoryTableReadWord(0xFFFFFE00, 0xFFFFFFFF, &SH2ReadWord);
    //MapMemoryTableReadLong(0xFFFFFE00, 0xFFFFFFFF, &SH2ReadLong);

    //// Cache addresses access
    //MapMemoryTableWriteByte(0x60000000, 0x6FFFFFFF, &CacheAddressesWriteByte);
    //MapMemoryTableWriteWord(0x60000000, 0x6FFFFFFF, &CacheAddressesWriteWord);
    //MapMemoryTableWriteLong(0x60000000, 0x6FFFFFFF, &CacheAddressesWriteLong);
    //MapMemoryTableReadByte(0x60000000, 0x6FFFFFFF, &CacheAddressesReadByte);
    //MapMemoryTableReadWord(0x60000000, 0x6FFFFFFF, &CacheAddressesReadWord);
    //MapMemoryTableReadLong(0x60000000, 0x6FFFFFFF, &CacheAddressesReadLong);

    //// Cache data access
    //MapMemoryTableWriteByte(0xC0000000, 0xCFFFFFFF, &CacheDataWriteByte);
    //MapMemoryTableWriteWord(0xC0000000, 0xCFFFFFFF, &CacheDataWriteWord);
    //MapMemoryTableWriteLong(0xC0000000, 0xCFFFFFFF, &CacheDataWriteLong);
    //MapMemoryTableReadByte(0xC0000000, 0xCFFFFFFF, &CacheDataReadByte);
    //MapMemoryTableReadWord(0xC0000000, 0xCFFFFFFF, &CacheDataReadWord);
    //MapMemoryTableReadLong(0xC0000000, 0xCFFFFFFF, &CacheDataReadLong);

    //MapMemoryTableWriteByte(0x80000000, 0x8FFFFFFF, &CacheDataWriteByte);
    //MapMemoryTableWriteWord(0x80000000, 0x8FFFFFFF, &CacheDataWriteWord);
    //MapMemoryTableWriteLong(0x80000000, 0x8FFFFFFF, &CacheDataWriteLong);
    //MapMemoryTableReadByte(0x80000000, 0x8FFFFFFF, &CacheDataReadByte);
    //MapMemoryTableReadWord(0x80000000, 0x8FFFFFFF, &CacheDataReadWord);
    //MapMemoryTableReadLong(0x80000000, 0x8FFFFFFF, &CacheDataReadLong);
}

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
        if ((p.path().extension() == ".cfg") && (p.path().filename() != "dummy.cfg")){
              files.push_back(p.path().filename().string());
        }
    }
    return files;
}

// Handlers


}
}