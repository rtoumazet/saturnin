//
// sh2_registers.h
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
/// \file	sh2_registers.h
///
/// \brief	Declares SH2 onchip registers and related enums.
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "emulator_defs.h"

namespace saturnin::sh2 {

/// \name Onboard i/o registers addresses
//@{
constexpr u32 serial_mode_register                       = 0xFFFFFE00;
constexpr u32 bit_rate_register                          = 0xFFFFFE01;
constexpr u32 serial_control_register                    = 0xFFFFFE02;
constexpr u32 transmit_data_register                     = 0xFFFFFE03;
constexpr u32 serial_status_register                     = 0xFFFFFE04;
constexpr u32 receive_data_register                      = 0xFFFFFE05;
constexpr u32 timer_interrupt_enable_register            = 0xFFFFFE10;
constexpr u32 free_running_timer_control_status_register = 0xFFFFFE11;
constexpr u32 free_running_counter                       = 0xFFFFFE12;
constexpr u32 free_running_counter_h                     = 0xFFFFFE12;
constexpr u32 free_running_counter_l                     = 0xFFFFFE13;
constexpr u32 output_compare_register                    = 0xFFFFFE14;
constexpr u32 timer_control_register                     = 0xFFFFFE16;
constexpr u32 timer_output_compare_control_register      = 0xFFFFFE17;
// constexpr u32 ficr                                              = 0xFFFFFE18;
constexpr u32 input_capture_register                            = 0xFFFFFE18;
constexpr u32 interrupt_priority_level_setting_register_a       = 0xFFFFFEE2;
constexpr u32 interrupt_priority_level_setting_register_b       = 0xFFFFFE60;
constexpr u32 vector_number_setting_register_a                  = 0xFFFFFE62;
constexpr u32 vector_number_setting_register_b                  = 0xFFFFFE64;
constexpr u32 vector_number_setting_register_c                  = 0xFFFFFE66;
constexpr u32 vector_number_setting_register_d                  = 0xFFFFFE68;
constexpr u32 dma_request_response_selection_control_register_0 = 0xFFFFFE71;
constexpr u32 dma_request_response_selection_control_register_1 = 0xFFFFFE72;
constexpr u32 watchdog_timer_control_status_register            = 0xFFFFFE80;
constexpr u32 watchdog_timer_counter                            = 0xFFFFFE81;
constexpr u32 reset_control_status_register                     = 0xFFFFFE82;
constexpr u32 standby_control_register                          = 0xFFFFFE91;
constexpr u32 cache_control_register                            = 0xFFFFFE92;
constexpr u32 interrupt_control_register                        = 0xFFFFFEE0;
constexpr u32 vector_number_setting_register_wdt                = 0xFFFFFEE4;
constexpr u32 divisor_register                                  = 0xFFFFFF00;
constexpr u32 dividend_register_l_32_bits                       = 0xFFFFFF04;
constexpr u32 division_control_register                         = 0xFFFFFF08;
constexpr u32 vector_number_setting_register_div                = 0xFFFFFF0C;
constexpr u32 dividend_register_h                               = 0xFFFFFF10;
constexpr u32 dividend_register_l                               = 0xFFFFFF14;
constexpr u32 dividend_register_h_shadow                        = 0xFFFFFF18;
constexpr u32 dividend_register_l_shadow                        = 0xFFFFFF1C;
constexpr u32 break_address_register_ah                         = 0xFFFFFF40;
constexpr u32 break_address_register_al                         = 0xFFFFFF42;
constexpr u32 break_address_mask_register_ah                    = 0xFFFFFF44;
constexpr u32 break_address_mask_register_al                    = 0xFFFFFF46;
constexpr u32 break_bus_cycle_register_a                        = 0xFFFFFF48;
constexpr u32 break_address_register_bh                         = 0xFFFFFF60;
constexpr u32 break_address_register_bl                         = 0xFFFFFF62;
constexpr u32 break_address_mask_register_bh                    = 0xFFFFFF64;
constexpr u32 break_address_mask_register_bl                    = 0xFFFFFF66;
constexpr u32 break_bus_cycle_register_b                        = 0xFFFFFF68;
constexpr u32 break_data_register_bh                            = 0xFFFFFF70;
constexpr u32 break_data_register_bl                            = 0xFFFFFF72;
constexpr u32 break_data_mask_register_bh                       = 0xFFFFFF74;
constexpr u32 break_data_mask_register_bl                       = 0xFFFFFF76;
constexpr u32 break_control_register                            = 0xFFFFFF78;
constexpr u32 dma_source_address_register_0                     = 0xFFFFFF80;
constexpr u32 dma_destination_address_register_0                = 0xFFFFFF84;
constexpr u32 dma_tranfer_count_register_0                      = 0xFFFFFF88;
constexpr u32 dma_channel_control_register_0                    = 0xFFFFFF8C;
constexpr u32 dma_source_address_register_1                     = 0xFFFFFF90;
constexpr u32 dma_destination_address_register_1                = 0xFFFFFF94;
constexpr u32 dma_tranfer_count_register_1                      = 0xFFFFFF98;
constexpr u32 dma_channel_control_register_1                    = 0xFFFFFF9C;
constexpr u32 dma_vector_number_register_0                      = 0xFFFFFFA0;
constexpr u32 dma_vector_number_register_1                      = 0xFFFFFFA8;
constexpr u32 dma_operation_register                            = 0xFFFFFFB0;
constexpr u32 bus_control_register1                             = 0xFFFFFFE0;
constexpr u32 bus_control_register2                             = 0xFFFFFFE4;
constexpr u32 wait_state_control_register                       = 0xFFFFFFE8;
constexpr u32 individual_memory_control_register                = 0xFFFFFFEC;
constexpr u32 refresh_timer_control_status_register             = 0xFFFFFFF0;
constexpr u32 refresh_timer_counter                             = 0xFFFFFFF4;
constexpr u32 refresh_time_constant_register                    = 0xFFFFFFF8;
//@}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   BitValue
///
/// \brief  Bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class BitValue : u8 {
    clear = 0, ///< Value when cleared
    set   = 1  ///< Value when set
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  StatusRegister
///
/// \brief  Status Register (SR).
///
/// \author Runik
/// \date   21/09/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class StatusRegister : public Register {
  public:
    using Register::Register;
    inline static const BitRange<u8>  m{9};           ///< Defines M bit.
    inline static const BitRange<u8>  q{8};           ///< Defines Q bit.
    inline static const BitRange<u8>  i{4, 7};        ///< Defines interrupt mask bits (I0-I3).
    inline static const BitRange<u8>  s{1};           ///< Defines S bit.
    inline static const BitRange<u8>  t{0};           ///< Defines T bit.
    inline static const BitRange<u16> all_bits{0, 9}; ///< Defines the range of all the bits of the register.
};

