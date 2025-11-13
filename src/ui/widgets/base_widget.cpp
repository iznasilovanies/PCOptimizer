#include "base_widget.h"
#include "../components/ui_common.h"
#include "../components/custom_button.h"
#include <algorithm>

namespace UI {

void BaseWidget::BeginWidget() {
    if (!m_config.visible) return;
    
    ImGui::PushID(m_id.c_str());
    
    ImVec2 cursorPos = ImGui::GetCursorScreenPos();
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    
    ImVec2 widgetPos = ImVec2(
        cursorPos.x + m_config.position.x,
        cursorPos.y + m_config.position.y
    );
    
    ImVec2 widgetSize = m_config.size;
    
    m_hovered = ImGui::IsMouseHoveringRect(
        widgetPos,
        ImVec2(widgetPos.x + widgetSize.x, widgetPos.y + widgetSize.y)
    );
    
    ImU32 bgColor = UI::Theme::Get().colorCardBg;
    ImU32 borderColor = UI::Theme::Get().colorBorder;
    
    if (m_hovered || m_dragging || m_resizing) {
        borderColor = UI::Theme::Get().colorAccent;
    }
    
    UI::DrawShadow(drawList, widgetPos, widgetSize, UI::Theme::Get().rounding, 2);
    
    drawList->AddRectFilled(
        widgetPos,
        ImVec2(widgetPos.x + widgetSize.x, widgetPos.y + widgetSize.y),
        bgColor,
        UI::Theme::Get().rounding
    );
    
    drawList->AddRect(
        widgetPos,
        ImVec2(widgetPos.x + widgetSize.x, widgetPos.y + widgetSize.y),
        borderColor,
        UI::Theme::Get().rounding,
        ImDrawFlags_None,
        m_hovered ? 2.0f : 1.0f
    );
    
    ImGui::SetCursorScreenPos(widgetPos);
    ImGui::BeginGroup();
    
    RenderHeader();
}

void BaseWidget::EndWidget() {
    if (!m_config.visible) return;
    
    ImGui::EndGroup();
    
    HandleDragDrop();
    RenderResizeHandles();
    
    ImGui::PopID();
}

void BaseWidget::RenderHeader() {
    ImVec2 headerPos = ImGui::GetCursorScreenPos();
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    
    const float headerHeight = 40.0f;
    ImVec2 headerSize(m_config.size.x, headerHeight);
    
    drawList->AddRectFilled(
        headerPos,
        ImVec2(headerPos.x + headerSize.x, headerPos.y + headerSize.y),
        UI::LerpColor(UI::Theme::Get().colorBackground, UI::Theme::Get().colorAccent, 0.05f),
        UI::Theme::Get().rounding,
        ImDrawFlags_RoundCornersTop
    );
    
    ImGui::SetCursorScreenPos(ImVec2(headerPos.x + 12, headerPos.y + (headerHeight - ImGui::CalcTextSize(m_title.c_str()).y) * 0.5f));
    ImGui::TextColored(
        ImGui::ColorConvertU32ToFloat4(UI::Theme::Get().colorText),
        "%s", m_title.c_str()
    );
    
    ImGui::SetCursorScreenPos(ImVec2(headerPos.x, headerPos.y + headerHeight));
}

void BaseWidget::RenderResizeHandles() {
    if (!m_config.resizable) return;
    
    ImVec2 widgetPos = ImVec2(
        ImGui::GetCursorScreenPos().x + m_config.position.x - m_config.size.x,
        ImGui::GetCursorScreenPos().y + m_config.position.y - m_config.size.y
    );
    
    ImVec2 handlePos(
        widgetPos.x + m_config.size.x - 15,
        widgetPos.y + m_config.size.y - 15
    );
    
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    
    bool handleHovered = ImGui::IsMouseHoveringRect(
        handlePos,
        ImVec2(handlePos.x + 15, handlePos.y + 15)
    );
    
    ImU32 handleColor = handleHovered
        ? UI::Theme::Get().colorAccent
        : UI::Theme::Get().colorTextSecondary;
    
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (i + j >= 2) {
                drawList->AddCircleFilled(
                    ImVec2(handlePos.x + 5 + i * 4, handlePos.y + 5 + j * 4),
                    1.5f,
                    handleColor
                );
            }
        }
    }
    
    if (handleHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        m_resizing = true;
        m_resizeStartSize = m_config.size;
        m_resizeStartPos = ImGui::GetMousePos();
    }
    
    if (m_resizing) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNWSE);
        
        if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
            ImVec2 delta = ImVec2(
                ImGui::GetMousePos().x - m_resizeStartPos.x,
                ImGui::GetMousePos().y - m_resizeStartPos.y
            );
            
            m_config.size.x = std::max(200.0f, m_resizeStartSize.x + delta.x);
            m_config.size.y = std::max(150.0f, m_resizeStartSize.y + delta.y);
        } else {
            m_resizing = false;
        }
    }
}

void BaseWidget::HandleDragDrop() {
    if (!m_config.draggable) return;
    
    ImVec2 headerPos = ImGui::GetCursorScreenPos();
    headerPos.y -= m_config.size.y;
    
    bool headerHovered = ImGui::IsMouseHoveringRect(
        headerPos,
        ImVec2(headerPos.x + m_config.size.x, headerPos.y + 40)
    );
    
    if (headerHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !m_resizing) {
        m_dragging = true;
        m_dragOffset = ImVec2(
            ImGui::GetMousePos().x - (headerPos.x + m_config.position.x),
            ImGui::GetMousePos().y - (headerPos.y + m_config.position.y)
        );
    }
    
    if (m_dragging) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
        
        if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
            ImVec2 mousePos = ImGui::GetMousePos();
            m_config.position.x = mousePos.x - headerPos.x - m_dragOffset.x;
            m_config.position.y = mousePos.y - headerPos.y - m_dragOffset.y;
            
            m_config.position.x = std::max(0.0f, m_config.position.x);
            m_config.position.y = std::max(0.0f, m_config.position.y);
        } else {
            m_dragging = false;
        }
    }
}

}
