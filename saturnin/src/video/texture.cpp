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
#include <saturnin/src/utilities.h> // hashCombine

namespace util = saturnin::utilities;

namespace saturnin::video {

using core::Log;
using core::tr;

std::map<size_t, Texture> Texture::texture_storage_;

Texture::Texture(const VdpType    vp,
                 const u32        address,
                 const ColorCount color_count,
                 std::vector<u8>& texture,
                 const u16        width,
                 const u16        height) :
    width_(width),
    height_(height) {
    calculateKey(vp, address, color_count);
    data_ = std::move(texture);
};

// static //
auto Texture::storeTexture(const Texture&& t) -> size_t {
    texture_storage_.erase(t.getKey());
    texture_storage_.emplace(t.getKey(), t);
    return t.getKey();
}

// static //
auto Texture::getTexture(const size_t key) -> Texture& {
    const auto it = texture_storage_.find(key);
    if (it != texture_storage_.end()) { return it->second; }
    Log::error("texture", tr("Texture with key {:#x} wasn't found ..."), key);
    throw std::runtime_error("Texture error !");
}

// static //
auto Texture::doesTextureExist(const Texture& t) -> bool {
    if (texture_storage_.find(t.getKey()) != texture_storage_.end()) { return true; }
    return false;
}

void Texture::calculateKey(const VdpType vp, const u32 address, const ColorCount color_count) {
    key_ = 0;
    util::hashCombine(key_, vp, address, color_count);
}
} // namespace saturnin::video
