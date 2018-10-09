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

//#include "imgui/imgui.h"
#include <vector>
#include "opengl.h"
#include "../config.h"

namespace video = saturnin::video;

namespace saturnin {
namespace gui {

    void show_test_window(bool& show_test_window);
    void show_core_window(const uint32_t tex);
    void showRenderingWindow(video::Opengl& opengl, uint32_t fbo, uint32_t width, uint32_t height);
    void showStvWindow(bool *opened);
    void showOptionsWindow(std::shared_ptr<core::Config>&, bool *opened);
    void buildGui(std::shared_ptr<core::Config>&, video::Opengl& opengl, uint32_t fbo, uint32_t width, uint32_t height);
    void renderToTexture(int32_t texture, const uint32_t width, const uint32_t height);
}
}