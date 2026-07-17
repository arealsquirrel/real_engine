
#include "imgui.h"
#include "real/core/logging.hpp"
#include <real/debug/cvars.hpp>
#include <real/container/color.hpp>

namespace real {

CVarSystem::CVarSystem() = default;

CVarSystem::~CVarSystem() {
	if(cvars.empty() == false)
		RL_LOG_ERROR("ts gunna crash please call clear_cvars");
}

void CVarSystem::clear_cvars() {
	cvars.clear();
}

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

}
