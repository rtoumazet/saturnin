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

#pragma warning(disable : 4275) // libconfig specific warning disable
#include <libconfig.h++>
#include <any>                        // any
#include <map>                        // map
#include <optional>                   // optional
#include <string>                     // string
#include <vector>                     // vector
#include <saturnin/src/memory.h>      // RomLoad, RomType
#include <saturnin/src/smpc.h>        // for AreaCode, PeripheralKey, PortStatus
#include <saturnin/src/utilities.h>   // toUnderlying
#include <saturnin/src/cdrom/cdrom.h> // CdromAccessMethod
#include <saturnin/src/video/vdp2.h>  // TvStandard

namespace libcfg = libconfig;
namespace cdrom  = saturnin::cdrom;
namespace util   = saturnin::utilities;
namespace video  = saturnin::video;

namespace saturnin::core {

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   AccessKeys
///
/// \brief  Keys used for accessing configuration data.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class AccessKeys {
    cfg_global_language,
    cfg_global_hardware_mode,
    cfg_global_area_code,
    cfg_global_set_time,
    cfg_rendering_tv_standard,
    cfg_rendering_legacy_opengl,
    cfg_paths_roms_stv,
    cfg_paths_bios_stv,
    cfg_paths_bios_saturn,
    cfg_cdrom_drive,
    cfg_cdrom_access_method,
    cfg_sound_soundcard,
    cfg_sound_disabled,
    cfg_controls_saturn_player_1,
    cfg_controls_saturn_player_1_connection,
    cfg_controls_saturn_player_2,
    cfg_controls_saturn_player_2_connection,
    cfg_controls_stv_board,
    cfg_controls_stv_player_1,
    cfg_controls_stv_player_2,
    cfg_log_cdrom,
    cfg_log_config,
    cfg_log_main,
    cfg_log_memory,
    cfg_log_sh2,
    cfg_log_scu,
    cfg_log_vdp1,
    cfg_log_vdp2,
    cfg_log_smpc,
    cfg_log_scsp,
    cfg_log_unimplemented,
    stv_game_name,
    stv_zip_name,
    stv_parent_set,
    stv_version,
    stv_release_date,
    stv_region,
    stv_files
};

