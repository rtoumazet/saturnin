//
// memory_impl.cpp
// Saturnin
//
// Copyright (c) 2025 Renaud Toumazet
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

// memory.h template implementations

#pragma once

#include <saturnin/src/pch.h>
#include <saturnin/src/memory.h>
#include <saturnin/src/cdrom/cdrom.h>
#include <saturnin/src/sh2/sh2.h>
#include <saturnin/src/sound/scsp.h>
#include <saturnin/src/video/vdp1.h>
#include <saturnin/src/video/vdp2/vdp2.h>

namespace saturnin::core {

// readCdBlock
// Generic definition
template<typename T>
readCdBlock<T>::operator Memory::ReadType<T>() const {
    return [](const Memory& m, const u32 addr) -> T { return m.modules_.cdrom()->readRegisters<T>(addr); };
}

// Explicit instanciations
template struct readCdBlock<u8>;
template struct readCdBlock<u16>;
template struct readCdBlock<u32>;

// writeCdBlock
// Generic definition
template<typename T>
writeCdBlock<T>::operator Memory::WriteType<T>() const {
    return [](Memory& m, const u32 addr, const T data) { m.modules_.cdrom()->writeRegisters<T>(addr, data); };
}

// Explicit instanciations
template struct writeCdBlock<u8>;
template struct writeCdBlock<u16>;
template struct writeCdBlock<u32>;

// readScsp
// Generic definition
template<typename T>
readScsp<T>::operator Memory::ReadType<T>() const {
    return [](const Memory& m, const u32 addr) -> T { return m.modules_.scsp()->read<T>(addr); };
}

// Explicit instanciations
template struct readScsp<u8>;
template struct readScsp<u16>;
template struct readScsp<u32>;

// writeScsp
// Generic definition
template<typename T>
writeScsp<T>::operator Memory::WriteType<T>() const {
    return [](Memory& m, const u32 addr, const T data) { m.modules_.scsp()->write<T>(addr, data); };
}

// Explicit instanciations
template struct writeScsp<u8>;
template struct writeScsp<u16>;
template struct writeScsp<u32>;

// readVdp1Registers
// Generic definition
template<typename T>
readVdp1Registers<T>::operator Memory::ReadType<T>() const {
    return [](const Memory& m, const u32 addr) -> T { return m.modules_.vdp1()->readRegisters<T>(addr); };
}

// Explicit instanciations
template struct readVdp1Registers<u8>;
template struct readVdp1Registers<u16>;
template struct readVdp1Registers<u32>;

// writeVdp1Registers
// Generic definition
template<typename T>
writeVdp1Registers<T>::operator Memory::WriteType<T>() const {
    return [](Memory& m, const u32 addr, const T data) { m.modules_.vdp1()->writeRegisters<T>(addr, data); };
}

// Explicit instanciations
template struct writeVdp1Registers<u8>;
template struct writeVdp1Registers<u16>;
template struct writeVdp1Registers<u32>;

// readVdp2Registers
// Generic definition
template<typename T>
readVdp2Registers<T>::operator Memory::ReadType<T>() const {
    return [](const Memory& m, const u32 addr) -> T { return m.modules_.vdp2()->readRegisters<T>(addr); };
}

// Explicit instanciations
template struct readVdp2Registers<u8>;
template struct readVdp2Registers<u16>;
template struct readVdp2Registers<u32>;

// readVdp2Registers
// Generic definition
template<typename T>
writeVdp2Registers<T>::operator Memory::WriteType<T>() const {
    return [](Memory& m, const u32 addr, const T data) {
        m.modules_.vdp2()->writeRegisters(addr, data);
        // :TODO: handle bitmap update
    };
}

// Explicit instanciations
template struct writeVdp2Registers<u8>;
template struct writeVdp2Registers<u16>;
template struct writeVdp2Registers<u32>;

// readSh2Registers
// Generic definition
template<typename T>
readSh2Registers<T>::operator Memory::ReadType<T>() const {
    return [](const Memory& m, const u32 addr) -> T {
        if (isMasterSh2InOperation(m)) { return m.modules_.masterSh2()->readRegisters<T>(addr); }
        return m.modules_.slaveSh2()->readRegisters<T>(addr);
    };
}

// Explicit instanciations
template struct readSh2Registers<u8>;
template struct readSh2Registers<u16>;
template struct readSh2Registers<u32>;

// writeSh2Registers
// Generic definition
template<typename T>
writeSh2Registers<T>::operator Memory::WriteType<T>() const {
    return [](Memory& m, const u32 addr, const T data) {
        if (isMasterSh2InOperation(m)) { return m.modules_.masterSh2()->writeRegisters<T>(addr, data); }
        return m.modules_.slaveSh2()->writeRegisters<T>(addr, data);
    };
}
// Explicit instanciations
template struct writeSh2Registers<u8>;
template struct writeSh2Registers<u16>;
template struct writeSh2Registers<u32>;

// writeCachePurgeArea
// Generic definition
template<typename T>
writeCachePurgeArea<T>::operator Memory::WriteType<T>() const {
    return [](Memory& m, const u32 addr, const T data) {
        if (isMasterSh2InOperation(m)) { return m.modules_.masterSh2()->writeCachePurgeArea<T>(addr, data); }
        return m.modules_.slaveSh2()->writeCachePurgeArea<T>(addr, data);
    };
}
// Explicit instanciations
template struct writeCachePurgeArea<u8>;
template struct writeCachePurgeArea<u16>;
template struct writeCachePurgeArea<u32>;

// readCacheAddresses
// Generic definition
template<typename T>
readCacheAddresses<T>::operator Memory::ReadType<T>() const {
    return [](const Memory& m, const u32 addr) -> T {
        if (isMasterSh2InOperation(m)) { return m.modules_.masterSh2()->readCacheAddresses<T>(addr); }
        return m.modules_.slaveSh2()->readCacheAddresses<T>(addr);
    };
}
// Explicit instanciations
template struct readCacheAddresses<u8>;
template struct readCacheAddresses<u16>;
template struct readCacheAddresses<u32>;

// writeCacheAddresses
// Generic definition
template<typename T>
writeCacheAddresses<T>::operator Memory::WriteType<T>() const {
    return [](Memory& m, const u32 addr, const T data) {
        if (isMasterSh2InOperation(m)) { return m.modules_.masterSh2()->writeCacheAddresses<T>(addr, data); }
        return m.modules_.slaveSh2()->writeCacheAddresses<T>(addr, data);
    };
}
// Explicit instanciations
template struct writeCacheAddresses<u8>;
template struct writeCacheAddresses<u16>;
template struct writeCacheAddresses<u32>;

// readCacheData
// Generic definition
template<typename T>
readCacheData<T>::operator Memory::ReadType<T>() const {
    return [](const Memory& m, const u32 addr) -> T {
        if (isMasterSh2InOperation(m)) { return m.modules_.masterSh2()->readCacheData<T>(addr); }
        return m.modules_.slaveSh2()->readCacheData<T>(addr);
    };
}
// Explicit instanciations
template struct readCacheData<u8>;
template struct readCacheData<u16>;
template struct readCacheData<u32>;

// writeCacheData
// Generic definition
template<typename T>
writeCacheData<T>::operator Memory::WriteType<T>() const {
    return [](Memory& m, const u32 addr, const T data) {
        if (isMasterSh2InOperation(m)) { return m.modules_.masterSh2()->writeCacheData<T>(addr, data); }
        return m.modules_.slaveSh2()->writeCacheData<T>(addr, data);
    };
}
// Explicit instanciations
template struct writeCacheData<u8>;
template struct writeCacheData<u16>;
template struct writeCacheData<u32>;

} // namespace saturnin::core
