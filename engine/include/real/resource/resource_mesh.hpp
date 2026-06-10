#ifndef REALLIB_RESOURCE_MESH_HPP
#define REALLIB_RESOURCE_MESH_HPP

#include "real/core/core.hpp"
#include "real/core/object.hpp"
#include "real/graphics/renderer.hpp"
#include "real/resource/resource.hpp"
#include <cstdint>

namespace real {

typedef char* ResourceMeshBuffer;
typedef char* MeshAddress;

class RenderPassGeometry;

class REALLIB_EXPORT ResourceMesh : public Resource {
RL_OBJECT(ResourceMesh, Resource)

public:
	ResourceMesh(Instance *_instance, std::vector<uint32_t> indexes, char *vertex_data, size_t vertex_data_size);
	virtual ~ResourceMesh();

	virtual void bind() = 0;
	virtual void unbind() = 0;
	virtual MeshAddress get_address() = 0;

private:
	ResourceMeshBuffer buffer;

	friend RenderPassGeometry;
};

}

#endif
