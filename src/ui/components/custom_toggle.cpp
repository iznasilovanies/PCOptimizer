#include "custom_toggle.h"
#include <imgui_internal.h>

namespace PCOptimizer {
namespace UI {

std::unordered_map<std::string, CustomToggle::ToggleState> CustomToggle::s_toggle_states;

CustomToggle::ToggleState& CustomToggle::GetState(const char* id) {
    return s_toggle_states[id];
}

void CustomToggle::DrawToggle(ImDrawList* draw_list, const ImVec2& min, const ImVec2& max, 
                              float toggle_amount, float hover_amount, float glow_amount, bool has_icon) {
    float height = max.y - min.y;
    float width = height * 2.0f;
    
    ImVec2 track_min = min;
    ImVec2 track_max = ImVec2(min.x + width, max.y);
    
    ImVec4 track_color_off = g_theme.background_light;
    ImVec4 track_color_on = g_theme.accent_color;
    ImVec4 track_color = LerpColor(track_color_off, track_color_on, toggle_amount);
    
    if (glow_amount > 0.0f) {
        for (int i = 0; i < 3; i++) {
            float expand = (i + 1) * 1.5f;
            ImVec2 glow_min = ImVec2(track_min.x - expand, track_min.y - expand);
            ImVec2 glow_max = ImVec2(track_max.x + expand, track_max.y + expand);
            draw_list->AddRectFilled(glow_min, glow_max, 
                                    ColorWithAlpha(track_color, glow_amount * 0.2f * (1.0f - i / 3.0f)), 
                                    height / 2.0f + expand);
        }
    }
    
    draw_list->AddRectFilled(track_min, track_max, 
                            ImGui::ColorConvertFloat4ToU32(track_color), 
                            height / 2.0f);
    
    float thumb_radius = (height - 6.0f) / 2.0f;
    float thumb_x = track_min.x + thumb_radius + 3.0f + toggle_amount * (width - height);
    float thumb_y = (track_min.y + track_max.y) / 2.0f;
    
    ImVec2 thumb_center(thumb_x, thumb_y);
    
    if (hover_amount > 0.0f) {
        draw_list->AddCircleFilled(thumb_center, thumb_radius + 4.0f + hover_amount * 2.0f, 
                                   ColorWithAlpha(ImVec4(1, 1, 1, 1), hover_amount * 0.15f), 32);
    }
    
    draw_list->AddCircleFilled(thumb_center, thumb_radius + 1.0f, 
                              ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 0.2f)), 32);
    
    draw_list->AddCircleFilled(thumb_center, thumb_radius, 
                              ImGui::ColorConvertFloat4ToU32(g_theme.text_primary), 32);
}

bool CustomToggle::Render(const char* label, bool* value) {
    return RenderWithIcon(nullptr, nullptr, label, value);
}

bool CustomToggle::RenderWithIcon(const char* icon_on, const char* icon_off, const char* label, bool* value) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) return false;
    
    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    
    const ImVec2 label_size = ImGui::CalcTextSize(label, nullptr, true);
    
    float toggle_height = 24.0f;
    float toggle_width = toggle_height * 2.0f;
    
    const ImVec2 pos = window->DC.CursorPos;
    const ImRect toggle_bb(pos, ImVec2(pos.x + toggle_width, pos.y + toggle_height));
    const ImRect total_bb(pos, ImVec2(toggle_bb.Max.x + (label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f), 
                                     toggle_bb.Max.y));
    
    ImGui::ItemSize(total_bb, style.FramePadding.y);
    if (!ImGui::ItemAdd(total_bb, id)) return false;
    
    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(toggle_bb, id, &hovered, &held);
    
    if (pressed) {
        *value = !*value;
    }
    
    ToggleState& state = GetState(label);
    
    float dt = ImGui::GetIO().DeltaTime;
    
    state.toggle.SetTarget(*value ? 1.0f : 0.0f);
    state.toggle.Update(dt);
    
    state.hover.SetTarget(hovered ? 1.0f : 0.0f);
    state.hover.Update(dt);
    
    state.glow.SetTarget(*value ? 1.0f : 0.0f);
    state.glow.Update(dt);
    
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    
    DrawToggle(draw_list, toggle_bb.Min, toggle_bb.Max, 
              state.toggle.Get(), state.hover.Get(), state.glow.Get(), 
              icon_on != nullptr || icon_off != nullptr);
    
    if (label_size.x > 0.0f) {
        ImVec2 label_pos = ImVec2(toggle_bb.Max.x + style.ItemInnerSpacing.x, 
                                 toggle_bb.Min.y + (toggle_height - label_size.y) * 0.5f);
        draw_list->AddText(label_pos, ImGui::ColorConvertFloat4ToU32(g_theme.text_primary), label);
    }
    
    return pressed;
}

} // namespace UI
} // namespace PCOptimizer
