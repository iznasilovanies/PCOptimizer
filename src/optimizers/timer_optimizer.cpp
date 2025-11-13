#include "timer_optimizer.h"
#include <spdlog/spdlog.h>

#undef min
#undef max

namespace Optimizer {

TimerOptimizer& TimerOptimizer::Get() {
    static TimerOptimizer instance;
    return instance;
}

void TimerOptimizer::Initialize() {
    if (m_initialized) return;
    
    m_ntdll = GetModuleHandleA("ntdll.dll");
    if (!m_ntdll) {
        spdlog::error("Failed to get ntdll.dll handle");
        return;
    }
    
    m_ntSetTimerResolution = (NtSetTimerResolutionPtr)GetProcAddress(m_ntdll, "NtSetTimerResolution");
    m_ntQueryTimerResolution = (NtQueryTimerResolutionPtr)GetProcAddress(m_ntdll, "NtQueryTimerResolution");
    
    if (!m_ntSetTimerResolution || !m_ntQueryTimerResolution) {
        spdlog::error("Failed to get NT timer functions");
        return;
    }
    
    m_initialized = true;
    spdlog::info("Timer optimizer initialized");
}

bool TimerOptimizer::SetTimerResolution(double milliseconds) {
    Initialize();
    if (!m_initialized) return false;
    
    ULONG desired = static_cast<ULONG>(milliseconds * 10000.0);
    ULONG actual = 0;
    
    LONG status = m_ntSetTimerResolution(desired, TRUE, &actual);
    
    if (status == 0) {
        spdlog::info("Set timer resolution to {:.2f}ms (actual: {:.2f}ms)", 
                     milliseconds, actual / 10000.0);
        return true;
    } else {
        spdlog::error("Failed to set timer resolution: 0x{:X}", status);
        return false;
    }
}

double TimerOptimizer::GetCurrentResolution() {
    Initialize();
    if (!m_initialized) return 15.6;
    
    ULONG min, max, current;
    LONG status = m_ntQueryTimerResolution(&min, &max, &current);
    
    if (status == 0) {
        return current / 10000.0;
    }
    return 15.6;
}

double TimerOptimizer::GetMinResolution() {
    Initialize();
    if (!m_initialized) return 0.5;
    
    ULONG min, max, current;
    LONG status = m_ntQueryTimerResolution(&min, &max, &current);
    
    if (status == 0) {
        return max / 10000.0;
    }
    return 0.5;
}

double TimerOptimizer::GetMaxResolution() {
    Initialize();
    if (!m_initialized) return 15.6;
    
    ULONG min, max, current;
    LONG status = m_ntQueryTimerResolution(&min, &max, &current);
    
    if (status == 0) {
        return min / 10000.0;
    }
    return 15.6;
}

bool TimerOptimizer::ResetToDefault() {
    return SetTimerResolution(15.6);
}

bool TimerOptimizer::ResetTimerResolution() {
    return ResetToDefault();
}

TimerCapabilities TimerOptimizer::GetCapabilities() {
    TimerCapabilities caps;
    caps.minResolution = GetMinResolution();
    caps.maxResolution = GetMaxResolution();
    caps.currentResolution = GetCurrentResolution();
    return caps;
}

}
