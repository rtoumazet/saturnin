//
// opengl_legacy.h
// Saturnin
//
// Copyright (c) 2019 Renaud Toumazet
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
/// \file	opengl_legacy.h
///
/// \brief	OpenGL class and functions for legacy code (OpenGL 2.1).
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <saturnin/src/pch.h>
#include <saturnin/src/emulator_defs.h>
#include <saturnin/src/video/opengl.h>

// Forward declarations
namespace saturnin::core {
class Config;
class EmulatorContext;
} // namespace saturnin::core

namespace saturnin::video {

using saturnin::core::Config;
using saturnin::core::EmulatorContext;

class OpenglLegacy : public Opengl {
  public:
    //@{
    // Constructors / Destructors
    OpenglLegacy(core::Config* config) : Opengl(config) { initialize(); };
    ~OpenglLegacy() final { shutdown(); };
    //@}

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void initialize() final;
    ///
    /// \brief  Initializes the object.
    ///
    /// \author Runik
    /// \date   16/10/2019
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void initialize() final;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void shutdown() final;
    ///
    /// \brief  Clean up code.
    ///
    /// \author Runik
    /// \date   16/10/2019
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void shutdown() final;

    // void initializeTexture(const u32 width, const u32 height);

    void preRender() final;

    void render() final;

    void postRender() final;

    void updateTextureSize(u32 width, u32 height) final;

  protected:
    [[nodiscard]] u32 generateTextureFromVector(u32 width, u32 height, const std::vector<u8>& data) const final;

  private:
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn [[nodiscard]] auto OpenglLegacy::generateEmptyTexture(const u32 width, const u32 height) const -> u32 final;
    ///
    /// \brief  Generates an empty texture.
    ///
    /// \author Runik
    /// \date   17/10/2019
    ///
    /// \param  width   Width of the generated texture.
    /// \param  height  Height of the generated texture.
    ///
    /// \return The empty texture.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    [[nodiscard]] auto generateEmptyTexture(u32 width, u32 height) const -> u32 final;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn u32 OpenglLegacy::bindTextureToFbo() const final;
    ///
    /// \brief  Binds the internal texture to the internal fbo.
    ///
    /// \author Runik
    /// \date   18/10/2019
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void bindTextureToFbo() const final;

    void deleteTexture() const final;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn s32 runLegacyOpengl(core::EmulatorContext& config);
///
/// \brief  Executes OpenGL code in legacy mode.
///
/// \author Runik
/// \date   03/04/2018
///
/// \param [in,out] config  A reference to the configuration object.
///
/// \return An int32_t.
////////////////////////////////////////////////////////////////////////////////////////////////////

auto runLegacyOpengl(EmulatorContext& state) -> s32;

}; // namespace saturnin::video
