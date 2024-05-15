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
#include <saturnin/src/video/opengl.h>
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

constexpr auto vertexes_per_tessellated_quad = u32{6}; // 2 triangles
constexpr auto vertexes_per_polyline         = u32{4};
constexpr auto vertexes_per_line             = u32{2};

constexpr auto check_gl_error = 1;

constexpr enum class RenderType { RenderType_drawElements, RenderType_drawTest };
constexpr auto render_type = RenderType::RenderType_drawElements;
// constexpr auto render_type = RenderType::RenderType_drawTest;

constexpr auto uses_fbo = false;

const std::unordered_map<ScrollScreen, VdpLayer> screen_to_layer = {
    {ScrollScreen::nbg3, VdpLayer::nbg3},
    {ScrollScreen::nbg2, VdpLayer::nbg2},
    {ScrollScreen::nbg1, VdpLayer::nbg1},
    {ScrollScreen::nbg0, VdpLayer::nbg0},
    {ScrollScreen::rbg1, VdpLayer::rbg1},
    {ScrollScreen::rbg0, VdpLayer::rbg0}
};

const std::unordered_map<VdpLayer, std::string> layer_to_name = {
    {VdpLayer::nbg0,      "nbg0"     },
    {VdpLayer::rbg1,      "rbg1"     },
    {VdpLayer::nbg1,      "nbg1"     },
    {VdpLayer::exbg,      "exbg"     },
    {VdpLayer::nbg2,      "nbg2"     },
    {VdpLayer::nbg3,      "nbg3"     },
    {VdpLayer::rbg0,      "rbg0"     },
    {VdpLayer::back,      "back"     },
    {VdpLayer::sprite,    "sprite"   },
    {VdpLayer::plane,     "plane"    },
    {VdpLayer::undefined, "undefined"}
};

Opengl::Opengl(core::Config* config) : config_(config){};

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
    initializeShaders();
    const auto main_vertex   = createVertexShader("main.vert");
    const auto main_fragment = createFragmentShader("main.frag");
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

void Opengl::preRender() {
    switchRenderedBuffer();

    // GLint drawFboId = 0;
    // glGetIntegerv(GL_FRAMEBUFFER_BINDING, &drawFboId);

    glBindFramebuffer(GLenum::GL_FRAMEBUFFER, fbo_type_to_id_[FboType::general]);

    attachTextureLayerToFbo(fbo_texture_array_id_,
                            getFboTextureLayer(currentRenderedBuffer()),
                            GLenum::GL_FRAMEBUFFER,
                            GLenum::GL_COLOR_ATTACHMENT0);

    // Viewport is the entire Saturn framebuffer
    glViewport(0, 0, saturn_framebuffer_width, saturn_framebuffer_height);

    gl::glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Scissor test calculation, to remove from display everything outside the current display area.
    glEnable(GL_SCISSOR_TEST);
    auto [scissor_x, scissor_y, scissor_width, scissor_height] = calculateViewportPosAndSize();
    glScissor(scissor_x, scissor_y, scissor_width, scissor_height);
};

void Opengl::postRender() {
    // Framebuffer is released
    checkGlError();

    glDisable(GL_SCISSOR_TEST);

    glBindFramebuffer(GLenum::GL_FRAMEBUFFER, 0);
};

void Opengl::initializeShaders() {
    const auto readFile = [](const std::string& filename) {
        std::ifstream input_file("./shaders/" + filename, std::ios::in);
        if (!input_file) { Log::exception(Logger::opengl, tr("Could not load shader '{}' !"), filename); }

        auto buffer = std::stringstream{};
        buffer << input_file.rdbuf();
        input_file.close();

        return buffer.str();
    };

    shaders_list_.try_emplace("main.vert", readFile("main.vert"));
    shaders_list_.try_emplace("main.frag", readFile("main.frag"));
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
        const auto addVdp1PartsToList = [&](const u32 priority) {
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

        for (auto priority = 7; priority > 0; --priority) {
            for (const auto& screen : screens_to_display) {
                addVdp2PartsToList(screen, priority);
            }
            addVdp1PartsToList(priority);
        }

        // Step three : wait to send data to the render thread.
        if constexpr (render_type == RenderType::RenderType_drawElements) {
            if (global_parts_list_.empty()) {
                std::unique_lock lk(parts_list_mutex_);
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
                parts_list_ = std::move(parts_list);
                data_condition_.wait(lk, [this]() { return parts_list_.empty(); });
            }
        }

        if constexpr (render_type == RenderType::RenderType_drawTest) {
            if (!parts_list.empty()) {
                std::vector<DrawRange>().swap(draw_range_);
                for (const auto& p : parts_list) {
                    DrawRange dr{};
                    dr.vertex_array_start = 0;
                }
            }
        }
    }
}

void Opengl::renderToAvailableTexture(const FboKey& key, const PartsList& parts_list) {
    Log::debug(Logger::opengl, "renderToAvailableFbo() call");
    const auto index = getAvailableFboTextureIndex();
    if (!index) {
        Log::debug(Logger::opengl, "- No FBO available for rendering");
        Log::debug(Logger::opengl, "renderToAvailableFbo() return");
        return;
    }

    const auto& [priority, layer] = key;
    Log::debug(Logger::opengl,
               "- Rendering key [priority={}, layer={}] to FBO with index {}",
               priority,
               layer_to_name.at(layer),
               *index);

    renderParts(parts_list, fbo_texture_pool_[*index]);

    Log::debug(Logger::opengl, "- Changing FBO status at index {} to 'reuse'", *index);

    fbo_texture_pool_status_[*index] = FboTextureStatus::reuse;
    fbo_key_to_fbo_pool_index_[key]  = *index;

    Log::debug(Logger::opengl, "renderToAvailableFbo() return");
}

