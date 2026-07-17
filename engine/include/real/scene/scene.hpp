#ifndef REALLIB_SCENE_HPP
#define REALLIB_SCENE_HPP

#include "entt/entity/fwd.hpp"
#include "real/core/core.hpp"
#include "real/core/instance.hpp"
#include "real/core/logging.hpp"
#include "real/core/object.hpp"
#include "real/core/object_container.hpp"
#include "real/core/types.hpp"
#include "real/core/uuid.hpp"
#include "real/scene/system.hpp"
#include <entt/entity/registry.hpp>
#include <set>

namespace real {

struct EntityHandle;

class REALLIB_EXPORT Scene : public Object {
RL_OBJECT(Scene, Object)

public:
    Scene(Instance *instance);
    ~Scene();

    EntityHandle create_entity(String name);
    EntityHandle create_entity(String name, UUID id);

	/*
	 * clones entities, but does not clone systems
	 */
	Ref<Scene> clone();

    void awake();
    void update(u32 delta_time);
    void destroy();

    template<typename T, typename ...Args>
    Ref<T> add_system(Args ...args) {
		RL_LOG_TRACE("adding system");
        auto [emp, ptr] = systems.make_emplace<T>(instance, this, std::forward<Args>(args)...);
        return ptr;
    }

	template<typename T>
	Ref<T> get_system() {
		return systems.get<T>();
	}

    void emit_component_added(const char *name);

	UniqueObjectSet<System> systems;

public:
    entt::registry *registry;
	std::set<entt::entity> entities;
    
    friend EntityHandle;
    friend System;
	EXPOSE_TO_EDITOR;
};

}

#endif
