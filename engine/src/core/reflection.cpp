
#include "imgui.h"
#include <cassert>
#include <real/core/reflection.hpp>
#include <real/math/vec2.hpp>
#include <real/math/vec3.hpp>
#include <real/math/vec4.hpp>
#include <string>

namespace real::reflect {

std::map<size_t, const reflect::Type*> Reflection::type_map;

FieldView TypeReflector::view_field(std::string name) {
	assert(type->fields.count(name) != 0);
	return FieldView(type->fields.at(name), view, offset);
}

bool TypeReflector::has_field(std::string name) {
	return (type->fields.count(name) != 0);
}

TypeReflector FieldView::next() {
	assert(field.next != nullptr);
	return TypeReflector(field.next, view, offset+field.offset);
}

bool FieldView::has_next() {
	return (field.next != nullptr);
}

}

namespace real {

void draw_field_imgui(char *mem, reflect::Field field, ImGuiID id) {
	ImGui::PushID(id);
	switch (field.primitive_type) {
		case reflect::PrimitiveTypes::t_struct:
			reflect_imgui(reflect::TypeReflector(field.next, mem, 0), id);
			break;
		case reflect::PrimitiveTypes::t_Vec2:
			ImGui::InputFloat2(field.field_name.c_str(), &((Vec2*)mem)->x);
			break;
		case reflect::PrimitiveTypes::t_Vec3:
			ImGui::InputFloat3(field.field_name.c_str(), &((Vec3*)mem)->x);
			break;
		case reflect::PrimitiveTypes::t_Vec4:
			ImGui::InputFloat4(field.field_name.c_str(), &((Vec4*)mem)->x);
			break;
		case reflect::PrimitiveTypes::t_int:
			ImGui::InputInt(field.field_name.c_str(), (int*)(mem));
			break;
		case reflect::PrimitiveTypes::t_float:
			ImGui::InputFloat(field.field_name.c_str(), (float*)(mem));
			break;
		default:
			ImGui::Text("%s not drawable at the moment", field.field_name.c_str());
			break;
	}
	ImGui::PopID();
}

void draw_field_imgui_array(char *mem, reflect::Field field, ImGuiID id) {
	const size_t type_size = field.field_size / field.array_dims;
	ImGui::PushID(id+field.offset);
	if(ImGui::TreeNode(field.field_name.c_str())) {
		for (size_t i = 0; i < field.array_dims; i++) {
			ImGui::PushID(id+i+field.offset+1);
			if(ImGui::TreeNode(std::to_string(i).c_str())) {
				draw_field_imgui(mem+(type_size*i), field, id);
				ImGui::TreePop();
			}
			ImGui::PopID();
		}

		ImGui::TreePop();
	}
	ImGui::PopID();
}

void draw_field_imgui_vector(char *mem, reflect::Field field, ImGuiID id) {
	
}

void reflect_imgui(reflect::TypeReflector refl, ImGuiID id) {
	ImGui::Text("%s", refl.type->name.c_str());

	for (auto [key, value] : refl.type->fields) {
		char *mem = refl.view+refl.offset+value.offset;
		
		if(value.storage_structure == reflect::StorageStructure::Array) {
			draw_field_imgui_array(mem, value, id);
		} else if(value.storage_structure == reflect::StorageStructure::Vector) {
			draw_field_imgui_vector(mem, value, id);
		} else {
			draw_field_imgui(mem, value, id);
		}
	}
}

}

