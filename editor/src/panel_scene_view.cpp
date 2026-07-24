
#include "panel_scene_view.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include "real/container/string_hash.hpp"
#include "real/graphics/graphics_system.hpp"
#include "real/graphics/sprite_renderer.hpp"
#include "real/scene/components.hpp"
#include "real/scene/entity.hpp"
#include <real/graphics/mesh_renderer.hpp>

namespace editor {

PanelSceneView::PanelSceneView(
    real::Ref<real::Instance> _instance, real::Ref<real::Scene> _scene)
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

	/*
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

	*/

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

    ImGui::InputFloat3("Position", &transform.position.x);
    ImGui::InputFloat3("Rotation", &transform.rotation.x);
    ImGui::InputFloat3("Scale", &transform.scale.x);

    draw_component<ComponentMeshRenderer>("Mesh Renderer", handle,
    [](auto &component) {
        ImGui::SeparatorText("Mesh Resource Data");
        ImGui::Text("sub mesh name: %s", component.sub_mesh.name.c_str());
        ImGui::Text("indices count: %lu", component.sub_mesh.count);
		
		if (ImGui::BeginCombo("Render Mode", "no")) {
			for (int i = 0; i < 4; i++) {
				const bool is_selected = (component.shader_mode == i);
				if (ImGui::Selectable(MeshRenderer::shader_mode_tostr[i], is_selected)) {
					component.shader_mode = (MeshRenderer::ShaderMode)i;
				}
				if (is_selected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			
			ImGui::EndCombo();
		}

		if(ImGui::TreeNode("Mesh Resource Submeshes")) {
			StringHash current_hash = StringHash(component.sub_mesh.name.c_str());
			if(ImGui::BeginCombo("Submesh", component.sub_mesh.name.c_str())) {
				int index = 0;
				for (const auto& pair : component.mesh.get()->meshes) {
					bool isSelected = (current_hash == pair.first);
					
					if (ImGui::Selectable(pair.second.name.c_str(), isSelected)) {
						component.sub_mesh = pair.second;
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

    draw_component<ComponentSpriteRenderer>("Sprite Renderer", handle,
    [](auto &sprite) {
        ImGui::ColorEdit4("Tint Color", &sprite.tint_color.r);
		constexpr u32 thumbnailSize = 128;
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		draw_list->AddCallback(ImGui::GetPlatformIO().DrawCallback_SetSamplerNearest, nullptr);
		auto tile = sprite.tile;
		auto [width, height] = sprite.texture.get()->texture->get_image_extent();
		ImVec2 size(tile.dimension.first, tile.dimension.second);
		float scale = std::min(thumbnailSize / size.x, thumbnailSize / size.y);
		ImVec2 display_size = ImVec2(size.x * scale, size.y * scale);
		ImVec2 uv0((float)tile.position.first / width, (float)tile.position.second / height);
		ImVec2 uv1(
				(float)(tile.position.first + tile.dimension.first) / width,
				(float)(tile.position.second + tile.dimension.second) / height);
		ImGui::Image(sprite.texture.get()->texture->get_imgui_textureID(), display_size, uv0, uv1);
		if(ImGui::TreeNode("Texture Resource Tiles")) {
			StringHash current_hash = StringHash(sprite.tile.name.c_str());
			if(ImGui::BeginCombo("Tile", sprite.tile.name.c_str())) {
				int index = 0;
				for (const auto& pair : sprite.texture.get()->tiles) {
					bool isSelected = (current_hash == pair.first);
					
					if (ImGui::Selectable(pair.second.name.c_str(), isSelected)) {
						sprite.tile = pair.second;
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
