
#include "game.hpp"

#include <real/real.hpp>
#include <imgui.h>
#include <real/graphics/mesh_renderer.hpp>

using namespace real;

EXPOSE_GAME_TO_REAL(MyGame)

void MyGame::start() {
	resource_database->load_resource_disk<ResourcePack>("../../engine/resources/resource_pack.json");

	auto graphics = scene->get_system<GraphicsSystem>();

	{
		auto viking_room = scene->create_entity("viking_room 1");
		viking_room.AddComponent<ComponentMeshRenderer>(
				resource_database->get_resource<ResourceMesh>("viking_room.obj"), 
				resource_database->get_resource<ResourceImage>("viking_room.png")
		);
		auto &transform = viking_room.GetComponent<ComponentTransform>();
		transform.rotation = Vec3(90.0f, 0.0f, 0.0f);
		transform.position = Vec3(-1.0f, 0.0f, -1.0f);
	}


	{
		auto viking_room = scene->create_entity("viking_room 2");
		viking_room.AddComponent<ComponentMeshRenderer>(
				resource_database->get_resource<ResourceMesh>("viking_room.obj"), 
				resource_database->get_resource<ResourceImage>("viking_room.png")
		);
		auto &transform = viking_room.GetComponent<ComponentTransform>();
		transform.rotation = Vec3(90.0f, 0.0f, 0.0f);
		transform.position = Vec3(-4.0f, 0.0f, -1.0f);
	}

	auto cam = scene->create_entity("camera");

	{
		auto &camera = cam.AddComponent<ComponentCamera>();
		camera.clear_color = Color4(1,0,0,1);
		auto &trans = cam.GetComponent<ComponentTransform>();
		trans.position = Vec3(0.0f, 0.0f, -3.0f);
	}

	graphics->set_main_camera(cam.handle);
}

void MyGame::update(u32 delta_time) {
	scene->update(delta_time);
}

MyGame::~MyGame() = default;

