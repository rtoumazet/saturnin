//
// opengl_utilities.h
// Saturnin
//
// Copyright (c) 2024 Renaud Toumazet
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

#pragma once

#include <saturnin/src/emulator_defs.h>
#include <saturnin/src/video/opengl/opengl.h>
#include <saturnin/src/video/vdp_common.h>

namespace saturnin::video {

// Queries if the current video card is capable of rendering modern opengl (ie version 3.3+).
auto isModernOpenglCapable() -> bool;

// Called when the rendering window closes.
void windowCloseCallback(GLFWwindow* window);

// Called when the window is resized, new size is passed as parameters.
void windowSizeCallback(GLFWwindow* window, int width, int height);

// OpenGL main loop.
auto runOpengl(core::EmulatorContext& state) -> s32;

// Updates the main window size and ratio aspect.
void updateMainWindowSizeAndRatio(GLFWwindow* window, u32 width, u32 height);

// Creates the main window, returns null if it fails, or the new main window.
auto createMainWindow(u32 width, u32 height, const std::string& title) -> GLFWwindow*;

// Functions to load PNG images.
auto loadPngImage(const char* filename) -> GLFWimage;
auto loadPngImage(const u8* data, const size_t size) -> GLFWimage;

//---------------
// Error handling
//---------------

// Checks if an OpenGL error was raised
void checkGlError();

// Formats an OpenGL message and sends it to the log.
void glDebugOutput(gl::GLenum                   source,
                   gl::GLenum                   type,
                   unsigned int                 id,
                   gl::GLenum                   severity,
                   [[maybe_unused]] int         length,
                   const char*                  message,
                   [[maybe_unused]] const void* userParam);

// Callback for OpenGL errors
void errorCallback(int error, const char* description);

// Generates vertex indices and draw ranges that will be used to batch draw from the list of parts.
auto generateVertexIndicesAndDrawRanges(const PartsList& parts) -> std::tuple<std::vector<u32>, std::vector<DrawRange>>;

// Initialize a VAO and returns the generated VAO id and the vertex buffer id.
auto initializeVao() -> std::tuple<u32, u32>;
}; // namespace saturnin::video
