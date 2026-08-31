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
#include <any>                            // any
#include <map>                            // map
#include <optional>                       // optional
#include <string>                         // string
#include <vector>                         // vector
#include <saturnin/src/exceptions.h>      // ConfigError
#include <saturnin/src/memory.h>          // RomLoad, RomType
#include <saturnin/src/smpc.h>            // for AreaCode, PeripheralKey, PortStatus
#include <saturnin/src/utilities.h>       // toUnderlying
#include <saturnin/src/cdrom/cdrom.h>     // CdromAccessMethod
#include <saturnin/src/sh2/sh2.h>         // Sh2Core
#include <saturnin/src/video/renderer.h>  // RendererType
#include <saturnin/src/video/vdp2/vdp2.h> // TvStandard

namespace libcfg = libconfig;
namespace util   = saturnin::utilities;

namespace saturnin::core {

// Keys used for accessing configuration data.
enum class AccessKeys : u8 {
    cfg_global_language,
    cfg_global_hardware_mode,
    cfg_global_area_code,
    cfg_global_set_time,
    cfg_global_stv_bios_bypass,
    cfg_rendering_tv_standard,
    // cfg_rendering_legacy_opengl,
    cfg_rendering_renderer,
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
    cfg_log_opengl,
    cfg_log_sh2,
    cfg_log_scu,
    cfg_log_vdp1,
    cfg_log_vdp2,
    cfg_log_smpc,
    cfg_log_scsp,
    cfg_log_unimplemented,
    cfg_advanced_sh2_core,
    stv_game_name,
    stv_zip_name,
    stv_parent_set,
    stv_version,
    stv_release_date,
    stv_region,
    stv_files
};

using MapKeys         = std::map<const AccessKeys, const std::string>;               // MapKeys alias definition.
using MapKeysDefault  = std::map<const AccessKeys, const std::any>;                  // MapKeysDefault alias definition.
using MapRomLoad      = std::map<const std::string, const RomLoad>;                  // MapRomLoad alias definition.
using MapRomType      = std::map<const std::string, const RomType>;                  // MapRomType alias definition.
using MapHardwareMode = std::map<const std::string, const HardwareMode>;             // MapHardwareMode alias definition.
using MapCdromAccess  = std::map<const std::string, const cdrom::CdromAccessMethod>; // MapCdromAccess alias definition.
using MapTvStandard   = std::map<const std::string, const video::TvStandard>;        // MapHardwareMode alias definition.
using MapAreaCode     = std::map<const std::string, const AreaCode>;                 // MapAreaCode alias definition.
using MapPortStatus   = std::map<const std::string, const PortStatus>;               // MapPeripheralConnection alias definition.
using MapLogLevel     = std::map<const std::string, const LogLevel>;                 // MapLogLevel alias definition.
using MapSh2Core      = std::map<const std::string, const sh2::Sh2Core>;             // MapSh2Core alias definition.
using MapRenderer     = std::map<const std::string, const video::RendererType>;      // MapRenderer alias definition.

// Manages configuration of the emulator.
class Config {
  public:
    //@{
    // Constructors / Destructors
    Config() = delete;
    explicit Config(std::string_view configuration_filename);
    Config(const Config&)                      = delete;
    Config(Config&&)                           = delete;
    auto operator=(const Config&) & -> Config& = delete;
    auto operator=(Config&&) & -> Config&      = delete;
    ~Config()                                  = default;
    //@}

    // Writes the current configuration to the file linked to the object.
    void writeFile();

    // Reads the configuration file linked to the object.
    auto readFile() -> bool;

    // Initializes the configuration by reading 'saturnin.cfg'. The file is created if missing
    auto initialize(bool isModernOpenGlCapable) -> bool;

    // Returns the group under the specified root setting, creates it if missing.
    static auto getGroup(libconfig::Setting& root, const std::string& group_name) -> libconfig::Setting&;

    void test();

    // Writes a value under the specified root setting.
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

    // String specilization of write_value.
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

    // Char array specialization of write_value.
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

