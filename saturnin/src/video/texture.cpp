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

using ReadOnlyLock  = std::shared_lock<SharedMutex>;
using UpdatableLock = std::unique_lock<SharedMutex>;

std::unordered_map<size_t, Texture> Texture::texture_storage_;
SharedMutex                         Texture::storage_mutex_;

Texture::Texture(const VdpType    vp,
                 const u32        address,
                 const u8         color_count,
                 const u16        palette_number,
                 std::vector<u8>& texture,
                 const u16        width,
                 const u16        height) :
    vdp_type_(vp),
    width_(width), height_(height), raw_data_(std::move(texture)) {
    key_ = calculateKey(vp, address, color_count, palette_number);
};

Texture::~Texture() {
    std::vector<u8>().swap(raw_data_); // Allocated texture data is deleted.
}

// static //
auto Texture::storeTexture(Texture t) -> size_t {
    {
        //        std::lock_guard lock(storage_mutex_);
        UpdatableLock lock(storage_mutex_);
        texture_storage_.erase(t.key());
        texture_storage_.emplace(t.key(), std::move(t));
    }
    return t.key();
}

// static //
void Texture::storeTextures(std::vector<Texture>& textures) {
    {
        UpdatableLock lock(storage_mutex_);
        for (auto& t : textures) {
            texture_storage_.erase(t.key());
            texture_storage_.emplace(t.key(), std::move(t));
        }
    }
}

// static //
auto Texture::getTexture(const size_t key) -> std::optional<Texture*> {
    {
        ReadOnlyLock lock(storage_mutex_);
        try {
            auto& t = texture_storage_.at(key);
            return &t;
        } catch (const std::out_of_range& oor) {
            Log::error(Logger::texture, tr("Texture with key {:#x} wasn't found ({})"), key, oor.what());
            return std::nullopt;
        }
        // const auto   it = texture_storage_.find(key);
        // if (it != texture_storage_.end()) { return it->second; }
    }
    // Log::error(Logger::texture, tr("Texture with key {:#x} wasn't found ..."), key);
    // return std::nullopt;
}

// static //
void Texture::deleteTextureData(Texture& t) {
    std::vector<u8>().swap(t.raw_data_); // Allocated texture data is deleted.
}

// static //
auto Texture::isTextureLoadingNeeded(const size_t key) -> bool {
    {
        ReadOnlyLock lock(storage_mutex_);
        if (texture_storage_.count(key) == 0) { return true; }
    }

    auto t = Texture::getTexture(key);
    if (t) {
        if ((*t)->isDiscarded()) {
            (*t)->isDiscarded(false);
            return true;
        }
        (*t)->isRecentlyUsed(true);
        return false;
    }

    // if (t) {
    //     if ((*t).isDiscarded()) {
    //         (*t).isDiscarded(false);
    //         storeTexture(*t);
    //         return true;
    //     }
    //     (*t).isRecentlyUsed(true);
    //     storeTexture(*t);
    //     return false;
    // }

    return true;
}

// static
auto Texture::calculateKey(const VdpType vp, const u32 address, const u8 color_count, const u16 palette_number) -> size_t {
    auto key = size_t{0};
    util::hashCombine(key, vp, address, color_count, palette_number);
    return key;
}

// static
void Texture::discardCache(Opengl* ogl, const VdpType t) {
    //    std::lock_guard lock(storage_mutex_);
    UpdatableLock lock(storage_mutex_);
    for (auto& [key, value] : texture_storage_) {
        auto discard_elt = (t == VdpType::not_set) ? true : ((value.vdpType() == t) ? true : false);
        if (discard_elt) { value.isDiscarded(true); }
    }
}

// static
void Texture::setCache(const VdpType t) {
    //    std::lock_guard lock(storage_mutex_);
    UpdatableLock lock(storage_mutex_);
    for (auto& [key, value] : texture_storage_) {
        auto set_elt = (t == VdpType::not_set) ? true : ((value.vdpType() == t) ? true : false);
        if (set_elt) { value.isRecentlyUsed(false); }
    }
}

