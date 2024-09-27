//
// opengl.cpp
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

#include <saturnin/src/pch.h>
#include <saturnin/src/video/opengl/opengl.h>
#include <Windows.h> // removes C4005 warning
#include <algorithm>
#include <fstream>    // ifstream
#include <filesystem> // filesystem
#include <iostream>   // cout
#include <lodepng.h>
#include <ranges>
#include <sstream> // stringstream
#include <glbinding/glbinding.h>
#include <glbinding/Version.h>
#include <glbinding-aux/ContextInfo.h>
#include <glbinding/gl21/gl.h>
#include <glbinding/gl21ext/gl.h>
#include <glbinding/gl33core/gl.h>
#include <glbinding/gl33ext/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <saturnin/src/config.h>
#include <saturnin/src/locale.h> // tr
#include <saturnin/src/log.h>
#include <saturnin/src/utilities.h> // format
#include <saturnin/src/timer.h>     // Timer
#include <saturnin/src/video/gui.h>
#include <saturnin/src/video/texture.h>
#include <saturnin/src/video/vdp_common.h>
#include <saturnin/src/video/vdp1.h>
#include <saturnin/src/resource_holder.hpp>

// using namespace gl;
using namespace gl21;
using namespace gl21ext;

using namespace std::string_literals; // enables s-suffix for std::string literals

