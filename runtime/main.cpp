
#include "real/core/game.hpp"
#include "real/core/instance.hpp"
#include "real/core/logging.hpp"
#include "real/core/types.hpp"
#include "real/debug/cvars.hpp"
#include "real/debug/instrumentation.hpp"
#include <iostream>
#include <memory>

#include <real/real.hpp>

using namespace real;

int main() {
	Log &log = Log::get();
	log.name = "game engine";
	log.log_level = real::LogLevel_Trace;
	log.sinks.push_back(new real::LogSink_Console());
    
	RL_INSTRUMENT_PROFILE_START("Startup");
	Graphics::init_backend({});

	Shared<Instance> instance = std::make_shared<Instance>();
    auto [game, dll] = Game::load_game_dll(instance);
	game->start();
	RL_INSTRUMENT_PROFILE_END;

	RL_INSTRUMENT_PROFILE_START("Runtime");
	while(instance->should_close() == false) {
		instance->renderer->start_frame();
		game->update(0);
		instance->renderer->end_frame();
	}
	RL_INSTRUMENT_PROFILE_END;

	RL_INSTRUMENT_PROFILE_START("Shutdown");
	CVarSystem::get().clear_cvars();
    Game::destroy_game_dll(game, dll);

	instance.reset();
	Graphics::destroy_backend();
	RL_INSTRUMENT_PROFILE_END;
}

