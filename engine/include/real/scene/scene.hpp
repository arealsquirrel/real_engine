#ifndef REALLIB_SCENE_HPP
#define REALLIB_SCENE_HPP

#include "real/core/instance.hpp"
#include "real/core/object.hpp"
#include "real/core/uuid.hpp"
#include <bitset>
#include <cstddef>
#include <map>
#include <vector>

namespace real {

class Scene : public Object {
RL_OBJECT(Scene, Object)

public:
    Scene(Instance *instance);
    ~Scene();
};

}

#endif