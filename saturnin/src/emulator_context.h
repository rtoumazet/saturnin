// 
// emulator_context.h
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
/// \file	emulator_context.h
///
/// \brief	Emulator context struct. 
///
/// Class used to handle the emulator state
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <memory> // unique_ptr, make_unique
#include <string> // string
#include "config.h"

namespace saturnin {
namespace core {

    /// \name Class declarations.
    /// Used to speed up build time as the .h files are included in the .cpp
    ///
    //@{
    //class CSH2;
    class Memory;
    //class COGL;
    //class InterruptHandler;
    //class Log;
    //class CCdRom;
    //class SMPC;
    //class SCU;
    //class Vdp1;
    //class Vdp2;
    //class SCSP;
    //@}

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \enum	Hardware_mode
    ///
    /// \brief	Hardware mode used. 
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    enum class Hardware_mode {
        saturn, ///< Saturn.
        stv     ///< ST-V.
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \enum   Emulation_status
    ///
    /// \brief  State of the emulation.
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    
    enum class Emulation_status {
        running,    ///< Emulator is running
        paused,     ///< Emulator is paused
        stopped     ///< Emulator is stopped
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \class  Emulator_context
    ///
    /// \brief  Regroups everything related to the emulator state.
    ///
    /// \author Runik
    /// \date   12/06/2018
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    struct Emulator_context {
        std::unique_ptr<Config> config;
        std::unique_ptr<Memory> memory{ std::make_unique<Memory>() };

        Hardware_mode    hardware_mode   { Hardware_mode::saturn };     ///< Hardware mode
        Emulation_status emulation_status{ Emulation_status::stopped }; ///< Emulation status

        /// \name Command line variables
        ///
        //@{
        std::string command_line{ "" };         ///< Command line content
        bool        autoload_binary  { false }; ///< True when the binary has to be automatically loaded.
        uint32_t    binary_address { 0 };       ///< The PC will be set to this address after loading the binary.
        //@}
        
        uint8_t stv_rom; ///< Current ST-V ROM loaded
    };
}
}