
#include "panel_resource_viewer.hpp"
#include "imgui.h"
#include "real/resource/resource_database.hpp"

namespace editor {

PanelResourceViewer::PanelResourceViewer(
		Shared<real::Instance> _instance, 
		std::optional<real::ResourceHandle<real::Resource>> _current_handle)
	: Panel(_instance), current_handle(_current_handle) {}

PanelResourceViewer::~PanelResourceViewer() {

}

void PanelResourceViewer::draw() {
	using namespace real;

	ImGui::Begin("Resource Viewer", nullptr, ImGuiWindowFlags_MenuBar);

	if(current_handle.has_value() == false) {
		ImGui::Text("No resource, drag and drop one to view");
		ImGui::End();
		return;
	}

	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("Resource")) {
			if (ImGui::MenuItem("Reload - Disk")) { 

			}

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}
	
	ResourceDatabase::Entry entry = instance->resource_database->get_entry(current_handle->get_uuid());
	ImGui::Text("name: %s", entry.name.c_str());
	ImGui::Separator();
	ImGui::Text("uuid: %llu", entry.id.uuid);
	ImGui::Text("type: %s", current_handle->get()->get_class_name());
	ImGui::Separator();

	ImGui::End();
}

}

