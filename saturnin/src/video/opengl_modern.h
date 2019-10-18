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

#include "opengl.h"
#include "../emulator_defs.h"

// Forward declarations
namespace saturnin::core {
    class Config;
    class Emulator_context;
}

namespace saturnin {
namespace video {

using saturnin::core::Config;
using saturnin::core::Emulator_context;

class OpenglModern : public Opengl {

public:
    //@{
    // Constructors / Destructors
    OpenglModern(core::Config* config) : Opengl(config) {};
    //@}

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn u32 OpenglModern::createFramebuffer();
    ///
    /// \brief  Creates the framebuffer used to display the Saturn data.
    ///
    /// \author Runik
    /// \date   12/10/2019
    ///
    /// \return The new framebuffer.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    u32 createFramebuffer();

    u32 createVertexShader();
    u32 createFragmentShader();
    u32 createProgramShader(const u32 vertex_shader, const u32 fragment_shader);
    void deleteShaders(std::vector<u32> shaders);

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
    /// \fn s32 Opengl::bindTextureToFramebuffer();
    ///
    /// \brief  Binds a texture to the current framebuffer.
    ///
    /// \author Runik
    /// \date   12/10/2019
    ///
    /// \return The texture id.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    s32 bindTextureToFramebuffer();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void initialize() override;
    ///
    /// \brief  Initializes the object.
    ///
    /// \author Runik
    /// \date   16/10/2019
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void initialize() override;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void shutdown() override;
    ///
    /// \brief  Clean up code.
    ///
    /// \author Runik
    /// \date   16/10/2019
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void shutdown() override;
    
    void preRender();

    void render();

    void postRender();

private:

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn u32 OpenglModern::generateEmptyTexture(const u32 width, const u32 height) const override;
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

    u32 generateEmptyTexture(const u32 width, const u32 height) const override;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn u32 OpenglLegacy::bindTextureToFbo() const override;
    ///
    /// \brief  Binds the internal texture to the internal fbo.
    ///
    /// \author Runik
    /// \date   18/10/2019
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void bindTextureToFbo() const override;

    void deleteTexture() const override;

    u32 program_shader_;
    u32 vao_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn s32 runModernOpengl(core::Emulator_context& config);
///
/// \brief  Executes OpenGL code in modern mode (3.3+).
///
/// \author Runik
/// \date   03/04/2018
///
/// \param [in,out] config  A reference to the configuration object.
///
/// \return An int32_t.
////////////////////////////////////////////////////////////////////////////////////////////////////

s32 runModernOpengl(Emulator_context& state);

};
};