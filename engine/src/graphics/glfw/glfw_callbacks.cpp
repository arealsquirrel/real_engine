
#include "fmt/base.h"
#include <real/graphics/window.hpp>

namespace real {

void real_glfw_error_callback(int error, const char *description) {
    fmt::println("GLFW error code {}, {}", error, description);
}

}