/////////////////////////////////////
// 5. Interrupt Controller (INTC)  //
/////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  InterruptPriorityLevelSettingRegisterA
///
/// \brief  Interrupt Priority Level Setting Register A (IPRA).
///
/// \author Runik
/// \date   28/10/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class InterruptPriorityLevelSettingRegisterA : public Register {
  public:
    using Register::Register;
    inline static const BitRange<u8>  divu_level{12, 15};  ///< Defines DIVU priority level.
    inline static const BitRange<u8>  dmac_level{8, 11};   ///< Defines DMAC0/DMAC1 priority level.
    inline static const BitRange<u8>  wdt_level{4, 7};     ///< Defines WDT/DRAM refresh priority level.
    inline static const BitRange<u16> all_bits{0, 15};     ///< Defines the range of all the bits of the register.
    inline static const BitRange<u8>  upper_8_bits{8, 15}; ///< Defines the range of the upper 8 bits of the register.
    inline static const BitRange<u8>  lower_8_bits{0, 7};  ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  InterruptPriorityLevelSettingRegisterB
///
/// \brief  Interrupt Priority Level Setting Register B (IPRB).
///
/// \author Runik
/// \date   29/10/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class InterruptPriorityLevelSettingRegisterB : public Register {
  public:
    using Register::Register;
    inline static const BitRange<u8>  sci_level{12, 15};   ///< Defines SCI priority level.
    inline static const BitRange<u8>  frt_level{8, 11};    ///< Defines FRT priority level.
    inline static const BitRange<u16> all_bits{0, 15};     ///< Defines the range of all the bits of the register.
    inline static const BitRange<u8>  upper_8_bits{8, 15}; ///< Defines the range of the upper 8 bits of the register.
    inline static const BitRange<u8>  lower_8_bits{0, 7};  ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  VectorNumberSettingRegisterA
///
/// \brief  Vector Number Setting Register A (VCRA).
///
/// \author Runik
/// \date   12/11/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class VectorNumberSettingRegisterA : public Register {
  public:
    using Register::Register;
    inline static const BitRange<u8>  sci_receive_error_vector{8, 14}; ///< Defines SCI receive error interrupt vector number.
    inline static const BitRange<u8>  sci_receive_data_full_vector{0,
                                                                  6}; ///< Defines SCI receive data full interrupt vector number.
    inline static const BitRange<u16> all_bits{0, 15};                 ///< Defines the range of all the bits of the register.
    inline static const BitRange<u8>  upper_8_bits{8, 15};             ///< Defines the range of the upper 8 bits of the register.
    inline static const BitRange<u8>  lower_8_bits{0, 7};              ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  VectorNumberSettingRegisterB
///
/// \brief  Vector Number Setting Register B (VCRB).
///
/// \author Runik
/// \date   12/11/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class VectorNumberSettingRegisterB : public Register {
  public:
    using Register::Register;
    inline static const BitRange<u8> sci_transmit_data_empty_vector{
        8, 14};                                                      ///< Defines SCI transmit data empty interrupt vector number.
    inline static const BitRange<u8>  sci_transmit_end_vector{0, 6}; ///< Defines SCI transmit end interrupt vector number.
    inline static const BitRange<u16> all_bits{0, 15};               ///< Defines the range of all the bits of the register.
    inline static const BitRange<u8>  upper_8_bits{8, 15};           ///< Defines the range of the upper 8 bits of the register.
    inline static const BitRange<u8>  lower_8_bits{0, 7};            ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  VectorNumberSettingRegisterC
///
/// \brief  Vector Number Setting Register C (VCRC).
///
/// \author Runik
/// \date   12/11/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class VectorNumberSettingRegisterC : public Register {
  public:
    using Register::Register;
    inline static const BitRange<u8>  frt_input_capture_vector{8, 14}; ///< Defines FRT input capture interrupt vector number.
    inline static const BitRange<u8>  frt_output_compare_vector{0, 6}; ///< Defines FRT output compare interrupt vector number.
    inline static const BitRange<u16> all_bits{0, 15};                 ///< Defines the range of all the bits of the register.
    inline static const BitRange<u8>  upper_8_bits{8, 15};             ///< Defines the range of the upper 8 bits of the register.
    inline static const BitRange<u8>  lower_8_bits{0, 7};              ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  VectorNumberSettingRegisterD
///
/// \brief  Vector Number Setting Register D (VCRD).
///
/// \author Runik
/// \date   12/11/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class VectorNumberSettingRegisterD : public Register {
  public:
    using Register::Register;
    inline static const BitRange<u8>  frt_overflow_vector{8, 14}; ///< Defines FRT overflow interrupt vector number.
    inline static const BitRange<u16> all_bits{0, 15};            ///< Defines the range of all the bits of the register.
    inline static const BitRange<u8>  upper_8_bits{8, 15};        ///< Defines the range of the upper 8 bits of the register.
    inline static const BitRange<u8>  lower_8_bits{0, 7};         ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  VectorNumberSettingRegisterWDT
///
/// \brief  Vector Number Setting Register WDT (VCRWDT).
///
/// \author Runik
/// \date   12/11/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class VectorNumberSettingRegisterWdt : public Register {
  public:
    using Register::Register;
    inline static const BitRange<u8>  wdt_interval_vector{8, 14};     ///< Defines WDT interval interrupt vector number.
    inline static const BitRange<u8>  wdt_compare_match_vector{0, 6}; ///< Defines WDT compare match interrupt vector number.
    inline static const BitRange<u16> all_bits{0, 15};                ///< Defines the range of all the bits of the register.
    inline static const BitRange<u8>  upper_8_bits{8, 15};            ///< Defines the range of the upper 8 bits of the register.
    inline static const BitRange<u8>  lower_8_bits{0, 7};             ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  VectorNumberSettingRegisterDiv
///
/// \brief  Vector Number Setting Register Div (VCRDIV).
///
/// \author Runik
/// \date   07/11/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class VectorNumberSettingRegisterDiv : public Register {
  public:
    using Register::Register;
    inline static const BitRange<u8>  divu_interrupt_vector{0, 6}; ///< Defines DIVU vector number.
    inline static const BitRange<u16> upper_16_bits{16, 31};       ///< Defines the range of the upper 16 bits of the register.
    inline static const BitRange<u16> lower_16_bits{0, 15};        ///< Defines the range of the lower 16 bits of the register.
    inline static const BitRange<u32> all_bits{0, 31};             ///< Defines the whole register bits
    inline static const u32           accessMask() { return 0b1111111; } ///< Returns access mask;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  VectorNumberSettingRegisterDma
///
/// \brief  Vector Number Setting Register DMA (VCRDMAx).
///
/// \author Runik
/// \date   12/11/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class VectorNumberSettingRegisterDma : public Register {
  public:
    using Register::Register;
    inline static const BitRange<u8>  dma_transfert_end_vector{0, 7};      ///< Defines DMACx vector number.
    inline static const BitRange<u32> all_bits{0, 31};                     ///< Defines the whole register bits
    inline static const u32           writeMask() { return 0x00000000FF; } ///< Returns write mask;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   NmiInputLevel
