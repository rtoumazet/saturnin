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

#include <saturnin/src/emulator_defs.h>
#include <saturnin/src/bitfield.h>

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
/// \union  StatusRegister
///
/// \brief  Status Register (SR).
///
/// \author Runik
/// \date   16/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union StatusRegister {
    u32            raw; ///< Raw representation.
    BitField<9>    m;   ///< Defines M bit.
    BitField<8>    q;   ///< Defines Q bit.
    BitField<4, 4> i;   ///< Defines interrupt mask bits (I0-I3).
    BitField<1>    s;   ///< Defines S bit.
    BitField<0>    t;   ///< Defines T bit.
};

/////////////////////////////////////
// 5. Interrupt Controller (INTC)  //
/////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union  InterruptPriorityLevelSettingRegisterA
///
/// \brief  Interrupt Priority Level Setting Register A (IPRA).
///
/// \author Runik
/// \date   17/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union InterruptPriorityLevelSettingRegisterA {
    u16             raw;          ///< Raw representation.
    BitField<12, 4> divu_level;   ///< Defines DIVU priority level.
    BitField<8, 4>  dmac_level;   ///< Defines DMAC0/DMAC1 priority level.
    BitField<4, 4>  wdt_level;    ///< Defines WDT/DRAM refresh priority level.
    BitField<8, 8>  upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8>  lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union  InterruptPriorityLevelSettingRegisterB
///
/// \brief  Interrupt Priority Level Setting Register B (IPRB).
///
/// \author Runik
/// \date   17/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union InterruptPriorityLevelSettingRegisterB {
    u16             raw;          ///< Raw representation.
    BitField<12, 4> sci_level;    ///< Defines SCI priority level.
    BitField<8, 4>  frt_level;    ///< Defines FRT priority level.
    BitField<8, 8>  upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8>  lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	VectorNumberSettingRegisterA
///
/// \brief	Vector Number Setting Register A (VCRA).
///
/// \author	Runik
/// \date	17/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union VectorNumberSettingRegisterA {
    u16            raw;                          ///< Raw representation.
    BitField<8, 7> sci_receive_error_vector;     ///< Defines SCI receive error interrupt vector number.
    BitField<0, 7> sci_receive_data_full_vector; ///< Defines SCI receive data full interrupt vector number.
    BitField<8, 8> upper_8_bits;                 ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits;                 ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	VectorNumberSettingRegisterB
///
/// \brief	Vector Number Setting Register B (VCRB).
///
/// \author	Runik
/// \date	17/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union VectorNumberSettingRegisterB {
    u16            raw;                            ///< Raw representation.
    BitField<8, 7> sci_transmit_data_empty_vector; ///< Defines SCI transmit data empty interrupt vector number.
    BitField<0, 7> sci_transmit_end_vector;        ///< Defines SCI transmit end interrupt vector number.
    BitField<8, 8> upper_8_bits;                   ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits;                   ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	VectorNumberSettingRegisterC
///
/// \brief	Vector Number Setting Register C (VCRC).
///
/// \author	Runik
/// \date	17/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union VectorNumberSettingRegisterC {
    u16            raw;                       ///< Raw representation.
    BitField<8, 7> frt_input_capture_vector;  ///< Defines FRT input capture interrupt vector number.
    BitField<0, 7> frt_output_compare_vector; ///< Defines FRT output compare interrupt vector number.
    BitField<8, 8> upper_8_bits;              ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits;              ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	VectorNumberSettingRegisterD
///
/// \brief	Vector Number Setting Register D (VCRD).
///
/// \author	Runik
/// \date	17/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union VectorNumberSettingRegisterD {
    u16            raw;                 ///< Raw representation.
    BitField<8, 7> frt_overflow_vector; ///< Defines FRT overflow interrupt vector number.
    BitField<8, 8> upper_8_bits;        ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits;        ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	VectorNumberSettingRegisterWdt
///
/// \brief	Vector Number Setting Register WDT (VCRWDT).
///
/// \author	Runik
/// \date	18/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union VectorNumberSettingRegisterWdt {
    u16            raw;                      ///< Raw representation.
    BitField<8, 7> wdt_interval_vector;      ///< Defines WDT interval interrupt vector number.
    BitField<0, 7> wdt_compare_match_vector; ///< Defines WDT compare match interrupt vector number.
    BitField<8, 8> upper_8_bits;             ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits;             ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	VectorNumberSettingRegisterDiv
