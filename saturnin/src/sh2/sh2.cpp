//
// sh2.cpp
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
#include <istream>
#include <saturnin/src/sh2/sh2.h>
#include <saturnin/src/config.h>
#include <saturnin/src/emulator_context.h>
#include <saturnin/src/interrupt_controller.h> //Interrupt
#include <saturnin/src/interrupt_sources.h>
#include <saturnin/src/scu.h>
#include <saturnin/src/sh2/basic_interpreter/sh2_instructions.h>
#include <saturnin/src/utilities.h>

namespace is = saturnin::core::interrupt_source;

namespace saturnin::sh2 {

using core::Config;
using core::Log;
using core::Logger;

// Do not remove, will be used !!!
// constexpr auto divu_normal_cycles_number   = u8{39};
// constexpr auto divu_overflow_cycles_number = u8{6};

Sh2::Sh2(Sh2Type st, core::EmulatorContext* ec) : modules_(ec), sh2_type_(st) { reset(); }

auto Sh2::readRegisters8(const u32 addr) const -> u8 {
    using Ipra   = Sh2Regs::Intc::Ipra;
    using Iprb   = Sh2Regs::Intc::Iprb;
    using Vcra   = Sh2Regs::Intc::Vcra;
    using Vcrb   = Sh2Regs::Intc::Vcrb;
    using Vcrc   = Sh2Regs::Intc::Vcrc;
    using Vcrd   = Sh2Regs::Intc::Vcrd;
    using Vcrwdt = Sh2Regs::Intc::Vcrwdt;
    using Icr    = Sh2Regs::Intc::Icr;
    using Frc    = Sh2Regs::Frt::Frc;
    using FrtIcr = Sh2Regs::Frt::Icr;

    switch (addr) {
        /////////////
        // 5. INTC //
        /////////////
        case interrupt_priority_level_setting_register_a: return static_cast<u8>(regs_.intc.ipra >> Ipra::HI_BYTE_SHFT);
        case interrupt_priority_level_setting_register_a + 1: return static_cast<u8>(regs_.intc.ipra >> Ipra::LO_BYTE_SHFT);
        case interrupt_priority_level_setting_register_b: return static_cast<u8>(regs_.intc.iprb >> Iprb::HI_BYTE_SHFT);
        case interrupt_priority_level_setting_register_b + 1: return static_cast<u8>(regs_.intc.iprb >> Iprb::LO_BYTE_SHFT);
        case vector_number_setting_register_a: return static_cast<u8>(regs_.intc.vcra >> Vcra::HI_BYTE_SHFT);
        case vector_number_setting_register_a + 1: return static_cast<u8>(regs_.intc.vcra >> Vcra::LO_BYTE_SHFT);
        case vector_number_setting_register_b: return static_cast<u8>(regs_.intc.vcrb >> Vcrb::HI_BYTE_SHFT);
        case vector_number_setting_register_b + 1: return static_cast<u8>(regs_.intc.vcrb >> Vcrb::LO_BYTE_SHFT);
        case vector_number_setting_register_c: return static_cast<u8>(regs_.intc.vcrc >> Vcrc::HI_BYTE_SHFT);
        case vector_number_setting_register_c + 1: return static_cast<u8>(regs_.intc.vcrc >> Vcrc::LO_BYTE_SHFT);
        case vector_number_setting_register_d: return static_cast<u8>(regs_.intc.vcrd >> Vcrd::HI_BYTE_SHFT);
        case vector_number_setting_register_d + 1: return static_cast<u8>(regs_.intc.vcrd >> Vcrd::LO_BYTE_SHFT);
        case vector_number_setting_register_wdt: return static_cast<u8>(regs_.intc.vcrwdt >> Vcrwdt::HI_BYTE_SHFT);
        case vector_number_setting_register_wdt + 1: return static_cast<u8>(regs_.intc.vcrwdt >> Vcrwdt::LO_BYTE_SHFT);
        case interrupt_control_register: return static_cast<u8>(regs_.intc.icr >> Icr::HI_BYTE_SHFT);
        case interrupt_control_register + 1: return static_cast<u8>(regs_.intc.icr >> Icr::LO_BYTE_SHFT);
        /////////////
        // 7. BSC //
        /////////////

        //////////////
        // 8. Cache //
        //////////////
        case cache_control_register: return regs_.cache.ccr.data();

        /////////////
        // 9. DMAC //
        /////////////
        case dma_request_response_selection_control_register_0: return regs_.dmac.drcr0.data();
        case dma_request_response_selection_control_register_1: return regs_.dmac.drcr1.data();
        //////////////
        // 10. DIVU //
        //////////////

        /////////////
        // 11. FRT //
        /////////////
        case timer_interrupt_enable_register: return regs_.frt.tier.data();
        case free_running_timer_control_status_register: return regs_.frt.ftcsr.data();
        case free_running_counter: return static_cast<u8>(regs_.frt.frc >> Frc::HI_BYTE_SHFT);
        case free_running_counter + 1: return static_cast<u8>(regs_.frt.frc >> Frc::LO_BYTE_SHFT);
        case timer_control_register: return regs_.frt.tcr.data();
        case timer_output_compare_control_register: return regs_.frt.tocr.data();
        case input_capture_register: return static_cast<u8>(regs_.frt.icr >> FrtIcr::HI_BYTE_SHFT);
        case input_capture_register + 1: return static_cast<u8>(regs_.frt.icr >> FrtIcr::LO_BYTE_SHFT);

        /////////////
        // 12. WDT //
        /////////////
        case watchdog_timer_control_status_register: return regs_.wdt.wtcsr.data();
        case watchdog_timer_counter: return regs_.wdt.wtcnt.data();
        case reset_control_status_register + 1: return regs_.wdt.rstcsr.data();

        /////////////
        // 13. SCI //
        /////////////
        case serial_mode_register: return regs_.sci.smr.data();
        case bit_rate_register: return regs_.sci.brr.data();
        case serial_control_register: return regs_.sci.scr.data();
        case transmit_data_register: return regs_.sci.tdr.data();
        case serial_status_register: return regs_.sci.ssr.data();
        case receive_data_register: return regs_.sci.rdr.data();

        //////////////////////////
        // 14. Power-Down modes //
        //////////////////////////
        case standby_control_register: return regs_.pdm.sbycr.data();

        default: unmappedAccess(addr); return 0;
    }
}

auto Sh2::readRegisters16(const u32 addr) const -> u16 {
    switch (addr) {
        /////////////
        // 5. INTC //
        /////////////
        case interrupt_priority_level_setting_register_a: return regs_.intc.ipra.data();
        case interrupt_priority_level_setting_register_b: return regs_.intc.iprb.data();
        case vector_number_setting_register_a: return regs_.intc.vcra.data();
        case vector_number_setting_register_b: return regs_.intc.vcrb.data();
        case vector_number_setting_register_c: return regs_.intc.vcrc.data();
        case vector_number_setting_register_d: return regs_.intc.vcrd.data();
        case vector_number_setting_register_wdt: return regs_.intc.vcrwdt.data();
        case interrupt_control_register: return regs_.intc.icr.data();
        case vector_number_setting_register_div:
            return static_cast<u16>(regs_.intc.vcrdiv >> Sh2Regs::Intc::Vcrdiv::HI_WORD_SHFT);
        case vector_number_setting_register_div + 2:
            return static_cast<u16>(regs_.intc.vcrdiv >> Sh2Regs::Intc::Vcrdiv::LO_WORD_SHFT);

        /////////////
        // 7. BSC //
        /////////////
        case bus_control_register1: return u16{0};
        case bus_control_register1 + 2: return static_cast<u16>(regs_.bsc.bcr1 >> Sh2Regs::Bsc::Bcr::LO_WORD_SHFT);
        case bus_control_register2: return u16{0};
        case bus_control_register2 + 2: return static_cast<u16>(regs_.bsc.bcr2 >> Sh2Regs::Bsc::Bcr::LO_WORD_SHFT);
        case wait_state_control_register: return u16{0};
        case wait_state_control_register + 2: return static_cast<u16>(regs_.bsc.wcr >> Sh2Regs::Bsc::Wcr::LO_WORD_SHFT);
        case individual_memory_control_register: return u16{0};
        case individual_memory_control_register + 2: return static_cast<u16>(regs_.bsc.mcr >> Sh2Regs::Bsc::Mcr::LO_WORD_SHFT);
        case refresh_timer_control_status_register: return u16{0};
        case refresh_timer_control_status_register + 2:
            return static_cast<u16>(regs_.bsc.rtcsr >> Sh2Regs::Bsc::Rtcsr::LO_WORD_SHFT);
        case refresh_timer_counter + 2: return static_cast<u16>(regs_.bsc.rtcnt >> Sh2Regs::Bsc::Rtcnt::LO_WORD_SHFT);
        case refresh_time_constant_register + 2: return static_cast<u16>(regs_.bsc.rtcor >> Sh2Regs::Bsc::Rtcor::LO_WORD_SHFT);

        //////////////
        // 8. Cache //
        //////////////
        // As this register is 8 bits wide, there shouldn't be 16 bits access
        // case cache_control_register: return cache_ccr_.get(bits_0_15);

        /////////////
        // 9. DMAC //
        /////////////

        //////////////
        // 10. DIVU //
        //////////////
        case division_control_register: {
            // if (divu_is_running_) divu_opcode_is_stalled_ = true;
            return static_cast<u16>(regs_.divu.dvcr >> Sh2Regs::Divu::Dvcr::HI_WORD_SHFT);
        }
        case division_control_register + 2: {
            // if (divu_is_running_) divu_opcode_is_stalled_ = true;
            return static_cast<u16>(regs_.divu.dvcr >> Sh2Regs::Divu::Dvcr::LO_WORD_SHFT);
        }

            /////////////
            // 11. FRT //
            /////////////

            /////////////
            // 12. WDT //
            /////////////

            /////////////
            // 13. SCI //
            /////////////

        default: unmappedAccess(addr); return 0;
    }
}

auto Sh2::readRegisters32(const u32 addr) const -> u32 {
    switch (addr) {
        /////////////
        // 5. INTC //
        /////////////
        case vector_number_setting_register_div: return regs_.intc.vcrdiv.data();
        case dma_vector_number_register_0: return regs_.intc.vcrdma0.data();
        case dma_vector_number_register_1: return regs_.intc.vcrdma1.data();

        /////////////
        // 7. BSC //
        /////////////
        // Upper 16 bits are 0x0000, but as they're initialized and never written to after that, no need to mask the return value.
        case bus_control_register1: return regs_.bsc.bcr1.data();
        case bus_control_register2: return regs_.bsc.bcr2.data();
        case wait_state_control_register: return regs_.bsc.wcr.data();
        case individual_memory_control_register: return regs_.bsc.mcr.data();
        case refresh_timer_control_status_register: return regs_.bsc.rtcsr.data();
        case refresh_timer_counter: return regs_.bsc.rtcnt.data();
        case refresh_time_constant_register: return regs_.bsc.rtcor.data();

        //////////////
        // 8. Cache //
        //////////////

        /////////////
        // 9. DMAC //
        /////////////
        case dma_source_address_register_0: return regs_.dmac.sar0.data();
        case dma_source_address_register_1: return regs_.dmac.sar1.data();
        case dma_destination_address_register_0: return regs_.dmac.dar0.data();
        case dma_destination_address_register_1: return regs_.dmac.dar1.data();
        case dma_tranfer_count_register_0: return regs_.dmac.tcr0.data();
        case dma_tranfer_count_register_1: return regs_.dmac.tcr0.data();
        case dma_channel_control_register_0: return regs_.dmac.chcr0.data();
        case dma_channel_control_register_1: return regs_.dmac.chcr1.data();
        case dma_operation_register: return regs_.dmac.dmaor.data();

        //////////////
        // 10. DIVU //
        //////////////
        case divisor_register: {
            // if (divu_is_running_) divu_opcode_is_stalled_ = true;
            return regs_.divu.dvsr.data();
        }
        case dividend_register_l_32_bits: {
            // if (divu_is_running_) divu_opcode_is_stalled_ = true;
            return regs_.divu.dvdnt.data();
        }
        case division_control_register: {
            // if (divu_is_running_) divu_opcode_is_stalled_ = true;
            return regs_.divu.dvcr.data();
        }
        case dividend_register_l: {
            // if (divu_is_running_) divu_opcode_is_stalled_ = true;
            return regs_.divu.dvdntl.data();
        }
        case dividend_register_l_shadow: {
            // if (divu_is_running_) divu_opcode_is_stalled_ = true;
            return regs_.divu.dvdntl_shadow.data();
        }
        case dividend_register_h: {
            // if (divu_is_running_) divu_opcode_is_stalled_ = true;
            return regs_.divu.dvdnth.data();
        }
        case dividend_register_h_shadow: {
            // if (divu_is_running_) divu_opcode_is_stalled_ = true;
            return regs_.divu.dvdnth_shadow.data();
        }
            /////////////
            // 11. FRT //
            /////////////

            /////////////
            // 12. WDT //
            /////////////

            /////////////
            // 13. SCI //
            /////////////

        default: unmappedAccess(addr); return 0;
    }
}

void Sh2::writeRegisters(u32 addr, u8 data) {
    using Ipra   = Sh2Regs::Intc::Ipra;
    using Iprb   = Sh2Regs::Intc::Iprb;
    using Vcra   = Sh2Regs::Intc::Vcra;
    using Vcrb   = Sh2Regs::Intc::Vcrb;
    using Vcrc   = Sh2Regs::Intc::Vcrc;
    using Vcrd   = Sh2Regs::Intc::Vcrd;
    using Vcrwdt = Sh2Regs::Intc::Vcrwdt;
    using Icr    = Sh2Regs::Intc::Icr;
    using Frc    = Sh2Regs::Frt::Frc;
    using Ocr    = Sh2Regs::Frt::Ocr;
    using Tocr   = Sh2Regs::Frt::Tocr;
    using FrtIcr = Sh2Regs::Frt::Icr;

    switch (addr) {
        /////////////
        // 5. INTC //
        /////////////
        case interrupt_priority_level_setting_register_a: regs_.intc.ipra.upd(Ipra::hiByte(data)); break;
        case interrupt_priority_level_setting_register_a + 1: regs_.intc.ipra.upd(Ipra::loByte(data)); break;
        case interrupt_priority_level_setting_register_b: regs_.intc.iprb.upd(Iprb::hiByte(data)); break;
        case interrupt_priority_level_setting_register_b + 1: regs_.intc.iprb.upd(Iprb::loByte(data)); break;
        case vector_number_setting_register_a: regs_.intc.vcra.upd(Vcra::hiByte(data)); break;
        case vector_number_setting_register_a + 1: regs_.intc.vcra.upd(Vcra::loByte(data)); break;
        case vector_number_setting_register_b: regs_.intc.vcrb.upd(Vcrb::hiByte(data)); break;
        case vector_number_setting_register_b + 1: regs_.intc.vcrb.upd(Vcrb::loByte(data)); break;
        case vector_number_setting_register_c: regs_.intc.vcrc.upd(Vcrc::hiByte(data)); break;
        case vector_number_setting_register_c + 1: regs_.intc.vcrc.upd(Vcrc::loByte(data)); break;
        case vector_number_setting_register_d: regs_.intc.vcrd.upd(Vcrd::hiByte(data)); break;
        case vector_number_setting_register_d + 1: break; // Read only
        case vector_number_setting_register_wdt: regs_.intc.vcrwdt.upd(Vcrwdt::hiByte(data)); break;
        case vector_number_setting_register_wdt + 1: regs_.intc.vcrwdt.upd(Vcrwdt::loByte(data)); break;
        case interrupt_control_register: regs_.intc.icr.upd(Icr::hiByte(data)); break;
        case interrupt_control_register + 1: regs_.intc.icr.upd(Icr::loByte(data)); break;

        /////////////
        // 7. BSC  //
        /////////////

        //////////////
        // 8. Cache //
        //////////////
        case cache_control_register:
            using Ccr = Sh2Regs::Cache::Ccr;
            Log::debug(Logger::sh2, "CCR byte write: {}", data);

            regs_.cache.ccr = data;
            if ((regs_.cache.ccr >> Ccr::cp_enum) == Ccr::CachePurge::cache_purge) {
                purgeCache();

                // cache purge bit is cleared after operation
                regs_.cache.ccr.clr(Ccr::cp);
            }
            break;

        /////////////
        // 9. DMAC //
        /////////////
        case dma_request_response_selection_control_register_0: regs_.dmac.drcr0 = data; break;
        case dma_request_response_selection_control_register_1: regs_.dmac.drcr1 = data; break;

        //////////////
        // 10. DIVU //
        //////////////

        /////////////
        // 11. FRT //
        /////////////
        case timer_interrupt_enable_register: regs_.frt.tier = data; break;
        case free_running_timer_control_status_register: regs_.frt.ftcsr = data; break;
        case free_running_counter: regs_.frt.frc.upd(Frc::hiByte(data)); break;
        case free_running_counter + 1: regs_.frt.frc.upd(Frc::loByte(data)); break;
        case output_compare_register:
            switch (regs_.frt.tocr >> Tocr::ocrs_enum) {
                using enum Tocr::OutputCompareRegisterSelect;
                case ocra: regs_.frt.ocra.upd(Ocr::hiByte(data)); break;
                case ocrb: regs_.frt.ocrb.upd(Ocr::hiByte(data)); break;
            }
            break;
        case output_compare_register + 1:
            switch (regs_.frt.tocr >> Tocr::ocrs_enum) {
                using enum Tocr::OutputCompareRegisterSelect;
                case ocra: regs_.frt.ocra.upd(Ocr::loByte(data)); break;
                case ocrb: regs_.frt.ocrb.upd(Ocr::loByte(data)); break;
            }
            break;
        case timer_control_register:
            using Tcr     = Sh2Regs::Frt::Tcr;
            regs_.frt.tcr = data;
            switch (regs_.frt.tcr >> Tcr::cks_enum) {
                using enum Tcr::FrtClockSelect;
                case internal_divided_by_8:
                    frt_clock_divisor_ = frt_clock_divisor_8;
                    frt_mask_          = frt_clock_divisor_mask_8;
                    break;
                case internal_divided_by_32:
                    frt_clock_divisor_ = frt_clock_divisor_32;
                    frt_mask_          = frt_clock_divisor_mask_32;
                    break;
                case internal_divided_by_128:
                    frt_clock_divisor_ = frt_clock_divisor_128;
                    frt_mask_          = frt_clock_divisor_mask_128;
                    break;
                case external: Log::warning(Logger::sh2, "FRT - External clock not implemented"); break;
            }
            break;
        case timer_output_compare_control_register: {
            constexpr auto access_mask = u8{0b00010011};
            regs_.frt.tocr             = (data & access_mask);
            break;
        }
        case input_capture_register: regs_.frt.icr.upd(FrtIcr::hiByte(data)); break;
        case input_capture_register + 1:
            regs_.frt.icr.upd(FrtIcr::loByte(data));
            break;

            /////////////
            // 12. WDT //
            /////////////

            /////////////
            // 13. SCI //
            /////////////

        case serial_mode_register: regs_.sci.smr = data; break;
        case bit_rate_register: regs_.sci.brr = data; break;
        case serial_control_register: regs_.sci.scr = data; break;
        case transmit_data_register: regs_.sci.tdr = data; break;
        case serial_status_register:
            regs_.sci.ssr = data;
            break;

            //////////////////////////
            // 14. Power-Down modes //
            //////////////////////////

        case standby_control_register:
            regs_.pdm.sbycr = data;
            Log::warning(Logger::sh2, "PWR - Standby control register write {:#0x}", data);
            break;

        default: unmappedAccess(addr, data); break;
    }
    // core::rawWrite<u8>(io_registers_, addr & sh2_memory_mask, data);
}

void Sh2::writeRegisters(u32 addr, u16 data) { // NOLINT(readability-convert-member-functions-to-static)
    using Icr    = Sh2Regs::Intc::Icr;
    using Vcrdiv = Sh2Regs::Intc::Vcrdiv;
    using Wcr    = Sh2Regs::Bsc::Wcr;
    using Mcr    = Sh2Regs::Bsc::Mcr;
    using Bcr    = Sh2Regs::Bsc::Bcr;
    using Rtcsr  = Sh2Regs::Bsc::Rtcsr;
    using Rtcnt  = Sh2Regs::Bsc::Rtcnt;
    using Rtcor  = Sh2Regs::Bsc::Rtcor;
    using Dvcr   = Sh2Regs::Divu::Dvcr;
    using Frc    = Sh2Regs::Frt::Frc;
    switch (addr) {
        /////////////
        // 5. INTC //
        /////////////
        case interrupt_priority_level_setting_register_a: regs_.intc.ipra = data; break;
        case interrupt_priority_level_setting_register_b: regs_.intc.iprb = data; break;
        case vector_number_setting_register_a: regs_.intc.vcra = data; break;
        case vector_number_setting_register_b: regs_.intc.vcrb = data; break;
        case vector_number_setting_register_c: regs_.intc.vcrc = data; break;
        case vector_number_setting_register_d: regs_.intc.vcrd = data; break;
        case vector_number_setting_register_wdt: regs_.intc.vcrwdt = data; break;
        case vector_number_setting_register_div: break; // Read only access
        case vector_number_setting_register_div + 2: regs_.intc.vcrdiv.upd(Vcrdiv::divuv(data)); break;
        case interrupt_control_register: {
            auto new_level         = Sh2Regs::Intc::IcrType{data}.any(Icr::nmil);
            auto old_level         = regs_.intc.icr.any(Icr::nmil);
            auto has_level_changed = (new_level != old_level);

            if ((regs_.intc.icr >> Icr::nmie_enum) == Icr::NmiEdgeDetection::rising) {
                if (has_level_changed) { Log::warning(Logger::sh2, "Rising edge NMI, not implemented !"); }
            } else {
                if (has_level_changed) { Log::warning(Logger::sh2, "Falling edge NMI, not implemented !"); }
            }

            // Will force exit from the Sleep instruction. Will have to be adapted using Power Down modes.
            is_nmi_registered_ = true;

            regs_.intc.icr = data;
        } break;

        /////////////
        // 7. BSC //
        /////////////
        case bus_control_register1 + 2: {
            constexpr auto write_mask = u16{0b0001111111110111};
            regs_.bsc.bcr1.upd(Bcr::loWord(data & write_mask));
            break;
        }
        case bus_control_register2 + 2: {
            constexpr auto write_mask = u16{0b0000000011111100};
            regs_.bsc.bcr2.upd(Bcr::loWord(data & write_mask));
            break;
        }
        case wait_state_control_register + 2: regs_.bsc.wcr.upd(Wcr::loWord(data)); break;
        case individual_memory_control_register + 2: regs_.bsc.mcr.upd(Mcr::loWord(data)); break;
        case refresh_timer_control_status_register + 2: {
            constexpr auto write_mask = u16{0b0000000011111000};
            regs_.bsc.rtcsr.upd(Rtcsr::loWord(data & write_mask));
            break;
        }
        case refresh_timer_counter + 2: {
            constexpr auto write_mask = u16{0b0000000011111111};
            regs_.bsc.rtcnt.upd(Rtcnt::loWord(data & write_mask));
            break;
        }
        case refresh_time_constant_register + 2: {
            constexpr auto write_mask = u16{0b0000000011111111};
            regs_.bsc.rtcor.upd(Rtcor::loWord(data & write_mask));

            break;
        }
        //////////////
        // 8. Cache //
        //////////////

        /////////////
        // 9. DMAC //
        /////////////

        //////////////
        // 10. DIVU //
        //////////////
        case division_control_register: break; // Read only access
        case division_control_register + 2: {
            // if (divu_is_running_) divu_opcode_is_stalled_ = true;
            static constexpr auto access_mask = u32{0b11};
            regs_.divu.dvcr.upd(Dvcr::loWord(data & access_mask));
            break;
        }

        /////////////
        // 11. FRT //
        /////////////
        case free_running_counter: regs_.frt.frc = data; break;
        case output_compare_register:
            using Tocr = Sh2Regs::Frt::Tocr;
            switch (regs_.frt.tocr >> Tocr::ocrs_enum) {
                using enum Tocr::OutputCompareRegisterSelect;
                case ocra: regs_.frt.ocra = data; break;
                case ocrb: regs_.frt.ocrb = data; break;
            }
            break;
        case input_capture_register: regs_.frt.icr = data; break;

        /////////////
        // 12. WDT //
        /////////////
        case watchdog_timer_control_status_register:
            regs_.wdt.wtcsr = static_cast<u8>(data >> 8);
            regs_.wdt.wtcnt = static_cast<u8>(data & 0xFF);
            break;
        case reset_control_status_register:
            regs_.wdt.rstcsr = static_cast<u8>(data >> 8);
            break;

            /////////////
            // 13. SCI //
            /////////////

        default:
            // core::rawWrite<u16>(io_registers_, addr & sh2_memory_mask, data);
            unmappedAccess(addr, data);
            break;
    }
}

void Sh2::writeRegisters(u32 addr, u32 data) {
    using Bcr   = Sh2Regs::Bsc::Bcr;
    using Wcr   = Sh2Regs::Bsc::Wcr;
    using Mcr   = Sh2Regs::Bsc::Mcr;
    using Rtcsr = Sh2Regs::Bsc::Rtcsr;
    using Rtcnt = Sh2Regs::Bsc::Rtcnt;
    using Rtcor = Sh2Regs::Bsc::Rtcor;
    using Dmaor = Sh2Regs::Dmac::Dmaor;

    constexpr auto allow_bsc_write_mask = u32{0xA55A0000};
    switch (addr) {
        /////////////
        // 5. INTC //
        /////////////
        case vector_number_setting_register_div: regs_.intc.vcrdiv = data; break;
        case dma_vector_number_register_0: regs_.intc.vcrdma0 = data; break;
        case dma_vector_number_register_1: regs_.intc.vcrdma1 = data; break;

        /////////////
        // 7. BSC //
        /////////////
        case bus_control_register1:
            if ((data & 0xFFFF0000) == allow_bsc_write_mask) {
                constexpr auto write_mask = u16{0b0001111111110111};
                regs_.bsc.bcr1.upd(Bcr::loWord(data & write_mask));
            }
            break;
        case bus_control_register2:
            if ((data & 0xFFFF0000) == allow_bsc_write_mask) {
                constexpr auto write_mask = u16{0b0000000011111100};
                regs_.bsc.bcr2.upd(Bcr::loWord(data & write_mask));
            }
            break;
        case wait_state_control_register:
            if ((data & 0xFFFF0000) == allow_bsc_write_mask) { regs_.bsc.wcr.upd(Wcr::loWord(data)); }
            break;
        case individual_memory_control_register:
            if ((data & 0xFFFF0000) == allow_bsc_write_mask) { regs_.bsc.mcr.upd(Mcr::loWord(data)); }
            break;
        case refresh_timer_control_status_register:
            if ((data & 0xFFFF0000) == allow_bsc_write_mask) {
                constexpr auto write_mask = u16{0b0000000011111000};
                regs_.bsc.rtcsr.upd(Rtcsr::loWord(data & write_mask));
            }
            break;
        case refresh_timer_counter:
            if ((data & 0xFFFF0000) == allow_bsc_write_mask) {
                constexpr auto write_mask = u16{0b0000000011111111};
                regs_.bsc.rtcnt.upd(Rtcnt::loWord(data & write_mask));
            }
            break;
        case refresh_time_constant_register:
            if ((data & 0xFFFF0000) == allow_bsc_write_mask) {
                constexpr auto write_mask = u16{0b0000000011111111};
                regs_.bsc.rtcor.upd(Rtcor::loWord(data & write_mask));
            }
            break;

        //////////////
        // 8. Cache //
        //////////////

        /////////////
        // 9. DMAC //
        /////////////
        case dma_source_address_register_0: regs_.dmac.sar0 = data; break;
        case dma_source_address_register_1: regs_.dmac.sar1 = data; break;
        case dma_destination_address_register_0: regs_.dmac.dar0 = data; break;
        case dma_destination_address_register_1: regs_.dmac.dar1 = data; break;
        case dma_tranfer_count_register_0: regs_.dmac.tcr0 = (data & 0x00FFFFFF); break;
        case dma_tranfer_count_register_1: regs_.dmac.tcr1 = (data & 0x00FFFFFF); break;
        case dma_channel_control_register_0:
            regs_.dmac.chcr0 = (data & 0x0000FFFF);
            executeDma();
            break;
        case dma_channel_control_register_1:
            regs_.dmac.chcr1 = (data & 0x0000FFFF);
            executeDma();
            break;
        case dma_operation_register: {
            auto new_priority = Sh2Regs::Dmac::DmaorType{data & 0xF} >> Dmaor::pr_enum;
            if (auto old_priority = regs_.dmac.dmaor >> Dmaor::pr_enum; new_priority != old_priority) {
                dmac_next_transfer_priority_ = (new_priority == Dmaor::PriorityMode::fixed)
                                                   ? DmaNextTransferPriority::channel_0_first
                                                   : DmaNextTransferPriority::channel_1_first;
            }
            regs_.dmac.dmaor = (data & 0xF);
            executeDma();
        } break;

        //////////////
        // 10. DIVU //
        //////////////
        case divisor_register: {
            // if (divu_is_running_) divu_opcode_is_stalled_ = true;
            regs_.divu.dvsr = data;
            break;
        }
        case dividend_register_l_32_bits: {
            // if (divu_is_running_) divu_opcode_is_stalled_ = true;
            regs_.divu.dvdnt = data;

            // ST-V needs some mirroring
            regs_.divu.dvdntl        = data;
            regs_.divu.dvdntl_shadow = data;

            // Sign extension for the upper 32 bits if needed

            ((data & sign_bit_32_mask) != 0) ? regs_.divu.dvdnth = u32_max : regs_.divu.dvdnth = 0;

            start32bitsDivision();
            runDivisionUnit(0);
            break;
        }
        case division_control_register: {
            // if (divu_is_running_) divu_opcode_is_stalled_ = true;
            regs_.divu.dvcr = data;
            break;
        }
        case dividend_register_l: {
            // if (divu_is_running_) divu_opcode_is_stalled_ = true;
            regs_.divu.dvdntl        = data;
            regs_.divu.dvdntl_shadow = data;

            start64bitsDivision();
            runDivisionUnit(0);
            break;
        }
        case dividend_register_h: {
            // if (divu_is_running_) divu_opcode_is_stalled_ = true;
            regs_.divu.dvdnth = data;
            break;
        }

            /////////////
            // 11. FRT //
            /////////////

            /////////////
            // 12. WDT //
            /////////////

            /////////////
            // 13. SCI //
            /////////////

        default:
            // core::rawWrite<u32>(io_registers_, addr & sh2_memory_mask, data);
            unmappedAccess(addr, data);
            break;
    }
}

void Sh2::write32CachePurgeArea([[maybe_unused]] const u32 addr, [[maybe_unused]] const u32 data) const {
    // TODO
    // Const will have to be removed
}

void Sh2::purgeCache() {
    // All the valid bits and LRU bits are initialized to 0
    constexpr auto cache_purge_mask = u32{0xFFFFFC0B};
    for (u8 i = 0; i < cache_lines_number; ++i) {
        // :WARNING: following code is untested
        auto data = core::rawRead<u32>(this->cache_addresses_, i);
        data &= cache_purge_mask;
        core::rawWrite<u32>(this->cache_addresses_, i, data);
    }
}

void Sh2::initializeOnChipRegisters() {
    // Interrupt Control
    regs_.intc.ipra    = {};
    regs_.intc.iprb    = {};
    regs_.intc.vcra    = {};
    regs_.intc.vcrb    = {};
    regs_.intc.vcrc    = {};
    regs_.intc.vcrd    = {};
    regs_.intc.vcrwdt  = {};
    regs_.intc.vcrdiv  = {}; // lower 16 bits are undefined
    regs_.intc.vcrdma0 = {}; // lower 8 bits are undefined
    regs_.intc.vcrdma1 = {}; // lower 8 bits are undefined
    regs_.intc.icr     = {};

    // Bus State Controler registers
    constexpr auto bsc_bcr1_master_default_value = u32{0x000003F0};
    constexpr auto bsc_bcr1_slave_default_value  = u32{0x000083F0};
    constexpr auto bsc_bcr2_default_value        = u32{0x000000FCu};
    constexpr auto bsc_wcr_default_value         = u32{0x0000AAFFu};
    const auto default_bcr1 = u32{(sh2_type_ == Sh2Type::master) ? bsc_bcr1_master_default_value : bsc_bcr1_slave_default_value};

    regs_.bsc.bcr1  = default_bcr1;
    regs_.bsc.bcr2  = bsc_bcr2_default_value;
    regs_.bsc.wcr   = bsc_wcr_default_value;
    regs_.bsc.mcr   = {};
    regs_.bsc.rtcsr = {};
    regs_.bsc.rtcnt = {};
    regs_.bsc.rtcor = {};

    // Cache registers
    regs_.cache.ccr = {};

    // Direct Memory Access Controler registers
    regs_.dmac.tcr0  = {}; // lower 24 bits are undefined
    regs_.dmac.tcr1  = {}; // lower 24 bits are undefined
    regs_.dmac.chcr0 = {};
    regs_.dmac.chcr1 = {};
    regs_.dmac.drcr0 = {};
    regs_.dmac.drcr1 = {};
    regs_.dmac.dmaor = {};

    // Division Unit
    regs_.divu.dvcr = {};

    // Free Running timer
    constexpr auto frt_ocra_default_value = u16{0xFFFF};
    constexpr auto frt_ocrb_default_value = u16{0xFFFF};
    constexpr auto frt_tier_default_value = u8{0x01};
    constexpr auto frt_tocr_default_value = u8{0xe0};

    regs_.frt.tier  = frt_tier_default_value;
    regs_.frt.ftcsr = {};
    regs_.frt.frc   = {};
    regs_.frt.ocra  = frt_ocra_default_value;
    regs_.frt.ocrb  = frt_ocrb_default_value;
    regs_.frt.tcr   = {};
    regs_.frt.tocr  = frt_tocr_default_value;

    // Watch Dog Timer
    constexpr auto wdt_wtcsr_default_value  = u8{0x18};
    constexpr auto wdt_rstcsr_default_value = u8{0x1F};

    regs_.wdt.wtcsr  = wdt_wtcsr_default_value;
    regs_.wdt.wtcnt  = {};
    regs_.wdt.rstcsr = wdt_rstcsr_default_value;

    // Serial Communication Interface
    constexpr auto sci_ssr_default_value = u8{0x84};

    regs_.sci.smr = {};
    regs_.sci.brr = u8_max;
    regs_.sci.scr = {};
    regs_.sci.tdr = u8_max;
    regs_.sci.ssr = sci_ssr_default_value;
    regs_.sci.rdr = {};

    // Power-Down Modes
    constexpr auto sbycr_default_value = u8{0x60};
    regs_.pdm.sbycr                    = sbycr_default_value;
}

void Sh2::powerOnReset() {
    constexpr auto pc_start_vector = u32{0x00000008};
    constexpr auto sp_start_vector = u32{0x0000000C};

    r_[sp_register_index] = modules_.memory()->read<u32>(sp_start_vector);
    vbr_                  = 0;
    regs_.sr              = {};
    regs_.sr.set(Sh2Regs::StatusRegister::i_default_value);
    gbr_  = 0;
    mach_ = 0;
    macl_ = 0;
    pr_   = 0;
    pc_   = modules_.memory()->read<u32>(pc_start_vector);

    for (u8 i = 0; i < general_registers_number; ++i) {
        r_[i] = 0;
    }

    if (is_binary_file_loaded_) {
        pc_                    = binary_file_start_address_;
        vbr_                   = 0x06000000;
        is_binary_file_loaded_ = false;
    }

    initializeOnChipRegisters();

    callstack_.clear();
}

void Sh2::start32bitsDivision() {
    // 32/32 division
    Log::debug(Logger::sh2, "32/32 division");

    // divu_opcode_is_stalled_ = false;

    // DVDNT is copied in DVDNTL and DVDNTH
    regs_.divu.dvdntl = regs_.divu.dvdnt.data();

    const auto dvdnt = regs_.divu.dvdnt.data();
    if (static_cast<s32>(dvdnt) < 0) {
        regs_.divu.dvdnth = u32_max;
    } else {
        regs_.divu.dvdnth = 0;
    }
    const auto dvsr = regs_.divu.dvsr.data();

    // Log::debug(Logger::sh2, "{:#x} / {:#x} -> {:#x},{:#x}", dvdnt, dvsr, pc_);

    divu_quot_ = 0;
    divu_rem_  = 0;
    if (!regs_.divu.dvsr.is(0)) {
        divu_quot_ = (s32)dvdnt / (s32)dvsr;
        divu_rem_  = (s32)dvdnt % (s32)dvsr;
        // Log::debug(Logger::sh2, "Quotient : {}, remainder : {}", divu_quot_, divu_rem_);
        Log::debug(Logger::sh2, "{:#x} / {:#x} -> {:#x}, {:#x}", dvdnt, dvsr, divu_quot_, divu_rem_);
    } else {
        // Log::debug(Logger::sh2, "Overflow detected !");
        Log::debug(Logger::sh2, "{:#x} / {:#x} -> Overflow detected !", dvdnt, dvsr);
        regs_.divu.dvcr.set(Sh2Regs::Divu::Dvcr::ovf);
    }

    // Overflow check
    const auto is_dvdnt_ovf = bool{(dvdnt & sign_bit_32_mask) != 0};
    const auto is_dvsr_ovf  = bool{(dvsr & sign_bit_32_mask) != 0};
    if (is_dvdnt_ovf && is_dvsr_ovf) {
        if ((divu_quot_ == INT32_MAX) && ((divu_rem_ & sign_bit_32_mask) != 0)) {
            // Log::debug(Logger::sh2, "Overflow detected !");
            Log::debug(Logger::sh2, "{:#x} / {:#x} -> Overflow detected !", dvdnt, dvsr);
            regs_.divu.dvcr.set(Sh2Regs::Divu::Dvcr::ovf);
        }
    }

    // 39 cycles for regular division, 6 cycles when overflow is detected
    // divu_remaining_cycles_ = (divu_dvcr_.get(DivisionControlRegister::overflow_flag) == OverflowFlag::overflow)
    //                             ? divu_overflow_cycles_number
    //                             : divu_normal_cycles_number;
    divu_remaining_cycles_ = 0;

    divu_is_running_ = true;
}

void Sh2::start64bitsDivision() {
    Log::debug(Logger::sh2, "64/32 division");

    // divu_opcode_is_stalled_ = false;

    const auto dvdntl = regs_.divu.dvdntl.data();
    const auto dvdnth = regs_.divu.dvdnth.data();
    const auto dvsr   = regs_.divu.dvsr.data();

    const auto dividend = (static_cast<u64>(dvdnth) << 32) | dvdntl;

    // Log::debug(Logger::sh2, "{} / {} {:x}", dividend, dvsr, pc_);

    // auto dvcr = DivisionControlRegister(io_registers_[division_control_register & sh2_memory_mask]);
    auto quotient  = s64{};
    auto remainder = s64{};
    // if (divu_dvsr_.raw != 0) {
    if (!regs_.divu.dvsr.is(0)) {
        quotient  = (s64)dividend / (s32)dvsr;
        remainder = (s64)dividend % (s32)dvsr;
        // Log::debug(Logger::sh2, "Quotient : {}, remainder : {}", quotient, remainder);
        Log::debug(Logger::sh2, "{:#x} / {:#x} -> {:#x}, {:#x}", dividend, dvsr, (u64)quotient, (u64)remainder);
    } else {
        // Log::debug(Logger::sh2, "Overflow detected !");
        Log::debug(Logger::sh2, "{:#x} / {:#x} -> Overflow detected !", dividend, dvsr);
        regs_.divu.dvcr.set(Sh2Regs::Divu::Dvcr::ovf);
    }

    // Overflow check
    const auto is_dvdnth_ovf = bool{(dvdnth & sign_bit_32_mask) != 0};
    const auto is_dvsr_ovf   = bool{(dvsr & sign_bit_32_mask) != 0};
    if (is_dvdnth_ovf && is_dvsr_ovf) {
        if ((quotient == INT32_MAX) && ((remainder & sign_bit_32_mask) != 0)) {
            // Log::debug(Logger::sh2, "Overflow detected !");
            Log::debug(Logger::sh2, "{:#x} / {:#x} -> Overflow detected !", dividend, dvsr);
            regs_.divu.dvcr.set(Sh2Regs::Divu::Dvcr::ovf);
        }
    }

    // 39 cycles for regular division, 6 cycles when overflow is detected
    // divu_remaining_cycles_ = (divu_dvcr_.get(DivisionControlRegister::overflow_flag) == OverflowFlag::overflow)
    //                             ? divu_overflow_cycles_number
    //                             : divu_normal_cycles_number;
    divu_remaining_cycles_ = 0;
    divu_quot_             = static_cast<s32>(quotient);
    divu_rem_              = static_cast<s32>(remainder);

    divu_is_running_ = true;
}

void Sh2::runInterruptController() {
    if (!is_interrupted_) {
        if (!pending_interrupts_.empty()) {
            const auto  interrupt_mask = (regs_.sr >> Sh2Regs::StatusRegister::i_shft);
            const auto& interrupt      = pending_interrupts_.front();
            if ((interrupt.level > interrupt_mask) || interrupt == is::nmi) {
                Log::debug(Logger::sh2,
                           "{} SH2 interrupt request {:#0x} {:#0x}, PC={:#0x}",
                           sh2_type_name_.at(sh2_type_),
                           interrupt.vector,
                           interrupt.level,
                           pc_);

                is_level_interrupted_[interrupt.level] = false;

                // SR and PC are saved to the stack.
                constexpr auto sr_stack_offset = u8{4};
                constexpr auto pc_stack_offset = u8{8};

                modules_.memory()->write(r_[sp_register_index] - sr_stack_offset, regs_.sr.data());
                modules_.memory()->write(r_[sp_register_index] - pc_stack_offset, pc_);

                r_[sp_register_index] = r_[sp_register_index] - 8; // Stack pointer is updated.

                // sr_.i = interrupt.level;
                regs_.sr.upd(Sh2Regs::StatusRegister::interruptMask(interrupt.level));

                if (interrupt != is::nmi) {
                    is_interrupted_    = true; // Entering interrupt mode.
                    current_interrupt_ = interrupt;
                    Log::debug(Logger::sh2,
                               "{} SH2 {} interrupt routine started, pc={:#0x}",
                               sh2_type_name_.at(sh2_type_),
                               interrupt.name,
                               pc_);
                }
                pc_ = modules_.memory()->read<u32>(interrupt.vector * 4 + vbr_);

                pending_interrupts_.pop_front(); // Interrupt is removed from the list.
            }
        }
    }
}

void Sh2::runDivisionUnit(const u8 cycles_to_run) {
    using Dvcr = Sh2Regs::Divu::Dvcr;
    divu_remaining_cycles_ -= cycles_to_run;
    if (divu_remaining_cycles_ == 0) {
        // auto dvcr = DivisionControlRegister(io_registers_[division_control_register & sh2_memory_mask]);
        if ((regs_.divu.dvcr >> Dvcr::ovf_enum) == Dvcr::OverflowFlag::overflow) {
            if ((regs_.divu.dvcr >> Dvcr::ovfie_enum) == Dvcr::InterruptRequestUponOverflow::enabled) {
                Log::debug(Logger::sh2, "DIVU - Sending division overflow interrupt");
                is::sh2_division_overflow.vector = static_cast<u8>(regs_.intc.vcrdiv >> Sh2Regs::Intc::Vcrdiv::DIVUV_SHFT);
                is::sh2_division_overflow.level  = static_cast<u8>(regs_.intc.ipra >> Sh2Regs::Intc::Ipra::DIVU_LEVEL_SHFT);
                sendInterrupt(is::sh2_division_overflow);
            }
            regs_.divu.dvcr.clr(Sh2Regs::Divu::Dvcr::ovf);
        } else {
            // Copy in DVDNTL and DVDNTH + ST-V mirroring
            regs_.divu.dvdnt         = divu_quot_;
            regs_.divu.dvdntl        = divu_quot_;
            regs_.divu.dvdntl_shadow = divu_quot_;

            regs_.divu.dvdnth        = divu_rem_;
            regs_.divu.dvdnth_shadow = divu_rem_;

            // if (divu_dvdnth_.toU32() == 0x16f0) modules_.context()->debugStatus(core::DebugStatus::paused);
        }
        divu_is_running_ = false;
        // divu_opcode_is_stalled_ = false;
    }
}

void Sh2::runFreeRunningTimer(const u8 cycles_to_run) {
    using Iprb                   = Sh2Regs::Intc::Iprb;
    using Vcrc                   = Sh2Regs::Intc::Vcrc;
    using Vcrd                   = Sh2Regs::Intc::Vcrd;
    using Tier                   = Sh2Regs::Frt::Tier;
    using Ftcsr                  = Sh2Regs::Frt::Ftcsr;
    const auto elapsed_cycles    = u32{frt_elapsed_cycles_ + cycles_to_run};
    const auto counter_increment = u32{elapsed_cycles / frt_clock_divisor_};
    // const auto cycles_remainder  = u32{elapsed_cycles % frt_clock_divisor_};

    if (counter_increment > 0) {
        const auto old_frc     = u32{regs_.frt.frc.data()};
        const auto current_frc = u32{old_frc + counter_increment};
        regs_.frt.frc          = static_cast<u16>(current_frc);

        frt_elapsed_cycles_ = elapsed_cycles & frt_mask_;

        // Checking overflow
        if (current_frc > u16_max) {
            regs_.frt.ftcsr.set(Ftcsr::ovf);
            if ((regs_.frt.tier >> Tier::ovie_enum) == Tier::TimerOverflowInterruptEnable::interrupt_request_enabled) {
                Log::debug(Logger::sh2, "FRT - Sending overflow interrupt");
                is::sh2_frt_overflow_flag_set.vector = static_cast<u8>(regs_.intc.vcrd >> Vcrd::FOVV_SHFT);
                is::sh2_frt_overflow_flag_set.level  = static_cast<u8>(regs_.intc.iprb >> Iprb::FRT_LEVEL_SHFT);
                sendInterrupt(is::sh2_frt_overflow_flag_set);
            }
        }

        // Checking comparison for OCRA
        const auto ocra = u32{regs_.frt.ocra.data()};
        if ((old_frc <= ocra) && (current_frc > ocra)) {
            regs_.frt.ftcsr.set(Ftcsr::ocfa);
            if ((regs_.frt.tier >> Tier::ociae_enum) == Tier::OutputCompareInterruptAEnable::interrupt_request_enabled) {
                Log::debug(Logger::sh2, "FRT - OCRA match");
                is::sh2_frt_output_compare_flag_a_set.vector = static_cast<u8>(regs_.intc.vcrc >> Vcrc::FOCV_SHFT);
                is::sh2_frt_output_compare_flag_a_set.level  = static_cast<u8>(regs_.intc.iprb >> Iprb::FRT_LEVEL_SHFT);
                sendInterrupt(is::sh2_frt_output_compare_flag_a_set);
            }
            if ((regs_.frt.ftcsr >> Ftcsr::cclra_enum) == Ftcsr::CounterClearA::clear_on_compare) { regs_.frt.frc = {}; }
        }

        // Checking comparison for OCRB
        const auto ocrb = u32{regs_.frt.ocrb.data()};
        if ((old_frc <= ocrb) && (current_frc > ocrb)) {
            regs_.frt.ftcsr.set(Ftcsr::ocfb);
            if ((regs_.frt.tier >> Tier::ocibe_enum) == Tier::OutputCompareInterruptBEnable::interrupt_request_enabled) {
                Log::debug(Logger::sh2, "FRT - OCRB match");
                is::sh2_frt_output_compare_flag_b_set.vector = static_cast<u8>(regs_.intc.vcrc >> Vcrc::FOCV_SHFT);
                is::sh2_frt_output_compare_flag_b_set.level  = static_cast<u8>(regs_.intc.iprb >> Iprb::FRT_LEVEL_SHFT);
                sendInterrupt(is::sh2_frt_output_compare_flag_b_set);
            }
        }

    } else {
        frt_elapsed_cycles_ += cycles_to_run;
    }
}

void Sh2::executeDma() {
    using Dmaor = Sh2Regs::Dmac::Dmaor;
    if ((regs_.dmac.dmaor >> Dmaor::dme_enum) == Dmaor::DmaMasterEnable::disabled) { return; }

    auto conf_channel_0{configureDmaTransfer(DmaChannel::channel_0)};
    auto conf_channel_1{configureDmaTransfer(DmaChannel::channel_1)};

    switch (dmac_next_transfer_priority_) {
        using enum DmaNextTransferPriority;
        case channel_0_first:
            executeDmaOnChannel(conf_channel_0);
            executeDmaOnChannel(conf_channel_1);
            break;
        case channel_1_first:
            executeDmaOnChannel(conf_channel_1);
            executeDmaOnChannel(conf_channel_0);
            break;
    }

    // if (!regs_.dmac.dmaor.any(Sh2Regs::Dmac::Dmaor::priority_round_robin)) {
    if ((regs_.dmac.dmaor >> Sh2Regs::Dmac::Dmaor::pr_enum) == Sh2Regs::Dmac::Dmaor::PriorityMode::fixed) {
        switch (dmac_next_transfer_priority_) {
            using enum DmaNextTransferPriority;
            case channel_0_first: dmac_next_transfer_priority_ = DmaNextTransferPriority::channel_1_first; break;
            case channel_1_first: dmac_next_transfer_priority_ = DmaNextTransferPriority::channel_0_first; break;
        }
    }
}

auto Sh2::dmaStartConditionsAreSatisfied(const DmaChannel dc) const
    -> bool { // NOLINT(readability-convert-member-functions-to-static)
    using Chcr  = Sh2Regs::Dmac::Chcr;
    using Dmaor = Sh2Regs::Dmac::Dmaor;
    // DE=1 TE=0 NMIF=0 AE=0
    switch (dc) {
        using enum DmaChannel;
        case channel_0: {
            auto de   = regs_.dmac.chcr0.any(Sh2Regs::Dmac::Chcr::de);
            auto te   = regs_.dmac.chcr0.any(Sh2Regs::Dmac::Chcr::te);
            auto nmif = regs_.dmac.dmaor.any(Dmaor::nmif);
            auto ae   = regs_.dmac.dmaor.any(Dmaor::ae);

            return (de && !te && !nmif && !ae);
        }
        case channel_1: {
            auto de   = regs_.dmac.chcr1.any(Sh2Regs::Dmac::Chcr::de);
            auto te   = regs_.dmac.chcr1.any(Sh2Regs::Dmac::Chcr::te);
            auto nmif = regs_.dmac.dmaor.any(Dmaor::nmif);
            auto ae   = regs_.dmac.dmaor.any(Dmaor::ae);

            return (de && !te && !nmif && !ae);
        }
        case channel_unknown: return false;
    }
    return false;
}

auto Sh2::configureDmaTransfer(const DmaChannel dc) const -> Sh2DmaConfiguration {
    auto conf = Sh2DmaConfiguration{};
    switch (dc) {
        using enum DmaChannel;
        case channel_0:
            conf.channel     = DmaChannel::channel_0;
            conf.counter     = regs_.dmac.tcr0.data() & 0x00FFFFFF;
            conf.source      = regs_.dmac.sar0.data();
            conf.destination = regs_.dmac.dar0.data();
            conf.chcr        = regs_.dmac.chcr0.data();
            conf.drcr        = regs_.dmac.drcr0.data();
            conf.interrupt   = is::sh2_dma_0_transfer_end;
            break;
        case channel_1:
            conf.channel     = DmaChannel::channel_1;
            conf.counter     = regs_.dmac.tcr1.data() & 0x00FFFFFF;
            conf.source      = regs_.dmac.sar1.data();
            conf.destination = regs_.dmac.dar1.data();
            conf.chcr        = regs_.dmac.chcr1.data();
            conf.drcr        = regs_.dmac.drcr1.data();
            conf.interrupt   = is::sh2_dma_1_transfer_end;
            break;
        default: Log::warning(Logger::sh2, "DMAC - Unknown DMA channel");
    }
    return conf;
}

void Sh2::executeDmaOnChannel(Sh2DmaConfiguration& conf) {
    using Drcr = Sh2Regs::Dmac::Drcr;
    using Chcr = Sh2Regs::Dmac::Chcr;
    if (dmaStartConditionsAreSatisfied(conf.channel)) {
        auto       sh2_type       = sh2_type_name_.at(sh2_type_);
        const auto channel_number = static_cast<u8>((conf.channel == DmaChannel::channel_0) ? 0 : 1);
        if ((conf.chcr >> Chcr::ar_enum) == Chcr::AutoRequestMode::module_request) {
            switch (conf.drcr >> Drcr::rs_enum) {
                using enum Drcr::ResourceSelect;
                case dreq: {
                    // External request is immediately executed without waiting for an external signal.
                    // Not sure how to implement this, could be interesting to check on the console ...
                    Log::debug(Logger::sh2, "DMAC ({}) - Channel {} external request for {} SH2", sh2_type, channel_number);
                    break;
                }
                case txi: {
                    Log::unimplemented("SH2 DMAC - Channel {} SCI transmit request for {} SH2 not implemented !",
                                       channel_number,
                                       sh2_type);
                    return;
                }
                case rxi: {
                    Log::unimplemented("SH2 DMAC - Channel {} SCI receive request for {} SH2 not implemented !",
                                       channel_number,
                                       sh2_type);
                    return;
                }
                default: {
                    Log::warning(Logger::sh2,
                                 "DMAC ({}) - Channel {} module request setting prohibited !",
                                 sh2_type,
                                 channel_number);
                    return;
                }
            }
        } else {
            Log::debug(Logger::sh2, "DMAC ({}) - Channel {} auto request", sh2_type, channel_number);
        }
        auto counter     = u32{conf.counter};
        auto source      = u32{conf.source};
        auto destination = u32{conf.destination};
        Log::debug(Logger::sh2, "DMAC ({}) - Channel {} transfer", sh2_type, channel_number);
        Log::debug(Logger::sh2, "PC={:#0x}", pc_);
        Log::debug(Logger::sh2, "Source:{:#0x}", source);
        Log::debug(Logger::sh2, "Destination:{:#0x}", destination);
        Log::debug(Logger::sh2, "Count:{:#0x}", counter);

        constexpr auto transfer_byte_size_1  = u8{0x1};
        constexpr auto transfer_byte_size_2  = u8{0x2};
        constexpr auto transfer_byte_size_4  = u8{0x4};
        constexpr auto transfer_byte_size_16 = u8{0x10};

        while (counter > 0) {
            auto transfer_size = u8{};
            switch (conf.chcr >> Chcr::ts_enum) {
                using enum Chcr::TransferSize;
                case one_byte_unit:
                    modules_.memory()->write<u8>(destination, modules_.memory()->read<u8>(source));
                    transfer_size = transfer_byte_size_1;
                    --counter;
                    break;
                case two_byte_unit:
                    modules_.memory()->write<u16>(destination, modules_.memory()->read<u16>(source));
                    transfer_size = transfer_byte_size_2;
                    --counter;
                    break;
                case four_byte_unit:
                    modules_.memory()->write<u32>(destination, modules_.memory()->read<u32>(source));
                    transfer_size = transfer_byte_size_4;
                    --counter;
                    break;
                case sixteen_byte_unit:
                    modules_.memory()->write<u32>(destination, modules_.memory()->read<u32>(source));
                    modules_.memory()->write<u32>(destination + 4, modules_.memory()->read<u32>(source + 4));
                    modules_.memory()->write<u32>(destination + 8, modules_.memory()->read<u32>(source + 8));
                    modules_.memory()->write<u32>(destination + 12, modules_.memory()->read<u32>(source + 12));
                    transfer_size = transfer_byte_size_16;
                    counter -= 4;
                    break;
            }

            switch (conf.chcr >> Chcr::sm_enum) {
                using enum Chcr::SourceAddressMode;
                case fixed: break;
                case incremented: source += transfer_size; break;
                case decremented: source -= transfer_size; break;
                case reserved: Log::warning(Logger::sh2, "Reserved source address mode used !");
            }

            // switch (toEnum<DestinationAddressMode>(conf.chcr.destination_address_mode)) {
            switch (conf.chcr >> Chcr::dm_enum) {
                using enum Chcr::DestinationAddressMode;
                case fixed: break;
                case incremented: destination += transfer_size; break;
                case decremented: destination -= transfer_size; break;
                case reserved: Log::warning(Logger::sh2, "Reserved destination address mode used !");
            }
        }

        switch (conf.channel) {
            case DmaChannel::channel_0:
                regs_.dmac.tcr0 = counter;
                regs_.dmac.sar0 = source;
                regs_.dmac.dar0 = destination;
                regs_.dmac.chcr0.set(Chcr::te);
                conf.interrupt.vector = static_cast<u8>(regs_.intc.vcrdma0 >> Sh2Regs::Intc::Vcrdma::VC_SHFT);
                break;
            case DmaChannel::channel_1:
                regs_.dmac.tcr1 = counter;
                regs_.dmac.sar1 = source;
                regs_.dmac.dar1 = destination;
                regs_.dmac.chcr1.set(Chcr::te);
                conf.interrupt.vector = static_cast<u8>(regs_.intc.vcrdma1 >> Sh2Regs::Intc::Vcrdma::VC_SHFT);
                break;
            case DmaChannel::channel_unknown: Log::warning(Logger::sh2, "Unknown DMA channel used !");
        }

        // if (toEnum<Sh2DmaInterruptEnable>(conf.chcr.interrupt_enable) == Sh2DmaInterruptEnable::enabled) {
        if ((conf.chcr >> Chcr::ie_enum) == Chcr::Sh2DmaInterruptEnable::enabled) {
            Log::debug(Logger::sh2, "DMAC ({}) - Sending DMA channel {} transfer end interrupt.", sh2_type, channel_number);
            conf.interrupt.level = static_cast<u8>(regs_.intc.ipra >> Sh2Regs::Intc::Ipra::DMAC_LEVEL_SHFT);
            sendInterrupt(conf.interrupt);
        }
    };
} // namespace saturnin::sh2

void Sh2::reset() {
    initializeOnChipRegisters();

    frt_elapsed_cycles_ = 0;
    frt_clock_divisor_  = frt_clock_divisor_8;
    frt_mask_           = frt_clock_divisor_mask_8;
}

void Sh2::sendInterrupt(const core::Interrupt& i) { // NOLINT(readability-convert-member-functions-to-static)
    if (i.level != 0) {
        if (pending_interrupts_.size() <= max_interrupt_number) {
            if (!is_level_interrupted_[i.level]) {
                is_level_interrupted_[i.level] = true;
                pending_interrupts_.push_front(i);

                // Sorting (greatest priority first)
                pending_interrupts_.sort();
                pending_interrupts_.reverse();

                Log::debug(Logger::sh2, "{} SH2 interrupt pending : {:#0x}", sh2_type_name_.at(sh2_type_), i.vector);
            }
        } else {
            // Max number of pending interrupts reached, nothing is added
            Log::debug(Logger::sh2, "Maximum number of pending interrupts reached");

            // When the interrupt is NMI, the lower priority interrupt is removed
            if (i.vector == is::vector_nmi) {
                pending_interrupts_.pop_back();
                pending_interrupts_.push_front(is::nmi);

                // Sorting (greatest priority first)
                pending_interrupts_.sort();
                pending_interrupts_.reverse();

                Log::debug(Logger::sh2, "NMI interrupt forced");
            }
        }
    }
}

void Sh2::sendInterruptCaptureSignal() {
    using Iprb  = Sh2Regs::Intc::Iprb;
    using Vcrc  = Sh2Regs::Intc::Vcrc;
    using Tier  = Sh2Regs::Frt::Tier;
    using Ftcsr = Sh2Regs::Frt::Ftcsr;
    if ((regs_.frt.tier >> Tier::icie_enum) == Tier::InterruptCaptureInterruptEnable::interrupt_request_enabled) {
        is::sh2_frt_input_capture.vector = static_cast<u8>(regs_.intc.vcrc >> Vcrc::FICV_SHFT);
        is::sh2_frt_input_capture.level  = static_cast<u8>(regs_.intc.iprb >> Iprb::FRT_LEVEL_SHFT);
        sendInterrupt(is::sh2_frt_input_capture);
    }

    regs_.frt.ftcsr.set(Ftcsr::icf);
    regs_.frt.icr = regs_.frt.frc.data();
}

auto Sh2::run() -> u8 {
    modules_.memory()->sh2_in_operation_ = sh2_type_;
    runInterruptController();
    current_opcode_ = modules_.memory()->read<u16>(pc_);
    basic_interpreter::execute(*this);
    // runDivisionUnit(cycles_elapsed_);
    runFreeRunningTimer(cycles_elapsed_);
    return cycles_elapsed_;
}

auto Sh2::getRegister(const Sh2Register reg) const -> u32 {
    switch (reg) {
        using enum Sh2Register;
        case pc: return pc_; break;
        case pr: return pr_; break;
        case macl: return macl_; break;
        case mach: return mach_; break;
        case vbr: return vbr_; break;
        case gbr: return gbr_; break;
        case sr: return regs_.sr.data(); break;
        case r0: return r_[0]; break;
        case r1: return r_[1]; break;
        case r2: return r_[2]; break;
        case r3: return r_[3]; break;
        case r4: return r_[4]; break;
        case r5: return r_[5]; break;
        case r6: return r_[6]; break;
        case r7: return r_[7]; break;
        case r8: return r_[8]; break;
        case r9: return r_[9]; break;
        case r10: return r_[10]; break;
        case r11: return r_[11]; break;
        case r12: return r_[12]; break;
        case r13: return r_[13]; break;
        case r14: return r_[14]; break;
        case r15: return r_[15]; break;
    }
    return 0;
}

void Sh2::addToCallstack(const u32 call_addr, const u32 return_addr) {
    callstack_.emplace_back(call_addr, return_addr);
    // if (emulatorContext()->debugStatus() == core::DebugStatus::wait_end_of_routine) { ++step_over_subroutine_depth_; }
}

void Sh2::popFromCallstack() {
    callstack_.pop_back();
    // if (emulatorContext()->debugStatus() == core::DebugStatus::wait_end_of_routine) { --step_over_subroutine_depth_; }
};

auto Sh2::callstack() -> std::vector<CallstackItem> {
    std::lock_guard lock(sh2_mutex_);
    return callstack_;
};

void Sh2::setBinaryFileStartAddress(const u32 val) {
    is_binary_file_loaded_     = true;
    binary_file_start_address_ = val;
}

bool sh2CoreSetup(core::Config* config) {
    switch (config->getCurrentSh2Core()) {
        using enum Sh2Core;
        case basic_interpreter: {
            sh2::basic_interpreter::initializeOpcodesLut();
            break;
        }
        case fast_interpreter: {
            break;
        }
        default: {
            Log::warning(Logger::sh2, "Unknown SH2 core !");
        }
    }

    return true;
}

auto isInstructionIllegal(const u16 inst) -> bool {
    // 'Illegal Slot' detection
    // Returns true if an ISI (illegal slot instruction) is detected
    return basic_interpreter::illegal_instruction_lut[inst];
}

void delaySlot(Sh2& s, const u32 addr) {
    // Algorithm :
    // Addr read and intruction fetch
    // if the instruction is BF,BT,BRA,BSR,JMP,JSR,RTS,RTE,TRAPA,BF/S,BT/S,BRAF or BSRF then
    //		-> illegal instruction slot
    // else
    //		Slot instruction execution
    // end

    auto current_inst_cycles = u32{s.cycles_elapsed_}; // We musn't forget the DS instruction count
    if (addr != ignored_delay_slot_address) { // Delay slot isn't detected after the Power On Reset (to prevent the "illegal
                                              // instruction slot")

        s.current_opcode_ = s.modules_.memory()->read<u16>(addr);

        if (isInstructionIllegal(s.current_opcode_)) {
            Log::error(Logger::sh2, "Illegal instruction slot");
            s.modules_.context()->emulationStatus(core::EmulationStatus::stopped);
        } else {
            // Delay slot instruction execution
            basic_interpreter::execute(s);
            s.cycles_elapsed_ += current_inst_cycles;
        }
    }
}

} // namespace saturnin::sh2
