
#include "game.hpp"
#include "real/core/color.hpp"
#include "real/core/string_hash.hpp"
#include "real/graphics/camera.hpp"
#include "real/graphics/graphics_system.hpp"
#include "real/graphics/sprite_renderer.hpp"
#include "real/resource/resource_image.hpp"
#include "real/scene/components.hpp"
#include "real/scene/entity.hpp"

#include <real/real.hpp>
#include <imgui.h>
#include <real/graphics/mesh_renderer.hpp>

using namespace real;

EXPOSE_GAME_TO_REAL(MyGame)

struct RotateObjectComponent {
	float rpt; // rotations per game tick ig
};

Camera camera;

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
		auto viking_room = scene->create_entity("viking room");
		viking_room.AddComponent<ComponentMeshRenderer>(
				resource_database->get_resource<ResourceMesh>("viking_room.obj"),
				resource_database->get_resource<ResourceImage>("viking_room.png")
		);

		auto &transform = viking_room.GetComponent<ComponentTransform>();
		transform.rotation = Vec3(0.0f, 0.0f, 0.0f);
		transform.position = Vec3(0.0f, 0.0f, -1.0f);
	}

	camera.translate_camera(Vec3(0,0,-3));
}

void MyGame::update(u32 delta_time) {
	renderer->attach_camera(camera);
	screen_framebuffer->clear_image(Color4(1.0f, 0.0f, 0.0f, 1.0f));
	scene->update(delta_time);

	auto view = scene->registry->view<RotateObjectComponent, ComponentTransform>();
	for(auto [ent, rotate, transform] : view.each()) {
		transform.rotation.z += rotate.rpt;
	}
}

MyGame::~MyGame() = default;

