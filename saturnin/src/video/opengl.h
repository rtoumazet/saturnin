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

#include <Windows.h> // removes C4005 warning
#include <imgui.h>
#include <map>    // map
#include <mutex>  // mutex
#include <string> // string
#include <tuple>  // tuple
#include <glm/mat4x4.hpp>
#include <saturnin/src/emulator_defs.h>
#include <saturnin/src/video/vdp1_part.h> // Vdp1Part

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
using utilities::toUnderlying;

constexpr auto minimum_window_width  = u16{512};
constexpr auto minimum_window_height = u16{512};
constexpr auto texture_array_width   = u16{512};
constexpr auto texture_array_height  = u16{512};
constexpr auto texture_array_depth   = u16{128};

enum class FboType : u8 {
    front_buffer,
    back_buffer,
    vdp1_debug_overlay,
    vdp2_debug_layer,
    priority_level_1,
    priority_level_2,
    priority_level_3,
    priority_level_4,
    priority_level_5,
    priority_level_6,
    priority_level_7
};

using FboData = std::pair<u32, u32>; // 1st is fbo id, 2nd is texture id.
using FboList = std::unordered_map<FboType, FboData>;

using PartsList = std::vector<std::unique_ptr<video::BaseRenderingPart>>;

enum class ShaderName { textured };
enum class ShaderType { vertex, fragment };
enum class GlslVersion { glsl_120, glsl_330 };

using ShaderKey   = std::tuple<GlslVersion, ShaderType, ShaderName>;
using ShadersList = std::map<ShaderKey, const char*>;

struct OpenglTexture {
    size_t                          key;                 ///< The Saturn texture key.
    u32                             opengl_id;           ///< Identifier of the OpenGL texture.
    u16                             texture_array_index; ///< The layer (or index) in the texture array.
    Size                            size;                ///< Texture size
    ScreenPos                       pos;                 ///< Position of the texture in the texture atlas.
    std::vector<TextureCoordinates> coords;              ///< The coordinates in the texture atlas
};

using LayerToTextures            = std::unordered_map<Layer, std::vector<OpenglTexture>>;
using LayerToTextureArrayIndexes = std::unordered_map<Layer, std::vector<u8>>;
using LayerToCacheReloadState    = std::unordered_map<Layer, bool>;

using TexturesLink = std::unordered_map<size_t, OpenglTexture>;

class Opengl {
  public:
    ///@{
    /// Constructors / Destructors
    Opengl() = default;
    explicit Opengl(core::Config* config);
    Opengl(const Opengl&)                      = default;
    Opengl(Opengl&&)                           = default;
    auto operator=(const Opengl&) & -> Opengl& = default;
    auto operator=(Opengl&&) & -> Opengl&      = default;
    ~Opengl();
    ///@}

    ///@{
    /// Accessors / Mutators
    [[nodiscard]] auto currentRenderedBuffer() const { return current_rendered_buffer_; };
    void               currentRenderedBuffer(const FboType type) { current_rendered_buffer_ = type; };
    [[nodiscard]] auto vdp1DebugOverlayTextureId() const { return getFboTextureId(FboType::vdp1_debug_overlay); };
    [[nodiscard]] auto vdp2DebugLayerTextureId() const -> u32 { return getFboTextureId(FboType::vdp2_debug_layer); };
    [[nodiscard]] auto fps() const { return fps_; };
    void               fps(std::string_view fps) { fps_ = fps; };
    void               saturnScreenResolution(const ScreenResolution& res) { saturn_screen_resolution_ = res; };
    auto               saturnScreenResolution() const -> ScreenResolution { return saturn_screen_resolution_; };
    void               hostScreenResolution(const ScreenResolution& res) { host_screen_resolution_ = res; };
    auto               hostScreenResolution() const -> ScreenResolution { return host_screen_resolution_; };
    void               partToHighlight(const Vdp1Part& part) { part_to_highlight_ = part; };
    auto               partToHighlight() const -> Vdp1Part { return part_to_highlight_; };

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

