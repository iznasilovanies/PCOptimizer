#include "topbar.h"
#include "../components/custom_button.h"
#include <imgui_internal.h>

namespace PCOptimizer {
namespace UI {

TopBar::TopBarState TopBar::s_state;

void TopBar::DrawLogo(const ImVec2& position) {
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    
    ImVec2 logo_pos = position;
    const char* logo_text = "PC OPTIMIZER";
    ImVec2 logo_size = ImGui::CalcTextSize(logo_text);
    
    draw_list->AddText(ImGui::GetFont(), 18.0f, logo_pos, 
                      ImGui::ColorConvertFloat4ToU32(g_theme.accent_color), 
                      logo_text);
    
    ImVec2 version_pos = ImVec2(logo_pos.x, logo_pos.y + logo_size.y + 2.0f);
    const char* version_text = "Premium v1.0";
    draw_list->AddText(ImGui::GetFont(), 10.0f, version_pos,
                      ImGui::ColorConvertFloat4ToU32(g_theme.text_secondary),
                      version_text);
}

void TopBar::DrawSystemTray(const ImVec2& position, const char* current_mode) {
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    
    ImVec2 mode_pos = position;
    const char* mode_prefix = "Mode: ";
    draw_list->AddText(mode_pos, ImGui::ColorConvertFloat4ToU32(g_theme.text_secondary), mode_prefix);
    
    ImVec2 mode_text_size = ImGui::CalcTextSize(mode_prefix);
    ImVec2 current_mode_pos = ImVec2(mode_pos.x + mode_text_size.x, mode_pos.y);
    draw_list->AddText(current_mode_pos, ImGui::ColorConvertFloat4ToU32(g_theme.accent_color), current_mode);
    
    ImVec2 perf_pos = ImVec2(mode_pos.x, mode_pos.y + 18.0f);
    const char* perf_text = "CPU: 23% | GPU: 67%";
    draw_list->AddText(ImGui::GetFont(), 11.0f, perf_pos,
                      ImGui::ColorConvertFloat4ToU32(g_theme.text_secondary),
                      perf_text);
}

void TopBar::DrawWindowControls(const ImVec2& position) {
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    
    const float button_size = 32.0f;
    const float button_spacing = 4.0f;
    
    ImVec2 close_pos = position;
    ImVec2 maximize_pos = ImVec2(close_pos.x - button_size - button_spacing, close_pos.y);
    ImVec2 minimize_pos = ImVec2(maximize_pos.x - button_size - button_spacing, maximize_pos.y);
    
    ImRect minimize_bb(minimize_pos, ImVec2(minimize_pos.x + button_size, minimize_pos.y + button_size));
    ImRect maximize_bb(maximize_pos, ImVec2(maximize_pos.x + button_size, maximize_pos.y + button_size));
    ImRect close_bb(close_pos, ImVec2(close_pos.x + button_size, close_pos.y + button_size));
    
    bool minimize_hovered = ImGui::IsMouseHoveringRect(minimize_bb.Min, minimize_bb.Max);
    bool maximize_hovered = ImGui::IsMouseHoveringRect(maximize_bb.Min, maximize_bb.Max);
    bool close_hovered = ImGui::IsMouseHoveringRect(close_bb.Min, close_bb.Max);
    
    float dt = ImGui::GetIO().DeltaTime;
    
    s_state.minimize_hover.SetTarget(minimize_hovered ? 1.0f : 0.0f);
    s_state.minimize_hover.Update(dt);
    
    s_state.maximize_hover.SetTarget(maximize_hovered ? 1.0f : 0.0f);
    s_state.maximize_hover.Update(dt);
    
    s_state.close_hover.SetTarget(close_hovered ? 1.0f : 0.0f);
    s_state.close_hover.Update(dt);
    
    if (minimize_hovered && ImGui::IsMouseClicked(0)) {
        s_state.wants_minimize = true;
    }
    
    if (maximize_hovered && ImGui::IsMouseClicked(0)) {
        s_state.wants_maximize = true;
    }
    
    if (close_hovered && ImGui::IsMouseClicked(0)) {
        s_state.wants_close = true;
    }
    
    ImVec4 minimize_bg = LerpColor(ImVec4(0, 0, 0, 0), g_theme.background_light, s_state.minimize_hover.Get());
    draw_list->AddRectFilled(minimize_bb.Min, minimize_bb.Max, 
                            ImGui::ColorConvertFloat4ToU32(minimize_bg), 4.0f);
    
    ImVec2 minimize_line_min = ImVec2(minimize_pos.x + 8.0f, minimize_pos.y + button_size / 2.0f);
    ImVec2 minimize_line_max = ImVec2(minimize_pos.x + button_size - 8.0f, minimize_pos.y + button_size / 2.0f);
    draw_list->AddLine(minimize_line_min, minimize_line_max, 
                      ImGui::ColorConvertFloat4ToU32(g_theme.text_primary), 2.0f);
    
    ImVec4 maximize_bg = LerpColor(ImVec4(0, 0, 0, 0), g_theme.background_light, s_state.maximize_hover.Get());
    draw_list->AddRectFilled(maximize_bb.Min, maximize_bb.Max, 
                            ImGui::ColorConvertFloat4ToU32(maximize_bg), 4.0f);
    
    ImVec2 maximize_rect_min = ImVec2(maximize_pos.x + 8.0f, maximize_pos.y + 8.0f);
    ImVec2 maximize_rect_max = ImVec2(maximize_pos.x + button_size - 8.0f, maximize_pos.y + button_size - 8.0f);
    draw_list->AddRect(maximize_rect_min, maximize_rect_max, 
                      ImGui::ColorConvertFloat4ToU32(g_theme.text_primary), 2.0f, 0, 2.0f);
    
    ImVec4 close_bg = LerpColor(ImVec4(0, 0, 0, 0), g_theme.error, s_state.close_hover.Get());
    draw_list->AddRectFilled(close_bb.Min, close_bb.Max, 
                            ImGui::ColorConvertFloat4ToU32(close_bg), 4.0f);
    
    ImVec2 close_center = ImVec2(close_pos.x + button_size / 2.0f, close_pos.y + button_size / 2.0f);
    float cross_size = 8.0f;
    draw_list->AddLine(ImVec2(close_center.x - cross_size, close_center.y - cross_size),
                      ImVec2(close_center.x + cross_size, close_center.y + cross_size),
                      ImGui::ColorConvertFloat4ToU32(g_theme.text_primary), 2.0f);
    draw_list->AddLine(ImVec2(close_center.x + cross_size, close_center.y - cross_size),
                      ImVec2(close_center.x - cross_size, close_center.y + cross_size),
                      ImGui::ColorConvertFloat4ToU32(g_theme.text_primary), 2.0f);
}

void TopBar::Render(const char* current_mode, float sidebar_width) {
    const float topbar_height = 48.0f;
    
    ImVec2 topbar_pos = ImVec2(sidebar_width, 0);
    ImVec2 topbar_size = ImVec2(ImGui::GetIO().DisplaySize.x - sidebar_width, topbar_height);
    
    ImGui::SetNextWindowPos(topbar_pos);
    ImGui::SetNextWindowSize(topbar_size);
    
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 12));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImGui::ColorConvertFloat4ToU32(g_theme.background_dark));
    
    ImGui::Begin("##TopBar", nullptr,
                ImGuiWindowFlags_NoTitleBar |
                ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoScrollbar |
                ImGuiWindowFlags_NoSavedSettings);
    
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 topbar_min = ImGui::GetWindowPos();
    ImVec2 topbar_max = ImVec2(topbar_min.x + topbar_size.x, topbar_min.y + topbar_size.y);
    
    draw_list->AddLine(ImVec2(topbar_min.x, topbar_max.y - 1),
                      ImVec2(topbar_max.x, topbar_max.y - 1),
                      ImGui::ColorConvertFloat4ToU32(g_theme.background_light), 1.0f);
    
    ImVec2 logo_pos = ImVec2(topbar_min.x + 16.0f, topbar_min.y + 8.0f);
    DrawLogo(logo_pos);
    
    ImVec2 system_tray_pos = ImVec2(topbar_max.x - 250.0f, topbar_min.y + 8.0f);
    DrawSystemTray(system_tray_pos, current_mode);
    
    ImVec2 window_controls_pos = ImVec2(topbar_max.x - 12.0f - 32.0f, topbar_min.y + 8.0f);
    DrawWindowControls(window_controls_pos);
    
    ImGui::End();
    
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(2);
}

bool TopBar::WantsMinimize() {
    bool result = s_state.wants_minimize;
    s_state.wants_minimize = false;
    return result;
}

bool TopBar::WantsMaximize() {
    bool result = s_state.wants_maximize;
    s_state.wants_maximize = false;
    return result;
}

bool TopBar::WantsClose() {
    bool result = s_state.wants_close;
    s_state.wants_close = false;
    return result;
}

} // namespace UI
} // namespace PCOptimizer
