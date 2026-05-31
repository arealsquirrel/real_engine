
#include "real/core/game.hpp"
#include "real/core/logging.hpp"
#include <real/real.hpp>

using namespace real;

REAL_ENTRY

int main() {
	Log &log = Log::get();
	log.name = "game engine";
	log.log_level = real::LogLevel_Trace;
	log.sinks.push_back(new real::LogSink_Console());

	Graphics::init_backend({});

	Game *game = new Game();
	game->renderer->init();
	game->start();

	while(game->should_close() == false) {
		game->update(0);

		auto frame = game->renderer->start_frame();
		game->render(frame);
		game->renderer->end_frame(frame);
	}

	game->destroy();

	Graphics::destroy_backend();
}

