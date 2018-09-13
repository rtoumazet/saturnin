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
//#define LIBCONFIG_STATIC
//#include <Shlwapi.h>
#include <libconfig.h++>
#include <iostream>
#include <map>
#include <sstream>
#include <string> // string
#include "log.h"
#include "memory.h" // Rom_load, Rom_type

namespace saturnin {
namespace core {

    enum class Access_keys {
        config_global,
        config_language,
        config_rendering,
        config_legacy_opengl,
        config_paths,
        config_roms_stv,
        config_bios_stv,
        config_bios_saturn,
        stv_game_name,
        stv_zip_name,
        stv_parent_set,
        stv_version,
        stv_release_date,
        stv_region,
        stv_files
    };

    enum class Rom_load;
    enum class Rom_type;

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

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \typedef    std::map<Access_keys, const char *> Map_keys
        ///
        /// \brief  Defines an alias representing the link between enumerators and keys.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        typedef std::map<Access_keys, const char *>Map_keys;
        static Map_keys single_keys; ///< Contains the keys alone (without their path).
        static Map_keys full_keys;  ///< Contains the keys with their full path.

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \typedef    std::map<const std::string, const uint8_t>Map_rom_load
        ///
        /// \brief  Defines an alias representing the correspondance between the rom load string value
        ///         defined in the config file and the rom load type.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        typedef std::map<const std::string, const Rom_load>Map_rom_load;
        static Map_rom_load rom_load;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \typedef    std::map<const std::string, const Rom_type>Map_rom_type
        ///
        /// \brief  Defines an alias representing the correspondance between the rom type string value
        ///         defined in the config file and the rom type type.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        typedef std::map<const std::string, const Rom_type>Map_rom_type;
        static Map_rom_type rom_type;

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
        /// \fn void Config::writeFile();
        ///
        /// \brief  Writes the current configuration to the file linked to the object.
        ///
        /// \author Runik
        /// \date   13/09/2018
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        void writeFile();

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn bool Config::readFile();
        ///
        /// \brief  Reads the configuration file linked to the object.
        ///
        /// \author Runik
        /// \date   24/12/2017
        ///
        /// \return True if it succeeds.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        bool readFile();

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

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn void Config::writeValue(const std::string& key, const T& value)
        ///
        /// \brief  Writes a value for the specified key.
        ///
        /// \author Runik
        /// \date   21/06/2018
        ///
        /// \param          key     Path of the value to write.
        /// \param          value   Value to write.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        template<class T>
        void writeValue(const Access_keys& key, const T& value) {
            try {
                libconfig::Setting &setting = cfg_.lookup(Config::full_keys[key]);
                setting = value;
            }
            catch (const libconfig::SettingNotFoundException& e) {
                std::string s = e.getPath();
                auto errorString = fmt::format(tr("Setting '{0}' not found !"), e.getPath());
            //    //Log::error("config", errorString);
            //    //Log::error("config", tr("Exiting ..."));

            //    std::exit(EXIT_FAILURE);
            }
            //catch (const libconfig::SettingTypeException& e) {

            //    //auto errorString = fmt::format(tr("Setting '{0}' using the wrong type !"), e.getPath());
            //    //Log::error("config", errorString);
            //    //Log::error("config", tr("Exiting ..."));

            //    std::exit(EXIT_FAILURE);
            //}
        }

       ////////////////////////////////////////////////////////////////////////////////////////////////////
       /// \fn  libconfig::Setting& Config::readValue(const Access_keys& value);
       ///
       /// \brief   Reads a value from the specified root setting.
       ///
       /// \author  Runik
       /// \date    28/08/2018
       ///
       /// \param   value   key of the value to read.
       ///
       /// \return  Read value.
       ////////////////////////////////////////////////////////////////////////////////////////////////////

       libconfig::Setting& readValue(const Access_keys& value);

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

        std::string filename_;  ///< Name of the configuration file used
        
        libconfig::Config cfg_; ///< Internal configuration object

    };
};
};