    void initialize();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Opengl::shutdown() const;
    ///
    /// \brief  Shuts down this object and frees any resources it is using
    ///
    /// \author Runik
    /// \date   08/04/2021
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void shutdown() const;

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
    /// \fn void Opengl::postRender() const;
    ///
    /// \brief  Processing done after rendering.
    ///
    /// \author Runik
    /// \date   08/04/2021
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void postRender() const;

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
    /// \fn auto Opengl::createVertexShader(const ShaderName name) -> u32;
    ///
    /// \brief  Creates a vertex shader.
    ///
    /// \author Runik
    /// \date   26/04/2021
    ///
    /// \param  name    The vertex shader name.
    ///
    /// \returns    The vertex shader id.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto createVertexShader(const ShaderName name) -> u32;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto Opengl::createFragmentShader(const ShaderName name) -> u32;
    ///
    /// \brief  Creates a fragment shader.
    ///
    /// \author Runik
    /// \date   26/04/2021
    ///
    /// \param  name    The fragment shader name.
    ///
    /// \returns    The fragment shader id.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto createFragmentShader(const ShaderName name) -> u32;

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
    /// \fn	static void Opengl::deleteShaders(const std::vector<u32>& shaders);
    ///
    /// \brief	Deletes the shaders.
    ///
    /// \author	Runik
    /// \date	26/04/2021
    ///
    /// \param 	shaders	The shaders to delete.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static void deleteShaders(const std::vector<u32>& shaders);

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
    /// \fn auto Opengl::areFbosInitialized() const ->bool
    ///
    /// \brief  Are framebuffer objects initialized.
    ///
    /// \author Runik
    /// \date   26/04/2021
    ///
    /// \returns    True if framebuffer objects are initialized.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto areFbosInitialized() const -> bool { return !fbo_list_.empty(); }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Opengl::render();
    ///
    /// \brief  Renders data if available
    ///
    /// \author Runik
    /// \date   21/05/2021
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void render();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto saturnin::video::Opengl::isThereSomethingToRender() const -> bool;
    ///
    /// \brief  Is there something to render
    ///
    /// \author Runik
    /// \date   06/10/2021
    ///
    /// \returns   True if there is something to render.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto isThereSomethingToRender() const -> bool;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto Opengl::getRenderedBufferTextureId() const -> u32;
    ///
    /// \brief  Gets the texture ID of the buffer currently rendered to.
    ///
    /// \author Runik
    /// \date   06/10/2021
    ///
    /// \returns    The rendered buffer texture identifier.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto getRenderedBufferTextureId() const -> u32;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Opengl::renderVdp1DebugOverlay();
    ///
    /// \brief  Renders the vertexes of the vdp1 part currently saved for debugging.
    ///
    /// \author Runik
    /// \date   16/06/2021
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void renderVdp1DebugOverlay();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Opengl::renderVdp2DebugLayer(core::EmulatorContext& state);
    ///
    /// \brief  Renders the vertexes of the vdp2 layer currently selected for display in the debug
    ///         window.
    ///
    /// \author Runik
    /// \date   16/06/2021
    ///
    /// \param [in,out] state   The emulator state.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void renderVdp2DebugLayer(core::EmulatorContext& state);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void saturnin::video::Opengl::addOrUpdateTexture(const size_t key, const Layer layer);
    ///
    /// \brief  Adds a texture to be created or updated on OpenGL.
    ///
    /// \author Runik
    /// \date   05/11/2021
    ///
    /// \param  key The texture key.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void addOrUpdateTexture(const size_t key, const Layer layer);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void saturnin::video::Opengl::removeTextureLink(const size_t key);
    ///
    /// \brief  Removes the link between the texture and the OpenGL id.
    ///
    /// \author Runik
    /// \date   03/06/2022
    ///
    /// \param  key The texture key.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void removeTextureLink(const size_t key);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Opengl::generateTextures();
    ///
    /// \brief  Generates the textures that will be used during rendering.
    ///
    /// \author Runik
    /// \date   05/11/2021
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void generateTextures();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	auto Opengl::getOpenglTexture(const size_t key) -> std::optional<OpenglTexture>;
    ///
    /// \brief	Gets texture identifier corresponding to the key.
    ///
    /// \author	Runik
    /// \date	05/11/2021
    ///
    /// \param 	key	The key.
    ///
    /// \returns	The OpenglTexture if found.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto getOpenglTexture(const size_t key) -> std::optional<OpenglTexture>;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto Opengl::isSaturnResolutionSet() const;
    ///
    /// \brief  Checks if the Saturn resolution is set.
    ///
    /// \author Runik
    /// \date   05/11/2021
    ///
    /// \returns    Returns true if the saturn screen resolution is set.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto isSaturnResolutionSet() const -> bool;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	auto Opengl::generateTextureFromTextureArrayLayer(const u32 layer, const size_t texture_key) -> u32;
    ///
    /// \brief	Generates a texture from a texture array layer.
    ///
    /// \author	Runik
    /// \date	10/12/2022
    ///
    /// \param 	layer	   	Layer of the texture array to get data from.
    /// \param 	texture_key	The key of the texture to highlight.
    ///
    /// \returns	The texture id generated from texture array layer.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto generateTextureFromTextureArrayLayer(const u32 layer, const size_t texture_key) -> u32;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	auto Opengl::getOpenglTextureDetails(const size_t key) -> std::string;
    ///
    /// \brief	Texture details
    ///
    /// \author	Runik
    /// \date	17/12/2022
    ///
    /// \param 	key	The key.
    ///
    /// \returns	The opengl texture details.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto getOpenglTextureDetails(const size_t key) -> std::string;