///
/// \brief	Vector Number Setting Register Div (VCRDIV).
///
/// \author	Runik
/// \date	18/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union VectorNumberSettingRegisterDiv {
    u32              raw;                   ///< Raw representation.
    BitField<0, 7>   divu_interrupt_vector; ///< Defines DIVU vector number.
    BitField<16, 16> upper_16_bits;         ///< Defines the range of the upper 16 bits of the register.
    BitField<0, 16>  lower_16_bits;         ///< Defines the range of the lower 16 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	VectorNumberSettingRegisterDma
///
/// \brief	Vector Number Setting Register DMA (VCRDMAx).
///
/// \author	Runik
/// \date	18/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union VectorNumberSettingRegisterDma {
    u32            raw;                      ///< Raw representation.
    BitField<0, 8> dma_transfert_end_vector; ///< Defines DMACx vector number.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   NmiInputLevel
///
/// \brief  ICR - NMIL bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class NmiInputLevel : bool {
    low  = 0, ///< NMI input level is low
    high = 1  ///< NMI input level is high
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   NmiEdgeDetection
///
/// \brief  ICR - NMIE bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class NmiEdgeDetection : bool {
    falling = 0, ///< Interrupt request detected on falling edge of NMI input (initial)
    rising  = 1  ///< Interrupt request detected on rising edge of NMI input
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   VectorMode
///
/// \brief  ICR - VECMD bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class VectorMode : bool {
    auto_vector     = 0, ///< Auto vector mode (initial)
    external_vector = 1  ///< External vector mode
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	InterruptControlRegister
///
/// \brief	Interrupt Control Register (ICR).
///
/// \author	Runik
/// \date	18/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union InterruptControlRegister {
    u16            raw;                ///< Raw representation.
    BitField<15>   nmi_input_level;    ///< Defines NMIL bit
    BitField<8>    nmi_edge_detection; ///< Defines NMIE bit.
    BitField<0>    vector_mode;        ///< Defines VECMD  bit.
    BitField<8, 8> upper_8_bits;       ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits;       ///< Defines the range of the lower 8 bits of the register.
};

//////////////////////////////////
// 7. Bus State Controler (BSC) //
//////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	BusControlRegister1
///
/// \brief	Bus Control Register 1 (BCR1).
///
/// \author	Runik
/// \date	18/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union BusControlRegister1 {
    u32             raw;           ///< Raw representation.
    BitField<0, 16> lower_16_bits; ///< Defines the range of the lower 16 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	BusControlRegister2
///
/// \brief	Bus Control Register 2 (BCR2).
///
/// \author	Runik
/// \date	19/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union BusControlRegister2 {
    u32             raw;           ///< Raw representation.
    BitField<0, 16> lower_16_bits; ///< Defines the range of the lower 16 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	WaitControlRegister
///
/// \brief	Wait Control Register (WCR).
///
/// \author	Runik
/// \date	19/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union WaitControlRegister {
    u32             raw;           ///< Raw representation.
    BitField<0, 16> lower_16_bits; ///< Defines the range of the lower 16 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	IndividualMemoryControlRegister
///
/// \brief	Individual Memory Control Register (MCR).
///
/// \author	Runik
/// \date	19/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union IndividualMemoryControlRegister {
    u32             raw;           ///< Raw representation.
    BitField<0, 16> lower_16_bits; ///< Defines the range of the lower 16 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	RefreshTimeControlStatusRegister
///
/// \brief	Refresh Time Control/Status Register (RTCSR).
///
/// \author	Runik
/// \date	19/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union RefreshTimeControlStatusRegister {
    u32             raw;           ///< Raw representation.
    BitField<0, 16> lower_16_bits; ///< Defines the range of the lower 16 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	RefreshTimerCounter
///
/// \brief	Refresh Timer Counter (RTCNT).
///
/// \author	Runik
/// \date	19/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union RefreshTimerCounter {
    u32             raw;           ///< Raw representation.
    BitField<0, 16> lower_16_bits; ///< Defines the range of the lower 16 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	RefreshTimerConstantRegister
///
/// \brief	Refresh Timer Constant Register (RTCOR).
///
/// \author	Runik
/// \date	19/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union RefreshTimerConstantRegister {
    u32             raw;           ///< Raw representation.
    BitField<0, 16> lower_16_bits; ///< Defines the range of the lower 16 bits of the register.
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

