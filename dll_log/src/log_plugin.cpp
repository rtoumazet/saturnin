// 
// log_plugin.cpp
// dll_log
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
#include <boost/filesystem.hpp>
#include <spdlog/spdlog.h>
#include "log_plugin.h"

namespace saturnin {
namespace log {

    LogPlugin::LogPlugin() {
        std::cout << "Constructing my_plugin_log" << std::endl;
        pattern_ = "[%R] [%n] %v";
    }

    std::string LogPlugin::version() const {
        return "v1.00";
    }

    void LogPlugin::log(const std::string& value) const {
        auto console = spdlog::stdout_logger_mt("console");
        console->info(value);
    }

    //std::shared_ptr<spdlog::sinks::simple_file_sink_mt> LogPlugin::createSink(const std::string & logger_path)
    //{
    //    createOrEmptyDirectory(logger_path);

    //    return std::make_shared<spdlog::sinks::simple_file_sink_mt>(logger_path);
    //}

    //std::shared_ptr<spdlog::logger> LogPlugin::createLogger(const std::string& logger_name, const std::string& logger_path) {
    //    createOrEmptyDirectory(logger_path);
    //    auto full_path = boost::filesystem::current_path() / logger_path;
    //    auto logger = spdlog::basic_logger_mt(logger_name, full_path.string());
    //    logger.get()->set_pattern(pattern_);
    //    return logger;
    //}

    //std::shared_ptr<spdlog::logger> LogPlugin::createLogger(const std::string & logger_name, const std::shared_ptr<spdlog::sinks::simple_file_sink_mt>& sink)
    //{
    //    auto logger = std::make_shared<spdlog::logger>(logger_name, sink);
    //    logger.get()->set_pattern(pattern_);
    //    return logger;
    //}

    //std::shared_ptr<spdlog::logger> LogPlugin::createLogger(const std::string & logger_name, const std::vector<spdlog::sink_ptr>& sinks)
    //{
    //    auto logger = std::make_shared<spdlog::logger>(logger_name, begin(sinks), end(sinks));
    //    logger.get()->set_pattern(pattern_);
    //    return logger;
    //}

    //std::shared_ptr<spdlog::logger> LogPlugin::createLogger(const std::string & logger_name, const std::string & logger_path, const std::shared_ptr<spdlog::sinks::simple_file_sink_mt>& globalSink)
    //{

    //    createOrEmptyDirectory(logger_path);
    //    std::vector<spdlog::sink_ptr> sinks;
    //    sinks.push_back(globalSink);
    //    sinks.push_back(std::make_shared<spdlog::sinks::simple_file_sink_mt>(logger_path));

    //    auto logger = std::make_shared<spdlog::logger>(logger_name, begin(sinks), end(sinks));
    //    logger.get()->set_pattern(pattern_);
    //    return logger;
    //}

    void LogPlugin::createOrEmptyDirectory(const std::string& path) {
        auto full_path = boost::filesystem::current_path() / path;
        full_path.make_preferred();
        boost::filesystem::create_directories(full_path.parent_path());
        boost::filesystem::remove(full_path);
    }

    LogPlugin::~LogPlugin() {
        std::cout << "Destructing my_plugin_log ;o)" << std::endl;
    }

    // Exporting `saturnin::log::plugin` variable with alias name `plugin`
    // (Has the same effect as `BOOST_DLL_ALIAS(saturnin::log::plugin, plugin)`)
    extern "C" BOOST_SYMBOL_EXPORT LogPlugin plugin;
    LogPlugin plugin;

}
}