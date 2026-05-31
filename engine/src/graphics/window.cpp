
#include "real/core/game.hpp"
#include "real/core/logging.hpp"
#include "real/core/object.hpp"
#include <GLFW/glfw3.h>
#include <real/graphics/window.hpp>
#include <assert.h>
#include <utility>

namespace real {

u32 Window::s_window_count = 0;

Window::Window(Game *_game, const WindowInfo &info)
	: Object(_game) {
    if(s_window_count++ == 0) {
        RL_LOG_INFO("initilizing GLFW {}:{}", GLFW_VERSION_MAJOR, GLFW_VERSION_MINOR);
        glfwSetErrorCallback(real_glfw_error_callback);
        glfwInit();
    }

	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(info.width, info.height, info.title, NULL, NULL);
}

Window::~Window() {
    glfwDestroyWindow(window);

    if(--s_window_count == 0) {
        RL_LOG_INFO("killing GLFW");
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

