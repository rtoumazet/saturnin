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
#include <saturnin/src/bit_register.h>

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

struct Sh2Regs {
    struct StatusRegister {
        GENERATE_USING(StatusRegister, u32);

        static constexpr auto t_pos = PosType(0);
        static constexpr auto s_pos = PosType(1);
        static constexpr auto i_pos = PosType(4);
        static constexpr auto q_pos = PosType(8);
        static constexpr auto m_pos = PosType(9);

        static constexpr auto t = BitsType(1, t_pos);
        static constexpr auto s = BitsType(1, s_pos);
        static constexpr auto q = BitsType(1, q_pos);
        static constexpr auto m = BitsType(1, m_pos);

        static constexpr auto one_bit_mask = 0x01;
        static constexpr auto t_shft       = ShiftedType(one_bit_mask, t_pos);
        static constexpr auto s_shft       = ShiftedType(one_bit_mask, s_pos);
        static constexpr auto q_shft       = ShiftedType(one_bit_mask, q_pos);
        static constexpr auto m_shft       = ShiftedType(one_bit_mask, m_pos);

        static constexpr auto i_mask          = 0xF;
        static constexpr auto i_default_value = MaskedType(i_mask, 0b1111, i_pos);

        static constexpr auto i_shft = ShiftedType(i_mask, i_pos);
        GENERATE_MASKED_RANGE("Sh2Regs::StatusRegister", INTERRUPT_MASK, interruptMask, i_mask, i_pos, i_mask);
    };
    using StatusRegisterType = Reg<u32, StatusRegister>;
    StatusRegisterType sr;

    /////////////////////////////////////
    // 5. Interrupt Controller (INTC)  //
    /////////////////////////////////////