void Opengl::clearFboTextures() {
    // Log::debug(Logger::opengl, "clearFboTextures() call");
    // for (u8 index = 0; auto& status : fbo_texture_pool_status_) {
    //     if (status == FboTextureStatus::to_clear) {
    //         // :WIP:
    //         // Log::debug(Logger::opengl, "- Clearing texture at index {}", index);
    //         // attachTextureLayerToFbo(fbo_texture_array_id_, index);
    //         // attachTextureToFbo(getFboTextureId(FboTextureType::vdp2_debug_layer));

    //        // gl::glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    //        // glClear(GL_COLOR_BUFFER_BIT);

    //        // Log::debug(Logger::opengl, "- Changing FBO texture status at index {} to 'unused'", index);
    //        // status = FboTextureStatus::unused;
    //    }
    //}
    // Log::debug(Logger::opengl, "clearFbos() return");
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

void Opengl::renderParts(const PartsList& parts_list, const u32 texture_id) {
    if (!parts_list.empty()) {
        const auto [vao, vertex_buffer] = initializeVao();

        glUseProgram(program_shaders_[ProgramShader::main]);
        glBindVertexArray(vao);                       // binding VAO
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer); // binding vertex buffer

        // Calculating the ortho projection matrix
        const auto proj_matrix = calculateDisplayViewportMatrix();

        // Sending the ortho projection matrix to the shader
        const auto uni_proj_matrix = glGetUniformLocation(program_shaders_[ProgramShader::main], "proj_matrix");
        glUniformMatrix4fv(uni_proj_matrix, 1, GL_FALSE, glm::value_ptr(proj_matrix));

        glActiveTexture(GLenum::GL_TEXTURE0);
        const auto sampler_loc = glGetUniformLocation(program_shaders_[ProgramShader::main], "sampler");

        glUniform1i(sampler_loc, 0);
        glBindTexture(GL_TEXTURE_2D_ARRAY, texture_id);

        const auto texture_used_loc = glGetUniformLocation(program_shaders_[ProgramShader::main], "is_texture_used");

        auto elements_buffer = u32{};
        glGenBuffers(1, &elements_buffer); // This buffer will be used to send indices data to the GPU
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elements_buffer);

        const auto&& [indices, draw_ranges] = generateVertexIndicesAndDrawRanges(parts_list);
        const auto vertexes                 = readVertexes(parts_list);

        // Sending data to the GPU
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * indices.size(), indices.data(), GL_STATIC_DRAW);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertexes.size(), vertexes.data(), GL_STATIC_DRAW);

        for (const auto& range : draw_ranges) {
            const auto is_texture_used = GLboolean(range.is_textured);
            glUniform1i(texture_used_loc, is_texture_used);

            glDrawRangeElements(range.primitive,
                                range.vertex_array_start,
                                range.vertex_array_end,
                                range.indices_nb,
                                GL_UNSIGNED_INT,
                                static_cast<GLuint*>(nullptr) + range.indices_array_start);
        }

        gl::glDeleteBuffers(1, &vertex_buffer);
        gl::glDeleteVertexArrays(1, &vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
        gl::glDeleteBuffers(1, &elements_buffer);
    }
}

void Opengl::renderFboTexture(const u32 texture_id) {
    const auto [vao, vertex_buffer] = initializeVao();

    glUseProgram(program_shaders_[ProgramShader::main]);
    glBindVertexArray(vao);                       // binding VAO
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer); // binding vertex buffer

    // Calculating the ortho projection matrix
    const auto proj_matrix = calculateDisplayViewportMatrix();

    // Sending the ortho projection matrix to the shader
    const auto uni_proj_matrix = glGetUniformLocation(program_shaders_[ProgramShader::main], "proj_matrix");
    glUniformMatrix4fv(uni_proj_matrix, 1, GL_FALSE, glm::value_ptr(proj_matrix));

    glActiveTexture(GLenum::GL_TEXTURE0);
    const auto sampler_loc = glGetUniformLocation(program_shaders_[ProgramShader::main], "sampler");

    glUniform1i(sampler_loc, 0);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    const auto texture_used_loc = glGetUniformLocation(program_shaders_[ProgramShader::main], "is_texture_used");
    glUniform1i(texture_used_loc, GLboolean(true));

    auto vertexes = std::vector<Vertex>{
        {0,    0,    0.0f, 0.0f, 0.0f, 0, 0, 0xff, 0xff, Gouraud()},
        {0,    2048, 1.0f, 0.0f, 0.0f, 0, 0, 0xff, 0xff, Gouraud()},
        {2048, 2048, 1.0f, 1.0f, 0.0f, 0, 0, 0xff, 0xff, Gouraud()},
        {0,    0,    0.0f, 0.0f, 0.0f, 0, 0, 0xff, 0xff, Gouraud()},
        {2048, 2048, 1.0f, 1.0f, 0.0f, 0, 0, 0xff, 0xff, Gouraud()},
        {2048, 0,    0.0f, 1.0f, 0.0f, 0, 0, 0xff, 0xff, Gouraud()}
    };

    // Sending vertex buffer data to the GPU
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertexes.size(), vertexes.data(), GL_STATIC_DRAW);

    // Drawing the list
    glDrawArrays(GL_TRIANGLES, 0, vertexes_per_tessellated_quad);

    // auto elements_buffer = u32{};
    // glGenBuffers(1, &elements_buffer); // This buffer will be used to send indices data to the GPU
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elements_buffer);

    // const auto&& [indices, draw_ranges] = generateVertexIndicesAndDrawRanges(parts_list);
    // const auto vertexes                 = readVertexes(parts_list);

    //// Sending data to the GPU
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * indices.size(), indices.data(), GL_STATIC_DRAW);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertexes.size(), vertexes.data(), GL_STATIC_DRAW);

    // for (const auto& range : draw_ranges) {
    //     const auto is_texture_used = GLboolean(range.is_textured);
    //     glUniform1i(texture_used_loc, is_texture_used);

    //    glDrawRangeElements(range.primitive,
    //                        range.vertex_array_start,
    //                        range.vertex_array_end,
    //                        range.indices_nb,
    //                        GL_UNSIGNED_INT,
    //                        static_cast<GLuint*>(nullptr) + range.indices_array_start);
    //}

    gl::glDeleteBuffers(1, &vertex_buffer);
    gl::glDeleteVertexArrays(1, &vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    // gl::glDeleteBuffers(1, &elements_buffer);
}

void Opengl::render() {
    if constexpr (uses_fbo) {
        GlobalPartsList global_parts_list;

        const auto getPartsFromThread = [&]() {
            std::lock_guard lock(parts_list_mutex_);
            if (!global_parts_list_.empty()) { global_parts_list = std::move(global_parts_list_); }
        };

        getPartsFromThread();

        clearFboTextures();
        clearFboKeys();

        // Rendering is done to FBOs depending on the priority and layer combo.
        for (const auto& [key, parts] : global_parts_list) {
            renderToAvailableTexture(key, parts);
        }

        preRender();

        // :TODO: Render the FBOs to the current framebuffer
        std::ranges::reverse_view rv{fbo_key_to_fbo_pool_index_};
        for (const auto& [key, index] : rv) {
            renderFboTexture(fbo_texture_pool_[index]);
        }

        postRender();

        const auto notifyMainThread = [&]() {
            std::lock_guard lk(parts_list_mutex_);
            GlobalPartsList().swap(global_parts_list);
            data_condition_.notify_one();
        };

        notifyMainThread();

    } else {
        PartsList parts_list;

        preRender();
        {
            std::lock_guard lock(parts_list_mutex_);
            if (!parts_list_.empty()) { parts_list = std::move(parts_list_); }
        }

        renderParts(parts_list, texture_array_id_);
        // renderParts(parts_list, getFboTextureId(currentRenderedBuffer()));

        {
            std::lock_guard lk(parts_list_mutex_);
            PartsList().swap(parts_list);
            data_condition_.notify_one();
        }

        postRender();
    }
}

