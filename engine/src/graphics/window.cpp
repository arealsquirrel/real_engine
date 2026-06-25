
#include "real/core/event.hpp"
#include "real/core/game.hpp"
#include "real/core/instance.hpp"
#include "real/core/logging.hpp"
#include "real/core/object.hpp"
#include "real/debug/timer.hpp"
#include <GLFW/glfw3.h>
#include <real/graphics/window.hpp>
#include <assert.h>
#include <utility>

namespace real {

u32 Window::s_window_count = 0;

// we will cross this bridge if we ever do multiple windows
static Instance *s_instance;
static Window *s_window;

static void glfw_resize_callback(GLFWwindow *window, int width, int height) {
	RL_LOG_INFO("RESIZING");
    s_instance->event_messenger->emit_event<EventWindowResize>(s_window, width, height);
}

Window::Window(Instance *_instance, const WindowInfo &info)
	: Object(_instance) {
    RL_INSTRUMENT_FUNCTION

    if(s_window_count++ == 0) {
        RL_LOG_INFO("initilizing GLFW {}:{}", GLFW_VERSION_MAJOR, GLFW_VERSION_MINOR);
        s_instance = _instance;
        s_window = this;
        glfwSetErrorCallback(real_glfw_error_callback);
		glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_WAYLAND);
        glfwInit();
    }

	// glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(info.width, info.height, info.title, NULL, NULL);

    glfwSetFramebufferSizeCallback(window, glfw_resize_callback);
}

Window::~Window() {
    RL_INSTRUMENT_FUNCTION
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

