
#include "real/graphics/graphics_system.hpp"
#include "real/math/mat4.hpp"
#include "real/resource/resource_image.hpp"
#include "real/scene/components.hpp"
#include <editor_gizmos.hpp>
#include <real/real.hpp>

namespace editor {

using namespace real;

EditorGizmos::EditorGizmos(real::Ref<Instance> _instance)
	: instance(_instance) {

	sprite_renderer = instance->renderer->subrenderers.get<SpriteRenderer>();
	icon_camera = instance->resource_database->get_resource<ResourceImage>("viking_room.png");
}

EditorGizmos::~EditorGizmos() {

}

void EditorGizmos::draw_gizmos(real::Ref<real::Scene> scene) {

	{
		auto view = scene->registry->view<ComponentCamera, ComponentTransform>();
		for (auto [ent, cam, trans] : view.each()) {
			sprite_renderer->draw_sprite(
						math::translate(trans.position), icon_camera.get(), Vec2(0,0), Vec2(1,1));
		}
	}
}

}

