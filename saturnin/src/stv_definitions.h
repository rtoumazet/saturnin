//
// stv_definitions.h
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
/// \file	stv_definitions.h
///
/// \brief	ST-V roms definitions.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <array>
#include <cstdint> // uint8_t
#include <string>  // string

namespace saturnin::core {

enum class Rom_stv : std::uint8_t {
    none                     = 0,  ///< None
    astra_superstars         = 1,  ///< Astra Superstars (IC13).
    baku_baku_animals        = 2,  ///< Baku Baku Animal (IC13)
    columns_97               = 3,  ///< Columns 97 (IC13)
    cotton_2                 = 4,  ///< Cotton 2
    cotton_boomerang         = 5,  ///< Cotton Boomerang
    decathlete               = 6,  ///< Decathlete (IC13)
    dynamite_deka            = 7,  ///< Dynamite Deka (IC13)
    ejihon_tantei_jimusyo    = 8,  ///< Ejihon Tantei Jimusyo (IC13)
    elan_doree               = 9,  ///< Fighting Dragon Legend Elan Doree
    final_fight_revenge      = 10, ///< Final Fight Revenge
    funky_heads_boxers       = 11, ///< Funky Head Boxers (IC13)
    find_love                = 12, ///< Find Love (IC13) - Must be set to 1P to boot
    final_arch               = 13, ///< Final Arch  (IC13)
    golden_axe_the_duel      = 14, ///< Golden Axe - The Duel (IC13)
    guardian_force           = 15, ///< Guardian Force
    power_instinct_3         = 16, ///< Power Instinct 3 - Groove On Fight
    intro_don_don            = 17, ///< Karaoke Quiz Intro Don Don! (IC13)
    kiwame_s                 = 18, ///< Pro Mahjong Kiwame S (IC13) - Must be set to 1P to boot
    maru_chan_de_goo         = 19, ///< Maru-Chan de Goo! (IC13)
    virtual_mahjong_2        = 20, ///< Virtual Mahjong 2 - My Fair Lady - Must be set to 1P to boot
    peeble_beach             = 21, ///< Pebble Beach - The Great Shot (IC13)
    purikura_daisakusen      = 22, ///< Purikura Daisakusen
    puyo_puyo_sun            = 23, ///< Puyo Puyo Sun (IC13)
    sando_r                  = 24, ///< Sando-R (IC13)
    taisen_tanto_r           = 25, ///< Taisen Tanto-R 'Sasshissu! (IC13)
    shienryu                 = 26, ///< Shienryu
    soukyugurentai           = 27, ///< Soukyugurentai / Terra Diver (IC13)
    steep_slope_sliders      = 28, ///< Steep Slope Sliders (IC13)
    virtua_fighter_kids      = 29, ///< Virtua Fighter Kids (IC13)
    virtua_fighter_remix     = 30, ///< Virtua Fighter Remix (IC13)
    virtual_mahjong          = 31, ///< Virtual Mahjong - Must be set to 1P to boot
    winter_heat              = 32, ///< Winter Heat (IC13)
    zen_nippon_pro_wrestling = 33, ///< Zen Nippon Pro-Wrestling Featuring Virtua (IC13)
    hanagumi_taisen_columns  = 34, ///< Hanagumi Taisen Columns - Sakura Wars
    radiant_silvergun        = 35, ///< Radiant Silvergun
    danchi_de_hanafuda       = 36, ///< Danchi de Hanafuda
    othello_shiyouyo         = 37, ///< Othello Shiyouyo
    shanghai                 = 38, ///< Shanghai - The Great Wall
    super_major_league       = 39, ///< Super Major League (IC13)
    tecmo_world_cup_98       = 40, ///< Tecmo World Cup '98 (IC13)
    suikoenbu                = 41, ///< Suikoenbu (IC13)
    sport_fishing_2          = 42, ///< Sport Fishing 2 (IC13)
    batman_forever           = 43, ///< Batman Forever (extra sound board)
    die_hard_arcade          = 44, ///< Die Hard Arcade
    critter_crusher          = 45, ///< Critter Crusher
    danshi_de_quiz           = 46, ///< Danchi de Quiz Okusan Yontaku Desuyo!
    daejeon_sanjeon_sujeon   = 47, ///< DaeJeon! SanJeon SuJeon
    sea_bass_fishing         = 48, ///< Sea Bass Fishing
    treasure_hunt            = 49, ///< Puzzle & Action: Treasure Hunt
    bomuleul_chajara         = 50, ///< Puzzle & Action: BoMulEul Chajara
    stress_busters           = 51, ///< Stress Busters
    name_club_3              = 52, ///< Name Club Ver.3
    print_club_2             = 53, ///< Print Club 2
    print_club_2_vol_3       = 54, ///< Print Club 2 Vol. 3
    print_club_pokemon_b     = 55, ///< Print Club Pokemon B
    decathlete_alt           = 56, ///< Decathlete alternate version
    magical_zunou_power      = 57, ///< Magical Zunou Power
    technical_bowling        = 58, ///< Technical Bowling
    microman_battle_charge   = 59, ///< Microman Battle Charge
    sport_fishing_2_j        = 60, ///< Sport Fishing 2 (J)
    mausuke_no_ojama         = 61  ///< Mausuke no Ojama the World
};

using Stv_rom_data = struct { ///< ST-V ROM data structure
    std::string name;         ///< rom name
    Rom_stv     id;           ///< identifiant
    std::string zip;          ///< zip name
    std::string parent;       ///< zip parent (if any)
    std::string version;      ///< version
};

/// ST-V roms list array
// clang-format off
constexpr auto stv_rom_list_size = std::uint8_t{ 61 };
static const auto stv_rom_list = std::array<Stv_rom_data, stv_rom_list_size>
{ {
    { "Astra Superstars",                          Rom_stv::astra_superstars,        "astrass"  ,""         ,"J         \n19980514  \nV1.002" },
    { "Baku Baku Animal",                          Rom_stv::baku_baku_animals,       "bakubaku" ,""         ,"J         \n19950407  \nV1.000" },
    { "Columns 97",                                Rom_stv::columns_97,              "colmns97" ,""         ,"JET       \n19961209  \nV1.000" },
    { "Cotton 2",                                  Rom_stv::cotton_2,                "cotton2"  ,""         ,"JUET      \n19970902  \nV1.000" },
    { "Cotton Boomerang",                          Rom_stv::cotton_boomerang,        "cottonbm" ,""         ,"JUET      \n19980709  \nV1.000" },
    { "Decathlete",                                Rom_stv::decathlete,              "decathlt" ,""         ,"JUET      \n19960709  \nV1.001" },
    { "Dynamite Deka",                             Rom_stv::dynamite_deka,           "dnmtdeka" ,"diehard"  ,"J         \n19960515  \nV1.000" },
    { "Ejihon Tantei Jimusyo",                     Rom_stv::ejihon_tantei_jimusyo,   "ejihon"   ,""         ,"J         \n19950613  \nV1.000" },
    { "Fighting Dragon Legend Elan Doree",         Rom_stv::elan_doree,              "elandore" ,""         ,"JUET      \n19980922  \nV1.006" },
    { "Final Fight Revenge",                       Rom_stv::final_fight_revenge,     "ffreveng" ,""         ,"JUET      \n19990714  \nV1.000" },
    { "Funky Head Boxers",                         Rom_stv::funky_heads_boxers,      "fhboxers" ,""         ,"JUETBKAL  \n19951218  \nV1.000" },
    { "Find Love",                                 Rom_stv::find_love,               "findlove" ,""         ,"J         \n19971212  \nV1.000" },
    { "Final Arch",                                Rom_stv::final_arch,              "finlarch" ,""         ,"J         \n19950714  \nV1.001" },
    { "Golden Axe - The Duel",                     Rom_stv::golden_axe_the_duel,     "gaxeduel" ,""         ,"JUETL     \n19950117  \nV1.000" },
    { "Guardian Force",                            Rom_stv::guardian_force,          "grdforce" ,""         ,"JUET      \n19980318  \nV0.105" },
    { "Power Instinct 3 - Groove On Fight",        Rom_stv::power_instinct_3,        "groovef"  ,""         ,"J         \n19970416  \nV1.001" },
    { "Karaoke Quiz Intro Don Don!",               Rom_stv::intro_don_don,           "introdon" ,""         ,"J         \n19960213  \nV1.000" },
    { "Pro Mahjong Kiwame S",                      Rom_stv::kiwame_s,                "kiwames"  ,""         ,"J         \n19951020  \nV1.208" },
    { "Maru-Chan de Goo!",                         Rom_stv::maru_chan_de_goo,        "maruchan" ,""         ,"J         \n19971216  \nV1.000" },
    { "Virtual Mahjong 2 - My Fair Lady",          Rom_stv::virtual_mahjong_2,       "myfairld" ,""         ,"J         \n19980608  \nV1.000" },
    { "Pebble Beach - The Great Shot",             Rom_stv::peeble_beach,            "pblbeach" ,""         ,"JUE       \n19950913  \nV0.990" },
    { "Princess Clara Daisakusen",                 Rom_stv::purikura_daisakusen,     "prikura"  ,""         ,"J         \n19960910  \nV1.000" },
    { "Puyo Puyo Sun",                             Rom_stv::puyo_puyo_sun,           "puyosun"  ,""         ,"J         \n19961115  \nV0.001" },
    { "Puzzle & Action: Sando-R",                  Rom_stv::sando_r,                 "sandor"   ,""         ,"J         \n19951114  \nV1.000" },
    { "Taisen Tanto-R Sashissu!!",                 Rom_stv::taisen_tanto_r,          "sassisu"  ,""         ,"J         \n19980216  \nV1.000" },
    { "Shienryu",                                  Rom_stv::shienryu,                "shienryu" ,""         ,"JUET      \n19961226  \nV1.000" },
    { "Soukyugurentai / Terra Diver",              Rom_stv::soukyugurentai,          "sokyugrt" ,""         ,"JUET      \n19960821  \nV1.000" },
    { "Steep Slope Sliders",                       Rom_stv::steep_slope_sliders,     "sss"      ,""         ,"JUET      \n19981110  \nV1.000" },
    { "Virtua Fighter Kids",                       Rom_stv::virtua_fighter_kids,     "vfkids"   ,""         ,"JUET      \n19960319  \nV0.000" },
    { "Virtua Fighter Remix",                      Rom_stv::virtua_fighter_remix,    "vfremix"  ,""         ,"JUETBKAL  \n19950428  \nV1.000" },
    { "Virtual Mahjong",                           Rom_stv::virtual_mahjong,         "vmahjong" ,""         ,"J         \n19961214  \nV1.000" },
    { "Winter Heat",                               Rom_stv::winter_heat,             "winterht" ,""         ,"JUET      \n19971012  \nV1.000" },
    { "Zen Nippon Pro-Wrestling Featuring Virtua", Rom_stv::zen_nippon_pro_wrestling,"znpwfv"   ,""         ,"J         \n19971123  \nV1.000" },
    { "Hanagumi Taisen Columns - Sakura Wars",     Rom_stv::hanagumi_taisen_columns, "hanagumi" ,""         ,"J         \n19971007  \nV1.010" },
    { "Radiant Silvergun",                         Rom_stv::radiant_silvergun,       "rsgun"    ,""         ,"JUET      \n19980523  \nV1.000" },
    { "Danchi de Hanafuda",                        Rom_stv::danchi_de_hanafuda,      "danchih"  ,""         ,"J         \n19990607  \nV1.400" },
    { "Othello Shiyouyo",                          Rom_stv::othello_shiyouyo,        "othellos" ,""         ,"J         \n19980423  \nV1.002" },
    { "Shanghai - The Great Wall",                 Rom_stv::shanghai,                "shanhigw" ,""         ,"JUE       \n19950623  \nV1.005" },
    { "Super Major League",                        Rom_stv::super_major_league,      "smleague" ,""         ,"U         \n19960108  \nV1.000" },
    { "Tecmo World Cup '98",                       Rom_stv::tecmo_world_cup_98,      "twcup98"  ,""         ,"JUET      \n19980410  \nV1.000" },
    { "Suikoenbu / Outlaws of the Lost Dynasty",   Rom_stv::suikoenbu,               "suikoenb" ,""         ,"JUETL     \n19950314  \nV2.001" },
    { "Sport Fishing 2",                           Rom_stv::sport_fishing_2,         "sfish2"   ,""         ,"UET       \n19951106  \nV1.10e" },
    { "Batman Forever",                            Rom_stv::batman_forever,          "batmanfr" ,""         ,"JUE       \n19960507  \nV1.000" },
    { "Die Hard Arcade",                           Rom_stv::die_hard_arcade,         "diehard"  ,""         ,"UET       \n19960515  \nV1.000" },
    { "Critter Crusher",                           Rom_stv::critter_crusher,         "critcrsh" ,""         ,"EA        \n19951204  \nV1.000" },
    { "Danchi de Quiz Okusan Yontaku Desuyo!",     Rom_stv::danshi_de_quiz,          "danchiq"  ,"danchih"  ,"J         \n20001128  \nV1.200" },
    { "DaeJeon! SanJeon SuJeon",                   Rom_stv::daejeon_sanjeon_sujeon,  "sanjeon"  ,""         ,"AJTUE     \n19990412  \nV1.000" },
    { "Sea Bass Fishing",                          Rom_stv::sea_bass_fishing,        "seabass"  ,""         ,"JUET      \n19971110  \nV0.001" },
    { "Puzzle & Action: Treasure Hunt",            Rom_stv::treasure_hunt,           "thunt"    ,""         ,"JUET      \n19970901  \nV2.00E" },
    { "Puzzle & Action: BoMulEul Chajara",         Rom_stv::bomuleul_chajara,        "thuntk"   ,""         ,"JUET      \n19970125  \nV2.00K" },
    { "Stress Busters",                            Rom_stv::stress_busters,          "stress"   ,""         ,"J         \n19981020  \nV1.000" },
    { "Name Club Ver.3",                           Rom_stv::name_club_3,             "nclubv3"  ,""         ,"J         \n19970723  \nV1.000" },
    { "Print Club 2",                              Rom_stv::print_club_2,            "pclub2"   ,""         ,"U         \n19970921  \nV1.000" },
    { "Print Club 2 Vol. 3",                       Rom_stv::print_club_2_vol_3,      "pclub2v3" ,""         ,"U         \n19990310  \nV1.000" },
    { "Print Club Pokemon B",                      Rom_stv::print_club_pokemon_b,    "pclubpok" ,""         ,"U         \n19991126  \nV1.000" },
    { "Decathlete",                                Rom_stv::decathlete,              "decathlto","decathlt" ,"JUET      \n19960424  \nV1.000" },
    { "Magical Zunou Power",                       Rom_stv::magical_zunou_power,     "magzun"   ,""         ,"J         \n19961031  \nV1.000" },
    { "Technical Bowling",                         Rom_stv::technical_bowling,       "techbowl" ,""         ,"J         \n19971212  \nV1.000" },
    { "Microman Battle Charge",                    Rom_stv::microman_battle_charge,  "micrombc" ,""         ,"J         \n19990326  \nV1.000" },
    { "Sport Fishing 2 (J)",                       Rom_stv::sport_fishing_2_j,       "sfish2j"  ,"sfish2"   ,"J         \n19951201  \nV1.100" },
    { "Mausuke no Ojama the World",                Rom_stv::mausuke_no_ojama,        "mausuke"  ,""         ,"J         \n19960314  \nV1.000" }
    // clang-format on
}};

} // namespace saturnin::core
