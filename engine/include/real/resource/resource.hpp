#ifndef REALLIB_RESOURCE_HPP
#define REALLIB_RESOURCE_HPP

#include "real/core/core.hpp"
#include "real/core/object.hpp"
#include <optional>
#include <real/core/types.hpp>

namespace real {

enum class REALLIB_EXPORT ResourceSerializerType {
    Disk,
    ZipFile,
};

using LoadStruct = void*;
using SaveStruct = void*;
using SaveObject = void*;

/**
 * @brief stores data in a specified format so the engine can use it
 */
class REALLIB_EXPORT Resource : public Object {
RL_OBJECT(Resource, Object)

public:
    Resource(Game *_game, std::optional<Path> _path=std::nullopt);
    virtual ~Resource();

	template<ResourceSerializerType ST, typename ResourceType>
	static ResourceType *load(
		Game *game,
		Optional<Path> path=std::nullopt);

	template<ResourceSerializerType ST, typename ResourceType>
	static SaveObject save(
		ResourceType *resource,
		const SaveStruct saveStruct=nullptr,
		Optional<Path> path=std::nullopt);

public:
	const std::optional<Path> path;
};

}

#endif
