//
// smpc.cpp
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

#include <saturnin/src/pch.h>
#include <saturnin/src/smpc.h>
#include <date/date.h>
#include <map>    // map
#include <string> // string
#include <saturnin/src/config.h>
#include <saturnin/src/emulator_context.h>
#include <saturnin/src/locale.h>
#include <saturnin/src/sh2.h>
#include <saturnin/src/sound/scsp.h>

namespace saturnin::core {

namespace uti = saturnin::utilities;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   PortMode
///
/// \brief  Port mode values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class PortMode : u8 {
    mode_15_byte  = 0b00, ///< 15-byte mode.
    mode_255_byte = 0b01, ///< 255-byte mode.
    mode_reserved = 0b10, ///< Sega reserved.
    mode_0_byte   = 0b11  ///< 0-byte mode.
};

using MapKeyboardLayout    = std::map<PeripheralKey, const std::string>;
const auto keyboard_layout = MapKeyboardLayout{{PeripheralKey::key_space, "space"},
                                               {PeripheralKey::key_apostrophe, "'"},
                                               {PeripheralKey::key_comma, ","},
                                               {PeripheralKey::key_minus, "-"},
                                               {PeripheralKey::key_period, "."},
                                               {PeripheralKey::key_slash, "/"},
                                               {PeripheralKey::key_semicolon, ";"},
                                               {PeripheralKey::key_equal, "="},
                                               {PeripheralKey::key_0, "0"},
                                               {PeripheralKey::key_1, "1"},
                                               {PeripheralKey::key_2, "2"},
                                               {PeripheralKey::key_3, "3"},
                                               {PeripheralKey::key_4, "4"},
                                               {PeripheralKey::key_5, "5"},
                                               {PeripheralKey::key_6, "6"},
                                               {PeripheralKey::key_7, "7"},
                                               {PeripheralKey::key_8, "8"},
                                               {PeripheralKey::key_9, "9"},
                                               {PeripheralKey::key_a, "a"},
                                               {PeripheralKey::key_b, "b"},
                                               {PeripheralKey::key_c, "c"},
                                               {PeripheralKey::key_d, "d"},
                                               {PeripheralKey::key_e, "e"},
                                               {PeripheralKey::key_f, "f"},
                                               {PeripheralKey::key_g, "g"},
                                               {PeripheralKey::key_h, "h"},
                                               {PeripheralKey::key_i, "i"},
                                               {PeripheralKey::key_j, "j"},
                                               {PeripheralKey::key_k, "k"},
                                               {PeripheralKey::key_l, "l"},
                                               {PeripheralKey::key_m, "m"},
                                               {PeripheralKey::key_n, "n"},
                                               {PeripheralKey::key_o, "o"},
                                               {PeripheralKey::key_p, "p"},
                                               {PeripheralKey::key_q, "q"},
                                               {PeripheralKey::key_r, "r"},
                                               {PeripheralKey::key_s, "s"},
                                               {PeripheralKey::key_t, "t"},
                                               {PeripheralKey::key_u, "u"},
                                               {PeripheralKey::key_v, "v"},
                                               {PeripheralKey::key_w, "w"},
                                               {PeripheralKey::key_x, "x"},
                                               {PeripheralKey::key_y, "y"},
                                               {PeripheralKey::key_z, "z"},
                                               {PeripheralKey::key_left_bracket, "["},
                                               {PeripheralKey::key_backslash, "\\"},
                                               {PeripheralKey::key_right_bracket, "]"},
                                               {PeripheralKey::key_grave_accent, "`"},

                                               /* Function keys */
                                               {PeripheralKey::key_escape, "esc"},
                                               {PeripheralKey::key_enter, "enter"},
                                               {PeripheralKey::key_tab, "tab"},
                                               {PeripheralKey::key_backspace, "backspace"},
                                               {PeripheralKey::key_insert, "insert"},
                                               {PeripheralKey::key_delete, "del"},
                                               {PeripheralKey::key_right, "right"},
                                               {PeripheralKey::key_left, "left"},
                                               {PeripheralKey::key_down, "down"},
                                               {PeripheralKey::key_up, "up"},
                                               {PeripheralKey::key_page_up, "page up"},
                                               {PeripheralKey::key_page_down, "page down"},
                                               {PeripheralKey::key_home, "home"},
                                               {PeripheralKey::key_end, "end"},
                                               {PeripheralKey::key_caps_lock, "caps lock"},
                                               {PeripheralKey::key_scroll_lock, "scroll lock"},
                                               {PeripheralKey::key_num_lock, "num lock"},
                                               {PeripheralKey::key_print_screen, "print screen"},
                                               {PeripheralKey::key_pause, "pause"},
                                               {PeripheralKey::key_f1, "f1"},
                                               {PeripheralKey::key_f2, "f2"},
                                               {PeripheralKey::key_f3, "f3"},
                                               {PeripheralKey::key_f4, "f4"},
                                               {PeripheralKey::key_f5, "f5"},
                                               {PeripheralKey::key_f6, "f6"},
                                               {PeripheralKey::key_f7, "f7"},
                                               {PeripheralKey::key_f8, "f8"},
                                               {PeripheralKey::key_f9, "f9"},
                                               {PeripheralKey::key_f10, "f10"},
                                               {PeripheralKey::key_f11, "f11"},
                                               {PeripheralKey::key_f12, "f12"},
                                               {PeripheralKey::key_kp_0, "keypad 0"},
                                               {PeripheralKey::key_kp_1, "keypad 1"},
                                               {PeripheralKey::key_kp_2, "keypad 2"},
                                               {PeripheralKey::key_kp_3, "keypad 3"},
                                               {PeripheralKey::key_kp_4, "keypad 4"},
                                               {PeripheralKey::key_kp_5, "keypad 5"},
                                               {PeripheralKey::key_kp_6, "keypad 6"},
                                               {PeripheralKey::key_kp_7, "keypad 7"},
                                               {PeripheralKey::key_kp_8, "keypad 8"},
                                               {PeripheralKey::key_kp_9, "keypad 9"},
                                               {PeripheralKey::key_kp_decimal, "keypad ."},
                                               {PeripheralKey::key_kp_divide, "keypad /"},
                                               {PeripheralKey::key_kp_multiply, "keypad *"},
                                               {PeripheralKey::key_kp_substract, "keypad -"},
                                               {PeripheralKey::key_kp_add, "keypad +"},
                                               {PeripheralKey::key_kp_enter, "keypad enter"},
                                               {PeripheralKey::key_kp_equal, "keypad ="},
                                               {PeripheralKey::key_left_shift, "left shift"},
                                               {PeripheralKey::key_left_control, "left control"},
                                               {PeripheralKey::key_left_alt, "left alt"},
                                               {PeripheralKey::key_left_super, "left super"},
                                               {PeripheralKey::key_right_shift, "right shift"},
                                               {PeripheralKey::key_right_control, "right control"},
                                               {PeripheralKey::key_right_alt, "right alt"},
                                               {PeripheralKey::key_right_super, "right super"},
                                               {PeripheralKey::key_menu, "menu"}};

