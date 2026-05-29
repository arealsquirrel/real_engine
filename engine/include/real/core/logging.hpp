#ifndef REALLIB_LOGGING_HPP
#define REALLIB_LOGGING_HPP

#include "fmt/color.h"
#include <cstdint>
#include <filesystem>
#include <fmt/base.h>
#include <fstream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include <fmt/core.h>
#include <real/core/core.hpp>

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
class REALLIB_EXPORT LogSink {
public:
	LogSink() = default;
    virtual ~LogSink() = default;

public:
    virtual void log_to_sink(LogData level, std::string_view str) = 0;
};

/**
 * @brief logs to console in color or not in color
 */
class REALLIB_EXPORT LogSink_Console : public LogSink {
public:
    LogSink_Console(bool _color=true);

public:
    void log_to_sink(LogData data, std::string_view str);

private:
    bool color;
};

class REALLIB_EXPORT LogSink_File : public LogSink {
public:
    LogSink_File(std::filesystem::path path);

public:
    ~LogSink_File();

public:
    void log_to_sink(LogData data, std::string_view str);

private:
    std::ofstream out_file;
};

class REALLIB_EXPORT LogSink_Buffer : public LogSink {
public:
    LogSink_Buffer();
	~LogSink_Buffer() = default;

public:
    void log_to_sink(LogData data, std::string_view str);

public:
	uint32_t index = 0;
	std::vector<std::pair<std::string, LogData>> buffer;
};

/**
 * @brief base logging class that logs data to various sinks
 */
class REALLIB_EXPORT Log {
public:
    Log(std::string _name="LOG", LogLevel _level=LogLevel_Trace);
    ~Log();

    template<typename ...Args>
    void log(LogData data, std::string_view in, Args&& ...args) {
        if(data.level < log_level) {
            return;
        }
        
        for(auto *sink : sinks)
            sink->log_to_sink(data,
					fmt::format(fmt::runtime(in), std::forward<Args>(args)...).c_str());
    }

	static Log &get();

public:
    std::vector<LogSink*> sinks;
    std::string name = "LOG";
    LogLevel log_level;

// private:
	//static Log *s_log;
};

}

#define RL_LOG_TRACE(...) ::real::Log::get().log({LogLevel_Trace,__TIME__,__FILE_NAME__,__LINE__}, __VA_ARGS__)
#define RL_LOG_INFO(...) ::real::Log::get().log({LogLevel_Info,__TIME__,__FILE_NAME__,__LINE__}, __VA_ARGS__)
#define RL_LOG_WARN(...) ::real::Log::get().log({LogLevel_Warn,__TIME__,__FILE_NAME__,__LINE__}, __VA_ARGS__)
#define RL_LOG_ERROR(...) ::real::Log::get().log({LogLevel_Fatal,__TIME__,__FILE_NAME__,__LINE__}, __VA_ARGS__)

#endif
