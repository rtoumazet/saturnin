// 
// opengl.h
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
/// \file	opengl.h
///
/// \brief	Declares the OpenGl class and related functions. 
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <windows.h> // removes C4005 warning
#include <cstdint>
#include <vector>
#include "../../lib/imgui/imgui_loader.h"
#include <GLFW/glfw3.h>

#include "../config.h"
#include "../emulator_context.h"

namespace saturnin {
namespace video {

class Opengl {

public:
    //@{
    // Constructors / Destructors
    Opengl()                           = delete;
    Opengl(core::Config* config);
    Opengl(const Opengl&)              = delete;
    Opengl(Opengl&&)                   = delete;
    Opengl& operator=(const Opengl&) & = delete;
    Opengl& operator=(Opengl&&) &      = delete;
    ~Opengl()                          = default;
    //@}

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn uint32_t Opengl::createFramebuffer();
    ///
    /// \brief  Creates the framebuffer used to display the Saturn data.
    ///
    /// \author Runik
    /// \date   09/10/2018
    ///
    /// \return The new framebuffer.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

	uint32_t createFramebuffer();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Opengl::preRendering(uint32_t& fbo);
    ///
    /// \brief  Pre rendering functions.
    ///
    /// \author Runik
    /// \date   09/10/2018
    ///
    /// \param [in,out] fbo The framebuffer used for rendering.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void preRendering(uint32_t& fbo);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Opengl::postRendering();
    ///
    /// \brief  Post rendering functions.
    ///
    /// \author Runik
    /// \date   09/10/2018
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void postRendering();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn int32_t Opengl::bindTextureToFramebuffer();
    ///
    /// \brief  Binds a texture to the current framebuffer.
    ///
    /// \author Runik
    /// \date   09/10/2018
    ///
    /// \return The texture id.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    int32_t bindTextureToFramebuffer();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn int32_t Opengl::calculateLegacyRendering();
    ///
    /// \brief  Calculates the OpenGL rendering in legacy mode.
    ///
    /// \author Runik
    /// \date   09/10/2018
    ///
    /// \return The id of the calculated texture.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    int32_t calculateLegacyRendering();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn int32_t Opengl::calculateModernRendering();
    ///
    /// \brief  Calculates the OpenGL rendering in modern modern.
    ///
    /// \author Runik
    /// \date   09/10/2018
    ///
    /// \return The id of the calculated texture.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    int32_t calculateModernRendering();

    uint32_t createVertexShader();
    uint32_t createFragmentShader();
    //uint32_t create_vertex_buffer_object(const float vertices[]);
    uint32_t createProgramShader(const uint32_t vertex_shader, const uint32_t fragment_shader);
    //uint32_t createVertexArrayObject(const uint32_t vertex_buffer_object, const float vertices[]);
    void deleteShaders(std::vector<uint32_t> shaders);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Opengl::setupTriangle();
    ///
    /// \brief  Sets up the triangle using shaders using modern OpenGL.
    ///
    /// \author Runik
    /// \date   09/10/2018
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void setupTriangle();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Opengl::drawTriangle();
    ///
    /// \brief  Draws a triangle using modern OpenGL.
    ///
    /// \author Runik
    /// \date   09/10/2018
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void drawTriangle();

    std::function<int32_t(void)> calculateRendering;    ///< function pointer to the rendering function

private:
    
    uint32_t      program_shader_;
    uint32_t      vao_;
    core::Config* config_;        ///< Configuration object
};


////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn bool OpenGl::is_modern_opengl_capable();
///
/// \brief  Query if the current video card is capable of rendering modern opengl (ie version
///         3.3+).
///
/// \author Runik
/// \date   24/12/2017
///
/// \return True if the video car can render modern OpenGL .
////////////////////////////////////////////////////////////////////////////////////////////////////

bool isModernOpenglCapable();

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn int32_t runLegacyOpengl(core::Emulator_context& config);
///
/// \brief  Executes OpenGL code in legacy mode.
///
/// \author Runik
/// \date   03/04/2018
///
/// \param [in,out] config  A reference to the configuration object.
///
/// \return An int32_t.
////////////////////////////////////////////////////////////////////////////////////////////////////

int32_t runLegacyOpengl(core::Emulator_context& state);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn int32_t runModernOpengl(core::Emulator_context& config);
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

int32_t runModernOpengl(core::Emulator_context& state);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn void windowCloseCallback(GLFWwindow* window);
///
/// \brief  Callback, called when the rendering window closes.
///
/// \author Runik
/// \date   13/10/2018
///
/// \param [in,out] window  If non-null, the window.
////////////////////////////////////////////////////////////////////////////////////////////////////

void windowCloseCallback(GLFWwindow* window);

bool loadPngImage(const std::vector<uint8_t>& source_data, std::vector<uint8_t>& image);

bool loadIcons(std::vector<uint8_t>& image);

};
};