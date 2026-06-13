
#include "real/core/object.hpp"
#include <real/core/event_listener.hpp>

namespace real {

EventListener::EventListener(Instance *_instance, Object *_attached)
    : ev_instance(_instance), attached(_attached) {}

EventListener::~EventListener() {
    for (UUID id : subscribed_events) {
        ev_instance->event_messenger->unsubscribe(attached, id);
    }
}

}
