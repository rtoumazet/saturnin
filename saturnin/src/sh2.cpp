// 
// sh2.cpp
// Saturnin
//
// Copyright (c) 2018-2019 Renaud Toumazet
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

namespace is = saturnin::core::interrupt_source;

namespace saturnin {
namespace sh2 {

using core::Log;

Sh2::Sh2(Sh2Type st, core::Emulator_context* ec) : sh2_type_(st), emulator_context_(ec) {
    reset();
}

core::Memory* Sh2::memory() const {
    return emulator_context_->memory();
}

Scu* Sh2::scu() const {
    return emulator_context_->scu();
}

core::Emulator_context* Sh2::emulatorContext(){
    return emulator_context_;
}

u8 Sh2::readRegisters8(const u32 addr) {
    switch (addr) {
        /////////////
        // 5. INTC //
        /////////////
        case interrupt_priority_level_setting_register_a:     return intc_ipra_.get(InterruptPriorityLevelSettingRegisterA::upper_8_bits);
        case interrupt_priority_level_setting_register_a + 1: return intc_ipra_.get(InterruptPriorityLevelSettingRegisterA::lower_8_bits);
        case interrupt_priority_level_setting_register_b:     return intc_iprb_.get(InterruptPriorityLevelSettingRegisterB::upper_8_bits);
        case interrupt_priority_level_setting_register_b + 1: return intc_iprb_.get(InterruptPriorityLevelSettingRegisterB::lower_8_bits);
        case vector_number_setting_register_a:                return intc_vcra_.get(VectorNumberSettingRegisterA::upper_8_bits);
        case vector_number_setting_register_a + 1:            return intc_vcra_.get(VectorNumberSettingRegisterA::lower_8_bits);
        case vector_number_setting_register_b:                return intc_vcrb_.get(VectorNumberSettingRegisterB::upper_8_bits);
        case vector_number_setting_register_b + 1:            return intc_vcrb_.get(VectorNumberSettingRegisterB::lower_8_bits);
        case vector_number_setting_register_c:                return intc_vcrc_.get(VectorNumberSettingRegisterC::upper_8_bits);
        case vector_number_setting_register_c + 1:            return intc_vcrc_.get(VectorNumberSettingRegisterC::lower_8_bits);
        case vector_number_setting_register_d:                return intc_vcrd_.get(VectorNumberSettingRegisterD::upper_8_bits);
        case vector_number_setting_register_d + 1:            return intc_vcrd_.get(VectorNumberSettingRegisterD::lower_8_bits);
        case vector_number_setting_register_wdt:              return intc_vcrwdt_.get(VectorNumberSettingRegisterWdt::upper_8_bits);
        case vector_number_setting_register_wdt + 1:          return intc_vcrwdt_.get(VectorNumberSettingRegisterWdt::lower_8_bits);
        case interrupt_control_register:                      return intc_icr_.get(InterruptControlRegister::upper_8_bits);
        case interrupt_control_register + 1:                  return intc_icr_.get(InterruptControlRegister::lower_8_bits);
        /////////////
        // 7. BSC //
        /////////////

        //////////////
        // 8. Cache //
        //////////////
        
        /////////////
        // 9. DMAC //
        /////////////
        
        //////////////
        // 10. DIVU //
        ////////////// 
        
        
        /////////////
        // 11. FRT //
        /////////////
        case timer_interrupt_enable_register:            return frt_tier_.get(TimerInterruptEnableRegister::all_bits);
        case free_running_timer_control_status_register: return frt_ftcsr_.get(FreeRunningTimerControlStatusRegister::all_bits);
        case free_running_counter:                       return frt_frc_.get(FreeRunningCounter::upper_8_bits);
        case free_running_counter + 1:                   return frt_frc_.get(FreeRunningCounter::lower_8_bits);
        case timer_control_register:                     return frt_tcr_.get(TimerControlRegister::all_bits);
        case timer_output_compare_control_register:      return frt_tocr_.get(TimerOutputCompareControlRegister::all_bits);
        case input_capture_register:                     return frt_icr_.get(InputCaptureRegister::upper_8_bits);
        case input_capture_register + 1:                 return frt_icr_.get(InputCaptureRegister::lower_8_bits);

        /////////////
        // 12. WDT //
        /////////////

        /////////////
        // 13. SCI //
        /////////////

        default: 
            unmappedAccess(addr);
            return 0;
    }
}

u16 Sh2::readRegisters16(const u32 addr) {
    switch (addr) {
        /////////////
        // 5. INTC //
        /////////////
        case interrupt_priority_level_setting_register_a: return intc_ipra_.get(InterruptPriorityLevelSettingRegisterA::all_bits);
        case interrupt_priority_level_setting_register_b: return intc_iprb_.get(InterruptPriorityLevelSettingRegisterB::all_bits);
        case vector_number_setting_register_a:            return intc_vcra_.get(VectorNumberSettingRegisterA::all_bits);
        case vector_number_setting_register_b:            return intc_vcrb_.get(VectorNumberSettingRegisterB::all_bits);
        case vector_number_setting_register_c:            return intc_vcrc_.get(VectorNumberSettingRegisterC::all_bits);
        case vector_number_setting_register_d:            return intc_vcrd_.get(VectorNumberSettingRegisterD::all_bits);
        case vector_number_setting_register_wdt:          return intc_vcrwdt_.get(VectorNumberSettingRegisterWdt::all_bits);
        case interrupt_control_register:                  return intc_icr_.get(InterruptControlRegister::all_bits);
        case vector_number_setting_register_div:          return intc_vcrdiv_.get(VectorNumberSettingRegisterDiv::upper_16_bits);
        case vector_number_setting_register_div + 2:      return intc_vcrdiv_.get(VectorNumberSettingRegisterDiv::lower_16_bits);

        /////////////
        // 7. BSC //
        /////////////

        //////////////
        // 8. Cache //
        //////////////

        /////////////
        // 9. DMAC //
        /////////////

        //////////////
        // 10. DIVU //
        ////////////// 
        case division_control_register:              return divu_dvcr_.get(DivisionControlRegister::upper_16_bits);
        case division_control_register + 2:          return divu_dvcr_.get(DivisionControlRegister::lower_16_bits);

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
            unmappedAccess(addr);
            return 0;
    }
}

u32 Sh2::readRegisters32(const u32 addr) {
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

        //////////////
        // 8. Cache //
        //////////////

        /////////////
        // 9. DMAC //
        /////////////

        //////////////
        // 10. DIVU //
        ////////////// 
        case divisor_register:                   return divu_dvsr_.toU32();
        case dividend_register_l_32_bits:        return divu_dvdnt_.toU32();
        case division_control_register:          return divu_dvcr_.toU32();
        case dividend_register_l:                
        case dividend_register_l_shadow:         return divu_dvdntl_.toU32();
        case dividend_register_h:                
        case dividend_register_h_shadow:         return divu_dvdnth_.toU32();


        /////////////
        // 11. FRT //
        /////////////

        /////////////
        // 12. WDT //
        /////////////

        /////////////
        // 13. SCI //
        /////////////




        ///////////////
        case bus_control_register1:
        case bus_control_register2:
        case wait_state_control_register:
        case individual_memory_control_register:
        case refresh_timer_control_status_register:
        case refresh_timer_counter:
        case refresh_time_constant_register: 
            return (core::rawRead<u32>(io_registers_, addr & sh2_memory_mask) & 0x0000FFFF);
        case dma_operation_register:         
            return (core::rawRead<u32>(io_registers_, addr & sh2_memory_mask) & 0x0000000F);
        default:
            unmappedAccess(addr);
            return 0;
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
        case vector_number_setting_register_a:
            intc_vcra_.set(VectorNumberSettingRegisterA::upper_8_bits, data);
            break;
        case vector_number_setting_register_a + 1:
            intc_vcra_.set(VectorNumberSettingRegisterA::lower_8_bits, data);
            break;
        case vector_number_setting_register_b:
            intc_vcrb_.set(VectorNumberSettingRegisterB::upper_8_bits, data);
            break;
        case vector_number_setting_register_b + 1:
            intc_vcrb_.set(VectorNumberSettingRegisterB::lower_8_bits, data);
            break;
        case vector_number_setting_register_c:
            intc_vcrc_.set(VectorNumberSettingRegisterC::upper_8_bits, data);
            break;
        case vector_number_setting_register_c + 1:
            intc_vcrc_.set(VectorNumberSettingRegisterC::lower_8_bits, data);
            break;
        case vector_number_setting_register_d:
            intc_vcrd_.set(VectorNumberSettingRegisterD::upper_8_bits, data);
            break;
        case vector_number_setting_register_d + 1: break;// Read only
        case vector_number_setting_register_wdt:
            intc_vcrwdt_.set(VectorNumberSettingRegisterWdt::upper_8_bits, data);
            break;
        case vector_number_setting_register_wdt + 1:
            intc_vcrwdt_.set(VectorNumberSettingRegisterWdt::lower_8_bits, data);
            break;
        case interrupt_control_register:
            intc_icr_.set(InterruptControlRegister::upper_8_bits, data);
            break;
        case interrupt_control_register + 1:
            intc_icr_.set(InterruptControlRegister::lower_8_bits, data);
            break;
        /////////////
        // 7. BSC  //
        /////////////

        //////////////
        // 8. Cache //
        //////////////

        /////////////
        // 9. DMAC //
        /////////////

        //////////////
        // 10. DIVU //
        ////////////// 

        /////////////
        // 11. FRT //
        /////////////
        case timer_interrupt_enable_register:
            frt_tier_.set(TimerInterruptEnableRegister::all_bits, data);
            break;
        case free_running_timer_control_status_register:
            frt_ftcsr_.set(FreeRunningTimerControlStatusRegister::all_bits, data);
            break;
        case free_running_counter:
            frt_frc_.set(FreeRunningCounter::upper_8_bits, data);
            break;
        case free_running_counter + 1:
            frt_frc_.set(FreeRunningCounter::lower_8_bits, data);
            break;
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
                case ClockSelect::internal_divided_by_8:
                    frt_clock_divisor_ = 8;
                    frt_mask_ = 0b00000111;
                    break;
                case ClockSelect::internal_divided_by_32:
                    frt_clock_divisor_ = 32;
                    frt_mask_ = 0b00011111;
                    break;
                case ClockSelect::internal_divided_by_128:
                    frt_clock_divisor_ = 128;
                    frt_mask_ = 0b01111111;
                    break;
                case ClockSelect::external:
                    Log::warning("sh2", "FRT - External clock not implemented");
                    break;
            }
            break;
        case timer_output_compare_control_register:
            frt_tocr_.set(TimerOutputCompareControlRegister::all_bits, static_cast<u8>(data & TimerOutputCompareControlRegister::accessMask())); 
            break;
        case input_capture_register:
            frt_icr_.set(InputCaptureRegister::upper_8_bits, data);
            break;
        case input_capture_register + 1:
            frt_icr_.set(InputCaptureRegister::lower_8_bits, data);
            break;



