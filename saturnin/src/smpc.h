// 
// smpc.h
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
/// \file	smpc.h
///
/// \brief	Declares everything related to the System Manager & Peripheral Control (SMPC).
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <Windows.h> // Removes C4005 warning
#include <GLFW/glfw3.h> // Keyboard handling
#include <vector> // vector
#include <chrono> // duration
#include "smpc_registers.h"

// Forward declarations
namespace saturnin::core {
    class EmulatorContext;
}

namespace saturnin {
namespace core {

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
/// \enum   PeripheralKeys
///
/// \brief  Keys.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class PeripheralKey : u16 {
    key_unknown       = 0,

    /* Printable keys */
    key_space         = GLFW_KEY_SPACE,
    key_apostrophe    = GLFW_KEY_APOSTROPHE,
    key_comma         = GLFW_KEY_COMMA,
    key_minus         = GLFW_KEY_MINUS,
    key_period        = GLFW_KEY_PERIOD,
    key_slash         = GLFW_KEY_SLASH,
    key_0             = GLFW_KEY_0,
    key_1             = GLFW_KEY_1,
    key_2             = GLFW_KEY_2,
    key_3             = GLFW_KEY_3,
    key_4             = GLFW_KEY_4,
    key_5             = GLFW_KEY_5,
    key_6             = GLFW_KEY_6,
    key_7             = GLFW_KEY_7,
    key_8             = GLFW_KEY_8,
    key_9             = GLFW_KEY_9,
    key_semicolon     = GLFW_KEY_SEMICOLON,
    key_equal         = GLFW_KEY_EQUAL,
    key_a             = GLFW_KEY_A,
    key_b             = GLFW_KEY_B,
    key_c             = GLFW_KEY_C,
    key_d             = GLFW_KEY_D,
    key_e             = GLFW_KEY_E,
    key_f             = GLFW_KEY_F,
    key_g             = GLFW_KEY_G,
    key_h             = GLFW_KEY_H,
    key_i             = GLFW_KEY_I,
    key_j             = GLFW_KEY_J,
    key_k             = GLFW_KEY_K,
    key_l             = GLFW_KEY_L,
    key_m             = GLFW_KEY_M,
    key_n             = GLFW_KEY_N,
    key_o             = GLFW_KEY_O,
    key_p             = GLFW_KEY_P,
    key_q             = GLFW_KEY_Q,
    key_r             = GLFW_KEY_R,
    key_s             = GLFW_KEY_S,
    key_t             = GLFW_KEY_T,
    key_u             = GLFW_KEY_U,
    key_v             = GLFW_KEY_V,
    key_w             = GLFW_KEY_W,
    key_x             = GLFW_KEY_X,
    key_y             = GLFW_KEY_Y,
    key_z             = GLFW_KEY_Z,
    key_left_bracket  = GLFW_KEY_LEFT_BRACKET,
    key_backslash     = GLFW_KEY_BACKSLASH,
    key_right_bracket = GLFW_KEY_RIGHT_BRACKET,
    key_grave_accent  = GLFW_KEY_GRAVE_ACCENT,

