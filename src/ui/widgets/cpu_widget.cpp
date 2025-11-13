#include "cpu_widget.h"
#include "../components/custom_button.h"
#include "../components/custom_dropdown.h"
#include "../components/ui_common.h"
#include <sstream>
#include <iomanip>

namespace UI {

CPUWidget::CPUWidget()
    : BaseWidget("cpu_monitor", "CPU Monitor", WidgetType::CPUMonitor) 
{
    m_config.size = ImVec2(600, 400);
    m_config.position = ImVec2(20, 20);
    
    auto cpuInfo = Monitor::MonitoringEngine::Get().GetCPUInfo();
    int coreCount = static_cast<int>(cpuInfo.size());
    
    m_usageSeries.reserve(coreCount);
    m_frequencySeries.reserve(coreCount);
    m_temperatureSeries.reserve(coreCount);
    
    ImU32 colors[] = {
        IM_COL32(0, 150, 255, 255),
        IM_COL32(255, 100, 0, 255),
        IM_COL32(0, 255, 150, 255),
        IM_COL32(255, 0, 150, 255),
        IM_COL32(150, 0, 255, 255),
        IM_COL32(255, 255, 0, 255),
        IM_COL32(0, 255, 255, 255),
        IM_COL32(255, 0, 255, 255)
    };
    
    for (int i = 0; i < coreCount; i++) {
        std::ostringstream label;
        label << "Core " << i;
        
        ImU32 color = colors[i % 8];
        
        m_usageSeries.emplace_back(label.str(), color);
        m_frequencySeries.emplace_back(label.str(), color);
        m_temperatureSeries.emplace_back(label.str(), color);
    }
}

void CPUWidget::Update(float deltaTime) {
    m_updateTimer += deltaTime;
    
    if (m_updateTimer >= m_updateInterval) {
        m_updateTimer = 0.0f;
        
        auto cpuInfo = Monitor::MonitoringEngine::Get().GetCPUInfo();
        
        for (size_t i = 0; i < cpuInfo.size() && i < m_usageSeries.size(); i++) {
            m_usageSeries[i].AddPoint(cpuInfo[i].usage);
            m_frequencySeries[i].AddPoint(cpuInfo[i].frequency);
            m_temperatureSeries[i].AddPoint(cpuInfo[i].temperature);
        }
    }
}

void CPUWidget::Render() {
    BeginWidget();
    
    ImVec2 contentPos = ImGui::GetCursorScreenPos();
    ImVec2 contentSize = ImVec2(m_config.size.x - 20, m_config.size.y - 60);
    
    ImGui::SetCursorScreenPos(ImVec2(contentPos.x + 10, contentPos.y + 10));
    
    ImGui::BeginGroup();
    
    std::vector<std::string> modes = {"Usage %", "Frequency MHz", "Temperature °C"};
    int selectedMode = static_cast<int>(m_displayMode);
    
    if (CustomDropdown::RenderSimple("##display_mode", selectedMode, modes, ImVec2(150, 30))) {
        m_displayMode = static_cast<DisplayMode>(selectedMode);
    }
    
    ImGui::Spacing();
    
    std::vector<GraphSeries>* currentSeries = &m_usageSeries;
    float minY = 0.0f;
    float maxY = 100.0f;
    
    switch (m_displayMode) {
        case DisplayMode::Usage:
            currentSeries = &m_usageSeries;
            maxY = 100.0f;
            break;
        case DisplayMode::Frequency:
            currentSeries = &m_frequencySeries;
            maxY = 5000.0f;
            break;
        case DisplayMode::Temperature:
            currentSeries = &m_temperatureSeries;
            maxY = 100.0f;
            break;
    }
    
    CustomGraph::Render(
        "##cpu_graph",
        *currentSeries,
        ImVec2(contentSize.x - 20, 200),
        GraphType::FilledLine,
        true,
        minY,
        maxY
    );
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    RenderStats();
    
    ImGui::EndGroup();
    
    EndWidget();
}

void CPUWidget::RenderStats() {
    auto cpuInfo = Monitor::MonitoringEngine::Get().GetCPUInfo();
    
    if (cpuInfo.empty()) {
        ImGui::TextColored(
            ImGui::ColorConvertU32ToFloat4(Theme::Get().colorTextSecondary),
            "No CPU data available"
        );
        return;
    }
    
    float avgUsage = 0.0f;
    float maxUsage = 0.0f;
    
    for (const auto& core : cpuInfo) {
        avgUsage += core.usage;
        maxUsage = std::max(maxUsage, core.usage);
    }
    
    avgUsage /= cpuInfo.size();
    
    ImGui::Columns(3, "cpu_stats", false);
    
    ImGui::TextColored(
        ImGui::ColorConvertU32ToFloat4(Theme::Get().colorTextSecondary),
        "Total Cores"
    );
    ImGui::Text("%d", static_cast<int>(cpuInfo.size()));
    
    ImGui::NextColumn();
    
    ImGui::TextColored(
        ImGui::ColorConvertU32ToFloat4(Theme::Get().colorTextSecondary),
        "Avg Usage"
    );
    ImGui::Text("%.1f%%", avgUsage);
    
    ImGui::NextColumn();
    
    ImGui::TextColored(
        ImGui::ColorConvertU32ToFloat4(Theme::Get().colorTextSecondary),
        "Max Usage"
    );
    ImGui::Text("%.1f%%", maxUsage);
    
    ImGui::Columns(1);
}

}
