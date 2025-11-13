#include "quantum_tweaker.h"
#include <spdlog/spdlog.h>

namespace Optimizer {

QuantumTweaker& QuantumTweaker::Get() {
    static QuantumTweaker instance;
    return instance;
}

HKEY QuantumTweaker::OpenPriorityControlKey() {
    HKEY hKey;
    LONG result = RegOpenKeyExA(HKEY_LOCAL_MACHINE,
                                "SYSTEM\\CurrentControlSet\\Control\\PriorityControl",
                                0, KEY_QUERY_VALUE | KEY_SET_VALUE, &hKey);
    
    if (result != ERROR_SUCCESS) {
        spdlog::error("Failed to open PriorityControl registry key");
        return nullptr;
    }
    
    return hKey;
}

int QuantumTweaker::GetCurrentPrioritySeparation() {
    HKEY hKey = OpenPriorityControlKey();
    if (!hKey) return -1;
    
    DWORD value = 0;
    DWORD size = sizeof(DWORD);
    LONG result = RegQueryValueExA(hKey, "Win32PrioritySeparation", nullptr, nullptr, (BYTE*)&value, &size);
    
    RegCloseKey(hKey);
    
    if (result == ERROR_SUCCESS) {
        return value;
    }
    return -1;
}

bool QuantumTweaker::SetWin32PrioritySeparation(int value) {
    HKEY hKey = OpenPriorityControlKey();
    if (!hKey) return false;
    
    DWORD dwValue = value;
    LONG result = RegSetValueExA(hKey, "Win32PrioritySeparation", 0, REG_DWORD, (BYTE*)&dwValue, sizeof(DWORD));
    
    RegCloseKey(hKey);
    
    if (result == ERROR_SUCCESS) {
        spdlog::info("Set Win32PrioritySeparation to {}", value);
        return true;
    } else {
        spdlog::error("Failed to set Win32PrioritySeparation");
        return false;
    }
}

bool QuantumTweaker::OptimizeForForeground(bool enable) {
    int current = GetCurrentPrioritySeparation();
    if (current == -1) current = 2;
    
    if (enable) {
        current |= 0x01;
    } else {
        current &= ~0x01;
    }
    
    return SetWin32PrioritySeparation(current);
}

bool QuantumTweaker::SetLongQuantum(bool enable) {
    int current = GetCurrentPrioritySeparation();
    if (current == -1) current = 2;
    
    if (enable) {
        current |= 0x10;
    } else {
        current &= ~0x10;
    }
    
    return SetWin32PrioritySeparation(current);
}

bool QuantumTweaker::SetVariableQuantum(bool enable) {
    int current = GetCurrentPrioritySeparation();
    if (current == -1) current = 2;
    
    if (enable) {
        current |= 0x02;
    } else {
        current &= ~0x02;
    }
    
    return SetWin32PrioritySeparation(current);
}

}
