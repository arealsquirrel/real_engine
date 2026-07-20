
#include "game.hpp"
#include "real/container/string_hash.hpp"
#include "real/graphics/graphics_system.hpp"
#include "real/graphics/sprite_renderer.hpp"
#include "real/math/vec3.hpp"
#include "real/resource/resource_image.hpp"
#include "real/resource/resource_mesh.hpp"
#include "real/scene/components.hpp"
#include "real/scene/entity.hpp"
#include <real/core/reflection.hpp>

#include <real/real.hpp>
#include <imgui.h>
#include <real/graphics/mesh_renderer.hpp>

using namespace real;

EXPOSE_GAME_TO_REAL(MyGame)

void MyGame::start() {
	auto graphics = scene->add_system<GraphicsSystem>(screen_framebuffer.get());

	{
		auto entity = scene->create_entity("sprite");

		entity.AddComponent<ComponentSpriteRenderer>(
			resource_database->load_resource_disk<ResourceImage>("resources/mahjong/mahjong_tiles.json")
		);
		
	}

	{
		auto entity = scene->create_entity("camera");
		entity.AddComponent<ComponentCamera>();
		entity.GetComponent<ComponentTransform>().position.z = -3;
		graphics->set_main_camera(entity);
	}
}

void MyGame::update(u32 delta_time) {
}

void MyGame::shutdown() {

}

MyGame::~MyGame() = default;

