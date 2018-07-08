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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file	config.h
///
/// \brief	Manages configuration of the emulator. 
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#pragma warning(disable:4275) // libconfig specific warning disable
#include <libconfig.h++>
#include <iostream>
#include <sstream>

namespace saturnin {
namespace core {

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \class  Config
    ///
    /// \brief  Manages configuration of the emulator.
    ///
    /// \author Runik
    /// \date   21/06/2018
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    class Config {
    public:

        static const std::string group_rendering;
        static const std::string group_paths;

        static const std::string key_legacy_opengl;
        static const std::string key_roms_stv;
        static const std::string key_bios_stv;
        static const std::string key_bios_saturn;

        //@{
        // Constructors / Destructors
        Config()                           = delete;
        Config(const std::string configuration_filename) : filename_(configuration_filename) {};
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

        bool lookup(const std::string& path) const;

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

        void writeFile(const std::string& filename);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn bool Config::readFile(const std::string& filename);
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

        bool readFile(const std::string& filename);

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

        bool initialize(const bool isModernOpenGlCapable);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn libconfig::Setting& Config::getGroup(libconfig::Setting& root, const std::string& group_name);
        ///
        /// \brief  Returns the group under the specified root setting, creates it if missing.
        ///
        /// \author Runik
        /// \date   18/06/2018
        ///
        /// \param [in,out] root        Setting to get the group from.
        /// \param          group_name  Name of the group to get.
        ///
        /// \return A reference to a Setting.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        libconfig::Setting& getGroup(libconfig::Setting& root, const std::string& group_name);

        void test();


        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn void Config::writeValue(libconfig::Setting& root, const std::string& key, const T& value)
        ///
        /// \brief  Writes a value under the specified root setting.
        ///
        /// \author Runik
        /// \date   21/06/2018
        ///
        /// \param [in,out] root    Root setting to write the value to .
        /// \param          key     Key of the value to write.
        /// \param          value   Value to write.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        template <class T> struct ToMySetting;
        template <> struct ToMySetting<bool>        { static constexpr libconfig::Setting::Type index = libconfig::Setting::TypeBoolean; };
        template <> struct ToMySetting<uint32_t>    { static constexpr libconfig::Setting::Type index = libconfig::Setting::TypeInt; };
        template <> struct ToMySetting<uint64_t>    { static constexpr libconfig::Setting::Type index = libconfig::Setting::TypeInt64; };
        template <> struct ToMySetting<float>       { static constexpr libconfig::Setting::Type index = libconfig::Setting::TypeFloat; };
        template <> struct ToMySetting<const char*> { static constexpr libconfig::Setting::Type index = libconfig::Setting::TypeString; };

        template<class T>
        void writeValue(libconfig::Setting& root, const std::string& key, const T& value)
        {
            if (!root.exists(key.c_str())) root.add(key.c_str(), ToMySetting<T>::index) = value;
            else {
                libconfig::Setting& s = root[key.c_str()];
                s = value;
            }
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn void Config::writeValue(libconfig::Setting& root, const std::string& key, const std::string& value)
        ///
        /// \brief  String specilization of write_value.
        ///
        /// \author Runik
        /// \date   21/06/2018
        ///
        /// \param [in,out] root    Root setting to write the value to .
        /// \param          key     Key of the value to write.
        /// \param          value   Value to write.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        void writeValue(libconfig::Setting& root, const std::string& key, const std::string& value)
        {
            if (!root.exists(key.c_str())) root.add(key.c_str(), libconfig::Setting::TypeString) = value.c_str();
            else {
                libconfig::Setting& s = root[key.c_str()];
                s = value.c_str();
            }
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn template<size_t N> void Config::writeValue(libconfig::Setting& root, const std::string& key, const char (&value)[N])
        ///
        /// \brief  Char array specilization of write_value.
        ///
        /// \author Runik
        /// \date   21/06/2018
        ///
        /// \param [in,out] root    Root setting to write the value to .
        /// \param          key     Key of the value to write.
        /// \param          value   Value to write.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        template<size_t N>
        void writeValue(libconfig::Setting& root, const std::string& key, const char (&value)[N])
        {
            if (!root.exists(key.c_str())) root.add(key.c_str(), libconfig::Setting::TypeString) = value;
            else {
                libconfig::Setting& s = root[key.c_str()];
                s = value;
            }
        }

       libconfig::Setting& readValue(const std::string& value);

    private:

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn void Config::generateConfigurationTree(const bool isModernOpenglCapable);
        ///
        /// \brief  Generates the configuration tree with default values in the configuration file.
        ///
        /// \author Runik
        /// \date   21/06/2018
        ///
        /// \param  isModernOpenglCapable   The is modern opengl capable.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        void generateConfigurationTree(const bool isModernOpenglCapable);

        std::string filename_;
        
        libconfig::Config cfg_; ///< Internal configuration object

    };

    template<typename T>
    std::string buildValue(const T& value)
    {
        std::ostringstream oss;
        oss << value;
        return oss.str();
    }

    template<typename T, typename ... Args >
    std::string buildValue(const T& value, const Args& ... args)
    {
        return buildValue(value) + "." + buildValue(args...);
    }
};
};