auto SaturnDigitalPad::toConfig(const PeripheralLayout layout) const -> std::vector<PeripheralKey> {
    switch (layout) {
        using enum PeripheralLayout;
        using enum PeripheralKey;
        case empty_layout:

            return std::vector<PeripheralKey>{key_unknown,
                                              key_unknown,
                                              key_unknown,
                                              key_unknown,
                                              key_unknown,
                                              key_unknown,
                                              key_unknown,
                                              key_unknown,
                                              key_unknown,
                                              key_unknown,
                                              key_unknown,
                                              key_unknown,
                                              key_unknown};
        case current_layout:
            return std::vector<PeripheralKey>{direction_left,
                                              direction_right,
                                              direction_up,
                                              direction_down,
                                              button_shoulder_left,
                                              button_shoulder_right,
                                              button_a,
                                              button_b,
                                              button_c,
                                              button_x,
                                              button_y,
                                              button_z,
                                              button_start};
        default:
            return std::vector<PeripheralKey>{key_left,
                                              key_right,
                                              key_up,
                                              key_down,
                                              key_z,
                                              key_e,
                                              key_s,
                                              key_d,
                                              key_f,
                                              key_x,
                                              key_c,
                                              key_v,
                                              key_enter};
    }
}
void SaturnDigitalPad::fromConfig(std::vector<PeripheralKey> config) {
    if (constexpr auto param_number = u8{13}; config.size() != param_number) {
        Log::warning(Logger::smpc, tr("Incorrect Saturn pad data"));
        const auto v   = SaturnDigitalPad().toConfig(PeripheralLayout::empty_layout);
        auto       pad = SaturnDigitalPad{};
        pad.fromConfig(v);
        *this = pad;
        return;
    }

    direction_left        = config[0];
    direction_right       = config[1];
    direction_up          = config[2];
    direction_down        = config[3];
    button_shoulder_left  = config[4];
    button_shoulder_right = config[5];
    button_a              = config[6];
    button_b              = config[7];
    button_c              = config[8];
    button_x              = config[9];
    button_y              = config[10];
    button_z              = config[11];
    button_start          = config[12];
}

auto StvPlayerControls::toConfig(const PeripheralLayout layout) const -> std::vector<PeripheralKey> {
    switch (layout) {
        using enum PeripheralLayout;
        using enum PeripheralKey;
        case empty_layout:
            return std::vector<PeripheralKey>{key_unknown,
                                              key_unknown,
                                              key_unknown,
                                              key_unknown,
                                              key_unknown,
                                              key_unknown,
                                              key_unknown,
                                              key_unknown};
        case current_layout:
            return std::vector<PeripheralKey>{direction_left,
                                              direction_right,
                                              direction_up,
                                              direction_down,
                                              button_1,
                                              button_2,
                                              button_3,
                                              button_4};
        default: return std::vector<PeripheralKey>{key_left, key_right, key_up, key_down, key_s, key_d, key_f, key_g};
    }
}

void StvPlayerControls::fromConfig(std::vector<PeripheralKey> config) {
    if (constexpr auto param_number = 8; config.size() != param_number) {
        Log::warning(Logger::smpc, tr("Incorrect ST-V player control data"));
        const auto v       = StvPlayerControls().toConfig(PeripheralLayout::empty_layout);
        auto       control = StvPlayerControls{};
        control.fromConfig(v);
        *this = control;
        return;
    }
    direction_left  = config[0];
    direction_right = config[1];
    direction_up    = config[2];
    direction_down  = config[3];
    button_1        = config[4];
    button_2        = config[5];
    button_3        = config[6];
    button_4        = config[7];
}

auto StvBoardControls::toConfig(const PeripheralLayout layout) const -> std::vector<PeripheralKey> {
    switch (layout) {
        using enum PeripheralLayout;
        using enum PeripheralKey;
        case empty_layout:
            return std::vector<PeripheralKey>{key_unknown, key_unknown, key_unknown, key_unknown, key_unknown, key_unknown};
        case current_layout:
            return std::vector<PeripheralKey>{service_switch, test_switch, p1_coin_switch, p2_coin_switch, p1_start, p2_start};
        default: return std::vector<PeripheralKey>{key_1, key_2, key_5, key_6, key_7, key_8};
    }
}

void StvBoardControls::fromConfig(std::vector<PeripheralKey> config) {
    if (constexpr auto param_number = u8{6}; config.size() != param_number) {
        Log::warning(Logger::smpc, tr("Incorrect ST-V board control data"));
        const auto v       = StvBoardControls().toConfig(PeripheralLayout::empty_layout);
        auto       control = StvBoardControls{};
        control.fromConfig(v);
        *this = control;
        return;
    }
    service_switch = config[0];
    test_switch    = config[1];
    p1_coin_switch = config[2];
    p2_coin_switch = config[3];
    p1_start       = config[4];
    p2_start       = config[5];
}

void Smpc::reset() {
    registers_.comreg = {};
    registers_.sr     = {};
    registers_.sf     = {};
    for (u8 i = 0; i < input_registers_number; ++i) {
        registers_.ireg[i] = {};
    }
    for (u8 i = 0; i < output_registers_number; ++i) {
        registers_.oreg[i] = {};
    }
    registers_.pdr1  = {};
    registers_.pdr2  = {};
    registers_.ddr1  = {};
    registers_.ddr2  = {};
    registers_.iosel = {};
    registers_.exle  = {};

    for (u8 i = 0; i < 4; ++i) {
        smem_[i] = 0;
    }

    // System clock is 320 at reset.
    switch (std::string ts = modules_.config()->readValue(core::AccessKeys::cfg_rendering_tv_standard);
            modules_.config()->getTvStandard(ts)) {
        using enum video::TvStandard;
        using enum SystemClock;
        case pal: clock_ = pal_320; break;
        case ntsc: clock_ = ntsc_320; break;
        default: Log::warning(Logger::smpc, tr("Could not set system clock !")); clock_ = not_set;
    }

    std::string p1c = modules_.config()->readValue(core::AccessKeys::cfg_controls_saturn_player_1_connection);
    port_1_status_  = modules_.config()->configToPortStatus(p1c);

    std::string p2c = modules_.config()->readValue(core::AccessKeys::cfg_controls_saturn_player_2_connection);
    port_2_status_  = modules_.config()->configToPortStatus(p2c);
}

