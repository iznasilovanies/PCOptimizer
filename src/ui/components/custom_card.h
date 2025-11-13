#pragma once
#include "ui_common.h"
#include <string>
#include <functional>

namespace PCOptimizer {
namespace UI {

class CustomCard {
public:
    static void Begin(const char* title, const ImVec2& size, bool* p_open = nullptr);
    static void End();
    
    static void BeginWithIcon(const char* icon, const char* title, const ImVec2& size, bool* p_open = nullptr);
    
private:
    struct CardState {
        AnimationState hover;
        AnimationState elevation;
    };
    
    static CardState s_current_card_state;
    static ImVec2 s_card_min;
    static ImVec2 s_card_max;
    static bool s_is_card_open;
    
    static void DrawCardBackground(ImDrawList* draw_list, const ImVec2& min, const ImVec2& max, 
                                  float hover_amount, float elevation_amount);
};

} // namespace UI
} // namespace PCOptimizer
