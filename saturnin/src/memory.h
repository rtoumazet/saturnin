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

#include "emulator_enums.h"
#include "config.h"

namespace saturnin {
namespace core {

const uint32_t memory_handler_size = 0x10000;

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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum	Memory_access
///
/// \brief	Way memory data can be accessed
////////////////////////////////////////////////////////////////////////////////////////////////////
enum class Memory_access {
    read8,
    read16,
    read32,
    write8,
    write16,
    write32
};
    
class Config; 

template<size_t Size>
using SizedUInt = std::conditional_t<Size == 8, uint8_t,
    std::conditional_t<Size == 16, uint16_t,
    std::conditional_t<Size == 32, uint32_t, void>>>;


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

    /// \name Saturn memory map definition.
    ///

    std::array <uint8_t, 0x100000>  workram_low;      ///< low workram (1MB).
    std::array <uint8_t, 0x100000>  workram_high;     ///< high workram (1MB).
    std::array <uint8_t, 0x80000>   rom;              ///< ROM (512KB).
    std::array <uint8_t, 0x80>      smpc;             ///< SMPC (128B).
    std::array <uint8_t, 0x10000>   backup_ram;       ///< Backup RAM (64KB).
    std::array <uint8_t, 0xD0>      scu;              ///< SCU (208B)
    std::array <uint8_t, 0x80000>   vdp2_vram;        ///< VDP2 video RAM (512KB)
    std::array <uint8_t, 0x1000>    vdp2_cram;        ///< VDP2 color RAM (4KB).
    std::array <uint8_t, 0x200>     vdp2_registers;   ///< VDP2 registers (512B).
    std::array <uint8_t, 0x80000>   vdp1_vram;        ///< VDP1 video RAM (512KB).
    std::array <uint8_t, 0x40000>   vdp1_framebuffer; ///< VDP1 framebuffer (256KB).
    std::array <uint8_t, 0x18>      vdp1_registers;   ///< VDP1 registers (24B).
    std::array <uint8_t, 0x100000>  sound_ram;        ///< Sound RAM (1MB).
    std::array <uint8_t, 0x100>     stv_io;           ///< STV I/O (256B).
    std::array <uint8_t, 0x3000000> cart;             ///< Cart (50MB).
    std::array <uint8_t, 0x400>     cache_addresses;  ///< Cache addresses (1KB).
    std::array <uint8_t, 0x1000>    cache_data;       ///< Cache data (4KB).

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
    ///                             uint8_t* destination, 
    ///                             const uint32_t size, 
    ///                             const Rom_load rom_load, 
    ///                             const uint8_t times_mirrored, 
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
                    uint8_t*	        destination,
                    const uint32_t      size,
                    const Rom_load      rom_load,
                    const uint8_t       times_mirrored,
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
    /// \fn template<size_t S> SizedUInt<S> Memory::read(const uint32_t addr);
    ///
    /// \brief  Saturn memory global read method.
    ///
    /// \author Runik
    /// \date   04/11/2018
    ///
    /// \tparam S       Size of the data in bits.
    /// \param  addr    Address to read in the memory map.
    ///
    /// \return         Data read
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    template<size_t S>
    SizedUInt<S> read(const uint32_t addr);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn template<size_t S> void Memory::write(const uint32_t addr, const SizedUInt<S> data);
    ///
    /// \brief  Writes.
    ///
    /// \author Runik
    /// \date   04/11/2018
    ///
    /// \tparam S       Size of the data in bits.
    /// \param  addr    Address to write to in the memory map.
    /// \param  data    Data to write.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    template<size_t S>
    void write(const uint32_t addr, const SizedUInt<S> data);

private:
    void initializeHandlers();
    
    std::shared_ptr<Config> config_;    ///< Configuration object

    /// \name Memory handlers functions typedefs
    /// std::function is not used here, as it implies a huge performance hit during execution
    //@{
    template <typename R, typename ...ARGS> using function = R(*)(ARGS...);
    template<size_t S>        using ReadType         = function<SizedUInt<S>, const Memory&, const uint32_t>;
    template<size_t S>        using WriteType        = function<void, Memory&, const uint32_t, const SizedUInt<S>>;
    template<class ReadType>  using ReadHandlerType  = std::array<ReadType, memory_handler_size>;
    template<class WriteType> using WriteHandlerType = std::array<WriteType, memory_handler_size>;
    template<std::size_t S>   using ReadHandler      = ReadHandlerType<ReadType<S>>;
    template<std::size_t S>   using WriteHandler     = WriteHandlerType<WriteType<S>>;
    //@}

