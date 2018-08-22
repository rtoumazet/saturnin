// 
// config.cpp
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
#include "config.h"
#include "locale.h"
#include "log.h"


using namespace libconfig;
using namespace std;

namespace saturnin {
namespace core {
    Config::Map_keys Config::keys_write = {
        { Access_keys::config_rendering,     "rendering"},
        { Access_keys::config_legacy_opengl, "legacy_opengl" },
        { Access_keys::config_paths,         "paths" },
        { Access_keys::config_roms_stv,      "roms_stv" },
        { Access_keys::config_bios_stv,      "bios_stv" },
        { Access_keys::config_bios_saturn,   "bios_saturn" },
    };
    Config::Map_keys Config::keys_read = {
        { Access_keys::config_rendering,     "rendering" },
        { Access_keys::config_legacy_opengl, "rendering.legacy_opengl" },
        { Access_keys::config_paths,         "paths" },
        { Access_keys::config_roms_stv,      "paths.roms_stv" },
        { Access_keys::config_bios_stv,      "paths.bios_stv" },
        { Access_keys::config_bios_saturn,   "paths.bios_saturn" },
        { Access_keys::stv_game_name,        "game_name" },
        { Access_keys::stv_zip_name,         "zip_name" },
        { Access_keys::stv_parent_set,       "parent_set" },
        { Access_keys::stv_version,          "version" },
        { Access_keys::stv_release_date,     "release_date" },
        { Access_keys::stv_region,           "region" },
        { Access_keys::stv_files,            "files" }


    };

    bool Config::lookup(const std::string& path) const {
        return cfg_.lookup(path.c_str()); // c_str() is needed, method will fail with a string
    }
    
    void Config::writeFile(const std::string & filename) {
        cfg_.writeFile(filename.c_str());
    }

    bool Config::readFile(const std::string & filename){
        try {
            cfg_.readFile(filename.c_str());
            return true;
        }
        catch (const FileIOException &fioex) {
            
            cout << fmt::format(tr("Could not read file {0} : ")) << fioex.what() << endl;
            return false;
        }
    }

    bool Config::initialize(const bool isModernOpenGlCapable) {
        if (!readFile(filename_)) {
            cout << tr("Creating configuration file.") << endl;
            generateConfigurationTree(isModernOpenGlCapable);
            writeFile(filename_);
        }
        return true;
    }

    void Config::generateConfigurationTree(const bool isModernOpenglCapable) {
        Setting& root = cfg_.getRoot();

        Setting& rendering = root.add(keys_write[Access_keys::config_rendering], Setting::TypeGroup);
        writeValue(rendering, keys_write[Access_keys::config_legacy_opengl], !isModernOpenglCapable);

        Setting& paths = root.add(keys_write[Access_keys::config_paths], Setting::TypeGroup);
        writeValue(paths, keys_write[Access_keys::config_roms_stv], "");
        writeValue(paths, keys_write[Access_keys::config_bios_stv], "");
        writeValue(paths, keys_write[Access_keys::config_bios_saturn], "");
    }

    Setting& Config::getGroup(Setting& root, const string& group_name) {
        if (!root.exists(group_name.c_str())) root.add(group_name.c_str(), Setting::TypeGroup);
        return root[group_name.c_str()];
    }

    void Config::test() {
        Setting& root = cfg_.getRoot();
        std::string str{"test"};
        
        writeValue(root, "test_c_string", str.c_str());
        writeValue(root, "test_string", std::string{ "test" });
        writeValue(root, "test_char_array", "test");

        writeFile(filename_);
    }

    //Setting& Config::readValue(const std::string& value) {
    Setting& Config::readValue(const Access_keys& value) {
        try {
            string key{ Config::keys_read[value] };
            return cfg_.lookup(key.c_str());
        }
        catch (const SettingNotFoundException& e) {
            
            auto errorString = fmt::format(tr("Setting '{0}' not found !"), e.getPath());
            Log::error("config", errorString);
            Log::error("config", tr("Exiting ..."));

            std::exit(EXIT_FAILURE);
        }
    }
};
};