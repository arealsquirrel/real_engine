#ifndef REALLIB_INPUT_HPP
#define REALLIB_INPUT_HPP

#include "real/core/instance.hpp"
#include "real/core/object.hpp"
#include "real/container/string_hash.hpp"
#include <cstddef>
#include <initializer_list>
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace real {

static constexpr size_t MAX_KEY_COMBO_SIZE = 5;

struct REALLIB_EXPORT KeyCombo {
    int keys[MAX_KEY_COMBO_SIZE];
    size_t arr_size;
};

struct REALLIB_EXPORT KeyGroup {
    std::vector<KeyCombo> combos;
    StringHash hash;
    std::string name;
};

class REALLIB_EXPORT Input : public Object {
RL_OBJECT(Input, Object)

public:
    Input(Instance *instance, Window *window);
    ~Input();

	void poll_events();

    bool poll_key(int glfw_keycode);

    double poll_mouse_x() const;
    double poll_mouse_y() const;
    std::pair<double, double> poll_mouse_pos() const;
    bool poll_mouse_button(int glfw_button) const;

    void register_key_group(std::initializer_list<KeyCombo> combos, std::string name);
    KeyGroup &get_key_group(StringHash hash);
    bool poll_key_group(StringHash hash) const;
    bool poll_key_combo(KeyCombo combo) const;

private:
    Window *window;
    friend Window;

    /* these values are set by the window every mouse callback */
    double x_mouse_pos;
    double y_mouse_pos;

    std::map<StringHash, KeyGroup> groups;
};

}

#endif
