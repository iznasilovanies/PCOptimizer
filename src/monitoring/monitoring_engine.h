#pragma once
#include <string>
#include <vector>
#include <map>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>

namespace Monitor {

struct CPUCoreInfo {
    int coreID;
    float frequency;
    float temperature;
    float usage;
    float cState;
};

struct GPUInfo {
    std::string name;
    float coreClock;
    float memoryClock;
    float temperature;
    float usage;
    float memoryUsage;
    float memoryTotal;
    float powerUsage;
    int fanSpeed;
};

struct RAMInfo {
    float totalGB;
    float usedGB;
    float availableGB;
    float usagePercent;
    int speedMHz;
    float latencyNs;
};

struct DiskInfo {
    std::string name;
    float readMBps;
    float writeMBps;
    int readIOPS;
    int writeIOPS;
    float latencyMs;
    float temperature;
    float usagePercent;
};

struct NetworkInfo {
    std::string adapterName;
    float uploadMbps;
    float downloadMbps;
    float latencyMs;
    float packetLoss;
};

struct ProcessInfo {
    std::string name;
    unsigned long pid;
    float cpuUsage;
    float gpuUsage;
    float memoryMB;
    int threads;
    int handles;
};

class MonitoringEngine {
public:
    static MonitoringEngine& Get();
    ~MonitoringEngine();
    
    void Start(int pollingRateMs = 1000);
    void Stop();
    bool IsRunning() const { return m_running; }
    
    std::vector<CPUCoreInfo> GetCPUInfo();
    GPUInfo GetGPUInfo();
    RAMInfo GetRAMInfo();
    std::vector<DiskInfo> GetDiskInfo();
    NetworkInfo GetNetworkInfo();
    std::vector<ProcessInfo> GetTopProcesses(int count = 10);
    
    void SetPollingRate(int ms);
    
private:
    MonitoringEngine();
    
    void MonitoringThread();
    
    void UpdateCPUInfo();
    void UpdateGPUInfo();
    void UpdateRAMInfo();
    void UpdateDiskInfo();
    void UpdateNetworkInfo();
    void UpdateProcessInfo();
    
    std::thread m_thread;
    std::atomic<bool> m_running{false};
    std::atomic<int> m_pollingRateMs{1000};
    
    std::mutex m_cpuMutex;
    std::mutex m_gpuMutex;
    std::mutex m_ramMutex;
    std::mutex m_diskMutex;
    std::mutex m_networkMutex;
    std::mutex m_processMutex;
    
    std::vector<CPUCoreInfo> m_cpuInfo;
    GPUInfo m_gpuInfo;
    RAMInfo m_ramInfo;
    std::vector<DiskInfo> m_diskInfo;
    NetworkInfo m_networkInfo;
    std::vector<ProcessInfo> m_topProcesses;
    
    std::chrono::steady_clock::time_point m_lastUpdate;
};

}
