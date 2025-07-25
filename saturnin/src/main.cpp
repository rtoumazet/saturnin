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
#include <saturnin/src/exceptions.h>  // MainError
#include <saturnin/src/locale.h>      // tr
#include <saturnin/src/log.h>         // Log
#include <saturnin/src/thread_pool.h> // TreadPool

namespace core      = saturnin::core;
namespace exception = saturnin::exception;

using core::EmulatorContext;
using core::Log;
using core::Logger;
using core::ThreadPool;
using core::tr;

auto main(int argc, char* argv[]) -> int {
    try {
        auto state = EmulatorContext{};
        Log::initialize();
        ThreadPool::initialize();
        while (state.renderingStatus() != core::RenderingStatus::stopped) {
            if (state.renderingStatus() == core::RenderingStatus::reset) { state.reset(); }
            if (!state.initialize(argc, argv)) { Log::exception(Logger::main, tr("Could not initialize the program ...")); }
            state.startInterface();
            state.stopEmulation();
        }
        Log::shutdown();
        ThreadPool::shutdown();
        std::exit(EXIT_SUCCESS);
    } catch (const std::runtime_error& e) {
        Log::error(Logger::generic, e.what());
        std::exit(EXIT_FAILURE);
    } catch (const std::exception& e) {
        Log::error(Logger::generic, e.what());
        std::exit(EXIT_FAILURE);
    } catch (...) {
        Log::error(Logger::generic, tr("Uncaught exception !"));
        std::exit(EXIT_FAILURE);
    }
};