void Opengl::renderTest() {
    preRender();

    const auto [vao, vertex_buffer] = initializeVao();

    glUseProgram(program_shaders_[ProgramShader::main]);
    glBindVertexArray(vao);                       // binding VAO
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer); // binding vertex buffer

    // Calculating the ortho projection matrix
    const auto proj_matrix = calculateDisplayViewportMatrix();

    // Sending the ortho projection matrix to the shader
    const auto uni_proj_matrix = glGetUniformLocation(program_shaders_[ProgramShader::main], "proj_matrix");
    glUniformMatrix4fv(uni_proj_matrix, 1, GL_FALSE, glm::value_ptr(proj_matrix));

    glActiveTexture(GLenum::GL_TEXTURE0);
    const auto sampler_loc = glGetUniformLocation(program_shaders_[ProgramShader::main], "sampler");
    // glUniform1i(sampler_loc, GLenum::GL_TEXTURE0);
    glUniform1i(sampler_loc, 0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, texture_array_id_);

    const auto texture_used_loc = glGetUniformLocation(program_shaders_[ProgramShader::main], "is_texture_used");
    // Sending the variable to configure the shader to use texture data.
    const auto is_texture_used = GLboolean(false);
    glUniform1i(texture_used_loc, is_texture_used);

    auto elements_buffer = u32{};
    glGenBuffers(1, &elements_buffer); // This buffer will be used to send indices data to the GPU
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elements_buffer);

    if constexpr (false) {
        // std::array<GLuint, 20> indices = {0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
        std::vector<GLuint> indices = {0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
        // std::span<GLuint>   indices(vindices);
        //      std::array<GLuint, 5> indices = {0, 1, 2, 3, 0};
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), &indices.front(), GL_STATIC_DRAW);

        auto vertexes = std::vector<Vertex>{
            {100, 100, 0.0f, 0.0f, 0.0f, 0xff, 0,    0,    0xff, Gouraud()},
            {100, 150, 1.0f, 0.0f, 0.0f, 0xff, 0,    0,    0xff, Gouraud()},
            {150, 150, 1.0f, 1.0f, 0.0f, 0xff, 0,    0,    0xff, Gouraud()},
            {150, 100, 0.0f, 1.0f, 0.0f, 0xff, 0,    0,    0xff, Gouraud()},

            {200, 200, 0.0f, 0.0f, 0.0f, 0,    0,    0xff, 0xff, Gouraud()},
            {200, 250, 1.0f, 0.0f, 0.0f, 0,    0,    0xff, 0xff, Gouraud()},
            {250, 250, 1.0f, 1.0f, 0.0f, 0,    0,    0xff, 0xff, Gouraud()},
            {250, 200, 0.0f, 1.0f, 0.0f, 0,    0,    0xff, 0xff, Gouraud()},

            {50,  50,  0.0f, 0.0f, 0.0f, 0,    0xff, 0,    0xff, Gouraud()},
            {100, 100, 1.0f, 0.0f, 0.0f, 0,    0xff, 0,    0xff, Gouraud()},

            {70,  70,  0.0f, 0.0f, 0.0f, 0xff, 0,    0,    0xff, Gouraud()},
            {120, 120, 1.0f, 0.0f, 0.0f, 0xff, 0,    0,    0xff, Gouraud()},

            {70,  70,  0.0f, 0.0f, 0.0f, 0,    0,    0xff, 0xff, Gouraud()},
            {70,  120, 1.0f, 0.0f, 0.0f, 0,    0,    0xff, 0xff, Gouraud()},
            {120, 120, 1.0f, 1.0f, 0.0f, 0,    0,    0xff, 0xff, Gouraud()},
            {120, 70,  0.0f, 1.0f, 0.0f, 0,    0,    0xff, 0xff, Gouraud()}
        };

        // Sending data to the GPU
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertexes.size(), vertexes.data(), GL_STATIC_DRAW);

        glDrawRangeElements(GL_TRIANGLES, 0, 7, 12, GL_UNSIGNED_INT, static_cast<GLuint*>(nullptr));
        glDrawRangeElements(GL_LINES, 8, 11, 4, GL_UNSIGNED_INT, static_cast<GLuint*>(nullptr) + 12);
        glDrawRangeElements(GL_LINE_LOOP, 12, 15, 4, GL_UNSIGNED_INT, static_cast<GLuint*>(nullptr) + 16);
    } else {
        auto parts = PartsList{};

        auto render_part                      = Vdp1Part{};
        render_part.common_vdp_data_.vertexes = {
            {100, 100, 0.0f, 0.0f, 0.0f, 0xff, 0, 0, 0xff, Gouraud()},
            {100, 150, 1.0f, 0.0f, 0.0f, 0xff, 0, 0, 0xff, Gouraud()},
            {150, 150, 1.0f, 1.0f, 0.0f, 0xff, 0, 0, 0xff, Gouraud()},
            {150, 100, 0.0f, 1.0f, 0.0f, 0xff, 0, 0, 0xff, Gouraud()}
        };
        render_part.common_vdp_data_.draw_type    = DrawType::non_textured_polygon;
        render_part.common_vdp_data_.vdp_type     = VdpType::vdp1;
        render_part.common_vdp_data_.color_offset = {
            {true, true, true},
            {0x80, 0x80, 0x80}
        };

        parts.emplace_back(render_part);

        render_part.common_vdp_data_.vertexes = {
            {200, 200, 0.0f, 0.0f, 0.0f, 0, 0, 0xff, 0xff, Gouraud()},
            {200, 250, 1.0f, 0.0f, 0.0f, 0, 0, 0xff, 0xff, Gouraud()},
            {250, 250, 1.0f, 1.0f, 0.0f, 0, 0, 0xff, 0xff, Gouraud()},
            {250, 200, 0.0f, 1.0f, 0.0f, 0, 0, 0xff, 0xff, Gouraud()}
        };
        render_part.common_vdp_data_.draw_type    = DrawType::non_textured_polygon;
        render_part.common_vdp_data_.vdp_type     = VdpType::vdp1;
        render_part.common_vdp_data_.color_offset = {
            {1, 1, 1},
            {0, 0, 0}
        };
        parts.emplace_back(render_part);

        render_part.common_vdp_data_.vertexes = {
            {50,  50,  0.0f, 0.0f, 0.0f, 0, 0xff, 0, 0xff, Gouraud()},
            {100, 100, 1.0f, 0.0f, 0.0f, 0, 0xff, 0, 0xff, Gouraud()}
        };
        render_part.common_vdp_data_.draw_type = DrawType::line;
        render_part.common_vdp_data_.vdp_type  = VdpType::vdp1;
        // render_part.common_vdp_data_.color_offset = {0x50, 0x50, 0x50};
        parts.emplace_back(render_part);

        render_part.common_vdp_data_.vertexes = {
            {70,  70,  0.0f, 0.0f, 0.0f, 0xff, 0, 0, 0xff, Gouraud()},
            {120, 120, 1.0f, 0.0f, 0.0f, 0xff, 0, 0, 0xff, Gouraud()}
        };
        render_part.common_vdp_data_.draw_type = DrawType::line;
        render_part.common_vdp_data_.vdp_type  = VdpType::vdp1;
        // render_part.common_vdp_data_.color_offset = {0x50, 0x50, 0x50};
        parts.emplace_back(render_part);

        render_part.common_vdp_data_.vertexes = {
            {70,  70,  0.0f, 0.0f, 0.0f, 0, 0, 0xff, 0xff, Gouraud()},
            {70,  120, 1.0f, 0.0f, 0.0f, 0, 0, 0xff, 0xff, Gouraud()},
            {120, 120, 1.0f, 1.0f, 0.0f, 0, 0, 0xff, 0xff, Gouraud()},
            {120, 70,  0.0f, 1.0f, 0.0f, 0, 0, 0xff, 0xff, Gouraud()}
        };
        render_part.common_vdp_data_.draw_type = DrawType::polyline;
        render_part.common_vdp_data_.vdp_type  = VdpType::vdp1;
        // render_part.common_vdp_data_.color_offset = {0x50, 0x50, 0x50};
        parts.emplace_back(render_part);

        const auto&& [indices, draw_ranges] = generateVertexIndicesAndDrawRanges(parts);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);

        const auto vertexes = readVertexes(parts);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertexes.size(), vertexes.data(), GL_STATIC_DRAW);

        for (const auto& range : draw_ranges) {
            const auto is_texture_used = GLboolean(range.is_textured);
            glUniform1i(texture_used_loc, is_texture_used);

            glDrawRangeElements(range.primitive,
                                range.vertex_array_start,
                                range.vertex_array_end,
                                range.indices_nb,
                                GL_UNSIGNED_INT,
                                static_cast<GLuint*>(nullptr) + range.indices_array_start);
        }
    }

    gl::glDeleteBuffers(1, &vertex_buffer);
    gl::glDeleteVertexArrays(1, &vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    gl::glDeleteBuffers(1, &elements_buffer);

    postRender();
}

