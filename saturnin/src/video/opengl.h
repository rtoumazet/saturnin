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
//#include <functional> // function
//#include <cstdint>
#include <vector>
#include"../emulator_defs.h"
//#include "../../lib/imgui/imgui_loader.h"

// Forward declarations
namespace saturnin::core {
    class Config;
}
struct GLFWwindow;

namespace saturnin {
namespace video {

using saturnin::core::Config;


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

    virtual void initialize(){};
    virtual void preRender() abstract;
    virtual s32 render() abstract;
    virtual void postRender() abstract;

    bool loadPngImage(const std::vector<uint8_t>& source_data, std::vector<uint8_t>& image);

    uint32_t loadIcons(std::vector<uint8_t>& image);

    uint32_t generateIconsTexture();

    uint32_t     iconsTextureId;    ///< Texture id storing data for UI icons

protected:
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn Config* Opengl::config() const;
    ///
    /// \brief  Returns the Configobject.
    ///
    /// \author Runik
    /// \date   12/10/2019
    ///
    /// \return The Config object.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    Config* config() const;
    
    u32 fbo_{};     ///< Framebuffer Object used for rendering to texture.
    u32 texture_{}; ///< Destination texture for render to texture.

private:
    
    //uint32_t      program_shader_;
    //uint32_t      vao_;
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

};
};