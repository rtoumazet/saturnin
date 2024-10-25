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
#include <memory> // unique_ptr
#include <mutex>  // mutex
#include <string> // string
#include <tuple>  // tuple
#include <glm/mat4x4.hpp>
#include <saturnin/src/emulator_defs.h>
#include <saturnin/src/video/vdp_common.h>
#include <saturnin/src/video/vdp1_part.h>      // Vdp1Part
#include <saturnin/src/video/vdp2/vdp2_part.h> // Vdp2Part
#include <saturnin/src/video/opengl/opengl_common.h>

// Forward declarations
namespace saturnin::core {
class Config;
class EmulatorContext;
} // namespace saturnin::core

namespace gl {
enum class GLenum : saturnin::u32;
}
struct GLFWwindow;
struct GLFWimage;

namespace saturnin::video {

// Forward declaration
class OpenglRender;
class OpenglTexturing;

enum class DrawType : u8;
enum class ScrollScreen;

using saturnin::core::Config;
using utilities::toUnderlying;

constexpr auto minimum_window_width          = u16{512};
constexpr auto minimum_window_height         = u16{512};
constexpr auto texture_array_width           = u16{1024};
constexpr auto texture_array_height          = u16{1024};
constexpr auto texture_array_depth           = u16{64};
constexpr auto fbo_texture_array_width       = u16{2048};
constexpr auto fbo_texture_array_height      = u16{2048};
constexpr auto check_gl_error                = 1;
constexpr auto vertexes_per_tessellated_quad = u32{6}; // 2 triangles
constexpr auto vertexes_per_polyline         = u32{4};
constexpr auto vertexes_per_line             = u32{2};

// Number max of needed FBO :
// - 2 display buffers (front + back)
// - 2 debug layers (vdp1 + vdp2)
// - 10 priority layers (4 NBG layers + 1 sprite layers, 2 priorities possible by layer using special function)
constexpr auto fbo_texture_array_depth = u16{14};
constexpr auto max_fbo_texture         = u8{20};

enum class TextureArrayType : u8 { saturn_part, framebuffer };
enum class FboTextureType : u8 { front_buffer, back_buffer, vdp1_debug_overlay, vdp2_debug_layer, priority };
enum class GuiTextureType : u8 { render_buffer, vdp1_debug_buffer, vdp2_debug_buffer, layer_buffer };
enum class FboType : u8 { general, for_gui, vdp2_debug };
enum class MutexType : u8 { parts_list = 0, textures_link = 1, texture_delete = 2 };

// Status of FBO textures in the pool.
enum class FboTextureStatus : u8 {
    reuse,   ///< FBO will be reused as is.
    unused,  ///< FBO isn't in use.
    to_clear ///< FBO will have to be cleared.
};

using FboData               = std::pair<u32, u32>; // Describes a framebuffer object. 1st is fbo id, 2nd is texture id.
using FboTextureTypeToLayer = std::array<FboTextureType, fbo_texture_array_depth>; // Defines the type of each FBO texture layer,
                                                                                   // the index of the array being the layer.
using GuiTextureTypeToId = std::unordered_map<GuiTextureType, u32>; // Defines the type of each texture used to render to GUI.
using FboTypeToId        = std::unordered_map<FboType, u32>;        // Link between a FboType and its id.

using FboTexturePool       = std::array<u32, max_fbo_texture>; // Pool of textures ids to be used for rendering by priority.
using FboTexturePoolStatus = std::array<FboTextureStatus, max_fbo_texture>; // State of the textures in the pool.

// Holds screen resolutions used in the program
struct ScreenResolutions {
    ScreenResolution saturn;
    ScreenResolution host;
};

struct OpenglTexture {
    size_t                          key;                 ///< The Saturn texture key.
    u32                             opengl_id;           ///< Identifier of the OpenGL texture.
    u16                             texture_array_index; ///< The layer (or index) in the texture array.
    Size                            size;                ///< Texture size
    ScreenPos                       pos;                 ///< Position of the texture in the texture atlas.
    std::vector<TextureCoordinates> coords;              ///< The coordinates of the texture.
};

struct PlaneTexture {
    ScreenOffset                    screen_offset;       ///< The screen offset of the plane
    u16                             texture_array_index; ///< The index in the texture array.
    Size                            size;                ///< Texture size
    ScreenPos                       pos;                 ///< Position of the texture in the texture atlas.
    std::vector<TextureCoordinates> coords;              ///< The coordinates of the texture.
};

// Used to draw different primitives with calls to glDrawRangeElements.
struct DrawRange {
    u32        vertex_array_start;  ///< Start position in vertex array.
    u32        vertex_array_end;    ///< End position in vertex array.
    u32        indices_nb;          ///< Number of indices to draw.
    size_t     indices_array_start; ///< Start position in indices array.
    bool       is_textured;         ///< True if the range must display textured data.
    DrawType   draw_type;           ///< Type of the draw.
    gl::GLenum primitive;           ///< The primitive used to draw the indices in the range.
};

using LayerToTextureArrayIndexes = std::unordered_map<VdpLayer, std::vector<u8>>;
using LayerToCacheReloadState    = std::unordered_map<VdpLayer, bool>;

using TexturesLink = std::unordered_map<size_t, OpenglTexture>;

class Opengl {
  public:
    // Constructors / Destructors
    Opengl() = default;
    explicit Opengl(core::Config* config);
    Opengl(const Opengl&)                      = default;
    Opengl(Opengl&&)                           = default;
    auto operator=(const Opengl&) & -> Opengl& = default;
    auto operator=(Opengl&&) & -> Opengl&      = default;
    ~Opengl();

