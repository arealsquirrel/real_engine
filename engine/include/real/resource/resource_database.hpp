#ifndef REALLIB_RESOURCE_DATABASE_HPP
#define REALLIB_RESOURCE_DATABASE_HPP

#include "fmt/format.h"
#include "real/core/logging.hpp"
#include "real/core/object.hpp"
#include "real/core/types.hpp"
#include "real/core/uuid.hpp"
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
	template<typename T>
	ResourceHandle<T> register_resource(
			T *resource, std::string name, UUID id=UUID(),
			std::optional<Path> load_path=std::nullopt, ResourceLoadFn load_fn=std::nullopt) {

		if(name == "") {
			if(load_path.has_value() == false) {
				RL_LOG_WARN("Registering resource without name or path, using UUID for name");
				name = fmt::format("{}", id.uuid);
			} else {
				name = load_path->filename();
			}
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

	void unregister_resource(std::string name);

	void clean_non_references();
	void clean_unloaded();

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