enum class CachePurge : bool {
    normal_operation = 0, ///< Normal operation.
    cache_purge      = 1  ///< Cache purge.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   TwoWayMode
///
/// \brief  CCR - CP bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class TwoWayMode : bool {
    four_way = 0, ///< Four way mode (initial).
    two_way  = 1  ///< Two way mode.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   DataReplacementDisable
///
/// \brief  CCR - OD bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class DataReplacementDisable : bool {
    normal_operation  = 0, ///< Normal operation (initial).
    data_not_replaced = 1  ///< Data not replaced even when wache miss occurs in data access.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   InstructionReplacementDisable
///
/// \brief  CCR - ID bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class InstructionReplacementDisable : bool {
    normal_operation  = 0, ///< Normal operation (initial).
    data_not_replaced = 1  ///< Data not replaced even when wache miss occurs in instruction fetch.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   CacheEnable
///
/// \brief  CCR - CE bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class CacheEnable : bool {
    cache_disabled = 0, ///< Cache disabled (initial).
    cache_enabled  = 1  ///< Cache enabled.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	CacheControlRegister
///
/// \brief	Cache Control Register (CCR).
///
/// \author	Runik
/// \date	19/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union CacheControlRegister {
    u8             raw;                             ///< Raw representation.
    BitField<6, 2> way_specification;               ///< Defines Wx bits.
    BitField<4>    cache_purge;                     ///< Defines CP bit.
    BitField<3>    two_way_mode;                    ///< Defines TW bits.
    BitField<2>    data_replacement_disable;        ///< Defines OD bits.
    BitField<1>    instruction_replacement_disable; ///< Defines ID bits.
    BitField<0>    cache_enable;                    ///< Defines CE bits.
};

//////////////////////////////////////////////
// 9. Direct Memory Access Controler (DMAC) //
//////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	DmaSourceAddressRegister
///
/// \brief	Dma Source Address Register (SAR0,SAR1).
///
/// \author	Runik
/// \date	19/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union DmaSourceAddressRegister {
    u32 raw; ///< Raw representation.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	DmaDestinationAddressRegister
///
/// \brief	Dma Destination Address Register (DAR0/DAR1).
///
/// \author	Runik
/// \date	19/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union DmaDestinationAddressRegister {
    u32 raw; ///< Raw representation.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	DmaTransferCountRegister
///
/// \brief	Dma Transfer Count Register (TCR0 / TCR1).
///
/// \author	Runik
/// \date	19/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union DmaTransferCountRegister {
    u32 raw; ///< Raw representation.
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

enum class AutoRequestMode : bool {
    module_request = 0, ///< Module request mode (initial).
    auto_request   = 1, ///< Auto request mode.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   AcknowledgeMode
///
/// \brief  CHCR0 - AM bit value.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class AcknowledgeMode : bool {
    output_read  = 0, ///< DACK output in read cycle/transfer from memory to device (initial).
    output_write = 1, ///< DACK output in write cycle/transfer from device to memory.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   AcknowledgeLevel
///
/// \brief  CHCR0 - AL bit value.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class AcknowledgeLevel : bool {
    active_low  = 0, ///< DACK signal is active low (initial).
    active_high = 1, ///< DACK signal is active high.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   DreqSelect
///
/// \brief  CHCR0 - DS bit value.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class DreqSelect : bool {
    by_level = 0, ///< Detected by level (initial).
    by_edge  = 1, ///< Detected by edge.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   DreqLevel
///
/// \brief  CHCR0 - DL bit value.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class DreqLevel : bool {
    low_level_or_fall  = 0, ///< DREQ detected by low level if 0, by fall if 1 (initial).
    high_level_or_rise = 1, ///< DREQ detected by high level if 0, by rise if 1.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   TransferBusMode
///
/// \brief  CHCR0 - TB bit value.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class TransferBusMode : bool {
    cycle_steal = 0, ///< Cycle steal mode (initial).
    burst       = 1, ///< Burst mode.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   TransferAddressMode
///
/// \brief  CHCR0 - TA bit value.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class TransferAddressMode : bool {
    dual_address   = 0, ///< Dual address mode (initial).
    single_address = 1, ///< Single address mode.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   InterruptEnable
///
/// \brief  CHCR0 - IE bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Sh2DmaInterruptEnable : bool {
    disabled = 0, ///< Interrupt disabled.
    enabled  = 1  ///< Interrupt enabled.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   TransferEndFlag
///
/// \brief  CHCR0 - TE bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class TransferEndFlag : bool {
    dma_not_ended_or_aborted = 0, ///< DMA has not ended or was aborted (initial).
    dma_ended_normally       = 1  ///< DMA has ended normally.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   DmaEnable
///
/// \brief  CHCR0 - DE bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Sh2DmaEnable : bool {
    dma_transfer_disabled = 0, ///< DMA transfer disabled (initial).
    dma_transfer_enabled  = 1  ///< DMA transfer enabled.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	DmaChannelControlRegister
///
/// \brief	DMA Channel Control Register (CHCR0/CHCR1).
///
/// \author	Runik
/// \date	19/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union DmaChannelControlRegister {
    u32             raw;                      ///< Raw representation.
    BitField<14, 2> destination_address_mode; ///< Defines AMx bits.
    BitField<12, 2> source_address_mode;      ///< Defines SMx bits.
    BitField<10, 2> transfer_size;            ///< Defines TSx bit.
    BitField<9>     auto_request_mode;        ///< Defines AR bit.
    BitField<8>     acknowledge_mode;         ///< Defines AM bit.
    BitField<7>     acknowledge_level;        ///< Defines AL bit.
    BitField<6>     dreq_select;              ///< Defines DS bit.
    BitField<5>     dreq_level;               ///< Defines DL bit.
    BitField<4>     transfer_busMode;         ///< Defines TB bit.
    BitField<3>     transfer_address_mode;    ///< Defines TA bit.
    BitField<2>     interrupt_enable;         ///< Defines IE bit.
    BitField<1>     transfer_end_flag;        ///< Defines TE bit.
    BitField<0>     dma_enable;               ///< Defines DE bit.
};

