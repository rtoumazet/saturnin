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
    width_(width), height_(height) {
    key_ = calculateKey(vp, address, color_count, palette_number);
    // if (key_ == 0xf6d6ea85f4cfd613) DebugBreak();
    // if (key_ == 0xa57b381a6e5b28d0) DebugBreak();
    raw_data_ = std::move(texture);
};

// static //
auto Texture::storeTexture(Texture&& t) -> size_t {
    {
        // std::lock_guard<std::mutex> lock(storage_mutex_);
        deleteTextureData(t);
        texture_storage_.erase(t.key());
        texture_storage_.emplace(t.key(), t);
    }
    return t.key();
}

// static //
auto Texture::getTexture(const size_t key) -> Texture {
    // std::lock_guard<std::mutex> lock(storage_mutex_);
    const auto it = texture_storage_.find(key);
    if (it != texture_storage_.end()) { return it->second; }
    Log::error(Logger::texture, tr("Texture with key {:#x} wasn't found ..."), key);
    throw std::runtime_error("Texture error !");
}

// static //
void Texture::deleteTextureData(Texture& t) {
    std::vector<u8>().swap(t.raw_data_); // Allocated texture data is deleted.
}

// static //
auto Texture::isTextureStored(const size_t key) -> bool { return (texture_storage_.count(key) > 0); }

// static //
auto Texture::isTextureLoadingNeeded(const size_t key) -> bool {
    if (!Texture::isTextureStored(key)) {
        return true;
    } else {
        auto& t = Texture::getTexture(key);
        if (t.isDiscarded()) {
            t.isDiscarded(false);
            return true;
        }
        t.isRecentlyUsed(true);
    }
    return false;
}

// static
auto Texture::calculateKey(const VdpType vp, const u32 address, const u8 color_count, const u16 palette_number) -> size_t {
    auto key = size_t{0};
    util::hashCombine(key, vp, address, color_count, palette_number);
    return key;
}

// static
// auto Texture::clearUnusedTextures() -> std::vector<u32> {
//    std::vector<u32> api_handles;
//    // for (auto& [key, value] : texture_storage_) {
//    //    if (value.is_discarded_) {
//    //        std::vector<u8>().swap(value.raw_data_); // Clears vector data and reallocates.
//    //        if (value.delete_on_gpu_) { api_handles.push_back(value.api_handle_); }
//    //    }
//    //}
//    for (auto& [key, value] : texture_storage_) {
//        std::vector<u8>().swap(value.raw_data_); // Clears vector data and reallocates.
//        if (value.delete_on_gpu_) { api_handles.push_back(value.api_handle_); }
//    }
//    texture_storage_.clear();
//
//    return api_handles;
//}

// static
void Texture::discardCache(const VdpType t) {
    std::lock_guard<std::mutex> lock(storage_mutex_);
    for (auto& [key, value] : texture_storage_) {
        auto discard_elt = (t == VdpType::not_set) ? true : ((value.vdpType() == t) ? true : false);
        if (discard_elt) {
            value.isDiscarded(true);
            // value.deleteOnGpu(true);
        }
    }
}

// static
void Texture::setCache(const VdpType t) {
    std::lock_guard<std::mutex> lock(storage_mutex_);
    for (auto& [key, value] : texture_storage_) {
        auto set_elt = (t == VdpType::not_set) ? true : ((value.vdpType() == t) ? true : false);
        if (set_elt) { value.isRecentlyUsed(false); }
    }
}

// static
void Texture::cleanCache(const VdpType t) {
    std::lock_guard<std::mutex> lock(storage_mutex_);
    for (auto iter = texture_storage_.begin(); iter != texture_storage_.end();) {
        auto is_elt_selected = (t == VdpType::not_set) ? true : ((iter->second.vdpType() == t) ? true : false);
        if (is_elt_selected) {
            if (!(iter->second.isRecentlyUsed())) {
                deleteTextureData(iter->second);
                // iter = texture_storage_.erase(iter);
            } else {
                //++iter;
            }
        } else {
            //++iter;
        }
        ++iter;
    }
}

// static
void Texture::deleteCache() {
    std::lock_guard<std::mutex> lock(storage_mutex_);
    for (auto& [key, value] : texture_storage_) {
        // std::vector<u8>().swap(value.raw_data_);
        deleteTextureData(value);
    }
    texture_storage_.clear();
}

} // namespace saturnin::video
