
#include "real/core/core.hpp"
#include "real/core/game.hpp"
#include "real/core/instance.hpp"
#include "real/core/logging.hpp"
#include "real/core/types.hpp"
#include "real/debug/cvars.hpp"
#include "real/graphics/graphics_system.hpp"
#include "real/graphics/renderpass_geometry.hpp"
#include <real/graphics/framebuffer.hpp>
#include <memory>

#include <real/real.hpp>

using namespace real;

int main(int argc, char **argv) {
	Log &log = Log::get();
	log.name = "game engine";
	log.log_level = real::LogLevel_Trace;
	log.sinks.push_back(new real::LogSink_Console());

	ArgParams params = parse_args(argc, argv);
    
	RL_INSTRUMENT_PROFILE_START("Startup");
	Graphics::init_backend({});
	Shared<Instance> instance = std::make_shared<Instance>(params);
    auto [game, dll] = Game::load_game_dll(instance, params);
	Shared<Framebuffer> screen_framebuffer = Framebuffer::create(instance.get(), params.window_width, params.window_height, true, MultisamplingCount::Eight);
	game->scene = std::make_shared<Scene>(instance.get());
	game->screen_framebuffer = screen_framebuffer;
	game->start();
	game->scene->awake();
	
	auto graphics_system = game->scene->get_system<GraphicsSystem>();
	RL_INSTRUMENT_PROFILE_END;

	RL_INSTRUMENT_PROFILE_START("Runtime");
	while(instance->should_close() == false) {
		instance->renderer->start_frame();
		game->update(0);
		game->scene->update(0);
		graphics_system->bind_main_camera();
		instance->renderer->end_frame(screen_framebuffer->get_color_resolve_image().get());
	}
	RL_INSTRUMENT_PROFILE_END;
 
	RL_INSTRUMENT_PROFILE_START("Shutdown");
	CVarSystem::get().clear_cvars();
	game->scene->destroy();
    Game::destroy_game_dll(game, dll);

	screen_framebuffer.reset();
	instance.reset();
	Graphics::destroy_backend();
	RL_INSTRUMENT_PROFILE_END;
}

