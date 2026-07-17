
#include "editor.hpp"
#include "imgui.h"
#include "panel_logs.hpp"
#include "panel_resource_database.hpp"
#include "panel_resource_viewer.hpp"
#include "panel_scene_view.hpp"
#include "real/core/allocator.hpp"
#include "real/core/core.hpp"
#include "real/core/instance.hpp"
#include "real/core/logging.hpp"
#include "real/graphics/graphics_system.hpp"
#include <cassert>
#include <real/real.hpp>

using namespace real;

editor::EditorExitReason reason = editor::EditorExitReason::NotExiting;

Ref<Instance> instance;
ArgParams params;

void init_engine() {
	Graphics::init_backend({});
	Log &log = Log::get();
	log.name = "game engine";
	log.log_level = real::LogLevel_Trace;
	log.sinks.push_back(new real::LogSink_Console());

	instance = create_ref<Instance>(global_system_allocator(), params);
}

void destroy_engine() {
	// assert(instance.use_count() == 1);
	instance.reset();
	Graphics::destroy_backend();
}

int main(int argc, char **argv) {
	params = parse_args(argc, argv);
	init_engine();

	editor::Editor *ed = new editor::Editor(instance, params);

	ed->load_game(params.game_dll_path);
	
	bool should_exit = false;
	while(should_exit == false) {
		instance->renderer->start_frame();
		should_exit = ed->render(0);
		instance->renderer->end_frame();
	}
	
	ed->destroy_game();
	
	delete ed;
	destroy_engine();
}

