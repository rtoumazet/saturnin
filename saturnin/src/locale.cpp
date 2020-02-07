//
// locale.cpp
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
#include <fstream>
#include <string>
#include "locale.h"

namespace fs = std::filesystem;

namespace saturnin::core {

Locale& Locale::getInstance() {
    static Locale instance;
    return instance;
}

spirit_po::default_catalog* Locale::catalog() { return cat_.get(); };

bool Locale::initialize(const std::string& country = "") {
    std::ifstream ifs("./lang/" + country + "/saturnin.po");
    std::string   po_file{std::istreambuf_iterator<char>{ifs}, std::istreambuf_iterator<char>()};

    // spirit_po::catalog<> cat{ spirit_po::catalog<>::from_range(po_file) };
    //    cat_ = spirit_po::catalog<>::from_range(po_file);
    cat_ = std::make_unique<spirit_po::default_catalog>(spirit_po::catalog<>::from_range(po_file));
    return true;
}

std::string tr(const std::string& str) {
    // return locale::translate(str);
    // return cat_.get()->gettext(str.c_str());
    return Locale::getInstance().catalog()->gettext_str(str);
    //return "";
}

}; // namespace saturnin::core
