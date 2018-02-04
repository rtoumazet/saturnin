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
#include <boost/filesystem.hpp>
#include <boost/dll/import.hpp> // for import_alias
#include "../../dll_log/src/log_plugin.h"
//#include <spdlog/sinks/file_sinks.h>
#include "locale.h"
#include "config.h"
#include "video/opengl.h"
#include "video/opengl_legacy.h"
#include "video/opengl_modern.h"

namespace dll = boost::dll;
using namespace std;
using namespace saturnin::log;
using namespace saturnin::video;
using namespace saturnin::core;

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error %d: %s\n", error, description);
}

int main(int argc, char *argv[])
{
    if ( !Locale::initialize() ) return 1;
    cout << saturnin::core::tr("Hello World") << endl;
    

    
    boost::filesystem::path lib_path(boost::filesystem::current_path());          // argv[1] contains path to directory with our plugin library
    boost::shared_ptr<LogPlugin> plugin;            // variable to hold a pointer to plugin variable
    std::cout << "loading the plugin" << std::endl;
    try {
        plugin = dll::import<LogPlugin>(    // type of imported symbol is located between `<` and `>`
            lib_path / "log",                // path to the library and library name
            "plugin",                           // name of the symbol to import
            dll::load_mode::append_decorations  // makes `libmy_plugin_sum.so` or `my_plugin_sum.dll` from `my_plugin_sum`
            );
        std::cout << "Version:  " << plugin->version() << std::endl;
        plugin->log("test");

    //    auto globalSink = plugin->createSink("logs/global.log");
    //    auto global = plugin->createLogger("global", globalSink);
    //    auto cdrom = plugin->createLogger("cdrom", "logs/cdrom.log", globalSink);
    //    auto vdp1 = plugin->createLogger("vdp1", "logs/vdp1.log", globalSink);

    //    global->info("test global");
    //    cdrom->info("cdrom test");
    //    vdp1->info("vdp1 test");

    }
    catch (const boost::system::system_error& e) {
        std::cout << e.what() << std::endl;
    }




    Config cfg;
    if ( !cfg.initialize(OpenGl::isModernOpenGlCapable()) ) return 1;

    bool isLegacyOpenGl = cfg.lookup("rendering.legacy_opengl");

    if (isLegacyOpenGl) return runLegacyOpenGl(); else return runModernOpenGl();
}