///
/// \brief  ICR - NMIL bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class NmiInputLevel : u8 {
    low  = 0b0, ///< NMI input level is low
    high = 0b1  ///< NMI input level is high
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   NmiEdgeDetection
///
/// \brief  ICR - NMIE bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class NmiEdgeDetection : u8 {
    falling = 0b0, ///< Interrupt request detected on falling edge of NMI input (initial)
    rising  = 0b1  ///< Interrupt request detected on rising edge of NMI input
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   VectorMode
///
/// \brief  ICR - VECMD bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class VectorMode : u8 {
    auto_vector     = 0b0, ///< Auto vector mode (initial)
    external_vector = 0b1  ///< External vector mode
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  InterruptControlRegister
///
/// \brief  Interrupt Control Register (ICR).
///
/// \author Runik
/// \date   02/02/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class InterruptControlRegister : public Register {
  public:
    using Register::Register;
    inline static const BitRange<NmiInputLevel>    nmi_input_level{15};   ///< Defines NMIL  bit.
    inline static const BitRange<NmiEdgeDetection> nmi_edge_detection{8}; ///< Defines NMIE  bit.
    inline static const BitRange<VectorMode>       vector_mode{0};        ///< Defines VECMD  bit.
    inline static const BitRange<u8>  upper_8_bits{8, 15}; ///< Defines the range of the upper 8 bits of the register.
    inline static const BitRange<u8>  lower_8_bits{0, 7};  ///< Defines the range of the lower 8 bits of the register.
    inline static const BitRange<u16> all_bits{0, 15};     ///< Defines the whole register bits
};

//////////////////////////////////
// 7. Bus State Controler (BSC) //
//////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  BusControlRegister1
///
/// \brief  Bus Control Register 1 (BCR1).
///
/// \author Runik
/// \date   02/02/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class BusControlRegister1 : public Register {
  public:
    using Register::Register;
    inline static const BitRange<u16> lower_16_bits{0, 15};                      ///< Defines the whole register bits
    inline static const BitRange<u32> all_bits{0, 31};                           ///< Defines the whole register bits
    static auto                       writeMask() { return 0b0001111111110111; } ///< returns write mask.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  BusControlRegister1
///
/// \brief  Bus Control Register 2 (BCR2).
///
/// \author Runik
/// \date   02/02/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class BusControlRegister2 : public Register {
  public:
    using Register::Register;
    inline static const BitRange<u16> lower_16_bits{0, 15};                      ///< Defines the whole register bits
    inline static const BitRange<u32> all_bits{0, 31};                           ///< Defines the whole register bits
    static auto                       writeMask() { return 0b0000000011111100; } ///< returns write mask;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  WaitControlRegister
///
/// \brief  Wait Control Register (WCR).
///
/// \author Runik
/// \date   26/11/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class WaitControlRegister : public Register {
  public:
    using Register::Register;
    inline static const BitRange<u16> lower_16_bits{0, 15}; ///< Defines the lower 16 bits of the register.
    inline static const BitRange<u32> all_bits{0, 31};      ///< Defines the whole register bits.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  IndividualMemoryControlRegister
///
/// \brief  Individual Memory Control Register (MCR).
///
/// \author Runik
/// \date   26/11/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class IndividualMemoryControlRegister : public Register {
  public:
    using Register::Register;
    inline static const BitRange<u16> lower_16_bits{0, 15}; ///< Defines the lower 16 bits of the register.
    inline static const BitRange<u32> all_bits{0, 31};      ///< Defines the whole register bits.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  RefreshTimeControlStatusRegister
///
/// \brief  Refresh Time Control/Status Register (RTCSR).
///
/// \author Runik
/// \date   27/11/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class RefreshTimeControlStatusRegister : public Register {
  public:
    using Register::Register;
    inline static const BitRange<u16> lower_16_bits{0, 15};                      ///< Defines the lower 16 bits of the register.
    inline static const BitRange<u32> all_bits{0, 31};                           ///< Defines the whole register bits.
    static auto                       writeMask() { return 0b0000000011111000; } ///< returns write mask;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  RefreshTimerCounter
///
/// \brief  Refresh Timer Counter (RTCNT).
///
/// \author Runik
/// \date   27/11/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class RefreshTimerCounter : public Register {
  public:
    using Register::Register;
    inline static const BitRange<u16> lower_16_bits{0, 15};                      ///< Defines the lower 16 bits of the register.
    inline static const BitRange<u32> all_bits{0, 31};                           ///< Defines the whole register bits.
    static auto                       writeMask() { return 0b0000000011111111; } ///< returns write mask;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  RefreshTimerConstantRegister
///
/// \brief  Refresh Timer Constant Register (RTCOR).
///
/// \author Runik
/// \date   28/11/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class RefreshTimerConstantRegister : public Register {
  public:
    using Register::Register;
    inline static const BitRange<u16> lower_16_bits{0, 15};                      ///< Defines the lower 16 bits of the register.
    inline static const BitRange<u32> all_bits{0, 31};                           ///< Defines the whole register bits.
    static auto                       writeMask() { return 0b0000000011111111; } ///< returns write mask;
};

//////////////
// 8. Cache //
//////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   WaySpecification
///
/// \brief  CCR - Wx bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class WaySpecification : u8 {
    way_0 = 0b00, ///< Way 0 (initial value).
    way_1 = 0b01, ///< Way 1
    way_2 = 0b10, ///< Way 2
    way_3 = 0b11  ///< Way 3
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   CachePurge
///
/// \brief  CCR - CP bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class CachePurge : u8 {
    normal_operation = 0b0, ///< Normal operation.
    cache_purge      = 0b1  ///< Cache purge.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   TwoWayMode
///
/// \brief  CCR - CP bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class TwoWayMode : u8 {
    four_way = 0b0, ///< Four way mode (initial).
    two_way  = 0b1  ///< Two way mode.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   DataReplacementDisable
///
/// \brief  CCR - OD bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class DataReplacementDisable : u8 {
    normal_operation  = 0b0, ///< Normal operation (initial).
    data_not_replaced = 0b1  ///< Data not replaced even when wache miss occurs in data access.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   InstructionReplacementDisable
///
/// \brief  CCR - ID bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class InstructionReplacementDisable : u8 {
    normal_operation  = 0b0, ///< Normal operation (initial).
    data_not_replaced = 0b1  ///< Data not replaced even when wache miss occurs in instruction fetch.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   CacheEnable
///
/// \brief  CCR - CE bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class CacheEnable : u8 {
    cache_disabled = 0b0, ///< Cache disabled (initial).
    cache_enabled  = 0b1  ///< Cache enabled.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  CacheControlRegister
///
/// \brief  Cache Control Register (CCR).
///
/// \author Runik
/// \date   02/02/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class CacheControlRegister : public Register {
  public:
    using Register::Register;
    inline static const BitRange<WaySpecification>              way_specification{6, 7};            ///< Defines Wx bits.
    inline static const BitRange<CachePurge>                    cache_purge{4};                     ///< Defines CP bit.
    inline static const BitRange<TwoWayMode>                    two_way_mode{3};                    ///< Defines TW bit.
    inline static const BitRange<DataReplacementDisable>        data_replacement_disable{2};        ///< Defines OD bit.
    inline static const BitRange<InstructionReplacementDisable> instruction_replacement_disable{1}; ///< Defines ID bit.
    inline static const BitRange<CacheEnable>                   cache_enable{0};                    ///< Defines CE bit.
    inline static const BitRange<u8>                            all_bits{0, 7}; ///< Defines the whole register bits.
    static auto                                                 writeMask() { return 0b11011111; } ///< returns write mask;
};

