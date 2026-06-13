
#include "game.hpp"

#include <real/real.hpp>
#include <imgui.h>
#include <glm/glm.hpp>
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "real/core/object.hpp"
#include "real/debug/cvars.hpp"
#include "real/graphics/buffer.hpp"
#include "real/resource/resource_image.hpp"

using namespace real;

struct SceneData {
	alignas(16) glm::mat4 projection;
	alignas(16) glm::mat4 view;
	alignas(16) float ambient;
	alignas(16) glm::vec3 light_color;
	alignas(16) glm::vec3 light_position;
};

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

void MyGame::start() {
	auto [width, height] = window->get_glfw_window_dimensions();
	
	auto shader = resource_database->load_resource_disk<ResourceShader>("../engine/resources/shaders/gradient.slang.spv");
	auto flat_shader = resource_database->load_resource_disk<ResourceShader>("../engine/resources/shaders/flat.slang.spv");
	mesh_resource = resource_database->load_resource_disk<ResourceMesh>("../engine/resources/meshes/viking_room.obj");
	mesh_texture = resource_database->load_resource_disk<ResourceImage>("../engine/resources/textures/viking_room.png");

	render_texture = resource_database->get_resource<ResourceImage>("_screen_color_texture");
	auto depth_texture = resource_database->get_resource<ResourceImage>("_screen_depth_texture");
	auto resolve_texture = resource_database->get_resource<ResourceImage>("_screen_color_resolve_texture");

	compute_pass = Graphics::create_render_pass_compute(
			instance.get(), shader, {{resolve_texture, ImageFormat::RENDER_ATTACHMENT_COLOR}}).release();

	geometry_pass = Graphics::create_render_pass_geometry(
			instance.get(), {
				.renderImage = render_texture,
				.depthImage = depth_texture,
				.topology = GeometryTopology::Triangle_list,
				.polygon_mode = GeometryPolygonMode::Fill,
				.front_face = GeometryFrontFace::CounterClockwise,
				.cull_mode = GeometryCullMode::BACK
			}, { flat_shader }, {}).release();

	buffer = UniformBuffer::create(instance.get(), sizeof(SceneData)).release();
}

void MyGame::update(u32 delta_time) {
	static float aspect = (float)1200 / 800;
	camera_projection = glm::perspective(glm::radians(pov->get_value()), aspect, 0.1f, 100.0f);
	camera_view = glm::mat4x4(1.0f);
	camera_view = glm::translate(camera_view, glm::vec3(0.0f, 0.0f, -4.0f));

	geometry_pass->begin_pass();
	SceneData *scene_data = buffer->get_data<SceneData>();
	scene_data->projection = camera_projection;
	scene_data->view = camera_view;
	model = glm::mat4x4(1.0f);
 	model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
	geometry_pass->set_variable("scene_data", buffer->get_handle());
	geometry_pass->set_variable("model", model);
	geometry_pass->set_variable("sampler", mesh_texture.get()->get_handle());
	geometry_pass->bind_descriptors();
	geometry_pass->draw_mesh(mesh_resource);
	geometry_pass->end_pass();

	instance->renderer->resolve_frame();

	compute_pass->begin_pass();
	compute_pass->set_variable("topColor", topGradientColor->get_value());
	compute_pass->set_variable("bottomColor", bottomGradientColor->get_value());
	compute_pass->bind_descriptors();
	compute_pass->dispatch(std::ceil(render_texture.get()->get_image_extent().first / 16.0), std::ceil(render_texture.get()->get_image_extent().second / 16.0), 1);
	compute_pass->end_pass();
}

MyGame::~MyGame() {
	delete compute_pass;
	delete geometry_pass;
	delete buffer;
	
	resource_database->unregister_resource("gradient.slang.spv");
	resource_database->unregister_resource("flat.slang.spv");
	resource_database->unregister_resource("viking_room.png");
	resource_database->unregister_resource("viking_room.obj");
}
