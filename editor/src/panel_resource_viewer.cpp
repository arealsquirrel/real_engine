
#include "panel_resource_viewer.hpp"
#include "imgui.h"
#include "real/core/game.hpp"
#include "real/core/instance.hpp"
#include "real/resource/resource.hpp"
#include "real/resource/resource_image.hpp"
#include "real/resource/resource_mesh.hpp"
#include "real/resource/resource_shader.hpp"
#include <algorithm>
#include <optional>

namespace editor {

template<>
void PanelResourceViewer::display(real::ResourceShader *resource) {
	ImGui::Text("fields");
	for(const auto &field : resource->get_layout().fields) {
		ImGui::Text("name:%s type:%s format:%s offset:%lu loc:%lu dim:%i", 
				field.name.c_str(),
				real::ShaderDataType_to_string(field.data_type),
				real::ShaderFieldType_to_string(field.type),
				field.offset, field.location, field.array_size);
	}
}

template<>
void PanelResourceViewer::display(real::ResourceImage *resource) {
	auto [width, height] = resource->texture->get_image_extent();
	ImVec2 room_avail = ImGui::GetContentRegionAvail();

	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	draw_list->AddCallback(ImGui::GetPlatformIO().DrawCallback_SetSamplerNearest, nullptr);

	static float padding = 16.0f;
	static float thumbnailSize = 128.0f;
	float cellSize = thumbnailSize + padding;
	float panelWidth = ImGui::GetContentRegionAvail().x;
	int columnCount = (int)(panelWidth / cellSize);
	if (columnCount < 1)
		columnCount = 1;

	ImGui::Text("Tiles");
	ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 16, 512);
	ImGui::SliderFloat("Padding", &padding, 0, 32);

	ImGui::Columns(columnCount, 0, false);

	for (auto tile : resource->tiles) {
		ImVec2 size(tile.second.dimension.first, tile.second.dimension.second);
		float scale = std::min(thumbnailSize / size.x, thumbnailSize / size.y);
		ImVec2 display_size = ImVec2(size.x * scale, size.y * scale);

		ImVec2 uv0((float)tile.second.position.first / width, (float)tile.second.position.second / height);
		ImVec2 uv1((float)(tile.second.position.first + tile.second.dimension.first) / width, (float)(tile.second.position.second + tile.second.dimension.second) / height);

		ImGui::Image(resource->texture->get_imgui_textureID(), display_size, uv0, uv1);
		ImGui::TextWrapped("%s", tile.second.name.c_str());
		ImGui::NextColumn();
	}
}

template<>
void PanelResourceViewer::display(real::ResourceMesh *resource) {
}

PanelResourceViewer::PanelResourceViewer(
		real::Ref<real::Instance> _instance, 
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
			const char *data = (const char*)payload->Data;
			std::string cname(data, payload->DataSize);
			cname.resize(payload->DataSize);
			current_handle = std::make_optional(instance->resource_database->get_resource<Resource>(cname));
			name = cname;
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
	} else if(cn == "VulkanResourceMesh") {
		display<ResourceMesh>((ResourceMesh*)current_handle->get());
	} else {
		ImGui::Text("unknown resource");
	}

	ImGui::End();
}

}

