#pragma once
#include "base_widget.h"

namespace UI {

enum class OptimizationProfile {
    Gaming,
    Streaming,
    Workstation,
    Balanced
};

class QuickActionsWidget : public BaseWidget {
public:
    QuickActionsWidget();
    ~QuickActionsWidget() override = default;
    
    void Update(float deltaTime) override;
    void Render() override;
    
    OptimizationProfile GetCurrentProfile() const { return m_currentProfile; }
    void SetCurrentProfile(OptimizationProfile profile) { m_currentProfile = profile; }
    
private:
    void RenderProfileButton(
        const char* label,
        const char* description,
        OptimizationProfile profile,
        ImU32 color
    );
    
    OptimizationProfile m_currentProfile = OptimizationProfile::Balanced;
};

}
