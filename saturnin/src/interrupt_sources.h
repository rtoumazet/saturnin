//
// interrupt_sources.h
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
/// \file	interrupt_sources.h
///
/// \brief	Lists the various interrupt sources used in the system.
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <saturnin/src/emulator_defs.h>        // u8
#include <saturnin/src/interrupt_controller.h> // Interrupt
#include <saturnin/src/scu_registers.h>        //InterruptMaskRegister, InterruptStatusRegister

namespace saturnin::core::interrupt_source {

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \namespace  InterruptSource
///
/// \brief  Regroups all the interrupt sources that can be sent by the system.
////////////////////////////////////////////////////////////////////////////////////////////////////

// clang-format off
static constexpr auto vector_nmi               =u8{ 0xB };
static constexpr auto vector_user_break        =u8{ 0xC };
static constexpr auto vector_v_blank_in        =u8{ 0x40 };
static constexpr auto vector_v_blank_in_slave  =u8{ 0x43 };
static constexpr auto vector_v_blank_out       =u8{ 0x41 };
static constexpr auto vector_h_blank_in        =u8{ 0x42 };
static constexpr auto vector_h_blank_in_slave  =u8{ 0x41 };
static constexpr auto vector_timer_0           =u8{ 0x43 };
static constexpr auto vector_timer_1           =u8{ 0x44 };
static constexpr auto vector_dsp_end           =u8{ 0x45 };
static constexpr auto vector_sound_request     =u8{ 0x46 };
static constexpr auto vector_system_manager    =u8{ 0x47 };
static constexpr auto vector_pad_interrupt     =u8{ 0x48 };
static constexpr auto vector_level_2_dma_end   =u8{ 0x49 };
static constexpr auto vector_level_1_dma_end   =u8{ 0x4A };
static constexpr auto vector_level_0_dma_end   =u8{ 0x4B };
static constexpr auto vector_dma_illegal       =u8{ 0x4C };
static constexpr auto vector_sprite_draw_end   =u8{ 0x4D };
static constexpr auto vector_external_00       =u8{ 0x50 };
static constexpr auto vector_external_01       =u8{ 0x51 };
static constexpr auto vector_external_02       =u8{ 0x52 };
static constexpr auto vector_external_03       =u8{ 0x53 };
static constexpr auto vector_external_04       =u8{ 0x54 };
static constexpr auto vector_external_05       =u8{ 0x55 };
static constexpr auto vector_external_06       =u8{ 0x56 };
static constexpr auto vector_external_07       =u8{ 0x57 };
static constexpr auto vector_external_08       =u8{ 0x58 };
static constexpr auto vector_external_09       =u8{ 0x59 };
static constexpr auto vector_external_10       =u8{ 0x5A };
static constexpr auto vector_external_11       =u8{ 0x5B };
static constexpr auto vector_external_12       =u8{ 0x5C };
static constexpr auto vector_external_13       =u8{ 0x5D };
static constexpr auto vector_external_14       =u8{ 0x5E };
static constexpr auto vector_external_15       =u8{ 0x5F };
static constexpr auto vector_frt_input_capture =u8{ 0x64 };
static constexpr auto vector_frt_input_capture2=u8{ 0x65 };

using Ist = ScuRegs::Ist;
using Ims = ScuRegs::Ims;

static const auto undefined         =Interrupt{ 0,                        0,    Ims::undefined_enum, Ist::undefined_enum, "Undefined" };
static const auto nmi               =Interrupt{ vector_nmi,               0xF, Ims::undefined_enum, Ist::undefined_enum, "NMI" };
static const auto user_break        =Interrupt{ vector_user_break,        0xF , Ims::undefined_enum, Ist::undefined_enum, "User break" };
static const auto v_blank_in        =Interrupt{ vector_v_blank_in,        0xF , Ims::v_blank_in_enum, Ist::v_blank_in_enum, "V-Blank-IN" };
static const auto v_blank_in_slave  =Interrupt{ vector_v_blank_in_slave,  0x6 , Ims::undefined_enum, Ist::undefined_enum, "V-Blank-IN (slave)" };
static const auto v_blank_out       =Interrupt{ vector_v_blank_out,       0xE , Ims::v_blank_out_enum, Ist::v_blank_out_enum, "V-Blank-OUT" };
static const auto h_blank_in        =Interrupt{ vector_h_blank_in,        0xD , Ims::h_blank_in_enum, Ist::h_blank_in_enum, "H-Blank-IN" };
static const auto h_blank_in_slave  =Interrupt{ vector_h_blank_in_slave,  0x2 , Ims::undefined_enum, Ist::undefined_enum, "H-Blank-IN (slave)" };
static const auto timer_0           =Interrupt{ vector_timer_0,           0xC , Ims::timer_0_enum, Ist::timer_0_enum, "Timer 0" };
static const auto timer_1           =Interrupt{ vector_timer_1,           0xB , Ims::timer_1_enum, Ist::timer_1_enum, "Timer 1" };
static const auto dsp_end           =Interrupt{ vector_dsp_end,           0xA , Ims::dsp_end_enum, Ist::dsp_end_enum, "DSP End" };
static const auto sound_request     =Interrupt{ vector_sound_request,     0x9 , Ims::sound_request_enum, Ist::sound_request_enum, "Sound Request"};
static const auto system_manager    =Interrupt{ vector_system_manager,    0x8 , Ims::system_manager_enum, Ist::system_manager_enum, "System Manager" };
static const auto pad_interrupt     =Interrupt{ vector_pad_interrupt,     0x8 , Ims::pad_enum, Ist::pad_enum, "PAD Interrupt" };
static const auto level_2_dma_end   =Interrupt{ vector_level_2_dma_end,   0x6 , Ims::level_2_dma_enum, Ist::level_2_dma_enum, "Level-2 DMA End" };
static const auto level_1_dma_end   =Interrupt{ vector_level_1_dma_end,   0x6 , Ims::level_1_dma_enum, Ist::level_1_dma_enum, "Level-1 DMA End"};
static const auto level_0_dma_end   =Interrupt{ vector_level_0_dma_end,   0x5 , Ims::level_0_dma_enum, Ist::level_0_dma_enum, "Level-0 DMA End" };
static const auto dma_illegal       =Interrupt{ vector_dma_illegal,       0x3 , Ims::dma_illegal_enum, Ist::dma_illegal_enum, "DMA Illegal"};
static const auto sprite_draw_end   =Interrupt{ vector_sprite_draw_end,   0x2 , Ims::sprite_draw_end_enum, Ist::sprite_draw_end_enum, "Sprite Draw End" };
static const auto external_00       =Interrupt{ vector_external_00,       0x7 , Ims::external_00_enum, Ist::external_00_enum, "External Interrupt 00" };
static const auto external_01       =Interrupt{ vector_external_01,       0x7 , Ims::external_01_enum, Ist::external_01_enum, "External Interrupt 01" };
static const auto external_02       =Interrupt{ vector_external_02,       0x7 , Ims::external_02_enum, Ist::external_02_enum, "External Interrupt 02" };
static const auto external_03       =Interrupt{ vector_external_03,       0x7 , Ims::external_03_enum, Ist::external_03_enum, "External Interrupt 03" };
static const auto external_04       =Interrupt{ vector_external_04,       0x4 , Ims::external_04_enum, Ist::external_04_enum, "External Interrupt 04" };
static const auto external_05       =Interrupt{ vector_external_05,       0x4 , Ims::external_05_enum, Ist::external_05_enum, "External Interrupt 05" };
static const auto external_06       =Interrupt{ vector_external_06,       0x4 , Ims::external_06_enum, Ist::external_06_enum, "External Interrupt 06" };
static const auto external_07       =Interrupt{ vector_external_07,       0x4 , Ims::external_07_enum, Ist::external_07_enum, "External Interrupt 07" };
static const auto external_08       =Interrupt{ vector_external_08,       0x1 , Ims::external_08_enum, Ist::external_08_enum, "External Interrupt 08" };
static const auto external_09       =Interrupt{ vector_external_09,       0x1 , Ims::external_09_enum, Ist::external_09_enum, "External Interrupt 09" };
static const auto external_10       =Interrupt{ vector_external_10,       0x1 , Ims::external_10_enum, Ist::external_10_enum, "External Interrupt 10" };
static const auto external_11       =Interrupt{ vector_external_11,       0x1 , Ims::external_11_enum, Ist::external_11_enum, "External Interrupt 11" };
static const auto external_12       =Interrupt{ vector_external_12,       0x1 , Ims::external_12_enum, Ist::external_12_enum, "External Interrupt 12" };
static const auto external_13       =Interrupt{ vector_external_13,       0x1 , Ims::external_13_enum, Ist::external_13_enum, "External Interrupt 13" };
static const auto external_14       =Interrupt{ vector_external_14,       0x1 , Ims::external_14_enum, Ist::external_14_enum, "External Interrupt 14" };
static const auto external_15       =Interrupt{ vector_external_15,       0x1 , Ims::external_15_enum, Ist::external_15_enum, "External Interrupt 15" };

//constexpr Interrupt frt_input_capture_signal  { 0x65, 0xF }; // Vector                 0x65 was registered in previous version of Saturnin, can't remind its purpose

// Following interrupts are dynamically set
static auto sh2_division_overflow            =Interrupt{ 0, 0 , Ims::undefined_enum, Ist::undefined_enum };
static auto sh2_dma_0_transfer_end           =Interrupt{ 0, 0 , Ims::undefined_enum, Ist::undefined_enum };
static auto sh2_dma_1_transfer_end           =Interrupt{ 0, 0 , Ims::undefined_enum, Ist::undefined_enum };
static auto sh2_wdt_overflow                 =Interrupt{ 0, 0 , Ims::undefined_enum, Ist::undefined_enum };
static auto sh2_bsc_values_matched           =Interrupt{ 0, 0 , Ims::undefined_enum, Ist::undefined_enum };
static auto sh2_sci_transmit_end             =Interrupt{ 0, 0 , Ims::undefined_enum, Ist::undefined_enum };
static auto sh2_sci_receive_error            =Interrupt{ 0, 0 , Ims::undefined_enum, Ist::undefined_enum };
static auto sh2_sci_receive_data_full        =Interrupt{ 0, 0 , Ims::undefined_enum, Ist::undefined_enum };
static auto sh2_sci_transmit_data_empty      =Interrupt{ 0, 0 , Ims::undefined_enum, Ist::undefined_enum };
static auto sh2_frt_input_capture            =Interrupt{ vector_frt_input_capture, 0x1 , Ims::undefined_enum, Ist::undefined_enum };
static auto sh2_frt_output_compare_flag_a_set=Interrupt{ 0, 0 , Ims::undefined_enum, Ist::undefined_enum };
static auto sh2_frt_output_compare_flag_b_set=Interrupt{ 0, 0 , Ims::undefined_enum, Ist::undefined_enum };
static auto sh2_frt_overflow_flag_set        =Interrupt{ 0, 0 , Ims::undefined_enum, Ist::undefined_enum };

// clang-format on
} // namespace saturnin::core::interrupt_source
