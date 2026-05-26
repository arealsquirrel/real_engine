
#include "real/core/game.hpp"
#include "real/core/logging.hpp"
#include "real/graphics/window.hpp"
#include <real/real.hpp>

using namespace real;

REAL_ENTRY

int main() {
	Shared<Instance> instance = std::make_shared<Instance>();
	
	{
		Log *log = Log::get();
		log->name = "game engine";
		log->log_level = real::LogLevel_Trace;
		log->sinks.push_back(new real::LogSink_Console());
	}

	Graphics::init_backend({});

	WindowInfo info;
	info.width = 1200;
	info.height = 800;
	info.title = "my engine :3";
	instance->init(info);

	auto game = game_entrypoint(instance);
	game->start();
	while(instance->update() == false) {
		game->update(0);

		auto frame = instance->renderer->start_frame();
		game->render(frame);
		instance->renderer->end_frame(frame);
	}

	game.reset();
	instance.reset();

	Graphics::destroy_backend();
}

