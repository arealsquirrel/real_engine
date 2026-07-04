
#include "real/core/logging.hpp"
#include "real/core/object.hpp"
#include "real/debug/timer.hpp"
#include "real/resource/resource_handle.hpp"
#include <cassert>
#include <real/resource/resource_database.hpp>

namespace real {

ResourceDatabase::ResourceDatabase(Instance *instance) 
	: Object(instance) {}

ResourceDatabase::~ResourceDatabase() {
	for (auto &[id, resource] : resource_map) {
		resource.free();
	}
}

void ResourceDatabase::unregister_resource(std::string name) {
	RL_INSTRUMENT_FUNCTION
	auto f = name_to_resource_UUID.find(name);
	if(f == name_to_resource_UUID.end()) {
		RL_LOG_WARN("can not find resource {}", name);
		return;
	}

	Entry en = f->second;
	name_to_resource_UUID.erase(name);
	resource_map.at(en.id).free();
	resource_map.erase(en.id);
	RL_LOG_TRACE("unloaded resource {}", name.c_str());
}

void ResourceDatabase::unregister_all() {
	resource_map.clear();
	name_to_resource_UUID.clear();
}

}

