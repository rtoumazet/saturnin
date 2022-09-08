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

#include <saturnin/src/pch.h>
#include <saturnin/src/log.h>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <saturnin/src/emulator_defs.h>

namespace fs = std::filesystem;

namespace saturnin::core {

constexpr auto max_messages = u16{500};

MapLogger                                          Log::loggers_;
MapLoggerName                                      Log::loggers_names_;
std::shared_ptr<spdlog::sinks::ringbuffer_sink_mt> Log::ringbuffer_sink_;

// static //
auto Log::initialize() -> bool {
    createConsole();
    auto file_sink   = createFileSink("logs/saturnin.log");
    ringbuffer_sink_ = createRingbufferSink();

    spdlog::sinks_init_list sink_list = {file_sink, ringbuffer_sink_};

    const auto loggers_names = std::vector<std::string>{"cdrom",
                                                        "config",
                                                        "main",
                                                        "memory",
                                                        "sh2",
                                                        "scu",
                                                        "vdp1",
                                                        "vdp2",
                                                        "opengl",
                                                        "exception",
                                                        "smpc",
                                                        "scsp",
                                                        "texture",
                                                        "unimplemented"};
    for (auto& n : loggers_names) {
        createLogger(n, sink_list);
    }
    spdlog::flush_every(std::chrono::seconds(3));

    loggers_names_ = {{Logger::cdrom, "cdrom"},
                      {Logger::config, "config"},
                      {Logger::main, "main"},
                      {Logger::memory, "memory"},
                      {Logger::sh2, "sh2"},
                      {Logger::scu, "scu"},
                      {Logger::vdp1, "vdp1"},
                      {Logger::vdp2, "vdp2"},
                      {Logger::opengl, "opengl"},
                      {Logger::exception, "exception"},
                      {Logger::smpc, "smpc"},
                      {Logger::scsp, "scsp"},
                      {Logger::texture, "texture"},
                      {Logger::unimplemented, "unimplemented"}};

    return true;
}

// static //
void Log::shutdown() {
    // Adding a delay to allow the thread to finish cleanly.
    using namespace std::this_thread;     // sleep_for
    using namespace std::chrono_literals; // ms
    const auto time_to_sleep = 2000ms;
    sleep_for(time_to_sleep);

    spdlog::shutdown();
}

// static //
auto Log::createFileSink(const std::string& logger_path) -> std::shared_ptr<spdlog::sinks::basic_file_sink_mt> {
    removeFile(logger_path);

    return std::make_shared<spdlog::sinks::basic_file_sink_mt>(logger_path);
}

// static //
auto Log::createConsoleSink() -> std::shared_ptr<spdlog::sinks::wincolor_stdout_sink_mt> {
    return std::make_shared<spdlog::sinks::wincolor_stdout_sink_mt>();
}

// static //
auto Log::createRingbufferSink() -> std::shared_ptr<spdlog::sinks::ringbuffer_sink_mt> {
    return std::make_shared<spdlog::sinks::ringbuffer_sink_mt>(max_messages);
}

// static //
void Log::createLogger(const std::string& logger_name, const spdlog::sinks_init_list& sinks_list) {
    auto       logger  = std::make_shared<spdlog::logger>(logger_name, sinks_list.begin(), sinks_list.end());
    const auto pattern = std::string{"[%X][%n][%l] %v"};
    logger->set_pattern(pattern);
    loggers_[logger_name] = logger;
    spdlog::register_logger(logger);
}

// static //
void Log::createConsole() {
    auto       console = spdlog::stdout_color_mt("console");
    const auto pattern = std::string{"[%X][%l] %v"};
    console->set_pattern(pattern);
    loggers_["console"] = console;
    // no need to register the console as it already exists
}

// static //
auto Log::getRingbuffer() -> std::string {
    const auto& msg_buffer = ringbuffer_sink_->last_formatted();
    auto        oss        = std::ostringstream{};
    std::copy(msg_buffer.begin(), msg_buffer.end(), std::ostream_iterator<std::string>(oss));
    return oss.str();
}

// static //
void Log::removeFile(const std::string& path) {
    auto full_path = fs::current_path() / path;
    full_path.make_preferred();
    fs::create_directories(full_path.parent_path());
    try {
        fs::remove(full_path);
    } catch (const fs::filesystem_error& e) { Log::error(Logger::exception, e.what()); }
}

// static //
void Log::flush() {
    for (auto const& l : loggers_) {
        l.second->flush();
    }
}

// static //
void Log::dumpBacktraceToConsole() { core::Log::loggers_.at("console")->dump_backtrace(); }

// static
void Log::setLogLevel(const std::string& logger, const LogLevel level) {
    switch (level) {
        using enum LogLevel;
        case off: {
            loggers_.at(logger)->set_level(spdlog::level::level_enum::off);
            break;
        }
        case debug: {
            loggers_.at(logger)->set_level(spdlog::level::level_enum::debug);
            break;
        }
        default: loggers_.at(logger)->set_level(spdlog::level::level_enum::info);
    }
}

} // namespace saturnin::core
