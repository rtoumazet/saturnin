//
// interrupt_controller.h
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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file	interrupt_controller.h
///
/// \brief	Declares everything related to interrupt management.
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string> // string
#include <saturnin/src/emulator_defs.h>
#include <saturnin/src/scu_registers.h> // InterruptMask, InterruptEnable

namespace saturnin::core {

struct Interrupt {
    u8                        vector;
    u8                        level;
    BitRange<InterruptMask>   mask;
    BitRange<InterruptEnable> status;
    std::string               name;

    operator int() const { return vector; } ///< Internal conversion operator.
};

} // namespace saturnin::core
