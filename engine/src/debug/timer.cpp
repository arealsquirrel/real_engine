
#include <real/debug/timer.hpp>

namespace real {

Timer::Timer(bool instrumented, Instrumentation::Data d)
	: data(d), instr(instrumented) {
	start = std::chrono::high_resolution_clock::now();
}

Timer::~Timer() {
	if(stopped == false)
		stop();

    if(instr)
	    Instrumentation::get().log_timer(*this);
}

void Timer::stop() {
	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
	micro_seconds = duration.count();
	stopped = true;
}

void Timer::restart() {
    start = std::chrono::high_resolution_clock::now();
    stopped = false;
    // micro_seconds = 0;
}

}
