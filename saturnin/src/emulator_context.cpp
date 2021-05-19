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

#include <saturnin/src/pch.h>
#include <saturnin/src/emulator_context.h>

#include <windows.h> // removes C4005 warning
#include <GLFW/glfw3.h>
#include <iostream>
#include <future>
#include <chrono>
#include <saturnin/src/config.h>
//#include <saturnin/src/locale.h>
#include <saturnin/src/log.h>
#include <saturnin/src/memory.h>
#include <saturnin/src/scu_registers.h>
#include <saturnin/src/scu.h>
#include <saturnin/src/sh2_instructions.h>
#include <saturnin/src/sh2_registers.h>
#include <saturnin/src/sh2.h>
#include <saturnin/src/smpc.h>
#include <saturnin/src/cdrom/cdrom.h>
#include <saturnin/src/cdrom/scsi.h>
#include <saturnin/src/sound/scsp.h>
#include <saturnin/src/video/opengl.h>
#include <saturnin/src/video/vdp1.h>
#include <saturnin/src/video/vdp1_registers.h>
#include <saturnin/src/video/vdp2.h>

namespace cdrom = saturnin::cdrom;
namespace video = saturnin::video;
namespace sh2   = saturnin::sh2;

namespace saturnin::core {

using cdrom::Cdrom;
using sh2::Sh2;
using sh2::Sh2Type;
using sound::Scsp;
using video::Opengl;
using video::Vdp1;
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
    vdp1_       = std::make_unique<Vdp1>(this);
    vdp2_       = std::make_unique<Vdp2>(this);
    opengl_     = std::make_unique<Opengl>(config_.get());
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
auto EmulatorContext::vdp1() -> Vdp1* { return vdp1_.get(); };
auto EmulatorContext::vdp2() -> Vdp2* { return vdp2_.get(); };
auto EmulatorContext::opengl() -> Opengl* { return opengl_.get(); };

auto EmulatorContext::initialize() -> bool {
    Log::initialize();

    if (!this->config()->initialize(video::isModernOpenglCapable())) { return false; }

    std::string country = config()->readValue(core::AccessKeys::cfg_global_language);
    if (!Locale::getInstance().initialize(country)) { return false; }

    std::string hm = config()->readValue(core::AccessKeys::cfg_global_hardware_mode);
    hardwareMode(config()->getHardwareMode(hm));

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
        default: break;
    }

    // static std::thread emu_thread;
    // if (ImGui::Button("Play")) {
    //    std::thread local_thread(&core::EmulatorContext::startEmulation, &state);
    //    emu_thread = move(local_thread);

    // TESTING //
    // const u32 sh2_freq_hz{28636400};
    // const double cycle_duration{ (double)1 / (double)sh2_freq_hz };
    // const std::chrono::seconds cycle_duration{ (double)1 / (double)sh2_freq_hz };
    // using seconds = std::chrono::duration<double>;
    // const seconds cycle_duration{((double)1 / (double)sh2_freq_hz)};
    // const std::chrono::seconds test{ cycle_duration };
    // using micro = std::chrono::duration<double, std::micro>;
    // constexpr u8 op_duration{30};
    // auto         val  = micro(op_duration) / cycle_duration;
    // auto         val2 = static_cast<u32>(val);

    // auto rtc  = getRtcTime();
    // auto year = rtc.getUpperYear();

    // IOSelect ios{0xFF};
    // auto     b = ios.get(IOSelect::all_bits);

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

    // Log::error(Logger::sh2, "Unexpected opcode({} SH2)\nOpcode: {:#06x}\nPC: {:#010x}", "Master", 0x4e73, 0x20000200);

    // scu_->dmaTest();

    // sh2::TimerInterruptEnableRegister r(0b100);
    // if (r.get(sh2::TimerInterruptEnableRegister::output_compare_interrupt_b_enable) ==
    // sh2::OutputCompareInterruptBEnable::interrupt_request_enabled) {
    //    Log::info(Logger::main, "enabled");
    //}
    // u8  tmp[4] = {0x12, 0x34, 0x56, 0x78};
    // u32 tmp2   = *(unsigned long*)&tmp;
    // video::CmdVertexCoordinate tst{0x1234};
    // auto                       b = tst.twoCompl();
    // video::CmdVertexCoordinate tst2{0x8234};
    // auto                       b2 = tst2.twoCompl();
    // TESTING //
}

void EmulatorContext::stopEmulation() {
    emulation_status_ = core::EmulationStatus::stopped;
    if (emulation_main_thread_.joinable()) { emulation_main_thread_.join(); }
}

void EmulatorContext::pauseEmulation() { debugStatus(DebugStatus::paused); }

void EmulatorContext::emulationMainThread() {
    try {
        Log::info(Logger::main, tr("Emulation main thread started"));

        opengl()->initialize(openglWindow());

        memory()->initialize();
        memory()->loadBios(hardware_mode_);

        sh2::initializeOpcodesLut();
        masterSh2()->powerOnReset();
        slaveSh2()->powerOnReset();
        smpc()->initialize();
        cdrom()->initialize();
        vdp1()->initialize();
        vdp2()->initialize();
        scsp()->initialize();

        while (emulationStatus() == EmulationStatus::running) {
            if (debugStatus() != DebugStatus::paused) {
                const auto cycles = masterSh2()->run();
                if (smpc()->isSlaveSh2On()) { slaveSh2()->run(); }
                smpc()->run(cycles);
                vdp1()->run(cycles);
                vdp2()->run(cycles);
                cdrom()->run(cycles);
                scsp()->run(cycles);
                //} else {
                //    notifyRenderingDone();
            }
        }
        opengl()->destroyRenderingContext();
        Log::info(Logger::main, tr("Emulation main thread finished"));
    } catch (const std::exception& e) { Log::error(Logger::exception, e.what()); } catch (...) {
        Log::error(Logger::exception, tr("Uncaught exception !"));
    }
}

void EmulatorContext::startInterface() { video::runOpengl(*this); }

void EmulatorContext::openglWindow(GLFWwindow* window) { opengl_window_ = window; }

auto EmulatorContext::openglWindow() const -> GLFWwindow* { return opengl_window_; }

// void EmulatorContext::opengl(video::Opengl* opengl) { opengl_ = opengl; };

void EmulatorContext::waitUntilRenderingDone() {
    is_rendering_done_ = false; // Reset the condition
    if (emulationStatus() != EmulationStatus::running) { is_rendering_done_ = true; }
    std::unique_lock<std::mutex> mlock(rendering_mutex_);                             // Acquiring the lock
    rendering_condition_variable_.wait(mlock, [this] { return is_rendering_done_; }); // Waiting for the signal
}

void EmulatorContext::notifyRenderingDone() {
    std::lock_guard<std::mutex> guard(rendering_mutex_); // Lock the mutex
    is_rendering_done_ = true;                           // Set the flag to true, means rendering is done
    rendering_condition_variable_.notify_one();          // Notify the condition variable
}

} // namespace saturnin::core
