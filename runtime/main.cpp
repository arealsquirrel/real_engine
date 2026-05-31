
#include "real/core/game.hpp"
#include "real/core/instance.hpp"
#include "real/core/logging.hpp"
#include "real/core/types.hpp"
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
	Game *game = new Game(instance);

	game->start();

	while(instance->should_close() == false) {
		game->update(0);

		auto frame = instance->renderer->start_frame();
		game->render(frame);
		instance->renderer->end_frame(frame);
	}

	game->destroy();
	delete game;
	instance.reset();

	Graphics::destroy_backend();
}

