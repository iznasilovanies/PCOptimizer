#include "widget_manager.h"
#include <fstream>
#include <algorithm>

namespace UI {

WidgetManager& WidgetManager::Get() {
    static WidgetManager instance;
    return instance;
}

void WidgetManager::AddWidget(std::shared_ptr<BaseWidget> widget) {
    if (!widget) return;
    
    auto it = m_widgetIndexMap.find(widget->GetID());
    if (it != m_widgetIndexMap.end()) {
        m_widgets[it->second] = widget;
    } else {
        m_widgetIndexMap[widget->GetID()] = m_widgets.size();
        m_widgets.push_back(widget);
    }
}

void WidgetManager::RemoveWidget(const std::string& widgetID) {
    auto it = m_widgetIndexMap.find(widgetID);
    if (it == m_widgetIndexMap.end()) return;
    
    m_widgets.erase(m_widgets.begin() + it->second);
    
    m_widgetIndexMap.clear();
    for (size_t i = 0; i < m_widgets.size(); i++) {
        m_widgetIndexMap[m_widgets[i]->GetID()] = i;
    }
}

std::shared_ptr<BaseWidget> WidgetManager::GetWidget(const std::string& widgetID) {
    auto it = m_widgetIndexMap.find(widgetID);
    if (it == m_widgetIndexMap.end()) return nullptr;
    
    return m_widgets[it->second];
}

void WidgetManager::UpdateAll(float deltaTime) {
    for (auto& widget : m_widgets) {
        if (widget && widget->IsVisible()) {
            widget->Update(deltaTime);
        }
    }
}

void WidgetManager::RenderAll() {
    for (auto& widget : m_widgets) {
        if (widget && widget->IsVisible()) {
            widget->Render();
        }
    }
}

void WidgetManager::SetGridSnap(bool enabled, float gridSize) {
    m_gridSnapEnabled = enabled;
    m_gridSize = gridSize;
}

void WidgetManager::SetAutoLayout(bool enabled) {
    m_autoLayoutEnabled = enabled;
    if (enabled) {
        ApplyAutoLayout();
    }
}

bool WidgetManager::SaveLayout(const std::string& filepath) {
    try {
        json layout;
        layout["version"] = "1.0";
        layout["widgets"] = json::array();
        
        for (const auto& widget : m_widgets) {
            if (!widget) continue;
            
            json widgetData;
            widgetData["id"] = widget->GetID();
            widgetData["type"] = static_cast<int>(widget->GetType());
            widgetData["title"] = widget->GetTitle();
            
            const auto& config = widget->GetConfig();
            widgetData["position"] = {config.position.x, config.position.y};
            widgetData["size"] = {config.size.x, config.size.y};
            widgetData["visible"] = config.visible;
            widgetData["resizable"] = config.resizable;
            widgetData["draggable"] = config.draggable;
            
            layout["widgets"].push_back(widgetData);
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) return false;
        
        file << layout.dump(4);
        file.close();
        
        return true;
    } catch (...) {
        return false;
    }
}

bool WidgetManager::LoadLayout(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) return false;
        
        json layout;
        file >> layout;
        file.close();
        
        if (!layout.contains("widgets")) return false;
        
        for (const auto& widgetData : layout["widgets"]) {
            std::string widgetID = widgetData["id"];
            
            auto widget = GetWidget(widgetID);
            if (!widget) continue;
            
            auto& config = widget->GetConfig();
            
            if (widgetData.contains("position")) {
                config.position.x = widgetData["position"][0];
                config.position.y = widgetData["position"][1];
            }
            
            if (widgetData.contains("size")) {
                config.size.x = widgetData["size"][0];
                config.size.y = widgetData["size"][1];
            }
            
            if (widgetData.contains("visible")) {
                config.visible = widgetData["visible"];
            }
            
            if (widgetData.contains("resizable")) {
                config.resizable = widgetData["resizable"];
            }
            
            if (widgetData.contains("draggable")) {
                config.draggable = widgetData["draggable"];
            }
        }
        
        return true;
    } catch (...) {
        return false;
    }
}

void WidgetManager::Clear() {
    m_widgets.clear();
    m_widgetIndexMap.clear();
}

void WidgetManager::ApplyGridSnap(ImVec2& position) {
    if (!m_gridSnapEnabled) return;
    
    position.x = std::round(position.x / m_gridSize) * m_gridSize;
    position.y = std::round(position.y / m_gridSize) * m_gridSize;
}

void WidgetManager::ApplyAutoLayout() {
    if (m_widgets.empty()) return;
    
    const float padding = 10.0f;
    const int columns = 3;
    
    ImVec2 currentPos(padding, padding);
    int columnIndex = 0;
    float maxHeightInRow = 0;
    
    for (auto& widget : m_widgets) {
        if (!widget || !widget->IsVisible()) continue;
        
        auto& config = widget->GetConfig();
        config.position = currentPos;
        
        maxHeightInRow = std::max(maxHeightInRow, config.size.y);
        
        currentPos.x += config.size.x + padding;
        columnIndex++;
        
        if (columnIndex >= columns) {
            currentPos.x = padding;
            currentPos.y += maxHeightInRow + padding;
            maxHeightInRow = 0;
            columnIndex = 0;
        }
    }
}

}
