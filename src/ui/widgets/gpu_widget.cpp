#include "gpu_widget.h"
#include "../components/ui_common.h"

namespace UI {

GPUWidget::GPUWidget()
    : BaseWidget("gpu_monitor", "GPU Monitor", WidgetType::GPUMonitor),
      m_coreClockSeries("Core Clock", IM_COL32(0, 150, 255, 255)),
      m_memoryClockSeries("Memory Clock", IM_COL32(255, 150, 0, 255)),
      m_temperatureSeries("Temperature", IM_COL32(255, 0, 100, 255)),
      m_usageSeries("Usage", IM_COL32(0, 255, 150, 255))
{
    m_config.size = ImVec2(600, 400);
    m_config.position = ImVec2(640, 20);
}

void GPUWidget::Update(float deltaTime) {
    m_updateTimer += deltaTime;
    
    if (m_updateTimer >= m_updateInterval) {
        m_updateTimer = 0.0f;
        
        auto gpuInfo = Monitor::MonitoringEngine::Get().GetGPUInfo();
        
        m_coreClockSeries.AddPoint(gpuInfo.coreClock);
        m_memoryClockSeries.AddPoint(gpuInfo.memoryClock);
        m_temperatureSeries.AddPoint(gpuInfo.temperature);
        m_usageSeries.AddPoint(gpuInfo.usage);
    }
}

void GPUWidget::Render() {
    BeginWidget();
    
    ImVec2 contentPos = ImGui::GetCursorScreenPos();
    ImVec2 contentSize = ImVec2(m_config.size.x - 20, m_config.size.y - 60);
    
    ImGui::SetCursorScreenPos(ImVec2(contentPos.x + 10, contentPos.y + 10));
    
    ImGui::BeginGroup();
    
    auto gpuInfo = Monitor::MonitoringEngine::Get().GetGPUInfo();
    
    ImGui::TextColored(
        ImGui::ColorConvertU32ToFloat4(Theme::Get().colorText),
        "%s", gpuInfo.name.c_str()
    );
    
    ImGui::Spacing();
    
    std::vector<GraphSeries> clockSeries = {m_coreClockSeries, m_memoryClockSeries};
    CustomGraph::Render(
        "##gpu_clocks",
        clockSeries,
        ImVec2(contentSize.x * 0.65f - 20, 150),
        GraphType::Line,
        true,
        0.0f,
        3000.0f
    );
    
    ImGui::SameLine();
    
    ImGui::BeginGroup();
    CustomProgress::RenderCircular(
        "Temp",
        gpuInfo.temperature,
        100.0f,
        50.0f,
        IM_COL32(255, 100, 0, 255)
    );
    ImGui::EndGroup();
    
    ImGui::Spacing();
    
    CustomProgress::RenderLinear(
        "VRAM Usage",
        gpuInfo.memoryUsage,
        gpuInfo.memoryTotal,
        ImVec2(contentSize.x - 20, 24),
        IM_COL32(0, 200, 255, 255),
        true,
        true
    );
    
    ImGui::Spacing();
    
    ImGui::Columns(3, "gpu_stats", false);
    
    ImGui::TextColored(
        ImGui::ColorConvertU32ToFloat4(Theme::Get().colorTextSecondary),
        "Power"
    );
    ImGui::Text("%.1f W", gpuInfo.powerUsage);
    
    ImGui::NextColumn();
    
    ImGui::TextColored(
        ImGui::ColorConvertU32ToFloat4(Theme::Get().colorTextSecondary),
        "Fan Speed"
    );
    ImGui::Text("%d RPM", gpuInfo.fanSpeed);
    
    ImGui::NextColumn();
    
    ImGui::TextColored(
        ImGui::ColorConvertU32ToFloat4(Theme::Get().colorTextSecondary),
        "Usage"
    );
    ImGui::Text("%.1f%%", gpuInfo.usage);
    
    ImGui::Columns(1);
    
    ImGui::EndGroup();
    
    EndWidget();
}

}
