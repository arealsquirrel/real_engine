
#include "editor.hpp"
#include "panel_logs.hpp"
#include "panel_resource_database.hpp"
#include "panel_resource_viewer.hpp"
#include "real/core/game.hpp"
#include "real/core/logging.hpp"
#include "real/debug/instrumentation.hpp"
#include "real/graphics/window.hpp"
#include "real/resource/resource_handle.hpp"
#include "real/resource/resource_image.hpp"
#include <real/real.hpp>

using namespace real;
int main() {
	real::LogSink_Buffer *log_buffer;
	Log &log = Log::get();
	log.name = "game engine";
	log.log_level = real::LogLevel_Trace;
	log_buffer = new real::LogSink_Buffer();
	log.sinks.push_back(new real::LogSink_Console());
	log.sinks.push_back(log_buffer);

	WindowInfo info;
	info.width = 1200;
	info.height = 800;
	info.title = "my engine :3";

	Graphics::init_backend({});
	editor::EditorExitReason reason = editor::EditorExitReason::NotExiting;

	// while(reason != editor::EditorExitReason::Reload) {
		Shared<Game> game = std::shared_ptr<Game>(new Game());
		Shared<Window> window = std::move(Graphics::create_window(game.get(), info));
		Shared<ResourceDatabase> resource_database = std::move(std::make_unique<ResourceDatabase>(game.get()));
		Shared<Renderer> renderer = std::move(Graphics::create_renderer(game.get(), window));
		game->window = window;
		game->resource_database = resource_database;
		game->renderer = renderer;
		game->renderer->init();
		game->start();

		editor::Editor *ed = new editor::Editor(game);
		ed->add_panel<editor::PanelResourceDatabase>();
		ed->add_panel<editor::PanelLogs>(log_buffer);

		while(game->should_close() == false && reason == editor::EditorExitReason::NotExiting) {
			game->update(0);
			auto frame = game->renderer->start_frame();
			reason = ed->render();
			game->render(frame);
			game->renderer->end_frame(frame);
		}

		game->destroy();
		delete ed;
		game.reset();
		window.reset();
		resource_database.reset();
		renderer.reset();
	// }

	Graphics::destroy_backend();
}

