#ifndef REALLIB_RESOURCE_MESH_HPP
#define REALLIB_RESOURCE_MESH_HPP

#include "real/core/core.hpp"
#include "real/resource/resource.hpp"
#include <cstdint>

namespace real {

typedef char* ResourceMeshBuffer;

class ResourceMesh : public Resource {
RL_CLASS(ResourceMesh)

public:
	ResourceMesh(Instance *_instance, std::vector<uint32_t> indexes, char *vertex_data, size_t vertex_data_size);
	~ResourceMesh();

	virtual void bind() = 0;
	virtual void unbind() = 0;

private:
	ResourceMeshBuffer buffer;
};

}

#endif
