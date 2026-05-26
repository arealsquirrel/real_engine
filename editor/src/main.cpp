
#include "editor.hpp"
#include "panel_resource_database.hpp"
#include "panel_resource_viewer.hpp"
#include "real/graphics/window.hpp"
#include "real/resource/resource_handle.hpp"
#include "real/resource/resource_image.hpp"
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

	editor::Editor *ed = new editor::Editor(instance);
	ed->add_panel<editor::PanelResourceDatabase>();

	auto game = game_entrypoint(instance);
	game->start();

	auto uhg = instance->resource_database->get_resource<ResourceImage>("gradient.comp");
	ResourceHandle<Resource> rh(uhg);
	ed->add_panel<editor::PanelResourceViewer>(rh);

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

