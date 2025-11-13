#include "monitoring_engine.h"
#include <Windows.h>
#include <Pdh.h>
#include <PdhMsg.h>

#undef min
#undef max

#include <algorithm>
#include <spdlog/spdlog.h>

#pragma comment(lib, "pdh.lib")

namespace Monitor {

MonitoringEngine& MonitoringEngine::Get() {
    static MonitoringEngine instance;
    return instance;
}

MonitoringEngine::MonitoringEngine() {
    m_lastUpdate = std::chrono::steady_clock::now();
}

MonitoringEngine::~MonitoringEngine() {
    Stop();
}

void MonitoringEngine::Start(int pollingRateMs) {
    if (m_running) return;
    
    m_pollingRateMs = pollingRateMs;
    m_running = true;
    m_thread = std::thread(&MonitoringEngine::MonitoringThread, this);
    
    spdlog::info("Monitoring engine started with polling rate: {}ms", pollingRateMs);
}

void MonitoringEngine::Stop() {
    if (!m_running) return;
    
    m_running = false;
    if (m_thread.joinable()) {
        m_thread.join();
    }
    
    spdlog::info("Monitoring engine stopped");
}

void MonitoringEngine::SetPollingRate(int ms) {
    m_pollingRateMs = std::max(100, std::min(5000, ms));
}

std::vector<CPUCoreInfo> MonitoringEngine::GetCPUInfo() {
    std::lock_guard<std::mutex> lock(m_cpuMutex);
    return m_cpuInfo;
}

GPUInfo MonitoringEngine::GetGPUInfo() {
    std::lock_guard<std::mutex> lock(m_gpuMutex);
    return m_gpuInfo;
}

RAMInfo MonitoringEngine::GetRAMInfo() {
    std::lock_guard<std::mutex> lock(m_ramMutex);
    return m_ramInfo;
}

std::vector<DiskInfo> MonitoringEngine::GetDiskInfo() {
    std::lock_guard<std::mutex> lock(m_diskMutex);
    return m_diskInfo;
}

NetworkInfo MonitoringEngine::GetNetworkInfo() {
    std::lock_guard<std::mutex> lock(m_networkMutex);
    return m_networkInfo;
}

std::vector<ProcessInfo> MonitoringEngine::GetTopProcesses(int count) {
    std::lock_guard<std::mutex> lock(m_processMutex);
    
    if (count >= static_cast<int>(m_topProcesses.size())) {
        return m_topProcesses;
    }
    
    return std::vector<ProcessInfo>(m_topProcesses.begin(), m_topProcesses.begin() + count);
}

void MonitoringEngine::MonitoringThread() {
    while (m_running) {
        auto start = std::chrono::steady_clock::now();
        
        UpdateCPUInfo();
        UpdateGPUInfo();
        UpdateRAMInfo();
        UpdateDiskInfo();
        UpdateNetworkInfo();
        UpdateProcessInfo();
        
        m_lastUpdate = std::chrono::steady_clock::now();
        
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - start
        );
        
        int sleepTime = m_pollingRateMs - static_cast<int>(elapsed.count());
        if (sleepTime > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
        }
    }
}

void MonitoringEngine::UpdateCPUInfo() {
    std::lock_guard<std::mutex> lock(m_cpuMutex);
    
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    int coreCount = sysInfo.dwNumberOfProcessors;
    
    m_cpuInfo.clear();
    m_cpuInfo.reserve(coreCount);
    
    static PDH_HQUERY cpuQuery = nullptr;
    static std::vector<PDH_HCOUNTER> cpuCounters;
    
    if (cpuQuery == nullptr) {
        PdhOpenQuery(nullptr, 0, &cpuQuery);
        cpuCounters.resize(coreCount);
        
        for (int i = 0; i < coreCount; i++) {
            wchar_t counterPath[256];
            swprintf_s(counterPath, L"\\Processor(%d)\\%% Processor Time", i);
            PdhAddCounterW(cpuQuery, counterPath, 0, &cpuCounters[i]);
        }
        
        PdhCollectQueryData(cpuQuery);
    }
    
    PdhCollectQueryData(cpuQuery);
    
    for (int i = 0; i < coreCount; i++) {
        CPUCoreInfo info;
        info.coreID = i;
        info.frequency = 0.0f;
        info.temperature = 0.0f;
        info.cState = 0.0f;
        
        PDH_FMT_COUNTERVALUE counterVal;
        if (PdhGetFormattedCounterValue(cpuCounters[i], PDH_FMT_DOUBLE, nullptr, &counterVal) == ERROR_SUCCESS) {
            info.usage = static_cast<float>(counterVal.doubleValue);
        } else {
            info.usage = 0.0f;
        }
        
        m_cpuInfo.push_back(info);
    }
}

