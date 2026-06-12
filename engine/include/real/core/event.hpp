#ifndef REALLIB_EVENT_HPP
#define REALLIB_EVENT_HPP

#include "real/core/core.hpp"
#include "real/core/instance.hpp"
#include "real/core/logging.hpp"
#include "real/core/object.hpp"
#include "real/core/types.hpp"
#include <functional>
#include <memory>
#include <real/core/uuid.hpp>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

namespace real {

#define RL_EVENT(name) public: \
    static constexpr const char *get_event_name() { return #name; } \
    static UUID get_event_id() { static UUID id; return id; }

struct REALLIB_EXPORT Event {
public:
    Event() = default;
    ~Event() = default;
};

class REALLIB_EXPORT EventMessenger : public Object {
RL_OBJECT(EventMessenger, Object)

public:
    struct EventFunctionHandle {
        EventFunctionHandle(UUID _attached)
            : attached_uuid(_attached) {}

        ~EventFunctionHandle() = default;

        virtual void call(Event &event, Object *from) = 0;
    
        UUID attached_uuid;
    };

    template<typename T>
    struct EventFunction : public EventFunctionHandle {
        EventFunction(EventFunctionPtr<T> _fn, UUID _attached)
            : EventFunctionHandle(_attached), fn(_fn) {}

        EventFunctionPtr<T> fn;

        void call(Event &event, Object *from) override {
            fn(static_cast<T&>(event), from);
        };
    };

public:
    EventMessenger(Instance *_instance);
    ~EventMessenger();

    template<typename T, typename ...Args>
    void emit_event(Object *from, Args&& ...args) {
        static_assert(std::is_base_of<Event, T>::value, "can not emit event that does not derive from Event");
        T event(std::forward<Args>(args)...);

        auto objs = event_map.find(T::get_event_id());
        if(objs == event_map.end()) {
            RL_LOG_WARN("can not emit event that is not in event_map {}", T::get_event_name());
            return;
        }

        for (auto &obj : objs->second) {
            obj->call(event, from);
        }
    }

    template<typename T>
    void subscribe(Object *attached, EventFunctionPtr<T> fn) {
        static_assert(std::is_base_of<Event, T>::value, "can not subscribe to event that does not derive from Event");
    
        auto vec_itr = event_map.find(T::get_event_id());
        if(vec_itr == event_map.end()) {
            vec_itr = event_map.emplace(T::get_event_id(), std::vector<Unique<EventFunctionHandle>>()).first;
        }

        vec_itr->second.push_back(std::move(std::unique_ptr<EventFunctionHandle>(new EventFunction<T>(fn, attached->get_instance_uuid()))));
    }

    template<typename T>
    void unsubscribe(Object *object) {
        static_assert(std::is_base_of<Event, T>::value, "can not subscribe to event that does not derive from Event");

        auto objs = event_map.find(T::get_event_id());
        if(objs == event_map.end()) {
            RL_LOG_WARN("can not remove event that is not in event_map {}", T::get_event_name());
            return;
        }

        for (auto it = objs->second.begin(); it != objs->second.end(); ++it) {
            if(it->get()->attached_uuid.uuid == object->get_instance_uuid().uuid) {
                it = objs->second.erase(it);
                return;
            }
        }
    }

private:
    std::unordered_map<UUID, std::vector<Unique<EventFunctionHandle>>> event_map;
};

}

#endif