  private:
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto Opengl::getShaderSource(const ShaderType type, const ShaderName name) -> const char*;
    ///
    /// \brief  Gets the shader source depending on the configuration
    ///
    /// \author Runik
    /// \date   08/04/2021
    ///
    /// \param  type    The shader type.
    /// \param  name    The shader name.
    ///
    /// \returns    Null if it fails, else the shader source.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto getShaderSource(const ShaderType type, const ShaderName name) -> const char*;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto Opengl::calculateDisplayViewportMatrix() const -> glm::highp_mat4;
    ///
    /// \brief  Calculates the display viewport matrix, adding letterbox or pillarbox when the display isn't exactly like the
    /// Saturn resolution.
    ///
    /// \author Runik
    /// \date   10/04/2021
    ///
    /// \returns    The calculated display viewport matrix.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto calculateDisplayViewportMatrix() const -> glm::highp_mat4;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static auto Opengl::initializeVao(const ShaderName name) -> std::tuple<u32, u32>;
    ///
    /// \brief  Initializes a vao.
    ///
    /// \author Runik
    /// \date   16/04/2021
    ///
    /// \param  name    Name of the related shader.
    ///
    /// \returns    The generated VAO id and the vertex buffer id.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static auto initializeVao(const ShaderName name) -> std::tuple<u32, u32>;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Opengl::initializeFbos();
    ///
    /// \brief  Initializes the framebuffer objects.
    ///
    /// \author Runik
    /// \date   25/04/2021
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void initializeFbos();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	void Opengl::initializeFbo(const FboType type);
    ///
    /// \brief	Initializes the framebuffer object.
    ///
    /// \author	Runik
    /// \date	15/09/2022
    ///
    /// \param 	type	The framebuffer object type.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void initializeFbo(const FboType type);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	auto Opengl::initializeTextureArray() const -> u32;
    ///
    /// \brief	Initializes the texture array.
    ///
    /// \author	Runik
    /// \date	15/09/2022
    ///
    /// \returns	The generated texture array id.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto initializeTextureArray() const -> u32;

    auto getFboId(const FboType type) const -> u32 { return fbo_list_.at(type).first; };
    auto getFboTextureId(const FboType type) const -> u32 { return fbo_list_.at(type).second; };
    void switchRenderedBuffer();