    // Accessors / Mutators
    [[nodiscard]] auto fps() const { return fps_; };
    void               fps(std::string_view fps) { fps_ = fps; };
    void               saturnScreenResolution(const ScreenResolution& res) { screen_resolutions_.saturn = res; };
    auto               saturnScreenResolution() const -> ScreenResolution { return screen_resolutions_.saturn; };
    void               hostScreenResolution(const ScreenResolution& res) { screen_resolutions_.host = res; };
    auto               hostScreenResolution() const -> ScreenResolution { return screen_resolutions_.host; };

    // Initializes this object
    void initialize();

    // Shuts down this object and frees any resources it is using
    void shutdown();

    // Actions executed on window resize.
    void onWindowResize(const u16 new_width, const u16 new_height);

    // Checks if the Framebuffer Objects (FBO) are initialized.
    auto areFbosInitialized() const -> bool;

    // Gets the texture ID of the buffer currently being rendered to.
    auto getRenderedBufferTextureId(const GuiTextureType type) -> u32;

    // Checks if the Saturn resolution is set.
    auto isSaturnResolutionSet() const -> bool;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	auto Opengl::getFboPoolIndex(const u8 priority, const Layer layer) -> std::optional<u8>;
    ///
    /// \brief	Gets the index of the relative priority + Layer in the FBO pool.
    ///
    /// \author	Runik
    /// \date	24/02/2024
    ///
    /// \param 	priority	The priority.
    /// \param 	layer   	The layer.
    ///
    /// \returns	The FBO pool index if found.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto getFboPoolIndex(const u8 priority, const VdpLayer layer) -> std::optional<u8>;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn	void Opengl::setFboTextureStatus(const u8 priority, const VdpLayer layer, const FboTextureStatus state);
    ///
    /// \brief	Sets FBO status
    ///
    /// \author	Runik
    /// \date	03/03/2024
    ///
    /// \param 	priority	The priority.
    /// \param 	layer   	The VDP layer.
    /// \param 	state   	The new state.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void setFboTextureStatus(const u8 priority, const VdpLayer layer, const FboTextureStatus state);
    void setFboTextureStatus(const u8 priority, const ScrollScreen screen, const FboTextureStatus state);

    // Sets FBO texture status for every priority of a specific screen.
    void setFboTextureStatus(const ScrollScreen screen, const FboTextureStatus state);

    auto getMutex(const MutexType& type) -> std::mutex&;

    // Interface to the OpenglRender object.
    auto render() -> OpenglRender* { return opengl_render_.get(); };
    // Interface to the OpenglTexturing object.
    auto texturing() -> OpenglTexturing* { return opengl_texturing_.get(); };

  private:
    friend class OpenglRender;

    // Clears FBO textures in the pool with the 'to_clear' status.
    void clearFboTextures();

    // Removes keys from the map for FBOs which status is not 'reuse'.
    void clearFboKeys();

    // Binds a FBO. Passing index 0 unbinds it.
    void bindFbo(const u32 fbo_id);

    // Unbinds current bound FBO.
    void unbindFbo();

    // Returns the next available FBO texture index (with status 'unused').
    auto getAvailableFboTextureIndex() -> std::optional<u8>;

    core::Config* config_; // Configuration object.

    std::unique_ptr<OpenglRender>    opengl_render_;    // OpenGL render object.
    std::unique_ptr<OpenglTexturing> opengl_texturing_; // OpenGL texturing object.

    FboKeyToFbo          fbo_key_to_fbo_pool_index_; // Link between a FBO key and its relative FBO index in the pool.
    FboTexturePool       fbo_texture_pool_;          // Pool of textures to be used by the FBO.
    FboTexturePoolStatus fbo_texture_pool_status_;   // State of each texture in the pool.

    ScreenResolutions screen_resolutions_; // Host and Saturn screen resolution

    std::array<std::mutex, 3> mutexes_{std::mutex(), std::mutex(), std::mutex()}; // Mutexes used to protect various shared data

    std::string fps_; // Calculated frames per second.
};

class OpenglTexturing {
  public:
    OpenglTexturing() = default;
    explicit OpenglTexturing(Opengl* opengl) : opengl_(opengl) {};
    ~OpenglTexturing()                                 = default;
    OpenglTexturing(const OpenglTexturing&)            = delete;
    OpenglTexturing(OpenglTexturing&&)                 = delete;
    OpenglTexturing& operator=(const OpenglTexturing&) = delete;
    OpenglTexturing& operator=(OpenglTexturing&&)      = delete;

