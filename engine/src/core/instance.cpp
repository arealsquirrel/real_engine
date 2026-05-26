
#include "real/core/instance.hpp"
#include "real/graphics/graphics.hpp"
#include "real/graphics/renderer.hpp"
#include "real/graphics/window.hpp"
#include "real/resource/resource_database.hpp"
#include <GLFW/glfw3.h>
#include <memory>

namespace real {
    
Instance::Instance() {
}

Instance::~Instance() {
	resource_database.reset();
    renderer.reset();
    window.reset();
}

void Instance::init(const WindowInfo &window_info) {
    RL_LOG_INFO("initilizing instance");
	window = Graphics::create_window(this, window_info);
	resource_database = std::make_unique<ResourceDatabase>();
	renderer = Graphics::create_renderer(this, window);
	renderer->init();
}

bool Instance::update() {
    glfwPollEvents();
    return window->should_exit();
}

}
