//
// texture.cpp
// Saturnin
//
// Copyright (c) 2021 Renaud Toumazet
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
#include <saturnin/src/video/texture.h>
#include <saturnin/src/video/opengl.h>
#include <saturnin/src/utilities.h> // hashCombine

namespace util = saturnin::utilities;

namespace saturnin::video {

using core::Log;
using core::Logger;
using core::tr;

std::map<size_t, Texture> Texture::texture_storage_;

Texture::Texture(const VdpType    vp,
                 const u32        address,
                 const u8         color_count,
                 const u16        palette_number,
                 std::vector<u8>& texture,
                 const u16        width,
                 const u16        height) :
    vdp_type_(vp),
    width_(width), height_(height) {
    key_ = calculateKey(vp, address, color_count, palette_number);
    // if (key_ == 0xf6d6ea85f4cfd613) DebugBreak();
    // if (key_ == 0xa57b381a6e5b28d0) DebugBreak();
    raw_data_ = std::move(texture);
};

// static //
auto Texture::storeTexture(Texture&& t) -> size_t {
    texture_storage_.erase(t.key());
    texture_storage_.emplace(t.key(), t);
    return t.key();
}

// static //
auto Texture::getTexture(const size_t key) -> Texture& {
    const auto it = texture_storage_.find(key);
    if (it != texture_storage_.end()) { return it->second; }
    Log::error(Logger::texture, tr("Texture with key {:#x} wasn't found ..."), key);
    throw std::runtime_error("Texture error !");
}

// static //
auto Texture::isTextureStored(const size_t key) -> bool {
    if (texture_storage_.find(key) != texture_storage_.end()) { return true; }
    return false;
}

// static
auto Texture::calculateKey(const VdpType vp, const u32 address, const u8 color_count, const u16 palette_number) -> size_t {
    auto key = size_t{0};
    util::hashCombine(key, vp, address, color_count, palette_number);
    return key;
}

// static
void Texture::discardTextures(const VdpType t) {
    for (auto& [key, value] : texture_storage_) {
        switch (t) {
            case VdpType::not_set: {
                value.isDiscarded(true);
                value.deleteOnGpu(true);
                break;
            }
            case VdpType::vdp1:
            case VdpType::vdp2: {
                if (value.vdpType() == t) {
                    value.isDiscarded(true);
                    value.deleteOnGpu(true);
                }
                break;
            }
        }
    }
}

} // namespace saturnin::video
