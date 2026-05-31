
#include "real/core/game.hpp"
#include <real/graphics/renderer.hpp>

namespace real {

Renderer::Renderer(Game *_game, Shared<Window> _window) 
    : Object(_game), window(_window){}

Renderer::~Renderer() = default;

}
