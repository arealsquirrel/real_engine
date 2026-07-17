#ifndef REALLIB_EVENT_HPP
#define REALLIB_EVENT_HPP

#include "real/container/ref.hpp"
#include "real/core/allocator.hpp"
#include "real/core/core.hpp"
#include "real/core/object.hpp"
#include "real/core/types.hpp"
#include "real/scene/entity.hpp"
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

struct REALLIB_EXPORT EventWindowResize : public Event {
RL_EVENT(EventWindowResize)

public:
    EventWindowResize(int _width, int _height)
        : width(_width), height(_height) {}
    ~EventWindowResize() = default;

    int width;
    int height;
};

struct REALLIB_EXPORT EventComponentAdded : public Event {
RL_EVENT(EventComponentAdded)

public:
    EventComponentAdded(const char *_data)
        : data(_data) {}
        
    ~EventComponentAdded() = default;

    const char *data;
};

struct REALLIB_EXPORT EventEntityAdded : public Event {
public:
    EventEntityAdded(EntityHandle _entity)
        : entity(_entity) {}

    ~EventEntityAdded() = default;

    EntityHandle entity;
};

class REALLIB_EXPORT EventMessenger : public RefCounted {
public:
    struct EventFunctionHandle {
        EventFunctionHandle(Object *attached);

        virtual ~EventFunctionHandle() = default;

        virtual void call(Event &event, Object *from) = 0;
    
        UUID attached_uuid;
    };

    template<typename T>
    struct EventFunction : public EventFunctionHandle {
        EventFunction(EventFunctionPtr<T> _fn, Object *at)
            : EventFunctionHandle(at), fn(_fn) {};

        EventFunctionPtr<T> fn;

        void call(Event &event, Object *from) override {
            fn(static_cast<T&>(event), from);
        };
    };

public:
    EventMessenger();
    ~EventMessenger();

    template<typename T, typename ...Args>
    void emit_event(Object *from, Args&& ...args) {
        static_assert(std::is_base_of<Event, T>::value, "can not emit event that does not derive from Event");
        T event(std::forward<Args>(args)...);
        emit_event(from, T::get_event_id(), event);
    }

    template<typename T>
    void subscribe(Object *attached, EventFunctionPtr<T> fn) {
        static_assert(std::is_base_of<Event, T>::value, "can not subscribe to event that does not derive from Event");
        subscribe(attached, T::get_event_id(), global_system_allocator()->allocate_object<EventFunction<T>>(fn, attached));
    }

    template<typename T>
    void unsubscribe(Object *object) {
        static_assert(std::is_base_of<Event, T>::value, "can not subscribe to event that does not derive from Event");
        unsubscribe(object, T::get_event_id());
    }

    void subscribe(Object *attached, UUID eventID, EventFunctionHandle *unq);
    void emit_event(Object *from, UUID eventID, Event &event);
    void unsubscribe(Object *object, UUID eventID);

private:
    std::unordered_map<UUID, std::vector<UniquePointer<EventFunctionHandle>>> event_map;
};

}

#endif
