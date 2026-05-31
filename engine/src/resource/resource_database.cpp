
#include "real/core/object.hpp"
#include "real/resource/resource_handle.hpp"
#include <real/resource/resource_database.hpp>

namespace real {

ResourceDatabase::ResourceDatabase(Game *game) 
	: Object(game) {}

ResourceDatabase::~ResourceDatabase() {
	for (auto &resource : resource_array) {
		resource.unload();
	}
}

ResourceDatabase::Entry ResourceDatabase::get_entry(std::string name) {
	return get_entry(name_to_resource_UUID.find(name)->second);
}

ResourceDatabase::Entry ResourceDatabase::get_entry(UUID id) {
	return uuid_to_entry.find(id)->second;
}

void ResourceDatabase::clean_non_references() {

}

void ResourceDatabase::clean_unloaded() {
	
}

}