    void initialize();

    // Shuts down this object and frees any resources it is using
    void shutdown();

    // Initializes a texture array.
    auto initializeTextureArray(const u32 width, const u32 height, const u32 depth) const -> u32;

    // Generates a texture from a vector of raw data. Returns the OpenGL id of the generated texture.
    [[nodiscard]] static auto generateTexture(u32 width, u32 height, const std::vector<u8>& data) -> u32;

    // Generates the textures that will be used during rendering.
    void generateTextures();

    // Generates a subtexture in the texture array of texture atlas.
    void generateSubTexture(const size_t key);

    // Deletes the texture on the GPU using its OpenGL texture id.
    static void deleteTexture(const u32 texture_id);

    // Pack textures in a texture array of texture atlases for the selected layer.
    void packTextures(std::vector<OpenglTexture>& textures, const VdpLayer layer);

    // Texture key is added so the linked texture is created or marked for recreation. Layer is used for cache management.
    void addOrUpdateTexture(const size_t key, const VdpLayer layer);

    // Removes the link between the Saturn texture and the OpenGL texture id.
    void removeTextureLink(const size_t key);

    // Gets texture identifier corresponding to the key if found.
    auto getOpenglTexture(const size_t key) -> std::optional<OpenglTexture>;

    // Calculates the texture coordinates in the texture array from the texture position and size.
    auto calculateTextureCoordinates(const ScreenPos& texture_pos_in_pixels,
                                     const Size&      texture_size,
                                     const u8         texture_array_index) const -> std::vector<TextureCoordinates>;

    // Returns all the vertexes from a parts list.
    auto readVertexes(const PartsList& parts) -> std::vector<Vertex>;

    // Returns the details of an Opengl texture
    auto getOpenglTextureDetails(const size_t key) -> std::string;

    auto getTextureId(const TextureArrayType type) -> u32;

    // Returns the current texture array index used by a specific layer.
    auto getCurrentTextureArrayIndex(const VdpLayer layer) -> u8;

    // Returns the next available texture array index regardless of the layer.
    auto getNextAvailableTextureArrayIndex() const -> u8;

    // Generates a texture from a texture array layer. Returns the id of the generated texture.
    auto generateTextureFromTextureArrayLayer(const GuiTextureType dst_texture_type, const u8 layer) -> u32;

    // Attachs a texture to the curently bound FBO.
    void attachTextureToFbo(const u32 texture_id, const gl::GLenum framebuffer_target, const gl::GLenum color_attachment);

    // Attachs a texture array layer to the curently bound FBO.
    void attachTextureLayerToFbo(const u32        texture_id,
                                 const u8         layer,
                                 const gl::GLenum framebuffer_target,
                                 const gl::GLenum color_attachment);

    // Initializes the framebuffer object and related elements.
    void initializeFbo();

    // Generates a framebuffer object.
    auto generateFbo(const FboType fbo_type) -> u32;

    auto getTextureArrayId() const { return texture_array_id_; };
    auto getFboTextureArrayId() const { return fbo_texture_array_id_; };
    auto getGuiTextureId(const GuiTextureType type) const { return gui_texture_type_to_id_.at(type); };
    auto getFboId(const FboType type) const { return fbo_type_to_id_.at(type); };

    // Gets the FBO texture layer currently used by the FboTextureType.
    auto getFboTextureLayer(const FboTextureType type) const -> u8;

    [[nodiscard]] auto vdp1DebugOverlayTextureId() const { return getFboTextureLayer(FboTextureType::vdp1_debug_overlay); };

    [[nodiscard]] auto vdp2DebugLayerTextureId() const -> u32 {
        return gui_texture_type_to_id_.at(GuiTextureType::vdp2_debug_buffer);
    };

  private:
    Opengl* opengl_;

    u32          texture_array_id_;               // Identifier for the texture array.
    TexturesLink textures_link_;                  // Link between the Texture key and the OpenglTexture.
    u32          texture_array_debug_layer_id_{}; // Identifier for the texture array debug layer.

    u32                   fbo_texture_array_id_;      // Identifier for the FBO texture array.
    GuiTextureTypeToId    gui_texture_type_to_id_;    // Links the texture to be used in the GUI to a type.
    FboTypeToId           fbo_type_to_id_;            // The framebuffer objects used in the app.
    FboTextureTypeToLayer fbo_texture_type_to_layer_; // Links the used FBO texture layer to a texture type. Index of the array
                                                      // is the layer, content is the type.

    LayerToCacheReloadState    layer_to_cache_reload_state_{};    // Stores if a layer needs its cache to be reloaded .
    LayerToTextureArrayIndexes layer_to_texture_array_indexes_{}; // Link between layers and texture array indexes.
};

}; // namespace saturnin::video
