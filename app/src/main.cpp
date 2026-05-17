
#include "real/core/instance.hpp"
#include "real/core/logging.hpp"
#include "real/graphics/graphics.hpp"
#include "real/graphics/window.hpp"
#include "real/resource/resource.hpp"
#include "real/resource/resource_shader.hpp"
#include <real/core/core.hpp>

int main(void) {
    real::print_version();

    using namespace real;
    Instance *instance = new Instance;

    /* -------- LOGGING -------- */
    instance->log.name = "engine";
    instance->log.log_level = real::LogLevel_Trace;
    instance->log.sinks.push_back(new real::LogSink_Console());

    /* -------- STATIC ENGINE LEVEL THINGS --------- */
    Graphics::init_backend({});

    /* -------- SETTING UP FOR INIT -------- */
    WindowInfo window_info {
        .width = 500,
        .height = 500,
        .title="hello"
    };

    instance->init(window_info);

    // holy fuck
    ResourceShader *shader = ResourceSerializer<ResourceSerializerType::Disk>::load<ResourceShader>(instance, "../engine/resources/shaders/gradient.comp.spv", instance->window->backend_handle());

    while (instance->update() == false) {
        
    }

    delete shader;

    delete instance;

    Graphics::destroy_backend();
}
