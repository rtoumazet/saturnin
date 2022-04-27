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

std::unordered_map<size_t, Texture> Texture::texture_storage_;
std::mutex                          Texture::storage_mutex_;

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
        texture_storage_.erase(t.key());
        texture_storage_.emplace(t.key(), std::move(t));
    }
    return t.key();
}

// static //
auto Texture::getTexture(const size_t key) -> std::optional<Texture> {
    {
        //        std::lock_guard lock(storage_mutex_);
        const auto it = texture_storage_.find(key);
        if (it != texture_storage_.end()) { return it->second; }
    }
    Log::error(Logger::texture, tr("Texture with key {:#x} wasn't found ..."), key);
    // throw std::runtime_error("Texture error !");
    return std::nullopt;
}

// static //
void Texture::deleteTextureData(Texture& t) {
    std::vector<u8>().swap(t.raw_data_); // Allocated texture data is deleted.
}

// static //
auto Texture::isTextureLoadingNeeded(const size_t key) -> bool {
    if (texture_storage_.count(key) == 0) { return true; }

    auto t = Texture::getTexture(key);
    if (t) {
        if ((*t).isDiscarded()) {
            (*t).isDiscarded(false);
            storeTexture(*t);
            return true;
        }
        (*t).isRecentlyUsed(true);
        storeTexture(*t);
        return false;
    }
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
    for (auto& [key, value] : texture_storage_) {
        auto discard_elt = (t == VdpType::not_set) ? true : ((value.vdpType() == t) ? true : false);
        if (discard_elt) { value.isDiscarded(true); }
    }
}

// static
void Texture::setCache(const VdpType t) {
    //    std::lock_guard lock(storage_mutex_);
    for (auto& [key, value] : texture_storage_) {
        auto set_elt = (t == VdpType::not_set) ? true : ((value.vdpType() == t) ? true : false);
        if (set_elt) { value.isRecentlyUsed(false); }
    }
}

// static
void Texture::cleanCache(Opengl* ogl, const VdpType t) {
    //    std::lock_guard lock(storage_mutex_);
    for (auto& [key, value] : texture_storage_) {
        auto is_elt_selected = (t == VdpType::not_set) ? true : ((value.vdpType() == t) ? true : false);
        if (is_elt_selected) {
            if (!(value.isRecentlyUsed())) {
                if (value.isDiscarded()) {
                    // WIP
                    ogl->addOrUpdateTexture(value.key());
                }
            }
        }
    }
}

// static
void Texture::deleteCache() {
    //    std::lock_guard lock(storage_mutex_);
    for (auto& [key, value] : texture_storage_) {
        // std::vector<u8>().swap(value.raw_data_);
        deleteTextureData(value);
    }
    texture_storage_.clear();
}

// static
auto Texture::detailedList() -> std::vector<DebugKey> {
    auto       list = std::vector<DebugKey>{};
    const auto mask = std::string("{}x{} key: {}");
    for (auto& [key, value] : texture_storage_) {
        list.emplace_back(fmt::format(mask, value.width_, value.height_, value.key_), value.key_);
    }
    return list;
}

} // namespace saturnin::video
