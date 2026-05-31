
#include "panel_logs.hpp"
#include "imgui.h"
#include "real/core/instance.hpp"
#include "real/core/logging.hpp"
#include <cstdint>

namespace editor {

ImVec4 ImGuiIntRGBToFloatRGB(int r, int g, int b){
    return ImVec4(r / 256.0f, g / 256.0f, b / 256.0f, 1.0f);
}

PanelLogs::PanelLogs(
		Shared<real::Instance> _instance, real::LogSink_Buffer *lb) 
	: Panel(_instance), log_buffer(lb) {};

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
		
		static ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;
		ImVec2 avail_size = ImGui::GetContentRegionAvail();
		ImVec2 outer_size = ImVec2(0.0f, avail_size.y);

		if (ImGui::BeginTable("Log Entries", 2, flags, outer_size)) {
			ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 20);
			ImGui::TableSetupColumn("Logs", ImGuiTableColumnFlags_None);
			
			ImGuiListClipper clipper;
			clipper.Begin(log_buffer->index);
			while (clipper.Step()) {
				for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++) {
					auto [str,data] = log_buffer->buffer[log_buffer->index-row-1];
					ImColor col(1.0f, 1.0f, 1.0f, 1.0f);
					switch (data.level) {
					case real::LogLevel_Trace: col = ImGuiIntRGBToFloatRGB(152,151,26); break;
					case real::LogLevel_Info:  col = ImGuiIntRGBToFloatRGB(69,133,136); break;
					case real::LogLevel_Warn:  col = ImGuiIntRGBToFloatRGB(215,153,33); break;
					case real::LogLevel_Error: col = ImGuiIntRGBToFloatRGB(69,133,136); break;
					default: break;
					}
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::TextColored(col, "%s", real::LogLevel_to_string[data.level]);
					ImGui::TableSetColumnIndex(1);
					ImGui::TextColored(col, "%s", str.c_str());
					ImGui::TextColored(col, "%s %s:%i", data.time, data.file, data.line);
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

