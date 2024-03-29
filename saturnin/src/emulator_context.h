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

#include <atomic> // atomic
#include <memory> // unique_ptr, make_unique
#include <string> // string
#include <thread> // thread
#include <saturnin/src/emulator_enums.h>
#include <saturnin/src/stv_definitions.h>

// Forward declarations
namespace saturnin::cdrom {
class Cdrom;
}
namespace saturnin::sh2 {
class Sh2;
enum class Sh2Type;
} // namespace saturnin::sh2
namespace saturnin::sound {
class Scsp;
}
namespace saturnin::video {
class BaseRenderingPart;
class Opengl;
class Vdp1;
class Vdp2;
} // namespace saturnin::video

struct GLFWwindow;

namespace saturnin::core {

/// \name Forward class declarations.
/// Used to speed up build time as the .h files are included in the .cpp
///
//@{
class Config;
class Memory;
class Scu;
class Smpc;
//@}

static const std::string saturnin_version{"0.5.0"};

enum class DebugPosition {
    on_subroutine_call,
    on_subroutine_return,
    on_status_change,
    after_intruction_exec,
    before_intruction_exec
};

using DebugPositionName = std::map<const DebugPosition, const std::string>; ///< (Immutable) name of the debug position

const DebugPositionName debug_position_name = {
    {DebugPosition::on_subroutine_call,     "On subroutine call"     },
    {DebugPosition::on_subroutine_return,   "On subroutine return"   },
    {DebugPosition::on_status_change,       "On status change"       },
    {DebugPosition::after_intruction_exec,  "After instruction exec" },
    {DebugPosition::before_intruction_exec, "Before instruction exec"}
};

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
    EmulatorContext(const EmulatorContext&)                      = delete;
    EmulatorContext(EmulatorContext&&)                           = delete;
    auto operator=(const EmulatorContext&) & -> EmulatorContext& = delete;
    auto operator=(EmulatorContext&&) & -> EmulatorContext&      = delete;
    ~EmulatorContext();
    //@}

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	auto EmulatorContext::initialize(int argc, char* argv[]) -> bool;
    ///
    /// \brief	Initializes the emulator
    ///
    /// \author	Runik
    /// \date	26/09/2018
    ///
    /// \param 		   	argc	The argc.
    /// \param [in,out]	argv	If non-null, the argv.
    ///
    /// \returns	True if the initialization went well.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto initialize(int argc, char* argv[]) -> bool;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	void EmulatorContext::reset() const;
    ///
    /// \brief	Resets this object
    ///
    /// \author	Runik
    /// \date	23/06/2022
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void reset() const;

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
    auto vdp1() -> video::Vdp1*;
    auto vdp2() -> video::Vdp2*;
    auto opengl() -> video::Opengl*;
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
    /// \fn void EmulatorContext::hardwareMode(const HardwareMode hm)
    ///
    /// \brief  Updates the hardware mode of the emulator.
    ///
    /// \author Runik
    /// \date   06/07/2020
    ///
    /// \param  hm  The new hardware mode.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void hardwareMode(const HardwareMode hm) { hardware_mode_ = hm; };

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
    /// \fn	auto EmulatorContext::emulationStatus() const
    ///
    /// \brief	Gets the current emulation status
    ///
    /// \author	Runik
    /// \date	07/04/2020
    ///
    /// \returns	The EmulationStatus.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto emulationStatus() const { return emulation_status_; };
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

    [[nodiscard]] auto renderingStatus() const -> RenderingStatus { return rendering_status_; };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	void EmulatorContext::debugStatus(const DebugStatus status);
    ///
    /// \brief	Sets the debug status of the emulator.
    ///
    /// \author	Runik
    /// \date	30/04/2020
    ///
    /// \param 	status	The new status.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void debugStatus(const DebugStatus status);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto EmulatorContext::debugStatus() -> DebugStatus
    ///
    /// \brief  Gets the current debug status.
    ///
    /// \author Runik
    /// \date   30/04/2020
    ///
    /// \returns    The DebugStatus.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    [[nodiscard]] auto debugStatus() const -> DebugStatus { return debug_status_; };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	void EmulatorContext::updateDebugStatus(const DebugPosition pos, const sh2::Sh2Type type);
    ///
    /// \brief	Updates the debug status
    ///
    /// \author	Runik
    /// \date	16/06/2023
    ///
    /// \param 	pos 	The position.
    /// \param 	type	Type of SH2 to process.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void updateDebugStatus(const DebugPosition pos, const sh2::Sh2Type type);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void EmulatorContext::openglWindow(GLFWwindow* window);
    ///
    /// \brief  Sets the Opengl window pointer.
    ///
    /// \author Runik
    /// \date   08/05/2020
    ///
    /// \param [in,out] window  If non-null, the OpenGL window pointer.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void openglWindow(GLFWwindow* window);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto EmulatorContext::openglWindow() const -> GLFWwindow*;
    ///
    /// \brief  Returns the Opengl window pointer.
    ///
    /// \author Runik
    /// \date   08/05/2020
    ///
    /// \returns    Null if it fails, else a pointer to a GLFWwindow.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    [[nodiscard]] auto openglWindow() const -> GLFWwindow*;

    void addToTrace(const u32 pc);
    void dumpTrace() const;

  private:
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	void EmulatorContext::emulationSetup();
    ///
    /// \brief	Sets up the various emulator modules.
    ///
    /// \author	Runik
    /// \date	23/01/2023
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void emulationSetup();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void EmulatorContext::emulationMainThread();
    ///
    /// \brief  Emulation main thread.
    ///
    /// \author Runik
    /// \date   26/10/2019
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void emulationMainThread();

    std::unique_ptr<Config>        config_;                      ///< Configuration object
    std::unique_ptr<Memory>        memory_;                      ///< Memory object
    std::unique_ptr<sh2::Sh2>      master_sh2_;                  ///< Master SH2 object
    std::unique_ptr<sh2::Sh2>      slave_sh2_;                   ///< Slave SH2 object
    std::unique_ptr<Scu>           scu_;                         ///< SCU object
    std::unique_ptr<Smpc>          smpc_;                        ///< SMPC object
    std::unique_ptr<sound::Scsp>   scsp_;                        ///< SCSP object
    std::unique_ptr<cdrom::Cdrom>  cdrom_;                       ///< CDROM object
    std::unique_ptr<video::Vdp1>   vdp1_;                        ///< Vdp1 object
    std::unique_ptr<video::Vdp2>   vdp2_;                        ///< Vdp2 object
    std::unique_ptr<video::Opengl> opengl_;                      ///< Opengl object

    HardwareMode    hardware_mode_{HardwareMode::saturn};        ///< Hardware mode
    EmulationStatus emulation_status_{EmulationStatus::stopped}; ///< Emulation status
    RenderingStatus rendering_status_{RenderingStatus::running}; ///< Rendering status.
    DebugStatus     debug_status_{DebugStatus::disabled};        ///< Debug status.

    /// \name Command line variables
    ///
    //@{
    std::string command_line_{};         ///< Command line content
    bool        binary_autoload_{false}; ///< True when the binary has to be automatically loaded.
    uint32_t    binary_address_{0};      ///< The PC will be set to this address after loading the binary.
    //@}

    Rom_stv stv_rom_{Rom_stv::none};     ///< Current ST-V ROM loaded.

    std::jthread emulation_main_thread_; ///< The emulation main thread.

    GLFWwindow* opengl_window_;          ///< The OpenGL window.

    std::vector<u32> trace_;
};

} // namespace saturnin::core
