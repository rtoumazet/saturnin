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

#include <Windows.h> // removes C4005 warning
#include <GLFW/glfw3.h>
#include <iostream>
#include <future>
#include <argagg/argagg.hpp>
#include <saturnin/src/config.h>
#include <saturnin/src/log.h>
#include <saturnin/src/memory.h>
#include <saturnin/src/scu.h>
#include <saturnin/src/sh2/basic_interpreter/sh2_instructions.h>
#include <saturnin/src/sh2/fast_interpreter/opcodes_generator.h>
#include <saturnin/src/sh2/sh2.h>
#include <saturnin/src/smpc.h>
#include <saturnin/src/cdrom/cdrom.h>
#include <saturnin/src/cdrom/scsi.h>
#include <saturnin/src/sound/scsp.h>
#include <saturnin/src/video/opengl.h>
#include <saturnin/src/video/vdp1.h>
#include <saturnin/src/video/vdp2.h>

namespace cdrom = saturnin::cdrom;
namespace video = saturnin::video;
namespace sh2   = saturnin::sh2;

namespace saturnin::core {

using cdrom::Cdrom;
using sh2::Sh2;
using sh2::Sh2Type;
using sound::Scsp;
using video::BaseRenderingPart;
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

auto EmulatorContext::initialize(int argc, char* argv[]) -> bool {
    // Locale is defaulted to english to handle the case when there's no config file created yet.
    if (!Locale::getInstance().initialize("en")) { return false; }

    Log::info(Logger::main, tr("Max number of threads available : {}"), std::thread::hardware_concurrency());

    // clang-format off
    argagg::parser parser{{
        {"help", {"-h", "--help"}, tr("Shows this help message"), 0}, 
        {"file", {"-f", "--file"}, tr("Binary file to load"), 1},
        {"set-pc", {"-s", "--set-pc"}, tr("Address to set the PC after loading the binary file, in hex. Default is 0x6004000."), 1},
        {"load-address", {"-l", "--load-address"}, tr("Saturn memory address to load the binary file to, in hex. Default is 0x6004000."), 1},
        {"auto-start", {"-a", "--auto-start"}, tr("Will auto start the emulator after loading when present."),},
        {"generate-sh2-opcodes", {"-g", "--generate-sh2-opcodes"}, tr("Generates an file containing SH2 opcodes."),}

    }};
    // clang-format on
    argagg::parser_results args;
    try {
        args = parser.parse(argc, argv);
        if (args["help"]) {
            std::stringstream usage;
            usage << parser;
            Log::info(Logger::main, usage.str());
        }

        if (args["file"]) {
            BinaryFileConfiguration file{};
            std::string             path = args["file"];
            file.full_path               = path;
            Log::info(Logger::main, tr("Loading binary file : {}"), file.full_path);

            file.load_address    = std::stoul(args["load-address"].as<std::string>("0x6004000"), nullptr, 16);
            file.start_address   = std::stoul(args["set-pc"].as<std::string>("0x6004000"), nullptr, 16);
            file.is_auto_started = args["auto-start"];

            if (memory()->loadBinaryFile(file)) { memory()->selectedBinaryFile(file); }
        }
        if (args["generate-sh2-opcodes"]) {
            if (!sh2::generateOpcodes()) {
                Log::error(Logger::main, tr("Error while generating SH2 opcodes"));
            } else {
                Log::info(Logger::main, tr("SH2 opcodes were generated"));
            }
        }
    } catch (const std::exception& e) {
        Log::error(Logger::main, tr("Error while parsing command line"));
        Log::error(Logger::main, "{}", e.what());
    }

    if (!this->config()->initialize(video::isModernOpenglCapable())) { return false; }

    std::string country = config()->readValue(core::AccessKeys::cfg_global_language);
    if (!Locale::getInstance().initialize(country)) { return false; }

    std::string hm = config()->readValue(core::AccessKeys::cfg_global_hardware_mode);
    hardwareMode(config()->getHardwareMode(hm));

    memory()->selectedStvGame(core::defaultStvGame());

    this->smpc()->initializePeripheralMappings();

    cdrom::Scsi::settingUpSptiFunctions();

    return cdrom::Scsi::initialize();
}

void EmulatorContext::reset() const { EmulatorContext(); }

void EmulatorContext::startEmulation() {
    switch (emulation_status_) {
        using enum EmulationStatus;
        case running: {
            debugStatus(DebugStatus::disabled);
            return;
        }
        case stopped: {
            emulationStatus(EmulationStatus::running);

            emulation_main_thread_ = std::jthread(&EmulatorContext::emulationMainThread, this);
            if (emulation_main_thread_.joinable()) { emulation_main_thread_.detach(); }
            break;
        }
        default: break;
    }
}

void EmulatorContext::stopEmulation() {
    emulation_status_ = core::EmulationStatus::stopped;
    if (emulation_main_thread_.joinable()) { emulation_main_thread_.join(); }
}

void EmulatorContext::pauseEmulation() { debugStatus(DebugStatus::paused); }

void EmulatorContext::emulationSetup() {
    memory()->initialize(hardware_mode_);

    sh2::initializeOpcodesLut();
    masterSh2()->powerOnReset();
    slaveSh2()->powerOnReset();
    smpc()->initialize();
    cdrom()->initialize();
    vdp1()->initialize();
    vdp2()->initialize();
    scsp()->initialize();
}

void EmulatorContext::emulationMainThread() {
    Log::info(Logger::main, tr("Emulation main thread started"));

    emulationSetup();

    while (emulationStatus() == EmulationStatus::running) {
        if (debugStatus() != DebugStatus::paused) {
            const auto cycles = masterSh2()->run();
            if (smpc()->isSlaveSh2On()) { slaveSh2()->run(); }
            smpc()->run(cycles);
            vdp2()->run(cycles);
            cdrom()->run(cycles);
            scsp()->run(cycles);
        }
    }
    Log::info(Logger::main, tr("Emulation main thread finished"));
}

void EmulatorContext::startInterface() {
    renderingStatus(core::RenderingStatus::running);
    video::runOpengl(*this);
}

void EmulatorContext::openglWindow(GLFWwindow* window) { opengl_window_ = window; }

auto EmulatorContext::openglWindow() const -> GLFWwindow* { return opengl_window_; }

} // namespace saturnin::core
