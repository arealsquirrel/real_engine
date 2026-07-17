
#include "real/core/object.hpp"

namespace real {

u32 Object::object_count = 0;

Object::Object(Instance *_instance) {
	instance = _instance;
	object_count++;
}

Object::Object(Instance *_instance, Allocator *_allocator) {
	instance = _instance;
	allocator = _allocator;
	object_count++;
}

Object::~Object() {
	object_count -= 1;
}

u32 Object::get_object_count() { return object_count; }

}
