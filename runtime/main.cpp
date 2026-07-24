
#include "real/core/allocator.hpp"
#include "real/core/core.hpp"
#include "real/core/game.hpp"
#include "real/core/instance.hpp"
#include "real/core/logging.hpp"
#include "real/debug/cvars.hpp"
#include "real/graphics/graphics_system.hpp"
#include "real/graphics/renderpass_geometry.hpp"
#include "real/scene/scene.hpp"
#include <real/graphics/framebuffer.hpp>

#include <real/real.hpp>
#include <tracy/Tracy.hpp>

using namespace real;

int main(int argc, char **argv) {
	FrameMarkStart("Startup");
	Log &log = Log::get();
	log.name = "game engine";
	log.log_level = real::LogLevel_Trace;
	log.sinks.push_back(new real::LogSink_Console());

	ArgParams params = parse_args(argc, argv);
    
	Graphics::init_backend({});
	{
		Ref<Instance> instance = create_ref<Instance>(global_system_allocator(), params);
		auto [game, dll] = Game::load_game_dll(instance, params);
		Ref<Framebuffer> screen_framebuffer = Framebuffer::create(instance.get(), params.window_width, params.window_height, true, MultisamplingCount::Eight).to_ref();
		Ref<Scene> scene = create_ref<Scene>(&instance->engine_allocator, instance.get());
		game->scene = scene;
		game->screen_framebuffer = screen_framebuffer;
		game->start();
		game->scene->awake();
		auto graphics_system = game->scene->get_system<GraphicsSystem>();
		graphics_system->bind_main_camera = true;
		FrameMarkEnd("Startup");

		while(instance->should_close() == false) {
			graphics_system->bind_main_camera = true;
			FrameMarkStart("Render Frame");
			instance->renderer->start_frame();
			game->update(0);
			game->scene->update(0);
			instance->renderer->end_frame(screen_framebuffer->get_color_resolve_image());
			FrameMarkEnd("Render Frame");
		}
 
		CVarSystem::get().clear_cvars();
		game->scene->destroy();
		Game::destroy_game_dll(game, dll);
	}

	Graphics::destroy_backend();
}

