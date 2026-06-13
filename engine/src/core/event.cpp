
#include "real/core/logging.hpp"
#include "real/core/object.hpp"
#include <memory>
#include <real/core/event.hpp>
#include <utility>
#include <vector>

namespace real {

EventMessenger::EventFunctionHandle::EventFunctionHandle(Object *attached)
    : attached_uuid(attached->get_instance_uuid()) {};

EventMessenger::EventMessenger() {}

EventMessenger::~EventMessenger() {

}

void EventMessenger::emit_event(Object *from, UUID eventID, Event &event) {
    auto objs = event_map.find(eventID);
    if(objs == event_map.end()) {
        RL_LOG_WARN("can not emit event that is not in event_map");
        return;
    }

    for (auto &obj : objs->second) {
        obj->call(event, from);
    }
}

void EventMessenger::subscribe(Object *attached, UUID eventID, EventFunctionHandle *unq) {
    auto vec_itr = event_map.find(eventID);
    if(vec_itr == event_map.end()) {
        vec_itr = event_map.emplace(eventID, std::vector<Unique<EventFunctionHandle>>()).first;
    }

    std::unique_ptr<EventFunctionHandle> hdn(unq);
    vec_itr->second.push_back(std::move(hdn));
}
 
void EventMessenger::unsubscribe(Object *object, UUID eventID) {
    auto objs = event_map.find(eventID);
    if(objs == event_map.end()) {
        RL_LOG_WARN("can not remove event that is not in event_map");
        return;
    }

    for (auto it = objs->second.begin(); it != objs->second.end(); ++it) {
        if(it->get()->attached_uuid.uuid == object->get_instance_uuid().uuid) {
            auto *p = it->release();
            delete p;
            it = objs->second.erase(it);
            return;
        }
    }
}

}
