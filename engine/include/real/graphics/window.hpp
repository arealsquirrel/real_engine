#ifndef REALLIB_WINDOW_HPP
#define REALLIB_WINDOW_HPP

#include <real/core/event.hpp>
#include "real/core/instance.hpp"
#include "real/core/object.hpp"
#include "real/core/types.hpp"
#include "real/graphics/input.hpp"
#include <utility>
#include <real/core/core.hpp>


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace real {

// class Instance;

struct WindowInfo {
    int width, height;
    const char *title;
};

/**
 * @brief makes and manages a window
 */
class REALLIB_EXPORT Window : public Object {
RL_OBJECT(Window, Object)

public:
    Window(Instance *_instance, const WindowInfo &info);
    ~Window();

    bool should_exit();
    void swap_buffers();

    GLFWwindow *glfw_window() { return window; }

	/* this makes an API call so please dont call it a bunch */
	std::pair<u32, u32> get_glfw_window_dimensions();

    bool key_pressed(int glfw_key);
    std::pair<double, double> get_mouse_position();

public:
    Unique<Input> input;

private:
    GLFWwindow *window;
    static u32 s_window_count;
};

void real_glfw_error_callback(int error, const char *description);

}

#endif
