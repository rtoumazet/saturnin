//
// vdp2_cache.cpp
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

#include <saturnin/src/pch.h>
#include <saturnin/src/video/vdp2/vdp2.h>
#include <saturnin/src/video/texture.h>
#include <saturnin/src/utilities.h> // toUnderlying

namespace saturnin::video {

namespace util = saturnin::utilities;
using util::toUnderlying;

//--------------------------------------------------------------------------------------------------------------
// CACHE methods
//--------------------------------------------------------------------------------------------------------------

void Vdp2::resetCacheState() const {
    modules_.memory()->was_vdp2_cram_accessed_ = false;
    for (auto& accessed : modules_.memory()->was_vdp2_page_accessed_) {
        accessed = false;
    }
    for (auto& accessed : modules_.memory()->was_vdp2_bitmap_accessed_) {
        accessed = false;
    }
}

auto Vdp2::isCacheDirty(const ScrollScreen screen) -> bool {
    if (modules_.memory()->was_vdp2_cram_accessed_) { return true; }

    const auto& bg       = bg_[toUnderlying(screen)];
    const auto& saved_bg = saved_bg_[toUnderlying(screen)];

    if (bg.is_display_enabled != saved_bg.is_display_enabled) { return true; }
    if (bg.is_transparency_code_valid != saved_bg.is_transparency_code_valid) { return true; }
    if (bg.character_color_number != saved_bg.character_color_number) { return true; }
    if (bg.bitmap_size != saved_bg.bitmap_size) { return true; }
    if (bg.format != saved_bg.format) { return true; }
    if (bg.character_pattern_size != saved_bg.character_pattern_size) { return true; }
    if (bg.pattern_name_data_size != saved_bg.pattern_name_data_size) { return true; }
    if (bg.plane_size != saved_bg.plane_size) { return true; }
    if (bg.map_offset != saved_bg.map_offset) { return true; }
    if (bg.bitmap_palette_number != saved_bg.bitmap_palette_number) { return true; }

    if (bg.format == ScrollScreenFormat::cell) {
        //  Checking the pages
        const auto page_address_start
            = (getScreen(screen).plane_a_start_address & core::vdp2_vram_memory_mask) >> core::vdp2_page_disp;
        const auto page_address_end
            = ((getScreen(screen).plane_a_start_address + getScreen(screen).page_size) & core::vdp2_vram_memory_mask)
              >> core::vdp2_page_disp;
        for (u32 i = page_address_start; i < page_address_end; ++i) {
            if (modules_.memory()->was_vdp2_page_accessed_[i]) { return true; }
        }
    } else {
        const auto bitmap_index
            = (getScreen(screen).bitmap_start_address & core::vdp2_vram_memory_mask) >> core::vdp2_bitmap_disp;

        if (modules_.memory()->was_vdp2_bitmap_accessed_[bitmap_index]) { return true; }
    }
    return false;
}

void Vdp2::discardCache([[maybe_unused]] const ScrollScreen screen) const {
    // 1) Textures used by the vdp2 parts of the screen are discarded
    Texture::discardCache(VdpType::vdp2_cell);
    Texture::discardCache(VdpType::vdp2_bitmap);

    // 2) Vdp2 parts are deleted
    // clearRenderData(screen);
}

} // namespace saturnin::video
