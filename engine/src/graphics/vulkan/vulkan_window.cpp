
#include <GLFW/glfw3.h>
#include <cassert>
#include <real/graphics/window.hpp>
#include <real/core/instance.hpp>

namespace real {

u32 Window::s_window_count = 0;

Window::Window(Instance *_instance, const WindowInfo &info) 
    : instance(_instance) {

    if(s_window_count++ == 0) {
        instance->log.info("initilizing GLFW {}:{}", GLFW_VERSION_MAJOR, GLFW_VERSION_MINOR);
        glfwSetErrorCallback(real_glfw_error_callback);
        assert(glfwInit());
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(info.width, info.height, info.title, NULL, NULL);

    /* now we can set up the swapchain and whatever for this window */
}

Window::~Window() {
    glfwDestroyWindow(window);

    if(--s_window_count == 0) {
        instance->log.info("killing GLFW");
        glfwTerminate();
    }
}

WindowBackend Window::backend_handle() {
    return backend;
}

bool Window::should_exit() {
    return glfwWindowShouldClose(window);
}

void Window::swap_buffers() {
    return glfwSwapBuffers(window);
}

}
