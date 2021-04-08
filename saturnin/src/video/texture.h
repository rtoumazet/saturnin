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

class Texture {
  public:
    ///@{
    /// Constructors / Destructors
    Texture() = delete;
    Texture(const VdpType    vp,
            const u32        address,
            const ColorCount color_count,
            std::vector<u8>& texture,
            const u16        width,
            const u16        height);
    Texture(const Texture&) = default;
    Texture(Texture&&)      = default;
    auto operator=(const Texture&) & -> Texture& = default;
    auto operator=(Texture&&) & -> Texture& = default;
    ~Texture()                              = default;
    ///@}

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto Texture::getKey() const -> const size_t
    ///
    /// \brief  Gets the key of the texture.
    ///
    /// \author Runik
    /// \date   29/03/2021
    ///
    /// \returns    The key.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto getKey() const -> const size_t { return key_; }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static auto Texture::storeTexture(const Texture&& t) -> size_t;
    ///
    /// \brief  Stores a texture, replacing the existing one if any.
    ///
    /// \author Runik
    /// \date   29/03/2021
    ///
    /// \param  t   A Texture to store.
    ///
    /// \returns    The key of the texture.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static auto storeTexture(const Texture&& t) -> size_t;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static auto Texture::getTexture(const size_t key) -> Texture&;
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

    static auto getTexture(const size_t key) -> Texture&;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static auto Texture::doesTextureExist(const Texture& t) -> bool;
    ///
    /// \brief  Checks if the texture exists.
    ///
    /// \author Runik
    /// \date   29/03/2021
    ///
    /// \param  t   A Texture to check.
    ///
    /// \returns    True if the texture already exists.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static auto doesTextureExist(const Texture& t) -> bool;

    u16             width_{};    ///< The texture width.
    u16             height_{};   ///< The texture height.
    std::vector<u8> raw_data_{}; ///< Raw texture data.

  private:
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Texture::calculateKey(const VdpType vp, const u32 address, const ColorCount color_count);
    ///
    /// \brief  Calculates the texture key that will be used in the storage.
    ///
    /// \author Runik
    /// \date   29/03/2021
    ///
    /// \param  vp          The type of VDP texture.
    /// \param  address     The address.
    /// \param  color_count Number of colors.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void calculateKey(const VdpType vp, const u32 address, const ColorCount color_count);

    static std::map<size_t, Texture> texture_storage_; ///< The texture storage

    VdpType vdp_type_{VdpType::not_set}; ///< What kind of VDP type is linked to this texture.
    // u16     width_{};                    ///< The texture width.
    // u16     height_{};                   ///< The texture height.
    bool   is_discarded{false}; ///< True if the texture is discarded.
    size_t key_{};              ///< The key of the part.

    // std::vector<u8> raw_data_{}; ///< Raw texture data.

    u32 api_handle_{}; ///< Handle to the graphics API.
};

} // namespace saturnin::video
