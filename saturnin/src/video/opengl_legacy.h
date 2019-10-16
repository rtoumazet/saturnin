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

#include "opengl.h"
#include "../emulator_defs.h"

// Forward declarations
namespace saturnin::core {
    class Config;
    class Emulator_context;
}

namespace saturnin {
namespace video {

using saturnin::core::Config;
using saturnin::core::Emulator_context;


class OpenglLegacy : public Opengl {

public:
    //@{
    // Constructors / Destructors
    OpenglLegacy(core::Config* config) : Opengl(config) { initialize();  };
    ~OpenglLegacy() { shutdown(); };
    //@}
        
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void initialize() override;
    ///
    /// \brief  Initializes the object.
    ///
    /// \author Runik
    /// \date   16/10/2019
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void initialize() override;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void shutdown() override;
    ///
    /// \brief  Clean up code.
    ///
    /// \author Runik
    /// \date   16/10/2019
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void shutdown() override;
        
    void preRender() override;

    void render() override;

    void postRender() override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn s32 runLegacyOpengl(core::Emulator_context& config);
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

s32 runLegacyOpengl(Emulator_context& state);

};
};