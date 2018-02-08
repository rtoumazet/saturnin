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

#include <iostream>
#include <boost/filesystem.hpp>
#include "log.h"

using namespace std;

namespace saturnin {
namespace core {
    std::map<std::string, std::shared_ptr<spdlog::logger>> Log::loggers_;

    /* static */
    bool Log::initialize() {
        string pattern = "[%X][%n][%l] %v";
        spdlog::set_pattern(pattern);
        auto sink = createSink("logs/saturnin.log");
        createLogger("vdp1", sink);
        createLogger("vdp2", sink);

        return true;
    }

    /* static */
    std::shared_ptr<spdlog::sinks::simple_file_sink_mt> Log::createSink(const std::string & logger_path)
    {
        createFile(logger_path);

        return std::make_shared<spdlog::sinks::simple_file_sink_mt>(logger_path);
    }

    /* static */
    void Log::createLogger(const std::string & logger_name, const std::shared_ptr<spdlog::sinks::simple_file_sink_mt>& sink)
    {
        auto logger = std::make_shared<spdlog::logger>(logger_name, sink);
        loggers_[logger_name] = logger;
        spdlog::register_logger(logger);
    }

    /* static */
    void Log::createFile(const std::string& path) {
        auto full_path = boost::filesystem::current_path() / path;
        full_path.make_preferred();
        boost::filesystem::create_directories(full_path.parent_path());
        boost::filesystem::remove(full_path);
    }
}
}