//
// opengl_compute_render.cpp
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

#include <saturnin/src/video/opengl/opengl_compute_render.h>

#include <glbinding/glbinding.h>
#include <glbinding/gl/gl.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <ranges>
#include <utility>
#include <vector>

#include <glm/gtc/type_ptr.hpp>

#include <saturnin/src/video/opengl/opengl_shaders.h>
#include <saturnin/src/video/opengl/opengl_texturing.h>
#include <saturnin/src/video/opengl/opengl_utilities.h>
#include <saturnin/src/log.h>
#include <saturnin/src/video/opengl/opengl_utilities.h>
#include <saturnin/src/video/vdp1_part.h>
#include <cstdlib>

using namespace gl;

namespace saturnin::video {

namespace {

// Builds a textured quad sampling the given atlas layer over the unit square.
auto makeTexturedQuad(const s16 x0, const s16 y0, const s16 x1, const s16 y1, const float layer, const u8 priority)
    -> RenderPart {
    Vdp1Part vp{};
    vp.common_vdp_data_.draw_type  = DrawType::textured_polygon;
    vp.common_vdp_data_.priority   = priority;
    vp.common_vdp_data_.texture_key = 0xDEADu; // no registered texture: keeps the raw tex coords

    const auto add = [&vp, layer](const s16 x, const s16 y, const float s, const float t) {
        auto v           = Vertex(x, y, s, t);
        v.tex_coords.p   = layer;
        v.color          = VertexColor(255, 255, 255, 255);
        v.color_offset   = ColorOffset{};
        vp.common_vdp_data_.vertexes.push_back(v);
    };
    add(x0, y0, 0.0f, 0.0f);
    add(x1, y0, 1.0f, 0.0f);
    add(x1, y1, 1.0f, 1.0f);
    add(x0, y1, 0.0f, 1.0f);
    return RenderPart{vp};
}

// A closed 4-vertex line loop (GL_LINE_LOOP) or a single segment (GL_LINES).
auto makeLines(const DrawType type, const std::vector<std::pair<s16, s16>>& points, const u8 r, const u8 g, const u8 b,
               const u8 priority) -> RenderPart {
    Vdp1Part vp{};
    vp.common_vdp_data_.draw_type = type;
    vp.common_vdp_data_.priority  = priority;
    for (const auto& [x, y] : points) {
        auto v           = Vertex(x, y, 0.0f, 0.0f);
        v.color          = VertexColor(r, g, b, 255);
        v.color_offset   = ColorOffset{};
        vp.common_vdp_data_.vertexes.push_back(v);
    }
    return RenderPart{vp};
}

// Fills one atlas layer with a deterministic checkerboard so both renderers fetch identical texels.
auto uploadPatternLayer(const u32 texture_id, const u32 layer) {
    constexpr auto size = 1024u;
    auto           data = std::vector<u8>(static_cast<size_t>(size) * size * 4u);
    for (auto y = 0u; y < size; ++y) {
        for (auto x = 0u; x < size; ++x) {
            const auto i       = (static_cast<size_t>(y) * size + x) * 4u;
            const auto checker = (((x / 64u) + (y / 64u)) % 2u) == 0u;
            data[i + 0]        = checker ? 255 : 40;
            data[i + 1]        = checker ? 128 : 200;
            data[i + 2]        = checker ? 0 : 255;
            data[i + 3]        = 255;
        }
    }
    glBindTexture(GL_TEXTURE_2D_ARRAY, texture_id);
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, static_cast<GLint>(layer), size, size, 1, GL_RGBA, GL_UNSIGNED_BYTE,
                    data.data());
}

// Builds a solid-colour axis-aligned quad as a VDP1 polygon part.
auto makeQuad(const s16 x0, const s16 y0, const s16 x1, const s16 y1, const u8 r, const u8 g, const u8 b,
              const u8 priority, const u8 alpha = 255) -> RenderPart {
    Vdp1Part vp{};
    vp.common_vdp_data_.draw_type = DrawType::non_textured_polygon;
    vp.common_vdp_data_.priority  = priority;

    const auto add = [&vp, r, g, b, alpha](const s16 x, const s16 y) {
        auto v         = Vertex(x, y, 0.0f, 0.0f);
        v.color        = VertexColor(r, g, b, alpha);
        v.color_offset = ColorOffset{};
        vp.common_vdp_data_.vertexes.push_back(v);
    };
    add(x0, y0);
    add(x1, y0);
    add(x1, y1);
    add(x0, y1);
    return RenderPart{vp};
}

} // namespace

