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

#include <cstdint>

namespace saturnin {
namespace core {
    enum class Rom_stv : std::uint8_t {
        astra_superstars = 1,			///< Astra Superstars (IC13).
        baku_baku_animals= 2,			///< Baku Baku Animal (IC13)
        columns_97       = 3			///< Columns 97 (IC13)
        //const boost::uint8_t COTTON2 = 4;			///< Cotton 2
        //const boost::uint8_t COTTONBM = 5;			///< Cotton Boomerang
        //const boost::uint8_t DECATHLT = 6;			///< Decathlete (IC13)
        //const boost::uint8_t DNMTDEKA = 7;			///< Dynamite Deka (IC13)
        //const boost::uint8_t EJIHON = 8;			///< Ejihon Tantei Jimusyo (IC13)
        //const boost::uint8_t ELANDORE = 9;			///< Fighting Dragon Legend Elan Doree
        //const boost::uint8_t FFREVENG = 10;			///< Final Fight Revenge
        //const boost::uint8_t FHBOXERS = 11;			///< Funky Head Boxers (IC13)
        //const boost::uint8_t FINDLOVE = 12;			///< Find Love (IC13) - Must be set to 1P to boot
        //const boost::uint8_t FINLARCH = 13;			///< Final Arch  (IC13)
        //const boost::uint8_t GAXEDUEL = 14;			///< Golden Axe - The Duel (IC13)
        //const boost::uint8_t GRDFORCE = 15;			///< Guardian Force
        //const boost::uint8_t GROOVEF = 16;			///< Power Instinct 3 - Groove On Fight
        //const boost::uint8_t INTRODON = 17;			///< Karaoke Quiz Intro Don Don! (IC13)
        //const boost::uint8_t KIWAMES = 18;			///< Pro Mahjong Kiwame S (IC13) - Must be set to 1P to boot
        //const boost::uint8_t MARUCHAN = 19;			///< Maru-Chan de Goo! (IC13)
        //const boost::uint8_t MYFAIRLD = 20;			///< Virtual Mahjong 2 - My Fair Lady - Must be set to 1P to boot
        //const boost::uint8_t PBLBEACH = 21; 		///< Pebble Beach - The Great Shot (IC13)
        //const boost::uint8_t PRIKURA = 22;			///< Purikura Daisakusen
        //const boost::uint8_t PUYOSUN = 23; 			///< Puyo Puyo Sun (IC13)
        //const boost::uint8_t SANDOR = 24;			///< Sando-R (IC13)
        //const boost::uint8_t SASSISU = 25;			///< Taisen Tanto-R 'Sasshissu! (IC13)
        //const boost::uint8_t SHIENRYU = 26;			///< Shienryu
        //const boost::uint8_t SOKYUGRT = 27;			///< Soukyugurentai / Terra Diver (IC13)
        //const boost::uint8_t SSS = 28; 				///< Steep Slope Sliders (IC13)
        //const boost::uint8_t VFKIDS = 29;			///< Virtua Fighter Kids (IC13)
        //const boost::uint8_t VFREMIX = 30; 			///< Virtua Fighter Remix (IC13)
        //const boost::uint8_t VMAHJONG = 31;			///< Virtual Mahjong - Must be set to 1P to boot
        //const boost::uint8_t WINTERHT = 32;			///< Winter Heat (IC13)
        //const boost::uint8_t ZNPWFV = 33;			///< Zen Nippon Pro-Wrestling Featuring Virtua (IC13)
        //const boost::uint8_t HANAGUMI = 34;			///< Hanagumi Taisen Columns - Sakura Wars
        //const boost::uint8_t RSGUN = 35;			///< Radiant Silvergun
        //const boost::uint8_t DANCHIH = 36; 			///< Danchi de Hanafuda
        //const boost::uint8_t OTHELLOS = 37;			///< Othello Shiyouyo
        //const boost::uint8_t SHANHIGW = 38;			///< Shanghai - The Great Wall
        //const boost::uint8_t SMLEAGUE = 39; 		///< Super Major League (IC13)
        //const boost::uint8_t TWCUP98 = 40; 			///< Tecmo World Cup '98 (IC13)
        //const boost::uint8_t SUIKOENB = 41;			///< Suikoenbu (IC13)
        //const boost::uint8_t SFISH2 = 42;			///< Sports Fishing 2 (IC13)
        //const boost::uint8_t BATMANFR = 43;			///< Batman Forever Revenge (extra sound board)
        //const boost::uint8_t DIEHARD = 44;			///< Die Hard Arcade
        //const boost::uint8_t CRITCRSH = 45;			///< Critter Crusher
        //const boost::uint8_t DANCHIQ = 46;			///< Danchi de Quiz Okusan Yontaku Desuyo!
        //const boost::uint8_t SANJEON = 47;			///< DaeJeon! SanJeon SuJeon
        //const boost::uint8_t SEABASS = 48;			///< Sea Bass Fishing
        //const boost::uint8_t THUNT = 49;			///< Puzzle & Action: Treasure Hunt
        //const boost::uint8_t THUNTK = 50;			///< Puzzle & Action: BoMulEul Chajara
        //const boost::uint8_t STRESS = 51;			///< Stress Busters
        //const boost::uint8_t NCLUBV3 = 52;			///< Name Club Ver.3
        //const boost::uint8_t PCLUB2 = 53;			///< Print Club 2
        //const boost::uint8_t PCLUB2V3 = 54;			///< Print Club 2 Vol. 3
        //const boost::uint8_t PCLUBPOK = 55;			///< Print Club Pokemon B
        //const boost::uint8_t DECATHLTO = 56;		///< Decathlete
        //const boost::uint8_t MAGZUN = 57;			///< Magical Zunou Power
        //const boost::uint8_t TECHBOWL = 58;			///< Technical Bowling
        //const boost::uint8_t MICROMBC = 59;			///< Microman Battle Charge
        //const boost::uint8_t SFISH2J = 60;			///< Sport Fishing 2 (J)
        //const boost::uint8_t MAUSUKE = 61;			///< Mausuke no Ojama the World
    };

}
}