namespace saturnin::video {

namespace uti = saturnin::utilities;

using core::Log;
using core::Logger;
using core::tr;

const std::unordered_map<ScrollScreen, VdpLayer> screen_to_layer = {
    {ScrollScreen::nbg3, VdpLayer::nbg3},
    {ScrollScreen::nbg2, VdpLayer::nbg2},
    {ScrollScreen::nbg1, VdpLayer::nbg1},
    {ScrollScreen::nbg0, VdpLayer::nbg0},
    {ScrollScreen::rbg1, VdpLayer::rbg1},
    {ScrollScreen::rbg0, VdpLayer::rbg0}
};

Opengl::Opengl(core::Config* config) : config_(config) {};

Opengl::~Opengl() { shutdown(); }

void Opengl::initialize() {
    hostScreenResolution(ScreenResolution{video::minimum_window_width, video::minimum_window_height});

    glbinding::initialize(glfwGetProcAddress);

    // Initialize textures to be used on the GUI side
    gui_texture_type_to_id_[GuiTextureType::render_buffer]
        = generateTexture(fbo_texture_array_width, fbo_texture_array_height, std::vector<u8>{});
    gui_texture_type_to_id_[GuiTextureType::vdp1_debug_buffer]
        = generateTexture(fbo_texture_array_width, fbo_texture_array_height, std::vector<u8>{});
    gui_texture_type_to_id_[GuiTextureType::vdp2_debug_buffer]
        = generateTexture(fbo_texture_array_width, fbo_texture_array_height, std::vector<u8>{});
    gui_texture_type_to_id_[GuiTextureType::layer_buffer]
        = generateTexture(texture_array_width, texture_array_height, std::vector<u8>{});

    initializeFbo();
    shaders_list_            = initializeShaders();
    const auto main_vertex   = createVertexShader(shaders_list_, "main.vert");
    const auto main_fragment = createFragmentShader(shaders_list_, "main.frag");
    program_shaders_.try_emplace(ProgramShader::main, createProgramShader(main_vertex, main_fragment));

    const auto shaders_to_delete = std::vector<u32>{main_vertex, main_fragment};
    deleteShaders(shaders_to_delete);

    texture_array_id_ = initializeTextureArray(texture_array_width, texture_array_height, texture_array_depth);

    // Clear texture array indexes data
    for (auto& [layer, indexes] : layer_to_texture_array_indexes_) {
        std::vector<u8>().swap(indexes);
    }
}

void Opengl::shutdown() const {
    for (auto& [name, id] : program_shaders_) {
        glDeleteProgram(id);
    }

    gl33core::glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
    glDeleteFramebuffers(1, &fbo_type_to_id_.at(FboType::general));
    glDeleteFramebuffers(1, &fbo_type_to_id_.at(FboType::for_gui));
    glDeleteFramebuffers(1, &fbo_type_to_id_.at(FboType::vdp2_debug));

    for (size_t i = 0; i < max_fbo_texture; ++i) {
        deleteTexture(fbo_texture_pool_[i]);
    }

    for (const auto& [type, texture_id] : gui_texture_type_to_id_) {
        deleteTexture(texture_id);
    }
}

void Opengl::displayFramebuffer(core::EmulatorContext& state) {
    // :TODO:
    // - Render each layer + priority to one specific FBO
    // - Put render() content in its own function, and name it renderToAvailableFbo()
    // - Render() itself must take care of rendering the different FBOs in order
    // - Find which FBO to use in the FBO priority list
    // - displayFramebuffer() must generate one PartList by layer + priority to display to the FBO
    // -

    if constexpr (uses_fbo) {
        GlobalPartsList global_parts_list;

        // Step one : get displayable layers
        using enum ScrollScreen;
        std::vector<ScrollScreen> screens_to_display;
        if (!state.vdp2()->isLayerDisabled(nbg3)) screens_to_display.push_back(nbg3);
        if (!state.vdp2()->isLayerDisabled(nbg2)) screens_to_display.push_back(nbg2);
        if (!state.vdp2()->isLayerDisabled(nbg1)) screens_to_display.push_back(nbg1);
        if (!state.vdp2()->isLayerDisabled(nbg0)) screens_to_display.push_back(nbg0);
        if (!state.vdp2()->isLayerDisabled(rbg1)) screens_to_display.push_back(rbg1);
        if (!state.vdp2()->isLayerDisabled(rbg0)) screens_to_display.push_back(rbg0);

        // Step two : populate parts list for each priority + layer couple
        const auto addVdp1PartsToList = [&](const u8 priority) {
            auto        local_parts = PartsList();
            const auto& vdp1_parts  = state.vdp1()->vdp1Parts(priority);
            if (!vdp1_parts.empty()) {
                local_parts.reserve(vdp1_parts.size());
                for (const auto& p : vdp1_parts) {
                    local_parts.emplace_back(p);
                }
                global_parts_list[{priority, VdpLayer::sprite}] = std::move(local_parts);
                // Sprite layer is recalculated every time, there's no cache for now.
                setFboTextureStatus(priority, VdpLayer::sprite, FboTextureStatus::to_clear);
            }
        };

        const auto addVdp2PartsToList = [&](const ScrollScreen screen, const u32 priority) {
            auto        local_parts = PartsList();
            const auto& vdp2_parts  = state.vdp2()->vdp2Parts(screen, priority);
            if (!vdp2_parts.empty()) {
                local_parts.reserve(vdp2_parts.size());
                for (const auto& p : vdp2_parts) {
                    local_parts.emplace_back(p);
                }
                global_parts_list[{priority, screen_to_layer.at(screen)}] = std::move(local_parts);
            }
        };

        for (u8 priority = 7; priority > 0; --priority) {
            for (const auto& screen : screens_to_display) {
                addVdp2PartsToList(screen, priority);
            }
            addVdp1PartsToList(priority);
        }

        // Step three : wait to send data to the render thread.
        if constexpr (render_type == RenderType::RenderType_drawElements) {
            if (global_parts_list_.empty()) {
                std::unique_lock lk(parts_list_mutex_);
                // std::unique_lock lk(mutexes_[uti::toUnderlying(MutexType::parts_list)]);
                global_parts_list_ = std::move(global_parts_list);
                data_condition_.wait(lk, [this]() { return global_parts_list_.empty(); });
            }
        }
    } else {
        auto parts_list = PartsList{};

        const auto addVdp2PartsToList = [&](const ScrollScreen s) {
            if (const auto& vdp2_planes = state.vdp2()->vdp2Parts(s, VdpType::vdp2_cell); !vdp2_planes.empty()) {
                parts_list.reserve(parts_list.size() + vdp2_planes.size());
                for (const auto& p : vdp2_planes) {
                    parts_list.emplace_back(p);
                }
            }

            const auto& vdp2_bitmaps = state.vdp2()->vdp2Parts(s, VdpType::vdp2_bitmap);
            if (!vdp2_bitmaps.empty()) {
                parts_list.reserve(parts_list.size() + vdp2_bitmaps.size());
                for (const auto& p : vdp2_bitmaps) {
                    parts_list.emplace_back(p);
                }
            }
        };

        const auto addVdp1PartsToList = [&]() {
            const auto& vdp1_parts = state.vdp1()->vdp1Parts();
            if (!vdp1_parts.empty()) {
                parts_list.reserve(parts_list.size() + vdp1_parts.size());
                for (const auto& p : vdp1_parts) {
                    parts_list.emplace_back(p);
                }
            }
        };

        if (!state.vdp2()->isLayerDisabled(ScrollScreen::nbg3)) { addVdp2PartsToList(ScrollScreen::nbg3); }
        if (!state.vdp2()->isLayerDisabled(ScrollScreen::nbg2)) { addVdp2PartsToList(ScrollScreen::nbg2); }
        if (!state.vdp2()->isLayerDisabled(ScrollScreen::nbg1)) { addVdp2PartsToList(ScrollScreen::nbg1); }
        if (!state.vdp2()->isLayerDisabled(ScrollScreen::nbg0)) { addVdp2PartsToList(ScrollScreen::nbg0); }
        if (!state.vdp2()->isLayerDisabled(ScrollScreen::rbg1)) { addVdp2PartsToList(ScrollScreen::rbg1); }
        if (!state.vdp2()->isLayerDisabled(ScrollScreen::rbg0)) { addVdp2PartsToList(ScrollScreen::rbg0); }
        addVdp1PartsToList();
        std::ranges::stable_sort(parts_list, [](const RenderPart& a, const RenderPart& b) { return a.priority < b.priority; });
        if constexpr (render_type == RenderType::RenderType_drawElements) {
            if (parts_list_.empty()) {
                std::unique_lock lk(parts_list_mutex_);
                // std::unique_lock lk(mutexes_[uti::toUnderlying(MutexType::parts_list)]);
                parts_list_ = std::move(parts_list);
                data_condition_.wait(lk, [this]() { return parts_list_.empty(); });
            }
        }
    }
}

void Opengl::clearFboTextures() {
    Log::debug(Logger::opengl, "clearFboTextures() call");
    for (u8 index = 0; auto& status : fbo_texture_pool_status_) {
        if (status == FboTextureStatus::to_clear) {
            //          :WIP:
            Log::debug(Logger::opengl, "- Clearing texture at index {}", index);
            //          attachTextureLayerToFbo(fbo_texture_array_id_, index);
            //          attachTextureToFbo(getFboTextureId(FboTextureType::vdp2_debug_layer));

            gl::glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            Log::debug(Logger::opengl, "- Changing FBO texture status at index {} to 'unused'", index);
            status = FboTextureStatus::unused;
        }
    }
    Log::debug(Logger::opengl, "clearFbos() return");
}

void Opengl::clearFboKeys() {
    // Log::debug(Logger::opengl, "clearFboKeys() call");
    // const auto count = std::erase_if(fbo_key_to_fbo_pool_index_, [this](const auto& item) {
    //     auto const& [key, index] = item;
    //     return fbo_pool_status_[index] != FboStatus::reuse;
    // });
    // Log::debug(Logger::opengl, "- {} FBO key(s) erased", count);
    // Log::debug(Logger::opengl, "clearFboKeys() return");
}

void Opengl::bindFbo(const u32 fbo_id) { gl33core::glBindFramebuffer(GL_FRAMEBUFFER, fbo_id); }

void Opengl::unbindFbo() { bindFbo(0); }

auto Opengl::getAvailableFboTextureIndex() -> std::optional<u8> {
    std::array<FboTextureStatus, 1> status = {FboTextureStatus::unused};
    const auto                      result = std::ranges::find_first_of(fbo_texture_pool_status_, status);
    if (result == fbo_texture_pool_status_.end()) {
        Log::warning(Logger::opengl, "No FBO texture available in the pool !");
        return std::nullopt;
    } else {
        const auto index = static_cast<u8>(std::distance(fbo_texture_pool_status_.begin(), result));
        Log::debug(Logger::opengl, "Available FBO texture index : {}", index);
        return std::optional(index);
    }
}

auto Opengl::getRenderedBufferTextureId(const GuiTextureType type) -> u32 {
    auto layer = u8{};

    switch (type) {
        using enum GuiTextureType;
        case render_buffer: {
            layer = getFboTextureLayer(currentRenderedBuffer());
            break;
        }
        case vdp1_debug_buffer: {
            layer = getFboTextureLayer(FboTextureType::vdp1_debug_overlay);
            break;
        }
        case vdp2_debug_buffer: {
            layer = getFboTextureLayer(FboTextureType::vdp2_debug_layer);
            break;
        }
    }

    auto texture_id = generateTextureFromTextureArrayLayer(type, layer);
    return texture_id;
}

auto Opengl::getFboPoolIndex(const u8 priority, const VdpLayer layer) -> std::optional<u8> {
    if (fbo_key_to_fbo_pool_index_.contains({priority, layer})) {
        return std::optional{fbo_key_to_fbo_pool_index_[{priority, layer}]};
    }
    return std::nullopt;
}

void Opengl::setFboTextureStatus(const u8 priority, const VdpLayer layer, const FboTextureStatus state) {
    const auto index = getFboPoolIndex(priority, layer);
    if (!index) { return; }

    fbo_texture_pool_status_[*index] = state;
}

void Opengl::setFboTextureStatus(const u8 priority, const ScrollScreen screen, const FboTextureStatus state) {
    setFboTextureStatus(priority, screen_to_layer.at(screen), state);
}

void Opengl::setFboTextureStatus(const ScrollScreen screen, const FboTextureStatus state) {
    for (auto priority = u8{1}; priority < 8; ++priority) {
        setFboTextureStatus(priority, screen_to_layer.at(screen), state);
    }
}

auto Opengl::getCurrentTextureArrayIndex(const VdpLayer layer) -> u8 {
    if (layer_to_texture_array_indexes_.at(layer).empty()) {
        auto index = getNextAvailableTextureArrayIndex();
        layer_to_texture_array_indexes_.at(layer).push_back(index);
        return index;
    } else {
        return *std::ranges::max_element(layer_to_texture_array_indexes_.at(layer));
    }
}

auto Opengl::getNextAvailableTextureArrayIndex() const -> u8 {
    auto appended_indexes = std::vector<u8>();
    for (const auto& [layer, indexes] : layer_to_texture_array_indexes_) {
        appended_indexes.insert(appended_indexes.end(), indexes.begin(), indexes.end());
    }

    std::ranges::sort(appended_indexes, [](const u8 a, const u8& b) { return a < b; });

    auto ref_index = u8{};
    for (const auto index : appended_indexes) {
        if (index > ref_index) { return ref_index; }
        ++ref_index;
    }
    return ref_index;
}

auto Opengl::isSaturnResolutionSet() const -> bool {
    return (saturn_screen_resolution_.width == 0 || saturn_screen_resolution_.height == 0) ? false : true;
}

auto Opengl::generateTextureFromTextureArrayLayer(const GuiTextureType dst_texture_type, const u8 layer) -> u32 {
    auto texture_width  = u32{};
    auto texture_height = u32{};
    auto src_texture_id = u32{};

    switch (dst_texture_type) {
        using enum GuiTextureType;
        case render_buffer:
        case vdp1_debug_buffer:
        case vdp2_debug_buffer: {
            texture_width  = fbo_texture_array_width;
            texture_height = fbo_texture_array_height;
            src_texture_id = fbo_texture_array_id_;
            break;
        }
        case layer_buffer: {
            texture_width  = texture_array_width;
            texture_height = texture_array_height;
            src_texture_id = texture_array_id_;
            break;
        }
    }

    glBindFramebuffer(GLenum::GL_FRAMEBUFFER, fbo_type_to_id_[FboType::for_gui]);

    attachTextureLayerToFbo(src_texture_id, layer, GLenum::GL_FRAMEBUFFER, GLenum::GL_COLOR_ATTACHMENT0);

    attachTextureToFbo(gui_texture_type_to_id_[dst_texture_type], GLenum::GL_DRAW_FRAMEBUFFER, GLenum::GL_COLOR_ATTACHMENT1);

    glBlitFramebuffer(0,
                      0,
                      texture_width,
                      texture_height,
                      0,
                      0,
                      texture_width,
                      texture_height,
                      gl::GL_COLOR_BUFFER_BIT,
                      GLenum::GL_LINEAR);

    glBindFramebuffer(GLenum::GL_FRAMEBUFFER, 0);

    return gui_texture_type_to_id_[dst_texture_type];
}

auto Opengl::calculateDisplayViewportMatrix() const -> glm::highp_mat4 {
    const auto host_res     = hostScreenResolution();
    const auto host_ratio   = static_cast<float>(host_res.width) / static_cast<float>(host_res.height);
    const auto saturn_res   = saturnScreenResolution();
    const auto saturn_ratio = static_cast<float>(saturn_res.width) / static_cast<float>(saturn_res.height);

    // If the Saturn resolution isn't set yet, calculation is aborted
    if ((saturn_res.height == 0) || (saturn_res.width == 0)) { return glm::mat4{}; }

    auto projection = glm::mat4{};
    auto view       = glm::mat4{1.0f};

    if (host_ratio >= saturn_ratio) {
        // Pillarbox display (wide viewport), use full height
        projection = glm::mat4{
            glm::ortho(0.0f, host_ratio / saturn_ratio * saturn_res.width, 0.0f, static_cast<float>(saturn_res.height))};

        // Centering the viewport
        const auto empty_zone = host_res.width - saturn_res.width * host_res.height / saturn_res.height;
        const auto amount     = static_cast<float>(empty_zone) / host_res.width;
        view                  = glm::translate(view, glm::vec3(amount, 0.0f, 0.0f));

    } else {
        // Letterbox display (tall viewport) use full width
        projection = glm::mat4{
            glm::ortho(0.f, static_cast<float>(saturn_res.width), 0.f, saturn_ratio / host_ratio * saturn_res.height)};

        // Centering the viewport
        const auto empty_zone = host_res.height - saturn_res.height * host_res.width / saturn_res.width;
        const auto amount     = static_cast<float>(empty_zone) / host_res.height;
        view                  = glm::translate(view, glm::vec3(0.0f, amount, 0.0f));
    }

    return view * projection;
}

// static
auto Opengl::initializeVao() -> std::tuple<u32, u32> {
    auto vao = u32{};
    glGenVertexArrays(1, &vao);

    // Creating the vertex buffer
    auto vertex_buffer = u32{};
    glGenBuffers(1, &vertex_buffer);

    // Binding the VAO
    glBindVertexArray(vao);

    //// Declaring vertex buffer data
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

    // position pointer
    glVertexAttribPointer(0, 2, GLenum::GL_SHORT, GL_FALSE, sizeof(Vertex), nullptr); // NOLINT: this is an index
    glEnableVertexAttribArray(0);                                                     // NOLINT: this is an index

    // texture coords pointer
    auto offset = GLintptr(sizeof(VertexPosition));
    glVertexAttribPointer(1, 3, GLenum::GL_FLOAT, GL_FALSE, sizeof(Vertex), std::bit_cast<GLvoid*>(offset));
    glEnableVertexAttribArray(1);

    // color pointer
    offset += GLintptr(sizeof(TextureCoordinates));
    glVertexAttribPointer(2, 4, GLenum::GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), std::bit_cast<GLvoid*>(offset));
    glEnableVertexAttribArray(2);

