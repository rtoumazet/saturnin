//
// render_interface.h
// Saturnin
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

#include <saturnin/src/emulator_defs.h>

namespace saturnin::core {
class EmulatorContext;
} // namespace saturnin::core

namespace saturnin::video {

class Vdp1Part;
enum class FboTextureType : u8;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class	RenderInterface
///
/// \brief	Platform-agnostic rendering interface.
///
/// Every backend (OpenGL 3.3, OpenGL 4.3 compute, ...) implements it, and the rest of the
/// application talks only to this interface. Only the surface actually used by call sites is
/// declared here; everything else stays private to the concrete backend.
////////////////////////////////////////////////////////////////////////////////////////////////////

class RenderInterface {
  public:
    RenderInterface()                                  = default;
    virtual ~RenderInterface()                         = default;
    RenderInterface(const RenderInterface&)            = delete;
    RenderInterface(RenderInterface&&)                 = delete;
    RenderInterface& operator=(const RenderInterface&) = delete;
    RenderInterface& operator=(RenderInterface&&)      = delete;

    // Lifecycle
    virtual void initialize() = 0;
    virtual void shutdown() const = 0;

    // Called once after every rendering module (renderer + texturing) has been initialized.
    virtual void postInitialize() {}

    // Rendering entry points
    virtual void renderSelector() = 0;
    [[nodiscard]] virtual auto isThereSomethingToRender() const -> bool = 0;
    virtual void renderVdp1DebugOverlay() = 0;
    virtual void renderVdp2DebugLayer(core::EmulatorContext& state) = 0;
    virtual void displayFramebuffer(core::EmulatorContext& state) = 0;

    // Debug / buffer selection accessors
    virtual void partToHighlight(const Vdp1Part& part) = 0;
    [[nodiscard]] virtual auto currentRenderedBuffer() const -> FboTextureType = 0;
    virtual void currentRenderedBuffer(const FboTextureType type) = 0;
};

} // namespace saturnin::video
