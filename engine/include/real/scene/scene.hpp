#ifndef REALLIB_SCENE_HPP
#define REALLIB_SCENE_HPP

#include "real/core/instance.hpp"
#include "real/core/object.hpp"
#include "real/scene/node.hpp"

namespace real {

class Scene : public Object {
RL_OBJECT(Scene, Object)

public:
    Scene(Instance *instance);
    ~Scene();

    Node *get_root_node() const;

private:
    friend Node;
    Node *root_node;
};

}

#endif