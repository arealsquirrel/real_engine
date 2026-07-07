#ifndef REALLIB_MESH_RENDERER_HPP
#define REALLIB_MESH_RENDERER_HPP

#include "real/core/color.hpp"
#include "real/core/object.hpp"
#include "real/graphics/graphics_system.hpp"
#include "real/math/mat4.hpp"
#include "real/resource/resource_handle.hpp"
#include "real/resource/resource_image.hpp"
#include "real/resource/resource_mesh.hpp"

namespace real {

class MeshRenderer : public SubRenderer {
RL_OBJECT(MeshRenderer, SubRenderer)

public:
	static constexpr u32 MAX_BATCH_SPRITE_COUNT = 32;
	
	enum ShaderMode : unsigned int {
		ShaderMode_Flat,
		ShaderMode_Lit,
		ShaderMode_Normal,
		ShaderMode_UV
	};

public:
	struct DrawCommand {
		Mat4 model;
		ResourceMesh *mesh;
		ResourceMesh::Mesh submesh;
		ResourceImage *image;
		Color4 tint_color;
		unsigned int texture_index;
		unsigned int shader_mode;
	};

public:
	using SubRenderer::SubRenderer;
	~MeshRenderer();

	void draw_mesh(Mat4 model,
			ResourceMesh *mesh, ResourceMesh::Mesh submesh, ResourceImage *texture,
			Color4 tint_color={1,1,1,1}, ShaderMode mode=ShaderMode_Flat);

	void awake() final override;
	void render(u32 deltatime) final override;
	void destroy() final override;

private:
	Unique<RenderPassGeometry> diffuse_pass;
	ResourceHandle<ResourceImage> image;
	std::vector<DrawCommand> draw_commands;
	u32 image_count;
	std::array<ResourceImage*, MAX_BATCH_SPRITE_COUNT> queued_images;
	std::map<UUID, u32> batched_images;
};

}

#endif
