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
#include <any>
#include <iostream>
#include <map>
#include <optional>
#include <sstream>
#include <string> // string
#include <vector> // vector
#include "log.h"
#include "memory.h" // RomLoad, RomType
#include "utilities.h" // toUnderlying
#include "cdrom/cdrom.h" // CdromAccessMethod

namespace cdrom = saturnin::cdrom;
namespace libcfg = libconfig;
namespace util = saturnin::utilities;

namespace saturnin {
namespace core {

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \enum   AccessKeys
    ///
    /// \brief  Keys used for accessing configuration data.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    enum class AccessKeys {
        cfg_global_language,
        cfg_global_hardware_mode,
        cfg_rendering_legacy_opengl,
        cfg_paths_roms_stv,
        cfg_paths_bios_stv,
        cfg_paths_bios_saturn,
        cfg_cdrom_drive,
        cfg_cdrom_access_method,
        cfg_sound_soundcard,
        cfg_sound_disabled,
        cfg_controls_saturn_player_1,
        cfg_controls_saturn_player_2,
        cfg_controls_stv_board,
        cfg_controls_stv_player_1,
        cfg_controls_stv_player_2,
        stv_game_name,
        stv_zip_name,
        stv_parent_set,
        stv_version,
        stv_release_date,
        stv_region,
        stv_files
    };

    enum class RomLoad;
    enum class RomType;

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

        using MapKeys = std::map<const AccessKeys, const std::string>;  ///< MapKeys alias definition.
        static MapKeys full_keys;  ///< Link between access keys enumerators and string keys.

        using MapKeysDefault = std::map<const AccessKeys, const std::any>;  ///< MapKeysDefault alias definition.
        static MapKeysDefault default_keys; ///< Link between access keys enumerators and default values.

        using MapRomLoad = std::map<const std::string, const RomLoad>;  ///< MapRomLoad alias definition.
        static MapRomLoad rom_load; ///< Link between the rom load string value defined in the config file and the RomLoad type.

        using MapRomType = std::map<const std::string, const RomType>;  ///< MapRomType alias definition.
        static MapRomType rom_type; ///< Link between the rom type string value defined in the config file and the RomType type.

        using MapCdromAccess = std::map<const std::string, const cdrom::CdromAccessMethod>;   ///< MapCdromAccess alias definition.
        static MapCdromAccess cdrom_access;///< Link between the cdrom access method string value defined in the config file and the CdromAccessMethod type.

        using MapHardwareMode = std::map<const std::string, const core::HardwareMode>;  ///< MapHardwareMode alias definition.
        static MapHardwareMode hardware_mode;   ///< Link between the hardware mode string value defined in the config file and the HardwareMode type.

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
        template <> struct ToMySetting<bool>               { static constexpr libconfig::Setting::Type index = libconfig::Setting::TypeBoolean; };
        template <> struct ToMySetting<uint16_t>           { static constexpr libconfig::Setting::Type index = libconfig::Setting::TypeInt; };
        template <> struct ToMySetting<uint32_t>           { static constexpr libconfig::Setting::Type index = libconfig::Setting::TypeInt; };
        template <> struct ToMySetting<int32_t>            { static constexpr libconfig::Setting::Type index = libconfig::Setting::TypeInt; };
        template <> struct ToMySetting<uint64_t>           { static constexpr libconfig::Setting::Type index = libconfig::Setting::TypeInt64; };
        template <> struct ToMySetting<int64_t>            { static constexpr libconfig::Setting::Type index = libconfig::Setting::TypeInt64; };
        template <> struct ToMySetting<float>              { static constexpr libconfig::Setting::Type index = libconfig::Setting::TypeFloat; };
        template <> struct ToMySetting<const char*>        { static constexpr libconfig::Setting::Type index = libconfig::Setting::TypeString; };
        template <> struct ToMySetting<std::string> { static constexpr libconfig::Setting::Type index = libconfig::Setting::TypeString; };

