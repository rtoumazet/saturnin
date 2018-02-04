// 
// log.h
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

#pragma once

#include <memory>
#include <string>
#include <spdlog/spdlog.h>

namespace saturnin {
namespace core {
    
    class Log {
    public:
        enum class Type {
            VDP1,
            VDP2,
            OPENGL,
            CDROM,
            SMPC,
            M68K,
            SCSP,
            SOUND,
            SH2,
            SH2_MASTER,
            SH2_SLAVE
        };

        enum class Severity {
            DEBUG,
            INFO,
            WARNING,
            ERR,
            FATAL
        };

        //@{
        // Constructors / Destructors
        Log();
        Log(const Log&)              = delete;
        Log(Log&&)                   = delete;
        Log& operator=(const Log&) & = delete;
        Log& operator=(Log&&) &      = delete;
        ~Log();
        //@}

        void log(const std::string& value) const;

        std::shared_ptr<spdlog::sinks::simple_file_sink_mt> createSink(const std::string& logger_path);

        std::shared_ptr<spdlog::logger> createLogger(const std::string& logger_name, const std::string& logger_path);
        std::shared_ptr<spdlog::logger> createLogger(const std::string& logger_name, const std::shared_ptr<spdlog::sinks::simple_file_sink_mt>& sink);
        std::shared_ptr<spdlog::logger> createLogger(const std::string& logger_name, const std::vector<spdlog::sink_ptr>& sinks);
        std::shared_ptr<spdlog::logger> createLogger(const std::string& logger_name, const std::string& logger_path, const std::shared_ptr<spdlog::sinks::simple_file_sink_mt>& globalSink);

        void createOrEmptyDirectory(const std::string& path);

    private:
        std::string pattern_;
        std::shared_ptr<spdlog::sinks::simple_file_sink_mt> globalSink_;
        std::shared_ptr<spdlog::logger> global_;
        std::shared_ptr<spdlog::logger> vdp1_;
        std::shared_ptr<spdlog::logger> cdrom_;
    };

};
};