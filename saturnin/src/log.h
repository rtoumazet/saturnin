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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file	log.h
///
/// \brief	Declares the static Log class. 
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <memory>
#include <string>
#include <map>
#include <spdlog/spdlog.h>
#include <fmt/format.h>
#include "locale.h" // tr

namespace saturnin {
namespace core {

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \class  Log
    ///
    /// \brief  Handles logging in the emulator. Spdlog wrapper.
    ///
    /// \author Runik
    /// \date   21/06/2018
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    class Log {
    public:
        //@{
        // Constructors / Destructors
        Log()                        = delete;
        Log(const Log&)              = delete;
        Log(Log&&)                   = delete;
        Log& operator=(const Log&) & = delete;
        Log& operator=(Log&&) &      = delete;
        ~Log()                       = delete;
        //@}

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn static bool Log::initialize();
        ///
        /// \brief  Initializes various logs used in Saturnin.
        ///
        /// \author Runik
        /// \date   08/02/2018
        ///
        /// \return True if it succeeds, false if it fails.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        static bool initialize();

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn template <typename... Args> static inline void Log::error(const std::string& logger_name, const std::string& value, const Args&... args)
        ///
        /// \brief  writes an error message to the specified logger.
        ///
        /// \author Runik
        /// \date   08/02/2018
        ///
        /// \tparam Args    Template arguments.
        /// \param  logger_name Name of the logger.
        /// \param  value       Text to write.
        /// \param  args        Variable arguments for formatting.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        template <typename... Args>
        static inline void error(const std::string& logger_name, const std::string& value, const Args&... args) {
            try {
                if (loggerExists(logger_name)) {
                    loggers_.at(logger_name)->error(value.c_str(), args...);
                    // errors are also logged to console, using original logger name
                    auto message{ "[{}] " + value };
                    loggers_.at("console")->error(message.c_str(), logger_name, args...);
                }
            }
            catch (const std::runtime_error& e) {
                loggers_.at("console")->warn(e.what());
            }
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn template <typename... Args> static inline void Log::warning(const std::string& logger_name, const std::string& value, const Args&... args)
        ///
        /// \brief  Writes a warning message to the specified logger.
        ///
        /// \author Runik
        /// \date   08/02/2018
        ///
        /// \tparam Args    Template arguments.
        /// \param  logger_name Name of the logger.
        /// \param  value       Text to write.
        /// \param  args        Variable arguments for formatting.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        template <typename... Args>
        static inline void warning(const std::string& logger_name, const std::string& value, const Args&... args) {
            try {
                if (loggerExists(logger_name)) {
                    loggers_.at(logger_name)->warn(value.c_str(), args...);
                    // warnings are also logged to console, using original logger name
                    auto message{ "[{}] " + value };
                    loggers_.at("console")->warn(message.c_str(), logger_name, args...);
                }
            }
            catch (const std::runtime_error& e) {
                loggers_.at("console")->warn(e.what());
            }
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn template <typename... Args> static inline void Log::info(const std::string& logger_name, const std::string& value, const Args&... args)
        ///
        /// \brief  Writes an info message to the specified logger.
        ///
        /// \author Runik
        /// \date   08/02/2018
        ///
        /// \tparam Args    Template arguments.
        /// \param  logger_name Name of the logger.
        /// \param  value       Text to write.
        /// \param  args        Variable arguments for formatting.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        template <typename... Args>
        static inline void info(const std::string& logger_name, const std::string& value, const Args&... args) {
            if (loggerExists(logger_name)) loggers_.at(logger_name)->info(value.c_str(), args...);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn template <typename... Args> static inline void Log::debug(const std::string& logger_name, const std::string& value, const Args&... args)
        ///
        /// \brief  Writes a debug message to the specified logger.
        ///
        /// \author Runik
        /// \date   08/02/2018
        ///
        /// \tparam Args    Template arguments.
        /// \param  logger_name Name of the logger.
        /// \param  value       Text to write.
        /// \param  args        Variable arguments for formatting.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        template <typename... Args>
        static inline void debug(const std::string& logger_name, const std::string& value, const Args&... args) {
            if (loggerExists(logger_name)) loggers_.at(logger_name)->debug(value.c_str(), args...);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn static inline bool Log::loggerExists(const std::string& logger_name);
        ///
        /// \brief  Queries if a given logger exists.
        ///
        /// \author Runik
        /// \date   08/02/2018
        ///
        /// \param  logger_name Name of the logger.
        ///
        /// \return True if the logger exists.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        static inline bool loggerExists(const std::string& logger_name){
            if (loggers_.count(logger_name) == 0) {
                throw std::runtime_error( fmt::format(tr("Log '{0}' is not defined !"), logger_name));
            }
            return true;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn static std::shared_ptr<spdlog::sinks::simple_file_sink_mt> Log::createFileSink(const std::string& logger_path);
        ///
        /// \brief  Creates a sink.
        ///
        /// \author Runik
        /// \date   08/02/2018
        ///
        /// \param  logger_path Full pathname of the logger file.
        ///
        /// \return The new sink.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        static std::shared_ptr<spdlog::sinks::simple_file_sink_mt> createFileSink(const std::string& logger_path);
        
        
        static std::shared_ptr<spdlog::sinks::wincolor_stdout_sink_mt> createConsoleSink();

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn static void Log::createLogger(const std::string& logger_name, const std::shared_ptr<spdlog::sinks::simple_file_sink_mt>& sink);
        ///
        /// \brief  Creates a logger.
        ///
        /// \author Runik
        /// \date   08/02/2018
        ///
        /// \param  logger_name Name of the logger.
        /// \param  sink        The sink.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        static void createLogger(const std::string& logger_name, const std::shared_ptr<spdlog::sinks::simple_file_sink_mt>& sink);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn static void Log::createConsole();
        ///
        /// \brief  Creates the console logger. Will be accessible using "console" name.
        ///
        /// \author Runik
        /// \date   23/06/2018
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        static void createConsole();


        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \fn static void Log::removeFile(const std::string& path);
        ///
        /// \brief  Creates the fileor empty directory.
        ///
        /// \author Runik
        /// \date   08/02/2018
        ///
        /// \param  path    Full pathname of the file.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        static void removeFile(const std::string& path);

    private:
        static std::map<std::string, std::shared_ptr<spdlog::logger>> loggers_; ///< Map containing all the loggers used in the program
    };

};
};