        case cache_control_register: {
            Log::debug("sh2", "CCR byte write: {}", data);

                auto ccr = CacheControlRegister(data);
                if (ccr.get(CacheControlRegister::cache_purge) == CachePurge::cache_purge) {
                    purgeCache();

                    // cache purge bit is cleared after operation
                    ccr.reset(CacheControlRegister::cache_purge);
                    data = ccr.toU32();
                }
            }
            break;
        default:
            unmappedAccess(addr, data);
            break;
    }
    //core::rawWrite<u8>(io_registers_, addr & sh2_memory_mask, data);
}

void Sh2::writeRegisters(u32 addr, u16 data) {
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
        case vector_number_setting_register_a:
            intc_vcra_.set(VectorNumberSettingRegisterA::all_bits, data);
            break;
        case vector_number_setting_register_b:
            intc_vcrb_.set(VectorNumberSettingRegisterB::all_bits, data);
            break;
        case vector_number_setting_register_c:
            intc_vcrc_.set(VectorNumberSettingRegisterC::all_bits, data);
            break;
        case vector_number_setting_register_d:
            intc_vcrd_.set(VectorNumberSettingRegisterD::all_bits, data);
            break;
        case vector_number_setting_register_wdt:
            intc_vcrwdt_.set(VectorNumberSettingRegisterWdt::all_bits, data);
            break;
        case vector_number_setting_register_div: break; // Read only access
        case vector_number_setting_register_div + 2:
            intc_vcrdiv_.set(VectorNumberSettingRegisterDiv::lower_16_bits, static_cast<u16>(data & DivisionControlRegister::accessMask()));
        case interrupt_control_register: {
            auto new_icr = InterruptControlRegister(data);
            switch (intc_icr_.get(InterruptControlRegister::nmi_edge_detection)) {
                case NmiEdgeDetection::falling:
                    if ((intc_icr_.get(InterruptControlRegister::nmi_input_level) == NmiInputLevel::high) &&
                            (new_icr.get(InterruptControlRegister::nmi_input_level) == NmiInputLevel::low)) {
                        Log::error("sh2", "Falling edge NMI, not implemented !");
                    }
                    break;
                case NmiEdgeDetection::rising:
                    if ((intc_icr_.get(InterruptControlRegister::nmi_input_level) == NmiInputLevel::low) &&
                            (new_icr.get(InterruptControlRegister::nmi_input_level) == NmiInputLevel::high)) {
                        Log::error("sh2", "Rising edge NMI, not implemented !");
                    }
                    break;
            }
            intc_icr_.set(InterruptControlRegister::all_bits, data);
        }
            break;

        /////////////
        // 7. BSC //
        /////////////

        //////////////
        // 8. Cache //
        //////////////

        /////////////
        // 9. DMAC //
        /////////////

        //////////////
        // 10. DIVU //
        ////////////// 
        case division_control_register: break;// Read only access
        case division_control_register + 2:          
            divu_dvcr_.set(DivisionControlRegister::lower_16_bits, static_cast<u16>(data & DivisionControlRegister::accessMask()));
            break;

        /////////////
        // 11. FRT //
        /////////////
        case free_running_counter:
            frt_frc_.set(FreeRunningCounter::all_bits, data);
            break;
        case output_compare_register:
            switch (frt_tocr_.get(TimerOutputCompareControlRegister::output_compare_register_select)) {
                case OutputCompareRegisterSelect::ocra: frt_ocra_.set(OutputCompareRegister::all_bits, data); break;
                case OutputCompareRegisterSelect::ocrb: frt_ocrb_.set(OutputCompareRegister::all_bits, data); break;
            }
            break;
        case input_capture_register:
            frt_icr_.set(InputCaptureRegister::all_bits, data);
            break;






        case bus_control_register1 + 2:
            core::rawWrite<u16>(io_registers_, addr & sh2_memory_mask, data & 0x00F7);
            break;
        case bus_control_register2 + 2:
            core::rawWrite<u16>(io_registers_, addr & sh2_memory_mask, data & 0x00FC);
            break;
        default:
            //core::rawWrite<u16>(io_registers_, addr & sh2_memory_mask, data);
            unmappedAccess(addr, data);
            break;
    }
}

