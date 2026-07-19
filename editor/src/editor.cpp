
#include "editor.hpp"
#include "editor_camera.hpp"
#include "editor_gizmos.hpp"
#include "imgui.h"
#include "panel_resource_database.hpp"
#include "panel_resource_viewer.hpp"
#include "panel_scene_view.hpp"
#include "real/container/color.hpp"
#include "real/container/ref.hpp"
#include "real/core/core.hpp"
#include <GLFW/glfw3.h>
#include <memory>
#include <real/config/config.hpp>
#include <string>
#include "panel.hpp"
#include <fmt/format.h>
#include "real/core/game.hpp"
#include "real/core/instance.hpp"
#include "real/core/logging.hpp"
#include "real/graphics/framebuffer.hpp"
#include "real/graphics/graphics_system.hpp"
#include "real/graphics/renderpass_geometry.hpp"

namespace editor {

using namespace real;

Editor::Editor(real::Ref<real::Instance> _instance, real::ArgParams _params) 
	: instance(_instance), camera(_instance.get()), params(_params), gizmos(instance) {

	camera.block_input = true;
	editor_state = EditorState::Editing;
	camera.camera->clear_color = real::Color4(1.0f, 1.0f, 1.0f, 1.0f);
	active_scene = create_ref<Scene>(&_instance->engine_allocator, instance.get());
	viewport_framebuffer = Framebuffer::create(instance.get(), params.window_width, params.window_height, true, MultisamplingCount::Eight).to_ref();
}

Editor::~Editor() {
	destroy_game();
}

void Editor::step_game() {

}

void Editor::step_editor() {

}

void Editor::load_game(Path path) {
	auto [loaded_game, dll] = Game::load_game_dll(instance, params);
	game = loaded_game;
	game_loader = dll;
	game->scene = active_scene;
	game->screen_framebuffer = viewport_framebuffer;
	game->start();
	game->scene->awake();
	graphics_system = active_scene->get_system<GraphicsSystem>();

	add_panel<editor::PanelResourceDatabase>(&instance->engine_allocator);
	add_panel<editor::PanelResourceViewer>(&instance->engine_allocator);
	add_panel<editor::PanelSceneView>(&instance->engine_allocator, active_scene);
}

void Editor::destroy_game() {
	graphics_system.reset();
	panels.clear();
	active_scene->destroy();
	Game::destroy_game_dll(game, game_loader);
	RL_LOG_TRACE("scene ref count {}", active_scene->get_reference_count());
	active_scene.reset();
}

bool Editor::render(u32 delta_time) {
	ImGui::DockSpaceOverViewport(0, nullptr, ImGuiDockNodeFlags_PassthruCentralNode);
	exiting = instance->should_close();

	if(editor_state == EditorState::Editing) {
		instance->renderer->attach_camera(*camera.camera.get());
		viewport_framebuffer->clear_image(Color4(1,0,0,1));
		gizmos.draw_gizmos(active_scene);
		graphics_system->update(delta_time);
	} else {
		game->update(delta_time);
		active_scene->update(delta_time);
		graphics_system->bind_main_camera();
	}

	render_toolbar();
	render_viewport();
	render_engine_panel();

	for(auto p : panels) {
		p->draw();
	}

	camera.update(delta_time);

	return exiting;
}

void Editor::render_engine_panel() {
	ImGui::Begin("Engine Panel");
	
	switch (editor_state) {
		case EditorState::Editing: 
			if(ImGui::Button("run")) set_running(); break;
		case EditorState::Paused: break;
		case EditorState::Running: 
			if(ImGui::Button("edit")) set_editing(); break;
	}

	ImGui::Text("Object Count: %u", real::Object::get_object_count());
	ImGui::Text("Stack Allocator mem: %u/%u", instance->frame_allocator.allocated_mem, instance->frame_allocator.alloc_size);
	ImGui::Text("System Allocator mem: %u/%u", instance->system_allocator.allocated_mem, instance->system_allocator.alloc_size);
	ImGui::Text("Engine Allocator mem: %u/%u", instance->engine_allocator.allocated_mem, instance->engine_allocator.alloc_size);

	LinkedListAllocator::Header *selected_block = nullptr;
	u32 total_size = 0;
	for(auto *iter = instance->engine_allocator.list_begin; iter != nullptr; iter = iter->next) {
		total_size += iter->size + sizeof(LinkedListAllocator::Header);
		ImVec4 col = iter->used ? ImVec4{1.0f, 0.0f, 0.0f, 1.0f} : ImVec4{0.0f, 1.0f, 0.0f, 1.0f};
		ImGui::TextColored(col, "[%p] size: %u", iter, iter->size);
	}

	ImGui::SeparatorText("Renderer");
	u32 micro_seconds = instance->renderer->render_stats.frame_time.micro_seconds; 
	ImGui::Text("Frametime %f ms", micro_seconds / 1000.0f);
	ImGui::Text("FPS: %f", 1000.0f / (micro_seconds / 1000.0f));
	ImGui::Text("Indices: %u", instance->renderer->render_stats.indicies);
	ImGui::Text("Vertices: %u", instance->renderer->render_stats.verticies);
	ImGui::SeparatorText("Window");
	auto [width, height] = instance->window->get_glfw_window_dimensions();
	ImGui::Text("Window size, x: %u, y: %u", width, height);
	auto [xpos, ypos] = instance->window->get_mouse_position();
	ImGui::Text("Mouse position: x: %f, y: %f", xpos, ypos);

	ImGui::SeparatorText("Current Game");

	for (auto &sys : active_scene->systems) {
		sys->draw_imgui();
	}

	ImGui::End(); 
}

void Editor::render_toolbar() {
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("Project")) {
			if (ImGui::MenuItem("Exit")) {
				exiting = true;
			}

			if (ImGui::MenuItem("Reload")) { 
			}

			ImGui::EndMenu();
		}

