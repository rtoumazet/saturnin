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

#include "../emulator_defs.h"

namespace saturnin::cdrom {

/// \name CDROM registers addresses
//@{
constexpr u32 hirq_register_address      = 0x25890008;
constexpr u32 hirq_mask_register_address = 0x2589000c;
constexpr u32 command_register_1_address = 0x25890018;
constexpr u32 command_register_2_address = 0x2589001C;
constexpr u32 command_register_3_address = 0x25890020;
constexpr u32 command_register_4_address = 0x25890024;
constexpr u32 toc_data_pointer_address   = 0x25898000;
constexpr u32 fetch_data_pointer_address = 0x25818000;

//@}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   Scdq
///
/// \brief  Values that represent SCDQ bit of HirqRegister.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Scdq : u8 {
    subcode_q_decode_in_progress = 0, ///< Subcode Q decode in progress for current sector.
    subcode_q_decoded            = 1  ///< Subcode Q has been decoded for current sector.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   Efls
///
/// \brief  Values that represent EFLS bit of HirqRegister.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Efls : u8 {
    file_system_in_progress = 0, ///< CD block file system in progress.
    file_system_finished    = 1  ///< CD block file system finished.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   Ecpy
///
/// \brief  Values that represent ECPY bit of HirqRegister.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Ecpy : u8 {
    sector_copy_or_move_in_progress = 0, ///< Sector copy or move in progress.
    sector_copy_or_move_finished    = 1  ///< Sector copy or move finished.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   Ehst
///
/// \brief  Values that represent EHST bit of HirqRegister.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Ehst : u8 {
    host_io_in_progress = 0, ///< Host I/O is in progress.
    host_io_finished    = 1  ///< Host I/O has finished.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   Esel
///
/// \brief  Values that represent ESEL bit of HirqRegister.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Esel : u8 {
    soft_reset_or_selector_set_in_progress = 0, ///< Soft reset or selector set in progress.
    soft_reset_or_selector_set_finished    = 1  ///< Soft reset or selector set finished.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   Dchg
///
/// \brief  Values that represent DCHG bit of HirqRegister.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Dchg : u8 {
    disk_has_not_changed = 0, ///< Disk has not changed.
    disk_changed         = 1  ///< Disk has been changed (tray opened and closed).
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   Pend
///
/// \brief  Values that represent PEND bit of HirqRegister.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Pend : u8 {
    cd_play_in_progress = 0, ///< CD play has not ended.
    cd_play_has_ended   = 1  ///< CD play has ended (FAD out of range).
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   Bful
///
/// \brief  Values that represent BFUL bit of HirqRegister.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Bful : u8 {
    buffer_not_full = 0, ///< CD buffer is not full.
    buffer_full     = 1  ///< CD buffer is full.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   Csct
///
/// \brief  Values that represent CSCT bit of HirqRegister.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Csct : u8 {
    sector_not_stored = 0, ///< Sector has not been stored or been discarded.
    sector_stored     = 1  ///< 1 sector has just been stored.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   Drdy
///
/// \brief  Values that represent DRDY bit of HirqRegister.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Drdy : u8 {
    not_setup      = 0, ///< Not setup.
    setup_complete = 1  ///< Data transfer setup complete, data can be read from fetch_data_pointer.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   Cmok
///
/// \brief  Values that represent CMOK bit of HirqRegister.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Cmok : u8 {
    processing = 0, ///< System still processing last command.
    ready      = 1  ///< System ready for new command.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  HirqStatusRegister
///
/// \brief  HIRQ status register.
///
/// \author Runik
/// \date   09/05/2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class HirqStatusRegister : public Register {
  public:
    using Register::Register;
    inline static const BitRange<Scdq> scdq{10};        ///< Defines the SCDQ bit.
    inline static const BitRange<Efls> efls{9};         ///< Defines the EFLS bit.
    inline static const BitRange<Ecpy> ecpy{8};         ///< Defines the ECPY bit.
    inline static const BitRange<Ehst> ehst{7};         ///< Defines the EHST bit.
    inline static const BitRange<Esel> esel{6};         ///< Defines the ESEL bit.
    inline static const BitRange<Dchg> dchg{5};         ///< Defines the DCHG bit.
    inline static const BitRange<Pend> pend{4};         ///< Defines the PEND bit.
    inline static const BitRange<Bful> bful{3};         ///< Defines the BFUL bit.
    inline static const BitRange<Csct> csct{2};         ///< Defines the CSCT bit.
    inline static const BitRange<Drdy> drdy{1};         ///< Defines the DRDY bit.
    inline static const BitRange<Cmok> cmok{0};         ///< Defines the CMOK bit.
    inline static const BitRange<u16>  all_bits{0, 15}; ///< Defines the range of all the bits of the register.
};

enum class InterruptStatus : u8 {
    masked  = 0, ///< Interrupt is masked.
    enabled = 1  ///< Interrupt is enabled.
};

class HirqMaskRegister : public Register {
  public:
    using Register::Register;
    inline static const BitRange<InterruptStatus> scdq_status{10}; ///< Defines the SCDQ bit interrupt status.
    inline static const BitRange<InterruptStatus> efls_status{9};  ///< Defines the EFLS bit interrupt status.
    inline static const BitRange<InterruptStatus> ecpy_status{8};  ///< Defines the ECPY bit interrupt status.
    inline static const BitRange<InterruptStatus> ehst_status{7};  ///< Defines the EHST bit interrupt status.
    inline static const BitRange<InterruptStatus> esel_status{6};  ///< Defines the ESEL bit interrupt status.
    inline static const BitRange<InterruptStatus> dchg_status{5};  ///< Defines the DCHG bit interrupt status.
    inline static const BitRange<InterruptStatus> pend_status{4};  ///< Defines the PEND bit interrupt status.
    inline static const BitRange<InterruptStatus> bful_status{3};  ///< Defines the BFUL bit interrupt status.
    inline static const BitRange<InterruptStatus> csct_status{2};  ///< Defines the CSCT bit interrupt status.
    inline static const BitRange<InterruptStatus> drdy_status{1};  ///< Defines the DRDY bit interrupt status.
    inline static const BitRange<InterruptStatus> cmok_status{0};  ///< Defines the CMOK bit interrupt status.
    inline static const BitRange<u16>             all_bits{0, 15}; ///< Defines the range of all the bits of the register.
};

class CommandRegister : public Register {
  public:
    using Register::Register;
    inline static const BitRange<u8> upper_8_bits{8, 15}; ///< 8..15 bits range of the register.
    inline static const BitRange<u8> lower_8_bits{0, 7};  ///< 0..7 bits range of the register.
    inline static const BitRange<u8> bits_4_7{4, 7};      ///< 4..7 bits range of the register.
    inline static const BitRange<u8> bits_0_3{0, 3};      ///< 0..3 bits range of the register.

    inline static const BitRange<u16> all_bits{0, 15}; ///< Defines the range of all the bits of the register.
};

} // namespace saturnin::cdrom
