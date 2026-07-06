#ifndef REALLIB_RESOURCE_MESH_HPP
#define REALLIB_RESOURCE_MESH_HPP

#include "real/core/core.hpp"
#include <map>
#include <real/core/event.hpp>
#include "real/core/object.hpp"
#include "real/core/string_hash.hpp"
#include "real/graphics/renderer.hpp"
#include "real/resource/resource.hpp"
#include <cstdint>
#include <string>
#include <vector>

namespace real {

typedef char* ResourceMeshBuffer;
typedef char* MeshAddress;

class RenderPassGeometry;

class REALLIB_EXPORT ResourceMesh : public Resource {
RL_OBJECT(ResourceMesh, Resource)

public:
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
	virtual ~ResourceMesh();

	virtual void bind() = 0;
	virtual void unbind() = 0;
	virtual MeshAddress get_address() = 0;
	virtual void upload_vertex_data(char *vertex_data, u32 size) = 0;
	virtual void upload_index_data(std::vector<u32> indices) = 0;

	std::vector<char*> to_glob() override;

	static Unique<ResourceMesh> create(
		Instance *instance, 
		std::vector<uint32_t> indices,
		char *data, size_t size, std::map<StringHash,ResourceMesh::Mesh> meshes={});

private:
	ResourceMeshBuffer buffer;

	friend RenderPassGeometry;

public:
	uint32_t indices_count {0};
	uint32_t verticie_count {0};
	std::map<StringHash, ResourceMesh::Mesh> meshes;
};

}

#endif
