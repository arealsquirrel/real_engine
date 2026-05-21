
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
    log.trace("initilizing instance");
	window = Graphics::create_window(this, window_info);
    renderer = Graphics::create_renderer(this, window);
	resource_database = std::make_unique<ResourceDatabase>();
}

bool Instance::update() {
    glfwPollEvents();
    return window->should_exit();
}

}
