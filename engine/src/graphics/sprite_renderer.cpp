
#include "real/core/logging.hpp"
#include "real/container/string_hash.hpp"
#include "real/graphics/graphics_system.hpp"
#include "real/graphics/renderpass_geometry.hpp"
#include "real/math/mat4.hpp"
#include "real/math/quaternion.hpp"
#include "real/resource/resource_mesh.hpp"
#include <real/graphics/sprite_renderer.hpp>
#include <tracy/Tracy.hpp>

namespace real {

SpriteRenderer::SpriteRenderer(Instance *_instance, Renderer *_renderer)
	: SubRenderer(_instance, _renderer) {

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
				.msaa = MultisamplingCount::Eight
			}, {sprite_shader}, {});

	instance->resource_database->unregister_resource("sprite.slang.spv");
	image_count = 0;
}

SpriteRenderer::~SpriteRenderer() = default;

void SpriteRenderer::destroy() {
	delete pass.release();
}

void SpriteRenderer::draw_sprite(
		Mat4 model, ResourceImage *texture,
		Vec2 uv0, Vec2 uv1, Color4 tint_color) {

	ZoneScoped

	auto img_itr = batched_images.find(texture->get_instance_uuid());
	if(img_itr == batched_images.end()) {
		batched_images.emplace(texture->get_instance_uuid(), image_count);
		queued_images[image_count] = texture;
		draw_commands_vec.push_back({model, uv0, uv1, tint_color, image_count, 0});
		image_count++;
	} else {
		draw_commands_vec.push_back({model, uv0, uv1, tint_color, img_itr->second, 0});
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

void SpriteRenderer::draw_commands(Framebuffer *framebuffer) {
	ZoneScoped

	if(draw_commands_vec.size() == 0)
		return;
		
	pass->begin_pass(framebuffer, false);
	mesh->upload_vertex_data((char*)draw_commands_vec.data(), sizeof(Vertex)*draw_commands_vec.size());
	pass->set_variable_array_image("sampler", queued_images.data(), MAX_BATCH_SPRITE_COUNT);
	pass->set_variable("scene_data", renderer->scene_data->get_handle());
	pass->bind_descriptors();
	pass->draw(mesh.get(), 6, draw_commands_vec.size(), 0, 0);
	pass->end_pass();
}

void SpriteRenderer::flush_commands() {
	draw_commands_vec.clear();
	batched_images.clear();
	image_count = 0;
}

}

