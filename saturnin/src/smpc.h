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

#include <vector> // vector
#include "smpc_registers.h"

// Forward declarations
namespace saturnin::core {
    class Emulator_context;
}

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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   SystemClock
///
/// \brief  System clock values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class SystemClock : u32 {
    not_set  = 0,
    ntsc_320 = 26874100,
    ntsc_352 = 28636400,
    pal_320  = 26687500,
    pal_352  = 28437500
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   PeripheralLayout
///
/// \brief  Peripheral layout values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class PeripheralLayout {
    empty,   ///< Empty layout, nothing is mapped
    default, ///< Default layout
    current  ///< Currently mapped layout
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct SaturnDigitalPad
///
/// \brief  Mapping between the Saturn Digital Pad and the host.
///
/// \author Runik
/// \date   15/12/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

struct SaturnDigitalPad {
    u16 direction_left;
    u16 direction_right;
    u16 direction_up;
    u16 direction_down;
    u16 button_shoulder_left;
    u16 button_shoulder_right;
    u16 button_a;
    u16 button_b;
    u16 button_c;
    u16 button_x;
    u16 button_y;
    u16 button_z;
    u16 button_start;

    std::vector<u16> toConfig(const PeripheralLayout);
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct SaturnPeripheralMapping
///
/// \brief  Peripheral mapping for the Saturn.
///
/// \author Runik
/// \date   15/12/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

struct SaturnPeripheralMapping {
    SaturnDigitalPad player_1;
    SaturnDigitalPad player_2;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct StvPlayerControls
///
/// \brief  Mapping between the ST-V player I/O and the host.
///
/// \author Runik
/// \date   15/12/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

struct StvPlayerControls {
    u16 direction_left;
    u16 direction_right;
    u16 direction_up;
    u16 direction_down;
    u16 button_1;
    u16 button_2;
    u16 button_3;
    u16 button_4;

    std::vector<u16> toConfig(const PeripheralLayout);
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct StvBoardControls
///
/// \brief  ST-V board controls.
///
/// \author Runik
/// \date   15/12/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

struct StvBoardControls {
    u16 service_switch;
    u16 test_switch;
    u16 p1_coin_switch;
    u16 p2_coin_switch;

    std::vector<u16> toConfig(const PeripheralLayout);
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \struct StvPeripheralMapping
///
/// \brief  Peripheral mapping for the ST-V.
///
/// \author Runik
/// \date   15/12/2019
////////////////////////////////////////////////////////////////////////////////////////////////////

struct StvPeripheralMapping {
    StvBoardControls board_controls;
    StvPlayerControls player_1;
    StvPlayerControls player_2;
};

class Smpc {
public:
    //@{
    // Constructors / Destructors
    Smpc() = delete;
    Smpc(Emulator_context* ec) : emulator_context_(ec) { reset();  };
    Smpc(const Smpc&) = delete;
    Smpc(Smpc&&) = delete;
    Smpc& operator=(const Smpc&) & = delete;
    Smpc& operator=(Smpc&&) & = delete;
    ~Smpc() = default;
    //@}
    
    u8 read(const u32 addr);
    void write(const u32 addr, const u8 data);

private:

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Smpc::reset();
    ///
    /// \brief  Resets the SMPC.
    ///
    /// \author Runik
    /// \date   14/12/2019
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void reset();

    Emulator_context* emulator_context_;    ///< Context of the emulator

    //@{
    // Internal registers
    CommandRegister       comreg_;
    StatusRegister        sr_;
    StatusFlag            sf_;
    InputRegister         ireg_[7];
    OutputRegister        oreg_[32];
    PortDataRegister      pdr1_;
    PortDataRegister      pdr2_;
    DataDirectionRegister ddr1_;
    DataDirectionRegister ddr2_;
    IOSelect              iosel_;
    ExternalLatchEnable   exle_;
    //@}
    
    SystemClock clock_{ SystemClock::not_set }; 
};

}
}