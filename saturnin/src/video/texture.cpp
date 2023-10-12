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
#include <saturnin/src/utilities.h> // hashCombine, format

namespace uti = saturnin::utilities;

namespace saturnin::video {

using core::Log;
using core::Logger;
using core::tr;

std::unordered_map<size_t, Texture> Texture::texture_storage_;
SharedMutex                         Texture::storage_mutex_;

Texture::Texture(const VdpType    vp,
                 const Layer      layer,
                 const u32        address,
                 const u8         color_count,
                 const u16        palette_number,
                 std::vector<u8>& texture,
                 const u16        width,
                 const u16        height) :
    vdp_type_(vp),
    layer_(layer),
    width_(width),
    height_(height),
    raw_data_(std::move(texture)) {
    key_ = calculateKey(vp, address, color_count, palette_number);
};

void Texture::shutdown() {
    std::vector<u8>().swap(raw_data_); // Allocated texture data is deleted.
}

auto Texture::storeTexture(Texture t) -> size_t {
    UpdatableLock lock(storage_mutex_);
    texture_storage_.erase(t.key());
    texture_storage_.try_emplace(t.key(), std::move(t));

    return t.key();
}

void Texture::storeTextures(std::vector<Texture>& textures) {
    UpdatableLock lock(storage_mutex_);
    for (auto& t : textures) {
        texture_storage_.erase(t.key());
        texture_storage_.try_emplace(t.key(), std::move(t));
    }
}

auto Texture::getTexture(const size_t key) -> std::optional<Texture*> {
    ReadOnlyLock lock(storage_mutex_);
    try {
        auto& t = texture_storage_.at(key);
        return &t;
    } catch (const std::out_of_range& oor) {
        Log::error(Logger::texture, tr("Texture with key {:#x} wasn't found ({})"), key, oor.what());
        return std::nullopt;
    }
}

void Texture::deleteTextureData(Texture& t) {
    std::vector<u8>().swap(t.raw_data_); // Allocated texture data is deleted.
}

auto Texture::isTextureLoadingNeeded(const size_t key) -> bool {
    if (!isTextureKeyStored(key)) { return true; }

    if (auto t = Texture::getTexture(key); t) {
        if ((*t)->isDiscarded()) {
            UpdatableLock lock(storage_mutex_);
            (*t)->isDiscarded(false);
            return true;
        }
        UpdatableLock lock(storage_mutex_);
        (*t)->isRecentlyUsed(true);
        return false;
    }

    return true;
}

auto Texture::isTextureKeyStored(const size_t key) -> bool {
    ReadOnlyLock lock(storage_mutex_);
    return texture_storage_.contains(key);
}

auto Texture::calculateKey(const VdpType vp, const u32 address, const u8 color_count, const u16 palette_number) -> size_t {
    auto key = size_t{0};
    uti::hashCombine(key, vp, address, color_count, palette_number);
    return key;
}

void Texture::discardCache(const VdpType t) {
    UpdatableLock lock(storage_mutex_);
    for (auto& [key, value] : texture_storage_) {
        const auto is_found    = ((value.vdpType() == t) ? true : false);
        const auto discard_elt = (t == VdpType::not_set) ? true : is_found;
        if (discard_elt) { value.isDiscarded(true); }
    }
}

void Texture::setCache(const VdpType t) {
    UpdatableLock lock(storage_mutex_);
    for (auto& [key, value] : texture_storage_) {
        const auto is_found = ((value.vdpType() == t) ? true : false);
        auto       set_elt  = (t == VdpType::not_set) ? true : is_found;
        if (set_elt) { value.isRecentlyUsed(false); }
    }
}

void Texture::cleanCache(Opengl* ogl, const VdpType t) {
    UpdatableLock                                     lock(storage_mutex_);
    std::vector<decltype(texture_storage_)::key_type> keys_to_erase;
    for (const auto& [key, value] : texture_storage_) {
        const auto is_found        = ((value.vdpType() == t) ? true : false);
        const auto is_elt_selected = (t == VdpType::not_set) ? true : is_found;
        if (is_elt_selected) {
            if (!(value.isRecentlyUsed())) {
                keys_to_erase.emplace_back(key);
                return;
            }

            if (value.isDiscarded()) {
                // WIP
                ogl->addOrUpdateTexture(value.key(), value.layer());
                keys_to_erase.emplace_back(key);
            }
        }
    }

    for (const auto& key : keys_to_erase) {
        texture_storage_.erase(key);
        ogl->removeTextureLink(key);
    }
}

void Texture::deleteCache() {
    UpdatableLock lock(storage_mutex_);
    for (auto& [key, value] : texture_storage_) {
        deleteTextureData(value);
    }
    texture_storage_.clear();
}

auto Texture::keysList() -> std::vector<DebugKey> {
    auto         list = std::vector<DebugKey>{};
    const auto   mask = std::string("{:x}");
    ReadOnlyLock lock(storage_mutex_);
    for (auto& [key, value] : texture_storage_) {
        list.emplace_back(uti::format(mask, value.key_), value.key_);
    }
    return list;
}

auto Texture::calculateTextureSize(const Size& max_size, const size_t texture_key) -> Size {
    auto       ratio   = 0.0;
    const auto texture = getTexture(texture_key);
    if ((*texture)->width() > (*texture)->height()) {
        if ((*texture)->width() > max_size.w) {
            ratio = (*texture)->width() / max_size.w;
        } else {
            ratio = max_size.w / (*texture)->width();
        }
    } else {
        if ((*texture)->width() > max_size.h) {
            ratio = (*texture)->height() / max_size.h;
        } else {
            ratio = max_size.h / (*texture)->height();
        }
    }
    const auto width  = (*texture)->width() % max_size.w * ratio;
    const auto height = (*texture)->height() % max_size.h * ratio;

    return Size{static_cast<u16>(width), static_cast<u16>(height)};
}

auto Texture::statistics() -> std::vector<std::string> {
    ReadOnlyLock lock(storage_mutex_);
    auto         stats = std::vector<std::string>{};
    stats.push_back(uti::format("Total number of textures : {}", texture_storage_.size()));

    const auto vdp1_nb
        = std::ranges::count_if(texture_storage_, [](const auto& t) { return t.second.vdpType() == VdpType::vdp1; });
    stats.push_back(uti::format("Number of VDP1 textures : {}", vdp1_nb));

    const auto vdp2_nb
        = std::ranges::count_if(texture_storage_, [](const auto& t) { return t.second.vdpType() == VdpType::vdp2; });
    stats.push_back(uti::format("Number of VDP2 textures : {}", vdp2_nb));

    auto max_width  = 0;
    auto max_height = 0;
    for (const auto& [key, value] : texture_storage_) {
        if (value.vdpType() == VdpType::vdp1) {
            if (value.height() > max_height) { max_height = value.height(); }
            if (value.width() > max_width) { max_width = value.width(); }
        }
    }
    stats.push_back(uti::format("Maximum VDP1 texture size : {}x{}", max_height, max_width));

    return stats;
}
} // namespace saturnin::video
