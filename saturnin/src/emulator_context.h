// 
// emulator_context.h
// Saturnin
//
// Copyright (c) 2018-2019 Renaud Toumazet
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
#include "scu.h"
#include "sh2.h"
#include "stv_definitions.h"

namespace saturnin {
namespace core {

using sh2::Sh2;

static const std::string saturnin_version{ "1.00" };

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
        /// \fn bool Emulator_context::startInterface();
        ///
        /// \brief  Starts the emulator GUI.
        ///
        /// \author Runik
        /// \date   24/10/2019
        ///
        /// \return True if it succeeds.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        bool startInterface();

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn Config* Emulator_context::config()
        ///
        /// \brief  Returns a pointer to config object.
        ///
        /// \author Runik
        /// \date   05/07/2018
        ///
        /// \return A pointer to the config object
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        Config* config();

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn Memory* Emulator_context::memory()
        ///
        /// \brief  Returns a pointer to the memory object.
        ///
        /// \author Runik
        /// \date   05/07/2018
        ///
        /// \return A pointer to the memory object.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        Memory* memory() { return memory_.get(); };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn Sh2* Emulator_context::masterSh2()
        ///
        /// \brief  Returns a pointer to the master SH2 objet.
        ///
        /// \author Runik
        /// \date   09/02/2019
        ///
        /// \return A pointer to the master SH2 object.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        sh2::Sh2* masterSh2() { return master_sh2_.get(); };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn Sh2* Emulator_context::slaveSh2()
        ///
        /// \brief  Returns a pointer to the slave SH2 objet.
        ///
        /// \author Runik
        /// \date   09/02/2019
        ///
        /// \return A pointer to the slave SH2 object.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        sh2::Sh2* slaveSh2() { return slave_sh2_.get(); };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn Scu* Emulator_context::scu()
        ///
        /// \brief  Returns a pointer to the SCU objet.
        ///
        /// \author Runik
        /// \date   09/02/2019
        ///
        /// \return A pointer to the SCU objet.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        Scu* scu() { return scu_.get(); };

        HardwareMode    hardwareMode_{ HardwareMode::saturn };     ///< Hardware mode
        EmulationStatus emulationStatus_{ EmulationStatus::stopped }; ///< Emulation status
        RenderingStatus renderingStatus_{ RenderingStatus::running }; ///< Rendering status.

    private:
        std::unique_ptr<Config>   config_;          ///< Configuration object
        std::unique_ptr<Memory>   memory_;          ///< Memory object
        std::unique_ptr<Sh2> master_sh2_;      ///< Master SH2 object
        std::unique_ptr<Sh2> slave_sh2_;       ///< Slave SH2 object
        std::unique_ptr<Scu>      scu_;             ///< SCU object

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