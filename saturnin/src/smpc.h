// 
// smpc.h
// Saturnin
//
// Copyright (c) 2018-2019 Renaud Toumazet
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
/// \file	smpc.h
///
/// \brief	Declares everything related to the System Manager & Peripheral Control (SMPC).
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace saturnin {
namespace core {

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   SmpcCommand
///
/// \brief  SMPC commands definition.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class SmpcCommand : u8 {
    // Resetable commands
    master_sh2_on = 0x00,       ///< MSHON command
    slave_sh2_on = 0x02,        ///< SSHON command
    slave_sh2_off = 0x03,       ///< SSHOFF command
    sound_on = 0x06,            ///< SNDON command
    sound_off = 0x07,           ///< SNDOFF command
    cd_on = 0x08,               ///< CDDN command
    cd_off = 0x09,              ///< CDOFF command
    reset_entire_system = 0x0d, ///< SYSRES command
    clock_change_352 = 0x0e,    ///< CKCHG352 command
    clock_change_320 = 0x0f,    ///< CKCHG320 command
    nmi_request = 0x18,         ///< NMIREQ command
    reset_enable = 0x19,        ///< RESENAB command
    reset_disable = 0x1a,       ///< RESDISA command

    // Non resetable commands
    interrupt_back = 0x10,      ///< INTBACK command
    smpc_memory_setting = 0x17, ///< SETSMEM command

    // RTC command
    time_setting = 0x16         ///< SETTIME command
};

class Smpc {
public:
    //@{
    // Constructors / Destructors
    Smpc() = delete;
    Smpc(Emulator_context* ec) : emulator_context_(ec) {};
    Smpc(const Smpc&) = delete;
    Smpc(Smpc&&) = delete;
    Smpc& operator=(const Smpc&) & = delete;
    Smpc& operator=(Smpc&&) & = delete;
    ~Smpc() = default;
    //@}

private:
    
    
    Emulator_context* emulator_context_;    ///< Context of the emulator

};

}
}