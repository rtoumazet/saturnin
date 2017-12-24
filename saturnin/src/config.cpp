// 
// config.cpp
// Saturnin
//
// Copyright (c) 2003-2017 Renaud Toumazet
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
    
    void Config::writeFile(const std::string & filename) {
        cfg_.writeFile(filename.c_str());
    }

    bool Config::readFile(const std::string & filename){
        try {
            cfg_.readFile(filename.c_str());
            return true;
        }
        catch (const FileIOException &fioex) {
            cout << translate("Could not read the configuration file.") << endl;
            return false;
        }
    }

    void Config::writeLegacyOpenGl(const bool value) {
        Setting& root = cfg_.getRoot();
        if (!root.exists("rendering")) root.add("rendering", Setting::TypeGroup);
        Setting& rendering = root["rendering"];
        if (!rendering.exists("legacy_opengl")) rendering.add("legacy_opengl", Setting::TypeBoolean) = value;
        else {
            Setting& legacy_opengl = rendering["legacy_opengl"];
            legacy_opengl = value;
        }
    }

    bool Config::initialize(const bool isModernOpenGlCapable) {
        if (!readFile("saturnin.cfg")) {
            cout << translate("Creating configuration file.") << endl;
            writeFile("saturnin.cfg");
            if (!readFile("saturnin.cfg")) return false;

            writeLegacyOpenGl(!isModernOpenGlCapable);

            writeFile("saturnin.cfg");
        }
        return true;
    }

};
};