#pragma once
#include <imgui.h>
#include <cmath>
#include <algorithm>

namespace PCOptimizer {
namespace UI {

struct Theme {
    ImVec4 accent_color = ImVec4(0.2f, 0.6f, 1.0f, 1.0f);
    ImVec4 accent_hover = ImVec4(0.3f, 0.7f, 1.0f, 1.0f);
    ImVec4 accent_active = ImVec4(0.15f, 0.5f, 0.9f, 1.0f);
    
    ImVec4 background = ImVec4(0.12f, 0.12f, 0.14f, 1.0f);
    ImVec4 background_light = ImVec4(0.16f, 0.16f, 0.18f, 1.0f);
    ImVec4 background_dark = ImVec4(0.08f, 0.08f, 0.10f, 1.0f);
    
    ImVec4 text_primary = ImVec4(0.95f, 0.95f, 0.95f, 1.0f);
    ImVec4 text_secondary = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
    ImVec4 text_disabled = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
    
    ImVec4 success = ImVec4(0.2f, 0.8f, 0.4f, 1.0f);
    ImVec4 warning = ImVec4(1.0f, 0.7f, 0.2f, 1.0f);
    ImVec4 error = ImVec4(0.9f, 0.3f, 0.3f, 1.0f);
    
    float rounding = 6.0f;
    float shadow_size = 8.0f;
    float animation_speed = 0.15f;
};

extern Theme g_theme;

struct AnimationState {
    float current = 0.0f;
    float target = 0.0f;
    float speed = 0.15f;
    
    void Update(float dt) {
        if (std::abs(target - current) > 0.001f) {
            current = current + (target - current) * std::min(speed * dt * 60.0f, 1.0f);
        } else {
            current = target;
        }
    }
    
    void SetTarget(float value) {
        target = value;
    }
    
    float Get() const {
        return current;
    }
};

inline ImVec4 LerpColor(const ImVec4& a, const ImVec4& b, float t) {
    return ImVec4(
        a.x + (b.x - a.x) * t,
        a.y + (b.y - a.y) * t,
        a.z + (b.z - a.z) * t,
        a.w + (b.w - a.w) * t
    );
}

inline ImU32 ColorWithAlpha(const ImVec4& color, float alpha) {
    return ImGui::ColorConvertFloat4ToU32(ImVec4(color.x, color.y, color.z, alpha));
}

inline void DrawGradientRect(ImDrawList* draw_list, const ImVec2& min, const ImVec2& max, 
                             ImU32 col_top, ImU32 col_bottom, float rounding = 0.0f) {
    if (rounding > 0.0f) {
        draw_list->AddRectFilledMultiColor(min, max, col_top, col_top, col_bottom, col_bottom);
        draw_list->AddRect(min, max, col_top, rounding);
    } else {
        draw_list->AddRectFilledMultiColor(min, max, col_top, col_top, col_bottom, col_bottom);
    }
}

inline void DrawShadow(ImDrawList* draw_list, const ImVec2& min, const ImVec2& max, 
                      float rounding, float shadow_size) {
    for (int i = 0; i < 4; i++) {
        float offset = shadow_size * (i + 1) / 4.0f;
        float alpha = (1.0f - (float)i / 4.0f) * 0.15f;
        ImVec2 shadow_min = ImVec2(min.x - offset, min.y - offset);
        ImVec2 shadow_max = ImVec2(max.x + offset, max.y + offset);
        draw_list->AddRectFilled(shadow_min, shadow_max, 
                                ColorWithAlpha(ImVec4(0, 0, 0, 1), alpha), 
                                rounding + offset);
    }
}

} // namespace UI
} // namespace PCOptimizer