    // Writes a value for the specified key.
    template<class T>
    void writeValue(const AccessKeys& key, const T& value) {
        try {
            auto& setting = cfg_.lookup(full_keys_[key]);
            setting       = value;
        } catch (const libconfig::SettingNotFoundException& e) {
            Log::exception(Logger::config, tr("Setting '{0}' not found !"), e.getPath());
        } catch (const libconfig::SettingTypeException& e) {
            Log::exception(Logger::config, tr("Setting '{0}' using the wrong type !"), e.getPath());
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
            Log::exception(Logger::config, tr("Setting '{0}' not found !"), e.getPath());
        } catch (const libconfig::SettingTypeException& e) {
            Log::exception(Logger::config, tr("Setting '{0}' using the wrong type !"), e.getPath());
        }
    }

    // Reads a value from the specified root setting.
    auto readValue(const AccessKeys& value) -> libconfig::Setting&;

    // Checks if the key exists in the configuration file.
    auto existsValue(const AccessKeys& value) -> bool;

    // Returns a vector populated with available languages.
    static auto listAvailableLanguages() -> std::vector<std::string>;

    // Returns a vector populated with area codes.
    auto listAreaCodes() const -> std::vector<std::string>;

    // Returns a vector populated with peripheral connections.
    auto listPeripheralConnections() const -> std::vector<std::string>;

    // Returns a vector populated with the different log levels.
    auto listLogLevels() const -> std::vector<std::string>;

    // Returns a vector populated with the different SH2 cores.
    auto listSh2Cores() const -> std::vector<std::string>;

    // Returns a vector populated with the different renderers.
    auto listRenderers() const -> std::vector<std::string>;

    // Configuration entry to port status
    auto configToPortStatus(const std::string& value) -> PortStatus;

    // Creates a configuration key with subgroups if needed, and adds the default value to the last token of the key.
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

    // Creates a default entry for the specified key.
    void createDefault(const AccessKeys& key);

    // Reads peripheral configuration of the key
    auto readPeripheralConfiguration(const AccessKeys& key) -> std::vector<PeripheralKey>;

    // Returns the hardware mode corresponding to the key.
    auto getHardwareMode(const std::string& key) -> HardwareMode;

    // Gets the hardware mode string key from a value. Returns nullopt if value is not found
    auto getHardwareModeKey(const HardwareMode value) const -> std::optional<std::string>;

    // Returns the cdrom access method corresponding to the key.
    auto getCdromAccess(const std::string& key) -> cdrom::CdromAccessMethod;

    // Gets the cdrom access method string key from a value. Returns nullopt if value is not found.
    auto getCdromAccessKey(const cdrom::CdromAccessMethod value) const -> std::optional<std::string>;

    // Returns the TV standard method corresponding to the key.
    auto getTvStandard(const std::string& key) -> video::TvStandard;

    // Gets the TV standard string key from a value. Returns nullopt if value is not found.
    auto getTvStandardKey(const video::TvStandard value) const -> std::optional<std::string>;

    // Returns the rom load method corresponding to the key.
    auto getRomLoad(const std::string& key) -> RomLoad;

    // Returns the rom type corresponding to the key.
    auto getRomType(const std::string& key) -> RomType;

    // Returns the area code corresponding to the key.
    auto getAreaCode(const std::string& key) -> AreaCode;

    // Gets the area code string key from a value. Returns nullopt if value is not found
    auto getAreaCodeKey(const AreaCode value) const -> std::optional<std::string>;

    // Returns the SH2 core saved in the configuration.
    auto getCurrentSh2Core() -> sh2::Sh2Core;

    // Returns the renderer corresponding to the key.
    auto getRenderer(const std::string& key) -> video::RendererType;

    // Updates the log level of the various loggers.
    void updateLogLevel();

  private:
    // Generates the configuration tree with default values in the configuration file.
    void generateConfigurationTree(bool isModernOpenglCapable);

    // Adds a group to a setting.
    static auto addGroup(libcfg::Setting& root, const std::string& group_name) -> std::string;

    // Returns the log level corresponding to the key.
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
    MapSh2Core    sh2_core_;    ///< Link between the sh2 core string value defined in the config file and the Sh2Core type.
    MapRenderer   renderer_;    ///< Link between the renderer string value defined in the config file and the Renderer type.
};

}; // namespace saturnin::core