        template<class T>
        void writeValue(libconfig::Setting& root, const std::string& key, const T& value) {
            if (key.empty()) {
                root.add(ToMySetting<std::decay_t<T>>::index) = value; 
                return;
            }
                    
            if (!root.exists(key.c_str())) root.add(key.c_str(), ToMySetting<std::decay_t<T>>::index) = value;
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

        void writeValue(libconfig::Setting& root, const std::string& key, const std::string& value) {
            if (!root.exists(key.c_str())) root.add(key.c_str(), libconfig::Setting::TypeString) = value.c_str();
            else {
                libconfig::Setting& s = root[key.c_str()];
                s = value.c_str();
            }
        }

        template<>
        void writeValue<const PeripheralKey&>(libconfig::Setting& root, const std::string& key, const PeripheralKey& value) {
            if (!root.exists(key.c_str())) root.add(key.c_str(), libconfig::Setting::TypeInt) = util::toUnderlying(value);
            else {
                libconfig::Setting& s = root[key.c_str()];
                s = util::toUnderlying(value);
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
        void writeValue(const AccessKeys& key, const T& value) {
            try {
                libconfig::Setting &setting = cfg_.lookup(Config::full_keys[key]);
                setting = value;
            }
            catch (const libconfig::SettingNotFoundException& e) {
                std::string s = e.getPath();
                auto errorString = fmt::format(tr("Setting '{0}' not found !"), e.getPath());
                Log::error("config", errorString);
                throw std::runtime_error("Config error !");
            }
            catch (const libconfig::SettingTypeException& e) {

                auto errorString = fmt::format(tr("Setting '{0}' using the wrong type !"), e.getPath());
                Log::error("config", errorString);
                throw std::runtime_error("Config error !");
            }
        }

        template<class T>
        void writeValue(const AccessKeys& key, const std::vector<T>& elements) {
            try {
                libconfig::Setting &setting = cfg_.lookup(Config::full_keys[key]);
                if (setting.isArray()) {
                    for (auto i = setting.begin(); i != setting.end(); ++i) {
                        u8 idx = (*i).getIndex();
                        setting[idx] = util::toUnderlying(elements[idx]);
                    }

                    //for (const T& e : elements) {
                    //    this->writeValue<const T&>(setting, "", e);
                    //}
                }
            }
            catch (const libconfig::SettingNotFoundException& e) {
                std::string s = e.getPath();
                auto errorString = fmt::format(tr("Setting '{0}' not found !"), e.getPath());
                Log::error("config", errorString);
                throw std::runtime_error("Config error !");
            }
            catch (const libconfig::SettingTypeException& e) {

                auto errorString = fmt::format(tr("Setting '{0}' using the wrong type !"), e.getPath());
                Log::error("config", errorString);
                throw std::runtime_error("Config error !");
            }
        }

       ////////////////////////////////////////////////////////////////////////////////////////////////////
       /// \fn  libconfig::Setting& Config::readValue(const AccessKeys& value);
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

       libconfig::Setting& readValue(const AccessKeys& value);

       ////////////////////////////////////////////////////////////////////////////////////////////////////
       /// \fn  bool Config::existsValue(const AccessKeys& key);
       ///
       /// \brief   Checks if the key exists in the configuration file.
       ///
       /// \author  Runik
       /// \date    28/12/2019
       ///
       /// \param   key The key to check.
       ///
       /// \return  True if the key exists.
       ////////////////////////////////////////////////////////////////////////////////////////////////////

       bool existsValue(const AccessKeys& key);

       ////////////////////////////////////////////////////////////////////////////////////////////////////
       /// \fn  std::vector<std::string> Config::listAvailableLanguages();
       ///
       /// \brief   Returns a vector populated with available languages.
       ///
       /// \author  Runik
       /// \date    15/09/2018
       ///
       /// \return  Available languages.
       ////////////////////////////////////////////////////////////////////////////////////////////////////

       std::vector<std::string> listAvailableLanguages();

       ////////////////////////////////////////////////////////////////////////////////////////////////////
       /// \fn  template<class T> void Config::add(const std::string key, T default)
       ///
       /// \brief   Creates a configuration key with subgroups if needed, and adds the default value to the last token of the key.
       ///
       /// \author  Runik
       /// \date    17/12/2019
       ///
       /// \tparam  T   Type of the default value
       /// \param   key     The key.
       /// \param   default Default value to add.
       ////////////////////////////////////////////////////////////////////////////////////////////////////

       template<class T>
       void add(const std::string key, T default) {
           auto tokens{ util::explode(key, '.') };
           std::string parent_path = "";
           for (const auto& t : tokens) {
               if (parent_path == "") {
                   libcfg::Setting& root = cfg_.getRoot();
                   parent_path = addGroup(root, t);
               } else {
                   if (&t != &tokens.back()) {
                       libcfg::Setting& root = cfg_.lookup(parent_path);
                       parent_path = addGroup(root, t);
                   } else {
                       libcfg::Setting& root = cfg_.lookup(parent_path);
                       this->writeValue<T>(root, t, default);
                   }
               }
           }
       }

       // Specialization for vector
       template<class T>
       void add(const std::string key, std::vector<T> elements) {
           auto tokens{ util::explode(key, '.') };
           std::string parent_path = "";
           for (const auto& t : tokens) {
               if (parent_path == "") {
                   libcfg::Setting& root = cfg_.getRoot();
                   parent_path = addGroup(root, t);
               } else {
                   if (&t != &tokens.back()) {
                       libcfg::Setting& root = cfg_.lookup(parent_path);
                       parent_path = addGroup(root, t);
                   } else {
                       libcfg::Setting& root = cfg_.lookup(parent_path);
                       libcfg::Setting& arr  = root.add(t, libcfg::Setting::TypeArray);
                       for (const T& e : elements) {
                           this->writeValue<const T&>(arr, "", e);
                       }
                   }
               }
           }
       }

       ////////////////////////////////////////////////////////////////////////////////////////////////////
       /// \fn  void Config::createDefault(const AccessKeys& key);
       ///
       /// \brief   Creates a default entry for the specified key.
       ///
       /// \author  Runik
       /// \date    28/12/2019
       ///
       /// \param   key The key to create.
       ////////////////////////////////////////////////////////////////////////////////////////////////////

       void createDefault(const AccessKeys& key);

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

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn std::string Config::addGroup(libcfg::Setting& root, const std::string& group_name);
        ///
        /// \brief  Adds a group to a setting.
        ///
        /// \author Runik
        /// \date   17/12/2019
        ///
        /// \param [in,out] root        Root setting to add the group to.
        /// \param          group_name  Group name to add.
        ///
        /// \return A std::string.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        std::string addGroup(libcfg::Setting& root, const std::string& group_name);

        std::string filename_;  ///< Name of the configuration file used
        
        libconfig::Config cfg_; ///< Internal configuration object

    };


};
};