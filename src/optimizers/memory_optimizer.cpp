#include "memory_optimizer.h"
#include <spdlog/spdlog.h>
#include <Psapi.h>

#pragma comment(lib, "Psapi.lib")

#undef min
#undef max

namespace Optimizer {

MemoryOptimizer& MemoryOptimizer::Get() {
    static MemoryOptimizer instance;
    return instance;
}

void MemoryOptimizer::Initialize() {
    if (m_initialized) return;
    
    m_ntdll = GetModuleHandleA("ntdll.dll");
    if (!m_ntdll) {
        spdlog::error("Failed to get ntdll.dll handle");
        return;
    }
    
    m_ntSetSystemInformation = (NtSetSystemInformationPtr)GetProcAddress(m_ntdll, "NtSetSystemInformation");
    
    if (!m_ntSetSystemInformation) {
        spdlog::warn("NtSetSystemInformation not available (requires admin)");
    }
    
    m_initialized = true;
}

bool MemoryOptimizer::SetProcessMemoryPriority(DWORD pid, MemoryPriority priority) {
    HANDLE hProcess = OpenProcess(PROCESS_SET_INFORMATION, FALSE, pid);
    if (!hProcess) {
        spdlog::error("Failed to open process {}: {}", pid, GetLastError());
        return false;
    }
    
    typedef enum _MEMORY_PRIORITY_INFORMATION {
        MemoryPriorityVeryLow = 1,
        MemoryPriorityLow = 2,
        MemoryPriorityMedium = 3,
        MemoryPriorityBelowNormal = 4,
        MemoryPriorityNormal = 5
    } MEMORY_PRIORITY_INFORMATION;
    
    MEMORY_PRIORITY_INFORMATION memInfo;
    memInfo = static_cast<MEMORY_PRIORITY_INFORMATION>(static_cast<int>(priority));
    
    typedef BOOL (WINAPI *SetProcessInformationPtr)(HANDLE, DWORD, LPVOID, DWORD);
    static SetProcessInformationPtr pSetProcessInformation = nullptr;
    
    if (!pSetProcessInformation) {
        HMODULE hKernel32 = GetModuleHandleA("kernel32.dll");
        pSetProcessInformation = (SetProcessInformationPtr)GetProcAddress(hKernel32, "SetProcessInformation");
    }
    
    bool success = false;
    if (pSetProcessInformation) {
        const DWORD ProcessMemoryPriority = 0;
        success = pSetProcessInformation(hProcess, ProcessMemoryPriority, &memInfo, sizeof(memInfo));
        
        if (success) {
            spdlog::info("Set memory priority for PID {} to {}", pid, static_cast<int>(priority));
        } else {
            spdlog::error("Failed to set memory priority: {}", GetLastError());
        }
    } else {
        spdlog::error("SetProcessInformation not available (Windows 8+)");
    }
    
    CloseHandle(hProcess);
    return success;
}

bool MemoryOptimizer::ClearStandbyList() {
    Initialize();
    if (!m_ntSetSystemInformation) {
        spdlog::error("NtSetSystemInformation not available");
        return false;
    }
    
    DWORD command = 4;
    LONG status = m_ntSetSystemInformation(0x50, &command, sizeof(command));
    
    if (status == 0) {
        spdlog::info("Cleared standby list");
        return true;
    } else {
        spdlog::error("Failed to clear standby list: 0x{:X}", status);
        return false;
    }
}

bool MemoryOptimizer::EmptyProcessWorkingSet(DWORD pid) {
    HANDLE hProcess = OpenProcess(PROCESS_SET_QUOTA, FALSE, pid);
    if (!hProcess) {
        spdlog::error("Failed to open process {}", pid);
        return false;
    }
    
    typedef BOOL (WINAPI *K32EmptyWorkingSetPtr)(HANDLE);
    static K32EmptyWorkingSetPtr pK32EmptyWorkingSet = nullptr;
    
    if (!pK32EmptyWorkingSet) {
        HMODULE hKernel32 = GetModuleHandleA("kernel32.dll");
        pK32EmptyWorkingSet = (K32EmptyWorkingSetPtr)GetProcAddress(hKernel32, "K32EmptyWorkingSet");
    }
    
    BOOL result = FALSE;
    if (pK32EmptyWorkingSet) {
        result = pK32EmptyWorkingSet(hProcess);
    }
    
    CloseHandle(hProcess);
    
    if (result) {
        spdlog::info("Emptied working set for PID {}", pid);
        return true;
    } else {
        spdlog::error("Failed to empty working set: {}", GetLastError());
        return false;
    }
}

bool MemoryOptimizer::SetSystemMemoryPriority(bool enableLowLatency) {
    HKEY hKey;
    LONG result = RegOpenKeyExA(HKEY_LOCAL_MACHINE, 
                                "SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Memory Management",
                                0, KEY_SET_VALUE, &hKey);
    
    if (result != ERROR_SUCCESS) {
        spdlog::error("Failed to open memory management registry key");
        return false;
    }
    
    DWORD value = enableLowLatency ? 1 : 0;
    RegSetValueExA(hKey, "LargeSystemCache", 0, REG_DWORD, (BYTE*)&value, sizeof(DWORD));
    
    RegCloseKey(hKey);
    
    spdlog::info("Set system memory priority (LowLatency: {})", enableLowLatency);
    return true;
}

}
