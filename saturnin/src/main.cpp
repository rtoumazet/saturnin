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

#include <saturnin/src/pch.h>
#include <saturnin/src/emulator_context.h> // EmulatorContext
#include <istream>
#include <saturnin/src/locale.h> // tr
#include <saturnin/src/log.h>    // Log

namespace core = saturnin::core;

using core::EmulatorContext;
using core::Log;
using core::Logger;
using core::tr;

auto main(int argc, char* argv[]) -> int {
    try {
        auto state = EmulatorContext{};
        Log::initialize();
        while (state.renderingStatus() != core::RenderingStatus::stopped) {
            if (!state.initialize()) {
                Log::error(Logger::main, tr("Could not initialize the program ..."));
                throw std::runtime_error("Main error !");
            }
            state.startInterface();
            state.stopEmulation();
        }
        Log::shutdown();
        std::exit(EXIT_SUCCESS);
    } catch (const std::runtime_error& e) { Log::error(Logger::exception, e.what()); } catch (const std::exception& e) {
        Log::error(Logger::exception, e.what());
    } catch (...) { Log::error(Logger::exception, tr("Uncaught exception !")); }
};
