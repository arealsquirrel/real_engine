
#include "real/resource/resource.hpp"

#define TINYGLTF_IMPLEMENTATION
#include <tiny_obj_loader.hpp>

#include <real/resource/resource_mesh.hpp>
#include <real/core/instance.hpp>

namespace real {

ResourceMesh::ResourceMesh(
		Instance *_instance,
		std::vector<uint32_t> indexes,
		char *vertex_data, size_t vertex_data_size)
	: Resource(_instance) {

}

ResourceMesh::~ResourceMesh() {

}

}

