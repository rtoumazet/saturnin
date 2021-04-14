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
#include <glm/mat4x4.hpp>
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

constexpr auto minimum_window_width  = u16{512};
constexpr auto minimum_window_height = u16{512};

enum class ShaderName { vertex, fragment };
enum class GlslVersion { glsl_120, glsl_330 };

using ShadersList = std::map<std::pair<GlslVersion, ShaderName>, const char*>;

class Opengl {
  public:
    ///@{
    /// Constructors / Destructors
    Opengl() = delete;
    Opengl(core::Config* config);
    Opengl(const Opengl&) = delete;
    Opengl(Opengl&&)      = delete;
    auto operator=(const Opengl&) & -> Opengl& = delete;
    auto operator=(Opengl&&) & -> Opengl& = delete;
    ~Opengl();
    ///@}

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Opengl::initialize();
    ///
    /// \brief  Initializes this object
    ///
    /// \author Runik
    /// \date   08/04/2021
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void initialize();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Opengl::shutdown();
    ///
    /// \brief  Shuts down this object and frees any resources it is using
    ///
    /// \author Runik
    /// \date   08/04/2021
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void shutdown();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Opengl::preRender();
    ///
    /// \brief  Processing done before rendering.
    ///
    /// \author Runik
    /// \date   08/04/2021
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void preRender();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Opengl::render();
    ///
    /// \brief  Rendering of data.
    ///
    /// \author Runik
    /// \date   08/04/2021
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void render();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Opengl::postRender();
    ///
    /// \brief  Processing done after rendering.
    ///
    /// \author Runik
    /// \date   08/04/2021
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void postRender();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Opengl::initializeShaders();
    ///
    /// \brief  Initializes the shaders.
    ///
    /// \author Runik
    /// \date   08/04/2021
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void initializeShaders();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Opengl::displayFramebuffer(core::EmulatorContext& state);
    ///
    /// \brief  Displays the framebuffer content (VDP1 + VDP2)
    ///
    /// \author Runik
    /// \date   27/01/2021
    ///
    /// \param [in,out] state   The state.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void displayFramebuffer(core::EmulatorContext& state);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto Opengl::getRenderingTexture() const -> const u32
    ///
    /// \brief  Gets the texture
    ///
    /// \author Runik
    /// \date   06/02/2021
    ///
    /// \returns    An u32.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    [[nodiscard]] auto getRenderingTexture() const -> const u32 { return this->rendering_texture_; };

    void renderingTexture(u32 texture) { this->rendering_texture_ = texture; };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Opengl::onWindowResize(const u16 width, const u16 height);
    ///
    /// \brief  Actions executed on window resize.
    ///
    /// \author Runik
    /// \date   08/02/2021
    ///
    /// \param  width   The new window width.
    /// \param  height  The new window height.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void onWindowResize(const u16 width, const u16 height);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Opengl::updateScreenResolution();
    ///
    /// \brief  Updates the screen resolution
    ///
    /// \author Runik
    /// \date   10/02/2021
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void updateScreenResolution();

    ///@{
    /// Getters / Setters
    void saturnScreenResolution(const ScreenResolution& res) { saturn_screen_resolution_ = res; };
    auto saturnScreenResolution() const -> ScreenResolution { return saturn_screen_resolution_; };
    void hostScreenResolution(const ScreenResolution& res) { host_screen_resolution_ = res; };
    auto hostScreenResolution() const -> ScreenResolution { return host_screen_resolution_; };
    ///@}

    auto        createVertexShader() -> u32;
    auto        createFragmentShader() -> u32;
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

  private:
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto Opengl::getShaderSource(const ShaderName name) -> const char*;
    ///
    /// \brief  Gets the shader source depending on the configuration
    ///
    /// \author Runik
    /// \date   08/04/2021
    ///
    /// \param  name    The shader name.
    ///
    /// \returns    Null if it fails, else the shader source.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto getShaderSource(const ShaderName name) -> const char*;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static auto Opengl::generateTexture(u32 width, u32 height, const std::vector<u8>& data) -> u32;
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

    [[nodiscard]] static auto generateTexture(u32 width, u32 height, const std::vector<u8>& data) -> u32;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto Opengl::calculateDisplayViewportMatrix() -> glm::highp_mat4;
    ///
    /// \brief  Calculates the display viewport matrix, adding letterbox or pillarbox when the display isn't exactly like the
    /// Saturn resolution.
    ///
    /// \author Runik
    /// \date   10/04/2021
    ///
    /// \returns    The calculated display viewport matrix.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto calculateDisplayViewportMatrix() -> glm::highp_mat4;

    core::Config* config_; ///< Configuration object

    u32  rendering_texture_{}; ///< Destination texture for render to texture.
    bool is_legacy_opengl_{};  ///< True if rendering in legacy opengl.

    ScreenResolution saturn_screen_resolution_{}; ///< Saturn screen resolution.
    ScreenResolution host_screen_resolution_{};   ///< Host screen resolution.

    u32 saturn_framebuffer_{}; ///< Framebuffer object used as Saturn's framebuffer. Will be rendered to a texture.

    u32                 program_shader_;
    u32                 vao_;
    u32                 fbo_{};    ///< Framebuffer Object used for rendering to texture.
    std::vector<Vertex> vertexes_; ///< Contains the geometry vertexes ready to be used in a buffer array for display

    u32 texture_;

    ShadersList shaders_list_;

    // std::vector<s16> vertexes_; ///< Contains the geometry vertexes ready to be used in a buffer array for display.
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
/// \param [in,out] window  If non-null, a pointer to the GLFW window.
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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn void checkShaderCompilation(const u32 shader);
///
/// \brief  Checks shader compilation and displays errors when detected.
///
/// \author Runik
/// \date   22/10/2019
///
/// \param  shader  The shader to check.
////////////////////////////////////////////////////////////////////////////////////////////////////

void checkShaderCompilation(u32 shader);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn void checkProgramCompilation(const u32 program);
///
/// \brief  Checks program compilation and displays errors when detected.
///
/// \author Runik
/// \date   22/10/2019
///
/// \param  program The program to check.
////////////////////////////////////////////////////////////////////////////////////////////////////

void checkProgramCompilation(u32 program);

}; // namespace saturnin::video
