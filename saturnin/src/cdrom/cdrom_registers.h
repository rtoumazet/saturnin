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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union  HirqStatusRegister
///
/// \brief  HIRQ status register.
///
/// \author Runik
/// \date   14/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union HirqStatusRegister {
    u16          raw;  ///< Raw representation.
    BitField<10> scdq; ///< Defines the SCDQ bit.
    BitField<9>  efls; ///< Defines the EFLS bit.
    BitField<8>  ecpy; ///< Defines the ECPY bit.
    BitField<7>  ehst; ///< Defines the EHST bit.
    BitField<6>  esel; ///< Defines the ESEL bit.
    BitField<5>  dchg; ///< Defines the DCHG bit.
    BitField<4>  pend; ///< Defines the PEND bit.
    BitField<3>  bful; ///< Defines the BFUL bit.
    BitField<2>  csct; ///< Defines the CSCT bit.
    BitField<1>  drdy; ///< Defines the DRDY bit.
    BitField<0>  cmok; ///< Defines the CMOK bit.
};

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
    file_system_in_progress = 0, ///< CD block file system in progress.
    file_system_finished    = 1  ///< CD block file system finished.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   Ecpy
///
/// \brief  Values that represent ECPY bit of HirqRegister.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Ecpy : bool {
    sector_copy_or_move_in_progress = 0, ///< Sector copy or move in progress.
    sector_copy_or_move_finished    = 1  ///< Sector copy or move finished.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   Ehst
///
/// \brief  Values that represent EHST bit of HirqRegister.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Ehst : bool {
    host_io_in_progress = 0, ///< Host I/O is in progress.
    host_io_finished    = 1  ///< Host I/O has finished.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   Esel
///
/// \brief  Values that represent ESEL bit of HirqRegister.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Esel : bool {
    soft_reset_or_selector_set_in_progress = 0, ///< Soft reset or selector set in progress.
    soft_reset_or_selector_set_finished    = 1  ///< Soft reset or selector set finished.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   Dchg
///
/// \brief  Values that represent DCHG bit of HirqRegister.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Dchg : bool {
    disk_has_not_changed = 0, ///< Disk has not changed.
    disk_changed         = 1  ///< Disk has been changed (tray opened and closed).
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   Pend
///
/// \brief  Values that represent PEND bit of HirqRegister.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Pend : bool {
    cd_play_in_progress = 0, ///< CD play has not ended.
    cd_play_has_ended   = 1  ///< CD play has ended (FAD out of range).
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   Bful
///
/// \brief  Values that represent BFUL bit of HirqRegister.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Bful : bool {
    buffer_not_full = 0, ///< CD buffer is not full.
    buffer_full     = 1  ///< CD buffer is full.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   Csct
///
/// \brief  Values that represent CSCT bit of HirqRegister.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Csct : bool {
    sector_not_stored = 0, ///< Sector has not been stored or been discarded.
    sector_stored     = 1  ///< 1 sector has just been stored.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   Drdy
///
/// \brief  Values that represent DRDY bit of HirqRegister.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Drdy : bool {
    not_setup      = 0, ///< Not setup.
    setup_complete = 1  ///< Data transfer setup complete, data can be read from fetch_data_pointer.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   Cmok
///
/// \brief  Values that represent CMOK bit of HirqRegister.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Cmok : bool {
    processing = 0, ///< System still processing last command.
    ready      = 1  ///< System ready for new command.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \union  HirqStatusRegister
///
/// \brief  HIRQ mask register.
///
/// \author Runik
/// \date   14/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union HirqMaskRegister {
    u16          raw;         ///< Raw representation.
    BitField<10> scdq_status; ///< Defines the SCDQ bit interrupt status.
    BitField<9>  efls_status; ///< Defines the EFLS bit interrupt status.
    BitField<8>  ecpy_status; ///< Defines the ECPY bit interrupt status.
    BitField<7>  ehst_status; ///< Defines the EHST bit interrupt status.
    BitField<6>  esel_status; ///< Defines the ESEL bit interrupt status.
    BitField<5>  dchg_status; ///< Defines the DCHG bit interrupt status.
    BitField<4>  pend_status; ///< Defines the PEND bit interrupt status.
    BitField<3>  bful_status; ///< Defines the BFUL bit interrupt status.
    BitField<2>  csct_status; ///< Defines the CSCT bit interrupt status.
    BitField<1>  drdy_status; ///< Defines the DRDY bit interrupt status.
    BitField<0>  cmok_status; ///< Defines the CMOK bit interrupt status.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   InterruptStatus
///
/// \brief  Values of the interrupt status.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum InterruptStatus : bool {
    masked  = 0, ///< Interrupt is masked.
    enabled = 1  ///< Interrupt is enabled.
};

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
/// \union  CommandRegister
///
/// \brief  Command register.
///
/// \author Runik
/// \date   14/01/2022
////////////////////////////////////////////////////////////////////////////////////////////////////

union CommandRegister {
    u16            raw;         ///< Raw representation.
    BitField<8, 8> command;     ///< Command (CR1 input).
    BitField<8, 8> status;      ///< Status (CR1 output).
    BitField<8, 8> high_8_bits; ///< 8..15 bits range of the register.
    BitField<0, 8> low_8_bits;  ///< 0..7 bits range of the register.
    BitField<4, 4> flag;        ///< 4 bit flag.
    BitField<0, 4> rep_cnt;     ///< 4 bit repeat frequency. Notification range 0x0..0xE (0..14 times)
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   CdDriveStatus
///
/// \brief  Values that represent CD drive status
////////////////////////////////////////////////////////////////////////////////////////////////////

enum CdDriveStatus : u8 {
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

// class CommandRegister : public Register {
//   public:
//     using Register::Register;
//     inline static const auto command      = BitRange<Command>{8, 15};       ///< Command (CR1 input).
//     inline static const auto status       = BitRange<CdDriveStatus>{8, 15}; ///< Status (CR1 output).
//     inline static const auto upper_8_bits = BitRange<u8>{8, 15};            ///< 8..15 bits range of the register.
//     inline static const auto lower_8_bits = BitRange<u8>{0, 7};             ///< 0..7 bits range of the register.
//     inline static const auto bits_4_7     = BitRange<u8>{4, 7};             ///< 4..7 bits range of the register.
//     inline static const auto bits_0_3     = BitRange<u8>{0, 3};             ///< 0..3 bits range of the register.
//     inline static const auto all_bits     = BitRange<u16>{0, 15}; ///< Defines the range of all the bits of the register.
// };

} // namespace saturnin::cdrom
