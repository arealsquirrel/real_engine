
#include "real/core/object.hpp"
#include <real/graphics/framebuffer.hpp>

namespace real {

Framebuffer::Framebuffer(
        Instance *_instance, u32 _width, u32 _height,
        bool _depth, MultisamplingCount _count)   
    : Object(_instance), width(_width), height(_height), depth(_depth), msaa(_count) {}

Framebuffer::~Framebuffer() = default;

}
