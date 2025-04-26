//
// config.cpp
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
//

#include <saturnin/src/pch.h>
#include <saturnin/src/config.h>
#include <GLFW/glfw3.h> // GLFW_KEYS
#include <filesystem>
#include <iostream> // cout
#include <saturnin/src/log.h>

namespace libcfg = libconfig;
namespace fs     = std::filesystem;
namespace uti    = saturnin::utilities;

namespace saturnin::core {

using core::Log;

Config::Config(std::string_view configuration_filename) : filename_(configuration_filename) {
    using enum AccessKeys;

    full_keys_ = {
        {cfg_global_language,                     "global.language"                    },
        {cfg_global_hardware_mode,                "global.hardware_mode"               },
        {cfg_global_area_code,                    "global.area_code"                   },
        {cfg_global_set_time,                     "global.set_time"                    },
        {cfg_global_stv_bios_bypass,              "global.stv_bios_bypass"             },
        {cfg_rendering_legacy_opengl,             "rendering.legacy_opengl"            },
        {cfg_rendering_tv_standard,               "rendering.tv_standard"              },
        {cfg_paths_roms_stv,                      "paths.roms_stv"                     },
        {cfg_paths_bios_stv,                      "paths.bios_stv"                     },
        {cfg_paths_bios_saturn,                   "paths.bios_saturn"                  },
        {cfg_cdrom_drive,                         "cdrom.drive"                        },
        {cfg_cdrom_access_method,                 "cdrom.access_method"                },
        {cfg_sound_soundcard,                     "sound.soundcard"                    },
        {cfg_sound_disabled,                      "sound.disabled"                     },
        {cfg_controls_saturn_player_1,            "controls.saturn.player_1"           },
        {cfg_controls_saturn_player_1_connection, "controls.saturn.player_1_connection"},
        {cfg_controls_saturn_player_2,            "controls.saturn.player_2"           },
        {cfg_controls_saturn_player_2_connection, "controls.saturn.player_2_connection"},
        {cfg_controls_stv_board,                  "controls.stv.board"                 },
        {cfg_controls_stv_player_1,               "controls.stv.player_1"              },
        {cfg_controls_stv_player_2,               "controls.stv.player_2"              },
        {cfg_log_cdrom,                           "logs.cdrom"                         },
        {cfg_log_config,                          "logs.config"                        },
        {cfg_log_main,                            "logs.main"                          },
        {cfg_log_memory,                          "logs.memory"                        },
        {cfg_log_opengl,                          "logs.opengl"                        },
        {cfg_log_sh2,                             "logs.sh2"                           },
        {cfg_log_scsp,                            "logs.scsp"                          },
        {cfg_log_scu,                             "logs.scu"                           },
        {cfg_log_smpc,                            "logs.smpc"                          },
        {cfg_log_vdp1,                            "logs.vdp1"                          },
        {cfg_log_vdp2,                            "logs.vdp2"                          },
        {cfg_log_unimplemented,                   "logs.unimplemented"                 },
        {cfg_advanced_sh2_core,                   "advanced.sh2_core"                  },
        {stv_game_name,                           "game_name"                          },
        {stv_zip_name,                            "zip_name"                           },
        {stv_parent_set,                          "parent_set"                         },
        {stv_version,                             "version"                            },
        {stv_release_date,                        "release_date"                       },
        {stv_region,                              "region"                             },
        {stv_files,                               "files"                              }
    };

    default_keys_ = {
        {cfg_global_language,                     std::string("en")               },
        {cfg_global_hardware_mode,                std::string("saturn")           },
        {cfg_global_area_code,                    std::string("europe_pal")       },
        {cfg_global_set_time,                     true                            },
        {cfg_global_stv_bios_bypass,              true                            },
        {cfg_rendering_tv_standard,               std::string("pal")              },
        {cfg_rendering_legacy_opengl,             false                           },
        {cfg_paths_roms_stv,                      std::string("")                 },
        {cfg_paths_bios_stv,                      std::string("")                 },
        {cfg_paths_bios_saturn,                   std::string("")                 },
        {cfg_cdrom_drive,                         std::string("-1:-1:-1")         },
        {cfg_cdrom_access_method,                 std::string("spti")             },
        {cfg_sound_soundcard,                     std::string("")                 },
        {cfg_sound_disabled,                      false                           },
        {cfg_controls_saturn_player_1_connection, std::string("direct")           },
        {cfg_controls_saturn_player_2_connection, std::string("none")             },
        {cfg_log_cdrom,                           std::string("info")             },
        {cfg_log_config,                          std::string("info")             },
        {cfg_log_main,                            std::string("info")             },
        {cfg_log_memory,                          std::string("info")             },
        {cfg_log_opengl,                          std::string("info")             },
        {cfg_log_sh2,                             std::string("info")             },
        {cfg_log_scsp,                            std::string("info")             },
        {cfg_log_scu,                             std::string("info")             },
        {cfg_log_smpc,                            std::string("info")             },
        {cfg_log_vdp1,                            std::string("info")             },
        {cfg_log_vdp2,                            std::string("info")             },
        {cfg_log_unimplemented,                   std::string("info")             },
        {cfg_advanced_sh2_core,                   std::string("basic_interpreter")}
    };

    rom_load_ = {
        {"not_interleaved",  RomLoad::not_interleaved },
        {"odd_interleaved",  RomLoad::odd_interleaved },
        {"even_interleaved", RomLoad::even_interleaved}
    };

    rom_type_ = {
        {"program", RomType::program},
        {"graphic", RomType::graphic},
        {"bios",    RomType::bios   }
    };

    cdrom_access_ = {
        {"aspi", cdrom::CdromAccessMethod::aspi},
        {"spti", cdrom::CdromAccessMethod::spti}
    };

    hardware_mode_ = {
        {"saturn", HardwareMode::saturn},
        {"stv",    HardwareMode::stv   }
    };

    tv_standard_ = {
        {"pal",  video::TvStandard::pal },
        {"ntsc", video::TvStandard::ntsc}
    };

    area_code_ = {
        {"japan",                      AreaCode::japan                     },
        {"asia_ntsc",                  AreaCode::asia_ntsc                 },
        {"north_america",              AreaCode::north_america             },
        {"central_south_america_ntsc", AreaCode::central_south_america_ntsc},
        {"korea",                      AreaCode::korea                     },
        {"asia_pal",                   AreaCode::asia_pal                  },
        {"europe_pal",                 AreaCode::europe_pal                },
        {"central_south_america_pal",  AreaCode::central_south_america_pal }
    };

    port_status_ = {
        {"none",     PortStatus::not_connected     },
        {"direct",   PortStatus::direct_connection },
        {"segatap",  PortStatus::sega_tap          },
        {"multitap", PortStatus::saturn_6p_multitap}
    };

    log_level_ = {
        {"off",   LogLevel::off  },
        {"info",  LogLevel::info },
        {"debug", LogLevel::debug}
    };

    sh2_core_ = {
        {"basic_interpreter", sh2::Sh2Core::basic_interpreter},
        {"fast_interpreter",  sh2::Sh2Core::fast_interpreter }
    };
};

void Config::writeFile() { cfg_.writeFile(this->filename_.c_str()); }

auto Config::readFile() -> bool {
    try {
        cfg_.readFile(filename_.c_str());
        return true;
    } catch (const libcfg::FileIOException& fioex) {
        const auto error = uti::format(tr("Could not read file {0} : {1}"), filename_, fioex.what());
        Log::error(Logger::config, error);
        return false;
    }
}

auto Config::initialize(const bool isModernOpenGlCapable) -> bool {
    if (!readFile()) {
        std::cout << tr("Creating configuration file.") << std::endl;
        this->generateConfigurationTree(isModernOpenGlCapable);
        this->writeFile();
    }

    updateLogLevel();
    return true;
}

void Config::generateConfigurationTree(const bool isModernOpenglCapable) {
    using enum AccessKeys;
    // clang-format off
    add(full_keys_[cfg_global_language],                     std::any_cast<const std::string&>(default_keys_[cfg_global_language]));
    add(full_keys_[cfg_global_hardware_mode],                std::any_cast<const std::string&>(default_keys_[cfg_global_hardware_mode]));
    add(full_keys_[cfg_global_area_code],                    std::any_cast<const std::string&>(default_keys_[cfg_global_area_code]));
    add(full_keys_[cfg_global_set_time],                     std::any_cast<const bool>(default_keys_[cfg_global_set_time]));
    add(full_keys_[cfg_global_stv_bios_bypass],              std::any_cast<const bool>(default_keys_[cfg_global_stv_bios_bypass]));
    add(full_keys_[cfg_rendering_tv_standard],               std::any_cast<const std::string&>(default_keys_[cfg_rendering_tv_standard]));
    add(full_keys_[cfg_rendering_legacy_opengl],             !isModernOpenglCapable);
    add(full_keys_[cfg_paths_roms_stv],                      std::any_cast<const std::string&>(default_keys_[cfg_paths_roms_stv]));
    add(full_keys_[cfg_paths_bios_stv],                      std::any_cast<const std::string&>(default_keys_[cfg_paths_bios_stv]));
    add(full_keys_[cfg_paths_bios_saturn],                   std::any_cast<const std::string&>(default_keys_[cfg_paths_bios_saturn]));
    add(full_keys_[cfg_cdrom_drive],                         std::any_cast<const std::string&>(default_keys_[cfg_cdrom_drive]));
    add(full_keys_[cfg_cdrom_access_method],                 std::any_cast<const std::string&>(default_keys_[cfg_cdrom_access_method]));
    add(full_keys_[cfg_sound_disabled],                      std::any_cast<const bool>(default_keys_[cfg_sound_disabled]));
    add(full_keys_[cfg_controls_saturn_player_1_connection], std::any_cast<const std::string&>(default_keys_[cfg_controls_saturn_player_1_connection]));
    add(full_keys_[cfg_controls_saturn_player_1],            SaturnDigitalPad().toConfig(PeripheralLayout::default_layout));
    add(full_keys_[cfg_controls_saturn_player_2_connection], std::any_cast<const std::string&>(default_keys_[cfg_controls_saturn_player_2_connection]));
    add(full_keys_[cfg_controls_saturn_player_2],            SaturnDigitalPad().toConfig(PeripheralLayout::empty_layout));
    add(full_keys_[cfg_controls_stv_board],                  StvBoardControls().toConfig(PeripheralLayout::default_layout));
    add(full_keys_[cfg_controls_stv_player_1],               StvPlayerControls().toConfig(PeripheralLayout::default_layout));
    add(full_keys_[cfg_controls_stv_player_2],               StvPlayerControls().toConfig(PeripheralLayout::empty_layout));
    add(full_keys_[cfg_log_cdrom],                           std::any_cast<const std::string&>(default_keys_[cfg_log_cdrom]));
    add(full_keys_[cfg_log_config],                           std::any_cast<const std::string&>(default_keys_[cfg_log_config]));
    add(full_keys_[cfg_log_main],                           std::any_cast<const std::string&>(default_keys_[cfg_log_main]));
    add(full_keys_[cfg_log_memory],                           std::any_cast<const std::string&>(default_keys_[cfg_log_memory]));
    add(full_keys_[cfg_log_opengl],                           std::any_cast<const std::string&>(default_keys_[cfg_log_opengl]));
    add(full_keys_[cfg_log_scsp],                           std::any_cast<const std::string&>(default_keys_[cfg_log_scsp]));
    add(full_keys_[cfg_log_scu],                           std::any_cast<const std::string&>(default_keys_[cfg_log_scu]));
    add(full_keys_[cfg_log_sh2],                           std::any_cast<const std::string&>(default_keys_[cfg_log_sh2]));
    add(full_keys_[cfg_log_smpc],                           std::any_cast<const std::string&>(default_keys_[cfg_log_smpc]));
    add(full_keys_[cfg_log_vdp1],                           std::any_cast<const std::string&>(default_keys_[cfg_log_vdp1]));
    add(full_keys_[cfg_log_vdp2],                           std::any_cast<const std::string&>(default_keys_[cfg_log_vdp2]));
    add(full_keys_[cfg_log_unimplemented],                  std::any_cast<const std::string&>(default_keys_[cfg_log_unimplemented]));
    add(full_keys_[cfg_advanced_sh2_core],                  std::any_cast<const std::string&>(default_keys_[cfg_advanced_sh2_core]));
    // clang-format on
}

auto Config::getGroup(libcfg::Setting& root, const std::string& group_name) -> libcfg::Setting& {
    if (!root.exists(group_name.c_str())) { root.add(group_name.c_str(), libcfg::Setting::TypeGroup); }
    return root[group_name.c_str()];
}

auto Config::addGroup(libcfg::Setting& root, const std::string& group_name) -> std::string {
    // libcfg::Setting doesn't have a copy constructor, so it has to be declared twice
    if (!root.exists(group_name)) {
        const auto& new_setting = root.add(group_name, libcfg::Setting::TypeGroup);
        return new_setting.getPath();
    }
    const auto& new_setting = root.lookup(group_name);
    return new_setting.getPath();
}

void Config::test() {
    auto&      root = cfg_.getRoot();
    const auto str  = std::string{"test"};

    Config::writeValue(root, "test_c_string", str.c_str());
    Config::writeValue(root, "test_string", std::string{"test"});
    Config::writeValue(root, "test_char_array", "test");

    this->writeFile();
}

auto Config::readValue(const AccessKeys& value) -> libcfg::Setting& {
    try {
        if (!existsValue(value)) { createDefault(value); }
        return cfg_.lookup(full_keys_[value]);
    } catch (const libcfg::SettingNotFoundException& e) {
        Log::exception(Logger::config, tr("Setting '{0}' not found !"), e.getPath());
    }
}

auto Config::existsValue(const AccessKeys& value) -> bool { return cfg_.exists(full_keys_[value]); }

void Config::createDefault(const AccessKeys& key) {
    using enum AccessKeys;
    auto createStringDefault = [this, &key]() { add(full_keys_[key], std::any_cast<const std::string>(default_keys_[key])); };
    auto createBoolDefault   = [this, &key]() { add(full_keys_[key], std::any_cast<const bool>(default_keys_[key])); };
    auto createSaturnControlDefault
        = [this, &key]() { add(full_keys_[key], SaturnDigitalPad().toConfig(PeripheralLayout::default_layout)); };
    auto createSaturnControlEmpty
        = [this, &key]() { add(full_keys_[key], SaturnDigitalPad().toConfig(PeripheralLayout::empty_layout)); };
    auto createStvBoardControlDefault
        = [this, &key]() { add(full_keys_[key], StvBoardControls().toConfig(PeripheralLayout::default_layout)); };
    auto createStvPlayerControlDefault
        = [this, &key]() { add(full_keys_[key], StvPlayerControls().toConfig(PeripheralLayout::default_layout)); };
    auto createStvPlayerControlEmpty
        = [this, &key]() { add(full_keys_[key], StvPlayerControls().toConfig(PeripheralLayout::empty_layout)); };

    if (std::unordered_map<AccessKeys, std::function<void()>> keyToLambda{
            {cfg_global_language,                     createStringDefault          },
            {cfg_global_hardware_mode,                createStringDefault          },
            {cfg_global_area_code,                    createStringDefault          },
            {cfg_rendering_tv_standard,               createStringDefault          },
            {cfg_paths_roms_stv,                      createStringDefault          },
            {cfg_paths_bios_stv,                      createStringDefault          },
            {cfg_paths_bios_saturn,                   createStringDefault          },
            {cfg_cdrom_drive,                         createStringDefault          },
            {cfg_cdrom_access_method,                 createStringDefault          },
            {cfg_sound_soundcard,                     createStringDefault          },
            {cfg_controls_saturn_player_1_connection, createStringDefault          },
            {cfg_controls_saturn_player_2_connection, createStringDefault          },
            {cfg_log_cdrom,                           createStringDefault          },
            {cfg_log_config,                          createStringDefault          },
            {cfg_log_main,                            createStringDefault          },
            {cfg_log_memory,                          createStringDefault          },
            {cfg_log_opengl,                          createStringDefault          },
            {cfg_log_scsp,                            createStringDefault          },
            {cfg_log_scu,                             createStringDefault          },
            {cfg_log_sh2,                             createStringDefault          },
            {cfg_log_smpc,                            createStringDefault          },
            {cfg_log_vdp1,                            createStringDefault          },
            {cfg_log_vdp2,                            createStringDefault          },
            {cfg_log_unimplemented,                   createStringDefault          },
            {cfg_global_set_time,                     createBoolDefault            },
            {cfg_global_stv_bios_bypass,              createBoolDefault            },
            {cfg_rendering_legacy_opengl,             createBoolDefault            },
            {cfg_sound_disabled,                      createBoolDefault            },
            {cfg_controls_saturn_player_1,            createSaturnControlDefault   },
            {cfg_controls_saturn_player_2,            createSaturnControlEmpty     },
            {cfg_controls_stv_board,                  createStvBoardControlDefault },
            {cfg_controls_stv_player_1,               createStvPlayerControlDefault},
            {cfg_controls_stv_player_2,               createStvPlayerControlEmpty  },
            {cfg_advanced_sh2_core,                   createStringDefault          }
    };
        keyToLambda.contains(key)) {
        keyToLambda[key]();
    } else {
        Log::exception(Logger::config, tr("Configuration : undefined default value '{}'!"), full_keys_[key]);
    }

    this->writeFile();
}
auto Config::readPeripheralConfiguration(const AccessKeys& key) -> std::vector<PeripheralKey> {
    auto        pad_values  = std::vector<PeripheralKey>{};
    const auto& pad_setting = readValue(key);
    for (int i = 0; i < pad_setting.getLength(); ++i) {
        pad_values.push_back(static_cast<PeripheralKey>(static_cast<int>(pad_setting[i])));
    }
    return pad_values;
}

auto Config::getRomLoad(const std::string& key) -> RomLoad { return rom_load_[key]; }

auto Config::getRomType(const std::string& key) -> RomType { return rom_type_[key]; }

auto Config::getHardwareMode(const std::string& key) -> HardwareMode { return hardware_mode_[key]; }

auto Config::getHardwareModeKey(const HardwareMode value) const -> std::optional<std::string> {
    if (auto it = util::getKeyFromValue(hardware_mode_, value); it != hardware_mode_.end()) {
        return std::optional<std::string>{it->first};
    }
    return std::nullopt;
}

auto Config::getCdromAccess(const std::string& key) -> cdrom::CdromAccessMethod { return cdrom_access_[key]; }

auto Config::getCdromAccessKey(const cdrom::CdromAccessMethod value) const -> std::optional<std::string> {
    if (auto it = util::getKeyFromValue(cdrom_access_, value); it != cdrom_access_.end()) {
        return std::optional<std::string>{it->first};
    }
    return std::nullopt;
}

auto Config::getTvStandard(const std::string& key) -> video::TvStandard { return tv_standard_[key]; }

auto Config::getTvStandardKey(const video::TvStandard value) const -> std::optional<std::string> {
    if (auto it = util::getKeyFromValue(tv_standard_, value); it != tv_standard_.end()) {
        return std::optional<std::string>{it->first};
    }
    return std::nullopt;
}

auto Config::getAreaCode(const std::string& key) -> AreaCode { return area_code_[key]; }

auto Config::getAreaCodeKey(const AreaCode value) const -> std::optional<std::string> {
    if (auto it = util::getKeyFromValue(area_code_, value); it != area_code_.end()) {
        return std::optional<std::string>{it->first};
    }
    return std::nullopt;
}

auto Config::getCurrentSh2Core() -> sh2::Sh2Core {
    const std::string key = readValue(core::AccessKeys::cfg_advanced_sh2_core);
    return sh2_core_[key];
}

void Config::updateLogLevel() {
    using enum AccessKeys;
    Log::setLogLevel("cdrom", getLogLevel(readValue(cfg_log_cdrom)));
    Log::setLogLevel("config", getLogLevel(readValue(cfg_log_config)));
    Log::setLogLevel("main", getLogLevel(readValue(cfg_log_main)));
    Log::setLogLevel("memory", getLogLevel(readValue(cfg_log_memory)));
    Log::setLogLevel("opengl", getLogLevel(readValue(cfg_log_opengl)));
    Log::setLogLevel("scu", getLogLevel(readValue(cfg_log_scu)));
    Log::setLogLevel("scsp", getLogLevel(readValue(cfg_log_scsp)));
    Log::setLogLevel("sh2", getLogLevel(readValue(cfg_log_sh2)));
    Log::setLogLevel("smpc", getLogLevel(readValue(cfg_log_smpc)));
    Log::setLogLevel("vdp1", getLogLevel(readValue(cfg_log_vdp1)));
    Log::setLogLevel("vdp2", getLogLevel(readValue(cfg_log_vdp2)));
    Log::setLogLevel("unimplemented", getLogLevel(readValue(cfg_log_unimplemented)));
}

auto Config::getLogLevel(const std::string& key) -> LogLevel { return log_level_[key]; }

/* static */
auto Config::listAvailableLanguages() -> std::vector<std::string> {
    const auto full_path = fs::current_path() / "lang";
    auto       files     = std::vector<std::string>{"en"}; // english is the default language, even if the directory isn't present
    for (auto& p : fs::directory_iterator(full_path)) {
        if ((p.is_directory()) && (p.path().filename().string() != "en")) { files.push_back(p.path().filename().string()); }
    }
    return files;
}

auto Config::listAreaCodes() const -> std::vector<std::string> {
    auto codes = std::vector<std::string>{};
    for (const auto& [key, value] : area_code_) {
        codes.push_back(key);
    }
    return codes;
}

auto Config::listPeripheralConnections() const -> std::vector<std::string> {
    auto connections = std::vector<std::string>{};
    for (const auto& [key, value] : port_status_) {
        connections.push_back(key);
    }
    return connections;
}

auto Config::listLogLevels() const -> std::vector<std::string> {
    auto levels = std::vector<std::string>{};
    for (const auto& [key, value] : log_level_) {
        levels.push_back(key);
    }
    return levels;
}

auto Config::listSh2Cores() const -> std::vector<std::string> {
    auto cores = std::vector<std::string>{};
    for (const auto& [key, value] : sh2_core_) {
        cores.push_back(key);
    }
    return cores;
}

auto Config::configToPortStatus(const std::string& value) -> PortStatus { return port_status_[value]; }

}; // namespace saturnin::core
