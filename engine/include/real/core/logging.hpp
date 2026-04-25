#ifndef REALLIB_LOGGING_HPP
#define REALLIB_LOGGING_HPP

#include <filesystem>
#include <fmt/base.h>
#include <fstream>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include <fmt/core.h>

namespace real {

enum LogLevel {
    LogLevel_Trace = 0,
    LogLevel_Info,
    LogLevel_Warn,
    LogLevel_Error,
    LogLevel_Fatal,
};

static const char *LogLevel_to_string[5] = {
    "trace", "info", "warn", "error", "fatal"
};

/**
 * @brief abstract class for log sinks
 */
class LogSink {
public:
    virtual ~LogSink() = default;

public:
    virtual void log_to_sink(LogLevel level, std::string_view str) = 0;
};

/**
 * @brief logs to console in color or not in color
 */
class LogSink_Console : public LogSink {
public:
    LogSink_Console(bool _color=true);

public:
    void log_to_sink(LogLevel level, std::string_view str);

private:
    bool color;
};

class LogSink_File : public LogSink {
public:
    LogSink_File(std::filesystem::path path);
    ~LogSink_File();

public:
    void log_to_sink(LogLevel level, std::string_view str);

private:
    std::ofstream out_file;
};

/**
 * @brief base logging class that logs data to various sinks
 */
class Log {
public:
    Log(std::string _name="LOG", LogLevel _level=LogLevel_Trace);
    ~Log();

    template<typename ...Args>
    void log(LogLevel level, std::string_view in, Args&& ...args) { 
        if(level < log_level) {
            return;
        }
        
        std::stringstream buf;
        buf << "[" << name << "]";
        buf << "[" << LogLevel_to_string[level] << "]";
        buf << "[" << __TIME__ << "] ";
        buf << fmt::format(fmt::runtime(in), std::forward<Args>(args)...) << "\n";

        for(auto *sink : sinks)
            sink->log_to_sink(level, buf.str());
    }

    template<typename ...Args>
    void info(std::string_view in, Args&& ...args) {
        log(LogLevel_Info, in, std::forward<Args>(args)...);
    }

    template<typename ...Args>
    void trace(std::string_view in, Args&& ...args) {
        log(LogLevel_Trace, in, std::forward<Args>(args)...);
    }

    template<typename ...Args>
    void warn(std::string_view in, Args&& ...args) {
        log(LogLevel_Warn, in, std::forward<Args>(args)...);
    }

    template<typename ...Args>
    void error(std::string_view in, Args&& ...args) {
        log(LogLevel_Error, in, std::forward<Args>(args)...);
    }

    template<typename ...Args>
    void fatal(std::string_view in, Args&& ...args) {
        log(LogLevel_Fatal, in, std::forward<Args>(args)...);
    }

public:
    std::vector<LogSink*> sinks;
    std::string name = "LOG";
    LogLevel log_level;
};

}

#endif