namespace {
constexpr auto prim_polygon  = u32{0};
constexpr auto prim_polyline = u32{1};
constexpr auto prim_line     = u32{2};

auto primitiveKind(const DrawType type) -> u32 {
    switch (type) {
        using enum DrawType;
        case polyline: return prim_polyline;
        case line: return prim_line;
        default: return prim_polygon;
    }
}

auto isDrawable(const DrawType type) -> bool {
    switch (type) {
        using enum DrawType;
        case textured_polygon:
        case non_textured_polygon:
        case polyline:
        case line: return true;
        default: return false;
    }
}

// GL_BYTE normalized maps s8 through max(c / 127, -1).
auto normalizeGouraud(const s8 value) -> float {
    return std::clamp(static_cast<float>(value) / 127.0f, -1.0f, 1.0f);
}

auto projectToScreen(const glm::mat4& projection, const float x, const float y, const float width, const float height)
    -> std::pair<float, float> {
    auto p = projection * glm::vec4(x, y, 0.0f, 1.0f);
    p.x /= p.w;
    p.y /= p.w;
    return {(p.x * 0.5f + 0.5f) * width, (p.y * 0.5f + 0.5f) * height};
}

// Must match local_size_x / local_size_y in raster.comp: one workgroup rasterizes one tile.
constexpr auto compute_tile_size = 16;

auto alignDown(const int value) -> int {
    return (value / compute_tile_size) * compute_tile_size;
}

auto alignUp(const int value) -> int {
    return ((value + compute_tile_size - 1) / compute_tile_size) * compute_tile_size;
}
} // namespace

void OpenglComputeRender::initialize() {
    // Keep the fixed-function program loaded: it stays the reference implementation for parity tests.
    OpenglRender::initialize();

    const auto shaders = initializeShaders();
    const auto compute = createComputeShader(shaders, "raster.comp");

    compute_program_ = glCreateProgram();
    glAttachShader(compute_program_, compute);
    glLinkProgram(compute_program_);
    checkProgramCompilation(compute_program_);
    glDeleteShader(compute);

    glGenBuffers(1, &vertex_ssbo_);
    glGenBuffers(1, &part_ssbo_);
    glGenBuffers(1, &tile_index_ssbo_);
    glGenBuffers(1, &tile_offset_ssbo_);

    core::Log::info(core::Logger::opengl, "OpenGL compute renderer (raster.comp) initialized.");
}

void OpenglComputeRender::postInitialize() {
    if (std::getenv("SATURNIN_COMPUTE_SELFTEST") != nullptr) { selfTest(); }
    if (std::getenv("SATURNIN_COMPUTE_PARITYTEST") != nullptr) { parityTest(); }
}

