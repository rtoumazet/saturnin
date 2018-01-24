#ifndef _BASE_PLUGIN_API_H
#define _BASE_PLUGIN_API_H

#include <string>
#include <spdlog/spdlog.h>

class my_plugin_api {
public:
   virtual std::string version() const = 0;
   virtual void log(const std::string&) const = 0;
   virtual std::shared_ptr<spdlog::sinks::simple_file_sink_mt> createSink(const std::string& logger_path) = 0;
   virtual std::shared_ptr<spdlog::logger> createLogger(const std::string& logger_name, const std::string& logger_path) = 0;
   virtual std::shared_ptr<spdlog::logger> createLogger(const std::string& logger_name, const std::shared_ptr<spdlog::sinks::simple_file_sink_mt>& sink) = 0;
   virtual std::shared_ptr<spdlog::logger> createLogger(const std::string& logger_name, const std::vector<spdlog::sink_ptr>& sinks) = 0;
   virtual std::shared_ptr<spdlog::logger> createLogger(const std::string& logger_name, const std::string& logger_path, const std::shared_ptr<spdlog::sinks::simple_file_sink_mt>& globalSink) = 0;

   virtual ~my_plugin_api() {}
};

#endif