//////////////////////////////////////////////
// 9. Direct Memory Access Controler (DMAC) //
//////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  DmaSourceAddressRegister
///
/// \brief  Dma Source Address Register (SAR0,SAR1).
///
/// \author Runik
/// \date   28/11/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class DmaSourceAddressRegister : public Register {
  public:
    using Register::Register;
    inline static const BitRange<u32> all_bits{0, 31}; ///< Defines the whole register bits
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  DmaDestinationAddressRegister
///
/// \brief  Dma Destination Address Register (DAR0/DAR1).
///
/// \author Runik
/// \date   28/11/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class DmaDestinationAddressRegister : public Register {
  public:
    using Register::Register;
    inline static const BitRange<u32> all_bits{0, 31}; ///< Defines the whole register bits
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  DmaTransferCountRegister
///
/// \brief  Dma Transfer Count Register (TCR0 / TCR1).
///
/// \author Runik
/// \date   28/11/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class DmaTransferCountRegister : public Register {
  public:
    using Register::Register;
    inline static const BitRange<u32> all_bits{0, 31};                   ///< Defines the whole register bits
    static auto                       writeMask() { return 0x00FFFFFF; } ///< returns write mask;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   DestinationAddressMode
///
/// \brief  CHCR0 - DMx bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class DestinationAddressMode : u8 {
    fixed       = 0b00, ///< Fixed destination address (initial).
    incremented = 0b01, ///< Destination address is incremented.
    decremented = 0b10, ///< Destination address is decremented.
    reserved    = 0b11  ///< Reserved.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   SourceAddressMode
///
/// \brief  CHCR0 - SMx bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class SourceAddressMode : u8 {
    fixed       = 0b00, ///< Fixed source address (initial).
    incremented = 0b01, ///< Source address is incremented.
    decremented = 0b10, ///< Source address is decremented.
    reserved    = 0b11  ///< Reserved.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   TransferSize
///
/// \brief  CHCR0 - TSx bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class TransferSize : u8 {
    one_byte_unit     = 0b00, ///< Byte unit (initial).
    two_byte_unit     = 0b01, ///< Word unit.
    four_byte_unit    = 0b10, ///< Long unit.
    sixteen_byte_unit = 0b11  ///< 16-byte unit (4 longword transfers).
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   AutoRequestMode
///
/// \brief  CHCR0 - AR bit value.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class AutoRequestMode : u8 {
    module_request = 0b0, ///< Module request mode (initial).
    auto_request   = 0b1, ///< Auto request mode.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   AcknowledgeMode
///
/// \brief  CHCR0 - AM bit value.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class AcknowledgeMode : u8 {
    output_read  = 0b0, ///< DACK output in read cycle/transfer from memory to device (initial).
    output_write = 0b1, ///< DACK output in write cycle/transfer from device to memory.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   AcknowledgeLevel
///
/// \brief  CHCR0 - AL bit value.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class AcknowledgeLevel : u8 {
    active_low  = 0b0, ///< DACK signal is active low (initial).
    active_high = 0b1, ///< DACK signal is active high.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   DreqSelect
///
/// \brief  CHCR0 - DS bit value.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class DreqSelect : u8 {
    by_level = 0b0, ///< Detected by level (initial).
    by_edge  = 0b1, ///< Detected by edge.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   DreqLevel
///
/// \brief  CHCR0 - DL bit value.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class DreqLevel : u8 {
    low_level_or_fall  = 0b0, ///< DREQ detected by low level if 0, by fall if 1 (initial).
    high_level_or_rise = 0b1, ///< DREQ detected by high level if 0, by rise if 1.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   TransferBusMode
///
/// \brief  CHCR0 - TB bit value.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class TransferBusMode : u8 {
    cycle_steal = 0b0, ///< Cycle steal mode (initial).
    burst       = 0b1, ///< Burst mode.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   TransferAddressMode
///
/// \brief  CHCR0 - TA bit value.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class TransferAddressMode : u8 {
    dual_address   = 0b0, ///< Dual address mode (initial).
    single_address = 0b1, ///< Single address mode.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   InterruptEnable
///
/// \brief  CHCR0 - IE bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Sh2DmaInterruptEnable : u8 {
    disabled = 0b0, ///< Interrupt disabled.
    enabled  = 0b1  ///< Interrupt enabled.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   TransferEndFlag
///
/// \brief  CHCR0 - TE bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class TransferEndFlag : u8 {
    dma_not_ended_or_aborted = 0b0, ///< DMA has not ended or was aborted (initial).
    dma_ended_normally       = 0b1  ///< DMA has ended normally.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   DmaEnable
///
/// \brief  CHCR0 - DE bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Sh2DmaEnable : u8 {
    dma_transfer_disabled = 0b0, ///< DMA transfer disabled (initial).
    dma_transfer_enabled  = 0b1  ///< DMA transfer enabled.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  DmaChannelControlRegister
///
/// \brief  DMA Channel Control Register (CHCR0/CHCR1).
///
/// \author Runik
/// \date   02/02/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class DmaChannelControlRegister : public Register {
  public:
    using Register::Register;
    inline static const BitRange<DestinationAddressMode> destination_address_mode{14, 15};  ///< Defines DMx  bits.
    inline static const BitRange<SourceAddressMode>      source_address_mode{12, 13};       ///< Defines SMx  bits.
    inline static const BitRange<TransferSize>           transfer_size{10, 11};             ///< Defines TSx  bits.
    inline static const BitRange<AutoRequestMode>        auto_request_mode{9};              ///< Defines AR  bit.
    inline static const BitRange<AcknowledgeMode>        acknowledge_mode{8};               ///< Defines AM  bit.
    inline static const BitRange<AcknowledgeLevel>       acknowledge_level{7};              ///< Defines AL  bit.
    inline static const BitRange<DreqSelect>             dreq_select{6};                    ///< Defines DS bit.
    inline static const BitRange<DreqLevel>              dreq_level{5};                     ///< Defines DL bit.
    inline static const BitRange<TransferBusMode>        transfer_busMode{4};               ///< Defines TB bit.
    inline static const BitRange<TransferAddressMode>    transfer_address_mode{3};          ///< Defines TA bit.
    inline static const BitRange<Sh2DmaInterruptEnable>  interrupt_enable{2};               ///< Defines IE bit.
    inline static const BitRange<TransferEndFlag>        transfer_end_flag{1};              ///< Defines TE bit.
    inline static const BitRange<Sh2DmaEnable>           dma_enable{0};                     ///< Defines DE bit.
    inline static const BitRange<u32>                    all_bits{0, 31};                   ///< Defines the whole register bits
    static auto                                          writeMask() { return 0x000000FF; } ///< returns write mask;
};

