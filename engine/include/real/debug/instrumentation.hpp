#ifndef REALLIB_INSTRUMENTATION_HPP
#define REALLIB_INSTRUMENTATION_HPP

#include "real/core/core.hpp"
#include "real/core/types.hpp"
#include <chrono>
#include <fstream>

namespace real {

struct Timer;

class REALLIB_EXPORT Instrumentation {
private:
	Instrumentation();

public:
	static Instrumentation &get() {
		static Instrumentation instr;
		return instr;
	}

	~Instrumentation();

public:
	struct Data {
		const char *file;
		const char *function;
		int line;
		size_t thread;
	};

public:
	void log_timer(Timer &timer);
	void start_profile(std::string name, size_t thread=0);
	void end_profile();
	void set_active(bool _active) { active = _active; }

private:
	bool active {true};
	std::string profile;
	std::ofstream out_file;
	std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
};

};

#endif
