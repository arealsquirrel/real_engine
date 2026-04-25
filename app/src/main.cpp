
#include "real/core/logging.hpp"
#include <real/core/core.hpp>

int main(void) {
    real::print_version();

    using namespace real;
    Log log;
    log.sinks.push_back(new LogSink_Console(false));

    log.trace("hello {}", 5);
    log.info("hello {}", 5);
    log.warn("hello {}", 5);
    log.error("hello {}", 5);
    log.fatal("hello {}", 5);
}
