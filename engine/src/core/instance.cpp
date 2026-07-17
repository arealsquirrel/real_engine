
#include "real/core/instance.hpp"
#include "real/core/core.hpp"
#include "real/core/event.hpp"
#include "real/core/logging.hpp"
#include "real/debug/timer.hpp"
#include "real/graphics/mesh_renderer.hpp"
#include "real/graphics/sprite_renderer.hpp"
#include "real/resource/resource_database.hpp"
#include "real/resource/resource_pack.hpp"
#include <GLFW/glfw3.h>
#include <memory>
#include <real/core/game.hpp>
#include <tracy/Tracy.hpp>
#include <utility>

namespace real {

Instance::Instance(ArgParams _arg_params)
	: arg_params(_arg_params), frame_allocator(1024), engine_allocator(10000) {
	
	ZoneScoped

	WindowInfo info;
	info.width = arg_params.window_width;
	info.height = arg_params.window_height;
	info.title = "my engine :3";
	event_messenger = std::make_unique<EventMessenger>();
	window = std::make_shared<Window>(this, info);
	resource_database = std::make_shared<ResourceDatabase>(this);
	renderer = std::move(Renderer::create(this, window));
    renderer->init();
	resource_database->load_resource_disk<ResourcePack>("../../engine/resources/resource_pack.json");

	renderer->subrenderers.make_emplace<MeshRenderer>(this, renderer.get());
	renderer->subrenderers.make_emplace<SpriteRenderer>(this, renderer.get());
}

Instance::~Instance() {
	ZoneScoped

	resource_database.reset();
	renderer->destroy_renderers();
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

