// 
// spti.h
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
/// \file	spti.h
///
/// \brief	Declares the spti class. 
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <cstdint>
#include <string>
#include "scsi.h"

namespace saturnin {
namespace cdrom {

const uint8_t spti_timeout = 5; // in seconds

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class	Spti
///
/// \brief	Spti class. 
///
/// \author	Runik
/// \date	01/03/2010
////////////////////////////////////////////////////////////////////////////////////////////////////
class Spti {
    public:

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	static bool initialize()
    ///
    /// \brief	Initialises the SPTI context.
    ///
    /// \author	Runik
    /// \date	01/03/2010
    ///
    /// \return	true if initialization is successful.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    static bool initialize();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static void Spti::shutdown();
    ///
    /// \brief  Releases the SPTI context.
    ///
    /// \author Runik
    /// \date   01/03/2010
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static void shutdown();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static std::vector<ScsiDriveInfo> Spti::scanBus();
    ///
    /// \brief  Scans the bus to get cdrom drives information.
    ///
    /// \author Runik
    /// \date   01/03/2010
    ///
    /// \return number of drive.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static std::vector<ScsiDriveInfo> scanBus();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static void Spti::test();
    ///
    /// \brief  Unused.
    ///
    /// \author Runik
    /// \date   01/03/2010
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static void test();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static std::string Spti::readSector(const std::uint32_t & fad, const std::int32_t & nb, const core::Config& cfg);
    ///
    /// \brief  Reads sector(s).
    ///
    /// \author Runik
    /// \date   01/03/2010
    ///
    /// \param  fad FAD where to read the sector.
    /// \param  nb  Number of sectors to read.
    /// \param  cfg The configuration object.
    ///
    /// \return The sector.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static std::string readSector(const std::uint32_t & fad, const std::int32_t & nb);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static std::string Spti::readOneSector(const std::uint32_t & fad);
    ///
    /// \brief  Reads one sector.
    ///
    /// \author Runik
    /// \date   19/03/2015
    ///
    /// \param  fad FAD where to read the sector.
    ///
    /// \return Sector read.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static std::string readOneSector(const std::uint32_t & fad);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static void Spti::inquiry(const HANDLE & h, ScsiDriveInfo & di);
    ///
    /// \brief  Inquiries the parameter drive.
    ///
    /// \author Runik
    /// \date   01/03/2010
    ///
    /// \param          h   Handle to the drive.
    /// \param [in,out] di  The drive structure information.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static void inquiry(const HANDLE & h, ScsiDriveInfo & di);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static void Spti::getAdapterAddress(const HANDLE & h, ScsiDriveInfo & di);
    ///
    /// \brief  Fills the SCSI_Drive structure with the drive address.
    ///
    /// \author Runik
    /// \date   01/03/2010
    ///
    /// \param          h   Handle to the drive.
    /// \param [in,out] di  the drive structure information.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static void	getAdapterAddress(const HANDLE & h, ScsiDriveInfo & di);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static bool Spti::readToc(ScsiToc& toc_data);
    ///
    /// \brief  Reads the CD TOC.
    ///
    /// \author Runik
    /// \date   01/03/2010
    ///
    /// \param [in,out] toc_data    information describing the toc.
    ///
    /// \return true if successful.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static bool	readToc(ScsiToc& toc_data);
};
}
}