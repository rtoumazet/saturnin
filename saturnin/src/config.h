// 
// config.h
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

#pragma once

#pragma warning(disable:4275) // libconfig specific warning disable
#include <libconfig.h++>

namespace saturnin {
namespace core {
    class Config {
    public:

        //@{
        // Constructors / Destructors
        Config()                           = default;
        Config(const Config&)              = delete;
        Config(Config&&)                   = delete; 
        Config& operator=(const Config&) & = delete;
        Config& operator=(Config&&) &      = delete;
        ~Config()                          = default;
        //@}

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn bool Config::lookup(const std::string& path) const;
        ///
        /// \brief  Looks up a given path to find its associated boolean value.
        ///
        /// \author Runik
        /// \date   23/12/2017
        ///
        /// \param  path    Path to the value.
        ///
        /// \return Read value.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        bool Config::lookup(const std::string& path) const;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn void Config::writeFile(const std::string& filename);
        ///
        /// \brief  Writes a file.
        ///
        /// \author Runik
        /// \date   24/12/2017
        ///
        /// \param  filename    Filename of the file.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        void Config::writeFile(const std::string& filename);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn bool Config::readFile(const std::string& filename) const;
        ///
        /// \brief  Reads a file.
        ///
        /// \author Runik
        /// \date   24/12/2017
        ///
        /// \param  filename    Filename of the file.
        ///
        /// \return True if it succeeds.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        bool Config::readFile(const std::string& filename);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn void Config::writeLegacyOpenGl(const bool value);
        ///
        /// \brief  Writes the legacy_opengl option value.
        ///
        /// \author Runik
        /// \date   24/12/2017
        ///
        /// \param  value   The value to write.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        void Config::writeLegacyOpenGl(const bool value);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn bool Config::initialize(const bool isModernOpenGlCapable);
        ///
        /// \brief  Initializes the configuration by reading 'saturnin.cfg'. The file is created if missing
        ///
        /// \author Runik
        /// \date   24/12/2017
        ///
        /// \param  isModernOpenGlCapable   True if the graphics card supports modern Opengl.
        ///
        /// \return True if it succeeds.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        bool Config::initialize(const bool isModernOpenGlCapable);

    
    private:
        libconfig::Config cfg_; ///< Internal configuration object

    };
};
};