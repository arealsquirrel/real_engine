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

	static constexpr const char *shader_mode_tostr[4] = {
		"Flat", "Lit", "Normal", "UV"
	};

public:
	struct DrawCommand {
		Mat4 model;
		ResourceMesh *mesh;
		ResourceMesh::Mesh submesh;
		ResourceImage *image;
		unsigned int texture_index;
		unsigned int shader_mode;
	};

public:
	using SubRenderer::SubRenderer;
	~MeshRenderer();

	void draw_mesh(Mat4 model,
			ResourceMesh *mesh, ResourceMesh::Mesh submesh, ResourceImage *texture,
			ShaderMode mode=ShaderMode_Flat);

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

struct REALLIB_EXPORT ComponentMeshRenderer {
    RL_COMPONENT(ComponentMeshRenderer)

    ComponentMeshRenderer(
			ResourceHandle<ResourceMesh> _mesh,
			ResourceHandle<ResourceImage> _texture)
        : mesh(_mesh), texture(_texture),
		  sub_mesh(_mesh.get()->meshes.begin()->second),
		  tile(_texture.get()->tiles[StringHash("_full_image")]){}

    ComponentMeshRenderer(
			ResourceHandle<ResourceMesh> _mesh,
			ResourceHandle<ResourceImage> _texture, 
			StringHash submesh_name, StringHash tile_name=StringHash("_full_image"))
        : mesh(_mesh), texture(_texture),
		  sub_mesh(_mesh.get()->meshes.at(submesh_name)),
		  tile(_texture.get()->tiles.at(tile_name)) {}

    ComponentMeshRenderer(const ComponentMeshRenderer &) = default;
    ~ComponentMeshRenderer() = default;

    ResourceHandle<ResourceMesh> mesh;
    ResourceHandle<ResourceImage> texture;
	ResourceMesh::Mesh sub_mesh;
	ResourceImage::Tile tile;
	MeshRenderer::ShaderMode shader_mode {MeshRenderer::ShaderMode_Lit};
};

}

#endif