auto Smpc::calculateCyclesNumber(const std::chrono::duration<double>& d) const -> u32 {
    using seconds             = std::chrono::duration<double>;
    const auto cycle_duration = seconds{(static_cast<double>(1) / static_cast<double>(uti::toUnderlying(clock_)))};
    return static_cast<u32>(d / cycle_duration);
}

auto Smpc::getSystemClock() const -> u32 { return uti::toUnderlying(clock_); }

auto Smpc::getRegisters() const -> const AddressToNameMap& { return address_to_name_; };

void Smpc::setCommandDuration() {
    using micro = std::chrono::duration<double, std::micro>;
    using milli = std::chrono::duration<double, std::milli>;
    if (registers_.comreg.is(SmpcRegisters::CommandRegister::master_sh2_on)
        || registers_.comreg.is(SmpcRegisters::CommandRegister::slave_sh2_on)
        || registers_.comreg.is(SmpcRegisters::CommandRegister::slave_sh2_off)
        || registers_.comreg.is(SmpcRegisters::CommandRegister::sound_on)
        || registers_.comreg.is(SmpcRegisters::CommandRegister::sound_off)
        || registers_.comreg.is(SmpcRegisters::CommandRegister::nmi_request)
        || registers_.comreg.is(SmpcRegisters::CommandRegister::reset_enable)
        || registers_.comreg.is(SmpcRegisters::CommandRegister::reset_disable)) {
        constexpr auto duration   = micro(30);
        command_remaining_cycles_ = calculateCyclesNumber(duration);
        return;

    } else if (registers_.comreg.is(SmpcRegisters::CommandRegister::cd_on)
               || registers_.comreg.is(SmpcRegisters::CommandRegister::cd_off)
               || registers_.comreg.is(SmpcRegisters::CommandRegister::smpc_memory_setting)) {
        constexpr auto duration   = micro(40);
        command_remaining_cycles_ = calculateCyclesNumber(duration);
        return;

    } else if (registers_.comreg.is(SmpcRegisters::CommandRegister::reset_entire_system)
               || registers_.comreg.is(SmpcRegisters::CommandRegister::clock_change_320)
               || registers_.comreg.is(SmpcRegisters::CommandRegister::clock_change_352)) {
        // Alpha is fixed to 0
        constexpr auto duration   = milli(100);
        command_remaining_cycles_ = calculateCyclesNumber(duration);
        return;
    } else if (registers_.comreg.is(SmpcRegisters::CommandRegister::time_setting)) {
        constexpr auto duration   = micro(70);
        command_remaining_cycles_ = calculateCyclesNumber(duration);
        return;
    } else if (registers_.comreg.is(SmpcRegisters::CommandRegister::interrupt_back)) {
        // Values are from previous Saturnin version, not sure how accurate they are ...
        constexpr auto intback_duration = micro(50);
        constexpr auto normal_duration  = micro(1500);
        command_remaining_cycles_
            = is_intback_processing_ ? calculateCyclesNumber(intback_duration) : calculateCyclesNumber(normal_duration);
        return;
    }

    Log::warning(Logger::smpc, tr("Unknown SMPC command '{}'"), registers_.comreg.data());
}

