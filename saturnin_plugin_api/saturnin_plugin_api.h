// 
// saturnin_plugin_api.h
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

#include <string>
//#include <spdlog/spdlog.h>

namespace saturnin {

    class saturnin_plugin_api {
    public:
        virtual std::string version() const = 0;
        //virtual void log(const std::string&) const = 0;
        //virtual std::shared_ptr<spdlog::sinks::simple_file_sink_mt> createSink(const std::string& logger_path) = 0;
        //virtual std::shared_ptr<spdlog::logger> createLogger(const std::string& logger_name, const std::string& logger_path) = 0;
        //virtual std::shared_ptr<spdlog::logger> createLogger(const std::string& logger_name, const std::shared_ptr<spdlog::sinks::simple_file_sink_mt>& sink) = 0;
        //virtual std::shared_ptr<spdlog::logger> createLogger(const std::string& logger_name, const std::vector<spdlog::sink_ptr>& sinks) = 0;
        //virtual std::shared_ptr<spdlog::logger> createLogger(const std::string& logger_name, const std::string& logger_path, const std::shared_ptr<spdlog::sinks::simple_file_sink_mt>& globalSink) = 0;

        virtual ~saturnin_plugin_api() {}
    };

}