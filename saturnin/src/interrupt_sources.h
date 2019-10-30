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

static constexpr u8 vector_nmi              { 0xB };
static constexpr u8 vector_user_break       { 0xC };
static constexpr u8 vector_v_blank_in       { 0x40 };
static constexpr u8 vector_v_blank_out      { 0x41 };
static constexpr u8 vector_h_blank_in       { 0x42 };
static constexpr u8 vector_timer_0          { 0x43 };
static constexpr u8 vector_timer_1          { 0x44 };
static constexpr u8 vector_dsp_end          { 0x45 };
static constexpr u8 vector_sound_request    { 0x46 };
static constexpr u8 vector_system_manager   { 0x47 };
static constexpr u8 vector_pad_interrupt    { 0x48 };
static constexpr u8 vector_level_2_dma_end  { 0x49 };
static constexpr u8 vector_level_1_dma_end  { 0x4A };
static constexpr u8 vector_level_0_dma_end  { 0x4B };
static constexpr u8 vector_dma_illegal      { 0x4C };
static constexpr u8 vector_sprite_draw_end  { 0x4D };
static constexpr u8 vector_external_00      { 0x50 };
static constexpr u8 vector_external_01      { 0x51 };
static constexpr u8 vector_external_02      { 0x52 };
static constexpr u8 vector_external_03      { 0x53 };
static constexpr u8 vector_external_04      { 0x54 };
static constexpr u8 vector_external_05      { 0x55 };
static constexpr u8 vector_external_06      { 0x56 };
static constexpr u8 vector_external_07      { 0x57 };
static constexpr u8 vector_external_08      { 0x58 };
static constexpr u8 vector_external_09      { 0x59 };
static constexpr u8 vector_external_10      { 0x5A };
static constexpr u8 vector_external_11      { 0x5B };
static constexpr u8 vector_external_12      { 0x5C };
static constexpr u8 vector_external_13      { 0x5D };
static constexpr u8 vector_external_14      { 0x5E };
static constexpr u8 vector_external_15      { 0x5F };
static constexpr u8 vector_frt_input_capture{ 0x64 };

static const Interrupt undefined         { 0,                        0,     InterruptMaskRegister::undefined, InterruptStatusRegister::undefined };
static const Interrupt nmi               { vector_nmi,               0xF  , InterruptMaskRegister::undefined, InterruptStatusRegister::undefined };
static const Interrupt user_break        { vector_user_break,        0xF  , InterruptMaskRegister::undefined, InterruptStatusRegister::undefined };
static const Interrupt v_blank_in        { vector_v_blank_in,        0xF , InterruptMaskRegister::v_blank_in, InterruptStatusRegister::v_blank_in };
static const Interrupt v_blank_out       { vector_v_blank_out,       0xE , InterruptMaskRegister::v_blank_out, InterruptStatusRegister::v_blank_out };
static const Interrupt h_blank_in        { vector_h_blank_in,        0xD , InterruptMaskRegister::h_blank_in, InterruptStatusRegister::h_blank_in };
static const Interrupt timer_0           { vector_timer_0,           0xC , InterruptMaskRegister::timer_0, InterruptStatusRegister::timer_0 };
static const Interrupt timer_1           { vector_timer_1,           0xB , InterruptMaskRegister::timer_1, InterruptStatusRegister::timer_1 };
static const Interrupt dsp_end           { vector_dsp_end,           0xA , InterruptMaskRegister::dsp_end, InterruptStatusRegister::dsp_end };
static const Interrupt sound_request     { vector_sound_request,     0x9 , InterruptMaskRegister::sound_request, InterruptStatusRegister::sound_request};
static const Interrupt system_manager    { vector_system_manager,    0x8 , InterruptMaskRegister::system_manager, InterruptStatusRegister::system_manager };
static const Interrupt pad_interrupt     { vector_pad_interrupt,     0x8 , InterruptMaskRegister::pad, InterruptStatusRegister::pad };
static const Interrupt level_2_dma_end   { vector_level_2_dma_end,   0x6 , InterruptMaskRegister::level_2_dma, InterruptStatusRegister::level_2_dma };
static const Interrupt level_1_dma_end   { vector_level_1_dma_end,   0x6 , InterruptMaskRegister::level_1_dma, InterruptStatusRegister::level_1_dma};
static const Interrupt level_0_dma_end   { vector_level_0_dma_end,   0x5 , InterruptMaskRegister::level_0_dma, InterruptStatusRegister::level_0_dma };
static const Interrupt dma_illegal       { vector_dma_illegal,       0x3 , InterruptMaskRegister::dma_illegal, InterruptStatusRegister::dma_illegal};
static const Interrupt sprite_draw_end   { vector_sprite_draw_end,   0x2 , InterruptMaskRegister::sprite_draw_end, InterruptStatusRegister::sprite_draw_end };
static const Interrupt external_00       { vector_external_00,       0x7 , InterruptMaskRegister::external_00, InterruptStatusRegister::external_00 };
static const Interrupt external_01       { vector_external_01,       0x7 , InterruptMaskRegister::external_01, InterruptStatusRegister::external_01 };
static const Interrupt external_02       { vector_external_02,       0x7 , InterruptMaskRegister::external_02, InterruptStatusRegister::external_02 };
static const Interrupt external_03       { vector_external_03,       0x7 , InterruptMaskRegister::external_03, InterruptStatusRegister::external_03 };
static const Interrupt external_04       { vector_external_04,       0x4 , InterruptMaskRegister::external_04, InterruptStatusRegister::external_04 };
static const Interrupt external_05       { vector_external_05,       0x4 , InterruptMaskRegister::external_05, InterruptStatusRegister::external_05 };
static const Interrupt external_06       { vector_external_06,       0x4 , InterruptMaskRegister::external_06, InterruptStatusRegister::external_06 };
static const Interrupt external_07       { vector_external_07,       0x4 , InterruptMaskRegister::external_07, InterruptStatusRegister::external_07 };
static const Interrupt external_08       { vector_external_08,       0x1 , InterruptMaskRegister::external_08, InterruptStatusRegister::external_08 };
static const Interrupt external_09       { vector_external_09,       0x1 , InterruptMaskRegister::external_09, InterruptStatusRegister::external_09 };
static const Interrupt external_10       { vector_external_10,       0x1 , InterruptMaskRegister::external_10, InterruptStatusRegister::external_10 };
static const Interrupt external_11       { vector_external_11,       0x1 , InterruptMaskRegister::external_11, InterruptStatusRegister::external_11 };
static const Interrupt external_12       { vector_external_12,       0x1 , InterruptMaskRegister::external_12, InterruptStatusRegister::external_12 };
static const Interrupt external_13       { vector_external_13,       0x1 , InterruptMaskRegister::external_13, InterruptStatusRegister::external_13 };
static const Interrupt external_14       { vector_external_14,       0x1 , InterruptMaskRegister::external_14, InterruptStatusRegister::external_14 };
static const Interrupt external_15       { vector_external_15,       0x1 , InterruptMaskRegister::external_15, InterruptStatusRegister::external_15 };
static Interrupt frt_input_capture       { vector_frt_input_capture, 0x1 , InterruptMaskRegister::undefined, InterruptStatusRegister::undefined };
//constexpr Interrupt frt_input_capture_signal  { 0x65, 0xF }; // Vector                 0x65 was registered in previous version of Saturnin, can't remind its purpose

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