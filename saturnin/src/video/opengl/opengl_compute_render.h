//
// opengl_compute_render.h
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

#include <saturnin/src/video/opengl/opengl_render.h>
#include <saturnin/src/video/vdp_common.h>

namespace saturnin::video {

// Byte-compatible with the std430 structs declared in shaders/raster.comp.
// 6 * vec4 = 96 bytes.
struct GpuVertex {
    float pos[4];
    float tex[4];
    float color[4];
    float gouraud[4];
    float offset_pos[4];
    float offset_val[4];
};

// 4 * uint = 16 bytes.
struct GpuPart {
    u32 first;
    u32 count;
    u32 flags; ///< bit0: textured, bits1-2: primitive kind (0 polygon, 1 polyline, 2 line).
    u32 pad;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class	OpenglComputeRender
///
/// \brief	OpenGL 4.3 compute-shader backend.
///
/// Reuses every non-raster stage of OpenglRender (parts list, pre/post render, display, GUI) and
/// replaces only the polygon draw path: the parts list is flattened into SSBOs and rasterized by a
/// single compute dispatch writing straight into the target image.
////////////////////////////////////////////////////////////////////////////////////////////////////

class OpenglComputeRender final : public OpenglRender {
  public:
    explicit OpenglComputeRender(Opengl* opengl) : OpenglRender(opengl) {};

    void initialize() override;
    void renderParts(const PartsList& parts_list, const u32 texture_id) override;
    void renderPartsLists(const MapOfPartsList& lists, const u32 texture_id) override;
    void postInitialize() override;

  private:
    void selfTest();
    void parityTest();

    u32 compute_program_{0};
    u32 vertex_ssbo_{0};
    u32 part_ssbo_{0};
};

} // namespace saturnin::video
