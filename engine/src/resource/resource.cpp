
#include "real/core/game.hpp"
#include <iostream>
#include <real/resource/resource.hpp>

namespace real {

Resource::Resource(Game *_game, std::optional<Path> _path) 
    : Object(_game), path(_path) {}

Resource::~Resource() {}

}

