
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float4.hpp"
#include "real/resource/resource_handle.hpp"
#include "real/resource/resource_mesh.hpp"
#include <memory>
#include <real/real.hpp>
#include <imgui.h>
#include <real/real.hpp>
#include <glm/glm.hpp>

using namespace real;

RenderPassCompute *compute_pass;
RenderPassGeometry *geometry_pass;
ResourceHandle<ResourceMesh> mesh_resource;

void Game::start() {
	auto [width, height] = window->get_glfw_window_dimensions();
	
	auto shader = resource_database->register_resource(
			Resource::load<ResourceSerializerType::Disk, ResourceShader>(
				this, "../engine/resources/shaders/gradient.slang.spv"), "gradient.slang");

	auto flat_shader = resource_database->register_resource(
			Resource::load<ResourceSerializerType::Disk, ResourceShader>(
				this, "../engine/resources/shaders/flat.slang.spv"), "flat.slang");

	mesh_resource = resource_database->register_resource(
			Resource::load<ResourceSerializerType::Disk, ResourceMesh>(
				this, "../engine/resources/meshes/monkey.obj"), "monkey");

	auto renderColorImage = resource_database->get_resource<ResourceImage>("_render_color_texture");
	auto renderDepthImage = resource_database->get_resource<ResourceImage>("_render_depth_texture");

	compute_pass = Graphics::create_render_pass_compute(
			this, shader, {{renderColorImage, ImageFormat::STORAGE}}).release();


	geometry_pass = Graphics::create_render_pass_geometry(
			this, {
				.renderImage = renderColorImage,
				.depthImage = renderDepthImage
			}, { flat_shader }, {}).release();
}

void Game::render(real::FrameContext frame) {	
	ImGui::ShowMetricsWindow();

	compute_pass->begin_pass(frame);

	ImGui::Begin("game menu");

	static glm::vec4 oneCol {1.0f, 0.0f, 0.0f, 1.0f};
	static glm::vec4 twoCol {0.0f, 0.0f, 1.0f, 1.0f};
	ImGui::ColorEdit4("col one", &oneCol.r);
	ImGui::ColorEdit4("col two", &twoCol.r);
	ImGui::End();

	compute_pass->set_variable("topColor", oneCol);
	compute_pass->set_variable("bottomColor", twoCol);
	compute_pass->end_pass(frame);



	geometry_pass->begin_pass(frame);

	// mesh_resource.get()->bind(frame);
	geometry_pass->draw_mesh(frame, mesh_resource);

	geometry_pass->end_pass(frame);
}

void Game::update(u32 delta_time) {
}

void Game::destroy() {
	delete compute_pass;
	delete geometry_pass;

	mesh_resource.unload();
}
