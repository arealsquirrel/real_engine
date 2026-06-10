
#include "game.hpp"

#include <real/real.hpp>
#include <imgui.h>
#include <glm/glm.hpp>
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "real/resource/resource_image.hpp"

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
	static glm::vec3 rot {};
	static glm::vec3 position {0.0f, 0.0f, -3.0f};
	static float aspect = (float)1200 / 800;

	glm::mat4 view(1.0f);
    view = glm::translate(view, position);
 	view = glm::rotate(view, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 projection;
	projection = glm::perspective(glm::radians(70.0f), aspect, 0.1f, 100.0f);
	projection[1][1] *= -1;

	return projection * view;
}

void MyGame::start() {
	auto [width, height] = window->get_glfw_window_dimensions();
	
	auto shader = resource_database->load_resource_disk<ResourceShader>("../engine/resources/shaders/gradient.slang.spv");
	auto flat_shader = resource_database->load_resource_disk<ResourceShader>("../engine/resources/shaders/flat.slang.spv");
	mesh_resource = resource_database->load_resource_disk<ResourceMesh>("../engine/resources/meshes/viking_room.obj");
	mesh_texture = resource_database->load_resource_disk<ResourceImage>("../engine/resources/textures/viking_room.png");

	auto render_texture = resource_database->get_resource<ResourceImage>("_screen_color_texture");
	auto depth_texture = resource_database->get_resource<ResourceImage>("_screen_depth_texture");

	compute_pass = Graphics::create_render_pass_compute(
			instance.get(), shader, {{render_texture, ImageFormat::RENDER_ATTACHMENT_COLOR}}).release();

	geometry_pass = Graphics::create_render_pass_geometry(
			instance.get(), {
				.renderImage = render_texture,
				.depthImage = depth_texture,
				.topology = GeometryTopology::Triangle_list,
				.polygon_mode = GeometryPolygonMode::Fill,
				.front_face = GeometryFrontFace::CounterClockwise,
				.cull_mode = GeometryCullMode::BACK
			}, { flat_shader }, {}).release();
}

void MyGame::update(u32 delta_time) {
	compute_pass->begin_pass();

	static glm::vec4 oneCol {1.0f, 0.8f, 0.8f, 1.0f};
	static glm::vec4 twoCol {0.1f, 0.1f, 1.0f, 1.0f};
	compute_pass->set_variable("topColor", oneCol);
	compute_pass->set_variable("bottomColor", twoCol);
	compute_pass->end_pass();

	geometry_pass->begin_pass();
	geometry_pass->set_variable("sampler", mesh_texture.get()->get_handle());
	geometry_pass->set_variable("render_matrix", get_projection());
	geometry_pass->draw_mesh(mesh_resource);

	
	geometry_pass->end_pass();
}

MyGame::~MyGame() {
	delete compute_pass;
	delete geometry_pass;
	
	resource_database->unregister_resource("gradient.slang.spv");
	resource_database->unregister_resource("flat.slang.spv");
	resource_database->unregister_resource("viking_room.png");
	resource_database->unregister_resource("viking_room.obj");
}
