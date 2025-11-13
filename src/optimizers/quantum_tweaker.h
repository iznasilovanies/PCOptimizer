#pragma once
#include <Windows.h>

namespace Optimizer {

class QuantumTweaker {
public:
    static QuantumTweaker& Get();
    
    bool SetWin32PrioritySeparation(int value);
    bool OptimizeForForeground(bool enable);
    bool SetLongQuantum(bool enable);
    bool SetVariableQuantum(bool enable);
    
    int GetCurrentPrioritySeparation();
    
private:
    QuantumTweaker() = default;
    
    HKEY OpenPriorityControlKey();
};

}
