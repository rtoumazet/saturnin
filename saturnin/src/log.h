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
#include <spdlog/sinks/ringbuffer_sink.h>
// NOLINTNEXTLINE(modernize - deprecated - headers)
#include <saturnin/src/exceptions.h> // LogError
#include <saturnin/src/locale.h>     // tr
#include <saturnin/src/utilities.h>  // format

namespace excpt = saturnin::exception;

namespace saturnin::core {

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   LogLevel
///
/// \brief  Log level values.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class LogLevel {
    info,  ///< Info, minimal logging.
    debug, ///< Debug, full log.
    off    ///< Logging is disabled.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \enum   Logger
///
/// \brief  Values that represent the various loggers.
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class Logger {
    cdrom,
    config,
    gui,
    main,
    memory,
    sh2,
    scu,
    vdp1,
    vdp2,
    opengl,
    smpc,
    scsp,
    texture,
    unimplemented,
    generic,
    test
};

using MapLogger     = std::map<std::string, std::shared_ptr<spdlog::logger>>; ///< MapLogger alias definition.
using MapLoggerName = std::map<const Logger, const std::string>;              ///< MapLoggerName alias definition.

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
    Log()                                = delete;
    Log(const Log&)                      = delete;
    Log(Log&&)                           = delete;
    auto operator=(const Log&) & -> Log& = delete;
    auto operator=(Log&&) & -> Log&      = delete;
    ~Log()                               = delete;
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
    /// \fn static void Log::shutdown();
    ///
    /// \brief  Shuts down this and frees any resources it is using
    ///
    /// \author Runik
    /// \date   01/10/2021
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static void shutdown();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn template <typename... Args> static inline void Log::error(std::string_view logger_name, const std::string& value,
    /// const Args&... args)
    ///
    /// \brief  writes an error message to the specified logger.
    ///
    /// \author Runik
    /// \date   08/02/2018
    ///
    /// \tparam Args    Template arguments.
    /// \param  logger  Type of the logger.
    /// \param  value       Text to write.
    /// \param  args        Variable arguments for formatting.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    template<typename... Args>
    static inline void error(const Logger logger, std::string_view value, const Args&... args) {
        try {
            const auto& logger_name = loggers_names_[logger];
            if (loggerExists(logger_name)) {
                loggers_.at(logger_name)->error(fmt::runtime(value), args...);

                // Errors are also logged to console, using original logger name.
                // Using append() here as operator '+' isn't available for string_view.
                const auto& message = std::string{"[{}] "}.append(value);
                loggers_.at("console")->error(fmt::runtime(message.c_str()), logger_name, args...);
            }
        } catch (const std::runtime_error& e) { loggers_.at("console")->warn(e.what()); }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn template <typename... Args> static inline void Log::error(std::string_view logger_name, const std::string& value,
    /// const Args&... args)
    ///
    /// \brief  writes an error message to the specified logger and throws.
    ///
    /// \author Runik
    /// \date   01/03/2023
    ///
    /// \tparam Args    Template arguments.
    /// \param  logger  Type of the logger.
    /// \param  value       Text to write.
    /// \param  args        Variable arguments for formatting.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    template<typename... Args>
    [[noreturn]] static inline void exception(const Logger logger, std::string_view value, const Args&... args) {
        try {
            const auto& logger_name = loggers_names_[logger];
            if (loggerExists(logger_name)) {
                loggers_.at(logger_name)->critical(fmt::runtime(value), args...);

                // Errors are also logged to console, using original logger name.
                // Using append() here as operator '+' isn't available for string_view.
                const auto& message = std::string{"[{}] "}.append(value);
                loggers_.at("console")->critical(fmt::runtime(message.c_str()), logger_name, args...);

                // Throwing the linked exception.
                const auto str = utilities::format(value, args...);
                switch (logger) {
                    using enum Logger;
                    case cdrom: throw excpt::CdromError(str);
                    case config: throw excpt::ConfigError(str);
                    case gui: throw excpt::GuiError(str);
                    case generic: throw excpt::GenericError(str);
                    case main: throw excpt::MainError(str);
                    case memory: throw excpt::MemoryError(str);
                    case opengl: throw excpt::OpenglError(str);
                    case scsp: throw excpt::ScspError(str);
                    case scu: throw excpt::ScuError(str);
                    case sh2: throw excpt::Sh2Error(str);
                    case smpc: throw excpt::SmpcError(str);
                    case test: throw excpt::TestError(str);
                    case texture: throw excpt::TextureError(str);
                    case unimplemented: throw excpt::UnimplementedError(str);
                    case vdp1: throw excpt::Vdp1Error(str);
                    case vdp2: throw excpt::Vdp2Error(str);
                }
            }
            //} catch (const std::runtime_error& e) {
        } catch (...) {
            throw;
            // loggers_.at("console")->warn(e.what());
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn template <typename... Args> static inline void Log::warning(std::string_view logger_name, const std::string&
    /// value, const Args&... args)
    ///
    /// \brief  Writes a warning message to the specified logger.
    ///
    /// \author Runik
    /// \date   08/02/2018
    ///
    /// \tparam Args    Template arguments.
    /// \param  logger  Type of the logger.
    /// \param  value       Text to write.
    /// \param  args        Variable arguments for formatting.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    template<typename... Args>
    static inline void warning(const Logger logger, std::string_view value, const Args&... args) {
        try {
            const auto& logger_name = loggers_names_[logger];
            if (loggerExists(logger_name)) {
                loggers_.at(logger_name)->warn(fmt::runtime(value), args...);
                // Warnings are also logged to console, using original logger name.
                // Using append() here as operator '+' isn't available for string_view.
                const auto& message = std::string{"[{}] "}.append(value);
                loggers_.at("console")->warn(fmt::runtime(message.c_str()), logger_name, args...);
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
    /// \param  logger  Type of the logger.
    /// \param  value       Text to write.
    /// \param  args        Variable arguments for formatting.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    template<typename... Args>
    static inline void info(const Logger logger, std::string_view value, const Args&... args) {
        const auto& logger_name = loggers_names_[logger];
        if (loggerExists(logger_name)) { loggers_.at(logger_name)->info(fmt::runtime(value), args...); }
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
    /// \param  logger  Type of the logger.
    /// \param  value       Text to write.
    /// \param  args        Variable arguments for formatting.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    template<typename... Args>
    static inline void debug(const Logger logger, std::string_view value, const Args&... args) {
        const auto& logger_name = loggers_names_[logger];
        if (loggerExists(logger_name)) { loggers_.at(logger_name)->debug(fmt::runtime(value), args...); }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn template<typename... Args> static inline void Log::unimplemented(const std::string& value, const Args&... args)
    ///
    /// \brief  Writes a debug message to the unimplemented logger.
    ///
    /// \tparam Args    Type of the arguments.
    /// \param  value   The value.
    /// \param  args    Variable arguments providing the arguments.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    template<typename... Args>
    static inline void unimplemented(const std::string& value, const Args&... args) {
        debug(Logger::unimplemented, value, args...);
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
            const auto error = utilities::format(tr("Log '{0}' is not defined !"), logger_name);
            throw exception::LogError(error);
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
    /// \fn static auto Log::createRingbufferSink() -> std::shared_ptr<spdlog::sinks::ringbuffer_sink_mt>;
    ///
    /// \brief  Creates ringbuffer sink.
    ///
    /// \author Runik
    /// \date   10/03/2021
    ///
    /// \returns    The new ringbuffer sink.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static auto createRingbufferSink() -> std::shared_ptr<spdlog::sinks::ringbuffer_sink_mt>;

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
    /// \fn static auto Log::getRingbuffer() -> std::string;
    ///
    /// \brief  Gets the ringbuffer content
    ///
    /// \author Runik
    /// \date   11/03/2021
    ///
    /// \returns    The ringbuffer content.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static auto getRingbuffer() -> std::string;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \fn static void Log::setLogLevel(const std::string& logger, const LogLevel level);
    ///
    /// \brief  Sets log level of the logger passed as parameter.
    ///
    /// \author Runik
    /// \date   12/05/2021
    ///
    /// \param  logger  The logger.
    /// \param  level   The new level.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    static void setLogLevel(const std::string& logger, const LogLevel level);

  private:
    static MapLogger                                          loggers_; ///< Map containing all the loggers used in the program
    static MapLoggerName                                      loggers_names_; ///< Link between logger enumerator and logger name.
    static std::shared_ptr<spdlog::sinks::ringbuffer_sink_mt> ringbuffer_sink_; ///< The ringbuffer sink
};
}; // namespace saturnin::core
