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

#include <saturnin/src/pch.h>
#include <saturnin/src/emulator_defs.h>

namespace saturnin::sh2 {

/// \name Onboard i/o registers addresses
//@{
constexpr auto serial_mode_register                              = u32{0xFFFFFE00};
constexpr auto bit_rate_register                                 = u32{0xFFFFFE01};
constexpr auto serial_control_register                           = u32{0xFFFFFE02};
constexpr auto transmit_data_register                            = u32{0xFFFFFE03};
constexpr auto serial_status_register                            = u32{0xFFFFFE04};
constexpr auto receive_data_register                             = u32{0xFFFFFE05};
constexpr auto timer_interrupt_enable_register                   = u32{0xFFFFFE10};
constexpr auto free_running_timer_control_status_register        = u32{0xFFFFFE11};
constexpr auto free_running_counter                              = u32{0xFFFFFE12};
constexpr auto free_running_counter_h                            = u32{0xFFFFFE12};
constexpr auto free_running_counter_l                            = u32{0xFFFFFE13};
constexpr auto output_compare_register                           = u32{0xFFFFFE14};
constexpr auto timer_control_register                            = u32{0xFFFFFE16};
constexpr auto timer_output_compare_control_register             = u32{0xFFFFFE17};
constexpr auto input_capture_register                            = u32{0xFFFFFE18};
constexpr auto interrupt_priority_level_setting_register_a       = u32{0xFFFFFEE2};
constexpr auto interrupt_priority_level_setting_register_b       = u32{0xFFFFFE60};
constexpr auto vector_number_setting_register_a                  = u32{0xFFFFFE62};
constexpr auto vector_number_setting_register_b                  = u32{0xFFFFFE64};
constexpr auto vector_number_setting_register_c                  = u32{0xFFFFFE66};
constexpr auto vector_number_setting_register_d                  = u32{0xFFFFFE68};
constexpr auto dma_request_response_selection_control_register_0 = u32{0xFFFFFE71};
constexpr auto dma_request_response_selection_control_register_1 = u32{0xFFFFFE72};
constexpr auto watchdog_timer_control_status_register            = u32{0xFFFFFE80};
constexpr auto watchdog_timer_counter                            = u32{0xFFFFFE81};
constexpr auto reset_control_status_register                     = u32{0xFFFFFE82};
constexpr auto standby_control_register                          = u32{0xFFFFFE91};
constexpr auto cache_control_register                            = u32{0xFFFFFE92};
constexpr auto interrupt_control_register                        = u32{0xFFFFFEE0};
constexpr auto vector_number_setting_register_wdt                = u32{0xFFFFFEE4};
constexpr auto divisor_register                                  = u32{0xFFFFFF00};
constexpr auto dividend_register_l_32_bits                       = u32{0xFFFFFF04};
constexpr auto division_control_register                         = u32{0xFFFFFF08};
constexpr auto vector_number_setting_register_div                = u32{0xFFFFFF0C};
constexpr auto dividend_register_h                               = u32{0xFFFFFF10};
constexpr auto dividend_register_l                               = u32{0xFFFFFF14};
constexpr auto dividend_register_h_shadow                        = u32{0xFFFFFF18};
constexpr auto dividend_register_l_shadow                        = u32{0xFFFFFF1C};
constexpr auto break_address_register_ah                         = u32{0xFFFFFF40};
constexpr auto break_address_register_al                         = u32{0xFFFFFF42};
constexpr auto break_address_mask_register_ah                    = u32{0xFFFFFF44};
constexpr auto break_address_mask_register_al                    = u32{0xFFFFFF46};
constexpr auto break_bus_cycle_register_a                        = u32{0xFFFFFF48};
constexpr auto break_address_register_bh                         = u32{0xFFFFFF60};
constexpr auto break_address_register_bl                         = u32{0xFFFFFF62};
constexpr auto break_address_mask_register_bh                    = u32{0xFFFFFF64};
constexpr auto break_address_mask_register_bl                    = u32{0xFFFFFF66};
constexpr auto break_bus_cycle_register_b                        = u32{0xFFFFFF68};
constexpr auto break_data_register_bh                            = u32{0xFFFFFF70};
constexpr auto break_data_register_bl                            = u32{0xFFFFFF72};
constexpr auto break_data_mask_register_bh                       = u32{0xFFFFFF74};
constexpr auto break_data_mask_register_bl                       = u32{0xFFFFFF76};
constexpr auto break_control_register                            = u32{0xFFFFFF78};
constexpr auto dma_source_address_register_0                     = u32{0xFFFFFF80};
constexpr auto dma_destination_address_register_0                = u32{0xFFFFFF84};
constexpr auto dma_tranfer_count_register_0                      = u32{0xFFFFFF88};
constexpr auto dma_channel_control_register_0                    = u32{0xFFFFFF8C};
constexpr auto dma_source_address_register_1                     = u32{0xFFFFFF90};
constexpr auto dma_destination_address_register_1                = u32{0xFFFFFF94};
constexpr auto dma_tranfer_count_register_1                      = u32{0xFFFFFF98};
constexpr auto dma_channel_control_register_1                    = u32{0xFFFFFF9C};
constexpr auto dma_vector_number_register_0                      = u32{0xFFFFFFA0};
constexpr auto dma_vector_number_register_1                      = u32{0xFFFFFFA8};
constexpr auto dma_operation_register                            = u32{0xFFFFFFB0};
constexpr auto bus_control_register1                             = u32{0xFFFFFFE0};
constexpr auto bus_control_register2                             = u32{0xFFFFFFE4};
constexpr auto wait_state_control_register                       = u32{0xFFFFFFE8};
constexpr auto individual_memory_control_register                = u32{0xFFFFFFEC};
constexpr auto refresh_timer_control_status_register             = u32{0xFFFFFFF0};
constexpr auto refresh_timer_counter                             = u32{0xFFFFFFF4};
constexpr auto refresh_time_constant_register                    = u32{0xFFFFFFF8};
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
    inline static const auto m        = BitRange<u8>{9};     ///< Defines M bit.
    inline static const auto q        = BitRange<u8>{8};     ///< Defines Q bit.
    inline static const auto i        = BitRange<u8>{4, 7};  ///< Defines interrupt mask bits (I0-I3).
    inline static const auto s        = BitRange<u8>{1};     ///< Defines S bit.
    inline static const auto t        = BitRange<u8>{0};     ///< Defines T bit.
    inline static const auto all_bits = BitRange<u16>{0, 9}; ///< Defines the range of all the bits of the register.
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
    inline static const auto divu_level   = BitRange<u8>{12, 15}; ///< Defines DIVU priority level.
    inline static const auto dmac_level   = BitRange<u8>{8, 11};  ///< Defines DMAC0/DMAC1 priority level.
    inline static const auto wdt_level    = BitRange<u8>{4, 7};   ///< Defines WDT/DRAM refresh priority level.
    inline static const auto upper_8_bits = BitRange<u8>{8, 15};  ///< Defines the range of the upper 8 bits of the register.
    inline static const auto lower_8_bits = BitRange<u8>{0, 7};   ///< Defines the range of the lower 8 bits of the register.
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
    inline static const auto sci_level    = BitRange<u8>{12, 15}; ///< Defines SCI priority level.
    inline static const auto frt_level    = BitRange<u8>{8, 11};  ///< Defines FRT priority level.
    inline static const auto upper_8_bits = BitRange<u8>{8, 15};  ///< Defines the range of the upper 8 bits of the register.
    inline static const auto lower_8_bits = BitRange<u8>{0, 7};   ///< Defines the range of the lower 8 bits of the register.
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
    inline static const auto sci_receive_error_vector
        = BitRange<u8>{8, 14}; ///< Defines SCI receive error interrupt vector number.
    inline static const auto sci_receive_data_full_vector
        = BitRange<u8>{0, 6};                                    ///< Defines SCI receive data full interrupt vector number.
    inline static const auto upper_8_bits = BitRange<u8>{8, 15}; ///< Defines the range of the upper 8 bits of the register.
    inline static const auto lower_8_bits = BitRange<u8>{0, 7};  ///< Defines the range of the lower 8 bits of the register.
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
    inline static const auto sci_transmit_data_empty_vector
        = BitRange<u8>{8, 14}; ///< Defines SCI transmit data empty interrupt vector number.
    inline static const auto sci_transmit_end_vector = BitRange<u8>{0, 6}; ///< Defines SCI transmit end interrupt vector number.
    inline static const auto upper_8_bits = BitRange<u8>{8, 15}; ///< Defines the range of the upper 8 bits of the register.
    inline static const auto lower_8_bits = BitRange<u8>{0, 7};  ///< Defines the range of the lower 8 bits of the register.
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
    inline static const auto frt_input_capture_vector
        = BitRange<u8>{8, 14}; ///< Defines FRT input capture interrupt vector number.
    inline static const auto frt_output_compare_vector
        = BitRange<u8>{0, 6};                                    ///< Defines FRT output compare interrupt vector number.
    inline static const auto upper_8_bits = BitRange<u8>{8, 15}; ///< Defines the range of the upper 8 bits of the register.
    inline static const auto lower_8_bits = BitRange<u8>{0, 7};  ///< Defines the range of the lower 8 bits of the register.
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
    inline static const auto frt_overflow_vector = BitRange<u8>{8, 14}; ///< Defines FRT overflow interrupt vector number.
    inline static const auto upper_8_bits = BitRange<u8>{8, 15}; ///< Defines the range of the upper 8 bits of the register.
    inline static const auto lower_8_bits = BitRange<u8>{0, 7};  ///< Defines the range of the lower 8 bits of the register.
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
    inline static const auto wdt_interval_vector = BitRange<u8>{8, 14}; ///< Defines WDT interval interrupt vector number.
    inline static const auto wdt_compare_match_vector
        = BitRange<u8>{0, 6};                                    ///< Defines WDT compare match interrupt vector number.
    inline static const auto upper_8_bits = BitRange<u8>{8, 15}; ///< Defines the range of the upper 8 bits of the register.
    inline static const auto lower_8_bits = BitRange<u8>{0, 7};  ///< Defines the range of the lower 8 bits of the register.
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
    inline static const auto divu_interrupt_vector = BitRange<u8>{0, 6}; ///< Defines DIVU vector number.
    inline static const auto upper_16_bits = BitRange<u16>{16, 31}; ///< Defines the range of the upper 16 bits of the register.
    inline static const auto lower_16_bits = BitRange<u16>{0, 15};  ///< Defines the range of the lower 16 bits of the register.
    static constexpr auto    access_mask   = u32{0b1111111};        ///< The access mask
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
    inline static const auto dma_transfert_end_vector = BitRange<u8>{0, 7}; ///< Defines DMACx vector number.
    static constexpr auto    access_mask              = u32{0x00000000FF};  ///< The access mask
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
    inline static const auto nmi_input_level    = BitRange<NmiInputLevel>{15};   ///< Defines NMIL  bit.
    inline static const auto nmi_edge_detection = BitRange<NmiEdgeDetection>{8}; ///< Defines NMIE  bit.
    inline static const auto vector_mode        = BitRange<VectorMode>{0};       ///< Defines VECMD  bit.
    inline static const auto upper_8_bits       = BitRange<u8>{8, 15}; ///< Defines the range of the upper 8 bits of the register.
    inline static const auto lower_8_bits       = BitRange<u8>{0, 7};  ///< Defines the range of the lower 8 bits of the register.
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
    inline static const auto lower_16_bits = BitRange<u16>{0, 15};    ///< Defines the whole register bits
    static constexpr auto    write_mask    = u16{0b0001111111110111}; ///< The write mask
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
    inline static const auto lower_16_bits = BitRange<u16>{0, 15};    ///< Defines the whole register bits
    static constexpr auto    write_mask    = u16{0b0000000011111100}; ///< The write mask
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
    inline static const auto lower_16_bits = BitRange<u16>{0, 15}; ///< Defines the lower 16 bits of the register.
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
    inline static const auto lower_16_bits = BitRange<u16>{0, 15}; ///< Defines the lower 16 bits of the register.
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
    inline static const auto lower_16_bits = BitRange<u16>{0, 15};    ///< Defines the lower 16 bits of the register.
    static constexpr auto    write_mask    = u16{0b0000000011111000}; ///< The write mask
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
    inline static const auto lower_16_bits = BitRange<u16>{0, 15};    ///< Defines the lower 16 bits of the register.
    static constexpr auto    write_mask    = u16{0b0000000011111111}; ///< The write mask
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
    inline static const auto lower_16_bits = BitRange<u16>{0, 15};    ///< Defines the lower 16 bits of the register.
    static constexpr auto    write_mask    = u16{0b0000000011111111}; ///< The write mask
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
    inline static const auto way_specification               = BitRange<WaySpecification>{6, 7};           ///< Defines Wx bits.
    inline static const auto cache_purge                     = BitRange<CachePurge>{4};                    ///< Defines CP bit.
    inline static const auto two_way_mode                    = BitRange<TwoWayMode>{3};                    ///< Defines TW bit.
    inline static const auto data_replacement_disable        = BitRange<DataReplacementDisable>{2};        ///< Defines OD bit.
    inline static const auto instruction_replacement_disable = BitRange<InstructionReplacementDisable>{1}; ///< Defines ID bit.
    inline static const auto cache_enable                    = BitRange<CacheEnable>{0};                   ///< Defines CE bit.
    static constexpr auto    write_mask                      = u8{0b11011111};                             ///< The write mask
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
    static constexpr auto write_mask = u32{0x00FFFFFF}; ///< The write mask
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
    inline static const auto destination_address_mode = BitRange<DestinationAddressMode>{14, 15}; ///< Defines DMx  bits.
    inline static const auto source_address_mode      = BitRange<SourceAddressMode>{12, 13};      ///< Defines SMx  bits.
    inline static const auto transfer_size            = BitRange<TransferSize>{10, 11};           ///< Defines TSx  bits.
    inline static const auto auto_request_mode        = BitRange<AutoRequestMode>{9};             ///< Defines AR  bit.
    inline static const auto acknowledge_mode         = BitRange<AcknowledgeMode>{8};             ///< Defines AM  bit.
    inline static const auto acknowledge_level        = BitRange<AcknowledgeLevel>{7};            ///< Defines AL  bit.
    inline static const auto dreq_select              = BitRange<DreqSelect>{6};                  ///< Defines DS bit.
    inline static const auto dreq_level               = BitRange<DreqLevel>{5};                   ///< Defines DL bit.
    inline static const auto transfer_busMode         = BitRange<TransferBusMode>{4};             ///< Defines TB bit.
    inline static const auto transfer_address_mode    = BitRange<TransferAddressMode>{3};         ///< Defines TA bit.
    inline static const auto interrupt_enable         = BitRange<Sh2DmaInterruptEnable>{2};       ///< Defines IE bit.
    inline static const auto transfer_end_flag        = BitRange<TransferEndFlag>{1};             ///< Defines TE bit.
    inline static const auto dma_enable               = BitRange<Sh2DmaEnable>{0};                ///< Defines DE bit.
    static constexpr auto    write_mask               = u32{0x000000FF};                          ///< The write mask
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
    inline static const auto resource_select = BitRange<ResourceSelect>{0, 1}; ///< Defines RSx bits.
    static constexpr auto    write_mask      = u8{0b00000011};                 ///< The write mask
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
    inline static const auto priority_mode      = BitRange<PriorityMode>{3};     ///< Defines PR bit.
    inline static const auto address_error_flag = BitRange<AddressErrorFlag>{2}; ///< Defines AE bit.
    inline static const auto nmi_flag           = BitRange<NmiFlag>{1};          ///< Defines NMIF bit.
    inline static const auto dma_master_enable  = BitRange<DmaMasterEnable>{0};  ///< Defines DME bit.
    static constexpr auto    write_mask         = u32{0x0000000F};               ///< The write mask
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
    inline static const auto interrupt_enable = BitRange<core::InterruptEnable>{1}; ///< Defines OVFIE bit.
    inline static const auto overflow_flag    = BitRange<OverflowFlag>{0};          ///< Defines OVF bit.
    inline static const auto upper_16_bits = BitRange<u16>{16, 31}; ///< Defines the range of the upper 16 bits of the register.
    inline static const auto lower_16_bits = BitRange<u16>{0, 15};  ///< Defines the range of the lower 16 bits of the register.
    static constexpr auto    access_mask   = u32{0b11};             ///< The access mask
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
    inline static const auto interrupt_capture_interrupt_enable
        = BitRange<InterruptCaptureInterruptEnable>{7}; ///< Defines ICIE bit.
    inline static const auto output_compare_interrupt_a_enable
        = BitRange<OutputCompareInterruptAEnable>{3}; ///< Defines OCIAE bit.
    inline static const auto output_compare_interrupt_b_enable
        = BitRange<OutputCompareInterruptBEnable>{2};                                                     ///< Defines OCIBE bit.
    inline static const auto timer_overflow_interrupt_enable = BitRange<TimerOverflowInterruptEnable>{1}; ///< Defines OVIE bit.
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
    inline static const auto input_capture_flag    = BitRange<InputCaptureFlag>{7};   ///< Defines ICF bit.
    inline static const auto output_compare_flag_a = BitRange<OutputCompareFlagA>{3}; ///< Defines OCFA bit.
    inline static const auto output_compare_flag_b = BitRange<OutputCompareFlagB>{2}; ///< Defines OCFB bit.
    inline static const auto timer_overflow_flag   = BitRange<TimerOverflowFlag>{1};  ///< Defines OVF bit.
    inline static const auto counter_clear_a       = BitRange<CounterClearA>{0};      ///< Defines CCLRA bit.
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
    inline static const auto upper_8_bits = BitRange<u8>{8, 15}; ///< Defines the range of the upper 8 bits of the register.
    inline static const auto lower_8_bits = BitRange<u8>{0, 7};  ///< Defines the range of the lower 8 bits of the register.
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
    inline static const auto upper_8_bits = BitRange<u8>{8, 15}; ///< Defines the range of the upper 8 bits of the register.
    inline static const auto lower_8_bits = BitRange<u8>{0, 7};  ///< Defines the range of the lower 8 bits of the register.
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
    inline static const auto input_edge_select = BitRange<InputEdgeSelect>{7};   ///< Defines IEDG bit.
    inline static const auto clock_select      = BitRange<FrtClockSelect>{0, 1}; ///< Defines IEDG bit.
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
    inline static const auto output_compare_register_select = BitRange<OutputCompareRegisterSelect>{4}; ///< Defines OCRS bit.
    inline static const auto output_level_a                 = BitRange<OutputLevelA>{1};                ///< Defines OLVLA bit.
    inline static const auto output_level_b                 = BitRange<OutputLevelB>{0};                ///< Defines OLVLB bit.
    static constexpr auto    access_mask                    = u8{0b00010011};                           ///< The access mask
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
    inline static const auto upper_8_bits = BitRange<u8>{8, 15}; ///< Defines the range of the upper 8 bits of the register.
    inline static const auto lower_8_bits = BitRange<u8>{0, 7};  ///< Defines the range of the lower 8 bits of the register.
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
    inline static const auto overflow_flag     = BitRange<WdtOverflowFlag>{7};   ///< Defines OVF bit.
    inline static const auto timer_mode_select = BitRange<TimerModeSelect>{6};   ///< Defines WT/IT bit.
    inline static const auto timer_enable      = BitRange<TimerEnable>{5};       ///< Defines TME bit.
    inline static const auto clock_select      = BitRange<WdtClockSelect>{0, 2}; ///< Defines CKSx bits.
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
    inline static const auto watchdog_timer_overflow_flag = BitRange<WatchdogTimerOverflowFlag>{7}; ///< Defines WOVF bit.
    inline static const auto reset_enable                 = BitRange<ResetEnable>{6};               ///< Defines RSTE bit.
    inline static const auto reset_select                 = BitRange<ResetSelect>{5};               ///< Defines RSTS bit.
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
    inline static const auto communication_mode  = BitRange<CommunicationMode>{7};  ///< Defines C/A bit.
    inline static const auto character_length    = BitRange<CharacterLength>{6};    ///< Defines CHR bit.
    inline static const auto parity_enable       = BitRange<ParityEnable>{5};       ///< Defines PE bit.
    inline static const auto parity_mode         = BitRange<ParityMode>{4};         ///< Defines O/E bit.
    inline static const auto stop_bit_length     = BitRange<StopBitLength>{3};      ///< Defines STOP bit.
    inline static const auto multiprocessor_mode = BitRange<MultiprocessorMode>{2}; ///< Defines MP bit.
    inline static const auto clock_select        = BitRange<SciClockSelect>{0, 1};  ///< Defines CKSx bit.
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
    inline static const auto transmit_interrupt_enable       = BitRange<TransmitInterruptEnable>{7};       ///< Defines TIE bit.
    inline static const auto receive_interrupts_enable       = BitRange<ReceiveInterruptEnable>{6};        ///< Defines RIE bit.
    inline static const auto transmit_enable                 = BitRange<TransmitEnable>{5};                ///< Defines TE bit.
    inline static const auto receive_enable                  = BitRange<ReceiveEnable>{4};                 ///< Defines ORE bit.
    inline static const auto multiprocessor_interrupt_enable = BitRange<MultiprocessorInterruptEnable>{3}; ///< Defines MPIE bit.
    inline static const auto transmit_end_interrupt_enable   = BitRange<TransmitEndInterruptEnable>{2};    ///< Defines TEIE bit.
    inline static const auto clock_enable                    = BitRange<ClockEnable>{0, 1};                ///< Defines CKEx bit.
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
    inline static const auto transmit_data_register_empty = BitRange<TransmitDataRegisterEmpty>{7}; ///< Defines TDRE bit.
    inline static const auto receive_data_register_full   = BitRange<ReceiveDataRegisterFull>{6};   ///< Defines RDRF bit.
    inline static const auto overrun_error                = BitRange<OverrunError>{5};              ///< Defines ORER bit.
    inline static const auto framing_error                = BitRange<FramingError>{4};              ///< Defines FER bit.
    inline static const auto parity_error                 = BitRange<ParityError>{3};               ///< Defines PER bit.
    inline static const auto transmit_end                 = BitRange<TransmitEnd>{2};               ///< Defines TEND bit.
    inline static const auto multiprocessor_bit           = BitRange<MultiprocessorBit>{1};         ///< Defines MPB bit.
    inline static const auto multiprocessor_bit_transfer  = BitRange<MultiprocessorBitTransfer>{0}; ///< Defines MPBT bit.
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
};

class StandbyControlRegister : public Register {
  public:
    using Register::Register;
};
} // namespace saturnin::sh2
