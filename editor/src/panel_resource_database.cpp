
#include "panel_resource_database.hpp"
#include "imgui.h"
#include "real/core/instance.hpp"
#include "real/core/types.hpp"
#include "real/resource/resource_database.hpp"
#include <string>
#include <real/resource/resource_image.hpp>

namespace editor {

PanelResourceDatabase::PanelResourceDatabase(Shared<real::Instance> _instance)
	: Panel(_instance) {

	icon_file = instance->resource_database->get_resource<real::ResourceImage>("file.png");
	icon_audio = instance->resource_database->get_resource<real::ResourceImage>("file_audio.png");
	icon_code = instance->resource_database->get_resource<real::ResourceImage>("file_code.png");
	icon_image = instance->resource_database->get_resource<real::ResourceImage>("file_image.png");
	icon_zipper = instance->resource_database->get_resource<real::ResourceImage>("file_zipper.png");
	icon_folder = instance->resource_database->get_resource<real::ResourceImage>("folder.png");

	project_root = "."; // set it to where ever the binary is run from
}

real::ResourceImage *PanelResourceDatabase::get_icon_from_path(Path path) {
	real::ResourceImage *icon = icon_file.get();

	if(path.extension() == ".png") {
		return icon_image.get();
	}

	if(path.extension() == ".cpp") {
		return icon_code.get();
	}

	if(path.extension() == ".hpp") {
		return icon_code.get();
	}

	return icon;
}

void PanelResourceDatabase::draw() {
	ImGui::Begin("Resource Database", nullptr, ImGuiWindowFlags_MenuBar);

	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("View")) {
			if (ImGui::MenuItem("Filesystem")) render_filesystem = true;
			if (ImGui::MenuItem("Database"))   render_filesystem = false;
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	if(render_filesystem)
		draw_filesystem_view(project_root);
	else
		draw_resource_icon_view();

	ImGui::End();
}

void PanelResourceDatabase::draw_resource_icon_view() {
	using namespace real;
	ResourceDatabase *db = instance->resource_database.get();
	ImVec2 room_avail = ImGui::GetContentRegionAvail();
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	draw_list->AddCallback(ImGui::GetPlatformIO().DrawCallback_SetSamplerNearest, nullptr);

	static float padding = 16.0f;
	static float thumbnailSize = 80.0f;
	float cellSize = thumbnailSize + padding;
	float panelWidth = ImGui::GetContentRegionAvail().x;
	int columnCount = (int)(panelWidth / cellSize);
	if (columnCount < 1)
		columnCount = 1;

	ImGui::Columns(columnCount, 0, false);

	for (auto &[name, id] : db->name_to_resource_UUID) {
		ImVec2 size(64.0f, 64.0f);
		float scale = std::min(thumbnailSize / size.x, thumbnailSize / size.y);
		ImVec2 display_size = ImVec2(size.x * scale, size.y * scale);

		ResourceImage *icon = icon_file.get();
		if(id.load_path.has_value()) icon = get_icon_from_path(id.load_path.value());

		ImGui::ImageWithBg(icon->get_imgui_textureID(), display_size, {0,0}, {1,1}, {0,0,0,0}, {0.2705882, 0.521, 0.533, 1.0f});

		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
			ImGui::SetDragDropPayload("REAL_RESOURCE_NAME", name.c_str(), name.size());
			ImGui::Text("Moving: %s", name.c_str()); 
			ImGui::EndDragDropSource();
		}

		ImGui::TextWrapped("%s", id.name.c_str());
		ImGui::NextColumn();
	}
}

void PanelResourceDatabase::draw_filesystem_view(Path path) {
	using namespace real;
	for (const auto& entry : std::filesystem::directory_iterator(path)) {
		bool isDirectory = entry.is_directory();
        
        // Push a unique ID for ImGui's internal stack
        ImGui::PushID(entry.path().string().c_str());

        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_DrawLinesFull | ImGuiTreeNodeFlags_DrawLinesToNodes;
        if (!isDirectory) {
            flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
        }

		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0, 0, 0, 0));
        bool open = ImGui::TreeNodeEx(("###"+entry.path().filename().string()).c_str(), flags);
		
		ImGui::SameLine();
		ResourceImage *icon = (entry.is_directory() == true) ? icon_folder.get() : get_icon_from_path(entry.path());
		ImGui::ImageWithBg(icon->get_imgui_textureID(), {20,20}, {0,0}, {1,1}, {0,0,0,0}, {0.596, 0.592, 0.101, 1.0f});
		ImGui::SameLine();
		ImGui::Text("%s", entry.path().filename().c_str());
		ImGui::PopStyleColor(2);

        // if (ImGui::IsItemClicked()) {
        // }

        if (isDirectory && open) {
            draw_filesystem_view(entry.path());
            ImGui::TreePop();
        }

        ImGui::PopID();
	}
}

}

