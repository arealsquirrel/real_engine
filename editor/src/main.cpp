
#include "editor.hpp"
#include "imgui.h"
#include "panel_logs.hpp"
#include "panel_resource_database.hpp"
#include "panel_resource_viewer.hpp"
#include "panel_scene_view.hpp"
#include "real/core/logging.hpp"
#include "real/graphics/graphics_system.hpp"
#include <real/real.hpp>

using namespace real;

int main(int argc, char **argv) {
	real::LogSink_Buffer *log_buffer;
	Log &log = Log::get();
	log.name = "game engine";
	log.log_level = real::LogLevel_Trace;
	log_buffer = new real::LogSink_Buffer();
	log.sinks.push_back(new real::LogSink_Console());
	log.sinks.push_back(log_buffer);
	ArgParams params = parse_args(argc, argv);
	Graphics::init_backend({});
	Shared<Instance> instance = std::make_shared<Instance>(params);

reload_game:
	editor::EditorExitReason reason = editor::EditorExitReason::NotExiting;
	editor::Editor *ed = new editor::Editor(instance);
	ed->add_panel<editor::PanelResourceDatabase>();
	ed->add_panel<editor::PanelLogs>(log_buffer);
	ed->add_panel<editor::PanelResourceViewer>();
	
	reason = editor::EditorExitReason::NotExiting;
	auto [game, dll] = Game::load_game_dll(instance, params);
	game->start();
	game->scene->awake();
	game->scene->get_system<GraphicsSystem>()->set_main_camera(ed->camera.camera);
	ed->editor_viewport = game->screen_framebuffer->get_color_resolve_image();
	ed->add_panel<editor::PanelSceneView>(game->scene);

	while(instance->should_close() == false && reason == editor::EditorExitReason::NotExiting) {
		instance->renderer->start_frame();
		ImGui::DockSpaceOverViewport(0, nullptr, ImGuiDockNodeFlags_PassthruCentralNode);
		game->update(0);
		CVarSystem::get().render_imgui();
		reason = ed->render(0);
		instance->renderer->end_frame();
	}

	RL_LOG_INFO("bye bye");
	game->scene->destroy();
	RL_LOG_INFO("bye bye");
	CVarSystem::get().clear_cvars();
	RL_LOG_INFO("bye bye");
	delete ed;
	Game::destroy_game_dll(game, dll);
	if(reason == editor::EditorExitReason::Reload) {
		log_buffer->index = 0;
		goto reload_game;
	}

	RL_LOG_INFO("bye bye");
	instance.reset();
	Graphics::destroy_backend();
}

