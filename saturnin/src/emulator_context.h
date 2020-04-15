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
#include "stv_definitions.h"

// Forward declarations
namespace saturnin::sh2 {
class Sh2;
}
namespace saturnin::sound {
class Scsp;
}
namespace saturnin::cdrom {
class Cdrom;
}

namespace saturnin::core {

/// \name Class declarations.
/// Used to speed up build time as the .h files are included in the .cpp
///
//@{
class Config;
class Memory;
class Scu;
class Smpc;
//@}

static const std::string saturnin_version{"1.00"};

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
    EmulatorContext(const EmulatorContext&) = delete;
    EmulatorContext(EmulatorContext&&)      = delete;
    auto operator=(const EmulatorContext&) & -> EmulatorContext& = delete;
    auto operator=(EmulatorContext&&) & -> EmulatorContext& = delete;
    ~EmulatorContext();
    //@}

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto initialize() -> bool
    ///
    /// \brief  Initializes the emulator
    ///
    /// \author Runik
    /// \date   26/09/2018
    ///
    /// \return True if the initialization went well.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto initialize() -> bool;

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
    /// \fn void EmulatorContext::pauseEmulation();
    ///
    /// \brief  Pauses the emulation.
    ///
    /// \author Runik
    /// \date   10/04/2020
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void pauseEmulation();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void EmulatorContext::startInterface();
    ///
    /// \brief  Starts the emulator GUI.
    ///
    /// \author Runik
    /// \date   24/10/2019
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void startInterface();

    //@{
    // Functions returning pointers to the various systems of the emulator
    auto config() -> Config*;
    auto memory() -> Memory*;
    auto masterSh2() -> sh2::Sh2*;
    auto slaveSh2() -> sh2::Sh2*;
    auto scu() -> Scu*;
    auto smpc() -> Smpc*;
    auto scsp() -> sound::Scsp*;
    auto cdrom() -> cdrom::Cdrom*;
    //@}

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto EmulatorContext::hardwareMode() const -> HardwareMode
    ///
    /// \brief  Current hardware mode of the emulator.
    ///
    /// \author Runik
    /// \date   26/10/2019
    ///
    /// \return A HardwareMode.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    [[nodiscard]] auto hardwareMode() const -> HardwareMode { return hardware_mode_; };

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
    /// \fn auto EmulatorContext::emulationStatus()
    ///
    /// \brief  Gets the current emulation status
    ///
    /// \author Runik
    /// \date   07/04/2020
    ///
    /// \returns    The EmulationStatus.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto emulationStatus() { return emulation_status_; };
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
    /// \fn auto EmulatorContext::renderingStatus() -> RenderingStatus
    ///
    /// \brief  Gets the current rendering status.
    ///
    /// \author Runik
    /// \date   26/10/2019
    ///
    /// \return The RenderingStatus.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto renderingStatus() -> RenderingStatus { return rendering_status_; };

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

    std::unique_ptr<Config>       config_;     ///< Configuration object
    std::unique_ptr<Memory>       memory_;     ///< Memory object
    std::unique_ptr<sh2::Sh2>     master_sh2_; ///< Master SH2 object
    std::unique_ptr<sh2::Sh2>     slave_sh2_;  ///< Slave SH2 object
    std::unique_ptr<Scu>          scu_;        ///< SCU object
    std::unique_ptr<Smpc>         smpc_;       ///< SMPC object
    std::unique_ptr<sound::Scsp>  scsp_;       ///< SCSP object
    std::unique_ptr<cdrom::Cdrom> cdrom_;      ///< CDROM object

    HardwareMode    hardware_mode_{HardwareMode::saturn};        ///< Hardware mode
    EmulationStatus emulation_status_{EmulationStatus::stopped}; ///< Emulation status
    RenderingStatus rendering_status_{RenderingStatus::running}; ///< Rendering status.

    /// \name Command line variables
    ///
    //@{
    std::string command_line_{""};       ///< Command line content
    bool        binary_autoload_{false}; ///< True when the binary has to be automatically loaded.
    uint32_t    binary_address_{0};      ///< The PC will be set to this address after loading the binary.
    //@}

    Rom_stv stv_rom_{Rom_stv::none}; ///< Current ST-V ROM loaded.

    std::thread emulation_main_thread_; ///< The emulation main thread.
};

} // namespace saturnin::core
