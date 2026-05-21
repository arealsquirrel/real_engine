

#include "real/core/instance.hpp"
#include "real/core/types.hpp"
#include "real/graphics/graphics.hpp"
#include <memory>

using namespace real;

extern void game_main(Shared<Instance> instance);

int main() {
	Shared<Instance> instance = std::make_shared<Instance>();
    instance->log.name = "game engine";
    instance->log.log_level = real::LogLevel_Trace;
    instance->log.sinks.push_back(new real::LogSink_Console());

	Graphics::init_backend({true, "Engine"});

	instance->log.info("passing to game main!");
	game_main(instance);

	instance.reset();
	Graphics::destroy_backend();
}

