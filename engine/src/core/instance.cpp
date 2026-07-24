
#include "real/core/instance.hpp"
#include "real/container/ref.hpp"
#include "real/core/core.hpp"
#include "real/core/event.hpp"
#include "real/core/logging.hpp"
#include "real/graphics/mesh_renderer.hpp"
#include "real/graphics/sprite_renderer.hpp"
#include "real/resource/resource_database.hpp"
#include "real/resource/resource_pack.hpp"
#include <GLFW/glfw3.h>
#include <real/core/game.hpp>
#include <tracy/Tracy.hpp>

namespace real {

Instance::Instance(ArgParams _arg_params)
	: arg_params(_arg_params), frame_allocator(1024), engine_allocator(70000) {
	
	ZoneScoped

	WindowInfo info;
	info.width = arg_params.window_width;
	info.height = arg_params.window_height;
	info.title = "my engine :3";
	event_messenger = create_unique<EventMessenger>(&engine_allocator);
	window = create_ref<Window>(&engine_allocator, this, info);
	resource_database = create_ref<ResourceDatabase>(&engine_allocator, this);
	renderer = Renderer::create(this, window).to_ref();
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
	event_messenger.destroy();
    window.reset();
}

bool Instance::should_close() {
	glfwPollEvents();
	return window->should_exit();
}

}

