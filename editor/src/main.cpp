
#include "editor.hpp"
#include "panel_logs.hpp"
#include "panel_resource_database.hpp"
#include "panel_resource_viewer.hpp"
#include "real/core/logging.hpp"
#include "real/graphics/window.hpp"
#include "real/resource/resource_handle.hpp"
#include "real/resource/resource_image.hpp"
#include <real/real.hpp>

using namespace real;

REAL_ENTRY

int main() {
	Shared<Instance> instance = std::make_shared<Instance>();
	real::LogSink_Buffer *log_buffer;
	
	{
		Log *log = Log::get();
		log->name = "game engine";
		log->log_level = real::LogLevel_Trace;
		log_buffer = new real::LogSink_Buffer();
		log->sinks.push_back(new real::LogSink_Console());
		log->sinks.push_back(log_buffer);
	}

	Graphics::init_backend({});

	WindowInfo info;
	info.width = 1200;
	info.height = 800;
	info.title = "my engine :3";
	instance->init(info);

	editor::Editor *ed = new editor::Editor(instance);
	ed->add_panel<editor::PanelResourceDatabase>();
	ed->add_panel<editor::PanelLogs>(log_buffer);
	
	auto game = game_entrypoint(instance);
	game->start();

	while(instance->update() == false) {
		game->update(0);

		auto frame = instance->renderer->start_frame();
		ed->render();
		game->render(frame);
		instance->renderer->end_frame(frame);
	}

	delete ed;
	game.reset();
	instance.reset();

	Graphics::destroy_backend();
}

