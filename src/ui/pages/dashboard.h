#pragma once
#include <memory>
#include "../widgets/widget_manager.h"
#include "../widgets/cpu_widget.h"
#include "../widgets/gpu_widget.h"
#include "../widgets/ram_widget.h"
#include "../widgets/quick_actions_widget.h"

namespace UI {

class Dashboard {
public:
    static Dashboard& Get();
    
    void Initialize();
    void Update(float deltaTime);
    void Render();
    void Shutdown();
    
    bool SaveLayout(const std::string& filepath);
    bool LoadLayout(const std::string& filepath);
    
    void ResetLayout();
    
private:
    Dashboard() = default;
    
    bool m_initialized = false;
};

}
