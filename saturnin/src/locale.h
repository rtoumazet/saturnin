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

#include <saturnin/src/pch.h>
//#pragma warning(push)
//#pragma warning(disable : 4267)
//#include <spirit_po/spirit_po.hpp>
//#pragma warning(pop)

namespace spirit_po {
using default_catalog = catalog<>;
} // namespace spirit_po

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
    // Locale()              = default;
    Locale(const Locale&) = delete;
    Locale(Locale&&)      = delete;
    auto operator=(const Locale&) & -> Locale& = delete;
    auto operator=(Locale&&) & -> Locale& = delete;
    ~Locale()                             = default;
    //@}

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static auto Locale::getInstance() -> Locale&;
    ///
    /// \brief  Gets the singleton instance.
    ///
    /// \author Runik
    /// \date   07/02/2020
    ///
    /// \return The instance.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static auto getInstance() -> Locale&;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static auto Locale::initialize(const std::string& country) -> bool;
    ///
    /// \brief  Initializes the locales.
    ///
    /// \author Runik
    /// \date   24/12/2017
    ///
    /// \return True if it succeeds.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto initialize(const std::string& country) -> bool;

    auto catalog() -> spirit_po::default_catalog*;

  private:
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn Locale::Locale() = default;
    ///
    /// \brief  Default constructor.
    ///
    /// \author Runik
    /// \date   07/02/2020
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    Locale() = default;

    std::unique_ptr<spirit_po::default_catalog> cat_; ///< Catalog of translated strings

}; // class Locale

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn auto tr(const std::string& str) -> std::string;
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

// auto tr(const std::string& str) -> std::string;

auto tr(const std::string& str) -> const char*;

}; // namespace saturnin::core
