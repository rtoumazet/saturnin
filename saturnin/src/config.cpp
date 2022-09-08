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
//#include <saturnin/src/locale.h> //NOLINT(modernize-deprecated-headers)
#include <saturnin/src/log.h>

namespace libcfg = libconfig;
namespace fs     = std::filesystem;
namespace cdrom  = saturnin::cdrom;
namespace uti    = saturnin::utilities;

namespace saturnin::core {

using core::Log;

// Config::Config(const std::string& configuration_filename) {
Config::Config(std::string_view configuration_filename) {
    filename_ = configuration_filename;

    full_keys_ = {{AccessKeys::cfg_global_language, "global.language"},
                  {AccessKeys::cfg_global_hardware_mode, "global.hardware_mode"},
                  {AccessKeys::cfg_global_area_code, "global.area_code"},
                  {AccessKeys::cfg_global_set_time, "global.set_time"},
                  {AccessKeys::cfg_rendering_legacy_opengl, "rendering.legacy_opengl"},
                  {AccessKeys::cfg_rendering_tv_standard, "rendering.tv_standard"},
                  {AccessKeys::cfg_paths_roms_stv, "paths.roms_stv"},
                  {AccessKeys::cfg_paths_bios_stv, "paths.bios_stv"},
                  {AccessKeys::cfg_paths_bios_saturn, "paths.bios_saturn"},
                  {AccessKeys::cfg_cdrom_drive, "cdrom.drive"},
                  {AccessKeys::cfg_cdrom_access_method, "cdrom.access_method"},
                  {AccessKeys::cfg_sound_soundcard, "sound.soundcard"},
                  {AccessKeys::cfg_sound_disabled, "sound.disabled"},
                  {AccessKeys::cfg_controls_saturn_player_1, "controls.saturn.player_1"},
                  {AccessKeys::cfg_controls_saturn_player_1_connection, "controls.saturn.player_1_connection"},
                  {AccessKeys::cfg_controls_saturn_player_2, "controls.saturn.player_2"},
                  {AccessKeys::cfg_controls_saturn_player_2_connection, "controls.saturn.player_2_connection"},
                  {AccessKeys::cfg_controls_stv_board, "controls.stv.board"},
                  {AccessKeys::cfg_controls_stv_player_1, "controls.stv.player_1"},
                  {AccessKeys::cfg_controls_stv_player_2, "controls.stv.player_2"},
                  {AccessKeys::cfg_log_cdrom, "logs.cdrom"},
                  {AccessKeys::cfg_log_config, "logs.config"},
                  {AccessKeys::cfg_log_main, "logs.main"},
                  {AccessKeys::cfg_log_memory, "logs.memory"},
                  {AccessKeys::cfg_log_sh2, "logs.sh2"},
                  {AccessKeys::cfg_log_scsp, "logs.scsp"},
                  {AccessKeys::cfg_log_scu, "logs.scu"},
                  {AccessKeys::cfg_log_smpc, "logs.smpc"},
                  {AccessKeys::cfg_log_vdp1, "logs.vdp1"},
                  {AccessKeys::cfg_log_vdp2, "logs.vdp2"},
                  {AccessKeys::cfg_log_unimplemented, "logs.unimplemented"},
                  {AccessKeys::stv_game_name, "game_name"},
                  {AccessKeys::stv_zip_name, "zip_name"},
                  {AccessKeys::stv_parent_set, "parent_set"},
                  {AccessKeys::stv_version, "version"},
                  {AccessKeys::stv_release_date, "release_date"},
                  {AccessKeys::stv_region, "region"},
                  {AccessKeys::stv_files, "files"}};

    default_keys_ = {{AccessKeys::cfg_global_language, std::string("en")},
                     {AccessKeys::cfg_global_hardware_mode, std::string("SATURN")},
                     {AccessKeys::cfg_global_area_code, std::string("EUROPE_PAL")},
                     {AccessKeys::cfg_global_set_time, true},
                     {AccessKeys::cfg_rendering_tv_standard, std::string("PAL")},
                     {AccessKeys::cfg_rendering_legacy_opengl, false},
                     {AccessKeys::cfg_paths_roms_stv, std::string("")},
                     {AccessKeys::cfg_paths_bios_stv, std::string("")},
                     {AccessKeys::cfg_paths_bios_saturn, std::string("")},
                     {AccessKeys::cfg_cdrom_drive, std::string("-1:-1:-1")},
                     {AccessKeys::cfg_cdrom_access_method, std::string("SPTI")},
                     {AccessKeys::cfg_sound_soundcard, std::string("")},
                     {AccessKeys::cfg_sound_disabled, false},
                     {AccessKeys::cfg_controls_saturn_player_1_connection, std::string("DIRECT")},
                     {AccessKeys::cfg_controls_saturn_player_2_connection, std::string("NONE")},
                     {AccessKeys::cfg_log_cdrom, std::string("INFO")},
                     {AccessKeys::cfg_log_config, std::string("INFO")},
                     {AccessKeys::cfg_log_main, std::string("INFO")},
                     {AccessKeys::cfg_log_memory, std::string("INFO")},
                     {AccessKeys::cfg_log_sh2, std::string("INFO")},
                     {AccessKeys::cfg_log_scsp, std::string("INFO")},
                     {AccessKeys::cfg_log_scu, std::string("INFO")},
                     {AccessKeys::cfg_log_smpc, std::string("INFO")},
                     {AccessKeys::cfg_log_vdp1, std::string("INFO")},
                     {AccessKeys::cfg_log_vdp2, std::string("INFO")},
                     {AccessKeys::cfg_log_unimplemented, std::string("INFO")}

    };

    rom_load_ = {{"NOT_INTERLEAVED", RomLoad::not_interleaved},
                 {"ODD_INTERLEAVED", RomLoad::odd_interleaved},
                 {"EVEN_INTERLEAVED", RomLoad::even_interleaved}};

    rom_type_ = {{"PROGRAM", RomType::program}, {"GRAPHIC", RomType::graphic}, {"BIOS", RomType::bios}};

    cdrom_access_ = {{"ASPI", cdrom::CdromAccessMethod::aspi}, {"SPTI", cdrom::CdromAccessMethod::spti}};

    hardware_mode_ = {{"SATURN", HardwareMode::saturn}, {"STV", HardwareMode::stv}};

    tv_standard_ = {{"PAL", video::TvStandard::pal}, {"NTSC", video::TvStandard::ntsc}};

    area_code_ = {{"JAPAN", AreaCode::japan},
                  {"ASIA_NTSC", AreaCode::asia_ntsc},
                  {"NORTH_AMERICA", AreaCode::north_america},
                  {"CENTRAL_SOUTH_AMERICA_NTSC", AreaCode::central_south_america_ntsc},
                  {"KOREA", AreaCode::korea},
                  {"ASIA_PAL", AreaCode::asia_pal},
                  {"EUROPE_PAL", AreaCode::europe_pal},
                  {"CENTRAL_SOUTH_AMERICA_PAL", AreaCode::central_south_america_pal}};

    port_status_ = {{"NONE", PortStatus::not_connected},
                    {"DIRECT", PortStatus::direct_connection},
                    {"SEGATAP", PortStatus::sega_tap},
                    {"MULTITAP", PortStatus::saturn_6p_multitap}};

    log_level_ = {{"OFF", LogLevel::off}, {"INFO", LogLevel::info}, {"DEBUG", LogLevel::debug}};
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
    // clang-format off
    add(full_keys_[AccessKeys::cfg_global_language],                     std::any_cast<const std::string&>(default_keys_[AccessKeys::cfg_global_language]));
    add(full_keys_[AccessKeys::cfg_global_hardware_mode],                std::any_cast<const std::string&>(default_keys_[AccessKeys::cfg_global_hardware_mode]));
    add(full_keys_[AccessKeys::cfg_global_area_code],                    std::any_cast<const std::string&>(default_keys_[AccessKeys::cfg_global_area_code]));
    add(full_keys_[AccessKeys::cfg_global_set_time],                    std::any_cast<const bool>(default_keys_[AccessKeys::cfg_global_set_time]));
    add(full_keys_[AccessKeys::cfg_rendering_tv_standard],               std::any_cast<const std::string&>(default_keys_[AccessKeys::cfg_rendering_tv_standard]));
    add(full_keys_[AccessKeys::cfg_rendering_legacy_opengl],             !isModernOpenglCapable);
    add(full_keys_[AccessKeys::cfg_paths_roms_stv],                      std::any_cast<const std::string&>(default_keys_[AccessKeys::cfg_paths_roms_stv]));
    add(full_keys_[AccessKeys::cfg_paths_bios_stv],                      std::any_cast<const std::string&>(default_keys_[AccessKeys::cfg_paths_bios_stv]));
    add(full_keys_[AccessKeys::cfg_paths_bios_saturn],                   std::any_cast<const std::string&>(default_keys_[AccessKeys::cfg_paths_bios_saturn]));
    add(full_keys_[AccessKeys::cfg_cdrom_drive],                         std::any_cast<const std::string&>(default_keys_[AccessKeys::cfg_cdrom_drive]));
    add(full_keys_[AccessKeys::cfg_cdrom_access_method],                 std::any_cast<const std::string&>(default_keys_[AccessKeys::cfg_cdrom_access_method]));
    add(full_keys_[AccessKeys::cfg_sound_disabled],                      std::any_cast<const bool>(default_keys_[AccessKeys::cfg_sound_disabled]));
    add(full_keys_[AccessKeys::cfg_controls_saturn_player_1_connection], std::any_cast<const std::string&>(default_keys_[AccessKeys::cfg_controls_saturn_player_1_connection]));
    add(full_keys_[AccessKeys::cfg_controls_saturn_player_1],            SaturnDigitalPad().toConfig(PeripheralLayout::default_layout));
    add(full_keys_[AccessKeys::cfg_controls_saturn_player_2_connection], std::any_cast<const std::string&>(default_keys_[AccessKeys::cfg_controls_saturn_player_2_connection]));
    add(full_keys_[AccessKeys::cfg_controls_saturn_player_2],            SaturnDigitalPad().toConfig(PeripheralLayout::empty_layout));
    add(full_keys_[AccessKeys::cfg_controls_stv_board],                  StvBoardControls().toConfig(PeripheralLayout::default_layout));
    add(full_keys_[AccessKeys::cfg_controls_stv_player_1],               StvPlayerControls().toConfig(PeripheralLayout::default_layout));
    add(full_keys_[AccessKeys::cfg_controls_stv_player_2],               StvPlayerControls().toConfig(PeripheralLayout::empty_layout));
    add(full_keys_[AccessKeys::cfg_log_cdrom],                           std::any_cast<const std::string&>(default_keys_[AccessKeys::cfg_log_cdrom]));
    add(full_keys_[AccessKeys::cfg_log_config],                           std::any_cast<const std::string&>(default_keys_[AccessKeys::cfg_log_config]));
    add(full_keys_[AccessKeys::cfg_log_main],                           std::any_cast<const std::string&>(default_keys_[AccessKeys::cfg_log_main]));
    add(full_keys_[AccessKeys::cfg_log_memory],                           std::any_cast<const std::string&>(default_keys_[AccessKeys::cfg_log_memory]));
    add(full_keys_[AccessKeys::cfg_log_scsp],                           std::any_cast<const std::string&>(default_keys_[AccessKeys::cfg_log_scsp]));
    add(full_keys_[AccessKeys::cfg_log_scu],                           std::any_cast<const std::string&>(default_keys_[AccessKeys::cfg_log_scu]));
    add(full_keys_[AccessKeys::cfg_log_sh2],                           std::any_cast<const std::string&>(default_keys_[AccessKeys::cfg_log_sh2]));
    add(full_keys_[AccessKeys::cfg_log_smpc],                           std::any_cast<const std::string&>(default_keys_[AccessKeys::cfg_log_smpc]));
    add(full_keys_[AccessKeys::cfg_log_vdp1],                           std::any_cast<const std::string&>(default_keys_[AccessKeys::cfg_log_vdp1]));
    add(full_keys_[AccessKeys::cfg_log_vdp2],                           std::any_cast<const std::string&>(default_keys_[AccessKeys::cfg_log_vdp2]));
    add(full_keys_[AccessKeys::cfg_log_unimplemented],                  std::any_cast<const std::string&>(default_keys_[AccessKeys::cfg_log_unimplemented]));
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
        const auto error = uti::format(tr("Setting '{0}' not found !"), e.getPath());
        Log::error(Logger::config, error);
        throw std::runtime_error("Config error !");
    }
}

