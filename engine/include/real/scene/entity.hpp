#ifndef REALLIB_ENTITY_HPP
#define REALLIB_ENTITY_HPP

#include "entt/entity/entity.hpp"
#include "real/scene/scene.hpp"
#include <entt/entity/fwd.hpp>

namespace real {

/*
 * This struct is a wrapper that allows us to interface
 * with the world registry and manipulate the components
 */
struct REALLIB_EXPORT EntityHandle {
public:
	EntityHandle(entt::entity t_entity, Scene *t_world);
	EntityHandle() = default;
	~EntityHandle() = default;

	// ! wacky error here, but the component struct has to hold data
	// ! or else this function will return void :3
	template <typename C>
	C &GetComponent() {
		return registry->get<C>(handle);
	}

	template <class C, typename... Args>
	C &AddComponent(Args &&...args) {
		return registry->emplace<C>(handle, std::forward<Args>(args)...);
	}

	template<class C, typename... Args>
	C &EmplaceOrReplace(Args &&...args) {
		return registry->emplace_or_replace<C>(handle, std::forward<Args>(args)...);
	}

	template <typename C>
	bool HasComponent() {
		return registry->any_of<C>(handle);
	}

	operator bool() { return !(handle == entt::null); }

public:
	entt::entity handle {entt::null};
	entt::registry *registry {nullptr};
};

}

#endif