void Smpc::executeCommand() {
    std::string ts = modules_.config()->readValue(core::AccessKeys::cfg_rendering_tv_standard);

    if (registers_.comreg.is(SmpcRegisters::CommandRegister::master_sh2_on)) {
        is_master_sh2_on_ = true;
        Log::debug(Logger::smpc, tr("-=Master SH2 ON=- command executed"));
        registers_.oreg[31] = registers_.comreg.data();
        registers_.sf.clr(SmpcRegisters::StatusFlag::sf);
        return;

    } else if (registers_.comreg.is(SmpcRegisters::CommandRegister::slave_sh2_on)) {
        is_slave_sh2_on_ = true;
        modules_.slaveSh2()->powerOnReset();
        Log::debug(Logger::smpc, tr("-=Slave SH2 ON=- command executed"));
        registers_.oreg[31] = registers_.comreg.data();
        registers_.sf.clr(SmpcRegisters::StatusFlag::sf);
        return;

    } else if (registers_.comreg.is(SmpcRegisters::CommandRegister::slave_sh2_off)) {
        is_slave_sh2_on_ = false;
        Log::debug(Logger::smpc, tr("-=Slave SH2 OFF=- command executed"));
        registers_.oreg[31] = registers_.comreg.data();
        registers_.sf.clr(SmpcRegisters::StatusFlag::sf);
        return;

    } else if (registers_.comreg.is(SmpcRegisters::CommandRegister::sound_on)) {
        is_sound_on_ = true;
        modules_.scsp()->reset();
        Log::debug(Logger::smpc, tr("-=Sound ON=- command executed"));
        registers_.oreg[31] = registers_.comreg.data();
        registers_.sf.clr(SmpcRegisters::StatusFlag::sf);
        return;

    } else if (registers_.comreg.is(SmpcRegisters::CommandRegister::sound_off)) {
        is_sound_on_ = false;
        // emulator_context_->scsp()->setSound(false);
        Log::debug(Logger::smpc, tr("-=Sound OFF=- command executed"));
        registers_.oreg[31] = registers_.comreg.data();
        registers_.sf.clr(SmpcRegisters::StatusFlag::sf);
        return;

    } else if (registers_.comreg.is(SmpcRegisters::CommandRegister::cd_on)) {
        is_cd_on_ = true;
        Log::debug(Logger::smpc, tr("-=CD ON=- command executed"));
        registers_.oreg[31] = registers_.comreg.data();
        registers_.sf.clr(SmpcRegisters::StatusFlag::sf);
        return;

    } else if (registers_.comreg.is(SmpcRegisters::CommandRegister::cd_on)) {
        is_cd_on_ = false;
        Log::debug(Logger::smpc, tr("-=CD OFF=- command executed"));
        registers_.oreg[31] = registers_.comreg.data();
        registers_.sf.clr(SmpcRegisters::StatusFlag::sf);
        return;

    } else if (registers_.comreg.is(SmpcRegisters::CommandRegister::reset_entire_system)) {
        modules_.masterSh2()->powerOnReset();
        modules_.slaveSh2()->powerOnReset();
        // emulator_context_->scsp()->reset();
        Log::debug(Logger::smpc, tr("-=Reset Entire System=- command executed"));
        registers_.oreg[31] = registers_.comreg.data();
        registers_.sf.clr(SmpcRegisters::StatusFlag::sf);
        return;

    } else if (registers_.comreg.is(SmpcRegisters::CommandRegister::clock_change_320)
               || registers_.comreg.is(SmpcRegisters::CommandRegister::clock_change_352)) {
        // -> VDP1, VDP2, SCU, SCSP : default power on value
        // -> Master SH2 : unknown
        // -> Slave SH2 : OFF
        // -> CD block kept
        // -> Work RAM kept
        // -> VRAM emptied
        switch (modules_.config()->getTvStandard(ts)) {
            using enum video::TvStandard;
            case pal:
                clock_ = registers_.comreg.is(SmpcRegisters::CommandRegister::clock_change_320) ? SystemClock::pal_320
                                                                                                : SystemClock::pal_352;
                break;
            case ntsc:
                clock_ = registers_.comreg.is(SmpcRegisters::CommandRegister::clock_change_320) ? SystemClock::ntsc_320
                                                                                                : SystemClock::ntsc_352;
            default: Log::warning(Logger::smpc, tr("Could not set system clock !")); clock_ = SystemClock::not_set;
        }
        is_slave_sh2_on_ = false;
        modules_.slaveSh2()->powerOnReset();
        modules_.cdrom()->refreshPeriod();
        modules_.vdp2()->onSystemClockUpdate();
        if (registers_.comreg.is(SmpcRegisters::CommandRegister::clock_change_320)) {
            is_horizontal_res_352 = false;
            Log::debug(Logger::smpc, tr("-=Clock Change 320 Mode=- command executed"));
        }
        if (registers_.comreg.is(SmpcRegisters::CommandRegister::clock_change_352)) {
            is_horizontal_res_352 = true;
            Log::debug(Logger::smpc, tr("-=Clock Change 352 Mode=- command executed"));
        }
        registers_.oreg[31] = registers_.comreg.data();
        registers_.sf.clr(SmpcRegisters::StatusFlag::sf);
        return;

    } else if (registers_.comreg.is(SmpcRegisters::CommandRegister::nmi_request)) {
        modules_.scu()->generateInterrupt(interrupt_source::nmi);
        Log::debug(Logger::smpc, tr("-=NMI Request=- command executed"));
        registers_.oreg[31] = registers_.comreg.data();
        registers_.sf.clr(SmpcRegisters::StatusFlag::sf);
        return;

    } else if (registers_.comreg.is(SmpcRegisters::CommandRegister::reset_enable)) {
        is_soft_reset_allowed_ = true;
        Log::debug(Logger::smpc, tr("-=Reset Enable=- command executed"));
        registers_.oreg[31] = registers_.comreg.data();
        registers_.sf.clr(SmpcRegisters::StatusFlag::sf);
        return;

    } else if (registers_.comreg.is(SmpcRegisters::CommandRegister::reset_disable)) {
        is_soft_reset_allowed_ = false;
        Log::debug(Logger::smpc, tr("-=Reset Disable=- command executed"));
        registers_.oreg[31] = registers_.comreg.data();
        registers_.sf.clr(SmpcRegisters::StatusFlag::sf);
        return;

    } else if (registers_.comreg.is(SmpcRegisters::CommandRegister::interrupt_back)) {
        executeIntback();
        return;

    } else if (registers_.comreg.is(SmpcRegisters::CommandRegister::smpc_memory_setting)) {
        for (u8 i = 0; i < 4; ++i) {
            smem_[i] = registers_.ireg[i].data();
        }
        Log::debug(Logger::smpc, tr("-=SMPC Memory Setting=- command executed"));
        registers_.oreg[31] = registers_.comreg.data();
        registers_.sf.clr(SmpcRegisters::StatusFlag::sf);
        return;

    } else if (registers_.comreg.is(SmpcRegisters::CommandRegister::time_setting)) {
        Log::debug(Logger::smpc, tr("-=Time Setting=- command executed"));
        registers_.oreg[31] = registers_.comreg.data();
        registers_.sf.clr(SmpcRegisters::StatusFlag::sf);
        return;
    }

    Log::warning(Logger::smpc, tr("Unknown SMPC command '{}'"), registers_.comreg.data());
}

void Smpc::executeIntback() {
    if (is_intback_processing_) {
        getPeripheralData();
        registers_.sr.clr(SmpcRegisters::StatusRegister::pdl);
        registers_.sf.clr(SmpcRegisters::StatusFlag::sf);
        registers_.oreg[31] = SmpcRegisters::OutputRegister::interrupt_back;
        Log::debug(Logger::smpc, tr("Interrupt request"));
        modules_.scu()->generateInterrupt(interrupt_source::system_manager);
        return;
    }

    Log::debug(Logger::smpc, tr("INTBACK started"));
    registers_.oreg[31] = {};

    auto is_status_returned          = registers_.ireg[0].is(SmpcRegisters::InputRegister::ireg0_status_acquisition);
    auto is_peripheral_data_returned = registers_.ireg[1].is(SmpcRegisters::InputRegister::ireg1_peripheral_data_returned);

    if (is_status_returned) {
        getStatus();

        if (is_peripheral_data_returned) {
            registers_.sr.set(SmpcRegisters::StatusRegister::pdl);
            is_intback_processing_ = true;
        } else {
            is_intback_processing_ = false;
        }
    } else {
        // Peripheral data only.
        getPeripheralData();
        registers_.sr.set(SmpcRegisters::StatusRegister::pdl);
        is_intback_processing_ = true;
    }

    Log::debug(Logger::smpc, tr("Interrupt request"));
    modules_.scu()->generateInterrupt(interrupt_source::system_manager);

    registers_.sf.clr(SmpcRegisters::StatusFlag::sf);
};