void Sh2::writeRegisters(u32 addr, u32 data) {
    switch (addr) {
        /////////////
        // 5. INTC //
        /////////////
        case vector_number_setting_register_div:
            intc_vcrdiv_.set(VectorNumberSettingRegisterDiv::all_bits, data);
            break;
        case dma_vector_number_register_0:
            intc_vcrdma0_.set(VectorNumberSettingRegisterDma::all_bits, data);
            break;
        case dma_vector_number_register_1:
            intc_vcrdma1_.set(VectorNumberSettingRegisterDma::all_bits, data);
            break;
        /////////////
        // 7. BSC //
        /////////////

        //////////////
        // 8. Cache //
        //////////////

        /////////////
        // 9. DMAC //
        /////////////

        //////////////
        // 10. DIVU //
        ////////////// 
        case divisor_register: 
            divu_dvsr_.set(DivisorRegister::all_bits, data);
            break;
        case dividend_register_l_32_bits:
            divu_dvdnt_.set(DividendRegister32Bits::all_bits, data);
            
            // ST-V needs some mirroring
            divu_dvdntl_.set(DividendRegisterL::all_bits, data);
            divu_dvdntl_shadow_.set(DividendRegisterL::all_bits, data);

            // Sign extension for the upper 32 bits if needed
            (data & 0x80000000) ? divu_dvdnth_.set(DividendRegisterH::all_bits, 0xFFFFFFFFu)
                : divu_dvdnth_.set(DividendRegisterH::all_bits, 0x00000000u);

            start32bitsDivision();
            break;
        case division_control_register:          
            divu_dvcr_.set(DivisionControlRegister::all_bits, data);
            break;
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




        case bus_control_register1:
            if ((data & 0xFFFF0000) == 0xA55A0000) {
                core::rawWrite<u16>(io_registers_, addr + 2 & sh2_memory_mask, data & BusControlRegister1::writeMask());
            }
            break;
        case bus_control_register2:
            if ((data & 0xFFFF0000) == 0xA55A0000) {
                core::rawWrite<u16>(io_registers_, addr + 2 & sh2_memory_mask, data & BusControlRegister2::writeMask());
            }
            break;
        case dma_channel_control_register_0:
            core::rawWrite<u32>(io_registers_, dma_channel_control_register_0 & sh2_memory_mask, data);

            if (DmaChannelControlRegister(data).get(DmaChannelControlRegister::interrupt_enable) == Sh2DmaInterruptEnable::enabled) {
                auto dor = DmaOperationRegister(core::rawRead<u32>(io_registers_, dma_operation_register & sh2_memory_mask));
                if (dor.get(DmaOperationRegister::dma_master_enable) == DmaMasterEnable::enabled) {
                    executeDma();
                }
            }
            break;
        case dma_channel_control_register_1:
            core::rawWrite<u32>(io_registers_, dma_channel_control_register_1 & 0x1FF, data);

            if (DmaChannelControlRegister(data).get(DmaChannelControlRegister::interrupt_enable) == Sh2DmaInterruptEnable::enabled) {
                auto dor = DmaOperationRegister(core::rawRead<u32>(io_registers_, dma_operation_register & sh2_memory_mask));
                if (dor.get(DmaOperationRegister::dma_master_enable) == DmaMasterEnable::enabled) {
                    executeDma();
                }
            }
            break;
        case dma_operation_register:
            core::rawWrite<u32>(io_registers_, dma_operation_register & sh2_memory_mask, data);
            if (DmaOperationRegister(data).get(DmaOperationRegister::dma_master_enable) == DmaMasterEnable::enabled) {
                executeDma();
            }
            break;
        default:
            //core::rawWrite<u32>(io_registers_, addr & sh2_memory_mask, data);
            unmappedAccess(addr, data);
            break;
    }

}

