#include "network_optimizer.h"
#include <spdlog/spdlog.h>

namespace Optimizer {

NetworkOptimizer& NetworkOptimizer::Get() {
    static NetworkOptimizer instance;
    return instance;
}

HKEY NetworkOptimizer::OpenTcpParametersKey() {
    HKEY hKey;
    LONG result = RegOpenKeyExA(HKEY_LOCAL_MACHINE,
                                "SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters",
                                0, KEY_QUERY_VALUE | KEY_SET_VALUE, &hKey);
    
    if (result != ERROR_SUCCESS) {
        spdlog::error("Failed to open TCP/IP parameters registry key");
        return nullptr;
    }
    
    return hKey;
}

bool NetworkOptimizer::SetRegistryValue(const std::string& valueName, DWORD value) {
    HKEY hKey = OpenTcpParametersKey();
    if (!hKey) return false;
    
    LONG result = RegSetValueExA(hKey, valueName.c_str(), 0, REG_DWORD, (BYTE*)&value, sizeof(DWORD));
    
    RegCloseKey(hKey);
    
    if (result == ERROR_SUCCESS) {
        spdlog::info("Set {} to {}", valueName, value);
        return true;
    } else {
        spdlog::error("Failed to set {}", valueName);
        return false;
    }
}

bool NetworkOptimizer::SetTcpAckFrequency(int value) {
    HKEY hKey;
    LONG result = RegOpenKeyExA(HKEY_LOCAL_MACHINE,
                                "SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces",
                                0, KEY_ENUMERATE_SUB_KEYS, &hKey);
    
    if (result != ERROR_SUCCESS) {
        spdlog::error("Failed to open network interfaces key");
        return false;
    }
    
    char subKeyName[256];
    DWORD index = 0;
    
    while (RegEnumKeyA(hKey, index++, subKeyName, sizeof(subKeyName)) == ERROR_SUCCESS) {
        std::string interfacePath = "SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces\\";
        interfacePath += subKeyName;
        
        HKEY hInterface;
        if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, interfacePath.c_str(), 0, KEY_SET_VALUE, &hInterface) == ERROR_SUCCESS) {
            DWORD dwValue = value;
            RegSetValueExA(hInterface, "TcpAckFrequency", 0, REG_DWORD, (BYTE*)&dwValue, sizeof(DWORD));
            RegSetValueExA(hInterface, "TCPNoDelay", 0, REG_DWORD, (BYTE*)&dwValue, sizeof(DWORD));
            RegCloseKey(hInterface);
        }
    }
    
    RegCloseKey(hKey);
    
    spdlog::info("Set TcpAckFrequency to {} for all interfaces", value);
    return true;
}

bool NetworkOptimizer::SetTcpNoDelay(bool enable) {
    return SetRegistryValue("TCPNoDelay", enable ? 1 : 0);
}

bool NetworkOptimizer::DisableNagling(bool disable) {
    return SetTcpAckFrequency(disable ? 1 : 2);
}

bool NetworkOptimizer::OptimizeTcpParameters() {
    bool success = true;
    
    success &= SetRegistryValue("TcpTimedWaitDelay", 30);
    success &= SetRegistryValue("MaxUserPort", 65534);
    success &= SetRegistryValue("TcpNumConnections", 16777214);
    success &= SetRegistryValue("DefaultTTL", 64);
    success &= SetRegistryValue("EnablePMTUDiscovery", 1);
    success &= SetRegistryValue("EnableTCPA", 1);
    
    spdlog::info("Optimized TCP parameters");
    return success;
}

bool NetworkOptimizer::EnableRSS(bool enable) {
    HKEY hKey;
    LONG result = RegOpenKeyExA(HKEY_LOCAL_MACHINE,
                                "SYSTEM\\CurrentControlSet\\Services\\Ndis\\Parameters",
                                0, KEY_SET_VALUE, &hKey);
    
    if (result != ERROR_SUCCESS) {
        result = RegCreateKeyExA(HKEY_LOCAL_MACHINE,
                                "SYSTEM\\CurrentControlSet\\Services\\Ndis\\Parameters",
                                0, nullptr, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, nullptr, &hKey, nullptr);
        
        if (result != ERROR_SUCCESS) {
            spdlog::error("Failed to open NDIS parameters key");
            return false;
        }
    }
    
    DWORD value = enable ? 1 : 0;
    RegSetValueExA(hKey, "RSS", 0, REG_DWORD, (BYTE*)&value, sizeof(DWORD));
    
    RegCloseKey(hKey);
    
    spdlog::info("{} RSS (Receive Side Scaling)", enable ? "Enabled" : "Disabled");
    return true;
}

bool NetworkOptimizer::SetReceiveBuffers(int size) {
    return SetRegistryValue("TcpWindowSize", size);
}

bool NetworkOptimizer::OptimizeForGaming() {
    spdlog::info("Applying gaming network optimizations");
    
    bool success = true;
    success &= SetTcpAckFrequency(1);
    success &= SetTcpNoDelay(true);
    success &= DisableNagling(true);
    success &= OptimizeTcpParameters();
    success &= EnableRSS(true);
    
    spdlog::info("Gaming network optimization complete");
    return success;
}

bool NetworkOptimizer::OptimizeForStreaming() {
    spdlog::info("Applying streaming network optimizations");
    
    bool success = true;
    success &= SetTcpAckFrequency(2);
    success &= SetReceiveBuffers(65536);
    success &= EnableRSS(true);
    success &= OptimizeTcpParameters();
    
    spdlog::info("Streaming network optimization complete");
    return success;
}

bool NetworkOptimizer::ResetToDefault() {
    spdlog::info("Resetting network settings to default");
    
    HKEY hKey = OpenTcpParametersKey();
    if (!hKey) return false;
    
    RegDeleteValueA(hKey, "TcpAckFrequency");
    RegDeleteValueA(hKey, "TCPNoDelay");
    RegDeleteValueA(hKey, "TcpTimedWaitDelay");
    RegDeleteValueA(hKey, "MaxUserPort");
    RegDeleteValueA(hKey, "TcpNumConnections");
    
    RegCloseKey(hKey);
    
    spdlog::info("Network settings reset complete");
    return true;
}

}
