
#include "real/scene/scene.hpp"
#include <real/scene/entity.hpp>

namespace real {

EntityHandle::EntityHandle(entt::entity _entity, Scene *_scene) 
    : handle(_entity), registry(_scene->registry) {}

}