// static
void Texture::cleanCache(Opengl* ogl, const VdpType t) {
    //    std::lock_guard lock(storage_mutex_);
    UpdatableLock                                     lock(storage_mutex_);
    std::vector<decltype(texture_storage_)::key_type> keys_to_erase;
    for (auto& [key, value] : texture_storage_) {
        auto is_elt_selected = (t == VdpType::not_set) ? true : ((value.vdpType() == t) ? true : false);
        if (is_elt_selected) {
            if (!(value.isRecentlyUsed())) {
                keys_to_erase.emplace_back(key);
            } else {
                if (value.isDiscarded()) {
                    // WIP
                    ogl->addOrUpdateTexture(value.key());
                    keys_to_erase.emplace_back(key);
                }
            }
        }
    }

    for (auto&& key : keys_to_erase) {
        texture_storage_.erase(key);
        ogl->removeTextureLink(key);
    }
}

// static
void Texture::deleteCache() {
    //    std::lock_guard lock(storage_mutex_);
    UpdatableLock lock(storage_mutex_);
    for (auto& [key, value] : texture_storage_) {
        deleteTextureData(value);
    }
    texture_storage_.clear();
}

// static
auto Texture::detailedList() -> std::vector<DebugKey> {
    auto         list = std::vector<DebugKey>{};
    const auto   mask = std::string("{}x{} | {:x}");
    ReadOnlyLock lock(storage_mutex_);
    for (auto& [key, value] : texture_storage_) {
        list.emplace_back(fmt::format(mask, value.width_, value.height_, value.key_), value.key_);
    }
    return list;
}

// static
auto Texture::calculateTextureSize(const ImageSize& max_size, const size_t texture_key) -> ImageSize {
    auto       ratio   = 0.0;
    const auto texture = getTexture(texture_key);
    if ((*texture)->width() > (*texture)->height()) {
        if ((*texture)->width() > max_size.width) {
            ratio = (*texture)->width() / max_size.width;
        } else {
            ratio = max_size.width / (*texture)->width();
        }
    } else {
        if ((*texture)->width() > max_size.height) {
            ratio = (*texture)->height() / max_size.height;
        } else {
            ratio = max_size.height / (*texture)->height();
        }
    }
    const auto width  = (*texture)->width() % max_size.width * ratio;
    const auto height = (*texture)->height() % max_size.height * ratio;

    return ImageSize{static_cast<u16>(width), static_cast<u16>(height)};
    // return ImageSize{};
}

// static
auto Texture::statistics() -> std::vector<std::string> {
    ReadOnlyLock lock(storage_mutex_);
    auto         stats = std::vector<std::string>{};
    stats.push_back(fmt::format("Total number of textures : {}", texture_storage_.size()));

    const auto vdp1_nb = std::count_if(texture_storage_.begin(), texture_storage_.end(), [](const auto& t) {
        return t.second.vdpType() == VdpType::vdp1;
    });
    stats.push_back(fmt::format("Number of VDP1 textures : {}", vdp1_nb));

    const auto vdp2_nb = std::count_if(texture_storage_.begin(), texture_storage_.end(), [](const auto& t) {
        return t.second.vdpType() == VdpType::vdp2;
    });
    stats.push_back(fmt::format("Number of VDP2 textures : {}", vdp2_nb));

    auto max_width  = 0;
    auto max_height = 0;
    for (const auto& t : texture_storage_) {
        if (t.second.vdpType() == VdpType::vdp1) {
            if (t.second.height() > max_height) { max_height = t.second.height(); }
            if (t.second.width() > max_width) { max_width = t.second.width(); }
        }
    }
    stats.push_back(fmt::format("Maximum VDP1 texture size : {}x{}", max_height, max_width));

    return stats;
}
} // namespace saturnin::video
