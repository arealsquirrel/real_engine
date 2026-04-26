
#include "real/core/instance.hpp"
#include "real/core/logging.hpp"
#include "real/resource/resource.hpp"
#include <real/core/core.hpp>

int main(void) {
    real::print_version();

    using namespace real;
    Instance instance;

    /* -------- LOGGING -------- */
    instance.log.name = "engine";
    instance.log.log_level = real::LogLevel_Trace;
    instance.log.sinks.push_back(new real::LogSink_Console());

    instance.log.info("look at me!");
}
