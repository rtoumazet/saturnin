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

namespace video = saturnin::video;
namespace core  = saturnin::core;

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error %d: %s\n", error, description);
}


int main(int argc, char *argv[])
{
    core::Emulator_context state;
    if (!state.config()->initialize(video::isModernOpenglCapable())) return 1;

    core::Log::initialize();

    std::string country = state.config()->readValue(core::Access_keys::config_language);
    if ( !core::Locale::initialize(country) ) return 1;
   
    std::cout << core::tr("Hello world.") << std::endl;




    state.memory()->loadBios(core::Hardware_mode::saturn);

    //state.memory()->loadStvGame("astra_superstars.cfg");

    core::write<32>(state.memory()->workram_low, 0, 0x12345678);
    auto val = core::read<16>(state.memory()->workram_low, 2);

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


    bool is_legacy_opengl = state.config()->readValue(core::Access_keys::config_legacy_opengl);
    if (is_legacy_opengl) return video::runLegacyOpengl(state.config()); else return video::runModernOpengl(state.config());
  
};