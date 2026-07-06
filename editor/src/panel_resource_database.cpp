
#include "panel_resource_database.hpp"
#include "imgui.h"
#include "panel.hpp"
#include "real/core/string_hash.hpp"
#include "real/core/types.hpp"
#include "real/resource/resource_database.hpp"
#include "real/resource/resource_image.hpp"
#include <string>

namespace editor {

PanelResourceDatabase::PanelResourceDatabase(
	Shared<real::Instance> _instance)
	: Panel(_instance) {

	editor_icons = instance->resource_database->get_resource<real::ResourceImage>("editor_icons.png");
	file_tile = editor_icons.get()->tiles.at(real::StringHash("file"));
}


void PanelResourceDatabase::draw() {
	using namespace real;

	ResourceDatabase *db = instance->resource_database.get();

	ImGui::Begin("Resource Database");

	/*
	static ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;
	ImVec2 outer_size = ImVec2(0.0f, 300);
	if (ImGui::BeginTable("Resources", 3, flags, outer_size)) {
		ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible
		ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_None);
		ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_None);
		ImGui::TableSetupColumn("Path", ImGuiTableColumnFlags_None);
		ImGui::TableHeadersRow();

		for (auto &[name, id] : db->name_to_resource_UUID) {
			auto &handle = db->resource_map[id.id];

			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);

			bool selected;
			ImGui::Selectable(id.name.c_str(), selected, ImGuiSelectableFlags_SpanAllColumns);

			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
                ImGui::SetDragDropPayload("REAL_RESOURCE_NAME", id.name.c_str(), id.name.size());
                ImGui::Text("Moving: %s", id.name.c_str()); 
                ImGui::EndDragDropSource();
            }

			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%s", handle.get()->object_name());
			ImGui::TableSetColumnIndex(2);
			if(id.load_path)
				ImGui::Text("%s", id.load_path->c_str());
			else
				ImGui::Text("None");
		}

		ImGui::EndTable();
	}
	*/

	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	draw_list->AddCallback(ImGui::GetPlatformIO().DrawCallback_SetSamplerNearest, nullptr);

	static float padding = 16.0f;
	static float thumbnailSize = 128.0f;
	float cellSize = thumbnailSize + padding;
	float panelWidth = ImGui::GetContentRegionAvail().x;
	int columnCount = (int)(panelWidth / cellSize);
	if (columnCount < 1)
		columnCount = 1;

	ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 16, 512);
	ImGui::SliderFloat("Padding", &padding, 0, 32);
	ImGui::Columns(columnCount, 0, false);

	auto [width, height] = editor_icons.get()->get_image_extent();
	ImVec2 room_avail = ImGui::GetContentRegionAvail();

	for (auto &[name, id] : db->name_to_resource_UUID) {
		ImVec2 size(file_tile.dimension.first, file_tile.dimension.second);
		float scale = std::min(thumbnailSize / size.x, thumbnailSize / size.y);
		ImVec2 display_size = ImVec2(size.x * scale, size.y * scale);

		ImVec2 uv0((float)file_tile.position.first / width, (float)file_tile.position.second / height);
		ImVec2 uv1((float)(file_tile.position.first + file_tile.dimension.first) / width, (float)(file_tile.position.second + file_tile.dimension.second) / height);

		ImGui::Image(editor_icons.get()->get_imgui_textureID(), display_size, uv0, uv1);

		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
			ImGui::SetDragDropPayload("REAL_RESOURCE_NAME", id.name.c_str(), id.name.size());
			ImGui::Text("Moving: %s", id.name.c_str()); 
			ImGui::EndDragDropSource();
		}

		ImGui::TextWrapped("%s", id.name.c_str());
		ImGui::NextColumn();
	}

	ImGui::End();
}

}

