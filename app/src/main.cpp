
#include "real/core/instance.hpp"
#include "real/core/logging.hpp"
#include "real/graphics/graphics.hpp"
#include "real/graphics/render_pass.hpp"
#include "real/graphics/window.hpp"
#include "real/resource/resource.hpp"
#include "real/resource/resource_shader.hpp"
#include <real/core/core.hpp>
#include <real/graphics/render_pass_compute.hpp>

int main(void) {
    real::print_version();

    using namespace real;
    Instance *instance = new Instance;

    /* -------- LOGGING -------- */
    instance->log.name = "game engine";
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
    ShaderLoadStruct load_shader {
        .fields={{.type=ShaderFieldType::STORAGE_IMAGE, .name="image what", .location=0}},
        .type=ShaderType::COMPUTE,
        .window=instance->window->backend_handle()
    };
    ResourceShader *shader = ResourceSerializer<ResourceSerializerType::Disk>::load<ResourceShader>(instance, "../engine/resources/shaders/gradient.comp.spv", &load_shader);
    
    RenderPassCompute *computePass = new RenderPassCompute(instance, shader);

    instance->log.info("starting");

    while (instance->update() == false) {
        auto frame = instance->renderer->start_frame();

        computePass->draw(instance->renderer, frame);

        instance->renderer->end_frame(frame);
    }

    delete computePass;
    delete shader;

    delete instance;

    Graphics::destroy_backend();
}
