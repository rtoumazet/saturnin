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
    Config::Config() {
        
        try {
            cfg_.readFile("saturnin.cfg");
        }
        catch (const FileIOException &fioex) {
            cout << translate("Could not read 'saturnin.cfg', file will be created");
            cfg_.clear();
            Setting& root = cfg_.getRoot();
            root.add("rendering", Setting::TypeGroup);
            Setting& rendering = root["rendering"];
            rendering.add("legacy_opengl", Setting::TypeBoolean) = false;
            cfg_.writeFile("saturnin.cfg");
            
        }
    };

    bool Config::lookup(const std::string& path) const {
        return cfg_.lookup(path.c_str()); // c_str() is needed, method will fail with a string
    }
};
};