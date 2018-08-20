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
   
    Log::initialize();

    Emulator_context state;
   if (!state.config()->initialize(isModernOpenglCapable())) return 1;

   state.memory()->loadBios(Hardware_mode::saturn);
   //state.memory()->loadRom("astrass", "EPR20825.13", &state.memory()->cart[0], 0x100000, Rom_load::odd_interleaved, 1, Rom_type::program);
   //state.memory()->loadRom("astrass", "EPR20825.13", &state.memory()->cart[0], 0x100000, Rom_load::not_interleaved, 1, Rom_type::program);
   state.memory()->loadRom("astrass", "EPR20825.13", &state.memory()->cart[0], 0x100000, Rom_load::odd_interleaved, 1, Rom_type::program);
   //state.memory()->loadRom("astrass", "EPR20825.13", &state.memory()->cart[0], 0x100000, Rom_load::even_interleaved, 1, Rom_type::program);

    write<32>(state.memory()->workram_low, 0, 0x12345678);
    auto val = read<16>(state.memory()->workram_low, 2);

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


    bool is_legacy_opengl = state.config()->readValue(Config_keys::legacy_opengl);
    if (is_legacy_opengl) return runLegacyOpengl(); else return runModernOpengl();
  
};