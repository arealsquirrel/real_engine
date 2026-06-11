
#include "real/graphics/buffer.hpp"
#include "real/core/object.hpp"

namespace real {

UniformBuffer::UniformBuffer(Instance *_instance, size_t _size) 
    : Object(_instance), size(_size) {}

UniformBuffer::~UniformBuffer() {

}
    
}
