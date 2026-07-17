
#include "game.hpp"
#include "real/container/color.hpp"
#include "real/container/string_hash.hpp"
#include "real/graphics/camera.hpp"
#include "real/graphics/graphics_system.hpp"
#include "real/graphics/sprite_renderer.hpp"
#include "real/math/vec3.hpp"
#include "real/resource/resource_image.hpp"
#include "real/scene/components.hpp"
#include "real/scene/entity.hpp"
#include <real/core/reflection.hpp>

#include <real/real.hpp>
#include <imgui.h>
#include <real/graphics/mesh_renderer.hpp>

using namespace real;

EXPOSE_GAME_TO_REAL(MyGame)

struct RotateObjectComponent {
	float rpt; // rotations per game tick ig
};

RL_REFLECT(RotateObjectComponent, RL_REFLECT_FIELD(rpt))

void MyGame::start() {
	auto graphics = scene->add_system<GraphicsSystem>(screen_framebuffer.get());

	{
		auto image = resource_database->get_resource<ResourceImage>("mk_16_16_nature_tileset_json-sheet.png");
		auto viking_room = scene->create_entity("square");
		viking_room.AddComponent<ComponentSpriteRenderer>(
			image, image.get()->tiles[StringHash("1")]
		);

		auto &transform = viking_room.GetComponent<ComponentTransform>();
		transform.rotation = Vec3(0.0f, 0.0f, 0.0f);
		transform.position = Vec3(0.0f, 0.0f, -1.0f);

		viking_room.AddComponent<RotateObjectComponent>(1.0f);
	}

	{
		auto image = resource_database->get_resource<ResourceImage>("mk_16_16_nature_tileset_json-sheet.png");
		auto viking_room = scene->create_entity("square1");
		viking_room.AddComponent<ComponentSpriteRenderer>(
			image, image.get()->tiles[StringHash("2")]
		);

		auto &transform = viking_room.GetComponent<ComponentTransform>();
		transform.rotation = Vec3(0.0f, 0.0f, 0.0f);
		transform.position = Vec3(0.0f, 0.5f, -1.0f);

		viking_room.AddComponent<RotateObjectComponent>(1.0f);
	}

	{
		auto entity = scene->create_entity("camera");
		entity.AddComponent<ComponentCamera>();
		entity.GetComponent<ComponentTransform>().position.z = -3;
		graphics->set_main_camera(entity);
	}
}

void MyGame::update(u32 delta_time) {
	auto view = scene->registry->view<RotateObjectComponent, ComponentTransform>();
	for(auto [ent, rotate, transform] : view.each()) {
		transform.rotation.z += rotate.rpt;
	}
}

void MyGame::shutdown() {

}

MyGame::~MyGame() = default;

