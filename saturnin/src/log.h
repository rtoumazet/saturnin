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

#include <iosfwd> // ostringstream
#include <map>    // map
#include <string> // string
#define SPDLOG_FMT_EXTERNAL
#define FMT_HEADER_ONLY
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/ostream_sink.h>
// NOLINTNEXTLINE(modernize-deprecated-headers)
#include "locale.h" // tr

namespace saturnin::core {

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
    Log()           = delete;
    Log(const Log&) = delete;
    Log(Log&&)      = delete;
    auto operator=(const Log&) & -> Log& = delete;
    auto operator=(Log&&) & -> Log& = delete;
    ~Log()                          = delete;
    //@}

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static auto Log::initialize() -> bool;
    ///
    /// \brief  Initializes various logs used in Saturnin.
    ///
    /// \author Runik
    /// \date   08/02/2018
    ///
    /// \return True if initialization is successful.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static auto initialize() -> bool;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn template <typename... Args> static inline void Log::error(const std::string& logger_name, const std::string& value,
    /// const Args&... args)
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

    template<typename... Args>
    static inline void error(const std::string& logger_name, const std::string& value, const Args&... args) {
        try {
            if (loggerExists(logger_name)) {
                loggers_.at(logger_name)->error(value.c_str(), args...);
                // errors are also logged to console, using original logger name
                const auto message = std::string{"[{}] " + value};
                loggers_.at("console")->error(message.c_str(), logger_name, args...);
            }
        } catch (const std::runtime_error& e) { loggers_.at("console")->warn(e.what()); }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn template <typename... Args> static inline void Log::warning(const std::string& logger_name, const std::string&
    /// value, const Args&... args)
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

    template<typename... Args>
    static inline void warning(const std::string& logger_name, const std::string& value, const Args&... args) {
        try {
            if (loggerExists(logger_name)) {
                loggers_.at(logger_name)->warn(value.c_str(), args...);
                // warnings are also logged to console, using original logger name
                const auto message = std::string{"[{}] " + value};
                loggers_.at("console")->warn(message.c_str(), logger_name, args...);
            }
        } catch (const std::runtime_error& e) { loggers_.at("console")->warn(e.what()); }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn template <typename... Args> static inline void Log::info(const std::string& logger_name, const std::string& value,
    /// const Args&... args)
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

    template<typename... Args>
    static inline void info(const std::string& logger_name, const std::string& value, const Args&... args) {
        if (loggerExists(logger_name)) { loggers_.at(logger_name)->info(value.c_str(), args...); }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn template <typename... Args> static inline void Log::debug(const std::string& logger_name, const std::string&
    /// value, const Args&... args)
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

    template<typename... Args>
    static inline void debug(const std::string& logger_name, const std::string& value, const Args&... args) {
        if (loggerExists(logger_name)) { loggers_.at(logger_name)->debug(value.c_str(), args...); }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static inline auto Log::loggerExists(const std::string& logger_name) -> bool
    ///
    /// \brief  Queries if a given logger exists.
    ///
    /// \author Runik
    /// \date   08/02/2018
    ///
    /// \exception  std::runtime_error  Raised when a runtime error condition occurs.
    ///
    /// \param  logger_name Name of the logger.
    ///
    /// \return True if the logger exists.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static inline auto loggerExists(const std::string& logger_name) -> bool {
        if (loggers_.count(logger_name) == 0) {
            throw std::runtime_error(fmt::format(tr("Log '{0}' is not defined !"), logger_name));
        }
        return true;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static auto Log::createFileSink(const std::string& logger_path) ->
    /// std::shared_ptr<spdlog::sinks::basic_file_sink_mt>;
    ///
    /// \brief  Creates a file sink.
    ///
    /// \author Runik
    /// \date   08/02/2018
    ///
    /// \param  logger_path Full pathname of the logger file.
    ///
    /// \return Sink linked to the logger file.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static auto createFileSink(const std::string& logger_path) -> std::shared_ptr<spdlog::sinks::basic_file_sink_mt>;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static auto Log::createConsoleSink() ->std::shared_ptr<spdlog::sinks::wincolor_stdout_sink_mt>;
    ///
    /// \brief  Creates a color console sink.
    ///
    /// \author Runik
    /// \date   30/06/2018
    ///
    /// \return Sink linked to the color console.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static auto createConsoleSink() -> std::shared_ptr<spdlog::sinks::wincolor_stdout_sink_mt>;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static auto Log::createStreamSink() -> std::shared_ptr<spdlog::sinks::ostream_sink_mt>;
    ///
    /// \brief  Creates a ostream sink.
    ///
    /// \author Runik
    /// \date   12/04/2020
    ///
    /// \returns    The new stream sink.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static auto createStreamSink() -> std::shared_ptr<spdlog::sinks::ostream_sink_mt>;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static void Log::createLogger(const std::string& logger_name, const
    /// std::shared_ptr<spdlog::sinks::simple_file_sink_mt>& sink);
    ///
    /// \brief  Creates a logger.
    ///
    /// \author Runik
    /// \date   08/02/2018
    ///
    /// \param  logger_name Name of the logger.
    /// \param  sink        The sink.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    // static void createLogger(const std::string& logger_name, const std::shared_ptr<spdlog::sinks::basic_file_sink_mt>& sink);
    static void createLogger(const std::string& logger_name, const spdlog::sinks_init_list& sink);

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
    /// \fn static void Log::createStream();
    ///
    /// \brief  Creates the stream logger, will be displayed in the gui.
    ///
    /// \author Runik
    /// \date   12/04/2020
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static void createStream();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static void Log::removeFile(const std::string& path);
    ///
    /// \brief  Removes the file if it exists, creates the the directory if it doesn't exist.
    ///
    /// \author Runik
    /// \date   08/02/2018
    ///
    /// \param  path    Full pathname of the file.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static void removeFile(const std::string& path);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static void Log::flush();
    ///
    /// \brief  Flushes the log.
    ///
    /// \author Runik
    /// \date   04/12/2019
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static void flush();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static void Log::dumpBacktraceToConsole();
    ///
    /// \brief  Dumps the backtrace to console.
    ///
    /// \author Runik
    /// \date   13/04/2020
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static void dumpBacktraceToConsole();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static auto Log::getStream() -> std::string;
    ///
    /// \brief  Gets the stream of logged messages
    ///
    /// \author Runik
    /// \date   14/04/2020
    ///
    /// \returns    A string containing every logged message.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static auto getStream() -> std::string;

  private:
    static std::ostringstream oss_;
    static std::map<std::string, std::shared_ptr<spdlog::logger>>
        loggers_; ///< Map containing all the loggers used in the program
};

}; // namespace saturnin::core
