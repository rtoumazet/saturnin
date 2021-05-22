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
#include <mutex>  // mutex
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

// Forward declaration
enum class DrawType;
class BaseRenderingPart;

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

    ///@{
    /// Accessors / Mutators
    [[nodiscard]] auto displayedTexture() const { return fbo_textures_[displayed_texture_index_]; };
    void               displayedTexture(u32 index) { displayed_texture_index_ = index; };
    [[nodiscard]] auto guiRenderingContext() const { return gui_rendering_context_; };
    void               guiRenderingContext(GLFWwindow* context) { gui_rendering_context_ = context; };
    //[[nodiscard]] auto emulatorRenderingContext() const { return emulator_rendering_context_; };
    // void               emulatorRenderingContext(GLFWwindow* context) { emulator_rendering_context_ = context; };
    [[nodiscard]] auto fps() const { return fps_; };
    void               fps(std::string fps) { fps_ = fps; };

    ///@}

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Opengl::initialize(GLFWwindow* gui_context);
    ///
    /// \brief  Initializes this object
    ///
    /// \author Runik
    /// \date   08/04/2021
    ///
    /// \param [in,out] gui_context If non-null, context for the graphical user interface.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void initialize(GLFWwindow* gui_context);

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

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto Opengl::createVertexShader() -> u32;
    ///
    /// \brief  Creates a vertex shader.
    ///
    /// \author Runik
    /// \date   26/04/2021
    ///
    /// \returns    The vertex shader id.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto createVertexShader() -> u32;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto Opengl::createFragmentShader() -> u32;
    ///
    /// \brief  Creates a fragment shader.
    ///
    /// \author Runik
    /// \date   26/04/2021
    ///
    /// \returns    The fragment shader id.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto createFragmentShader() -> u32;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static auto Opengl::createProgramShader(u32 vertex_shader, u32 fragment_shader) -> u32;
    ///
    /// \brief  Creates a program shader.
    ///
    /// \author Runik
    /// \date   26/04/2021
    ///
    /// \param  vertex_shader   The vertex shader.
    /// \param  fragment_shader The fragment shader.
    ///
    /// \returns    The program shader id.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static auto createProgramShader(u32 vertex_shader, u32 fragment_shader) -> u32;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static void Opengl::deleteShaders(std::vector<u32> shaders);
    ///
    /// \brief  Deletes the shaders.
    ///
    /// \author Runik
    /// \date   26/04/2021
    ///
    /// \param  shaders The shaders to delete.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static void deleteShaders(std::vector<u32> shaders);

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
    /// \returns    The OpenGL id of the generated texture.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    [[nodiscard]] static auto generateTexture(u32 width, u32 height, const std::vector<u8>& data) -> u32;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static void Opengl::deleteTexture(const u32 texture);
    ///
    /// \brief  Deletes the texture described by key on the GPU.
    ///
    /// \author Runik
    /// \date   14/04/2021
    ///
    /// \param  texture The OpenGL texture identifier.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static void deleteTexture(const u32 texture);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Opengl::renderBatch(const DrawType type, const std::vector<Vertex>& draw_list, const std::vector<u32>&
    /// textures_list);
    ///
    /// \brief  Renders a batch of vertexes of the specified type.
    ///
    /// \author Runik
    /// \date   16/04/2021
    ///
    /// \param  type            The type of data to draw.
    /// \param  draw_list       The vertexes to draw.
    /// \param  textures_list   List of textures.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void renderBatch(const DrawType type, const std::vector<Vertex>& draw_list, const std::vector<u32>& textures_list);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Opengl::initializeRenderingContext();
    ///
    /// \brief  Initializes the rendering context.
    ///
    /// \author Runik
    /// \date   26/04/2021
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    // void initializeRenderingContext();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Opengl::destroyRenderingContext();
    ///
    /// \brief  Destroys the rendering context.
    ///
    /// \author Runik
    /// \date   26/04/2021
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    // void destroyRenderingContext();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Opengl::makeRenderingContextCurrent();
    ///
    /// \brief  Makes rendering context current.
    ///
    /// \author Runik
    /// \date   26/04/2021
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    // void makeRenderingContextCurrent();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto Opengl::areFbosInitialized() ->bool
    ///
    /// \brief  Are framebuffer objects initialized.
    ///
    /// \author Runik
    /// \date   26/04/2021
    ///
    /// \returns    True if framebuffer objects are initialized.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto areFbosInitialized() -> bool { return !fbos_.empty(); }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Opengl::render();
    ///
    /// \brief  Renders data if available
    ///
    /// \author Runik
    /// \date   21/05/2021
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void render();

    auto isThereSomethingToRender() -> const bool;

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

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static auto Opengl::initializeVao(const std::vector<Vertex>& vertexes) -> u32;
    ///
    /// \brief  Initializes the vao based on the vertexes.
    ///
    /// \author Runik
    /// \date   16/04/2021
    ///
    /// \param  vertexes    The vertexes.
    ///
    /// \returns    The generated VAO id.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static auto initializeVao(const std::vector<Vertex>& vertexes) -> u32;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Opengl::initializeFbos();
    ///
    /// \brief  Initializes the framebuffer objects.
    ///
    /// \author Runik
    /// \date   25/04/2021
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void initializeFbos();

    void calculateFps();

    core::Config* config_;                ///< Configuration object.
    GLFWwindow*   gui_rendering_context_; ///< Context used for GUI rendering.
    // GLFWwindow*   emulator_rendering_context_; ///< Context used for  emulator rendering.

    u32              displayed_texture_index_{}; ///< Index of complete texture displayed by the GUI, will be one of fbo_texture.
    std::vector<u32> fbos_;                      ///< The framebuffer objects used for rendering.
    std::vector<u32> fbo_textures_;              ///< The textures used by the framebuffer objects.
    bool             is_legacy_opengl_{};        ///< True if rendering in legacy opengl.

    ScreenResolution saturn_screen_resolution_{}; ///< Saturn screen resolution.
    ScreenResolution host_screen_resolution_{};   ///< Host screen resolution.

    std::vector<std::unique_ptr<video::BaseRenderingPart>> ///< List of parts
               parts_list_;                                // Will have to be moved to the platform agnostic renderer.
    std::mutex parts_list_mutex_; ///< Prevents rendering thread to use the list while it's being processed.

    u32         program_shader_; ///< The program shader.
    ShadersList shaders_list_;   ///< List of shaders.

    std::string fps_; ///< The frames per second.
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
