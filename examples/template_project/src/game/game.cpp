
#include "game.hpp"

#include <real/real.hpp>
#include <imgui.h>
#include <glm/glm.hpp>
#include "glm/ext/vector_float3.hpp"
#include "real/graphics/sprite_renderer.hpp"

using namespace real;

EXPOSE_GAME_TO_REAL(MyGame)

void MyGame::start() {
	resource_database->load_resource_disk<ResourcePack>("../../engine/resources/resource_pack.json");

	auto graphics = scene->get_system<GraphicsSystem>();

	{
		auto viking_room = scene->create_entity("viking_room");
		viking_room.AddComponent<ComponentMeshRenderer>(
				resource_database->get_resource<ResourceMesh>("viking_room.obj"), 
				resource_database->get_resource<ResourceImage>("viking_room.png")
		);
		auto &transform = viking_room.GetComponent<ComponentTransform>();
		transform.rotation = glm::vec3(1.53f, 0.0f, 0.0f);
		transform.position = glm::vec3(1.0f, 0.0f, 0.0f);
	}

	{
		auto entity = scene->create_entity("square");
		entity.AddComponent<ComponentSpriteRenderer>(
				resource_database->get_resource<ResourceImage>("mk_16_16_nature_tileset_json-sheet.png"));
		auto &transform = entity.GetComponent<ComponentTransform>();
	}

	auto cam = scene->create_entity("camera");

	{
		auto &camera = cam.AddComponent<ComponentCamera>();
		camera.clear_color = Color4(1,0,0,1);
		auto &trans = cam.GetComponent<ComponentTransform>();
		trans.position = glm::vec3(0.0f, 0.0f, -3.0f);
	}

	graphics->set_main_camera(cam);
}

void MyGame::update(u32 delta_time) {
	scene->update(delta_time);
}

MyGame::~MyGame() = default;

