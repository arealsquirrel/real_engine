
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
	icon_camera = instance->resource_database->get_resource<ResourceImage>("camera.png");
	icon_sun_light = instance->resource_database->get_resource<ResourceImage>("sun_light.png");
}

EditorGizmos::~EditorGizmos() {

}

void EditorGizmos::draw_gizmos(real::Ref<real::Scene> scene) {
	{
		auto view = scene->registry->view<ComponentCamera, ComponentTransform>();
		for (auto [ent, cam, trans] : view.each()) {
			Mat4 model = math::translate(trans.position)*math::make_mat4_from_q(math::make_q_from_euler_angles(trans.rotation.x, trans.rotation.y-90, trans.rotation.z))*math::scale(Vec3(.5,.5,.5));
			sprite_renderer->draw_sprite(model, icon_camera.get());
		}
	}

	Ref<GraphicsSystem> graphics_system = scene->get_system<GraphicsSystem>();
	Vec4 lpos = graphics_system->scene_data->get_data<real::SceneData>()->light_position;
	sprite_renderer->draw_sprite(math::translate(Vec3(lpos.x, lpos.y, lpos.z)), icon_sun_light.get(), Vec2(0,0), Vec2(1,1));
}

}