    /// \Memory handlers
    //@{
    ReadHandler<8>   read_8_handler_;
    ReadHandler<16>  read_16_handler_;
    ReadHandler<32>  read_32_handler_;
    WriteHandler<8>  write_8_handler_;
    WriteHandler<16> write_16_handler_;
    WriteHandler<32> write_32_handler_;
    //@}

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn template<size_t S> void Memory::initializeHandler(uint32_t begin, uint32_t end, ReadType<S> func);
    ///
    /// \brief  Binds the read function to a memory range in the Saturn memory.
    ///
    /// \author Runik
    /// \date   04/11/2018
    ///
    /// \tparam S       Size of the data in bits.
    /// \param  begin   Start address.
    /// \param  end     End address.
    /// \param  func    Function to bind.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    template<size_t S>
    void initializeHandler(uint32_t begin, uint32_t end, ReadType<S> func);

    
    constexpr void initializeReadHandler(size_t begin, uint32_t end) {
        constexpr std::array<size_t, 3> arr{ 8,16,32 };
        for (auto a : arr) {
            initializeHandler<a>(begin, end, readDummy<a>);
        }
    
    }
    //template <size_t S, typename Func = ReadTypeWrapper<S>>
    //void initializeReadHandler(uint32_t begin, uint32_t end, Func func) {
    //    initializeHandler<8>(0x00000000, 0xFFFFFFFF, Func<8>()());
    //    initializeHandler<16>(0x00000000, 0xFFFFFFFF, Func<16>()());
    //    initializeHandler<32>(0x00000000, 0xFFFFFFFF, Func<32>()());
    //}
    
    //template <function<typename T, const Memory&, const uint32_t>>
    //void initializeReadHandler(uint32_t begin, uint32_t end) {
    //    initializeHandler<8>(0x00000000, 0xFFFFFFFF, func<8>()());
    //    initializeHandler<16>(0x00000000, 0xFFFFFFFF, func<16>()());
    //    initializeHandler<32>(0x00000000, 0xFFFFFFFF, func<32>()());
    //}

