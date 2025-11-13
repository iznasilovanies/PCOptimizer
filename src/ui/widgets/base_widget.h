#pragma once
#include <imgui.h>
#include <string>
#include <memory>

namespace UI {

enum class WidgetType {
    CPUMonitor,
    GPUMonitor,
    RAMMonitor,
    DiskMonitor,
    NetworkMonitor,
    ProcessMonitor,
    QuickActions,
    Custom
};

struct WidgetConfig {
    ImVec2 position;
    ImVec2 size;
    bool visible = true;
    bool resizable = true;
    bool draggable = true;
    
    WidgetConfig(ImVec2 pos = ImVec2(0, 0), ImVec2 size = ImVec2(400, 300))
        : position(pos), size(size) {}
};

class BaseWidget {
public:
    BaseWidget(const std::string& id, const std::string& title, WidgetType type)
        : m_id(id), m_title(title), m_type(type) {}
    
    virtual ~BaseWidget() = default;
    
    virtual void Update(float deltaTime) = 0;
    virtual void Render() = 0;
    
    const std::string& GetID() const { return m_id; }
    const std::string& GetTitle() const { return m_title; }
    WidgetType GetType() const { return m_type; }
    
    WidgetConfig& GetConfig() { return m_config; }
    const WidgetConfig& GetConfig() const { return m_config; }
    
    bool IsVisible() const { return m_config.visible; }
    void SetVisible(bool visible) { m_config.visible = visible; }
    
    bool IsHovered() const { return m_hovered; }
    bool IsDragging() const { return m_dragging; }
    bool IsResizing() const { return m_resizing; }
    
protected:
    void BeginWidget();
    void EndWidget();
    
    void RenderHeader();
    void RenderResizeHandles();
    void HandleDragDrop();
    
    std::string m_id;
    std::string m_title;
    WidgetType m_type;
    WidgetConfig m_config;
    
    bool m_hovered = false;
    bool m_dragging = false;
    bool m_resizing = false;
    
    ImVec2 m_dragOffset;
    ImVec2 m_resizeStartSize;
    ImVec2 m_resizeStartPos;
};

}
