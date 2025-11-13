#include "interrupt_optimizer.h"
#include <spdlog/spdlog.h>
#include <SetupAPI.h>
#include <devguid.h>
#include <initguid.h>
#include <cfgmgr32.h>

#pragma comment(lib, "Setupapi.lib")
#pragma comment(lib, "Cfgmgr32.lib")

namespace Optimizer {

InterruptOptimizer& InterruptOptimizer::Get() {
    static InterruptOptimizer instance;
    return instance;
}

std::vector<InterruptDevice> InterruptOptimizer::EnumerateDevices() {
    std::vector<InterruptDevice> devices;
    
    HDEVINFO deviceInfoSet = SetupDiGetClassDevs(nullptr, nullptr, nullptr, DIGCF_ALLCLASSES | DIGCF_PRESENT);
    if (deviceInfoSet == INVALID_HANDLE_VALUE) {
        spdlog::error("Failed to get device info set");
        return devices;
    }
    
    SP_DEVINFO_DATA deviceInfoData;
    deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
    
    for (DWORD i = 0; SetupDiEnumDeviceInfo(deviceInfoSet, i, &deviceInfoData); i++) {
        char deviceName[256] = {0};
        char deviceInstanceId[256] = {0};
        
        SetupDiGetDeviceRegistryPropertyA(deviceInfoSet, &deviceInfoData, SPDRP_DEVICEDESC, 
                                         nullptr, (PBYTE)deviceName, sizeof(deviceName), nullptr);
        
        CM_Get_Device_IDA(deviceInfoData.DevInst, deviceInstanceId, sizeof(deviceInstanceId), 0);
        
        InterruptDevice device;
        device.deviceName = deviceName;
        device.deviceInstanceId = deviceInstanceId;
        device.irqNumber = -1;
        device.currentAffinity = 0;
        device.interruptPolicy = 0;
        
        devices.push_back(device);
    }
    
    SetupDiDestroyDeviceInfoList(deviceInfoSet);
    
    spdlog::info("Enumerated {} devices", devices.size());
    return devices;
}

std::string InterruptOptimizer::GetRegistryPath(const std::string& deviceInstanceId) {
    return "SYSTEM\\CurrentControlSet\\Enum\\" + deviceInstanceId + "\\Device Parameters\\Interrupt Management\\Affinity Policy";
}

bool InterruptOptimizer::SetAffinityInRegistry(const std::string& deviceInstanceId, DWORD_PTR affinity) {
    std::string regPath = "SYSTEM\\CurrentControlSet\\Enum\\" + deviceInstanceId + "\\Device Parameters\\Interrupt Management\\Affinity Policy";
    
    HKEY hKey;
    LONG result = RegOpenKeyExA(HKEY_LOCAL_MACHINE, regPath.c_str(), 0, KEY_SET_VALUE, &hKey);
    
    if (result != ERROR_SUCCESS) {
        result = RegCreateKeyExA(HKEY_LOCAL_MACHINE, regPath.c_str(), 0, nullptr, 
                                REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, nullptr, &hKey, nullptr);
        if (result != ERROR_SUCCESS) {
            spdlog::error("Failed to create registry key: {}", regPath);
            return false;
        }
    }
    
    DWORD policyValue = 4;
    RegSetValueExA(hKey, "DevicePolicy", 0, REG_DWORD, (BYTE*)&policyValue, sizeof(DWORD));
    
    RegSetValueExA(hKey, "AssignmentSetOverride", 0, REG_QWORD, (BYTE*)&affinity, sizeof(DWORD_PTR));
    
    RegCloseKey(hKey);
    
    spdlog::info("Set interrupt affinity for {}: 0x{:X}", deviceInstanceId, affinity);
    return true;
}

bool InterruptOptimizer::SetPolicyInRegistry(const std::string& deviceInstanceId, int policy) {
    std::string regPath = "SYSTEM\\CurrentControlSet\\Enum\\" + deviceInstanceId + "\\Device Parameters\\Interrupt Management\\Affinity Policy";
    
    HKEY hKey;
    LONG result = RegCreateKeyExA(HKEY_LOCAL_MACHINE, regPath.c_str(), 0, nullptr, 
                                 REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, nullptr, &hKey, nullptr);
    
    if (result != ERROR_SUCCESS) {
        spdlog::error("Failed to create registry key");
        return false;
    }
    
    DWORD policyValue = policy;
    RegSetValueExA(hKey, "DevicePolicy", 0, REG_DWORD, (BYTE*)&policyValue, sizeof(DWORD));
    
    RegCloseKey(hKey);
    return true;
}

bool InterruptOptimizer::SetDeviceAffinity(const std::string& deviceInstanceId, DWORD_PTR affinityMask) {
    return SetAffinityInRegistry(deviceInstanceId, affinityMask);
}

bool InterruptOptimizer::SetDevicePolicy(const std::string& deviceInstanceId, int policy) {
    return SetPolicyInRegistry(deviceInstanceId, policy);
}

bool InterruptOptimizer::RouteGPUInterruptsToCore(int coreId) {
    DWORD_PTR affinity = 1ULL << coreId;
    
    auto devices = EnumerateDevices();
    bool success = false;
    
    for (const auto& device : devices) {
        if (device.deviceName.find("NVIDIA") != std::string::npos || 
            device.deviceName.find("AMD") != std::string::npos ||
            device.deviceName.find("Radeon") != std::string::npos ||
            device.deviceName.find("GeForce") != std::string::npos ||
            device.deviceName.find("Display") != std::string::npos) {
            
            if (SetDeviceAffinity(device.deviceInstanceId, affinity)) {
                spdlog::info("Routed GPU '{}' interrupts to core {}", device.deviceName, coreId);
                success = true;
            }
        }
    }
    
    return success;
}

bool InterruptOptimizer::RouteNetworkInterruptsToCore(int coreId) {
    DWORD_PTR affinity = 1ULL << coreId;
    
    auto devices = EnumerateDevices();
    bool success = false;
    
    for (const auto& device : devices) {
        if (device.deviceName.find("Network") != std::string::npos ||
            device.deviceName.find("Ethernet") != std::string::npos ||
            device.deviceName.find("Wi-Fi") != std::string::npos ||
            device.deviceName.find("Wireless") != std::string::npos) {
            
            if (SetDeviceAffinity(device.deviceInstanceId, affinity)) {
                spdlog::info("Routed Network '{}' interrupts to core {}", device.deviceName, coreId);
                success = true;
            }
        }
    }
    
    return success;
}

bool InterruptOptimizer::RouteUSBInterruptsToCore(int coreId) {
    DWORD_PTR affinity = 1ULL << coreId;
    
    auto devices = EnumerateDevices();
    bool success = false;
    
    for (const auto& device : devices) {
        if (device.deviceName.find("USB") != std::string::npos ||
            device.deviceName.find("xHCI") != std::string::npos ||
            device.deviceName.find("EHCI") != std::string::npos) {
            
            if (SetDeviceAffinity(device.deviceInstanceId, affinity)) {
                spdlog::info("Routed USB '{}' interrupts to core {}", device.deviceName, coreId);
                success = true;
            }
        }
    }
    
    return success;
}

}
