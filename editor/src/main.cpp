
#include "real/core/game.hpp"
#include "real/graphics/window.hpp"
#include <real/real.hpp>

using namespace real;

REAL_ENTRY

int main() {
	Shared<Instance> instance = std::make_shared<Instance>();
    instance->log.name = "game engine";
    instance->log.log_level = real::LogLevel_Trace;
    instance->log.sinks.push_back(new real::LogSink_Console());

	Graphics::init_backend({});

	WindowInfo info;
	info.width = 1200;
	info.height = 1200;
	info.title = "my engine :3";
	instance->init(info);

	auto game = game_entrypoint(instance);
	game->start();
	while(instance->update() == false) {
		game->update(0);
	}

	game.reset();
	instance.reset();

	Graphics::destroy_backend();
}

