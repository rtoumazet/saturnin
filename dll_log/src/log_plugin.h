// 
// log_plugin.h
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
#pragma once

#include <iostream>
#include <boost/config.hpp> // for BOOST_SYMBOL_EXPORT
#include <spdlog/spdlog.h>
#include "../../saturnin_plugin_api/saturnin_plugin_api.h"

namespace saturnin {
    namespace log {

        class my_plugin_log : public saturnin_plugin_api {
        public:
            my_plugin_log();
            ~my_plugin_log();

            std::string version() const;

            void log(const std::string& value) const;

            std::shared_ptr<spdlog::sinks::simple_file_sink_mt> createSink(const std::string& logger_path);

            std::shared_ptr<spdlog::logger> createLogger(const std::string& logger_name, const std::string& logger_path);
            std::shared_ptr<spdlog::logger> createLogger(const std::string& logger_name, const std::shared_ptr<spdlog::sinks::simple_file_sink_mt>& sink);
            std::shared_ptr<spdlog::logger> createLogger(const std::string& logger_name, const std::vector<spdlog::sink_ptr>& sinks);
            std::shared_ptr<spdlog::logger> createLogger(const std::string& logger_name, const std::string& logger_path, const std::shared_ptr<spdlog::sinks::simple_file_sink_mt>& globalSink);

            void createOrEmptyDirectory(const std::string& path);

        private:
            std::string pattern_;
        };

    }
} // namespace my_namespace