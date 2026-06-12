#ifndef REALLIB_SCENE_HPP
#define REALLIB_SCENE_HPP

#include "real/core/instance.hpp"
#include "real/core/object.hpp"

namespace real {

class Scene : public Object {
RL_OBJECT(Scene, Object)

public:
    Scene(Instance *instance);
    ~Scene();

};

}

#endif