enum class ResourceSelect : u8 {
    dreq     = 0b00, ///< DREQ (external request) (initial).
    rxi      = 0b01, ///< RXI (on chip SCI receive data full interrupt transfer request).
    txi      = 0b10, ///< TXI (on chip SCI transmit data empty interrupt transfer request).
    reserved = 0b11  ///< Reserved (setting prohibited)
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	DmaRequestResponseSelectionControlRegister
///
/// \brief	Dma Request/Response Selection Control Register (DRCR0 / DRCR1).
///
/// \author	Runik
/// \date	19/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union DmaRequestResponseSelectionControlRegister {
    u8             raw;             ///< Raw representation.
    BitField<0, 2> resource_select; ///< Defines RSx bits.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   PriorityMode
///
/// \brief  DMAOR - PR bit.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class PriorityMode : bool {
    fixed       = 0, ///< Fixed priority (initial)
    round_robin = 1, ///< Round robin
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   AddressErrorFlag
///
/// \brief  DMAOR - AE bit.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class AddressErrorFlag : bool {
    no_dmac_address_error = 0, ///< No DMAC address error (initial)
    dmac_address_error    = 1, ///< Address error by DMAC
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   NmiFlag
///
/// \brief  DMAOR - NMIF bit.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class NmiFlag : bool {
    no_nmif_interrupt      = 0, ///< No NMIF interrupt (initial)
    nmif_interrupt_occured = 1, ///< NMIF has occurred
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   DmaMasterEnable
///
/// \brief  DMAOR - DME bit.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class DmaMasterEnable : bool {
    disabled = 0, ///< DMA transfers disabled on all channels (initial)
    enabled  = 1, ///< DMA transfers enabled on all channels
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	DmaOperationRegister
///
/// \brief	DMA Operation Register (DMAOR).
///
/// \author	Runik
/// \date	19/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union DmaOperationRegister {
    u32         raw;                ///< Raw representation.
    BitField<3> priority_mode;      ///< Defines PR bit.
    BitField<2> address_error_flag; ///< Defines AE bit.
    BitField<1> nmi_flag;           ///< Defines NMIF bit.
    BitField<0> dma_master_enable;  ///< Defines DME bit.
};

//////////////////////////////
// 10. Division Unit (DIVU) //
//////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	DivisorRegister
///
/// \brief	Divisor Register (DVSR).
///
/// \author	Runik
/// \date	19/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union DivisorRegister {
    u32 raw; ///< Raw representation.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	DividendRegister32Bits
///
/// \brief	Dividend Register L for 32 bits Division (DVDNT).
///
/// \author	Runik
/// \date	19/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union DividendRegister32Bits {
    u32 raw; ///< Raw representation.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   OverflowFlag
///
/// \brief  DVCR - OVF bit value.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class OverflowFlag : bool {
    no_overflow = 0, ///< No overflow has occurred (initial)
    overflow    = 1  ///< Overflow has occurred
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	DivisionControlRegister
///
/// \brief	Division Control Register (DVCR).
///
/// \author	Runik
/// \date	19/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union DivisionControlRegister {
    u32              raw;              ///< Raw representation.
    BitField<1>      interrupt_enable; ///< Defines OVFIE bit.
    BitField<0>      overflow_flag;    ///< Defines OVF bit.
    BitField<16, 16> upper_16_bits;    ///< Defines the range of the upper 16 bits of the register.
    BitField<0, 16>  lower_16_bits;    ///< Defines the range of the lower 16 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	DividendRegisterH
