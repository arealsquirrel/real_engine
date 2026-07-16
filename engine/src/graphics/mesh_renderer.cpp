
#include "real/core/logging.hpp"
#include "real/graphics/framebuffer.hpp"
#include "real/graphics/renderer.hpp"
#include "real/graphics/renderpass_geometry.hpp"
#include <real/graphics/mesh_renderer.hpp>
#include <tracy/Tracy.hpp>

namespace real {

MeshRenderer::MeshRenderer(Instance *_instance, Renderer *_renderer)
	: SubRenderer(_instance, _renderer) {
    auto flat_shader = instance->resource_database->get_resource<ResourceShader>("mesh.slang.spv");
	image = instance->resource_database->get_resource<ResourceImage>("prototype_512x512_green1.png");
	queued_images.fill(image.get());

    diffuse_pass = RenderPassGeometry::create(
        instance, {
				.depth = true,
				.topology = GeometryTopology::Triangle_list,
				.polygon_mode = GeometryPolygonMode::Fill,
				.front_face = GeometryFrontFace::CounterClockwise,
				.cull_mode = GeometryCullMode::BACK,
				.msaa = MultisamplingCount::Eight,
			}, {flat_shader}, {});

	instance->resource_database->unregister_resource("mesh.slang.spv");
	image_count = 0;
}

MeshRenderer::~MeshRenderer() = default;

void MeshRenderer::destroy() {
	delete diffuse_pass.release();
}

void MeshRenderer::draw_mesh(Mat4 model,
		ResourceMesh *mesh, ResourceMesh::Mesh submesh,
		ResourceImage *texture, ShaderMode mode) {

	ZoneScoped

	auto img_itr = batched_images.find(texture->get_instance_uuid());
	if(img_itr == batched_images.end()) {
		batched_images.emplace(texture->get_instance_uuid(), image_count);
		queued_images[image_count] = texture;
		draw_commands_cmd.push_back({model, mesh, submesh, texture, image_count, (unsigned int)mode});
		image_count++;
	} else {
		draw_commands_cmd.push_back({model, mesh, submesh, texture, image_count, mode});
	}
}

void MeshRenderer::draw_commands(Framebuffer *framebuffer) {
	ZoneScoped

    diffuse_pass->begin_pass(framebuffer);
	diffuse_pass->set_variable("scene_data", renderer->scene_data->get_handle());
	diffuse_pass->set_variable_array_image("sampler", queued_images.data(), MAX_BATCH_SPRITE_COUNT);
	diffuse_pass->bind_descriptors();

	for (auto &command : draw_commands_cmd) {
		diffuse_pass->set_variable("inv_model", math::inverse(command.model));
		diffuse_pass->set_variable("shader_id", command.shader_mode);
		diffuse_pass->set_variable("texture_index", command.texture_index);
		diffuse_pass->set_variable("model", command.model);
		diffuse_pass->draw_mesh(command.mesh, command.submesh);
	}

    diffuse_pass->end_pass();
}

void MeshRenderer::flush_commands() {
	draw_commands_cmd.clear();
	batched_images.clear();
	image_count = 0;
}

}

