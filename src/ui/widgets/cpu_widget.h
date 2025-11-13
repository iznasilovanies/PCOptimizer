#pragma once
#include "base_widget.h"
#include "../components/custom_graph.h"
#include "../../monitoring/monitoring_engine.h"
#include <vector>

namespace UI {

class CPUWidget : public BaseWidget {
public:
    CPUWidget();
    ~CPUWidget() override = default;
    
    void Update(float deltaTime) override;
    void Render() override;
    
private:
    void RenderHeader();
    void RenderCoreGraphs();
    void RenderStats();
    
    std::vector<GraphSeries> m_usageSeries;
    std::vector<GraphSeries> m_frequencySeries;
    std::vector<GraphSeries> m_temperatureSeries;
    
    enum class DisplayMode {
        Usage,
        Frequency,
        Temperature
    };
    
    DisplayMode m_displayMode = DisplayMode::Usage;
    float m_updateTimer = 0.0f;
    const float m_updateInterval = 1.0f;
};

}
