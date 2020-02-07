//
// locale.h
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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file	locale.h
///
/// \brief	Declares everything related to localization.
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <spirit_po/spirit_po.hpp>

namespace spirit_po {
using default_catalog = catalog<>;
}

namespace saturnin::core {

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class  Locale
///
/// \brief  Handles localization.
///
/// \author Runik
/// \date   21/06/2018
////////////////////////////////////////////////////////////////////////////////////////////////////

class Locale {
  public:
    //@{
    // Constructors / Destructors
    //Locale()              = default;
    Locale(const Locale&) = delete;
    Locale(Locale&&)      = delete;
    Locale& operator=(const Locale&) & = delete;
    Locale& operator=(Locale&&) & = delete;
    ~Locale()                     = default;
    //@}

    static Locale& getInstance();

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

    bool initialize(const std::string& country);

    spirit_po::default_catalog* catalog();


private:
    Locale() = default;

    std::unique_ptr<spirit_po::default_catalog> cat_;
}; // classLocale

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn std::string tr(const std::string& str);
    ///
    /// \brief  Translates the given string.
    ///
    /// \author Runik
    /// \date   23/01/2018
    ///
    /// \param  str The string to translate.
    ///
    /// \return The translated string.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

std::string tr(const std::string& str);

}; // namespace saturnin::core
