
#include "real/graphics/graphics.hpp"
#include "real/resource/resource_database.hpp"
#include <GLFW/glfw3.h>
#include <real/core/game.hpp>
#include <utility>

namespace real {

Game::Game() {
	/*
	WindowInfo info;
	info.width = 1200;
	info.height = 800;
	info.title = "my engine :3";
	window = Graphics::create_window(this, info);
	resource_database = std::make_unique<ResourceDatabase>(this);
	renderer = std::move(Graphics::create_renderer(this, window));
	*/
}

Game::~Game() {
	/*
	resource_database.reset();
    renderer.reset();
    window.reset();
	*/
}

bool Game::should_close() {
	glfwPollEvents();
	return window->should_exit();
}

}