    auto calculateViewportPosAndSize() const -> std::tuple<u32, u32, u32, u32>;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto Opengl::getVertexesNumberByDrawType(const PartsList& parts_list) -> u64;
    ///
    /// \brief  Returns the number of vertexes needed to display the parts.
    ///
    /// \author Runik
    /// \date   23/03/2022
    ///
    /// \returns    The number of vertexes.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto getVertexesNumberByDrawType(const PartsList& parts_list) const -> u64;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	void Opengl::packTextures(std::vector<OpenglTexture>& textures, const Layer layer);
    ///
    /// \brief	Pack textures in a texture array of texture atlases for the selected layer.
    ///
    /// \author	Runik
    /// \date	22/09/2022
    ///
    /// \param [in,out]	textures	The textures to pack.
    /// \param 		   	layer   	The layer.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void packTextures(std::vector<OpenglTexture>& textures, const Layer layer);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	void Opengl::generateSubTexture(const size_t key);
    ///
    /// \brief	Generates a subtexture in the texture array of texture atlas.
    ///
    /// \author	Runik
    /// \date	24/09/2022
    ///
    /// \param 	key	Key to the texture details.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void generateSubTexture(const size_t key);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	auto Opengl::getCurrentTextureArrayIndex(const Layer layer) -> u8;
    ///
    /// \brief	Returns the current texture array index used by one layer.
    ///
    /// \author	Runik
    /// \date	15/09/2023
    ///
    /// \param 	layer	Layer.
    ///
    /// \returns	The current texture array index used by the layer.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto getCurrentTextureArrayIndex(const Layer layer) -> u8;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	void Opengl::getNextAvailableTextureArrayIndex() const;
    ///
    /// \brief	Returns the next available texture array index regardless of the layer.
    ///
    /// \author	Runik
    /// \date	15/09/2023
    ///
    /// \returns    Texture array index.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto getNextAvailableTextureArrayIndex() const -> u8;

    core::Config* config_; ///< Configuration object.

    FboList fbo_list_;                ///< List of framebuffer objects used in the program.
    FboType current_rendered_buffer_; ///< The current rendered buffer (front or back)

    bool is_legacy_opengl_{}; ///< True if rendering in legacy opengl.

    ScreenResolution saturn_screen_resolution_{}; ///< Saturn screen resolution.
    ScreenResolution host_screen_resolution_{};   ///< Host screen resolution.

    PartsList parts_list_;        // Will have to be moved to the platform agnostic renderer.
    Vdp1Part  part_to_highlight_; ///< Part that will be highlighted during debug.

    u32                        texture_array_id_;                 ///< Identifier for the texture array.
    TexturesLink               textures_link_;                    ///< Link between the Texture key and the OpenglTexture.
    u32                        texture_array_debug_layer_id_{};   ///< Identifier for the texture array debug layer.
    LayerToTextureArrayIndexes layer_to_texture_array_indexes_{}; ///< Link between layers and texture array indexes.
    LayerToCacheReloadState    layer_to_cache_reload_state_{};    ///< Stores if a layer needs its cache to be reloaded .

    // std::vector<u32> textures_to_delete_; ///< List of the textures id to delete.

    std::mutex parts_list_mutex_;     ///< Mutex protecting parts_list_.
    std::mutex textures_link_mutex_;  ///< Mutex protecting textures_link_.
    std::mutex texture_delete_mutex_; ///< Mutex protecting textures_to_delete_.

    std::condition_variable data_condition_; ///< Condition variable to synchronize between emulation and UI thread.

    PartsList parts_list_debug_; ///< List of parts used to generate textures for debugging
                                 // Will have to be moved to the platform agnostic renderer.

    u32         program_shader_; ///< Program shader used to render parts.
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
/// \return True if the video card can render modern OpenGL .
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
/// \fn	auto createMainWindow(u32 width, u32 height, const std::string& title) -> GLFWwindow*;
///
/// \brief	Creates the main window.
///
/// \author	Runik
/// \date	25/01/2021
///
/// \param 	width 	The window's width.
/// \param 	height	The window's height.
/// \param 	title 	The window's title.
///
/// \returns	Null if it fails, else the new main window.
////////////////////////////////////////////////////////////////////////////////////////////////////

auto createMainWindow(u32 width, u32 height, const std::string& title) -> GLFWwindow*;

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

auto loadPngImage(const u8* data, const size_t size) -> GLFWimage;

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

void checkGlError();

}; // namespace saturnin::video
