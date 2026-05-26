
#include <fmt/base.h>
#include <real/core/logging.hpp>
#include <string_view>
#include <fmt/color.h>

namespace real {

Log *Log::s_log = nullptr;

LogSink_Console::LogSink_Console(bool _color) 
    : color(_color) {}

void LogSink_Console::log_to_sink(LogData data, std::string_view str) {
    if(color == false) {
        fmt::print(fmt::runtime(str));
        return;
    } 

    switch (data.level) {
    case LogLevel_Trace:
        fmt::print(fg(fmt::terminal_color::white), fmt::runtime(str));
		fmt::print("\n");
        break;
    case LogLevel_Info:
        fmt::print(fg(fmt::terminal_color::bright_blue), fmt::runtime(str));
		fmt::print("\n");
        break;
    case LogLevel_Warn:
        fmt::print(fg(fmt::terminal_color::bright_yellow), fmt::runtime(str));
		fmt::print("\n");
        break;
    case LogLevel_Error:
        fmt::print(fg(fmt::terminal_color::bright_red), fmt::runtime(str));
		fmt::print("\n");
        break;
    case LogLevel_Fatal:
        fmt::print(fg(fmt::terminal_color::bright_red) | fmt::emphasis::bold, fmt::runtime(str));
		fmt::print("\n");
        break;
    }
}

LogSink_File::LogSink_File(std::filesystem::path path) 
    : out_file(path) {}

LogSink_File::~LogSink_File() {
    out_file.close();
}

void LogSink_File::log_to_sink(LogData level, std::string_view str) {
    out_file << str;
    out_file.flush();
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
