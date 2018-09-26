// 
// main.cpp
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

#include "log.h"
#include "emulator_context.h"

namespace core  = saturnin::core;

int main(int argc, char *argv[]) {
    core::Emulator_context state;
    if (!state.initialize()) {
        core::Log::error("main", core::tr("Could not initialize the program ..."));
        std::exit(EXIT_FAILURE);
    }

    if (!state.run()) {
        core::Log::error("main", core::tr("An error occured during program execution ..."));
        std::exit(EXIT_FAILURE);
    }
};