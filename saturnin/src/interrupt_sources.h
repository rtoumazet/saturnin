// 
// interrupt_sources.h
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
/// \file	interrupt_sources.h
///
/// \brief	Lists the various interrupt sources used in the system. 
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "interrupt_controller.h" // Interrupt

namespace saturnin {
namespace core {

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \namespace  InterruptSource
///
/// \brief  Regroups all the interrupt sources that can be sent by the system.
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace InterruptSource {

constexpr Interrupt nmi                  { 0xB, 0xF };
constexpr Interrupt user_break           { 0xC, 0xF };
constexpr Interrupt v_blank_in           { 0x40, 0xF };
constexpr Interrupt v_blank_out          { 0x41, 0xE };
constexpr Interrupt h_blank_in           { 0x42, 0xD };
constexpr Interrupt timer_0              { 0x43, 0xC };
constexpr Interrupt timer_1              { 0x44, 0xB };
constexpr Interrupt dsp_end              { 0x45, 0xA };
constexpr Interrupt sound_request        { 0x46, 0x9 };
constexpr Interrupt system_manager       { 0x47, 0x8 };
constexpr Interrupt pad_interrupt        { 0x48, 0x8 };
constexpr Interrupt level_2_dma_end      { 0x49, 0x6 };
constexpr Interrupt level_1_dma_end      { 0x4A, 0x6 };
constexpr Interrupt level_0_dma_end      { 0x4B, 0x5 };
constexpr Interrupt dma_illegal          { 0x4C, 0x3 };
constexpr Interrupt sprite_draw_end      { 0x4D, 0x2 };
constexpr Interrupt external_interrupt_00{ 0x50, 0x7 };
constexpr Interrupt external_interrupt_01{ 0x51, 0x7 };
constexpr Interrupt external_interrupt_02{ 0x52, 0x7 };
constexpr Interrupt external_interrupt_03{ 0x53, 0x7 };
constexpr Interrupt external_interrupt_04{ 0x54, 0x4 };
constexpr Interrupt external_interrupt_05{ 0x55, 0x4 };
constexpr Interrupt external_interrupt_06{ 0x56, 0x4 };
constexpr Interrupt external_interrupt_07{ 0x57, 0x4 };
constexpr Interrupt external_interrupt_08{ 0x58, 0x1 };
constexpr Interrupt external_interrupt_09{ 0x59, 0x1 };
constexpr Interrupt external_interrupt_10{ 0x5A, 0x1 };
constexpr Interrupt external_interrupt_11{ 0x5B, 0x1 };
constexpr Interrupt external_interrupt_12{ 0x5C, 0x1 };
constexpr Interrupt external_interrupt_13{ 0x5D, 0x1 };
constexpr Interrupt external_interrupt_14{ 0x5E, 0x1 };
constexpr Interrupt external_interrupt_15{ 0x5F, 0x1 };

// Following interrupts are dynamically set
Interrupt sh2_division_overflow            { 0, 0 };
Interrupt sh2_dma_0_transfer_end           { 0, 0 };
Interrupt sh2_dma_1_transfer_end           { 0, 0 };
Interrupt sh2_wdt_overflow                 { 0, 0 };
Interrupt sh2_bsc_values_matched           { 0, 0 };
Interrupt sh2_sci_transmit_end             { 0, 0 };
Interrupt sh2_sci_receive_error            { 0, 0 };
Interrupt sh2_sci_receive_data_full        { 0, 0 };
Interrupt sh2_sci_transmit_data_empty      { 0, 0 };
Interrupt sh2_frt_input_capture_flag_set   { 0, 0 };
Interrupt sh2_frt_output_compare_flag_a_set{ 0, 0 };
Interrupt sh2_frt_output_compare_flag_b_set{ 0, 0 };
Interrupt sh2_frt_overflow_flag_set        { 0, 0 };
}

}
}