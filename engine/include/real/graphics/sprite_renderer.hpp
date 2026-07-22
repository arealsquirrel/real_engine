#ifndef REALLIB_SPRITE_RENDERER_HPP
#define REALLIB_SPRITE_RENDERER_HPP

#include "real/container/color.hpp"
#include "real/container/ref.hpp"
#include "real/core/core.hpp"
#include "real/core/object.hpp"
#include "real/container/string_hash.hpp"
#include "real/core/types.hpp"
#include "real/core/uuid.hpp"
#include "real/graphics/buffer.hpp"
#include "real/graphics/graphics_system.hpp"
#include "real/graphics/renderpass_geometry.hpp"
#include "real/math/mat4.hpp"
#include "real/math/vec2.hpp"
#include "real/math/vec3.hpp"
#include "real/resource/resource_handle.hpp"
#include "real/resource/resource_image.hpp"
#include "real/resource/resource_mesh.hpp"
#include <array>
#include <map>
#include <vector>

namespace real {

class REALLIB_EXPORT ComponentSpriteRenderer {
public:
	ComponentSpriteRenderer(ResourceHandle<ResourceImage> _texture, ResourceImage::Tile _tile, Color4 _tint_color={1.0f,1.0f,1.0f,1.0f})
		: texture(_texture), tile(_tile), tint_color(_tint_color) {}

	ComponentSpriteRenderer(ResourceHandle<ResourceImage> _texture, Color4 _tint_color={1.0f, 1.0f, 1.0f, 1.0f})
		: texture(_texture), tile(_texture.get()->tiles[StringHash("_full_image")]), tint_color(_tint_color) {}

	ComponentSpriteRenderer(const ComponentSpriteRenderer &renderer) = default; 
	~ComponentSpriteRenderer() = default;

	ResourceHandle<ResourceImage> texture;
	ResourceImage::Tile tile;
	Color4 tint_color;
};
	
class REALLIB_EXPORT SpriteRenderer : public SubRenderer {
RL_OBJECT(SpriteRenderer, SubRenderer)

public:
	static constexpr u32 MAX_BATCH_SPRITE_COUNT = 32;

public:
	struct Vertex {
		Mat4 transform;
		Vec2 uv0;
		Vec2 uv1;
		Color4 color;
		uint texture_id;
		uint shader_id;
	};

public:
	SpriteRenderer(Instance *_instance, Renderer *_renderer);
	~SpriteRenderer();

	void draw_commands(Framebuffer *framebuffer) final override;
	void flush_commands() final override;
	void destroy() final override;

	void draw_sprite(Mat4 model, ResourceImage *image, Vec2 uv0={0.0f, 0.0f}, Vec2 uv1={1.0f, 1.0f}, Color4 tint_color={1,1,1,1});
	void draw_sprite(Mat4 model, ResourceImage *image, ResourceImage::Tile tile, Color4 tint_color);
	void draw_sprite(ResourceImage *image, Color4 tint_color={1,1,1,1}, Vec3 position={0,0,0}, Vec3 rotation={0,0,0}, Vec3 scale={1,1,1});
	void draw_sprite(ResourceImage *image, ResourceImage::Tile tile, Color4 tint_color={1,1,1,1}, Vec3 position={0,0,0}, Vec3 rotation={0,0,0}, Vec3 scale={1,1,1});

private:
	UniquePointer<RenderPassGeometry> pass;
	UniquePointer<VertexBuffer> vertex_buffer;
	ResourceHandle<ResourceImage> image;
	
	std::vector<Vertex> draw_commands_vec;
	u32 image_count;
	std::array<ResourceImage*, MAX_BATCH_SPRITE_COUNT> queued_images;
	std::map<UUID, u32> batched_images;
};

}

#endif
