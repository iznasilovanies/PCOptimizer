#pragma once
#include <Windows.h>

namespace Optimizer {

enum class MemoryPriority {
    VeryLow = 1,
    Low = 2,
    Medium = 3,
    BelowNormal = 4,
    Normal = 5,
    AboveNormal = 6,
    High = 7,
    VeryHigh = 8
};

class MemoryOptimizer {
public:
    static MemoryOptimizer& Get();
    
    bool SetProcessMemoryPriority(DWORD pid, MemoryPriority priority);
    bool ClearStandbyList();
    bool EmptyProcessWorkingSet(DWORD pid);
    bool SetSystemMemoryPriority(bool enableLowLatency);
    
private:
    MemoryOptimizer() = default;
    
    typedef LONG (NTAPI *NtSetSystemInformationPtr)(ULONG, PVOID, ULONG);
    
    bool m_initialized = false;
    HMODULE m_ntdll = nullptr;
    NtSetSystemInformationPtr m_ntSetSystemInformation = nullptr;
    
    void Initialize();
};

}