///
/// \brief	Dividend Register H (DVDNTH).
///
/// \author	Runik
/// \date	19/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union DividendRegisterH {
    u32 raw; ///< Raw representation.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	DividendRegisterL
///
/// \brief	Dividend Register L.
///
/// \author	Runik
/// \date	19/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union DividendRegisterL {
    u32 raw; ///< Raw representation.
};

//////////////////////////////////////////
// 11. 16 Bits Free Running Timer (FRT) //
//////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   InterruptCaptureInterruptEnable
///
/// \brief  TIER - ICIE values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class InterruptCaptureInterruptEnable : bool {
    interrupt_request_disabled = 0, ///< Interrupt request (ICI) caused by ICF disabled (initial)
    interrupt_request_enabled  = 1  ///< Interrupt request (ICI) caused by ICF enabled
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   OutputCompareInterruptAEnable
///
/// \brief  TIER - OCIAE values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class OutputCompareInterruptAEnable : bool {
    interrupt_request_disabled = 0, ///< Interrupt request (ICIA) caused by OCFA disabled (initial)
    interrupt_request_enabled  = 1  ///< Interrupt request (ICIA) caused by OCFA enabled
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   OutputCompareInterruptBEnable
///
/// \brief  TIER - OCIBE values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class OutputCompareInterruptBEnable : bool {
    interrupt_request_disabled = 0, ///< Interrupt request (ICIB) caused by OCFB disabled (initial)
    interrupt_request_enabled  = 1  ///< Interrupt request (ICIB) caused by OCFB enabled
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   TimerOverflowInterruptEnable
///
/// \brief  TIER - OVIE values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class TimerOverflowInterruptEnable : bool {
    interrupt_request_disabled = 0, ///< Interrupt request (FOVI) caused by OVF disabled (initial)
    interrupt_request_enabled  = 1  ///< Interrupt request (FOVI) caused by OVF enabled
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	TimerInterruptEnableRegister
///
/// \brief	Timer Interrupt Enable Register (TIER).
///
/// \author	Runik
/// \date	19/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union TimerInterruptEnableRegister {
    u8          raw;                                ///< Raw representation.
    BitField<7> interrupt_capture_interrupt_enable; ///< Defines ICIE bit.
    BitField<3> output_compare_interrupt_a_enable;  ///< Defines OCIAE bit.
    BitField<2> output_compare_interrupt_b_enable;  ///< Defines OCIBE bit.
    BitField<1> timer_overflow_interrupt_enable;    ///< Defines OVIE bit.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   InputCaptureFlag
///
/// \brief  FTCSR - ICF values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class InputCaptureFlag : bool {
    clear = 0, ///< Clear conditions: 0 is written when 1 is read
    set   = 1  ///< Set conditions: when the FRC value is sent to ICR by the input capture signal
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   OutputCompareFlagA
///
/// \brief  FTCSR - OCFA values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class OutputCompareFlagA : bool {
    clear = 0, ///< Clear conditions: 0 is written when 1 is read
    set   = 1  ///< Set conditions: when the FRC value becomes equal to OCRA
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   OutputCompareFlagB
///
/// \brief  FTCSR - OCFB values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class OutputCompareFlagB : bool {
    clear = 0, ///< Clear conditions: 0 is written when 1 is read
    set   = 1  ///< Set conditions: when the FRC value becomes equal to OCRB
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   TimerOverflowFlag
///
/// \brief  FTCSR - OVF values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class TimerOverflowFlag : bool {
    clear = 0, ///< Clear conditions: 0 is written when 1 is read
    set   = 1  ///< Set conditions: when the FRC value changes from 0xFFFF to 0x0000
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   CounterClearA
///
/// \brief  FTCSR - CCLRA values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class CounterClearA : bool {
    clear_disabled   = 0, ///< FRC clear disabled (initial)
    clear_on_compare = 1  ///< FRC cleared on compare match A
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	FreeRunningTimerControlStatusRegister
///
/// \brief	Free Running Timer Control/Status register (FTCSR).
///
/// \author	Runik
/// \date	19/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union FreeRunningTimerControlStatusRegister {
    u8          raw;                   ///< Raw representation.
    BitField<7> input_capture_flag;    ///< Defines ICF bit.
    BitField<3> output_compare_flag_a; ///< Defines OCFA bit.
    BitField<2> output_compare_flag_b; ///< Defines OCFB bit.
    BitField<1> timer_overflow_flag;   ///< Defines OVF bit.
    BitField<0> counter_clear_a;       ///< Defines CCLRA bit.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	FreeRunningCounter
