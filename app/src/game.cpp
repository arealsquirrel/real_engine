
#include "glm/ext/vector_float4.hpp"
#include "real/core/instance.hpp"
#include "real/resource/resource.hpp"
#include "real/resource/resource_shader.hpp"
#include <memory>
#include <real/real.hpp>
#include <imgui.h>
#include "game.hpp"
#include <glm/glm.hpp>

using namespace real;

Unique<Game> game_entrypoint(Shared<Instance> instance) {
	return std::make_unique<AppGame>(instance);
}

AppGame::~AppGame() {

}

void AppGame::start() {
	auto [width, height] = instance->window->get_glfw_window_dimensions();
	
	auto shader = instance->resource_database->register_resource(
			Resource::load<ResourceSerializerType::Disk, ResourceShader>(
				instance.get(), "../engine/resources/shaders/gradient.slang.spv"), "gradient.slang");


	auto renderImage = instance->resource_database->get_resource<ResourceImage>("_render_texture");
	compute_pass = Graphics::create_render_pass_compute(
			instance.get(),
			shader,
			{{renderImage, ImageFormat::STORAGE}});

	auto flat_shader = instance->resource_database->register_resource(
			Resource::load<ResourceSerializerType::Disk, ResourceShader>(
				instance.get(), "../engine/resources/shaders/flat.slang.spv"), "flat.slang");

	auto triangle_shader = instance->resource_database->register_resource(
			Resource::load<ResourceSerializerType::Disk, ResourceShader>(
				instance.get(), "../engine/resources/shaders/test_triangle.slang.spv"), "test_triangle.slang");
	geometry_pass = Graphics::create_render_pass_geometry(
			instance.get(), {},
			{ flat_shader },
			{{renderImage, ImageFormat::COLOR}});
}

void AppGame::render(real::FrameContext frame) {	
	compute_pass->begin_pass(frame);

	ImGui::Begin("game menu");

	static glm::vec4 oneCol {0.0f, 0.0f, 0.0f, 1.0f};
	static glm::vec4 twoCol {0.0f, 0.0f, 0.0f, 1.0f};

	ImGui::ColorEdit4("col one", &oneCol.r);
	ImGui::ColorEdit4("col two", &twoCol.r);

	ImGui::End();

	compute_pass->set_variable("topColor", oneCol);
	compute_pass->set_variable("bottomColor", twoCol);
	compute_pass->end_pass(frame);

	geometry_pass->begin_pass(frame);
	geometry_pass->end_pass(frame);
}

void AppGame::update(u32 delta_time) {
}

