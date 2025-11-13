#pragma once
#include <Windows.h>
#include <string>
#include <vector>

namespace Optimizer {

struct PowerPlan {
    GUID guid;
    std::string name;
    bool active;
};

class PowerOptimizer {
public:
    static PowerOptimizer& Get();
    
    std::vector<PowerPlan> GetPowerPlans();
    bool SetActivePlan(const GUID& guid);
    GUID GetActivePlan();
    
    bool SetCoreParking(int minCores, int maxCores);
    bool DisableThrottling();
    bool SetMaxFrequency(int percent);
    
private:
    PowerOptimizer() = default;
};

}
