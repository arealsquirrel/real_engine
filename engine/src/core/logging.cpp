
#include <fmt/base.h>
#include <real/core/logging.hpp>
#include <string_view>
#include <fmt/color.h>
#include <strings.h>
#include <utility>

namespace real {


LogSink_Console::LogSink_Console(bool _color) 
    : color(_color) {}

void LogSink_Console::log_to_sink(LogData data, std::string_view str) {
    if(color == false) {
        fmt::print(fmt::runtime(str));
        return;
    } 

	std::stringstream buf;
	buf << "[" << LogLevel_to_string[data.level] << "]";
	buf << "[" << data.file << ":" << data.line << "] " << str << "\n";

    switch (data.level) {
    case LogLevel_Trace:
        fmt::print(fg(fmt::terminal_color::white), fmt::runtime(buf.str()));
        break;
    case LogLevel_Info:
        fmt::print(fg(fmt::terminal_color::blue), fmt::runtime(buf.str()));
        break;
    case LogLevel_Warn:
        fmt::print(fg(fmt::terminal_color::yellow), fmt::runtime(buf.str()));
        break;
    case LogLevel_Error:
        fmt::print(fg(fmt::terminal_color::red), fmt::runtime(buf.str()));
        break;
    case LogLevel_Fatal:
        fmt::print(fg(fmt::terminal_color::red), fmt::runtime(buf.str()));
        break;
    }
}

LogSink_File::LogSink_File(std::filesystem::path path) 
    : out_file(path) {}

LogSink_File::~LogSink_File() {
    out_file.close();
}

void LogSink_File::log_to_sink(LogData data, std::string_view str) {
    out_file << str;
    out_file.flush();
}

LogSink_Buffer::LogSink_Buffer()
	: buffer(1000) {}

void LogSink_Buffer::log_to_sink(LogData data, std::string_view str) {
	buffer.data()[index++ % 1000] = std::make_pair(str, data);
}

Log &Log::get() {
	static Log log {};
	return log;
}

Log::Log(std::string _name, LogLevel _level)
    : name(_name), log_level(_level) {
}

Log::~Log() {
    for(auto *s : sinks) {
        delete s;
    }
}

}
