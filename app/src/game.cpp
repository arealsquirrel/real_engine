
#include "game.hpp"

#include <real/real.hpp>
#include <imgui.h>
#include <glm/glm.hpp>
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/trigonometric.hpp"
#include "real/core/object.hpp"
#include "real/debug/cvars.hpp"
#include "real/graphics/buffer.hpp"
#include "real/graphics/camera.hpp"
#include "real/graphics/render_pass_geometry.hpp"
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

void MyGame::start() {
	auto [width, height] = window->get_glfw_window_dimensions();
	
	auto shader = resource_database->load_resource_disk<ResourceShader>("../engine/resources/shaders/gradient.slang.spv");
	auto flat_shader = resource_database->load_resource_disk<ResourceShader>("../engine/resources/shaders/flat.slang.spv");
	mesh_resource = resource_database->load_resource_disk<ResourceMesh>("../engine/resources/meshes/viking_room.obj");
	mesh_texture = resource_database->load_resource_disk<ResourceImage>("../engine/resources/textures/viking_room.png");

	compute_pass = Graphics::create_render_pass_compute(
			instance.get(), shader, {{screen_framebuffer->get_color_resolve_image(), ImageFormat::RENDER_ATTACHMENT_COLOR}}).release();
	
	geometry_pass = Graphics::create_render_pass_geometry(
			instance.get(), {
				.depth = true,
				.topology = GeometryTopology::Triangle_list,
				.polygon_mode = GeometryPolygonMode::Fill,
				.front_face = GeometryFrontFace::CounterClockwise,
				.cull_mode = GeometryCullMode::BACK,
				.msaa = MultisamplingCount::Eight,
			}, { flat_shader }, {}).release();

	buffer = UniformBuffer::create(instance.get(), sizeof(CameraData)).release();
	camera.translate_camera(glm::vec3(0.0f, 0.0f, -3.0f));
	camera.viewport(1200, 800);
}

void MyGame::update(u32 delta_time) {
	screen_framebuffer->bind();
	screen_framebuffer->clear_image();

	geometry_pass->begin_pass(screen_framebuffer.get());
	*(buffer->get_data<CameraData>()) = camera.get_camera_data();
	model = glm::mat4x4(1.0f);
 	model = glm::rotate(model, glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
 	model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
	geometry_pass->set_variable("scene_data", buffer->get_handle());
	geometry_pass->set_variable("model", model);
	geometry_pass->set_variable("sampler", mesh_texture.get()->get_handle());
	geometry_pass->bind_descriptors();
	geometry_pass->draw_mesh(mesh_resource);
	geometry_pass->end_pass();

	screen_framebuffer->unbind();
	
	compute_pass->begin_pass();
	compute_pass->set_variable("topColor", topGradientColor->get_value());
	compute_pass->set_variable("bottomColor", bottomGradientColor->get_value());
	compute_pass->bind_descriptors();
	compute_pass->dispatch(std::ceil(1200 / 16.0), std::ceil(800 / 16.0), 1);
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
