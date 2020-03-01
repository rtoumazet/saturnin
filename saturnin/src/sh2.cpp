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

#include "sh2.h"
#include "emulator_context.h"
#include "interrupt_sources.h"
#include "scu_registers.h"
#include "scu.h"
#include "utilities.h"

namespace is = saturnin::core::interrupt_source;

namespace saturnin::sh2 {

using core::Log;

// Masks constants
constexpr u32 allow_bsc_write_mask{0xA55A0000};
constexpr u32 cache_purge_mask{0xFFFFFC0B};

// Default values
// BSC
constexpr u32 bsc_bcr1_master_default_value{0x000003F0};
constexpr u32 bsc_bcr1_slave_default_value{0x000083F0};
constexpr u32 bsc_bcr2_default_value{0x000000FCu};
constexpr u32 bsc_wcr_default_value{0x0000AAFFu};
constexpr u8  frt_tier_default_value{0x01};
constexpr u16 frt_ocra_default_value{0xFFFF};
constexpr u16 frt_ocrb_default_value{0xFFFF};
constexpr u8  frt_tocr_default_value{0xe0};
constexpr u8  wdt_wtcsr_default_value{0x18};
constexpr u8  wdt_rstcsr_default_value{0x1F};
constexpr u8  sci_ssr_default_value{0x84};

constexpr u8 divu_normal_cycles_number{39};
constexpr u8 divu_overflow_cycles_number{6};

constexpr u32 pc_start_vector{0x00000008};
constexpr u32 sp_start_vector{0x0000000C};
constexpr u8  sr_stack_offset{4};
constexpr u8  pc_stack_offset{8};

constexpr u8 transfer_byte_size_1{0x1};
constexpr u8 transfer_byte_size_2{0x2};
constexpr u8 transfer_byte_size_4{0x4};
constexpr u8 transfer_byte_size_16{0x10};

Sh2::Sh2(Sh2Type st, core::EmulatorContext* ec) : sh2_type_(st), emulator_context_(ec) { reset(); }

auto Sh2::memory() const -> core::Memory* { return emulator_context_->memory(); }

auto Sh2::scu() const -> Scu* { return emulator_context_->scu(); }

auto Sh2::emulatorContext() -> core::EmulatorContext* { return emulator_context_; }

auto Sh2::readRegisters8(const u32 addr) -> u8 {
    switch (addr) {
        /////////////
        // 5. INTC //
        /////////////
        case interrupt_priority_level_setting_register_a:
            return intc_ipra_.get(InterruptPriorityLevelSettingRegisterA::upper_8_bits);
        case interrupt_priority_level_setting_register_a + 1:
            return intc_ipra_.get(InterruptPriorityLevelSettingRegisterA::lower_8_bits);
        case interrupt_priority_level_setting_register_b:
            return intc_iprb_.get(InterruptPriorityLevelSettingRegisterB::upper_8_bits);
        case interrupt_priority_level_setting_register_b + 1:
            return intc_iprb_.get(InterruptPriorityLevelSettingRegisterB::lower_8_bits);
        case vector_number_setting_register_a: return intc_vcra_.get(VectorNumberSettingRegisterA::upper_8_bits);
        case vector_number_setting_register_a + 1: return intc_vcra_.get(VectorNumberSettingRegisterA::lower_8_bits);
        case vector_number_setting_register_b: return intc_vcrb_.get(VectorNumberSettingRegisterB::upper_8_bits);
        case vector_number_setting_register_b + 1: return intc_vcrb_.get(VectorNumberSettingRegisterB::lower_8_bits);
        case vector_number_setting_register_c: return intc_vcrc_.get(VectorNumberSettingRegisterC::upper_8_bits);
        case vector_number_setting_register_c + 1: return intc_vcrc_.get(VectorNumberSettingRegisterC::lower_8_bits);
        case vector_number_setting_register_d: return intc_vcrd_.get(VectorNumberSettingRegisterD::upper_8_bits);
        case vector_number_setting_register_d + 1: return intc_vcrd_.get(VectorNumberSettingRegisterD::lower_8_bits);
        case vector_number_setting_register_wdt: return intc_vcrwdt_.get(VectorNumberSettingRegisterWdt::upper_8_bits);
        case vector_number_setting_register_wdt + 1: return intc_vcrwdt_.get(VectorNumberSettingRegisterWdt::lower_8_bits);
        case interrupt_control_register: return intc_icr_.get(InterruptControlRegister::upper_8_bits);
        case interrupt_control_register + 1: return intc_icr_.get(InterruptControlRegister::lower_8_bits);
        /////////////
        // 7. BSC //
        /////////////

        //////////////
        // 8. Cache //
        //////////////

        /////////////
        // 9. DMAC //
        /////////////
        case dma_request_response_selection_control_register_0:
            return dmac_drcr0_.get(DmaRequestResponseSelectionControlRegister::all_bits);
        case dma_request_response_selection_control_register_1:
            return dmac_drcr1_.get(DmaRequestResponseSelectionControlRegister::all_bits);
        //////////////
        // 10. DIVU //
        //////////////

        /////////////
        // 11. FRT //
        /////////////
        case timer_interrupt_enable_register: return frt_tier_.get(TimerInterruptEnableRegister::all_bits);
        case free_running_timer_control_status_register: return frt_ftcsr_.get(FreeRunningTimerControlStatusRegister::all_bits);
        case free_running_counter: return frt_frc_.get(FreeRunningCounter::upper_8_bits);
        case free_running_counter + 1: return frt_frc_.get(FreeRunningCounter::lower_8_bits);
        case timer_control_register: return frt_tcr_.get(TimerControlRegister::all_bits);
        case timer_output_compare_control_register: return frt_tocr_.get(TimerOutputCompareControlRegister::all_bits);
        case input_capture_register: return frt_icr_.get(InputCaptureRegister::upper_8_bits);
        case input_capture_register + 1: return frt_icr_.get(InputCaptureRegister::lower_8_bits);

        /////////////
        // 12. WDT //
        /////////////
        case watchdog_timer_control_status_register: return wdt_wtcsr_.get(WatchdogTimerControlStatusRegister::all_bits);
        case watchdog_timer_counter: return wdt_wtcnt_.get(WatchdogTimerCounter::all_bits);
        case reset_control_status_register + 1: return wdt_rstcsr_.get(ResetControlStatusRegister::all_bits);

        /////////////
        // 13. SCI //
        /////////////
        case serial_mode_register: return sci_smr_.get(SerialModeRegister::all_bits);
        case bit_rate_register: return sci_brr_.get(BitRateRegister::all_bits);
        case serial_control_register: return sci_scr_.get(SerialControlRegister::all_bits);
        case transmit_data_register: return sci_tdr_.get(TransmitDataRegister::all_bits);
        case serial_status_register: return sci_ssr_.get(SerialStatusRegister::all_bits);
        case receive_data_register: return sci_rdr_.get(ReceiveDataRegister::all_bits);

        default: unmappedAccess(addr); return 0;
    }
}

auto Sh2::readRegisters16(const u32 addr) -> u16 {
    switch (addr) {
        /////////////
        // 5. INTC //
        /////////////
        case interrupt_priority_level_setting_register_a: return intc_ipra_.get(InterruptPriorityLevelSettingRegisterA::all_bits);
        case interrupt_priority_level_setting_register_b: return intc_iprb_.get(InterruptPriorityLevelSettingRegisterB::all_bits);
        case vector_number_setting_register_a: return intc_vcra_.get(VectorNumberSettingRegisterA::all_bits);
        case vector_number_setting_register_b: return intc_vcrb_.get(VectorNumberSettingRegisterB::all_bits);
        case vector_number_setting_register_c: return intc_vcrc_.get(VectorNumberSettingRegisterC::all_bits);
        case vector_number_setting_register_d: return intc_vcrd_.get(VectorNumberSettingRegisterD::all_bits);
        case vector_number_setting_register_wdt: return intc_vcrwdt_.get(VectorNumberSettingRegisterWdt::all_bits);
        case interrupt_control_register: return intc_icr_.get(InterruptControlRegister::all_bits);
        case vector_number_setting_register_div: return intc_vcrdiv_.get(VectorNumberSettingRegisterDiv::upper_16_bits);
        case vector_number_setting_register_div + 2: return intc_vcrdiv_.get(VectorNumberSettingRegisterDiv::lower_16_bits);

        /////////////
        // 7. BSC //
        /////////////
        case bus_control_register1 + 2: return bsc_bcr1_.get(BusControlRegister1::lower_16_bits);
        case bus_control_register2 + 2: return bsc_bcr2_.get(BusControlRegister2::lower_16_bits);
        case wait_state_control_register + 2: return bsc_wcr_.get(WaitControlRegister::lower_16_bits);
        case individual_memory_control_register + 2: return bsc_mcr_.get(IndividualMemoryControlRegister::lower_16_bits);
        case refresh_timer_control_status_register + 2: return bsc_rtcsr_.get(RefreshTimeControlStatusRegister::lower_16_bits);
        case refresh_timer_counter + 2: return bsc_rtcnt_.get(RefreshTimerCounter::lower_16_bits);
        case refresh_time_constant_register + 2: return bsc_rtcor_.get(RefreshTimerConstantRegister::lower_16_bits);

        //////////////
        // 8. Cache //
        //////////////
        case cache_control_register: return cache_ccr_.get(CacheControlRegister::all_bits);

        /////////////
        // 9. DMAC //
        /////////////

        //////////////
        // 10. DIVU //
        //////////////
        case division_control_register: return divu_dvcr_.get(DivisionControlRegister::upper_16_bits);
        case division_control_register + 2:
            return divu_dvcr_.get(DivisionControlRegister::lower_16_bits);

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

auto Sh2::readRegisters32(const u32 addr) -> u32 {
    switch (addr) {
        /////////////
        // 5. INTC //
        /////////////
        case vector_number_setting_register_div: return intc_vcrdiv_.toU32();
        case dma_vector_number_register_0: return intc_vcrdma0_.toU32();
        case dma_vector_number_register_1: return intc_vcrdma1_.toU32();

        /////////////
        // 7. BSC //
        /////////////
        // Upper 16 bits are 0x0000, but as they're initialized and never written to after that, no need to mask the return value.
        case bus_control_register1: return bsc_bcr1_.toU32();
        case bus_control_register2: return bsc_bcr2_.toU32();
        case wait_state_control_register: return bsc_wcr_.toU32();
        case individual_memory_control_register: return bsc_mcr_.toU32();
        case refresh_timer_control_status_register: return bsc_rtcsr_.toU32();
        case refresh_timer_counter: return bsc_rtcnt_.toU32();
        case refresh_time_constant_register: return bsc_rtcor_.toU32();

        //////////////
        // 8. Cache //
        //////////////

        /////////////
        // 9. DMAC //
        /////////////
        case dma_source_address_register_0: return dmac_sar0_.toU32();
        case dma_source_address_register_1: return dmac_sar1_.toU32();
        case dma_destination_address_register_0: return dmac_dar0_.toU32();
        case dma_destination_address_register_1: return dmac_dar1_.toU32();
        case dma_tranfer_count_register_0: return dmac_tcr0_.toU32();
        case dma_tranfer_count_register_1: return dmac_tcr1_.toU32();
        case dma_channel_control_register_0: return dmac_chcr0_.toU32();
        case dma_channel_control_register_1: return dmac_chcr1_.toU32();
        case dma_operation_register: return dmac_dmaor_.toU32();

        //////////////
        // 10. DIVU //
        //////////////
        case divisor_register: return divu_dvsr_.toU32();
        case dividend_register_l_32_bits: return divu_dvdnt_.toU32();
        case division_control_register: return divu_dvcr_.toU32();
        case dividend_register_l:
        case dividend_register_l_shadow: return divu_dvdntl_.toU32();
        case dividend_register_h:
        case dividend_register_h_shadow:
            return divu_dvdnth_.toU32();

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
        case interrupt_priority_level_setting_register_a:
            intc_ipra_.set(InterruptPriorityLevelSettingRegisterA::upper_8_bits, data);
            break;
        case interrupt_priority_level_setting_register_a + 1:
            intc_ipra_.set(InterruptPriorityLevelSettingRegisterA::lower_8_bits, data);
            break;
        case interrupt_priority_level_setting_register_b:
            intc_iprb_.set(InterruptPriorityLevelSettingRegisterB::upper_8_bits, data);
            break;
        case vector_number_setting_register_a: intc_vcra_.set(VectorNumberSettingRegisterA::upper_8_bits, data); break;
        case vector_number_setting_register_a + 1: intc_vcra_.set(VectorNumberSettingRegisterA::lower_8_bits, data); break;
        case vector_number_setting_register_b: intc_vcrb_.set(VectorNumberSettingRegisterB::upper_8_bits, data); break;
        case vector_number_setting_register_b + 1: intc_vcrb_.set(VectorNumberSettingRegisterB::lower_8_bits, data); break;
        case vector_number_setting_register_c: intc_vcrc_.set(VectorNumberSettingRegisterC::upper_8_bits, data); break;
        case vector_number_setting_register_c + 1: intc_vcrc_.set(VectorNumberSettingRegisterC::lower_8_bits, data); break;
        case vector_number_setting_register_d: intc_vcrd_.set(VectorNumberSettingRegisterD::upper_8_bits, data); break;
        case vector_number_setting_register_d + 1: break; // Read only
        case vector_number_setting_register_wdt: intc_vcrwdt_.set(VectorNumberSettingRegisterWdt::upper_8_bits, data); break;
        case vector_number_setting_register_wdt + 1: intc_vcrwdt_.set(VectorNumberSettingRegisterWdt::lower_8_bits, data); break;
        case interrupt_control_register: intc_icr_.set(InterruptControlRegister::upper_8_bits, data); break;
        case interrupt_control_register + 1: intc_icr_.set(InterruptControlRegister::lower_8_bits, data); break;

        /////////////
        // 7. BSC  //
        /////////////

        //////////////
        // 8. Cache //
        //////////////
        case cache_control_register:
            Log::debug("sh2", "CCR byte write: {}", data);

            cache_ccr_.set(CacheControlRegister::all_bits, data);
            if (cache_ccr_.get(CacheControlRegister::cache_purge) == CachePurge::cache_purge) {
                purgeCache();

                // cache purge bit is cleared after operation
                cache_ccr_.reset(CacheControlRegister::cache_purge);
            }
            break;

        /////////////
        // 9. DMAC //
        /////////////
        case dma_request_response_selection_control_register_0:
            dmac_drcr0_.set(DmaRequestResponseSelectionControlRegister::all_bits, data);
            break;
        case dma_request_response_selection_control_register_1:
            dmac_drcr1_.set(DmaRequestResponseSelectionControlRegister::all_bits, data);
            break;

        //////////////
        // 10. DIVU //
        //////////////

        /////////////
        // 11. FRT //
        /////////////
        case timer_interrupt_enable_register: frt_tier_.set(TimerInterruptEnableRegister::all_bits, data); break;
        case free_running_timer_control_status_register:
            frt_ftcsr_.set(FreeRunningTimerControlStatusRegister::all_bits, data);
            break;
        case free_running_counter: frt_frc_.set(FreeRunningCounter::upper_8_bits, data); break;
        case free_running_counter + 1: frt_frc_.set(FreeRunningCounter::lower_8_bits, data); break;
        case output_compare_register:
            switch (frt_tocr_.get(TimerOutputCompareControlRegister::output_compare_register_select)) {
                case OutputCompareRegisterSelect::ocra: frt_ocra_.set(OutputCompareRegister::upper_8_bits, data); break;
                case OutputCompareRegisterSelect::ocrb: frt_ocrb_.set(OutputCompareRegister::upper_8_bits, data); break;
            }
            break;
        case output_compare_register + 1:
            switch (frt_tocr_.get(TimerOutputCompareControlRegister::output_compare_register_select)) {
                case OutputCompareRegisterSelect::ocra: frt_ocra_.set(OutputCompareRegister::lower_8_bits, data); break;
                case OutputCompareRegisterSelect::ocrb: frt_ocrb_.set(OutputCompareRegister::lower_8_bits, data); break;
            }
            break;
        case timer_control_register:
            frt_tcr_.set(TimerControlRegister::all_bits, data);
            switch (frt_tcr_.get(TimerControlRegister::clock_select)) {
                case FrtClockSelect::internal_divided_by_8:
                    frt_clock_divisor_ = frt_clock_divisor_8;
                    frt_mask_          = frt_clock_divisor_mask_8;
                    break;
                case FrtClockSelect::internal_divided_by_32:
                    frt_clock_divisor_ = frt_clock_divisor_32;
                    frt_mask_          = frt_clock_divisor_mask_32;
                    break;
                case FrtClockSelect::internal_divided_by_128:
                    frt_clock_divisor_ = frt_clock_divisor_128;
                    frt_mask_          = frt_clock_divisor_mask_128;
                    break;
                case FrtClockSelect::external: Log::warning("sh2", "FRT - External clock not implemented"); break;
            }
            break;
        case timer_output_compare_control_register:
            frt_tocr_.set(TimerOutputCompareControlRegister::all_bits,
                          static_cast<u8>(data & TimerOutputCompareControlRegister::access_mask));
            break;
        case input_capture_register: frt_icr_.set(InputCaptureRegister::upper_8_bits, data); break;
        case input_capture_register + 1:
            frt_icr_.set(InputCaptureRegister::lower_8_bits, data);
            break;

            /////////////
            // 12. WDT //
            /////////////

            /////////////
            // 13. SCI //
            /////////////

        case serial_mode_register: sci_smr_.set(SerialModeRegister::all_bits, data); break;
        case bit_rate_register: sci_brr_.set(BitRateRegister::all_bits, data); break;
        case serial_control_register: sci_scr_.set(SerialControlRegister::all_bits, data); break;
        case transmit_data_register: sci_tdr_.set(TransmitDataRegister::all_bits, data); break;
        case serial_status_register: sci_ssr_.set(SerialStatusRegister::all_bits, data); break;

        default: unmappedAccess(addr, data); break;
    }
    // core::rawWrite<u8>(io_registers_, addr & sh2_memory_mask, data);
}

void Sh2::writeRegisters(u32 addr, u16 data) { // NOLINT(readability-convert-member-functions-to-static)
    switch (addr) {
        /////////////
        // 5. INTC //
        /////////////
        case interrupt_priority_level_setting_register_a:
            intc_ipra_.set(InterruptPriorityLevelSettingRegisterA::all_bits, data);
            break;
        case interrupt_priority_level_setting_register_b:
            intc_iprb_.set(InterruptPriorityLevelSettingRegisterB::all_bits, data);
            break;
        case vector_number_setting_register_a: intc_vcra_.set(VectorNumberSettingRegisterA::all_bits, data); break;
        case vector_number_setting_register_b: intc_vcrb_.set(VectorNumberSettingRegisterB::all_bits, data); break;
        case vector_number_setting_register_c: intc_vcrc_.set(VectorNumberSettingRegisterC::all_bits, data); break;
        case vector_number_setting_register_d: intc_vcrd_.set(VectorNumberSettingRegisterD::all_bits, data); break;
        case vector_number_setting_register_wdt: intc_vcrwdt_.set(VectorNumberSettingRegisterWdt::all_bits, data); break;
        case vector_number_setting_register_div: break; // Read only access
        case vector_number_setting_register_div + 2:
            intc_vcrdiv_.set(VectorNumberSettingRegisterDiv::lower_16_bits,
                             static_cast<u16>(data & DivisionControlRegister::access_mask));
            break;
        case interrupt_control_register: {
            auto new_icr = InterruptControlRegister(data);
            switch (intc_icr_.get(InterruptControlRegister::nmi_edge_detection)) {
                case NmiEdgeDetection::falling:
                    if ((intc_icr_.get(InterruptControlRegister::nmi_input_level) == NmiInputLevel::high)
                        && (new_icr.get(InterruptControlRegister::nmi_input_level) == NmiInputLevel::low)) {
                        Log::warning("sh2", "Falling edge NMI, not implemented !");
                    }
                    break;
                case NmiEdgeDetection::rising:
                    if ((intc_icr_.get(InterruptControlRegister::nmi_input_level) == NmiInputLevel::low)
                        && (new_icr.get(InterruptControlRegister::nmi_input_level) == NmiInputLevel::high)) {
                        Log::warning("sh2", "Rising edge NMI, not implemented !");
                    }
                    break;
            }
            intc_icr_.set(InterruptControlRegister::all_bits, data);
        } break;

        /////////////
        // 7. BSC //
        /////////////
        case bus_control_register1 + 2:
            bsc_bcr1_.set(BusControlRegister1::lower_16_bits, static_cast<u16>(data & BusControlRegister1::write_mask));
            break;
        case bus_control_register2 + 2:
            bsc_bcr2_.set(BusControlRegister2::lower_16_bits, static_cast<u16>(data & BusControlRegister2::write_mask));
            break;
        case wait_state_control_register + 2: bsc_wcr_.set(WaitControlRegister::lower_16_bits, data); break;
        case individual_memory_control_register + 2: bsc_mcr_.set(IndividualMemoryControlRegister::lower_16_bits, data); break;
        case refresh_timer_control_status_register + 2:
            bsc_rtcsr_.set(RefreshTimeControlStatusRegister::lower_16_bits,
                           static_cast<u16>(data & RefreshTimeControlStatusRegister::write_mask));
            break;
        case refresh_timer_counter + 2:
            bsc_rtcnt_.set(RefreshTimerCounter::lower_16_bits, static_cast<u16>(data & RefreshTimerCounter::write_mask));
            break;
        case refresh_time_constant_register + 2:
            bsc_rtcor_.set(RefreshTimerConstantRegister::lower_16_bits,
                           static_cast<u16>(data & RefreshTimerConstantRegister::write_mask));
            break;

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
        case division_control_register + 2:
            divu_dvcr_.set(DivisionControlRegister::lower_16_bits, static_cast<u16>(data & DivisionControlRegister::access_mask));
            break;

        /////////////
        // 11. FRT //
        /////////////
        case free_running_counter: frt_frc_.set(FreeRunningCounter::all_bits, data); break;
        case output_compare_register:
            switch (frt_tocr_.get(TimerOutputCompareControlRegister::output_compare_register_select)) {
                case OutputCompareRegisterSelect::ocra: frt_ocra_.set(OutputCompareRegister::all_bits, data); break;
                case OutputCompareRegisterSelect::ocrb: frt_ocrb_.set(OutputCompareRegister::all_bits, data); break;
            }
            break;
        case input_capture_register: frt_icr_.set(InputCaptureRegister::all_bits, data); break;

        /////////////
        // 12. WDT //
        /////////////
        case watchdog_timer_control_status_register:
            wdt_wtcsr_.set(WatchdogTimerControlStatusRegister::all_bits, static_cast<u8>(data >> number_of_bits_8));
            wdt_wtcnt_.set(WatchdogTimerCounter::all_bits, static_cast<u8>(data & bitmask_00FF));
            break;
        case reset_control_status_register:
            wdt_rstcsr_.set(ResetControlStatusRegister::all_bits, static_cast<u8>(data >> number_of_bits_8));
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
    switch (addr) {
        /////////////
        // 5. INTC //
        /////////////
        case vector_number_setting_register_div: intc_vcrdiv_.set(VectorNumberSettingRegisterDiv::all_bits, data); break;
        case dma_vector_number_register_0: intc_vcrdma0_.set(VectorNumberSettingRegisterDma::all_bits, data); break;
        case dma_vector_number_register_1: intc_vcrdma1_.set(VectorNumberSettingRegisterDma::all_bits, data); break;

        /////////////
        // 7. BSC //
        /////////////
        case bus_control_register1:
            if ((data & bitmask_FFFF0000) == allow_bsc_write_mask) {
                bsc_bcr1_.set(BusControlRegister1::lower_16_bits, static_cast<u16>(data & BusControlRegister1::write_mask));
            }
            break;
        case bus_control_register2:
            if ((data & bitmask_FFFF0000) == allow_bsc_write_mask) {
                bsc_bcr2_.set(BusControlRegister2::lower_16_bits, static_cast<u16>(data & BusControlRegister2::write_mask));
            }
            break;
        case wait_state_control_register:
            if ((data & bitmask_FFFF0000) == allow_bsc_write_mask) {
                bsc_wcr_.set(WaitControlRegister::lower_16_bits, static_cast<u16>(data));
            }
            break;
        case individual_memory_control_register:
            if ((data & bitmask_FFFF0000) == allow_bsc_write_mask) {
                bsc_mcr_.set(IndividualMemoryControlRegister::lower_16_bits, static_cast<u16>(data));
            }
            break;
        case refresh_timer_control_status_register:
            if ((data & bitmask_FFFF0000) == allow_bsc_write_mask) {
                bsc_rtcsr_.set(RefreshTimeControlStatusRegister::lower_16_bits,
                               static_cast<u16>(data & RefreshTimeControlStatusRegister::write_mask));
            }
            break;
        case refresh_timer_counter:
            if ((data & bitmask_FFFF0000) == allow_bsc_write_mask) {
                bsc_rtcnt_.set(RefreshTimerCounter::lower_16_bits, static_cast<u16>(data & RefreshTimerCounter::write_mask));
            }
            break;
        case refresh_time_constant_register:
            if ((data & bitmask_FFFF0000) == allow_bsc_write_mask) {
                bsc_rtcor_.set(RefreshTimerConstantRegister::lower_16_bits,
                               static_cast<u16>(data & RefreshTimerConstantRegister::write_mask));
            }
            break;

        //////////////
        // 8. Cache //
        //////////////

        /////////////
        // 9. DMAC //
        /////////////
        case dma_source_address_register_0: dmac_sar0_.set(DmaSourceAddressRegister::all_bits, data); break;
        case dma_source_address_register_1: dmac_sar1_.set(DmaSourceAddressRegister::all_bits, data); break;
        case dma_destination_address_register_0: dmac_dar0_.set(DmaDestinationAddressRegister::all_bits, data); break;
        case dma_destination_address_register_1: dmac_dar1_.set(DmaDestinationAddressRegister::all_bits, data); break;
        case dma_tranfer_count_register_0:
            dmac_tcr0_.set(DmaTransferCountRegister::all_bits, data & DmaTransferCountRegister::write_mask);
            break;
        case dma_tranfer_count_register_1:
            dmac_tcr1_.set(DmaTransferCountRegister::all_bits, data & DmaTransferCountRegister::write_mask);
            break;
        case dma_channel_control_register_0:
            dmac_chcr0_.set(DmaChannelControlRegister::all_bits, data & DmaChannelControlRegister::write_mask);
            executeDma();
            break;
        case dma_channel_control_register_1:
            dmac_chcr1_.set(DmaChannelControlRegister::all_bits, data & DmaChannelControlRegister::write_mask);
            executeDma();
            break;
        case dma_operation_register: {
            auto new_dmaor = DmaOperationRegister(data & DmaOperationRegister::write_mask);
            if (dmac_dmaor_.get(DmaOperationRegister::priority_mode) != new_dmaor.get(DmaOperationRegister::priority_mode)) {
                dmac_next_transfer_priority_ = (new_dmaor.get(DmaOperationRegister::priority_mode) == PriorityMode::fixed)
                                                   ? DmaNextTransferPriority::channel_0_first
                                                   : DmaNextTransferPriority::channel_1_first;
            }
            dmac_dmaor_ = new_dmaor;
            executeDma();
        } break;

        //////////////
        // 10. DIVU //
        //////////////
        case divisor_register: divu_dvsr_.set(DivisorRegister::all_bits, data); break;
        case dividend_register_l_32_bits:
            divu_dvdnt_.set(DividendRegister32Bits::all_bits, data);

            // ST-V needs some mirroring
            divu_dvdntl_.set(DividendRegisterL::all_bits, data);
            divu_dvdntl_shadow_.set(DividendRegisterL::all_bits, data);

            // Sign extension for the upper 32 bits if needed

            ((data & sign_bit_32_mask) != 0) ? divu_dvdnth_.set(DividendRegisterH::all_bits, UINT32_MAX)
                                             : divu_dvdnth_.set(DividendRegisterH::all_bits, 0x00000000u);

            start32bitsDivision();
            break;
        case division_control_register: divu_dvcr_.set(DivisionControlRegister::all_bits, data); break;
        case dividend_register_l:
            divu_dvdntl_.set(DividendRegisterL::all_bits, data);
            divu_dvdntl_shadow_.set(DividendRegisterL::all_bits, data);

            start64bitsDivision();
            break;
        case dividend_register_h:
            divu_dvdnth_.set(DividendRegisterH::all_bits, data);
            break;

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

void Sh2::purgeCache() { // NOLINT(readability-convert-member-functions-to-static)
    // All the valid bits and LRU bits are initialized to 0
    for (u8 i = 0; i < cache_lines_number; ++i) {
        // :WARNING: following code is untested
        u32 data = core::rawRead<u32>(cache_addresses_, i);
        data &= cache_purge_mask;
        core::rawWrite<u32>(cache_addresses_, i, data);
    }
}

void Sh2::initializeOnChipRegisters() {
    // Interrupt Control
    intc_ipra_.reset();
    intc_iprb_.reset();
    intc_vcra_.reset();
    intc_vcrb_.reset();
    intc_vcrc_.reset();
    intc_vcrd_.reset();
    intc_vcrwdt_.reset();
    intc_vcrdiv_.reset();  // lower 16 bits are undefined
    intc_vcrdma0_.reset(); // lower 8 bits are undefined
    intc_vcrdma1_.reset(); // lower 8 bits are undefined
    intc_icr_.reset();

    // Bus State Controler registers
    u32 default_bcr1 = (sh2_type_ == Sh2Type::master) ? bsc_bcr1_master_default_value : bsc_bcr1_slave_default_value;
    bsc_bcr1_.set(BusControlRegister1::all_bits, default_bcr1);
    bsc_bcr2_.set(BusControlRegister2::all_bits, bsc_bcr2_default_value);
    bsc_wcr_.set(WaitControlRegister::all_bits, bsc_wcr_default_value);
    bsc_mcr_.reset();
    bsc_rtcsr_.reset();
    bsc_rtcnt_.reset();
    bsc_rtcor_.reset();

    // Cache registers
    cache_ccr_.reset();

    // Direct Memory Access Controler registers
    dmac_tcr0_.reset(); // lower 24 bits are undefined
    dmac_tcr1_.reset(); // lower 24 bits are undefined
    dmac_chcr0_.reset();
    dmac_chcr1_.reset();
    dmac_drcr0_.reset();
    dmac_drcr1_.reset();
    dmac_dmaor_.reset();

    // Division Unit
    divu_dvcr_.reset();

    // Free Running timer
    frt_tier_.set(TimerInterruptEnableRegister::all_bits, frt_tier_default_value);
    frt_ftcsr_.reset();
    frt_frc_.reset();
    frt_ocra_.set(OutputCompareRegister::all_bits, frt_ocra_default_value);
    frt_ocrb_.set(OutputCompareRegister::all_bits, frt_ocrb_default_value);
    frt_tcr_.reset();
    frt_tocr_.set(TimerOutputCompareControlRegister::all_bits, frt_tocr_default_value);
    frt_tcr_.reset();

    // Watch Dog Timer
    wdt_wtcsr_.set(WatchdogTimerControlStatusRegister::all_bits, wdt_wtcsr_default_value);
    wdt_wtcnt_.reset();
    wdt_rstcsr_.set(ResetControlStatusRegister::all_bits, wdt_rstcsr_default_value);

    // Serial Communication Interface
    sci_smr_.reset();
    sci_brr_.set();
    sci_scr_.reset();
    sci_tdr_.set();
    sci_ssr_.set(SerialStatusRegister::all_bits, sci_ssr_default_value);
    sci_rdr_.reset();
}

void Sh2::powerOnReset() {
    pc_                   = memory()->read<u32>(pc_start_vector);
    r_[sp_register_index] = memory()->read<u32>(sp_start_vector);
    vbr_                  = 0;
    sr_.reset();
    gbr_  = 0;
    mach_ = 0;
    macl_ = 0;
    pr_   = 0;

    for (u8 i = 0; i < general_registers_number; ++i) {
        r_[i] = 0;
    }
    initializeOnChipRegisters();
}

void Sh2::start32bitsDivision() {
    // 32/32 division
    Log::debug("sh2", "32/32 division");

    // DVDNT is copied in DVDNTL and DVDNTH
    divu_dvdntl_.set(DividendRegisterL::all_bits, divu_dvdnt_.toU32());

    s32 dvdnt = divu_dvdnt_.toU32();
    if (dvdnt < 0) {
        divu_dvdnth_.set();
    } else {
        divu_dvdnth_.reset();
    }
    s32 dvsr = divu_dvsr_.toU32();

    Log::debug("sh2", "Dividend : {}, divisor : {}", dvdnt, dvsr);

    divu_quot_ = 0;
    divu_rem_  = 0;
    if (divu_dvsr_.any()) {
        divu_quot_ = dvdnt / dvsr;
        divu_rem_  = dvdnt % dvsr;
    } else {
        divu_dvcr_.set(DivisionControlRegister::overflow_flag);
    }

    // Overflow check
    bool is_dvdnt_ovf{(dvdnt & sign_bit_32_mask) != 0};
    bool is_dvsr_ovf{(dvsr & sign_bit_32_mask) != 0};
    if (is_dvdnt_ovf && is_dvsr_ovf) {
        if ((divu_quot_ == INT32_MAX) && ((divu_rem_ & sign_bit_32_mask) != 0)) {
            divu_dvcr_.set(DivisionControlRegister::overflow_flag);
        }
    }

    // 39 cycles for regular division, 6 cycles when overflow is detected
    divu_remaining_cycles_ = (divu_dvcr_.get(DivisionControlRegister::overflow_flag) == OverflowFlag::overflow)
                                 ? divu_overflow_cycles_number
                                 : divu_normal_cycles_number;

    divu_is_running_ = true;
}

void Sh2::start64bitsDivision() {
    Log::debug("sh2", "64/32 division");

    s32 dvdntl = divu_dvdntl_.toU32();
    s32 dvdnth = divu_dvdnth_.toU32();
    s32 dvsr   = divu_dvsr_.toU32();

    s64 dividend = (static_cast<s64>(dvdnth) << number_of_bits_32) | dvdntl;

    Log::debug("sh2", "Dividend : {}, divisor : {}", dividend, dvsr);

    // auto dvcr = DivisionControlRegister(io_registers_[division_control_register & sh2_memory_mask]);
    s64 quotient{};
    s64 remainder{};
    if (divu_dvsr_.any()) {
        quotient  = dividend / dvsr;
        remainder = dividend % dvsr;
    } else {
        divu_dvcr_.set(DivisionControlRegister::overflow_flag);
    }

    // Overflow check
    bool is_dvdnth_ovf{(dvdnth & sign_bit_32_mask) != 0};
    bool is_dvsr_ovf{(dvsr & sign_bit_32_mask) != 0};
    if (is_dvdnth_ovf && is_dvsr_ovf) {
        if ((quotient == INT32_MAX) && ((remainder & sign_bit_32_mask) != 0)) {
            divu_dvcr_.set(DivisionControlRegister::overflow_flag);
        }
    }

    // 39 cycles for regular division, 6 cycles when overflow is detected
    divu_remaining_cycles_ = (divu_dvcr_.get(DivisionControlRegister::overflow_flag) == OverflowFlag::overflow)
                                 ? divu_overflow_cycles_number
                                 : divu_normal_cycles_number;
    divu_quot_ = static_cast<s32>(quotient);
    divu_rem_  = static_cast<s32>(remainder);

    divu_is_running_ = true;
}

void Sh2::runInterruptController() {
    if (!is_interrupted_) {
        if (!pending_interrupts_.empty()) {
            u8   interrupt_mask = sr_.get(StatusRegister::i);
            auto interrupt      = pending_interrupts_.front();
            if (interrupt.level > interrupt_mask) {
                Log::debug("sh2",
                           "{} SH2 interrupt request {:#0x} {:#0x}, PC={:#0x}",
                           sh2_type_name_.at(sh2_type_),
                           interrupt.vector,
                           interrupt.level,
                           pc_);

                is_level_interrupted_[interrupt.level] = false;

                // SR and PC are saved to the stack.
                memory()->write(r_[sp_register_index] - sr_stack_offset, sr_.toU32());
                memory()->write(r_[sp_register_index] - pc_stack_offset, pc_);

                r_[sp_register_index] = r_[sp_register_index] - displacement_8; // Stack pointer is updated.

                sr_.set(StatusRegister::i, interrupt.level);

                if (interrupt != is::nmi) {
                    is_interrupted_    = true; // Entering interrupt mode.
                    current_interrupt_ = interrupt;
                    Log::debug("sh2",
                               "{} SH2 {} interrupt routine started, pc={:#0x}",
                               sh2_type_name_.at(sh2_type_),
                               interrupt.name,
                               pc_);
                }
                pc_ = memory()->read<u32>(interrupt.vector * 4 + vbr_);

                pending_interrupts_.pop_front(); // Interrupt is removed from the list.
            }
        }
    }
}

void Sh2::runDivisionUnit(const u8 cycles_to_run) {
    divu_remaining_cycles_ -= cycles_to_run;
    if (divu_remaining_cycles_ == 0) {
        // auto dvcr = DivisionControlRegister(io_registers_[division_control_register & sh2_memory_mask]);
        if (divu_dvcr_.get(DivisionControlRegister::overflow_flag) == OverflowFlag::overflow) {
            if (divu_dvcr_.get(DivisionControlRegister::interrupt_enable) == core::InterruptEnable::enabled) {
                Log::debug("sh2", "DIVU - Sending division overflow interrupt");
                is::sh2_division_overflow.vector = intc_vcrdiv_.get(VectorNumberSettingRegisterDiv::divu_interrupt_vector);
                is::sh2_division_overflow.level  = intc_ipra_.get(InterruptPriorityLevelSettingRegisterA::divu_level);
                sendInterrupt(is::sh2_division_overflow);
            }
        } else {
            // Copy in DVDNTL and DVDNTH + ST-V mirroring
            divu_dvdnt_.set(DividendRegister32Bits::all_bits, static_cast<u32>(divu_quot_));
            divu_dvdntl_.set(DividendRegisterL::all_bits, static_cast<u32>(divu_quot_));
            divu_dvdntl_shadow_.set(DividendRegisterL::all_bits, static_cast<u32>(divu_quot_));

            divu_dvdnth_.set(DividendRegisterH::all_bits, static_cast<u32>(divu_rem_));
            divu_dvdnth_shadow_.set(DividendRegisterH::all_bits, static_cast<u32>(divu_rem_));
        }
        divu_is_running_ = false;
    }
}

void Sh2::runFreeRunningTimer(const u8 cycles_to_run) {
    u32 elapsed_cycles{frt_elapsed_cycles_ + cycles_to_run};
    u32 counter_increment{elapsed_cycles / frt_clock_divisor_};
    u32 cycles_remainder{elapsed_cycles % frt_clock_divisor_};

    if (counter_increment > 0) {
        u32 old_frc{frt_frc_.get(FreeRunningCounter::all_bits)};
        u32 current_frc{old_frc + counter_increment};
        frt_frc_.set(FreeRunningCounter::all_bits, static_cast<u16>(current_frc));

        frt_elapsed_cycles_ = elapsed_cycles & frt_mask_;

        // Checking overflow
        if (current_frc > UINT16_MAX) {
            frt_ftcsr_.set(FreeRunningTimerControlStatusRegister::timer_overflow_flag);
            if (frt_tier_.get(TimerInterruptEnableRegister::timer_overflow_interrupt_enable)
                == TimerOverflowInterruptEnable::interrupt_request_enabled) {
                Log::debug("sh2", "FRT - Sending overflow interrupt");
                is::sh2_frt_overflow_flag_set.vector = intc_vcrd_.get(VectorNumberSettingRegisterD::frt_overflow_vector);
                is::sh2_frt_overflow_flag_set.level  = intc_iprb_.get(InterruptPriorityLevelSettingRegisterB::frt_level);
                sendInterrupt(is::sh2_frt_overflow_flag_set);
            }
        }

        // Checking comparison for OCRA
        u32 ocra = frt_ocra_.toU32();
        if ((old_frc <= ocra) && (current_frc > ocra)) {
            frt_ftcsr_.set(FreeRunningTimerControlStatusRegister::output_compare_flag_a);
            if (frt_tier_.get(TimerInterruptEnableRegister::output_compare_interrupt_a_enable)
                == OutputCompareInterruptAEnable::interrupt_request_enabled) {
                Log::debug("sh2", "FRT - OCRA match");
                is::sh2_frt_output_compare_flag_a_set.vector
                    = intc_vcrc_.get(VectorNumberSettingRegisterC::frt_output_compare_vector);
                is::sh2_frt_output_compare_flag_a_set.level = intc_iprb_.get(InterruptPriorityLevelSettingRegisterB::frt_level);
                sendInterrupt(is::sh2_frt_output_compare_flag_a_set);
            }
            if (frt_ftcsr_.get(FreeRunningTimerControlStatusRegister::counter_clear_a) == CounterClearA::clear_on_compare) {
                frt_frc_.reset();
            }
        }

        // Checking comparison for OCRB
        u32 ocrb = frt_ocrb_.toU32();
        if ((old_frc <= ocrb) && (current_frc > ocrb)) {
            frt_ftcsr_.set(FreeRunningTimerControlStatusRegister::output_compare_flag_b);
            if (frt_tier_.get(TimerInterruptEnableRegister::output_compare_interrupt_b_enable)
                == OutputCompareInterruptBEnable::interrupt_request_enabled) {
                Log::debug("sh2", "FRT - OCRB match");
                is::sh2_frt_output_compare_flag_b_set.vector
                    = intc_vcrc_.get(VectorNumberSettingRegisterC::frt_output_compare_vector);
                is::sh2_frt_output_compare_flag_b_set.level = intc_iprb_.get(InterruptPriorityLevelSettingRegisterB::frt_level);
                sendInterrupt(is::sh2_frt_output_compare_flag_b_set);
            }
        }

    } else {
        frt_elapsed_cycles_ += cycles_to_run;
    }
}

void Sh2::executeDma() {
    if (dmac_dmaor_.get(DmaOperationRegister::dma_master_enable) == DmaMasterEnable::disabled) {
        return;
    }

    auto conf_channel_0{configureDmaTransfer(DmaChannel::channel_0)};
    auto conf_channel_1{configureDmaTransfer(DmaChannel::channel_1)};

    switch (dmac_next_transfer_priority_) {
        case DmaNextTransferPriority::channel_0_first:
            executeDmaOnChannel(conf_channel_0);
            executeDmaOnChannel(conf_channel_1);
            break;
        case DmaNextTransferPriority::channel_1_first:
            executeDmaOnChannel(conf_channel_1);
            executeDmaOnChannel(conf_channel_0);
            break;
    }

    if (dmac_dmaor_.get(DmaOperationRegister::priority_mode) == PriorityMode::round_robin) {
        switch (dmac_next_transfer_priority_) {
            case DmaNextTransferPriority::channel_0_first:
                dmac_next_transfer_priority_ = DmaNextTransferPriority::channel_1_first;
                break;
            case DmaNextTransferPriority::channel_1_first:
                dmac_next_transfer_priority_ = DmaNextTransferPriority::channel_0_first;
                break;
        }
    }
}

auto Sh2::dmaStartConditionsAreSatisfied(const DmaChannel dc) -> bool { // NOLINT(readability-convert-member-functions-to-static)
    // DE=1 TE=0 NMIF=0 AE=0
    switch (dc) {
        case DmaChannel::channel_0: {
            bool channel_0_is_set = dmac_chcr0_.get(DmaChannelControlRegister::dma_enable) == Sh2DmaEnable::dma_transfer_enabled;
            channel_0_is_set
                &= dmac_chcr0_.get(DmaChannelControlRegister::transfer_end_flag) == TransferEndFlag::dma_not_ended_or_aborted;
            channel_0_is_set &= dmac_dmaor_.get(DmaOperationRegister::nmi_flag) == NmiFlag::no_nmif_interrupt;
            channel_0_is_set
                &= dmac_dmaor_.get(DmaOperationRegister::address_error_flag) == AddressErrorFlag::no_dmac_address_error;
            return channel_0_is_set;
        }
        case DmaChannel::channel_1: {
            bool channel_1_is_set = dmac_chcr1_.get(DmaChannelControlRegister::dma_enable) == Sh2DmaEnable::dma_transfer_enabled;
            channel_1_is_set
                &= dmac_chcr1_.get(DmaChannelControlRegister::transfer_end_flag) == TransferEndFlag::dma_not_ended_or_aborted;
            channel_1_is_set &= dmac_dmaor_.get(DmaOperationRegister::nmi_flag) == NmiFlag::no_nmif_interrupt;
            channel_1_is_set
                &= dmac_dmaor_.get(DmaOperationRegister::address_error_flag) == AddressErrorFlag::no_dmac_address_error;
            return channel_1_is_set;
        }
        case DmaChannel::channel_unknown: return false;
    }
    return false;
}

auto Sh2::configureDmaTransfer(const DmaChannel dc) -> Sh2DmaConfiguration {
    Sh2DmaConfiguration conf;
    switch (dc) {
        case DmaChannel::channel_0:
            conf.channel     = DmaChannel::channel_0;
            conf.counter     = dmac_tcr0_.toU32() & bitmask_00FFFFFF;
            conf.source      = dmac_sar0_.toU32();
            conf.destination = dmac_dar0_.toU32();
            conf.chcr        = dmac_chcr0_;
            conf.interrupt   = is::sh2_dma_0_transfer_end;
            break;
        case DmaChannel::channel_1:
            conf.channel     = DmaChannel::channel_1;
            conf.counter     = dmac_tcr1_.toU32() & bitmask_00FFFFFF;
            conf.source      = dmac_sar1_.toU32();
            conf.destination = dmac_dar1_.toU32();
            conf.chcr        = dmac_chcr1_;
            conf.interrupt   = is::sh2_dma_1_transfer_end;
            break;
        default: Log::warning("sh2", "DMAC - Unknown DMA channel");
    }
    return conf;
}

void Sh2::executeDmaOnChannel(Sh2DmaConfiguration& conf) {
    if (dmaStartConditionsAreSatisfied(conf.channel)) {
        u8 channel_number = (conf.channel == DmaChannel::channel_0) ? 0 : 1;
        if (conf.chcr.get(DmaChannelControlRegister::auto_request_mode) == AutoRequestMode::module_request) {
            Log::warning("sh2", "DMAC - Channel {} module request not implemented !", channel_number);
        } else {
            u32 counter{conf.counter};
            u32 source{conf.source};
            u32 destination{conf.destination};
            Log::debug("sh2", "DMAC - Channel {} transfer", channel_number);
            Log::debug("sh2", "PC={:#0x}", pc_);
            Log::debug("sh2", "Source:{:#0x}", source);
            Log::debug("sh2", "Destination:{:#0x}", destination);
            Log::debug("sh2", "Count:{:#0x}", counter);

            while (counter > 0) {
                u8 transfer_size{};
                switch (conf.chcr.get(DmaChannelControlRegister::transfer_size)) {
                    case TransferSize::one_byte_unit:
                        memory()->write<u8>(destination, memory()->read<u8>(source));
                        transfer_size = transfer_byte_size_1;
                        --counter;
                        break;
                    case TransferSize::two_byte_unit:
                        memory()->write<u16>(destination, memory()->read<u16>(source));
                        transfer_size = transfer_byte_size_2;
                        --counter;
                        break;
                    case TransferSize::four_byte_unit:
                        memory()->write<u32>(destination, memory()->read<u32>(source));
                        transfer_size = transfer_byte_size_4;
                        --counter;
                        break;
                    case TransferSize::sixteen_byte_unit:
                        memory()->write<u32>(destination, memory()->read<u32>(source));
                        memory()->write<u32>(destination + displacement_4, memory()->read<u32>(source + displacement_4));
                        memory()->write<u32>(destination + displacement_8, memory()->read<u32>(source + displacement_8));
                        memory()->write<u32>(destination + displacement_12, memory()->read<u32>(source + displacement_12));
                        transfer_size = transfer_byte_size_16;
                        counter -= 4;
                        break;
                }

                switch (conf.chcr.get(DmaChannelControlRegister::source_address_mode)) {
                    case SourceAddressMode::fixed: break;
                    case SourceAddressMode::incremented: source += transfer_size; break;
                    case SourceAddressMode::decremented: source -= transfer_size; break;
                }

                switch (conf.chcr.get(DmaChannelControlRegister::destination_address_mode)) {
                    case DestinationAddressMode::fixed: break;
                    case DestinationAddressMode::incremented: destination += transfer_size; break;
                    case DestinationAddressMode::decremented: destination -= transfer_size; break;
                }
            }

            switch (conf.channel) {
                case DmaChannel::channel_0:
                    dmac_tcr0_.set(DmaTransferCountRegister::all_bits, counter);
                    dmac_sar0_.set(DmaSourceAddressRegister::all_bits, source);
                    dmac_dar0_.set(DmaDestinationAddressRegister::all_bits, destination);
                    dmac_chcr0_.set(DmaChannelControlRegister::transfer_end_flag);
                    break;
                case DmaChannel::channel_1:
                    dmac_tcr1_.set(DmaTransferCountRegister::all_bits, counter);
                    dmac_sar1_.set(DmaSourceAddressRegister::all_bits, source);
                    dmac_dar1_.set(DmaDestinationAddressRegister::all_bits, destination);
                    dmac_chcr1_.set(DmaChannelControlRegister::transfer_end_flag);
                    break;
            }

            if (conf.chcr.get(DmaChannelControlRegister::interrupt_enable) == Sh2DmaInterruptEnable::enabled) {
                Log::debug("sh2", "DMAC - Sending DMA channel {} transfer end interrupt.", channel_number);
                conf.interrupt.vector = intc_vcrc_.get(VectorNumberSettingRegisterDma::dma_transfert_end_vector);
                conf.interrupt.level  = intc_iprb_.get(InterruptPriorityLevelSettingRegisterA::dmac_level);
                sendInterrupt(conf.interrupt);
            }
        }
    };
}

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

                Log::debug("sh2", "{} SH2 interrupt pending : {:#0x}", sh2_type_name_.at(sh2_type_), i.vector);
            }
        } else {
            // Max number of pending interrupts reached, nothing is added
            Log::debug("sh2", "Maximum number of pending interrupts reached");

            // When the interrupt is NMI, the lower priority interrupt is removed
            if (i.vector == is::vector_nmi) {
                pending_interrupts_.pop_back();
                pending_interrupts_.push_front(is::nmi);

                // Sorting (greatest priority first)
                pending_interrupts_.sort();
                pending_interrupts_.reverse();

                Log::debug("sh2", "NMI interrupt forced");
            }
        }
    }
}

void Sh2::sendInterruptCaptureSignal() {
    if (frt_tier_.get(TimerInterruptEnableRegister::interrupt_capture_interrupt_enable)
        == InterruptCaptureInterruptEnable::interrupt_request_enabled) {
        is::sh2_frt_input_capture.vector = intc_vcrc_.get(VectorNumberSettingRegisterC::frt_input_capture_vector);
        is::sh2_frt_input_capture.level  = intc_iprb_.get(InterruptPriorityLevelSettingRegisterB::frt_level);
        sendInterrupt(is::sh2_frt_input_capture);
    }

    frt_ftcsr_.set(FreeRunningTimerControlStatusRegister::input_capture_flag);
    frt_icr_.set(InputCaptureRegister::all_bits, frt_frc_.get(FreeRunningCounter::all_bits));
}

auto Sh2::run() -> u8 {
    runInterruptController();
    u8 cycles_to_run = 1; // Will have to be changed when instruction execution is plugged in

    runDivisionUnit(cycles_to_run);
    runFreeRunningTimer(cycles_to_run);
    return cycles_to_run;
}

} // namespace saturnin::sh2
