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
//#define NOMINMAX
//#include <chrono>
#include <iostream>
#include <future>
#include <chrono>
#include "cdrom/cdrom.h"
#include "cdrom/scsi.h"
#include "sound/scsp.h"
#include "video/opengl.h"
#include "video/opengl_legacy.h"
#include "video/opengl_modern.h"
#include "video/vdp2.h"
#include "config.h"
#include "emulator_context.h"
#include "locale.h"
#include "log.h"
#include "memory.h"
#include "scu_registers.h"
#include "scu.h"
#include "sh2_instructions.h"
#include "sh2_registers.h"
#include "sh2.h"
#include "smpc.h"
#include <GLFW/glfw3.h>

namespace cdrom = saturnin::cdrom;
namespace video = saturnin::video;
namespace sh2   = saturnin::sh2;

namespace saturnin::core {

using cdrom::Cdrom;
using sh2::Sh2;
using sh2::Sh2Type;
using sound::Scsp;
using video::Vdp2;

EmulatorContext::EmulatorContext() {
    config_     = std::make_unique<Config>("saturnin.cfg");
    master_sh2_ = std::make_unique<Sh2>(Sh2Type::master, this);
    slave_sh2_  = std::make_unique<Sh2>(Sh2Type::slave, this);
    memory_     = std::make_unique<Memory>(this);
    scu_        = std::make_unique<Scu>(this);
    smpc_       = std::make_unique<Smpc>(this);
    scsp_       = std::make_unique<Scsp>(this);
    cdrom_      = std::make_unique<Cdrom>(this);
    vdp2_       = std::make_unique<Vdp2>(this);
}

EmulatorContext::~EmulatorContext() = default;

auto EmulatorContext::config() -> Config* { return config_.get(); };
auto EmulatorContext::memory() -> Memory* { return memory_.get(); };
auto EmulatorContext::masterSh2() -> Sh2* { return master_sh2_.get(); };
auto EmulatorContext::slaveSh2() -> Sh2* { return slave_sh2_.get(); };
auto EmulatorContext::scu() -> Scu* { return scu_.get(); };
auto EmulatorContext::smpc() -> Smpc* { return smpc_.get(); };
auto EmulatorContext::scsp() -> Scsp* { return scsp_.get(); };
auto EmulatorContext::cdrom() -> Cdrom* { return cdrom_.get(); };
auto EmulatorContext::vdp2() -> Vdp2* { return vdp2_.get(); };

auto EmulatorContext::initialize() -> bool {
    Log::initialize();

    if (!this->config()->initialize(video::isModernOpenglCapable())) { return false; }

    std::string country = config()->readValue(core::AccessKeys::cfg_global_language);
    if (!Locale::getInstance().initialize(country)) { return false; }

    std::string hm = config()->readValue(core::AccessKeys::cfg_global_hardware_mode);
    hardwareMode(core::Config::hardware_mode[hm]);

    this->smpc()->initializePeripheralMappings();

    cdrom::Scsi::settingUpSptiFunctions();

    return cdrom::Scsi::initialize();
}

void EmulatorContext::startEmulation() {
    switch (emulation_status_) {
        case EmulationStatus::running: {
            debugStatus(DebugStatus::disabled);
            return;
        }
        case EmulationStatus::stopped: {
            emulationStatus(EmulationStatus::running);

            emulation_main_thread_ = std::thread(&EmulatorContext::emulationMainThread, this);
            if (emulation_main_thread_.joinable()) { emulation_main_thread_.detach(); }
            break;
        }
    }

    // static std::thread emu_thread;
    // if (ImGui::Button("Play")) {
    //    std::thread local_thread(&core::EmulatorContext::startEmulation, &state);
    //    emu_thread = move(local_thread);

    // TESTING //
    const u32 sh2_freq_hz{28636400};
    // const double cycle_duration{ (double)1 / (double)sh2_freq_hz };
    // const std::chrono::seconds cycle_duration{ (double)1 / (double)sh2_freq_hz };
    using seconds = std::chrono::duration<double>;
    const seconds cycle_duration{((double)1 / (double)sh2_freq_hz)};
    // const std::chrono::seconds test{ cycle_duration };
    using micro = std::chrono::duration<double, std::micro>;
    constexpr u8 op_duration{30};
    auto         val  = micro(op_duration) / cycle_duration;
    auto         val2 = static_cast<u32>(val);

    auto rtc  = getRtcTime();
    auto year = rtc.getUpperYear();

    // cdrom::CommandRegister cr{0b1111};
    // const u8               data{0b0011};
    // cr &= data;

    // boost::filesystem::path lib_path(boost::filesystem::current_path());          // argv[1] contains path to directory with
    // our plugin library boost::shared_ptr<LogPlugin> plugin;            // variable to hold a pointer to plugin variable
    // std::cout << "loading the plugin" << std::endl;
    // try {
    //    plugin = dll::import<LogPlugin>(    // type of imported symbol is located between `<` and `>`
    //        lib_path / "log",                // path to the library and library name
    //        "plugin",                           // name of the symbol to import
    //        dll::load_mode::append_decorations  // makes `libmy_plugin_sum.so` or `my_plugin_sum.dll` from `my_plugin_sum`
    //        );
    //    std::cout << "Version:  " << plugin->version() << std::endl;
    //    plugin->log("test");

    // Log::error("sh2", "Unexpected opcode({} SH2)\nOpcode: {:#06x}\nPC: {:#010x}", "Master", 0x4e73, 0x20000200);

    // scu_->dmaTest();

    // sh2::TimerInterruptEnableRegister r(0b100);
    // if (r.get(sh2::TimerInterruptEnableRegister::output_compare_interrupt_b_enable) ==
    // sh2::OutputCompareInterruptBEnable::interrupt_request_enabled) {
    //    Log::info("main", "enabled");
    //}

    // TESTING //
}

void EmulatorContext::stopEmulation() {
    emulation_status_ = core::EmulationStatus::stopped;
    if (emulation_main_thread_.joinable()) { emulation_main_thread_.join(); }
}

void EmulatorContext::pauseEmulation() { debugStatus(DebugStatus::paused); }

void EmulatorContext::emulationMainThread() {
    try {
        Log::info("main", tr("Emulation main thread started"));

        memory()->initialize();
        memory()->loadBios(hardware_mode_);

        sh2::initializeOpcodesLut();
        masterSh2()->powerOnReset();
        slaveSh2()->powerOnReset();
        smpc()->initialize();
        cdrom()->initialize();
        vdp2()->initialize();

        while (emulationStatus() == EmulationStatus::running) {
            if (debugStatus() != DebugStatus::paused) {
                auto cycles = masterSh2()->run();
                if (smpc()->isSlaveSh2On()) { slaveSh2()->run(); }
                smpc()->run(cycles);
                vdp2()->run(cycles);
                cdrom()->run(cycles);
            }
        }
        Log::info("main", tr("Emulation main thread finished"));
    } catch (const std::exception& e) { Log::error("exception", e.what()); } catch (...) {
        Log::error("exception", tr("Uncaught exception !"));
    }
}

void EmulatorContext::startInterface() {
    bool is_legacy_opengl = this->config()->readValue(core::AccessKeys::cfg_rendering_legacy_opengl);
    (is_legacy_opengl) ? video::runLegacyOpengl(*this) : video::runModernOpengl(*this);
}

void EmulatorContext::openglWindow(GLFWwindow* window) { opengl_window_ = window; }

auto EmulatorContext::openglWindow() const -> GLFWwindow* { return opengl_window_; }

} // namespace saturnin::core
