
#include "real/core/instance.hpp"
#include "real/graphics/graphics.hpp"
#include "real/resource/resource_database.hpp"
#include <GLFW/glfw3.h>
#include <real/core/game.hpp>
#include <utility>

namespace real {

Game::Game(Shared<Instance> _instance) 
	: instance(_instance) {

	window = _instance->window; 
	resource_database = _instance->resource_database;
	renderer = _instance->renderer;
}

Game::~Game() {}

}