    // gouraud color pointer
    offset += GLintptr(sizeof(VertexColor));
    glVertexAttribPointer(3, 3, GLenum::GL_BYTE, GL_TRUE, sizeof(Vertex), std::bit_cast<GLvoid*>(offset));
    glEnableVertexAttribArray(3);

    // color offset pointer R sign
    offset += GLintptr(sizeof(Gouraud));
    glVertexAttribPointer(4, 3, GLenum::GL_UNSIGNED_BYTE, GL_FALSE, sizeof(Vertex), std::bit_cast<GLvoid*>(offset));
    glEnableVertexAttribArray(4);

    // color offset pointer R value
    offset += GLintptr(sizeof(std::array<u8, 3>));
    glVertexAttribPointer(5, 3, GLenum::GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), std::bit_cast<GLvoid*>(offset));
    glEnableVertexAttribArray(5);

    glBindVertexArray(0);
    return std::make_tuple(vao, vertex_buffer);
}

void Opengl::initializeFbo() {
    fbo_texture_array_id_ = initializeTextureArray(fbo_texture_array_width, fbo_texture_array_height, fbo_texture_array_depth);

    // Generating a pool of textures to be used with the FBO.
    // for (size_t i = 0; i < max_fbo_texture; ++i) {
    //    fbo_texture_pool_[i]        = generateTexture(saturn_framebuffer_width, saturn_framebuffer_height,
    //    std::vector<u8>()); fbo_texture_pool_status_[i] = FboTextureStatus::unused;
    //}

    // Some textures are setup as a specific types in the FBO texture pool.
    // Front and back buffers are switched every frame : one will be used as the last complete rendering by the GUI while
    // the other will be rendered to.
    // Textures not yet linked to a type will be used as a 'priority' type on demand when rendering.
    using enum FboTextureType;
    fbo_texture_type_to_layer_ = {front_buffer, back_buffer, vdp1_debug_overlay, vdp2_debug_layer};
    for (u8 i = 4; i < fbo_texture_array_depth; ++i) {
        fbo_texture_type_to_layer_[i] = priority;
    }

    currentRenderedBuffer(front_buffer);

    // Generating FBOs used by the renderer.
    using enum FboType;
    fbo_type_to_id_[general]    = generateFbo(general);
    fbo_type_to_id_[for_gui]    = generateFbo(for_gui);
    fbo_type_to_id_[vdp2_debug] = generateFbo(vdp2_debug);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo_type_to_id_[general]);
}

