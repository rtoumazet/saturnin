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
#include "sh2.h"

namespace saturnin {
namespace core {

static const u32 stv_io_port_a{ 0x400001 };
static const u32 stv_io_port_b{ 0x400003 };
static const u32 stv_io_port_c{ 0x400005 };
static const u32 stv_io_port_d{ 0x400007 };

static const u32 stv_protection_register_address{ 0x04FFFFFC };
static const u32 stv_protection_enabled{ 0x04FFFFF1 };
static       u32 stv_protection_offset{};

static const u32 memory_handler_size{ 0x10000 };

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
class Sh2;

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
    Memory(std::shared_ptr<Config> config, 
           Hardware_mode hm, 
           std::shared_ptr<Sh2> m,
           std::shared_ptr<Sh2> s) :    config_(config),
                                        hardware_mode_(hm),
                                        master_sh2_(m),
                                        slave_sh2_(s) {
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

    std::array <u8, 0x100000>  workram_low_;      ///< low workram (1MB).
    std::array <u8, 0x100000>  workram_high_;     ///< high workram (1MB).
    std::array <u8, 0x80000>   rom_;              ///< ROM (512KB).
    std::array <u8, 0x80>      smpc_;             ///< SMPC (128B).
    std::array <u8, 0x10000>   backup_ram_;       ///< Backup RAM (64KB).
    std::array <u8, 0xD0>      scu_;              ///< SCU (208B)
    std::array <u8, 0x80000>   vdp2_vram_;        ///< VDP2 video RAM (512KB)
    std::array <u8, 0x1000>    vdp2_cram_;        ///< VDP2 color RAM (4KB).
    std::array <u8, 0x200>     vdp2_registers_;   ///< VDP2 registers (512B).
    std::array <u8, 0x80000>   vdp1_vram_;        ///< VDP1 video RAM (512KB).
    std::array <u8, 0x40000>   vdp1_framebuffer_; ///< VDP1 framebuffer (256KB).
    std::array <u8, 0x18>      vdp1_registers_;   ///< VDP1 registers (24B).
    std::array <u8, 0x100000>  sound_ram_;        ///< Sound RAM (1MB).
    std::array <u8, 0x100>     stv_io_;           ///< STV I/O (256B).
    std::array <u8, 0x3000000> cart_;             ///< Cart (50MB).
    std::array <u8, 0x400>     cache_addresses_;  ///< Cache addresses (1KB).
    std::array <u8, 0x1000>    cache_data_;       ///< Cache data (4KB).

    Hardware_mode hardware_mode_{Hardware_mode::saturn}; ///< Current hardware mode
    
    bool vdp2_cram_was_accessed_{ false }; ///< true when VDP2 color ram was accessed

    Sh2_type sh2_in_operation_ { Sh2_type::unknown}; ///< Which SH2 is in operation
    bool interrupt_signal_is_sent_from_master_sh2_{ false }; ///< InterruptCapture signal sent to the slave SH2 (minit)
    bool interrupt_signal_is_sent_from_slave_sh2_{ false }; ///< InterruptCapture signal sent to the master SH2 (sinit)

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
                    u8*	                destination,
                    const u32           size,
                    const Rom_load      rom_load,
                    const u8            times_mirrored,
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

    u32 readStvProtection(const u32 addr, u32 data) const;

    void writeStvProtection(const u32 addr, u32 data);

    bool isStvProtectionEnabled() const;

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
    T read(const u32 addr) {
        auto& handler = std::get < ReadHandler<T>& >(std::tie(read_8_handler_, read_16_handler_, read_32_handler_));
        return handler[addr >> 16](*this, addr);
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn template<typename T> void Memory::write(const u32 addr, const T data);
    ///
    /// \brief  Saturn memory global write method.
    ///
    /// \author Runik
    /// \date   04/11/2018
    ///
    /// \tparam T       Type of data to write.
    /// \param  addr    Address to write to in the memory map.
    /// \param  data    Data to write.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    template<typename T>
    void write(const u32 addr, const T data) {
        auto& handler = std::get < WriteHandler<T>& >(std::tie(write_8_handler_, write_16_handler_, write_32_handler_));
        handler[addr >> 16](*this, addr, data);
    }

    std::shared_ptr<Sh2> masterSh2() const { return master_sh2_; };
    std::shared_ptr<Sh2> slaveSh2() const { return slave_sh2_; };

private:
    void initializeHandlers();
    
    std::shared_ptr<Config> config_;    ///< Configuration object
    std::shared_ptr<Sh2> master_sh2_;   ///< Master SH2 object
    std::shared_ptr<Sh2> slave_sh2_;    ///< Slave SH2 object

    /// \Memory handlers
    //@{
    ReadHandler<u8>   read_8_handler_;
    ReadHandler<u16>  read_16_handler_;
    ReadHandler<u32>  read_32_handler_;
    WriteHandler<u8>  write_8_handler_;
    WriteHandler<u16> write_16_handler_;
    WriteHandler<u32> write_32_handler_;
    //@}
    
    u32 stv_protection_offset_{};


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
    void initializeHandler(u32 begin,
                                   u32 end,
                                   ReadType<T> func) {
        begin >>= 16;
        end >>= 16;

        auto t = std::tie(read_8_handler_, read_16_handler_, read_32_handler_);
        for (u32 current = begin; current <= end; ++current) {
            auto& handler = std::get < ReadHandler<T>& >(t);
            handler[current & 0xFFFF] = func;
        }
    }

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
    void initializeHandler(u32 begin,
                                   u32 end,
                                   WriteType<T> func) {
        begin >>= 16;
        end >>= 16;

        auto t = std::tie(write_8_handler_, write_16_handler_, write_32_handler_);
        for (u32 current = begin; current <= end; ++current) {
            auto& handler = std::get < WriteHandler<T>& >(t);
            handler[current & 0xFFFF] = func;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn template <template <class> class OpType, class... T> auto Memory::initializeHandlers(u32 begin, u32 end)
    ///
    /// \brief  Initializes a group of handlers.
    ///
    /// \author Runik
    /// \date   20/12/2018
    ///
    /// \param  begin   Start address.
    /// \param  end     End address.
    ///
    /// \return An auto.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    template <template <class> class OpType, class... T>
    auto initializeHandlers(u32 begin, u32 end) {
        (initializeHandler<T>(begin, end, OpType<T>{}), ...);
    }

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
/// \fn template<typename T, typename U, size_t N> T rawRead(const std::array<U, N>& arr, const u32 addr)
///
/// \brief  Reads a value from an array. 
///         Usage : auto val = read<u32>(memory, 0);
///
/// \author Runik
/// \date   07/06/2018
///
/// \tparam T   Type of data to read.
/// \tparam U   Type of data stored in the array.
/// \tparam N   Size of the array.
/// \param  arr     The array to read from.
/// \param  addr    The address to read data from.
///
/// \return Data read.
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T, typename U, size_t N>
T rawRead(const std::array<U, N>& arr, const u32 addr) {
    T returnValue{ arr[addr] };
    for (u8 i = 1; i < sizeof(T); ++i) {
        returnValue <<= 8;
        returnValue |= arr[addr + i];
    }
    return returnValue;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn template<typename T, typename U, size_t N> void rawWrite(std::array<U, N>& arr, const u32 addr, const T value)
///
/// \brief  Writes a value to an array. Maps 8, 16 and 32 to u8, uint16_t and u32, respectively.
///         Usage : write&lt;32&gt;(memory, 0, 0x12345678);
///
/// \author Runik
/// \date   07/06/2018
///
/// \tparam T   Type of data to write.
/// \tparam U   Type of data stored in the array.
/// \tparam N   Size of the array.
/// \param [in,out] arr     The array to write to.
/// \param          addr    The address to write data to.
/// \param          value   The value to write.
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T, typename U, size_t N>
void rawWrite(std::array<U, N>& arr, const u32 addr, const T value) {
    constexpr u8 bitsByByte{ std::numeric_limits<u8>::digits };
    constexpr u8 offset{ std::numeric_limits<T>::digits };
    for (u8 i = 0; i <= sizeof(T) - 1; ++i) {
        arr[addr + i] = (value >> (offset - (bitsByByte * i + bitsByByte))) & 0xff;
    }
}

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
        return [](const Memory& m, const u32 addr) -> T { return rawRead<T>(m.rom_, addr & 0x7FFFF);};
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct readSmpc
///
/// \brief  SMPC read handler.
///
/// \author Runik
/// \date   01/11/2018
///
/// \tparam T   type of data to read (u8, u16 or u32).
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct readSmpc{
    operator Memory::ReadType<T>() const {
        return [](const Memory& m, const u32 addr) -> T { 
            return rawRead<T>(m.smpc_, addr & 0x7F); 
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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct writeSmpc
///
/// \brief  SMPC write handler.
///
/// \author Runik
/// \date   01/11/2018
///
/// \tparam T   type of data to write (u8, u16 or u32).
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct writeSmpc {
    operator Memory::WriteType<T>() const {
        return [](Memory& m, const u32 addr, const T data) {
            rawWrite<T>(m.smpc_, addr & 0x7F, data);
        };
    }
};

// Specialization for 8 bits data.
template<>
struct writeSmpc<uint8_t> {
    operator Memory::WriteType<u8>() const {
        return [](Memory& m, const u32 addr, const u8 data) {
            core::Log::warning("memory", fmt::format(core::tr("Write ({}) needs to be handled through SMPC {:#0x} : {:#x}"), 8, addr, data));
        };
    }
};

 ////////////////////////////////////////////////////////////////////////////////////////////////////
 /// \struct    readBackupRam
 ///
 /// \brief Backup RAM read handler.
 ///
 /// \author    Runik
 /// \date  04/11/2018
 ///
 /// \tparam T   type of data to read (u8, u16 or u32).
 ////////////////////////////////////////////////////////////////////////////////////////////////////

 template<typename T>
 struct readBackupRam {
    operator Memory::ReadType<T>() const {
        return [](const Memory& m, const u32 addr) -> T {
            return rawRead<T>(m.backup_ram_, addr & 0xFFFF);
        };
    }
 };

 ////////////////////////////////////////////////////////////////////////////////////////////////////
 /// \struct    writeBackupRam
 ///
 /// \brief Backup RAM write handler.
 ///
 /// \author    Runik
 /// \date  04/11/2018
 ///
 /// \tparam T   type of data to write (u8, u16 or u32).
 ////////////////////////////////////////////////////////////////////////////////////////////////////

 template<typename T>
 struct writeBackupRam {
     operator Memory::WriteType<T>() const {
         return [](Memory& m, const u32 addr, const T data) {
             rawWrite<T>(m.backup_ram_, addr & 0xFFFF, data);
         };
     }
 };

 ////////////////////////////////////////////////////////////////////////////////////////////////////
 /// \struct    readWorkramLow
 ///
 /// \brief Low workram read handler.
 ///
 /// \author    Runik
 /// \date  06/11/2018
 ///
 /// \tparam T   type of data to read (u8, u16 or u32).
 ////////////////////////////////////////////////////////////////////////////////////////////////////

 template<typename T>
 struct readWorkramLow {
     operator Memory::ReadType<T>() const {
         return [](const Memory& m, const u32 addr) -> T {
             return rawRead<T>(m.workram_low_, addr & 0xFFFFF);
         };
     }
 };

 ////////////////////////////////////////////////////////////////////////////////////////////////////
 /// \struct    writeWorkramLow
 ///
 /// \brief Low workram write handler.
 ///
 /// \author    Runik
 /// \date  06/11/2018
 ///
 /// \tparam T   type of data to write (u8, u16 or u32).
 ////////////////////////////////////////////////////////////////////////////////////////////////////

 template<typename T>
 struct writeWorkramLow {
     operator Memory::WriteType<T>() const {
         return [](Memory& m, const u32 addr, const T data) {
             rawWrite<T>(m.workram_low_, addr & 0xFFFFF, data);
         };
     }
 };

 ////////////////////////////////////////////////////////////////////////////////////////////////////
 /// \struct    readStvIo
 ///
 /// \brief Stv i/o read handler.
 ///
 /// \author    Runik
 /// \date  22/12/2018
 ///
 /// \tparam T   type of data to read (u8, u16 or u32).
 ////////////////////////////////////////////////////////////////////////////////////////////////////

 template<typename T>
 struct readStvIo{
     operator Memory::ReadType<T>() const {
         return [](const Memory& m, const u32 addr) -> T {
             return rawRead<T>(m.stv_io_, addr & 0xFF);
         };
     }
 };

// Specialization for 8 bits data.
template<>
struct readStvIo<u8> {
    operator Memory::ReadType<u8>() const {
        return [](const Memory& m, const u32 addr) -> u8 {
             // WIP use gainput/glfw3 to manage inputs
             u8 data{};
             switch (addr & 0x00FFFFFF) {
                 case stv_io_port_a:
                     if (GetAsyncKeyState('X') & 0x8000) data |= 0x01; //p1 A
                     if (GetAsyncKeyState('F') & 0x8000) data |= 0x02; //p1 B
                     if (GetAsyncKeyState('D') & 0x8000) data |= 0x04; //p1 C
                     if (GetAsyncKeyState('S') & 0x8000) data |= 0x08; //p1 D
                     if (GetAsyncKeyState(VK_DOWN) & 0x8000) data |= 0x10; //p1 DOWN
                     if (GetAsyncKeyState(VK_UP) & 0x8000) data |= 0x20; //p1 UP
                     if (GetAsyncKeyState(VK_LEFT) & 0x8000) data |= 0x80; //p1 LEFT
                     if (GetAsyncKeyState(VK_RIGHT) & 0x8000) data |= 0x40; //p1 RIGHT
                     break;
                 case stv_io_port_b:
                     //if (GetAsyncKeyState(VK_X)&0x8000) data|=0x01; //p2 A
                     //if (GetAsyncKeyState(VK_F)&0x8000) data|=0x02; //p2 B
                     //if (GetAsyncKeyState(VK_D)&0x8000) data|=0x04; //p2 C
                     //if (GetAsyncKeyState(VK_S)&0x8000) data|=0x08; //p2 D
                     //if (GetAsyncKeyState(VK_DOWN)&0x8000) data|=0x10; //p2 DOWN
                     //if (GetAsyncKeyState(VK_UP)&0x8000) data|=0x20; //p2 UP
                     //if (GetAsyncKeyState(VK_LEFT)&0x8000) data|=0x80; //p2 LEFT
                     //if (GetAsyncKeyState(VK_RIGHT)&0x8000) data|=0x40; //p2 RIGHT
                     break;
                 case stv_io_port_c:
                     if (GetAsyncKeyState(VK_F3) & 0x8000) data |= 0x01; // P1 Coin 
                     //if (GetAsyncKeyState(VK_4)&0x8000) data|=0x02; // P2 Coin
                     //if (GetAsyncKeyState(VK_F1)&0x8000) data|=0x04; // Test
                     //if (GetAsyncKeyState(VK_F2)&0x8000) data|=0x08; // Service
                     if (GetAsyncKeyState(VK_F4) & 0x8000) data |= 0x10; // P1 Start
                     //if (GetAsyncKeyState(VK_2)&0x8000) data|=0x20; // P2 Start
                     //if (GetAsyncKeyState(VK_7)&0x8000) data|=0x40;
                     //if (GetAsyncKeyState(VK_8)&0x8000) data|=0x80;
                     break;
                 case stv_io_port_d:
                     data = rawRead<u8>(m.stv_io_, addr & 0xFF);
                     data |= 0x3;
                     break;
                 default:
                     data = rawRead<u8>(m.stv_io_, addr & 0xFF);
             }
             data = ~data;
             return data;
         };
     }
 };

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct writeStvIo
///
/// \brief  Stv i/o write handler.
///
/// \author Runik
/// \date   22/12/2018
///
/// \tparam T   type of data to write (u8, u16 or u32).
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct writeStvIo{
    operator Memory::WriteType<T>() const {
        return [](Memory& m, const u32 addr, const T data) {
            rawWrite<T>(m.stv_io_, addr & 0xFF, data);
        };
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn inline u32 calculateRelativeCartAddress(const u32 addr)
///
/// \brief  Calculates the relative cart address from a real address.
///
/// \author Runik
/// \date   28/12/2018
///
/// \param  addr    The real address.
///
/// \return The calculated relative cart address.
////////////////////////////////////////////////////////////////////////////////////////////////////

inline u32 calculateRelativeCartAddress(const u32 addr) {
    u32 temp{ (addr >> 1) & 0x02000000 };
    return (addr & 0x01FFFFFF) | temp;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct readCart
///
/// \brief  Cart read handler.
///
/// \author Runik
/// \date   28/12/2018
///
/// \tparam T   type of data to read (u8, u16 or u32).
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct readCart{
    operator Memory::ReadType<T>() const {
        return [](const Memory& m, const u32 addr) -> T {
            u32 relative_addr = calculateRelativeCartAddress(addr);
            return rawRead<T>(m.cart_, relative_addr);
        };
    }
};

// Specialization for 32 bits data.
template<>
struct readCart<u32> {
    operator Memory::ReadType<u32>() const {
        return [](const Memory& m, const u32 addr) -> u32 {
            u32 relative_addr = calculateRelativeCartAddress(addr);
            u32 data{ rawRead<u32>(m.cart_, relative_addr) };

            if ((addr & 0x0FFFFFFF) == stv_protection_register_address) {
                if (m.hardware_mode_ == Hardware_mode::stv) {
                    data = m.readStvProtection(addr, data);
                }
            }
            return data;
        };
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct writeCart
///
/// \brief  Cart write handler.
///
/// \author Runik
/// \date   28/12/2018
///
/// \tparam T   type of data to write (u8, u16 or u32).
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct writeCart{
    operator Memory::WriteType<T>() const {
        return [](Memory& m, const u32 addr, const T data) {
            u32 relative_addr = calculateRelativeCartAddress(addr);
            rawWrite<T>(m.cart_, relative_addr, data);
        };
    }
};

// Specialization for 8 bits data.
template<>
struct writeCart<u8> {
    operator Memory::WriteType<u8>() const {
        return [](Memory& m, const u32 addr, const u8 data) {
            
            if (m.hardware_mode_ == Hardware_mode::stv) {
                if ((addr & 0x0FFFFFFF) == stv_protection_enabled) {
                    if (data == 0x1) { // Is the protection enabled ?
                        m.writeStvProtection(addr, data);
                    }
                }
            }
        };
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct readCdBlock
///
/// \brief  CD block read handler.
///
/// \author Runik
/// \date   28/12/2018
///
/// \tparam T   type of data to read (u8, u16 or u32).
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct readCdBlock{
    operator Memory::ReadType<T>() const {
        return [](const Memory& m, const u32 addr) -> T {
            core::Log::error("memory", fmt::format(core::tr("Read ({}) needs to be handled through CD-ROM {:#0x}"), sizeof(T)*8, addr));
            return 0;
        };
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct writeCdBlock
///
/// \brief  CD block write handler.
///
/// \author Runik
/// \date   28/12/2018
///
/// \tparam T   type of data to write (u8, u16 or u32).
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct writeCdBlock{
    operator Memory::WriteType<T>() const {
        return [](Memory& m, const u32 addr, const T data) {
            core::Log::error("memory", fmt::format(core::tr("Write ({}) needs to be handled through CD-ROM {:#0x} : {:#x}"), sizeof(T)*8, addr, data));
        };
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct readScsp
///
/// \brief  SCSP read handler.
///
/// \author Runik
/// \date   28/12/2018
///
/// \tparam T   type of data to read (u8, u16 or u32).
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct readScsp{
    operator Memory::ReadType<T>() const {
        return [](const Memory& m, const u32 addr) -> T {
            core::Log::error("memory", fmt::format(core::tr("Read ({}) needs to be handled through SCSP {:#0x}"), sizeof(T) * 8, addr));
            return 0;
        };
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct writeScsp
///
/// \brief  SCSP write handler.
///
/// \author Runik
/// \date   28/12/2018
///
/// \tparam T   type of data to write (u8, u16 or u32).
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct writeScsp{
    operator Memory::WriteType<T>() const {
        return [](Memory& m, const u32 addr, const T data) {
            core::Log::error("memory", fmt::format(core::tr("Write ({}) needs to be handled through SCSP {:#0x} : {:#x}"), sizeof(T) * 8, addr, data));
        };
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct readVdp1Ram
///
/// \brief  VDP1 RAM read handler.
///
/// \author Runik
/// \date   28/12/2018
///
/// \tparam T   type of data to read (u8, u16 or u32).
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct readVdp1Ram{
    operator Memory::ReadType<T>() const {
        return [](const Memory& m, const u32 addr) -> T {
            return rawRead<T>(m.vdp1_vram_, addr & 0x7FFFF);
        };
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct writeVdp1Ram
///
/// \brief  VDP1 RAM write handler.
///
/// \author Runik
/// \date   28/12/2018
///
/// \tparam T   type of data to write (u8, u16 or u32).
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct writeVdp1Ram{
    operator Memory::WriteType<T>() const {
        return [](Memory& m, const u32 addr, const T data) {
            rawWrite<T>(m.vdp1_vram_, addr & 0x7FFFF, data);
            // :TODO: Handle character address cache
        };
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct readVdp1Framebuffer
///
/// \brief  VDP1 framebuffer read handler.
///
/// \author Runik
/// \date   28/12/2018
///
/// \tparam T   type of data to read (u8, u16 or u32).
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct readVdp1Framebuffer{
    operator Memory::ReadType<T>() const {
        return [](const Memory& m, const u32 addr) -> T {
            return rawRead<T>(m.vdp1_framebuffer_, addr & 0x3FFFF);
        };
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct writeVdp1Framebuffer
///
/// \brief  VDP1 framebuffer write handler.
///
/// \author Runik
/// \date   28/12/2018
///
/// \tparam T   type of data to write (u8, u16 or u32).
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct writeVdp1Framebuffer{
    operator Memory::WriteType<T>() const {
        return [](Memory& m, const u32 addr, const T data) {
            rawWrite<T>(m.vdp1_framebuffer_, addr & 0x3FFFF, data);
        };
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct readVdp1Registers
///
/// \brief  VDP1 registers read handler.
///
/// \author Runik
/// \date   28/12/2018
///
/// \tparam T   type of data to read (u8, u16 or u32).
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct readVdp1Registers{
    operator Memory::ReadType<T>() const {
        return [](const Memory& m, const u32 addr) -> T {
            return rawRead<T>(m.vdp1_registers_, addr & 0x1F);
        };
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct writeVdp1Registers
///
/// \brief  VDP1 registers write handler.
///
/// \author Runik
/// \date   28/12/2018
///
/// \tparam T   type of data to write (u8, u16 or u32).
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct writeVdp1Registers{
    operator Memory::WriteType<T>() const {
        return [](Memory& m, const u32 addr, const T data) {
            rawWrite<T>(m.vdp1_registers_, addr & 0x1F, data);
        };
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct readVdp2Vram
///
/// \brief  VDP2 VRAM read handler.
///
/// \author Runik
/// \date   28/12/2018
///
/// \tparam T   type of data to read (u8, u16 or u32).
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct readVdp2Vram{
    operator Memory::ReadType<T>() const {
        return [](const Memory& m, const u32 addr) -> T {
            return rawRead<T>(m.vdp2_vram_, addr & 0x7FFFF);
        };
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct writeVdp2Vram
///
/// \brief  VDP2 VRAM write handler.
///
/// \author Runik
/// \date   28/12/2018
///
/// \tparam T   type of data to write (u8, u16 or u32).
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct writeVdp2Vram {
    operator Memory::WriteType<T>() const {
        return [](Memory& m, const u32 addr, const T data) {
            rawWrite<T>(m.vdp2_vram_, addr & 0x7FFFF, data);
            // :TODO: handle VDP2 page cache
        };
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct readVdp2Cram
///
/// \brief  VDP2 CRAM read handler.
///
/// \author Runik
/// \date   29/12/2018
///
/// \tparam T   type of data to read (u8, u16 or u32).
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct readVdp2Cram {
    operator Memory::ReadType<T>() const {
        return [](const Memory& m, const u32 addr) -> T {
            return rawRead<T>(m.vdp2_cram_, addr & 0xFFF);
        };
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct writeVdp2Cram
///
/// \brief  VDP2 CRAM write handler.
///
/// \author Runik
/// \date   29/12/2018
///
/// \tparam T   type of data to write (u8, u16 or u32).
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct writeVdp2Cram {
    operator Memory::WriteType<T>() const {
        return [](Memory& m, const u32 addr, const T data) {
            rawWrite<T>(m.vdp2_cram_, addr & 0xFFF, data);
            m.vdp2_cram_was_accessed_ = true;
        };
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct readVdp2Registers
///
/// \brief  VDP2 registers read handler.
///
/// \author Runik
/// \date   29/12/2018
///
/// \tparam T   type of data to read (u8, u16 or u32).
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct readVdp2Registers {
    operator Memory::ReadType<T>() const {
        return [](const Memory& m, const u32 addr) -> T {
            return rawRead<T>(m.vdp2_registers_, addr & 0x1FF);
        };
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct writeVdp2Registers
///
/// \brief  VDP2 registers read handler.
///
/// \author Runik
/// \date   29/12/2018
///
/// \tparam T   type of data to write (u8, u16 or u32).
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct writeVdp2Registers {
    operator Memory::WriteType<T>() const {
        return [](Memory& m, const u32 addr, const T data) {
            rawWrite<T>(m.vdp2_registers_, addr & 0x1FF, data);
            // :TODO: handle bitmap update
        };
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct readScu
///
/// \brief  SCU read handler.
///
/// \author Runik
/// \date   29/12/2018
///
/// \tparam T   type of data to read (u8, u16 or u32).
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct readScu {
    operator Memory::ReadType<T>() const {
        return [](const Memory& m, const u32 addr) -> T {
            return rawRead<T>(m.scu_, addr & 0xFF);
        };
    }
};

// Specialization for 32 bits data.
template<>
struct readScu<u32> {
    operator Memory::ReadType<u32>() const {
        return [](const Memory& m, const u32 addr) -> u32 {
            core::Log::error("memory", fmt::format(core::tr("Read ({}) needs to be handled through SCU {:#0x}"), sizeof(u32) * 8, addr));

            return 0;
        };
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct writeScu
///
/// \brief  SCU write handler.
///
/// \author Runik
/// \date   29/12/2018
///
/// \tparam T   type of data to write (u8, u16 or u32).
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct writeScu{
    operator Memory::WriteType<T>() const {
        return [](Memory& m, const u32 addr, const T data) {
            rawWrite<T>(m.scu_, addr, data);
        };
    }
};

// Specialization for 32 bits data.
template<>
struct writeCart<u32> {
    operator Memory::WriteType<u32>() const {
        return [](Memory& m, const u32 addr, const u32 data) {
            core::Log::error("memory", fmt::format(core::tr("Write({}) needs to be handled through SCU {:#0x}"), sizeof(u32) * 8, addr));
        };
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct readWorkramHigh
///
/// \brief  High workram read handler.
///
/// \author Runik
/// \date   29/12/2018
///
/// \tparam T   type of data to read (u8, u16 or u32).
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct readWorkramHigh{
    operator Memory::ReadType<T>() const {
        return [](const Memory& m, const u32 addr) -> T {
            return rawRead<T>(m.workram_high_, addr & 0xFFFFF);
        };
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct writeWorkramHigh
///
/// \brief  High workram write handler.
///
/// \author Runik
/// \date   29/12/2018
///
/// \tparam T   type of data to write (u8, u16 or u32).
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct writeWorkramHigh{
    operator Memory::WriteType<T>() const {
        return [](Memory& m, const u32 addr, const T data) {
            rawWrite<T>(m.workram_high_, addr & 0xFFFFF, data);
        };
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct readSh2Registers
///
/// \brief  SH2 registers read handler.
///
/// \author Runik
/// \date   29/12/2018
///
/// \tparam T   type of data to read (u8, u16 or u32).
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct readSh2Registers {
    operator Memory::ReadType<T>() const {
        return [](const Memory& m, const u32 addr) -> T {
            if (isMasterSh2InOperation(m)) return m.masterSh2()->read<T>(addr);
            else return m.slaveSh2()->read<T>(addr);
        };
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct writeSh2Registers
///
/// \brief SH2 registers write handler.
///
/// \author Runik
/// \date   29/12/2018
///
/// \tparam T   type of data to write (u8, u16 or u32).
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct writeSh2Registers{
    operator Memory::WriteType<T>() const {
        return [](Memory& m, const u32 addr, const T data) {
            if (isMasterSh2InOperation(m)) return m.masterSh2()->write<T>(addr, data);
            else return m.slaveSh2()->write<T>(addr, data);
        };
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct writeMasterSh2Frt
///
/// \brief  Master SH2 FRT write handler.
///
/// \author Runik
/// \date   30/12/2018
///
/// \tparam T   type of data to write (u8, u16 or u32).
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct writeMasterSh2Frt {
    operator Memory::WriteType<T>() const {
        return [](Memory& m, const u32 addr, const T data) {
            m.interrupt_signal_is_sent_from_master_sh2_ = true;
        };
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct writeSlaveSh2Frt
///
/// \brief  Slave SH2 FRT write handler.
///
/// \author Runik
/// \date   30/12/2018
///
/// \tparam T   type of data to write (u8, u16 or u32).
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct writeSlaveSh2Frt{
    operator Memory::WriteType<T>() const {
        return [](Memory& m, const u32 addr, const T data) {
            m.interrupt_signal_is_sent_from_slave_sh2_ = true;
        };
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct readCacheAdresses
///
/// \brief  Cache adresses read handler.
///
/// \author Runik
/// \date   30/12/2018
///
/// \tparam T   type of data to read (u8, u16 or u32).
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct readCacheAddresses {
    operator Memory::ReadType<T>() const {
        return [](const Memory& m, const u32 addr) -> T {
            return rawRead<T>(m.cache_addresses_, addr & 0x3FF);
        };
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct writeCacheAddresses
///
/// \brief  Cache addresses write handler.
///
/// \author Runik
/// \date   30/12/2018
///
/// \tparam T   type of data to write (u8, u16 or u32).
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct writeCacheAddresses{
    operator Memory::WriteType<T>() const {
        return [](Memory& m, const u32 addr, const T data) {
            rawWrite<T>(m.cache_addresses_, addr & 0x3FF, data);
        };
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct readCacheData
///
/// \brief  Cache data read handler.
///
/// \author Runik
/// \date   30/12/2018
///
/// \tparam T   type of data to read (u8, u16 or u32).
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct readCacheData{
    operator Memory::ReadType<T>() const {
        return [](const Memory& m, const u32 addr) -> T {
            return rawRead<T>(m.cache_data_, addr & 0xFFF);
        };
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct writeCacheData
///
/// \brief  Cache data write handler.
///
/// \author Runik
/// \date   30/12/2018
///
/// \tparam T   type of data to write (u8, u16 or u32).
////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
struct writeCacheData{
    operator Memory::WriteType<T>() const {
        return [](Memory& m, const u32 addr, const T data) {
            rawWrite<T>(m.cache_data_, addr & 0xFFF, data);
        };
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn inline bool isMasterSh2InOperation(const Memory& m);
///
/// \brief  Checks which SH2 is in operation.
///
/// \author Runik
/// \date   01/01/2019
///
/// \return Returns true if master SH2 is in operation, false if slave SH2 is in operation.
////////////////////////////////////////////////////////////////////////////////////////////////////

inline bool isMasterSh2InOperation(const Memory& m);

}
}