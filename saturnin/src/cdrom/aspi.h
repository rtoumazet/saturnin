//
// aspi.h
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
//
////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file	aspi.h
///
/// \brief	Declares the aspi class.
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <string>
#include <cstdint>
#include <functional> // function
//#include <scsidefs.h>
//#include <wnaspi32.h>

namespace saturnin {
namespace cdrom {

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \typedef	uint32_t (WINAPI *func)(LPSRB)
///
/// \brief	Used to load "SendASPI32Command".
////////////////////////////////////////////////////////////////////////////////////////////////////
// typedef uint32_t(WINAPI* func)(LPSRB);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class	Aspi
///
/// \brief	Aspi class.
///
/// \author	Runik
/// \date	01/03/2010
////////////////////////////////////////////////////////////////////////////////////////////////////
class Aspi {
  public:
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	static bool initialize()
    ///
    /// \brief	Initialises this object.
    ///
    /// \author	Runik
    /// \date	01/03/2010
    ///
    /// \return	true if initialization is successful.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    static bool initialize();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	static void shutdown()
    ///
    /// \brief	Releases everything related to this object.
    ///
    /// \author	Runik
    /// \date	01/03/2010
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    static void shutdown();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static std::vector<ScsiDriveInfo> Aspi::scanBus();
    ///
    /// \brief  Scans the bus.
    ///
    /// \author Runik
    /// \date   01/03/2010
    ///
    /// \return The list of drives found.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static std::vector<ScsiDriveInfo> scanBus();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	static std::string readSector(const uint32_t & fad, const int32_t & nb)
    ///
    /// \brief	Reads sector(s).
    ///
    /// \author	Runik
    /// \date	01/03/2010
    ///
    /// \param	fad	FAD where to read the sector.
    /// \param	nb	Number of sectors to read.
    ///
    /// \return	Sector(s) data.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    static std::string readSector(const uint32_t& fad, const int32_t& nb);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	static bool readToc(ScsiToc& toc_data)
    ///
    /// \brief	Reads the CD TOC.
    ///
    /// \author	Runik
    /// \date	01/03/2010
    ///
    /// \param [in,out]	TOCData	information describing the toc.
    ///
    /// \return	true if successful.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    static bool readToc(ScsiToc& toc_data);

  private:
    static HINSTANCE hWnAspi32; ///< Handle to the wnaspi32 dll.

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	static void inquiry(SCSI_DriveInfo& di)
    ///
    /// \brief	Inquiries the parameter drive.
    ///
    /// \author	Runik
    /// \date	01/03/2010
    ///
    /// \param [in,out]	di	the drive information.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    static void inquiry(ScsiDriveInfo& di);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	static void setTimeOut()
    ///
    /// \brief	Sets the time out.
    ///
    /// \author	Runik
    /// \date	01/03/2010
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    static void setTimeOut();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	static void resetBus()
    ///
    /// \brief	Resets the bus.
    ///
    /// \author	Runik
    /// \date	01/03/2010
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    static void resetBus();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \property	static boost::function<uint32_t (void)> pfGetAspi32SupportInfo
    ///
    /// \brief	Function pointer to GetAspi32SupportInfo.
    ///
    /// \return	pointed function error code
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    // static std::function<uint32_t(void)> pfGetAspi32SupportInfo;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \property	static boost::function<uint32_t (LPSRB)> pfSendAspi32Command
    ///
    /// \brief	Function pointer to SendAspi32Command.
    ///
    /// \return	pointed function error code.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    // static std::function<uint32_t(LPSRB)> pfSendAspi32Command;
};
} // namespace cdrom
} // namespace saturnin