void Opengl::renderSelector() {
    if constexpr (render_type == RenderType::RenderType_drawElements) { render(); }
    if constexpr (render_type == RenderType::RenderType_drawTest) { renderTest(); }
}

auto Opengl::isThereSomethingToRender() const -> bool {
    if constexpr (render_type == RenderType::RenderType_drawElements) {
        if constexpr (uses_fbo) {
            return !global_parts_list_.empty();
        } else {
            return !parts_list_.empty();
        }
    }
    if constexpr (render_type == RenderType::RenderType_drawTest) { return true; }
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

void Opengl::renderVdp1DebugOverlay() {
    //----------- Pre render -----------//
    glBindFramebuffer(GLenum::GL_FRAMEBUFFER, fbo_type_to_id_[FboType::general]);

    attachTextureLayerToFbo(fbo_texture_array_id_,
                            getFboTextureLayer(FboTextureType::vdp1_debug_overlay),
                            GLenum::GL_FRAMEBUFFER,
                            GLenum::GL_COLOR_ATTACHMENT0);

    // Viewport is the entire Saturn framebuffer
    glViewport(0, 0, saturn_framebuffer_width, saturn_framebuffer_height);

    gl::glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Scissor test calculation, to remove from display everything outside the current Saturn resolution.
    glEnable(GL_SCISSOR_TEST);
    auto [scissor_x, scissor_y, scissor_width, scissor_height] = calculateViewportPosAndSize();
    glScissor(scissor_x, scissor_y, scissor_width, scissor_height);

    //----------- Render -----------------//
    // Calculating the ortho projection matrix
    const auto [vao_simple, vertex_buffer_simple] = initializeVao();
    const auto proj_matrix                        = calculateDisplayViewportMatrix();

    auto part = partToHighlight();

    auto vertexes = std::vector<Vertex>{};

    if (!part.common_vdp_data_.vertexes.empty()) {
        const auto debug_color                  = VertexColor({0xff, 0, 0, 0xff});
        part.common_vdp_data_.vertexes[0].color = debug_color;
        part.common_vdp_data_.vertexes[1].color = debug_color;
        part.common_vdp_data_.vertexes[2].color = debug_color;
        part.common_vdp_data_.vertexes[3].color = debug_color;

        vertexes.reserve(vertexes_per_tessellated_quad);
        vertexes.emplace_back(part.common_vdp_data_.vertexes[0]);
        vertexes.emplace_back(part.common_vdp_data_.vertexes[1]);
        vertexes.emplace_back(part.common_vdp_data_.vertexes[2]);
        vertexes.emplace_back(part.common_vdp_data_.vertexes[0]);
        vertexes.emplace_back(part.common_vdp_data_.vertexes[2]);
        vertexes.emplace_back(part.common_vdp_data_.vertexes[3]);

        glUseProgram(program_shaders_[ProgramShader::main]);
        glBindVertexArray(vao_simple);                       // binding VAO
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_simple); // binding vertex buffer

        // Sending vertex buffer data to the GPU
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertexes.size(), vertexes.data(), GL_STATIC_DRAW);

        // Sending the ortho projection matrix to the shader
        const auto uni_proj_matrix = glGetUniformLocation(program_shaders_[ProgramShader::main], "proj_matrix");
        glUniformMatrix4fv(uni_proj_matrix, 1, GL_FALSE, glm::value_ptr(proj_matrix));

        // Sending the variable to configure the shader to use color.
        const auto uni_use_texture = glGetUniformLocation(program_shaders_[ProgramShader::main], "is_texture_used");
        const auto is_texture_used = GLboolean(false);
        glUniform1i(uni_use_texture, is_texture_used);

        // Drawing the list
        glDrawArrays(GL_TRIANGLES, 0, vertexes_per_tessellated_quad);
    }

    //------ Post render --------//

    glDisable(GL_SCISSOR_TEST);
    glBindFramebuffer(GLenum::GL_FRAMEBUFFER, 0);
};

