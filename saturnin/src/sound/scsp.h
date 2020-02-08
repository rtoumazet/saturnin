//
// scsp.h
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
/// \file	scsp.h
///
/// \brief	Declares the SCSP class.
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../log.h"

// Forward declarations
namespace saturnin::core {
class EmulatorContext;
}

namespace saturnin::sound {

using saturnin::core::EmulatorContext;
using saturnin::core::Log;

class Scsp {
  public:
    //@{
    // Constructors / Destructors
    Scsp() = delete;
    Scsp(EmulatorContext* ec){};
    Scsp(const Scsp&) = delete;
    Scsp(Scsp&&)      = delete;
    Scsp& operator=(const Scsp&) & = delete;
    Scsp& operator=(Scsp&&) & = delete;
    ~Scsp()                   = default;
    //@}

    void reset() { Log::error("scsp", "reset() not implemented"); };
    void setSound(bool) { Log::error("scsp", "setSound() not implemented"); };
};

} // namespace saturnin::sound