void Sh2::purgeCache() {
    // All the valid bits and LRU bits are initialized to 0
    for (u8 i = 0; i < 32; ++i) {
        // :WARNING: the following code is untested
        u32 data = core::rawRead<u32>(cache_addresses_, i);
        data &= 0xFFFFFC0B;
        core::rawWrite<u32>(cache_addresses_, i, data);
    }
}

void Sh2::initializeOnChipRegisters() {
    // Interrupt Control
    intc_ipra_.set(InterruptPriorityLevelSettingRegisterA::all_bits, static_cast<u16>(0x0000));
    intc_iprb_.set(InterruptPriorityLevelSettingRegisterB::all_bits, static_cast<u16>(0x0000));
    intc_vcra_.set(VectorNumberSettingRegisterA::all_bits, static_cast<u16>(0x0000));
    intc_vcrb_.set(VectorNumberSettingRegisterB::all_bits, static_cast<u16>(0x0000));
    intc_vcrc_.set(VectorNumberSettingRegisterC::all_bits, static_cast<u16>(0x0000));
    intc_vcrd_.set(VectorNumberSettingRegisterD::all_bits, static_cast<u16>(0x0000));
    intc_vcrwdt_.set(VectorNumberSettingRegisterWdt::all_bits, static_cast<u16>(0x0000));
    intc_vcrdiv_.set(VectorNumberSettingRegisterDiv::all_bits, 0x00000000u); // lower 16 bits are undefined
    intc_vcrdma0_.set(VectorNumberSettingRegisterDma::all_bits, 0x00000000u); // lower 8 bits are undefined
    intc_vcrdma1_.set(VectorNumberSettingRegisterDma::all_bits, 0x00000000u); // lower 8 bits are undefined
    intc_icr_.set(InterruptControlRegister::all_bits, static_cast<u16>(0x0000));

    // Bus State Controler registers
    switch (sh2_type_) {
        case Sh2Type::master: core::rawWrite<u32>(io_registers_, bus_control_register1 & sh2_memory_mask, 0x000003F0); break;
        case Sh2Type::slave:  core::rawWrite<u32>(io_registers_, bus_control_register1 & sh2_memory_mask, 0x000083F0); break;
        default: Log::error("sh2", "Unknown SH2 type");
    }
   
    core::rawWrite<u32>(io_registers_, bus_control_register2                 & sh2_memory_mask, 0x000000FC);
    core::rawWrite<u32>(io_registers_, wait_state_control_register           & sh2_memory_mask, 0x0000AAFF);
    core::rawWrite<u32>(io_registers_, individual_memory_control_register    & sh2_memory_mask, 0x00000000);
    core::rawWrite<u32>(io_registers_, refresh_timer_control_status_register & sh2_memory_mask, 0x00000000);
    core::rawWrite<u32>(io_registers_, refresh_timer_counter                 & sh2_memory_mask, 0x00000000);
    core::rawWrite<u32>(io_registers_, refresh_time_constant_register        & sh2_memory_mask, 0x00000000);

    // Direct Memory Access Controler registers
    core::rawWrite<u32>(io_registers_, dma_channel_control_register_0                   & sh2_memory_mask, 0x00000000);
    core::rawWrite<u8>(io_registers_, dma_request_response_selection_control_register_0 & sh2_memory_mask, 0x00);
    core::rawWrite<u32>(io_registers_, dma_channel_control_register_1                   & sh2_memory_mask, 0x00000000);
    core::rawWrite<u8>(io_registers_, dma_request_response_selection_control_register_1 & sh2_memory_mask, 0x00);
    core::rawWrite<u32>(io_registers_, dma_operation_register                           & sh2_memory_mask, 0x00000000);

    // Division Unit
    divu_dvcr_.set(DivisionControlRegister::all_bits, 0x00000000u);

    // Serial Communication Interface
    core::rawWrite<u8>(io_registers_, serial_mode_register    & sh2_memory_mask, 0x00);
    core::rawWrite<u8>(io_registers_, bit_rate_register       & sh2_memory_mask, 0xFF);
    core::rawWrite<u8>(io_registers_, serial_control_register & sh2_memory_mask, 0x00);
    core::rawWrite<u8>(io_registers_, transmit_data_register  & sh2_memory_mask, 0xFF);
    core::rawWrite<u8>(io_registers_, serial_status_register  & sh2_memory_mask, 0x84);
    core::rawWrite<u8>(io_registers_, receive_data_register   & sh2_memory_mask, 0x00);

    // Free Running timer
    frt_tier_.set(TimerInterruptEnableRegister::all_bits, static_cast<u8>(0x01));
    frt_ftcsr_.set(FreeRunningTimerControlStatusRegister::all_bits, static_cast<u8>(0x00));
    frt_frc_.set(FreeRunningCounter::all_bits, static_cast<u16>(0x0000));
    frt_ocra_.set(OutputCompareRegister::all_bits, static_cast<u16>(0xFFFF));
    frt_ocrb_.set(OutputCompareRegister::all_bits, static_cast<u16>(0xFFFF));
    frt_tcr_.set(TimerControlRegister::all_bits, static_cast<u8>(0x00));
    frt_tocr_.set(TimerOutputCompareControlRegister::all_bits, static_cast<u8>(0xe0));
    frt_tcr_.set(TimerControlRegister::all_bits, static_cast<u8>(0x0000));

    // Watch Dog Timer
    core::rawWrite<u8>(io_registers_, watchdog_timer_control_status_register     & sh2_memory_mask, 0x18);
    core::rawWrite<u8>(io_registers_, watchdog_timer_counter                     & sh2_memory_mask, 0x00);
    core::rawWrite<u8>(io_registers_, reset_control_status_register              & sh2_memory_mask, 0x1F);
}

