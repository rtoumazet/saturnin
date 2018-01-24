#ifndef _LOG_PLUGIN_H
#define _LOG_PLUGIN_H

#include <iostream>
#include <boost/config.hpp> // for BOOST_SYMBOL_EXPORT
#include <spdlog/spdlog.h>
#include "base_plugin_api.h"

namespace my_namespace {

class my_plugin_log : public my_plugin_api {
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

} // namespace my_namespace
#endif