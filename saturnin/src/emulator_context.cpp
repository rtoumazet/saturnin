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
#include <argagg/argagg.hpp>
#include <saturnin/src/config.h>
// #include <saturnin/src/locale.h>
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
#include <saturnin/src/video/base_rendering_part.h>

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
        {"set-pc", {"-s", "--set-pc"}, 
            tr("Address to set the PC after loading the binary file, in hex. Default is 0x6004000."), 1},
        {"load-address", {"-l", "--load-address"}, 
            tr("Saturn memory address to load the binary file to, in hex. Default is 0x6004000."), 1},
        {"auto-start", {"-a", "--auto-start"}, 
            tr("Will auto start the emulator after loading when present."),},

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

void EmulatorContext::reset() { EmulatorContext(); }

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

    runTests();
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
            vdp1()->run(cycles);
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

void runTests() {
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
    //
    // video::ScrollScreenStatus sss;
    // video::ColorOffsetBRed a{0x01E9};
    // video::ColorOffsetBRed b{0x00e3};

    // auto a_t = (bool)a.sign;
    // auto b_t = (bool)b.sign;

    // if (a.sign == 1) {
    //     //     //
    //     //     // sss.color_offset_red = -(~((u16)(a.raw + 1)));
    //     //     sss.color_offset_red = -((u16)(~a.raw) + 1);
    // }
    /*using saturnin::utilities::swap_words;
    using saturnin::utilities::swapEndianness;
    using namespace std::literals;

    std::vector<u32>                                   test1;
    std::chrono::time_point<std::chrono::steady_clock> start_time = std::chrono::steady_clock::now();
    std::chrono::duration<double>                      elapsed_time{};
    s32                                                iterations = 1000000;
    u16                                                val        = 0x1234;
    for (int i = 0; i < iterations; ++i) {
        test1.push_back(swapEndianness<u16>(val));
        val++;
    }

    auto res     = (std::chrono::duration_cast<std::chrono::microseconds>(elapsed_time)).count();
    elapsed_time = std::chrono::steady_clock::now() - start_time;
    res          = (std::chrono::duration_cast<std::chrono::microseconds>(elapsed_time)).count();
    core::Log::warning(Logger::main, "swap_endianness {}�s"s, res);*/
    //  TESTING //

    constexpr auto run_bitfields_benchmarks = true;
    if constexpr (run_bitfields_benchmarks) {
        using namespace saturnin::video;

        {
            auto                                               test       = std::vector<u32>{};
            std::chrono::time_point<std::chrono::steady_clock> start_time = std::chrono::steady_clock::now();
            std::chrono::duration<double>                      elapsed_time{};
            constexpr auto                                     iterations{1000000};
            constexpr auto                                     tvmd = TvScreenMode{0b1000000000000011};
            auto                                               val  = u32{};

            for (int i = 0; i < iterations; ++i) {
                if (toEnum<HorizontalResolution>(tvmd.horizontal_resolution) == HorizontalResolution::hi_res_704) { ++val; }
                test.push_back(val);
            }

            elapsed_time = std::chrono::steady_clock::now() - start_time;
            auto res     = (std::chrono::duration_cast<std::chrono::microseconds>(elapsed_time)).count();
            using namespace std::literals;
            core::Log::warning(Logger::main, "Original bitfields {} �s"s, res);
        }

        {
            auto                                               test       = std::vector<u32>{};
            std::chrono::time_point<std::chrono::steady_clock> start_time = std::chrono::steady_clock::now();
            std::chrono::duration<double>                      elapsed_time{};
            constexpr auto                                     iterations{1000000};
            constexpr auto                                     raw = u16{0b1000000000000011};
            auto                                               val = u32{};

            // static_assert(bf::bit_field<3, 0>::get(raw) == 0b011);

            // u8 a = 0;
            // if (bf::bit_field<3, 0, bf::bit_field_config<u16>{}>::get(HorizontalResolution::hi_res_704) == 0b011) { a = 1; }

            // for (int i = 0; i < iterations; ++i) {
            //     if (toEnum<HorizontalResolution>(tvmd.horizontal_resolution) == HorizontalResolution::hi_res_704) { ++val; }
            //     test.push_back(val);
            // }

            elapsed_time = std::chrono::steady_clock::now() - start_time;
            auto res     = (std::chrono::duration_cast<std::chrono::microseconds>(elapsed_time)).count();
            using namespace std::literals;
            core::Log::warning(Logger::main, "Revised bitfields {} �s"s, res);
        }
    }
}

} // namespace saturnin::core
