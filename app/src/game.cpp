
#include "game.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/ext/vector_float4.hpp"
#include "real/core/core.hpp"
#include "real/core/game.hpp"
#include "real/core/instance.hpp"
#include "real/core/types.hpp"
#include "real/resource/resource_handle.hpp"
#include "real/resource/resource_mesh.hpp"
#include <real/real.hpp>
#include <imgui.h>
#include <real/real.hpp>
#include <glm/glm.hpp>

using namespace real;

extern "C" {
	REALLIB_EXPORT
	Game *game_create(Shared<Instance> instance) {
		return new MyGame(instance);
	}

	REALLIB_EXPORT
	void game_destroy(Game *game) {
		delete (MyGame*)game;
	}
}

static inline glm::mat4 get_projection() {
	ImGui::Begin("mesh projection");

	static glm::vec3 rot {};
	static glm::vec3 position {0.0f, 0.0f, -3.0f};
	ImGui::InputFloat3("Rotation", &rot.x);
	ImGui::InputFloat3("Position", &position.x);

	glm::mat4 view(1.0f);
	view = glm::scale(view, glm::vec3(1.0f, 1.0f, 1.0f));
    view = glm::translate(view, position);
 	view = glm::rotate(view, glm::radians(rot.x), glm::vec3(1.0f, 0.0f, 0.0f));
 	view = glm::rotate(view, glm::radians(rot.y), glm::vec3(0.0f, 1.0f, 0.0f));
 	view = glm::rotate(view, glm::radians(rot.z), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 projection;
	float aspect = (float)1200 / 800;
	projection = glm::perspective(glm::radians(70.0f), aspect, 0.1f, 100.0f);
	projection[1][1] *= -1;

	ImGui::End();

	return projection * view;
}

void MyGame::start() {
	auto [width, height] = window->get_glfw_window_dimensions();
	
	auto shader = resource_database->register_resource(
			Resource::load<ResourceSerializerType::Disk, ResourceShader>(
				instance.get(), "../engine/resources/shaders/gradient.slang.spv"), "gradient.slang");

	auto flat_shader = resource_database->register_resource(
			Resource::load<ResourceSerializerType::Disk, ResourceShader>(
				instance.get(), "../engine/resources/shaders/flat.slang.spv"), "flat.slang");

	mesh_resource = resource_database->register_resource(
			Resource::load<ResourceSerializerType::Disk, ResourceMesh>(
				instance.get(), "../engine/resources/meshes/viking_room.obj"), "viking_room.obj");


	resource_database->register_resource(
			Resource::load<ResourceSerializerType::Disk, ResourceImage>(
		 		instance.get(), "../engine/resources/textures/viking_room.png"), "viking_room.png");

	auto renderColorImage = resource_database->get_resource<ResourceImage>("_render_color_texture");
	auto renderDepthImage = resource_database->get_resource<ResourceImage>("_render_depth_texture");

	compute_pass = Graphics::create_render_pass_compute(
			instance.get(), shader, {{renderColorImage, ImageFormat::STORAGE}}).release();


	geometry_pass = Graphics::create_render_pass_geometry(
			instance.get(), {
				.renderImage = renderColorImage,
				.depthImage = renderDepthImage
			}, { flat_shader }, {}).release();
}

void MyGame::render(real::FrameContext frame) {
	compute_pass->begin_pass(frame);
	static glm::vec4 oneCol {1.0f, 0.8f, 0.8f, 1.0f};
	static glm::vec4 twoCol {0.1f, 0.1f, 1.0f, 1.0f};
	compute_pass->set_variable("topColor", oneCol);
	compute_pass->set_variable("bottomColor", twoCol);
	compute_pass->end_pass(frame);

	geometry_pass->begin_pass(frame);	
	geometry_pass->set_variable("render_matrix", get_projection());
	geometry_pass->draw_mesh(frame, mesh_resource);
	geometry_pass->end_pass(frame);
}

void MyGame::update(u32 delta_time) {
}

MyGame::~MyGame() {
	delete compute_pass;
	delete geometry_pass;
	
	resource_database->unregister_resource("gradient.slang");
	resource_database->unregister_resource("flat.slang");
	resource_database->unregister_resource("viking_room.png");
	resource_database->unregister_resource("viking_room.obj");
}
