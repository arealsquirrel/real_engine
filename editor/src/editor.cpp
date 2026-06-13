
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
	ImGui::Image(id, drawSize, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

	ImGui::End();
	ImGui::PopStyleVar();
}

}

