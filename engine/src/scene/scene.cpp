
#include "entt/entity/fwd.hpp"
#include "real/core/event.hpp"
#include "real/core/logging.hpp"
#include "real/core/object.hpp"
#include "real/core/types.hpp"
#include "real/core/uuid.hpp"
#include "real/scene/components.hpp"
#include <memory>
#include <real/scene/entity.hpp>
#include <real/scene/scene.hpp>
#include <tracy/Tracy.hpp>

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

Shared<Scene> Scene::clone() {
	Shared<Scene> scene = std::make_shared<Scene>(instance);

	for (auto entity : entities) {
		EntityHandle src_entity(entity, this);
		auto &src_id = src_entity.GetComponent<ComponentID>();
		EntityHandle dst_entity = scene->create_entity(src_id.name, src_id.id);
		dst_entity.EmplaceOrReplace<ComponentTransform>(src_entity.GetComponent<ComponentTransform>());
	}

	for (auto &system : systems) {
		scene->systems.emplace(system);
	}
	
	return scene;
}

EntityHandle Scene::create_entity(String name) {
    return create_entity(name, UUID());
}

EntityHandle Scene::create_entity(String name, UUID id) {
    auto entity = registry->create();
    EntityHandle eh(entity, this);

    eh.AddComponent<ComponentID>(name, id);
    eh.AddComponent<ComponentTransform>();
	entities.emplace(entity);

    return eh;
}

void Scene::awake() {
    for (auto s : systems) {
		s->awake();
    }
}

void Scene::update(u32 delta_time) {
	ZoneScoped

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
