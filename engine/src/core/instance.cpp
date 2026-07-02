
#include "real/core/instance.hpp"
#include "real/core/core.hpp"
#include "real/core/event.hpp"
#include "real/core/logging.hpp"
#include "real/debug/timer.hpp"
#include "real/resource/resource_database.hpp"
#include <GLFW/glfw3.h>
#include <memory>
#include <real/core/game.hpp>
#include <utility>

namespace real {

Instance::Instance(ArgParams _arg_params)
	: arg_params(_arg_params), frame_allocator(1024) {
	RL_INSTRUMENT_FUNCTION
	WindowInfo info;
	info.width = arg_params.window_width;
	info.height = arg_params.window_height;
	info.title = "my engine :3";
	event_messenger = std::make_unique<EventMessenger>();
	window = std::make_shared<Window>(this, info);
	resource_database = std::make_shared<ResourceDatabase>(this);
	renderer = std::move(Renderer::create(this, window));
    renderer->init();
}

Instance::~Instance() {
	RL_INSTRUMENT_FUNCTION
	resource_database.reset();
    renderer.reset();
    window.reset();
	auto *p = event_messenger.release();
	delete p;
}

bool Instance::should_close() {
	glfwPollEvents();
	return window->should_exit();
}

}

