#ifndef REALLIB_INSTRUMENTATION_HPP
#define REALLIB_INSTRUMENTATION_HPP

#include <chrono>
#include <fstream>

namespace real {

class Instrumentation {
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

	struct Timer {
		Timer(Data d);
		~Timer();

		void stop();

		std::chrono::time_point<std::chrono::high_resolution_clock> start;
		Data data;
		long micro_seconds;
		bool stopped {false};
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
};

};


#define RL_INSTRUMENT_PROFILE_END ::real::Instrumentation::get().end_profile();
#define RL_INSTRUMENT_PROFILE_START(name) ::real::Instrumentation::get().start_profile(name);
#define RL_INSTRUMENT_FUNCTION ::real::Instrumentation::Timer t({__FILE_NAME__, __FUNCTION__, __LINE__});

#endif
