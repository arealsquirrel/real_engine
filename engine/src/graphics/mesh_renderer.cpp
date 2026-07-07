
#include <real/graphics/mesh_renderer.hpp>

namespace real {


MeshRenderer::~MeshRenderer() {

}

void MeshRenderer::draw_mesh(Mat4 model,
		ResourceMesh *mesh, ResourceMesh::Mesh submesh,
		ResourceImage *texture, ShaderMode mode) {

	auto img_itr = batched_images.find(texture->get_instance_uuid());
	if(img_itr == batched_images.end()) {
		batched_images.emplace(texture->get_instance_uuid(), image_count);
		queued_images[image_count] = texture;
		draw_commands.push_back({model, mesh, submesh, texture, image_count, (unsigned int)mode});
		image_count++;
	} else {
		draw_commands.push_back({model, mesh, submesh, texture, image_count, mode});
	}
}

void MeshRenderer::awake() {
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
				.msaa = graphics_system->get_framebuffer()->get_msaa(),
			}, {flat_shader}, {});

	instance->resource_database->unregister_resource("mesh.slang.spv");
	image_count = 0;
}

void MeshRenderer::render(u32 deltatime) {

    auto view = scene->registry->view<ComponentTransform, ComponentMeshRenderer>();
    for(auto [entity, trans, mesh] : view.each()) {
		draw_mesh(trans.get_transform(), mesh.mesh.get(), mesh.sub_mesh, mesh.texture.get(), mesh.shader_mode);
    }

    diffuse_pass->begin_pass(graphics_system->get_framebuffer());
	diffuse_pass->set_variable("scene_data", graphics_system->get_camera_uniform_buffer_handle());
	diffuse_pass->set_variable_array_image("sampler", queued_images.data(), MAX_BATCH_SPRITE_COUNT);
	diffuse_pass->bind_descriptors();

	for (auto &command : draw_commands) {
		diffuse_pass->set_variable("inv_model", math::inverse(command.model));
		diffuse_pass->set_variable("shader_id", command.shader_mode);
		diffuse_pass->set_variable("texture_index", command.texture_index);
		diffuse_pass->set_variable("model", command.model);
		diffuse_pass->draw_mesh(command.mesh, command.submesh);
	}

    diffuse_pass->end_pass();

	draw_commands.clear();
	batched_images.clear();
	image_count = 0;
}

void MeshRenderer::destroy() {
	delete diffuse_pass.release();
}

}