// Dev-only: rasterizes a synthetic parts list through the real renderParts() path and reads the
// result back, so the integration can be exercised without a disc image.
void OpenglComputeRender::selfTest() {
    // No emulation was run, so nothing has set the Saturn resolution yet: the viewport matrix would be zero.
    opengl_->saturnScreenResolution(ScreenResolution{320, 224});

    const auto quad = [](const s16 x0, const s16 y0, const s16 x1, const s16 y1, const u8 r, const u8 g, const u8 b,
                         const u8 priority) {
        Vdp1Part vp{};
        vp.common_vdp_data_.draw_type = DrawType::non_textured_polygon;
        vp.common_vdp_data_.priority  = priority;
        const auto add                = [&vp, r, g, b](const s16 x, const s16 y) {
            auto v         = Vertex(x, y, 0.0f, 0.0f);
            v.color        = VertexColor(r, g, b, 255);
            v.color_offset = ColorOffset{};
            vp.common_vdp_data_.vertexes.push_back(v);
        };
        add(x0, y0);
        add(x1, y0);
        add(x1, y1);
        add(x0, y1);
        return RenderPart{vp};
    };

    PartsList parts;
    parts.push_back(quad(0, 0, 320, 224, 255, 0, 255, 0));     // magenta, whole Saturn surface
    parts.push_back(quad(100, 60, 220, 160, 0, 255, 0, 1));    // green, on top, centred

    // Clear the target layer first so "no write" is distinguishable from "wrote the background".
    const auto layer = opengl_->texturing()->getFboTextureLayer(currentRenderedBuffer());
    glBindFramebuffer(GL_FRAMEBUFFER, opengl_->texturing()->getFboId(FboType::general));
    glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, opengl_->texturing()->getFboTextureArrayId(), 0, layer);
    glClearColor(0.0f, 0.0f, 0.25f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    renderParts(parts, opengl_->texturing()->getTextureArrayId());

    glBindFramebuffer(GL_FRAMEBUFFER, opengl_->texturing()->getFboId(FboType::general));
    glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, opengl_->texturing()->getFboTextureArrayId(), 0, layer);
    glReadBuffer(GL_COLOR_ATTACHMENT0);

    auto row = std::vector<u8>(fbo_texture_array_width * 4);
    glReadPixels(0, fbo_texture_array_height / 2, fbo_texture_array_width, 1, GL_RGBA, GL_UNSIGNED_BYTE, row.data());

    auto magenta = 0, green = 0, background = 0, other = 0;
    for (auto x = 0u; x < fbo_texture_array_width; ++x) {
        const auto r = row[x * 4], g = row[x * 4 + 1], b = row[x * 4 + 2];
        if (r > 200 && g < 60 && b > 200) { ++magenta; }
        else if (r < 60 && g > 200 && b < 60) { ++green; }
        else if (r < 30 && g < 30 && b > 50) { ++background; }
        else { ++other; }
    }
    core::Log::info(core::Logger::opengl,
                    "Self-test layer {} row {}: magenta={} green={} background={} other={} gl_error={}",
                    static_cast<s32>(layer),
                    fbo_texture_array_height / 2,
                    magenta,
                    green,
                    background,
                    other,
                    static_cast<s32>(glGetError()));
}

