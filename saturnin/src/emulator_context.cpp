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
#include "emulator_context.h"
#include "cdrom/scsi.h"
#include "video/opengl.h"
#include "scu_registers.h"
#include "sh2_registers.h"


namespace cdrom = saturnin::cdrom;
namespace video = saturnin::video;
namespace sh2 = saturnin::sh2;

namespace saturnin {
namespace core {

Emulator_context::Emulator_context() {
    config_     = std::make_unique<Config>("saturnin.cfg");
    master_sh2_ = std::make_unique<sh2::Sh2>(sh2::Sh2Type::master, this);
    slave_sh2_  = std::make_unique<sh2::Sh2>(sh2::Sh2Type::slave, this);
    memory_     = std::make_unique<Memory>(this);
    scu_        = std::make_unique<Scu>(this);
}

bool Emulator_context::initialize() {
    core::Log::initialize();

    if (!this->config()->initialize(video::isModernOpenglCapable())) return false;

    std::string country = this->config()->readValue(core::Access_keys::config_language);
    if (!core::Locale::initialize(country)) return false;

    cdrom::Scsi::settingUpSptiFunctions();
    if (!cdrom::Scsi::initialize()) return false;

    return true;
}

bool Emulator_context::run() {
    this->memory()->loadBios(core::HardwareMode::saturn);

    sh2::initializeOpcodesLut();

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

    std::cout << core::tr("Hello world.") << std::endl;
    core::rawWrite<uint32_t>(this->memory()->workram_low_, 0, 0x12345678);
    auto val = core::rawRead<uint16_t>(this->memory()->workram_low_, 2);
    core::rawWrite<uint32_t>(this->memory()->rom_, 0, 0x12345678);

    auto blah = this->memory()->read<uint8_t>(0);

    Log::error("sh2", "Unexpected opcode({} SH2)\nOpcode: {:#06x}\nPC: {:#010x}", "Master", 0x4e73, 0x20000200);

    sh2::StatusRegister sr{ 0 };
    sr.set(sh2::StatusRegister::i);
    u8 i = sr.get(sh2::StatusRegister::i);
    sr.set(sh2::StatusRegister::i, static_cast<u8>(0x8));
    i = sr.get(sh2::StatusRegister::i);

    auto dmr = DmaModeRegister(0x000000AA);
    //auto w = isr.test();
    //auto y = isr.testRange();
    dmr.set(DmaModeRegister::startingFactorSelect);
    dmr.reset(DmaModeRegister::startingFactorSelect);
    //isr.set(InterruptStatusRegister::bBus, StartingFactorSelect::timer_1);
    
    {
        core::rawWrite<uint32_t>(this->memory()->scu_, 0, 0x00112233);
        auto start = std::chrono::steady_clock::now();

        for (uint32_t i = 0; i < 1000000; ++i) {
            auto dmr = DmaModeRegister(core::rawRead<uint32_t>(this->memory()->scu_, 0));
            auto t = dmr.get(DmaModeRegister::startingFactorSelect);
            //dmr.set(DmaModeRegister::startingFactorSelect, StartingFactorSelect::h_blank_in);
            //core::rawWrite<uint32_t>(this->memory()->scu_, 0, dmr.toUlong());
            
            //auto dmr2 = DmaModeRegister(core::rawRead<uint32_t>(this->memory()->scu_, 0));
            //dmr.reset(DmaModeRegister::startingFactorSelect);
            //core::rawWrite<uint32_t>(this->memory()->scu_, 0, dmr.toUlong());
        }
        auto end = std::chrono::steady_clock::now();
        auto diff = end - start;
        std::cout << std::chrono::duration <double, std::milli>(diff).count() << " ms" << std::endl;
    }

    {
        core::rawWrite<uint32_t>(this->memory()->scu_, 0, 0x00112233);
        auto start = std::chrono::steady_clock::now();

        for (uint32_t i = 0; i < 1000000; ++i) {
            uint32_t dmr = core::rawRead<uint32_t>(this->memory()->scu_, 0);

            auto t = static_cast<StartingFactorSelect>(dmr & 0x7);
            //dmr &= 0xFFFFFFF8;
            //dmr |= static_cast<uint32_t>(StartingFactorSelect::h_blank_in);
            //core::rawWrite<uint32_t>(this->memory()->scu_, 0, dmr);

            //uint32_t dmr2 = core::rawRead<uint32_t>(this->memory()->scu_, 0);
            //dmr &= 0xFFFFFFF8;
            //core::rawWrite<uint32_t>(this->memory()->scu_, 0, dmr);
        }
        auto end = std::chrono::steady_clock::now();
        auto diff = end - start;
        std::cout << std::chrono::duration <double, std::milli>(diff).count() << " ms" << std::endl;
    }

    scu_->dmaTest();

    s32 a = 0x01234567;
    s32 b = 0x89abcdef;
    //s64 dividend = static_cast<u64>((static_cast<u64>(a) << 32) | static_cast<u64>(b));
    s64 dividend = (static_cast<s64>(a) << 32) | (b & 0xffffffff);
  
    // TESTING //

    uint8_t status{};
    while (this->renderingStatus_ != RenderingStatus::stopped) {
        this->renderingStatus_ = RenderingStatus::running;
        bool is_legacy_opengl = this->config()->readValue(core::Access_keys::config_legacy_opengl);
        status = (is_legacy_opengl) ? video::runLegacyOpengl(*this) : video::runModernOpengl(*this);

    }
    return status;
}

}
}