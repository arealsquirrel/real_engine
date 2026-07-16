
#include "real/core/event_listener.hpp"
#include "real/core/instance.hpp"
#include "real/core/logging.hpp"
#include "real/debug/timer.hpp"
#include "real/graphics/framebuffer.hpp"
#include "real/graphics/graphics_system.hpp"
#include <GLFW/glfw3.h>
#include <cassert>
#include <real/core/game.hpp>
#include <tracy/Tracy.hpp>
#include <utility>
#include <dlfcn.h>

namespace real {

Game::Game(Shared<Instance> _instance, ArgParams params) 
	: EventListener(_instance.get(), nullptr), instance(_instance) {

	window = _instance->window; 
	resource_database = _instance->resource_database;
	renderer = _instance->renderer;
}

Game::~Game() {}

std::pair<Game*, DLLGameLoad> Game::load_game_dll(Shared<Instance> instance, ArgParams params) {
	ZoneScoped

	DLLGameLoad load;
	load.game_dll_handle = dlopen(params.game_dll_path.c_str(), RTLD_LAZY);
    if (!load.game_dll_handle) {
		RL_LOG_ERROR("Cannot open game libary {}", dlerror());
        assert(false);
    }

    load.create_game = (create_real_game_f*)dlsym(load.game_dll_handle, "game_create");
    const char* dlsym_error = dlerror();
    if (dlsym_error) {
        RL_LOG_ERROR("Cannot load symbol create");
        dlclose(load.game_dll_handle);
        assert(false);
    }

    load.destroy_game = (destroy_real_game_f*)dlsym(load.game_dll_handle, "game_destroy");
    dlsym_error = dlerror();
    if (dlsym_error) {
        RL_LOG_ERROR("Cannot load symbol destroy");
        dlclose(load.game_dll_handle);
        assert(false);
    }

	return std::make_pair(load.create_game(instance, params), load);
}

void Game::destroy_game_dll(Game* game, DLLGameLoad load) {
	ZoneScoped
	load.destroy_game(game);
	dlclose(load.game_dll_handle);
}

}

