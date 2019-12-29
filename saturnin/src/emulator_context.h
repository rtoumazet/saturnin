// 
// EmulatorContext.h
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
/// \file	EmulatorContext.h
///
/// \brief	Emulator context struct. 
///
/// Class used to handle the emulator state
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <memory> // unique_ptr, make_unique
#include <string> // string
#include <thread> // thread
#include "emulator_enums.h"
#include "config.h"
#include "memory.h"
#include "scu.h"
#include "sh2.h"
#include "smpc.h"
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
    /// \class  EmulatorContext
    ///
    /// \brief  Regroups everything related to the emulator state.
    ///
    /// \author Runik
    /// \date   12/06/2018
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    class EmulatorContext {
    public:
        //@{
        // Constructors / Destructors
        EmulatorContext();
        EmulatorContext(const EmulatorContext&)              = delete;
        EmulatorContext(EmulatorContext&&)                   = delete;
        EmulatorContext& operator=(const EmulatorContext&) & = delete;
        EmulatorContext& operator=(EmulatorContext&&) &      = delete;
        ~EmulatorContext()                                    = default;
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
        /// \fn void EmulatorContext::startEmulation();
        ///
        /// \brief  Starts the emulation.
        ///
        /// \author Runik
        /// \date   26/09/2018
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        void startEmulation();

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn void EmulatorContext::stopEmulation();
        ///
        /// \brief  Stops the emulation.
        ///
        /// \author Runik
        /// \date   26/10/2019
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        void stopEmulation();

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn void EmulatorContext::startInterface();
        ///
        /// \brief  Starts the emulator GUI.
        ///
        /// \author Runik
        /// \date   24/10/2019
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        void startInterface();

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn Config* EmulatorContext::config()
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
        /// \fn Memory* EmulatorContext::memory()
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
        /// \fn Sh2* EmulatorContext::masterSh2()
        ///
        /// \brief  Returns a pointer to the master SH2 object.
        ///
        /// \author Runik
        /// \date   09/02/2019
        ///
        /// \return A pointer to the master SH2 object.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        sh2::Sh2* masterSh2() { return master_sh2_.get(); };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn Sh2* EmulatorContext::slaveSh2()
        ///
        /// \brief  Returns a pointer to the slave SH2 object.
        ///
        /// \author Runik
        /// \date   09/02/2019
        ///
        /// \return A pointer to the slave SH2 object.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        sh2::Sh2* slaveSh2() { return slave_sh2_.get(); };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn Scu* EmulatorContext::scu()
        ///
        /// \brief  Returns a pointer to the SCU object.
        ///
        /// \author Runik
        /// \date   09/02/2019
        ///
        /// \return A pointer to the SCU objetc.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        Scu* scu() { return scu_.get(); };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn Smpc* EmulatorContext::smpc()
        ///
        /// \brief  Returns a pointer to the SMPC object.
        ///
        /// \author Runik
        /// \date   12/12/2019
        ///
        /// \return A pointer to the SMPC object.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        Smpc* smpc() { return smpc_.get(); };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn HardwareMode EmulatorContext::hardwareMode() const
        ///
        /// \brief  Current hardware mode of the emulator.
        ///
        /// \author Runik
        /// \date   26/10/2019
        ///
        /// \return A HardwareMode.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        HardwareMode hardwareMode() const { return hardware_mode_; };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn void EmulatorContext::emulationStatus(const EmulationStatus status) const
        ///
        /// \brief  Sets the emulation status of the emulator.
        ///
        /// \author Runik
        /// \date   26/10/2019
        ///
        /// \param  status  The new status.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        void emulationStatus(const EmulationStatus status) { emulation_status_ = status; };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn void EmulatorContext::renderingStatus(const RenderingStatus status)
        ///
        /// \brief  Sets the rendering status of the emulator.
        ///
        /// \author Runik
        /// \date   26/10/2019
        ///
        /// \param  status  The new status.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        void renderingStatus(const RenderingStatus status) { rendering_status_ = status; };

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn RenderingStatus EmulatorContext::renderingStatus()
        ///
        /// \brief  Gets the current rendering status.
        ///
        /// \author Runik
        /// \date   26/10/2019
        ///
        /// \return The RenderingStatus.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        RenderingStatus renderingStatus() { return rendering_status_; };
    private:
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn void EmulatorContext::emulationMainThread();
        ///
        /// \brief  Emulation main thread.
        ///
        /// \author Runik
        /// \date   26/10/2019
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        void emulationMainThread();

        std::unique_ptr<Config> config_;     ///< Configuration object
        std::unique_ptr<Memory> memory_;     ///< Memory object
        std::unique_ptr<Sh2>    master_sh2_; ///< Master SH2 object
        std::unique_ptr<Sh2>    slave_sh2_;  ///< Slave SH2 object
        std::unique_ptr<Scu>    scu_;        ///< SCU object
        std::unique_ptr<Smpc>   smpc_;       ///< SMPC object

        HardwareMode    hardware_mode_{ HardwareMode::saturn };     ///< Hardware mode
        EmulationStatus emulation_status_{ EmulationStatus::stopped }; ///< Emulation status
        RenderingStatus rendering_status_{ RenderingStatus::running }; ///< Rendering status.

                                                    
        /// \name Command line variables
        ///
        //@{
        std::string command_line_{ "" };       ///< Command line content
        bool        binary_autoload_{ false }; ///< True when the binary has to be automatically loaded.
        uint32_t    binary_address_{ 0 };      ///< The PC will be set to this address after loading the binary.
        //@}

        Rom_stv stv_rom_{ Rom_stv::none }; ///< Current ST-V ROM loaded.

        std::thread emulation_main_thread_; ///< The emulation main thread.

    };
}
}