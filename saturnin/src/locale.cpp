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

namespace saturnin {
namespace core {

//spirit_po::catalog<> Locale::cat_ = std::make_unique<spirit_po::catalog<>>();

/* static */
bool Locale::initialize(const std::string& country = "") {
    std::ifstream ifs("./lang/" + country + "/saturnin.po");
    std::string   po_file{std::istreambuf_iterator<char>{ifs}, std::istreambuf_iterator<char>()};

    // spirit_po::catalog<> cat{ spirit_po::catalog<>::from_range(po_file) };
//    cat_ = spirit_po::catalog<>::from_range(po_file);

    return true;
}

std::string tr(const std::string& str) {
    // return locale::translate(str);
    return "";
}

}; // namespace core
}; // namespace saturnin
