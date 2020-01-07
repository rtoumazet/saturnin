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

#include <map> // map
#include <string> // string
#include "smpc.h"
#include "emulator_context.h"

namespace saturnin {
namespace core {

namespace util = saturnin::utilities;

using MapKeyboardLayout = std::map<PeripheralKey, const std::string>;
MapKeyboardLayout keyboard_layout = { 
    { PeripheralKey::key_space, "space"}, 
    { PeripheralKey::key_apostrophe, "'"},
    { PeripheralKey::key_comma, ","},
    { PeripheralKey::key_minus, "-"},
    { PeripheralKey::key_period, "."},
    { PeripheralKey::key_slash, "/"},
    { PeripheralKey::key_semicolon, ";"},
    { PeripheralKey::key_equal, "="},
    { PeripheralKey::key_0, "0" },
    { PeripheralKey::key_1, "1" },
    { PeripheralKey::key_2, "2" },
    { PeripheralKey::key_3, "3"},
    { PeripheralKey::key_4, "4"},
    { PeripheralKey::key_5, "5"},
    { PeripheralKey::key_6, "6"},
    { PeripheralKey::key_7, "7"},
    { PeripheralKey::key_8, "8"},
    { PeripheralKey::key_9, "9"},
    { PeripheralKey::key_a, "a"},
    { PeripheralKey::key_b, "b"},
    { PeripheralKey::key_c, "c"},
    { PeripheralKey::key_d, "d"},
    { PeripheralKey::key_e, "e"},
    { PeripheralKey::key_f, "f"},
    { PeripheralKey::key_g, "g"},
    { PeripheralKey::key_h, "h"},
    { PeripheralKey::key_i, "i"},
    { PeripheralKey::key_j, "j"},
    { PeripheralKey::key_k, "k"},
    { PeripheralKey::key_l, "l"},
    { PeripheralKey::key_m, "m"},
    { PeripheralKey::key_n, "n"},
    { PeripheralKey::key_o, "o"},
    { PeripheralKey::key_p, "p"},
    { PeripheralKey::key_q, "q"},
    { PeripheralKey::key_r, "r"},
    { PeripheralKey::key_s, "s"},
    { PeripheralKey::key_t, "t"},
    { PeripheralKey::key_u, "u"},
    { PeripheralKey::key_v, "v"},
    { PeripheralKey::key_w, "w"},
    { PeripheralKey::key_x, "x"},
    { PeripheralKey::key_y, "y"},
    { PeripheralKey::key_z, "z"},
    { PeripheralKey::key_left_bracket, "["},
    { PeripheralKey::key_backslash, "\\"},
    { PeripheralKey::key_right_bracket, "]"},
    { PeripheralKey::key_grave_accent, "`"},

    /* Function keys */
    { PeripheralKey::key_escape, "esc"},
    { PeripheralKey::key_enter, "enter"},
    { PeripheralKey::key_tab, "tab"},
    { PeripheralKey::key_backspace, "backspace"},
    { PeripheralKey::key_insert, "insert"},
    { PeripheralKey::key_delete, "del"},
    { PeripheralKey::key_right, "right"},
    { PeripheralKey::key_left, "left"},
    { PeripheralKey::key_down, "down"},
    { PeripheralKey::key_up, "up"},
    { PeripheralKey::key_page_up, "page up"},
    { PeripheralKey::key_page_down, "page down"},
    { PeripheralKey::key_home, "home"},
    { PeripheralKey::key_end, "end"},
    { PeripheralKey::key_caps_lock, "caps lock"},
    { PeripheralKey::key_scroll_lock, "scroll lock"},
    { PeripheralKey::key_num_lock, "num lock"},
    { PeripheralKey::key_print_screen, "print screen"},
    { PeripheralKey::key_pause, "pause"},
    { PeripheralKey::key_f1, "f1"},
    { PeripheralKey::key_f2, "f2"},
    { PeripheralKey::key_f3, "f3"},
    { PeripheralKey::key_f4, "f4"},
    { PeripheralKey::key_f5, "f5"},
    { PeripheralKey::key_f6, "f6"},
    { PeripheralKey::key_f7, "f7"},
    { PeripheralKey::key_f8, "f8"},
    { PeripheralKey::key_f9, "f9"},
    { PeripheralKey::key_f10, "f10"},
    { PeripheralKey::key_f11, "f11"},
    { PeripheralKey::key_f12, "f12"},
    { PeripheralKey::key_kp_0, "keypad 0"},
    { PeripheralKey::key_kp_1, "keypad 1"},
    { PeripheralKey::key_kp_2, "keypad 2"},
    { PeripheralKey::key_kp_3, "keypad 3"},
    { PeripheralKey::key_kp_4, "keypad 4"},
    { PeripheralKey::key_kp_5, "keypad 5"},
    { PeripheralKey::key_kp_6, "keypad 6"},
    { PeripheralKey::key_kp_7, "keypad 7"},
    { PeripheralKey::key_kp_8, "keypad 8"},
    { PeripheralKey::key_kp_9, "keypad 9"},
    { PeripheralKey::key_kp_decimal, "keypad ."},
    { PeripheralKey::key_kp_divide, "keypad /"},
    { PeripheralKey::key_kp_multiply, "keypad *"},
    { PeripheralKey::key_kp_substract, "keypad -"},
    { PeripheralKey::key_kp_add, "keypad +"},
    { PeripheralKey::key_kp_enter, "keypad enter"},
    { PeripheralKey::key_kp_equal, "keypad ="},
    { PeripheralKey::key_left_shift, "left shift"},
    { PeripheralKey::key_left_control, "left control"},
    { PeripheralKey::key_left_alt, "left alt"},
    { PeripheralKey::key_left_super, "left super"},
    { PeripheralKey::key_right_shift, "right shift"},
    { PeripheralKey::key_right_control, "right control"},
    { PeripheralKey::key_right_alt, "right alt"},
    { PeripheralKey::key_right_super, "right super"},
    { PeripheralKey::key_menu, "menu"}
};
    
std::vector<PeripheralKey> SaturnDigitalPad::toConfig(const PeripheralLayout layout) {
    switch (layout) {
        case PeripheralLayout::empty:
            return std::vector<PeripheralKey>{
                PeripheralKey::key_unknown, PeripheralKey::key_unknown, PeripheralKey::key_unknown, PeripheralKey::key_unknown,
                PeripheralKey::key_unknown, PeripheralKey::key_unknown,
                PeripheralKey::key_unknown, PeripheralKey::key_unknown, PeripheralKey::key_unknown,
                PeripheralKey::key_unknown, PeripheralKey::key_unknown, PeripheralKey::key_unknown,
                PeripheralKey::key_unknown
            };
        case PeripheralLayout::current:
            return std::vector<PeripheralKey> {
                direction_left, direction_right, direction_up, direction_down,
                button_shoulder_left, button_shoulder_right,
                button_a, button_b, button_c,
                button_x, button_y, button_z,
                button_start
            };
        default:
            return std::vector<PeripheralKey> {
                PeripheralKey::key_left, PeripheralKey::key_right, PeripheralKey::key_up, PeripheralKey::key_down,
                PeripheralKey::key_z, PeripheralKey::key_e,
                PeripheralKey::key_s, PeripheralKey::key_d, PeripheralKey::key_f,
                PeripheralKey::key_x, PeripheralKey::key_c, PeripheralKey::key_v,
                PeripheralKey::key_enter
            };
    }
}
void SaturnDigitalPad::fromConfig(std::vector<PeripheralKey> config) {
    if (config.size() != 13) {
        Log::warning("smpc", tr("Incorrect Saturn pad data"));
        auto v = SaturnDigitalPad().toConfig(PeripheralLayout::empty);
        SaturnDigitalPad pad;
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

std::vector<PeripheralKey> StvPlayerControls::toConfig(const PeripheralLayout layout) {
    switch (layout) {
        case PeripheralLayout::empty: 
            return std::vector<PeripheralKey> {
                PeripheralKey::key_unknown, PeripheralKey::key_unknown, PeripheralKey::key_unknown, PeripheralKey::key_unknown,
                PeripheralKey::key_unknown, PeripheralKey::key_unknown, PeripheralKey::key_unknown, PeripheralKey::key_unknown
            };
        case PeripheralLayout::current:
            return std::vector<PeripheralKey> {
                direction_left, direction_right, direction_up, direction_down,
                button_1, button_2, button_3, button_4 
            };
        default:
            return std::vector<PeripheralKey> {
                PeripheralKey::key_left, PeripheralKey::key_right, PeripheralKey::key_up, PeripheralKey::key_down,
                PeripheralKey::key_s, PeripheralKey::key_d, PeripheralKey::key_f, PeripheralKey::key_g
            };
    }
}

void StvPlayerControls::fromConfig(std::vector<PeripheralKey> config) {
    if (config.size() != 8) {
        Log::warning("smpc", tr("Incorrect ST-V player control data"));
        auto v = StvPlayerControls().toConfig(PeripheralLayout::empty);
        StvPlayerControls control;
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

std::vector<PeripheralKey> StvBoardControls::toConfig(const PeripheralLayout layout) {
    switch (layout) {
        case PeripheralLayout::empty:   
            return std::vector<PeripheralKey> { 
                PeripheralKey::key_unknown, PeripheralKey::key_unknown, PeripheralKey::key_unknown, 
                PeripheralKey::key_unknown, PeripheralKey::key_unknown, PeripheralKey::key_unknown
            };
        case PeripheralLayout::current: 
            return std::vector<PeripheralKey> { 
                service_switch, test_switch, p1_coin_switch, 
                p2_coin_switch, p1_start, p2_start 
            };
        default:                        
            return std::vector<PeripheralKey> { 
                PeripheralKey::key_1, PeripheralKey::key_2, PeripheralKey::key_5, 
                PeripheralKey::key_6, PeripheralKey::key_7, PeripheralKey::key_8 };
    }
}

void StvBoardControls::fromConfig(std::vector<PeripheralKey> config) {
    if (config.size() != 6) {
        Log::warning("smpc", tr("Incorrect ST-V board control data"));
        auto v = StvBoardControls().toConfig(PeripheralLayout::empty);
        StvBoardControls control;
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

void Smpc::reset(){
    comreg_.reset();
    sr_.reset();
    sf_.reset();
    for (u8 i = 0; i < 7; ++i) {
        ireg_[i].reset();
    }
    for (u8 i = 0; i < 32; ++i) {
        oreg_[i].reset();
    }
    pdr1_.reset();
    pdr2_.reset();
    ddr1_.reset();
    ddr2_.reset();
    iosel_.reset();
    exle_.reset();

    clock_ = SystemClock::pal_320; // System clock is 320 at reset. PAL fixed for now.
}

u32 Smpc::calculateCyclesNumber(const std::chrono::duration<double>& d) {
    using seconds = std::chrono::duration<double>;
    const seconds cycle_duration{ (static_cast<double>(1) / static_cast<double>(util::toUnderlying(clock_))) };
    //using micro = std::chrono::duration<double, std::micro>;

    return static_cast<u32>(d / cycle_duration);
}

void Smpc::setCommandDuration() {
    using micro = std::chrono::duration<double, std::micro>;
    using milli = std::chrono::duration<double, std::milli>;
    switch (comreg_.get(CommandRegister::smpc_command)) {
        case SmpcCommand::master_sh2_on: 
        case SmpcCommand::slave_sh2_on: 
        case SmpcCommand::slave_sh2_off: 
        case SmpcCommand::sound_on: 
        case SmpcCommand::sound_off: 
        case SmpcCommand::nmi_request: 
        case SmpcCommand::reset_enable: 
        case SmpcCommand::reset_disable: 
            intback_remaining_cycles_ = calculateCyclesNumber(micro(30)); 
            break;
        case SmpcCommand::cd_on:
        case SmpcCommand::cd_off:
        case SmpcCommand::smpc_memory_setting:
            intback_remaining_cycles_ = calculateCyclesNumber(micro(40));
            break;
        case SmpcCommand::reset_entire_system:
        case SmpcCommand::clock_change_320:
        case SmpcCommand::clock_change_352:
            // Alpha is fixed to 0
            intback_remaining_cycles_ = calculateCyclesNumber(milli(100));
            break;
        case SmpcCommand::time_setting:
            intback_remaining_cycles_ = calculateCyclesNumber(micro(70));
            break;
        case SmpcCommand::interrupt_back:

            break;
        default:

            break;
    }
}

u8 Smpc::read(const u32 addr) {
    switch (addr) {
        case status_register:
            if (emulator_context_->hardwareMode() == HardwareMode::stv) return 0xCF;
            return sr_.get(StatusRegister::all_bits);
        case status_flag: return sf_.get(StatusFlag::sf);
        case output_register_0: return oreg_[0].get(OutputRegister::all_bits);
        case output_register_1: return oreg_[1].get(OutputRegister::all_bits);
        case output_register_2: return oreg_[2].get(OutputRegister::all_bits);
        case output_register_3: return oreg_[3].get(OutputRegister::all_bits);
        case output_register_4: return oreg_[4].get(OutputRegister::all_bits);
        case output_register_5: return oreg_[5].get(OutputRegister::all_bits);
        case output_register_6: return oreg_[6].get(OutputRegister::all_bits);
        case output_register_7: return oreg_[7].get(OutputRegister::all_bits);
        case output_register_8: return oreg_[8].get(OutputRegister::all_bits);
        case output_register_9: return oreg_[9].get(OutputRegister::all_bits);
        case output_register_10: return oreg_[10].get(OutputRegister::all_bits);
        case output_register_11: return oreg_[11].get(OutputRegister::all_bits);
        case output_register_12: return oreg_[12].get(OutputRegister::all_bits);
        case output_register_13: return oreg_[13].get(OutputRegister::all_bits);
        case output_register_14: return oreg_[14].get(OutputRegister::all_bits);
        case output_register_15: return oreg_[15].get(OutputRegister::all_bits);
        case output_register_16: return oreg_[16].get(OutputRegister::all_bits);
        case output_register_17: return oreg_[17].get(OutputRegister::all_bits);
        case output_register_18: return oreg_[18].get(OutputRegister::all_bits);
        case output_register_19: return oreg_[19].get(OutputRegister::all_bits);
        case output_register_20: return oreg_[20].get(OutputRegister::all_bits);
        case output_register_21: return oreg_[21].get(OutputRegister::all_bits);
        case output_register_22: return oreg_[22].get(OutputRegister::all_bits);
        case output_register_23: return oreg_[23].get(OutputRegister::all_bits);
        case output_register_24: return oreg_[24].get(OutputRegister::all_bits);
        case output_register_25: return oreg_[25].get(OutputRegister::all_bits);
        case output_register_26: return oreg_[26].get(OutputRegister::all_bits);
        case output_register_27: return oreg_[27].get(OutputRegister::all_bits);
        case output_register_28: return oreg_[28].get(OutputRegister::all_bits);
        case output_register_29: return oreg_[29].get(OutputRegister::all_bits);
        case output_register_30: return oreg_[30].get(OutputRegister::all_bits);
        case output_register_31: return oreg_[31].get(OutputRegister::all_bits);
        case port_data_register_1: 
            if (emulator_context_->hardwareMode() == HardwareMode::stv) return 0xFF;
            return pdr1_.get(PortDataRegister::all_bits);
        case port_data_register_2: 
            //if (emulator_context_->hardwareMode() == HardwareMode::stv) {
            //    pdr2_.reset();
            //    if (glfwGetKey(glfwGetCurrentContext(), util::toUnderlying(stv_mapping_.board_controls.test_switch)) == GLFW_PRESS) {

            //    }
            //}
            return pdr2_.get(PortDataRegister::all_bits);

        default:
            return 0;
    }
}

void Smpc::write(const u32 addr, const u8 data) {
    switch (addr) {
        case command_register:
            comreg_.set(CommandRegister::all_bits, data);
            setCommandDuration();
            break;
        case status_flag: sf_.set(StatusFlag::sf, data); break;
        case input_register_0:

            break;
        case input_register_1: ireg_[1].set(InputRegister::all_bits, data); break;
        case input_register_2: ireg_[2].set(InputRegister::all_bits, data); break;
        case input_register_3: ireg_[3].set(InputRegister::all_bits, data); break;
        case input_register_4: ireg_[4].set(InputRegister::all_bits, data); break;
        case input_register_5: ireg_[5].set(InputRegister::all_bits, data); break;
        case input_register_6: ireg_[6].set(InputRegister::all_bits, data); break;
        case port_data_register_1: pdr1_.set(PortDataRegister::all_bits, data); break;
        case port_data_register_2: 
            if (emulator_context_->hardwareMode() == HardwareMode::stv) {
                if (data & 0x10) {
                    Log::debug("smpc", tr("-= Sound OFF =-"));

                    Log::warning("smpc", "SCSP not linked !!!");
                    //EmuState::pScsp->Reset();
                    //EmuState::pScsp->SetSound(false);
                } else {
                    Log::debug("smpc", tr("-= Sound ON =-"));

                    Log::warning("smpc", "SCSP not linked !!!");
                    //soundStatus = 0x1;
                    //EmuState::pScsp->SetSound(true);
                }
            }
            pdr2_.set(PortDataRegister::all_bits, data); 
            break;
        case data_direction_register_1: ddr1_.set(DataDirectionRegister::all_bits, data); break;
        case data_direction_register_2: ddr2_.set(DataDirectionRegister::all_bits, data); break;
        case io_select_register: iosel_.set(IOSelect::all_bits, data); break;
        case external_latch_register: exle_.set(ExternalLatchEnable::all_bits, data); break;
        default:
            break;
    }
}

std::vector<PeripheralKey> Smpc::listAvailableKeys() {
    std::vector<PeripheralKey> v {};
    for (std::pair<PeripheralKey, const std::string&> key : keyboard_layout) {
        v.push_back(key.first);
    }
    return v;
}

void Smpc::initializePeripheralMappings() {
    saturn_mapping_.player_1.fromConfig(emulator_context_->config()->readPeripheralConfiguration(core::AccessKeys::cfg_controls_saturn_player_1));
    saturn_mapping_.player_2.fromConfig(emulator_context_->config()->readPeripheralConfiguration(core::AccessKeys::cfg_controls_saturn_player_2));
    stv_mapping_.board_controls.fromConfig(emulator_context_->config()->readPeripheralConfiguration(core::AccessKeys::cfg_controls_stv_board));
    stv_mapping_.player_1.fromConfig(emulator_context_->config()->readPeripheralConfiguration(core::AccessKeys::cfg_controls_stv_player_1));
    stv_mapping_.player_2.fromConfig(emulator_context_->config()->readPeripheralConfiguration(core::AccessKeys::cfg_controls_stv_player_2));
}

SaturnPeripheralMapping Smpc::getSaturnPeripheralMapping() {
    return saturn_mapping_;
}

StvPeripheralMapping Smpc::getStvPeripheralMapping() {
    return stv_mapping_;
}

std::string getKeyName(const PeripheralKey pk) {
    return keyboard_layout[pk];
}

}
}