void Opengl::renderVdp2DebugLayer(core::EmulatorContext& state) {
    //----------- Pre render -----------//
    glBindFramebuffer(GLenum::GL_FRAMEBUFFER, fbo_type_to_id_[FboType::vdp2_debug]);

    // attachTextureLayerToFbo(fbo_texture_array_id_,
    //                         getFboTextureLayer(FboTextureType::vdp2_debug_layer),
    //                         GLenum::GL_DRAW_FRAMEBUFFER,
    //                         GLenum::GL_COLOR_ATTACHMENT0);

    // Viewport is the entire Saturn framebuffer
    glViewport(0, 0, saturn_framebuffer_width, saturn_framebuffer_height);

    gl::glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    //----------- Render -----------------//
    PartsList parts_list;
    if constexpr (false) {
        if (state.vdp2()->screenInDebug() != video::ScrollScreen::none) {
            const auto vdp2_parts = state.vdp2()->vdp2Parts(state.vdp2()->screenInDebug(), VdpType::vdp2_cell);
            if (!vdp2_parts.empty()) {
                parts_list.reserve(parts_list.size() + vdp2_parts.size());
                for (const auto& p : vdp2_parts) {
                    parts_list.emplace_back(p);
                }
            }
        }
    } else {
        auto render_part                      = Vdp1Part{};
        render_part.common_vdp_data_.vertexes = {
            {100, 100, 0.0f, 0.0f, 0.0f, 0xff, 0, 0, 0xff, Gouraud()},
            {100, 150, 1.0f, 0.0f, 0.0f, 0xff, 0, 0, 0xff, Gouraud()},
            {150, 150, 1.0f, 1.0f, 0.0f, 0xff, 0, 0, 0xff, Gouraud()},
            {150, 100, 0.0f, 1.0f, 0.0f, 0xff, 0, 0, 0xff, Gouraud()}
        };
        render_part.common_vdp_data_.draw_type    = DrawType::non_textured_polygon;
        render_part.common_vdp_data_.vdp_type     = VdpType::vdp1;
        render_part.common_vdp_data_.color_offset = {
            {true, true, true},
            {0x80, 0x80, 0x80}
        };

        parts_list.emplace_back(render_part);
    }

    if (!parts_list.empty()) {
        constexpr auto vertexes_per_tessellated_quad = u32{6}; // 2 triangles

        // Calculating the ortho projection matrix
        const auto [vao, vertex_buffer] = initializeVao();
        const auto proj_matrix          = calculateDisplayViewportMatrix();

        for (auto& part : parts_list) {
            if (part.vertexes.empty()) { continue; }

            glUseProgram(program_shaders_[ProgramShader::main]);
            glBindVertexArray(vao);                       // binding VAO
            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer); // binding vertex buffer

            // Sending the ortho projection matrix to the shader
            const auto uni_proj_matrix = glGetUniformLocation(program_shaders_[ProgramShader::main], "proj_matrix");
            glUniformMatrix4fv(uni_proj_matrix, 1, GL_FALSE, glm::value_ptr(proj_matrix));

            // Sending the variable to configure the shader to use texture data.
            const auto uni_use_texture = glGetUniformLocation(program_shaders_[ProgramShader::main], "is_texture_used");
            const auto is_texture_used = GLboolean(true);
            glUniform1i(uni_use_texture, is_texture_used);

            // const auto opengl_tex = getOpenglTexture(part.texture_key);
            // if (opengl_tex.has_value()) {
            //     // Replacing texture coordinates of the vertex by those of the OpenGL texture.
            //     for (auto& v : part.vertexes) {
            //         if ((v.tex_coords.s == 0.0) && (v.tex_coords.t == 0.0)) {
            //             v.tex_coords = opengl_tex->coords[0];
            //             continue;
            //         }
            //         if ((v.tex_coords.s == 1.0) && (v.tex_coords.t == 0.0)) {
            //             v.tex_coords = opengl_tex->coords[1];
            //             continue;
            //         }
            //         if ((v.tex_coords.s == 1.0) && (v.tex_coords.t == 1.0)) {
            //             v.tex_coords = opengl_tex->coords[2];
            //             continue;
            //         }
            //         if ((v.tex_coords.s == 0.0) && (v.tex_coords.t == 1.0)) {
            //             v.tex_coords = opengl_tex->coords[3];
            //             continue;
            //         }
            //     }
            //     glBindTexture(GL_TEXTURE_2D, (*opengl_tex).opengl_id);
            // }

            // Quad is tessellated into 2 triangles, using a texture

            auto vertexes = std::vector<Vertex>{};

            vertexes.reserve(vertexes_per_tessellated_quad);
            // Transforming one quad in 2 triangles
            vertexes.emplace_back(part.vertexes[0]);
            vertexes.emplace_back(part.vertexes[1]);
            vertexes.emplace_back(part.vertexes[2]);
            vertexes.emplace_back(part.vertexes[0]);
            vertexes.emplace_back(part.vertexes[2]);
            vertexes.emplace_back(part.vertexes[3]);

            // Sending vertex buffer data to the GPU
            glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertexes.size(), vertexes.data(), GL_STATIC_DRAW);
            // glActiveTexture(GLenum::GL_TEXTURE0);

            glDrawArrays(GL_TRIANGLES, 0, vertexes_per_tessellated_quad);
        }

        gl::glDeleteBuffers(1, &vertex_buffer);
        gl::glDeleteVertexArrays(1, &vao);
    }

    //------ Post render --------//
    // Framebuffer is released
    gl::glBindFramebuffer(GL_FRAMEBUFFER, 0);

}; // namespace saturnin::video

void Opengl::addOrUpdateTexture(const size_t key, const VdpLayer layer) {
    // If the key doesn't exist it will be automatically added.
    // If it does, opengl link will be reset in order to regenerate the texture.
    // const auto opengl_tex = getOpenglTexture(key);
    // const auto& texture = Texture::getTexture(key);

    // if (texture) {
    std::lock_guard lock(textures_link_mutex_);
    textures_link_[key].key                 = key;
    textures_link_[key].opengl_id           = 0;
    textures_link_[key].texture_array_index = 0;
    layer_to_cache_reload_state_[layer]     = true; // This layer will have to be reloaded
    // textures_link_[key].width     = (*texture)->width();
    // textures_link_[key].height    = (*texture)->height();
    // textures_link_[key].size      = (*texture)->size();
    // }
}

void Opengl::removeTextureLink(const size_t key) { textures_link_.erase(key); }

void Opengl::generateTextures() {
    // Textures are generated in a 128 layers texture array, each layer being a texture atlas of
    // 1024*1024 pixels.
    // In theory, the maximum number of different VDP2 cells that could be stored by the Saturn at a given time is 0x80000
    // (ie both RBG at maximum size, with every cell different).
    // Maximum VDP1 texture size is 504*255.
    // Textures are grouped by layers, meaning that every layer can be linked to multiple texture array indexes. This way
    // it's not mandatory to regenerate the whole texture atlas every frame.

    // OpenglTextures don't have to be deleted, as they are part of a texture atlas now

    //

    auto local_textures_link = TexturesLink();
    local_textures_link      = textures_link_;

    auto layer_to_textures = LayerToTextures{};

    for (const auto& [key, ogl_tex] : local_textures_link) {
        const auto t = Texture::getTexture(key);

        auto opengl_tex = getOpenglTexture(key);
        if (opengl_tex) { (*opengl_tex).size = {(**t).width(), (**t).height()}; }
        layer_to_textures[(*t)->layer()].push_back(*opengl_tex);
    }

    for (auto& [layer, textures] : layer_to_textures) {
        if (layer_to_cache_reload_state_[layer]) { packTextures(textures, layer); }
    }

    // Reset cache reload state
    for (auto& [layer, state] : layer_to_cache_reload_state_) {
        state = false;
    }
}