// Dev-only: renders the same parts list through the 3.3 fixed-function path and the compute path,
// then compares both layers pixel by pixel.
void OpenglComputeRender::parityTest() {
    opengl_->saturnScreenResolution(ScreenResolution{320, 224});

    PartsList parts;
    parts.push_back(makeQuad(0, 0, 320, 224, 255, 0, 255, 0));
    parts.push_back(makeQuad(100, 60, 220, 160, 0, 255, 0, 1));
    parts.push_back(makeQuad(40, 30, 120, 90, 0, 0, 255, 2));

    const auto tex = opengl_->texturing()->getTextureArrayId();

    constexpr auto pattern_layer = 5u;
    uploadPatternLayer(tex, pattern_layer);
    parts.push_back(makeTexturedQuad(240, 150, 310, 220, static_cast<float>(pattern_layer), 3));
    // Semi-transparent white: exercises GL_SRC_ALPHA / GL_ONE_MINUS_SRC_ALPHA against earlier parts.
    parts.push_back(makeQuad(60, 40, 180, 120, 255, 255, 255, 4, 128));
    parts.push_back(makeLines(DrawType::polyline, {{11, 11}, {61, 11}, {61, 61}, {11, 61}}, 255, 255, 0, 5));
    parts.push_back(makeLines(DrawType::line, {{201, 181}, {301, 181}}, 0, 255, 255, 6));

    // Simulate renderByScreenPriority: distinct priority lists rendered sequentially into the
    // same FBO layer, with a single clear before the whole sequence.
    PartsList priority_parts;
    priority_parts.push_back(makeQuad(0, 0, 160, 224, 0, 255, 0, 7, 96));   // translucent green
    priority_parts.push_back(makeQuad(140, 0, 320, 112, 255, 255, 0, 8));   // opaque yellow

    // Reverse iteration means the higher key renders first. Put the base list at key 2 and the
    // translucent priority list at key 1 so it blends on top.
    MapOfPartsList priority_lists;
    priority_lists[{1, VdpLayer::sprite}] = priority_parts;
    priority_lists[{2, VdpLayer::sprite}] = parts;

    const auto fbo   = opengl_->texturing()->getFboId(FboType::general);
    const auto array = opengl_->texturing()->getFboTextureArrayId();
    const auto base  = opengl_->texturing()->getFboTextureLayer(currentRenderedBuffer());
    const auto l_ref = static_cast<u32>(base);
    // The compute path targets the layer of the current rendered buffer, so both passes share it and
    // are read back one after the other.
    const auto l_cmp = l_ref;

    glDisable(GL_SCISSOR_TEST);
    glViewport(0, 0, fbo_texture_array_width, fbo_texture_array_height);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    const auto bindLayer = [&](const u32 layer) {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, array, 0, layer);
    };
    const auto prepare = [&](const u32 layer) {
        bindLayer(layer);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    };

    prepare(l_ref);
    const auto readLayer = [&](const u32 layer) {
        auto pixels = std::vector<u8>(static_cast<size_t>(fbo_texture_array_width) * fbo_texture_array_height * 4u);
        bindLayer(layer);
        glReadBuffer(GL_COLOR_ATTACHMENT0);
        glReadPixels(0, 0, fbo_texture_array_width, fbo_texture_array_height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
        return pixels;
    };

    const auto ref_start = std::chrono::high_resolution_clock::now();
    OpenglRender::renderPartsLists(priority_lists, tex); // qualified: the 3.3 fixed-function implementation
    const auto ref_ms = std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - ref_start).count();
    const auto ref = readLayer(l_ref);

    prepare(l_cmp);
    const auto cmp_start = std::chrono::high_resolution_clock::now();
    renderPartsLists(priority_lists, tex); // virtual: the compute implementation (single flattened dispatch)
    const auto cmp_ms = std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - cmp_start).count();
    const auto cmp = readLayer(l_cmp);

    auto differing = 0u, max_diff = 0u, worst_x = 0u, worst_y = 0u;
    auto sum_diff  = 0ull;
    auto pattern_px = 0u;
    auto blend_px = 0u;
    auto lines_px = 0u;
    auto priority_px = 0u;
    const auto closeTo = [](const u8 a, const u8 b) { return static_cast<s32>(a) - static_cast<s32>(b) <= 2 && static_cast<s32>(b) - static_cast<s32>(a) <= 2; };
    for (auto i = 0u; i < ref.size(); i += 4) {
        // Sanity: the checkerboard texels must be visible, otherwise the textured branch is untested.
        if ((closeTo(ref[i], 255) && closeTo(ref[i + 1], 128) && closeTo(ref[i + 2], 0))
            || (closeTo(ref[i], 40) && closeTo(ref[i + 1], 200) && closeTo(ref[i + 2], 255))) {
            ++pattern_px;
        }
        // Sanity: white alpha=128 over the green quad must produce (128,255,128) somewhere.
        if (closeTo(ref[i], 128) && closeTo(ref[i + 1], 255) && closeTo(ref[i + 2], 128)) { ++blend_px; }
        // Sanity: opaque yellow loop and cyan segment must be visible.
        if ((closeTo(ref[i], 255) && closeTo(ref[i + 1], 255) && closeTo(ref[i + 2], 0))
            || (closeTo(ref[i], 0) && closeTo(ref[i + 1], 255) && closeTo(ref[i + 2], 255))) {
            ++lines_px;
        }
        // Second priority list: translucent green over magenta -> (159,96,159); opaque yellow.
        if (closeTo(ref[i], 159) && closeTo(ref[i + 1], 96) && closeTo(ref[i + 2], 159)) { ++priority_px; }
        auto d = 0u;
        for (auto c = 0u; c < 3u; ++c) {
            const auto delta = static_cast<u32>(std::abs(static_cast<s32>(ref[i + c]) - static_cast<s32>(cmp[i + c])));
            d                = delta > d ? delta : d;
        }
        if (d > 2u) { ++differing; }
        if (d > max_diff) {
            max_diff = d;
            worst_x  = (i / 4u) % fbo_texture_array_width;
            worst_y  = (i / 4u) / fbo_texture_array_width;
        }
        sum_diff += d;
    }
    core::Log::info(core::Logger::opengl,
                    "Parity ref_layer={} cmp_layer={} total_px={} differing={} max_diff={} mean_diff={} pattern_px={} blend_px={} lines_px={} priority_px={} ref_ms={:.2f} cmp_ms={:.2f} worst=({},{}) gl_error={}",
                    static_cast<s32>(l_ref),
                    static_cast<s32>(l_cmp),
                    ref.size() / 4u,
                    differing,
                    max_diff,
                    static_cast<double>(sum_diff) / static_cast<double>(ref.size() / 4u),
                    pattern_px,
                    blend_px,
                    lines_px,
                    priority_px,
                    ref_ms,
                    cmp_ms,
                    worst_x,
                    worst_y,
                    static_cast<s32>(glGetError()));
}

