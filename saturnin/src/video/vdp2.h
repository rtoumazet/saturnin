// 
// vdp2.h
// Saturnin
//
// Copyright (c) 2020 Renaud Toumazet
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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file	vdp1.h
///
/// \brief	Declares the Vdp2 class and related functions. 
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace saturnin {
namespace video {

// Saturn video resolution
//  Horizontal resolution : 320 or 352 dots (PAL or NTSC)
//  Vertical resolution :
//  - PAL   
//     - 312.5 lines non interlaced (50Hz / frame), 625 lines interlaced  (25Hz / frame)  
//     - 288 active lines (non interlaced), 576 active lines (interlaced)  
//     - 24.5 blanking lines (non interlaced), 49 blanking lines (interlaced)
//  - NTSC :
//     - 262.5 lines non interlaced (60Hz / frame), 525 lines interlaced (30Hz / frame)  
//     - 240 active lines (non interlaced), 480 active lines (interlaced)  
//     - 22.5 blanking lines (non interlaced), 45 blanking lines (interlaced)


////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   TvStandard
///
/// \brief  Tv standard used.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class TvStandard : s8 {
    unknown = -1,   ///< Unknown.
    pal  = 0,       ///< PAL.
    ntsc = 1        ///< NTSC.
};

}
}