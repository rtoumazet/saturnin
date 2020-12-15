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

#include <saturnin/src/locale.h> // NOLINT(modernize-deprecated-headers)

#include <fstream> // ifstream
#include <string>

namespace saturnin::core {

auto Locale::getInstance() -> Locale& {
    static Locale instance;
    return instance;
}

auto Locale::catalog() -> spirit_po::default_catalog* { return cat_.get(); };

auto Locale::initialize(const std::string& country = "") -> bool { // NOLINT(readability-convert-member-functions-to-static)
    auto ifs     = std::ifstream("./lang/" + country + "/saturnin.po");
    auto po_file = std::string{std::istreambuf_iterator<char>{ifs}, std::istreambuf_iterator<char>()};

    cat_ = std::make_unique<spirit_po::default_catalog>(spirit_po::catalog<>::from_range(po_file));
    return true;
}

// auto tr(const std::string& str) -> std::string { return Locale::getInstance().catalog()->gettext_str(str); }

auto tr(const std::string& str) -> const char* { return Locale::getInstance().catalog()->gettext(str.c_str()); }

}; // namespace saturnin::core
