// 
// sh2.cpp
// Saturnin
//
// Copyright (c) 2018-2019 Renaud Toumazet
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

#include "sh2.h"

namespace saturnin {
namespace core {

u32 Sh2::read32(u32 addr) const {
    switch (addr & 0x1FF) {
        case dvdnt:
        case dvdntl_shadow:
            return rawRead<u32>(this->io_registers_, dvdntl & 0x1FF);
            break;
        case dvdnth_shadow:
            return rawRead<u32>(this->io_registers_, dvdnth & 0x1FF);
            break;
        default:
            return rawRead<u32>(this->io_registers_, addr & 0x1FF);
    }
}
}
}