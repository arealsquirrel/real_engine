#ifndef REALLIB_RESOURCE_HPP
#define REALLIB_RESOURCE_HPP

#include "real/core/core.hpp"
#include "real/core/object.hpp"
#include <real/core/types.hpp>
#include <vector>

namespace real {
	
/**
 * @brief stores data in a specified format so the engine can use it
 */
class REALLIB_EXPORT Resource : public Object {
RL_OBJECT(Resource, Object)

public:
    Resource(Instance *_instance);
    virtual ~Resource();

    virtual std::vector<char*> to_glob() = 0;
};

}

#endif
