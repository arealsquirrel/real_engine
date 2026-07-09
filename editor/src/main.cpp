
#include "editor.hpp"
#include "imgui.h"
#include "panel_logs.hpp"
#include "panel_resource_database.hpp"
#include "panel_resource_viewer.hpp"
#include "panel_scene_view.hpp"
#include "real/core/core.hpp"
#include "real/core/instance.hpp"
#include "real/core/logging.hpp"
#include "real/graphics/graphics_system.hpp"
#include <cassert>
#include <real/real.hpp>

using namespace real;

editor::EditorExitReason reason = editor::EditorExitReason::NotExiting;

void run_game(Shared<Instance> instance, editor::Editor *ed, real::ArgParams params) {
	reason = editor::EditorExitReason::NotExiting;
	/*
	game->scene = std::make_shared<Scene>(instance.get());
	game->start();
	game->scene->awake();
	*/
	
	/*
	auto graphics_system = game->scene->get_system<GraphicsSystem>();
	instance->renderer->attach_camera(*ed->camera.camera.get());
	ed->add_panel<editor::PanelSceneView>(game->scene);
	ed->editor_viewport = game->screen_framebuffer->get_color_resolve_image();

	while(instance->should_close() == false && reason == editor::EditorExitReason::NotExiting) {
		instance->renderer->start_frame();
		instance->renderer->attach_camera(*ed->camera.camera.get());
		game->screen_framebuffer->clear_image();
		ImGui::DockSpaceOverViewport(0, nullptr, ImGuiDockNodeFlags_PassthruCentralNode);
		game->scene->update(0);
		CVarSystem::get().render_imgui();
		reason = ed->render(0);
		instance->renderer->end_frame();
	}

	game->scene->destroy();
	CVarSystem::get().clear_cvars();

	// remove all panels with referance to world
	auto it = ed->panels.begin();
	while (it != ed->panels.end()) {
		if(editor::PanelSceneView* derivedPtr = dynamic_cast<editor::PanelSceneView*>((*it).get())) {
			it = ed->panels.erase(it); 
		} else {
			++it;
		}
	}

	Game::destroy_game_dll(game, dll);
	if(reason == editor::EditorExitReason::Reload) {
		instance->resource_database->unregister_all();
		run_game(instance, ed, params);
	}
	*/
}

Shared<Instance> instance;
ArgParams params;

void init_engine() {
	Graphics::init_backend({});
	Log &log = Log::get();
	log.name = "game engine";
	log.log_level = real::LogLevel_Trace;
	log.sinks.push_back(new real::LogSink_Console());

	instance = std::make_shared<Instance>(params);
}

void destroy_engine() {
	assert(instance.use_count() == 1);
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

