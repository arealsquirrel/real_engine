#ifndef REALLIB_WINDOW_HPP
#define REALLIB_WINDOW_HPP

#include "real/core/types.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace real {

class Instance;

struct WindowInfo {
    int width, height;
    const char *title;
};

typedef void* WindowBackend;

/**
 * @brief makes and manages a window
 */
class Window {
public:
    Window(Instance *_instance, const WindowInfo &info);
    ~Window();

    bool should_exit();
    void swap_buffers();

    WindowBackend backend_handle();
    GLFWwindow *glfw_window() { return window; }

private:
    Instance *instance;
    GLFWwindow *window;
    WindowBackend backend;
    static u32 s_window_count;
};

void real_glfw_error_callback(int error, const char *description);

}

#endif