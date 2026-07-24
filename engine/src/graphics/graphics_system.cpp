
#include "imgui.h"
#include "real/core/event_listener.hpp"
#include "real/core/instance.hpp"
#include "real/core/logging.hpp"
#include "real/core/object.hpp"
#include "real/graphics/buffer.hpp"
#include "real/graphics/framebuffer.hpp"
#include <real/graphics/renderpass_geometry.hpp>
#include <real/graphics/graphics_system.hpp>
#include <real/scene/scene.hpp>
#include <real/core/event.hpp>
#include <tracy/Tracy.hpp>
#include "real/graphics/mesh_renderer.hpp"
#include "real/graphics/renderer.hpp"
#include "real/graphics/sprite_renderer.hpp"
#include "real/scene/components.hpp"
#include "real/scene/entity.hpp"

namespace real {

GraphicsSystem::GraphicsSystem(Instance *_instance, Scene *_scene, Framebuffer *_framebuffer) 
    : System(_instance, _scene), framebuffer(_framebuffer), 
    EventListener(_instance, this) {

	scene_data = UniformBuffer::create(instance, sizeof(SceneData));
	SceneData *data = scene_data->get_data<SceneData>();
	data->ambient_power = 0.5;
	data->light_color = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
	data->light_position = Vec4(0.0f, 0.0f, 0.0f, 0.0f);
	data->view_position = Vec3(0.0f, 0.0f, 0.0f);
}

GraphicsSystem::~GraphicsSystem() {
	scene_data.destroy();
}

void GraphicsSystem::awake() {
	mesh_renderer = instance->renderer->subrenderers.get<MeshRenderer>();
	sprite_renderer = instance->renderer->subrenderers.get<SpriteRenderer>();
}

void GraphicsSystem::update(u32 delta_time) {

	ZoneScoped

	framebuffer->bind();

	{
		auto view = scene->registry->view<ComponentCamera, ComponentTransform>();
		for (auto [ent, camera, trans] : view.each()) {
			camera.camera.set_position(trans.position);
		}
	}

	if(bind_main_camera) {
		auto &cam = main_camera_entity.GetComponent<ComponentCamera>();
		SceneData *data = scene_data->get_data<SceneData>();
		data->proj = cam.camera.proj;
		data->view = cam.camera.view;
		data->view_position = main_camera_entity.GetComponent<ComponentTransform>().position;
		framebuffer->clear_image(cam.clear_color);
	} else {
		framebuffer->clear_image();
	}

	{
		auto view = scene->registry->view<ComponentMeshRenderer, ComponentTransform>();
		for (auto [ent, mesh_comp, trans] : view.each())
			mesh_renderer->draw_mesh(
					trans.get_transform(), mesh_comp.mesh.get(),
					mesh_comp.sub_mesh, mesh_comp.texture.get()->texture.get(), mesh_comp.shader_mode);
		mesh_renderer->draw_commands(framebuffer, scene_data.get());
		mesh_renderer->flush_commands();
	}

	{
		auto view = scene->registry->view<ComponentSpriteRenderer, ComponentTransform>();
		for (auto [ent, sprite, trans] : view.each())
			sprite_renderer->draw_sprite(
					trans.get_transform(), sprite.texture.get()->texture.get());
		sprite_renderer->draw_commands(framebuffer, scene_data.get());
		sprite_renderer->flush_commands();
	}	

	framebuffer->unbind();
}

void GraphicsSystem::destroy() {}

void GraphicsSystem::draw_imgui() {
	SceneData *data = scene_data->get_data<SceneData>();
	ImGui::DragFloat("ambient power", &data->ambient_power);
	ImGui::ColorEdit3("light color", &data->light_color.x);
	ImGui::InputFloat3("light position", &data->light_position.x);
}

}
