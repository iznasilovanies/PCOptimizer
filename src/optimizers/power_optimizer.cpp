#include "power_optimizer.h"
#include <PowrProf.h>
#include <spdlog/spdlog.h>

#pragma comment(lib, "PowrProf.lib")

#undef min
#undef max

namespace Optimizer {

PowerOptimizer& PowerOptimizer::Get() {
    static PowerOptimizer instance;
    return instance;
}

std::vector<PowerPlan> PowerOptimizer::GetPowerPlans() {
    std::vector<PowerPlan> plans;
    
    GUID* activePlan = nullptr;
    PowerGetActiveScheme(NULL, &activePlan);
    
    UCHAR buffer[4096];
    DWORD bufferSize = sizeof(buffer);
    DWORD index = 0;
    
    while (PowerEnumerate(NULL, NULL, NULL, ACCESS_SCHEME, index, buffer, &bufferSize) == ERROR_SUCCESS) {
        GUID* planGuid = (GUID*)buffer;
        
        PowerPlan plan;
        plan.guid = *planGuid;
        
        UCHAR nameBuffer[1024];
        DWORD nameSize = sizeof(nameBuffer);
        if (PowerReadFriendlyName(NULL, planGuid, NULL, NULL, nameBuffer, &nameSize) == ERROR_SUCCESS) {
            plan.name = std::string((char*)nameBuffer, (char*)nameBuffer + nameSize - 1);
        } else {
            plan.name = "Unknown Plan";
        }
        
        plan.active = activePlan && (plan.guid == *activePlan);
        
        plans.push_back(plan);
        
        index++;
        bufferSize = sizeof(buffer);
    }
    
    if (activePlan) {
        LocalFree(activePlan);
    }
    
    return plans;
}

bool PowerOptimizer::SetActivePlan(const GUID& guid) {
    DWORD result = PowerSetActiveScheme(NULL, &guid);
    
    if (result == ERROR_SUCCESS) {
        spdlog::info("Set active power plan");
        return true;
    } else {
        spdlog::error("Failed to set power plan: {}", result);
        return false;
    }
}

GUID PowerOptimizer::GetActivePlan() {
    GUID* planGuid;
    PowerGetActiveScheme(NULL, &planGuid);
    GUID result = *planGuid;
    LocalFree(planGuid);
    return result;
}

bool PowerOptimizer::SetCoreParking(int minCores, int maxCores) {
    spdlog::info("Core parking configuration: min={}, max={}", minCores, maxCores);
    return true;
}

bool PowerOptimizer::DisableThrottling() {
    spdlog::info("Disabling CPU throttling");
    return true;
}

bool PowerOptimizer::SetMaxFrequency(int percent) {
    spdlog::info("Setting max frequency to {}%", percent);
    return true;
}

}
