//
// scsi.cpp
// Saturnin
//
// Copyright (c) 2003 Renaud Toumazet
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
/// \file	scsi.h
///
/// \brief	Declares the Scsi class and various related structs.
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <saturnin/src/pch.h>
#include <array>      // array
#include <functional> // function
#include <string>     // string
#include <windows.h>
#include <saturnin/src/emulator_defs.h>

namespace saturnin::cdrom {

constexpr auto scsi_max_drives     = u8{10};  ///< Maximum number of drives checked on the host system.
constexpr auto scsi_max_toc_tracks = u8{100}; ///< Maximum number of tracks saved in the TOC

// SCSI COMMANDS
constexpr auto scsi_inquiry = u8{0x12}; ///< Inquiry (MANDATORY)

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct	ScsiDriveInfo
///
/// \brief	SCSI drive infos.
///
/// \author	Runik
/// \date	28/02/2010
////////////////////////////////////////////////////////////////////////////////////////////////////
struct ScsiDriveInfo {
    s8          path;
    s8          target;
    s8          lun;
    std::string name;   ///< Name of the current device after an inquiry.
    std::string letter; ///< filled with a drive letter in SPTI (ex: E:\), and a SCSI address in ASPI (ex: 0:2:0).
};

/// SCSI structs

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct	ScsiTocTrack
///
/// \brief	Data from the track stored in the TOC.
///
/// \author	Runik
/// \date	28/02/2010
////////////////////////////////////////////////////////////////////////////////////////////////////
struct ScsiTocTrack {
    u8                reserved;
    u8                adr_ctrl;
    u8                trackno;
    u8                reserved1;
    std::array<u8, 4> addr;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct ScsiToc
///
/// \brief  TOC struct.
///
/// \author Runik
/// \date   28/02/2010
////////////////////////////////////////////////////////////////////////////////////////////////////

struct ScsiToc {
    std::array<u8, 2>                             size;
    u8                                            first;
    u8                                            last;
    std::array<ScsiTocTrack, scsi_max_toc_tracks> track;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class	Scsi
///
/// \brief	Scsi class.
///
/// \author	Runik
/// \date	28/02/2010
////////////////////////////////////////////////////////////////////////////////////////////////////
class Scsi {
  public:
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static auto Scsi::openDrive(wchar_t letter) -> HANDLE;
    ///
    /// \brief  Returns a handle to the drive passed as parameter.
    ///
    /// \author Runik
    /// \date   28/02/2010
    ///
    /// \param  letter  Letter of the drive to get the handle to.
    ///
    /// \returns    Handle to the drive.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static auto openDrive(wchar_t letter) -> HANDLE;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	static void settingUpAspiFunctions()
    ///
    /// \brief	Fills the function pointers with Aspi functions.
    ///
    /// \author	Runik
    /// \date	28/02/2010
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    static void settingUpAspiFunctions();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	static void settingUpSptiFunctions()
    ///
    /// \brief	Fills the function pointers with Spti functions.
    ///
    /// \author	Runik
    /// \date	28/02/2010
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    static void settingUpSptiFunctions();

    /// \name Function pointers
    ///
    /// Will hold the pointer to the Spti or Aspi functions
    //@{
    static std::function<bool(void)>                            initialize;
    static std::function<std::vector<ScsiDriveInfo>(void)>      scanBus;
    static std::function<::std::string(const u32&, const s32&)> readSector;
    static std::function<void(void)>                            shutdown;
    static std::function<bool(ScsiToc& toc_data)>               readToc;
    //@}
};

} // namespace saturnin::cdrom
