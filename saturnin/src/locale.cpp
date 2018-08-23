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

#include <iostream>                // cout
#include <boost/locale.hpp>        // generator
#include "locale.h"

namespace locale = boost::locale;

namespace saturnin {
namespace core {

    /* static */
    bool Locale::initialize()
    {
        locale::generator gen;
        // Specify location of dictionaries
        gen.add_messages_path("./lang");
        gen.add_messages_domain("saturnin");
        // Generate locales and imbue them to iostream
        std::locale::global(gen("fr"));
        std::cout.imbue(std::locale());

        return true;
    }

    std::string tr(const std::string& str) {
        return locale::translate(str);
    }

};
};