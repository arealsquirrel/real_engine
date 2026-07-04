
#include "real/core/logging.hpp"
#include "real/core/uuid.hpp"
#include "real/resource/resource_database.hpp"
#include "real/resource/resource_handle.hpp"
#include "real/resource/resource_image.hpp"
#include "real/resource/resource_mesh.hpp"
#include "real/resource/resource_shader.hpp"
#include <filesystem>
#include <real/resource/resource_pack.hpp>
#include <nlohmann/json.hpp>

namespace real {

UUID pack_string_to_uuid(std::string str) {
	if(str == "Image")
		return ResourceImage::object_typeinfo_static()->id.uuid;
	if(str == "Shader")
		return ResourceShader::object_typeinfo_static()->id.uuid;
	if(str == "Mesh")
		return ResourceMesh::object_typeinfo_static()->id.uuid;

	return {0};
}

ResourcePack::ResourcePack(Instance *_instance, std::set<std::pair<Path, UUID>> _resources)
	: Resource(_instance), resources(_resources) {

	// do one whole pass for metadata
	for (auto [path, uuid] : resources) {
		if(uuid.uuid == ResourceImage::object_typeinfo_static()->id.uuid) {
			RL_LOG_INFO("image {} {}", uuid.uuid, path.c_str());
			if(path.extension() != ".json")
				continue;
			
			instance->resource_database->load_resource_disk<ResourceImage>(path);
		}
	}

	for (auto [path, uuid] : resources) {
		if(uuid.uuid == ResourceImage::object_typeinfo_static()->id.uuid) {
			RL_LOG_TRACE("image {} {}", uuid.uuid, path.c_str());
			if(path.extension() == ".json")
				continue;
			if(instance->resource_database->has_resource(path.filename()) == false)
				instance->resource_database->load_resource_disk<ResourceImage>(path);

		} else if(uuid.uuid == ResourceShader::object_typeinfo_static()->id.uuid) {
			RL_LOG_TRACE("shader {} {}", uuid.uuid, path.c_str());
			instance->resource_database->load_resource_disk<ResourceShader>(path);
		} else if(uuid.uuid == ResourceMesh::object_typeinfo_static()->id.uuid) {
			RL_LOG_TRACE("mesh {} {}", uuid.uuid, path.c_str());
			if(path.extension() == ".mtl")
				continue;
			instance->resource_database->load_resource_disk<ResourceMesh>(path);
		}
	}
}

ResourcePack::~ResourcePack() = default;

template<>
ResourceHandle<ResourcePack> ResourceDatabase::load_resource_disk(Path path, std::string name) {
	using namespace nlohmann;

	std::ifstream f(path);
	nlohmann::json js = nlohmann::json::parse(f);

	std::set<std::pair<Path, UUID>> resources;
	for(auto val : js["directories"].items()) {
		Path rec_path = Path(path.parent_path().string() + "/" + val.value()["path"].get<std::string>());
		UUID id = pack_string_to_uuid(val.value()["type"].get<std::string>());
		if(id.uuid == 0) {
			RL_LOG_WARN("unknown resource type {}", val.value()["type"].get<std::string>());
			continue;
		}

		if(std::filesystem::exists(rec_path) == false) {
			RL_LOG_WARN("can not find path {}", rec_path.c_str());
			continue;
		}

		for (auto file : std::filesystem::recursive_directory_iterator(rec_path)) {
			if(file.is_regular_file())
				resources.emplace(std::make_pair(file.path(), id));
		}
	}

	auto *pack = new ResourcePack(instance, resources);
	return register_resource(pack, path.filename().string());
}

}

