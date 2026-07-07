
#include "real/core/string_hash.hpp"
#include "real/graphics/graphics_system.hpp"
#include "real/graphics/renderpass_geometry.hpp"
#include "real/math/mat4.hpp"
#include "real/math/quaternion.hpp"
#include "real/resource/resource_mesh.hpp"
#include "real/scene/components.hpp"
#include <real/graphics/sprite_renderer.hpp>

namespace real {

SpriteRenderer::~SpriteRenderer() = default;

void SpriteRenderer::awake() {
	mesh = ResourceMesh::create(instance, {}, nullptr, sizeof(Vertex)*1000);
    auto sprite_shader = instance->resource_database->get_resource<ResourceShader>("sprite.slang.spv");
	image = instance->resource_database->get_resource<ResourceImage>("prototype_512x512_green1.png");

	queued_images.fill(image.get());

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
	image_count = 0;
}

void SpriteRenderer::draw_sprite(
		Mat4 model, ResourceImage *texture,
		Vec2 uv0, Vec2 uv1, Color4 tint_color) {

	auto img_itr = batched_images.find(texture->get_instance_uuid());
	if(img_itr == batched_images.end()) {
		batched_images.emplace(texture->get_instance_uuid(), image_count);
		queued_images[image_count] = texture;
		draw_commands.push_back({model, uv0, uv1, tint_color, image_count, 0});
		image_count++;
	} else {
		draw_commands.push_back({model, uv0, uv1, tint_color, img_itr->second, 0});
	}
}

void SpriteRenderer::draw_sprite(Mat4 model, ResourceImage *texture, ResourceImage::Tile tile, Color4 tint_color) {
	auto [width, height] = texture->get_image_extent();
	Vec2 uv0((float)tile.position.first / width, (float)tile.position.second / height);
	Vec2 uv1(
			(float)(tile.position.first+tile.dimension.first) / width, 
			(float)(tile.position.second+tile.dimension.second) / height);

	draw_sprite(model, texture, uv0, uv1, tint_color);
}

void SpriteRenderer::draw_sprite(
		ResourceImage *image, Color4 tint_color,
		Vec3 position, Vec3 rotation, Vec3 scale) {
	
	draw_sprite(math::translate(position)*math::make_mat4_from_q(math::make_q_from_euler_angles(rotation.x, rotation.y, rotation.z))*math::scale(scale),
			image, image->tiles[StringHash("_full_image")], tint_color);
}

void SpriteRenderer::draw_sprite(ResourceImage *image, ResourceImage::Tile tile, Color4 tint_color, Vec3 position, Vec3 rotation, Vec3 scale) {
	draw_sprite(math::translate(position)*math::make_mat4_from_q(math::make_q_from_euler_angles(rotation.x, rotation.y, rotation.z))*math::scale(scale),
			image, tile, tint_color);
}

void SpriteRenderer::render(u32 deltatime) {
	auto view = scene->registry->view<ComponentTransform, ComponentSpriteRenderer>();

	for(auto [ent, trans, sprite] : view.each()) {
		draw_sprite(trans.get_transform(), sprite.texture.get(), sprite.tile, sprite.tint_color);
	}

	if(draw_commands.size() == 0)
		return;
		
	pass->begin_pass(graphics_system->get_framebuffer(), false);
	mesh->upload_vertex_data((char*)draw_commands.data(), sizeof(Vertex)*draw_commands.size());
	pass->set_variable_array_image("sampler", queued_images.data(), MAX_BATCH_SPRITE_COUNT);
	pass->set_variable("scene_data", graphics_system->get_camera_uniform_buffer_handle());
	pass->bind_descriptors();
	pass->draw(mesh.get(), 6, draw_commands.size(), 0, 0);
	pass->end_pass();
	draw_commands.clear();
	batched_images.clear();
	image_count = 0;
}

void SpriteRenderer::destroy() {
	delete pass.release();
}

}

