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

namespace interrupt_source {

static const Interrupt nmi                     { 0xB, 0xF  , InterruptMaskRegister::undefined, InterruptStatusRegister::undefined };
static const Interrupt user_break                  { 0xC, 0xF  , InterruptMaskRegister::undefined, InterruptStatusRegister::undefined };
static const Interrupt v_blank_in                  { 0x40, 0xF , InterruptMaskRegister::vBlankIn, InterruptStatusRegister::vBlankIn };
static const Interrupt v_blank_out                 { 0x41, 0xE , InterruptMaskRegister::vBlankOut, InterruptStatusRegister::vBlankOut };
static const Interrupt h_blank_in                  { 0x42, 0xD , InterruptMaskRegister::hBlankIn, InterruptStatusRegister::hBlankIn };
static const Interrupt timer_0                     { 0x43, 0xC , InterruptMaskRegister::timer0, InterruptStatusRegister::timer0 };
static const Interrupt timer_1                     { 0x44, 0xB , InterruptMaskRegister::timer1, InterruptStatusRegister::timer1 };
static const Interrupt dsp_end                     { 0x45, 0xA , InterruptMaskRegister::dspEnd, InterruptStatusRegister::dspEnd };
static const Interrupt sound_request               { 0x46, 0x9 , InterruptMaskRegister::soundRequest, InterruptStatusRegister::soundRequest};
static const Interrupt system_manager              { 0x47, 0x8 , InterruptMaskRegister::systemManager, InterruptStatusRegister::systemManager };
static const Interrupt pad_interrupt               { 0x48, 0x8 , InterruptMaskRegister::pad, InterruptStatusRegister::pad };
static const Interrupt level_2_dma_end             { 0x49, 0x6 , InterruptMaskRegister::level2Dma, InterruptStatusRegister::level2Dma };
static const Interrupt level_1_dma_end             { 0x4A, 0x6 , InterruptMaskRegister::level1Dma, InterruptStatusRegister::level1Dma};
static const Interrupt level_0_dma_end             { 0x4B, 0x5 , InterruptMaskRegister::level0Dma, InterruptStatusRegister::level0Dma };
static const Interrupt dma_illegal                 { 0x4C, 0x3 , InterruptMaskRegister::dmaIllegal, InterruptStatusRegister::dmaIllegal};
static const Interrupt sprite_draw_end             { 0x4D, 0x2 , InterruptMaskRegister::spriteDrawEnd, InterruptStatusRegister::spriteDrawEnd };
static const Interrupt external_interrupt_00       { 0x50, 0x7 , InterruptMaskRegister::external00, InterruptStatusRegister::external00 };
static const Interrupt external_interrupt_01       { 0x51, 0x7 , InterruptMaskRegister::external01, InterruptStatusRegister::external01 };
static const Interrupt external_interrupt_02       { 0x52, 0x7 , InterruptMaskRegister::external02, InterruptStatusRegister::external02 };
static const Interrupt external_interrupt_03       { 0x53, 0x7 , InterruptMaskRegister::external03, InterruptStatusRegister::external03 };
static const Interrupt external_interrupt_04       { 0x54, 0x4 , InterruptMaskRegister::external04, InterruptStatusRegister::external04 };
static const Interrupt external_interrupt_05       { 0x55, 0x4 , InterruptMaskRegister::external05, InterruptStatusRegister::external05 };
static const Interrupt external_interrupt_06       { 0x56, 0x4 , InterruptMaskRegister::external06, InterruptStatusRegister::external06 };
static const Interrupt external_interrupt_07       { 0x57, 0x4 , InterruptMaskRegister::external07, InterruptStatusRegister::external07 };
static const Interrupt external_interrupt_08       { 0x58, 0x1 , InterruptMaskRegister::external08, InterruptStatusRegister::external08 };
static const Interrupt external_interrupt_09       { 0x59, 0x1 , InterruptMaskRegister::external09, InterruptStatusRegister::external09 };
static const Interrupt external_interrupt_10       { 0x5A, 0x1 , InterruptMaskRegister::external10, InterruptStatusRegister::external10 };
static const Interrupt external_interrupt_11       { 0x5B, 0x1 , InterruptMaskRegister::external11, InterruptStatusRegister::external11 };
static const Interrupt external_interrupt_12       { 0x5C, 0x1 , InterruptMaskRegister::external12, InterruptStatusRegister::external12 };
static const Interrupt external_interrupt_13       { 0x5D, 0x1 , InterruptMaskRegister::external13, InterruptStatusRegister::external13 };
static const Interrupt external_interrupt_14       { 0x5E, 0x1 , InterruptMaskRegister::external14, InterruptStatusRegister::external14 };
static const Interrupt external_interrupt_15       { 0x5F, 0x1 , InterruptMaskRegister::external15, InterruptStatusRegister::external15 };
static const Interrupt frt_input_capture_interrupt { 0x64, 0xF , InterruptMaskRegister::undefined, InterruptStatusRegister::undefined };
//constexpr Interrupt frt_input_capture_signal  { 0x65, 0xF }; // Vector 0x65 was registered in previous version of Saturnin, can't remind its purpose

// Following interrupts are dynamically set
static Interrupt sh2_division_overflow            { 0, 0 , InterruptMaskRegister::undefined, InterruptStatusRegister::undefined };
static Interrupt sh2_dma_0_transfer_end           { 0, 0 , InterruptMaskRegister::undefined, InterruptStatusRegister::undefined };
static Interrupt sh2_dma_1_transfer_end           { 0, 0 , InterruptMaskRegister::undefined, InterruptStatusRegister::undefined };
static Interrupt sh2_wdt_overflow                 { 0, 0 , InterruptMaskRegister::undefined, InterruptStatusRegister::undefined };
static Interrupt sh2_bsc_values_matched           { 0, 0 , InterruptMaskRegister::undefined, InterruptStatusRegister::undefined };
static Interrupt sh2_sci_transmit_end             { 0, 0 , InterruptMaskRegister::undefined, InterruptStatusRegister::undefined };
static Interrupt sh2_sci_receive_error            { 0, 0 , InterruptMaskRegister::undefined, InterruptStatusRegister::undefined };
static Interrupt sh2_sci_receive_data_full        { 0, 0 , InterruptMaskRegister::undefined, InterruptStatusRegister::undefined };
static Interrupt sh2_sci_transmit_data_empty      { 0, 0 , InterruptMaskRegister::undefined, InterruptStatusRegister::undefined };
static Interrupt sh2_frt_input_capture_flag_set   { 0, 0 , InterruptMaskRegister::undefined, InterruptStatusRegister::undefined };
static Interrupt sh2_frt_output_compare_flag_a_set{ 0, 0 , InterruptMaskRegister::undefined, InterruptStatusRegister::undefined };
static Interrupt sh2_frt_output_compare_flag_b_set{ 0, 0 , InterruptMaskRegister::undefined, InterruptStatusRegister::undefined };
static Interrupt sh2_frt_overflow_flag_set        { 0, 0 , InterruptMaskRegister::undefined, InterruptStatusRegister::undefined };
}

}
}