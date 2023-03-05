//
// scsp.cpp
// Saturnin
//
// Copyright (c) 2021 Renaud Toumazet
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

#include <saturnin/src/pch.h>
#include <saturnin/src/sound/scsp.h>
#include <saturnin/src/config.h>
#include <saturnin/src/interrupt_sources.h>
#include <saturnin/src/locale.h> // tr
#include <saturnin/src/memory.h>
extern "C" {
#include <saturnin/lib/musashi/m68k.h>        // Musashi
#include <saturnin/lib/scsp_stef/scsp_stef.h> // Stef's SCSP core
}

namespace saturnin::sound {

using saturnin::core::rawRead;
using saturnin::core::rawWrite;
using saturnin::core::tr;
using saturnin::utilities::swapEndianness;
using saturnin::utilities::swapWords;

constexpr auto sound_ram_mask               = u32{0x1FFFFF};
constexpr auto sound_ram_upper_boundary     = u32{0x80000};
constexpr auto scsp_registers_start_address = u32{0x100000};
constexpr auto m68k_frequency               = u32{11'289'600};
constexpr auto sample_frequency             = u32{44'100};
constexpr auto m68k_cycles_per_sample       = u32{m68k_frequency / sample_frequency};

EmulatorModules Scsp::external_access_modules_;

void Scsp::initialize() {
    // scsp_init(Scsp::ram().data(), Scsp::scsp68kInterruptHandler, Scsp::scspHostInterruptHandler);
    scsp_init(Scsp::ram().data(),
              reinterpret_cast<void*>(Scsp::scsp68kInterruptHandler),
              reinterpret_cast<void*>(Scsp::scspHostInterruptHandler));
    m68k_set_cpu_type(M68K_CPU_TYPE_68000);
    reset();
}

void Scsp::reset() {
    m68k_pulse_reset();
    calculateSamplesPerFrame();
    calculate68KCyclesRatio();
}

void Scsp::run(const u32 cycles) {
    if (modules_.smpc()->isSoundOn()) {
        const auto m68k_cycles = static_cast<u32>(cycles / m68k_cycles_ratio_);
        elapsed_68k_cycles_ += m68k_cycles;

        // timers update
        const auto elapsed_samples = elapsed_68k_cycles_ / m68k_cycles_per_sample;
        scsp_update_timer(elapsed_samples);
        elapsed_68k_cycles_ %= m68k_cycles_per_sample;

        m68k_execute(m68k_cycles);
    }
}

void Scsp::scspHostInterruptHandler() {
    using namespace saturnin::core::interrupt_source;
    using saturnin::core::StartingFactorSelect;

    Log::debug(Logger::scsp, tr("Interrupt request"));
    external_access_modules_.scu()->generateInterrupt(sound_request);
    external_access_modules_.scu()->sendStartFactor(StartingFactorSelect::sound_req);
}

void Scsp::scsp68kInterruptHandler(const u32 level) { m68k_set_irq(level); }

auto Scsp::read8(const u32 addr) const -> u8 {
    auto local_addr = addr & sound_ram_mask;
    if (local_addr < sound_ram_upper_boundary) { return rawRead<u8>(Scsp::ram(), local_addr ^ 1); }
    if (local_addr >= scsp_registers_start_address) { return scsp_r_b(local_addr); }
    return 0;
}

auto Scsp::read16(const u32 addr) const -> u16 {
    auto local_addr = addr & sound_ram_mask;
    if (local_addr < sound_ram_upper_boundary) { return swapEndianness<u16>(rawRead<u16>(Scsp::ram(), local_addr)); }
    if (local_addr >= scsp_registers_start_address) { return scsp_r_w(local_addr); }
    return 0;
}

auto Scsp::read32(const u32 addr) const -> u32 {
    auto local_addr = addr & sound_ram_mask;
    if (local_addr < sound_ram_upper_boundary) { return swapWords(swapEndianness<u32>(rawRead<u32>(Scsp::ram(), local_addr))); }
    if (local_addr >= scsp_registers_start_address) { return scsp_r_d(local_addr); }
    return 0;
}

void Scsp::write8(const u32 addr, const u8 data) const {
    auto local_addr = addr & sound_ram_mask;
    if (local_addr < sound_ram_upper_boundary) {
        rawWrite<u8>(Scsp::ram(), local_addr ^ 1, data);
    } else if (local_addr >= scsp_registers_start_address) {
        scsp_w_b(local_addr, data);
    }
}

void Scsp::write16(const u32 addr, const u16 data) const {
    auto local_addr = addr & sound_ram_mask;
    if (local_addr < sound_ram_upper_boundary) {
        rawWrite<u16>(Scsp::ram(), local_addr, swapEndianness<u16>(data));
    } else if (local_addr >= scsp_registers_start_address) {
        scsp_w_w(local_addr, data);
    }
}

void Scsp::write32(const u32 addr, const u32 data) const {
    auto local_addr = addr & sound_ram_mask;
    if (local_addr < sound_ram_upper_boundary) {
        rawWrite<u32>(Scsp::ram(), local_addr, swapEndianness<u32>(swapWords(data)));
    } else if (local_addr >= scsp_registers_start_address) {
        scsp_w_d(local_addr, data);
    }
}

void Scsp::calculateSamplesPerFrame() {
    using namespace saturnin::core;
    constexpr auto sample_rate            = u16{44100};
    constexpr auto pal_frames_per_second  = u8{50};
    constexpr auto ntsc_frames_per_second = u8{60};

    std::string ts = modules_.config()->readValue(AccessKeys::cfg_rendering_tv_standard);
    switch (modules_.config()->getTvStandard(ts)) {
        using enum video::TvStandard;
        case pal: {
            samples_per_frame_ = sample_rate / pal_frames_per_second;
            break;
        }
        case ntsc: {
            samples_per_frame_ = sample_rate / ntsc_frames_per_second;
            break;
        }
        default: {
            Log::warning(Logger::scsp, tr("Could not calculate samples number per frame !"));
            samples_per_frame_ = 0;
        }
    }
}

void Scsp::calculate68KCyclesRatio() {
    const auto sh2_frequency = modules_.smpc()->getSystemClock();
    m68k_cycles_ratio_       = static_cast<float>(sh2_frequency) / static_cast<float>(m68k_frequency);
}

// Musashi functions
extern "C" void m68k_write_memory_8(u32 address, u32 value) {
    address &= sound_ram_mask;
    // if (address == 0x700) __debugbreak();

    if (address < sound_ram_upper_boundary) {
        rawWrite<u8>(Scsp::ram(), address ^ 1, value);
    } else if (address >= scsp_registers_start_address) {
        scsp_w_b(address, value);
    }
}

extern "C" void m68k_write_memory_16(u32 address, u32 value) {
    address &= sound_ram_mask;
    // if (address == 0x700) __debugbreak();
    if (address < sound_ram_upper_boundary) {
        rawWrite<u16>(Scsp::ram(), address, swapEndianness<u16>(value));
    } else if (address >= scsp_registers_start_address) {
        scsp_w_w(address, value);
    }
}

extern "C" void m68k_write_memory_32(u32 address, u32 value) {
    address &= sound_ram_mask;
    // if (address == 0x700) __debugbreak();
    if (address < sound_ram_upper_boundary) {
        rawWrite<u32>(Scsp::ram(), address, swapEndianness<u32>(swapWords(value)));
    } else if (address >= scsp_registers_start_address) {
        scsp_w_d(address, value);
    }
}

extern "C" auto m68k_read_memory_8(u32 address) -> u32 {
    address &= sound_ram_mask;
    // if (address == 0x700) __debugbreak();
    if (address < sound_ram_upper_boundary) { return rawRead<u8>(Scsp::ram(), address ^ 1); }
    if (address >= scsp_registers_start_address) { return scsp_r_b(address); }
    return 0;
}

extern "C" auto m68k_read_memory_16(u32 address) -> u32 {
    address &= sound_ram_mask;
    // if (address == 0x700) __debugbreak();
    if (address < sound_ram_upper_boundary) { return swapEndianness<u16>(rawRead<u16>(Scsp::ram(), address)); }
    if (address >= scsp_registers_start_address) { return scsp_r_w(address); }
    return 0;
}

extern "C" auto m68k_read_memory_32(u32 address) -> u32 {
    address &= sound_ram_mask;
    // if (address == 0x700) __debugbreak();
    if (address < sound_ram_upper_boundary) { return util::swapWords(swapEndianness<u32>(rawRead<u32>(Scsp::ram(), address))); }
    if (address >= scsp_registers_start_address) { return scsp_r_d(address); }
    return 0;
}
} // namespace saturnin::sound
