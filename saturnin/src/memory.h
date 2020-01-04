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
#include <vector> // vector
#include <Windows.h> // VK constants

#include "emulator_defs.h"
#include "emulator_enums.h"
#include "log.h"
#include "scu.h"
#include "smpc.h"
#include "utilities.h" // to�nderlying


// Forward declarations
namespace saturnin::sh2 {
    class Sh2;
    enum class Sh2Type;
}
namespace sh2 = saturnin::sh2;
namespace util = saturnin::utilities;

namespace saturnin {
namespace core {

// Forward declarations
class EmulatorContext;
class Config;
class Scu;
class Smpc;

using saturnin::sh2::Sh2;
using saturnin::sh2::Sh2Type;

constexpr u32 rom_memory_mask              = 0x7FFFF;
constexpr u32 smpc_memory_mask             = 0x7F;
constexpr u32 backup_ram_memory_mask       = 0xFFFF;
constexpr u32 workram_low_memory_mask      = 0xFFFFF;
constexpr u32 stv_io_memory_mask           = 0xFF;
constexpr u32 cart_memory_mask             = 0x01FFFFFF;
constexpr u32 vdp1_ram_memory_mask         = 0x7FFFF;
constexpr u32 vdp1_framebuffer_memory_mask = 0x3FFFF;
constexpr u32 vdp1_registers_memory_mask   = 0x1F;
constexpr u32 vdp2_vram_memory_mask        = 0x7FFFF;
constexpr u32 vdp2_cram_memory_mask        = 0xFFF;
constexpr u32 vdp2_registers_memory_mask   = 0x1FF;
constexpr u32 scu_memory_mask              = 0xFF;
constexpr u32 workram_high_memory_mask     = 0xFFFFF;

static const u32 stv_io_port_a{ 0x400001 };
static const u32 stv_io_port_b{ 0x400003 };
static const u32 stv_io_port_c{ 0x400005 };
static const u32 stv_io_port_d{ 0x400007 };

static const u32 stv_protection_register_address{ 0x04FFFFFC };
static const u32 stv_protection_enabled{ 0x04FFFFF1 };
static       u32 stv_protection_offset{};

static const u32 memory_handler_size{ 0x10000 };

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum	RomType
///
/// \brief	ROM type. 
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class RomType {
    program, ///< Program ROM.
    graphic, ///< Graphic ROM.
    bios	 ///< Bios.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum	RomLoad
///
/// \brief	Way ROM data is loaded. 
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class RomLoad {
    not_interleaved, ///< Data loaded sequentially. 
    odd_interleaved, ///< Data loaded on odd bytes. 
    even_interleaved ///< Data loaded on even bytes.
};

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
    Memory(EmulatorContext* ec) : emulator_context_(ec) {
        initialize();
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
    //std::array <u8, 0x400>     cache_addresses_;  ///< Cache addresses (1KB).
    //std::array <u8, 0x1000>    cache_data_;       ///< Cache data (4KB).

    HardwareMode HardwareMode_{HardwareMode::saturn}; ///< Current hardware mode
    
    bool vdp2_cram_was_accessed_{ false }; ///< true when VDP2 color ram was accessed

    sh2::Sh2Type sh2_in_operation_ ; ///< Which SH2 is in operation
    //bool interrupt_signal_is_sent_from_master_sh2_{ false }; ///< InterruptCapture signal sent to the slave SH2 (minit)
    //bool interrupt_signal_is_sent_from_slave_sh2_{ false }; ///< InterruptCapture signal sent to the master SH2 (sinit)

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Memory::loadBios(const HardwareMode mode);
    ///
    /// \brief  Loads the BIOS into memory.
    ///
    /// \author Runik
    /// \date   18/06/2018
    ///
    /// \param  mode    Hardware mode of the bios to load (Saturn/ST-V).
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void loadBios(const saturnin::core::HardwareMode mode);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn bool Memory::loadRom(   const std::string& zip_name, 
    ///                             const std::string& file_name, 
    ///                             u8* destination, 
    ///                             const u32 size, 
    ///                             const RomLoad RomLoad, 
    ///                             const u8 times_mirrored, 
    ///                             const RomType RomType);
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
    /// \param          RomLoad        Type of loading used (non interleaved, odd interleaved or even interleaved).
    /// \param          times_mirrored  Number of times the data has to be mirrored after the initial loading.
    /// \param          RomType        Rom type (bios, program or graphic).
    ///
    /// \return True if it succeeds.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    bool loadRom(   const std::string&  zip_name,
                    const std::string&  file_name,
                    u8*	                destination,
                    const u32           size,
                    const RomLoad      RomLoad,
                    const u8            times_mirrored,
                    const RomType      RomType);

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

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Memory::sendFrtInterruptToMaster() const;
    ///
    /// \brief  Sends a FRT interrupt to the master SH2.
    ///
    /// \author Runik
    /// \date   14/02/2019
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void sendFrtInterruptToMaster() const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Memory::sendFrtInterruptToSlave() const;
    ///
    /// \brief  Sends a FRT interrupt to the slave SH2.
    ///
    /// \author Runik
    /// \date   14/02/2019
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void sendFrtInterruptToSlave() const;

    /// \name Context objects accessors
    //@{
    Sh2* masterSh2() const;
    Sh2* slaveSh2() const;
    Scu*      scu() const;
    Config*   config() const;
    Smpc* smpc() const;
    //@}

private:

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Memory::initialize();
    ///
    /// \brief  Initializes the Memory object.
    ///
    /// \author Runik
    /// \date   27/09/2019
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void initialize();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Memory::initializeHandlers();
    ///
    /// \brief  Initializes all the memory handlers of the memory map.
    ///
    /// \author Runik
    /// \date   09/02/2019
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void initializeHandlers();

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

    EmulatorContext* emulator_context_; ///< Emulator context object.

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

};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn void mirrorData( u8* data, 
///                             const u32 size, 
///                             const u8 times_mirrored, 
///                             const RomLoad RomLoad);
///
/// \brief  Mirrors data in memory. Data between 0 and size-1 will mirrored times_mirrored times
///
/// \author Runik
/// \date   21/08/2018
///
/// \param [in,out] data            If non-null, the array containing the data to be mirrored.
/// \param          size            Size of the data to mirror.
/// \param          times_mirrored  Number of times the data has to be mirrored.
/// \param          RomLoad        Type of loading used (non interleaved, odd interleaved or even interleaved).
////////////////////////////////////////////////////////////////////////////////////////////////////

void mirrorData(u8* data, const u32 size, const u8 times_mirrored, const RomLoad RomLoad);

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
///         Don't forget to use the related mask against the address value not to be off bounds ...
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
///         Usage : writ<u32>(memory, 0, 0x12345678);
///         Don't forget to use the related mask against the address value not to be off bounds ...
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
            Log::warning("memory", fmt::format(core::tr("Write ({}) to unmapped area {:#0x} : {:#x}"), sizeof(T), addr, data));
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
            Log::warning("memory", fmt::format(core::tr("Read ({}) from unmapped area {:#0x}"), sizeof(T), addr));
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
        return [](const Memory& m, const u32 addr) -> T { return rawRead<T>(m.rom_, addr & rom_memory_mask);};
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
            Log::warning("memory", core::tr("SMPC read ({}) access {:#0x}"), sizeof(T) * 8, addr);
            return rawRead<T>(m.smpc_, addr & smpc_memory_mask);
        };
    }
};

// Specialization for 8 bits data.
template<>
struct readSmpc<uint8_t> {
    operator Memory::ReadType<u8>() const {
        return [](const Memory& m, const u32 addr) -> u8 {
            //Log::warning("memory", core::tr("Read ({}) needs to be handled through SMPC {:#0x}"), 8, addr);
            //return 0;
            return m.smpc()->read(addr);
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
            Log::warning("memory", core::tr("SMPC write ({}) access {:#0x} : {:#x}"), sizeof(T) * 8, addr, data);
            //rawWrite<T>(m.smpc_, addr & smpc_memory_mask, data);
        };
    }
};

// Specialization for 8 bits data.
template<>
struct writeSmpc<uint8_t> {
    operator Memory::WriteType<u8>() const {
        return [](Memory& m, const u32 addr, const u8 data) {
            m.smpc()->write(addr, data);
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
            return rawRead<T>(m.backup_ram_, addr & backup_ram_memory_mask);
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
             rawWrite<T>(m.backup_ram_, addr & backup_ram_memory_mask, data);
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
             return rawRead<T>(m.workram_low_, addr & workram_low_memory_mask);
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
             rawWrite<T>(m.workram_low_, addr & workram_low_memory_mask, data);
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
             return rawRead<T>(m.stv_io_, addr & stv_io_memory_mask);
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
                     auto p1 = m.smpc()->getStvPeripheralMapping().player_1;
                     if (glfwGetKey(glfwGetCurrentContext(), util::toUnderlying(p1.button_1))          == GLFW_PRESS) data |= 0x01;
                     if (glfwGetKey(glfwGetCurrentContext(), util::toUnderlying(p1.button_2))          == GLFW_PRESS) data |= 0x02;
                     if (glfwGetKey(glfwGetCurrentContext(), util::toUnderlying(p1.button_3))          == GLFW_PRESS) data |= 0x04;
                     if (glfwGetKey(glfwGetCurrentContext(), util::toUnderlying(p1.button_4))          == GLFW_PRESS) data |= 0x08;
                     if (glfwGetKey(glfwGetCurrentContext(), util::toUnderlying(p1.direction_down))    == GLFW_PRESS) data |= 0x10;
                     if (glfwGetKey(glfwGetCurrentContext(), util::toUnderlying(p1.direction_up))      == GLFW_PRESS) data |= 0x20;
                     if (glfwGetKey(glfwGetCurrentContext(), util::toUnderlying(p1.direction_right))   == GLFW_PRESS) data |= 0x40;
                     if (glfwGetKey(glfwGetCurrentContext(), util::toUnderlying(p1.direction_left))    == GLFW_PRESS) data |= 0x80;
                     break;
                 case stv_io_port_b:
                     auto p2 = m.smpc()->getStvPeripheralMapping().player_2;
                     if (glfwGetKey(glfwGetCurrentContext(), util::toUnderlying(p2.button_1))          == GLFW_PRESS) data |= 0x01;
                     if (glfwGetKey(glfwGetCurrentContext(), util::toUnderlying(p2.button_2))          == GLFW_PRESS) data |= 0x02;
                     if (glfwGetKey(glfwGetCurrentContext(), util::toUnderlying(p2.button_3))          == GLFW_PRESS) data |= 0x04;
                     if (glfwGetKey(glfwGetCurrentContext(), util::toUnderlying(p2.button_4))          == GLFW_PRESS) data |= 0x08;
                     if (glfwGetKey(glfwGetCurrentContext(), util::toUnderlying(p2.direction_down))    == GLFW_PRESS) data |= 0x10;
                     if (glfwGetKey(glfwGetCurrentContext(), util::toUnderlying(p2.direction_up))      == GLFW_PRESS) data |= 0x20;
                     if (glfwGetKey(glfwGetCurrentContext(), util::toUnderlying(p2.direction_right))   == GLFW_PRESS) data |= 0x40;
                     if (glfwGetKey(glfwGetCurrentContext(), util::toUnderlying(p2.direction_left))    == GLFW_PRESS) data |= 0x80;
                     break;
                 case stv_io_port_c:
                     auto board = m.smpc()->getStvPeripheralMapping().board_controls;
                     if (glfwGetKey(glfwGetCurrentContext(), util::toUnderlying(board.p1_coin_switch)) == GLFW_PRESS) data |= 0x01;
                     if (glfwGetKey(glfwGetCurrentContext(), util::toUnderlying(board.p2_coin_switch)) == GLFW_PRESS) data |= 0x02;
                     if (glfwGetKey(glfwGetCurrentContext(), util::toUnderlying(board.test_switch))    == GLFW_PRESS) data |= 0x04;
                     if (glfwGetKey(glfwGetCurrentContext(), util::toUnderlying(board.service_switch)) == GLFW_PRESS) data |= 0x08;
                     if (glfwGetKey(glfwGetCurrentContext(), util::toUnderlying(board.p1_start))       == GLFW_PRESS) data |= 0x10;
                     if (glfwGetKey(glfwGetCurrentContext(), util::toUnderlying(board.p2_start))       == GLFW_PRESS) data |= 0x20;
                     break;
                 case stv_io_port_d:
                     data = rawRead<u8>(m.stv_io_, addr & stv_io_memory_mask);
                     data |= 0x3;
                     break;
                 default:
                     data = rawRead<u8>(m.stv_io_, addr & stv_io_memory_mask);
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
            rawWrite<T>(m.stv_io_, addr & stv_io_memory_mask, data);
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
    return (addr & cart_memory_mask) | temp;
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
                if (m.HardwareMode_ == HardwareMode::stv) {
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
            
            if (m.HardwareMode_ == HardwareMode::stv) {
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
            Log::warning("memory", core::tr("Read ({}) needs to be handled through CD-ROM {:#0x}"), sizeof(T)*8, addr);
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
            Log::warning("memory", core::tr("Write ({}) needs to be handled through CD-ROM {:#0x} : {:#x}"), sizeof(T)*8, addr, data);
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
            Log::warning("memory", core::tr("Read ({}) needs to be handled through SCSP {:#0x}"), sizeof(T) * 8, addr);
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
            Log::warning("memory", core::tr("Write ({}) needs to be handled through SCSP {:#0x} : {:#x}"), sizeof(T) * 8, addr, data);
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
            return rawRead<T>(m.vdp1_vram_, addr & vdp1_ram_memory_mask);
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
            rawWrite<T>(m.vdp1_vram_, addr & vdp1_ram_memory_mask, data);
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
            return rawRead<T>(m.vdp1_framebuffer_, addr & vdp1_framebuffer_memory_mask);
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
            rawWrite<T>(m.vdp1_framebuffer_, addr & vdp1_framebuffer_memory_mask, data);
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
            return rawRead<T>(m.vdp1_registers_, addr & vdp1_registers_memory_mask);
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
            rawWrite<T>(m.vdp1_registers_, addr & vdp1_registers_memory_mask, data);
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
            return rawRead<T>(m.vdp2_vram_, addr & vdp2_vram_memory_mask);
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
            rawWrite<T>(m.vdp2_vram_, addr & vdp2_vram_memory_mask, data);
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
            return rawRead<T>(m.vdp2_cram_, addr & vdp2_cram_memory_mask);
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
            rawWrite<T>(m.vdp2_cram_, addr & vdp2_cram_memory_mask, data);
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
            return rawRead<T>(m.vdp2_registers_, addr & vdp2_registers_memory_mask);
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
            rawWrite<T>(m.vdp2_registers_, addr & vdp2_registers_memory_mask, data);
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
            return rawRead<T>(m.scu_, addr & scu_memory_mask);
        };
    }
};

// Specialization for 32 bits data.
template<>
struct readScu<u32> {
    operator Memory::ReadType<u32>() const {
        return [](const Memory& m, const u32 addr) -> u32 {
            return m.scu()->read32(addr);
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
            rawWrite<T>(m.scu_, addr & scu_memory_mask, data);
        };
    }
};

// Specialization for 32 bits data.
template<>
struct writeScu<u32> {
    operator Memory::WriteType<u32>() const {
        return [](Memory& m, const u32 addr, const u32 data) {
            return m.scu()->write32(addr, data);
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
            return rawRead<T>(m.workram_high_, addr & workram_high_memory_mask);
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
            rawWrite<T>(m.workram_high_, addr & workram_high_memory_mask, data);
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
            if (isMasterSh2InOperation(m)) return m.masterSh2()->readRegisters<T>(addr);
            else return m.slaveSh2()->readRegisters<T>(addr);
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
            if (isMasterSh2InOperation(m)) return m.masterSh2()->writeRegisters<T>(addr, data);
            else return m.slaveSh2()->writeRegisters<T>(addr, data);
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
            //m.interrupt_signal_is_sent_from_master_sh2_ = true;
            Log::warning("memory", core::tr("{}bits write to the master SH2 FRT memory area !"), sizeof(T) * 8);
        };
    }
};

// Specialization for 16 bits data.
template<>
struct writeMasterSh2Frt<u16> {
    operator Memory::WriteType<u16>() const {
        return [](Memory& m, const u32 addr, const u16 data) {
            m.sendFrtInterruptToMaster();
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
            //m.interrupt_signal_is_sent_from_slave_sh2_ = true;
            Log::warning("memory", core::tr("{}bits write to the slave SH2 FRT memory area !"), sizeof(T) * 8);
        };
    }
};

// Specialization for 16 bits data.
template<>
struct writeSlaveSh2Frt<u16> {
    operator Memory::WriteType<u16>() const {
        return [](Memory& m, const u32 addr, const u16 data) {
            m.sendFrtInterruptToSlave();
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
            if (isMasterSh2InOperation(m)) return m.masterSh2()->readCacheAddresses<T>(addr);
            else return m.slaveSh2()->readCacheAddresses<T>(addr);
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
            if (isMasterSh2InOperation(m)) return m.masterSh2()->writeCacheAddresses<T>(addr, data);
            else return m.slaveSh2()->writeCacheAddresses<T>(addr, data);
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
            if (isMasterSh2InOperation(m)) return m.masterSh2()->readCacheData<T>(addr);
            else return m.slaveSh2()->readCacheData<T>(addr);
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
            if (isMasterSh2InOperation(m)) return m.masterSh2()->writeCacheData<T>(addr, data);
            else return m.slaveSh2()->writeCacheData<T>(addr, data);
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