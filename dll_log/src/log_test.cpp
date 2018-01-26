// LogTest.cpp : définit le point d'entrée pour l'application console.
//

#include "stdafx.h"
#include <cstdlib> // EXIT_FAILURE
#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/dll/import.hpp> // for import_alias
#include "../LogDll/src/base_plugin_api.h"

namespace dll = boost::dll;

enum TransferType {
    PROGRAM = 0,
    DATA    = 1
};

int main(int argc, char* argv[])
{
    boost::filesystem::path lib_path(boost::filesystem::current_path());          // argv[1] contains path to directory with our plugin library
    boost::shared_ptr<my_plugin_api> plugin;            // variable to hold a pointer to plugin variable
    std::cout << "loading the plugin" << std::endl;
    try {
        plugin = dll::import<my_plugin_api>(    // type of imported symbol is located between `<` and `>`
            lib_path / "LogDll",                // path to the library and library name
            "plugin",                           // name of the symbol to import
            dll::load_mode::append_decorations  // makes `libmy_plugin_sum.so` or `my_plugin_sum.dll` from `my_plugin_sum`
            );
        std::cout << "Version:  " << plugin->version() << std::endl;
        plugin->log("test");

        auto globalSink = plugin->createSink("logs/global.log");
        auto global     = plugin->createLogger("global", globalSink);
        auto cdrom      = plugin->createLogger("cdrom", "logs/cdrom.log", globalSink);
        auto vdp1       = plugin->createLogger("vdp1", "logs/vdp1.log", globalSink);

        global->info("test global");
        cdrom->info("cdrom test");
        vdp1->info("vdp1 test");

        std::map<std::string, TransferType> transferMap;
        transferMap["PROGRAM"]  = TransferType::PROGRAM;
        transferMap["DATA"]     = TransferType::DATA;

        std::cout << "Program : " << transferMap["PROGRAM"] << std::endl;
        std::cout << "Data: " << transferMap["DATA"] << std::endl;
    }
    catch (const boost::system::system_error& e) {
        std::cout << e.what() << std::endl;
    }

    
    return EXIT_SUCCESS;
}

