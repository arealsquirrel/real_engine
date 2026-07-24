
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
		auto entity = scene->create_entity("awesomeness");

		entity.AddComponent<ComponentMeshRenderer>(
			resource_database->load_resource_disk<ResourceMesh>("resources/models/sponza.obj"),
			resource_database->get_resource<ResourceImage>("prototype_512x512_green1.png")
		);

		auto &trans = entity.GetComponent<ComponentTransform>();
		trans.scale = Vec3(0.01, -0.01, 0.01);
		trans.position = Vec3(12, 2, -2.5);
	}

	{
		auto entity = scene->create_entity("camera");
		entity.AddComponent<ComponentCamera>();
		auto &trans = entity.GetComponent<ComponentTransform>();
		trans.position = Vec3(0, 0, -3);
		trans.rotation = Vec3(0, 90, 0);
		graphics->main_camera_entity = entity;
	}
}

void MyGame::update(u32 delta_time) {
}

void MyGame::shutdown() {

}

MyGame::~MyGame() = default;

