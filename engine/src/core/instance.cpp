
#include "real/core/instance.hpp"
#include "real/graphics/graphics.hpp"
#include "real/graphics/renderer.hpp"
#include "real/graphics/window.hpp"
#include <GLFW/glfw3.h>
#include <memory>

namespace real {
    
Instance::Instance() {
}

Instance::~Instance() {
    renderer.reset();
    window.reset();
}

void Instance::init(const WindowInfo &window_info) {
    log.trace("initilizing instance");
	window = Graphics::create_window(this, window_info);
    renderer = Graphics::create_renderer(this, window);
}

bool Instance::update() {
    glfwPollEvents();
    return window->should_exit();
}

}
