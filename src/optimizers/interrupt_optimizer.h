#pragma once
#include <Windows.h>
#include <string>
#include <vector>

namespace Optimizer {

struct InterruptDevice {
    std::string deviceName;
    std::string deviceInstanceId;
    int irqNumber;
    DWORD_PTR currentAffinity;
    int interruptPolicy;
};

class InterruptOptimizer {
public:
    static InterruptOptimizer& Get();
    
    std::vector<InterruptDevice> EnumerateDevices();
    
    bool SetDeviceAffinity(const std::string& deviceInstanceId, DWORD_PTR affinityMask);
    bool SetDevicePolicy(const std::string& deviceInstanceId, int policy);
    
    bool RouteGPUInterruptsToCore(int coreId);
    bool RouteNetworkInterruptsToCore(int coreId);
    bool RouteUSBInterruptsToCore(int coreId);
    
private:
    InterruptOptimizer() = default;
    
    std::string GetRegistryPath(const std::string& deviceInstanceId);
    bool SetAffinityInRegistry(const std::string& deviceInstanceId, DWORD_PTR affinity);
    bool SetPolicyInRegistry(const std::string& deviceInstanceId, int policy);
};

}
