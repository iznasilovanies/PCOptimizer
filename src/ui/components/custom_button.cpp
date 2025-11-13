#include "custom_button.h"
#include <imgui_internal.h>

namespace PCOptimizer {
namespace UI {

std::unordered_map<std::string, CustomButton::ButtonState> CustomButton::s_button_states;

CustomButton::ButtonState& CustomButton::GetState(const char* id) {
    return s_button_states[id];
}

ImVec4 CustomButton::GetButtonColor(ButtonStyle style, bool is_hovered, bool is_active) {
    switch (style) {
        case ButtonStyle::Primary:
            return is_active ? g_theme.accent_active : 
                   is_hovered ? g_theme.accent_hover : g_theme.accent_color;
        case ButtonStyle::Secondary:
            return is_active ? g_theme.background_dark : 
                   is_hovered ? g_theme.background_light : g_theme.background;
        case ButtonStyle::Success:
            return g_theme.success;
        case ButtonStyle::Warning:
            return g_theme.warning;
        case ButtonStyle::Error:
            return g_theme.error;
        case ButtonStyle::Ghost:
            return ImVec4(0, 0, 0, is_hovered ? 0.2f : 0.0f);
        default:
            return g_theme.accent_color;
    }
}

void CustomButton::DrawButton(const ImVec2& min, const ImVec2& max, ImU32 color, float hover_amount, float click_amount) {
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    
    float shadow_amount = hover_amount * 0.5f;
    if (shadow_amount > 0.01f) {
        DrawShadow(draw_list, min, max, g_theme.rounding, g_theme.shadow_size * shadow_amount);
    }
    
    ImVec2 adjusted_min = min;
    ImVec2 adjusted_max = max;
    if (click_amount > 0.0f) {
        float offset = click_amount * 2.0f;
        adjusted_min.x += offset;
        adjusted_min.y += offset;
        adjusted_max.x -= offset;
        adjusted_max.y -= offset;
    }
    
    ImVec4 col_top = ImGui::ColorConvertU32ToFloat4(color);
    ImVec4 col_bottom = ImVec4(
        col_top.x * 0.8f,
        col_top.y * 0.8f,
        col_top.z * 0.8f,
        col_top.w
    );
    
    DrawGradientRect(draw_list, adjusted_min, adjusted_max, 
                    ImGui::ColorConvertFloat4ToU32(col_top),
                    ImGui::ColorConvertFloat4ToU32(col_bottom),
                    g_theme.rounding);
    
    if (hover_amount > 0.0f) {
        float glow_alpha = hover_amount * 0.4f;
        ImU32 glow_color = ColorWithAlpha(col_top, glow_alpha);
        for (int i = 0; i < 3; i++) {
            float expand = (i + 1) * 1.5f;
            ImVec2 glow_min = ImVec2(adjusted_min.x - expand, adjusted_min.y - expand);
            ImVec2 glow_max = ImVec2(adjusted_max.x + expand, adjusted_max.y + expand);
            draw_list->AddRect(glow_min, glow_max, glow_color, g_theme.rounding + expand, 0, 1.5f);
        }
    }
}

void CustomButton::DrawRipple(ImDrawList* draw_list, const ImVec2& center, float progress, ImU32 color) {
    if (progress <= 0.0f || progress >= 1.0f) return;
    
    float radius = progress * 100.0f;
    float alpha = (1.0f - progress) * 0.5f;
    
    draw_list->AddCircleFilled(center, radius, ColorWithAlpha(ImGui::ColorConvertU32ToFloat4(color), alpha), 32);
}

bool CustomButton::Render(const char* label, const ImVec2& size, ButtonStyle style) {
    return RenderWithIcon(nullptr, label, size, style);
}

bool CustomButton::RenderWithIcon(const char* icon, const char* label, const ImVec2& size, ButtonStyle style) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) return false;
    
    ImGuiContext& g = *GImGui;
    const ImGuiStyle& imgui_style = g.Style;
    const ImGuiID id = window->GetID(label);
    
    ImVec2 label_size = ImGui::CalcTextSize(label, nullptr, true);
    ImVec2 icon_size = icon ? ImGui::CalcTextSize(icon, nullptr, true) : ImVec2(0, 0);
    
    ImVec2 button_size = size;
    if (button_size.x == 0.0f) {
        button_size.x = label_size.x + icon_size.x + imgui_style.FramePadding.x * 2.0f + (icon ? 8.0f : 0.0f);
    }
    if (button_size.y == 0.0f) {
        button_size.y = std::max(label_size.y, icon_size.y) + imgui_style.FramePadding.y * 2.0f;
    }
    
    const ImVec2 pos = window->DC.CursorPos;
    const ImRect bb(pos, ImVec2(pos.x + button_size.x, pos.y + button_size.y));
    
    ImGui::ItemSize(bb, imgui_style.FramePadding.y);
    if (!ImGui::ItemAdd(bb, id)) return false;
    
    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);
    
    ButtonState& state = GetState(label);
    
    float dt = ImGui::GetIO().DeltaTime;
    state.hover.SetTarget(hovered ? 1.0f : 0.0f);
    state.hover.Update(dt);
    
    state.click.SetTarget(held ? 1.0f : 0.0f);
    state.click.Update(dt);
    
    if (pressed) {
        state.ripple_active = true;
        state.ripple_progress = 0.0f;
        state.ripple_pos = ImGui::GetMousePos();
    }
    
    if (state.ripple_active) {
        state.ripple_progress += dt * 2.0f;
        if (state.ripple_progress >= 1.0f) {
            state.ripple_active = false;
        }
    }
    
    ImVec4 button_color = GetButtonColor(style, hovered, held);
    ImU32 col = ImGui::ColorConvertFloat4ToU32(button_color);
    
    DrawButton(bb.Min, bb.Max, col, state.hover.Get(), state.click.Get());
    
    if (state.ripple_active) {
        DrawRipple(ImGui::GetWindowDrawList(), state.ripple_pos, state.ripple_progress, 
                  ColorWithAlpha(ImVec4(1, 1, 1, 1), 0.3f));
    }
    
    ImVec2 text_pos = ImVec2(
        bb.Min.x + (bb.Max.x - bb.Min.x - label_size.x - icon_size.x - (icon ? 8.0f : 0.0f)) * 0.5f,
        bb.Min.y + (bb.Max.y - bb.Min.y - label_size.y) * 0.5f
    );
    
    if (icon) {
        ImGui::GetWindowDrawList()->AddText(text_pos, ImGui::ColorConvertFloat4ToU32(g_theme.text_primary), icon);
        text_pos.x += icon_size.x + 8.0f;
    }
    
    ImGui::GetWindowDrawList()->AddText(text_pos, ImGui::ColorConvertFloat4ToU32(g_theme.text_primary), label);
    
    return pressed;
}

} // namespace UI
} // namespace PCOptimizer