void Smpc::getStatus() {
    Log::debug(Logger::smpc, tr("INTBACK returning status data"));
    registers_.sr = {};
    registers_.sr.clr(SmpcRegisters::StatusRegister::bit_7);
    registers_.sr.set(SmpcRegisters::StatusRegister::bit_6);

    if (registers_.ireg[1].is(SmpcRegisters::InputRegister::ireg1_peripheral_data_returned)) {
        registers_.sr.set(SmpcRegisters::StatusRegister::pde);
    } else {
        registers_.sr.clr(SmpcRegisters::StatusRegister::pde);
    }

    for (u8 i = 0; i < output_registers_number; ++i) {
        registers_.oreg[i] = {};
    }
    if (is_soft_reset_allowed_) {
        registers_.oreg[0].clr(SmpcRegisters::OutputRegister::oreg0_reset_disable);
    } else {
        registers_.oreg[0].set(SmpcRegisters::OutputRegister::oreg0_reset_disable);
    }

    const bool is_time_set = modules_.config()->readValue(core::AccessKeys::cfg_global_set_time);
    registers_.oreg[0].upd(SmpcRegisters::OutputRegister::setTime(is_time_set));

    auto rtc           = getRtcTime();
    registers_.oreg[1] = rtc.getUpperYear();
    registers_.oreg[2] = rtc.getLowerYear();
    registers_.oreg[3] = rtc.getDayMonth();
    registers_.oreg[4] = rtc.getDays();
    registers_.oreg[5] = rtc.getHours();
    registers_.oreg[6] = rtc.getMinutes();
    registers_.oreg[7] = rtc.getSeconds();
    registers_.oreg[8] = {}; // No cartridge code handling for now

    std::string ac     = modules_.config()->readValue(core::AccessKeys::cfg_global_area_code);
    registers_.oreg[9] = uti::toUnderlying(modules_.config()->getAreaCode(ac));

    registers_.oreg[10].clr(SmpcRegisters::OutputRegister::bit_7);
    registers_.oreg[10].upd(SmpcRegisters::OutputRegister::horizontalRes(is_horizontal_res_352));
    registers_.oreg[10].set(SmpcRegisters::OutputRegister::bit_5);
    registers_.oreg[10].set(SmpcRegisters::OutputRegister::bit_4);
    registers_.oreg[10].upd(SmpcRegisters::OutputRegister::softResetAllowed(is_soft_reset_allowed_));
    registers_.oreg[10].set(SmpcRegisters::OutputRegister::bit_2);
    registers_.oreg[10].clr(SmpcRegisters::OutputRegister::bit_1); // SYSRES is never triggered by software
    registers_.oreg[10].upd(SmpcRegisters::OutputRegister::soundOn(is_sound_on_));

    //  oreg_[index_11][bit_6] = is_cd_on_;
    registers_.oreg[11].clr(SmpcRegisters::OutputRegister::bit_6);

    registers_.oreg[12] = smem_[0];
    registers_.oreg[13] = smem_[1];
    registers_.oreg[14] = smem_[2];
    registers_.oreg[15] = smem_[3];
};

void Smpc::getPeripheralData() {
    Log::debug(Logger::smpc, tr("INTBACK returning peripheral data"));

    // SMPC Peripheral result :
    // [SR]
    // [Port 1 Data] (1)
    // [Port 2 Data] (1)

    // (1) Port x Data structure :
    // [Port x Status] (2)
    // [Peripheral 1 Data] (3)
    // [Peripheral 2 Data] (3)
    // ...
    // [Peripheral m Data] (3)
    // 0 < m < 15

    // (2) Port x Status structure :
    // [Multitap ID | Num. of connectors]

    // (3) Peripheral x Data structure
    // [Saturn Peripheral ID] (4)
    // [Extension Data Size]
    // [Peripheral x 1st Data]
    // [Peripheral x 2nd Data]
    // ...
    // [Peripheral x nth Data]
    // n max is 255

    // (4) Saturn Peripheral ID structure :
    // [Saturn Peripheral Type | Data Size]

    // SR page 66
    registers_.sr = {};
    registers_.sr.set(SmpcRegisters::StatusRegister::bit_7);

    registers_.sr.upd(SmpcRegisters::StatusRegister::p2md(registers_.ireg[1] >> SmpcRegisters::InputRegister::p2md_shft));
    registers_.sr.upd(SmpcRegisters::StatusRegister::p1md(registers_.ireg[1] >> SmpcRegisters::InputRegister::p1md_shft));

    for (u32 i = 0; i < output_registers_number; ++i) {
        registers_.oreg[i] = {};
    }

    switch (toEnum<PortMode>(registers_.ireg[1] >> SmpcRegisters::InputRegister::p1md_shft)) {
        using enum PortMode;
        case mode_0_byte: break; // no data returned
        case mode_15_byte:
        case mode_255_byte: {
            // no difference between 15 byte and 255 byte for now
            auto port_1_data = PortData{};
            switch (port_1_status_) {
                case PortStatus::not_connected: {
                    full_peripheral_data_table_.emplace_back(uti::toUnderlying(port_1_status_));
                    full_peripheral_data_table_.emplace_back(0xff);
                    full_peripheral_data_table_.emplace_back(0xff);
                    full_peripheral_data_table_.emplace_back(0xff);
                    break;
                }
                case PortStatus::direct_connection: {
                    const auto pad_data = generatePeripheralData(SaturnPeripheralId::saturn_standard_pad);
                    full_peripheral_data_table_.emplace_back(uti::toUnderlying(port_1_status_));

                    const auto local_data_size
                        = u8{(pad_data.data_size != 0) ? pad_data.data_size : pad_data.extension_data_size};
                    full_peripheral_data_table_.emplace_back(local_data_size);
                    full_peripheral_data_table_.reserve(full_peripheral_data_table_.size() + local_data_size);
                    full_peripheral_data_table_.insert(std::end(full_peripheral_data_table_),
                                                       std::begin(pad_data.peripheral_data_table),
                                                       std::end(pad_data.peripheral_data_table));
                    break;
                }
                default: {
                    Log::warning(Logger::smpc, tr("Port Status not implemented"));
                }
            }
            break;
        }
        default: Log::warning(Logger::smpc, tr("Port Status reserved"));
    }

    switch (toEnum<PortMode>(registers_.ireg[1] >> SmpcRegisters::InputRegister::p2md_shft)) {
        using enum PortMode;
        case mode_0_byte: break; // no data returned
        case mode_15_byte:
        case mode_255_byte: // no difference between 15 byte and 255 byte for now
        {
            auto port_2_data = PortData{};
            switch (port_2_status_) {
                using enum PortStatus;
                case not_connected: {
                    full_peripheral_data_table_.emplace_back(uti::toUnderlying(port_2_status_));
                    full_peripheral_data_table_.emplace_back(0xff);
                    full_peripheral_data_table_.emplace_back(0xff);
                    full_peripheral_data_table_.emplace_back(0xff);
                    break;
                }
                case direct_connection: {
                    const auto pad_data = generatePeripheralData(SaturnPeripheralId::saturn_standard_pad);
                    full_peripheral_data_table_.emplace_back(uti::toUnderlying(port_2_status_));
                    const auto local_data_size
                        = u8{(pad_data.data_size != 0) ? pad_data.data_size : pad_data.extension_data_size};
                    full_peripheral_data_table_.reserve(full_peripheral_data_table_.size() + local_data_size);
                    full_peripheral_data_table_.insert(std::end(full_peripheral_data_table_),
                                                       std::begin(pad_data.peripheral_data_table),
                                                       std::end(pad_data.peripheral_data_table));
                    break;
                }
                default: {
                    Log::warning(Logger::smpc, tr("Port Status not implemented"));
                }
            }
            break;
        }
        default: Log::warning(Logger::smpc, tr("Port Status reserved"));
    }

    // Checking if there's more data to send than existing OREG registers
    if (full_peripheral_data_table_.size() > output_registers_number) {
        for (u32 i = 0; i < output_registers_number; ++i) {
            registers_.oreg[i] = full_peripheral_data_table_[i];
        }
        full_peripheral_data_table_.erase(full_peripheral_data_table_.begin(),
                                          full_peripheral_data_table_.begin() + output_registers_number);

        registers_.sr.set(SmpcRegisters::StatusRegister::pde);
    } else {
        for (u32 i = 0; i < full_peripheral_data_table_.size(); ++i) {
            registers_.oreg[i] = full_peripheral_data_table_[i];
        }
        full_peripheral_data_table_.clear();

        registers_.sr.clr(SmpcRegisters::StatusRegister::pde);
    }

} // namespace saturnin::core

