//
// opengl_modern.h
// Saturnin
//
// Copyright (c) 2019 Renaud Toumazet
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
/// \file	opengl_modern.h
///
/// \brief	OpenGL class and functions for modern code  (Version 3+).
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <saturnin/src/emulator_defs.h>
#include <saturnin/src/video/opengl.h>

// Forward declarations
namespace saturnin::core {
class Config;
class EmulatorContext;
} // namespace saturnin::core

namespace saturnin::video {

using saturnin::core::Config;
using saturnin::core::EmulatorContext;

class OpenglModern final : public Opengl {
  public:
    //@{
    // Constructors / Destructors
    OpenglModern(core::Config* config) : Opengl(config) { initialize(); };
    ~OpenglModern() final { shutdown(); };
    //@}

    static auto createVertexShader() -> u32;
    static auto createFragmentShader() -> u32;
    static auto createProgramShader(u32 vertex_shader, u32 fragment_shader) -> u32;
    static void deleteShaders(std::vector<u32> shaders);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Opengl::setupTriangle();
    ///
    /// \brief  Sets up the triangle using shaders using modern OpenGL.
    ///
    /// \author Runik
    /// \date   12/10/2019
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void setupTriangle();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Opengl::drawTriangle();
    ///
    /// \brief  Draws a triangle using modern OpenGL.
    ///
    /// \author Runik
    /// \date   12/10/2019
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void drawTriangle();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void initialize() final;
    ///
    /// \brief  Initializes the object.
    ///
    /// \author Runik
    /// \date   16/10/2019
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void initialize() final;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void shutdown() final;
    ///
    /// \brief  Clean up code.
    ///
    /// \author Runik
    /// \date   16/10/2019
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void shutdown() final;

    void preRender() final;

    void render() final;

    void postRender() final;

  private:
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn [[nodiscard]] auto OpenglModern::generateEmptyTexture(u32 width, u32 height) const -> u32 final;
    ///
    /// \brief  Generates an empty texture.
    ///
    /// \author Runik
    /// \date   17/10/2019
    ///
    /// \param  width   Width of the generated texture.
    /// \param  height  Height of the generated texture.
    ///
    /// \return The empty texture.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    //[[nodiscard]] auto generateEmptyTexture(u32 width, u32 height) const -> u32 final;

    u32 saturn_framebuffer_{}; ///< Framebuffer object used as Saturn's framebuffer. Will be rendered to a texture.

    u32 program_shader_;
    u32 vao_;
    u32 texture_;
};

}; // namespace saturnin::video