using MapKeys         = std::map<const AccessKeys, const std::string>;               ///< MapKeys alias definition.
using MapKeysDefault  = std::map<const AccessKeys, const std::any>;                  ///< MapKeysDefault alias definition.
using MapRomLoad      = std::map<const std::string, const RomLoad>;                  ///< MapRomLoad alias definition.
using MapRomType      = std::map<const std::string, const RomType>;                  ///< MapRomType alias definition.
using MapHardwareMode = std::map<const std::string, const HardwareMode>;             ///< MapHardwareMode alias definition.
using MapCdromAccess  = std::map<const std::string, const cdrom::CdromAccessMethod>; ///< MapCdromAccess alias definition.
using MapTvStandard   = std::map<const std::string, const video::TvStandard>;        ///< MapHardwareMode alias definition.
using MapAreaCode     = std::map<const std::string, const AreaCode>;                 ///< MapAreaCode alias definition.
using MapPortStatus   = std::map<const std::string, const PortStatus>; ///< MapPeripheralConnection alias definition.
using MapLogLevel     = std::map<const std::string, const LogLevel>;   ///< MapLogLevel alias definition.

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
    //@{
    // Constructors / Destructors
    Config() = delete;
    // Config(const std::string configuration_filename) : filename_(configuration_filename) {};
    Config(const std::string& configuration_filename);
    Config(const Config&) = delete;
    Config(Config&&)      = delete;
    auto operator=(const Config&) & -> Config& = delete;
    auto operator=(Config&&) & -> Config& = delete;
    ~Config()                             = default;
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
    /// \fn auto Config::readFile() -> bool;
    ///
    /// \brief  Reads the configuration file linked to the object.
    ///
    /// \author Runik
    /// \date   24/12/2017
    ///
    /// \return True if it succeeds.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto readFile() -> bool;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto Config::initialize(const bool isModernOpenGlCapable) -> bool;
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

    auto initialize(bool isModernOpenGlCapable) -> bool;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static auto Config::getGroup(libconfig::Setting& root, const std::string& group_name) -> libconfig::Setting&
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

    static auto getGroup(libconfig::Setting& root, const std::string& group_name) -> libconfig::Setting&;

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

    template<class T>
    struct ToMySetting;
    template<>
    struct ToMySetting<bool> {
        static constexpr libconfig::Setting::Type index = libconfig::Setting::TypeBoolean;
    };
    template<>
    struct ToMySetting<uint16_t> {
        static constexpr libconfig::Setting::Type index = libconfig::Setting::TypeInt;
    };
    template<>
    struct ToMySetting<uint32_t> {
        static constexpr libconfig::Setting::Type index = libconfig::Setting::TypeInt;
    };
    template<>
    struct ToMySetting<int32_t> {
        static constexpr libconfig::Setting::Type index = libconfig::Setting::TypeInt;
    };
    template<>
    struct ToMySetting<uint64_t> {
        static constexpr libconfig::Setting::Type index = libconfig::Setting::TypeInt64;
    };
    template<>
    struct ToMySetting<int64_t> {
        static constexpr libconfig::Setting::Type index = libconfig::Setting::TypeInt64;
    };
    template<>
    struct ToMySetting<float> {
        static constexpr libconfig::Setting::Type index = libconfig::Setting::TypeFloat;
    };
    template<>
    struct ToMySetting<const char*> {
        static constexpr libconfig::Setting::Type index = libconfig::Setting::TypeString;
    };
    template<>
    struct ToMySetting<std::string> {
        static constexpr libconfig::Setting::Type index = libconfig::Setting::TypeString;
    };

    template<class T>
    void writeValue(libconfig::Setting& root, const std::string& key, const T& value) {
        if (key.empty()) {
            root.add(ToMySetting<std::decay_t<T>>::index) = value;
            return;
        }

        if (!root.exists(key.c_str())) {
            root.add(key.c_str(), ToMySetting<std::decay_t<T>>::index) = value;
        } else {
            libconfig::Setting& s = root[key.c_str()];
            s                     = value;
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

    static void writeValue(libconfig::Setting& root, const std::string& key, const std::string& value) {
        if (!root.exists(key.c_str())) {
            root.add(key.c_str(), libconfig::Setting::TypeString) = value.c_str();
        } else {
            libconfig::Setting& s = root[key.c_str()];
            s                     = value.c_str();
        }
    }

    template<>
    void writeValue<const PeripheralKey&>(libconfig::Setting& root, const std::string& key, const PeripheralKey& value) {
        if (key.empty()) {
            root.add(libconfig::Setting::TypeInt) = util::toUnderlying(value);
            return;
        }

        if (!root.exists(key.c_str())) {
            root.add(key.c_str(), libconfig::Setting::TypeInt) = util::toUnderlying(value);
        } else {
            libconfig::Setting& s = root[key.c_str()];
            s                     = util::toUnderlying(value);
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
    void
        writeValue(libconfig::Setting& root, const std::string& key, const char (&value)[N]) { // NOLINT(modernize-avoid-c-arrays)
        if (!root.exists(key.c_str())) {
            root.add(key.c_str(), libconfig::Setting::TypeString) = value;
        } else {
            libconfig::Setting& s = root[key.c_str()];
            s                     = value;
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
            auto& setting = cfg_.lookup(full_keys_[key]);
            setting       = value;
        } catch (const libconfig::SettingNotFoundException& e) {
            logError("Setting '{0}' not found !", e.getPath());
            throw std::runtime_error("Config error !");
        } catch (const libconfig::SettingTypeException& e) {
            logError("Setting '{0}' using the wrong type !", e.getPath());
            throw std::runtime_error("Config error !");
        }
    }

    template<class T>
    void writeValue(const AccessKeys& key, const std::vector<T>& elements) {
        try {
            auto& setting = cfg_.lookup(full_keys_[key]);
            if (setting.isArray()) {
                for (auto i = setting.begin(); i != setting.end(); ++i) {
                    const auto idx = static_cast<u8>((*i).getIndex());
                    setting[idx]   = util::toUnderlying(elements[idx]);
                }
            }
        } catch (const libconfig::SettingNotFoundException& e) {
            logError("Setting '{0}' not found !", e.getPath());
            throw std::runtime_error("Config error !");
        } catch (const libconfig::SettingTypeException& e) {
            logError("Setting '{0}' using the wrong type !", e.getPath());
            throw std::runtime_error("Config error !");
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn  auto Config::readValue(const AccessKeys& value) -> libconfig::Setting&;
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

    auto readValue(const AccessKeys& value) -> libconfig::Setting&;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn  auto Config::existsValue(const AccessKeys& key) -> bool;
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

    auto existsValue(const AccessKeys& value) -> bool;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static auto Config::listAvailableLanguages() -> std::vector<std::string>;
    ///
    /// \brief   Returns a vector populated with available languages.
    ///
    /// \author  Runik
    /// \date    15/09/2018
    ///
    /// \return  Available languages.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static auto listAvailableLanguages() -> std::vector<std::string>;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto saturnin::core::Config::listAreaCodes() -> std::vector<std::string>;
    ///
    /// \brief  Returns a vector populated with area codes.
    ///
    /// \author Runik
    /// \date   26/01/2020
    ///
    /// \returns    A std::vector&lt;std::string&gt;
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto listAreaCodes() -> std::vector<std::string>;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto saturnin::core::Config::listPeripheralConnections() -> std::vector<std::string>;
    ///
    /// \brief  Returns a vector populated with peripheral connections.
    ///
    /// \author Runik
    /// \date   17/03/2020
    ///
    /// \returns    A std::vector&lt;std::string&gt;
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto listPeripheralConnections() -> std::vector<std::string>;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto Config::listLogLevels() -> std::vector<std::string>;
    ///
    /// \brief  Returns a vector populated with the different log levels.
    ///
    /// \author Runik
    /// \date   12/05/2021
    ///
    /// \returns    A std::vector&lt;std::string&gt;
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto listLogLevels() -> std::vector<std::string>;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto saturnin::core::Config::configToPortStatus(std::string value) -> PortStatus;
    ///
    /// \brief  Configuration entry to port status
    ///
    /// \author Runik
    /// \date   18/03/2020
    ///
    /// \param  value   The config value.
    ///
    /// \returns    A PortStatus.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto configToPortStatus(std::string value) -> PortStatus;

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
    /// \param   def_value Default value to add.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    template<class T>
    void add(const std::string key, T def_value) { // NOLINT(readability-avoid-const-params-in-decls)
        const auto tokens{util::explode(key, '.')};
        auto       parent_path = std::string{};
        for (const auto& t : tokens) {
            if (parent_path.empty()) {
                auto& root  = cfg_.getRoot();
                parent_path = addGroup(root, t);
            } else {
                if (&t != &tokens.back()) {
                    auto& root  = cfg_.lookup(parent_path);
                    parent_path = addGroup(root, t);
                } else {
                    auto& root = cfg_.lookup(parent_path);
                    this->writeValue<T>(root, t, def_value);
                }
            }
        }
    }

    // Specialization for vector
    template<class T>
    void add(const std::string key, std::vector<T> elements) {
        const auto tokens{util::explode(key, '.')};
        auto       parent_path = std::string{};
        for (const auto& t : tokens) {
            if (parent_path.empty()) {
                auto& root  = cfg_.getRoot();
                parent_path = addGroup(root, t);
            } else {
                if (&t != &tokens.back()) {
                    auto& root  = cfg_.lookup(parent_path);
                    parent_path = addGroup(root, t);
                } else {
                    auto& root = cfg_.lookup(parent_path);
                    auto& arr  = root.add(t, libcfg::Setting::TypeArray);
                    for (const T& e : elements) {
                        this->writeValue<const T&>(arr, "", e);
                    }
                }
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn  auto Config::createDefault(const AccessKeys& key) -> std::vector<PeripheralKey>;
    ///
    /// \brief   Creates a default entry for the specified key.
    ///
    /// \author  Runik
    /// \date    28/12/2019
    ///
    /// \param   key The key to create.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void createDefault(const AccessKeys& key);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto saturnin::core::Config::readPeripheralConfiguration(const AccessKeys& key) -> std::vector<PeripheralKey>;
    ///
    /// \brief  Reads peripheral configuration of the key
    ///
    /// \author Runik
    /// \date   13/12/2020
    ///
    /// \param  key The key.
    ///
    /// \returns    The peripheral configuration.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto readPeripheralConfiguration(const AccessKeys& key) -> std::vector<PeripheralKey>;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto Config::getHardwareMode(const std::string& key) -> HardwareMode;
    ///
    /// \brief  Returns the hardware mode corresponding to the key.
    ///
    /// \author Runik
    /// \date   13/12/2020
    ///
    /// \param  key The key.
    ///
    /// \returns    The hardware mode.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto getHardwareMode(const std::string& key) -> HardwareMode;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto saturnin::core::Config::getHardwareModeKey(const HardwareMode value) const -> std::optional<std::string>;
    ///
    /// \brief  Gets the hardware mode string key from a value. Returns nullopt if value is not found
    ///
    /// \author Runik
    /// \date   13/12/2020
    ///
    /// \param  value   The value.
    ///
    /// \returns    The hardware mode key.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto getHardwareModeKey(const HardwareMode value) const -> std::optional<std::string>;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto Config::getCdromAccess(const std::string& key) -> cdrom::CdromAccessMethod;
    ///
    /// \brief  Returns the cdrom access method corresponding to the key.
    ///
    /// \author Runik
    /// \date   13/12/2020
    ///
    /// \param  key The key.
    ///
    /// \returns    The cdrom access method.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto getCdromAccess(const std::string& key) -> cdrom::CdromAccessMethod;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto saturnin::core::Config::getCdromAccessKey(const cdrom::CdromAccessMethod value) const ->
    /// std::optional<std::string>;
    ///
    /// \brief  Gets the cdrom access method string key from a value. Returns nullopt if value is not
    ///         found
    ///
    /// \author Runik
    /// \date   13/12/2020
    ///
    /// \param  value   The value.
    ///
    /// \returns    The cdrom access method key.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto getCdromAccessKey(const cdrom::CdromAccessMethod value) const -> std::optional<std::string>;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto saturnin::core::Config::getTvStandard(const std::string& key) -> video::TvStandard;
    ///
    /// \brief  Returns the TV standard method corresponding to the key.
    ///
    /// \author Runik
    /// \date   13/12/2020
    ///
    /// \param  key The key.
    ///
    /// \returns    The TV standard.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto getTvStandard(const std::string& key) -> video::TvStandard;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto saturnin::core::Config::getTvStandardKey(const video::TvStandard value) const -> std::optional<std::string>;
    ///
    /// \brief  Gets the TV standard string key from a value. Returns nullopt if value is not
    ///         found
    ///
    /// \author Runik
    /// \date   13/12/2020
    ///
    /// \param  value   The value.
    ///
    /// \returns    The TV standard key.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto getTvStandardKey(const video::TvStandard value) const -> std::optional<std::string>;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto Config::getRomLoad(const std::string& key) -> RomLoad;
    ///
    /// \brief  Returns the rom load method corresponding to the key.
    ///
    /// \author Runik
    /// \date   13/12/2020
    ///
    /// \param  key The key.
    ///
    /// \returns    The rom load method.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto getRomLoad(const std::string& key) -> RomLoad;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto Config::getRomType(const std::string& key) -> RomType;
    ///
    /// \brief  Returns the rom type corresponding to the key.
    ///
    /// \author Runik
    /// \date   13/12/2020
    ///
    /// \param  key The key.
    ///
    /// \returns    The rom type.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto getRomType(const std::string& key) -> RomType;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto Config::getAreaCode(const std::string& key) -> AreaCode;
    ///
    /// \brief  Returns the area code corresponding to the key.
    ///
    /// \author Runik
    /// \date   13/12/2020
    ///
    /// \param  key The key.
    ///
    /// \returns    The area code.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto getAreaCode(const std::string& key) -> AreaCode;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto saturnin::core::Config::getAreaCodeKey(const AreaCode value) const -> std::optional<std::string>;
    ///
    /// \brief  Gets the area code string key from a value. Returns nullopt if value is not found
    ///
    /// \author Runik
    /// \date   13/12/2020
    ///
    /// \param  value   The value.
    ///
    /// \returns    The area code key.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto getAreaCodeKey(const AreaCode value) const -> std::optional<std::string>;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Config::updateLogLevel();
    ///
    /// \brief  Updates the log level of the various loggers.
    ///
    /// \author Runik
    /// \date   13/05/2021
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void updateLogLevel();

  private:
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Config::generateConfigurationTree(bool isModernOpenglCapable);
    ///
    /// \brief  Generates the configuration tree with default values in the configuration file.
    ///
    /// \author Runik
    /// \date   21/06/2018
    ///
    /// \param  isModernOpenglCapable   The is modern opengl capable.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void generateConfigurationTree(bool isModernOpenglCapable);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static auto Config::addGroup(libcfg::Setting& root, const std::string& group_name) -> std::string;
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

    static auto addGroup(libcfg::Setting& root, const std::string& group_name) -> std::string;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn auto Config::getLogLevel(const std::string& key) -> LogLevel;
    ///
    /// \brief  Returns the log level corresponding to the key.
    ///
    /// \author Runik
    /// \date   12/05/2021
    ///
    /// \param  key The key.
    ///
    /// \returns    The log level.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    auto getLogLevel(const std::string& key) -> LogLevel;

    std::string filename_; ///< Name of the configuration file used

    libconfig::Config cfg_; ///< Internal configuration object

    MapKeys        full_keys_;    ///< Link between access keys enumerators and string keys.
    MapKeysDefault default_keys_; ///< Link between access keys enumerators and default values.
    MapRomLoad     rom_load_;     ///< Link between the rom load string value defined in the config file and the RomLoad type.
    MapRomType     rom_type_;     ///< Link between the rom type string value defined in the config file and the RomType type.
    MapHardwareMode
        hardware_mode_; ///< Link between the hardware mode string value defined in the config file and the HardwareMode type.
    MapCdromAccess cdrom_access_; ///< Link between the cdrom access method string value defined in the config file and the
                                  ///< CdromAccessMethod type.
    MapTvStandard tv_standard_; ///< Link between the tv standard string value defined in the config file and the TvStandard type.
    MapAreaCode   area_code_;   ///< Link between the area code string value defined in the config file and the AreaCode type.
    MapPortStatus port_status_; ///< Link between the port status string value defined in the config file and the PortStatus type.
    MapLogLevel   log_level_;   ///< Link between the log level string value defined in the config file and the LogLevel type.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn void logError(const std::string& error, const std::string& path);
///
/// \brief  Logs an error while fetching a setting.
///
/// \author Runik
/// \date   12/12/2020
///
/// \param  error   The raw error message (not translated).
/// \param  path    Path of the setting.
////////////////////////////////////////////////////////////////////////////////////////////////////

void logError(const std::string& error, const std::string& path);

}; // namespace saturnin::core