auto Smpc::generatePeripheralData(const SaturnPeripheralId id) -> PeripheralData {
    auto peripheral_data                 = PeripheralData{};
    peripheral_data.saturn_peripheral_id = id;
    peripheral_data.data_size            = uti::toUnderlying(id) & 0x0F;
    if (peripheral_data.data_size != 0) { peripheral_data.extension_data_size = 0; }

    const auto p1 = getSaturnPeripheralMapping().player_1;
    switch (id) {
        using enum SaturnPeripheralId;
        case saturn_standard_pad: {
            auto first_data = SaturnPadData::SaturnStandardPad1stDataType{};
            first_data      = u8_max;
            if (isKeyPressed(p1.direction_right, openglWindow())) {
                first_data.clr(SaturnPadData::SaturnStandardPad1stData::direction_right);
            }
            if (isKeyPressed(p1.direction_left, openglWindow())) {
                first_data.clr(SaturnPadData::SaturnStandardPad1stData::direction_left);
            }
            if (isKeyPressed(p1.direction_down, openglWindow())) {
                first_data.clr(SaturnPadData::SaturnStandardPad1stData::direction_down);
            }
            if (isKeyPressed(p1.direction_up, openglWindow())) {
                first_data.clr(SaturnPadData::SaturnStandardPad1stData::direction_up);
            }
            if (isKeyPressed(p1.button_start, openglWindow())) {
                first_data.clr(SaturnPadData::SaturnStandardPad1stData::button_start);
            }
            if (isKeyPressed(p1.button_a, openglWindow())) { first_data.clr(SaturnPadData::SaturnStandardPad1stData::button_a); }
            if (isKeyPressed(p1.button_c, openglWindow())) { first_data.clr(SaturnPadData::SaturnStandardPad1stData::button_c); }
            if (isKeyPressed(p1.button_b, openglWindow())) { first_data.clr(SaturnPadData::SaturnStandardPad1stData::button_b); }
            peripheral_data.peripheral_data_table.push_back(first_data.data());

            auto second_data = SaturnPadData::SaturnStandardPad2ndDataType{};
            second_data      = u8_max;
            if (isKeyPressed(p1.button_shoulder_right, openglWindow())) {
                second_data.clr(SaturnPadData::SaturnStandardPad2ndData::button_shoulder_right);
            }
            if (isKeyPressed(p1.button_x, openglWindow())) { second_data.clr(SaturnPadData::SaturnStandardPad2ndData::button_x); }
            if (isKeyPressed(p1.button_y, openglWindow())) { second_data.clr(SaturnPadData::SaturnStandardPad2ndData::button_y); }
            if (isKeyPressed(p1.button_z, openglWindow())) { second_data.clr(SaturnPadData::SaturnStandardPad2ndData::button_z); }
            if (isKeyPressed(p1.button_shoulder_left, openglWindow())) {
                second_data.clr(SaturnPadData::SaturnStandardPad2ndData::button_shoulder_left);
            }
            peripheral_data.peripheral_data_table.push_back(second_data.data());
            break;
        }
        default: {
            Log::warning(Logger::smpc, tr("Peripheral not implemented"));
        }
    }

    return peripheral_data;
}

