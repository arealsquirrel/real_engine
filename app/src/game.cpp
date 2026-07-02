
#include "game.hpp"

#include <real/real.hpp>
#include <imgui.h>
#include <glm/glm.hpp>
#include "glm/ext/vector_float3.hpp"
#include "real/core/game.hpp"
#include "real/core/string_hash.hpp"

using namespace real;

EXPOSE_GAME_TO_REAL(MyGame)

void MyGame::start() {
	auto graphics = scene->add_system<GraphicsSystem>(screen_framebuffer.get());

	mesh_resource = resource_database->load_resource_disk<ResourceMesh>("../engine/resources/meshes/viking_room.obj");
	mesh_texture = resource_database->load_resource_disk<ResourceImage>("../engine/resources/textures/viking_room.png");
	auto collection = resource_database->load_resource_disk<ResourceMesh>("../engine/resources/meshes/combine_meshes.obj");

	auto room = scene->create_entity("viking_room");
	room.AddComponent<ComponentMeshRenderer>(mesh_resource, mesh_texture);

	auto cube = scene->create_entity("cube");
	cube.AddComponent<ComponentMeshRenderer>(collection, mesh_texture, StringHash("Cube"));

	auto cam = scene->create_entity("camera");
	cam.AddComponent<ComponentCamera>();
	auto &trans = cam.GetComponent<ComponentTransform>();
	trans.position = glm::vec3(0.0f, 0.0f, -3.0f);

	graphics->set_main_camera(cam);
}

void MyGame::update(u32 delta_time) {
	scene->update(delta_time);
}

MyGame::~MyGame() {
}
