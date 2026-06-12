
#include "real/core/event.hpp"
#include "real/core/object.hpp"
#include <gtest/gtest.h>
#include <real/real.hpp>

using namespace real;

struct TestEvent : public Event {
RL_EVENT(TestEvent)

TestEvent(int _x)
    : x(_x) {};

~TestEvent() = default;

int x;
};

TEST(Events, event) {
    EventMessenger messenger(nullptr);

    int trigger = 0;

    Object rec(nullptr);
    Object sender(nullptr);
    Object random(nullptr);
    messenger.subscribe<TestEvent>(&rec, 
        [&](TestEvent &event, Object *from){
        EXPECT_EQ(event.x, 2);
        EXPECT_EQ(from->get_instance_uuid(), sender.get_instance_uuid());
        EXPECT_NE(from->get_instance_uuid(), random.get_instance_uuid());
        trigger++;
    });

    messenger.emit_event<TestEvent>(&sender, 2);

    messenger.unsubscribe<TestEvent>(&rec);

    messenger.emit_event<TestEvent>(&sender, 2);
    EXPECT_EQ(trigger, 1);
}
