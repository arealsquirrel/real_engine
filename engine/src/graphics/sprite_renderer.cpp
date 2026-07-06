
#include "real/graphics/graphics_system.hpp"
#include "real/graphics/renderpass_geometry.hpp"
#include "real/resource/resource_mesh.hpp"
#include "real/scene/components.hpp"
#include <real/graphics/sprite_renderer.hpp>

namespace real {

SpriteRenderer::~SpriteRenderer() = default;

void SpriteRenderer::awake() {
	mesh = ResourceMesh::create(instance, {}, nullptr, sizeof(Vertex)*1000);
    auto sprite_shader = instance->resource_database->get_resource<ResourceShader>("sprite.slang.spv");
	image = instance->resource_database->get_resource<ResourceImage>("prototype_512x512_green1.png");

	pass = RenderPassGeometry::create(instance,
			{
				.depth = true,
				.topology = GeometryTopology::Triangle_list,
				.polygon_mode = GeometryPolygonMode::Fill,
				.front_face = GeometryFrontFace::CounterClockwise,
				.cull_mode = GeometryCullMode::BACK,
				.msaa = graphics_system->get_framebuffer()->get_msaa()
			}, {sprite_shader}, {});

	instance->resource_database->unregister_resource("sprite.slang.spv");
}

void SpriteRenderer::render(u32 deltatime) {
	auto view = scene->registry->view<ComponentTransform, ComponentSpriteRenderer>();

	for(auto [ent, trans, sprite] : view.each()) {
		auto tile = sprite.tile;
		auto [width, height] = sprite.texture.get()->get_image_extent();
		Vec2 uv0((float)tile.position.first / width, (float)tile.position.second / height);
		Vec2 uv1(
				(float)(tile.position.first+tile.dimension.first) / width, 
				(float)(tile.position.second+tile.dimension.second) / height);
		draw_commands.push_back({trans.get_transform(), uv0, uv1, sprite.tint_color});
		batched_images.emplace(sprite.texture.get_uuid(), queued_images.size());
		queued_images.push_back(sprite.texture.get());
	}

	if(draw_commands.size() == 0)
		return;
		
	pass->begin_pass(graphics_system->get_framebuffer(), false);
	mesh->upload_vertex_data((char*)draw_commands.data(), sizeof(Vertex)*draw_commands.size());
	pass->set_variable("sampler", queued_images[0]->get_handle());
    pass->set_variable("scene_data", graphics_system->get_camera_uniform_buffer_handle());
	pass->bind_descriptors();
	pass->draw(mesh.get(), 6, draw_commands.size(), 0, 0);
	pass->end_pass();

	draw_commands.clear();
}

void SpriteRenderer::destroy() {
	delete pass.release();
}

}

