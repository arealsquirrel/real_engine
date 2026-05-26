
#include "editor.hpp"
#include "imgui.h"
#include "real/core/core.hpp"
#include <real/config/config.hpp>
#include <string>
#include "panel.hpp"

namespace editor {

Editor::Editor(Shared<real::Instance> _instance) 
	: instance(_instance) {}

Editor::~Editor() {
}

void Editor::render() {
	ImGui::DockSpaceOverViewport(0, nullptr, ImGuiDockNodeFlags_PassthruCentralNode);

	if (ImGui::BeginMainMenuBar()) {
		std::string version_string = fmt::format(
				"real engine, version {}.{}.{}, {}",
				RL_VERSION_MAJOR, RL_VERSION_MINOR, RL_VERSION_PATCH,
				STRINGIFY_EXP(RL_BUILD_TYPE));
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(version_string.c_str()).x);
		ImGui::Text("%s", version_string.c_str());

	  	ImGui::EndMainMenuBar();
	}


	ImGui::Begin("editor");

	for(auto p : panels) {
		p->draw();
	}

	ImGui::End();
}

}

