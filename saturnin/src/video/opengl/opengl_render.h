//
// opengl_render.h
// Saturnin
//
// Copyright (c) 2024 Renaud Toumazet
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

#pragma once

#include <array>
#include <saturnin/src/emulator_defs.h>
#include <saturnin/src/video/opengl/opengl.h>
#include <saturnin/src/video/opengl/opengl_texturing.h>
#include <saturnin/src/video/render_interface.h>
#include <saturnin/src/video/vdp_common.h>

namespace saturnin::video {

enum class RenderType { RenderType_drawElements, RenderType_drawTest };
constexpr auto render_type = RenderType::RenderType_drawElements;

using MapOfPartsList = std::map<FboKey, PartsList>; // Parts list by priority + layer

class OpenglRender : public RenderInterface {
  public:
    OpenglRender() = default;
    explicit OpenglRender(Opengl* opengl) : opengl_(opengl) {};
    ~OpenglRender()                              = default;
    OpenglRender(const OpenglRender&)            = delete;
    OpenglRender(OpenglRender&&)                 = delete;
    OpenglRender& operator=(const OpenglRender&) = delete;
    OpenglRender& operator=(OpenglRender&&)      = delete;

    void initialize() override;
    void shutdown() const override;

    // Pre/post rendering functions
    void preRender();
    void postRender() const;

    // Renders the list of parts to a specific texture.
    virtual void renderParts(const PartsList& parts_list, const u32 texture_id);
    // Renders all priority lists in painter's order. The compute backend flattens them into one dispatch.
    virtual void renderPartsLists(MapOfPartsList& lists, const u32 texture_id);

    // Called once after every OpenGL module (renderer + texturing) is initialized.
    virtual void postInitialize() {}

    // Renders data if available.
    void render();
    void renderByScreenPriority();
    void renderByParts();

    // Renders test data if available.
    void renderTest();

    // Renders the vertexes of the vdp1 part currently saved to a specific overlay layer. Useful for debugging.
    void renderVdp1DebugOverlay() override;

    // Renders the vertexes of the vdp2 layer currently selected for display in the debug window.
    void renderVdp2DebugLayer(core::EmulatorContext& state) override;

    // Selects the renderer to use, depending on conditions
    void renderSelector() override;

    // Checks if there's something to render.
    auto isThereSomethingToRender() const -> bool override;

    // Switch between back and front rendering buffers.
    void switchRenderedBuffer();

    // Calculates the display viewport matrix, adding letterbox or pillarbox when the display isn't exactly like the Saturn
    // resolution.
    auto calculateDisplayViewportMatrix() const -> glm::highp_mat4;

    // Calculates and returns viewport position and size.
    auto calculateViewportPosAndSize() const -> std::tuple<u32, u32, u32, u32>;

    // Displays the framebuffer content (VDP1 + VDP2)
    void displayFramebuffer(core::EmulatorContext& state) override;
    void displayFramebufferByScreenPriority(core::EmulatorContext& state);
    void displayFramebufferByParts(core::EmulatorContext& state);

    // Accessors / mutators
    void               partToHighlight(const Vdp1Part& part) override { part_to_highlight_ = part; };
    auto               partToHighlight() const -> Vdp1Part { return part_to_highlight_; };
    [[nodiscard]] auto currentRenderedBuffer() const -> FboTextureType override { return current_rendered_buffer_; };
    void               currentRenderedBuffer(const FboTextureType type) override { current_rendered_buffer_ = type; };

  protected:
    Opengl* opengl_{nullptr};

  private:
    Vdp1Part part_to_highlight_{}; // Part that will be highlighted during debug.

    FboTextureType current_rendered_buffer_{}; // The current rendered buffer (front or back)

    // Following parts data will have to be moved to the platform agnostic renderer
    MapOfPartsList parts_lists_; // Map of parts_list, one entry by FboKey. When used with use_fbo = false, all parts are using
                                 // the same map entry.
    PartsList parts_list_debug_; // List of parts used to generate textures for debugging.

    std::condition_variable data_condition_; // Condition variable to synchronize between emulation and UI thread.

    // Double-buffered handoff for the mixed parts path: the emulation thread deposits into
    // render_slots_[render_slot_write_] and only blocks when that slot is still unconsumed, so it can run one
    // frame ahead of the render thread. The consumer always takes the newest slot and drops the stale one.
    std::array<PartsList, 2> render_slots_{};
    u8                       render_slot_write_{0};

    // Persistent GL objects shared by every draw path (created once in initialize()). Creating and deleting a
    // VAO + two buffers on every draw call was pure driver overhead.
    mutable u32 vao_{0};
    mutable u32 vertex_buffer_{0};
    mutable u32 elements_buffer_{0};

    // Uniform locations are constant for the program's lifetime: queried once instead of per draw call.
    s32 uni_proj_matrix_{-1};
    s32 uni_sampler_{-1};
    s32 uni_is_texture_used_{-1};

    Shaders shaders_{}; // Shaders storage
};

}; // namespace saturnin::video
