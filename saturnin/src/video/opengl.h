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
#include <imgui.h>
#include <map> // map
#include <string>
#include <vector>
#include "../emulator_defs.h"
//#include "../../lib/imgui/imgui_loader.h"

// Forward declarations
namespace saturnin::core {
class Config;
}
struct GLFWwindow;

namespace saturnin::video {

using saturnin::core::Config;

enum class IconId { play, pause, stop, step_into, step_over, step_out, config, file, debug };

class Opengl {
  public:
    //@{
    // Constructors / Destructors
    Opengl() = delete;
    Opengl(core::Config* config);
    Opengl(const Opengl&) = delete;
    Opengl(Opengl&&)      = delete;
    auto operator=(const Opengl&) & -> Opengl& = delete;
    auto operator=(Opengl&&) & -> Opengl& = delete;
    virtual ~Opengl()                     = default;
    //@}

    //@{
    // Abstract functions
    virtual void               initialize()                                             = 0;
    virtual void               shutdown()                                               = 0;
    virtual void               preRender()                                              = 0;
    virtual void               render()                                                 = 0;
    virtual void               postRender()                                             = 0;
    [[nodiscard]] virtual auto generateEmptyTexture(u32 width, u32 height) const -> u32 = 0;
    virtual void               updateTextureSize(u32 width, u32 height)                 = 0;
    virtual void               deleteTexture() const                                    = 0;
    //@}

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn u32 OpenglLegacy::bindTextureToFbo() const abstract;
    ///
    /// \brief  Binds the internal texture to the internal fbo.
    ///
    /// \author Runik
    /// \date   18/10/2019
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    virtual void bindTextureToFbo() const = 0;

    [[nodiscard]] auto texture() const -> u32 { return this->texture_; };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn [[nodiscard]] auto Opengl::isWindowResized(const u32 new_width, const u32 new_height) const -> bool;
    ///
    /// \brief  Checks if the window size has changed.
    ///
    /// \author Runik
    /// \date   18/10/2019
    ///
    /// \return True if the window has been resized.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    [[nodiscard]] auto isWindowResized(u32 new_width, u32 new_height) const -> bool;

    void initializeTexture(u32 width, u32 height);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto Opengl::generateTextureFromFile(const std::string& filename) const -> u32;
    ///
    /// \brief  Generates a texture from a png file located in the ./res directory.
    ///
    /// \author Runik
    /// \date   22/04/2020
    ///
    /// \param  filename    Name of the file to generate the texture from.
    ///
    /// \returns    The id of the generated texture.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    [[nodiscard]] auto generateTextureFromFile(const std::string& filename) const -> u32;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto Opengl::getIconTexture(const IconId id) -> ImTextureID;
    ///
    /// \brief  Gets the texture id of the IconId.
    ///
    /// \author Runik
    /// \date   22/04/2020
    ///
    /// \param  id  The icon identifier.
    ///
    /// \returns    The icon texture id.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    [[nodiscard]] auto getIconTexture(const IconId id) -> ImTextureID;

    static auto loadPngImage(const std::vector<uint8_t>& source_data, std::vector<uint8_t>& image) -> bool;

    static auto loadIcons(std::vector<uint8_t>& image) -> u32;

    static auto generateIconsTexture() -> u32;

    uptr iconsTextureId; ///< Texture id storing data for UI icons

  protected:
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn [[nodiscard]] auto Opengl::config() const -> Config*;
    ///
    /// \brief  Returns the Configobject.
    ///
    /// \author Runik
    /// \date   12/10/2019
    ///
    /// \return The Config object.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    [[nodiscard]] auto config() const -> Config*;

    void setTextureDimension(u32 width, u32 height);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn virtual auto Opengl::generateTextureFromVector(const u32 width, const u32 height, std::vector<u8>& data) const -> u32
    /// = 0;
    ///
    /// \brief  Generates a texture from a vector.
    ///
    /// \author Runik
    /// \date   22/04/2020
    ///
    /// \param          width   Width of the texture.
    /// \param          height  Height of the texture.
    /// \param [in,out] data    Texture data.
    ///
    /// \returns    The id of the generated texture.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    virtual auto generateTextureFromVector(const u32 width, const u32 height, const std::vector<u8>& data) const -> u32 = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto Opengl::generateUiIcons() -> bool;
    ///
    /// \brief  Generates the user interface icons.
    ///
    /// \author Runik
    /// \date   22/04/2020
    ///
    /// \returns    True if it succeeds.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto generateUiIcons() -> bool;

    std::map<IconId, u32> icons_map_;

    u32 fbo_{};     ///< Framebuffer Object used for rendering to texture.
    u32 texture_{}; ///< Destination texture for render to texture.

    u32 current_texture_width_{};  ///< Width of the texture
    u32 current_texture_height_{}; ///< Height of the texture

  private:
    // uint32_t      program_shader_;
    // uint32_t      vao_;
    core::Config* config_; ///< Configuration object
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn auto OpenGl::is_modern_opengl_capable() -> bool;
///
/// \brief  Query if the current video card is capable of rendering modern opengl (ie version
///         3.3+).
///
/// \author Runik
/// \date   24/12/2017
///
/// \return True if the video car can render modern OpenGL .
////////////////////////////////////////////////////////////////////////////////////////////////////

auto isModernOpenglCapable() -> bool;

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

}; // namespace saturnin::video
