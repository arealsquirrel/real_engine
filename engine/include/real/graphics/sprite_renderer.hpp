#ifndef REALLIB_SPRITE_RENDERER_HPP
#define REALLIB_SPRITE_RENDERER_HPP

#include "real/core/color.hpp"
#include "real/core/core.hpp"
#include "real/core/object.hpp"
#include "real/core/types.hpp"
#include "real/core/uuid.hpp"
#include "real/graphics/graphics_system.hpp"
#include "real/graphics/renderpass_geometry.hpp"
#include "real/math/mat4.hpp"
#include "real/math/vec2.hpp"
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
	static constexpr u32 MAX_BATCH_SPRITE_COUNT = 8;

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
	using SubRenderer::SubRenderer;
	~SpriteRenderer();

	void awake() final override;
	void render(u32 deltatime) final override;
	void destroy() final override;

private:
	Unique<RenderPassGeometry> pass;
	Unique<ResourceMesh> mesh;
	ResourceHandle<ResourceImage> image;
	
	std::vector<Vertex> draw_commands;
	
	u32 image_count;
	std::array<ResourceImage*, MAX_BATCH_SPRITE_COUNT> queued_images;
	std::map<UUID, u32> batched_images;
};

}

#endif
