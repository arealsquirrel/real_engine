#ifndef REALLIB_SCENE_HPP
#define REALLIB_SCENE_HPP

#include "entt/entity/fwd.hpp"
#include "real/core/instance.hpp"
#include "real/core/object.hpp"
#include "real/core/types.hpp"
#include "real/core/uuid.hpp"
#include "real/scene/system.hpp"
#include <entt/entity/registry.hpp>
#include <memory>
#include <set>
#include <utility>
#include <vector>

namespace real {

class EntityHandle;

class REALLIB_EXPORT Scene : public Object {
RL_OBJECT(Scene, Object)

public:
    Scene(Instance *instance);
    ~Scene();

    EntityHandle create_entity(String name);
    EntityHandle create_entity(String name, UUID id);

    void awake();
    void update(u32 delta_time);
    void destroy();

    template<typename T, typename ...Args>
    Shared<T> add_system(Args ...args) {
        static_assert(std::is_base_of_v<System, T>, "system type must inherit from system");
        auto a = std::make_shared<T>(instance, this, std::forward<Args>(args)...);
        systems.push_back(a);
        return a;
    }

    void emit_component_added(const char *name);

public:
    entt::registry *registry;

private:
    std::vector<Shared<System>> systems;
    
    friend EntityHandle;
    friend System;
};

}

#endif