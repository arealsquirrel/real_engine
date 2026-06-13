
#include "glm/ext/vector_float3.hpp"
#include "glm/ext/vector_float4.hpp"
#include "imgui.h"
#include <real/debug/cvars.hpp>
#include <real/core/color.hpp>

namespace real {

CVarSystem::CVarSystem() = default;
CVarSystem::~CVarSystem() = default;

CVarSystem &CVarSystem::get() {
    static CVarSystem system;
    return system;
}

void CVarSystem::render_imgui() {
    ImGui::Begin("CVars");
    for (auto &cv : cvars) {
        cv.second->render_imgui();
    }
    ImGui::End();
}

#define CVAR_DEFINITION(type, CVAR_TYPE_NAME) \
    template<> \
    CVarType CVar<type>::get_static_cvar_type() { return CVarType::CVAR_TYPE_NAME; } \
    template<> \
    CVarType CVar<type>::get_cvar_type() const { return CVarType::CVAR_TYPE_NAME; } \
    template<> \
    void CVar<type>::render_imgui()

CVAR_DEFINITION(float, Float) {
    switch (params) {
        case CVarEditParamers::ReadOnly:
            ImGui::Text("%s: %f", name.c_str(), value);
        case CVarEditParamers::ReadWrite:
            ImGui::InputFloat(name.c_str(), &value);
    }
}

CVAR_DEFINITION(Color4,Color4) {
    ImGui::ColorEdit4(name.c_str(), &value.r);
}

CVAR_DEFINITION(glm::vec4, Float4) {
    ImGui::InputFloat4(name.c_str(), &value[0]);
}

CVAR_DEFINITION(glm::vec3, Float3) {
    ImGui::InputFloat3(name.c_str(), &value[0]);
}

}
