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

        void Config::write_file(const std::string& filename);

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

        bool Config::read_file(const std::string& filename);

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

        void Config::write_legacy_opengl(const bool value);

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

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn libconfig::Setting&& Config::get_group(libconfig::Setting& root, const std::string& group_name);
        ///
        /// \brief  Adds a group.
        ///
        /// \author Runik
        /// \date   18/06/2018
        ///
        /// \param [in,out] root        Setting to get the group from.
        /// \param          group_name  Name of the group to get.
        ///
        /// \return A reference to a Setting.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        libconfig::Setting& Config::get_group(libconfig::Setting& root, const std::string& group_name);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn void Config::write_value(libconfig::Setting& root, const std::string& key, const std::string& value);
        ///
        /// \brief  Writes a value.
        ///
        /// \author Runik
        /// \date   18/06/2018
        ///
        /// \param [in,out] root    Root setting.
        /// \param          key     Key to modify.
        /// \param          value   Value of the key.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        //void Config::write_value(libconfig::Setting& root, const std::string& key, const std::string& value);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn void Config::write_value(libconfig::Setting&, const std::string&, const bool);
        ///
        /// \brief  Writes a value.
        ///
        /// \author Runik
        /// \date   18/06/2018
        ///
        /// \param [in,out] parameter1  The root.
        /// \param          parameter2  The key.
        /// \param          bool        The value.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        //void Config::write_value(libconfig::Setting&, const std::string&, const bool);

        //template <class T> class ToMySetting {};
        //template <> ToMySetting<bool>  { static libconfig::Setting::Type index = { libconfig::Setting::TypeBoolean }; };
        ////template <> ToMySetting<int> { static libconfig::Setting::Type index = { libconfig::Setting::TypeInt }; };
        ////template <> ToMySetting<char*> { static libconfig::Setting::Type index = { libconfig::Setting::TypeString }; };

        template <class T> struct ToMySetting;
        template <> struct ToMySetting<bool>        { static constexpr libconfig::Setting::Type index = libconfig::Setting::TypeBoolean; };
        template <> struct ToMySetting<uint32_t>    { static constexpr libconfig::Setting::Type index = libconfig::Setting::TypeInt; };
        template <> struct ToMySetting<uint64_t>    { static constexpr libconfig::Setting::Type index = libconfig::Setting::TypeInt64; };
        template <> struct ToMySetting<float>       { static constexpr libconfig::Setting::Type index = libconfig::Setting::TypeFloat; };
        template <> struct ToMySetting<const char*> { static constexpr libconfig::Setting::Type index = libconfig::Setting::TypeString; };

        template<class T>
        void write_value(libconfig::Setting& root, const std::string& key, const T& value)
        {
            if (!root.exists(key.c_str())) root.add(key.c_str(), ToMySetting<T>::index) = value;
            else {
                libconfig::Setting& s = root[key.c_str()];
                s = value;
            }
        }
   
    private:
        libconfig::Config cfg_; ///< Internal configuration object

    };
};
};