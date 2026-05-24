
#include "real/core/instance.hpp"
#include "real/resource/resource.hpp"
#include "real/resource/resource_shader.hpp"
#include <real/real.hpp>
#include <imgui.h>

using namespace real;

static void create_resources(Shared<Instance> instance) {
	auto [width, height] = instance->window->get_glfw_window_dimensions();

	auto shader = instance->resource_database->register_resource(
			Resource::load<ResourceSerializerType::Disk, ResourceShader>(
				instance.get(), "../engine/resources/shaders/gradient.slang.spv"), "gradient.comp");

	instance->log.trace("shader {}", 
			ShaderType_to_string(shader.get()->type));
}

void game_main(Shared<Instance> instance) {
	create_resources(instance);

	while(instance->update() == false) {
		auto frame = instance->renderer->start_frame();

		ImGui::ShowDemoWindow();

		instance->renderer->end_frame(frame);
	}
}

