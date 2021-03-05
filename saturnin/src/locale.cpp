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

#include <saturnin/src/pch.h>
#include <saturnin/src/locale.h> // NOLINT(modernize-deprecated-headers)
#include <fstream>               // ifstream
#include <string>
#include <saturnin/lib/spiritless_po/include/spiritless_po/spiritless_po.h>

namespace saturnin::core {

auto Locale::getInstance() -> Locale& {
    static Locale instance;
    return instance;
}

auto Locale::catalog() -> spiritless_po::Catalog* { return cat_.get(); };

auto Locale::initialize(const std::string& country = "") -> bool { // NOLINT(readability-convert-member-functions-to-static)
    auto filename = std::string("./lang/" + country + "/saturnin.po");
    auto ifs      = std::ifstream(filename);

    cat_ = std::make_unique<spiritless_po::Catalog>();
    if (!cat_->Add(ifs)) {
        // for (const auto& s : cat_->GetError()) {
        //    cerr << argv[ii + 1] << ": " << s << endl;
        //}
    }

    return true;
}

auto tr(const std::string& str) -> std::string { return Locale::getInstance().catalog()->gettext(str); }

}; // namespace saturnin::core
