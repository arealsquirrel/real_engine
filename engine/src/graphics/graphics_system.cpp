
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
#include <cstring>
#include <real/graphics/render_pass_geometry.hpp>
#include <real/graphics/graphics_system.hpp>
#include <real/scene/scene.hpp>
#include <real/core/event.hpp>

namespace real {

GraphicsSystem::GraphicsSystem(Instance *_instance, Scene *_scene, Framebuffer *_framebuffer) 
    : System(_instance, _scene), 
    EventListener(_instance, this),
    framebuffer(_framebuffer) {}

GraphicsSystem::~GraphicsSystem() = default;

void GraphicsSystem::awake() {
    auto flat_shader = instance->resource_database->load_resource_disk<ResourceShader>("../engine/resources/shaders/flat.slang.spv");

    diffuse_pass = Graphics::create_render_pass_geometry(
        instance, {
				.depth = true,
				.topology = GeometryTopology::Triangle_list,
				.polygon_mode = GeometryPolygonMode::Fill,
				.front_face = GeometryFrontFace::CounterClockwise,
				.cull_mode = GeometryCullMode::BACK,
				.msaa = framebuffer->get_msaa(),
			}, 
            {flat_shader},
            {}).release();

    camera_uniform_buffer = UniformBuffer::create(instance, sizeof(CameraData)).release();
}

void GraphicsSystem::update(u32 delta_time) {
    if(main_camera == false)
        return;

    // update the main camera
    auto &trans = main_camera.GetComponent<ComponentTransform>();
    auto &camera = main_camera.GetComponent<ComponentCamera>();
    camera.camera.set_position(trans.position);
    camera.camera.set_rotation(trans.rotation);

    framebuffer->bind();
    framebuffer->clear_image(camera.clear_color);
    diffuse_pass->begin_pass(framebuffer);

    auto view = scene->registry->view<ComponentTransform, ComponentMeshRenderer>();
    for(auto [entity, trans, mesh] : view.each()) {
        *(camera_uniform_buffer->get_data<CameraData>()) = camera.camera.get_camera_data();
        glm::mat4 model = glm::mat4x4(1.0f);
        diffuse_pass->set_variable("scene_data", camera_uniform_buffer->get_handle());
        diffuse_pass->set_variable("model", trans.get_transform());
        diffuse_pass->set_variable("sampler", mesh.texture.get()->get_handle());
        diffuse_pass->bind_descriptors();
        diffuse_pass->draw_mesh(mesh.mesh);
    }

    diffuse_pass->end_pass();
    framebuffer->unbind();
}

void GraphicsSystem::destroy() {
    instance->resource_database->unregister_resource("flat.slang.spv");
    delete camera_uniform_buffer;
    delete diffuse_pass;
}

void GraphicsSystem::draw_imgui() {
	u32 micro_seconds = instance->renderer->render_stats.frame_time.micro_seconds; 
	ImGui::Text("Frametime %f ms", micro_seconds / 1000.0f);
	ImGui::Text("FPS: %f", 1000.0f / (micro_seconds / 1000.0f));
	ImGui::Text("Indices: %u", instance->renderer->render_stats.indicies);
	ImGui::Text("Vertices: %u", instance->renderer->render_stats.verticies);
	ImGui::Text("Draw framebuffer size, x: %u, y: %u", framebuffer->get_width(), framebuffer->get_height());
	ImGui::Text("Framebuffer multisampling %u", framebuffer->get_msaa());
}

}
