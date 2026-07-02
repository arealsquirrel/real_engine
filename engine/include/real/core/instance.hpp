#ifndef REALLIB_INSTANCE_HPP
#define REALLIB_INSTANCE_HPP

#include "real/core/types.hpp"
#include <real/core/core.hpp>
#include <real/core/allocator.hpp>

namespace real {

class EventMessenger;
class ResourceDatabase;
class Window;
class Renderer;

class REALLIB_EXPORT Instance {
public:
	Instance(ArgParams _arg_params);
	~Instance();

public:
	bool should_close();

public:
	Shared<ResourceDatabase> resource_database;
	Shared<Window> window;
    Shared<Renderer> renderer;
	Unique<EventMessenger> event_messenger;
	ArgParams arg_params;

	// cleared every frame!
	StackAllocator frame_allocator;
	SystemAllocator system_allocator;
};

}

#endif
