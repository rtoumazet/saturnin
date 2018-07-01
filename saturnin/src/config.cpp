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
            
            cout << tr("Could not read the configuration file: ") << fioex.what() << endl;
            return false;
        }
    }

    bool Config::initialize(const bool isModernOpenGlCapable) {
        if (!readFile(filename_)) {
            cout << tr("Creating configuration file.") << endl;
            writeFile(filename_);
            if (!readFile(filename_)) return false;

            generateConfigurationTree(isModernOpenGlCapable);

            writeFile(filename_);
        }
        return true;
    }

    void Config::generateConfigurationTree(const bool isModernOpenglCapable) {
        Setting& root = cfg_.getRoot();
        
        Setting& rendering = root.add("rendering", Setting::TypeGroup);
        writeValue(rendering, "legacy_opengl", !isModernOpenglCapable);

        Setting& paths = root.add("paths", Setting::TypeGroup);
        writeValue(paths, "roms_stv", "");
        writeValue(paths, "bios_stv", "");
        writeValue(paths, "bios_saturn", "");
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

    Setting& Config::readValue(const std::string& value) {
        try {
            return cfg_.lookup(value.c_str());
        }
        catch (const SettingNotFoundException& e) {
            
            auto errorString = fmt::format(tr("Setting '{0}' not found !"), e.getPath());
            Log::error("config", errorString);
            Log::error("config", tr("Exiting ..."));

            std::exit(EXIT_FAILURE);
        }

        //try {
        //    bool is_legacy_opengl = cfg.readValue("rendering.legacy_opengl");
        //    if (is_legacy_opengl) return run_legacy_opengl(); else return run_modern_opengl();
        //}
        //catch (const  SettingNotFoundException& e) {
        //    cout << tr("Setting not found: ") << e.what() << endl;

        //}
    }
};
};