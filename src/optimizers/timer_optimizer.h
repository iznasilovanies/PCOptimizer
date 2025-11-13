#pragma once
#include <Windows.h>

namespace Optimizer {

struct TimerCapabilities {
    double minResolution;
    double maxResolution;
    double currentResolution;
};

class TimerOptimizer {
public:
    static TimerOptimizer& Get();
    
    bool SetTimerResolution(double milliseconds);
    double GetCurrentResolution();
    double GetMinResolution();
    double GetMaxResolution();
    
    TimerCapabilities GetCapabilities();
    
    bool ResetToDefault();
    bool ResetTimerResolution();
    
private:
    TimerOptimizer() = default;
    
    typedef LONG (NTAPI *NtSetTimerResolutionPtr)(ULONG, BOOLEAN, PULONG);
    typedef LONG (NTAPI *NtQueryTimerResolutionPtr)(PULONG, PULONG, PULONG);
    
    bool m_initialized = false;
    HMODULE m_ntdll = nullptr;
    NtSetTimerResolutionPtr m_ntSetTimerResolution = nullptr;
    NtQueryTimerResolutionPtr m_ntQueryTimerResolution = nullptr;
    
    void Initialize();
};

}
