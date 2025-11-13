#pragma once
#include "../components/ui_common.h"
#include <string>

namespace PCOptimizer {
namespace UI {

class TopBar {
public:
    static void Render(const char* current_mode, float sidebar_width);
    
    static bool WantsMinimize();
    static bool WantsMaximize();
    static bool WantsClose();
    
private:
    struct TopBarState {
        AnimationState minimize_hover;
        AnimationState maximize_hover;
        AnimationState close_hover;
        bool wants_minimize = false;
        bool wants_maximize = false;
        bool wants_close = false;
    };
    
    static TopBarState s_state;
    
    static void DrawWindowControls(const ImVec2& position);
    static void DrawLogo(const ImVec2& position);
    static void DrawSystemTray(const ImVec2& position, const char* current_mode);
};

} // namespace UI
} // namespace PCOptimizer
