#pragma once
#include <Windows.h>
#include <string>

namespace Optimizer {

class NetworkOptimizer {
public:
    static NetworkOptimizer& Get();
    
    bool SetTcpAckFrequency(int value);
    bool SetTcpNoDelay(bool enable);
    bool DisableNagling(bool disable);
    
    bool OptimizeTcpParameters();
    bool EnableRSS(bool enable);
    bool SetReceiveBuffers(int size);
    
    bool OptimizeForGaming();
    bool OptimizeForStreaming();
    bool ResetToDefault();
    
private:
    NetworkOptimizer() = default;
    
    HKEY OpenTcpParametersKey();
    bool SetRegistryValue(const std::string& valueName, DWORD value);
};

}
