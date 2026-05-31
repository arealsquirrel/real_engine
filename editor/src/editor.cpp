
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

	ImGui::DockSpaceOverViewport(0, nullptr, ImGuiDockNodeFlags_PassthruCentralNode);

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

	for(auto p : panels) {
		p->draw();
	}

	return r;
}

}

