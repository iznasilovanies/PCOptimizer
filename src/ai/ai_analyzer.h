#pragma once
#include <string>
#include <vector>
#include <Windows.h>

namespace Optimizer {

enum class RecommendationType {
    GamingOptimization,
    StreamingOptimization,
    WorkstationOptimization,
    MemoryOptimization,
    NetworkOptimization,
    PowerOptimization
};

struct Recommendation {
    RecommendationType type;
    std::string title;
    std::string description;
    int priority;
    bool canAutoApply;
};

struct SystemAnalysisResult {
    float cpuUsage;
    float ramUsagePercent;
    int processCount;
    bool hasGamingProcess;
    bool hasStreamingProcess;
    bool hasHighNetworkUsage;
    std::vector<Recommendation> recommendations;
};

class AIAnalyzer {
public:
    static AIAnalyzer& Get();
    
    SystemAnalysisResult AnalyzeSystem();
    std::vector<Recommendation> GenerateRecommendations();
    
    bool IsGamingProcess(const std::string& processName);
    bool IsStreamingProcess(const std::string& processName);
    
private:
    AIAnalyzer() = default;
    
    void AnalyzeProcesses(SystemAnalysisResult& result);
    void AnalyzeResources(SystemAnalysisResult& result);
    void GenerateRecommendationsFromAnalysis(SystemAnalysisResult& result);
};

}
