
#include "real/core/object.hpp"
#include <real/scene/system.hpp>

namespace real {

System::System(Instance *_instance, Scene *_scene) 
    : Object(_instance), scene(_scene) {}

System::~System() = default;

void System::draw_imgui() {

}

}
