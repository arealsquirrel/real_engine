
#include <GLFW/glfw3.h>
#include <real/graphics/window.hpp>
#include <real/core/instance.hpp>
#include <assert.h>
#include <utility>

namespace real {

u32 Window::s_window_count = 0;

Window::Window(Instance *_instance, const WindowInfo &info)
	: instance(_instance) {
    instance->log.info("Creating window");
    if(s_window_count++ == 0) {
        instance->log.info("initilizing GLFW {}:{}", GLFW_VERSION_MAJOR, GLFW_VERSION_MINOR);
        glfwSetErrorCallback(real_glfw_error_callback);
        assert(glfwInit());
    }

	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(info.width, info.height, info.title, NULL, NULL);
}

Window::~Window() {
    instance->log.info("destroying window");
    glfwDestroyWindow(window);

    if(--s_window_count == 0) {
        instance->log.info("killing GLFW");
        glfwTerminate();
    }
}

bool Window::should_exit() {
    return glfwWindowShouldClose(window);
}

void Window::swap_buffers() {
    return glfwSwapBuffers(window);
}

std::pair<u32, u32> Window::get_glfw_window_dimensions() {
	int w, h;
	glfwGetWindowSize(window, &w, &h);
	return std::make_pair((u32)w, (u32)h);
}

}

