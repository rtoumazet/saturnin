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
#include "utilities.h" // toUnderlying

namespace libcfg    = libconfig;
namespace fs        = std::filesystem;
namespace cdrom     = saturnin::cdrom;
namespace util      = saturnin::utilities;

namespace saturnin {
namespace core {

using core::Log;

Config::Map_keys Config::single_keys = {
    { Access_keys::config_global,        "global"},
    { Access_keys::config_language,      "language"},
    { Access_keys::config_hardware_mode, "hardware_mode" },
    { Access_keys::config_rendering,     "rendering"},
    { Access_keys::config_legacy_opengl, "legacy_opengl" },
    { Access_keys::config_paths,         "paths" },
    { Access_keys::config_roms_stv,      "roms_stv" },
    { Access_keys::config_bios_stv,      "bios_stv" },
    { Access_keys::config_bios_saturn,   "bios_saturn" },
    { Access_keys::config_cdrom,         "cdrom" },
    { Access_keys::config_drive,         "drive" },
    { Access_keys::config_access_method, "access_method" },
    { Access_keys::config_sound,         "sound" },
    { Access_keys::config_soundcard,     "soundcard" },
    { Access_keys::config_sound_disabled,"disabled" },
};

Config::Map_keys Config::full_keys = {
    { Access_keys::config_global,        "global"},
    { Access_keys::config_language,      "global.language"},
    { Access_keys::config_hardware_mode, "global.hardware_mode" },
    { Access_keys::config_rendering,     "rendering" },
    { Access_keys::config_legacy_opengl, "rendering.legacy_opengl" },
    { Access_keys::config_paths,         "paths" },
    { Access_keys::config_roms_stv,      "paths.roms_stv" },
    { Access_keys::config_bios_stv,      "paths.bios_stv" },
    { Access_keys::config_bios_saturn,   "paths.bios_saturn" },
    { Access_keys::config_cdrom,         "cdrom" },
    { Access_keys::config_drive,         "cdrom.drive" },
    { Access_keys::config_access_method, "cdrom.access_method" },
    { Access_keys::config_sound,         "sound" },
    { Access_keys::config_soundcard,     "sound.soundcard" },
    { Access_keys::config_sound_disabled,"sound.disabled" },
    { Access_keys::stv_game_name,        "game_name" },
    { Access_keys::stv_zip_name,         "zip_name" },
    { Access_keys::stv_parent_set,       "parent_set" },
    { Access_keys::stv_version,          "version" },
    { Access_keys::stv_release_date,     "release_date" },
    { Access_keys::stv_region,           "region" },
    { Access_keys::stv_files,            "files" }
};

Config::Map_rom_load Config::rom_load = {
    {"NOT_INTERLEAVED", Rom_load::not_interleaved},
    {"ODD_INTERLEAVED", Rom_load::odd_interleaved},
    {"EVEN_INTERLEAVED", Rom_load::even_interleaved}
};

Config::Map_rom_type Config::rom_type = {
    {"PROGRAM", Rom_type::program},
    {"GRAPHIC", Rom_type::graphic},
    {"BIOS",    Rom_type::bios}
};

Config::Map_cdrom_access Config::cdrom_access = {
    {"ASPI", cdrom::Cdrom_access_method::aspi},
    {"SPTI", cdrom::Cdrom_access_method::spti}
};

Config::Map_HardwareMode Config::hardware_mode= {
    {"SATURN", core::HardwareMode::saturn},
    {"STV", core::HardwareMode::stv}
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


    add("global.language", "en");
    
    core::Config::Map_HardwareMode::const_iterator it_hm = util::getKeyFromValue(core::Config::hardware_mode, core::HardwareMode::saturn);
    if (it_hm != core::Config::hardware_mode.end()) add("global.hardware_mode", it_hm->first.c_str());
    else Log::warning("config", core::tr("Unknown harware mode ..."));
  
    add("rendering.legacy_opengl", !isModernOpenglCapable);
    add("paths.roms_stv", "");
    add("paths.bios_stv", "");
    add("paths.bios_saturn", "");
    add("cdrom.drive", "-1:-1:-1");

    core::Config::Map_cdrom_access::const_iterator it = util::getKeyFromValue(core::Config::cdrom_access, cdrom::Cdrom_access_method::spti);
    if (it != core::Config::cdrom_access.end()) add("cdrom.access_method", it->first.c_str());
    else Log::warning("config", core::tr("Unknown drive access method ..."));

    add("sound.disabled", false);
    
    SaturnDigitalPad pad;
    pad.mapDefaultLayout();
    add("controls.saturn.player_1", pad.toConfig());
    add("controls.saturn.player_2", std::vector<u16>());

    StvBoardControls controls;
    controls.mapDefaultLayout();
    add("controls.stv.board", controls.toConfig());
    
    StvPlayerControls stv_player;
    stv_player.mapDefaultLayout();
    add("controls.stv.player_1", stv_player.toConfig());
    add("controls.stv.player_2", std::vector<u16>());

}

std::vector<u16> Config::generateControllerMapping(const HardwareMode& hm) {
    switch (hm) {
        case HardwareMode::saturn:
            SaturnDigitalPad pad;
            pad.direction_left        = GLFW_KEY_LEFT;
            pad.direction_right       = GLFW_KEY_RIGHT;
            pad.direction_up          = GLFW_KEY_UP;
            pad.direction_down        = GLFW_KEY_DOWN;
            pad.button_shoulder_left  = GLFW_KEY_Z;
            pad.button_shoulder_right = GLFW_KEY_E;
            pad.button_a              = GLFW_KEY_S;
            pad.button_b              = GLFW_KEY_D;
            pad.button_c              = GLFW_KEY_F;
            pad.button_x              = GLFW_KEY_X;
            pad.button_y              = GLFW_KEY_C;
            pad.button_z              = GLFW_KEY_V;
            pad.button_start          = GLFW_KEY_ENTER;
            return pad.toConfig();
        case HardwareMode::stv:
            StvBoardControls board;
            StvPlayerControls player;
            break;
    }

    return std::vector<u16>();
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

libcfg::Setting& Config::readValue(const Access_keys& value) {
    try {
        return cfg_.lookup(Config::full_keys[value]);
    }
    catch (const libcfg::SettingNotFoundException& e) {
            
        auto errorString = fmt::format(tr("Setting '{0}' not found !"), e.getPath());
        Log::error("config", errorString);
        throw std::runtime_error("Config error !");
    }
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