    /* Function keys */
    key_escape        = GLFW_KEY_ESCAPE,
    key_enter         = GLFW_KEY_ENTER,
    key_tab           = GLFW_KEY_TAB,
    key_backspace     = GLFW_KEY_BACKSPACE,
    key_insert        = GLFW_KEY_INSERT,
    key_delete        = GLFW_KEY_DELETE,
    key_right         = GLFW_KEY_RIGHT,
    key_left          = GLFW_KEY_LEFT,
    key_down          = GLFW_KEY_DOWN,
    key_up            = GLFW_KEY_UP,
    key_page_up       = GLFW_KEY_PAGE_UP,
    key_page_down     = GLFW_KEY_PAGE_DOWN,
    key_home          = GLFW_KEY_HOME,
    key_end           = GLFW_KEY_END,
    key_caps_lock     = GLFW_KEY_CAPS_LOCK,
    key_scroll_lock   = GLFW_KEY_SCROLL_LOCK,
    key_num_lock      = GLFW_KEY_NUM_LOCK,
    key_print_screen  = GLFW_KEY_PRINT_SCREEN,
    key_pause         = GLFW_KEY_PAUSE,
    key_f1            = GLFW_KEY_F1,
    key_f2            = GLFW_KEY_F2,
    key_f3            = GLFW_KEY_F3,
    key_f4            = GLFW_KEY_F4,
    key_f5            = GLFW_KEY_F5,
    key_f6            = GLFW_KEY_F6,
    key_f7            = GLFW_KEY_F7,
    key_f8            = GLFW_KEY_F8,
    key_f9            = GLFW_KEY_F9,
    key_f10           = GLFW_KEY_F10,
    key_f11           = GLFW_KEY_F11,
    key_f12           = GLFW_KEY_F12,
    key_kp_0          = GLFW_KEY_KP_0,
    key_kp_1          = GLFW_KEY_KP_1,
    key_kp_2          = GLFW_KEY_KP_2,
    key_kp_3          = GLFW_KEY_KP_3,
    key_kp_4          = GLFW_KEY_KP_4,
    key_kp_5          = GLFW_KEY_KP_5,
    key_kp_6          = GLFW_KEY_KP_6,
    key_kp_7          = GLFW_KEY_KP_7,
    key_kp_8          = GLFW_KEY_KP_8,
    key_kp_9          = GLFW_KEY_KP_9,
    key_kp_decimal    = GLFW_KEY_KP_DECIMAL,
    key_kp_divide     = GLFW_KEY_KP_DIVIDE,
    key_kp_multiply   = GLFW_KEY_KP_MULTIPLY,
    key_kp_substract  = GLFW_KEY_KP_SUBTRACT,
    key_kp_add        = GLFW_KEY_KP_ADD,
    key_kp_enter      = GLFW_KEY_KP_ENTER,
    key_kp_equal      = GLFW_KEY_KP_EQUAL,
    key_left_shift    = GLFW_KEY_LEFT_SHIFT,
    key_left_control  = GLFW_KEY_LEFT_CONTROL,
    key_left_alt      = GLFW_KEY_LEFT_ALT,
    key_left_super    = GLFW_KEY_LEFT_SUPER,
    key_right_shift   = GLFW_KEY_RIGHT_SHIFT,
    key_right_control = GLFW_KEY_RIGHT_CONTROL,
    key_right_alt     = GLFW_KEY_RIGHT_ALT,
    key_right_super   = GLFW_KEY_RIGHT_SUPER,
    key_menu          = GLFW_KEY_MENU
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
    PeripheralKey direction_left;
    PeripheralKey direction_right;
    PeripheralKey direction_up;
    PeripheralKey direction_down;
    PeripheralKey button_shoulder_left;
    PeripheralKey button_shoulder_right;
    PeripheralKey button_a;
    PeripheralKey button_b;
    PeripheralKey button_c;
    PeripheralKey button_x;
    PeripheralKey button_y;
    PeripheralKey button_z;
    PeripheralKey button_start;

    std::vector<PeripheralKey> toConfig(const PeripheralLayout);
    void fromConfig(std::vector<PeripheralKey>);
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
    PeripheralKey direction_left;
    PeripheralKey direction_right;
    PeripheralKey direction_up;
    PeripheralKey direction_down;
    PeripheralKey button_1;
    PeripheralKey button_2;
    PeripheralKey button_3;
    PeripheralKey button_4;

    std::vector<PeripheralKey> toConfig(const PeripheralLayout);
    void fromConfig(std::vector<PeripheralKey>);
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
    PeripheralKey service_switch;
    PeripheralKey test_switch;
    PeripheralKey p1_coin_switch;
    PeripheralKey p2_coin_switch;
    PeripheralKey p1_start;
    PeripheralKey p2_start;

