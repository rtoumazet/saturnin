// 
// memory.h
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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file	memory.h
///
/// \brief	Declares the memory class and the memory handlers. 
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <array> // array
#include <filesystem> // filesystem
#include <memory> // shared_pointer
#include <vector> // vector

#include "emulator_defs.h"
#include "emulator_enums.h"
#include "config.h"

namespace saturnin {
namespace core {

const u32 memory_handler_size = 0x10000;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum	Rom_type
///
/// \brief	ROM type. 
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Rom_type {
    program, ///< Program ROM.
    graphic, ///< Graphic ROM.
    bios	 ///< Bios.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum	Rom_load
///
/// \brief	Way ROM data is loaded. 
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Rom_load {
    not_interleaved, ///< Data loaded sequentially. 
    odd_interleaved, ///< Data loaded on odd bytes. 
    even_interleaved ///< Data loaded on even bytes.
};

class Config; 

//template<typename Tize>
//using SizedUInt = std::conditional_t<Size == 8, u8,
//    std::conditional_t<Size == 16, uint16_t,
//    std::conditional_t<Size == 32, u32, void>>>;


////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  Memory
///
/// \brief  Encapsulates Saturn memory map and related methods.
///
/// \author Runik
/// \date   07/06/2018
////////////////////////////////////////////////////////////////////////////////////////////////////

class Memory {
public:
        
    //@{
    // Constructors / Destructors
    Memory()                           = delete;
    Memory(std::shared_ptr<Config> config) : config_(config) {
        initializeHandlers();
    };
    Memory(const Memory&)              = delete;
    Memory(Memory&&)                   = delete;
    Memory& operator=(const Memory&) & = delete;
    Memory& operator=(Memory&&) &      = delete;
    ~Memory()                          = default;
    //@}

    /// \name Memory handlers functions typedefs
    /// std::function is not used here, as it implies a huge performance hit during execution
    //@{
    template <typename R, typename ...ARGS> using function = R(*)(ARGS...);
    template<typename T>        using ReadType = function<T, const Memory&, const u32>;
    template<typename T>        using WriteType = function<void, Memory&, const u32, const T>;
    template<class ReadType>    using ReadHandlerType = std::array<ReadType, memory_handler_size>;
    template<class WriteType>   using WriteHandlerType = std::array<WriteType, memory_handler_size>;
    template<typename T>        using ReadHandler = ReadHandlerType<ReadType<T>>;
    template<typename T>        using WriteHandler = WriteHandlerType<WriteType<T>>;
    //@}

    /// \name Saturn memory map definition.
    ///

    std::array <u8, 0x100000>  workram_low;      ///< low workram (1MB).
    std::array <u8, 0x100000>  workram_high;     ///< high workram (1MB).
    std::array <u8, 0x80000>   rom;              ///< ROM (512KB).
    std::array <u8, 0x80>      smpc;             ///< SMPC (128B).
    std::array <u8, 0x10000>   backup_ram;       ///< Backup RAM (64KB).
    std::array <u8, 0xD0>      scu;              ///< SCU (208B)
    std::array <u8, 0x80000>   vdp2_vram;        ///< VDP2 video RAM (512KB)
    std::array <u8, 0x1000>    vdp2_cram;        ///< VDP2 color RAM (4KB).
    std::array <u8, 0x200>     vdp2_registers;   ///< VDP2 registers (512B).
    std::array <u8, 0x80000>   vdp1_vram;        ///< VDP1 video RAM (512KB).
    std::array <u8, 0x40000>   vdp1_framebuffer; ///< VDP1 framebuffer (256KB).
    std::array <u8, 0x18>      vdp1_registers;   ///< VDP1 registers (24B).
    std::array <u8, 0x100000>  sound_ram;        ///< Sound RAM (1MB).
    std::array <u8, 0x100>     stv_io;           ///< STV I/O (256B).
    std::array <u8, 0x3000000> cart;             ///< Cart (50MB).
    std::array <u8, 0x400>     cache_addresses;  ///< Cache addresses (1KB).
    std::array <u8, 0x1000>    cache_data;       ///< Cache data (4KB).

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Memory::loadBios(const Hardware_mode mode);
    ///
    /// \brief  Loads the BIOS into memory.
    ///
    /// \author Runik
    /// \date   18/06/2018
    ///
    /// \param  mode    Hardware mode of the bios to load (Saturn/ST-V).
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void loadBios(const saturnin::core::Hardware_mode mode);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn bool Memory::loadRom(   const std::string& zip_name, 
    ///                             const std::string& file_name, 
    ///                             u8* destination, 
    ///                             const u32 size, 
    ///                             const Rom_load rom_load, 
    ///                             const u8 times_mirrored, 
    ///                             const Rom_type rom_type);
    ///
    /// \brief  Loads a ST-V rom from a zip file in memory (cart area).
    ///
    /// \author Runik
    /// \date   21/08/2018
    ///
    /// \param          zip_name        Name of the zip file container.
    /// \param          file_name       Filename inside the zip to load.
    /// \param [in,out] destination     If non-null, destination of the file content.
    /// \param          size            Size of data to load.
    /// \param          rom_load        Type of loading used (non interleaved, odd interleaved or even interleaved).
    /// \param          times_mirrored  Number of times the data has to be mirrored after the initial loading.
    /// \param          rom_type        Rom type (bios, program or graphic).
    ///
    /// \return True if it succeeds.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    bool loadRom(   const std::string&  zip_name,
                    const std::string&  file_name,
                    u8*	        destination,
                    const u32      size,
                    const Rom_load      rom_load,
                    const u8       times_mirrored,
                    const Rom_type      rom_type);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn bool Memory::loadStvGame(const std::string& file_name);
    ///
    /// \brief  Loads a ST-V game into the cart area.
    ///
    /// \author Runik
    /// \date   28/08/2018
    ///
    /// \param  file_name   configuration file name, with cfg extension.
    ///
    /// \return True if the game is loaded.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    bool loadStvGame(const std::string& config_filename);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Memory::swapCartArea();
    ///
    /// \brief  Swaps data in the cart area if needed.
    ///
    /// \author Runik
    /// \date   30/08/2018
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void swapCartArea();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn template<typename T> T Memory::read(const u32 addr);
    ///
    /// \brief  Saturn memory global read method.
    ///
    /// \author Runik
    /// \date   04/11/2018
    ///
    /// \tparam T       Type of data to return.
    /// \param  addr    Address to read in the memory map.
    ///
    /// \return         Data read
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    template<typename T>
    T read(const u32 addr);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn template<typename T> void Memory::write(const u32 addr, const T data);
    ///
    /// \brief  Writes.
    ///
    /// \author Runik
    /// \date   04/11/2018
    ///
    /// \tparam T       Type of data to write.
    /// \param  addr    Address to write to in the memory map.
    /// \param  data    Data to write.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    template<typename T>
    void write(const u32 addr, const T data);

private:
    void initializeHandlers();
    
    std::shared_ptr<Config> config_;    ///< Configuration object

    /// \Memory handlers
    //@{
    ReadHandler<u8>   read_8_handler_;
    ReadHandler<u16>  read_16_handler_;
    ReadHandler<u32>  read_32_handler_;
    WriteHandler<u8>  write_8_handler_;
    WriteHandler<u16> write_16_handler_;
    WriteHandler<u32> write_32_handler_;
    //@}

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn template<typename T> void Memory::initializeHandler(u32 begin, u32 end, ReadType<T> func);
    ///
    /// \brief  Binds the read function to a memory range in the Saturn memory.
    ///
    /// \author Runik
    /// \date   04/11/2018
    ///
    /// \tparam T       Type of data.
    /// \param  begin   Start address.
    /// \param  end     End address.
    /// \param  func    Function to bind.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    template<typename T>
    void initializeHandler(u32 begin, u32 end, ReadType<T> func);

    template <template <class> class ReadType, class... T>
    auto initializeHandlers(u32 begin, u32 end);
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn template<typename T> void Memory::initializeHandler(u32 begin, u32 end, WriteType<T> func);
    ///
    /// \brief  Binds the write function to a memory range in the Saturn memory.
    ///
    /// \author Runik
    /// \date   04/11/2018
    ///
    /// \tparam T       Type of data.
    /// \param  begin   Start address.
    /// \param  end     End address.
    /// \param  func    Function to bind.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    template<typename T>
    void initializeHandler(u32 begin, u32 end, WriteType<T> func);

    //template <template <class> class WriteType, class... T>
    //auto initializeWriteHandlers(u32 begin, u32 end);

};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn void mirrorData( u8* data, 
///                             const u32 size, 
///                             const u8 times_mirrored, 
///                             const Rom_load rom_load);
///
/// \brief  Mirrors data in memory. Data between 0 and size-1 will mirrored times_mirrored times
///
/// \author Runik
/// \date   21/08/2018
///
/// \param [in,out] data            If non-null, the array containing the data to be mirrored.
/// \param          size            Size of the data to mirror.
/// \param          times_mirrored  Number of times the data has to be mirrored.
/// \param          rom_load        Type of loading used (non interleaved, odd interleaved or even interleaved).
////////////////////////////////////////////////////////////////////////////////////////////////////

void mirrorData(u8* data, const u32 size, const u8 times_mirrored, const Rom_load rom_load);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn std::vector<std::filesystem::path> listStvConfigurationFiles();
///
/// \brief  Returns a vector populated with valid ST-V configuration files found in.
///
/// \author Runik
/// \date   04/09/2018
///
/// \return The stv configuration files.
////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<std::string> listStvConfigurationFiles();

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn U read(const std::array<T, N>& arr, const u32 addr)
///
/// \brief  Reads a value from an array.
///         Maps 8, 16 and 32 to u8, uint16_t and u32, respectively
///         Usage : auto val = read<32>(memory, 0);
///         
/// \author Runik
/// \date   07/06/2018
///
/// \param  arr The array to read from.
/// \param  adr The address.
///
/// \return Data read
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename U, typename T, size_t N>
U rawRead(const std::array<T, N>& arr, const u32 addr);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn void write(std::array<T, N>& arr, u32 adr, U value)
///
/// \brief  Writes a value to an array.
///         Maps 8, 16 and 32 to u8, uint16_t and u32, respectively.
///         Usage : write<32>(memory, 0, 0x12345678);
///         
/// \author Runik
/// \date   07/06/2018
///
/// \param [in,out] arr     The array to write to.
/// \param          adr     The address to write data to in the array.
/// \param          value   The value to write.
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename U, typename T, size_t N>
void rawWrite(std::array<T, N>& arr, const u32 addr, const U value);

// Handlers

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct writeDummy
///
/// \brief  Write dummy handler.
///
/// \author Runik
/// \date   18/12/2018
///
/// \tparam T   type of data to read (u8, u16 or u32).
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct writeDummy {
    operator Memory::WriteType<T>() const {
        return [](Memory& m, const u32 addr, const T data) {
            core::Log::warning("memory", fmt::format(core::tr("Write ({}) to unmapped area {:#0x} : {:#x}"), sizeof(T), addr, data));
        };
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct readDummy
///
/// \brief  Read dummy handler.
///
/// \author Runik
/// \date   01/11/2018
///
/// \tparam T   type of data to read (u8, u16 or u32).
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct readDummy {
    operator Memory::ReadType<T>() const {
        return [](const Memory& m, const u32 addr) -> T {
            core::Log::warning("memory", fmt::format(core::tr("Read ({}) from unmapped area {:#0x}"), sizeof(T), addr));
            return T{};
        };
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct readRom
///
/// \brief  ROM read handler.
///
/// \author Runik
/// \date   01/11/2018
///
/// \tparam T   type of data to read (u8, u16 or u32).
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct readRom{
    operator Memory::ReadType<T>() const {
        return [](const Memory& m, const u32 addr) -> T { return rawRead<T>(m.rom, addr & 0x7FFFF);};
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn template<typename T> T readSmpc(const Memory& m, const u32 addr)
///
/// \brief  SMPC read handler.
///
/// \author Runik
/// \date   01/11/2018
///
/// \tparam T       Type of data.
/// \param  m       Memory to process.
/// \param  addr    Address to read.
///
/// \return Data read.
////////////////////////////////////////////////////////////////////////////////////////////////////

//template<typename T>
//T readSmpc(const Memory& m, const u32 addr);

//// SMPC handlers
//template<typename T>
//T readSmpc(const Memory& m, const u32 addr) {
//    return rawRead<S>(m.smpc, addr & 0x7F);
//}

template<typename T>
struct readSmpc{
    operator Memory::ReadType<T>() const {
        return [](const Memory& m, const u32 addr) -> T { 
            return rawRead<T>(m.smpc, addr & 0x7F); 
        };
    }
};

// Specialization for 8 bits data.
template<>
struct readSmpc<uint8_t> {
    operator Memory::ReadType<u8>() const {
        return [](const Memory& m, const u32 addr) -> u8 {
            core::Log::error("memory", fmt::format(core::tr("Read ({}) needs to be handled through SMPC {:#0x}"), 8, addr));
            return 0;
        };
    }
};

// Specialization for 8 bits data.
//template<typename T>
//struct readSmpc {
//    operator Memory::ReadType<u8>() const {
//        return [](const Memory& m, const u32 addr) -> u8 { 
//            core::Log::error("memory", fmt::format(core::tr("Read ({}) needs to be handled through SMPC {:#0x}"), 8, addr));
//            return 0;
//        };
//    }
//};


////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn template<typename T> void writeSmpc(Memory& m, const u32 addr, const T data)
///
/// \brief  SMPC write handler.
///
/// \author Runik
/// \date   01/11/2018
///
/// \tparam T       Type of data.
/// \param  m       Memory to process.
/// \param  addr    Address to write to.
/// \param  data    Data to write.
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
void writeSmpc(Memory& m, const u32 addr, const T data);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn template<typename T> T readBackupRam(const Memory& m, const u32 addr);
///
/// \brief  Backup RAM read handler.
///
/// \author Runik
/// \date   04/11/2018
///
/// \tparam T       Type of data.
/// \param  m       Memory to process.
/// \param  addr    Address to read.
///
/// \return Data read.
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
T readBackupRam(const Memory& m, const u32 addr);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn template<typename T> void writeBackupRam(Memory& m, const u32 addr, const T data)
///
/// \brief  Backup RAM write handler.
///
/// \author Runik
/// \date   04/11/2018
///
/// \tparam T       Type of data.
/// \param  m       Memory to process.
/// \param  addr    Address to write to.
/// \param  data    Data to write.
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
void writeBackupRam(Memory& m, const u32 addr, const T data);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn template<typename T> T readWorkramLow(const Memory& m, const u32 addr);
///
/// \brief  Low workram read handler.
///
/// \author Runik
/// \date   06/11/2018
///
/// \tparam T       Type of data.
/// \param  m       Memory to process.
/// \param  addr    Address to read.
///
/// \return Data read.
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
T readWorkramLow(const Memory& m, const u32 addr);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn template<typename T> void writeWorkramLow(Memory& m, const u32 addr, const T data)
///
/// \brief  Low workram write handler.
///
/// \author Runik
/// \date   06/11/2018
///
/// \tparam T       Type of data.
/// \param  m       Memory to process.
/// \param  addr    Address to write to.
/// \param  data    Data to write.
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
void writeWorkramLow(Memory& m, const u32 addr, const T data);

}
}

#include "memory_impl.h"