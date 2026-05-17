
#include "real/core/instance.hpp"
#include "real/graphics/renderer.hpp"
#include "real/graphics/window.hpp"
#include <GLFW/glfw3.h>

namespace real {
    
Instance::Instance() {
}

Instance::~Instance() {
    delete renderer;
    delete window;
}

void Instance::init(const WindowInfo &window_info) {
    window = new Window(this, window_info);
    renderer = new Renderer(this, window);
}

bool Instance::update() {
    renderer->draw();
    glfwPollEvents();
    return window->should_exit();
}

}
