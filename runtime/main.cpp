
#include "real/core/game.hpp"
#include "real/core/instance.hpp"
#include "real/core/logging.hpp"
#include "real/core/types.hpp"
#include <iostream>
#include <memory>

#include <real/real.hpp>

using namespace real;

REAL_ENTRY

int main() {
	Log &log = Log::get();
	log.name = "game engine";
	log.log_level = real::LogLevel_Trace;
	log.sinks.push_back(new real::LogSink_Console());
    
	Graphics::init_backend({});

	Shared<Instance> instance = std::make_shared<Instance>();
    auto [game, dll] = Game::load_game_dll(instance);
	game->start();

	while(instance->should_close() == false) {
		instance->renderer->start_frame();
		game->update(0);
		instance->renderer->end_frame();
	}

	instance.reset();
    Game::destroy_game_dll(game, dll);
	Graphics::destroy_backend();
}