void MonitoringEngine::UpdateGPUInfo() {
    std::lock_guard<std::mutex> lock(m_gpuMutex);
    
    m_gpuInfo.name = "Unknown GPU";
    m_gpuInfo.coreClock = 0.0f;
    m_gpuInfo.memoryClock = 0.0f;
    m_gpuInfo.temperature = 0.0f;
    m_gpuInfo.usage = 0.0f;
    m_gpuInfo.memoryUsage = 0.0f;
    m_gpuInfo.memoryTotal = 0.0f;
    m_gpuInfo.powerUsage = 0.0f;
    m_gpuInfo.fanSpeed = 0;
}

void MonitoringEngine::UpdateRAMInfo() {
    std::lock_guard<std::mutex> lock(m_ramMutex);
    
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);
    
    m_ramInfo.totalGB = static_cast<float>(memInfo.ullTotalPhys) / (1024.0f * 1024.0f * 1024.0f);
    m_ramInfo.availableGB = static_cast<float>(memInfo.ullAvailPhys) / (1024.0f * 1024.0f * 1024.0f);
    m_ramInfo.usedGB = m_ramInfo.totalGB - m_ramInfo.availableGB;
    m_ramInfo.usagePercent = static_cast<float>(memInfo.dwMemoryLoad);
    m_ramInfo.speedMHz = 0;
    m_ramInfo.latencyNs = 0.0f;
}

void MonitoringEngine::UpdateDiskInfo() {
    std::lock_guard<std::mutex> lock(m_diskMutex);
    
    m_diskInfo.clear();
    
    DWORD drives = GetLogicalDrives();
    for (int i = 0; i < 26; i++) {
        if (drives & (1 << i)) {
            char driveLetter = 'A' + i;
            std::string drivePath = std::string(1, driveLetter) + ":\\";
            
            UINT driveType = GetDriveTypeA(drivePath.c_str());
            if (driveType == DRIVE_FIXED || driveType == DRIVE_REMOVABLE) {
                DiskInfo info;
                info.name = drivePath;
                info.readMBps = 0.0f;
                info.writeMBps = 0.0f;
                info.readIOPS = 0;
                info.writeIOPS = 0;
                info.latencyMs = 0.0f;
                info.temperature = 0.0f;
                
                ULARGE_INTEGER freeBytesAvailable, totalNumberOfBytes, totalNumberOfFreeBytes;
                if (GetDiskFreeSpaceExA(drivePath.c_str(), &freeBytesAvailable, &totalNumberOfBytes, &totalNumberOfFreeBytes)) {
                    float usedBytes = static_cast<float>(totalNumberOfBytes.QuadPart - totalNumberOfFreeBytes.QuadPart);
                    info.usagePercent = (usedBytes / totalNumberOfBytes.QuadPart) * 100.0f;
                }
                
                m_diskInfo.push_back(info);
            }
        }
    }
}

void MonitoringEngine::UpdateNetworkInfo() {
    std::lock_guard<std::mutex> lock(m_networkMutex);
    
    m_networkInfo.adapterName = "Default";
    m_networkInfo.uploadMbps = 0.0f;
    m_networkInfo.downloadMbps = 0.0f;
    m_networkInfo.latencyMs = 0.0f;
    m_networkInfo.packetLoss = 0.0f;
}

void MonitoringEngine::UpdateProcessInfo() {
    std::lock_guard<std::mutex> lock(m_processMutex);
    
    m_topProcesses.clear();
}

}
