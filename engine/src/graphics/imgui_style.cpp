
#include <real/graphics/imgui_style.hpp>

#include <imgui.h>

namespace real {

ImVec4 ImGuiIntRGBToFloatRGB(int r, int g, int b){
    return ImVec4(r / 256.0f, g / 256.0f, b / 256.0f, 1.0f);
}

void RealImguiStyle::style_imgui() {
    ImGuiStyle *style = &ImGui::GetStyle();
    style->WindowMinSize = ImVec2(160, 20);
    style->FramePadding = ImVec2(4, 4);
    style->ItemSpacing = ImVec2(6, 4);
    style->PopupRounding = 6.0f;
    style->ItemInnerSpacing = ImVec2(6, 4);
    style->Alpha = 0.95f;
    style->FrameRounding = 6.0f;
    style->IndentSpacing = 6.0f;
    style->ColumnsMinSpacing = 50.0f;
    style->GrabMinSize = 14.0f;
    style->GrabRounding = 16.0f;
    style->ScrollbarSize = 12.0f;
    style->ScrollbarRounding = 16.0f;
    style->WindowRounding = 8.0f;
	style->WindowBorderSize = 0;
	style->DockingSeparatorSize = 6.0f;
	style->SeparatorSize = 6.0f;
	style->CellPadding = ImVec2{10.0f, 6.0f};
	style->FramePadding = ImVec2{10.0f, 4.0f};
	style->ItemInnerSpacing = ImVec2{6.0f, 4.0f};
	style->ItemSpacing = ImVec2{6.0f, 6.0f};
    // style->DockingSeparatorSize = 6.0f;

    ImVec4 darkWindowBackground = ImGuiIntRGBToFloatRGB(29, 32, 33);
    ImVec4 windowBackground = ImGuiIntRGBToFloatRGB(40, 40, 40);
    ImVec4 textColor = ImGuiIntRGBToFloatRGB(235, 219, 178);
    ImVec4 textDisabled = ImGuiIntRGBToFloatRGB(235, 219, 178);

    // backgrounds for the things that are not windows
    // buttons and tabs and such
    ImVec4 background1 = ImGuiIntRGBToFloatRGB(60, 56, 54);
    ImVec4 background2 = ImGuiIntRGBToFloatRGB(80, 73, 69);
    ImVec4 background3 = ImGuiIntRGBToFloatRGB(102, 92, 84);

    ImVec4 active = ImGuiIntRGBToFloatRGB(152, 151, 26); // green
    ImVec4 hovered1 = ImGuiIntRGBToFloatRGB(184, 187, 38);
    ImVec4 hovered2 = ImGuiIntRGBToFloatRGB(184, 187, 38);

    style->Colors[ImGuiCol_Tab] = active;
    style->Colors[ImGuiCol_TabActive] = windowBackground;
    style->Colors[ImGuiCol_TabHovered] = hovered2;
    style->Colors[ImGuiCol_TabDimmedSelected] = active;
    style->Colors[ImGuiCol_TabDimmedSelectedOverline] = active;
    style->Colors[ImGuiCol_TabSelectedOverline] = background1;

    style->Colors[ImGuiCol_FrameBg] = background1;
    style->Colors[ImGuiCol_FrameBgHovered] = hovered2;
    style->Colors[ImGuiCol_FrameBgActive] = active;

    style->Colors[ImGuiCol_Text] = textColor;
    style->Colors[ImGuiCol_TextDisabled] = textDisabled;
    style->Colors[ImGuiCol_WindowBg] = windowBackground;
    style->Colors[ImGuiCol_Border] = darkWindowBackground;
    style->Colors[ImGuiCol_BorderShadow] = darkWindowBackground;

    style->Colors[ImGuiCol_TitleBg] = background1;
    style->Colors[ImGuiCol_TitleBgCollapsed] = background3;
    style->Colors[ImGuiCol_TitleBgActive] = active;

    style->Colors[ImGuiCol_MenuBarBg] = darkWindowBackground;

    style->Colors[ImGuiCol_ScrollbarBg] = background1;
    style->Colors[ImGuiCol_ScrollbarGrab] = darkWindowBackground;
    style->Colors[ImGuiCol_ScrollbarGrabHovered] = hovered2;
    style->Colors[ImGuiCol_ScrollbarGrabActive] = active;
    style->Colors[ImGuiCol_CheckMark] = active;

    style->Colors[ImGuiCol_SliderGrab] = background3;
    style->Colors[ImGuiCol_SliderGrabActive] = active;

    style->Colors[ImGuiCol_Button] = background1;
    style->Colors[ImGuiCol_ButtonHovered] = hovered1;
    style->Colors[ImGuiCol_ButtonActive] = active;

    style->Colors[ImGuiCol_Header] = background1;
    style->Colors[ImGuiCol_HeaderHovered] = hovered1;
    style->Colors[ImGuiCol_HeaderActive] = active;

    style->Colors[ImGuiCol_Separator] = darkWindowBackground;
    style->Colors[ImGuiCol_SeparatorHovered] = darkWindowBackground;

    style->Colors[ImGuiCol_SeparatorActive] = active;
    style->Colors[ImGuiCol_ResizeGrip] = background1;
    style->Colors[ImGuiCol_ResizeGripHovered] = hovered2;
    style->Colors[ImGuiCol_ResizeGripActive] = active;
    style->Colors[ImGuiCol_PlotLines] = active;
    style->Colors[ImGuiCol_PlotLinesHovered] = active;
    style->Colors[ImGuiCol_PlotHistogram] = active;
    style->Colors[ImGuiCol_PlotHistogramHovered] = active;
    style->Colors[ImGuiCol_TextSelectedBg] = textColor;
    style->Colors[ImGuiCol_PopupBg] = background2;

    style->Colors[ImGuiCol_DockingPreview] = active;
    style->Colors[ImGuiCol_DockingEmptyBg] = ImVec4(0, 0, 0, 0);

    ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF("../engine/resources/fonts/FiraMono-Regular.ttf", 15.0f);
}

}