enum class ResourceSelect : u8 {
    dreq     = 0b00, ///< DREQ (external request) (initial).
    rxi      = 0b01, ///< RXI (on chip SCI receive data full interrupt transfer request).
    txi      = 0b10, ///< TXI (on chip SCI transmit data empty interrupt transfer request).
    reserved = 0b11  ///< Reserved (setting prohibited)
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  DmaRequestResponseSelectionControlRegister
///
/// \brief  Dma Request/Response Selection Control Register (DRCR0 / DRCR1).
///
/// \author Runik
/// \date   28/11/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class DmaRequestResponseSelectionControlRegister : public Register {
  public:
    using Register::Register;
    inline static const BitRange<ResourceSelect> resource_select{0, 1};             ///< Defines RSx bits.
    inline static const BitRange<u8>             all_bits{0, 7};                    ///< Defines the whole register bits
    static auto                                  writeMask() { return 0b00000011; } ///< returns write mask;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   PriorityMode
///
/// \brief  DMAOR - PR bit.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class PriorityMode : u8 {
    fixed       = 0b0, ///< Fixed priority (initial)
    round_robin = 0b1, ///< Round robin
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   AddressErrorFlag
///
/// \brief  DMAOR - AE bit.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class AddressErrorFlag : u8 {
    no_dmac_address_error = 0b0, ///< No DMAC address error (initial)
    dmac_address_error    = 0b1, ///< Address error by DMAC
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   NmiFlag
///
/// \brief  DMAOR - NMIF bit.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class NmiFlag : u8 {
    no_nmif_interrupt      = 0b0, ///< No NMIF interrupt (initial)
    nmif_interrupt_occured = 0b1, ///< NMIF has occurred
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   DmaMasterEnable
///
/// \brief  DMAOR - DME bit.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class DmaMasterEnable : u8 {
    disabled = 0b0, ///< DMA transfers disabled on all channels (initial)
    enabled  = 0b1, ///< DMA transfers enabled on all channels
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  DmaOperationRegister
///
/// \brief  DMA Operation Register (DMAOR).
///
/// \author Runik
/// \date   03/02/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class DmaOperationRegister : public Register {
  public:
    using Register::Register;
    inline static const BitRange<PriorityMode>     priority_mode{3};                  ///< Defines PR bit.
    inline static const BitRange<AddressErrorFlag> address_error_flag{2};             ///< Defines AE bit.
    inline static const BitRange<NmiFlag>          nmi_flag{1};                       ///< Defines NMIF bit.
    inline static const BitRange<DmaMasterEnable>  dma_master_enable{0};              ///< Defines DME bit.
    inline static const BitRange<u32>              all_bits{0, 31};                   ///< Defines the whole register bits
    static auto                                    writeMask() { return 0x0000000F; } ///< returns write mask;
};

//////////////////////////////
// 10. Division Unit (DIVU) //
//////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  DivisorRegister
///
/// \brief  Divisor Register (DVSR).
///
/// \author Runik
/// \date   11/09/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class DivisorRegister : public Register {
  public:
    using Register::Register;
    inline static const BitRange<u32> all_bits{0, 31}; ///< Defines the whole register bits
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  DividendRegister32Bits
///
/// \brief  Dividend Register L for 32 bits Division (DVDNT).
///
/// \author Runik
/// \date   07/11/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class DividendRegister32Bits : public Register {
  public:
    using Register::Register;
    inline static const BitRange<u32> all_bits{0, 31}; ///< Defines the whole register bits
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   OverflowFlag
///
/// \brief  DVCR - OVF bit value.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class OverflowFlag : u8 {
    no_overflow = 0b0, ///< No overflow has occurred (initial)
    overflow    = 0b1  ///< Overflow has occurred
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  DivisionControlRegister
///
/// \brief  Division Control Register (DVCR).
///
/// \author Runik
/// \date   11/09/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class DivisionControlRegister : public Register {
  public:
    using Register::Register;
    inline static const BitRange<core::InterruptEnable> interrupt_enable{1}; ///< Defines OVFIE bit.
    inline static const BitRange<OverflowFlag>          overflow_flag{0};    ///< Defines OVF bit.
    inline static const BitRange<u16> upper_16_bits{16, 31};        ///< Defines the range of the upper 16 bits of the register.
    inline static const BitRange<u16> lower_16_bits{0, 15};         ///< Defines the range of the lower 16 bits of the register.
    inline static const BitRange<u32> all_bits{0, 31};              ///< Defines the whole register bits
    inline static const u32           accessMask() { return 0b11; } ///< Returns access mask;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  DividendRegisterH
///
/// \brief  Dividend Register H (DVDNTH).
///
/// \author Runik
/// \date   07/11/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class DividendRegisterH : public Register {
  public:
    using Register::Register;
    inline static const BitRange<u32> all_bits{0, 31}; ///< Defines the whole register bits
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  DividendRegisterL
///
/// \brief  Dividend Register L.
///
/// \author Runik
/// \date   07/11/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class DividendRegisterL : public Register {
  public:
    using Register::Register;
    inline static const BitRange<u32> all_bits{0, 31}; ///< Defines the whole register bits
};

