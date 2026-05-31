
#include "real/core/game.hpp"
#include <iostream>
#include <real/resource/resource.hpp>

namespace real {

Resource::Resource(Instance *_instance, std::optional<Path> _path) 
    : Object(_instance), path(_path) {}

Resource::~Resource() {}

}

