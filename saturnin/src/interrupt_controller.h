// 
// interrupt_controller.h
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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file	interrupt_controller.h
///
/// \brief	Declares everything related to interrupt management. 
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "emulator_defs.h"
#include "scu_registers.h"

namespace saturnin {
namespace core {

// Forward declarations
class Emulator_context;
    
struct Interrupt {
    u8                        vector;
    u8                        level;
    BitRange<InterruptMask>   mask;
    BitRange<InterruptEnable> status;

    operator int() const { return vector; } ///< Internal conversion operator.
};

class Interrupt_controller {
public:
    //@{
    // Constructors / Destructors
    Interrupt_controller() = delete;
    Interrupt_controller(Emulator_context* ec) : emulator_context_(ec) {};
    Interrupt_controller(const Interrupt_controller&) = delete;
    Interrupt_controller(Interrupt_controller&&) = delete;
    Interrupt_controller& operator=(const Interrupt_controller&) & = delete;
    Interrupt_controller& operator=(Interrupt_controller&&) & = delete;
    ~Interrupt_controller() = default;
    //@}

    /// \name Context objects accessors
    //@{
    Emulator_context* emulatorContext() const;
    //@}

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn void Interrupt_controller::run();
    ///
    /// \brief  Runs the interrupt controller.
    ///
    /// \author Runik
    /// \date   22/11/2019
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void run();

private:
    Emulator_context* emulator_context_; ///< Pointer to the emulator context object.
};

}
}