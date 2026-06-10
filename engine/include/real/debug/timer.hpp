#ifndef REALLIB_TIMER_HPP
#define REALLIB_TIMER_HPP

#include <real/debug/instrumentation.hpp>

namespace real {

struct Timer {
    Timer(bool instrumented=false, Instrumentation::Data d={});
    ~Timer();

    void stop();
    void restart();

    std::chrono::time_point<std::chrono::high_resolution_clock> start;
    Instrumentation::Data data;
    long micro_seconds;
    bool stopped {false};
    bool instr;
};

}

#endif