auto Opengl::generateFbo(const FboType fbo_type) -> u32 {
    auto fbo = u32{};
    gl33core::glGenFramebuffers(1, &fbo);

    switch (fbo_type) {
        using enum FboType;
        case general: {
            glBindFramebuffer(GLenum::GL_FRAMEBUFFER, fbo);

            glBindTexture(GLenum::GL_TEXTURE_2D_ARRAY, fbo_texture_array_id_);

            glEnable(GLenum::GL_BLEND);
            glBlendFunc(GLenum::GL_SRC_ALPHA, GLenum::GL_ONE_MINUS_SRC_ALPHA);

            // Attaching the color texture currently used as framebuffer to the FBO.
            attachTextureLayerToFbo(fbo_texture_array_id_,
                                    getFboTextureLayer(currentRenderedBuffer()),
                                    GLenum::GL_FRAMEBUFFER,
                                    GLenum::GL_COLOR_ATTACHMENT0);
            break;
        }
        case for_gui: {
            glBindFramebuffer(GLenum::GL_FRAMEBUFFER, fbo);

            attachTextureLayerToFbo(fbo_texture_array_id_,
                                    getFboTextureLayer(currentRenderedBuffer()),
                                    GLenum::GL_FRAMEBUFFER,
                                    GLenum::GL_COLOR_ATTACHMENT0);

            attachTextureToFbo(gui_texture_type_to_id_[GuiTextureType::render_buffer],
                               GLenum::GL_DRAW_FRAMEBUFFER,
                               GLenum::GL_COLOR_ATTACHMENT1);

            glDrawBuffer(GL_COLOR_ATTACHMENT1);

            break;
        }
        case vdp2_debug: {
            glBindFramebuffer(GLenum::GL_FRAMEBUFFER, fbo);

            glActiveTexture(GLenum::GL_TEXTURE0);

            glEnable(GLenum::GL_BLEND);
            glBlendFunc(GLenum::GL_SRC_ALPHA, GLenum::GL_ONE_MINUS_SRC_ALPHA);

            attachTextureToFbo(gui_texture_type_to_id_[GuiTextureType::vdp2_debug_buffer],
                               GLenum::GL_DRAW_FRAMEBUFFER,
                               GLenum::GL_COLOR_ATTACHMENT0);

            glDrawBuffer(GL_COLOR_ATTACHMENT0);
            break;
        }
    }

    if (const auto status = gl33core::glCheckFramebufferStatus(GLenum::GL_FRAMEBUFFER);
        status != gl::GLenum::GL_FRAMEBUFFER_COMPLETE) {
        Log::exception(Logger::opengl, tr("Could not initialize framebuffer object !"));
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return fbo;
}

auto Opengl::initializeTextureArray(const u32 width, const u32 height, const u32 depth) const -> u32 {
    auto texture = u32{};
    glGenTextures(1, &texture);
    glActiveTexture(GLenum::GL_TEXTURE0);
    glBindTexture(GLenum::GL_TEXTURE_2D_ARRAY, texture);
    // The `my_gl_format` represents your cpu-side channel layout.
    // Both GL_RGBA and GL_BGRA are common. See the "format" section
    // of this page: https://www.opengl.org/wiki/GLAPI/glTexImage3D
    glTexImage3D(GL_TEXTURE_2D_ARRAY,
                 0,                        // mipmap level
                 GL_RGBA,                  // gpu texel format
                 width,                    // width
                 height,                   // height
                 depth,                    // depth
                 0,                        // border
                 GLenum::GL_RGBA,          // cpu pixel format
                 GLenum::GL_UNSIGNED_BYTE, // cpu pixel coord type
                 nullptr);                 // no data for now

    // set the texture wrapping parameters
    glTexParameteri(GLenum::GL_TEXTURE_2D_ARRAY, GLenum::GL_TEXTURE_WRAP_S, GLenum::GL_REPEAT);
    glTexParameteri(GLenum::GL_TEXTURE_2D_ARRAY, GLenum::GL_TEXTURE_WRAP_T, GLenum::GL_REPEAT);

    // disabling mipmaps
    glTexParameteri(GLenum::GL_TEXTURE_2D_ARRAY, GLenum::GL_TEXTURE_MIN_FILTER, GLenum::GL_NEAREST);
    glTexParameteri(GLenum::GL_TEXTURE_2D_ARRAY, GLenum::GL_TEXTURE_MAG_FILTER, GLenum::GL_NEAREST);

    glBindTexture(GLenum::GL_TEXTURE_2D_ARRAY, 0);
    return texture;
}

inline auto Opengl::getFboTextureLayer(const FboTextureType type) const -> u8 {
    return static_cast<u8>(
        std::distance(fbo_texture_type_to_layer_.begin(), std::ranges::find(fbo_texture_type_to_layer_, type)));
}

void Opengl::switchRenderedBuffer() {
    current_rendered_buffer_
        = (current_rendered_buffer_ == FboTextureType::back_buffer) ? FboTextureType::front_buffer : FboTextureType::back_buffer;
}

void Opengl::attachTextureLayerToFbo(const u32        texture_id,
                                     const u8         layer,
                                     const gl::GLenum framebuffer_target,
                                     const gl::GLenum color_attachment) {
    glFramebufferTextureLayer(framebuffer_target, color_attachment, texture_id, 0, layer);
}

void Opengl::attachTextureToFbo(const u32 texture_id, const gl::GLenum framebuffer_target, const gl::GLenum color_attachment) {
    glFramebufferTexture2D(framebuffer_target, color_attachment, GLenum::GL_TEXTURE_2D, texture_id, 0);
}

auto Opengl::calculateViewportPosAndSize() const -> std::tuple<u32, u32, u32, u32> {
    const auto host_res     = hostScreenResolution();
    const auto host_ratio   = static_cast<float>(host_res.width) / static_cast<float>(host_res.height);
    const auto saturn_res   = saturnScreenResolution();
    const auto saturn_ratio = static_cast<float>(saturn_res.width) / static_cast<float>(saturn_res.height);

    // Viewport position and size
    auto x      = u32{};
    auto y      = u32{};
    auto width  = u32{};
    auto height = u32{};

    if (host_ratio >= saturn_ratio) {
        // Pillarbox display (wide viewport), use full height
        if ((saturn_res.height != 0) && (host_res.width != 0)) {
            const auto empty_zone = host_res.width - saturn_res.width * host_res.height / saturn_res.height;
            x                     = empty_zone * saturn_framebuffer_width / host_res.width / 2;
            y                     = 0;
            width                 = (host_res.width - empty_zone) * saturn_framebuffer_width / host_res.width;
            height                = saturn_framebuffer_height;
        }
    } else {
        // Letterbox display (tall viewport) use full width
        if ((saturn_res.width != 0) && (host_res.height != 0)) {
            const auto empty_zone = host_res.height - saturn_res.height * host_res.width / saturn_res.width;
            x                     = 0;
            y                     = empty_zone * saturn_framebuffer_height / host_res.height / 2;
            width                 = saturn_framebuffer_width;
            height                = (host_res.height - empty_zone) * saturn_framebuffer_height / host_res.height;
        }
    }

    return std::make_tuple(x, y, width, height);
}

auto runOpengl(core::EmulatorContext& state) -> s32 {
    const bool is_legacy_opengl = state.config()->readValue(core::AccessKeys::cfg_rendering_legacy_opengl);

    // Setup window
    glfwSetErrorCallback(error_callback);
    if (glfwInit() == GLFW_FALSE) { return EXIT_FAILURE; }

    // Decide GL+GLSL versions
#if __APPLE__
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    if (!is_legacy_opengl) {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // Required on Mac
    }
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    if (!is_legacy_opengl) {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE,
                       GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
        // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // 3.0+ only

        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
    }
#endif
    const std::string rendering_mode = is_legacy_opengl ? core::tr("Legacy") : core::tr("Modern");
    const std::string window_title
        = util::format(core::tr("Saturnin {0} - {1} rendering"), core::saturnin_version, rendering_mode);

    const auto window = createMainWindow(minimum_window_width, minimum_window_height, window_title);
    if (window == nullptr) { return EXIT_FAILURE; }
    state.openglWindow(window);

    // glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    // const auto render_window = glfwCreateWindow(1, 1, "invisible", nullptr, ihm_window);

    glfwSetWindowCloseCallback(window, windowCloseCallback);
    glfwSetWindowSizeCallback(window, windowSizeCallback);

    glfwSetWindowUserPointer(window, static_cast<void*>(&state));

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync
    const auto ico_16 = rh::embed("saturnin-ico-16.png");
    const auto ico_32 = rh::embed("saturnin-ico-32.png");
    const auto ico_48 = rh::embed("saturnin-ico-48.png");
    auto       icons  = std::array<GLFWimage, 3>{
        {loadPngImage(ico_16.data(), ico_16.size()),
         loadPngImage(ico_32.data(), ico_32.size()),
         loadPngImage(ico_48.data(), ico_48.size())}
    };
    glfwSetWindowIcon(window, 3, icons.data());

    int flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (uti::toEnum<ContextFlagMask>(flags) == ContextFlagMask::GL_CONTEXT_FLAG_DEBUG_BIT) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }

    glbinding::initialize(glfwGetProcAddress);

    // glCopyImageSubData()

    // Setup Dear ImGui binding
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // auto flags = ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    //(void)flags;
    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    //  io.ConfigViewportDecorations
    //  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    //  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Viewports

    // Setup style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsClassic();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    is_legacy_opengl ? ImGui_ImplOpenGL3_Init() : ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
    // (there is a default font, this is only if you want to change it. see extra_fonts/README.txt for more details)
    // ImGuiIO& io = ImGui::GetIO();
    // io.Fonts->AddFontDefault();
    // io.Fonts->AddFontFromFileTTF("../../extra_fonts/Cousine-Regular.ttf", 15.0f);
    // io.Fonts->AddFontFromFileTTF("../../extra_fonts/DroidSans.ttf", 16.0f);
    // io.Fonts->AddFontFromFileTTF("../../extra_fonts/Roboto-Medium.ttf", 16.0f);
    // io.Fonts->AddFontFromFileTTF("../../extra_fonts/ProggyTiny.ttf", 10.0f);
    // io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());

    // Adding glyphs that will be used as images in text
    // ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontDefault();
    static const std::array<ImWchar, 3> icons_ranges = {0xe900, 0xe908, 0}; // Will not be copied by AddFont* so keep in scope.
    ImFontConfig                        config;
    config.MergeMode            = true;
    config.FontDataOwnedByAtlas = false;
    auto       data             = rh::embed("saturnin-icons.ttf");
    const auto glyph_offset     = ImVec2(0, 2);
    config.GlyphOffset          = glyph_offset;
    constexpr auto font_size    = 13.0f;

    io.Fonts->AddFontFromMemoryTTF((void*)data.data(), static_cast<u32>(data.size()), font_size, &config, icons_ranges.data());
    io.Fonts->Build();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Viewports

    const auto clear_color = ImVec4{0.0f, 0.0f, 0.0f, 1.00f};

    updateMainWindowSizeAndRatio(window, minimum_window_width, minimum_window_height);

    state.opengl()->initialize();

    Log::info(Logger::opengl, "Card : {}", (char*)glGetString(GL_RENDERER));

    if (state.memory()->selectedBinaryFile().full_path.size() > 0) {
        state.startEmulation();
        if (!state.memory()->selectedBinaryFile().is_auto_started) { state.debugStatus(core::DebugStatus::paused); }
    }

    // glGetString(GL_RENDERER);

    // Main loop
    while (glfwWindowShouldClose(window) == GLFW_FALSE) {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two
        // flags.

        glfwPollEvents();

        // Start the ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Rendering
        auto display_w = s32{};
        auto display_h = s32{};
        glfwMakeContextCurrent(window);
        glfwGetFramebufferSize(window, &display_w, &display_h);

        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        gui::buildGui(state);
        if (state.renderingStatus() != core::RenderingStatus::running) { glfwSetWindowShouldClose(window, GLFW_TRUE); }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        if ((io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) > 0) {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}

auto generateVertexIndicesAndDrawRanges(const PartsList& parts) -> std::tuple<std::vector<u32>, std::vector<DrawRange>> {
    const auto indices_per_polygon  = std::vector<u32>{0, 1, 2, 0, 2, 3};
    const auto indices_per_polyline = std::vector<u32>{0, 1, 2, 3};
    const auto indices_per_line     = std::vector<u32>{0, 1};

    using TypeToIndices = std::map<const DrawType, const std::vector<u32>>;

    using enum DrawType;
    const auto typeToIndices = TypeToIndices{
        {textured_polygon,     indices_per_polygon },
        {non_textured_polygon, indices_per_polygon },
        {polyline,             indices_per_polyline},
        {line,                 indices_per_line    }
    };

    using TypeToVertexIncrement      = std::map<const DrawType, const u32>;
    const auto typeToVertexIncrement = TypeToVertexIncrement{
        {textured_polygon,     4},
        {non_textured_polygon, 4},
        {polyline,             4},
        {line,                 2}
    };

    using TypeIsTextured      = std::map<const DrawType, const bool>;
    const auto typeIsTextured = TypeIsTextured{
        {textured_polygon,     true },
        {non_textured_polygon, false},
        {polyline,             false},
        {line,                 false}
    };

    using TypeToPrimitive      = std::map<const DrawType, const gl::GLenum>;
    const auto typeToPrimitive = TypeToPrimitive{
        {textured_polygon,     gl::GL_TRIANGLES},
        {non_textured_polygon, gl::GL_TRIANGLES},
        {polyline,             gl::GL_LINE_LOOP},
        {line,                 gl::GL_LINES    }
    };

    auto indices              = std::vector<u32>{};
    auto ranges               = std::vector<DrawRange>{};
    auto increment            = u32{};
    auto current_range        = DrawRange{.vertex_array_start  = 0,
                                          .vertex_array_end    = 0,
                                          .indices_nb          = 0,
                                          .indices_array_start = 0,
                                          .is_textured         = false,
                                          .draw_type           = DrawType::undefined,
                                          .primitive           = gl::GL_TRIANGLES};
    auto previous_vertexes_nb = u32{};

    if (!parts.empty()) {
        current_range.draw_type = parts.at(0).draw_type;

        indices.reserve(parts.size()
                        * indices_per_polygon.size()); // Size will be a bit bigger than needed, but that's not important.
        for (const auto& p : parts) {
            if (!typeToIndices.contains(p.draw_type)) { continue; } // Non drawable parts are skipped

            if (current_range.draw_type != p.draw_type && current_range.indices_nb > 0) {
                --current_range.vertex_array_end;
                current_range.indices_array_start += previous_vertexes_nb;
                ranges.push_back(current_range);

                previous_vertexes_nb             = current_range.indices_nb;
                current_range.vertex_array_start = current_range.vertex_array_end + 1;
                current_range.vertex_array_end   = current_range.vertex_array_start;
                current_range.indices_nb         = 0;
                current_range.is_textured        = typeIsTextured.at(p.draw_type);
                current_range.draw_type          = p.draw_type;
                current_range.primitive          = typeToPrimitive.at(p.draw_type);
            } else {
                current_range.is_textured = typeIsTextured.at(p.draw_type);
            }

            std::ranges::transform(typeToIndices.at(p.draw_type), std::back_inserter(indices), [&increment](u32 i) {
                return i + increment;
            });

            increment += typeToVertexIncrement.at(p.draw_type);

            current_range.vertex_array_end += typeToVertexIncrement.at(p.draw_type);
            current_range.indices_nb += static_cast<u32>(typeToIndices.at(p.draw_type).size());
        }

        // The last range is added if needed
        if (typeToPrimitive.contains(current_range.draw_type)) {
            --current_range.vertex_array_end;
            current_range.indices_array_start += previous_vertexes_nb;
            current_range.is_textured = typeIsTextured.at(current_range.draw_type);
            current_range.draw_type   = current_range.draw_type;
            current_range.primitive   = typeToPrimitive.at(current_range.draw_type);

            ranges.push_back(current_range);
        }
    }

    return {indices, ranges};
}
}; // namespace saturnin::video
