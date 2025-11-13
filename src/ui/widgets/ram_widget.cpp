#include "ram_widget.h"
#include "../components/ui_common.h"

namespace UI {

RAMWidget::RAMWidget()
    : BaseWidget("ram_monitor", "RAM Monitor", WidgetType::RAMMonitor)
{
    m_config.size = ImVec2(400, 250);
    m_config.position = ImVec2(20, 440);
}

void RAMWidget::Update(float deltaTime) {
    m_updateTimer += deltaTime;
    
    if (m_updateTimer >= m_updateInterval) {
        m_updateTimer = 0.0f;
    }
}

void RAMWidget::Render() {
    BeginWidget();
    
    ImVec2 contentPos = ImGui::GetCursorScreenPos();
    ImVec2 contentSize = ImVec2(m_config.size.x - 20, m_config.size.y - 60);
    
    ImGui::SetCursorScreenPos(ImVec2(contentPos.x + 10, contentPos.y + 10));
    
    ImGui::BeginGroup();
    
    auto ramInfo = Monitor::MonitoringEngine::Get().GetRAMInfo();
    
    ImGui::TextColored(
        ImGui::ColorConvertU32ToFloat4(Theme::Get().colorText),
        "Total: %.1f GB", ramInfo.totalGB
    );
    
    ImGui::Spacing();
    
    CustomProgress::RenderLinear(
        "",
        ramInfo.usedGB,
        ramInfo.totalGB,
        ImVec2(contentSize.x - 20, 40),
        IM_COL32(0, 150, 255, 255),
        false,
        true
    );
    
    ImGui::SetCursorScreenPos(ImVec2(
        contentPos.x + (m_config.size.x - ImGui::CalcTextSize("%.1f / %.1f GB").x) * 0.5f,
        contentPos.y + 80
    ));
    
    ImGui::Text("%.1f / %.1f GB", ramInfo.usedGB, ramInfo.totalGB);
    
    ImGui::SetCursorScreenPos(ImVec2(contentPos.x + 10, contentPos.y + 120));
    
    ImGui::Columns(2, "ram_stats", false);
    
    ImGui::TextColored(
        ImGui::ColorConvertU32ToFloat4(Theme::Get().colorTextSecondary),
        "Speed"
    );
    ImGui::Text("%d MHz", ramInfo.speedMHz);
    
    ImGui::NextColumn();
    
    ImGui::TextColored(
        ImGui::ColorConvertU32ToFloat4(Theme::Get().colorTextSecondary),
        "Latency"
    );
    ImGui::Text("%.1f ns", ramInfo.latencyNs);
    
    ImGui::Columns(1);
    
    ImGui::EndGroup();
    
    EndWidget();
}

}
