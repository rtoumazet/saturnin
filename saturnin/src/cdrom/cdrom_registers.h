//
// cdrom_registers.h
// Saturnin
//
// Copyright (c) 2020 Renaud Toumazet
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
/// \file	cdrom_registers.h
///
/// \brief	Declares CDROM registers and related enums.
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <saturnin/src/emulator_defs.h>
#include <saturnin/src/bitfield.h>
#include <saturnin/src/bit_register.h>

namespace saturnin::cdrom {

/// \name CDROM registers addresses
//@{
constexpr auto hirq_register_address      = u32{0x25890008};
constexpr auto hirq_mask_register_address = u32{0x2589000c};
constexpr auto command_register_1_address = u32{0x25890018};
constexpr auto command_register_2_address = u32{0x2589001C};
constexpr auto command_register_3_address = u32{0x25890020};
constexpr auto command_register_4_address = u32{0x25890024};
constexpr auto toc_data_pointer_address   = u32{0x25898000};
constexpr auto fetch_data_pointer_address = u32{0x25818000};
//@}

struct CdromRegs {
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	HIrqReq
    ///
    /// \brief	HIRQ status register.
    ///
    /// \author	Runik
    /// \date	04/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct HIrqReq {
        GENERATE_USING(HIrqReq, u16);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   Scdq
        ///
        /// \brief  Values that represent SCDQ bit of HirqRegister.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class Scdq {
            subcode_q_decode_in_progress = 0u, ///< Subcode Q decode in progress for current sector.
            subcode_q_decoded            = 1u  ///< Subcode Q has been decoded for current sector.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   Efls
        ///
        /// \brief  Values that represent EFLS bit of HirqRegister.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class Efls : bool {
            file_system_in_progress = false, ///< CD block file system in progress.
            file_system_finished    = true   ///< CD block file system finished.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   Ecpy
        ///
        /// \brief  Values that represent ECPY bit of HirqRegister.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class Ecpy : bool {
            sector_copy_or_move_in_progress = false, ///< Sector copy or move in progress.
            sector_copy_or_move_finished    = true   ///< Sector copy or move finished.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   Ehst
        ///
        /// \brief  Values that represent EHST bit of HirqRegister.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class Ehst : bool {
            host_io_in_progress = false, ///< Host I/O is in progress.
            host_io_finished    = true   ///< Host I/O has finished.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   Esel
        ///
        /// \brief  Values that represent ESEL bit of HirqRegister.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class Esel : bool {
            soft_reset_or_selector_set_in_progress = false, ///< Soft reset or selector set in progress.
            soft_reset_or_selector_set_finished    = true   ///< Soft reset or selector set finished.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   Dchg
        ///
        /// \brief  Values that represent DCHG bit of HirqRegister.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class Dchg : bool {
            disk_has_not_changed = false, ///< Disk has not changed.
            disk_changed         = true   ///< Disk has been changed (tray opened and closed).
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   Pend
        ///
        /// \brief  Values that represent PEND bit of HirqRegister.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class Pend : bool {
            cd_play_in_progress = false, ///< CD play has not ended.
            cd_play_has_ended   = true   ///< CD play has ended (FAD out of range).
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   Bful
        ///
        /// \brief  Values that represent BFUL bit of HirqRegister.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class Bful : bool {
            buffer_not_full = false, ///< CD buffer is not full.
            buffer_full     = true   ///< CD buffer is full.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   Csct
        ///
        /// \brief  Values that represent CSCT bit of HirqRegister.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class Csct : bool {
            sector_not_stored = false, ///< Sector has not been stored or been discarded.
            sector_stored     = true   ///< 1 sector has just been stored.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   Drdy
        ///
        /// \brief  Values that represent DRDY bit of HirqRegister.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class Drdy : bool {
            not_setup      = false, ///< Not setup.
            setup_complete = true   ///< Data transfer setup complete, data can be read from fetch_data_pointer.
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   Cmok
        ///
        /// \brief  Values that represent CMOK bit of HirqRegister.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class Cmok : bool {
            processing = false, ///< System still processing last command.
            ready      = true   ///< System ready for new command.
        };

