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
#include "emulator_enums.h"
#include "config.h"
#include "memory.h"
#include "stv_definitions.h"

namespace saturnin {
namespace core {

static std::string saturnin_version = "1.00";

    /// \name Class declarations.
    /// Used to speed up build time as the .h files are included in the .cpp
    ///
    //@{
    //class CSH2;
    //class Memory;
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
    /// \class  Emulator_context
    ///
    /// \brief  Regroups everything related to the emulator state.
    ///
    /// \author Runik
    /// \date   12/06/2018
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    class Emulator_context {
    public:
        //@{
        // Constructors / Destructors
        Emulator_context();
        Emulator_context(const Emulator_context&)              = delete;
        Emulator_context(Emulator_context&&)                   = delete;
        Emulator_context& operator=(const Emulator_context&) & = delete;
        Emulator_context& operator=(Emulator_context&&) &      = delete;
        ~Emulator_context()                                    = default;
        //@}
        // 

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn void initialize()
        ///
        /// \brief  Initializes the emulator
        ///
        /// \author Runik
        /// \date   26/09/2018
        ///
        /// \return True if the initialization went well.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        bool initialize();

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn void initialize()
        ///
        /// \brief  Runs the emulation
        ///
        /// \author Runik
        /// \date   26/09/2018
        ///
        /// \return True if the program ran without errors.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        bool run();

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn std::shared_ptr<Config>& Emulator_context::config()
        ///
        /// \brief  Returns a reference to the shared config object.
        ///
        /// \author Runik
        /// \date   05/07/2018
        ///
        /// \return A reference to a std::shared_ptr&lt;Config&gt;
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        std::shared_ptr<Config>& config() { return config_; };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn std::unique_ptr<Memory>& Emulator_context::memory()
        ///
        /// \brief  Returns a reference to the memory object.
        ///
        /// \author Runik
        /// \date   05/07/2018
        ///
        /// \return A reference to a std::unique_ptr&lt;Memory&gt;
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        std::unique_ptr<Memory>& memory(){return memory_;};


        Hardware_mode    hardware_mode{ Hardware_mode::saturn };     ///< Hardware mode
        Emulation_status emulation_status{ Emulation_status::stopped }; ///< Emulation status


    private:
        std::shared_ptr<Config> config_; ///< Configuration object
        std::unique_ptr<Memory> memory_; ///< Memory object

        /// \name Command line variables
        ///
        //@{
        std::string command_line_{ "" };       ///< Command line content
        bool        binary_autoload_{ false }; ///< True when the binary has to be automatically loaded.
        uint32_t    binary_address_{ 0 };      ///< The PC will be set to this address after loading the binary.
        //@}

        Rom_stv stv_rom_{ Rom_stv::none }; ///< Current ST-V ROM loaded

    };
}
}