///
/// \brief	Free Running Counter (FRC).
///
/// \author	Runik
/// \date	19/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union FreeRunningCounter {
    u16            raw;          ///< Raw representation.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	OutputCompareRegister
///
/// \brief	Output Compare Register (OCRA / OCRB).
///
/// \author	Runik
/// \date	19/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union OutputCompareRegister {
    u16            raw;          ///< Raw representation.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   InputEdgeSelect
///
/// \brief  TCR - IEDG bit value.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class InputEdgeSelect : bool {
    falling = 0, ///< Input captured on falling edge (initial)
    rising  = 1  ///< Input captured on rising edge
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
/// \union	TimerControlRegister
///
/// \brief	Timer Control Register (TCR).
///
/// \author	Runik
/// \date	20/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union TimerControlRegister {
    u8             raw;               ///< Raw representation.
    BitField<7>    input_edge_select; ///< Defines IEDG bit.
    BitField<0, 2> clock_select;      ///< Defines CKSx bits.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   OutputCompareRegisterSelect
///
/// \brief  TOCR - OCRS bit value.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class OutputCompareRegisterSelect : bool {
    ocra = 0, ///< Selects register OCRA
    ocrb = 1  ///< Selects register OCRB
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   OutputLevelA
///
/// \brief  TOCR - OLVLA bit value.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class OutputLevelA : bool {
    outputs_0 = 0, ///< Outputs 0 on compare match A
    outputs_1 = 1  ///< Outputs 1 on compare match A
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   OutputLevelB
///
/// \brief  TOCR - OLVLB bit value.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class OutputLevelB : bool {
    outputs_0 = 0, ///< Outputs 0 on compare match B
    outputs_1 = 1  ///< Outputs 1 on compare match B
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	TimerOutputCompareControlRegister
///
/// \brief	Timer Output Compare Control Register (TOCR).
///
/// \author	Runik
/// \date	20/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union TimerOutputCompareControlRegister {
    u8          raw;                            ///< Raw representation.
    BitField<4> output_compare_register_select; ///< Defines OCRS bit.
    BitField<1> output_level_a;                 ///< Defines OLVLA bit.
    BitField<0> output_level_b;                 ///< Defines OLVLB bit.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	InputCaptureRegister
///
/// \brief	Input Capture Register (ICR).
///
/// \author	Runik
/// \date	20/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union InputCaptureRegister {
    u16            raw;          ///< Raw representation.
    BitField<8, 8> upper_8_bits; ///< Defines the range of the upper 8 bits of the register.
    BitField<0, 8> lower_8_bits; ///< Defines the range of the lower 8 bits of the register.
};

///////////////////////////////
// 12. Watch Dog Timer (WDT) //
///////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	WatchdogTimerCounter
///
/// \brief	Watchdog Timer Counter (WTCNT).
///
/// \author	Runik
/// \date	20/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union WatchdogTimerCounter {
    u8 raw; ///< Raw representation.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   WdtOverflowFlag
///
/// \brief  WTCSR - OVF value.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class WdtOverflowFlag : bool {
    no_overflow = 0, ///< No overflow of WTCNT in interval timer mode (initial)
    overflow    = 1  ///< WTCNT overflow in interval timer mode
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   TimerModeSelect
///
/// \brief WTCSR - (WT /IT) value.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class TimerModeSelect : bool {
    interval_timer_mode = 0, ///< Interval timer mode option
    watchdog_timer_mode = 1  ///< Watchdog timer mode option
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   TimerEnable
///
/// \brief  WTCSR - TME value.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class TimerEnable : bool {
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
/// \union	WatchdogTimerControlStatusRegister
///
/// \brief	Watchdog Timer Control/Status Register (WTCSR).
///
/// \author	Runik
/// \date	20/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union WatchdogTimerControlStatusRegister {
    u8             raw;               ///< Raw representation.
    BitField<7>    overflow_flag;     ///< Defines OVF bit.
    BitField<6>    timer_mode_select; ///< Defines WT/IT bit.
    BitField<5>    timer_enable;      ///< Defines TME bit.
    BitField<0, 3> clock_select;      ///< Defines CKSx bit.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   WatchdogTimerOverflowFlag
///
/// \brief  RSTCSR - WOVF bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class WatchdogTimerOverflowFlag : bool {
    no_overflow = 0, ///< No WTCNT overflow in watchdof timer mode (initial).
    overflow    = 1  ///< Set by WTCNT overflow in watchdog mode.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   ResetEnable
///
/// \brief  RSTCSR - RSTE bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class ResetEnable : bool {
    not_reset_when_overflow = 0, ///< Not reset when WTCNT overflows (initial).
    reset_when_overflow     = 1, ///< Reset when WTCNT overflows.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   ResetSelect
