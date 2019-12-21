// 
// smpc.cpp
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

#include "smpc.h"
#include "emulator_context.h"

namespace saturnin {
namespace core {

std::vector<PeripheralKey> SaturnDigitalPad::toConfig(const PeripheralLayout layout) {
    switch (layout) {
        case PeripheralLayout::empty: return std::vector<PeripheralKey>();
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
        Log::error("smpc", tr("Incorrect Saturn pad data"));
        throw std::runtime_error(tr("Incorrect Saturn pad data"));
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

    //auto key = glfwGetKeyName(direction_left, NULL);
}

std::vector<PeripheralKey> StvPlayerControls::toConfig(const PeripheralLayout layout) {
    switch (layout) {
        case PeripheralLayout::empty: return std::vector<PeripheralKey>();
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
        Log::error("smpc", tr("Incorrect ST-V player control data"));
        throw std::runtime_error(tr("Incorrect ST-V player control data"));
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
        case PeripheralLayout::empty:   return std::vector<PeripheralKey>();
        case PeripheralLayout::current: return std::vector<PeripheralKey> { service_switch, test_switch, p1_coin_switch, p2_coin_switch };
        default:                        
            return std::vector<PeripheralKey> { PeripheralKey::key_1, PeripheralKey::key_2, PeripheralKey::key_5, PeripheralKey::key_6 };
    }
}

void StvBoardControls::fromConfig(std::vector<PeripheralKey> config) {
    if (config.size() != 4) {
        Log::error("smpc", tr("Incorrect ST-V board control data"));
        throw std::runtime_error(tr("Incorrect ST-V board control data"));
    }
    service_switch = config[0];
    test_switch    = config[1];
    p1_coin_switch = config[2];
    p2_coin_switch = config[3];
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
            if (emulator_context_->hardwareMode() == HardwareMode::stv) {
                pdr2_.reset();
                if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_F1) == GLFW_PRESS) {

                }
            }
            return pdr2_.get(PortDataRegister::all_bits);

        default:
            return 0;
    }
}

void Smpc::write(const u32 addr, const u8 data) {

}

}
}