#include "sidebar.h"
#include "../components/custom_button.h"
#include <imgui_internal.h>

namespace PCOptimizer {
namespace UI {

Sidebar::SidebarState Sidebar::s_state;
std::vector<NavigationItem> Sidebar::s_navigation_items;

void Sidebar::InitNavigationItems() {
    if (!s_navigation_items.empty()) return;
    
    s_navigation_items = {
        { "🏠", "HOME", NavigationPage::Home, {} },
        { "⚡", "PERFORMANCE", NavigationPage::Performance, {
            { "🎮", "Gaming Mode", NavigationPage::PerformanceGaming, {} },
            { "📡", "Streaming Mode", NavigationPage::PerformanceStreaming, {} },
            { "💼", "Workstation", NavigationPage::PerformanceWorkstation, {} },
            { "🔥", "Enthusiast", NavigationPage::PerformanceEnthusiast, {} }
        }},
        { "💻", "SYSTEM", NavigationPage::System, {
            { "⚙️", "CPU Optimizer", NavigationPage::SystemCPU, {} },
            { "🧠", "RAM Manager", NavigationPage::SystemRAM, {} },
            { "💾", "Disk Optimizer", NavigationPage::SystemDisk, {} },
            { "📊", "Hardware Monitor", NavigationPage::SystemHardwareMonitor, {} }
        }},
        { "🎨", "GRAPHICS", NavigationPage::Graphics, {
            { "🖥️", "GPU Settings", NavigationPage::GraphicsGPU, {} },
            { "📺", "Display Settings", NavigationPage::GraphicsDisplay, {} }
        }},
        { "🌐", "NETWORK", NavigationPage::Network, {
            { "🔌", "Connection Hub", NavigationPage::NetworkConnection, {} },
            { "⚡", "Latency Optimizer", NavigationPage::NetworkLatency, {} }
        }},
        { "🤖", "AI ANALYZER", NavigationPage::AIAnalyzer, {} },
        { "📋", "PROFILES", NavigationPage::Profiles, {
            { "✏️", "Custom Profiles", NavigationPage::ProfilesCustom, {} },
            { "💾", "Import/Export", NavigationPage::ProfilesImportExport, {} }
        }},
        { "⚙️", "SETTINGS", NavigationPage::Settings, {
            { "🔧", "Preferences", NavigationPage::SettingsPreferences, {} },
            { "ℹ️", "About", NavigationPage::SettingsAbout, {} }
        }}
    };
}

void Sidebar::DrawItemBackground(const ImVec2& min, const ImVec2& max, bool is_selected, bool is_hovered, float alpha) {
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    
    if (is_selected) {
        ImVec4 bg_color = LerpColor(g_theme.background_light, g_theme.accent_color, 0.3f);
        draw_list->AddRectFilled(min, max, ImGui::ColorConvertFloat4ToU32(bg_color), 4.0f);
        
        ImVec2 indicator_min = min;
        ImVec2 indicator_max = ImVec2(min.x + 4.0f, max.y);
        draw_list->AddRectFilled(indicator_min, indicator_max, 
                                ImGui::ColorConvertFloat4ToU32(g_theme.accent_color), 
                                2.0f);
    } else if (is_hovered && alpha > 0.0f) {
        ImVec4 hover_color = g_theme.background_light;
        draw_list->AddRectFilled(min, max, ColorWithAlpha(hover_color, alpha * 0.5f), 4.0f);
    }
}

void Sidebar::RenderItem(NavigationItem& item, NavigationPage& current_page, int depth) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) return;
    
    const ImVec2 cursor_pos = ImGui::GetCursorScreenPos();
    const float item_height = 44.0f;
    const float indent = depth * 20.0f;
    const float sidebar_width = GetWidth();
    
    ImVec2 item_min = ImVec2(cursor_pos.x + indent, cursor_pos.y);
    ImVec2 item_max = ImVec2(cursor_pos.x + sidebar_width - 8.0f, cursor_pos.y + item_height);
    
    ImRect bb(item_min, item_max);
    ImGui::ItemSize(bb);
    if (!ImGui::ItemAdd(bb, 0)) return;
    
    bool is_selected = (item.page == current_page && depth == 0);
    bool is_hovered = ImGui::IsMouseHoveringRect(item_min, item_max);
    
    static int hovered_index = -1;
    static int selected_index = -1;
    
    int item_index = (int)((uintptr_t)&item % 16);
    
    if (is_hovered && ImGui::IsMouseClicked(0)) {
        current_page = item.page;
        if (!item.submenu.empty()) {
            item.is_expanded = !item.is_expanded;
        }
    }
    
    float dt = ImGui::GetIO().DeltaTime;
    s_state.item_hover[item_index].SetTarget(is_hovered ? 1.0f : 0.0f);
    s_state.item_hover[item_index].Update(dt);
    
