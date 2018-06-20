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
#include <boost/locale.hpp> // translate
#include "config.h"

using namespace boost::locale;
using namespace libconfig;
using namespace std;

namespace saturnin {
namespace core {
    bool Config::lookup(const std::string& path) const {
        return cfg_.lookup(path.c_str()); // c_str() is needed, method will fail with a string
    }
    
    void Config::write_file(const std::string & filename) {
        cfg_.writeFile(filename.c_str());
    }

    bool Config::read_file(const std::string & filename){
        try {
            cfg_.readFile(filename.c_str());
            return true;
        }
        catch (const FileIOException &fioex) {
            cout << translate("Could not read the configuration file.") << endl;
            return false;
        }
    }

    void Config::write_legacy_opengl(const bool value) {
        Setting& rendering = get_group(cfg_.getRoot(), "rendering");
        
        
        //if (!rendering.exists("legacy_opengl")) rendering.add("legacy_opengl", Setting::TypeBoolean) = value;
        //else {
        //    Setting& legacy_opengl = rendering["legacy_opengl"];
        //    legacy_opengl = value;
        //}
        write_value(rendering, "legacy_opengl", value);
        
        //string str{ "str" };
        //write_value(rendering, "test_string", str.c_str());

    }

    bool Config::initialize(const bool isModernOpenGlCapable) {
        if (!read_file("saturnin.cfg")) {
            cout << translate("Creating configuration file.") << endl;
            write_file("saturnin.cfg");
            if (!read_file("saturnin.cfg")) return false;

            // generating parameters skeleton
            Setting& root = cfg_.getRoot();
            root.add("rendering", Setting::TypeGroup);

            write_legacy_opengl(!isModernOpenGlCapable);

            write_file("saturnin.cfg");
        }
        return true;
    }

    Setting& Config::get_group(Setting& root, const string& group_name) {
        if (!root.exists(group_name.c_str())) root.add(group_name.c_str(), Setting::TypeGroup);
        return root[group_name.c_str()];
    }

    void Config::test() {
        Setting& root = cfg_.getRoot();
        std::string str{"test"};
        
        //write_value(root, "test_string", str);
        write_value(root, "test_c_string", str.c_str());
        write_value(root, "test_string", std::string{ "test" });
        write_value(root, "test_char_array", "test");

        write_file("saturnin.cfg");
    }

    //void Config::write_value(Setting& root, const string& key, const string& value) {
    //    if (!root.exists(key.c_str())) root.add(key.c_str(), Setting::TypeString) = value;
    //    else {
    //        Setting& s = root[key.c_str()];
    //        s = value;
    //    }

    //}

    //void Config::write_value(Setting& root, const string& key, const bool value) {
    //    if (!root.exists(key.c_str())) root.add(key.c_str(), Setting::TypeBoolean) = value;
    //    else {
    //        Setting& s = root[key.c_str()];
    //        s = value;
    //    }
    //}

};
};