void OpenglComputeRender::renderParts(const PartsList& parts_list, const u32 texture_id) {
    if (parts_list.empty()) { return; }

    // 1. Flatten the parts list into GPU-friendly pools. Projection is done once per vertex here;
    //    the compute kernel only consumes framebuffer-space coordinates.
    std::vector<GpuVertex> verts;
    std::vector<GpuPart>   parts;
    verts.reserve(parts_list.size() * vertexes_per_polyline);
    parts.reserve(parts_list.size());

    const auto projection = calculateDisplayViewportMatrix();
    const auto fb_width   = static_cast<float>(fbo_texture_array_width);
    const auto fb_height  = static_cast<float>(fbo_texture_array_height);

    // Only the area the fixed-function path would touch needs rasterizing: its scissor rect.
    // Clipping is done here, per frame and after projection, rather than when the VDP2 parts are
    // generated: part positions are screen space for the current frame only, so a cell that
    // scrolls into view must still be available.
    auto clip_min_x = 0.0f;
    auto clip_min_y = 0.0f;
    auto clip_max_x = fb_width;
    auto clip_max_y = fb_height;
    if (glIsEnabled(GL_SCISSOR_TEST)) {
        if (const auto [vx, vy, vw, vh] = calculateViewportPosAndSize(); (vw > 0) && (vh > 0)) {
            clip_min_x = static_cast<float>(vx);
            clip_min_y = static_cast<float>(vy);
            clip_max_x = static_cast<float>(vx + vw);
            clip_max_y = static_cast<float>(vy + vh);
        }
    }

    // Use the same vertex preparation as the fixed-function path: it remaps texture coordinates to
    // the registered atlas subtexture and applies the part-level colour offset.
    const auto vertexes = opengl_->texturing()->readVertexes(parts_list);
    auto       index    = size_t{0};
    auto       screen   = std::vector<std::pair<float, float>>{};
    auto       min_x    = fb_width;
    auto       min_y    = fb_height;
    auto       max_x    = 0.0f;
    auto       max_y    = 0.0f;
    for (const auto& part : parts_list) {
        const auto count = part.vertexes.size();
        screen.resize(count);

        auto part_min_x = fb_width;
        auto part_min_y = fb_height;
        auto part_max_x = 0.0f;
        auto part_max_y = 0.0f;
        for (auto i = size_t{0}; i < count; ++i) {
            const auto& v = vertexes[index + i];
            screen[i] = projectToScreen(projection,
                                        static_cast<float>(v.pos.x),
                                        static_cast<float>(v.pos.y),
                                        fb_width,
                                        fb_height);
            part_min_x = std::min(part_min_x, screen[i].first);
            part_min_y = std::min(part_min_y, screen[i].second);
            part_max_x = std::max(part_max_x, screen[i].first);
            part_max_y = std::max(part_max_y, screen[i].second);
        }

        // Discard what the fixed-function path would scissor out; it cannot contribute a pixel.
        const auto bounds_min_x = std::max(part_min_x - 1.0f, clip_min_x);
        const auto bounds_min_y = std::max(part_min_y - 1.0f, clip_min_y);
        const auto bounds_max_x = std::min(part_max_x + 1.0f, clip_max_x);
        const auto bounds_max_y = std::min(part_max_y + 1.0f, clip_max_y);

        if (isDrawable(part.draw_type) && (count > 0) && (bounds_min_x <= bounds_max_x)
            && (bounds_min_y <= bounds_max_y)) {
            const auto textured = (part.draw_type == DrawType::textured_polygon) ? u32{1} : u32{0};
            parts.push_back({static_cast<u32>(verts.size()),
                             static_cast<u32>(count),
                             textured | (primitiveKind(part.draw_type) << 1u),
                             u32{0},
                             {bounds_min_x, bounds_min_y, bounds_max_x, bounds_max_y}});
            min_x = std::min(min_x, bounds_min_x);
            min_y = std::min(min_y, bounds_min_y);
            max_x = std::max(max_x, bounds_max_x);
            max_y = std::max(max_y, bounds_max_y);

            for (auto i = size_t{0}; i < count; ++i) {
                const auto& v = vertexes[index + i];
                verts.push_back(
                    {{screen[i].first, screen[i].second, 0.0f, 0.0f},
                     {v.tex_coords.s, v.tex_coords.t, v.tex_coords.p, 0.0f},
                     {v.color.r / 255.0f, v.color.g / 255.0f, v.color.b / 255.0f, v.color.a / 255.0f},
                     {normalizeGouraud(v.gouraud.r), normalizeGouraud(v.gouraud.g), normalizeGouraud(v.gouraud.b), 0.0f},
                     {static_cast<float>(v.color_offset.signs[0]),
                      static_cast<float>(v.color_offset.signs[1]),
                      static_cast<float>(v.color_offset.signs[2]),
                      0.0f},
                     {v.color_offset.values[0] / 255.0f, v.color_offset.values[1] / 255.0f, v.color_offset.values[2] / 255.0f, 0.0f}});
            }
        }
        index += count;
    }
    if (parts.empty()) { return; }

    // Dispatch only the tile-aligned rectangle that actually contains visible geometry. The padding
    // in each part bound absorbs conservative rounding at polygon edges.
    const auto fb_w     = static_cast<int>(fbo_texture_array_width);
    const auto fb_h     = static_cast<int>(fbo_texture_array_height);
    const auto origin_x = alignDown(std::clamp(static_cast<int>(std::floor(min_x)), 0, fb_w - 1));
    const auto origin_y = alignDown(std::clamp(static_cast<int>(std::floor(min_y)), 0, fb_h - 1));
    const auto end_x    = std::min(alignUp(std::clamp(static_cast<int>(std::ceil(max_x)), origin_x + 1, fb_w)), fb_w);
    const auto end_y    = std::min(alignUp(std::clamp(static_cast<int>(std::ceil(max_y)), origin_y + 1, fb_h)), fb_h);
    const auto tiles_x  = (end_x - origin_x) / compute_tile_size;
    const auto tiles_y  = (end_y - origin_y) / compute_tile_size;

    // 2. Bucket the parts per tile: one workgroup of the dispatch is exactly one tile, so a pixel
    //    only inspects the parts overlapping its own tile instead of the whole list. Parts are
    //    appended in draw order, which preserves painter's order inside every bucket.
    const auto tileSpan = [](const float lo, const float hi, const int origin, const int limit) {
        const auto first = std::clamp(static_cast<int>(std::floor(lo)), origin, limit - 1);
        const auto last  = std::clamp(static_cast<int>(std::ceil(hi)), origin, limit - 1);
        return std::pair{(first - origin) / compute_tile_size, (last - origin) / compute_tile_size};
    };

    const auto tile_count = static_cast<size_t>(tiles_x) * static_cast<size_t>(tiles_y);
    auto       tile_offsets = std::vector<u32>(tile_count + 1, 0);
    for (const auto& p : parts) {
        const auto [tx0, tx1] = tileSpan(p.bounds[0], p.bounds[2], origin_x, end_x);
        const auto [ty0, ty1] = tileSpan(p.bounds[1], p.bounds[3], origin_y, end_y);
        for (auto ty = ty0; ty <= ty1; ++ty) {
            for (auto tx = tx0; tx <= tx1; ++tx) { ++tile_offsets[static_cast<size_t>(ty) * tiles_x + tx + 1]; }
        }
    }
    for (auto t = size_t{0}; t < tile_count; ++t) { tile_offsets[t + 1] += tile_offsets[t]; }

    auto tile_indices = std::vector<u32>(tile_offsets.back());
    auto cursor       = std::vector<u32>(tile_offsets.begin(), tile_offsets.end() - 1);
    for (auto pi = size_t{0}; pi < parts.size(); ++pi) {
        const auto& p         = parts[pi];
        const auto [tx0, tx1] = tileSpan(p.bounds[0], p.bounds[2], origin_x, end_x);
        const auto [ty0, ty1] = tileSpan(p.bounds[1], p.bounds[3], origin_y, end_y);
        for (auto ty = ty0; ty <= ty1; ++ty) {
            for (auto tx = tx0; tx <= tx1; ++tx) {
                tile_indices[cursor[static_cast<size_t>(ty) * tiles_x + tx]++] = static_cast<u32>(pi);
            }
        }
    }

    // 3. Upload to the SSBOs.
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, vertex_ssbo_);
    glBufferData(GL_SHADER_STORAGE_BUFFER, static_cast<GLsizeiptr>(verts.size() * sizeof(GpuVertex)), verts.data(), GL_STREAM_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, part_ssbo_);
    glBufferData(GL_SHADER_STORAGE_BUFFER, static_cast<GLsizeiptr>(parts.size() * sizeof(GpuPart)), parts.data(), GL_STREAM_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, tile_index_ssbo_);
    glBufferData(GL_SHADER_STORAGE_BUFFER, static_cast<GLsizeiptr>(tile_indices.size() * sizeof(u32)), tile_indices.data(), GL_STREAM_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, tile_offset_ssbo_);
    glBufferData(GL_SHADER_STORAGE_BUFFER, static_cast<GLsizeiptr>(tile_offsets.size() * sizeof(u32)), tile_offsets.data(), GL_STREAM_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, vertex_ssbo_);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, part_ssbo_);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, tile_index_ssbo_);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, tile_offset_ssbo_);

    // 4. Bind the Saturn texture array (source) on unit 0.
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, texture_id);

    // 5. Bind the target layer as an image. The FBO must be unbound first: a texture must not be a
    //    framebuffer attachment and an image at the same time (feedback loop => dropped stores).
    auto previous_fbo = GLint{0};
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previous_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    const auto target_texture = opengl_->texturing()->getFboTextureArrayId();
    const auto target_layer   = opengl_->texturing()->getFboTextureLayer(currentRenderedBuffer());
    glBindImageTexture(0, target_texture, 0, GL_FALSE, target_layer, GL_READ_WRITE, GL_RGBA8);

    // Dev-only GPU timing hook: glFinish() makes the measured interval include actual execution.
    static const auto timing_enabled = std::getenv("SATURNIN_COMPUTE_TIMING") != nullptr;
    static auto       timing_frame   = u32{0};
    if (timing_enabled) { glFinish(); }
    const auto timing_start = std::chrono::steady_clock::now();

    // 6. One dispatch, one workgroup per tile.
    glUseProgram(compute_program_);
    glUniform2f(glGetUniformLocation(compute_program_, "fb_size"), fb_width, fb_height);
    glUniform2i(glGetUniformLocation(compute_program_, "dispatch_origin"), origin_x, origin_y);
    glUniform1i(glGetUniformLocation(compute_program_, "tiles_x"), tiles_x);

    glDispatchCompute(static_cast<u32>(tiles_x), static_cast<u32>(tiles_y), 1);
    glMemoryBarrier(MemoryBarrierMask::GL_ALL_BARRIER_BITS);

    if (timing_enabled) {
        glFinish();
        const auto elapsed = std::chrono::duration<double, std::milli>(
            std::chrono::steady_clock::now() - timing_start).count();
        if (++timing_frame % 60u == 1u) {
            core::Log::info(core::Logger::opengl,
                            "Compute dispatch rect=({},{}) {}x{} tiles={}x{} parts={} entries={} gpu_ms={:.2f}",
                            origin_x, origin_y, end_x - origin_x, end_y - origin_y,
                            tiles_x, tiles_y, parts.size(), tile_indices.size(), elapsed);
        }
    }

    // Restore the FBO binding for the rest of the frame.
    glBindFramebuffer(GL_FRAMEBUFFER, static_cast<GLuint>(previous_fbo));

    if (live_parity_pending_) { liveParityCompare(parts_list, texture_id, target_layer); }
}

