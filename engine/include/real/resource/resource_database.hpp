#ifndef REALLIB_RESOURCE_DATABASE_HPP
#define REALLIB_RESOURCE_DATABASE_HPP

#include "fmt/format.h"
#include "real/core/logging.hpp"
#include "real/core/object.hpp"
#include "real/core/types.hpp"
#include "real/core/uuid.hpp"
#include "real/debug/timer.hpp"
#include "real/resource/resource.hpp"
#include "real/resource/resource_handle.hpp"
#include <functional>
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
    ResourceDatabase(Instance *Instance);
    ~ResourceDatabase();

	using ResourceLoadFn = std::optional<std::function<ResourceHandle<Resource>(Path)>>;

	struct Entry {
		UUID id;
		std::string name;
		std::optional<Path> load_path;
		ResourceLoadFn load_fn;
	};

public:
	/**
	 * @brief gives a resource over for managment by the resource database.
	 * 
	 * @tparam T the type of the resource
	 * @param resource 
	 * @param name the name of the resource it will be known as in the resource database
	 * @param id the id of the resource in the lookup
	 * @param load_path 
	 * @param load_fn 
	 * @return ResourceHandle<T> a handle to the resource as it is stored in the database
	 */
	template<typename T>
	ResourceHandle<T> register_resource(
			T *resource, std::string name, UUID id=UUID(),
			std::optional<Path> load_path=std::nullopt, ResourceLoadFn load_fn=std::nullopt) {
		
		RL_INSTRUMENT_FUNCTION

		if(name == "") {
			if(load_path.has_value() == false) {
				RL_LOG_WARN("Registering resource without name or path, using UUID for name");
				name = fmt::format("{}", id.uuid);
			} else {
				name = load_path->filename();
			}
		}

		if (name_to_resource_UUID.find(name) != name_to_resource_UUID.end()) {
			RL_LOG_WARN("name collision, renaming resource {}", name);
			name.append("_");
		}

		if(resource_map.find(id) != resource_map.end()) {
			RL_LOG_WARN("UUID collision, giving new ID to resource {}", id.uuid);
			id = UUID(); // lets hope this works :D
		}

		ResourceHandle<T> handle(this, resource, ResourceState::Loaded, id);
		resource_map.emplace(id, handle);
		name_to_resource_UUID.emplace(name, Entry{id, name, load_path, load_fn});
		RL_LOG_TRACE("registered resource {}", name.c_str());
		return handle;
	}

	template<typename T>
	ResourceHandle<T> get_resource(UUID id) {
		return ResourceHandle<T>(
				resource_map.at(id));
	}

	template<typename T>
	ResourceHandle<T> get_resource(std::string name) {
		return get_resource<T>(name_to_resource_UUID.find(name)->second.id);
	}

	/**
	 * @brief removes a resource from the database. Deletes and unloads the resource handle
	 * 
	 * @param name the name of the resource to be unloaded
	 */
	void unregister_resource(std::string name);

	template<class R>
	ResourceHandle<R> load_resource_disk(Path path, std::string name="");

	template<class R>
	ResourceHandle<R> load_resource_glob(Path path, std::string name="");

private:
	EXPOSE_TO_EDITOR;	

	std::unordered_map<UUID, ResourceHandle<Resource>> resource_map;
	std::unordered_map<std::string, Entry> name_to_resource_UUID;
};

}

#endif
