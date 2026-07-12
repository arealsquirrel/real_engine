
#include "entt/entity/fwd.hpp"
#include "real/core/event.hpp"
#include "real/core/logging.hpp"
#include "real/core/object.hpp"
#include "real/core/types.hpp"
#include "real/core/uuid.hpp"
#include "real/scene/components.hpp"
#include <real/scene/entity.hpp>
#include <real/scene/scene.hpp>

namespace real {

Scene::Scene(Instance *instance)
    : Object(instance) {

	RL_LOG_INFO("Scene Made");
    registry = new entt::registry();
}

Scene::~Scene() {
	RL_LOG_INFO("Scene destroyed");
	delete registry;
}

EntityHandle Scene::create_entity(String name) {
    return create_entity(name, UUID());
}

EntityHandle Scene::create_entity(String name, UUID id) {
    auto entity = registry->create();
    EntityHandle eh(entity, this);

    eh.AddComponent<ComponentID>(name, id);
    eh.AddComponent<ComponentTransform>();

    return eh;
}

void Scene::awake() {
    for (auto s : systems) {
		s->awake();
    }
}

void Scene::update(u32 delta_time) {
    for (auto s : systems) {
    	s->update(delta_time);
	}
}

void Scene::destroy() {
    for (auto s : systems) {
		s->destroy();
    }
}

void Scene::emit_component_added(const char *data) {
    instance->event_messenger->emit_event<EventComponentAdded>(this, data);
}

}