void Sh2::powerOnReset() {
    pc_ = memory()->read<u32>(0x00000008);
    r_[15] = memory()->read<u32>(0x0000000c);
    vbr_ = 0;
    sr_.reset();
    gbr_ = 0;
    mach_ = 0;
    macl_ = 0;
    pr_ = 0;
    for (u8 i = 0; i < 15; ++i) r_[i] = 0;

    initializeOnChipRegisters();
}

void Sh2::start32bitsDivision() {
    // 32/32 division
    Log::debug("sh2", "32/32 division");

    // DVDNT is copied in DVDNTL and DVDNTH
    divu_dvdntl_.set(DividendRegisterL::all_bits, divu_dvdnt_.toU32());

    s32 dvdnt = divu_dvdnt_.toU32();
    if (dvdnt < 0) divu_dvdnth_.set();
    else           divu_dvdnth_.reset();

    s32 dvsr = divu_dvsr_.toU32();
    
    Log::debug("sh2", "Dividend : {}, divisor : {}", dvdnt, dvsr);
    
    divu_quot_ = 0;
    divu_rem_  = 0;
    if (divu_dvsr_.any()) {
        divu_quot_ = dvdnt / dvsr;
        divu_rem_  = dvdnt % dvsr;
    } else divu_dvcr_.set(DivisionControlRegister::overflow_flag);
    
    // Overflow check
    if ((dvdnt & 0x80000000) && (dvsr & 0x80000000)) {
        if ((divu_quot_ == 0x7FFFFFFF) && (divu_rem_ & 0x80000000)) divu_dvcr_.set(DivisionControlRegister::overflow_flag);
    }

    // 39 cycles for regular division, 6 cycles when overflow is detected
    divu_remaining_cycles_ = (divu_dvcr_.get(DivisionControlRegister::overflow_flag) == OverflowFlag::overflow) ? 6 : 39;

    divu_is_running_ = true;
}

