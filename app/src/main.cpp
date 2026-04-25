
#include "real/core/instance.hpp"
#include "real/core/logging.hpp"
#include <real/core/core.hpp>

int main(void) {
    real::print_version();

    real::Instance instance;

    /* -------- LOGGING -------- */
    instance.log.name = "engine";
    instance.log.log_level = real::LogLevel_Trace;
    instance.log.sinks.push_back(new real::LogSink_Console());

    instance.log.info("look at me!");
}
