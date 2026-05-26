
#include "panel_logs.hpp"
#include "imgui.h"
#include "real/core/logging.hpp"
#include <cstdint>

namespace editor {

PanelLogs::PanelLogs(
		Shared<real::Instance> _instance, real::LogSink_Buffer *lb) 
	: Panel(_instance), log_buffer(lb) {}

void PanelLogs::draw() {
	ImGui::Begin("Log");

	ImGui::SameLine();
	bool clear = ImGui::Button("Clear");
	ImGui::SameLine();
	bool copy = ImGui::Button("Copy");
	ImGui::SameLine();
	
	ImGui::Separator();

	if (ImGui::BeginChild("scrolling", ImVec2(0, 0), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar)) {
		if(clear) {
			log_buffer->index = 0;
		} if (copy)
			ImGui::LogToClipboard();

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		
		static ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;
		ImVec2 avail_size = ImGui::GetContentRegionAvail();
		ImVec2 outer_size = ImVec2(0.0f, avail_size.y);

		if (ImGui::BeginTable("Log Entries", 4, flags, outer_size)) {
			ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible
			ImGui::TableSetupColumn("Level", ImGuiTableColumnFlags_WidthFixed, 50);
			ImGui::TableSetupColumn("Time", ImGuiTableColumnFlags_WidthFixed, 80);
			ImGui::TableSetupColumn("File", ImGuiTableColumnFlags_WidthFixed, 150);
			ImGui::TableSetupColumn("Log", ImGuiTableColumnFlags_None);
			ImGui::TableHeadersRow();
			
			ImGuiListClipper clipper;
			clipper.Begin(log_buffer->index);
			while (clipper.Step()) {
				for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++) {
					auto [str,data] = log_buffer->buffer[row];
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::Text("%s", real::LogLevel_to_string[data.level]);
					ImGui::TableSetColumnIndex(1);
					ImGui::Text("%s", data.time);
					ImGui::TableSetColumnIndex(2);
					ImGui::Text("%s:%i", data.file, data.line);
					ImGui::TableSetColumnIndex(3);
					ImGui::Text("%s", str.c_str());
				}
			}
			ImGui::EndTable();
		}
    	ImGui::PopStyleVar();
	}

	ImGui::EndChild();
	ImGui::End();
}

}

