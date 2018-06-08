// 
// emustate.h
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
/// \file	emustate.h
///
/// \brief	Emulation state class. 
///
/// Class used to handle emulation state data
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <memory> // unique_ptr, make_unique

namespace saturnin {
namespace core {

    /// \name Class declarations.
    /// Used to speed up build time as the .h files are included in the .cpp
    ///
    //@{
    //class CSH2;
    class Memory;
    //class COGL;
    //class InterruptHandler;
    //class Log;
    //class CCdRom;
    //class SMPC;
    //class SCU;
    //class Vdp1;
    //class Vdp2;
    //class SCSP;
    //@}

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \enum	Hardware_mode
    ///
    /// \brief	Values that represent the hardware mode. 
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    enum class Hardware_mode {
        saturn, ///< Saturn.
        stv     ///< ST-V.
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \enum	Rom_type
    ///
    /// \brief	Values that represent the ROM type. 
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    enum class Rom_type {
        program, ///< Program ROM.
        graphic, ///< Graphic ROM.
        bios	 ///< Bios.
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \enum	Rom_load
    ///
    /// \brief	Values that represent the way ROM is loaded. 
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    enum class Rom_load {
        not_interleaved, ///< Loads data sequentially. 
        odd_interleaved, ///< Loads data on odd bytes. 
        even_interleaved ///< Loads data on even bytes.
    };

    struct Emu_state {
        Hardware_mode current_hardware_mode;

        std::unique_ptr<Memory> memory = std::make_unique<Memory>();
    };
}
}