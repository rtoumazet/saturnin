//
// gui.h
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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file	gui.h
///
/// \brief	Declares everything related to GUI display.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>
#include <saturnin/src/emulator_context.h> // EmulatorContext
#include <saturnin/src/emulator_defs.h>    // s32, u32, Coord

namespace video = saturnin::video;

// Forward declarations
namespace saturnin::video {
class Opengl;
}

namespace saturnin::gui {

constexpr auto icon_debug      = "\xEE\xA4\x80 ";
constexpr auto icon_config     = "\xEE\xA4\x81 ";
constexpr auto icon_step_into  = "\xEE\xA4\x82 ";
constexpr auto icon_step_out   = "\xEE\xA4\x83 ";
constexpr auto icon_step_over  = "\xEE\xA4\x84 ";
constexpr auto icon_play       = "\xEE\xA4\x85 ";
constexpr auto icon_pause      = "\xEE\xA4\x86 ";
constexpr auto icon_stop       = "\xEE\xA4\x87 ";
constexpr auto icon_next_frame = "\xEE\xA4\x88 ";

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn void showImguiDemoWindow(bool show_window);
///
/// \brief  Displays the imgui demo window.
///
/// \author Runik
/// \date   24/10/2019
///
/// \param  show_window True to display the window.
////////////////////////////////////////////////////////////////////////////////////////////////////

void showImguiDemoWindow(bool show_window);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn void showCoreWindow(core::EmulatorContext& state);
///
/// \brief  Displays the core window.
///
/// \author Runik
/// \date   24/10/2019
///
/// \param [in,out] state   The emulator context.
////////////////////////////////////////////////////////////////////////////////////////////////////

void showCoreWindow(core::EmulatorContext& state);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn void showMainMenu(core::EmulatorContext& state);
///
/// \brief  Shows the main menu in the main window
///
/// \author Runik
/// \date   10/02/2021
///
/// \param [in,out] state   The emulator context.
////////////////////////////////////////////////////////////////////////////////////////////////////

void showMainMenu(core::EmulatorContext& state);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn void showRenderingWindow(core::EmulatorContext& state);
///
/// \brief  Displays the rendering window.
///
/// \author Runik
/// \date   24/10/2019
///
/// \param [in,out] state   The emulator context.
////////////////////////////////////////////////////////////////////////////////////////////////////

void showRenderingWindow(core::EmulatorContext& state);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn void showStvWindow();
///
/// \brief  Displays the ST-V games choice window.
///
/// \author Runik
/// \date   24/10/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

void showStvWindow();

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn void showLogWindow(core::EmulatorContext& state, bool* opened);
///
/// \brief  Displays the log window.
///
/// \author Runik
/// \date   24/10/2019
///
/// \param [in,out] state   The emulator context.
/// \param [in,out] opened  True will display the window.

////////////////////////////////////////////////////////////////////////////////////////////////////

void showLogWindow(core::EmulatorContext& state, bool* opened);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn void showSh2DebugWindow(core::EmulatorContext& state, bool* opened);
///
/// \brief  Displays the debug window for both SH2.
///
/// \author Runik
/// \date   06/04/2020
///
/// \param [in,out] state   The emulator context.
/// \param [in,out] opened  True will display the window.
////////////////////////////////////////////////////////////////////////////////////////////////////

void showSh2DebugWindow(core::EmulatorContext& state, bool* opened);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn void showMemoryDebugWindow(core::EmulatorContext& state, bool* opened);
///
/// \brief  Displays the memory debug window
///
/// \author Runik
/// \date   27/04/2020
///
/// \param [in,out] state   The emulator state.
/// \param [in,out] opened  If non-null, true if opened.
////////////////////////////////////////////////////////////////////////////////////////////////////

void showMemoryDebugWindow(core::EmulatorContext& state, bool* opened);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn void showVdp1DebugWindow(core::EmulatorContext& state, bool* opened);
///
/// \brief  Displays the VDP1 debug window.
///
/// \author Runik
/// \date   10/06/2021
///
/// \param [in,out] state   The emulator state.
/// \param [in,out] opened  If non-null, true if opened.
////////////////////////////////////////////////////////////////////////////////////////////////////

void showVdp1DebugWindow(core::EmulatorContext& state, bool* opened);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn void showVdp2DebugWindow(core::EmulatorContext& state, bool* opened);
///
/// \brief  Displays the VDP2 debug window.
///
/// \author Runik
/// \date   21/02/2021
///
/// \param [in,out] state   The emulator state.
/// \param [in,out] opened  If non-null, true if opened.
////////////////////////////////////////////////////////////////////////////////////////////////////

void showVdp2DebugWindow(core::EmulatorContext& state, bool* opened);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn void showSmpcDebugWindow(core::EmulatorContext& state, bool* opened);
///
/// \brief  Displays the SMPC debug window.
///
/// \author Runik
/// \date   21/02/2021
///
/// \param [in,out] state   The emulator state.
/// \param [in,out] opened  If non-null, true if opened.
////////////////////////////////////////////////////////////////////////////////////////////////////

void showSmpcDebugWindow(core::EmulatorContext& state, bool* opened);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn void showTexturesDebugWindow(core::EmulatorContext& state, bool* opened);
///
/// \brief  Displays the textures debug window.
///
/// \author Runik
/// \date   22/04/2022
///
/// \param [in,out] state   The emulator state.
/// \param [in,out] opened  If non-null, true if opened.
////////////////////////////////////////////////////////////////////////////////////////////////////

void showTexturesDebugWindow(core::EmulatorContext& state, bool* opened);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn void showBinaryLoadWindow(core::EmulatorContext& state, bool* opened);
///
/// \brief  Displays the window to load binary files.
///
/// \author Runik
/// \date   12/08/2021
///
/// \param [in,out] state   The emulator state.
/// \param [in,out] opened  If non-null, true if opened.
////////////////////////////////////////////////////////////////////////////////////////////////////

void showBinaryLoadWindow(core::EmulatorContext& state, bool* opened);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn void buildGui(core::EmulatorContext& state);
///
/// \brief  Builds a graphical user interface.
///
/// \author Runik
/// \date   24/10/2019
///
/// \param [in,out] state   The emulator context.
////////////////////////////////////////////////////////////////////////////////////////////////////

void buildGui(core::EmulatorContext& state);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn void addTextureToDrawList(s32 texture, u32 width, u32 height, u8 alpha);
///
/// \brief  Adds the rendered texture to the draw list.
///
/// \author Runik
/// \date   24/10/2019
///
/// \param  texture The texture add.
/// \param  width   Width of the texture.
/// \param  height  Height of the texture.
/// \param  alpha   The alpha.
////////////////////////////////////////////////////////////////////////////////////////////////////

void addTextureToDrawList(s32 texture, u32 width, u32 height, u8 alpha);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn auto getMouseCoordinates(core::EmulatorContext& state) -> Coord;
///
/// \brief  Returns mouse coordinates in the ImGui window
///
/// \author Runik
/// \date   21/01/2021
///
/// \param [in,out] state   The emulator context.
///
/// \returns    The mouse coordinates.
////////////////////////////////////////////////////////////////////////////////////////////////////

auto getMouseCoordinates(core::EmulatorContext& state) -> Coord;

} // namespace saturnin::gui
