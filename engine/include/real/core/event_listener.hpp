#ifndef REALLIB_EVENT_LISTENER_HPP
#define REALLIB_EVENT_LISTENER_HPP

#include "real/core/core.hpp"
#include "real/core/event.hpp"
#include "real/core/instance.hpp"
#include "real/core/logging.hpp"
#include "real/core/uuid.hpp"
#include <real/core/object.hpp>
#include <set>

namespace real {

class REALLIB_EXPORT EventListener {
public:
    EventListener(Instance *_instance, Object *_attached);
    ~EventListener();

    template<typename T>
    void event_subscribe(EventFunctionPtr<T> fn) {
        static_assert(std::is_base_of<Event, T>::value, "T does not derive from Event");
        auto f = subscribed_events.find(T::get_event_id());
        if(f != subscribed_events.end()) {
            RL_LOG_WARN("{} Attempting to subscribe to event that has already been subscribed to {}",
                 attached->object_name(), T::get_event_name());
            return;
        }

        subscribed_events.emplace(T::get_event_id());

        ev_instance->event_messenger->subscribe<T>(attached, fn);
    }

    template<typename T>
    void event_unsubscribe() {
        static_assert(std::is_base_of<Event, T>::value, "T does not derive from Event");
        auto f = subscribed_events.find(T::get_event_id());
        if(f == subscribed_events.end()) {
            RL_LOG_WARN("{} Attempting to subscribe to event that not been subscribed to {}",
                 attached->object_name(), T::get_event_name());
            return;
        }

        f.erase();

        ev_instance->event_messenger->unsubscribe<T>(attached);
    }

private:
    std::set<UUID> subscribed_events;
    Object *attached;
    Instance *ev_instance;
};

}

#endif