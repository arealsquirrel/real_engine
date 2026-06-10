
#include "imgui.h"
#include "real/core/game.hpp"
#include <real/graphics/renderer.hpp>

namespace real {

Renderer::Renderer(Instance *_instance, Shared<Window> _window) 
    : Object(_instance), window(_window) {}

Renderer::~Renderer() = default;

void Renderer::imgui_draw_stats() {
    ImGui::Begin("Renderer Stats");

    // Sample data array
    static float fps_data[500] = { 0.2f, 0.6f, 0.4f, 0.8f, 0.3f, 0.9f, 0.5f };
    static int frame = 0;
    float ffps = 1 / (stats.frame_time.micro_seconds / 1000.0f / 1000.0f);
    fps_data[frame] = ffps; 
    frame = (frame + 1) % 500;

    ImGui::Text("Indicies %u", stats.indicies);
    ImGui::Text("Verticies %u", stats.verticies);
    ImGui::Text("Draw calls %u", stats.draw_calls);
    ImGui::Text("Frame time %f", stats.frame_time.micro_seconds / 1000.0f);
    ImGui::Text("FPS %f", ffps);
    ImGui::PlotLines("FPS over time", fps_data, IM_ARRAYSIZE(fps_data), 0, NULL, 0.0f, 1000.0f, ImVec2(300, 50));
    ImGui::End();
}

}
