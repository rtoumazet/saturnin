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
#include "locale.h" // NOLINT(modernize-deprecated-headers)

namespace fs = std::filesystem;

namespace saturnin::core {

auto Locale::getInstance() -> Locale& {
    static Locale instance;
    return instance;
}

auto Locale::catalog() -> spirit_po::default_catalog* { return cat_.get(); };

auto Locale::initialize(const std::string& country = "") -> bool { // NOLINT(readability-convert-member-functions-to-static)
    std::ifstream ifs("./lang/" + country + "/saturnin.po");
    std::string   po_file{std::istreambuf_iterator<char>{ifs}, std::istreambuf_iterator<char>()};

    cat_ = std::make_unique<spirit_po::default_catalog>(spirit_po::catalog<>::from_range(po_file));
    return true;
}

auto tr(const std::string& str) -> std::string { return Locale::getInstance().catalog()->gettext_str(str); }

}; // namespace saturnin::core
