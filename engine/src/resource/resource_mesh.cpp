
#include "real/core/types.hpp"
#include "real/resource/resource.hpp"
#include <cstdint>
#include <unordered_map>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.hpp>

#include <real/resource/resource_mesh.hpp>
#include <real/core/instance.hpp>
#include <glm/glm.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

namespace real {

struct Vertex {
	glm::vec3 pos;
	float uv_x;
	glm::vec3 normal;
	float uv_y;
	glm::vec4 color;

	bool operator==(const Vertex& other) const {
    	return pos == other.pos && color == other.color && uv_x == other.uv_x && uv_y == other.uv_y;
	}

};

}

namespace std {
    template<> struct hash<real::Vertex> {
        size_t operator()(real::Vertex const& vertex) const {
            return ((hash<glm::vec3>()(vertex.pos) ^
                   (hash<glm::vec3>()(vertex.color) << 1)) >> 1);
        }
    };
}

namespace real {

ResourceMesh::ResourceMesh(
		Instance *_instance,
		std::vector<uint32_t> indexes,
		char *vertex_data, size_t vertex_data_size)
	: Resource(_instance) {}

ResourceMesh::~ResourceMesh() {

}

template<>
ResourceMesh *Resource::load<ResourceSerializerType::Disk,ResourceMesh>(
		Instance *instance, Optional<Path> path) {

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn;
	std::string err;
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path->c_str());

	std::unordered_map<Vertex, uint32_t> uniqueVertices;

	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {
			Vertex vertex{};

			if (uniqueVertices.count(vertex) == 0) {
				uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
				vertices.push_back(vertex);
			}

			indices.push_back(uniqueVertices[vertex]);
		}
	}
}

}

/*
*/