///
/// \brief  RSTCSR - RSTS bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class ResetSelect : bool {
    power_on_reset = 0, ///< Power-on reset (initial)
    manual_reset   = 1  ///< Manual reset
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	ResetControlStatusRegister
///
/// \brief	Reset Control Status Register (RSTCSR).
///
/// \author	Runik
/// \date	20/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union ResetControlStatusRegister {
    u8          raw;                          ///< Raw representation.
    BitField<7> watchdog_timer_overflow_flag; ///< Defines WOVF bit.
    BitField<6> reset_enable;                 ///< Defines RSTE bit.
    BitField<5> reset_select;                 ///< Defines RSTS bit.
};

/////////////////////////////////////////////
// 13.Serial Communication Interface (SCI) //
/////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   CommunicationMode
///
/// \brief  SMR - C/A bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class CommunicationMode : bool {
    asynchronous        = 0, ///< Asynchronous mode (initial).
    clocked_synchronous = 1, ///< Clocked synchronous mode.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   CharacterLength
///
/// \brief  SMR - CHR bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class CharacterLength : bool {
    eight_bit_data = 0, ///< 8-bit data (initial).
    seven_bit_data = 1, ///< 7-bit data.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   ParityMode
///
/// \brief  SMR - O/E bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class ParityEnable : bool {
    parity_bit_not_added = 0, ///< Parity bit not added or checked (initial).
    parity_bit_added     = 1, ///< Parity bit added and checked.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   ParityMode
///
/// \brief  SMR - O/E bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class ParityMode : bool {
    even_parity = 0, ///< Even parity (initial).
    odd_parity  = 1, ///< Odd parity.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   StopBitLength
///
/// \brief  SMR - STOP bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class StopBitLength : bool {
    one_stop_bit  = 0, ///< One stop bit (initial).
    two_stop_bits = 1, ///< Two stop bits.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   MultiprocessorMode
///
/// \brief  SMR - MP bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class MultiprocessorMode : bool {
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
/// \union	SerialModeRegister
///
/// \brief	Serial Mode Register (SMR).
///
/// \author	Runik
/// \date	20/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union SerialModeRegister {
    u8             raw;                 ///< Raw representation.
    BitField<7>    communication_mode;  ///< Defines C/A bit.
    BitField<6>    character_length;    ///< Defines CHR bit.
    BitField<5>    parity_enable;       ///< Defines PE bit.
    BitField<4>    parity_mode;         ///< Defines O/E bit.
    BitField<3>    stop_bit_length;     ///< Defines STOP bit.
    BitField<2>    multiprocessor_mode; ///< Defines MP bit.
    BitField<0, 2> clock_select;        ///< Defines CKSx bit.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	BitRateRegister
///
/// \brief	Bit Rate Register (BRR).
///
/// \author	Runik
/// \date	20/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union BitRateRegister {
    u8 raw; ///< Raw representation.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   TransmitInterruptEnable
///
/// \brief  SCR - TIE bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class TransmitInterruptEnable : bool {
    interrupt_disabled = 0, ///< TXI request is disabled (initial).
    interrupt_enabled  = 1  ///< TXI request is enabled.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   ReceiveInterruptEnable
///
/// \brief  SCR - REI bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class ReceiveInterruptEnable : bool {
    interrupts_disabled = 0, ///< RXI and ERI requests are disabled (initial).
    interrupts_enabled  = 1  ///< RXI and ERI requests are enabled.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   TransmitEnable
///
/// \brief  SCR - TE bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class TransmitEnable : bool {
    transmitter_disabled = 0, ///< Transmitter disabled (initial).
    transmitter_enabled  = 1  ///< Transmitter enabled.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   ReceiveEnable
///
/// \brief  SCR - RE bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class ReceiveEnable : bool {
    receiver_disabled = 0, ///< Receiver disabled (initial).
    receiver_enabled  = 1  ///< Receiver enabled.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   MultiprocessorInterruptEnable
///
/// \brief  SCR - MPIE bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class MultiprocessorInterruptEnable : bool {
    interrupt_disabled = 0, ///< Multiprocessor interrupts are disabled (initial).
    interrupt_enabled  = 1  ///< Multiprocessor interrupts are enabled.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   TransmitEndInterruptEnable
