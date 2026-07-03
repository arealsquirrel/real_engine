
#include "glm/ext/matrix_float4x4.hpp"
#include "imgui.h"
#include "real/core/color.hpp"
#include "real/core/event.hpp"
#include "real/core/event_listener.hpp"
#include "real/core/instance.hpp"
#include "real/core/logging.hpp"
#include "real/core/object.hpp"
#include "real/graphics/buffer.hpp"
#include "real/graphics/camera.hpp"
#include "real/graphics/framebuffer.hpp"
#include "real/graphics/graphics.hpp"
#include "real/scene/components.hpp"
#include <memory>
#include <real/graphics/renderpass_geometry.hpp>
#include <real/graphics/graphics_system.hpp>
#include <real/scene/scene.hpp>
#include <real/core/event.hpp>
#include <variant>

namespace real {

GraphicsSystem::GraphicsSystem(Instance *_instance, Scene *_scene, Framebuffer *_framebuffer) 
    : System(_instance, _scene), 
    EventListener(_instance, this),
    framebuffer(_framebuffer) {

	sub_renderers.push_back(std::make_shared<SubRendererDiffuse3D>(instance, this, scene));
}

GraphicsSystem::~GraphicsSystem() = default;

void GraphicsSystem::awake() {
    camera_uniform_buffer = UniformBuffer::create(instance, sizeof(CameraData));

	for(auto &sr : sub_renderers) {
		sr->awake();
	}

	for(auto &post : post_effects) {
		post->awake();
	}
}

void GraphicsSystem::update(u32 delta_time) {
    if(main_camera.valueless_by_exception())
        return;

	if(std::holds_alternative<EntityHandle>(main_camera)) {
		auto cam_comp = std::get<EntityHandle>(main_camera);
		auto &trans = cam_comp.GetComponent<ComponentTransform>();
		auto &camera = cam_comp.GetComponent<ComponentCamera>();
		camera.camera.set_position(trans.position);
		camera.camera.set_rotation(trans.rotation);
		*(camera_uniform_buffer->get_data<CameraData>()) = camera.camera.get_camera_data();
		framebuffer->clear_image(camera.clear_color);
	} else {
		auto cam = std::get<Shared<Camera>>(main_camera);
		*(camera_uniform_buffer->get_data<CameraData>()) = cam->get_camera_data();
		framebuffer->clear_image(Color4(1.0f, 1.0f, 1.0f, 0.0f));
	}

	for(auto &sr : sub_renderers) {
		sr->render(delta_time);
	}

    framebuffer->unbind();

	for(auto &post : post_effects) {
		post->render(delta_time);
	}
}

void GraphicsSystem::destroy() {}

void GraphicsSystem::draw_imgui() {
	ImGui::Text("Draw framebuffer size, x: %u, y: %u", framebuffer->get_width(), framebuffer->get_height());
	ImGui::Text("Framebuffer multisampling %u", framebuffer->get_msaa());
}

SubRenderer::SubRenderer(Instance *_instance, GraphicsSystem *_graphics_system, Scene *_scene) 
	: Object(_instance), graphics_system(_graphics_system), scene(_scene) {}


PostEffect::PostEffect(Instance *_instance, GraphicsSystem *_graphics_system, Scene *_scene)
	: Object(_instance), graphics_system(_graphics_system), scene(_scene) {}



void SubRendererDiffuse3D::awake() {
    auto flat_shader = instance->resource_database->load_resource_disk<ResourceShader>("../engine/resources/shaders/flat.slang.spv");

    diffuse_pass = Graphics::create_render_pass_geometry(
        instance, {
				.depth = true,
				.topology = GeometryTopology::Triangle_list,
				.polygon_mode = GeometryPolygonMode::Fill,
				.front_face = GeometryFrontFace::CounterClockwise,
				.cull_mode = GeometryCullMode::BACK,
				.msaa = graphics_system->get_framebuffer()->get_msaa(),
			}, {flat_shader}, {});

	instance->resource_database->unregister_resource("flat.slang.spv");
}

void SubRendererDiffuse3D::render(u32 deltatime) {
    diffuse_pass->begin_pass(graphics_system->get_framebuffer());
    auto view = scene->registry->view<ComponentTransform, ComponentMeshRenderer>();
    for(auto [entity, trans, mesh] : view.each()) {
        diffuse_pass->set_variable("scene_data", graphics_system->get_camera_uniform_buffer_handle());
        diffuse_pass->set_variable("model", trans.get_transform());
        diffuse_pass->set_variable("sampler", mesh.texture.get()->get_handle());
        diffuse_pass->bind_descriptors();
        diffuse_pass->draw_mesh(mesh.mesh, mesh.sub_mesh);
    }
    diffuse_pass->end_pass();
}

void SubRendererDiffuse3D::destroy() {
	delete diffuse_pass.release();
}

}
