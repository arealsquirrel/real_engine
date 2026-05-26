
#include "panel_resource_viewer.hpp"
#include "imgui.h"
#include "real/resource/resource.hpp"
#include "real/resource/resource_database.hpp"
#include "real/resource/resource_shader.hpp"

namespace editor {

template<>
void PanelResourceViewer::display(real::ResourceShader *resource) {
	ImGui::Text("%s", real::ShaderType_to_string(resource->type));

	ImGui::Text("fields");
	for(auto &field : resource->layout.fields) {
		ImGui::Text("%s %s %s %lu", 
				field.name.c_str(),
				real::ShaderFieldType_to_string(field.type),
				real::ShaderDataType_to_string(field.data),
				field.offset);
	}
}

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

	std::string cn = current_handle->get()->get_class_name();
	if(cn == "ResourceShader") {
		display<ResourceShader>((ResourceShader*)current_handle->get());
	} else {
		ImGui::Text("unknown resource");
	}

	ImGui::End();
}

}

