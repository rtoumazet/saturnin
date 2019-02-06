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

#include "emulator_context.h"
#include "cdrom/scsi.h"
#include "video/opengl.h"
#include "scu_registers.h"

namespace cdrom = saturnin::cdrom;
namespace video = saturnin::video;

namespace saturnin {
namespace core {

Emulator_context::Emulator_context() {
    config_     = std::make_unique<Config>("saturnin.cfg");
    master_sh2_ = std::make_unique<Sh2>(true);
    slave_sh2_  = std::make_unique<Sh2>(false);
    memory_     = std::make_unique<Memory>(config_.get(), 
                                           hardwareMode_, 
                                           master_sh2_.get(),
                                           slave_sh2_.get());
    scu_        = std::make_unique<Scu>(memory_.get());
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
    //core::writeDummy<8>(0x12345678, 0x0);
    //core::writeDummy<32>(0x12345678, 0x0);
    //core::readDummy<16>(0x0);
    //core::readDummy<32>(0x0);

    auto blah = this->memory()->read<uint8_t>(0);

    auto isr = InterruptStatusRegister(0x000000AB);
    auto w = isr.test();
    
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