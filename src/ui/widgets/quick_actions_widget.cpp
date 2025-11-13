#include "quick_actions_widget.h"
#include "../components/custom_button.h"
#include "../components/ui_common.h"

namespace UI {

QuickActionsWidget::QuickActionsWidget()
    : BaseWidget("quick_actions", "Quick Actions", WidgetType::Custom)
{
    m_config.size = ImVec2(1240, 180);
    m_config.position = ImVec2(20, 710);
}

void QuickActionsWidget::Update(float deltaTime) {
}

void QuickActionsWidget::Render() {
    BeginWidget();
    
    ImVec2 contentPos = ImGui::GetCursorScreenPos();
    ImVec2 contentSize = ImVec2(m_config.size.x - 20, m_config.size.y - 60);
    
    ImGui::SetCursorScreenPos(ImVec2(contentPos.x + 10, contentPos.y + 10));
    
    ImGui::BeginGroup();
    
    const float buttonWidth = (contentSize.x - 30) / 4.0f;
    const float buttonHeight = contentSize.y - 20;
    
    ImGui::BeginGroup();
    RenderProfileButton(
        "Gaming Mode",
        "Min input lag, stable 1% lows",
        OptimizationProfile::Gaming,
        IM_COL32(0, 150, 255, 255)
    );
    ImGui::EndGroup();
    
    ImGui::SameLine(0, 10);
    
    ImGui::BeginGroup();
    RenderProfileButton(
        "Streaming Mode",
        "Balance game+OBS, encoder priority",
        OptimizationProfile::Streaming,
        IM_COL32(150, 0, 255, 255)
    );
    ImGui::EndGroup();
    
    ImGui::SameLine(0, 10);
    
    ImGui::BeginGroup();
    RenderProfileButton(
        "Workstation Mode",
        "Rendering/compilation, disk I/O",
        OptimizationProfile::Workstation,
        IM_COL32(255, 150, 0, 255)
    );
    ImGui::EndGroup();
    
    ImGui::SameLine(0, 10);
    
    ImGui::BeginGroup();
    RenderProfileButton(
        "Balanced Mode",
        "Default Windows settings",
        OptimizationProfile::Balanced,
        IM_COL32(0, 200, 150, 255)
    );
    ImGui::EndGroup();
    
    ImGui::EndGroup();
    
    EndWidget();
}

void QuickActionsWidget::RenderProfileButton(
    const char* label,
    const char* description,
    OptimizationProfile profile,
    ImU32 color
) {
    ImVec2 cursorPos = ImGui::GetCursorScreenPos();
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    
    const float width = (m_config.size.x - 50) / 4.0f;
    const float height = m_config.size.y - 80;
    
    bool isActive = (m_currentProfile == profile);
    bool hovered = ImGui::IsMouseHoveringRect(
        cursorPos,
        ImVec2(cursorPos.x + width, cursorPos.y + height)
    );
    
    ImU32 bgColor = Theme::Get().colorCardBg;
    ImU32 borderColor = isActive ? color : Theme::Get().colorBorder;
    float borderWidth = isActive ? 3.0f : (hovered ? 2.0f : 1.0f);
    
    if (hovered) {
        bgColor = LerpColor(bgColor, color, 0.1f);
    }
    
    drawList->AddRectFilled(
        cursorPos,
        ImVec2(cursorPos.x + width, cursorPos.y + height),
        bgColor,
        Theme::Get().rounding
    );
    
    drawList->AddRect(
        cursorPos,
        ImVec2(cursorPos.x + width, cursorPos.y + height),
        borderColor,
        Theme::Get().rounding,
        ImDrawFlags_None,
        borderWidth
    );
    
    ImVec2 labelSize = ImGui::CalcTextSize(label);
    ImVec2 labelPos(
        cursorPos.x + (width - labelSize.x) * 0.5f,
        cursorPos.y + 20
    );
    
    drawList->AddText(
        labelPos,
        color,
        label
    );
    
    ImVec2 descSize = ImGui::CalcTextSize(description);
    ImVec2 descPos(
        cursorPos.x + (width - descSize.x) * 0.5f,
        cursorPos.y + 50
    );
    
    drawList->AddText(
        descPos,
        Theme::Get().colorTextSecondary,
        description
    );
    
    if (isActive) {
        ImVec2 activeIndicatorPos(
            cursorPos.x + (width - 60) * 0.5f,
            cursorPos.y + height - 30
        );
        
        drawList->AddRectFilled(
            activeIndicatorPos,
            ImVec2(activeIndicatorPos.x + 60, activeIndicatorPos.y + 20),
            color,
            Theme::Get().rounding
        );
        
        ImVec2 activeTextSize = ImGui::CalcTextSize("ACTIVE");
        drawList->AddText(
            ImVec2(
                activeIndicatorPos.x + (60 - activeTextSize.x) * 0.5f,
                activeIndicatorPos.y + (20 - activeTextSize.y) * 0.5f
            ),
            IM_COL32(255, 255, 255, 255),
            "ACTIVE"
        );
    }
    
    ImGui::SetCursorScreenPos(cursorPos);
    ImGui::InvisibleButton(label, ImVec2(width, height));
    
    if (ImGui::IsItemClicked()) {
        m_currentProfile = profile;
    }
    
    if (hovered) {
        ImGui::SetTooltip("%s", description);
    }
    
    ImGui::SetCursorScreenPos(ImVec2(cursorPos.x, cursorPos.y + height));
    ImGui::Dummy(ImVec2(width, 0));
}

}
