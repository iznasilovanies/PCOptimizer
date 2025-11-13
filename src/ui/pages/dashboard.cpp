#include "dashboard.h"
#include <spdlog/spdlog.h>

namespace UI {

Dashboard& Dashboard::Get() {
    static Dashboard instance;
    return instance;
}

void Dashboard::Initialize() {
    if (m_initialized) return;
    
    spdlog::info("Initializing Dashboard...");
    
    WidgetManager::Get().Clear();
    
    auto cpuWidget = std::make_shared<CPUWidget>();
    WidgetManager::Get().AddWidget(cpuWidget);
    
    auto gpuWidget = std::make_shared<GPUWidget>();
    WidgetManager::Get().AddWidget(gpuWidget);
    
    auto ramWidget = std::make_shared<RAMWidget>();
    WidgetManager::Get().AddWidget(ramWidget);
    
    auto quickActionsWidget = std::make_shared<QuickActionsWidget>();
    WidgetManager::Get().AddWidget(quickActionsWidget);
    
    WidgetManager::Get().SetGridSnap(true, 20.0f);
    
    m_initialized = true;
    
    spdlog::info("Dashboard initialized with {} widgets", WidgetManager::Get().GetWidgets().size());
}

void Dashboard::Update(float deltaTime) {
    if (!m_initialized) return;
    
    WidgetManager::Get().UpdateAll(deltaTime);
}

void Dashboard::Render() {
    if (!m_initialized) return;
    
    WidgetManager::Get().RenderAll();
}

void Dashboard::Shutdown() {
    if (!m_initialized) return;
    
    WidgetManager::Get().Clear();
    m_initialized = false;
    
    spdlog::info("Dashboard shut down");
}

bool Dashboard::SaveLayout(const std::string& filepath) {
    return WidgetManager::Get().SaveLayout(filepath);
}

bool Dashboard::LoadLayout(const std::string& filepath) {
    return WidgetManager::Get().LoadLayout(filepath);
}

void Dashboard::ResetLayout() {
    Shutdown();
    Initialize();
}

}
