
#include "real/graphics/input.hpp"
#include "real/core/object.hpp"
#include "real/container/string_hash.hpp"
#include <GLFW/glfw3.h>
#include <real/graphics/window.hpp>

namespace real {

Input::Input(Instance *_instance, Window *_window)
    : Object(_instance), window(_window) {}

Input::~Input() = default;

double Input::poll_mouse_x() const {
    return x_mouse_pos;
}
double Input::poll_mouse_y() const {
    return y_mouse_pos;
}
std::pair<double, double> Input::poll_mouse_pos() const {
    return std::make_pair(x_mouse_pos, y_mouse_pos);
}

bool Input::poll_key(int glfw_keycode) {
    int state = glfwGetKey(window->glfw_window(), glfw_keycode);
    return (state == GLFW_PRESS);
}

bool Input::poll_mouse_button(int glfw_button) const {
    int state = glfwGetMouseButton(window->glfw_window(), glfw_button);
    return (state == GLFW_PRESS);
}

void Input::register_key_group(
    std::initializer_list<KeyCombo> combos, std::string name) {

    auto hash = StringHash(name);
    KeyGroup group {combos, hash, name};
    groups.emplace(hash, group);
}

bool Input::poll_key_group(StringHash hash) const {
    const auto &group = groups.at(hash);
    for (auto combo : group.combos) {
        if(poll_key_combo(combo)) {
            return true;
        }
    }

    return false;
}

bool Input::poll_key_combo(KeyCombo combo) const {
    for (size_t i = 0; i < combo.arr_size; i++) {
        int state = glfwGetKey(window->glfw_window(), GLFW_KEY_E);
        if(state != GLFW_PRESS) {
            return false;
        }
    }

    return true;
}

}