auto Config::existsValue(const AccessKeys& value) -> bool { return cfg_.exists(full_keys_[value]); }

void Config::createDefault(const AccessKeys& key) {
    switch (key) {
        using enum AccessKeys;
        case cfg_global_language:
        case cfg_global_hardware_mode:
        case cfg_global_area_code:
        case cfg_rendering_tv_standard:
        case cfg_paths_roms_stv:
        case cfg_paths_bios_stv:
        case cfg_paths_bios_saturn:
        case cfg_cdrom_drive:
        case cfg_cdrom_access_method:
        case cfg_sound_soundcard:
        case cfg_controls_saturn_player_1_connection:
        case cfg_controls_saturn_player_2_connection:
        case cfg_log_cdrom:
        case cfg_log_config:
        case cfg_log_main:
        case cfg_log_memory:
        case cfg_log_scsp:
        case cfg_log_scu:
        case cfg_log_sh2:
        case cfg_log_smpc:
        case cfg_log_vdp1:
        case cfg_log_vdp2:
        case cfg_log_unimplemented: {
            add(full_keys_[key], std::any_cast<const std::string>(default_keys_[key]));
            break;
        }
        case cfg_global_set_time:
        case cfg_rendering_legacy_opengl:
        case cfg_sound_disabled: {
            add(full_keys_[key], std::any_cast<const bool>(default_keys_[key]));
            break;
        }
        case cfg_controls_saturn_player_1:
            add(full_keys_[key], SaturnDigitalPad().toConfig(PeripheralLayout::default_layout));
            break;
        case cfg_controls_saturn_player_2:
            add(full_keys_[key], SaturnDigitalPad().toConfig(PeripheralLayout::empty_layout));
            break;
        case cfg_controls_stv_board: add(full_keys_[key], StvBoardControls().toConfig(PeripheralLayout::default_layout)); break;
        case cfg_controls_stv_player_1:
            add(full_keys_[key], StvPlayerControls().toConfig(PeripheralLayout::default_layout));
            break;
        case cfg_controls_stv_player_2: add(full_keys_[key], StvPlayerControls().toConfig(PeripheralLayout::empty_layout)); break;
        default: {
            Log::error(Logger::config, tr("Undefined default value '{}'!"), full_keys_[key]);
            throw std::runtime_error("Config error !");
        }
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
    const auto it = util::getKeyFromValue(hardware_mode_, value);
    if (it != hardware_mode_.end()) { return std::optional<std::string>{it->first}; }
    return std::nullopt;
}

auto Config::getCdromAccess(const std::string& key) -> cdrom::CdromAccessMethod { return cdrom_access_[key]; }

auto Config::getCdromAccessKey(const cdrom::CdromAccessMethod value) const -> std::optional<std::string> {
    const auto it = util::getKeyFromValue(cdrom_access_, value);
    if (it != cdrom_access_.end()) { return std::optional<std::string>{it->first}; }
    return std::nullopt;
}

auto Config::getTvStandard(const std::string& key) -> video::TvStandard { return tv_standard_[key]; }

auto Config::getTvStandardKey(const video::TvStandard value) const -> std::optional<std::string> {
    const auto it = util::getKeyFromValue(tv_standard_, value);
    if (it != tv_standard_.end()) { return std::optional<std::string>{it->first}; }
    return std::nullopt;
}

auto Config::getAreaCode(const std::string& key) -> AreaCode { return area_code_[key]; }

auto Config::getAreaCodeKey(const AreaCode value) const -> std::optional<std::string> {
    const auto it = util::getKeyFromValue(area_code_, value);
    if (it != area_code_.end()) { return std::optional<std::string>{it->first}; }
    return std::nullopt;
}

void Config::updateLogLevel() {
    Log::setLogLevel("cdrom", getLogLevel(readValue(AccessKeys::cfg_log_cdrom)));
    Log::setLogLevel("config", getLogLevel(readValue(AccessKeys::cfg_log_config)));
    Log::setLogLevel("main", getLogLevel(readValue(AccessKeys::cfg_log_main)));
    Log::setLogLevel("memory", getLogLevel(readValue(AccessKeys::cfg_log_memory)));
    Log::setLogLevel("scu", getLogLevel(readValue(AccessKeys::cfg_log_scu)));
    Log::setLogLevel("scsp", getLogLevel(readValue(AccessKeys::cfg_log_scsp)));
    Log::setLogLevel("sh2", getLogLevel(readValue(AccessKeys::cfg_log_sh2)));
    Log::setLogLevel("smpc", getLogLevel(readValue(AccessKeys::cfg_log_smpc)));
    Log::setLogLevel("vdp1", getLogLevel(readValue(AccessKeys::cfg_log_vdp1)));
    Log::setLogLevel("vdp2", getLogLevel(readValue(AccessKeys::cfg_log_vdp2)));
    Log::setLogLevel("unimplemented", getLogLevel(readValue(AccessKeys::cfg_log_unimplemented)));
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

auto Config::listAreaCodes() -> std::vector<std::string> {
    auto codes = std::vector<std::string>{};
    for (const auto& code : area_code_) {
        codes.push_back(code.first);
    }
    return codes;
}

auto Config::listPeripheralConnections() -> std::vector<std::string> {
    auto connections = std::vector<std::string>{};
    for (const auto& connection : port_status_) {
        connections.push_back(connection.first);
    }
    return connections;
}

auto Config::listLogLevels() -> std::vector<std::string> {
    auto levels = std::vector<std::string>{};
    for (const auto& level : log_level_) {
        levels.push_back(level.first);
    }
    return levels;
}

auto Config::configToPortStatus(const std::string value) -> PortStatus { return port_status_[value]; }

void logError(const std::string& error, const std::string& path) {
    const auto str = uti::format(error, path);
    Log::error(Logger::config, str);
};

}; // namespace saturnin::core
