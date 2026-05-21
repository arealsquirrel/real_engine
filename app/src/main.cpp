
#include "real/core/instance.hpp"
#include "real/core/logging.hpp"
#include "real/graphics/graphics.hpp"
#include "real/graphics/window.hpp"
#include <real/core/core.hpp>
#include <real/graphics/render_pass_compute.hpp>
#include <imgui.h>

int main(void) {
    real::print_version();

    using namespace real;
    Instance *instance = new Instance;

    /* -------- LOGGING -------- */
    instance->log.name = "game engine";
    instance->log.log_level = real::LogLevel_Trace;
    instance->log.sinks.push_back(new real::LogSink_Console());

    /* -------- STATIC ENGINE LEVEL THINGS --------- */
    Graphics::init_backend({true, "Engine"});

    /* -------- SETTING UP FOR INIT -------- */
    WindowInfo window_info {
        .width = 800,
        .height = 800,
        .title="hello"
    };

    instance->init(window_info);

    instance->log.info("starting");

    while (instance->update() == false) {
        auto frame = instance->renderer->start_frame();
        ImGui::ShowDemoWindow();
        instance->renderer->end_frame(frame);
    }

	delete instance;

    Graphics::destroy_backend();
}
