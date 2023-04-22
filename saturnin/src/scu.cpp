//
// scu.cpp
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

#include <saturnin/src/pch.h>
#include <saturnin/src/scu.h>
#include <istream>
#include <saturnin/src/emulator_context.h>
#include <saturnin/src/locale.h>
#include <saturnin/src/memory.h>
#include <saturnin/src/scu_registers.h>
#include <saturnin/src/sh2/sh2.h>
#include <saturnin/src/utilities.h> // format

namespace uti = saturnin::utilities;

namespace saturnin::core {

// SCU DMA accesses
// Write to A-Bus prohibited
// Read from VDP2 area prohibited (B-Bus)
// Write to VP1 registers (B-Bus) must use 2 bytes
// Access to Workram L not possible
// During DMA operation A -> B or B -> A, no CPU access to A-Bus

Scu::Scu(EmulatorContext* ec) : modules_(ec) { initializeRegisters(); };

auto Scu::read32(const u32 addr) const -> u32 {
    switch (addr) {
        case dsp_program_control_port: return regs_.ppaf.data();
        case dsp_program_ram_dataport: return regs_.ppd.data();
        case dsp_data_ram_address_port: return regs_.pda.data();
        case dsp_data_ram_data_port: return regs_.pdd.data();
        case level_0_dma_enable_register: return regs_.d0en.data();
        case level_1_dma_enable_register: return regs_.d1en.data();
        case level_2_dma_enable_register: return regs_.d2en.data();
        case level_0_dma_add_value_register: return regs_.d0ad.data();
        case level_1_dma_add_value_register: return regs_.d1ad.data();
        case level_2_dma_add_value_register: return regs_.d2ad.data();
        case level_0_dma_read_address: return regs_.d0r.data();
        case level_1_dma_read_address: return regs_.d1r.data();
        case level_2_dma_read_address: return regs_.d2r.data();
        case level_0_dma_write_address: return regs_.d0w.data();
        case level_1_dma_write_address: return regs_.d1w.data();
        case level_2_dma_write_address: return regs_.d2w.data();
        case level_0_dma_mode_register: return regs_.d0md.data();
        case level_1_dma_mode_register: return regs_.d1md.data();
        case level_2_dma_mode_register: return regs_.d2md.data();
        case level_0_dma_transfer_byte_number: return regs_.d0c.data();
        case level_1_dma_transfer_byte_number: return regs_.d1c.data();
        case level_2_dma_transfer_byte_number: return regs_.d2c.data();
        case interrupt_status_register: return regs_.ist.data();
        case interrupt_mask_register: return regs_.ims.data();
        default: return rawRead<u32>(modules_.memory()->scu_, addr & scu_memory_mask);
    }
    return rawRead<u32>(modules_.memory()->scu_, addr & scu_memory_mask);
};

void Scu::write32(const u32 addr, const u32 data) {
    using Dxen = ScuRegs::Dxen;
    using Ppaf = ScuRegs::Ppaf;

    switch (addr) {
        case dsp_program_control_port: {
            if ((regs_.ppaf >> Ppaf::t0_enum) == Ppaf::D0BusDmaExecution::dma_is_executing) {
                Log::unimplemented("SCU DSP - D0 Bus DMA execution");
            }

            if ((regs_.ppaf >> Ppaf::ex_enum) == Ppaf::ProgramExecuteControl::program_execution_begins) {
                Log::unimplemented("SCU DSP - Program execution");
                // #ifdef _DEBUGEMU
                //                 std::ostringstream oss;
                //                 oss << "DSP program execution started at address : " << showbase << hex <<
                //                 static_cast<uint32_t>((current_DSP_program_control_port_ & DSP_P));
                //                 EmuState::pLog->ScuDsp(oss.str());
                // #endif
                //                 PC_ = static_cast<uint8_t>((current_DSP_program_control_port_ & DSP_P));
                //                 DSPProgramDisasm(PC_);
                regs_.ppaf = {};                                       // DSP interception
                setInterruptStatusRegister(interrupt_source::dsp_end); // DSP interrupt flag
            }

            break;
        }
        case dsp_program_ram_dataport: {
            auto index = regs_.ppaf >> Ppaf::p_shft;
            rawWrite(program_ram_, index * 4, data);
            ++index;
            regs_.ppaf.upd(ScuRegs::Ppaf::programRamAddress(index));
            regs_.ppd = data;
            return;
        }
        case dsp_data_ram_address_port: {
            regs_.pda = data;
            return;
        }
        case dsp_data_ram_data_port: {
            regs_.pdd = data;
            return;
        }
        case level_0_dma_enable_register: {
            regs_.d0en = data;
            if ((regs_.d0en >> Dxen::dxen_enum) == Dxen::DmaEnable::enabled) {
                const auto dma_0_config = configureDmaTransfer(DmaLevel::level_0);
                addDmaToQueue(dma_0_config);
            }
            return;

            // if (DmaEnableRegister(data).get(DmaEnableRegister::dmaEnable) == DmaEnable::enabled) {
            //    auto dma_0_config = configureDmaTransfer(DmaLevel::level_0);
            //    //executeDma(dma_0_config);
            //    addDmaToQueue(dma_0_config);
            //}
            // break;
        }
        case level_1_dma_enable_register: {
            regs_.d1en = data;
            if ((regs_.d1en >> Dxen::dxen_enum) == Dxen::DmaEnable::enabled) {
                const auto dma_1_config = configureDmaTransfer(DmaLevel::level_1);
                addDmaToQueue(dma_1_config);
            }
            return;
        }
        case level_2_dma_enable_register: {
            regs_.d2en = data;
            if ((regs_.d2en >> Dxen::dxen_enum) == Dxen::DmaEnable::enabled) {
                const auto dma_2_config = configureDmaTransfer(DmaLevel::level_2);
                addDmaToQueue(dma_2_config);
            }
            return;
        }
        case level_0_dma_add_value_register: regs_.d0ad = data; return;
        case level_1_dma_add_value_register: regs_.d1ad = data; return;
        case level_2_dma_add_value_register: regs_.d2ad = data; return;
        case level_0_dma_read_address: regs_.d0r = data; return;
        case level_1_dma_read_address: regs_.d1r = data; return;
        case level_2_dma_read_address: regs_.d2r = data; return;
        case level_0_dma_write_address: regs_.d0w = data; return;
        case level_1_dma_write_address: regs_.d1w = data; return;
        case level_2_dma_write_address: regs_.d2w = data; return;
        case level_0_dma_mode_register: regs_.d0md = data; return;
        case level_1_dma_mode_register: regs_.d1md = data; return;
        case level_2_dma_mode_register: regs_.d2md = data; return;
        case level_0_dma_transfer_byte_number: regs_.d0c = data; return;
        case level_1_dma_transfer_byte_number: regs_.d1c = data; return;
        case level_2_dma_transfer_byte_number: regs_.d2c = data; return;
        case interrupt_status_register: regs_.ist = data; return;
        case interrupt_mask_register: regs_.ims = data; return;
        case timer_0_compare_register: regs_.t0c = data; return;
        case timer_1_set_data_register: regs_.t1s = data; return;
        case timer_1_mode_register:
            regs_.t1md = data;
            if (isTimer1Enabled()) { Log::warning(Logger::scu, tr("Timer 1 enabled")); }
            return;
        case dma_status_register:

            //            // DMA registers write
            //            dmaUpdate_ = true;
            break;
        default: break;
    }

    rawWrite<u32>(modules_.memory()->scu_, addr & scu_memory_mask, data);
};

void Scu::executeDma(DmaConfiguration& dc) {
    using Dxad = ScuRegs::Dxad;
    using Dxmd = ScuRegs::Dxmd;

    constexpr auto max_transfer_byte_number = u32{0x100000};
    constexpr auto address_add_0            = u8{0};
    constexpr auto address_add_2            = u8{2};
    constexpr auto address_add_4            = u8{4};
    constexpr auto address_add_8            = u8{8};
    constexpr auto address_add_16           = u8{16};
    constexpr auto address_add_32           = u8{32};
    constexpr auto address_add_64           = u8{64};
    constexpr auto address_add_128          = u8{128};

    switch (dc.dma_mode) {
        using enum Dxmd::DmaMode;
        case direct: {
            Log::debug(Logger::scu, "Direct Mode DMA - Level {}", utilities::toUnderlying<DmaLevel>(dc.dma_level));
            auto write_address     = u32{dc.write_address};
            auto read_address      = u32{dc.read_address};
            auto count             = u32{(dc.transfer_byte_number == 0) ? max_transfer_byte_number : dc.transfer_byte_number};
            auto read_address_add  = static_cast<u8>((dc.read_add_value == Dxad::ReadAddressAddValue::add_4) ? 4 : 0);
            auto write_address_add = u8{0};

            Log::debug(Logger::scu, "Read address : {:#x}", read_address);
            Log::debug(Logger::scu, "Write address : {:#x}", write_address);
            Log::debug(Logger::scu, "Size : {:#x}", count);
            Log::debug(Logger::scu, "Read add : {:#x}", read_address_add);

            switch (dc.write_add_value) {
                using enum Dxad::WriteAddressAddValue;
                case add_0: write_address_add = address_add_0; break;
                case add_2: write_address_add = address_add_2; break;
                case add_4: write_address_add = address_add_4; break;
                case add_8: write_address_add = address_add_8; break;
                case add_16: write_address_add = address_add_16; break;
                case add_32: write_address_add = address_add_32; break;
                case add_64: write_address_add = address_add_64; break;
                case add_128: write_address_add = address_add_128; break;
            }
            Log::debug(Logger::scu, "Write add : {:#x}", write_address_add);

            auto byte_counter = u32{};
            auto word_counter = u32{};
            auto long_counter = u32{};
            auto data         = u32{};
            auto read_offset  = u32{};

            switch (const auto write_bus = getDmaBus(dc.write_address)) {
                using enum DmaBus;
                case a_bus: {
                    Log::debug(Logger::scu, "A-Bus transfer");

                    switch (getScuRegion(read_address)) {
                        using enum ScuRegion;
                        case unknown: Log::warning(Logger::scu, tr("Unknown SCU region : {}"), read_address); break;
                        case a_bus_cs2: break;
                        default: read_address_add = 4;
                    }

                    switch (getScuRegion(write_address)) {
                        using enum ScuRegion;
                        case rom:
                        case smpc:
                        case backup_ram:
                        case work_ram_l:
                        case minit:
                        case sinit:
                        case a_bus_cs0:
                        case a_bus_cs1:
                        case a_bus_dummy: write_address_add = 4; break;

                        default:
                            if (write_address_add != 0) { write_address_add = 4; }
                            break;
                    }
                    // auto source_array = modules_.memory()->getArray(read_address);

                    while (byte_counter < dc.transfer_byte_number) {
                        data = modules_.memory()->read<u8>((read_address & 0x7FFFFFFFu) + read_offset
                                                           + long_counter * read_address_add);
                        modules_.memory()->write<u8>(write_address + read_offset + long_counter * write_address_add, data);

                        // auto source = (read_address & 0x7FFFFFFFu) + read_offset + long_counter * read_address_add;
                        // auto dest   = write_address + read_offset + long_counter * write_address_add;
                        // break;
                        ++read_offset;

                        if (read_offset == 4) {
                            read_offset = 0;
                            ++long_counter;
                        }
                        ++byte_counter;
                    }
                    break;
                }
                case b_bus: {
                    // B-Bus write
                    // 32 bits splitted into 2*16 bits
                    Log::debug(Logger::scu, "B-Bus transfer");

                    switch (getScuRegion(read_address)) {
                        using enum ScuRegion;
                        case unknown: Log::warning(Logger::scu, tr("Unknown SCU region : {}"), read_address); break;
                        case a_bus_cs2: break;
                        default: read_address_add = 4;
                    }

                    auto write_offset = u32{};

                    if ((read_address_add == 4) && (write_address_add == 2)) {
                        Log::debug(Logger::scu, "Burst copy");
                        modules_.memory()->burstCopy(read_address, write_address, dc.transfer_byte_number);
                    } else {
                        while (byte_counter < dc.transfer_byte_number) {
                            data = modules_.memory()->read<u8>((read_address & 0x7FFFFFFFu) + read_offset
                                                               + long_counter * read_address_add);
                            modules_.memory()->write<u8>(write_address + write_offset + word_counter * write_address_add, data);

                            ++read_offset;
                            ++write_offset;

                            if (read_offset == 4) {
                                read_offset = 0;
                                ++long_counter;
                            }
                            if (write_offset == 2) {
                                write_offset = 0;
                                ++word_counter;
                            }
                            ++byte_counter;
                        }
                    }
                    break;
                }
                case cpu_bus: {
                    Log::debug(Logger::scu, "CPU-Bus transfer");

                    switch (getScuRegion(read_address)) {
                        using enum ScuRegion;
                        case unknown: Log::warning(Logger::scu, tr("Unknown SCU region : {}"), read_address); break;
                        case a_bus_cs2: break;
                        default: read_address_add = 4;
                    }
                    write_address_add = 4;

                    while (byte_counter < dc.transfer_byte_number) {
                        data = modules_.memory()->read<u8>((read_address & 0x7FFFFFFFu) + read_offset
                                                           + long_counter * read_address_add);
                        modules_.memory()->write<u8>(write_address + read_offset + long_counter * write_address_add, data);

                        ++read_offset;

                        if (read_offset == 4) {
                            read_offset = 0;
                            ++long_counter;
                        }

                        ++byte_counter;
                    }
                    break;
                }
                case dsp_bus: {
                    Log::warning(Logger::scu, "DSP-Bus transfer - not implemented");
                    break;
                }
                default: Log::warning(Logger::scu, tr("Unknown DMA bus ! : {}"), dc.write_address); break;
            }

            if (dc.read_address_update == Dxmd::ReadAddressUpdate::update) { dc.read_address += long_counter * read_address_add; }

            if (dc.write_address_update == Dxmd::WriteAddressUpdate::update) {
                dc.write_address += word_counter * write_address_add;
            }

            sendDmaEndInterrupt(dc.dma_level);
            resetDmaEnable(dc);
            Log::debug(Logger::scu, "Level {} direct DMA completed", utilities::toUnderlying<DmaLevel>(dc.dma_level));

            break;
        }
        case indirect: {
            Log::debug(Logger::scu, "Indirect Mode DMA - Level {}", utilities::toUnderlying<DmaLevel>(dc.dma_level));
            constexpr auto storage_buffer_size            = u8{0xC};
            auto           execute_address_storage_buffer = u32{dc.write_address};

            auto read_address_add  = static_cast<u8>((dc.read_add_value == Dxad::ReadAddressAddValue::add_4) ? 4 : 0);
            auto write_address_add = u8{0};
            switch (dc.write_add_value) {
                using enum Dxad::WriteAddressAddValue;
                case add_0: write_address_add = address_add_0; break;
                case add_2: write_address_add = address_add_2; break;
                case add_4: write_address_add = address_add_4; break;
                case add_8: write_address_add = address_add_8; break;
                case add_16: write_address_add = address_add_16; break;
                case add_32: write_address_add = address_add_32; break;
                case add_64: write_address_add = address_add_64; break;
                case add_128: write_address_add = address_add_128; break;
            }

            auto write_address = u32{};
            auto read_address  = u32{};
            auto count         = u32{};
            //= (dc.transfer_byte_number == 0) ? 0x100000 : dc.transfer_byte_number;

            auto is_transfer_done = bool{false};
            while (!is_transfer_done) {
                read_address  = modules_.memory()->read<u32>(execute_address_storage_buffer + 8);
                write_address = modules_.memory()->read<u32>(execute_address_storage_buffer + 4);
                count         = modules_.memory()->read<u32>(execute_address_storage_buffer);
                if (count == 0) { count = max_transfer_byte_number; }

                Log::debug(Logger::scu, "Read address : {:#x}", read_address);
                Log::debug(Logger::scu, "Write address : {:#x}", write_address);
                Log::debug(Logger::scu, "Size : {:#x}", count);
                Log::debug(Logger::scu, "Read add : {:#x}", read_address_add);
                Log::debug(Logger::scu, "Write add : {:#x}", write_address_add);
                auto byte_counter = u32{};
                auto word_counter = u32{};
                auto long_counter = u32{};
                auto data         = u32{};
                auto read_offset  = u32{};

                const auto write_bus = getDmaBus(dc.write_address);
                switch (write_bus) {
                    using enum DmaBus;
                    case a_bus: {
                        Log::debug(Logger::scu, "A-Bus transfer");

                        switch (getScuRegion(read_address)) {
                            using enum ScuRegion;
                            case unknown: Log::warning(Logger::scu, tr("Unknown SCU region : {}"), read_address); break;
                            case a_bus_cs2: break;
                            default: read_address_add = 4;
                        }

                        switch (getScuRegion(write_address)) {
                            using enum ScuRegion;
                            case rom:
                            case smpc:
                            case backup_ram:
                            case work_ram_l:
                            case minit:
                            case sinit:
                            case a_bus_cs0:
                            case a_bus_cs1:
                            case a_bus_dummy: write_address_add = 4; break;

                            default:
                                if (write_address_add != 0) { write_address_add = 4; }
                                break;
                        }

                        while (byte_counter < count) {
                            data = modules_.memory()->read<u8>((read_address & 0x7FFFFFFFu) + read_offset
                                                               + long_counter * read_address_add);
                            modules_.memory()->write<u8>(write_address + read_offset + long_counter * write_address_add, data);

                            ++read_offset;

                            if (read_offset == 4) {
                                read_offset = 0;
                                ++long_counter;
                            }
                            ++byte_counter;
                        }
                        break;
                    }
                    case b_bus: {
                        // B-Bus write
                        // 32 bits splitted into 2*16 bits
                        Log::debug(Logger::scu, "B-Bus transfer");

                        switch (getScuRegion(read_address)) {
                            using enum ScuRegion;
                            case unknown: Log::warning(Logger::scu, tr("Unknown SCU region : {}"), read_address); break;
                            case a_bus_cs2: break;
                            default: read_address_add = 4;
                        }

                        u32 write_offset{};

                        while (byte_counter < count) {
                            data = modules_.memory()->read<u8>((read_address & 0x7FFFFFFFu) + read_offset
                                                               + long_counter * read_address_add);
                            modules_.memory()->write<u8>(write_address + write_offset + word_counter * write_address_add, data);

                            ++read_offset;
                            ++write_offset;

                            if (read_offset == 4) {
                                read_offset = 0;
                                ++long_counter;
                            }
                            if (write_offset == 2) {
                                write_offset = 0;
                                ++word_counter;
                            }
                            ++byte_counter;
                        }
                        break;
                    }
                    case cpu_bus: {
                        Log::debug(Logger::scu, "CPU-Bus transfer");

                        switch (getScuRegion(read_address)) {
                            using enum ScuRegion;
                            case unknown: Log::warning(Logger::scu, "Unknown SCU region : {}", read_address); break;
                            case a_bus_cs2: break;
                            default: read_address_add = 4;
                        }
                        write_address_add = 4;

                        while (byte_counter < count) {
                            data = modules_.memory()->read<u8>((read_address & 0x7FFFFFFFu) + read_offset
                                                               + long_counter * read_address_add);
                            modules_.memory()->write<u8>(write_address + read_offset + long_counter * write_address_add, data);

                            ++read_offset;

                            if (read_offset == 4) {
                                read_offset = 0;
                                ++long_counter;
                            }

                            ++byte_counter;
                        }
                        break;
                    }
                    case dsp_bus: {
                        Log::warning(Logger::scu, "DSP-Bus transfer - not implemented");
                        break;
                    }
                    default: Log::warning(Logger::scu, "Unknown DMA bus ! : {}", dc.write_address); break;
                }

                if ((read_address & indirect_dma_end_code) > 0) { is_transfer_done = true; }
                execute_address_storage_buffer += storage_buffer_size;
            }

            if (dc.write_address_update == Dxmd::WriteAddressUpdate::update) {
                dmaUpdateWriteAddress(dc.dma_level, execute_address_storage_buffer);
            }

            sendDmaEndInterrupt(dc.dma_level);
            resetDmaEnable(dc);
            Log::debug(Logger::scu, "Level {} indirect DMA completed", utilities::toUnderlying<DmaLevel>(dc.dma_level));

            break;
        }
        default: Log::warning(Logger::scu, "Unknown DMA mode !");
    }

    // SCU <-> B-Bus : 32 bits
    // B-Bus <-> CPU : 16 bits

    // CPU-BUS -> A-BUS
    // CPU-BUS <- A-BUS
    // CPU-BUS -> B-BUS
    // CPU-BUS <- B-BUS
    // A-BUS -> B-BUS
    // A-BUS <- B-BUS
    // DSP-BUS -> A-BUS
    // DSP-BUS <- A-BUS
    // DSP-BUS -> B-BUS
    // DSP-BUS <- B-BUS
    // DSP-BUS -> CPU-BUS
    // DSP-BUS <- CPU-BUS

    // If DMA enable bit is set and start factor occurs, DMA transfer is added to the queue
}

void Scu::setInterruptStatusRegister(const Interrupt& i) { regs_.ist.upd(i.status, ScuRegs::Ist::InterruptEnable::enabled); };

void Scu::resetInterruptStatusRegister(const Interrupt& i) { regs_.ist.upd(i.status, ScuRegs::Ist::InterruptEnable::disabled); };

void Scu::generateInterrupt(const Interrupt& i) {
    if (i.level != 0) {
        if (!isInterruptMasked(i)) {
            setInterruptStatusRegister(i);
            modules_.masterSh2()->sendInterrupt(i);
        }

        // There's no checking for the interrupt status in the SCU for the slave SH2, they are always sent.
        if (modules_.smpc()->isSlaveSh2On()) {
            switch (i.vector) {
                case is::vector_nmi:
                case is::vector_frt_input_capture:
                case is::vector_frt_input_capture2: modules_.slaveSh2()->sendInterrupt(i); break;
                case is::vector_v_blank_in: modules_.slaveSh2()->sendInterrupt(is::v_blank_in_slave); break;
                case is::vector_h_blank_in: modules_.slaveSh2()->sendInterrupt(is::h_blank_in_slave); break;
            }
        }
    }
};

auto Scu::isInterruptMasked(const Interrupt& i) -> bool {
    using Ims = ScuRegs::Ims;
    switch (i) {
        case is::vector_nmi: return true; // NMI interrupt is always accepted
        case is::vector_system_manager:
            if (modules_.context()->hardwareMode() == HardwareMode::stv) { return true; }
            return (regs_.ims >> i.mask) == Ims::InterruptMask::masked;
        case is::vector_v_blank_in:
        case is::vector_v_blank_out:
        case is::vector_h_blank_in:
        case is::vector_timer_0:
        case is::vector_timer_1:
        case is::vector_dsp_end:
        case is::vector_sound_request:
        case is::vector_pad_interrupt:
        case is::vector_level_2_dma_end:
        case is::vector_level_1_dma_end:
        case is::vector_level_0_dma_end:
        case is::vector_dma_illegal:
        case is::vector_sprite_draw_end:
        case is::vector_external_00:
        case is::vector_external_01:
        case is::vector_external_02:
        case is::vector_external_03:
        case is::vector_external_04:
        case is::vector_external_05:
        case is::vector_external_06:
        case is::vector_external_07:
        case is::vector_external_08:
        case is::vector_external_09:
        case is::vector_external_10:
        case is::vector_external_11:
        case is::vector_external_12:
        case is::vector_external_13:
        case is::vector_external_14:
        case is::vector_external_15: return (regs_.ims >> i.mask) == Ims::InterruptMask::masked;
        default: Log::warning(Logger::scu, uti::format("Unknown interrupt vector {}", i.vector));
    }
    return false;
}

auto Scu::isInterruptExecuting(const Interrupt& i) -> bool {
    using Ist = ScuRegs::Ist;
    return (regs_.ist >> i.status) == Ist::InterruptEnable::enabled;
}

void Scu::sendStartFactor(const ScuRegs::Dxmd::StartingFactorSelect sfs) {
    for (auto& dc : dma_queue_) {
        if (dc.dma_status == DmaStatus::waiting_start_factor) {
            if (dc.starting_factor_select == sfs) { dc.dma_status = DmaStatus::queued; }
        }
    }

    if (!dma_queue_.empty()) { activateDma(); }
}

void Scu::clearInterruptFlag(const Interrupt& i) {
    using Ist = ScuRegs::Ist;
    regs_.ist.upd(i.status, Ist::InterruptEnable::disabled);
}

void Scu::initializeRegisters() {
    // DMA
    constexpr auto address_add_default_value = u32{0x101};
    regs_.d0ad                               = address_add_default_value;
    regs_.d1ad                               = address_add_default_value;
    regs_.d2ad                               = address_add_default_value;
    regs_.d0en                               = {};
    regs_.d1en                               = {};
    regs_.d2en                               = {};
    constexpr auto mode_default_value        = u32{0x7};
    regs_.d0md                               = mode_default_value;
    regs_.d1md                               = mode_default_value;
    regs_.d2md                               = mode_default_value;
    rawWrite<u32>(modules_.memory()->scu_, dma_forced_stop & scu_memory_mask, 0x00000000);
    rawWrite<u32>(modules_.memory()->scu_, dma_status_register & scu_memory_mask, 0x00000000);

    // DSP
    rawWrite<u32>(modules_.memory()->scu_, dsp_program_control_port & scu_memory_mask, 0x00000000);
    rawWrite<u32>(modules_.memory()->scu_, dsp_data_ram_address_port & scu_memory_mask, 0x00000000);
    rawWrite<u32>(modules_.memory()->scu_, dsp_data_ram_data_port & scu_memory_mask, 0x00000000);

    // Timer
    regs_.t1md = {};

    // Interrupt control
    constexpr auto interrupt_mask_default_value = u32{0xBFFF};
    regs_.ims                                   = interrupt_mask_default_value;
    regs_.ist                                   = {};

    // A-BUS control
    rawWrite<u32>(modules_.memory()->scu_, a_bus_interrupt_acknowledge & scu_memory_mask, 0x00000000);
    rawWrite<u32>(modules_.memory()->scu_, a_bus_set_register & scu_memory_mask, 0x00000000);
    rawWrite<u32>(modules_.memory()->scu_, (a_bus_set_register + 4) & scu_memory_mask, 0x00000000);
    rawWrite<u32>(modules_.memory()->scu_, a_bus_refresh_register & scu_memory_mask, 0x00000000);

    // SCU control
    rawWrite<u32>(modules_.memory()->scu_, scu_sdram_select_register & scu_memory_mask, 0x00000000);
    rawWrite<u32>(modules_.memory()->scu_, scu_version_register & scu_memory_mask, 0x00000000);
}

auto Scu::configureDmaTransfer(const DmaLevel level) -> DmaConfiguration {
    auto dc      = DmaConfiguration{};
    dc.dma_level = level;
    switch (level) {
        using enum DmaLevel;
        case level_0:
            initializeDmaReadAddress(dc, regs_.d0r);
            initializeDmaWriteAddress(dc, regs_.d0w);
            initializeDmaTransferByteNumber(dc);
            initializeDmaAddressAdd(dc, regs_.d0ad);
            initializeDmaEnable(dc, regs_.d0en);
            initializeDmaMode(dc, regs_.d0md);
            break;
        case level_1:
            initializeDmaReadAddress(dc, regs_.d1r);
            initializeDmaWriteAddress(dc, regs_.d1w);
            initializeDmaTransferByteNumber(dc);
            initializeDmaAddressAdd(dc, regs_.d1ad);
            initializeDmaEnable(dc, regs_.d1en);
            initializeDmaMode(dc, regs_.d1md);
            break;
        case level_2:
            initializeDmaReadAddress(dc, regs_.d2r);
            initializeDmaWriteAddress(dc, regs_.d2w);
            initializeDmaTransferByteNumber(dc);
            initializeDmaAddressAdd(dc, regs_.d2ad);
            initializeDmaEnable(dc, regs_.d2en);
            initializeDmaMode(dc, regs_.d2md);
            break;
        default: break;
    }
    return dc;
}

/* static */
void Scu::initializeDmaMode(DmaConfiguration& dc, const ScuRegs::DxmdType& reg) {
    dc.dma_mode               = reg >> ScuRegs::Dxmd::dxmod_enum;
    dc.read_address_update    = reg >> ScuRegs::Dxmd::dxrup_enum;
    dc.write_address_update   = reg >> ScuRegs::Dxmd::dxwup_enum;
    dc.starting_factor_select = reg >> ScuRegs::Dxmd::dxft_enum;
}

/* static */
void Scu::initializeDmaEnable(DmaConfiguration& dc, const ScuRegs::DxenType& reg) {
    dc.dma_enable   = reg >> ScuRegs::Dxen::dxen_enum;
    dc.dma_starting = reg >> ScuRegs::Dxen::dxgo_enum;
}

/* static */
void Scu::initializeDmaAddressAdd(DmaConfiguration& dc, const ScuRegs::DxadType& reg) {
    dc.read_add_value  = reg >> ScuRegs::Dxad::dxra_enum;
    dc.write_add_value = reg >> ScuRegs::Dxad::dxwa_enum;
}

void Scu::initializeDmaTransferByteNumber(DmaConfiguration& dc) const {
    switch (dc.dma_level) {
        using enum DmaLevel;
        case level_0: dc.transfer_byte_number = regs_.d0c >> ScuRegs::D0c::tbn_shft; break;
        case level_1: dc.transfer_byte_number = regs_.d1c >> ScuRegs::D1c::tbn_shft; break;
        case level_2: dc.transfer_byte_number = regs_.d2c >> ScuRegs::D2c::tbn_shft; break;
        default: Log::warning(Logger::scu, "Unknown DMA level !"); break;
    }
}
/* static */
void Scu::initializeDmaWriteAddress(DmaConfiguration& dc, const ScuRegs::DxwType& reg) {
    dc.write_address = reg >> ScuRegs::Dxw::wa_shft;
}

/* static */
void Scu::initializeDmaReadAddress(DmaConfiguration& dc, const ScuRegs::DxrType& reg) {
    dc.read_address = reg >> ScuRegs::Dxr::ra_shft;
}

void Scu::addDmaToQueue(const DmaConfiguration& dc) {
    auto config = DmaConfiguration{dc};

    switch (config.starting_factor_select) {
        using enum ScuRegs::Dxmd::StartingFactorSelect;
        case dma_start_factor: config.dma_status = DmaStatus::queued; break;
        default: config.dma_status = DmaStatus::waiting_start_factor;
    }
    dma_queue_.push_back(config);
}

void Scu::activateDma() {
    using Dxen = ScuRegs::Dxen;

    // Check for emptiness is done before the call.

    dmaSort();

    // Timing is not handled for now, DMA transfer is immediate

    // This case should only happen when timing is handled
    if (dma_queue_[0].dma_status == DmaStatus::active) { return; }

    for (auto& dc : dma_queue_) {
        if (dc.dma_status == DmaStatus::queued) {
            executeDma(dc);
            if (dc.dma_enable == Dxen::DmaEnable::enabled) {
                dc.dma_status = DmaStatus::waiting_start_factor;
            } else {
                dc.dma_status = DmaStatus::finished;
            }
        }
    }
}

/* static */
auto Scu::getDmaBus(const u32 address) -> DmaBus {
    const auto a = u32{address & 0xFFFFFFF};

    switch (getScuRegion(a)) {
        using enum ScuRegion;
        case work_ram_h:
        case work_ram_l:
        case backup_ram:
        case rom:
        case smpc: {
            return DmaBus::cpu_bus;
        }
        case a_bus_cs0:
        case a_bus_cs1:
        case a_bus_dummy:
        case a_bus_cs2: {
            return DmaBus::a_bus;
        }
        case vdp1:
        case vdp2:
        case sound: {
            return DmaBus::b_bus;
        }
        default:
            // DSP bus not implemented
            Log::warning(Logger::scu, "DMA - Bus access not implemented {}", address);
            return DmaBus::unknown_bus;
    }
}

/* static */
auto Scu::getScuRegion(const u32 address) -> ScuRegion {
    const auto a = u32{address & 0xFFFFFFF};

    constexpr auto rom_start = u32{0};
    constexpr auto rom_end   = u32{0x80000};
    if ((a >= rom_start) && (a < rom_end)) { return ScuRegion::rom; }

    constexpr auto smpc_start = u32{0x100000};
    constexpr auto smpc_end   = u32{0x100080};
    if ((a >= smpc_start) && (a < smpc_end)) { return ScuRegion::smpc; }

    constexpr auto backup_ram_start = u32{0x180000};
    constexpr auto backup_ram_end   = u32{0x190000};
    if ((a >= backup_ram_start) && (a < backup_ram_end)) { return ScuRegion::backup_ram; }

    constexpr auto workram_l_start = u32{0x200000};
    constexpr auto workram_l_end   = u32{0x300000};
    if ((a >= workram_l_start) && (a < workram_l_end)) { return ScuRegion::work_ram_l; }

    constexpr auto minit_start = u32{0x1000000};
    constexpr auto minit_end   = u32{0x1000004};
    if ((a >= minit_start) && (a < minit_end)) { return ScuRegion::minit; }

    constexpr auto sinit_start = u32{0x1800000};
    constexpr auto sinit_end   = u32{0x1800004};
    if ((a >= sinit_start) && (a < sinit_end)) { return ScuRegion::sinit; }

    constexpr auto a_bus_cs0_start = u32{0x2000000};
    constexpr auto a_bus_cs0_end   = u32{0x4000000};
    if ((a >= a_bus_cs0_start) && (a < a_bus_cs0_end)) { return ScuRegion::a_bus_cs0; }

    constexpr auto a_bus_cs1_start = u32{0x4000000};
    constexpr auto a_bus_cs1_end   = u32{0x5000000};
    if ((a >= a_bus_cs1_start) && (a < a_bus_cs1_end)) { return ScuRegion::a_bus_cs1; }

    constexpr auto a_bus_dummy_start = u32{0x5000000};
    constexpr auto a_bus_dummy_end   = u32{0x5800000};
    if ((a >= a_bus_dummy_start) && (a < a_bus_dummy_end)) { return ScuRegion::a_bus_dummy; }

    constexpr auto a_bus_cs2_start = u32{0x5800000};
    constexpr auto a_bus_cs2_end   = u32{0x5900000};
    if ((a >= a_bus_cs2_start) && (a < a_bus_cs2_end)) { return ScuRegion::a_bus_cs2; }

    constexpr auto sound_start = u32{0x5A00000};
    constexpr auto sound_end   = u32{0x5B00EE4};
    if ((a >= sound_start) && (a < sound_end)) { return ScuRegion::sound; }

    constexpr auto vdp1_start_area_1 = u32{0x5C00000};
    constexpr auto vdp1_end_area_1   = u32{0x5CC0000};
    if ((a >= vdp1_start_area_1) && (a < vdp1_end_area_1)) { return ScuRegion::vdp1; }

    constexpr auto vdp1_start_area_2 = u32{0x5D00000};
    constexpr auto vdp1_end_area_2   = u32{0x5D00018};
    if ((a >= vdp1_start_area_2) && (a < vdp1_end_area_2)) { return ScuRegion::vdp1; }

    constexpr auto vdp2_start_area_1 = u32{0x5E00000};
    constexpr auto vdp2_end_area_1   = u32{0x5E80000};
    if ((a >= vdp2_start_area_1) && (a < vdp2_end_area_1)) { return ScuRegion::vdp2; }

    constexpr auto vdp2_start_area_2 = u32{0x5F00000};
    constexpr auto vdp2_end_area_2   = u32{0x5F01000};
    if ((a >= vdp2_start_area_2) && (a < vdp2_end_area_2)) { return ScuRegion::vdp2; }

    constexpr auto vdp2_start_area_3 = u32{0x5F80000};
    constexpr auto vdp2_end_area_3   = u32{0x5F80120};
    if ((a >= vdp2_start_area_3) && (a < vdp2_end_area_3)) { return ScuRegion::vdp2; }

    constexpr auto scu_regs_start = u32{0x5FE0000};
    constexpr auto scu_regs_end   = u32{0x5FE00D0};
    if ((a >= scu_regs_start) && (a < scu_regs_end)) { return ScuRegion::scu_register; }

    constexpr auto workram_h_start = u32{0x6000000};
    constexpr auto workram_h_end   = u32{0x6100000};
    if ((a >= workram_h_start) && (a < workram_h_end)) { return ScuRegion::work_ram_h; }

    return ScuRegion::unknown;
}

void Scu::dmaTest() {
    using Dxmd    = ScuRegs::Dxmd;
    auto dc       = DmaConfiguration{};
    dc.dma_status = DmaStatus::finished;
    dma_queue_.push_back(dc);
    dc.dma_status = DmaStatus::queued;
    dma_queue_.push_back(dc);
    dc.starting_factor_select = Dxmd::StartingFactorSelect::h_blank_in;
    dc.dma_status             = DmaStatus::waiting_start_factor;
    dma_queue_.push_back(dc);
    dc.starting_factor_select = Dxmd::StartingFactorSelect::h_blank_in;
    dma_queue_.push_back(dc);
    dc.starting_factor_select = Dxmd::StartingFactorSelect::v_blank_out;
    dma_queue_.push_back(dc);

    sendStartFactor(Dxmd::StartingFactorSelect::h_blank_in);
}

void Scu::sendDmaEndInterrupt(const DmaLevel l) {
    switch (l) {
        using enum DmaLevel;
        case level_0: generateInterrupt(interrupt_source::level_0_dma_end); break;
        case level_1: generateInterrupt(interrupt_source::level_1_dma_end); break;
        case level_2: generateInterrupt(interrupt_source::level_2_dma_end); break;
        case level_unknown: Log::warning(Logger::scu, "Unknown DMA level !");
    }
}

void Scu::resetDmaEnable(DmaConfiguration& dc) const {
    using Dxen = ScuRegs::Dxen;
    using Dxmd = ScuRegs::Dxmd;
    if (dc.dma_enable == Dxen::DmaEnable::enabled && dc.starting_factor_select == Dxmd::StartingFactorSelect::dma_start_factor) {
        dc.dma_enable = Dxen::DmaEnable::disabled;
    }
}

void Scu::dmaUpdateWriteAddress(const DmaLevel l, const u32 data) {
    switch (l) {
        using enum DmaLevel;
        case level_0: regs_.d0w = data; break;
        case level_1: regs_.d1w = data; break;
        case level_2: regs_.d2w = data; break;
        case level_unknown: Log::warning(Logger::scu, "Unknown DMA level !");
    }
}

void Scu::dmaSort() {
    std::ranges::sort(dma_queue_,
                      [](const DmaConfiguration& dc1, const DmaConfiguration& dc2) { return dc1.dma_status < dc2.dma_status; });
}

auto Scu::getTimer0CompareValue() const -> u32 { return regs_.t0c >> ScuRegs::T0c::t0c_shft; }

auto Scu::isTimer1Enabled() const -> bool {
    using T1md = ScuRegs::T1md;
    return (regs_.t1md >> T1md::tenb_enum) == T1md::TimerEnable::timer_operation_on;
};

void Scu::onVblankIn() {
    generateInterrupt(interrupt_source::v_blank_in);
    sendStartFactor(ScuRegs::Dxmd::StartingFactorSelect::v_blank_in);
};
void Scu::onVblankOut() {
    generateInterrupt(interrupt_source::v_blank_out);
    sendStartFactor(ScuRegs::Dxmd::StartingFactorSelect::v_blank_out);
};
void Scu::onHblankIn() {
    generateInterrupt(interrupt_source::h_blank_in);
    sendStartFactor(ScuRegs::Dxmd::StartingFactorSelect::h_blank_in);
};
void Scu::onTimer0() {
    generateInterrupt(interrupt_source::timer_0);
    sendStartFactor(ScuRegs::Dxmd::StartingFactorSelect::timer_0);
};

} // namespace saturnin::core
