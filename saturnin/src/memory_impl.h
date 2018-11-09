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

template<size_t S>
//void Memory::initializeReadHandler(uint32_t begin,
void Memory::initializeHandler(uint32_t begin,
                           uint32_t end,
                           ReadType<S> func) {
    begin >>= 16;
    end >>= 16;

    auto t = std::tie(read_8_handler_, read_16_handler_, read_32_handler_);
    for (uint32_t current = begin; current <= end; ++current) {
        auto& handler = std::get < ReadHandler<S>& >(t);
        handler[current & 0xFFFF] = func;
    }
}
template<class T>
void Memory::initializeReadHandlerGlobal(uint32_t begin,
                                   uint32_t end,
                                   T func) {
    begin >>= 16;
    end >>= 16;

    for (uint32_t current = begin; current <= end; ++current) {
        read_8_handler_ = [current & 0xFFFF] = func<8>;
        read_16_handler_ = [current & 0xFFFF] = func<16>;
        read_32_handler_ = [current & 0xFFFF] = func<32>;
    }
}

//template <size_t S, typename R, typename ...ARGS>
//void Memory::initializeHandler(uint32_t begin, uint32_t end, function<R, ARGS...> func) {
//
//    begin >>= 16;
//    end >>= 16;
//
//    if (std::is_void<R>::value) {
//        // void return type implies write functions
//        auto t = std::tie(write_8_handler_, write_16_handler_, write_32_handler_);
//        for (uint32_t current = begin; current <= end; ++current) {
//            auto& handler = std::get < WriteHandler<S>& >(t);
//            handler[current & 0xFFFF] = func;
//        }
//    } else {
//        // read functions
//        auto t = std::tie(read_8_handler_, read_16_handler_, read_32_handler_);
//        //for (uint32_t current = begin; current <= end; ++current) {
//        //    auto& handler = std::get < ReadHandler<S>& >(t);
//        //    handler[current & 0xFFFF] = func;
//        //}
//    }
//}

template<size_t S>
//void Memory::initializeWriteHandler(uint32_t begin,
//                            uint32_t end,
//                            WriteType<S> func) {
	void Memory::initializeHandler(uint32_t begin,
		uint32_t end,
		WriteType<S> func) {
    begin >>= 16;
    end >>= 16;

    auto t = std::tie(write_8_handler_, write_16_handler_, write_32_handler_);
    for (uint32_t current = begin; current <= end; ++current) {
        auto& handler = std::get < WriteHandler<S>& >(t);
        handler[current & 0xFFFF] = func;
    }
}

template<size_t S>
SizedUInt<S> Memory::read(const uint32_t addr) {
    auto& handler = std::get < ReadHandler<S>& >(std::tie(read_8_handler_, read_16_handler_, read_32_handler_));
    return handler[addr >> 16](*this, addr);
}

template<size_t S>
void Memory::write(const uint32_t addr, const SizedUInt<S> data) {
    auto& handler = std::get < WriteHandler<S>& >(std::tie(write_8_handler_, write_16_handler_, write_32_handler_));
    handler[addr >> 16](*this, addr, data);
}

template<size_t S, typename T, size_t N>
SizedUInt<S> rawRead(const std::array<T, N>& arr, const uint32_t addr) {
    SizedUInt<S> returnValue{ arr[addr] };
    for (uint8_t i = 1; i < sizeof(SizedUInt<S>); ++i) {
        returnValue <<= 8;
        returnValue |= arr[addr + i];
    }
    return returnValue;
}

template<size_t S, typename T, size_t N>
void rawWrite(std::array<T, N>& arr, const uint32_t addr, const SizedUInt<S> value) {
    constexpr uint8_t bitsByByte{ std::numeric_limits<uint8_t>::digits };
    constexpr uint8_t offset{ std::numeric_limits<SizedUInt<S>>::digits };
    for (uint8_t i = 0; i <= sizeof(SizedUInt<S>) - 1; ++i) {
        arr[addr + i] = (value >> (offset - (bitsByByte * i + bitsByByte))) & 0xff;
    }
}

// Handlers

// Dummy handlers
template<size_t S>
void writeDummy(Memory& m, const uint32_t addr, const SizedUInt<S> data) {
    core::Log::warning("memory", fmt::format(core::tr("Write ({}) to unmapped area {:#0x} : {:#x}"), S, addr, data));
}

template<size_t S>
SizedUInt<S> readDummy(const Memory& m, const uint32_t addr) {
    core::Log::warning("memory", fmt::format(core::tr("Read ({}) from unmapped area {:#0x}"), S, addr));
    return SizedUInt<S>{};
}

// ROM handlers
template<size_t S>
SizedUInt<S> readRom(const Memory& m, const uint32_t addr) {
    return rawRead<S>(m.rom, addr & 0x7FFFF);
}

// SMPC handlers
template<size_t S>
SizedUInt<S> readSmpc(const Memory& m, const uint32_t addr) {
    return rawRead<S>(m.smpc, addr & 0x7F);
}

// Specialization for 8 bits data.
inline SizedUInt<8> readSmpc(const Memory& m, const uint32_t addr) {
    core::Log::error("memory", fmt::format(core::tr("Read ({}) needs to be handled through SMPC {:#0x}"), 8, addr));
    return 0;
}

template<size_t S>
void writeSmpc(Memory& m, const uint32_t addr, const SizedUInt<S> data) {
    rawWrite<S>(m.smpc, addr & 0x7F, data);
}

// Specialization for 8 bits data.
inline void writeSmpc(Memory& m, const uint32_t addr, const SizedUInt<8> data) {
    core::Log::warning("memory", fmt::format(core::tr("Write ({}) needs to be handled through SMPC {:#0x} : {:#x}"), 8, addr, data));
}

// Backup RAM handlers
template<size_t S>
SizedUInt<S> readBackupRam(const Memory& m, const uint32_t addr) {
    return rawRead<S>(m.backup_ram, addr & 0xFFFF);
}

template<size_t S>
void writeBackupRam(Memory& m, const uint32_t addr, const SizedUInt<S> data) {
    rawWrite<S>(m.backup_ram, addr & 0xFFFF, data);
}

// Low workram handlers
template<size_t S>
SizedUInt<S> readWorkramLow(const Memory& m, const uint32_t addr) {
    return rawRead<S>(m.workram_low, addr & 0xFFFFF);
}

template<size_t S>
void writeWorkramLow(Memory& m, const uint32_t addr, const SizedUInt<S> data) {
    rawWrite<S>(m.workram_low, addr & 0xFFFFF, data);
}

}
}