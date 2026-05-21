
#include "real/resource/resource_handle.hpp"
#include <real/resource/resource_database.hpp>

namespace real {

ResourceDatabase::ResourceDatabase() {}

ResourceDatabase::~ResourceDatabase() {

}

void ResourceDatabase::unload_resource(std::string name) {
	unload_resource(name_to_resource_UUID.find(name)->second);
}

void ResourceDatabase::unload_resource(UUID hash) {
	ResourceHandle<Resource> handle = resource_array[uuid_to_entry.find(hash)->second.arr_index];
	handle.set_state(ResourceState::Unloaded);
}

ResourceDatabase::Entry ResourceDatabase::get_entry(std::string name) {
	return get_entry(name_to_resource_UUID.find(name)->second);
}

ResourceDatabase::Entry ResourceDatabase::get_entry(UUID id) {
	return uuid_to_entry.find(id)->second;
}

}

