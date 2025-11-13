#include "thread_optimizer.h"
#include <TlHelp32.h>
#include <spdlog/spdlog.h>

#undef min
#undef max

namespace Optimizer {

ThreadOptimizer& ThreadOptimizer::Get() {
    static ThreadOptimizer instance;
    return instance;
}

bool ThreadOptimizer::SetProcessAffinity(DWORD pid, DWORD_PTR affinityMask) {
    HANDLE hProcess = OpenProcess(PROCESS_SET_INFORMATION, FALSE, pid);
    if (!hProcess) {
        spdlog::error("Failed to open process {}: {}", pid, GetLastError());
        return false;
    }
    
    BOOL result = SetProcessAffinityMask(hProcess, affinityMask);
    CloseHandle(hProcess);
    
    if (!result) {
        spdlog::error("Failed to set process affinity: {}", GetLastError());
        return false;
    }
    
    spdlog::info("Set process {} affinity to 0x{:X}", pid, affinityMask);
    return true;
}

bool ThreadOptimizer::SetProcessPriority(DWORD pid, int priorityClass) {
    HANDLE hProcess = OpenProcess(PROCESS_SET_INFORMATION, FALSE, pid);
    if (!hProcess) {
        spdlog::error("Failed to open process {}: {}", pid, GetLastError());
        return false;
    }
    
    BOOL result = SetPriorityClass(hProcess, priorityClass);
    CloseHandle(hProcess);
    
    if (!result) {
        spdlog::error("Failed to set process priority: {}", GetLastError());
        return false;
    }
    
    spdlog::info("Set process {} priority to {}", pid, priorityClass);
    return true;
}

bool ThreadOptimizer::SetThreadAffinity(DWORD tid, DWORD_PTR affinityMask) {
    HANDLE hThread = OpenThread(THREAD_SET_INFORMATION, FALSE, tid);
    if (!hThread) {
        spdlog::error("Failed to open thread {}: {}", tid, GetLastError());
        return false;
    }
    
    DWORD_PTR result = SetThreadAffinityMask(hThread, affinityMask);
    CloseHandle(hThread);
    
    if (result == 0) {
        spdlog::error("Failed to set thread affinity: {}", GetLastError());
        return false;
    }
    
    spdlog::info("Set thread {} affinity to 0x{:X}", tid, affinityMask);
    return true;
}

bool ThreadOptimizer::SetThreadPriority(DWORD tid, int priority) {
    HANDLE hThread = OpenThread(THREAD_SET_INFORMATION, FALSE, tid);
    if (!hThread) {
        spdlog::error("Failed to open thread {}: {}", tid, GetLastError());
        return false;
    }
    
    BOOL result = ::SetThreadPriority(hThread, priority);
    CloseHandle(hThread);
    
    if (!result) {
        spdlog::error("Failed to set thread priority: {}", GetLastError());
        return false;
    }
    
    spdlog::info("Set thread {} priority to {}", tid, priority);
    return true;
}

std::vector<ProcessInfo> ThreadOptimizer::GetProcessList() {
    std::vector<ProcessInfo> processes;
    
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return processes;
    }
    
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);
    
    if (Process32First(hSnapshot, &pe32)) {
        do {
            ProcessInfo info;
            info.pid = pe32.th32ProcessID;
            info.name = std::string(pe32.szExeFile, pe32.szExeFile + strlen(pe32.szExeFile));
            info.affinityMask = 0;
            info.priority = 0;
            
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pe32.th32ProcessID);
            if (hProcess) {
                DWORD_PTR processAffinity, systemAffinity;
                if (GetProcessAffinityMask(hProcess, &processAffinity, &systemAffinity)) {
                    info.affinityMask = processAffinity;
                }
                info.priority = GetPriorityClass(hProcess);
                CloseHandle(hProcess);
            }
            
            processes.push_back(info);
            
        } while (Process32Next(hSnapshot, &pe32));
    }
    
    CloseHandle(hSnapshot);
    return processes;
}

std::vector<ThreadInfo> ThreadOptimizer::GetThreadsForProcess(DWORD pid) {
    std::vector<ThreadInfo> threads;
    
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return threads;
    }
    
    THREADENTRY32 te32;
    te32.dwSize = sizeof(THREADENTRY32);
    
    if (Thread32First(hSnapshot, &te32)) {
        do {
            if (te32.th32OwnerProcessID == pid) {
                ThreadInfo info;
                info.tid = te32.th32ThreadID;
                info.name = "Thread " + std::to_string(te32.th32ThreadID);
                info.affinityMask = 0;
                info.priority = 0;
                
                HANDLE hThread = OpenThread(THREAD_QUERY_INFORMATION, FALSE, te32.th32ThreadID);
                if (hThread) {
                    info.priority = ::GetThreadPriority(hThread);
                    CloseHandle(hThread);
                }
                
                threads.push_back(info);
            }
        } while (Thread32Next(hSnapshot, &te32));
    }
    
    CloseHandle(hSnapshot);
    return threads;
}

DWORD_PTR ThreadOptimizer::GetSystemAffinityMask() {
    DWORD_PTR processAffinity, systemAffinity;
    GetProcessAffinityMask(GetCurrentProcess(), &processAffinity, &systemAffinity);
    return systemAffinity;
}

int ThreadOptimizer::GetCoreCount() {
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    return sysInfo.dwNumberOfProcessors;
}

}
