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

#include <windows.h> // removes C4005 warning
#include <GLFW/glfw3.h> // GLFW_KEYS
#include <iostream> // cout
#include <vector> // vector
#include "config.h"
#include "cdrom/cdrom.h"
#include "locale.h"
#include "log.h"


namespace libcfg    = libconfig;
namespace fs        = std::filesystem;
namespace cdrom     = saturnin::cdrom;

namespace saturnin {
namespace core {

using core::Log;

Config::MapKeys Config::full_keys = { 
    { AccessKeys::cfg_global_language,         "global.language" }, 
    { AccessKeys::cfg_global_hardware_mode,    "global.hardware_mode" },
    { AccessKeys::cfg_rendering_legacy_opengl, "rendering.legacy_opengl" },
    { AccessKeys::cfg_rendering_tv_standard,   "rendering.tv_standard" },
    { AccessKeys::cfg_paths_roms_stv,          "paths.roms_stv" },
    { AccessKeys::cfg_paths_bios_stv,          "paths.bios_stv" },
    { AccessKeys::cfg_paths_bios_saturn,       "paths.bios_saturn" },
    { AccessKeys::cfg_cdrom_drive,             "cdrom.drive" },
    { AccessKeys::cfg_cdrom_access_method,     "cdrom.access_method" },
    { AccessKeys::cfg_sound_soundcard,         "sound.soundcard" },
    { AccessKeys::cfg_sound_disabled,          "sound.disabled" },
    { AccessKeys::cfg_controls_saturn_player_1,"controls.saturn.player_1" },
    { AccessKeys::cfg_controls_saturn_player_2,"controls.saturn.player_2" },
    { AccessKeys::cfg_controls_stv_board,      "controls.stv.board" },
    { AccessKeys::cfg_controls_stv_player_1,   "controls.stv.player_1" },
    { AccessKeys::cfg_controls_stv_player_2,   "controls.stv.player_2" },
    { AccessKeys::stv_game_name,               "game_name" },
    { AccessKeys::stv_zip_name,                "zip_name" },
    { AccessKeys::stv_parent_set,              "parent_set" },
    { AccessKeys::stv_version,                 "version" },
    { AccessKeys::stv_release_date,            "release_date" },
    { AccessKeys::stv_region,                  "region" },
    { AccessKeys::stv_files,                   "files" }
};

Config::MapKeysDefault Config::default_keys = {
    { AccessKeys::cfg_global_language,         std::string("en") },
    { AccessKeys::cfg_global_hardware_mode,    std::string("SATURN") },
    { AccessKeys::cfg_rendering_tv_standard,   std::string("PAL") },
    { AccessKeys::cfg_rendering_legacy_opengl, false },
    { AccessKeys::cfg_paths_roms_stv,          std::string("") },
    { AccessKeys::cfg_paths_bios_stv,          std::string("") },
    { AccessKeys::cfg_paths_bios_saturn,       std::string("") },
    { AccessKeys::cfg_cdrom_drive,             std::string("-1:-1:-1") },
    { AccessKeys::cfg_cdrom_access_method,     std::string("SPTI") },
    { AccessKeys::cfg_sound_soundcard,         std::string("") },
    { AccessKeys::cfg_sound_disabled,          false }
};

Config::MapRomLoad Config::rom_load = {
    {"NOT_INTERLEAVED", RomLoad::not_interleaved},
    {"ODD_INTERLEAVED", RomLoad::odd_interleaved},
    {"EVEN_INTERLEAVED", RomLoad::even_interleaved}
};

Config::MapRomType Config::rom_type = {
    {"PROGRAM", RomType::program},
    {"GRAPHIC", RomType::graphic},
    {"BIOS",    RomType::bios}
};

Config::MapCdromAccess Config::cdrom_access = {
    {"ASPI", cdrom::CdromAccessMethod::aspi},
    {"SPTI", cdrom::CdromAccessMethod::spti}
};

Config::MapHardwareMode Config::hardware_mode= {
    {"SATURN", HardwareMode::saturn},
    {"STV",    HardwareMode::stv}
};

Config::MapTvStandard Config::tv_standard = {
    {"PAL",  video::TvStandard::pal},
    {"NTSC", video::TvStandard::ntsc}
};

void Config::writeFile() {
    cfg_.writeFile(this->filename_.c_str());
}

bool Config::readFile(){
    try {
        cfg_.readFile(filename_.c_str());
        return true;
    }
    catch (const libcfg::FileIOException &fioex) {
            
        auto errorString = fmt::format(tr("Could not read file {0} : {1}"), filename_, fioex.what());
        //cout << fmt::format(tr("Could not read file {0} "), filename) << fioex.what() << endl;
        Log::error("config", errorString);
        //throw std::runtime_error("Config error !");
        return false;
    }
}

bool Config::initialize(const bool isModernOpenGlCapable) {
    if (!readFile()) {
        std::cout << tr("Creating configuration file.") << std::endl;
        this->generateConfigurationTree(isModernOpenGlCapable);
        this->writeFile();
    }
    return true;
}

void Config::generateConfigurationTree(const bool isModernOpenglCapable) {

    add(full_keys[AccessKeys::cfg_global_language],          std::any_cast<const std::string&>(default_keys[AccessKeys::cfg_global_language]));
    add(full_keys[AccessKeys::cfg_global_hardware_mode],     std::any_cast<const std::string&>(default_keys[AccessKeys::cfg_global_hardware_mode]));
    add(full_keys[AccessKeys::cfg_rendering_tv_standard],    std::any_cast<const std::string&>(default_keys[AccessKeys::cfg_rendering_tv_standard]));
    add(full_keys[AccessKeys::cfg_rendering_legacy_opengl],  !isModernOpenglCapable);
    add(full_keys[AccessKeys::cfg_paths_roms_stv],           std::any_cast<const std::string&>(default_keys[AccessKeys::cfg_paths_roms_stv]));
    add(full_keys[AccessKeys::cfg_paths_bios_stv],           std::any_cast<const std::string&>(default_keys[AccessKeys::cfg_paths_bios_stv]));
    add(full_keys[AccessKeys::cfg_paths_bios_saturn],        std::any_cast<const std::string&>(default_keys[AccessKeys::cfg_paths_bios_saturn]));
    add(full_keys[AccessKeys::cfg_cdrom_drive],              std::any_cast<const std::string&>(default_keys[AccessKeys::cfg_cdrom_drive]));
    add(full_keys[AccessKeys::cfg_cdrom_access_method],      std::any_cast<const std::string&>(default_keys[AccessKeys::cfg_cdrom_access_method]));
    add(full_keys[AccessKeys::cfg_sound_disabled],           std::any_cast<const bool>(default_keys[AccessKeys::cfg_sound_disabled]));
    add(full_keys[AccessKeys::cfg_controls_saturn_player_1], SaturnDigitalPad().toConfig(PeripheralLayout::default));
    add(full_keys[AccessKeys::cfg_controls_saturn_player_2], SaturnDigitalPad().toConfig(PeripheralLayout::empty));
    add(full_keys[AccessKeys::cfg_controls_stv_board],       StvBoardControls().toConfig(PeripheralLayout::default));
    add(full_keys[AccessKeys::cfg_controls_stv_player_1],    StvPlayerControls().toConfig(PeripheralLayout::default));
    add(full_keys[AccessKeys::cfg_controls_stv_player_2],    StvPlayerControls().toConfig(PeripheralLayout::empty));
}

libcfg::Setting& Config::getGroup(libcfg::Setting& root, const std::string& group_name) {
    if (!root.exists(group_name.c_str())) root.add(group_name.c_str(), libcfg::Setting::TypeGroup);
    return root[group_name.c_str()];
}

std::string Config::addGroup(libcfg::Setting& root, const std::string& group_name) {
    // libcfg::Setting doesn't have a copy constructor, so it has to be declared twice
    if (!root.exists(group_name)) {
        libcfg::Setting& new_setting = root.add(group_name, libcfg::Setting::TypeGroup);
        return new_setting.getPath();
    } else {
        libcfg::Setting& new_setting = root.lookup(group_name);
        return new_setting.getPath();
    }
}

void Config::test() {
    libcfg::Setting& root = cfg_.getRoot();
    std::string str{"test"};
        
    this->writeValue(root, "test_c_string", str.c_str());
    this->writeValue(root, "test_string", std::string{ "test" });
    this->writeValue(root, "test_char_array", "test");

    this->writeFile();
}

libcfg::Setting& Config::readValue(const AccessKeys& value) {
    try {
        if (!existsValue(value)) createDefault(value);
        return cfg_.lookup(Config::full_keys[value]);
    }
    catch (const libcfg::SettingNotFoundException& e) {
        auto errorString = fmt::format(tr("Setting '{0}' not found !"), e.getPath());
        Log::error("config", errorString);
        throw std::runtime_error("Config error !");
    }
}

bool Config::existsValue(const AccessKeys& value) {
    return cfg_.exists(Config::full_keys[value]);
}

void Config::createDefault(const AccessKeys& value) {
    switch (value) {
        case AccessKeys::cfg_global_language: 
        case AccessKeys::cfg_global_hardware_mode: 
        case AccessKeys::cfg_rendering_tv_standard: 
        case AccessKeys::cfg_paths_roms_stv:
        case AccessKeys::cfg_paths_bios_stv:
        case AccessKeys::cfg_paths_bios_saturn:
        case AccessKeys::cfg_cdrom_drive:
        case AccessKeys::cfg_cdrom_access_method:
        case AccessKeys::cfg_sound_soundcard:
            add(full_keys[value], std::any_cast<const std::string>(default_keys[value]));
            break;
        case AccessKeys::cfg_rendering_legacy_opengl:
        case AccessKeys::cfg_sound_disabled:
            add(full_keys[value], std::any_cast<const bool>(default_keys[value]));
            break;
        default : {
            Log::error("config", tr("Undefined default value !"));
            throw std::runtime_error("Config error !");
        }
    }
    writeFile();
}
std::vector<PeripheralKey> Config::readPeripheralConfiguration(const AccessKeys& key) {
    std::vector<PeripheralKey> pad_values;
    const auto& pad_setting = readValue(key);
    for (int i = 0; i < pad_setting.getLength(); ++i) {
        pad_values.push_back(static_cast<PeripheralKey>(static_cast<int>(pad_setting[i])));
    }
    return pad_values;
}

std::vector<std::string> Config::listAvailableLanguages() {
    auto full_path = fs::current_path() / "lang";
    std::vector<std::string> files{ "en" }; // english is the default language, even if the directory isn't present
    for (auto& p : fs::directory_iterator(full_path)) {
        if ((p.is_directory()) && (p.path().filename().string() != "en")) {
            files.push_back(p.path().filename().string());
        }
    }
    return files;
}

};
};