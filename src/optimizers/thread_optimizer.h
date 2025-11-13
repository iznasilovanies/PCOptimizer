#pragma once
#include <Windows.h>
#include <string>
#include <vector>

namespace Optimizer {

struct ProcessInfo {
    DWORD pid;
    std::string name;
    DWORD_PTR affinityMask;
    int priority;
};

struct ThreadInfo {
    DWORD tid;
    std::string name;
    DWORD_PTR affinityMask;
    int priority;
};

class ThreadOptimizer {
public:
    static ThreadOptimizer& Get();
    
    bool SetProcessAffinity(DWORD pid, DWORD_PTR affinityMask);
    bool SetProcessPriority(DWORD pid, int priorityClass);
    
    bool SetThreadAffinity(DWORD tid, DWORD_PTR affinityMask);
    bool SetThreadPriority(DWORD tid, int priority);
    
    std::vector<ProcessInfo> GetProcessList();
    std::vector<ThreadInfo> GetThreadsForProcess(DWORD pid);
    
    DWORD_PTR GetSystemAffinityMask();
    int GetCoreCount();
    
private:
    ThreadOptimizer() = default;
};

}
