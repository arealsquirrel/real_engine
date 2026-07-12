
#include "real/core/event_listener.hpp"
#include "real/core/instance.hpp"
#include "real/core/logging.hpp"
#include "real/core/object.hpp"
#include "real/graphics/framebuffer.hpp"
#include <real/graphics/renderpass_geometry.hpp>
#include <real/graphics/graphics_system.hpp>
#include <real/scene/scene.hpp>
#include <real/core/event.hpp>
#include "real/graphics/mesh_renderer.hpp"
#include "real/graphics/sprite_renderer.hpp"
#include "real/scene/components.hpp"
#include "real/scene/entity.hpp"

namespace real {

GraphicsSystem::GraphicsSystem(Instance *_instance, Scene *_scene, Framebuffer *_framebuffer) 
    : System(_instance, _scene), framebuffer(_framebuffer), 
    EventListener(_instance, this) {}

GraphicsSystem::~GraphicsSystem() = default;

void GraphicsSystem::set_main_camera(EntityHandle entity) {
	auto &cam_comp = entity.GetComponent<ComponentCamera>();
	// main_camera = cam_comp.camera;
}

void GraphicsSystem::awake() {
	mesh_renderer = instance->renderer->subrenderers.get<MeshRenderer>();
	sprite_renderer = instance->renderer->subrenderers.get<SpriteRenderer>();
}

void GraphicsSystem::update(u32 delta_time) {

	// instance->renderer->attach_camera(main_camera);
	// framebuffer->clear_image(main_camera->clear_color);

	{
	auto view = scene->registry->view<ComponentMeshRenderer, ComponentTransform>();
	for (auto [ent, mesh_comp, trans] : view.each()) {
		mesh_renderer->draw_mesh(trans.get_transform(), mesh_comp.mesh.get(), mesh_comp.sub_mesh, mesh_comp.texture.get());
	}
	}

	{
	auto view = scene->registry->view<ComponentSpriteRenderer, ComponentTransform>();
	for (auto [ent, sprite, trans] : view.each()) {
		sprite_renderer->draw_sprite(trans.get_transform(), sprite.texture.get(), sprite.tile, sprite.tint_color);
	}
	}	

	framebuffer->bind();
	mesh_renderer->draw_commands(framebuffer);
	mesh_renderer->flush_commands();

	sprite_renderer->draw_commands(framebuffer);
	sprite_renderer->flush_commands();
	framebuffer->unbind();
}

void GraphicsSystem::destroy() {}

void GraphicsSystem::draw_imgui() {
}

}
