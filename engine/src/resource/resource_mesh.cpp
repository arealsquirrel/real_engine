
#include "real/core/game.hpp"
#include "real/core/logging.hpp"
#include "real/core/string_hash.hpp"
#include "real/core/types.hpp"
#include "real/core/uuid.hpp"
#include "real/debug/timer.hpp"
#include "real/graphics/graphics.hpp"
#include "real/math/vec3.hpp"
#include "real/math/vec4.hpp"
#include "real/resource/resource.hpp"
#include <cstdint>
#include <tracy/Tracy.hpp>
#include <unordered_map>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.hpp>

#include <real/resource/resource_mesh.hpp>

namespace real {

struct Vertex {
	Vec3 pos;
	float uv_x;
	Vec3 normal;
	float uv_y;
	Vec4 color;

	bool operator==(const Vertex& other) const {
    	return pos == other.pos && color == other.color && uv_x == other.uv_x && uv_y == other.uv_y && other.normal == normal;
	}
};

}

namespace real {

ResourceMesh::ResourceMesh(
		Instance *_instance,
		std::vector<uint32_t> indexes,
		char *vertex_data, size_t vertex_data_size,
		std::map<StringHash, ResourceMesh::Mesh> _meshes)
	: Resource(_instance), meshes(_meshes) {

	if(meshes.size() == 0) {
		ResourceMesh::Mesh mesh = Mesh{
			.name = std::to_string(get_instance_uuid()),
			.start_index = 0,
			.count = indexes.size(),
		};

		StringHash hash("mesh");

		meshes.insert({hash, mesh});
	}
}

ResourceMesh::~ResourceMesh() {

}

template<>
ResourceHandle<ResourceMesh> ResourceDatabase::load_resource_disk<>(Path path, std::string name) {
	ZoneScoped

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn;
	std::string err;
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str());

	std::map<StringHash, ResourceMesh::Mesh> meshes;
	for (const auto& shape : shapes) {
		RL_LOG_TRACE("mesh {}, found mesh {}", path.filename().c_str(), shape.name.c_str());
		ResourceMesh::Mesh mesh {shape.name.c_str(), indices.size()};
		u32 count = 0;

		for (const auto& index : shape.mesh.indices) {
			Vertex vertex{};
			count++;

			vertex.pos = {
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};

			vertex.normal = {
				attrib.normals[3 * index.normal_index + 0],
				attrib.normals[3 * index.normal_index + 1],
				attrib.normals[3 * index.normal_index + 2],
			};

			vertex.color = {1.0f, 1.0f, 1.0f, 1.0f};

			vertex.uv_x = attrib.texcoords[2 * index.texcoord_index + 0],
    		vertex.uv_y = 1.0f - attrib.texcoords[2 * index.texcoord_index + 1];

			vertices.push_back(vertex);
	        indices.push_back(indices.size());
		}

		mesh.count = count;
		meshes.insert({StringHash(shape.name.c_str()), mesh});
	}

	auto *mesh = ResourceMesh::create(instance, indices, (char*)vertices.data(), vertices.size()*sizeof(Vertex), meshes).release();
	mesh->verticie_count = vertices.size();
	mesh->indices_count = indices.size();

	return register_resource(mesh, name, UUID(), path);
}

}