// Using the simplest (and fastest) method to pack textures in the atlas. Better algorithms could be used to
// improve packing, but there's a non trivial performance tradeoff, with a big increase in complexity.
// So for now, keeping things simple.
void Opengl::packTextures(std::vector<OpenglTexture>& textures, const VdpLayer layer) {
    glBindTexture(GL_TEXTURE_2D_ARRAY, texture_array_id_);
    // First, indexes used by the layer are cleared and removed from the used list.
    auto empty_data = std::vector<u8>(texture_array_width * texture_array_height * 4);
    if (!layer_to_texture_array_indexes_[layer].empty()) {
        for (const auto& index : layer_to_texture_array_indexes_[layer]) {
            glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
                            0,
                            0,
                            0,
                            index,
                            texture_array_width,
                            texture_array_height,
                            1,
                            GLenum::GL_RGBA,
                            GLenum::GL_UNSIGNED_BYTE,
                            empty_data.data());
        }
        std::vector<u8>().swap(layer_to_texture_array_indexes_[layer]);
    }

    // Second, texture are sorted by horizontal size.
    std::ranges::sort(textures, [](const OpenglTexture& a, const OpenglTexture& b) { return a.size.h > b.size.h; });

    auto current_index          = getCurrentTextureArrayIndex(layer);
    auto x_pos                  = u16{};
    auto y_pos                  = u16{};
    auto current_row_max_height = u16{};

    for (const auto& texture : textures) {
        if ((x_pos + texture.size.w) > texture_array_width) {
            // Texture doesn't fit in the remaining space of the row ... looping around to next row using
            // the maximum height from the previous row.
            y_pos += current_row_max_height;
            x_pos                  = 0;
            current_row_max_height = 0;
        }

        if ((y_pos + texture.size.h) > texture_array_width) {
            // Texture doesn't fit in the remaining space of the last row ... moving one layer forward.
            current_index = getNextAvailableTextureArrayIndex();
            layer_to_texture_array_indexes_.at(layer).push_back(current_index);
            x_pos                  = 0;
            y_pos                  = 0;
            current_row_max_height = 0;
        }
        // This is the position of the rectangle
        textures_link_[texture.key].texture_array_index = current_index;
        textures_link_[texture.key].size                = texture.size;
        textures_link_[texture.key].pos                 = {x_pos, y_pos};

        textures_link_[texture.key].coords = calculateTextureCoordinates({x_pos, y_pos}, texture.size, current_index);

        generateSubTexture(texture.key);

        x_pos += texture.size.w; // Move along to the next spot in the row.

        // Just saving the largest height in the new row
        if (texture.size.h > current_row_max_height) { current_row_max_height = texture.size.h; }
    }
    // texture_array_max_used_layer_ = current_layer;
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

auto Opengl::calculateTextureCoordinates(const ScreenPos& pos,
                                         const Size&      size,
                                         const u8         texture_array_index) const -> std::vector<TextureCoordinates> {
    // Calculating the texture coordinates in the atlas
    // (x1,y1)     (x2,y1)
    //        .---.
    //        |   |
    //        .---.
    // (x1,y2)     (x2,y2)

    auto x1 = static_cast<float>(pos.x) / static_cast<float>(texture_array_width);
    auto x2 = static_cast<float>(pos.x + size.w) / static_cast<float>(texture_array_width);
    auto y1 = static_cast<float>(pos.y) / static_cast<float>(texture_array_height);
    auto y2 = static_cast<float>(pos.y + size.h) / static_cast<float>(texture_array_height);

    return std::vector{
        TextureCoordinates{x1, y1, static_cast<float>(texture_array_index)},
        TextureCoordinates{x2, y1, static_cast<float>(texture_array_index)},
        TextureCoordinates{x2, y2, static_cast<float>(texture_array_index)},
        TextureCoordinates{x1, y2, static_cast<float>(texture_array_index)}
    };
}

auto Opengl::readVertexes(const PartsList& parts) -> std::vector<Vertex> {
    auto vertexes = std::vector<Vertex>{};
    for (auto p : parts) {
        const auto opengl_tex = getOpenglTexture(p.texture_key);
        if (opengl_tex.has_value()) {
            // Replacing texture coordinates of the vertex by those of the OpenGL texture.
            for (auto& v : p.vertexes) {
                v.color_offset = p.color_offset;
                if ((v.tex_coords.s == 0.0) && (v.tex_coords.t == 0.0)) {
                    v.tex_coords = opengl_tex->coords[0];
                    continue;
                }
                if ((v.tex_coords.s == 1.0) && (v.tex_coords.t == 0.0)) {
                    v.tex_coords = opengl_tex->coords[1];
                    continue;
                }
                if ((v.tex_coords.s == 1.0) && (v.tex_coords.t == 1.0)) {
                    v.tex_coords = opengl_tex->coords[2];
                    continue;
                }
                if ((v.tex_coords.s == 0.0) && (v.tex_coords.t == 1.0)) {
                    v.tex_coords = opengl_tex->coords[3];
                    continue;
                }
            }
        } else {
            for (auto& v : p.vertexes) {
                v.color_offset = p.color_offset;
            }
        }
        std::ranges::copy(p.vertexes.begin(), p.vertexes.end(), std::back_inserter(vertexes));
    }

    return vertexes;
}

auto Opengl::getOpenglTexture(const size_t key) -> std::optional<OpenglTexture> {
    std::lock_guard lock(textures_link_mutex_);
    const auto      it = textures_link_.find(key);
    return (it == textures_link_.end()) ? std::nullopt : std::optional<OpenglTexture>(it->second);
}

