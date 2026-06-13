
#include "editor.hpp"
#include "imgui.h"
#include "panel_logs.hpp"
#include "panel_resource_database.hpp"
#include "panel_resource_viewer.hpp"
#include "real/core/game.hpp"
#include "real/core/logging.hpp"
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
	ed->add_panel<editor::PanelResourceViewer>();

reload_game:
	reason = editor::EditorExitReason::NotExiting;
	auto [game, dll] = Game::load_game_dll(instance);
	game->start();
	log_buffer->index = 0;

	while(instance->should_close() == false && reason == editor::EditorExitReason::NotExiting) {
		
		// game->renderer->start_frame();
		instance->renderer->start_frame();
		ImGui::DockSpaceOverViewport(0, nullptr, ImGuiDockNodeFlags_PassthruCentralNode);
		game->update(0);
		reason = ed->render();
		ImGui::ShowStyleEditor();
		instance->renderer->end_frame();
		// game->renderer->end_frame();
	}

	Game::destroy_game_dll(game, dll);
	if(reason == editor::EditorExitReason::Reload) {
		goto reload_game;
	}

	delete ed;
	instance.reset();	

	Graphics::destroy_backend();
}