//////////////////////////////////////////
// 11. 16 Bits Free Running Timer (FRT) //
//////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   InterruptCaptureInterruptEnable
///
/// \brief  TIER - ICIE values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class InterruptCaptureInterruptEnable : u8 {
    interrupt_request_disabled = 0b0, ///< Interrupt request (ICI) caused by ICF disabled (initial)
    interrupt_request_enabled  = 0b1  ///< Interrupt request (ICI) caused by ICF enabled
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   OutputCompareInterruptAEnable
///
/// \brief  TIER - OCIAE values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class OutputCompareInterruptAEnable : u8 {
    interrupt_request_disabled = 0b0, ///< Interrupt request (ICIA) caused by OCFA disabled (initial)
    interrupt_request_enabled  = 0b1  ///< Interrupt request (ICIA) caused by OCFA enabled
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   OutputCompareInterruptBEnable
///
/// \brief  TIER - OCIBE values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class OutputCompareInterruptBEnable : u8 {
    interrupt_request_disabled = 0b0, ///< Interrupt request (ICIB) caused by OCFB disabled (initial)
    interrupt_request_enabled  = 0b1  ///< Interrupt request (ICIB) caused by OCFB enabled
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   TimerOverflowInterruptEnable
///
/// \brief  TIER - OVIE values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class TimerOverflowInterruptEnable : u8 {
    interrupt_request_disabled = 0b0, ///< Interrupt request (FOVI) caused by OVF disabled (initial)
    interrupt_request_enabled  = 0b1  ///< Interrupt request (FOVI) caused by OVF enabled
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  TimerInterruptEnableRegister
///
/// \brief  Timer Interrupt Enable Register (TIER).
///
/// \author Runik
/// \date   26/10/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class TimerInterruptEnableRegister : public Register {
  public:
    using Register::Register;
    inline static const BitRange<InterruptCaptureInterruptEnable> interrupt_capture_interrupt_enable{7}; ///< Defines ICIE bit.
    inline static const BitRange<OutputCompareInterruptAEnable>   output_compare_interrupt_a_enable{3};  ///< Defines OCIAE bit.
    inline static const BitRange<OutputCompareInterruptBEnable>   output_compare_interrupt_b_enable{2};  ///< Defines OCIBE bit.
    inline static const BitRange<TimerOverflowInterruptEnable>    timer_overflow_interrupt_enable{1};    ///< Defines OVIE bit.
    inline static const BitRange<u8>                              all_bits{0, 7}; ///< Defines the whole register bits
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   InputCaptureFlag
///
/// \brief  FTCSR - ICF values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class InputCaptureFlag : u8 {
    clear = 0b0, ///< Clear conditions: 0 is written when 1 is read
    set   = 0b1  ///< Set conditions: when the FRC value is sent to ICR by the input capture signal
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   OutputCompareFlagA
///
/// \brief  FTCSR - OCFA values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class OutputCompareFlagA : u8 {
    clear = 0b0, ///< Clear conditions: 0 is written when 1 is read
    set   = 0b1  ///< Set conditions: when the FRC value becomes equal to OCRA
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   OutputCompareFlagB
///
/// \brief  FTCSR - OCFB values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class OutputCompareFlagB : u8 {
    clear = 0b0, ///< Clear conditions: 0 is written when 1 is read
    set   = 0b1  ///< Set conditions: when the FRC value becomes equal to OCRB
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   TimerOverflowFlag
///
/// \brief  FTCSR - OVF values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class TimerOverflowFlag : u8 {
    clear = 0b0, ///< Clear conditions: 0 is written when 1 is read
    set   = 0b1  ///< Set conditions: when the FRC value changes from 0xFFFF to 0x0000
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   CounterClearA
///
/// \brief  FTCSR - CCLRA values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class CounterClearA : u8 {
    clear_disabled   = 0b0, ///< FRC clear disabled (initial)
    clear_on_compare = 0b1  ///< FRC cleared on compare match A
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  FreeRunningTimerControlStatusRegister
///
/// \brief  Free Running Timer Control/Status register (FTCSR).
///
/// \author Runik
/// \date   05/11/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class FreeRunningTimerControlStatusRegister : public Register {
  public:
    using Register::Register;
    inline static const BitRange<InputCaptureFlag>   input_capture_flag{7};    ///< Defines ICF bit.
    inline static const BitRange<OutputCompareFlagA> output_compare_flag_a{3}; ///< Defines OCFA bit.
    inline static const BitRange<OutputCompareFlagB> output_compare_flag_b{2}; ///< Defines OCFB bit.
    inline static const BitRange<TimerOverflowFlag>  timer_overflow_flag{1};   ///< Defines OVF bit.
    inline static const BitRange<CounterClearA>      counter_clear_a{0};       ///< Defines CCLRA bit.
    inline static const BitRange<u8>                 all_bits{0, 7};           ///< Defines the whole register bits
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  FreeRunningCounter
///
/// \brief  Free Running Counter (FRC).
///
/// \author Runik
/// \date   05/11/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class FreeRunningCounter : public Register {
  public:
    using Register::Register;
    inline static const BitRange<u8>  upper_8_bits{8, 15}; ///< Defines the range of the upper 8 bits of the register.
    inline static const BitRange<u8>  lower_8_bits{0, 7};  ///< Defines the range of the lower 8 bits of the register.
    inline static const BitRange<u16> all_bits{0, 15};     ///< Defines the whole register bits
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  OutputCompareRegister
///
/// \brief  Output Compare Register (OCRA / OCRB).
///
/// \author Runik
/// \date   30/10/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class OutputCompareRegister : public Register {
  public:
    using Register::Register;
    inline static const BitRange<u8>  upper_8_bits{8, 15}; ///< Defines the range of the upper 8 bits of the register.
    inline static const BitRange<u8>  lower_8_bits{0, 7};  ///< Defines the range of the lower 8 bits of the register.
    inline static const BitRange<u16> all_bits{0, 15};     ///< Defines the whole register bits
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   InputEdgeSelect
///
/// \brief  TCR - IEDG bit value.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class InputEdgeSelect : u8 {
    falling = 0b0, ///< Input captured on falling edge (initial)
    rising  = 0b1  ///< Input captured on rising edge
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   FrtClockSelect
///
/// \brief  TCR - CKSx bits values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class FrtClockSelect : u8 {
    internal_divided_by_8   = 0b00, ///< Internal clock /8 (initial)
    internal_divided_by_32  = 0b01, ///< Internal clock /32
    internal_divided_by_128 = 0b10, ///< Internal clock /128
    external                = 0b11  ///< External clock
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  TimerControlRegister
///
/// \brief  Timer Control Register (TCR).
///
/// \author Runik
/// \date   26/10/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class TimerControlRegister : public Register {
  public:
    using Register::Register;
    inline static const BitRange<InputEdgeSelect> input_edge_select{7}; ///< Defines IEDG bit.
    inline static const BitRange<FrtClockSelect>  clock_select{0, 1};   ///< Defines IEDG bit.
    inline static const BitRange<u8>              all_bits{0, 7};       ///< Defines the whole register bits
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   OutputCompareRegisterSelect
///
/// \brief  TOCR - OCRS bit value.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class OutputCompareRegisterSelect : u8 {
    ocra = 0b0, ///< Selects register OCRA
    ocrb = 0b1  ///< Selects register OCRB
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   OutputLevelA
///
/// \brief  TOCR - OLVLA bit value.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class OutputLevelA : u8 {
    outputs_0 = 0b0, ///< Outputs 0 on compare match A
    outputs_1 = 0b1  ///< Outputs 1 on compare match A
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   OutputLevelB
///
/// \brief  TOCR - OLVLB bit value.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class OutputLevelB : u8 {
    outputs_0 = 0b0, ///< Outputs 0 on compare match B
    outputs_1 = 0b1  ///< Outputs 1 on compare match B
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  TimerOutputCompareControlRegister
///
/// \brief  Timer Output Compare Control Register (TOCR).
///
/// \author Runik
/// \date   03/02/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class TimerOutputCompareControlRegister : public Register {
  public:
    using Register::Register;
    inline static const BitRange<OutputCompareRegisterSelect> output_compare_register_select{4}; ///< Defines OCRS bit.
    inline static const BitRange<OutputLevelA>                output_level_a{1};                 ///< Defines OLVLA bit.
    inline static const BitRange<OutputLevelB>                output_level_b{0};                 ///< Defines OLVLB bit.
    inline static const BitRange<u8>                          all_bits{0, 7}; ///< Defines the whole register bits
    inline static const u8                                    accessMask() { return 0b00010011; } ///< Returns access mask;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  InputCaptureRegister
///
/// \brief  Input Capture Register (ICR).
///
/// \author Runik
/// \date   07/12/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class InputCaptureRegister : public Register {
  public:
    using Register::Register;
    inline static const BitRange<u8>  upper_8_bits{8, 15}; ///< Defines the range of the upper 8 bits of the register.
    inline static const BitRange<u8>  lower_8_bits{0, 7};  ///< Defines the range of the lower 8 bits of the register.
    inline static const BitRange<u16> all_bits{0, 15};     ///< Defines the whole register bits
};

///////////////////////////////
// 12. Watch Dog Timer (WDT) //
///////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  WatchdogTimerCounter
///
/// \brief  Watchdog Timer Counter (WTCNT).
///
/// \author Runik
/// \date   07/12/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class WatchdogTimerCounter : public Register {
  public:
    using Register::Register;
    inline static const BitRange<u8> all_bits{0, 7}; ///< Defines the whole register bits
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   WdtOverflowFlag
///
/// \brief  WTCSR - OVF value.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class WdtOverflowFlag : u8 {
    no_overflow = 0, ///< No overflow of WTCNT in interval timer mode (initial)
    overflow    = 1  ///< WTCNT overflow in interval timer mode
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   TimerModeSelect
///
/// \brief WTCSR - (WT /IT) value.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class TimerModeSelect : u8 {
    interval_timer_mode = 0, ///< Interval timer mode option
    watchdog_timer_mode = 1  ///< Watchdog timer mode option
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   TimerEnable
///
/// \brief  WTCSR - TME value.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class TimerEnable : u8 {
    timer_disabled = 0, ///< Timer disabled : WTCNT is initialized to 0x00 and count up stops. (initial)
    timer_enabled  = 1, ///< Timer enabled : WTCNT starts counting.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   WdtClockSelect
