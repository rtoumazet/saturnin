// 
// emulator_context.cpp
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

#include "emulator_context.h"

namespace saturnin {
namespace core {

    Emulator_context::Emulator_context() {
        //Config config("saturnin.cfg");

        //std::unique_ptr<Memory> memory{ std::make_unique<Memory>(config) };
        memory_ = std::make_unique<Memory>(new Memory);

        Hardware_mode    hardware_mode{ Hardware_mode::saturn };     ///< Hardware mode
        Emulation_status emulation_status{ Emulation_status::stopped }; ///< Emulation status

                                                                        /// \name Command line variables
                                                                        ///
                                                                        //@{
        std::string command_line{ "" };         ///< Command line content
        bool        autoload_binary{ false }; ///< True when the binary has to be automatically loaded.
        uint32_t    binary_address{ 0 };       ///< The PC will be set to this address after loading the binary.
                                               //@}

        uint8_t stv_rom; ///< Current ST-V ROM loaded
    }

}
}