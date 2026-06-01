
#include "real/core/logging.hpp"
#include "real/core/object.hpp"
#include "real/resource/resource_handle.hpp"
#include <cassert>
#include <real/resource/resource_database.hpp>

namespace real {

ResourceDatabase::ResourceDatabase(Instance *instance) 
	: Object(instance) {}

ResourceDatabase::~ResourceDatabase() {
	for (auto &[id, resource] : resource_map) {
		resource.unload();
	}
}

/*
ResourceDatabase::Entry ResourceDatabase::get_entry(std::string name) {
	assert(name_to_resource_UUID.find(name) != name_to_resource_UUID.end());
	return get_entry(name_to_resource_UUID.find(name)->second);
}


ResourceDatabase::Entry ResourceDatabase::get_entry(UUID id) {
	return uuid_to_entry.find(id)->second;
}
*/

void ResourceDatabase::unregister_resource(std::string name) {
	// Entry e = get_entry(name);
	UUID id = name_to_resource_UUID.at(name);
	name_to_resource_UUID.erase(name);
	// uuid_to_entry.erase(id);
	resource_map.at(id).unload();
	resource_map.erase(id);
	RL_LOG_TRACE("unloaded resource {}", name.c_str());
}


void ResourceDatabase::clean_non_references() {

}

void ResourceDatabase::clean_unloaded() {
	
}

}

