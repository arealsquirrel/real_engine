#ifndef REALLIB_LOGGING_HPP
#define REALLIB_LOGGING_HPP

#include "fmt/color.h"
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

struct LogData {
	LogLevel level;
	const char *time;
	const char *file;
	int line;
};

/**
 * @brief abstract class for log sinks
 */
class LogSink {
public:
    virtual ~LogSink() = default;

public:
    virtual void log_to_sink(LogData level, std::string_view str) = 0;
};

/**
 * @brief logs to console in color or not in color
 */
class LogSink_Console : public LogSink {
public:
    LogSink_Console(bool _color=true);

public:
    void log_to_sink(LogData level, std::string_view str);

private:
    bool color;
};

class LogSink_File : public LogSink {
public:
    LogSink_File(std::filesystem::path path);

public:
    ~LogSink_File();

public:
    void log_to_sink(LogData level, std::string_view str);

private:
    std::ofstream out_file;
};

class LogSink_Buffer : public LogSink {
public:
    LogSink_Buffer();

public:
    void log_to_sink(LogLevel level, std::string_view str);

private:
};

/**
 * @brief base logging class that logs data to various sinks
 */
class Log {
public:
    Log(std::string _name="LOG", LogLevel _level=LogLevel_Trace);
    ~Log();

    template<typename ...Args>
    void log(LogData data, std::string_view in, Args&& ...args) {
        if(data.level < log_level) {
            return;
        }
        
        std::stringstream buf;
        buf << "[" << name << "]";
        buf << "[" << LogLevel_to_string[data.level] << "]";
        buf << "[" << data.time << "] ";

        for(auto *sink : sinks)
            sink->log_to_sink(data,
					fmt::format(fmt::runtime(in), std::forward<Args>(args)...).c_str());
    }

	static Log *get() {
		if(s_log == nullptr)
			s_log = new Log("engine_log.txt");
		
		return s_log;
	}

public:
    std::vector<LogSink*> sinks;
    std::string name = "LOG";
    LogLevel log_level;

private:
	static Log *s_log;
};

}

#define RL_LOG_TRACE(...) ::real::Log::get()->log({LogLevel_Trace,__TIME__,__FILE__,__LINE__}, __VA_ARGS__)
#define RL_LOG_INFO(...) ::real::Log::get()->log({LogLevel_Info,__TIME__,__FILE__,__LINE__}, __VA_ARGS__)
#define RL_LOG_WARN(...) ::real::Log::get()->log({LogLevel_Warn,__TIME__,__FILE__,__LINE__}, __VA_ARGS__)
#define RL_LOG_ERROR(...) ::real::Log::get()->log({LogLevel_Fatal,__TIME__,__FILE__,__LINE__}, __VA_ARGS__)

#endif
