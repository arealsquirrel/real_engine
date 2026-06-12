
#include <chrono>
#include <real/debug/instrumentation.hpp>
#include <real/core/logging.hpp>
#include <real/debug/timer.hpp>

namespace real {

Instrumentation::~Instrumentation() {

}

Instrumentation::Instrumentation() {

}

void Instrumentation::log_timer(Timer &timer) {
	if(active == false)
		return;

	if(out_file.is_open() == false) {
		RL_LOG_WARN("can not log when there is no active profile");
		return;
	}

	std::stringstream json;
	u32 ts = std::chrono::duration_cast<std::chrono::microseconds>(timer.start - start_time).count();
	
	json << std::setprecision(3) << std::fixed;
	json << ",{";
	json << "\"cat\":\"function\",";
	json << "\"dur\":" << (timer.micro_seconds) << ',';
	json << "\"name\":\"" << (timer.data.function) << "\",";
	json << "\"ph\":\"X\",";
	json << "\"pid\":0,";
	json << "\"tid\":" << timer.data.thread << ",";
	json << "\"ts\":" << (ts);
	json << "}";

	out_file << json.str();
	out_file.flush();
}

void Instrumentation::start_profile(std::string name, size_t thread) {
	if(out_file.is_open()) {
		RL_LOG_ERROR("can not start a profile when one is already open");
	}

	profile = name;
	out_file.open(name.append(".json"));

	out_file << "{\"otherData\": {},\"traceEvents\":[{}";
	out_file.flush();
	start_time = std::chrono::high_resolution_clock::now();
}

void Instrumentation::end_profile() {
	out_file << "]}";
	out_file.flush();
	out_file.close();
}

}