///
/// \brief  WTCSR - CKSx values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class WdtClockSelect : u8 {
    divided_by_2    = 0b000, ///< System clock divided by 2 (initial)
    divided_by_64   = 0b001, ///< System clock divided by 64
    divided_by_128  = 0b010, ///< System clock divided by 128
    divided_by_256  = 0b011, ///< System clock divided by 256
    divided_by_512  = 0b100, ///< System clock divided by 512
    divided_by_1024 = 0b101, ///< System clock divided by 1024
    divided_by_4096 = 0b110, ///< System clock divided by 4096
    divided_by_8192 = 0b111, ///< System clock divided by 8192

};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  WatchdogTimerControlStatusRegister
///
/// \brief  Watchdog Timer Control/Status Register (WTCSR).
///
/// \author Runik
/// \date   07/12/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class WatchdogTimerControlStatusRegister : public Register {
  public:
    using Register::Register;
    inline static const BitRange<WdtOverflowFlag> overflow_flag{7};     ///< Defines OVF bit.
    inline static const BitRange<TimerModeSelect> timer_mode_select{6}; ///< Defines WT/IT bit.
    inline static const BitRange<TimerEnable>     timer_enable{5};      ///< Defines TME bit.
    inline static const BitRange<WdtClockSelect>  clock_select{0, 2};   ///< Defines CKSx bits.
    inline static const BitRange<u8>              all_bits{0, 7};       ///< Defines the whole register bits
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   WatchdogTimerOverflowFlag
///
/// \brief  RSTCSR - WOVF bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class WatchdogTimerOverflowFlag : u8 {
    no_overflow = 0, ///< No WTCNT overflow in watchdof timer mode (initial).
    overflow    = 1  ///< Set by WTCNT overflow in watchdog mode.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   ResetEnable
///
/// \brief  RSTCSR - RSTE bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class ResetEnable : u8 {
    not_reset_when_overflow = 0, ///< Not reset when WTCNT overflows (initial).
    reset_when_overflow     = 1, ///< Reset when WTCNT overflows.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   ResetSelect
///
/// \brief  RSTCSR - RSTS bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class ResetSelect : u8 {
    power_on_reset = 0, ///< Power-on reset (initial)
    manual_reset   = 1  ///< Manual reset
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  ResetControlStatusRegister
///
/// \brief  Reset Control Status Register (RSTCSR).
///
/// \author Runik
/// \date   07/12/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class ResetControlStatusRegister : public Register {
  public:
    using Register::Register;
    inline static const BitRange<WatchdogTimerOverflowFlag> watchdog_timer_overflow_flag{7}; ///< Defines WOVF bit.
    inline static const BitRange<ResetEnable>               reset_enable{6};                 ///< Defines RSTE bit.
    inline static const BitRange<ResetSelect>               reset_select{5};                 ///< Defines RSTS bit.
    inline static const BitRange<u8>                        all_bits{0, 7};                  ///< Defines the whole register bits
};

/////////////////////////////////////////////
// 13.Serial Communication Interface (SCI) //
/////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   CommunicationMode
///
/// \brief  SMR - C/A bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class CommunicationMode : u8 {
    asynchronous        = 0, ///< Asynchronous mode (initial).
    clocked_synchronous = 1, ///< Clocked synchronous mode.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   CharacterLength
///
/// \brief  SMR - CHR bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class CharacterLength : u8 {
    eight_bit_data = 0, ///< 8-bit data (initial).
    seven_bit_data = 1, ///< 7-bit data.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   ParityMode
///
/// \brief  SMR - O/E bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class ParityEnable : u8 {
    parity_bit_not_added = 0, ///< Parity bit not added or checked (initial).
    parity_bit_added     = 1, ///< Parity bit added and checked.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   ParityMode
///
/// \brief  SMR - O/E bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class ParityMode : u8 {
    even_parity = 0, ///< Even parity (initial).
    odd_parity  = 1, ///< Odd parity.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   StopBitLength
///
/// \brief  SMR - STOP bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class StopBitLength : u8 {
    one_stop_bit  = 0, ///< One stop bit (initial).
    two_stop_bits = 1, ///< Two stop bits.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   MultiprocessorMode
///
/// \brief  SMR - MP bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class MultiprocessorMode : u8 {
    multiprocessor_function_disabled = 0, ///< Multiprocessor mode enabled (initial).
    multiprocessor_function_enabled  = 1, ///< Multiprocessor mode disabled.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   SciClockSelect
///
/// \brief  SMR - CKSx bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class SciClockSelect : u8 {
    divided_by_4   = 0b00, ///< Clock divided by 4 (initial).
    divided_by_16  = 0b01, ///< Clock divided by 16.
    divided_by_64  = 0b10, ///< Clock divided by 64.
    divided_by_256 = 0b11, ///< Clock divided by 256.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  SerialModeRegister
///
/// \brief  Serial Mode Register (SMR).
///
/// \author Runik
/// \date   07/12/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class SerialModeRegister : public Register {
  public:
    using Register::Register;
    inline static const BitRange<CommunicationMode>  communication_mode{7};  ///< Defines C/A bit.
    inline static const BitRange<CharacterLength>    character_length{6};    ///< Defines CHR bit.
    inline static const BitRange<ParityEnable>       parity_enable{5};       ///< Defines PE bit.
    inline static const BitRange<ParityMode>         parity_mode{4};         ///< Defines O/E bit.
    inline static const BitRange<StopBitLength>      stop_bit_length{3};     ///< Defines STOP bit.
    inline static const BitRange<MultiprocessorMode> multiprocessor_mode{2}; ///< Defines MP bit.
    inline static const BitRange<SciClockSelect>     clock_select{0, 1};     ///< Defines CKSx bit.
    inline static const BitRange<u8>                 all_bits{0, 7};         ///< Defines the whole register bits
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  BitRateRegister
///
/// \brief  Bit Rate Register (BRR).
///
/// \author Runik
/// \date   07/12/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class BitRateRegister : public Register {
  public:
    using Register::Register;
    inline static const BitRange<u8> all_bits{0, 7}; ///< Defines the whole register bits
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   TransmitInterruptEnable
///
/// \brief  SCR - TIE bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class TransmitInterruptEnable : u8 {
    interrupt_disabled = 0, ///< TXI request is disabled (initial).
    interrupt_enabled  = 1  ///< TXI request is enabled.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   ReceiveInterruptEnable
///
/// \brief  SCR - REI bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class ReceiveInterruptEnable : u8 {
    interrupts_disabled = 0, ///< RXI and ERI requests are disabled (initial).
    interrupts_enabled  = 1  ///< RXI and ERI requests are enabled.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   TransmitEnable
