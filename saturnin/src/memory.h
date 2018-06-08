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

namespace saturnin {
namespace core {

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \class  Memory
    ///
    /// \brief  Regroups everything related to the Saturn memory map.
    ///
    /// \author Runik
    /// \date   07/06/2018
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    class Memory {
    public:
        
        /// \name Saturn memory map definition.
        ///

        std::array <uint8_t, 0x100000>	workram_low;          ///< low workram (1MB).
        std::array <uint8_t, 0x100000>  workram_high;         ///< high workram (1MB).
        std::array <uint8_t, 0x80000>   rom;                  ///< ROM (512KB).
        std::array <uint8_t, 0x80>		smpc;                 ///< SMPC (128B).
        std::array <uint8_t, 0x10000>	backup_ram;           ///< Backup RAM (64KB).
        std::array <uint8_t, 0xD0>		scu;                  ///< SCU (208B)
        std::array <uint8_t, 0x80000>	vdp2_vram;            ///< VDP2 video RAM (512KB)
        std::array <uint8_t, 0x1000>	vdp2_cram;            ///< VDP2 color RAM (4KB).
        std::array <uint8_t, 0x200>		vdp2_registers;       ///< VDP2 registers (512B).
        std::array <uint8_t, 0x80000>	vdp1_vram;            ///< VDP1 video RAM (512KB).
        std::array <uint8_t, 0x40000>	vdp1_framebuffer;     ///< VDP1 framebuffer (256KB).
        std::array <uint8_t, 0x18>		vdp1_registers;       ///< VDP1 registers (24B).
        std::array <uint8_t, 0x100000>	sound_ram;            ///< Sound RAM (1MB).
        std::array <uint8_t, 0x100>		stv_io;               ///< STV I/O (256B).
        std::array <uint8_t, 0x3000000>	cart;                 ///< Cart (50MB).
        std::array <uint8_t, 0x400>		cache_addresses;      ///< Cache addresses (1KB).
        std::array <uint8_t, 0x1000>	cache_data;           ///< Cache data (4KB).
    };


    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn SizedUInt<Size> read(const std::array<T, N>& arr, const uint32_t adr)
    ///
    /// \brief  Reads a value from an array.
    ///         Maps 8, 16 and 32 to uint8_t, uint16_t and uint32_t, respectively
    ///         
    /// \author Runik
    /// \date   07/06/2018
    ///
    /// \param  arr The array to read from.
    /// \param  adr The address.
    ///
    /// \return A SizedUInt&lt;Size&gt;
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    template<size_t Size>   ///< The sized u int
    using SizedUInt = std::conditional_t<Size == 8, uint8_t,
        std::conditional_t<Size == 16, uint16_t,
        std::conditional_t<Size == 32, uint32_t, void>>>;

    template<size_t Size, typename T, size_t N>
    SizedUInt<Size> read(const std::array<T, N>& arr, const uint32_t adr)
    {
        SizedUInt<Size> returnValue{ arr[adr] };
        for (uint8_t i = 1; i < sizeof(SizedUInt<Size>); ++i) {
            returnValue <<= 8;
            returnValue |= arr[adr + i];
        }
        return returnValue;
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void write(std::array<T, N>& arr, uint32_t adr, SizedUInt<Size> value)
    ///
    /// \brief  Writes a value to an array.
    ///         Maps 8, 16 and 32 to uint8_t, uint16_t and uint32_t, respectively.
    ///         
    /// \author Runik
    /// \date   07/06/2018
    ///
    /// \param [in,out] arr     The array to write to.
    /// \param          adr     The address to write data to in the array.
    /// \param          value   The value to write.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    template<size_t Size>
    using SizedUInt = std::conditional_t<Size == 8, uint8_t,
        std::conditional_t<Size == 16, uint16_t,
        std::conditional_t<Size == 32, uint32_t, void>>>;

    template<size_t Size, typename T, size_t N>
    void write(std::array<T, N>& arr, const uint32_t adr, const SizedUInt<Size> value)
    {
        constexpr uint8_t bitsByByte{ std::numeric_limits<uint8_t>::digits };
        constexpr uint8_t offset{ std::numeric_limits<SizedUInt<Size>>::digits };
        for (uint8_t i = 0; i <= sizeof(SizedUInt<Size>) - 1; ++i) {
            arr[adr + i] = (value >> (offset - (bitsByByte * i + bitsByByte))) & 0xff;
        }
    }


}
}