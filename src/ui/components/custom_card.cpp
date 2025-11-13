#include "custom_card.h"
#include <imgui_internal.h>

namespace PCOptimizer {
namespace UI {

CustomCard::CardState CustomCard::s_current_card_state;
ImVec2 CustomCard::s_card_min;
ImVec2 CustomCard::s_card_max;
bool CustomCard::s_is_card_open = false;

void CustomCard::DrawCardBackground(ImDrawList* draw_list, const ImVec2& min, const ImVec2& max, 
                                    float hover_amount, float elevation_amount) {
    float shadow_size = g_theme.shadow_size * (1.0f + elevation_amount);
    DrawShadow(draw_list, min, max, g_theme.rounding, shadow_size);
    
    draw_list->AddRectFilled(min, max, 
                            ImGui::ColorConvertFloat4ToU32(g_theme.background_light), 
                            g_theme.rounding);
    
    ImVec4 border_color = LerpColor(
        ImVec4(g_theme.background_light.x * 1.2f, g_theme.background_light.y * 1.2f, 
               g_theme.background_light.z * 1.2f, 1.0f),
        g_theme.accent_color,
        hover_amount * 0.5f
    );
    
    draw_list->AddRect(min, max, 
                      ImGui::ColorConvertFloat4ToU32(border_color), 
                      g_theme.rounding, 0, 1.0f + hover_amount);
    
    if (hover_amount > 0.0f) {
        for (int i = 0; i < 2; i++) {
            float offset = (i + 1) * 2.0f;
            ImVec2 glow_min = ImVec2(min.x - offset, min.y - offset);
            ImVec2 glow_max = ImVec2(max.x + offset, max.y + offset);
            draw_list->AddRect(glow_min, glow_max, 
                              ColorWithAlpha(g_theme.accent_color, hover_amount * 0.1f * (1.0f - i / 2.0f)),
                              g_theme.rounding + offset, 0, 1.5f);
        }
    }
}

void CustomCard::Begin(const char* title, const ImVec2& size, bool* p_open) {
    BeginWithIcon(nullptr, title, size, p_open);
}

void CustomCard::BeginWithIcon(const char* icon, const char* title, const ImVec2& size, bool* p_open) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) {
        s_is_card_open = false;
        return;
    }
    
    const ImGuiStyle& style = ImGui::GetStyle();
    const ImVec2 pos = window->DC.CursorPos;
    
    ImVec2 card_size = size;
    if (card_size.x == 0.0f) card_size.x = ImGui::GetContentRegionAvail().x;
    if (card_size.y == 0.0f) card_size.y = 200.0f;
    
    s_card_min = pos;
    s_card_max = ImVec2(pos.x + card_size.x, pos.y + card_size.y);
    
    ImRect bb(s_card_min, s_card_max);
    ImGui::ItemSize(bb);
    
    bool hovered = ImGui::IsMouseHoveringRect(s_card_min, s_card_max);
    
    float dt = ImGui::GetIO().DeltaTime;
    s_current_card_state.hover.SetTarget(hovered ? 1.0f : 0.0f);
    s_current_card_state.hover.Update(dt);
    
    s_current_card_state.elevation.SetTarget(hovered ? 1.0f : 0.0f);
    s_current_card_state.elevation.Update(dt);
    
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    DrawCardBackground(draw_list, s_card_min, s_card_max, 
                      s_current_card_state.hover.Get(), 
                      s_current_card_state.elevation.Get());
    
    float header_height = 40.0f;
    ImVec2 header_min = s_card_min;
    ImVec2 header_max = ImVec2(s_card_max.x, s_card_min.y + header_height);
    
    draw_list->AddRectFilled(header_min, header_max, 
                            ColorWithAlpha(g_theme.background_dark, 0.3f),
                            g_theme.rounding, ImDrawFlags_RoundCornersTop);
    
    ImVec2 title_pos = ImVec2(header_min.x + 16.0f, header_min.y + (header_height - ImGui::CalcTextSize(title).y) * 0.5f);
    
    if (icon) {
        ImVec2 icon_size = ImGui::CalcTextSize(icon);
        draw_list->AddText(title_pos, ImGui::ColorConvertFloat4ToU32(g_theme.accent_color), icon);
        title_pos.x += icon_size.x + 8.0f;
    }
    
    draw_list->AddText(title_pos, ImGui::ColorConvertFloat4ToU32(g_theme.text_primary), title);
    
    if (p_open) {
        ImVec2 close_pos = ImVec2(header_max.x - 30.0f, header_min.y + (header_height - 16.0f) * 0.5f);
        ImRect close_bb(close_pos, ImVec2(close_pos.x + 16.0f, close_pos.y + 16.0f));
        
        bool close_hovered = ImGui::IsMouseHoveringRect(close_bb.Min, close_bb.Max);
        bool close_clicked = close_hovered && ImGui::IsMouseClicked(0);
        
        if (close_clicked) {
            *p_open = false;
        }
        
        ImU32 close_color = ImGui::ColorConvertFloat4ToU32(
            close_hovered ? g_theme.error : g_theme.text_secondary
        );
        
        draw_list->AddText(close_pos, close_color, "X");
    }
    
    ImVec2 content_min = ImVec2(s_card_min.x + 12.0f, header_max.y + 8.0f);
    ImVec2 content_max = ImVec2(s_card_max.x - 12.0f, s_card_max.y - 8.0f);
    
    ImGui::SetCursorScreenPos(content_min);
    ImGui::BeginGroup();
    
    ImGui::PushClipRect(content_min, content_max, true);
    
    s_is_card_open = true;
}

void CustomCard::End() {
    if (!s_is_card_open) return;
    
    ImGui::PopClipRect();
    ImGui::EndGroup();
    
    ImGui::SetCursorScreenPos(ImVec2(s_card_min.x, s_card_max.y + 8.0f));
    
    s_is_card_open = false;
}

} // namespace UI
} // namespace PCOptimizer
