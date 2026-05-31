
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
	Graphics::init_backend({});
	editor::EditorExitReason reason = editor::EditorExitReason::NotExiting;
	Shared<Instance> instance = std::make_shared<Instance>();
	editor::Editor *ed = new editor::Editor(instance);
	ed->add_panel<editor::PanelResourceDatabase>();
	ed->add_panel<editor::PanelLogs>(log_buffer);

reload_game:
	reason = editor::EditorExitReason::NotExiting;
	Game *game = new Game(instance);
	game->start();

	while(instance->should_close() == false && reason == editor::EditorExitReason::NotExiting) {
		game->update(0);
		auto frame = game->renderer->start_frame();
		reason = ed->render();
		game->render(frame);
		game->renderer->end_frame(frame);
	}

	game->destroy();
	delete game;
	if(reason == editor::EditorExitReason::Reload) {
		goto reload_game;
	}

	delete ed;
	instance.reset();	

	Graphics::destroy_backend();
}