auto Smpc::read(const u32 addr) -> u8 {
    switch (addr) {
        case status_register:
            if (modules_.context()->hardwareMode() == HardwareMode::stv) {
                constexpr auto default_stv_data = u8{0xcf};
                return default_stv_data;
            }
            return registers_.sr.data();
        case status_flag: return registers_.sf.data();
        case output_register_0: return registers_.oreg[0].data();
        case output_register_1: return registers_.oreg[1].data();
        case output_register_2: return registers_.oreg[2].data();
        case output_register_3: return registers_.oreg[3].data();
        case output_register_4: return registers_.oreg[4].data();
        case output_register_5: return registers_.oreg[5].data();
        case output_register_6: return registers_.oreg[6].data();
        case output_register_7: return registers_.oreg[7].data();
        case output_register_8: return registers_.oreg[8].data();
        case output_register_9: return registers_.oreg[9].data();
        case output_register_10: return registers_.oreg[10].data();
        case output_register_11: return registers_.oreg[11].data();
        case output_register_12: return registers_.oreg[12].data();
        case output_register_13: return registers_.oreg[13].data();
        case output_register_14: return registers_.oreg[14].data();
        case output_register_15: return registers_.oreg[15].data();
        case output_register_16: return registers_.oreg[16].data();
        case output_register_17: return registers_.oreg[17].data();
        case output_register_18: return registers_.oreg[18].data();
        case output_register_19: return registers_.oreg[19].data();
        case output_register_20: return registers_.oreg[20].data();
        case output_register_21: return registers_.oreg[21].data();
        case output_register_22: return registers_.oreg[22].data();
        case output_register_23: return registers_.oreg[23].data();
        case output_register_24: return registers_.oreg[24].data();
        case output_register_25: return registers_.oreg[25].data();
        case output_register_26: return registers_.oreg[26].data();
        case output_register_27: return registers_.oreg[27].data();
        case output_register_28: return registers_.oreg[28].data();
        case output_register_29: return registers_.oreg[29].data();
        case output_register_30: return registers_.oreg[30].data();
        case output_register_31: return registers_.oreg[31].data();
        case port_data_register_1:
            if (modules_.context()->hardwareMode() == HardwareMode::stv) {
                constexpr auto default_stv_data = u8{0xff};
                return default_stv_data;
            }
            return registers_.pdr1.shifted(SmpcRegisters::PortDataRegister::pdr_shft);
        case port_data_register_2:
            if (modules_.context()->hardwareMode() == HardwareMode::stv) {
                registers_.pdr2 = 0;

                // const auto controls = getStvPeripheralMapping().board_controls;
                // controls.test_switch;
                //    if (glfwGetKey(glfwGetCurrentContext(), uti::toUnderlying(stv_mapping_.board_controls.test_switch)) ==
                //    GLFW_PRESS) {

                //    }
            }
            registers_.pdr2 = 0xFF;

            return registers_.pdr2.shifted(SmpcRegisters::PortDataRegister::pdr_shft);

        default: return 0;
    }
}

auto Smpc::rawRead(const u32 addr) -> u8 {
    switch (addr) {
        case input_register_0: return registers_.ireg[0].data();
        case input_register_1: return registers_.ireg[1].data();
        case input_register_2: return registers_.ireg[2].data();
        case input_register_3: return registers_.ireg[3].data();
        case input_register_4: return registers_.ireg[4].data();
        case input_register_5: return registers_.ireg[5].data();
        case input_register_6: return registers_.ireg[6].data();
        default: return read(addr);
    }
}

void Smpc::write(const u32 addr, const u8 data) {
    switch (addr) {
        case command_register:
            registers_.comreg = data;
            setCommandDuration();
            break;
        case status_flag: registers_.sf = data; break;
        case input_register_0: {
            auto old_ireg0     = registers_.ireg[0];
            registers_.ireg[0] = data;
            if (is_intback_processing_) {
                if (registers_.ireg[0].is(SmpcRegisters::InputRegister::ireg0_break_request)) {
                    Log::debug(Logger::smpc, tr("INTBACK break request"));
                    registers_.sr.clr(SmpcRegisters::StatusRegister::upper_nibble);
                    is_intback_processing_ = false;
                    break;
                }

                auto old_continue = old_ireg0 >> SmpcRegisters::InputRegister::continue_request_shft;
                auto new_continue = registers_.ireg[0] >> SmpcRegisters::InputRegister::continue_request_shft;
                if (new_continue != old_continue) {
                    Log::debug(Logger::smpc, tr("INTBACK continue request"));
                    setCommandDuration();
                    registers_.sf.set(SmpcRegisters::StatusFlag::sf);
                    break;
                }
                is_intback_processing_ = false;
            }
            break;
        }
        case input_register_1: registers_.ireg[1] = data; break;
        case input_register_2: registers_.ireg[2] = data; break;
        case input_register_3: registers_.ireg[3] = data; break;
        case input_register_4: registers_.ireg[4] = data; break;
        case input_register_5: registers_.ireg[5] = data; break;
        case input_register_6: registers_.ireg[6] = data; break;
        case port_data_register_1: registers_.pdr1.upd(SmpcRegisters::PortDataRegister::pdr(data)); break;
        case port_data_register_2:
            if (modules_.context()->hardwareMode() == HardwareMode::stv) {
                constexpr auto sound_status = u8{0x10};
                if ((data & sound_status) > 0) {
                    Log::debug(Logger::smpc, tr("-=Sound OFF=-"));

                    is_sound_on_ = false;
                } else {
                    Log::debug(Logger::smpc, tr("-=Sound ON=-"));

                    is_sound_on_ = true;
                    modules_.scsp()->reset();
                }
            }
            registers_.pdr2.upd(SmpcRegisters::PortDataRegister::pdr(data));
            break;
        case data_direction_register_1: registers_.ddr1.upd(SmpcRegisters::DataDirectionRegister::ddr(data)); break;
        case data_direction_register_2: registers_.ddr2.upd(SmpcRegisters::DataDirectionRegister::ddr(data)); break;
        case io_select_register: registers_.iosel = data; break;
        case external_latch_register: registers_.exle = data; break;
        default: break;
    }
}

/* static */
auto Smpc::listAvailableKeys() -> std::vector<PeripheralKey> {
    auto v = std::vector<PeripheralKey>{};
    for (std::pair<PeripheralKey, const std::string&> key : keyboard_layout) {
        v.push_back(key.first);
    }
    return v;
}

void Smpc::initializePeripheralMappings() {
    saturn_mapping_.player_1.fromConfig(
        modules_.config()->readPeripheralConfiguration(core::AccessKeys::cfg_controls_saturn_player_1));
    saturn_mapping_.player_2.fromConfig(
        modules_.config()->readPeripheralConfiguration(core::AccessKeys::cfg_controls_saturn_player_2));
    stv_mapping_.board_controls.fromConfig(
        modules_.config()->readPeripheralConfiguration(core::AccessKeys::cfg_controls_stv_board));
    stv_mapping_.player_1.fromConfig(modules_.config()->readPeripheralConfiguration(core::AccessKeys::cfg_controls_stv_player_1));
    stv_mapping_.player_2.fromConfig(modules_.config()->readPeripheralConfiguration(core::AccessKeys::cfg_controls_stv_player_2));
}

auto Smpc::getSaturnPeripheralMapping() const -> SaturnPeripheralMapping { return saturn_mapping_; }

auto Smpc::getStvPeripheralMapping() const -> StvPeripheralMapping { return stv_mapping_; }

