//
// log.cpp
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

#include <filesystem>
#include <iostream>
#include "log.h"

namespace fs = std::filesystem;

namespace saturnin::core {
std::map<std::string, std::shared_ptr<spdlog::logger>> Log::loggers_;

/* static */
auto Log::initialize() -> bool {
    createConsole();
    auto sink = createFileSink("logs/saturnin.log");
    createLogger("cdrom", sink);
    createLogger("config", sink);
    createLogger("main", sink);
    createLogger("memory", sink);
    createLogger("sh2", sink);
    createLogger("scu", sink);
    createLogger("vdp1", sink);
    createLogger("vdp2", sink);
    createLogger("opengl", sink);
    createLogger("exception", sink);
    createLogger("smpc", sink);

    // :TODO: replace by spdlog::enable_backtrace() when version 1.4.X is ported to vcpkg
    spdlog::flush_every(std::chrono::seconds(3));

    auto log_file = fs::current_path() / "logs" / "saturnin.log";

    return true;
}

/* static */
auto Log::createFileSink(const std::string& logger_path) -> std::shared_ptr<spdlog::sinks::basic_file_sink_mt> {
    removeFile(logger_path);

    return std::make_shared<spdlog::sinks::basic_file_sink_mt>(logger_path);
}

/* static */
auto Log::createConsoleSink() -> std::shared_ptr<spdlog::sinks::wincolor_stdout_sink_mt> {
    return std::make_shared<spdlog::sinks::wincolor_stdout_sink_mt>();
}

/* static */
void Log::createLogger(const std::string& logger_name, const std::shared_ptr<spdlog::sinks::basic_file_sink_mt>& sink) {
    auto        logger  = std::make_shared<spdlog::logger>(logger_name, sink);
    std::string pattern = "[%X][%n][%l] %v";
    logger->set_pattern(pattern);
    loggers_[logger_name] = logger;
    spdlog::register_logger(logger);
}

/* static */
void Log::createConsole() {
    auto        console = spdlog::stdout_color_mt("console");
    std::string pattern = "[%X][%l] %v";
    console->set_pattern(pattern);
    loggers_["console"] = console;
    // no need to register the console as it already exists
}
/* static */
void Log::removeFile(const std::string& path) {
    auto full_path = fs::current_path() / path;
    full_path.make_preferred();
    fs::create_directories(full_path.parent_path());
    fs::remove(full_path);
}

/* static */
void Log::flush() {
    for (auto const& l : loggers_) {
        l.second->flush();
    }
}
} // namespace saturnin::core
