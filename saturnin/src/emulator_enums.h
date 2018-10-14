// 
// emulator_enums.h
// Saturnin
//
// Copyright (c) 2018 Renaud Toumazet
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
/// \file	emulator_enums.h
///
/// \brief	Emulator enumerations. 
///
/// Regroups enumerations related to the emulator context
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace saturnin {
namespace core {

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum	Hardware_mode
///
/// \brief	Hardware mode used. 
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Hardware_mode {
    saturn  = 0, ///< Saturn.
    stv     = 1  ///< ST-V.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   Emulation_status
///
/// \brief  State of the emulation.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Emulation_status {
    running,    ///< Emulator is running
    paused,     ///< Emulator is paused
    stopped,    ///< Emulator is stopped
    reset       ///< Emulator is reset
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   Rendering_status
///
/// \brief  Rendering status. Needed to reset display when modifying rendring type in the options.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Rendering_status {
    running,
    reset,
    stopped
};

}
}