///
/// \brief  SCR - TEIE bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class TransmitEndInterruptEnable : bool {
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
/// \union	SerialControlRegister
///
/// \brief	Serial Control Register (SCR).
///
/// \author	Runik
/// \date	20/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union SerialControlRegister {
    u8             raw;                             ///< Raw representation.
    BitField<7>    transmit_interrupt_enable;       ///< Defines TIE bit.
    BitField<6>    receive_interrupts_enable;       ///< Defines RIE bit.
    BitField<5>    transmit_enable;                 ///< Defines TE bit.
    BitField<4>    receive_enable;                  ///< Defines ORE bit.
    BitField<3>    multiprocessor_interrupt_enable; ///< Defines MPIE bit.
    BitField<2>    transmit_end_interrupt_enable;   ///< Defines TEIE bit.
    BitField<0, 2> clock_enable;                    ///< Defines CKEx bit.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	TransmitDataRegister
///
/// \brief	Transmit Data Register (TDR).
///
/// \author	Runik
/// \date	20/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union TransmitDataRegister {
    u8 raw; ///< Raw representation.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	TransmitShiftRegister
///
/// \brief	Transmit Shift Register (TSR).
///
/// \author	Runik
/// \date	20/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union TransmitShiftRegister {
    u8 raw; ///< Raw representation.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   TransmitDataRegisterEmpty
///
/// \brief  SSR - TDRE bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class TransmitDataRegisterEmpty : bool {
    contains_valid_data         = 0, ///< TDR contains valid transmit data (initial).
    does_not_contain_valid_data = 1  ///< TDR does not contain valid transmit data.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   ReceiveDataRegisterFull
///
/// \brief  SSR - RDRF bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class ReceiveDataRegisterFull : bool {
    no_valid_data_received = 0, ///< RDR does not contain valid received data (initial).
    valid_data_received    = 1  ///< RDR contains valid received data.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   OverrunError
///
/// \brief  SSR - ORER bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class OverrunError : bool {
    in_progress_or_ended_normally = 0, ///< Receiving is in progress or has ended normally (initial).
    error_occurred                = 1  ///< A receive overrun error occurred.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   FramingError
///
/// \brief  SSR - FER bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class FramingError : bool {
    in_progress_or_ended_normally = 0, ///< Receiving is in progress or has ended normally (initial).
    error_occurred                = 1  ///< A receive framing error occurred.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   ParityError
///
/// \brief  SSR - PER bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class ParityError : bool {
    in_progress_or_ended_normally = 0, ///< Receiving is in progress or has ended normally (initial).
    error_occurred                = 1  ///< A receive parity error occurred.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   TransmitEnd
///
/// \brief  SSR - TEND bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class TransmitEnd : bool {
    transmission_in_progress = 0, ///< Transmission is in progress.
    end_of_transmission      = 1  ///< End of transmission (initial).
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   MultiprocessorBit
///
/// \brief  SSR - MPB bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class MultiprocessorBit : bool {
    bit_value_0 = 0, ///< Multiprocessor bit value in receive data is 0 (initial).
    bit_value_1 = 1  ///< Multiprocessor bit value in receive data is 1.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   MultiprocessorBitTransfer
///
/// \brief  SSR - MPBT bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class MultiprocessorBitTransfer : bool {
    bit_value_0 = 0, ///< Multiprocessor bit value in transmit data is 0 (initial).
    bit_value_1 = 1  ///< Multiprocessor bit value in transmit data is 1.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	SerialStatusRegister
///
/// \brief	Serial Status Register (SSR).
///
/// \author	Runik
/// \date	20/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union SerialStatusRegister {
    u8          raw;                          ///< Raw representation.
    BitField<7> transmit_data_register_empty; ///< Defines TDRE bit.
    BitField<6> receive_data_register_full;   ///< Defines RDRF bit.
    BitField<5> overrun_error;                ///< Defines ORER bit.
    BitField<4> framing_error;                ///< Defines FER bit.
    BitField<3> parity_error;                 ///< Defines PER bit.
    BitField<2> transmit_end;                 ///< Defines TEND bit.
    BitField<1> multiprocessor_bit;           ///< Defines MPB bit.
    BitField<0> multiprocessor_bit_transfer;  ///< Defines MPBT bit.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	ReceiveDataRegister
///
/// \brief	Receive Data Register (RDR).
///
/// \author	Runik
/// \date	20/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union ReceiveDataRegister {
    u8 raw; ///< Raw representation.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	ReceiveShiftRegister
///
/// \brief	Receive Shift Register (RSR).
///
/// \author	Runik
/// \date	20/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union ReceiveShiftRegister {
    u8 raw; ///< Raw representation.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union	StandbyControlRegister
///
/// \brief	Standby Control Register (SCR)
///
/// \author	Runik
/// \date	20/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union StandbyControlRegister {
    u8 raw; ///< Raw representation.
};

} // namespace saturnin::sh2