//
// vdp2_part.cpp
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
#include <saturnin/src/video/vdp2_part.h>
#include <saturnin/src/video/vdp2.h> // PatternNameData

namespace saturnin::video {

Vdp2Part::Vdp2Part(const PatternNameData& pnd, ScreenPos&& pos, size_t texture_key) {
    setVdpType(VdpType::vdp2);
    scroll_screen_pos_       = std::move(pos);
    character_number_        = pnd.character_number;
    palette_number_          = pnd.palette_number;
    is_horizontally_flipped_ = pnd.is_horizontally_flipped;
    is_vertically_flipped_   = pnd.is_vertically_flipped;
    texture_key_             = texture_key;
};

void Vdp2Part::renderPart() { displayCell(); };

void Vdp2Part::displayCell() {
    // glGenTextures(1, &texture_);
    // glBindTexture(GLenum::GL_TEXTURE_2D,
    //              texture_); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object

    //// set the texture wrapping parameters
    // glTexParameteri(GLenum::GL_TEXTURE_2D,
    //                GLenum::GL_TEXTURE_WRAP_S,
    //                GLenum::GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
    // glTexParameteri(GLenum::GL_TEXTURE_2D, GLenum::GL_TEXTURE_WRAP_T, GLenum::GL_REPEAT);
    //// set texture filtering parameters
    // glTexParameteri(GLenum::GL_TEXTURE_2D, GLenum::GL_TEXTURE_MIN_FILTER, GLenum::GL_NEAREST);
    // glTexParameteri(GLenum::GL_TEXTURE_2D, GLenum::GL_TEXTURE_MAG_FILTER, GLenum::GL_NEAREST);
    // auto window = glfwGetCurrentContext();
    // auto state  = reinterpret_cast<core::EmulatorContext*>(glfwGetWindowUserPointer(window));
    // if (!state->vdp2()->vdp2_parts_[3].empty()) {
    //    auto  key = state->vdp2()->vdp2_parts_[3][0].getTextureKey();
    //    auto& tex = Texture::getTexture(key);

    //    glTexImage2D(GLenum::GL_TEXTURE_2D,
    //                 0,
    //                 GLenum::GL_RGB,
    //                 tex.width_,
    //                 tex.height_,
    //                 0,
    //                 GLenum::GL_RGB,
    //                 GLenum::GL_UNSIGNED_BYTE,
    //                 tex.data_.data());
    //}
}

} // namespace saturnin::video
