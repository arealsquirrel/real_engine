
#include "game.hpp"

#include <real/real.hpp>
#include <imgui.h>
#include <glm/glm.hpp>
#include "glm/ext/vector_float3.hpp"
#include "real/core/logging.hpp"
#include "real/core/object.hpp"
#include "real/graphics/graphics_system.hpp"
#include "real/resource/resource_image.hpp"
#include "real/scene/components.hpp"
#include <real/scene/scene.hpp>
#include <real/scene/entity.hpp>

using namespace real;

extern "C" {
	REALLIB_EXPORT
	Game *game_create(Shared<Instance> instance, ArgParams params) {
		return new MyGame(instance, params);
	}

	REALLIB_EXPORT
	void game_destroy(Game *game) {
		delete (MyGame*)game;
	}
}

void MyGame::start() {
	auto graphics = scene->add_system<GraphicsSystem>(screen_framebuffer.get());

	mesh_resource = resource_database->load_resource_disk<ResourceMesh>("../engine/resources/meshes/viking_room.obj");
	mesh_texture = resource_database->load_resource_disk<ResourceImage>("../engine/resources/textures/viking_room.png");

	auto room = scene->create_entity("viking_room");
	room.AddComponent<ComponentMeshRenderer>(mesh_resource, mesh_texture);

	auto cam = scene->create_entity("camera");
	cam.AddComponent<ComponentCamera>();
	auto &trans = cam.GetComponent<ComponentTransform>();
	trans.position = glm::vec3(0.0f, 0.0f, -3.0f);

	graphics->set_main_camera(cam);
}

void MyGame::update(u32 delta_time) {
	auto [x, y] = window->input->poll_mouse_pos();
	RL_LOG_INFO("mouse pos {} {}", x ,y);

	scene->update(delta_time);
}

MyGame::~MyGame() {
	resource_database->unregister_resource("viking_room.png");
	resource_database->unregister_resource("viking_room.obj");
}
