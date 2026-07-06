#ifndef REALLIB_RESOURCE_PACK_HPP
#define REALLIB_RESOURCE_PACK_HPP

#include "real/core/instance.hpp"
#include "real/core/object.hpp"
#include "real/core/types.hpp"
#include "real/core/uuid.hpp"
#include "real/resource/resource.hpp"
#include <set>

namespace real {

class REALLIB_EXPORT ResourcePack : public Resource {
RL_OBJECT(ResourcePack, Resource)

public:
	ResourcePack(Instance *_instance, std::set<std::pair<Path, UUID>> resources);
	~ResourcePack();

	std::vector<char*> to_glob() override;

	std::set<std::pair<Path, UUID>> resources;
};

}

#endif