void Sh2::start64bitsDivision() {
    Log::debug("sh2", "64/32 division");

    s32 dvdntl = divu_dvdntl_.toU32();
    s32 dvdnth = divu_dvdnth_.toU32();
    s32 dvsr   = divu_dvsr_.toU32();
    
    s64 dividend = (static_cast<s64>(dvdnth) << 32) | (dvdntl & 0xffffffff);

    Log::debug("sh2", "Dividend : {}, divisor : {}", dividend, dvsr);

    //auto dvcr = DivisionControlRegister(io_registers_[division_control_register & sh2_memory_mask]);
    s64 quotient{};
    s64 remainder{};
    if (divu_dvsr_.any()) {
        quotient  = dividend / dvsr;
        remainder = dividend % dvsr;
    } else 
        divu_dvcr_.set(DivisionControlRegister::overflow_flag);

    // Overflow check
    if ((dvdnth & 0x80000000) && (dvsr & 0x80000000)) {
        if ((quotient == 0x7FFFFFFF) && (remainder & 0x80000000)) divu_dvcr_.set(DivisionControlRegister::overflow_flag);
    }

    // 39 cycles for regular division, 6 cycles when overflow is detected
    divu_remaining_cycles_ = (divu_dvcr_.get(DivisionControlRegister::overflow_flag) == OverflowFlag::overflow) ? 6 : 39;
    divu_quot_             = static_cast<s32>(quotient);
    divu_rem_              = static_cast<s32>(remainder);

    divu_is_running_ = true;
}

