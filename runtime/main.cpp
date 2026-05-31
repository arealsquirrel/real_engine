
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

	WindowInfo info;
	info.width = 1200;
	info.height = 800;
	info.title = "my engine :3";
	Shared<Window> window = std::move(Graphics::create_window(game, info));
	Shared<ResourceDatabase> resource_database = std::move(std::make_unique<ResourceDatabase>(game));
	Shared<Renderer> renderer = std::move(Graphics::create_renderer(game, window));

	game->window = window;
	game->resource_database = resource_database;
	game->renderer = renderer;

	game->renderer->init();
	game->start();

	while(game->should_close() == false) {
		game->update(0);

		auto frame = game->renderer->start_frame();
		game->render(frame);
		game->renderer->end_frame(frame);
	}

	game->destroy();
	delete game;
	window.reset();
	resource_database.reset();
	renderer.reset();

	Graphics::destroy_backend();
}

