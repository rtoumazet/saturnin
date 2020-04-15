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
#include "opengl.h"
#include "../emulator_context.h"

namespace video = saturnin::video;

namespace saturnin::gui {

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
/// \fn void showCoreWindow(core::EmulatorContext& state, const video::Opengl& opengl);
///
/// \brief  Displays the core window.
///
/// \author Runik
/// \date   24/10/2019
///
/// \param [in,out] state   The emulator context.
/// \param          opengl  The opengl object.
////////////////////////////////////////////////////////////////////////////////////////////////////

void showCoreWindow(core::EmulatorContext& state, const video::Opengl& opengl);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn void showRenderingWindow(video::Opengl& opengl, u32 width, u32 height);
///
/// \brief  Displays the rendering window.
///
/// \author Runik
/// \date   24/10/2019
///
/// \param [in,out] opengl  The opengl object.
/// \param          width   Width of the rendering window.
/// \param          height  Height of the rendering window.
////////////////////////////////////////////////////////////////////////////////////////////////////

void showRenderingWindow(video::Opengl& opengl, u32 width, u32 height);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn void showStvWindow(bool* opened);
///
/// \brief  Displays the ST-V games choice window.
///
/// \author Runik
/// \date   24/10/2019
///
/// \param [in,out] opened  True to display the window.
////////////////////////////////////////////////////////////////////////////////////////////////////

void showStvWindow(bool* opened);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn void showOptionsWindow(core::EmulatorContext& state, bool* opened);
///
/// \brief  Displays the options window.
///
/// \author Runik
/// \date   24/10/2019
///
/// \param [in,out] state   The emulator context.
/// \param [in,out] opened  True to display the window.
////////////////////////////////////////////////////////////////////////////////////////////////////

void showOptionsWindow(core::EmulatorContext& state, bool* opened);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn void showLogWindow(bool* opened);
///
/// \brief  Displays the log window.
///
/// \author Runik
/// \date   24/10/2019
///
/// \param [in,out] opened  True will display the window.
////////////////////////////////////////////////////////////////////////////////////////////////////

void showLogWindow(bool* opened);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn void showDebugSh2Window(core::EmulatorContext& state,bool* opened);
///
/// \brief  Display the debug window for both SH2.
///
/// \author Runik
/// \date   06/04/2020
///
/// \param [in,out] state   The emulator context.
/// \param [in,out] opened  True will display the window.
////////////////////////////////////////////////////////////////////////////////////////////////////

void showDebugSh2Window(core::EmulatorContext& state, bool* opened);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn void buildGui(core::EmulatorContext& state, video::Opengl& opengl, u32 width, u32 height);
///
/// \brief  Builds a graphical user interface.
///
/// \author Runik
/// \date   24/10/2019
///
/// \param [in,out] state   The emulator context.
/// \param [in,out] opengl  The opengl object.
/// \param          width   Width of the rendering window.
/// \param          height  Height of the rendering window.
////////////////////////////////////////////////////////////////////////////////////////////////////

void buildGui(core::EmulatorContext& state, video::Opengl& opengl, u32 width, u32 height);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn void addTextureToDrawList(int32_t texture, uint32_t width, uint32_t height);
///
/// \brief  Adds the rendered texture to the draw list.
///
/// \author Runik
/// \date   24/10/2019
///
/// \param  texture The texture add.
/// \param  width   Width of the texture.
/// \param  height  Height of the texture.
////////////////////////////////////////////////////////////////////////////////////////////////////

void addTextureToDrawList(int32_t texture, uint32_t width, uint32_t height);

} // namespace saturnin::gui