        GENERATE_BIT_WITH_ENUM(cmok, 0, 0b1, Cmok);  ///< Defines the CMOK bit.
        GENERATE_BIT_WITH_ENUM(drdy, 1, 0b1, Drdy);  ///< Defines the DRDY bit.
        GENERATE_BIT_WITH_ENUM(csct, 2, 0b1, Csct);  ///< Defines the CSCT bit.
        GENERATE_BIT_WITH_ENUM(bful, 3, 0b1, Bful);  ///< Defines the BFUL bit.
        GENERATE_BIT_WITH_ENUM(pend, 4, 0b1, Pend);  ///< Defines the PEND bit.
        GENERATE_BIT_WITH_ENUM(dchg, 5, 0b1, Dchg);  ///< Defines the DCHG bit.
        GENERATE_BIT_WITH_ENUM(esel, 6, 0b1, Esel);  ///< Defines the ESEL bit.
        GENERATE_BIT_WITH_ENUM(ehst, 7, 0b1, Ehst);  ///< Defines the EHST bit.
        GENERATE_BIT_WITH_ENUM(ecpy, 8, 0b1, Ecpy);  ///< Defines the ECPY bit.
        GENERATE_BIT_WITH_ENUM(efls, 9, 0b1, Efls);  ///< Defines the EFLS bit.
        GENERATE_BIT_WITH_ENUM(scdq, 10, 0b1, Scdq); ///< Defines the SCDQ bit.
    };
    using HIrqReqType = Reg<u16, HIrqReq>;
    HIrqReqType hirqreq;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	HIrqMask
    ///
    /// \brief	HIRQ mask register.
    ///
    /// \author	Runik
    /// \date	05/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct HIrqMask {
        GENERATE_USING(HIrqMask, u16);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum	InterruptStatus
        ///
        /// \brief	Values of the interrupt status.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class InterruptStatus : bool {
            masked  = false, ///< Interrupt is masked.
            enabled = true   ///< Interrupt is enabled.
        };

        GENERATE_BIT_WITH_ENUM(cmok_status, 0, 0b1, InterruptStatus);  ///< Defines the SCDQ bit interrupt status.
        GENERATE_BIT_WITH_ENUM(drdy_status, 1, 0b1, InterruptStatus);  ///< Defines the EFLS bit interrupt status.
        GENERATE_BIT_WITH_ENUM(csct_status, 2, 0b1, InterruptStatus);  ///< Defines the ECPY bit interrupt status.
        GENERATE_BIT_WITH_ENUM(bful_status, 3, 0b1, InterruptStatus);  ///< Defines the EHST bit interrupt status.
        GENERATE_BIT_WITH_ENUM(pend_status, 4, 0b1, InterruptStatus);  ///< Defines the ESEL bit interrupt status.
        GENERATE_BIT_WITH_ENUM(dchg_status, 5, 0b1, InterruptStatus);  ///< Defines the DCHG bit interrupt status.
        GENERATE_BIT_WITH_ENUM(esel_status, 6, 0b1, InterruptStatus);  ///< Defines the PEND bit interrupt status.
        GENERATE_BIT_WITH_ENUM(ehst_status, 7, 0b1, InterruptStatus);  ///< Defines the BFUL bit interrupt status.
        GENERATE_BIT_WITH_ENUM(ecpy_status, 8, 0b1, InterruptStatus);  ///< Defines the CSCT bit interrupt status.
        GENERATE_BIT_WITH_ENUM(efls_status, 9, 0b1, InterruptStatus);  ///< Defines the DRDY bit interrupt status.
        GENERATE_BIT_WITH_ENUM(scdq_status, 10, 0b1, InterruptStatus); ///< Defines the CMOK bit interrupt status.
    };
    using HIrqMaskType = Reg<u16, HIrqMask>;
    HIrqMaskType hirqmask;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \struct	Cr
    ///
    /// \brief	Command register.
    ///
    /// \author	Runik
    /// \date	05/04/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Cr {
        GENERATE_USING(Cr, u16);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   Command
        ///
        /// \brief  Values of the cdblock commands.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class Command : u8 {
            get_status                       = 0x00,
            get_hardware_info                = 0x01,
            get_toc                          = 0x02,
            get_session_info                 = 0x03,
            init_system                      = 0x04,
            open_tray                        = 0x05,
            end_data_transfer                = 0x06,
            cd_play                          = 0x10,
            seek                             = 0x11,
            scan                             = 0x12,
            get_current_subcode              = 0x20,
            set_connection                   = 0x30,
            get_connection                   = 0x31,
            get_last_buffer_destination      = 0x32,
            set_filter_range                 = 0x40,
            get_filter_range                 = 0x41,
            set_filter_subheader_condition   = 0x42,
            get_filter_subheader_condition   = 0x43,
            set_filter_mode                  = 0x44,
            get_filter_mode                  = 0x45,
            set_filter_connection            = 0x46,
            get_filter_connection            = 0x47,
            reset_selector                   = 0x48,
            get_buffer_size                  = 0x50,
            get_sector_number                = 0x51,
            calculate_actual_size            = 0x52,
            get_actual_size                  = 0x53,
            get_sector_info                  = 0x54,
            execute_fad_search               = 0x55,
            get_fad_search_results           = 0x56,
            set_sector_length                = 0x60,
            get_sector_data                  = 0x61,
            delete_sector_data               = 0x62,
            get_then_delete_sector_data      = 0x63,
            put_sector_data                  = 0x64,
            copy_sector_data                 = 0x65,
            move_sector_data                 = 0x66,
            get_copy_error                   = 0x67,
            change_directory                 = 0x70,
            read_directory                   = 0x71,
            get_file_system_scope            = 0x72,
            get_file_info                    = 0x73,
            read_file                        = 0x74,
            abort_file                       = 0x75,
            mpeg_get_status                  = 0x90,
            mpeg_get_interrupt               = 0x91,
            mpeg_set_interrupt               = 0x92,
            mpeg_init                        = 0x93,
            mpeg_set_mode                    = 0x94,
            mpeg_play                        = 0x95,
            mpeg_set_decoding_method         = 0x96,
            mpeg_out_decoding_sync           = 0x97,
            mpeg_get_timecode                = 0x98,
            mpeg_get_pts                     = 0x99,
            mpeg_set_connection              = 0x9a,
            mpeg_get_connection              = 0x9b,
            mpeg_change_connection           = 0x9c,
            mpeg_set_stream                  = 0x9d,
            mpeg_get_stream                  = 0x9e,
            mpeg_get_picture_size            = 0x9f,
            mpeg_display                     = 0xa0,
            mpeg_set_window                  = 0xa1,
            mpeg_set_border_color            = 0xa2,
            mpeg_set_fade                    = 0xa3,
            mpeg_set_video_effects           = 0xa4,
            mpeg_get_image                   = 0xa5,
            mpeg_set_image                   = 0xa6,
            mpeg_read_image                  = 0xa7,
            mpeg_write_image                 = 0xa8,
            mpeg_read_sector                 = 0xa9,
            mpeg_write_sector                = 0xaa,
            mpeg_get_lsi                     = 0xae,
            mpeg_set_lsi                     = 0xaf,
            device_authentication_check      = 0xe0,
            get_device_authentication_status = 0xe1,
            get_mpeg_card_boot_roms          = 0xe2,
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \enum   CdDriveStatus
        ///
        /// \brief  Values that represent CD drive status
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        enum class CdDriveStatus : u8 {
            busy                = 0x00,
            paused              = 0x01,
            standby             = 0x02,
            playing             = 0x03,
            seeking             = 0x04,
            scanning            = 0x05,
            drive_is_open       = 0x06,
            no_disc_inserted    = 0x07,
            retrying            = 0x08,
            error               = 0x09,
            fatal_error         = 0x0a,
            periodical_response = 0x20,
            transfer_request    = 0x40,
            waiting             = 0x80,
            command_rejected    = 0xff
        };

        static constexpr auto rep_cnt_pos = PosType(0); ///< 4 bit repeat frequency. Notification range 0x0..0xE (0..14 times).
        static constexpr auto flag_pos    = PosType(4); ///< 4 bit flag.
        static constexpr auto lo_byte_pos = PosType(0); ///< 0..7 bits range of the register.
        static constexpr auto hi_byte_pos = PosType(8); ///< 8..15 bits range of the register.

        static constexpr auto byte_mask = 0xFF;
        GENERATE_MASKED_RANGE("CdromRegs::Cr", LO_BYTE, loByte, byte_mask, lo_byte_pos, byte_mask);
        GENERATE_MASKED_RANGE("CdromRegs::Cr", HI_BYTE, hiByte, byte_mask, hi_byte_pos, byte_mask);

        static constexpr auto rep_cnt_mask = 0b1111;
        GENERATE_MASKED_RANGE("CdromRegs::Cr", REP_CNT, repCnt, rep_cnt_mask, rep_cnt_pos, rep_cnt_mask);

        static constexpr auto flag_mask = 0b1111;
        GENERATE_MASKED_RANGE("CdromRegs::Cr", FLAG, flag, flag_mask, flag_pos, flag_mask);

        GENERATE_BIT_WITH_ENUM(command, 8, 0b11111111, Command);      ///< Command (CR1 input).
        GENERATE_BIT_WITH_ENUM(status, 8, 0b11111111, CdDriveStatus); ///< Status (CR1 output).
    };
    using CrType = Reg<u16, Cr>;
    CrType cr1;
    CrType cr2;
    CrType cr3;
    CrType cr4;
};

} // namespace saturnin::cdrom