///
/// \brief  SCR - TE bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class TransmitEnable : u8 {
    transmitter_disabled = 0, ///< Transmitter disabled (initial).
    transmitter_enabled  = 1  ///< Transmitter enabled.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   ReceiveEnable
///
/// \brief  SCR - RE bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class ReceiveEnable : u8 {
    receiver_disabled = 0, ///< Receiver disabled (initial).
    receiver_enabled  = 1  ///< Receiver enabled.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   MultiprocessorInterruptEnable
///
/// \brief  SCR - MPIE bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class MultiprocessorInterruptEnable : u8 {
    interrupt_disabled = 0, ///< Multiprocessor interrupts are disabled (initial).
    interrupt_enabled  = 1  ///< Multiprocessor interrupts are enabled.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   TransmitEndInterruptEnable
///
/// \brief  SCR - TEIE bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class TransmitEndInterruptEnable : u8 {
    interrupt_disabled = 0, ///< Transmit end interrupt (TEI) requests are disabled (initial).
    interrupt_enabled  = 1  ///< Transmit end interrupt (TEI) requests are enabled.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   CommunicationMode
///
/// \brief  SCR - CKEx bits values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class ClockEnable : u8 {
    value_00 = 0b00, ///< Value 0b00 (initial).
    value_01 = 0b01, ///< Value 0b01.
    value_10 = 0b10, ///< Value 0b10.
    value_11 = 0b11  ///< Value 0b11.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  SerialControlRegister
///
/// \brief  Serial Control Register (SCR).
///
/// \author Runik
/// \date   10/12/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class SerialControlRegister : public Register {
  public:
    using Register::Register;
    inline static const BitRange<TransmitInterruptEnable>       transmit_interrupt_enable{7};       ///< Defines TIE bit.
    inline static const BitRange<ReceiveInterruptEnable>        receive_interrupts_enable{6};       ///< Defines RIE bit.
    inline static const BitRange<TransmitEnable>                transmit_enable{5};                 ///< Defines TE bit.
    inline static const BitRange<ReceiveEnable>                 receive_enable{4};                  ///< Defines ORE bit.
    inline static const BitRange<MultiprocessorInterruptEnable> multiprocessor_interrupt_enable{3}; ///< Defines MPIE bit.
    inline static const BitRange<TransmitEndInterruptEnable>    transmit_end_interrupt_enable{2};   ///< Defines TEIE bit.
    inline static const BitRange<ClockEnable>                   clock_enable{0, 1};                 ///< Defines CKEx bit.
    inline static const BitRange<u8>                            all_bits{0, 7}; ///< Defines the whole register bits
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  TransmitDataRegister
///
/// \brief  Transmit Data Register (TDR).
///
/// \author Runik
/// \date   07/12/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class TransmitDataRegister : public Register {
  public:
    using Register::Register;
    inline static const BitRange<u8> all_bits{0, 7}; ///< Defines the whole register bits
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  TransmitShiftRegister
///
/// \brief  Transmit Shift Register (TSR).
///
/// \author Runik
/// \date   07/12/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class TransmitShiftRegister : public Register {
  public:
    using Register::Register;
    inline static const BitRange<u8> all_bits{0, 7}; ///< Defines the whole register bits
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   TransmitDataRegisterEmpty
///
/// \brief  SSR - TDRE bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class TransmitDataRegisterEmpty : u8 {
    contains_valid_data         = 0, ///< TDR contains valid transmit data (initial).
    does_not_contain_valid_data = 1  ///< TDR does not contain valid transmit data.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   ReceiveDataRegisterFull
///
/// \brief  SSR - RDRF bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class ReceiveDataRegisterFull : u8 {
    no_valid_data_received = 0, ///< RDR does not contain valid received data (initial).
    valid_data_received    = 1  ///< RDR contains valid received data.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   OverrunError
///
/// \brief  SSR - ORER bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class OverrunError : u8 {
    in_progress_or_ended_normally = 0, ///< Receiving is in progress or has ended normally (initial).
    error_occurred                = 1  ///< A receive overrun error occurred.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   FramingError
///
/// \brief  SSR - FER bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class FramingError : u8 {
    in_progress_or_ended_normally = 0, ///< Receiving is in progress or has ended normally (initial).
    error_occurred                = 1  ///< A receive framing error occurred.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   ParityError
///
/// \brief  SSR - PER bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class ParityError : u8 {
    in_progress_or_ended_normally = 0, ///< Receiving is in progress or has ended normally (initial).
    error_occurred                = 1  ///< A receive parity error occurred.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   TransmitEnd
///
/// \brief  SSR - TEND bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class TransmitEnd : u8 {
    transmission_in_progress = 0, ///< Transmission is in progress.
    end_of_transmission      = 1  ///< End of transmission (initial).
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   MultiprocessorBit
///
/// \brief  SSR - MPB bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class MultiprocessorBit : u8 {
    bit_value_0 = 0, ///< Multiprocessor bit value in receive data is 0 (initial).
    bit_value_1 = 1  ///< Multiprocessor bit value in receive data is 1.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   MultiprocessorBitTransfer
///
/// \brief  SSR - MPBT bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class MultiprocessorBitTransfer : u8 {
    bit_value_0 = 0, ///< Multiprocessor bit value in transmit data is 0 (initial).
    bit_value_1 = 1  ///< Multiprocessor bit value in transmit data is 1.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  SerialStatusRegister
///
/// \brief  Serial Status Register (SSR).
///
/// \author Runik
/// \date   07/12/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class SerialStatusRegister : public Register {
  public:
    using Register::Register;
    inline static const BitRange<TransmitDataRegisterEmpty> transmit_data_register_empty{7}; ///< Defines TDRE bit.
    inline static const BitRange<ReceiveDataRegisterFull>   receive_data_register_full{6};   ///< Defines RDRF bit.
    inline static const BitRange<OverrunError>              overrun_error{5};                ///< Defines ORER bit.
    inline static const BitRange<FramingError>              framing_error{4};                ///< Defines FER bit.
    inline static const BitRange<ParityError>               parity_error{3};                 ///< Defines PER bit.
    inline static const BitRange<TransmitEnd>               transmit_end{2};                 ///< Defines TEND bit.
    inline static const BitRange<MultiprocessorBit>         multiprocessor_bit{1};           ///< Defines MPB bit.
    inline static const BitRange<MultiprocessorBitTransfer> multiprocessor_bit_transfer{0};  ///< Defines MPBT bit.
    inline static const BitRange<u8>                        all_bits{0, 7};                  ///< Defines the whole register bits
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  ReceiveDataRegister
///
/// \brief  Receive Data Register (RDR).
///
/// \author Runik
/// \date   07/12/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class ReceiveDataRegister : public Register {
  public:
    using Register::Register;
    inline static const BitRange<u8> all_bits{0, 7}; ///< Defines the whole register bits
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  ReceiveShiftRegister
///
/// \brief  Receive Shift Register (RSR).
///
/// \author Runik
/// \date   07/12/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class ReceiveShiftRegister : public Register {
  public:
    using Register::Register;
    inline static const BitRange<u8> all_bits{0, 7}; ///< Defines the whole register bits
};
} // namespace saturnin::sh2
