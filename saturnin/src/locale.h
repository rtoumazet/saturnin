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

#include <memory>
#include <string>

// Forward declaration
namespace spiritless_po {
class Catalog;
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

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto Locale::catalog() -> spiritless_po::Catalog*;
    ///
    /// \brief  Gets the catalog
    ///
    /// \author Runik
    /// \date   17/01/2021
    ///
    /// \returns    Null if it fails, else a pointer to a spiritless_po::Catalog.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto catalog() -> spiritless_po::Catalog*;

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

    std::unique_ptr<spiritless_po::Catalog> cat_; ///< Catalog of translated strings

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
/// \returns    The translated string.
////////////////////////////////////////////////////////////////////////////////////////////////////

auto tr(const std::string& str) -> std::string;

}; // namespace saturnin::core
