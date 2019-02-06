// 
// sh2_registers.h
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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file	sh2_registers.h
///
/// \brief	Declares SH2 onchip registers and related enums. 
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "emulator_defs.h"

namespace saturnin {
namespace core {

/// \name Onboard i/o registers addresses
//@{
constexpr u32 serial_mode_register                              = 0xFFFFFE00;
constexpr u32 bit_rate_register                                 = 0xFFFFFE01;
constexpr u32 serial_control_register                           = 0xFFFFFE02;
constexpr u32 transmit_data_register                            = 0xFFFFFE03;
constexpr u32 serial_status_register                            = 0xFFFFFE04;
constexpr u32 receive_data_register                             = 0xFFFFFE05;
constexpr u32 timer_interrupt_enable_register                   = 0xFFFFFE10;
constexpr u32 free_running_timer_control_status_register        = 0xFFFFFE11;
constexpr u32 free_running_counter                              = 0xFFFFFE12;
constexpr u32 free_running_counter_h                            = 0xFFFFFE12;
constexpr u32 free_running_counter_l                            = 0xFFFFFE13;
constexpr u32 output_compare_register                           = 0xFFFFFE14;
constexpr u32 output_compare_register_b_h                       = 0xFFFFFE14;
constexpr u32 output_compare_register_b_l                       = 0xFFFFFE15;
constexpr u32 output_compare_register_a_h                       = 0xFFFFFE14;
constexpr u32 output_compare_register_a_l                       = 0xFFFFFE15;
constexpr u32 timer_control_register                            = 0xFFFFFE16;
constexpr u32 timer_output_compare_control_register             = 0xFFFFFE17;
constexpr u32 ficr                                              = 0xFFFFFE18;
constexpr u32 input_capture_register_h                          = 0xFFFFFE18;
constexpr u32 input_capture_register_l                          = 0xFFFFFE19;
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

/////////////////////////////////////
// 5. Interrupt Controller (INTC)  //
/////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   NmiInputLevel
///
/// \brief  ICR - NMIL bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class NmiInputLevel : u8 {
    low = 0b0, ///< NMI input level is low
    high = 0b1  ///< NMI input level is high
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   NmiEdgeDetection
///
/// \brief  ICR - NMIE bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class NmiEdgeDetection : u8 {
    falling = 0b0, ///< Interrupt request detected on falling edge of NMI input (initial)
    rising = 0b1  ///< Interrupt request detected on rising edge of NMI input
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   VectorMode
///
/// \brief  ICR - VECMD bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class VectorMode : u8 {
    auto_vector = 0b0,   ///< Auto vector mode (initial)
    external_vector = 0b1    ///< External vector mode
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  InterruptControlRegister
///
/// \brief  Interrupt Control Register (ICR).
///         
/// \author Runik
/// \date   02/02/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class InterruptControlRegister : Register {
    public:
    using Register::Register;
    auto nmiInputLevel() { return static_cast<NmiInputLevel>(extract(15)); };      ///< returns NMIL bit. 
    auto nmiEdgeDetection() { return static_cast<NmiEdgeDetection>(extract(8)); };  ///< returns NMIE bit. 
    auto vectorMode() { return static_cast<VectorMode>(extract(0)); };              ///< returns VECMD bit. 
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

class BusControlRegister1 : Register {
    public:
    using Register::Register;
    static auto writeMask() { return 0b0001111111110111; } ///< returns write mask;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  BusControlRegister1
///
/// \brief  Bus Control Register 2 (BCR2).
///
/// \author Runik
/// \date   02/02/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class BusControlRegister2 : Register {
    public:
    using Register::Register;
    static auto writeMask() { return 0b0000000011111100; } ///< returns write mask;
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
    way_0 = 0b00,  ///< Way 0 (initial value).
    way_1 = 0b01,  ///< Way 1
    way_2 = 0b10,  ///< Way 2
    way_3 = 0b11   ///< Way 3
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   CachePurge
///
/// \brief  CCR - CP bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class CachePurge : u8 {
    normal_operation = 0b0,   ///< Normal operation.
    cache_purge = 0b1    ///< Cache purge.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   TwoWayMode
///
/// \brief  CCR - CP bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class TwoWayMode : u8 {
    four_way = 0b0,   ///< Four way mode (initial).
    two_way = 0b1    ///< Two way mode.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   DataReplacementDisable
///
/// \brief  CCR - OD bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class DataReplacementDisable : u8 {
    normal_operation = 0b0,   ///< Normal operation (initial).
    data_not_replaced = 0b1    ///< Data not replaced even when wache miss occurs in data access.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   InstructionReplacementDisable
///
/// \brief  CCR - ID bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class InstructionReplacementDisable : u8 {
    normal_operation = 0b0, ///< Normal operation (initial).
    data_not_replaced = 0b1  ///< Data not replaced even when wache miss occurs in instruction fetch.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   CacheEnable
///
/// \brief  CCR - CE bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class CacheEnable : u8 {
    cache_disabled = 0b0, ///< Cache disabled (initial).
    cache_enabled = 0b1  ///< Cache enabled.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  CacheControlRegister
///
/// \brief  Cache Control Register (CCR).
///
/// \author Runik
/// \date   02/02/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class CacheControlRegister : Register {
    public:
    using Register::Register;
    auto waySpecification() { return static_cast<WaySpecification>(extract(6, 7)); }                          ///< returns Wx bits.
    auto cachePurge() { return static_cast<CachePurge>(extract(4)); }                                       ///< returns CP bit.
    auto twoWayMode() { return static_cast<TwoWayMode>(extract(3)); }                                       ///< returns TW bit.
    auto dataReplacementDisable() { return static_cast<DataReplacementDisable>(extract(2)); }               ///< returns OD bit.
    auto instructionReplacementDisable() { return static_cast<InstructionReplacementDisable>(extract(1)); } ///< returns ID bit.
    auto cacheEnable() { return static_cast<CacheEnable>(extract(0)); }                                     ///< returns CE bit.
};

//////////////////////////////////////////////
// 9. Direct Memory Access Controler (DMAC) //
//////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   InterruptEnable
///
/// \brief  CHCR0 - DMx bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class DestinationAddressMode : u8 {
    fixed = 0b00, ///< Fixed destination address (initial).
    incremented = 0b01, ///< Destination address is incremented.
    decremented = 0b10, ///< Destination address is decremented.
    reserved = 0b11  ///< Reserved.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   SourceAddressMode
///
/// \brief  CHCR0 - SMx bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class SourceAddressMode : u8 {
    fixed = 0b00, ///< Fixed source address (initial).
    incremented = 0b01, ///< Source address is incremented.
    decremented = 0b10, ///< Source address is decremented.
    reserved = 0b11  ///< Reserved.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   TransferSize
///
/// \brief  CHCR0 - TSx bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class TransferSize : u8 {
    byte_unit = 0b00, ///< Byte unit (initial).
    word_unit = 0b01, ///< Word unit.
    long_unit = 0b10, ///< Long unit.
    sixteen_byte_unit = 0b11  ///< 16-byte unit (4 longword transfers).
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   AutoRequestMode
///
/// \brief  CHCR0 - AR bit value.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class AutoRequestMode : u8 {
    module_request = 0b0, ///< Module request mode (initial).
    auto_request = 0b1, ///< Auto request mode.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   AcknowledgeMode
///
/// \brief  CHCR0 - AM bit value.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class AcknowledgeMode : u8 {
    output_read = 0b0, ///< DACK output in read cycle/transfer from memory to device (initial).
    output_write = 0b1, ///< DACK output in write cycle/transfer from device to memory.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   AcknowledgeLevel
///
/// \brief  CHCR0 - AL bit value.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class AcknowledgeLevel : u8 {
    active_low = 0b0, ///< DACK signal is active low (initial).
    active_high = 0b1, ///< DACK signal is active high.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   DreqSelect
///
/// \brief  CHCR0 - DS bit value.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class DreqSelect : u8 {
    by_level = 0b0, ///< Detected by level (initial).
    by_edge = 0b1, ///< Detected by edge.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   DreqLevel
///
/// \brief  CHCR0 - DL bit value.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class DreqLevel : u8 {
    low_level_or_fall = 0b0, ///< DREQ detected by low level if 0, by fall if 1 (initial).
    high_level_or_rise = 0b1, ///< DREQ detected by high level if 0, by rise if 1.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   TransferBusMode
///
/// \brief  CHCR0 - TB bit value.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class TransferBusMode : u8 {
    cycle_steal = 0b0, ///< Cycle steal mode (initial).
    burst = 0b1, ///< Burst mode.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   TransferAddressMode
///
/// \brief  CHCR0 - TA bit value.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class TransferAddressMode : u8 {
    dual_address = 0b0, ///< Dual address mode (initial).
    single_address = 0b1, ///< Single address mode.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   InterruptEnable
///
/// \brief  CHCR0 - IE bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class InterruptEnable : u8 {
    disabled = 0b0,   ///< Interrupt disabled.
    enabled = 0b1    ///< Interrupt enabled.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   TransferEndFlag
///
/// \brief  CHCR0 - TE bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class TransferEndFlag : u8 {
    dma_not_ended_or_aborted = 0b0, ///< DMA has not ended or was aborted (initial).
    dma_ended_normally = 0b1  ///< DMA has ended normally.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   DmaEnable
///
/// \brief  CHCR0 - DE bit values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Sh2DmaEnable : u8 {
    dma_transfer_disabled = 0b0, ///< DMA transfer disabled (initial).
    dma_transfer_enabled = 0b1  ///< DMA transfer enabled.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  DmaChannelControlRegister0
///
/// \brief  DMA Channel Control Register 0 (CHCR0).
///
/// \author Runik
/// \date   02/02/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class DmaChannelControlRegister : Register {
    public:
    using Register::Register;
    auto destinationAddressMode() { return static_cast<DestinationAddressMode>(extract(14, 15)); } ///< returns DMx  bits.
    auto sourceAddressMode() { return static_cast<SourceAddressMode>(extract(12, 13)); } ///< returns SMx  bits.
    auto transferSize() { return static_cast<TransferSize>(extract(10, 11)); } ///< returns TSx bits.
    auto autoRequestMode() { return static_cast<AutoRequestMode>(extract(9)); } ///< returns AR bit.
    auto acknowledgeMode() { return static_cast<AcknowledgeMode>(extract(8)); } ///< returns AM bit.
    auto acknowledgeLevel() { return static_cast<AcknowledgeLevel>(extract(7)); } ///< returns AL bit.
    auto dreqSelect() { return static_cast<DreqSelect>(extract(6)); } ///< returns DS bit.
    auto dreqLevel() { return static_cast<DreqLevel>(extract(5)); } ///< returns DL bit.
    auto transferBusMode() { return static_cast<TransferBusMode>(extract(4)); } ///< returns TB bit.
    auto transferAddressMode() { return static_cast<TransferAddressMode>(extract(3)); } ///< returns TA bit.
    auto interruptEnable() { return static_cast<InterruptEnable>(extract(2)); } ///< returns IE bit.
    auto transferEndFlag() { return static_cast<TransferEndFlag>(extract(1)); } ///< returns TE bit.
    auto dmaEnable() { return static_cast<Sh2DmaEnable>(extract(0)); } ///< returns DE bit.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   PriorityMode
///
/// \brief  DMAOR - PR bit.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class PriorityMode : u8 {
    fixed = 0b0,  ///< Fixed priority (initial)
    round_robin = 0b1,  ///< Round robin
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   AddressErrorFlag
///
/// \brief  DMAOR - AE bit.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class AddressErrorFlag : u8 {
    no_dmac_address_error = 0b0,  ///< No DMAC address error (initial)
    dmac_address_error = 0b1,  ///< Address error by DMAC
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   NmiFlag
///
/// \brief  DMAOR - NMIF bit.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class NmiFlag : u8 {
    no_nmif_interrupt = 0b0,  ///< No NMIF interrupt (initial)
    nmif_interrupt_occured = 0b1,  ///< NMIF has occurred
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   DmaMasterEnable
///
/// \brief  DMAOR - DME bit.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class DmaMasterEnable : u8 {
    disabled = 0b0,  ///< DMA transfers disabled on all channels (initial)
    enabled = 0b1,  ///< DMA transfers enabled on all channels
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  DmaOperationRegister
///
/// \brief  DMA Operation Register (DMAOR).
///
/// \author Runik
/// \date   03/02/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class DmaOperationRegister : Register {
    public:
    using Register::Register;
    auto priorityMode() { return static_cast<PriorityMode>(extract(3)); }         ///< returns PR bit.
    auto addressErrorFlag() { return static_cast<AddressErrorFlag>(extract(2)); } ///< returns AE bit.
    auto nmiFlag() { return static_cast<NmiFlag>(extract(1)); }                   ///< returns NMIF bit.
    auto dmaMasterEnable() { return static_cast<DmaMasterEnable>(extract(0)); }   ///< returns DME bit.
};

//////////////////////////////////////////
// 11. 16 Bits Free Running Timer (FRT) //
//////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   OutputCompareRegisterSelect
///
/// \brief  TOCR - OCRS bit value.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class OutputCompareRegisterSelect : u8 {
    ocra = 0b0,  ///< Selects register OCRA
    ocrb = 0b1   ///< Selects register OCRB
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   OutputLevelA
///
/// \brief  TOCR - OLVLA bit value.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class OutputLevelA : u8 {
    outputs_0 = 0b0,  ///< Outputs 0 on compare match A
    outputs_1 = 0b1   ///< Outputs 1 on compare match A
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   OutputLevelB
///
/// \brief  TOCR - OLVLB bit value.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class OutputLevelB : u8 {
    outputs_0 = 0b0,  ///< Outputs 0 on compare match B
    outputs_1 = 0b1   ///< Outputs 1 on compare match B
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  TimerOutputCompareControlRegister
///
/// \brief  Timer Output Compare Control Register (TOCR).
///
/// \author Runik
/// \date   03/02/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

class TimerOutputCompareControlRegister : Register {
    public:
    using Register::Register;
    auto outputCompareRegisterSelect() { return static_cast<OutputCompareRegisterSelect>(extract(4)); } ///< returns OCRS bit.
    auto outputLevelA() { return static_cast<OutputLevelA>(extract(1)); }                               ///< returns OLVLA bit.
    auto outputLevelB() { return static_cast<OutputLevelB>(extract(0)); }                               ///< returns OLVLB bit.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   InputEdgeSelect
///
/// \brief  TCR - IEDG bit value.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class InputEdgeSelect : u8 {
    falling = 0b0,  ///< Input captured on falling edge (initial)
    rising = 0b1   ///< Input captured on rising edge
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   ClockSelect
///
/// \brief  TCR - CKSx bits values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class ClockSelect : u8 {
    internal_divided_by_8 = 0b00, ///< Internal clock /8 (initial)
    internal_divided_by_32 = 0b01, ///< Internal clock /32
    internal_divided_by_128 = 0b10, ///< Internal clock /128
    external = 0b11  ///< External clock
};

class TimerControlRegister : Register {
    public:
    using Register::Register;
    auto inputEdgeSelect() { return static_cast<InputEdgeSelect>(extract(7)); }  ///< returns IEDG bit.
    auto clockSelect() { return static_cast<ClockSelect>(extract(0, 1)); }          ///< returns CKSx bits.
    void clockSelect(ClockSelect) {}
};

}
}