// Dev-only: render the first real (non-synthetic) parts list through both backends and compare the
// displayed area. Triggered by SATURNIN_COMPUTE_LIVE_PARITY=1.
void OpenglComputeRender::liveParityCompare(const PartsList& parts, const u32 texture_id, const u8 compute_layer) {
    live_parity_pending_ = false;
    auto skipped = 0u;
    for (const auto& p : parts) { if (!isDrawable(p.draw_type)) { ++skipped; } }
    core::Log::info(core::Logger::opengl, "Live parity input: {} parts, {} non-drawable", parts.size(), skipped);

    const auto fbo   = opengl_->texturing()->getFboId(FboType::general);
    const auto array = opengl_->texturing()->getFboTextureArrayId();
    const auto count = static_cast<size_t>(fbo_texture_array_width) * fbo_texture_array_height * 4u;

    const auto readLayer = [&](const u8 layer) {
        auto pixels = std::vector<u8>(count);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, array, 0, layer);
        glReadBuffer(GL_COLOR_ATTACHMENT0);
        glReadPixels(0, 0, fbo_texture_array_width, fbo_texture_array_height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
        return pixels;
    };

    const auto cmp = readLayer(compute_layer);

    // The fixed-function path needs its own target layer, cleared and with the same GL state.
    const auto scratch = opengl_->texturing()->getFboTextureLayer(FboTextureType::priority);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, array, 0, scratch);
    glViewport(0, 0, fbo_texture_array_width, fbo_texture_array_height);
    glDisable(GL_SCISSOR_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    OpenglRender::renderParts(parts, texture_id);
    const auto ref = readLayer(scratch);

    // Only the visible (scissored) area matters; the compute kernel intentionally ignores scissor.
    const auto [sx, sy, sw, sh] = calculateViewportPosAndSize();
    auto differing = 0u;
    auto max_diff  = 0u;
    auto total     = 0u;
    for (auto y = sy; y < sy + sh; ++y) {
        for (auto x = sx; x < sx + sw; ++x) {
            const auto i = (static_cast<size_t>(y) * fbo_texture_array_width + x) * 4u;
            ++total;
            for (auto c = 0u; c < 3u; ++c) {
                const auto d = static_cast<u32>(ref[i + c] > cmp[i + c]
                                                    ? ref[i + c] - cmp[i + c]
                                                    : cmp[i + c] - ref[i + c]);
                if (d != 0u) { ++differing; }
                if (d > max_diff) { max_diff = d; }
            }
        }
    }
    core::Log::info(core::Logger::opengl,
                    "Live parity parts={} visible_px={} differing={} max_diff={} gl_error={}",
                    parts.size(), total, differing, max_diff, static_cast<s32>(glGetError()));
}

void OpenglComputeRender::renderPartsLists(const MapOfPartsList& lists, const u32 texture_id) {
    // All priority lists end up in the same FBO layer in reverse key order, so flattening them into
    // one parts list produces the same painter's-order result with a single dispatch.
    auto flattened = PartsList{};
    for (const auto& [key, parts_list] : lists | std::views::reverse) {
        flattened.insert(flattened.end(), parts_list.begin(), parts_list.end());
    }
    renderParts(flattened, texture_id);
}

} // namespace saturnin::video
