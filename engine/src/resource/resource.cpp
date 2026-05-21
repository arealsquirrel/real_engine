
#include <iostream>
#include <real/resource/resource.hpp>

namespace real {

Resource::Resource(Instance *_instance) 
    : instance(_instance) {}

Resource::~Resource() {}

}

