// 
// locale.h
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

#pragma once

#pragma warning(disable:4275) // libconfig specific warning disable
#include <libconfig.h++>

namespace saturnin {
namespace core {
    class Locale {
    public:

        //@{
        // Constructors / Destructors
        Locale()                           = delete;
        Locale(const Locale&)              = delete;
        Locale(Locale&&)                   = delete;
        Locale& operator=(const Locale&) & = delete;
        Locale& operator=(Locale&&) &      = delete;
        ~Locale()                          = delete;
        //@}

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn static bool Locale::initialize();
        ///
        /// \brief  Initializes the locales.
        ///
        /// \author Runik
        /// \date   24/12/2017
        ///
        /// \return True if it succeeds.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        static bool initialize();
    };
};
};