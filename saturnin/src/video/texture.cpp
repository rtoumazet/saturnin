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

std::map<size_t, Texture> Texture::texture_storage_;

void Texture::storeTexture(const Texture&& t) {
    texture_storage_.erase(t.getKey());
    texture_storage_.emplace(t.getKey(), t);
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