void Smpc::initialize() {
    Log::info(Logger::smpc, tr("SMPC initialization"));
    initializeRegisterNameMap();
    reset();
}

void Smpc::run(const s8 cycles) {
    if (command_remaining_cycles_ > 0) {
        command_remaining_cycles_ -= cycles;
        if (command_remaining_cycles_ <= 0) { executeCommand(); }
    }
}

auto Smpc::openglWindow() const -> GLFWwindow* { return modules_.context()->openglWindow(); };

void Smpc::addToRegisterNameMap(const u32 addr, const std::string& name) {
    address_to_name_.insert(AddressToNameMap::value_type(addr, name));
}

void Smpc::initializeRegisterNameMap() {
    addToRegisterNameMap(command_register, "Command Register");
    addToRegisterNameMap(status_register, "Status Register");
    addToRegisterNameMap(status_flag, "Status Flag");
    addToRegisterNameMap(input_register_0, "IREG0");
    addToRegisterNameMap(input_register_1, "IREG1");
    addToRegisterNameMap(input_register_2, "IREG2");
    addToRegisterNameMap(input_register_3, "IREG3");
    addToRegisterNameMap(input_register_4, "IREG4");
    addToRegisterNameMap(input_register_5, "IREG5");
    addToRegisterNameMap(input_register_6, "IREG6");
    addToRegisterNameMap(output_register_0, "OREG0");
    addToRegisterNameMap(output_register_1, "OREG1");
    addToRegisterNameMap(output_register_2, "OREG2");
    addToRegisterNameMap(output_register_3, "OREG3");
    addToRegisterNameMap(output_register_4, "OREG4");
    addToRegisterNameMap(output_register_5, "OREG5");
    addToRegisterNameMap(output_register_6, "OREG6");
    addToRegisterNameMap(output_register_7, "OREG7");
    addToRegisterNameMap(output_register_8, "OREG8");
    addToRegisterNameMap(output_register_9, "OREG9");
    addToRegisterNameMap(output_register_10, "OREG10");
    addToRegisterNameMap(output_register_11, "OREG11");
    addToRegisterNameMap(output_register_12, "OREG12");
    addToRegisterNameMap(output_register_13, "OREG13");
    addToRegisterNameMap(output_register_14, "OREG14");
    addToRegisterNameMap(output_register_15, "OREG15");
    addToRegisterNameMap(output_register_16, "OREG16");
    addToRegisterNameMap(output_register_17, "OREG17");
    addToRegisterNameMap(output_register_18, "OREG18");
    addToRegisterNameMap(output_register_19, "OREG19");
    addToRegisterNameMap(output_register_20, "OREG20");
    addToRegisterNameMap(output_register_21, "OREG21");
    addToRegisterNameMap(output_register_22, "OREG22");
    addToRegisterNameMap(output_register_23, "OREG23");
    addToRegisterNameMap(output_register_24, "OREG24");
    addToRegisterNameMap(output_register_25, "OREG25");
    addToRegisterNameMap(output_register_26, "OREG26");
    addToRegisterNameMap(output_register_27, "OREG27");
    addToRegisterNameMap(output_register_28, "OREG28");
    addToRegisterNameMap(output_register_29, "OREG29");
    addToRegisterNameMap(output_register_30, "OREG30");
    addToRegisterNameMap(output_register_31, "OREG31");
}

auto getKeyName(const PeripheralKey pk) -> std::string { return keyboard_layout.at(pk); }

auto getRtcTime() -> RtcTime {
    using namespace date;
    using namespace std::chrono;
    namespace chr = std::chrono;

    const auto tp      = chr::system_clock::now();
    const auto dp      = floor<days>(tp);    // Day part of time_point
    const auto time    = make_time(tp - dp); // Time part of time_point
    const auto today   = chr::year_month_day(dp);
    const auto weekday = chr::year_month_weekday(dp);

    auto       rtc      = RtcTime{};
    const auto year     = static_cast<int>(today.year());
    const auto year_bcd = uti::dec2bcd(year);
    rtc.year_1000_bcd   = std::bitset<4>((year_bcd >> displacement_12) & bitmask_0F);
    rtc.year_100_bcd    = std::bitset<4>((year_bcd >> displacement_8) & bitmask_0F);
    rtc.year_10_bcd     = std::bitset<4>((year_bcd >> displacement_4) & bitmask_0F);
    rtc.year_1_bcd      = std::bitset<4>(year_bcd & bitmask_0F);

    rtc.month_hex = static_cast<unsigned>(today.month());
    rtc.day_hex   = weekday.weekday().c_encoding();

    // const auto month     = static_cast<unsigned>(today.month());
    // const auto month_bcd = uti::dec2bcd(month);
    // rtc.day_10_bcd       = std::bitset<4>((month_bcd >> displacement_4) & bitmask_0F);
    // rtc.day_1_bcd        = std::bitset<4>(month_bcd & bitmask_0F);
    const auto day     = static_cast<unsigned>(today.day());
    const auto day_bcd = uti::dec2bcd(day);
    rtc.day_10_bcd     = std::bitset<4>((day_bcd >> displacement_4) & bitmask_0F);
    rtc.day_1_bcd      = std::bitset<4>(day_bcd & bitmask_0F);

    const auto hour     = time.hours().count();
    const auto hour_bcd = uti::dec2bcd(hour);
    rtc.hour_10_bcd     = std::bitset<4>((hour_bcd >> displacement_4) & bitmask_0F);
    rtc.hour_1_bcd      = std::bitset<4>(hour_bcd & bitmask_0F);

    const auto minute     = time.minutes().count();
    const auto minute_bcd = uti::dec2bcd(minute);
    rtc.minute_10_bcd     = std::bitset<4>((minute_bcd >> displacement_4) & bitmask_0F);
    rtc.minute_1_bcd      = std::bitset<4>(minute_bcd & bitmask_0F);

    const auto second     = static_cast<u16>(time.seconds().count());
    const auto second_bcd = uti::dec2bcd(second);
    rtc.second_10_bcd     = std::bitset<4>((second_bcd >> displacement_4) & bitmask_0F);
    rtc.second_1_bcd      = std::bitset<4>(second_bcd & bitmask_0F);

    return rtc;
}

auto isKeyPressed(const PeripheralKey pk, GLFWwindow* window) -> bool {
    return glfwGetKey(window, uti::toUnderlying(pk)) == GLFW_PRESS;
}

} // namespace saturnin::core
