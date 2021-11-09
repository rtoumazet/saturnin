//
// texture.h
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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file	texture.h
///
/// \brief	Texture definition. Will be used by VDP1 and VDP2 parts.
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <vector> // vector
#include <saturnin/src/emulator_defs.h>
#include <saturnin/src/video/base_rendering_part.h> // VdpType
#include <saturnin/src/video/vdp2.h>                // ColorCount

namespace saturnin::video {

class Opengl;

enum class StorageType { current, previous };

class Texture {
  public:
    ///@{
    /// Constructors / Destructors
    Texture() = delete;
    Texture(const VdpType    vp,
            const u32        address,
            const u8         color_count,
            const u16        palette_number,
            std::vector<u8>& texture,
            const u16        width,
            const u16        height);
    Texture(const Texture&) = default;
    Texture(Texture&&)      = default;
    auto operator=(const Texture&) & -> Texture& = default;
    auto operator=(Texture&&) & -> Texture& = default;
    ~Texture();
    ///@}

    ///@{
    /// Accessors / Mutators
    auto key() const { return key_; }
    auto apiHandle() const { return api_handle_; }
    void apiHandle(const u32 h) { api_handle_ = h; }
    auto width() const { return width_; }
    auto height() const { return height_; }
    auto rawData() const -> const std::vector<u8>& { return raw_data_; }
    auto isDiscarded() const { return is_discarded_; }
    void isDiscarded(const bool discarded) { is_discarded_ = discarded; }
    auto isRecentlyUsed() const { return is_recently_used_; }
    void isRecentlyUsed(const bool used) { is_recently_used_ = used; }
    auto vdpType() const { return vdp_type_; }
    ///@}

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static auto Texture::storeTexture(Texture&& t) -> size_t;
    ///
    /// \brief  Stores a texture, replacing the existing one if any.
    ///
    /// \author Runik
    /// \date   29/03/2021
    ///
    /// \param [in,out] t   A Texture to store.
    ///
    /// \returns    The key of the texture.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static auto storeTexture(Texture&& t) -> size_t;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static auto Texture::getTexture(const size_t key) -> Texture;
    ///
    /// \brief  Gets a texture
    ///
    /// \author Runik
    /// \date   04/04/2021
    ///
    /// \param  key Key of the texture.
    ///
    /// \returns    The texture.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static auto getTexture(const size_t key) -> Texture;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static void Texture::deleteTextureData(Texture& t);
    ///
    /// \brief  Deletes raw data of the texture
    ///
    /// \author Runik
    /// \date   27/10/2021
    ///
    /// \param [in,out] t   The texture.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static void deleteTextureData(Texture& t);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static auto Texture::isTextureStored( const size_t key) -> bool;
    ///
    /// \brief  Checks if the texture exists.
    ///
    /// \author Runik
    /// \date   29/03/2021
    ///
    /// \param  key Key of the texture to check.
    ///
    /// \returns    True if the texture is already stored.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static auto isTextureStored(const size_t key) -> bool;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static auto Texture::isTextureLoadingNeeded(const size_t key) -> bool;
    ///
    /// \brief  Checks if the texture linked to the key needs to be loaded / reloaded. Not stored ->
    ///         texture will be loaded. Stored but discarded -> texture will be reloaded. Stored and
    ///         not discarded -> texture will be reused.
    ///
    /// \author Runik
    /// \date   25/08/2021
    ///
    /// \param  key Key of the texture.
    ///
    /// \returns    True if texture needs to be loaded.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static auto isTextureLoadingNeeded(const size_t key) -> bool;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static auto Texture::calculateKey(const VdpType vp, const u32 address, const u8 color_count, const u16 palette_number
    /// = 0) -> size_t;
    ///
    /// \brief  Calculates the texture key that will be used in the storage.
    ///
    /// \author Runik
    /// \date   29/03/2021
    ///
    /// \param  vp              The type of VDP texture.
    /// \param  address         The address.
    /// \param  color_count     Number of colors.
    /// \param  palette_number  (Optional) Palette number.
    ///
    /// \returns    The calculated key.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static auto calculateKey(const VdpType vp, const u32 address, const u8 color_count, const u16 palette_number = 0) -> size_t;

    /*static auto deleteTexturesOnGpu() {
        for (auto& t : texture_storage_) {
            t.second.delete_on_gpu_ = true;
        }
    }*/

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static auto clearUnusedTextures() -> std::vector<size_t>;
    ///
    /// \brief  Clears the unused textures
    ///
    /// \author Runik
    /// \date   18/08/2021
    ///
    /// \returns    A vector containing the handles of the textures on the GPU;
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    // static auto clearUnusedTextures() -> std::vector<u32>;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static void discardCache(const VdpType t = VdpType::not_set);
    ///
    /// \brief  Marks textures in the pool as discarded.
    ///
    /// \author Runik
    /// \date   30/07/2021
    ///
    /// \param  t   (Optional) A VdpType textures to discard.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static void discardCache(Opengl* ogl, const VdpType t = VdpType::not_set);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static void setCache(const VdpType t = VdpType::not_set);
    ///
    /// \brief  Sets every texture in the cache as unused.
    ///
    /// \author Runik
    /// \date   20/08/2021
    ///
    /// \param  t   (Optional) A VdpType to process.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static void setCache(const VdpType t = VdpType::not_set);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static void cleanCache(const VdpType t = VdpType::not_set);
    ///
    /// \brief  Removes unused textures from the cache.
    ///
    /// \author Runik
    /// \date   20/08/2021
    ///
    /// \param  t   (Optional) A VdpType to process.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static void cleanCache(const VdpType t = VdpType::not_set);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static void deleteCache();
    ///
    /// \brief  Deletes the cache content.
    ///
    /// \author Runik
    /// \date   20/08/2021
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static void deleteCache();

  private:
    static std::unordered_map<size_t, Texture> texture_storage_; ///< The current texture storage.
    static std::mutex                          storage_mutex_;   ///< Used for multithreading access to the texture pool.

    VdpType vdp_type_{VdpType::not_set}; ///< What kind of VDP type is linked to this texture.
    u16     width_{};                    ///< The texture width.
    u16     height_{};                   ///< The texture height.
    bool    is_discarded_{false};        ///< True if the texture is discarded.
    bool    is_recently_used_{true};     ///< True if the texture was used during the current frame.
                                         //    bool    delete_on_gpu_{false};       ///< True to delete the texture on the GPU.
    size_t key_{};                       ///< The key of the part.
    u32    api_handle_{};                ///< Handle to the graphics API.

    std::vector<u8> raw_data_{}; ///< Raw texture data.
};

} // namespace saturnin::video
