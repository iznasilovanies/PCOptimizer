#pragma once
#include "base_widget.h"
#include "../components/custom_progress.h"
#include "../../monitoring/monitoring_engine.h"

namespace UI {

class RAMWidget : public BaseWidget {
public:
    RAMWidget();
    ~RAMWidget() override = default;
    
    void Update(float deltaTime) override;
    void Render() override;
    
private:
    float m_updateTimer = 0.0f;
    const float m_updateInterval = 1.0f;
};

}
