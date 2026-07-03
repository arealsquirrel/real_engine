
#include "panel_scene_view.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include "real/core/string_hash.hpp"
#include "real/graphics/graphics_system.hpp"
#include "real/scene/components.hpp"
#include "real/scene/entity.hpp"

namespace editor {

PanelSceneView::PanelSceneView(
    Shared<real::Instance> _instance, Shared<real::Scene> _scene)
    : Panel(_instance), scene(_scene) {}

PanelSceneView::~PanelSceneView() = default;

void PanelSceneView::draw() {
    ImGui::Begin("Scene View");

    auto view = scene->registry->view<real::ComponentID>();
    for (auto [ent, tr] : view->each()) {
        ImGuiTreeNodeFlags entityTreeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanAvailWidth;
        if (ImGui::TreeNodeEx(tr.name.c_str(), entityTreeFlags)) {
            if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
                handle = real::EntityHandle(ent, scene.get());
            }

            ImGui::TreePop();
        }
    }

	ImGui::SeparatorText("Systems");

	for (auto sys : scene->systems) {
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{4, 6});
		float lineHeight = GImGui->Font->LegacySize + GImGui->Style.FramePadding.y * 2.0f;
		const ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
		bool open = ImGui::TreeNodeEx(sys->object_typeinfo()->name, flags);
    	ImGui::PopStyleVar();

		if (open) {
			((real::System*)sys.get())->draw_imgui();
			ImGui::TreePop();
		}
	}

    ImGui::End();

    ImGui::Begin("Entity Properties");

    if (handle) {
        render_properties();
    }

    ImGui::End();
}

template <class T, typename F>
void draw_component(const char *name, real::EntityHandle entity, const F &drawFunction) {
    using namespace real;

    if (!entity.HasComponent<T>())
        return;

    auto &component = entity.GetComponent<T>();

    auto contentRegionAvail = ImGui::GetContentRegionAvail();

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{4, 6});
    float lineHeight = GImGui->Font->LegacySize + GImGui->Style.FramePadding.y * 2.0f;
    ImGui::Separator();
    const ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
    bool open = ImGui::TreeNodeEx(name, flags);
    bool deleteComponent = false;
    ImGui::PopStyleVar();
    ImGui::SameLine(contentRegionAvail.x - lineHeight * 0.5f);
    if (ImGui::Button("+", ImVec2{lineHeight, lineHeight})) {
        ImGui::OpenPopup("ComponentPopUp");
    }

    if (ImGui::BeginPopup("ComponentPopUp")) {
        if (ImGui::MenuItem("Delete Component"))
            deleteComponent = true;

        ImGui::EndPopup();
    }

    if (open) {
        drawFunction(component);
        ImGui::TreePop();
    }
}

void PanelSceneView::render_properties() {
    using namespace real;

    auto &tag = handle.GetComponent<ComponentID>();
    auto &transform = handle.GetComponent<ComponentTransform>();
    ImGui::Text("%s", tag.name.c_str());
    ImGui::Text("%llu", tag.id.uuid);

    ImGui::SeparatorText("Transform");

    ImGui::InputFloat3("Position", &transform.position[0]);
    ImGui::InputFloat3("Rotation", &transform.rotation[0]);
    ImGui::InputFloat3("Scale", &transform.scale[0]);

    draw_component<ComponentMeshRenderer>("Mesh Renderer", handle,
    [](auto &component) {
        ImGui::SeparatorText("Mesh Resource Data");
        ImGui::Text("sub mesh name: %s", component.sub_mesh.name.c_str());
        ImGui::Text("indices count: %lu", component.sub_mesh.count);
		if(ImGui::TreeNode("Mesh Resource Submeshes")) {
			StringHash current_hash = StringHash(component.sub_mesh.name.c_str());
			if(ImGui::BeginCombo("Submesh", component.sub_mesh.name.c_str())) {
				int index = 0;
				for (const auto& pair : component.mesh.get()->meshes) {
					bool isSelected = (current_hash == pair.first);
					
					if (ImGui::Selectable(pair.second.name.c_str(), isSelected)) {
						component.sub_mesh = pair.second;
						// selectedKey = pair.first; 
					}

					if (isSelected) {
						ImGui::SetItemDefaultFocus();
					}
					index++;
				}

				ImGui::EndCombo();
			}

			ImGui::TreePop();
		}
    });

    draw_component<ComponentCamera>("Camera", handle,
    [](auto &cam) {
        ImGui::ColorEdit4("Clear Color", &cam.clear_color.r);
    });
}

}
