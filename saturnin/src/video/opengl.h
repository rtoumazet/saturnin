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
#include <map>    // map
#include <string> // string
#include <saturnin/src/emulator_defs.h>

// Forward declarations
namespace saturnin::core {
class Config;
class EmulatorContext;
} // namespace saturnin::core
struct GLFWwindow;
struct GLFWimage;

namespace saturnin::video {

using saturnin::core::Config;

constexpr auto minimum_window_width  = u16{320};
constexpr auto minimum_window_height = u16{224};

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

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Opengl::displayFramebuffer();
    ///
    /// \brief  Displays the framebuffer content (VDP1 + VDP2)
    ///
    /// \author Runik
    /// \date   27/01/2021
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void displayFramebuffer();

    //@{
    // Abstract functions
    [[nodiscard]] virtual auto generateEmptyTexture(u32 width, u32 height) const -> u32 = 0;
    // virtual void               updateTextureSize(u32 width, u32 height)                 = 0;
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

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto Opengl::texture() const -> u32
    ///
    /// \brief  Gets the texture
    ///
    /// \author Runik
    /// \date   06/02/2021
    ///
    /// \returns    An u32.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    [[nodiscard]] auto renderingTexture() const -> const u32 { return this->rendering_texture_; };

    void renderingTexture(u32 texture) { this->rendering_texture_ = texture; };

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
    /// \fn auto Opengl::getIconTexture(IconId id) -> ImTextureID;
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

    [[nodiscard]] auto getIconTexture(IconId id) -> ImTextureID;

    static auto loadPngImage(const std::vector<uint8_t>& source_data, std::vector<uint8_t>& image) -> bool;

    static auto loadIcons(std::vector<uint8_t>& image) -> u32;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn virtual void Opengl::onWindowResize(u16 width, u16 height) = 0;
    ///
    /// \brief  Actions executed on window resize.
    ///
    /// \author Runik
    /// \date   08/02/2021
    ///
    /// \param  width   The new window width.
    /// \param  height  The new window height.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    virtual void onWindowResize(u16 width, u16 height) = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Opengl::updateScreenResolution();
    ///
    /// \brief  Updates the screen resolution
    ///
    /// \author Runik
    /// \date   10/02/2021
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void updateScreenResolution();

    //@{
    // Getters / Setters
    void saturnScreenResolution(const ScreenResolution& res) { saturn_screen_resolution_ = res; };
    auto saturnScreenResolution() const -> ScreenResolution { return saturn_screen_resolution_; };
    void hostScreenResolution(const ScreenResolution& res) { host_screen_resolution_ = res; };
    auto hostScreenResolution() const -> ScreenResolution { return host_screen_resolution_; };
    //@}

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

    // void setTextureDimension(u32 width, u32 height);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn virtual auto Opengl::generateTextureFromVector(const u32 width, const u32 height, const std::vector<u8>& data) const
    /// -> u32 = 0;
    ///
    /// \brief  Generates a texture from a vector.
    ///
    /// \author Runik
    /// \date   22/04/2020
    ///
    /// \param  width   Width of the texture.
    /// \param  height  Height of the texture.
    /// \param  data    Texture data.
    ///
    /// \returns    The id of the generated texture.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    [[nodiscard]] virtual auto generateTextureFromVector(u32 width, u32 height, const std::vector<u8>& data) const -> u32;

    u32 fbo_{}; ///< Framebuffer Object used for rendering to texture.

  private:
    //@{
    // Abstract functions
    virtual void initialize() = 0;
    virtual void shutdown()   = 0;
    virtual void preRender()  = 0;
    virtual void render()     = 0;
    virtual void postRender() = 0;
    //@}

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

    // uint32_t      program_shader_;
    // uint32_t      vao_;
    core::Config* config_; ///< Configuration object

    u32 rendering_texture_{}; ///< Destination texture for render to texture.

    std::map<IconId, u32> icons_map_; ///< The icons map.

    ScreenResolution saturn_screen_resolution_{}; ///< Saturn screen resolution.
    ScreenResolution host_screen_resolution_{};   ///< Host screen resolution.
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
/// \param [in,out] window  If non-null, a pointer to the GLFW window.
////////////////////////////////////////////////////////////////////////////////////////////////////

void windowCloseCallback(GLFWwindow* window);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn void windowSizeCallback(GLFWwindow* window, int width, int height);
///
/// \brief  Callback, called when the window is resized.
///
/// \author Runik
/// \date   08/02/2021
///
/// \param [in,out] window  If non-null, a pointer to the GLFW window..
/// \param          width   The width.
/// \param          height  The height.
////////////////////////////////////////////////////////////////////////////////////////////////////

void windowSizeCallback(GLFWwindow* window, int width, int height);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn auto runOpengl(EmulatorContext& state) -> s32;
///
/// \brief  Executes OpenGL code.
///
/// \author Runik
/// \date   03/04/2018
///
/// \param [in,out] state   A reference to the configuration object.
///
/// \returns    An int32_t.
////////////////////////////////////////////////////////////////////////////////////////////////////

auto runOpengl(core::EmulatorContext& state) -> s32;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn void updateMainWindowSizeAndRatio(GLFWwindow* window, u32 width, u32 height);
///
/// \brief  Updates the main window size and ratio aspect.
///
/// \author Runik
/// \date   24/01/2021
///
/// \param [in,out] window  If non-null, a pointer to the GLFW window.
/// \param          width   The new window width.
/// \param          height  The new window height.
////////////////////////////////////////////////////////////////////////////////////////////////////

void updateMainWindowSizeAndRatio(GLFWwindow* window, u32 width, u32 height);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn auto createMainWindow(u32 width, u32 height, std::string title) -> GLFWwindow*;
///
/// \brief  Creates the main window.
///
/// \author Runik
/// \date   25/01/2021
///
/// \param  width   The window's width.
/// \param  height  The window's height.
/// \param  title   The window's title.
///
/// \returns    Null if it fails, else the new main window.
////////////////////////////////////////////////////////////////////////////////////////////////////

auto createMainWindow(u32 width, u32 height, std::string title) -> GLFWwindow*;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn auto loadPngImage(const char* filename) -> GLFWimage;
///
/// \brief  Loads PNG image
///
/// \author Runik
/// \date   08/02/2021
///
/// \param  filename    Filename of the file.
///
/// \returns    The PNG image.
////////////////////////////////////////////////////////////////////////////////////////////////////

auto loadPngImage(const char* filename) -> GLFWimage;

}; // namespace saturnin::video
