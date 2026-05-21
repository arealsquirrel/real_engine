
#include <real/graphics/renderer.hpp>

namespace real {

Renderer::Renderer(Instance *_instance, Shared<Window> _window) 
    : instance(_instance), window(_window){}

Renderer::~Renderer() = default;

}
