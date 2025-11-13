#pragma once
#include "ui_common.h"
#include <string>
#include <unordered_map>

namespace PCOptimizer {
namespace UI {

class CustomToggle {
public:
    static bool Render(const char* label, bool* value);
    static bool RenderWithIcon(const char* icon_on, const char* icon_off, const char* label, bool* value);
    
private:
    struct ToggleState {
        AnimationState toggle;
        AnimationState hover;
        AnimationState glow;
    };
    
    static std::unordered_map<std::string, ToggleState> s_toggle_states;
    
    static ToggleState& GetState(const char* id);
    static void DrawToggle(ImDrawList* draw_list, const ImVec2& min, const ImVec2& max, 
                          float toggle_amount, float hover_amount, float glow_amount, bool has_icon);
};

} // namespace UI
} // namespace PCOptimizer
