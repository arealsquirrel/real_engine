

#include "real/core/instance.hpp"
#include "real/core/types.hpp"
#include "real/graphics/graphics.hpp"
#include <memory>

using namespace real;

constexpr u32 WINDOW_WIDTH = 800;
constexpr u32 WINDOW_HEIGHT = 800;
constexpr const char* APPLICATION_NAME = "game engine wahoo";

extern void game_main(Shared<Instance> instance);

int main() {
	Shared<Instance> instance = std::make_shared<Instance>();
    instance->log.name = "game engine";
    instance->log.log_level = real::LogLevel_Trace;
    instance->log.sinks.push_back(new real::LogSink_Console());

	Graphics::init_backend({true, APPLICATION_NAME});

	WindowInfo window_info {
		.width = WINDOW_WIDTH,
		.height = WINDOW_HEIGHT,
		.title = APPLICATION_NAME
	};

	instance->init(window_info);

	instance->log.info("passing to game main!");

	game_main(instance);

	instance.reset();
	Graphics::destroy_backend();
}

