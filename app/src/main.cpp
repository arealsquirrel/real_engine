
#include "real/core/instance.hpp"
#include "real/core/logging.hpp"
#include "real/graphics/window.hpp"
#include "real/resource/resource.hpp"
#include <GLFW/glfw3.h>
#include <real/core/core.hpp>

int main(void) {
    real::print_version();

    using namespace real;
    Instance instance;

    /* -------- LOGGING -------- */
    instance.log.name = "engine";
    instance.log.log_level = real::LogLevel_Trace;
    instance.log.sinks.push_back(new real::LogSink_Console());

    /* -------- SETTING UP FOR INIT -------- */
    WindowInfo window_info {
        .width = 500,
        .height = 500,
        .title="hello"
    };

    instance.init(window_info);

    while (!instance.window->should_exit()) {
        glfwPollEvents();
    }
}
