#pragma once
#include "ui_common.h"
#include <string>
#include <unordered_map>

namespace PCOptimizer {
namespace UI {

enum class ButtonStyle {
    Primary,
    Secondary,
    Success,
    Warning,
    Error,
    Ghost
};

class CustomButton {
public:
    static bool Render(const char* label, const ImVec2& size, ButtonStyle style = ButtonStyle::Primary);
    static bool RenderWithIcon(const char* icon, const char* label, const ImVec2& size, ButtonStyle style = ButtonStyle::Primary);
    
private:
    struct ButtonState {
        AnimationState hover;
        AnimationState click;
        float ripple_progress = 0.0f;
        ImVec2 ripple_pos;
        bool ripple_active = false;
    };
    
    static std::unordered_map<std::string, ButtonState> s_button_states;
    
    static ButtonState& GetState(const char* id);
    static ImVec4 GetButtonColor(ButtonStyle style, bool is_hovered, bool is_active);
    static void DrawButton(const ImVec2& min, const ImVec2& max, ImU32 color, float hover_amount, float click_amount);
    static void DrawRipple(ImDrawList* draw_list, const ImVec2& center, float progress, ImU32 color);
};

} // namespace UI
} // namespace PCOptimizer