    DrawItemBackground(item_min, item_max, is_selected, is_hovered, s_state.item_hover[item_index].Get());
    
    ImVec2 icon_pos = ImVec2(item_min.x + 12.0f, item_min.y + (item_height - ImGui::CalcTextSize(item.icon).y) * 0.5f);
    ImVec2 label_pos = ImVec2(icon_pos.x + 32.0f, item_min.y + (item_height - ImGui::CalcTextSize(item.label).y) * 0.5f);
    
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    
    ImU32 icon_color = ImGui::ColorConvertFloat4ToU32(
        is_selected ? g_theme.accent_color : 
        is_hovered ? g_theme.text_primary : g_theme.text_secondary
    );
    
    ImU32 label_color = ImGui::ColorConvertFloat4ToU32(
        is_selected || is_hovered ? g_theme.text_primary : g_theme.text_secondary
    );
    
    if (depth == 0 && strlen(item.icon) > 0) {
        draw_list->AddText(icon_pos, icon_color, item.icon);
    }
    
    if (!s_state.collapsed || depth > 0) {
        draw_list->AddText(label_pos, label_color, item.label);
    }
    
    if (!item.submenu.empty() && !s_state.collapsed) {
        const char* arrow = item.is_expanded ? "▼" : "▶";
        ImVec2 arrow_size = ImGui::CalcTextSize(arrow);
        ImVec2 arrow_pos = ImVec2(item_max.x - arrow_size.x - 12.0f, 
                                 item_min.y + (item_height - arrow_size.y) * 0.5f);
        draw_list->AddText(arrow_pos, label_color, arrow);
    }
    
    ImGui::SetCursorScreenPos(ImVec2(cursor_pos.x, cursor_pos.y + item_height));
    
    if (item.is_expanded && !s_state.collapsed) {
        for (auto& subitem : item.submenu) {
            RenderItem(subitem, current_page, depth + 1);
        }
    }
}

void Sidebar::Render(NavigationPage& current_page) {
    InitNavigationItems();
    
    const float sidebar_width_expanded = 240.0f;
    const float sidebar_width_collapsed = 64.0f;
    
    float dt = ImGui::GetIO().DeltaTime;
    s_state.collapse.SetTarget(s_state.collapsed ? 1.0f : 0.0f);
    s_state.collapse.Update(dt);
    
    float current_width = sidebar_width_expanded - (sidebar_width_expanded - sidebar_width_collapsed) * s_state.collapse.Get();
    
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(current_width, ImGui::GetIO().DisplaySize.y));
    
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImGui::ColorConvertFloat4ToU32(g_theme.background_dark));
    
    ImGui::Begin("##Sidebar", nullptr, 
                ImGuiWindowFlags_NoTitleBar | 
                ImGuiWindowFlags_NoResize | 
                ImGuiWindowFlags_NoMove | 
                ImGuiWindowFlags_NoScrollbar |
                ImGuiWindowFlags_NoSavedSettings);
    
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 sidebar_min = ImGui::GetWindowPos();
    ImVec2 sidebar_max = ImVec2(sidebar_min.x + current_width, sidebar_min.y + ImGui::GetIO().DisplaySize.y);
    
    draw_list->AddLine(ImVec2(sidebar_max.x - 1, sidebar_min.y), 
                      ImVec2(sidebar_max.x - 1, sidebar_max.y),
                      ImGui::ColorConvertFloat4ToU32(g_theme.background_light), 1.0f);
    
    ImGui::SetCursorPosY(48.0f);
    
    for (auto& item : s_navigation_items) {
        RenderItem(item, current_page);
    }
    
    ImVec2 collapse_btn_pos = ImVec2(sidebar_min.x + 8.0f, sidebar_max.y - 48.0f);
    ImGui::SetCursorScreenPos(collapse_btn_pos);
    
    const char* collapse_icon = s_state.collapsed ? "▶" : "◀";
    if (CustomButton::Render(collapse_icon, ImVec2(current_width - 16.0f, 32.0f), ButtonStyle::Ghost)) {
        s_state.collapsed = !s_state.collapsed;
    }
    
    ImGui::End();
    
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(2);
}

void Sidebar::SetCollapsed(bool collapsed) {
    s_state.collapsed = collapsed;
}

bool Sidebar::IsCollapsed() {
    return s_state.collapsed;
}

float Sidebar::GetWidth() {
    const float sidebar_width_expanded = 240.0f;
    const float sidebar_width_collapsed = 64.0f;
    return sidebar_width_expanded - (sidebar_width_expanded - sidebar_width_collapsed) * s_state.collapse.Get();
}

} // namespace UI
} // namespace PCOptimizer
