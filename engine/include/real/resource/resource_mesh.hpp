#ifndef REALLIB_RESOURCE_MESH_HPP
#define REALLIB_RESOURCE_MESH_HPP

#include "real/container/ref.hpp"
#include "real/core/core.hpp"
#include <map>
#include <real/core/event.hpp>
#include "real/core/instance.hpp"
#include "real/core/object.hpp"
#include "real/container/string_hash.hpp"
#include "real/graphics/buffer.hpp"
#include "real/resource/resource.hpp"
#include "real/resource/resource_image.hpp"
#include <cstdint>
#include <string>
#include <vector>

namespace real {

class RenderPassGeometry;

class REALLIB_EXPORT ResourceMesh : public Resource {
RL_OBJECT(ResourceMesh, Resource)

public:
	struct Material {
		ResourceImage *diffuse_image;
		float metallic;
		float roughness;
	};

	struct Mesh {
		std::string name;
		size_t start_index;
		size_t count;
	};

public:
	ResourceMesh(Instance *_instance,
			std::vector<uint32_t> indexes,
			char *vertex_data, size_t vertex_data_size,
			std::map<StringHash, ResourceMesh::Mesh> meshes={});

	~ResourceMesh();

	friend RenderPassGeometry;

public:
	std::map<StringHash, ResourceMesh::Mesh> meshes;
	UniquePointer<IndexBuffer> index_buffer;
	UniquePointer<VertexBuffer> vertex_buffer;
};

}

#endif