    struct Intc {
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \struct	Ipra
        ///
        /// \brief	Interrupt Priority Level Setting Register A (IPRA).
        ///
        /// \author	Runik
        /// \date	25/03/2023
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        struct Ipra {
            GENERATE_USING(Ipra, u16);

            static constexpr auto wdt_level_pos  = PosType(4);  ///< Defines WDT/DRAM refresh priority level.
            static constexpr auto dmac_level_pos = PosType(8);  ///< Defines DMAC0/DMAC1 priority level.
            static constexpr auto divu_level_pos = PosType(12); ///< Defines DIVU refresh priority level.
            static constexpr auto lo_byte_pos    = PosType(0);  ///< Defines the range of the upper 8 bits of the register.
            static constexpr auto hi_byte_pos    = PosType(8);  ///< Defines the range of the lower 8 bits of the register.

            static constexpr auto wdt_level_mask = 0xF;
            GENERATE_MASKED_RANGE("Sh2Regs::Intc::Ipra", WDT_LEVEL, wdtLevel, wdt_level_mask, wdt_level_pos, wdt_level_mask);

            static constexpr auto dmac_level_mask = 0xF;
            GENERATE_MASKED_RANGE("Sh2Regs::Intc::Ipra", DMAC_LEVEL, dmacLevel, dmac_level_mask, dmac_level_pos, dmac_level_mask);

            static constexpr auto divu_level_mask = 0xF;
            GENERATE_MASKED_RANGE("Sh2Regs::Intc::Ipra", DIVU_LEVEL, divuLevel, divu_level_mask, divu_level_pos, divu_level_mask);

            static constexpr auto byte_mask = 0xFF;
            GENERATE_MASKED_RANGE("Sh2Regs::Intc::Ipra", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
            GENERATE_MASKED_RANGE("Sh2Regs::Intc::Ipra", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
        };
        using IpraType = Reg<u16, Ipra>;
        IpraType ipra;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \struct	Iprb
        ///
        /// \brief	Interrupt Priority Level Setting Register B (IPRB).
        ///
        /// \author	Runik
        /// \date	26/03/2023
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        struct Iprb {
            GENERATE_USING(Iprb, u16);

            static constexpr auto frt_level_pos = PosType(8);  ///< Defines FRT priority level.
            static constexpr auto sci_level_pos = PosType(12); ///< Defines SCI priority level.
            static constexpr auto lo_byte_pos   = PosType(0);  ///< Defines the range of the upper 8 bits of the register.
            static constexpr auto hi_byte_pos   = PosType(8);  ///< Defines the range of the lower 8 bits of the register.

            static constexpr auto frt_level_mask = 0xF;
            GENERATE_MASKED_RANGE("Sh2Regs::Intc::Iprb", FRT_LEVEL, frtLevel, frt_level_mask, frt_level_pos, frt_level_mask);

            static constexpr auto sci_level_mask = 0xF;
            GENERATE_MASKED_RANGE("Sh2Regs::Intc::Iprb", SCI_LEVEL, sciLevel, sci_level_mask, sci_level_pos, sci_level_mask);

            static constexpr auto byte_mask = 0xFF;
            GENERATE_MASKED_RANGE("Sh2Regs::Intc::Iprb", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
            GENERATE_MASKED_RANGE("Sh2Regs::Intc::Iprb", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
        };
        using IprbType = Reg<u16, Iprb>;
        IprbType iprb;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \struct	Vcra
        ///
        /// \brief	Vector Number Setting Register A (VCRA).
        ///
        /// \author	Runik
        /// \date	26/03/2023
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        struct Vcra {
            GENERATE_USING(Vcra, u16);

            static constexpr auto srxv_pos    = PosType(0); ///< Defines SCI receive data full interrupt vector number.
            static constexpr auto serv_pos    = PosType(8); ///< Defines SCI receive error interrupt vector number.
            static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
            static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

            static constexpr auto srxv_mask = 0x7F;
            GENERATE_MASKED_RANGE("Sh2Regs::Intc::Vcra", SRXV, srxv, srxv_mask, srxv_pos, srxv_mask);

            static constexpr auto serv_mask = 0x7F;
            GENERATE_MASKED_RANGE("Sh2Regs::Intc::Vcra", SERV, serv, serv_mask, serv_pos, serv_mask);

            static constexpr auto byte_mask = 0xFF;
            GENERATE_MASKED_RANGE("Sh2Regs::Intc::Vcra", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
            GENERATE_MASKED_RANGE("Sh2Regs::Intc::Vcra", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
        };
        using VcraType = Reg<u16, Vcra>;
        VcraType vcra;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \struct	Vcrb
        ///
        /// \brief	Vector Number Setting Register B (VCRB).
        ///
        /// \author	Runik
        /// \date	26/03/2023
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        struct Vcrb {
            GENERATE_USING(Vcrb, u16);

            static constexpr auto stev_pos    = PosType(0); ///< Defines SCI transmit end interrupt vector number.
            static constexpr auto stxv_pos    = PosType(8); ///< Defines SCI transmit data empty interrupt vector number.
            static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
            static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

            static constexpr auto stev_mask = 0x7F;
            GENERATE_MASKED_RANGE("Sh2Regs::Intc::Vcrb", STEV, stev, stev_mask, stev_pos, stev_mask);

            static constexpr auto stxv_mask = 0x7F;
            GENERATE_MASKED_RANGE("Sh2Regs::Intc::Vcrb", STXV, stxv, stxv_mask, stxv_pos, stxv_mask);

            static constexpr auto byte_mask = 0xFF;
            GENERATE_MASKED_RANGE("Sh2Regs::Intc::Vcrb", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
            GENERATE_MASKED_RANGE("Sh2Regs::Intc::Vcrb", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
        };
        using VcrbType = Reg<u16, Vcrb>;
        VcrbType vcrb;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \struct	Vcrc
        ///
        /// \brief	Vector Number Setting Register C (VCRC).
        ///
        /// \author	Runik
        /// \date	26/03/2023
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        struct Vcrc {
            GENERATE_USING(Vcrc, u16);

            static constexpr auto focv_pos    = PosType(0); ///< Defines FRT output compare interrupt vector number.
            static constexpr auto ficv_pos    = PosType(8); ///< Defines FRT input capture interrupt vector number.
            static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
            static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

            static constexpr auto focv_mask = 0x7F;
            GENERATE_MASKED_RANGE("Sh2Regs::Intc::Vcrc", FOCV, focv, focv_mask, focv_pos, focv_mask);

            static constexpr auto ficv_mask = 0x7F;
            GENERATE_MASKED_RANGE("Sh2Regs::Intc::Vcrc", FICV, ficv, ficv_mask, ficv_pos, ficv_mask);

            static constexpr auto byte_mask = 0xFF;
            GENERATE_MASKED_RANGE("Sh2Regs::Intc::Vcrc", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
            GENERATE_MASKED_RANGE("Sh2Regs::Intc::Vcrc", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
        };
        using VcrcType = Reg<u16, Vcrc>;
        VcrcType vcrc;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \struct	Vcrd
        ///
        /// \brief	Vector Number Setting Register D (VCRD).
        ///
        /// \author	Runik
        /// \date	26/03/2023
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        struct Vcrd {
            GENERATE_USING(Vcrd, u16);

            static constexpr auto fovv_pos    = PosType(8); ///< Defines FRT overflow interrupt vector number.
            static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
            static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

            static constexpr auto fovv_mask = 0x7F;
            GENERATE_MASKED_RANGE("Sh2Regs::Intc::Vcrd", FOVV, fovv, fovv_mask, fovv_pos, fovv_mask);

            static constexpr auto byte_mask = 0xFF;
            GENERATE_MASKED_RANGE("Sh2Regs::Intc::Vcrd", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
            GENERATE_MASKED_RANGE("Sh2Regs::Intc::Vcrd", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
        };
        using VcrdType = Reg<u16, Vcrd>;
        VcrdType vcrd;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \struct	Vcrwdt
        ///
        /// \brief	Vector Number Setting Register WDT (VCRWDT).
        ///
        /// \author	Runik
        /// \date	26/03/2023
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        struct Vcrwdt {
            GENERATE_USING(Vcrwdt, u16);

            static constexpr auto bcmv_pos    = PosType(0); ///< Defines BSC compare match interrupt vector number.
            static constexpr auto witv_pos    = PosType(8); ///< Defines WDT interval interrupt vector number.
            static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
            static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

            static constexpr auto bcmv_mask = 0x7F;
            GENERATE_MASKED_RANGE("Sh2Regs::Intc::Vcrwdt", BCMV, bcmv, bcmv_mask, bcmv_pos, bcmv_mask);

            static constexpr auto witv_mask = 0x7F;
            GENERATE_MASKED_RANGE("Sh2Regs::Intc::Vcrwdt", WITV, witv, witv_mask, witv_pos, witv_mask);

            static constexpr auto byte_mask = 0xFF;
            GENERATE_MASKED_RANGE("Sh2Regs::Intc::Vcrwdt", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
            GENERATE_MASKED_RANGE("Sh2Regs::Intc::Vcrwdt", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
        };
        using VcrwdtType = Reg<u16, Vcrwdt>;
        VcrwdtType vcrwdt;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \struct	Vcrdiv
        ///
        /// \brief	Vector Number Setting Register Div (VCRDIV).
        ///
        /// \author	Runik
        /// \date	26/03/2023
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        struct Vcrdiv {
            using PosType     = Pos<u32, Vcrdiv>;
            using BitsType    = Bits<u32, Vcrdiv>;
            using MaskedType  = Masked<u32, Vcrdiv>;
            using ShiftedType = Shifted<u32, Vcrdiv>;
            GENERATE_USING(Vcrdiv, u32);

            static constexpr auto divuv_pos   = PosType(0);  ///< Defines DIVU interrupt vector number.
            static constexpr auto lo_word_pos = PosType(0);  ///< Defines the range of the upper 16 bits of the register.
            static constexpr auto hi_word_pos = PosType(16); ///< Defines the range of the lower 16 bits of the register.

            static constexpr auto divuv_mask = 0x7F;
            GENERATE_MASKED_RANGE("Sh2Regs::Intc::Vcrdiv", DIVUV, divuv, divuv_mask, divuv_pos, divuv_mask);

            static constexpr auto word_mask = 0xFFFF;
            GENERATE_MASKED_RANGE("Sh2Regs::Intc::Vcrdiv", LO_WORD, loWord, word_mask, lo_word_pos, word_mask);
            GENERATE_MASKED_RANGE("Sh2Regs::Intc::Vcrdiv", HI_WORD, hiWord, word_mask, hi_word_pos, word_mask);
        };
        using VcrdivType = Reg<u32, Vcrdiv>;
        VcrdivType vcrdiv;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \struct	Vcrdma0
        ///
        /// \brief	Vector Number Setting Register DMA (VCRDMAx).
        ///
        /// \author	Runik
        /// \date	26/03/2023
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        struct Vcrdma {
            GENERATE_USING(Vcrdma, u32);

            static constexpr auto vc_pos      = PosType(0);  ///< Defines DMACx transfer end interrupt vector number.
            static constexpr auto lo_word_pos = PosType(0);  ///< Defines the range of the upper 16 bits of the register.
            static constexpr auto hi_word_pos = PosType(16); ///< Defines the range of the lower 16 bits of the register.

            static constexpr auto vc_mask = 0xFF;
            GENERATE_MASKED_RANGE("Sh2Regs::Intc::Vcrdma", VC, vc, vc_mask, vc_pos, vc_mask);

            static constexpr auto word_mask = 0xFFFF;
            GENERATE_MASKED_RANGE("Sh2Regs::Intc::Vcrdma", LO_WORD, loWord, word_mask, lo_word_pos, word_mask);
            GENERATE_MASKED_RANGE("Sh2Regs::Intc::Vcrdma", HI_WORD, hiWord, word_mask, hi_word_pos, word_mask);
        };
        using VcrdmaType = Reg<u32, Vcrdma>;
        VcrdmaType vcrdma0;
        VcrdmaType vcrdma1;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \struct	Icr
        ///
        /// \brief	Interrupt Control Register (ICR).
        ///
        /// \author	Runik
        /// \date	26/03/2023
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        struct Icr {
            GENERATE_USING(Icr, u16);

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// \enum   NmiInputLevel
            ///
            /// \brief  ICR - NMIL bit values.
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            enum class NmiInputLevel : bool {
                low  = false, ///< NMI input level is low
                high = true   ///< NMI input level is high
            };

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// \enum   NmiEdgeDetection
            ///
            /// \brief  ICR - NMIE bit values.
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            enum class NmiEdgeDetection : bool {
                falling = false, ///< Interrupt request detected on falling edge of NMI input (initial)
                rising  = true   ///< Interrupt request detected on rising edge of NMI input
            };

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// \enum   VectorMode
            ///
            /// \brief  ICR - VECMD bit values.
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            enum class VectorMode : bool {
                auto_vector     = false, ///< Auto vector mode (initial)
                external_vector = true   ///< External vector mode
            };

            static constexpr auto lo_byte_pos = PosType(0);         ///< Defines the range of the upper 8 bits of the register.
            static constexpr auto hi_byte_pos = PosType(8);         ///< Defines the range of the lower 8 bits of the register.

            GENERATE_BIT_WITH_ENUM(vecmd, 0, 0b1, VectorMode);      ///< Vector mode.
            GENERATE_BIT_WITH_ENUM(nmie, 8, 0b1, NmiEdgeDetection); ///< NMI edge detection.
            GENERATE_BIT_WITH_ENUM(nmil, 15, 0b1, NmiInputLevel);   ///< NMI input level.

            static constexpr auto byte_mask = 0xFF;
            GENERATE_MASKED_RANGE("Sh2Regs::Intc::Icr", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
            GENERATE_MASKED_RANGE("Sh2Regs::Intc::Icr", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
        };
        using IcrType = Reg<u16, Icr>;
        IcrType icr;
    };
    Intc intc;

    //////////////////////////////////
    // 7. Bus State Controler (BSC) //
    //////////////////////////////////
    struct Bsc {
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \struct	Bcr
        ///
        /// \brief	Bus Control Register (BCR1 and BCR2).
        ///
        /// \author	Runik
        /// \date	27/03/2023
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        struct Bcr {
            GENERATE_USING(Bcr, u32);

            static constexpr auto lo_word_pos = PosType(0); ///< Defines the range of the upper 16 bits of the register.

            static constexpr auto word_mask = 0xFFFF;
            GENERATE_MASKED_RANGE("Sh2Regs::Bsc::Bcr", LO_WORD, loWord, word_mask, lo_word_pos, word_mask);
        };
        using BcrType = Reg<u32, Bcr>;
        BcrType bcr1;
        BcrType bcr2;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \struct	Wcr
        ///
        /// \brief	Wait Control Register (WCR).
        ///
        /// \author	Runik
        /// \date	27/03/2023
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        struct Wcr {
            GENERATE_USING(Wcr, u32);

            static constexpr auto lo_word_pos = PosType(0); ///< Defines the range of the upper 16 bits of the register.

            static constexpr auto word_mask = 0xFFFF;
            GENERATE_MASKED_RANGE("Sh2Regs::Bsc::Wcr", LO_WORD, loWord, word_mask, lo_word_pos, word_mask);
        };
        using WcrType = Reg<u32, Wcr>;
        WcrType wcr;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \struct	Mcr
        ///
        /// \brief	Individual Memory Control Register (MCR).
        ///
        /// \author	Runik
        /// \date	27/03/2023
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        struct Mcr {
            GENERATE_USING(Mcr, u32);

            static constexpr auto lo_word_pos = PosType(0); ///< Defines the range of the upper 16 bits of the register.

            static constexpr auto word_mask = 0xFFFF;
            GENERATE_MASKED_RANGE("Sh2Regs::Bsc::Mcr", LO_WORD, loWord, word_mask, lo_word_pos, word_mask);
        };
        using McrType = Reg<u32, Mcr>;
        McrType mcr;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \struct	Rtcsr
        ///
        /// \brief	Refresh Time Control/Status Register (RTCSR).
        ///
        /// \author	Runik
        /// \date	27/03/2023
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        struct Rtcsr {
            GENERATE_USING(Rtcsr, u32);

            static constexpr auto lo_word_pos = PosType(0); ///< Defines the range of the upper 16 bits of the register.

            static constexpr auto word_mask = 0xFFFF;
            GENERATE_MASKED_RANGE("Sh2Regs::Bsc::Rtcsr", LO_WORD, loWord, word_mask, lo_word_pos, word_mask);
        };
        using RtcsrType = Reg<u32, Rtcsr>;
        RtcsrType rtcsr;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \struct	Rtcnt
        ///
        /// \brief	Refresh Timer Counter (RTCNT).
        ///
        /// \author	Runik
        /// \date	27/03/2023
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        struct Rtcnt {
            GENERATE_USING(Rtcnt, u32);

            static constexpr auto lo_word_pos = PosType(0); ///< Defines the range of the upper 16 bits of the register.

            static constexpr auto word_mask = 0xFFFF;
            GENERATE_MASKED_RANGE("Sh2Regs::Bsc::Rtcnt", LO_WORD, loWord, word_mask, lo_word_pos, word_mask);
        };
        using RtcntType = Reg<u32, Rtcnt>;
        RtcntType rtcnt;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \struct	Rtcor
        ///
        /// \brief	Refresh Timer Constant Register (RTCOR).
        ///
        /// \author	Runik
        /// \date	27/03/2023
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        struct Rtcor {
            GENERATE_USING(Rtcor, u32);

            static constexpr auto lo_word_pos = PosType(0); ///< Defines the range of the upper 16 bits of the register.

            static constexpr auto word_mask = 0xFFFF;
            GENERATE_MASKED_RANGE("Sh2Regs::Bsc::Rtcor", LO_WORD, loWord, word_mask, lo_word_pos, word_mask);
        };
        using RtcorType = Reg<u32, Rtcor>;
        RtcorType rtcor;
    };
    Bsc bsc;

    //////////////
    // 8. Cache //
    //////////////
    struct Cache {
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \struct	Ccr
        ///
        /// \brief	Cache control register (CCR).
        ///
        /// \author	Runik
        /// \date	28/03/2023
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        struct Ccr {
            GENERATE_USING(Ccr, u8);

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
                normal_operation = false, ///< Normal operation.
                cache_purge      = true   ///< Cache purge.
            };

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// \enum   TwoWayMode
            ///
            /// \brief  CCR - TW bit values.
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            enum class TwoWayMode : bool {
                four_way = false, ///< Four way mode (initial).
                two_way  = true   ///< Two way mode.
            };

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// \enum   DataReplacementDisable
            ///
            /// \brief  CCR - OD bit values.
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            enum class DataReplacementDisable : bool {
                normal_operation  = false, ///< Normal operation (initial).
                data_not_replaced = true   ///< Data not replaced even when wache miss occurs in data access.
            };

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// \enum   InstructionReplacementDisable
            ///
            /// \brief  CCR - ID bit values.
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            enum class InstructionReplacementDisable : bool {
                normal_operation  = false, ///< Normal operation (initial).
                data_not_replaced = true   ///< Data not replaced even when wache miss occurs in instruction fetch.
            };

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// \enum   CacheEnable
            ///
            /// \brief  CCR - CE bit values.
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            enum class CacheEnable : bool {
                cache_disabled = false, ///< Cache disabled (initial).
                cache_enabled  = true   ///< Cache enabled.
            };

            GENERATE_BIT_WITH_ENUM(ce, 0, 0b1, CacheEnable);                   ///< Cache enable.
            GENERATE_BIT_WITH_ENUM(id, 1, 0b1, InstructionReplacementDisable); ///< Instruction replacement disable.
            GENERATE_BIT_WITH_ENUM(od, 2, 0b1, DataReplacementDisable);        ///< Data replacement disable.
            GENERATE_BIT_WITH_ENUM(tw, 3, 0b1, TwoWayMode);                    ///< Two way mode.
            GENERATE_BIT_WITH_ENUM(cp, 4, 0b1, CachePurge);                    ///< Cache purge.
            GENERATE_BIT_WITH_ENUM(wx, 6, 0b11, WaySpecification);             ///< Way specification.
        };
        using CcrType = Reg<u8, Ccr>;
        CcrType ccr;
    };
    Cache cache;

    //////////////////////////////////////////////
    // 9. Direct Memory Access Controler (DMAC) //
    //////////////////////////////////////////////
    struct Dmac {
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \struct	Sar
        ///
        /// \brief	Dma Source Address Register (SAR0,SAR1).
        ///
        /// \author	Runik
        /// \date	28/03/2023
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        struct Sar {};
        using SarType = Reg<u32, Sar>;
        SarType sar0;
        SarType sar1;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \struct	Dar
        ///
        /// \brief	Dma Destination Address Register (DAR0/DAR1).
        ///
        /// \author	Runik
        /// \date	28/03/2023
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        struct Dar {};
        using DarType = Reg<u32, Dar>;
        DarType dar0;
        DarType dar1;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \struct	Tcr
        ///
        /// \brief	Dma Transfer Count Register (TCR0 / TCR1).
        ///
        /// \author	Runik
        /// \date	28/03/2023
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        struct Tcr {};
        using TcrType = Reg<u32, Tcr>;
        TcrType tcr0;
        TcrType tcr1;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \struct	Chcr
        ///
        /// \brief	DMA Channel Control Register (CHCR0/CHCR1).
        ///
        /// \author	Runik
        /// \date	28/03/2023
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        struct Chcr {
            GENERATE_USING(Chcr, u32);

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// \enum   DestinationAddressMode
            ///
            /// \brief  CHCR - DMx bit values.
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
            /// \brief  CHCR - SMx bit values.
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
            /// \brief  CHCR - TSx bit values.
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
            /// \brief  CHCR - AR bit value.
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            enum class AutoRequestMode : bool {
                module_request = false, ///< Module request mode (initial).
                auto_request   = true   ///< Auto request mode.
            };

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// \enum   AcknowledgeMode
            ///
            /// \brief  CHCR - AM bit value.
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            enum class AcknowledgeMode : bool {
                output_read  = false, ///< DACK output in read cycle/transfer from memory to device (initial).
                output_write = true   ///< DACK output in write cycle/transfer from device to memory.
            };

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// \enum   AcknowledgeLevel
            ///
            /// \brief  CHCR - AL bit value.
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            enum class AcknowledgeLevel : bool {
                active_low  = false, ///< DACK signal is active low (initial).
                active_high = true   ///< DACK signal is active high.
            };

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// \enum   DreqSelect
            ///
            /// \brief  CHCR - DS bit value.
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            enum class DreqSelect : bool {
                by_level = false, ///< Detected by level (initial).
                by_edge  = true   ///< Detected by edge.
            };

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// \enum   DreqLevel
            ///
            /// \brief  CHCR - DL bit value.
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            enum class DreqLevel : bool {
                low_level_or_fall  = false, ///< DREQ detected by low level if 0, by fall if 1 (initial).
                high_level_or_rise = true   ///< DREQ detected by high level if 0, by rise if 1.
            };

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// \enum   TransferBusMode
            ///
            /// \brief  CHCR - TB bit value.
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            enum class TransferBusMode : bool {
                cycle_steal = false, ///< Cycle steal mode (initial).
                burst       = true   ///< Burst mode.
            };

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// \enum   TransferAddressMode
            ///
            /// \brief  CHCR - TA bit value.
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            enum class TransferAddressMode : bool {
                dual_address   = false, ///< Dual address mode (initial).
                single_address = true   ///< Single address mode.
            };

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// \enum   InterruptEnable
            ///
            /// \brief  CHCR - IE bit values.
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            enum class Sh2DmaInterruptEnable : bool {
                disabled = false, ///< Interrupt disabled.
                enabled  = true   ///< Interrupt enabled.
            };

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// \enum   TransferEndFlag
            ///
            /// \brief  CHCR - TE bit values.
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            enum class TransferEndFlag : bool {
                dma_not_ended_or_aborted = false, ///< DMA has not ended or was aborted (initial).
                dma_ended_normally       = true   ///< DMA has ended normally.
            };

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// \enum   DmaEnable
            ///
            /// \brief  CHCR - DE bit values.
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            enum class Sh2DmaEnable : bool {
                dma_transfer_disabled = false, ///< DMA transfer disabled (initial).
                dma_transfer_enabled  = true   ///< DMA transfer enabled.
            };

            GENERATE_BIT_WITH_ENUM(de, 0, 0b1, Sh2DmaEnable);             ///< DMA enable.
            GENERATE_BIT_WITH_ENUM(te, 1, 0b1, TransferEndFlag);          ///< Transfer end flag.
            GENERATE_BIT_WITH_ENUM(ie, 2, 0b1, Sh2DmaInterruptEnable);    ///< Interrupt enable.
            GENERATE_BIT_WITH_ENUM(ta, 3, 0b1, TransferAddressMode);      ///< Transfer address mode.
            GENERATE_BIT_WITH_ENUM(tb, 4, 0b1, TransferBusMode);          ///< Transfer bus mode.
            GENERATE_BIT_WITH_ENUM(dl, 5, 0b1, DreqLevel);                ///< Dreq level.
            GENERATE_BIT_WITH_ENUM(ds, 6, 0b1, DreqSelect);               ///< Dreq select.
            GENERATE_BIT_WITH_ENUM(al, 7, 0b1, AcknowledgeLevel);         ///< Acknowledge level.
            GENERATE_BIT_WITH_ENUM(am, 8, 0b1, AcknowledgeMode);          ///< Acknowledge mode.
            GENERATE_BIT_WITH_ENUM(ar, 9, 0b1, AutoRequestMode);          ///< Auto request mode.
            GENERATE_BIT_WITH_ENUM(ts, 10, 0b11, TransferSize);           ///< Transfer size.
            GENERATE_BIT_WITH_ENUM(sm, 12, 0b11, SourceAddressMode);      ///< Source address mode.
            GENERATE_BIT_WITH_ENUM(dm, 14, 0b11, DestinationAddressMode); ///< Destination address mode.
        };
        using ChcrType = Reg<u32, Chcr>;
        ChcrType chcr0;
        ChcrType chcr1;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \struct	Drcr
        ///
        /// \brief	Dma Request/Response Selection Control Register (DRCR0 / DRCR1).
        ///
        /// \author	Runik
        /// \date	28/03/2023
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        struct Drcr {
            GENERATE_USING(Drcr, u8);

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// \enum	ResourceSelect
            ///
            /// \brief	DRCR - RSx bits.
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            enum class ResourceSelect : u8 {
                dreq     = 0b00, ///< DREQ (external request) (initial).
                rxi      = 0b01, ///< RXI (on chip SCI receive data full interrupt transfer request).
                txi      = 0b10, ///< TXI (on chip SCI transmit data empty interrupt transfer request).
                reserved = 0b11  ///< Reserved (setting prohibited)
            };

            GENERATE_BIT_WITH_ENUM(rs, 0, 0b11, ResourceSelect); ///< Resource select.
        };
        using DrcrType = Reg<u8, Drcr>;
        DrcrType drcr0;
        DrcrType drcr1;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \struct	Dmaor
        ///
        /// \brief	DMA Operation Register (DMAOR).
        ///
        /// \author	Runik
        /// \date	29/03/2023
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        struct Dmaor {
            GENERATE_USING(Dmaor, u32);

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// \enum   DmaMasterEnable
            ///
            /// \brief  DMAOR - DME bit.
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            enum class DmaMasterEnable : bool {
                disabled = false, ///< DMA transfers disabled on all channels (initial)
                enabled  = true   ///< DMA transfers enabled on all channels
            };

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// \enum   NmiFlag
            ///
            /// \brief  DMAOR - NMIF bit.
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            enum class NmiFlag : bool {
                no_nmif_interrupt      = false, ///< No NMIF interrupt (initial)
                nmif_interrupt_occured = true   ///< NMIF has occurred
            };

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// \enum   AddressErrorFlag
            ///
            /// \brief  DMAOR - AE bit.
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            enum class AddressErrorFlag : bool {
                no_dmac_address_error = false, ///< No DMAC address error (initial)
                dmac_address_error    = true   ///< Address error by DMAC
            };

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// \enum   PriorityMode
            ///
            /// \brief  DMAOR - PR bit.
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            enum class PriorityMode : bool {
                fixed       = false, ///< Fixed priority (initial)
                round_robin = true   ///< Round robin
            };

            GENERATE_BIT_WITH_ENUM(dme, 0, 0b1, DmaMasterEnable); ///< DMA master enable.
            GENERATE_BIT_WITH_ENUM(nmif, 1, 0b1, NmiFlag);        ///< NMI flag.
            GENERATE_BIT_WITH_ENUM(ae, 2, 0b1, AddressErrorFlag); ///< Address error flag.
            GENERATE_BIT_WITH_ENUM(pr, 3, 0b1, PriorityMode);     ///< Priority mode.
        };
        using DmaorType = Reg<u32, Dmaor>;
        DmaorType dmaor;
    };
    Dmac dmac;

    //////////////////////////////
    // 10. Division Unit (DIVU) //
    //////////////////////////////

    struct Divu {
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \struct	Dvsr
        ///
        /// \brief	Divisor Register (DVSR).
        ///
        /// \author	Runik
        /// \date	01/04/2023
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        struct Dvsr {};
        using DvsrType = Reg<u32, Dvsr>;
        DvsrType dvsr;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \struct	Dvdnt
        ///
        /// \brief	Dividend Register L for 32 bits Division (DVDNT).
        ///
        /// \author	Runik
        /// \date	01/04/2023
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        struct Dvdnt {};
        using DvdntType = Reg<u32, Dvdnt>;
        DvdntType dvdnt;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \struct	Dvcr
        ///
        /// \brief	Division Control Register (DVCR).
        ///
        /// \author	Runik
        /// \date	01/04/2023
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        struct Dvcr {
            GENERATE_USING(Dvcr, u32);

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// \enum   OverflowFlag
            ///
            /// \brief  DVCR - OVF bit value.
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            enum class OverflowFlag : bool {
                no_overflow = false, ///< No overflow has occurred (initial)
                overflow    = true   ///< Overflow has occurred
            };

            enum class InterruptRequestUponOverflow : bool {
                disabled = false, ///< Interrupt request (OVFI) caused by OVF disabled (initial)
                enabled  = true   ///< Interrupt request (OVFI) caused by OVF enabled.
            };

            static constexpr auto lo_word_pos = PosType(0);    ///< Defines the range of the upper 16 bits of the register.
            static constexpr auto hi_word_pos = PosType(16);   ///< Defines the range of the lower 16 bits of the register.

            GENERATE_BIT_WITH_ENUM(ovf, 0, 0b1, OverflowFlag); ///< Overflow flag.
            GENERATE_BIT_WITH_ENUM(ovfie, 1, 0b1, InterruptRequestUponOverflow); ///< Interrupt request caused by overflow.

            static constexpr auto word_mask = 0xFFFF;
            GENERATE_MASKED_RANGE("Sh2Regs::Divu::Dvcr", LO_WORD, loWord, word_mask, lo_word_pos, word_mask);
            GENERATE_MASKED_RANGE("Sh2Regs::Divu::Dvcr", HI_WORD, hiWord, word_mask, hi_word_pos, word_mask);
        };
        using DvcrType = Reg<u32, Dvcr>;
        DvcrType dvcr;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \struct	Dvdnth
        ///
        /// \brief	Dividend Register H (DVDNTH).
        ///
        /// \author	Runik
        /// \date	01/04/2023
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        struct Dvdnth {};
        using DvdnthType = Reg<u32, Dvdnth>;
        DvdnthType dvdnth;
        DvdnthType dvdnth_shadow;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \struct	Dvdntl
        ///
        /// \brief	Dividend Register L.
        ///
        /// \author	Runik
        /// \date	01/04/2023
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        struct Dvdntl {};
        using DvdntlType = Reg<u32, Dvdntl>;
        DvdntlType dvdntl;
        DvdntlType dvdntl_shadow;
    };
    Divu divu;

    //////////////////////////////////////////
    // 11. 16 Bits Free Running Timer (FRT) //
    //////////////////////////////////////////

    struct Frt {
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \struct	Tier
        ///
        /// \brief	Timer Interrupt Enable Register (TIER).
        ///
        /// \author	Runik
        /// \date	02/04/2023
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        struct Tier {
            GENERATE_USING(Tier, u8);

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// \enum   InterruptCaptureInterruptEnable
            ///
            /// \brief  TIER - ICIE values.
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            enum class InterruptCaptureInterruptEnable : bool {
                interrupt_request_disabled = false, ///< Interrupt request (ICI) caused by ICF disabled (initial)
                interrupt_request_enabled  = true   ///< Interrupt request (ICI) caused by ICF enabled
            };

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// \enum   OutputCompareInterruptAEnable
            ///
            /// \brief  TIER - OCIAE values.
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            enum class OutputCompareInterruptAEnable : bool {
                interrupt_request_disabled = false, ///< Interrupt request (ICIA) caused by OCFA disabled (initial)
                interrupt_request_enabled  = true   ///< Interrupt request (ICIA) caused by OCFA enabled
            };

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// \enum   OutputCompareInterruptBEnable
            ///
            /// \brief  TIER - OCIBE values.
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            enum class OutputCompareInterruptBEnable : bool {
                interrupt_request_disabled = false, ///< Interrupt request (ICIB) caused by OCFB disabled (initial)
                interrupt_request_enabled  = true   ///< Interrupt request (ICIB) caused by OCFB enabled
            };

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// \enum   TimerOverflowInterruptEnable
            ///
            /// \brief  TIER - OVIE values.
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            enum class TimerOverflowInterruptEnable : bool {
                interrupt_request_disabled = false, ///< Interrupt request (FOVI) caused by OVF disabled (initial)
                interrupt_request_enabled  = true   ///< Interrupt request (FOVI) caused by OVF enabled
            };

            GENERATE_BIT_WITH_ENUM(ovie, 1, 0b1, TimerOverflowInterruptEnable);    ///< Timer overflow interrupt enable.
            GENERATE_BIT_WITH_ENUM(ocibe, 2, 0b1, OutputCompareInterruptBEnable);  ///< Output compare interrupt B enable.
            GENERATE_BIT_WITH_ENUM(ociae, 3, 0b1, OutputCompareInterruptAEnable);  ///< Output compare interrupt A enable.
            GENERATE_BIT_WITH_ENUM(icie, 7, 0b1, InterruptCaptureInterruptEnable); ///< Interrupt capture interrupt enable.
        };
        using TierType = Reg<u8, Tier>;
        TierType tier;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \struct	Ftcsr
        ///
        /// \brief	Free Running Timer Control/Status register (FTCSR).
        ///
        /// \author	Runik
        /// \date	02/04/2023
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        struct Ftcsr {
            GENERATE_USING(Ftcsr, u8);

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// \enum   InputCaptureFlag
            ///
            /// \brief  FTCSR - ICF values.
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            enum class InputCaptureFlag : bool {
                clear = false, ///< Clear conditions: 0 is written when 1 is read
                set   = true   ///< Set conditions: when the FRC value is sent to ICR by the input capture signal
            };

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// \enum   OutputCompareFlagA
            ///
            /// \brief  FTCSR - OCFA values.
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            enum class OutputCompareFlagA : bool {
                clear = false, ///< Clear conditions: 0 is written when 1 is read
                set   = true   ///< Set conditions: when the FRC value becomes equal to OCRA
            };

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// \enum   OutputCompareFlagB
            ///
            /// \brief  FTCSR - OCFB values.
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            enum class OutputCompareFlagB : bool {
                clear = false, ///< Clear conditions: 0 is written when 1 is read
                set   = true   ///< Set conditions: when the FRC value becomes equal to OCRB
            };

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// \enum   TimerOverflowFlag
            ///
            /// \brief  FTCSR - OVF values.
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            enum class TimerOverflowFlag : bool {
                clear = false, ///< Clear conditions: 0 is written when 1 is read
                set   = true   ///< Set conditions: when the FRC value changes from 0xFFFF to 0x0000
            };

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// \enum   CounterClearA
            ///
            /// \brief  FTCSR - CCLRA values.
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            enum class CounterClearA : bool {
                clear_disabled   = false, ///< FRC clear disabled (initial)
                clear_on_compare = true   ///< FRC cleared on compare match A
            };

            GENERATE_BIT_WITH_ENUM(cclra, 0, 0b1, CounterClearA);     ///< Counter clear A.
            GENERATE_BIT_WITH_ENUM(ocfa, 1, 0b1, TimerOverflowFlag);  ///< Timer overflow flag.
            GENERATE_BIT_WITH_ENUM(ocfb, 2, 0b1, OutputCompareFlagB); ///< Output compare flag B.
            GENERATE_BIT_WITH_ENUM(ovf, 3, 0b1, OutputCompareFlagA);  ///< Output compare flag A.
            GENERATE_BIT_WITH_ENUM(icf, 7, 0b1, InputCaptureFlag);    ///< Input capture flag.
        };
        using FtcsrType = Reg<u8, Ftcsr>;
        FtcsrType ftcsr;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \struct	Frc
        ///
        /// \brief	Free Running Counter (FRC).
        ///
        /// \author	Runik
        /// \date	02/04/2023
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        struct Frc {
            GENERATE_USING(Frc, u16);

            static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
            static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

            static constexpr auto byte_mask = 0xFF;
            GENERATE_MASKED_RANGE("Sh2Regs::Frt::Frc", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
            GENERATE_MASKED_RANGE("Sh2Regs::Frt::Frc", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
        };
        using FrcType = Reg<u16, Frc>;
        FrcType frc;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \struct	Ocr
        ///
        /// \brief	Output Compare Register (OCRA / OCRB).
        ///
        /// \author	Runik
        /// \date	02/04/2023
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        struct Ocr {
            GENERATE_USING(Ocr, u16);

            static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
            static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

            static constexpr auto byte_mask = 0xFF;
            GENERATE_MASKED_RANGE("Sh2Regs::Frt::Ocr", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
            GENERATE_MASKED_RANGE("Sh2Regs::Frt::Ocr", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
        };
        using OcrType = Reg<u16, Ocr>;
        OcrType ocra;
        OcrType ocrb;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \struct	Tcr
        ///
        /// \brief	Timer Control Register (TCR).
        ///
        /// \author	Runik
        /// \date	03/04/2023
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        struct Tcr {
            GENERATE_USING(Tcr, u8);

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// \enum   InputEdgeSelect
            ///
            /// \brief  TCR - IEDG bit value.
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            enum class InputEdgeSelect : bool {
                falling = false, ///< Input captured on falling edge (initial)
                rising  = true   ///< Input captured on rising edge
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

            GENERATE_BIT_WITH_ENUM(cks, 0, 0b11, FrtClockSelect);  ///< Clock select.
            GENERATE_BIT_WITH_ENUM(iedg, 7, 0b1, InputEdgeSelect); ///< Input edge select.
        };
        using TcrType = Reg<u8, Tcr>;
        TcrType tcr;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \struct	Tocr
        ///
        /// \brief	Timer Output Compare Control Register (TOCR).
        ///
        /// \author	Runik
        /// \date	03/04/2023
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        struct Tocr {
            GENERATE_USING(Tocr, u8);

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// \enum   OutputCompareRegisterSelect
            ///
            /// \brief  TOCR - OCRS bit value.
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            enum class OutputCompareRegisterSelect : bool {
                ocra = false, ///< Selects register OCRA
                ocrb = true   ///< Selects register OCRB
            };

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// \enum   OutputLevelA
            ///
            /// \brief  TOCR - OLVLA bit value.
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            enum class OutputLevelA : bool {
                outputs_0 = false, ///< Outputs 0 on compare match A
                outputs_1 = true   ///< Outputs 1 on compare match A
            };

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// \enum   OutputLevelB
            ///
            /// \brief  TOCR - OLVLB bit value.
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            enum class OutputLevelB : bool {
                outputs_0 = false, ///< Outputs 0 on compare match B
                outputs_1 = true   ///< Outputs 1 on compare match B
            };

            GENERATE_BIT_WITH_ENUM(olvlb, 0, 0b1, OutputLevelB);               ///< Output level B.
            GENERATE_BIT_WITH_ENUM(olvla, 1, 0b1, OutputLevelA);               ///< Output level A.
            GENERATE_BIT_WITH_ENUM(ocrs, 4, 0b1, OutputCompareRegisterSelect); ///< Output compare register select.
        };
        using TocrType = Reg<u8, Tocr>;
        TocrType tocr;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \struct	Icr
        ///
        /// \brief	Input Capture Register (ICR).
        ///
        /// \author	Runik
        /// \date	03/04/2023
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        struct Icr {
            GENERATE_USING(Icr, u16);

            static constexpr auto lo_byte_pos = PosType(0); ///< Defines the range of the upper 8 bits of the register.
            static constexpr auto hi_byte_pos = PosType(8); ///< Defines the range of the lower 8 bits of the register.

            static constexpr auto byte_mask = 0xFF;
            GENERATE_MASKED_RANGE("Sh2Regs::Frt::Icr", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
            GENERATE_MASKED_RANGE("Sh2Regs::Frt::Icr", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);
        };
        using IcrType = Reg<u16, Icr>;
        IcrType icr;
    };
    Frt frt;

    ///////////////////////////////
    // 12. Watch Dog Timer (WDT) //
    ///////////////////////////////

    struct Wdt {
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \struct	Wtcnt
        ///
        /// \brief	Watchdog Timer Counter (WTCNT).
        ///
        /// \author	Runik
        /// \date	03/04/2023
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        struct Wtcnt {};
        using WtcntType = Reg<u8, Wtcnt>;
        WtcntType wtcnt;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \struct	Wtcsr
        ///
        /// \brief	Watchdog Timer Control/Status Register (WTCSR).
        ///
        /// \author	Runik
        /// \date	03/04/2023
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        struct Wtcsr {
            GENERATE_USING(Wtcsr, u8);

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// \enum   WdtOverflowFlag
            ///
            /// \brief  WTCSR - OVF value.
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            enum class WdtOverflowFlag : bool {
                no_overflow = false, ///< No overflow of WTCNT in interval timer mode (initial)
                overflow    = true   ///< WTCNT overflow in interval timer mode
            };

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// \enum   TimerModeSelect
            ///
            /// \brief WTCSR - (WT /IT) value.
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            enum class TimerModeSelect : bool {
                interval_timer_mode = false, ///< Interval timer mode option
                watchdog_timer_mode = true   ///< Watchdog timer mode option
            };

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// \enum   TimerEnable
            ///
            /// \brief  WTCSR - TME value.
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            enum class TimerEnable : bool {
                timer_disabled = false, ///< Timer disabled : WTCNT is initialized to 0x00 and count up stops. (initial)
                timer_enabled  = true   ///< Timer enabled : WTCNT starts counting.
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

            GENERATE_BIT_WITH_ENUM(cks, 0, 0b111, WdtClockSelect); ///< Clock select.
            GENERATE_BIT_WITH_ENUM(tme, 5, 0b1, TimerEnable);      ///< Timer enable.
            GENERATE_BIT_WITH_ENUM(wtit, 6, 0b1, TimerModeSelect); ///< Timer mode select.
            GENERATE_BIT_WITH_ENUM(ovf, 7, 0b1, WdtOverflowFlag);  ///< Overflow flag.
        };
        using WtcsrType = Reg<u8, Wtcsr>;
        WtcsrType wtcsr;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \struct	Rstcsr
        ///
        /// \brief	Reset Control Status Register (RSTCSR).
        ///
        /// \author	Runik
        /// \date	03/04/2023
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        struct Rstcsr {
            GENERATE_USING(Rstcsr, u8);

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// \enum   WatchdogTimerOverflowFlag
            ///
            /// \brief  RSTCSR - WOVF bit values.
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            enum class WatchdogTimerOverflowFlag : bool {
                no_overflow = false, ///< No WTCNT overflow in watchdof timer mode (initial).
                overflow    = true   ///< Set by WTCNT overflow in watchdog mode.
            };

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// \enum   ResetEnable
            ///
            /// \brief  RSTCSR - RSTE bit values.
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            enum class ResetEnable : bool {
                not_reset_when_overflow = false, ///< Not reset when WTCNT overflows (initial).
                reset_when_overflow     = true   ///< Reset when WTCNT overflows.
            };

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// \enum   ResetSelect
            ///
            /// \brief  RSTCSR - RSTS bit values.
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            enum class ResetSelect : bool {
                power_on_reset = false, ///< Power-on reset (initial)
                manual_reset   = true   ///< Manual reset
            };

            GENERATE_BIT_WITH_ENUM(rsts, 5, 0b1, ResetSelect);               ///< Reset select.
            GENERATE_BIT_WITH_ENUM(rste, 6, 0b1, ResetEnable);               ///< Reset enable.
            GENERATE_BIT_WITH_ENUM(wovf, 7, 0b1, WatchdogTimerOverflowFlag); ///< Watchdog timer overflow flag
        };
        using RstcsrType = Reg<u8, Rstcsr>;
        RstcsrType rstcsr;
    };
    Wdt wdt;

    /////////////////////////////////////////////
    // 13.Serial Communication Interface (SCI) //
    /////////////////////////////////////////////

    struct Sci {
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \struct	Smr
        ///
        /// \brief	Serial Mode Register (SMR).
        ///
        /// \author	Runik
        /// \date	03/04/2023
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        struct Smr {
            GENERATE_USING(Smr, u8);

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// \enum   CommunicationMode
            ///
            /// \brief  SMR - C/A bit values.
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            enum class CommunicationMode : bool {
                asynchronous        = false, ///< Asynchronous mode (initial).
                clocked_synchronous = true   ///< Clocked synchronous mode.
            };

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// \enum   CharacterLength
            ///
            /// \brief  SMR - CHR bit values.
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            enum class CharacterLength : bool {
                eight_bit_data = false, ///< 8-bit data (initial).
                seven_bit_data = true   ///< 7-bit data.
            };

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// \enum   ParityMode
            ///
            /// \brief  SMR - O/E bit values.
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            enum class ParityEnable : bool {
                parity_bit_not_added = false, ///< Parity bit not added or checked (initial).
                parity_bit_added     = true   ///< Parity bit added and checked.
            };

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// \enum   ParityMode
            ///
            /// \brief  SMR - O/E bit values.
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            enum class ParityMode : bool {
                even_parity = false, ///< Even parity (initial).
                odd_parity  = true   ///< Odd parity.
            };

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// \enum   StopBitLength
            ///
            /// \brief  SMR - STOP bit values.
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            enum class StopBitLength : bool {
                one_stop_bit  = false, ///< One stop bit (initial).
                two_stop_bits = true   ///< Two stop bits.
            };

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// \enum   MultiprocessorMode
            ///
            /// \brief  SMR - MP bit values.
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            enum class MultiprocessorMode : bool {
                multiprocessor_function_disabled = false, ///< Multiprocessor mode enabled (initial).
                multiprocessor_function_enabled  = true   ///< Multiprocessor mode disabled.
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

            GENERATE_BIT_WITH_ENUM(cks, 0, 0b11, SciClockSelect);   ///< Clock select.
            GENERATE_BIT_WITH_ENUM(mp, 2, 0b1, MultiprocessorMode); ///< Multiprocessor mode.
            GENERATE_BIT_WITH_ENUM(stop, 3, 0b1, StopBitLength);    ///< Stop bit length.
            GENERATE_BIT_WITH_ENUM(oe, 4, 0b1, ParityMode);         ///< Parity mode.
            GENERATE_BIT_WITH_ENUM(pe, 5, 0b1, ParityEnable);       ///< Parity enable.
            GENERATE_BIT_WITH_ENUM(chr, 6, 0b1, CharacterLength);   ///< Character length.
            GENERATE_BIT_WITH_ENUM(ca, 7, 0b1, CommunicationMode);  ///< Communication mode.
        };
        using SmrType = Reg<u8, Smr>;
        SmrType smr;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \struct	Brr
        ///
        /// \brief	Bit Rate Register (BRR).
        ///
        /// \author	Runik
        /// \date	04/04/2023
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        struct Brr {};
        using BrrType = Reg<u8, Brr>;
        BrrType brr;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \struct	Scr
        ///
        /// \brief	Serial Control Register (SCR).
        ///
        /// \author	Runik
        /// \date	04/04/2023
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        struct Scr {
            GENERATE_USING(Scr, u8);

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// \enum   TransmitInterruptEnable
            ///
            /// \brief  SCR - TIE bit values.
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            enum class TransmitInterruptEnable : bool {
                interrupt_disabled = false, ///< TXI request is disabled (initial).
                interrupt_enabled  = true   ///< TXI request is enabled.
            };

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// \enum   ReceiveInterruptEnable
            ///
            /// \brief  SCR - REI bit values.
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            enum class ReceiveInterruptEnable : bool {
                interrupts_disabled = false, ///< RXI and ERI requests are disabled (initial).
                interrupts_enabled  = true   ///< RXI and ERI requests are enabled.
            };

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// \enum   TransmitEnable
            ///
            /// \brief  SCR - TE bit values.
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            enum class TransmitEnable : bool {
                transmitter_disabled = false, ///< Transmitter disabled (initial).
                transmitter_enabled  = true   ///< Transmitter enabled.
            };

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// \enum   ReceiveEnable
            ///
            /// \brief  SCR - RE bit values.
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            enum class ReceiveEnable : bool {
                receiver_disabled = false, ///< Receiver disabled (initial).
                receiver_enabled  = true   ///< Receiver enabled.
            };

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// \enum   MultiprocessorInterruptEnable
            ///
            /// \brief  SCR - MPIE bit values.
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            enum class MultiprocessorInterruptEnable : bool {
                interrupt_disabled = false, ///< Multiprocessor interrupts are disabled (initial).
                interrupt_enabled  = true   ///< Multiprocessor interrupts are enabled.
            };

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// \enum   TransmitEndInterruptEnable
            ///
            /// \brief  SCR - TEIE bit values.
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            enum class TransmitEndInterruptEnable : bool {
                interrupt_disabled = false, ///< Transmit end interrupt (TEI) requests are disabled (initial).
                interrupt_enabled  = true   ///< Transmit end interrupt (TEI) requests are enabled.
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

            GENERATE_BIT_WITH_ENUM(cke, 0, 0b11, ClockEnable);                   ///< Clock enable.
            GENERATE_BIT_WITH_ENUM(teie, 2, 0b1, TransmitEndInterruptEnable);    ///< Transmit end interrupt enable.
            GENERATE_BIT_WITH_ENUM(mpie, 3, 0b1, MultiprocessorInterruptEnable); ///< Multiprocessor interrupt enable.
            GENERATE_BIT_WITH_ENUM(ore, 4, 0b1, ReceiveEnable);                  ///< Receiver enabled.
            GENERATE_BIT_WITH_ENUM(te, 5, 0b1, TransmitEnable);                  ///< Transmitter enabled.
            GENERATE_BIT_WITH_ENUM(rie, 6, 0b1, ReceiveInterruptEnable);         ///< Receive interrupt enabled.
            GENERATE_BIT_WITH_ENUM(tie, 7, 0b1, TransmitInterruptEnable);        ///< Transmit interrupt enabled.
        };
        using ScrType = Reg<u8, Scr>;
        ScrType scr;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \struct	Tdr
        ///
        /// \brief	Transmit Data Register (TDR).
        ///
        /// \author	Runik
        /// \date	04/04/2023
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        struct Tdr {};
        using TdrType = Reg<u8, Tdr>;
        TdrType tdr;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \struct	Tsr
        ///
        /// \brief	Transmit Shift Register (TSR).
        ///
        /// \author	Runik
        /// \date	04/04/2023
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        struct Tsr {};
        using TsrType = Reg<u8, Tsr>;
        TsrType tsr;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \struct	Ssr
        ///
        /// \brief	Serial Status Register (SSR).
        ///
        /// \author	Runik
        /// \date	04/04/2023
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        struct Ssr {
            GENERATE_USING(Ssr, u8);

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// \enum   TransmitDataRegisterEmpty
            ///
            /// \brief  SSR - TDRE bit values.
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            enum class TransmitDataRegisterEmpty : bool {
                contains_valid_data         = false, ///< TDR contains valid transmit data (initial).
                does_not_contain_valid_data = true   ///< TDR does not contain valid transmit data.
            };

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// \enum   ReceiveDataRegisterFull
            ///
            /// \brief  SSR - RDRF bit values.
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            enum class ReceiveDataRegisterFull : bool {
                no_valid_data_received = false, ///< RDR does not contain valid received data (initial).
                valid_data_received    = true   ///< RDR contains valid received data.
            };

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// \enum   OverrunError
            ///
            /// \brief  SSR - ORER bit values.
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            enum class OverrunError : bool {
                in_progress_or_ended_normally = false, ///< Receiving is in progress or has ended normally (initial).
                error_occurred                = true   ///< A receive overrun error occurred.
            };

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// \enum   FramingError
            ///
            /// \brief  SSR - FER bit values.
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            enum class FramingError : bool {
                in_progress_or_ended_normally = false, ///< Receiving is in progress or has ended normally (initial).
                error_occurred                = true   ///< A receive framing error occurred.
            };

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// \enum   ParityError
            ///
            /// \brief  SSR - PER bit values.
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            enum class ParityError : bool {
                in_progress_or_ended_normally = false, ///< Receiving is in progress or has ended normally (initial).
                error_occurred                = true   ///< A receive parity error occurred.
            };

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// \enum   TransmitEnd
            ///
            /// \brief  SSR - TEND bit values.
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            enum class TransmitEnd : bool {
                transmission_in_progress = false, ///< Transmission is in progress.
                end_of_transmission      = true   ///< End of transmission (initial).
            };

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// \enum   MultiprocessorBit
            ///
            /// \brief  SSR - MPB bit values.
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            enum class MultiprocessorBit : bool {
                bit_value_0 = false, ///< Multiprocessor bit value in receive data is 0 (initial).
                bit_value_1 = true   ///< Multiprocessor bit value in receive data is 1.
            };

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            /// \enum   MultiprocessorBitTransfer
            ///
            /// \brief  SSR - MPBT bit values.
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            enum class MultiprocessorBitTransfer : bool {
                bit_value_0 = false, ///< Multiprocessor bit value in transmit data is 0 (initial).
                bit_value_1 = true   ///< Multiprocessor bit value in transmit data is 1.
            };

            GENERATE_BIT_WITH_ENUM(mpbt, 0, 0b1, MultiprocessorBitTransfer); ///< Multiprocessor bit in transmit data.
            GENERATE_BIT_WITH_ENUM(mpb, 1, 0b1, MultiprocessorBit);          ///< Multiprocessor bit in receive data.
            GENERATE_BIT_WITH_ENUM(tend, 2, 0b1, TransmitEnd);               ///< Transmission has ended.
            GENERATE_BIT_WITH_ENUM(per, 3, 0b1, ParityError);                ///< Parity error.
            GENERATE_BIT_WITH_ENUM(fer, 4, 0b1, FramingError);               ///< Framing error.
            GENERATE_BIT_WITH_ENUM(orer, 5, 0b1, OverrunError);              ///< Overrun error.
            GENERATE_BIT_WITH_ENUM(rdrf, 6, 0b1, ReceiveDataRegisterFull);   ///< Receive data register full.
            GENERATE_BIT_WITH_ENUM(tdre, 7, 0b1, TransmitDataRegisterEmpty); ///< Transmit data register empty.
        };
        using SsrType = Reg<u8, Ssr>;
        SsrType ssr;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \struct	Rdr
        ///
        /// \brief	Receive Data Register (RDR).
        ///
        /// \author	Runik
        /// \date	04/04/2023
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        struct Rdr {};
        using RdrType = Reg<u8, Rdr>;
        RdrType rdr;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \struct	Rsr
        ///
        /// \brief	Receive Shift Register (RSR).
        ///
        /// \author	Runik
        /// \date	04/04/2023
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        struct Rsr {};
        using RsrType = Reg<u8, Rsr>;
        RsrType rsr;
    };
    Sci sci;

    /////////////////////////////////////////////
    // 14.Power-Down Modes                     //
    /////////////////////////////////////////////

    struct PowerDownModes {
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \struct	Sbycr
        ///
        /// \brief	Standby Control Register (SBYCR)
        ///
        /// \author	Runik
        /// \date	04/04/2023
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        struct Sbycr {};
        using SbycrType = Reg<u8, Sbycr>;
        SbycrType sbycr;
    };
    PowerDownModes pdm;
};

} // namespace saturnin::sh2