    std::vector<PeripheralKey> toConfig(const PeripheralLayout);
    void fromConfig(std::vector<PeripheralKey>);
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
    Smpc(EmulatorContext* ec) : emulator_context_(ec) { reset();  };
    Smpc(const Smpc&) = delete;
    Smpc(Smpc&&) = delete;
    Smpc& operator=(const Smpc&) & = delete;
    Smpc& operator=(Smpc&&) & = delete;
    ~Smpc() = default;
    //@}
    
    u8 read(const u32 addr);
    void write(const u32 addr, const u8 data);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn std::vector<PeripheralKey> Smpc::listAvailableKeys();
    ///
    /// \brief  Returns the keyboard keys available.
    ///
    /// \author Runik
    /// \date   03/01/2020
    ///
    /// \return A std::vector&lt;PeripheralKey&gt;
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<PeripheralKey> listAvailableKeys();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Smpc::initializePeripheralMappings();
    ///
    /// \brief  Initializes the peripheral mappings for both Saturn and ST-V.
    ///
    /// \author Runik
    /// \date   03/01/2020
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void initializePeripheralMappings();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn SaturnPeripheralMapping Smpc::getSaturnPeripheralMapping();
    ///
    /// \brief  Returns the current Saturn peripheral mapping.
    ///
    /// \author Runik
    /// \date   03/01/2020
    ///
    /// \return The saturn peripheral mapping.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    SaturnPeripheralMapping getSaturnPeripheralMapping();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn StvPeripheralMapping Smpc::getStvPeripheralMapping();
    ///
    /// \brief  Returns the current ST-V peripheral mapping.
    ///
    /// \author Runik
    /// \date   03/01/2020
    ///
    /// \return The stv peripheral mapping.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    StvPeripheralMapping getStvPeripheralMapping();
    
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

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Smpc::setCommandDuration();
    ///
    /// \brief  Sets the SMPC command duration.
    ///
    /// \author Runik
    /// \date   11/01/2020
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void setCommandDuration();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Smpc::executeCommand();
    ///
    /// \brief  Executes the current SMPC command.
    ///
    /// \author Runik
    /// \date   11/01/2020
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void executeCommand();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn u32 Smpc::calculateCyclesNumber(const std::chrono::duration<double>& d);
    ///
    /// \brief  Calculates the number of cycles needed for the duration.
    ///
    /// \author Runik
    /// \date   06/01/2020
    ///
    /// \param  d   duration.
    ///
    /// \return The number of cycles needed for the duration.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    u32 calculateCyclesNumber(const std::chrono::duration<double>& d);

    EmulatorContext* emulator_context_;    ///< Context of the emulator

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

    SaturnPeripheralMapping saturn_mapping_;    ///< Saturn paripheral mapping
    StvPeripheralMapping    stv_mapping_;   ///< ST-V peripehral mapping

    u16 intback_remaining_cycles_ {}; ///< The intback remaining cycles

    u32 total_display_duration_in_cycles_{};    ///< Number of cycles needed to display a full frame.
    u32 visible_display_duration_in_cycles_{};  ///< Number of cycles needed to display the visible part of a frame (total - vblank).
    u32 vblank_duration_in_cycles_{};           ///< Number of cycles needed to display the vblank part of a frame (total - visible).

    bool is_master_sh2_on_{ false }; ///< Master SH2 status
    bool is_slave_sh2_on_{ false }; ///< Slave SH2 status
    bool is_sound_on_{ false }; ///< Sound status
    bool is_soft_reset_allowed_{ false }; ///< NMI generation from reset button status

    std::array <u8, 0x4> smem_;   ///< SMPC battery backupable memory (4B).
    
    
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn std::string getKeyName(const PeripheralKey pk);
///
/// \brief  Gets printable name of the key.
///
/// \author Runik
/// \date   30/12/2019
///
/// \param  pk  The key.
///
/// \return The key name.
////////////////////////////////////////////////////////////////////////////////////////////////////

std::string getKeyName(const PeripheralKey pk);

}
}