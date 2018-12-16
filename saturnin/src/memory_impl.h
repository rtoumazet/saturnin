// 
// memory_impl.h
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
/// \file	memory_impl.h
///
/// \brief	Memory templates implementations, not to be included directly. Use memory.h instead
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <array> // array
#include <tuple> // get, tie

namespace saturnin {
namespace core {

template<typename T>
void Memory::initializeHandler(u32 begin,
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

template<typename T>
	void Memory::initializeHandler(u32 begin,
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

template <template <class> class ReadType, class... T>
auto Memory::initializeHandlers(u32 begin, u32 end) {
    (initializeHandler<T>(begin, end, ReadType<T>{}), ...);
}

template <template <class> class WriteType, class... T>
auto Memory::initializeWriteHandlers(u32 begin, u32 end) {
    (initializeHandler<T>(begin, end, WriteType<T>{}), ...);
}

template<typename T>
T Memory::read(const u32 addr) {
    auto& handler = std::get < ReadHandler<T>& >(std::tie(read_8_handler_, read_16_handler_, read_32_handler_));
    return handler[addr >> 16](*this, addr);
}

template<typename T>
void Memory::write(const u32 addr, const T data) {
    auto& handler = std::get < WriteHandler<T>& >(std::tie(write_8_handler_, write_16_handler_, write_32_handler_));
    handler[addr >> 16](*this, addr, data);
}

template<typename U, typename T, size_t N>
U rawRead(const std::array<T, N>& arr, const u32 addr) {
    U returnValue{ arr[addr] };
    for (u8 i = 1; i < sizeof(U); ++i) {
        returnValue <<= 8;
        returnValue |= arr[addr + i];
    }
    return returnValue;
}

template<typename U, typename T, size_t N>
void rawWrite(std::array<T, N>& arr, const u32 addr, const U value) {
    constexpr u8 bitsByByte{ std::numeric_limits<u8>::digits };
    constexpr u8 offset{ std::numeric_limits<U>::digits };
    for (u8 i = 0; i <= sizeof(U) - 1; ++i) {
        arr[addr + i] = (value >> (offset - (bitsByByte * i + bitsByByte))) & 0xff;
    }
}

// Handlers

// Dummy handlers
//template<typename T>
//void writeDummy(Memory& m, const u32 addr, const T data) {
//    core::Log::warning("memory", fmt::format(core::tr("Write ({}) to unmapped area {:#0x} : {:#x}"), sizeof(T), addr, data));
//}

/*template<typename T>
T readDummy(const Memory& m, const u32 addr) {
    core::Log::warning("memory", fmt::format(core::tr("Read ({}) from unmapped area {:#0x}"), sizeof(T), addr));
    return T{};
}*/
//template<typename T>
//struct readDummy {
//    operator Memory::ReadType<T>() const {
//        return [](const u32 addr) -> T {
//            core::Log::warning("memory", fmt::format(core::tr("Read ({}) from unmapped area {:#0x}"), sizeof(T), addr));
//            return T{};
//        };
//    }
//};

// ROM handlers
template<typename T>
T readRom(const Memory& m, const u32 addr) {
    return rawRead<S>(m.rom, addr & 0x7FFFF);
}

// SMPC handlers
template<typename T>
T readSmpc(const Memory& m, const u32 addr) {
    return rawRead<S>(m.smpc, addr & 0x7F);
}

// Specialization for 8 bits data.
inline u8 readSmpc(const Memory& m, const u32 addr) {
    core::Log::error("memory", fmt::format(core::tr("Read ({}) needs to be handled through SMPC {:#0x}"), 8, addr));
    return 0;
}

template<typename T>
void writeSmpc(Memory& m, const u32 addr, const T data) {
    rawWrite<S>(m.smpc, addr & 0x7F, data);
}

// Specialization for 8 bits data.
inline void writeSmpc(Memory& m, const u32 addr, const u8 data) {
    core::Log::warning("memory", fmt::format(core::tr("Write ({}) needs to be handled through SMPC {:#0x} : {:#x}"), 8, addr, data));
}

// Backup RAM handlers
template<typename T>
T readBackupRam(const Memory& m, const u32 addr) {
    return rawRead<S>(m.backup_ram, addr & 0xFFFF);
}

template<typename T>
void writeBackupRam(Memory& m, const u32 addr, const T data) {
    rawWrite<S>(m.backup_ram, addr & 0xFFFF, data);
}

// Low workram handlers
template<typename T>
T readWorkramLow(const Memory& m, const u32 addr) {
    return rawRead<S>(m.workram_low, addr & 0xFFFFF);
}

template<typename T>
void writeWorkramLow(Memory& m, const u32 addr, const T data) {
    rawWrite<S>(m.workram_low, addr & 0xFFFFF, data);
}

}
}