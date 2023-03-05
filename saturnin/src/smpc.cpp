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

// using sound::Scsp;

using MapKeyboardLayout = std::map<PeripheralKey, const std::string>;
auto keyboard_layout    = MapKeyboardLayout{{PeripheralKey::key_space, "space"},
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

auto SaturnDigitalPad::toConfig(const PeripheralLayout layout) -> std::vector<PeripheralKey> {
    switch (layout) {
        using enum PeripheralLayout;
        case empty_layout:
            return std::vector<PeripheralKey>{PeripheralKey::key_unknown,
                                              PeripheralKey::key_unknown,
                                              PeripheralKey::key_unknown,
                                              PeripheralKey::key_unknown,
                                              PeripheralKey::key_unknown,
                                              PeripheralKey::key_unknown,
                                              PeripheralKey::key_unknown,
                                              PeripheralKey::key_unknown,
                                              PeripheralKey::key_unknown,
                                              PeripheralKey::key_unknown,
                                              PeripheralKey::key_unknown,
                                              PeripheralKey::key_unknown,
                                              PeripheralKey::key_unknown};
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
            return std::vector<PeripheralKey>{PeripheralKey::key_left,
                                              PeripheralKey::key_right,
                                              PeripheralKey::key_up,
                                              PeripheralKey::key_down,
                                              PeripheralKey::key_z,
                                              PeripheralKey::key_e,
                                              PeripheralKey::key_s,
                                              PeripheralKey::key_d,
                                              PeripheralKey::key_f,
                                              PeripheralKey::key_x,
                                              PeripheralKey::key_c,
                                              PeripheralKey::key_v,
                                              PeripheralKey::key_enter};
    }
}
void SaturnDigitalPad::fromConfig(std::vector<PeripheralKey> config) {
    constexpr auto param_number = u8{13};
    if (config.size() != param_number) {
        Log::warning(Logger::smpc, tr("Incorrect Saturn pad data"));
        const auto v   = SaturnDigitalPad().toConfig(PeripheralLayout::empty_layout);
        auto       pad = SaturnDigitalPad{};
        pad.fromConfig(v);
        *this = pad;
        return;
    }

    direction_left        = config[index_0];
    direction_right       = config[index_1];
    direction_up          = config[index_2];
    direction_down        = config[index_3];
    button_shoulder_left  = config[index_4];
    button_shoulder_right = config[index_5];
    button_a              = config[index_6];
    button_b              = config[index_7];
    button_c              = config[index_8];
    button_x              = config[index_9];
    button_y              = config[index_10];
    button_z              = config[index_11];
    button_start          = config[index_12];
}

auto StvPlayerControls::toConfig(const PeripheralLayout layout) -> std::vector<PeripheralKey> {
    switch (layout) {
        using enum PeripheralLayout;
        case empty_layout:
            return std::vector<PeripheralKey>{PeripheralKey::key_unknown,
                                              PeripheralKey::key_unknown,
                                              PeripheralKey::key_unknown,
                                              PeripheralKey::key_unknown,
                                              PeripheralKey::key_unknown,
                                              PeripheralKey::key_unknown,
                                              PeripheralKey::key_unknown,
                                              PeripheralKey::key_unknown};
        case current_layout:
            return std::vector<PeripheralKey>{direction_left,
                                              direction_right,
                                              direction_up,
                                              direction_down,
                                              button_1,
                                              button_2,
                                              button_3,
                                              button_4};
        default:
            return std::vector<PeripheralKey>{PeripheralKey::key_left,
                                              PeripheralKey::key_right,
                                              PeripheralKey::key_up,
                                              PeripheralKey::key_down,
                                              PeripheralKey::key_s,
                                              PeripheralKey::key_d,
                                              PeripheralKey::key_f,
                                              PeripheralKey::key_g};
    }
}

void StvPlayerControls::fromConfig(std::vector<PeripheralKey> config) {
    constexpr auto param_number = u8{8};
    if (config.size() != param_number) {
        Log::warning(Logger::smpc, tr("Incorrect ST-V player control data"));
        const auto v       = StvPlayerControls().toConfig(PeripheralLayout::empty_layout);
        auto       control = StvPlayerControls{};
        control.fromConfig(v);
        *this = control;
        return;
    }
    direction_left  = config[index_0];
    direction_right = config[index_1];
    direction_up    = config[index_2];
    direction_down  = config[index_3];
    button_1        = config[index_4];
    button_2        = config[index_5];
    button_3        = config[index_6];
    button_4        = config[index_7];
}

auto StvBoardControls::toConfig(const PeripheralLayout layout) -> std::vector<PeripheralKey> {
    switch (layout) {
        using enum PeripheralLayout;
        case empty_layout:
            return std::vector<PeripheralKey>{PeripheralKey::key_unknown,
                                              PeripheralKey::key_unknown,
                                              PeripheralKey::key_unknown,
                                              PeripheralKey::key_unknown,
                                              PeripheralKey::key_unknown,
                                              PeripheralKey::key_unknown};
        case current_layout:
            return std::vector<PeripheralKey>{service_switch, test_switch, p1_coin_switch, p2_coin_switch, p1_start, p2_start};
        default:
            return std::vector<PeripheralKey>{PeripheralKey::key_1,
                                              PeripheralKey::key_2,
                                              PeripheralKey::key_5,
                                              PeripheralKey::key_6,
                                              PeripheralKey::key_7,
                                              PeripheralKey::key_8};
    }
}

void StvBoardControls::fromConfig(std::vector<PeripheralKey> config) {
    constexpr auto param_number = u8{6};
    if (config.size() != param_number) {
        Log::warning(Logger::smpc, tr("Incorrect ST-V board control data"));
        const auto v       = StvBoardControls().toConfig(PeripheralLayout::empty_layout);
        auto       control = StvBoardControls{};
        control.fromConfig(v);
        *this = control;
        return;
    }
    service_switch = config[index_0];
    test_switch    = config[index_1];
    p1_coin_switch = config[index_2];
    p2_coin_switch = config[index_3];
    p1_start       = config[index_4];
    p2_start       = config[index_5];
}

void Smpc::reset() {
    comreg_.raw = {};
    sr_.raw     = {};
    sf_.raw     = {};
    for (u8 i = 0; i < input_registers_number; ++i) {
        ireg_[i].raw = {};
    }
    for (u8 i = 0; i < output_registers_number; ++i) {
        oreg_[i].raw = {};
    }
    pdr1_.raw  = {};
    pdr2_.raw  = {};
    ddr1_.raw  = {};
    ddr2_.raw  = {};
    iosel_.raw = {};
    exle_.raw  = {};

    for (u8 i = 0; i < 4; ++i) {
        smem_[i] = 0;
    }

    // System clock is 320 at reset.
    std::string ts = modules_.config()->readValue(core::AccessKeys::cfg_rendering_tv_standard);
    switch (modules_.config()->getTvStandard(ts)) {
        using enum video::TvStandard;
        case pal: clock_ = SystemClock::pal_320; break;
        case ntsc: clock_ = SystemClock::ntsc_320; break;
        default: Log::warning(Logger::smpc, tr("Could not set system clock !")); clock_ = SystemClock::not_set;
    }

    std::string p1c = modules_.config()->readValue(core::AccessKeys::cfg_controls_saturn_player_1_connection);
    port_1_status_  = modules_.config()->configToPortStatus(p1c);

    std::string p2c = modules_.config()->readValue(core::AccessKeys::cfg_controls_saturn_player_2_connection);
    port_2_status_  = modules_.config()->configToPortStatus(p2c);
}

auto Smpc::calculateCyclesNumber(const std::chrono::duration<double>& d) -> u32 {
    using seconds             = std::chrono::duration<double>;
    const auto cycle_duration = seconds{(static_cast<double>(1) / static_cast<double>(uti::toUnderlying(clock_)))};
    return static_cast<u32>(d / cycle_duration);
}

auto Smpc::getSystemClock() -> u32 { return uti::toUnderlying(clock_); }

auto Smpc::getRegisters() const -> const AddressToNameMap& { return address_to_name_; };

void Smpc::setCommandDuration() {
    using micro = std::chrono::duration<double, std::micro>;
    using milli = std::chrono::duration<double, std::milli>;
    switch (toEnum<SmpcCommand>(comreg_.raw)) {
        using enum SmpcCommand;
        case master_sh2_on:
        case slave_sh2_on:
        case slave_sh2_off:
        case sound_on:
        case sound_off:
        case nmi_request:
        case reset_enable:
        case reset_disable: {
            constexpr auto duration   = micro(30);
            command_remaining_cycles_ = calculateCyclesNumber(duration);
            break;
        }
        case cd_on:
        case cd_off:
        case smpc_memory_setting: {
            constexpr auto duration   = micro(40);
            command_remaining_cycles_ = calculateCyclesNumber(duration);
            break;
        }
        case reset_entire_system:
        case clock_change_320:
        case clock_change_352: {
            // Alpha is fixed to 0
            constexpr auto duration   = milli(100);
            command_remaining_cycles_ = calculateCyclesNumber(duration);
            break;
        }
        case time_setting: {
            constexpr auto duration   = micro(70);
            command_remaining_cycles_ = calculateCyclesNumber(duration);
            break;
        }
        case interrupt_back: {
            // intback_remaining_cycles_ = calculateCyclesNumber(milli(320));
            // Values are from previous Saturnin version, not sure how accurate they are ...
            constexpr auto intback_duration = micro(50);
            constexpr auto normal_duration  = micro(1500);
            command_remaining_cycles_
                = is_intback_processing_ ? calculateCyclesNumber(intback_duration) : calculateCyclesNumber(normal_duration);

            break;
        }
        default: break;
    }
}

void Smpc::executeCommand() {
    std::string ts = modules_.config()->readValue(core::AccessKeys::cfg_rendering_tv_standard);
    auto        command{toEnum<SmpcCommand>(comreg_.raw)};
    switch (command) {
        using enum SmpcCommand;
        case master_sh2_on:
            is_master_sh2_on_ = true;
            Log::debug(Logger::smpc, tr("-=Master SH2 ON=- command executed"));
            oreg_[index_31].raw = uti::toUnderlying(command);
            sf_.sf              = false;
            break;
        case slave_sh2_on:
            is_slave_sh2_on_ = true;
            modules_.slaveSh2()->powerOnReset();
            Log::debug(Logger::smpc, tr("-=Slave SH2 ON=- command executed"));
            oreg_[index_31].raw = uti::toUnderlying(command);
            sf_.sf              = false;
            break;
        case slave_sh2_off:
            is_slave_sh2_on_ = false;
            Log::debug(Logger::smpc, tr("-=Slave SH2 OFF=- command executed"));
            oreg_[index_31].raw = uti::toUnderlying(command);
            sf_.sf              = false;
            break;
        case sound_on:
            is_sound_on_ = true;
            modules_.scsp()->reset();
            Log::debug(Logger::smpc, tr("-=Sound ON=- command executed"));
            oreg_[index_31].raw = uti::toUnderlying(command);
            sf_.sf              = false;
            break;
        case sound_off:
            is_sound_on_ = false;
            // emulator_context_->scsp()->setSound(false);
            Log::debug(Logger::smpc, tr("-=Sound OFF=- command executed"));
            oreg_[index_31].raw = uti::toUnderlying(command);
            sf_.sf              = false;
            break;
        case cd_on:
            is_cd_on_ = true;
            Log::debug(Logger::smpc, tr("-=CD ON=- command executed"));
            oreg_[index_31].raw = uti::toUnderlying(command);
            sf_.sf              = false;
            break;
        case cd_off:
            is_cd_on_ = false;
            Log::debug(Logger::smpc, tr("-=CD OFF=- command executed"));
            oreg_[index_31].raw = uti::toUnderlying(command);
            sf_.sf              = false;
            break;
        case reset_entire_system:
            modules_.masterSh2()->powerOnReset();
            modules_.slaveSh2()->powerOnReset();
            // emulator_context_->scsp()->reset();
            Log::debug(Logger::smpc, tr("-=Reset Entire System=- command executed"));
            oreg_[index_31].raw = uti::toUnderlying(command);
            sf_.sf              = false;
            break;
        case clock_change_320:
        case clock_change_352:
            // -> VDP1, VDP2, SCU, SCSP : default power on value
            // -> Master SH2 : unknown
            // -> Slave SH2 : OFF
            // -> CD block kept
            // -> Work RAM kept
            // -> VRAM emptied
            switch (modules_.config()->getTvStandard(ts)) {
                using enum video::TvStandard;
                case pal:
                    clock_ = (command == SmpcCommand::clock_change_320) ? SystemClock::pal_320 : SystemClock::pal_352;
                    break;
                case ntsc: clock_ = (command == SmpcCommand::clock_change_320) ? SystemClock::ntsc_320 : SystemClock::ntsc_352;
                default: Log::warning(Logger::smpc, tr("Could not set system clock !")); clock_ = SystemClock::not_set;
            }
            is_slave_sh2_on_ = false;
            modules_.slaveSh2()->powerOnReset();
            modules_.cdrom()->refreshPeriod();
            modules_.vdp2()->onSystemClockUpdate();
            if (command == SmpcCommand::clock_change_320) {
                is_horizontal_res_352 = false;
                Log::debug(Logger::smpc, tr("-=Clock Change 320 Mode=- command executed"));
            }
            if (command == SmpcCommand::clock_change_352) {
                is_horizontal_res_352 = true;
                Log::debug(Logger::smpc, tr("-=Clock Change 352 Mode=- command executed"));
            }
            oreg_[index_31].raw = uti::toUnderlying(command);
            sf_.sf              = false;
            break;
        case nmi_request:
            modules_.scu()->generateInterrupt(interrupt_source::nmi);
            Log::debug(Logger::smpc, tr("-=NMI Request=- command executed"));
            oreg_[index_31].raw = uti::toUnderlying(command);
            sf_.sf              = false;
            break;
        case reset_enable:
            is_soft_reset_allowed_ = true;
            Log::debug(Logger::smpc, tr("-=Reset Enable=- command executed"));
            oreg_[index_31].raw = uti::toUnderlying(command);
            sf_.sf              = false;
            break;
        case reset_disable:
            is_soft_reset_allowed_ = false;
            Log::debug(Logger::smpc, tr("-=Reset Disable=- command executed"));
            oreg_[index_31].raw = uti::toUnderlying(command);
            sf_.sf              = false;
            break;
        case interrupt_back:
            executeIntback();
            // sf_.reset();
            break;
        case smpc_memory_setting:
            for (u8 i = 0; i < 4; ++i) {
                smem_[i] = ireg_[i].raw;
            }
            Log::debug(Logger::smpc, tr("-=SMPC Memory Setting=- command executed"));
            oreg_[index_31].raw = uti::toUnderlying(command);
            sf_.sf              = false;
            break;
        case time_setting:
            Log::debug(Logger::smpc, tr("-=Time Setting=- command executed"));
            oreg_[index_31].raw = uti::toUnderlying(command);
            sf_.sf              = false;
            break;
        default: Log::warning(Logger::smpc, tr("Unknown SMPC command '{}'"), uti::toUnderlying(command));
    }
}

void Smpc::executeIntback() {
    if (is_intback_processing_) {
        getPeripheralData();
        next_peripheral_return_ = PeripheralDataLocation::second_or_above_peripheral_data;
        sf_.sf                  = false;
        oreg_[index_31].raw     = uti::toUnderlying(SmpcCommand::interrupt_back);
        Log::debug(Logger::smpc, tr("Interrupt request"));
        modules_.scu()->generateInterrupt(interrupt_source::system_manager);
        return;
    }

    Log::debug(Logger::smpc, tr("INTBACK started"));
    oreg_[index_31].raw = {};

    auto is_status_returned
        = bool{toEnum<SmpcStatusAcquisition>(ireg_[index_0].ireg0_status_acquisition) == SmpcStatusAcquisition::status_returned};
    auto is_peripheral_data_returned = bool{toEnum<PeripheralDataEnable>(ireg_[index_1].ireg1_peripheral_data_enable)
                                            == PeripheralDataEnable::peripheral_data_returned};

    if (is_status_returned) {
        getStatus();

        if (is_peripheral_data_returned) {
            next_peripheral_return_ = PeripheralDataLocation::first_peripheral_data;
            is_intback_processing_  = true;
        } else {
            is_intback_processing_ = false;
        }
    } else {
        // Peripheral data only.
        getPeripheralData();
        next_peripheral_return_ = PeripheralDataLocation::first_peripheral_data;
        is_intback_processing_  = true;
    }

    Log::debug(Logger::smpc, tr("Interrupt request"));
    modules_.scu()->generateInterrupt(interrupt_source::system_manager);

    sf_.sf = false;
};

void Smpc::getStatus() {
    Log::debug(Logger::smpc, tr("INTBACK returning status data"));
    sr_.raw                          = {};
    sr_.bit_7                        = false;
    sr_.bit_6                        = true;
    auto is_peripheral_data_returned = bool{toEnum<PeripheralDataEnable>(ireg_[index_1].ireg1_peripheral_data_enable)
                                            == PeripheralDataEnable::peripheral_data_returned};
    if (is_peripheral_data_returned) {
        sr_.peripheral_data_remaining = PeripheralDataRemaining::remaining_peripheral_data;
    } else {
        sr_.peripheral_data_remaining = PeripheralDataRemaining::no_remaining_peripheral_data;
    }

    for (u8 i = 0; i < output_registers_number; ++i) {
        oreg_[i].raw = {};
    }
    if (is_soft_reset_allowed_) {
        oreg_[index_0].oreg0_reset_status = ResetStatus::enabled;
    } else {
        oreg_[index_0].oreg0_reset_status = ResetStatus::disabled;
    }

    const bool is_time_set        = modules_.config()->readValue(core::AccessKeys::cfg_global_set_time);
    const auto set_time_value     = is_time_set ? SetTime::set_time : SetTime::not_set_time;
    oreg_[index_0].oreg0_set_time = set_time_value;

    auto rtc           = getRtcTime();
    oreg_[index_1].raw = rtc.getUpperYear();
    oreg_[index_2].raw = rtc.getLowerYear();
    oreg_[index_3].raw = rtc.getDayMonth();
    oreg_[index_4].raw = rtc.getDays();
    oreg_[index_5].raw = rtc.getHours();
    oreg_[index_6].raw = rtc.getMinutes();
    oreg_[index_7].raw = rtc.getSeconds();
    oreg_[index_8].raw = {}; // No cartridge code handling for now

    std::string ac     = modules_.config()->readValue(core::AccessKeys::cfg_global_area_code);
    oreg_[index_9].raw = uti::toUnderlying(modules_.config()->getAreaCode(ac));

    oreg_[index_10].bit_7 = false;
    oreg_[index_10].bit_6 = is_horizontal_res_352;
    oreg_[index_10].bit_5 = true;
    oreg_[index_10].bit_4 = true;
    oreg_[index_10].bit_3 = is_soft_reset_allowed_;
    oreg_[index_10].bit_2 = true;
    oreg_[index_10].bit_1 = false; // SYSRES is never triggered by software
    oreg_[index_10].bit_0 = is_sound_on_;

    // oreg_[index_11].reset();
    //  oreg_[index_11][bit_6] = is_cd_on_;
    oreg_[index_11].bit_6 = false;

    oreg_[index_12].raw = smem_[0];
    oreg_[index_13].raw = smem_[1];
    oreg_[index_14].raw = smem_[2];
    oreg_[index_15].raw = smem_[3];

    // oreg_[index_31].reset();
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
    sr_.raw                      = {};
    sr_.bit_7                    = true;
    sr_.peripheral_data_location = next_peripheral_return_;
    sr_.port_2_mode              = ireg_[index_1].ireg1_port_2_mode;
    sr_.port_1_mode              = ireg_[index_1].ireg1_port_1_mode;

    for (u32 i = 0; i < output_registers_number; ++i) {
        oreg_[i].raw = {};
    }

    switch (toEnum<PortMode>(ireg_[index_1].ireg1_port_1_mode)) {
        using enum PortMode;
        case mode_0_byte: break; // no data returned
        case mode_15_byte:
        case mode_255_byte: {
            // no difference between 15 byte and 255 byte for now
            auto port_1_data = PortData{};
            switch (port_1_status_) {
                case PortStatus::not_connected: {
                    full_peripheral_data_table_.emplace_back(uti::toUnderlying(port_1_status_));
                    full_peripheral_data_table_.emplace_back(u8_max);
                    full_peripheral_data_table_.emplace_back(u8_max);
                    full_peripheral_data_table_.emplace_back(u8_max);
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

    switch (toEnum<PortMode>(ireg_[index_1].ireg1_port_2_mode)) {
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
                    full_peripheral_data_table_.emplace_back(u8_max);
                    full_peripheral_data_table_.emplace_back(u8_max);
                    full_peripheral_data_table_.emplace_back(u8_max);
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
            oreg_[i].raw = full_peripheral_data_table_[i];
        }
        full_peripheral_data_table_.erase(full_peripheral_data_table_.begin(),
                                          full_peripheral_data_table_.begin() + output_registers_number);

        sr_.peripheral_data_remaining = PeripheralDataRemaining::remaining_peripheral_data;
    } else {
        for (u32 i = 0; i < full_peripheral_data_table_.size(); ++i) {
            oreg_[i].raw = full_peripheral_data_table_[i];
        }
        full_peripheral_data_table_.clear();

        sr_.peripheral_data_remaining = PeripheralDataRemaining::no_remaining_peripheral_data;
    }

} // namespace saturnin::core

auto Smpc::generatePeripheralData(const SaturnPeripheralId id) -> PeripheralData {
    auto peripheral_data                 = PeripheralData{};
    peripheral_data.saturn_peripheral_id = id;
    peripheral_data.data_size            = uti::toUnderlying(id) & bitmask_0F;
    if (peripheral_data.data_size != 0) { peripheral_data.extension_data_size = 0; }

    const auto p1 = getSaturnPeripheralMapping().player_1;
    switch (id) {
        using enum SaturnPeripheralId;
        case saturn_standard_pad: {
            auto first_data = SaturnStandardPad1stData{};
            first_data.raw  = u8_max;
            if (isKeyPressed(p1.direction_right, openglWindow())) { first_data.direction_right = false; }
            if (isKeyPressed(p1.direction_left, openglWindow())) { first_data.direction_left = false; }
            if (isKeyPressed(p1.direction_down, openglWindow())) { first_data.direction_down = false; }
            if (isKeyPressed(p1.direction_up, openglWindow())) { first_data.direction_up = false; }
            if (isKeyPressed(p1.button_start, openglWindow())) { first_data.button_start = false; }
            if (isKeyPressed(p1.button_a, openglWindow())) { first_data.button_a = false; }
            if (isKeyPressed(p1.button_c, openglWindow())) { first_data.button_c = false; }
            if (isKeyPressed(p1.button_b, openglWindow())) { first_data.button_b = false; }
            peripheral_data.peripheral_data_table.push_back(first_data.raw);

            auto second_data = SaturnStandardPad2ndData{};
            second_data.raw  = u8_max;
            if (isKeyPressed(p1.button_shoulder_right, openglWindow())) { second_data.button_shoulder_right = false; }
            if (isKeyPressed(p1.button_x, openglWindow())) { second_data.button_x = false; }
            if (isKeyPressed(p1.button_y, openglWindow())) { second_data.button_y = false; }
            if (isKeyPressed(p1.button_z, openglWindow())) { second_data.button_z = false; }
            if (isKeyPressed(p1.button_shoulder_left, openglWindow())) { second_data.button_shoulder_left = false; }
            peripheral_data.peripheral_data_table.push_back(second_data.raw);
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
            return sr_.raw;
        case status_flag: return sf_.raw;
        case output_register_0: return oreg_[index_0].raw;
        case output_register_1: return oreg_[index_1].raw;
        case output_register_2: return oreg_[index_2].raw;
        case output_register_3: return oreg_[index_3].raw;
        case output_register_4: return oreg_[index_4].raw;
        case output_register_5: return oreg_[index_5].raw;
        case output_register_6: return oreg_[index_6].raw;
        case output_register_7: return oreg_[index_7].raw;
        case output_register_8: return oreg_[index_8].raw;
        case output_register_9: return oreg_[index_9].raw;
        case output_register_10: return oreg_[index_10].raw;
        case output_register_11: return oreg_[index_11].raw;
        case output_register_12: return oreg_[index_12].raw;
        case output_register_13: return oreg_[index_13].raw;
        case output_register_14: return oreg_[index_14].raw;
        case output_register_15: return oreg_[index_15].raw;
        case output_register_16: return oreg_[index_16].raw;
        case output_register_17: return oreg_[index_17].raw;
        case output_register_18: return oreg_[index_18].raw;
        case output_register_19: return oreg_[index_19].raw;
        case output_register_20: return oreg_[index_20].raw;
        case output_register_21: return oreg_[index_21].raw;
        case output_register_22: return oreg_[index_22].raw;
        case output_register_23: return oreg_[index_23].raw;
        case output_register_24: return oreg_[index_24].raw;
        case output_register_25: return oreg_[index_25].raw;
        case output_register_26: return oreg_[index_26].raw;
        case output_register_27: return oreg_[index_27].raw;
        case output_register_28: return oreg_[index_28].raw;
        case output_register_29: return oreg_[index_29].raw;
        case output_register_30: return oreg_[index_30].raw;
        case output_register_31: return oreg_[index_31].raw;
        case port_data_register_1:
            if (modules_.context()->hardwareMode() == HardwareMode::stv) {
                constexpr auto default_stv_data = u8{0xff};
                return default_stv_data;
            }
            return pdr1_.pdr;
        case port_data_register_2:
            if (modules_.context()->hardwareMode() == HardwareMode::stv) {
                pdr2_.pdr = 0;
                // const auto controls = getStvPeripheralMapping().board_controls;
                // controls.test_switch;
                //    if (glfwGetKey(glfwGetCurrentContext(), uti::toUnderlying(stv_mapping_.board_controls.test_switch)) ==
                //    GLFW_PRESS) {

                //    }
            }
            pdr2_.raw = u8_max;

            return pdr2_.pdr;

        default: return 0;
    }
}

auto Smpc::rawRead(const u32 addr) -> u8 {
    switch (addr) {
        case input_register_0: return ireg_[index_0].raw;
        case input_register_1: return ireg_[index_1].raw;
        case input_register_2: return ireg_[index_2].raw;
        case input_register_3: return ireg_[index_3].raw;
        case input_register_4: return ireg_[index_4].raw;
        case input_register_5: return ireg_[index_5].raw;
        case input_register_6: return ireg_[index_6].raw;
        default: return read(addr);
    }
}

void Smpc::write(const u32 addr, const u8 data) {
    switch (addr) {
        case command_register:
            comreg_.raw = data;
            setCommandDuration();
            break;
        case status_flag: sf_.raw = data; break;
        case input_register_0: {
            auto old_ireg0     = ireg_[index_0];
            ireg_[index_0].raw = data;
            if (is_intback_processing_) {
                if (toEnum<IntbackBreakRequest>(ireg_[index_0].ireg0_break_request) == IntbackBreakRequest::requested) {
                    Log::debug(Logger::smpc, tr("INTBACK break request"));
                    sr_.raw &= bitmask_0F;
                    is_intback_processing_ = false;
                    break;
                }
                auto old_continue = old_ireg0.ireg0_continue_request;
                auto new_continue = ireg_[index_0].ireg0_continue_request;
                if (new_continue != old_continue) {
                    Log::debug(Logger::smpc, tr("INTBACK continue request"));
                    setCommandDuration();
                    sf_.sf = true;
                    break;
                }
                is_intback_processing_ = false;
            }
            break;
        }
        case input_register_1: ireg_[index_1].raw = data; break;
        case input_register_2: ireg_[index_2].raw = data; break;
        case input_register_3: ireg_[index_3].raw = data; break;
        case input_register_4: ireg_[index_4].raw = data; break;
        case input_register_5: ireg_[index_5].raw = data; break;
        case input_register_6: ireg_[index_6].raw = data; break;
        case port_data_register_1: pdr1_.pdr = data; break;
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
            pdr2_.pdr = data;
            break;
        case data_direction_register_1: ddr1_.ddr = data; break;
        case data_direction_register_2: ddr2_.ddr = data; break;
        case io_select_register: iosel_.raw = data; break;
        case external_latch_register: exle_.exle = data; break;
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

auto Smpc::getSaturnPeripheralMapping() -> SaturnPeripheralMapping { return saturn_mapping_; }

auto Smpc::getStvPeripheralMapping() -> StvPeripheralMapping { return stv_mapping_; }

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

auto getKeyName(const PeripheralKey pk) -> std::string { return keyboard_layout[pk]; }

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
