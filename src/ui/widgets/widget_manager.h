#pragma once
#include "base_widget.h"
#include <nlohmann/json.hpp>
#include <vector>
#include <memory>
#include <map>
#include <string>

namespace UI {

using json = nlohmann::json;

class WidgetManager {
public:
    static WidgetManager& Get();
    
    void AddWidget(std::shared_ptr<BaseWidget> widget);
    void RemoveWidget(const std::string& widgetID);
    std::shared_ptr<BaseWidget> GetWidget(const std::string& widgetID);
    
    void UpdateAll(float deltaTime);
    void RenderAll();
    
    void SetGridSnap(bool enabled, float gridSize = 20.0f);
    void SetAutoLayout(bool enabled);
    
    bool SaveLayout(const std::string& filepath);
    bool LoadLayout(const std::string& filepath);
    
    void Clear();
    
    const std::vector<std::shared_ptr<BaseWidget>>& GetWidgets() const { return m_widgets; }
    
private:
    WidgetManager() = default;
    
    void ApplyGridSnap(ImVec2& position);
    void ApplyAutoLayout();
    
    std::vector<std::shared_ptr<BaseWidget>> m_widgets;
    std::map<std::string, size_t> m_widgetIndexMap;
    
    bool m_gridSnapEnabled = false;
    float m_gridSize = 20.0f;
    bool m_autoLayoutEnabled = false;
};

}
