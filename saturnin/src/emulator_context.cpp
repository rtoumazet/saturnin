// 
// emulator_context.cpp
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
using sh2::StatusRegister;

static std::exception_ptr exp_ptr = nullptr;

Emulator_context::Emulator_context() {
    config_     = std::make_unique<Config>("saturnin.cfg");
    master_sh2_ = std::make_unique<Sh2>(Sh2Type::master, this);
    slave_sh2_  = std::make_unique<Sh2>(Sh2Type::slave, this);
    memory_     = std::make_unique<Memory>(this);
    scu_        = std::make_unique<Scu>(this);
}

Config* Emulator_context::config() { return config_.get(); };

bool Emulator_context::initialize() {
    Log::initialize();

    if (!this->config()->initialize(video::isModernOpenglCapable())) return false;

    std::string country = this->config()->readValue(core::Access_keys::config_language);
    if (!core::Locale::initialize(country)) return false;

    cdrom::Scsi::settingUpSptiFunctions();
    if (!cdrom::Scsi::initialize()) return false;

    return true;
}

void Emulator_context::startEmulation() {
    if (emulation_status_ == EmulationStatus::running) return;

    emulation_status_ = EmulationStatus::running;

    memory()->loadBios(hardware_mode_);

    sh2::initializeOpcodesLut();

    emulation_main_thread_ = std::thread (&Emulator_context::emulationMainThread, this);
    //emulation_main_thread_.join();

    // Getting the exception from the thread (if any), and propagating it to the main exception handler.
    if (exp_ptr) { std::rethrow_exception(exp_ptr); }
    
    
    //static std::thread emu_thread;
    //if (ImGui::Button("Play")) {
    //    std::thread local_thread(&core::Emulator_context::startEmulation, &state);
    //    emu_thread = move(local_thread);

    // TESTING //
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

void Emulator_context::stopEmulation() {
    emulation_status_ = core::EmulationStatus::stopped;
    if (emulation_main_thread_.joinable()) emulation_main_thread_.join();
}

void Emulator_context::emulationMainThread() {
    try {
        Log::info("main", tr("Emulation main thread started"));
        throw std::runtime_error(tr("Exception during main emulation thread !"));
        while (this->emulation_status_ == EmulationStatus::running) {
            master_sh2_->run();
            slave_sh2_->run();
        }
        Log::info("main", tr("Emulation main thread finished"));
    }
    catch (...) {
        exp_ptr = std::current_exception();
    }
}

void Emulator_context::startInterface() {
    bool is_legacy_opengl = this->config()->readValue(core::Access_keys::config_legacy_opengl);
    (is_legacy_opengl) ? video::runLegacyOpengl(*this) : video::runModernOpengl(*this);
}

}
}