#ifndef REALLIB_RESOURCE_HPP
#define REALLIB_RESOURCE_HPP

#include "real/core/core.hpp"
#include "real/core/instance.hpp"
#include <cstdint>
#include <optional>
#include <real/core/types.hpp>

namespace real {

enum class ResourceSerializerType {
    Disk,
    ZipFile,
};

using LoadStruct = void*;
using SaveStruct = void*;
using SaveObject = void*;

/**
 * @brief stores data in a specified format so the engine can use it
 */
class Resource {
RL_CLASS(Resource)

public:
    Resource(Instance *_instance, std::optional<Path> _path=std::nullopt);
    virtual ~Resource();

	template<ResourceSerializerType ST, typename ResourceType>
	static ResourceType *load(
		Instance *instance,
		Optional<Path> path=std::nullopt);

	template<ResourceSerializerType ST, typename ResourceType>
	static SaveObject save(
		ResourceType *resource,
		const SaveStruct saveStruct=nullptr,
		Optional<Path> path=std::nullopt);

public:
	const std::optional<Path> path;

protected:
    uint32_t size;
    Instance *instance;
};

}

#endif
