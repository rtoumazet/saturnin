//
// opengl_utilities.cpp
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

#include <saturnin/src/pch.h>
#include <saturnin/src/video/opengl/opengl.h>

#include <glbinding/glbinding.h>
#include <glbinding/Version.h>
#include <glbinding-aux/ContextInfo.h>
#include <glbinding/gl21/gl.h>
#include <glbinding/gl21ext/gl.h>
#include <glbinding/gl33core/gl.h>
#include <glbinding/gl33ext/gl.h>

namespace saturnin::video {

using namespace gl21;
using namespace gl21ext;

using core::tr;

void windowSizeCallback(GLFWwindow* window, const int width, const int height) {
    const auto state = std::bit_cast<core::EmulatorContext*>(glfwGetWindowUserPointer(window));
    state->opengl()->onWindowResize(static_cast<u16>(width), static_cast<u16>(height));
}

} // namespace saturnin::video
