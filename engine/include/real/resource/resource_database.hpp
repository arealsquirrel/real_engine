#ifndef REALLIB_RESOURCE_DATABASE_HPP
#define REALLIB_RESOURCE_DATABASE_HPP

#include "real/core/object.hpp"
#include "real/core/types.hpp"
#include "real/core/uuid.hpp"
#include "real/resource/resource.hpp"
#include "real/resource/resource_handle.hpp"
#include <optional>
#include <real/core/string_hash.hpp>
#include <string>
#include <unordered_map>
#include <vector>
#include <real/core/core.hpp>

namespace real {

/**
 * @brief a data structure for storing and retrieving resources by name
 * some resources may depend on other resources so we have to generate a resource tree of sorts,
 * this will happen in a while because this first requires having resources and whatnot
 */
class REALLIB_EXPORT ResourceDatabase : public Object {
RL_OBJECT(ResourceDatabase, Object)

public:
    ResourceDatabase(Game *game);
    ~ResourceDatabase();

	struct Entry {
		u32 arr_index;
		UUID id;
		std::string name;
	};

public:
	template<typename T>
	ResourceHandle<T> register_resource(
			T *resource, std::string name,
			UUID id=UUID()) {

		u32 i = resource_array.size();
		ResourceHandle<T> handle(this, resource, ResourceState::Loaded, id);
		resource_array.push_back(handle);
		uuid_to_entry.emplace(id, Entry{i, id, name});
		name_to_resource_UUID.emplace(name, id);
		return handle;
	}

	template<typename T>
	ResourceHandle<T> get_resource(UUID id) {
		return ResourceHandle<T>(
				resource_array[uuid_to_entry.find(id)->second.arr_index]);
	}

	template<typename T>
	ResourceHandle<T> get_resource(std::string name) {
		return get_resource<T>(name_to_resource_UUID.find(name)->second);
	}

	/*
	template<typename T, ResourceSerializerType ST>
	void reload(std::string name) {
		ResourceHandle<T> r = get_resource<T>(name);
		Entry e = uuid_to_entry.at(r.get_uuid());

		ResourceHandle<T> newResource(
				this, Resource::load<ST, T>(instance.get()),
				ResourceState::Loaded, r.get_uuid());

		resource_array.at(e.arr_index) = newResource;
	}
	*/

	void clean_non_references();
	void clean_unloaded();

	Entry get_entry(std::string name);
	Entry get_entry(UUID id);

private:
	EXPOSE_TO_EDITOR;	

	std::vector<ResourceHandle<Resource>> resource_array;
	std::unordered_map<UUID, Entry> uuid_to_entry;
	std::unordered_map<std::string, UUID> name_to_resource_UUID;
};

}

#endif
