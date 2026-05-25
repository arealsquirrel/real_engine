
#include <real/core/game.hpp>

namespace real {

Game::Game(Shared<Instance> _instance) 
	: instance(_instance) {}

Game::~Game() = default;

}

