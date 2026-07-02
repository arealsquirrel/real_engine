
#include "editor.hpp"
#include "imgui.h"
#include "real/core/core.hpp"
#include <real/config/config.hpp>
#include <string>
#include "panel.hpp"
#include <fmt/format.h>
#include "real/core/game.hpp"
#include "real/core/instance.hpp"

namespace editor {

Editor::Editor(Shared<real::Instance> _instance) 
	: instance(_instance) {}

Editor::~Editor() {
}

EditorExitReason Editor::render() {
	EditorExitReason r = EditorExitReason::NotExiting;

	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("Project")) {
			if (ImGui::MenuItem("Exit")) { 
				r = EditorExitReason::Exit;
			}

			if (ImGui::MenuItem("Reload")) { 
				r = EditorExitReason::Reload;
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

	viewport();

	for(auto p : panels) {
		p->draw();
	}

	ImGui::Begin("Engine Panel");
	ImGui::Text("Object Count: %u", real::Object::get_object_count());
	ImGui::Text("Stack Allocator mem: %u/%u", instance->frame_allocator.allocated_mem, instance->frame_allocator.alloc_size);
	ImGui::Text("System Allocator mem: %u/%u", instance->system_allocator.allocated_mem, instance->system_allocator.alloc_size);
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
	ImGui::End(); 

	return r;
}

void Editor::viewport() {
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
	ImGui::Begin("Viewport");
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

	auto id = editor_viewport.get()->get_imgui_textureID();
	ImGui::Image(id, drawSize);

	ImGui::End();
	ImGui::PopStyleVar();
}

}

