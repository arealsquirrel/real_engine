
#include <fmt/base.h>
#include <real/core/logging.hpp>
#include <string_view>
#include <fmt/color.h>

namespace real {

LogSink_Console::LogSink_Console(bool _color) 
    : color(_color) {}

void LogSink_Console::log_to_sink(LogLevel level, std::string_view str) {
    if(color == false) {
        fmt::print(fmt::runtime(str));
        return;
    } 

    switch (level) {
    case LogLevel_Trace:
        fmt::print(fg(fmt::terminal_color::white), fmt::runtime(str));
        break;
    case LogLevel_Info:
        fmt::print(fg(fmt::terminal_color::bright_blue), fmt::runtime(str));
        break;
    case LogLevel_Warn:
        fmt::print(fg(fmt::terminal_color::bright_yellow), fmt::runtime(str));
        break;
    case LogLevel_Error:
        fmt::print(fg(fmt::terminal_color::bright_red), fmt::runtime(str));
        break;
    case LogLevel_Fatal:
        fmt::print(fg(fmt::terminal_color::bright_red) | fmt::emphasis::bold, fmt::runtime(str));
        break;
    }
}

LogSink_File::LogSink_File(std::filesystem::path path) 
    : out_file(path) {}

LogSink_File::~LogSink_File() {
    out_file.close();
}

void LogSink_File::log_to_sink(LogLevel level, std::string_view str) {
    out_file << str;
    out_file.flush();
}


Log::Log(std::string _name, LogLevel _level)
    : name(_name), log_level(_level) {
}

Log::~Log() {}

}
