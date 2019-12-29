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

#pragma warning(push, 3) 
#include <functional>   // function
#include <string>       // string
#include <windows.h>
#pragma warning(pop) 

namespace saturnin {
namespace cdrom {

const uint8_t scsi_max_drives       = 10;  ///< Maximum number of drives checked on the host system. 
const uint8_t scsi_max_toc_tracks   = 100; ///< Maximum number of tracks saved in the TOC

// SCSI COMMANDS
const uint8_t scsi_inquiry = 0x12;   ///< Inquiry (MANDATORY)

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct	ScsiDriveInfo
///
/// \brief	SCSI drive infos.
///
/// \author	Runik
/// \date	28/02/2010
////////////////////////////////////////////////////////////////////////////////////////////////////
struct ScsiDriveInfo {
    int8_t	        path;
    int8_t	        target;
    int8_t	        lun;
    std::wstring	name;	///< Name of the current device after an inquiry. 
    std::wstring	letter; ///< filled with a drive letter in SPTI (ex: E:\), and a SCSI address in ASPI (ex: 0:2:0). 
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
    uint8_t reserved;
    uint8_t adr_ctrl;
    uint8_t trackno;
    uint8_t reserved1;
    uint8_t addr[4];
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
    uint8_t size[2];
    uint8_t first;
    uint8_t last;
    ScsiTocTrack track[scsi_max_toc_tracks];
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
    /// \fn	static HANDLE openDrive(const wchar_t cLetter)
    ///
    /// \brief	Returns a handle to the drive passed as parameter. 
    ///
    /// \author	Runik
    /// \date	28/02/2010
    ///
    /// \param	letter	Letter of the drive to get the handle to. 
    ///
    /// \return	Handle to the drive. 
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    static HANDLE openDrive(const wchar_t letter);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	static void settingUpAspiFunctions()
    ///
    /// \brief	Fills the function pointers with Aspi functions. 
    ///
    /// \author	Runik
    /// \date	28/02/2010
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    static void	settingUpAspiFunctions();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	static void settingUpSptiFunctions()
    ///
    /// \brief	Fills the function pointers with Spti functions. 
    ///
    /// \author	Runik
    /// \date	28/02/2010
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    static void	settingUpSptiFunctions();

    /// \name Function pointers
    ///
    /// Will hold the pointer to the Spti or Aspi functions
    //@{
    static std::function<bool(void)>									    initialize;
    static std::function<std::vector<ScsiDriveInfo>(void)>  	            scanBus;
    static std::function<::std::string(const uint32_t &, const int32_t &)>  readSector;
    static std::function<void(void)>									    shutdown;
    static std::function<bool(ScsiToc& toc_data)>						    readToc;
    //@}
};

}
}
