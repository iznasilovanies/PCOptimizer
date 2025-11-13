#pragma once
#include "../components/ui_common.h"
#include <string>
#include <vector>
#include <functional>

namespace PCOptimizer {
namespace UI {

enum class NavigationPage {
    Home,
    
    Performance,
    PerformanceGaming,
    PerformanceStreaming,
    PerformanceWorkstation,
    PerformanceEnthusiast,
    
    System,
    SystemCPU,
    SystemRAM,
    SystemDisk,
    SystemHardwareMonitor,
    
    Graphics,
    GraphicsGPU,
    GraphicsDisplay,
    
    Network,
    NetworkConnection,
    NetworkLatency,
    
    AIAnalyzer,
    
    Profiles,
    ProfilesCustom,
    ProfilesImportExport,
    
    Settings,
    SettingsPreferences,
    SettingsAbout
};

struct NavigationItem {
    const char* icon;
    const char* label;
    NavigationPage page;
    std::vector<NavigationItem> submenu;
    bool is_expanded = false;
};

class Sidebar {
public:
    static void Render(NavigationPage& current_page);
    static void SetCollapsed(bool collapsed);
    static bool IsCollapsed();
    static float GetWidth();
    
private:
    struct SidebarState {
        AnimationState collapse;
        AnimationState item_hover[32];
        bool collapsed = false;
    };
    
    static SidebarState s_state;
    static std::vector<NavigationItem> s_navigation_items;
    
    static void InitNavigationItems();
    static void RenderItem(NavigationItem& item, NavigationPage& current_page, int depth = 0);
    static void DrawItemBackground(const ImVec2& min, const ImVec2& max, bool is_selected, bool is_hovered, float alpha);
};

} // namespace UI
} // namespace PCOptimizer
