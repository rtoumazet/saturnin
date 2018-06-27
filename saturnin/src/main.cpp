// 
// main.cpp
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

#include <iostream> // cout
#include "locale.h"
#include "config.h"
#include "log.h"
#include "video/opengl.h"
#include "emulator_context.h"
#include "memory.h"
//#include "video/opengl_legacy.h"
//#include "video/opengl_modern.h"

using namespace std;
using namespace saturnin::video;
using namespace saturnin::core;

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error %d: %s\n", error, description);
}


int main(int argc, char *argv[])
{
    if ( !Locale::initialize() ) return 1;
   
    std::cout << tr("Hello World") << std::endl;
    Log::initialize();

    //spdlog::get("global")->debug("global test");
    //spdlog::get("cdrom")->error("cdrom test");
    spdlog::get("vdp1")->warn("vdp1 test");
    Log::error("vdp2", "Error {}", 1);
    Log::error("sound", "snd");
    Log::warning("vdp1", "snd");

    Emulator_context emu_state;

    write<32>(emu_state.memory->workram_low, 0, 0x12345678);
    auto val = read<16>(emu_state.memory->workram_low, 2);

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

    Config cfg("saturnin.cfg");
    if ( !cfg.initialize(isModernOpenglCapable()) ) return 1;
    
    bool is_legacy_opengl = cfg.readValue("rendering.legacy_opengl");
    if (is_legacy_opengl) return runLegacyOpengl(); else return runModernOpengl();
  
};