    //struct executor {
    //    template <size_t S>
    //    void func(std::string_view const & sw) const
    //    {
    //        execute<T>(sw);
    //    }
    //};

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn template<size_t S> void Memory::initializeHandler(uint32_t begin, uint32_t end, WriteType<S> func);
    ///
    /// \brief  Binds the write function to a memory range in the Saturn memory.
    ///
    /// \author Runik
    /// \date   04/11/2018
    ///
    /// \tparam S       Size of the data in bits.
    /// \param  begin   Start address.
    /// \param  end     End address.
    /// \param  func    Function to bind.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    template<size_t S>
    void initializeHandler(uint32_t begin, uint32_t end, WriteType<S> func);
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn void mirrorData( uint8_t* data, 
///                             const uint32_t size, 
///                             const uint8_t times_mirrored, 
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

void mirrorData(uint8_t* data, const uint32_t size, const uint8_t times_mirrored, const Rom_load rom_load);

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
/// \fn SizedUInt<S> read(const std::array<T, N>& arr, const uint32_t addr)
///
/// \brief  Reads a value from an array.
///         Maps 8, 16 and 32 to uint8_t, uint16_t and uint32_t, respectively
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

template<size_t S, typename T, size_t N>
SizedUInt<S> rawRead(const std::array<T, N>& arr, const uint32_t addr);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn void write(std::array<T, N>& arr, uint32_t adr, SizedUInt<S> value)
///
/// \brief  Writes a value to an array.
///         Maps 8, 16 and 32 to uint8_t, uint16_t and uint32_t, respectively.
///         Usage : write<32>(memory, 0, 0x12345678);
///         
/// \author Runik
/// \date   07/06/2018
///
/// \param [in,out] arr     The array to write to.
/// \param          adr     The address to write data to in the array.
/// \param          value   The value to write.
////////////////////////////////////////////////////////////////////////////////////////////////////

template<size_t S, typename T, size_t N>
void rawWrite(std::array<T, N>& arr, const uint32_t addr, const SizedUInt<S> value);

// Handlers

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn template<size_t S> void writeDummy(const Memory& m, const uint32_t addr, const SizedUInt<S> data)
///
/// \brief  Dummy write handler.
///
/// \author Runik
/// \date   01/11/2018
///
/// \tparam S       Size of the data in bits.
/// \param  m       Memory to process.
/// \param  addr    Address to write to.
/// \param  data    Data to write.
////////////////////////////////////////////////////////////////////////////////////////////////////

template<size_t S>
void writeDummy(Memory& m, const uint32_t addr, const SizedUInt<S> data);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn template<size_t S> SizedUInt<S> readDummy(const Memory& m, const uint32_t addr)
///
/// \brief  Dummy read handler.
///
/// \author Runik
/// \date   01/11/2018
///
/// \tparam S       Size of the data in bits.
/// \param  m       Memory to process.
/// \param  addr    Address to read.
///
/// \return Data read.
////////////////////////////////////////////////////////////////////////////////////////////////////

template<size_t S>
SizedUInt<S> readDummy(const Memory& m, const uint32_t addr);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn template<size_t S> SizedUInt<S> readRom(const Memory& m, const uint32_t addr)
///
/// \brief  ROM read handler.
///
/// \author Runik
/// \date   01/11/2018
///
/// \tparam S       Size of the data in bits.
/// \param  m       Memory to process.
/// \param  addr    Address to read.
///
/// \return Data read.
////////////////////////////////////////////////////////////////////////////////////////////////////

template<size_t S>
SizedUInt<S> readRom(const Memory& m, const uint32_t addr);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn template<size_t S> SizedUInt<S> readSmpc(const Memory& m, const uint32_t addr)
///
/// \brief  SMPC read handler.
///
/// \author Runik
/// \date   01/11/2018
///
/// \tparam S       Size of the data in bits.
/// \param  m       Memory to process.
/// \param  addr    Address to read.
///
/// \return Data read.
////////////////////////////////////////////////////////////////////////////////////////////////////

template<size_t S>
SizedUInt<S> readSmpc(const Memory& m, const uint32_t addr);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn template<size_t S> void writeSmpc(Memory& m, const uint32_t addr, const SizedUInt<S> data)
///
/// \brief  SMPC write handler.
///
/// \author Runik
/// \date   01/11/2018
///
/// \tparam S       Size of the data in bits.
/// \param  m       Memory to process.
/// \param  addr    Address to write to.
/// \param  data    Data to write.
////////////////////////////////////////////////////////////////////////////////////////////////////

template<size_t S>
void writeSmpc(Memory& m, const uint32_t addr, const SizedUInt<S> data);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn template<size_t S> SizedUInt<S> readBackupRam(const Memory& m, const uint32_t addr);
///
/// \brief  Backup RAM read handler.
///
/// \author Runik
/// \date   04/11/2018
///
/// \tparam S       Size of the data in bits.
/// \param  m       Memory to process.
/// \param  addr    Address to read.
///
/// \return Data read.
////////////////////////////////////////////////////////////////////////////////////////////////////

template<size_t S>
SizedUInt<S> readBackupRam(const Memory& m, const uint32_t addr);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn template<size_t S> void writeBackupRam(Memory& m, const uint32_t addr, const SizedUInt<S> data)
///
/// \brief  Backup RAM write handler.
///
/// \author Runik
/// \date   04/11/2018
///
/// \tparam S       Size of the data in bits.
/// \param  m       Memory to process.
/// \param  addr    Address to write to.
/// \param  data    Data to write.
////////////////////////////////////////////////////////////////////////////////////////////////////

template<size_t S>
void writeBackupRam(Memory& m, const uint32_t addr, const SizedUInt<S> data);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn template<size_t S> SizedUInt<S> readWorkramLow(const Memory& m, const uint32_t addr);
///
/// \brief  Low workram read handler.
///
/// \author Runik
/// \date   06/11/2018
///
/// \tparam S       Size of the data in bits.
/// \param  m       Memory to process.
/// \param  addr    Address to read.
///
/// \return Data read.
////////////////////////////////////////////////////////////////////////////////////////////////////

template<size_t S>
SizedUInt<S> readWorkramLow(const Memory& m, const uint32_t addr);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn template<size_t S> void writeWorkramLow(Memory& m, const uint32_t addr, const SizedUInt<S> data)
///
/// \brief  Low workram write handler.
///
/// \author Runik
/// \date   06/11/2018
///
/// \tparam S       Size of the data in bits.
/// \param  m       Memory to process.
/// \param  addr    Address to write to.
/// \param  data    Data to write.
////////////////////////////////////////////////////////////////////////////////////////////////////

template<size_t S>
void writeWorkramLow(Memory& m, const uint32_t addr, const SizedUInt<S> data);

}
}

#include "memory_impl.h"