auto Opengl::getOpenglTextureDetails(const size_t key) -> std::string {
    auto            details = std::string{};
    std::lock_guard lock(textures_link_mutex_);
    if (const auto it = textures_link_.find(key); it != textures_link_.end()) {
        details += util::format("Key: 0x{:x}\n", it->second.key);
        details += util::format("Position: {},{}\n", it->second.pos.x, it->second.pos.y);
        details += util::format("Size: {},{}\n", it->second.size.w, it->second.size.h);
        details += util::format("Layer: {}\n", it->second.texture_array_index);
        details += util::format("X1: {},{},{}\n", it->second.coords[0].s, it->second.coords[0].t, it->second.coords[0].p);
        details += util::format("X2: {},{},{}\n", it->second.coords[1].s, it->second.coords[1].t, it->second.coords[1].p);
        details += util::format("Y2: {},{},{}\n", it->second.coords[2].s, it->second.coords[2].t, it->second.coords[2].p);
        details += util::format("Y1: {},{},{}\n", it->second.coords[3].s, it->second.coords[3].t, it->second.coords[3].p);
    }
    return details;
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

void Opengl::generateSubTexture(const size_t key) {
    const auto ogl_tex = getOpenglTexture(key);

    if (ogl_tex) {
        const auto tex = Texture::getTexture(key);
        if (tex) {
            glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
                            0,
                            ogl_tex->pos.x,
                            ogl_tex->pos.y,
                            ogl_tex->texture_array_index,
                            ogl_tex->size.w,
                            ogl_tex->size.h,
                            1,
                            GLenum::GL_RGBA,
                            GLenum::GL_UNSIGNED_BYTE,
                            (*tex)->rawData().data());
            checkGlError();
        }
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
auto Opengl::getTextureId(const TextureArrayType type) -> u32 {
    // There are 2 texture arrays used in the program :
    // - one is used as a texture atlas for VDP parts rendering (texture_array_id_)
    // - the other as framebuffers linked to the main FBO, handling debug layers, priority rendering, front and back
    // framebuffer (fbo_texture_array_id_), etc.

    switch (type) {
        using enum TextureArrayType;
        case saturn_part: {
            // Checking if the texture already exists. If not it'll be created.
            if (texture_array_debug_layer_id_ == 0) {
                texture_array_debug_layer_id_ = generateTexture(texture_array_width, texture_array_height, std::vector<u8>{});
            }
            // :TODO: update texture data based on the texture array layer specified.

            return texture_array_debug_layer_id_;
        }
        case framebuffer: {
            // :TODO: Search in the texture pool the id of the texture ... is it really needed ?
            break;
        }
    }

    return u32();
};

void Opengl::onWindowResize(const u16 width, const u16 height) { hostScreenResolution({width, height}); }

auto Opengl::createVertexShader(std::string_view name) -> u32 {
    const auto vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    auto       source        = getShaderSource(name);
    glShaderSource(vertex_shader, 1, &source, nullptr);
    glCompileShader(vertex_shader);
    checkShaderCompilation(vertex_shader);

    return vertex_shader;
}

auto Opengl::createFragmentShader(std::string_view name) -> u32 {
    const auto fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    auto       source          = getShaderSource(name);
    glShaderSource(fragment_shader, 1, &source, nullptr);
    glCompileShader(fragment_shader);

    checkShaderCompilation(fragment_shader);

    return fragment_shader;
}

// static
auto Opengl::createProgramShader(const u32 vertex_shader, const u32 fragment_shader) -> u32 {
    const auto shader_program = glCreateProgram();

    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    checkProgramCompilation(shader_program);

    return shader_program;
}

// static
void Opengl::deleteShaders(const std::vector<u32>& shaders) {
    for (auto shader : shaders) {
        glDeleteShader(shader);
    }
}

// static
auto Opengl::generateTexture(const u32 width, const u32 height, const std::vector<u8>& data) -> u32 {
    auto texture = u32{};
    glGenTextures(1, &texture);
    glActiveTexture(GLenum::GL_TEXTURE0);
    glBindTexture(GLenum::GL_TEXTURE_2D, texture);

    // set the texture wrapping parameters
    glTexParameteri(GLenum::GL_TEXTURE_2D, GLenum::GL_TEXTURE_WRAP_S, GLenum::GL_REPEAT);
    glTexParameteri(GLenum::GL_TEXTURE_2D, GLenum::GL_TEXTURE_WRAP_T, GLenum::GL_REPEAT);

    // set texture filtering parameters
    glTexParameteri(GLenum::GL_TEXTURE_2D, GLenum::GL_TEXTURE_MIN_FILTER, GLenum::GL_NEAREST);
    glTexParameteri(GLenum::GL_TEXTURE_2D, GLenum::GL_TEXTURE_MAG_FILTER, GLenum::GL_NEAREST);

    glTexImage2D(GLenum::GL_TEXTURE_2D,
                 0,
                 GLenum::GL_RGBA,
                 width,
                 height,
                 0,
                 GLenum::GL_RGBA,
                 GLenum::GL_UNSIGNED_BYTE,
                 data.data());

    glBindTexture(GLenum::GL_TEXTURE_2D, 0);

    return texture;
}

// static
void Opengl::deleteTexture(const u32 texture) {
    //
    glDeleteTextures(1, &texture);
}

static void error_callback(int error, const char* description) { fprintf(stderr, "Error %d: %s\n", error, description); }

auto Opengl::getShaderSource(std::string_view name) -> const char* {
    if (auto search = shaders_list_.find(name); search != shaders_list_.end()) {
        return search->second.c_str();

    } else {
        Log::exception(Logger::opengl, "Shader {} not found !", name);
    }
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
    glVertexAttribPointer(0, 2, GLenum::GL_SHORT, GL_FALSE, sizeof(Vertex), 0); // NOLINT: this is an index
    glEnableVertexAttribArray(0);                                               // NOLINT: this is an index

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
    fbo_texture_type_to_layer_ = {FboTextureType::front_buffer,
                                  FboTextureType::back_buffer,
                                  FboTextureType::vdp1_debug_overlay,
                                  FboTextureType::vdp2_debug_layer};
    for (u8 i = 4; i < fbo_texture_array_depth; ++i) {
        fbo_texture_type_to_layer_[i] = FboTextureType::priority;
    }

    currentRenderedBuffer(FboTextureType::front_buffer);

    // Generating the main FBO used by the renderer.
    // fbo_ = generateFbo(FboType::general);

    fbo_type_to_id_[FboType::general]    = generateFbo(FboType::general);
    fbo_type_to_id_[FboType::for_gui]    = generateFbo(FboType::for_gui);
    fbo_type_to_id_[FboType::vdp2_debug] = generateFbo(FboType::vdp2_debug);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo_type_to_id_[FboType::general]);
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

            glBindTexture(GLenum::GL_TEXTURE_2D_ARRAY, fbo_texture_array_id_);

            glEnable(GLenum::GL_BLEND);
            glBlendFunc(GLenum::GL_SRC_ALPHA, GLenum::GL_ONE_MINUS_SRC_ALPHA);

            // Attaching the color texture currently used as framebuffer to the FBO.
            attachTextureLayerToFbo(fbo_texture_array_id_,
                                    getFboTextureLayer(currentRenderedBuffer()),
                                    GLenum::GL_READ_FRAMEBUFFER,
                                    GLenum::GL_COLOR_ATTACHMENT0);

            attachTextureToFbo(gui_texture_type_to_id_[GuiTextureType::vdp2_debug_buffer],
                               GLenum::GL_DRAW_FRAMEBUFFER,
                               GLenum::GL_COLOR_ATTACHMENT1);

            glDrawBuffer(GL_COLOR_ATTACHMENT1);
            break;
        }
    }

    const auto status = gl33core::glCheckFramebufferStatus(GLenum::GL_FRAMEBUFFER);
    if (status != gl::GLenum::GL_FRAMEBUFFER_COMPLETE) {
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
    // glTexParameteri(GLenum::GL_TEXTURE_2D_ARRAY, GLenum::GL_TEXTURE_MIN_FILTER, GLenum::GL_LINEAR);
    // glTexParameteri(GLenum::GL_TEXTURE_2D_ARRAY, GLenum::GL_TEXTURE_MAG_FILTER, GLenum::GL_LINEAR);
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

auto isModernOpenglCapable() -> bool {
    if (glfwInit() == GLFW_FALSE) { return false; }

    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    constexpr auto h_window_size = u16{1280};
    constexpr auto v_window_size = u16{720};

    auto window = glfwCreateWindow(h_window_size, v_window_size, "", nullptr, nullptr);
    if (window == nullptr) {
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);

    glbinding::initialize(glfwGetProcAddress, false);
    const auto version = glbinding::aux::ContextInfo::version();

    glfwDestroyWindow(window);
    glfwTerminate();

    Log::info(Logger::opengl, "Max version supported : {}", version.toString());

    return (version >= glbinding::Version(3, 3));
}

void windowCloseCallback(GLFWwindow* window) {
    auto state = reinterpret_cast<core::EmulatorContext*>(glfwGetWindowUserPointer(window));
    state->stopEmulation();
    state->renderingStatus(core::RenderingStatus::stopped);

    // Adding a delay to allow the thread to finish cleanly.
    using namespace std::this_thread;     // sleep_for
    using namespace std::chrono_literals; // ms
    const auto time_to_sleep = 20ms;
    sleep_for(time_to_sleep);
}

void glDebugOutput(gl::GLenum   source,
                   gl::GLenum   type,
                   unsigned int id,
                   gl::GLenum   severity,
                   gl::GLsizei  length,
                   const char*  message,
                   const void*  userParam) {
    // ignore non-significant error/warning codes
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

    // Log::warning(Logger::opengl, "OpenGL error : {}", (int)error); }
    Log::warning(Logger::opengl, "---------------");
    Log::warning(Logger::opengl, "Debug message ({}): {}", id, message);

    auto source_str = std::string{};
    switch (source) {
        case GL_DEBUG_SOURCE_API: source_str = "API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM: source_str = "Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: source_str = "Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY: source_str = "Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION: source_str = "Application"; break;
        case GL_DEBUG_SOURCE_OTHER: source_str = "Other"; break;
    }
    Log::warning(Logger::opengl, "Source: {}", source_str);

    auto type_str = std::string{};
    switch (type) {
        case GL_DEBUG_TYPE_ERROR: type_str = "Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: type_str = "Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: type_str = "Undefined Behaviour"; break;
        case GL_DEBUG_TYPE_PORTABILITY: type_str = "Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE: type_str = "Performance"; break;
        case GL_DEBUG_TYPE_MARKER: type_str = "Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP: type_str = "Type: Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP: type_str = "Type: Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER: type_str = "Type: Other"; break;
    }
    Log::warning(Logger::opengl, "Type: {}", type_str);

    auto severity_str = std::string{};
    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH: severity_str = "high"; break;
        case GL_DEBUG_SEVERITY_MEDIUM: severity_str = "medium"; break;
        case GL_DEBUG_SEVERITY_LOW: severity_str = "low"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: severity_str = "notification"; break;
    }
    Log::warning(Logger::opengl, "Severity: {}", severity_str);
    // std::cout << std::endl;
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

void updateMainWindowSizeAndRatio(GLFWwindow* window, const u32 width, const u32 height) {
    glfwSetWindowSizeLimits(window, width, height, GLFW_DONT_CARE, GLFW_DONT_CARE);
}

auto createMainWindow(const u32 width, const u32 height, const std::string& title) -> GLFWwindow* {
    return glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
}

auto loadPngImage(const char* filename) -> GLFWimage {
    const auto full_path{std::filesystem::current_path() / "res" / filename};
    auto       image  = GLFWimage{};
    auto       width  = u32{};
    auto       height = u32{};
    const auto error  = lodepng_decode32_file(&(image.pixels), &width, &height, full_path.string().c_str());
    if (error != 0) {
        Log::warning(Logger::opengl, lodepng_error_text(error));
        return image;
    }
    image.width  = width;
    image.height = height;

    return image;
}

auto loadPngImage(const u8* data, const size_t size) -> GLFWimage {
    // const auto full_path{std::filesystem::current_path() / "res" / filename};
    auto       image  = GLFWimage{};
    auto       width  = u32{};
    auto       height = u32{};
    const auto error  = lodepng_decode32(&(image.pixels), &width, &height, data, size);
    if (error != 0) {
        Log::warning(Logger::opengl, lodepng_error_text(error));
        return image;
    }
    image.width  = width;
    image.height = height;

    return image;
}

void windowSizeCallback(GLFWwindow* window, int width, int height) {
    const auto state = std::bit_cast<core::EmulatorContext*>(glfwGetWindowUserPointer(window));
    state->opengl()->onWindowResize(width, height);
}

void checkShaderCompilation(const u32 shader) {
    auto success = GLboolean{};
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    auto length = s32{};
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    if (success == GL_FALSE) {
        auto v = std::vector<char>(length);
        glGetShaderInfoLog(shader, length, nullptr, v.data());
        const auto info = std::string(v.begin(), v.end());

        auto type        = GLenum{};
        auto shader_type = std::string{};
        glGetShaderiv(shader, GL_SHADER_TYPE, &type);
        switch (type) {
            case GL_VERTEX_SHADER: shader_type = "Vertex shader"; break;
            case GL_FRAGMENT_SHADER: shader_type = "Fragment shader"; break;
            default: shader_type = "Unknown shader"; break;
        }

        Log::exception(Logger::opengl, "{} compilation failed : {}", shader_type, info);
    }
}

void checkProgramCompilation(const u32 program) {
    auto success = GLboolean{};
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    auto length = s32{};
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

    if (success == GL_FALSE) {
        auto v = std::vector<char>(length);
        glGetProgramInfoLog(program, length, nullptr, v.data());
        const auto info = std::string(v.begin(), v.end());

        Log::exception(Logger::opengl, "Shader program link failed : {}", info);
    }
}

void checkGlError() {
    if (check_gl_error) {
        const auto error = glGetError();
        if (error != GLenum::GL_NO_ERROR) { Log::warning(Logger::opengl, "OpenGL error : {}", (int)error); }
    }
}

auto generateVertexIndicesAndDrawRanges(const PartsList& parts) -> std::tuple<std::vector<u32>, std::vector<DrawRange>> {
    const auto indices_per_polygon  = std::vector<u32>{0, 1, 2, 0, 2, 3};
    const auto indices_per_polyline = std::vector<u32>{0, 1, 2, 3};
    const auto indices_per_line     = std::vector<u32>{0, 1};

    using TypeToIndices      = std::map<const DrawType, const std::vector<u32>>;
    const auto typeToIndices = TypeToIndices{
        {DrawType::textured_polygon,     indices_per_polygon },
        {DrawType::non_textured_polygon, indices_per_polygon },
        {DrawType::polyline,             indices_per_polyline},
        {DrawType::line,                 indices_per_line    }
    };

    using TypeToVertexIncrement      = std::map<const DrawType, const u32>;
    const auto typeToVertexIncrement = TypeToVertexIncrement{
        {DrawType::textured_polygon,     4},
        {DrawType::non_textured_polygon, 4},
        {DrawType::polyline,             4},
        {DrawType::line,                 2}
    };

    using TypeIsTextured      = std::map<const DrawType, const bool>;
    const auto typeIsTextured = TypeIsTextured{
        {DrawType::textured_polygon,     true },
        {DrawType::non_textured_polygon, false},
        {DrawType::polyline,             false},
        {DrawType::line,                 false}
    };

    using TypeToPrimitive      = std::map<const DrawType, const gl::GLenum>;
    const auto typeToPrimitive = TypeToPrimitive{
        {DrawType::textured_polygon,     gl::GL_TRIANGLES},
        {DrawType::non_textured_polygon, gl::GL_TRIANGLES},
        {DrawType::polyline,             gl::GL_LINE_LOOP},
        {DrawType::line,                 gl::GL_LINES    }
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

            // current_range.is_textured = typeIsTextured.at(p.draw_type);

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
