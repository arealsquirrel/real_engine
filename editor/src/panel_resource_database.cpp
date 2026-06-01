
#include "panel_resource_database.hpp"
#include "imgui.h"
#include "real/core/types.hpp"
#include "real/resource/resource_database.hpp"

namespace editor {

void PanelResourceDatabase::draw() {
	using namespace real;

	ResourceDatabase *db = instance->resource_database.get();

	ImGui::Begin("Resource Database");

	static ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;
	ImVec2 outer_size = ImVec2(0.0f, 300);
	if (ImGui::BeginTable("Resources", 3, flags, outer_size)) {
		ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible
		ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_None);
		ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_None);
		ImGui::TableSetupColumn("Path", ImGuiTableColumnFlags_None);
		ImGui::TableHeadersRow();

		// Demonstrate using clipper for large vertical lists
		
		// ImGuiListClipper clipper;
		// clipper.Begin(db->resource_map.size());
		// while (clipper.Step()) {
		for (auto &[id, handle] : db->resource_map) {
			// auto &handle = db->resource_array[row];
			// real::ResourceDatabase::Entry e = db->uuid_to_entry[handle.get_uuid()];
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("idk bru");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%s", handle.get()->object_name());
			ImGui::TableSetColumnIndex(2);
			if(handle.get()->path.has_value())
				ImGui::Text("%s", handle.get()->path->c_str());
			else
				ImGui::Text("None");
		}

		ImGui::EndTable();
	}

	ImGui::End();
}

}

