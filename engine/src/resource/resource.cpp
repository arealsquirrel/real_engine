
#include <iostream>
#include <real/resource/resource.hpp>

namespace real {

Resource::Resource(Instance *_instance, std::optional<Path> _path) 
    : instance(_instance), path(_path) {}

Resource::~Resource() {}

}

