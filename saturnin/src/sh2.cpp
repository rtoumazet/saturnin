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
#include <saturnin/src/sh2.h>
#include <saturnin/src/emulator_context.h>
#include <saturnin/src/interrupt_controller.h> //Interrupt
#include <saturnin/src/interrupt_sources.h>
#include <saturnin/src/scu.h>
#include <saturnin/src/sh2_instructions.h>
#include <saturnin/src/utilities.h>

namespace is = saturnin::core::interrupt_source;

namespace saturnin::sh2 {

using core::Log;
using core::Logger;

// Do not remove, will be used !!!
// constexpr auto divu_normal_cycles_number   = u8{39};
// constexpr auto divu_overflow_cycles_number = u8{6};

Sh2::Sh2(Sh2Type st, core::EmulatorContext* ec) : modules_(ec), sh2_type_(st) { reset(); }

auto Sh2::readRegisters8(const u32 addr) const -> u8 {
    switch (addr) {
        /////////////
        // 5. INTC //
        /////////////
        case interrupt_priority_level_setting_register_a:
            return static_cast<u8>(regs_.intc.ipra >> Sh2Regs::Intc::Ipra::HI_BYTE_SHFT);
        case interrupt_priority_level_setting_register_a + 1:
            return static_cast<u8>(regs_.intc.ipra >> Sh2Regs::Intc::Ipra::LO_BYTE_SHFT);
        case interrupt_priority_level_setting_register_b:
            return static_cast<u8>(regs_.intc.iprb >> Sh2Regs::Intc::Iprb::HI_BYTE_SHFT);
        case interrupt_priority_level_setting_register_b + 1:
            return static_cast<u8>(regs_.intc.iprb >> Sh2Regs::Intc::Iprb::LO_BYTE_SHFT);
        case vector_number_setting_register_a: return static_cast<u8>(regs_.intc.vcra >> Sh2Regs::Intc::Vcra::HI_BYTE_SHFT);
        case vector_number_setting_register_a + 1: return static_cast<u8>(regs_.intc.vcra >> Sh2Regs::Intc::Vcra::LO_BYTE_SHFT);
        case vector_number_setting_register_b: return static_cast<u8>(regs_.intc.vcrb >> Sh2Regs::Intc::Vcrb::HI_BYTE_SHFT);
        case vector_number_setting_register_b + 1: return static_cast<u8>(regs_.intc.vcrb >> Sh2Regs::Intc::Vcrb::LO_BYTE_SHFT);
        case vector_number_setting_register_c: return static_cast<u8>(regs_.intc.vcrc >> Sh2Regs::Intc::Vcrc::HI_BYTE_SHFT);
        case vector_number_setting_register_c + 1: return static_cast<u8>(regs_.intc.vcrc >> Sh2Regs::Intc::Vcrc::LO_BYTE_SHFT);
        case vector_number_setting_register_d: return static_cast<u8>(intc_vcrd_.upper_8_bits);
        case vector_number_setting_register_d + 1: return intc_vcrd_.lower_8_bits;
        case vector_number_setting_register_wdt: return static_cast<u8>(intc_vcrwdt_.upper_8_bits);
        case vector_number_setting_register_wdt + 1: return intc_vcrwdt_.lower_8_bits;
        case interrupt_control_register: return static_cast<u8>(intc_icr_.upper_8_bits);
        case interrupt_control_register + 1: return intc_icr_.lower_8_bits;
        /////////////
        // 7. BSC //
        /////////////

        //////////////
        // 8. Cache //
        //////////////
        case cache_control_register: return cache_ccr_.raw;

        /////////////
        // 9. DMAC //
        /////////////
        case dma_request_response_selection_control_register_0: return dmac_drcr0_.raw;
        case dma_request_response_selection_control_register_1: return dmac_drcr1_.raw;
        //////////////
        // 10. DIVU //
        //////////////

        /////////////
        // 11. FRT //
        /////////////
        case timer_interrupt_enable_register: return frt_tier_.raw;
        case free_running_timer_control_status_register: return frt_ftcsr_.raw;
        case free_running_counter: return static_cast<u8>(frt_frc_.upper_8_bits);
        case free_running_counter + 1: return frt_frc_.lower_8_bits;
        case timer_control_register: return frt_tcr_.raw;
        case timer_output_compare_control_register: return frt_tocr_.raw;
        case input_capture_register: return static_cast<u8>(frt_icr_.upper_8_bits);
        case input_capture_register + 1: return frt_icr_.lower_8_bits;

        /////////////
        // 12. WDT //
        /////////////
        case watchdog_timer_control_status_register: return wdt_wtcsr_.raw;
        case watchdog_timer_counter: return wdt_wtcnt_.raw;
        case reset_control_status_register + 1: return wdt_rstcsr_.raw;

        /////////////
        // 13. SCI //
        /////////////
        case serial_mode_register: return sci_smr_.raw;
        case bit_rate_register: return sci_brr_.raw;
        case serial_control_register: return sci_scr_.raw;
        case transmit_data_register: return sci_tdr_.raw;
        case serial_status_register: return sci_ssr_.raw;
        case receive_data_register: return sci_rdr_.raw;

        //////////////////////////
        // 14. Power-Down modes //
        //////////////////////////
        case standby_control_register: return sbycr_.raw;

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
        case vector_number_setting_register_d: return intc_vcrd_.raw;
        case vector_number_setting_register_wdt: return intc_vcrwdt_.raw;
        case interrupt_control_register: return intc_icr_.raw;
        case vector_number_setting_register_div: return intc_vcrdiv_.upper_16_bits;
        case vector_number_setting_register_div + 2: return intc_vcrdiv_.lower_16_bits;

        /////////////
        // 7. BSC //
        /////////////
        case bus_control_register1: return u16{0};
        case bus_control_register1 + 2: return bsc_bcr1_.lower_16_bits;
        case bus_control_register2: return u16{0};
        case bus_control_register2 + 2: return bsc_bcr2_.lower_16_bits;
        case wait_state_control_register: return u16{0};
        case wait_state_control_register + 2: return bsc_wcr_.lower_16_bits;
        case individual_memory_control_register: return u16{0};
        case individual_memory_control_register + 2: return bsc_mcr_.lower_16_bits;
        case refresh_timer_control_status_register: return u16{0};
        case refresh_timer_control_status_register + 2: return bsc_rtcsr_.lower_16_bits;
        case refresh_timer_counter + 2: return bsc_rtcnt_.lower_16_bits;
        case refresh_time_constant_register + 2: return bsc_rtcor_.lower_16_bits;

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
            return divu_dvcr_.upper_16_bits;
        }
        case division_control_register + 2: {
            // if (divu_is_running_) divu_opcode_is_stalled_ = true;
            return divu_dvcr_.lower_16_bits;
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
        case vector_number_setting_register_div: return intc_vcrdiv_.raw;
        case dma_vector_number_register_0: return intc_vcrdma0_.raw;
        case dma_vector_number_register_1: return intc_vcrdma1_.raw;

        /////////////
        // 7. BSC //
        /////////////
        // Upper 16 bits are 0x0000, but as they're initialized and never written to after that, no need to mask the return value.
        case bus_control_register1: return bsc_bcr1_.raw;
        case bus_control_register2: return bsc_bcr2_.raw;
        case wait_state_control_register: return bsc_wcr_.raw;
        case individual_memory_control_register: return bsc_mcr_.raw;
        case refresh_timer_control_status_register: return bsc_rtcsr_.raw;
        case refresh_timer_counter: return bsc_rtcnt_.raw;
        case refresh_time_constant_register: return bsc_rtcor_.raw;

        //////////////
        // 8. Cache //
        //////////////

        /////////////
        // 9. DMAC //
        /////////////
        case dma_source_address_register_0: return dmac_sar0_.raw;
        case dma_source_address_register_1: return dmac_sar1_.raw;
        case dma_destination_address_register_0: return dmac_dar0_.raw;
        case dma_destination_address_register_1: return dmac_dar1_.raw;
        case dma_tranfer_count_register_0: return dmac_tcr0_.raw;
        case dma_tranfer_count_register_1: return dmac_tcr1_.raw;
        case dma_channel_control_register_0: return dmac_chcr0_.raw;
        case dma_channel_control_register_1: return dmac_chcr1_.raw;
        case dma_operation_register: return dmac_dmaor_.raw;

        //////////////
        // 10. DIVU //
        //////////////
        case divisor_register: {
            // if (divu_is_running_) divu_opcode_is_stalled_ = true;
            return divu_dvsr_.raw;
        }
        case dividend_register_l_32_bits: {
            // if (divu_is_running_) divu_opcode_is_stalled_ = true;
            return divu_dvdnt_.raw;
        }
        case division_control_register: {
            // if (divu_is_running_) divu_opcode_is_stalled_ = true;
            return divu_dvcr_.raw;
        }
        case dividend_register_l: {
            // if (divu_is_running_) divu_opcode_is_stalled_ = true;
            return divu_dvdntl_.raw;
        }
        case dividend_register_l_shadow: {
            // if (divu_is_running_) divu_opcode_is_stalled_ = true;
            return divu_dvdntl_shadow_.raw;
        }
        case dividend_register_h: {
            // if (divu_is_running_) divu_opcode_is_stalled_ = true;
            return divu_dvdnth_.raw;
        }
        case dividend_register_h_shadow: {
            // if (divu_is_running_) divu_opcode_is_stalled_ = true;
            return divu_dvdnth_shadow_.raw;
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
    switch (addr) {
        /////////////
        // 5. INTC //
        /////////////
        case interrupt_priority_level_setting_register_a: regs_.intc.ipra.upd(Sh2Regs::Intc::Ipra::hiByte(data)); break;
        case interrupt_priority_level_setting_register_a + 1: regs_.intc.ipra.upd(Sh2Regs::Intc::Ipra::loByte(data)); break;
        case interrupt_priority_level_setting_register_b: regs_.intc.iprb.upd(Sh2Regs::Intc::Iprb::hiByte(data)); break;
        case interrupt_priority_level_setting_register_b + 1: regs_.intc.iprb.upd(Sh2Regs::Intc::Iprb::loByte(data)); break;
        case vector_number_setting_register_a: regs_.intc.vcra.upd(Sh2Regs::Intc::Vcra::hiByte(data)); break;
        case vector_number_setting_register_a + 1: regs_.intc.vcra.upd(Sh2Regs::Intc::Vcra::loByte(data)); break;
        case vector_number_setting_register_b: regs_.intc.vcrb.upd(Sh2Regs::Intc::Vcrb::hiByte(data)); break;
        case vector_number_setting_register_b + 1: regs_.intc.vcrb.upd(Sh2Regs::Intc::Vcrb::loByte(data)); break;
        case vector_number_setting_register_c: regs_.intc.vcrc.upd(Sh2Regs::Intc::Vcrc::hiByte(data)); break;
        case vector_number_setting_register_c + 1: regs_.intc.vcrc.upd(Sh2Regs::Intc::Vcrc::loByte(data)); break;
        case vector_number_setting_register_d: intc_vcrd_.upper_8_bits = data; break;
        case vector_number_setting_register_d + 1: break; // Read only
        case vector_number_setting_register_wdt: intc_vcrwdt_.upper_8_bits = data; break;
        case vector_number_setting_register_wdt + 1: intc_vcrwdt_.lower_8_bits = data; break;
        case interrupt_control_register: intc_icr_.upper_8_bits = data; break;
        case interrupt_control_register + 1: intc_icr_.lower_8_bits = data; break;

        /////////////
        // 7. BSC  //
        /////////////

        //////////////
        // 8. Cache //
        //////////////
        case cache_control_register:
            Log::debug(Logger::sh2, "CCR byte write: {}", data);

            cache_ccr_.raw = data;
            if (toEnum<CachePurge>(cache_ccr_.cache_purge) == CachePurge::cache_purge) {
                purgeCache();

                // cache purge bit is cleared after operation
                cache_ccr_.cache_purge = false;
            }
            break;

        /////////////
        // 9. DMAC //
        /////////////
        case dma_request_response_selection_control_register_0: dmac_drcr0_.raw = data; break;
        case dma_request_response_selection_control_register_1: dmac_drcr1_.raw = data; break;

        //////////////
        // 10. DIVU //
        //////////////

        /////////////
        // 11. FRT //
        /////////////
        case timer_interrupt_enable_register: frt_tier_.raw = data; break;
        case free_running_timer_control_status_register: frt_ftcsr_.raw = data; break;
        case free_running_counter: frt_frc_.upper_8_bits = data; break;
        case free_running_counter + 1: frt_frc_.lower_8_bits = data; break;
        case output_compare_register:
            switch (toEnum<OutputCompareRegisterSelect>(frt_tocr_.output_compare_register_select)) {
                using enum OutputCompareRegisterSelect;
                case ocra: frt_ocra_.upper_8_bits = data; break;
                case ocrb: frt_ocrb_.upper_8_bits = data; break;
            }
            break;
        case output_compare_register + 1:
            switch (toEnum<OutputCompareRegisterSelect>(frt_tocr_.output_compare_register_select)) {
                using enum OutputCompareRegisterSelect;
                case ocra: frt_ocra_.lower_8_bits = data; break;
                case ocrb: frt_ocrb_.lower_8_bits = data; break;
            }
            break;
        case timer_control_register:
            frt_tcr_.raw = data;
            switch (toEnum<FrtClockSelect>(frt_tcr_.clock_select)) {
                using enum FrtClockSelect;
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
            frt_tocr_.raw              = (data & access_mask);
            break;
        }
        case input_capture_register: frt_icr_.upper_8_bits = data; break;
        case input_capture_register + 1:
            frt_icr_.lower_8_bits = data;
            break;

            /////////////
            // 12. WDT //
            /////////////

            /////////////
            // 13. SCI //
            /////////////

        case serial_mode_register: sci_smr_.raw = data; break;
        case bit_rate_register: sci_brr_.raw = data; break;
        case serial_control_register: sci_scr_.raw = data; break;
        case transmit_data_register: sci_tdr_.raw = data; break;
        case serial_status_register:
            sci_ssr_.raw = data;
            break;

            //////////////////////////
            // 14. Power-Down modes //
            //////////////////////////

        case standby_control_register:
            sbycr_.raw = data;
            Log::warning(Logger::sh2, "PWR - Standby control register write {:#0x}", data);
            break;

        default: unmappedAccess(addr, data); break;
    }
    // core::rawWrite<u8>(io_registers_, addr & sh2_memory_mask, data);
}

void Sh2::writeRegisters(u32 addr, u16 data) { // NOLINT(readability-convert-member-functions-to-static)
    switch (addr) {
        /////////////
        // 5. INTC //
        /////////////
        case interrupt_priority_level_setting_register_a: regs_.intc.ipra = data; break;
        case interrupt_priority_level_setting_register_b: regs_.intc.iprb = data; break;
        case vector_number_setting_register_a: regs_.intc.vcra = data; break;
        case vector_number_setting_register_b: regs_.intc.vcrb = data; break;
        case vector_number_setting_register_c: regs_.intc.vcrc = data; break;
        case vector_number_setting_register_d: intc_vcrd_.raw = data; break;
        case vector_number_setting_register_wdt: intc_vcrwdt_.raw = data; break;
        case vector_number_setting_register_div: break; // Read only access
        case vector_number_setting_register_div + 2: intc_vcrdiv_.lower_16_bits = (data & bitmask_7F); break;
        case interrupt_control_register: {
            auto new_icr = InterruptControlRegister{data};
            switch (toEnum<NmiEdgeDetection>(intc_icr_.nmi_edge_detection)) {
                using enum NmiEdgeDetection;
                using enum NmiInputLevel;
                case falling:
                    if ((toEnum<NmiInputLevel>(intc_icr_.nmi_input_level) == high)
                        && (toEnum<NmiInputLevel>(new_icr.nmi_input_level) == low)) {
                        Log::warning(Logger::sh2, "Falling edge NMI, not implemented !");
                    }
                    break;
                case rising:
                    if ((toEnum<NmiInputLevel>(intc_icr_.nmi_input_level) == low)
                        && (toEnum<NmiInputLevel>(new_icr.nmi_input_level) == high)) {
                        Log::warning(Logger::sh2, "Rising edge NMI, not implemented !");
                    }
                    break;
            }

            // Will force exit from the Sleep instruction. Will have to be adapted using Power Down modes.
            is_nmi_registered_ = true;

            intc_icr_.raw = data;
        } break;

        /////////////
        // 7. BSC //
        /////////////
        case bus_control_register1 + 2: {
            constexpr auto write_mask = u16{0b0001111111110111};
            bsc_bcr1_.lower_16_bits   = (data & write_mask);
            break;
        }
        case bus_control_register2 + 2: {
            constexpr auto write_mask = u16{0b0000000011111100};
            bsc_bcr2_.lower_16_bits   = (data & write_mask);
            break;
        }
        case wait_state_control_register + 2: bsc_wcr_.lower_16_bits = data; break;
        case individual_memory_control_register + 2: bsc_mcr_.lower_16_bits = data; break;
        case refresh_timer_control_status_register + 2: {
            constexpr auto write_mask = u16{0b0000000011111000};
            bsc_rtcsr_.lower_16_bits  = (data & write_mask);
            break;
        }
        case refresh_timer_counter + 2: {
            constexpr auto write_mask = u16{0b0000000011111111};
            bsc_rtcnt_.lower_16_bits  = (data & write_mask);
            break;
        }
        case refresh_time_constant_register + 2: {
            constexpr auto write_mask = u16{0b0000000011111111};
            bsc_rtcor_.lower_16_bits  = (data & write_mask);
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
            divu_dvcr_.lower_16_bits          = (data & access_mask);
            break;
        }

        /////////////
        // 11. FRT //
        /////////////
        case free_running_counter: frt_frc_.raw = data; break;
        case output_compare_register:
            switch (toEnum<OutputCompareRegisterSelect>(frt_tocr_.output_compare_register_select)) {
                using enum OutputCompareRegisterSelect;
                case ocra: frt_ocra_.raw = data; break;
                case ocrb: frt_ocrb_.raw = data; break;
            }
            break;
        case input_capture_register: frt_icr_.raw = data; break;

        /////////////
        // 12. WDT //
        /////////////
        case watchdog_timer_control_status_register:
            wdt_wtcsr_.raw = static_cast<u8>(data >> number_of_bits_8);
            wdt_wtcnt_.raw = static_cast<u8>(data & bitmask_00FF);
            break;
        case reset_control_status_register:
            wdt_rstcsr_.raw = static_cast<u8>(data >> number_of_bits_8);
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
    constexpr auto allow_bsc_write_mask = u32{0xA55A0000};
    switch (addr) {
        /////////////
        // 5. INTC //
        /////////////
        case vector_number_setting_register_div: intc_vcrdiv_.raw = data; break;
        case dma_vector_number_register_0: intc_vcrdma0_.raw = data; break;
        case dma_vector_number_register_1: intc_vcrdma1_.raw = data; break;

        /////////////
        // 7. BSC //
        /////////////
        case bus_control_register1:
            if ((data & bitmask_FFFF0000) == allow_bsc_write_mask) {
                constexpr auto write_mask = u16{0b0001111111110111};
                bsc_bcr1_.lower_16_bits   = (data & write_mask);
            }
            break;
        case bus_control_register2:
            if ((data & bitmask_FFFF0000) == allow_bsc_write_mask) {
                constexpr auto write_mask = u16{0b0000000011111100};
                bsc_bcr2_.lower_16_bits   = (data & write_mask);
            }
            break;
        case wait_state_control_register:
            if ((data & bitmask_FFFF0000) == allow_bsc_write_mask) { bsc_wcr_.lower_16_bits = data; }
            break;
        case individual_memory_control_register:
            if ((data & bitmask_FFFF0000) == allow_bsc_write_mask) { bsc_mcr_.lower_16_bits = data; }
            break;
        case refresh_timer_control_status_register:
            if ((data & bitmask_FFFF0000) == allow_bsc_write_mask) {
                constexpr auto write_mask = u16{0b0000000011111000};
                bsc_rtcsr_.lower_16_bits  = (data & write_mask);
            }
            break;
        case refresh_timer_counter:
            if ((data & bitmask_FFFF0000) == allow_bsc_write_mask) {
                constexpr auto write_mask = u16{0b0000000011111111};
                bsc_rtcnt_.lower_16_bits  = (data & write_mask);
            }
            break;
        case refresh_time_constant_register:
            if ((data & bitmask_FFFF0000) == allow_bsc_write_mask) {
                constexpr auto write_mask = u16{0b0000000011111111};
                bsc_rtcor_.lower_16_bits  = (data & write_mask);
            }
            break;

        //////////////
        // 8. Cache //
        //////////////

        /////////////
        // 9. DMAC //
        /////////////
        case dma_source_address_register_0: dmac_sar0_.raw = data; break;
        case dma_source_address_register_1: dmac_sar1_.raw = data; break;
        case dma_destination_address_register_0: dmac_dar0_.raw = data; break;
        case dma_destination_address_register_1: dmac_dar1_.raw = data; break;
        case dma_tranfer_count_register_0: dmac_tcr0_.raw = (data & bitmask_00FFFFFF); break;
        case dma_tranfer_count_register_1: dmac_tcr1_.raw = (data & bitmask_00FFFFFF); break;
        case dma_channel_control_register_0:
            dmac_chcr0_.raw = (data & bitmask_0000FFFF);
            executeDma();
            break;
        case dma_channel_control_register_1:
            dmac_chcr1_.raw = (data & bitmask_0000FFFF);
            executeDma();
            break;
        case dma_operation_register: {
            auto new_dmaor = DmaOperationRegister{data & bitmask_0F};
            if (dmac_dmaor_.priority_mode != new_dmaor.priority_mode) {
                dmac_next_transfer_priority_ = (toEnum<PriorityMode>(new_dmaor.priority_mode) == PriorityMode::fixed)
                                                   ? DmaNextTransferPriority::channel_0_first
                                                   : DmaNextTransferPriority::channel_1_first;
            }
            dmac_dmaor_ = new_dmaor;
            executeDma();
        } break;

        //////////////
        // 10. DIVU //
        //////////////
        case divisor_register: {
            // if (divu_is_running_) divu_opcode_is_stalled_ = true;
            divu_dvsr_.raw = data;
            // if (data == 0x96385) modules_.context()->debugStatus(core::DebugStatus::paused);
            break;
        }
        case dividend_register_l_32_bits: {
            // if (divu_is_running_) divu_opcode_is_stalled_ = true;
            divu_dvdnt_.raw = data;

            // ST-V needs some mirroring
            divu_dvdntl_.raw        = data;
            divu_dvdntl_shadow_.raw = data;

            // Sign extension for the upper 32 bits if needed

            ((data & sign_bit_32_mask) != 0) ? divu_dvdnth_.raw = u32_max : divu_dvdnth_.raw = 0;

            start32bitsDivision();
            runDivisionUnit(0);
            break;
        }
        case division_control_register: {
            // if (divu_is_running_) divu_opcode_is_stalled_ = true;
            divu_dvcr_.raw = data;
            break;
        }
        case dividend_register_l: {
            // if (divu_is_running_) divu_opcode_is_stalled_ = true;
            divu_dvdntl_.raw        = data;
            divu_dvdntl_shadow_.raw = data;

            start64bitsDivision();
            runDivisionUnit(0);
            break;
        }
        case dividend_register_h: {
            // if (divu_is_running_) divu_opcode_is_stalled_ = true;
            divu_dvdnth_.raw = data;
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

void Sh2::write32CachePurgeArea(const u32 addr, const u32 data) const {
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
    regs_.intc.ipra   = {};
    regs_.intc.iprb   = {};
    regs_.intc.vcra   = {};
    regs_.intc.vcrb   = {};
    regs_.intc.vcrc   = {};
    intc_vcrd_.raw    = {};
    intc_vcrwdt_.raw  = {};
    intc_vcrdiv_.raw  = {}; // lower 16 bits are undefined
    intc_vcrdma0_.raw = {}; // lower 8 bits are undefined
    intc_vcrdma1_.raw = {}; // lower 8 bits are undefined
    intc_icr_.raw     = {};

    // Bus State Controler registers
    constexpr auto bsc_bcr1_master_default_value = u32{0x000003F0};
    constexpr auto bsc_bcr1_slave_default_value  = u32{0x000083F0};
    constexpr auto bsc_bcr2_default_value        = u32{0x000000FCu};
    constexpr auto bsc_wcr_default_value         = u32{0x0000AAFFu};
    const auto default_bcr1 = u32{(sh2_type_ == Sh2Type::master) ? bsc_bcr1_master_default_value : bsc_bcr1_slave_default_value};

    bsc_bcr1_.raw  = default_bcr1;
    bsc_bcr2_.raw  = bsc_bcr2_default_value;
    bsc_wcr_.raw   = bsc_wcr_default_value;
    bsc_mcr_.raw   = {};
    bsc_rtcsr_.raw = {};
    bsc_rtcnt_.raw = {};
    bsc_rtcor_.raw = {};

    // Cache registers
    cache_ccr_.raw = {};

    // Direct Memory Access Controler registers
    dmac_tcr0_.raw  = {}; // lower 24 bits are undefined
    dmac_tcr1_.raw  = {}; // lower 24 bits are undefined
    dmac_chcr0_.raw = {};
    dmac_chcr1_.raw = {};
    dmac_drcr0_.raw = {};
    dmac_drcr1_.raw = {};
    dmac_dmaor_.raw = {};

    // Division Unit
    divu_dvcr_.raw = {};

    // Free Running timer
    constexpr auto frt_ocra_default_value = u16{0xFFFF};
    constexpr auto frt_ocrb_default_value = u16{0xFFFF};
    constexpr auto frt_tier_default_value = u8{0x01};
    constexpr auto frt_tocr_default_value = u8{0xe0};

    frt_tier_.raw  = frt_tier_default_value;
    frt_ftcsr_.raw = {};
    frt_frc_.raw   = {};
    frt_ocra_.raw  = frt_ocra_default_value;
    frt_ocrb_.raw  = frt_ocrb_default_value;
    frt_tcr_.raw   = {};
    frt_tocr_.raw  = frt_tocr_default_value;

    // Watch Dog Timer
    constexpr auto wdt_wtcsr_default_value  = u8{0x18};
    constexpr auto wdt_rstcsr_default_value = u8{0x1F};

    wdt_wtcsr_.raw  = wdt_wtcsr_default_value;
    wdt_wtcnt_.raw  = {};
    wdt_rstcsr_.raw = wdt_rstcsr_default_value;

    // Serial Communication Interface
    constexpr auto sci_ssr_default_value = u8{0x84};

    sci_smr_.raw = {};
    sci_brr_.raw = u8_max;
    sci_scr_.raw = {};
    sci_tdr_.raw = u8_max;
    sci_ssr_.raw = sci_ssr_default_value;
    sci_rdr_.raw = {};

    // Power-Down Modes
    constexpr auto sbycr_default_value = u8{0x60};
    sbycr_.raw                         = sbycr_default_value;
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
    divu_dvdntl_.raw = divu_dvdnt_.raw;

    const auto dvdnt = divu_dvdnt_.raw;
    if (static_cast<s32>(dvdnt) < 0) {
        divu_dvdnth_.raw = u32_max;
    } else {
        divu_dvdnth_.raw = 0;
    }
    const auto dvsr = divu_dvsr_.raw;

    // Log::debug(Logger::sh2, "{:#x} / {:#x} -> {:#x},{:#x}", dvdnt, dvsr, pc_);

    divu_quot_ = 0;
    divu_rem_  = 0;
    if (divu_dvsr_.raw != 0) {
        divu_quot_ = (s32)dvdnt / (s32)dvsr;
        divu_rem_  = (s32)dvdnt % (s32)dvsr;
        // Log::debug(Logger::sh2, "Quotient : {}, remainder : {}", divu_quot_, divu_rem_);
        Log::debug(Logger::sh2, "{:#x} / {:#x} -> {:#x}, {:#x}", dvdnt, dvsr, divu_quot_, divu_rem_);
    } else {
        // Log::debug(Logger::sh2, "Overflow detected !");
        Log::debug(Logger::sh2, "{:#x} / {:#x} -> Overflow detected !", dvdnt, dvsr);
        divu_dvcr_.overflow_flag = true;
    }

    // Overflow check
    const auto is_dvdnt_ovf = bool{(dvdnt & sign_bit_32_mask) != 0};
    const auto is_dvsr_ovf  = bool{(dvsr & sign_bit_32_mask) != 0};
    if (is_dvdnt_ovf && is_dvsr_ovf) {
        if ((divu_quot_ == INT32_MAX) && ((divu_rem_ & sign_bit_32_mask) != 0)) {
            // Log::debug(Logger::sh2, "Overflow detected !");
            Log::debug(Logger::sh2, "{:#x} / {:#x} -> Overflow detected !", dvdnt, dvsr);
            divu_dvcr_.overflow_flag = true;
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

    const auto dvdntl = divu_dvdntl_.raw;
    const auto dvdnth = divu_dvdnth_.raw;
    const auto dvsr   = divu_dvsr_.raw;

    const auto dividend = (static_cast<u64>(dvdnth) << number_of_bits_32) | dvdntl;

    // if ((dvdntl == 0x3bf20000) && (dvsr == 0x140000)) { modules_.context()->debugStatus(core::DebugStatus::paused); }
    // Log::debug(Logger::sh2, "{} / {} {:x}", dividend, dvsr, pc_);

    // auto dvcr = DivisionControlRegister(io_registers_[division_control_register & sh2_memory_mask]);
    auto quotient  = s64{};
    auto remainder = s64{};
    if (divu_dvsr_.raw != 0) {
        quotient  = (s64)dividend / (s32)dvsr;
        remainder = (s64)dividend % (s32)dvsr;
        // Log::debug(Logger::sh2, "Quotient : {}, remainder : {}", quotient, remainder);
        Log::debug(Logger::sh2, "{:#x} / {:#x} -> {:#x}, {:#x}", dividend, dvsr, (u64)quotient, (u64)remainder);
    } else {
        // Log::debug(Logger::sh2, "Overflow detected !");
        Log::debug(Logger::sh2, "{:#x} / {:#x} -> Overflow detected !", dividend, dvsr);
        divu_dvcr_.overflow_flag = true;
    }

    // Overflow check
    const auto is_dvdnth_ovf = bool{(dvdnth & sign_bit_32_mask) != 0};
    const auto is_dvsr_ovf   = bool{(dvsr & sign_bit_32_mask) != 0};
    if (is_dvdnth_ovf && is_dvsr_ovf) {
        if ((quotient == INT32_MAX) && ((remainder & sign_bit_32_mask) != 0)) {
            // Log::debug(Logger::sh2, "Overflow detected !");
            Log::debug(Logger::sh2, "{:#x} / {:#x} -> Overflow detected !", dividend, dvsr);
            divu_dvcr_.overflow_flag = true;
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
    divu_remaining_cycles_ -= cycles_to_run;
    if (divu_remaining_cycles_ == 0) {
        // auto dvcr = DivisionControlRegister(io_registers_[division_control_register & sh2_memory_mask]);
        if (toEnum<OverflowFlag>(divu_dvcr_.overflow_flag) == OverflowFlag::overflow) {
            if (toEnum<core::InterruptEnable>(divu_dvcr_.interrupt_enable) == core::InterruptEnable::enabled) {
                Log::debug(Logger::sh2, "DIVU - Sending division overflow interrupt");
                is::sh2_division_overflow.vector = intc_vcrdiv_.divu_interrupt_vector;
                is::sh2_division_overflow.level  = static_cast<u8>(regs_.intc.ipra >> Sh2Regs::Intc::Ipra::DIVU_LEVEL_SHFT);
                sendInterrupt(is::sh2_division_overflow);
            }
            divu_dvcr_.overflow_flag = false;
        } else {
            // Copy in DVDNTL and DVDNTH + ST-V mirroring
            divu_dvdnt_.raw         = divu_quot_;
            divu_dvdntl_.raw        = divu_quot_;
            divu_dvdntl_shadow_.raw = divu_quot_;

            divu_dvdnth_.raw        = divu_rem_;
            divu_dvdnth_shadow_.raw = divu_rem_;

            // if (divu_dvdnth_.toU32() == 0x16f0) modules_.context()->debugStatus(core::DebugStatus::paused);
        }
        divu_is_running_ = false;
        // divu_opcode_is_stalled_ = false;
    }
}

void Sh2::runFreeRunningTimer(const u8 cycles_to_run) {
    const auto elapsed_cycles    = u32{frt_elapsed_cycles_ + cycles_to_run};
    const auto counter_increment = u32{elapsed_cycles / frt_clock_divisor_};
    // const auto cycles_remainder  = u32{elapsed_cycles % frt_clock_divisor_};

    if (counter_increment > 0) {
        const auto old_frc     = u32{frt_frc_.raw};
        const auto current_frc = u32{old_frc + counter_increment};
        frt_frc_.raw           = current_frc;

        frt_elapsed_cycles_ = elapsed_cycles & frt_mask_;

        // Checking overflow
        if (current_frc > u16_max) {
            frt_ftcsr_.timer_overflow_flag = true;
            if (toEnum<TimerOverflowInterruptEnable>(frt_tier_.timer_overflow_interrupt_enable)
                == TimerOverflowInterruptEnable::interrupt_request_enabled) {
                Log::debug(Logger::sh2, "FRT - Sending overflow interrupt");
                is::sh2_frt_overflow_flag_set.vector = static_cast<u8>(intc_vcrd_.frt_overflow_vector);
                is::sh2_frt_overflow_flag_set.level  = static_cast<u8>(regs_.intc.iprb >> Sh2Regs::Intc::Iprb::FRT_LEVEL_SHFT);
                sendInterrupt(is::sh2_frt_overflow_flag_set);
            }
        }

        // Checking comparison for OCRA
        const auto ocra = frt_ocra_.raw;
        if ((old_frc <= ocra) && (current_frc > ocra)) {
            frt_ftcsr_.output_compare_flag_a = true;
            if (toEnum<OutputCompareInterruptAEnable>(frt_tier_.output_compare_interrupt_a_enable)
                == OutputCompareInterruptAEnable::interrupt_request_enabled) {
                Log::debug(Logger::sh2, "FRT - OCRA match");
                is::sh2_frt_output_compare_flag_a_set.vector = static_cast<u8>(regs_.intc.vcrc >> Sh2Regs::Intc::Vcrc::FOCV_SHFT);
                is::sh2_frt_output_compare_flag_a_set.level
                    = static_cast<u8>(regs_.intc.iprb >> Sh2Regs::Intc::Iprb::FRT_LEVEL_SHFT);
                sendInterrupt(is::sh2_frt_output_compare_flag_a_set);
            }
            if (toEnum<CounterClearA>(frt_ftcsr_.counter_clear_a) == CounterClearA::clear_on_compare) { frt_frc_.raw = {}; }
        }

        // Checking comparison for OCRB
        const auto ocrb = frt_ocrb_.raw;
        if ((old_frc <= ocrb) && (current_frc > ocrb)) {
            frt_ftcsr_.output_compare_flag_b = true;
            if (toEnum<OutputCompareInterruptBEnable>(frt_tier_.output_compare_interrupt_b_enable)
                == OutputCompareInterruptBEnable::interrupt_request_enabled) {
                Log::debug(Logger::sh2, "FRT - OCRB match");
                is::sh2_frt_output_compare_flag_b_set.vector = static_cast<u8>(regs_.intc.vcrc >> Sh2Regs::Intc::Vcrc::FOCV_SHFT);
                is::sh2_frt_output_compare_flag_b_set.level
                    = static_cast<u8>(regs_.intc.iprb >> Sh2Regs::Intc::Iprb::FRT_LEVEL_SHFT);
                sendInterrupt(is::sh2_frt_output_compare_flag_b_set);
            }
        }

    } else {
        frt_elapsed_cycles_ += cycles_to_run;
    }
}

void Sh2::executeDma() {
    if (toEnum<DmaMasterEnable>(dmac_dmaor_.dma_master_enable) == DmaMasterEnable::disabled) { return; }

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

    if (toEnum<PriorityMode>(dmac_dmaor_.priority_mode) == PriorityMode::round_robin) {
        switch (dmac_next_transfer_priority_) {
            using enum DmaNextTransferPriority;
            case channel_0_first: dmac_next_transfer_priority_ = DmaNextTransferPriority::channel_1_first; break;
            case channel_1_first: dmac_next_transfer_priority_ = DmaNextTransferPriority::channel_0_first; break;
        }
    }
}

auto Sh2::dmaStartConditionsAreSatisfied(const DmaChannel dc) const
    -> bool { // NOLINT(readability-convert-member-functions-to-static)
    // DE=1 TE=0 NMIF=0 AE=0
    switch (dc) {
        using enum DmaChannel;
        case channel_0: {
            auto channel_0_is_set = bool{toEnum<Sh2DmaEnable>(dmac_chcr0_.dma_enable) == Sh2DmaEnable::dma_transfer_enabled};
            channel_0_is_set
                &= toEnum<TransferEndFlag>(dmac_chcr0_.transfer_end_flag) == TransferEndFlag::dma_not_ended_or_aborted;
            channel_0_is_set &= toEnum<NmiFlag>(dmac_dmaor_.nmi_flag) == NmiFlag::no_nmif_interrupt;
            channel_0_is_set
                &= toEnum<AddressErrorFlag>(dmac_dmaor_.address_error_flag) == AddressErrorFlag::no_dmac_address_error;
            return channel_0_is_set;
        }
        case channel_1: {
            auto channel_1_is_set = bool{toEnum<Sh2DmaEnable>(dmac_chcr1_.dma_enable) == Sh2DmaEnable::dma_transfer_enabled};
            channel_1_is_set
                &= toEnum<TransferEndFlag>(dmac_chcr1_.transfer_end_flag) == TransferEndFlag::dma_not_ended_or_aborted;
            channel_1_is_set &= toEnum<NmiFlag>(dmac_dmaor_.nmi_flag) == NmiFlag::no_nmif_interrupt;
            channel_1_is_set
                &= toEnum<AddressErrorFlag>(dmac_dmaor_.address_error_flag) == AddressErrorFlag::no_dmac_address_error;
            return channel_1_is_set;
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
            conf.counter     = dmac_tcr0_.raw & bitmask_00FFFFFF;
            conf.source      = dmac_sar0_.raw;
            conf.destination = dmac_dar0_.raw;
            conf.chcr        = dmac_chcr0_;
            conf.drcr        = dmac_drcr0_;
            conf.interrupt   = is::sh2_dma_0_transfer_end;
            break;
        case channel_1:
            conf.channel     = DmaChannel::channel_1;
            conf.counter     = dmac_tcr1_.raw & bitmask_00FFFFFF;
            conf.source      = dmac_sar1_.raw;
            conf.destination = dmac_dar1_.raw;
            conf.chcr        = dmac_chcr1_;
            conf.drcr        = dmac_drcr1_;
            conf.interrupt   = is::sh2_dma_1_transfer_end;
            break;
        default: Log::warning(Logger::sh2, "DMAC - Unknown DMA channel");
    }
    return conf;
}

void Sh2::executeDmaOnChannel(Sh2DmaConfiguration& conf) {
    if (dmaStartConditionsAreSatisfied(conf.channel)) {
        auto       sh2_type       = sh2_type_name_.at(sh2_type_);
        const auto channel_number = static_cast<u8>((conf.channel == DmaChannel::channel_0) ? 0 : 1);
        if (toEnum<AutoRequestMode>(conf.chcr.auto_request_mode) == AutoRequestMode::module_request) {
            switch (toEnum<ResourceSelect>(conf.drcr.resource_select)) {
                using enum ResourceSelect;
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
            switch (toEnum<TransferSize>(conf.chcr.transfer_size)) {
                using enum TransferSize;
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
                    modules_.memory()->write<u32>(destination + displacement_4,
                                                  modules_.memory()->read<u32>(source + displacement_4));
                    modules_.memory()->write<u32>(destination + 8, modules_.memory()->read<u32>(source + 8));
                    modules_.memory()->write<u32>(destination + displacement_12,
                                                  modules_.memory()->read<u32>(source + displacement_12));
                    transfer_size = transfer_byte_size_16;
                    counter -= 4;
                    break;
            }

            switch (toEnum<SourceAddressMode>(conf.chcr.source_address_mode)) {
                using enum SourceAddressMode;
                case fixed: break;
                case incremented: source += transfer_size; break;
                case decremented: source -= transfer_size; break;
                case reserved: Log::warning(Logger::sh2, "Reserved source address mode used !");
            }

            switch (toEnum<DestinationAddressMode>(conf.chcr.destination_address_mode)) {
                using enum DestinationAddressMode;
                case fixed: break;
                case incremented: destination += transfer_size; break;
                case decremented: destination -= transfer_size; break;
                case reserved: Log::warning(Logger::sh2, "Reserved destination address mode used !");
            }
        }

        switch (conf.channel) {
            case DmaChannel::channel_0:
                dmac_tcr0_.raw                = counter;
                dmac_sar0_.raw                = source;
                dmac_dar0_.raw                = destination;
                dmac_chcr0_.transfer_end_flag = true;
                conf.interrupt.vector         = intc_vcrdma0_.dma_transfert_end_vector;
                break;
            case DmaChannel::channel_1:
                dmac_tcr1_.raw                = counter;
                dmac_sar1_.raw                = source;
                dmac_dar1_.raw                = destination;
                dmac_chcr1_.transfer_end_flag = true;
                conf.interrupt.vector         = intc_vcrdma1_.dma_transfert_end_vector;
                break;
            case DmaChannel::channel_unknown: Log::warning(Logger::sh2, "Unknown DMA channel used !");
        }

        if (toEnum<Sh2DmaInterruptEnable>(conf.chcr.interrupt_enable) == Sh2DmaInterruptEnable::enabled) {
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
    if (toEnum<InterruptCaptureInterruptEnable>(frt_tier_.interrupt_capture_interrupt_enable)
        == InterruptCaptureInterruptEnable::interrupt_request_enabled) {
        is::sh2_frt_input_capture.vector = static_cast<u8>(regs_.intc.vcrc >> Sh2Regs::Intc::Vcrc::FICV_SHFT);
        is::sh2_frt_input_capture.level  = static_cast<u8>(regs_.intc.iprb >> Sh2Regs::Intc::Iprb::FRT_LEVEL_SHFT);
        sendInterrupt(is::sh2_frt_input_capture);
    }

    frt_ftcsr_.input_capture_flag = true;
    frt_icr_.raw                  = frt_frc_.raw;
}

auto Sh2::run() -> u8 {
    modules_.memory()->sh2_in_operation_ = sh2_type_;
    runInterruptController();
    current_opcode_ = modules_.memory()->read<u16>(pc_);
    execute(*this);
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
        case r0: return r_[index_0]; break;
        case r1: return r_[index_1]; break;
        case r2: return r_[index_2]; break;
        case r3: return r_[index_3]; break;
        case r4: return r_[index_4]; break;
        case r5: return r_[index_5]; break;
        case r6: return r_[index_6]; break;
        case r7: return r_[index_7]; break;
        case r8: return r_[index_8]; break;
        case r9: return r_[index_9]; break;
        case r10: return r_[index_10]; break;
        case r11: return r_[index_11]; break;
        case r12: return r_[index_12]; break;
        case r13: return r_[index_13]; break;
        case r14: return r_[index_14]; break;
        case r15: return r_[index_15]; break;
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

} // namespace saturnin::sh2