		std::string version_string = fmt::format(
				"real engine, version {}.{}.{}, {}",
				RL_VERSION_MAJOR, RL_VERSION_MINOR, RL_VERSION_PATCH,
				STRINGIFY_EXP(RL_BUILD_TYPE));
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(version_string.c_str()).x);
		ImGui::Text("%s", version_string.c_str());

	  	ImGui::EndMainMenuBar();
	}
}

void Editor::render_viewport() {
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
	ImGui::Begin("Viewport");

	if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
		glfwSetInputMode(instance->window->glfw_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		camera.block_input = false;
	}

	if(instance->window->input->poll_key(GLFW_KEY_ESCAPE)) {
		camera.block_input = true;
		glfwSetInputMode(instance->window->glfw_window(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	ImVec2 windowSize = ImGui::GetContentRegionAvail();
	ImVec2 imageSize = ImVec2((float)1280, (float)720);
	float imgAspect = imageSize.x / imageSize.y;
	float windowAspect = windowSize.x / windowSize.y;
	ImVec2 drawSize;
	if (windowAspect > imgAspect) {
		drawSize = ImVec2(windowSize.y * imgAspect, windowSize.y);
	} else {
		drawSize = ImVec2(windowSize.x, windowSize.x / imgAspect);
	}

	auto id = viewport_framebuffer->get_color_resolve_image().get()->get_imgui_textureID();
	ImGui::Image(id, drawSize);

	ImGui::End();
	ImGui::PopStyleVar();
}

void Editor::set_editing() {
	switch (editor_state) {
		case EditorState::Editing: break;
		case EditorState::Paused: break;
		case EditorState::Running:
			/*
			game->shutdown();
			active_scene.reset();
			active_scene = create_ref<Scene>(&instance->engine_allocator, instance.get());
			game->scene = active_scene;
			game->start();
			game->scene->awake();
			editor_state = EditorState::Editing;
			RL_LOG_INFO("began editing game");
			*/
			return;
	}
}

void Editor::set_running() {
	switch (editor_state) {
		case EditorState::Editing: 
			/*
			game->shutdown();
			active_scene.reset();
			active_scene = create_ref<Scene>(&instance->engine_allocator, instance.get());
			game->scene = active_scene;
			game->start();
			game->scene->awake();
			editor_state = EditorState::Running;
			RL_LOG_INFO("began running game");
			*/
			return;

		case EditorState::Paused: break;
		case EditorState::Running: break;
	}
}

void Editor::set_paused() {
	switch (editor_state) {
		case EditorState::Editing: break;
		case EditorState::Paused: break;
		case EditorState::Running: break;

	}
}

}

