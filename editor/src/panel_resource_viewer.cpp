
#include "panel_resource_viewer.hpp"
#include "imgui.h"
#include "real/core/game.hpp"
#include "real/core/instance.hpp"
#include "real/resource/resource.hpp"
#include "real/resource/resource_image.hpp"
#include "real/resource/resource_shader.hpp"
#include <algorithm>
#include <optional>

namespace editor {

template<>
void PanelResourceViewer::display(real::ResourceShader *resource) {
	ImGui::Text("fields");
	for(auto &field : resource->layout.fields) {
		ImGui::Text("name:%s type:%s format:%s offset:%lu loc:%lu dim:%i", 
				field.name.c_str(),
				real::ShaderDataType_to_string(field.data_type),
				ShaderFieldType_to_string(field.type),
				field.offset, field.location, field.array_size);
	}
}

template<>
void PanelResourceViewer::display(real::ResourceImage *resource) {
	auto [width, height] = resource->get_image_extent();
	ImVec2 room_avail = ImGui::GetContentRegionAvail();
	float scale = std::min(room_avail.x / width, room_avail.y / height);
	ImVec2 display_size = ImVec2(width * scale, height * scale);
	ImGui::Image(resource->get_imgui_textureID(), display_size);
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

	ImVec2 available_size = ImGui::GetContentRegionAvail();
	ImGui::Dummy(available_size);
	ImGui::SetCursorPos(ImGui::GetWindowContentRegionMin());

	if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("REAL_RESOURCE_NAME")) {
           	// IM_ASSERT(payload->DataSize == sizeof(std::string));
			// std::string *p = (std::string*)payload->Data;
			std::string cname = std::string((const char*)payload->Data);
			current_handle = std::make_optional(instance->resource_database->get_resource<Resource>(cname));
			name = cname;
			// auto handle = instance->resource_database->get_resource<Resource>(name);

			// RL_LOG_TRACE("drag and drop {}", name);
        }

    	ImGui::EndDragDropTarget();
    }

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
	
	ImGui::Text("name: %s", name.c_str());
	ImGui::Separator();
	ImGui::Text("uuid: %llu", current_handle->get_uuid().uuid);
	ImGui::Text("type: %s", current_handle->get()->object_name());
	ImGui::Separator();

	std::string cn = current_handle->get()->object_name();
	if(cn == "VulkanResourceShader") {
		display<ResourceShader>((ResourceShader*)current_handle->get());
	} else if(cn == "VulkanResourceImage") {
		display<ResourceImage>((ResourceImage*)current_handle->get());
	} else {
		ImGui::Text("unknown resource");
	}

	ImGui::End();
}

}