void Sh2::runDivisionUnit(const u8 cycles_to_run) {
    divu_remaining_cycles_ -= cycles_to_run;
    if (divu_remaining_cycles_ == 0) {
        //auto dvcr = DivisionControlRegister(io_registers_[division_control_register & sh2_memory_mask]);
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

    u32 elapsed_cycles{ frt_elapsed_cycles_ + cycles_to_run };
    u32 counter_increment { elapsed_cycles / frt_clock_divisor_ };
    u32 cycles_remainder  { elapsed_cycles % frt_clock_divisor_ };

    if (counter_increment) {
        u32 old_frc{ frt_frc_.get(FreeRunningCounter::all_bits) };
        u32 current_frc{ old_frc + counter_increment };
        frt_frc_.set(FreeRunningCounter::all_bits, static_cast<u16>(current_frc));

        frt_elapsed_cycles_ = elapsed_cycles & frt_mask_;

        // Checking overflow
        if (current_frc > 0xFFFF) {
            frt_ftcsr_.set(FreeRunningTimerControlStatusRegister::timer_overflow_flag);
            if (frt_tier_.get(TimerInterruptEnableRegister::timer_overflow_interrupt_enable) == TimerOverflowInterruptEnable::interrupt_request_enabled) {
                Log::debug("sh2", "FRT - Sending overflow interrupt");
                is::sh2_frt_overflow_flag_set.vector = intc_vcrd_.get(VectorNumberSettingRegisterD::frt_overflow_vector);
                is::sh2_frt_overflow_flag_set.level  = intc_iprb_.get(InterruptPriorityLevelSettingRegisterB::frt_level);
                sendInterrupt(is::sh2_frt_overflow_flag_set);
            }
        }

        // Checking comparison for OCRA
        u32 ocra = frt_ocra_.toU32();
        if ( (old_frc <= ocra) && (current_frc > ocra) ) {
            frt_ftcsr_.set(FreeRunningTimerControlStatusRegister::output_compare_flag_a);
            if(frt_tier_.get(TimerInterruptEnableRegister::output_compare_interrupt_a_enable) == OutputCompareInterruptAEnable::interrupt_request_enabled) {
                Log::debug("sh2", "FRT - OCRA match");
                is::sh2_frt_output_compare_flag_a_set.vector = intc_vcrc_.get(VectorNumberSettingRegisterC::frt_output_compare_vector);
                is::sh2_frt_output_compare_flag_a_set.level  = intc_iprb_.get(InterruptPriorityLevelSettingRegisterB::frt_level);
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
            if (frt_tier_.get(TimerInterruptEnableRegister::output_compare_interrupt_b_enable) == OutputCompareInterruptBEnable::interrupt_request_enabled) {
                Log::debug("sh2", "FRT - OCRB match");
                is::sh2_frt_output_compare_flag_b_set.vector = intc_vcrc_.get(VectorNumberSettingRegisterC::frt_output_compare_vector);
                is::sh2_frt_output_compare_flag_b_set.level = intc_iprb_.get(InterruptPriorityLevelSettingRegisterB::frt_level);
                sendInterrupt(is::sh2_frt_output_compare_flag_b_set);
            }
        }

    } else {
        frt_elapsed_cycles_ += cycles_to_run;
    }
}

void Sh2::executeDma() {

}

void Sh2::reset() {
    initializeOnChipRegisters();

    frt_elapsed_cycles_ = 0;
    frt_clock_divisor_  = 8;
    frt_mask_           = 0b00000111;

}

void Sh2::sendInterrupt(const core::Interrupt& i) {
    if (i.level != 0) {
        scu()->setInterruptStatusRegister(i);
        if (!scu()->isInterruptMasked(i, sh2_type_)) {
            if (pending_interrupts_.size() <= max_interrupt_number) {
                if (!is_level_interrupted_[i.level]) {
                    is_level_interrupted_[i.level] = true;
                    pending_interrupts_.push_front(i);
                    
                    // Sorting (greatest priority first)
                    pending_interrupts_.sort();
                    pending_interrupts_.reverse();

                    Log::debug("sh2", "{} SH2 interrupt pending : {:#0x}", (sh2_type_== Sh2Type::master) ? "Master" : "Slave", i.vector);
                }
            }
            else {
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
}

u8 Sh2::run() {
    u8 cycles_to_run = 1; // Will have to be changed when instruction execution is plugged in
    runDivisionUnit(cycles_to_run);
    runFreeRunningTimer(cycles_to_run);
    return cycles_to_run;
}

}
}