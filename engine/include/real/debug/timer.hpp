#ifndef REALLIB_TIMER_HPP
#define REALLIB_TIMER_HPP

#include <real/debug/instrumentation.hpp>

namespace real {

struct REALLIB_EXPORT Timer {
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

/* lowkinienuly dont give a fuck. thats how nonchilliant I be. */

#ifdef RL_ENABLE_INSTRUMENATION
#define RL_INSTRUMENT_PROFILE_END ::real::Instrumentation::get().end_profile();
#define RL_INSTRUMENT_PROFILE_START(name) ::real::Instrumentation::get().start_profile(name);
#define RL_INSTRUMENT_FUNCTION ::real::Timer t(true, {__FILE_NAME__, __FUNCTION__, __LINE__});
#else
#define RL_INSTRUMENT_PROFILE_END
#define RL_INSTRUMENT_PROFILE_START(name)
#define RL_INSTRUMENT_FUNCTION
#endif

}

#endif
