// 
// EmulatorContext.cpp
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

#include <chrono>
#include <iostream>
#include <future>
#include <chrono>
#include "cdrom/scsi.h"
#include "video/opengl.h"
#include "video/opengl_legacy.h"
#include "video/opengl_modern.h"
#include "config.h"
#include "emulator_context.h"
#include "log.h"
#include "scu_registers.h"
#include "sh2_instructions.h"
#include "sh2_registers.h"
#include "sh2.h"


namespace cdrom = saturnin::cdrom;
namespace video = saturnin::video;
namespace sh2   = saturnin::sh2;

namespace saturnin {
namespace core {

using sh2::Sh2;
using sh2::Sh2Type;

EmulatorContext::EmulatorContext() {
    config_     = std::make_unique<Config>("saturnin.cfg");
    master_sh2_ = std::make_unique<Sh2>(Sh2Type::master, this);
    slave_sh2_  = std::make_unique<Sh2>(Sh2Type::slave, this);
    memory_     = std::make_unique<Memory>(this);
    scu_        = std::make_unique<Scu>(this);
    smpc_       = std::make_unique<Smpc>(this);
}

Config* EmulatorContext::config() { return config_.get(); };

bool EmulatorContext::initialize() {
    Log::initialize();

    if (!this->config()->initialize(video::isModernOpenglCapable())) return false;

    std::string country = this->config()->readValue(core::AccessKeys::cfg_global_language);
    if (!core::Locale::initialize(country)) return false;

    this->smpc()->initializePeripheralMappings();

    cdrom::Scsi::settingUpSptiFunctions();
    if (!cdrom::Scsi::initialize()) return false;

    return true;
}

void EmulatorContext::startEmulation() {
    if (emulation_status_ == EmulationStatus::running) return;

    emulation_status_ = EmulationStatus::running;

    memory()->loadBios(hardware_mode_);

    sh2::initializeOpcodesLut();

    emulation_main_thread_ = std::thread (&EmulatorContext::emulationMainThread, this);
    if (emulation_main_thread_.joinable()) emulation_main_thread_.detach();

    
    //static std::thread emu_thread;
    //if (ImGui::Button("Play")) {
    //    std::thread local_thread(&core::EmulatorContext::startEmulation, &state);
    //    emu_thread = move(local_thread);

    // TESTING //
    const u32 sh2_freq_hz{ 28636400 };
    //const double cycle_duration{ (double)1 / (double)sh2_freq_hz };
    //const std::chrono::seconds cycle_duration{ (double)1 / (double)sh2_freq_hz };
    using seconds = std::chrono::duration<double>;
    const seconds cycle_duration{ ((double)1 / (double)sh2_freq_hz) };
    //const std::chrono::seconds test{ cycle_duration };
    using micro = std::chrono::duration<double, std::micro>;
    auto val = micro(30) / cycle_duration;
    auto val2 = static_cast<u32>(val);
     
    
    //boost::filesystem::path lib_path(boost::filesystem::current_path());          // argv[1] contains path to directory with our plugin library
    //boost::shared_ptr<LogPlugin> plugin;            // variable to hold a pointer to plugin variable
    //std::cout << "loading the plugin" << std::endl;
    //try {
    //    plugin = dll::import<LogPlugin>(    // type of imported symbol is located between `<` and `>`
    //        lib_path / "log",                // path to the library and library name
    //        "plugin",                           // name of the symbol to import
    //        dll::load_mode::append_decorations  // makes `libmy_plugin_sum.so` or `my_plugin_sum.dll` from `my_plugin_sum`
    //        );
    //    std::cout << "Version:  " << plugin->version() << std::endl;
    //    plugin->log("test");

    //Log::error("sh2", "Unexpected opcode({} SH2)\nOpcode: {:#06x}\nPC: {:#010x}", "Master", 0x4e73, 0x20000200);

    //scu_->dmaTest();

    //sh2::TimerInterruptEnableRegister r(0b100);
    //if (r.get(sh2::TimerInterruptEnableRegister::output_compare_interrupt_b_enable) == sh2::OutputCompareInterruptBEnable::interrupt_request_enabled) {
    //    Log::info("main", "enabled");
    //}

    // TESTING //
    
}

void EmulatorContext::stopEmulation() {
    emulation_status_ = core::EmulationStatus::stopped;
    if (emulation_main_thread_.joinable()) emulation_main_thread_.join();
}

void EmulatorContext::emulationMainThread() {
    try {
        Log::info("main", tr("Emulation main thread started"));
        
        while (this->emulation_status_ == EmulationStatus::running) {
            master_sh2_->run();
            slave_sh2_->run();
            //throw std::runtime_error(tr("Exception during main emulation thread !"));
        }
        Log::info("main", tr("Emulation main thread finished"));
    }
    catch (const std::exception& e) {
        Log::error("exception", e.what());
    }
    catch (...) {
        Log::error("exception", tr("Uncaught exception !"));
    }
}

void EmulatorContext::startInterface() {
    bool is_legacy_opengl = this->config()->readValue(core::AccessKeys::cfg_rendering_legacy_opengl);
    (is_legacy_opengl) ? video::runLegacyOpengl(*this) : video::runModernOpengl(*this);
}

}
}