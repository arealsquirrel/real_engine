#ifndef REALLIB_INSTANCE_HPP
#define REALLIB_INSTANCE_HPP

#include "real/container/ref.hpp"
#include "real/core/types.hpp"
#include <real/core/core.hpp>
#include <real/core/allocator.hpp>

namespace real {

class EventMessenger;
class ResourceDatabase;
class Window;
class Renderer;

class REALLIB_EXPORT Instance : public RefCounted {
public:
	Instance(ArgParams _arg_params);
	~Instance();

public:
	bool should_close();

public:
	Ref<ResourceDatabase> resource_database;
	Ref<Window> window;
    Ref<Renderer> renderer;
	UniquePointer<EventMessenger> event_messenger;
	ArgParams arg_params;

	LinkedListAllocator engine_allocator;
	StackAllocator frame_allocator;
	SystemAllocator system_allocator;
};

}

#endif
