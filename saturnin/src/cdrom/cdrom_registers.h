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
    inline static const auto scdq     = BitRange<Scdq>{10};   ///< Defines the SCDQ bit.
    inline static const auto efls     = BitRange<Efls>{9};    ///< Defines the EFLS bit.
    inline static const auto ecpy     = BitRange<Ecpy>{8};    ///< Defines the ECPY bit.
    inline static const auto ehst     = BitRange<Ehst>{7};    ///< Defines the EHST bit.
    inline static const auto esel     = BitRange<Esel>{6};    ///< Defines the ESEL bit.
    inline static const auto dchg     = BitRange<Dchg>{5};    ///< Defines the DCHG bit.
    inline static const auto pend     = BitRange<Pend>{4};    ///< Defines the PEND bit.
    inline static const auto bful     = BitRange<Bful>{3};    ///< Defines the BFUL bit.
    inline static const auto csct     = BitRange<Csct>{2};    ///< Defines the CSCT bit.
    inline static const auto drdy     = BitRange<Drdy>{1};    ///< Defines the DRDY bit.
    inline static const auto cmok     = BitRange<Cmok>{0};    ///< Defines the CMOK bit.
    inline static const auto all_bits = BitRange<u16>{0, 15}; ///< Defines the range of all the bits of the register.
};

enum class InterruptStatus : u8 {
    masked  = 0, ///< Interrupt is masked.
    enabled = 1  ///< Interrupt is enabled.
};

class HirqMaskRegister : public Register {
  public:
    using Register::Register;
    inline static const auto scdq_status = BitRange<InterruptStatus>{10}; ///< Defines the SCDQ bit interrupt status.
    inline static const auto efls_status = BitRange<InterruptStatus>{9};  ///< Defines the EFLS bit interrupt status.
    inline static const auto ecpy_status = BitRange<InterruptStatus>{8};  ///< Defines the ECPY bit interrupt status.
    inline static const auto ehst_status = BitRange<InterruptStatus>{7};  ///< Defines the EHST bit interrupt status.
    inline static const auto esel_status = BitRange<InterruptStatus>{6};  ///< Defines the ESEL bit interrupt status.
    inline static const auto dchg_status = BitRange<InterruptStatus>{5};  ///< Defines the DCHG bit interrupt status.
    inline static const auto pend_status = BitRange<InterruptStatus>{4};  ///< Defines the PEND bit interrupt status.
    inline static const auto bful_status = BitRange<InterruptStatus>{3};  ///< Defines the BFUL bit interrupt status.
    inline static const auto csct_status = BitRange<InterruptStatus>{2};  ///< Defines the CSCT bit interrupt status.
    inline static const auto drdy_status = BitRange<InterruptStatus>{1};  ///< Defines the DRDY bit interrupt status.
    inline static const auto cmok_status = BitRange<InterruptStatus>{0};  ///< Defines the CMOK bit interrupt status.
    inline static const auto all_bits    = BitRange<u16>{0, 15};          ///< Defines the range of all the bits of the register.
};

class CommandRegister : public Register {
  public:
    using Register::Register;
    inline static const auto upper_8_bits = BitRange<u8>{8, 15};  ///< 8..15 bits range of the register.
    inline static const auto lower_8_bits = BitRange<u8>{0, 7};   ///< 0..7 bits range of the register.
    inline static const auto bits_4_7     = BitRange<u8>{4, 7};   ///< 4..7 bits range of the register.
    inline static const auto bits_0_3     = BitRange<u8>{0, 3};   ///< 0..3 bits range of the register.
    inline static const auto all_bits     = BitRange<u16>{0, 15}; ///< Defines the range of all the bits of the register.
};

} // namespace saturnin::cdrom
