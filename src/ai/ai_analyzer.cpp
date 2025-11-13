#include "ai_analyzer.h"
#include "../monitoring/monitoring_engine.h"
#include "../optimizers/thread_optimizer.h"
#include <spdlog/spdlog.h>
#include <algorithm>

namespace Optimizer {

AIAnalyzer& AIAnalyzer::Get() {
    static AIAnalyzer instance;
    return instance;
}

bool AIAnalyzer::IsGamingProcess(const std::string& processName) {
    std::string lowerName = processName;
    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
    
    const char* gamingKeywords[] = {
        "game", "steam", "epic", "origin", "uplay", "battlenet", "gog",
        "valorant", "league", "dota", "csgo", "fortnite", "apex", "warzone",
        "minecraft", "roblox", "amongus", "fallguys", "destiny", "overwatch",
        "rainbow6", "siege", "pubg", "tarkov", "hunt", "division"
    };
    
    for (const char* keyword : gamingKeywords) {
        if (lowerName.find(keyword) != std::string::npos) {
            return true;
        }
    }
    
    return false;
}

bool AIAnalyzer::IsStreamingProcess(const std::string& processName) {
    std::string lowerName = processName;
    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
    
    const char* streamingKeywords[] = {
        "obs", "streamlabs", "xsplit", "discord", "twitch", "youtube",
        "nvidia broadcast", "nvbroadcast", "amdlink", "encoder"
    };
    
    for (const char* keyword : streamingKeywords) {
        if (lowerName.find(keyword) != std::string::npos) {
            return true;
        }
    }
    
    return false;
}

void AIAnalyzer::AnalyzeProcesses(SystemAnalysisResult& result) {
    auto processes = ThreadOptimizer::Get().GetProcessList();
    
    result.processCount = processes.size();
    result.hasGamingProcess = false;
    result.hasStreamingProcess = false;
    
    for (const auto& proc : processes) {
        if (IsGamingProcess(proc.name)) {
            result.hasGamingProcess = true;
            spdlog::debug("Detected gaming process: {}", proc.name);
        }
        
        if (IsStreamingProcess(proc.name)) {
            result.hasStreamingProcess = true;
            spdlog::debug("Detected streaming process: {}", proc.name);
        }
    }
}

void AIAnalyzer::AnalyzeResources(SystemAnalysisResult& result) {
    auto cpuInfo = Monitor::MonitoringEngine::Get().GetCPUInfo();
    auto ramInfo = Monitor::MonitoringEngine::Get().GetRAMInfo();
    
    if (!cpuInfo.empty()) {
        float totalUsage = 0.0f;
        for (const auto& core : cpuInfo) {
            totalUsage += core.usage;
        }
        result.cpuUsage = totalUsage / cpuInfo.size();
    } else {
        result.cpuUsage = 0.0f;
    }
    
    result.ramUsagePercent = ramInfo.usagePercent;
}

void AIAnalyzer::GenerateRecommendationsFromAnalysis(SystemAnalysisResult& result) {
    if (result.hasGamingProcess) {
        Recommendation rec;
        rec.type = RecommendationType::GamingOptimization;
        rec.title = "Gaming Process Detected";
        rec.description = "Apply Gaming profile for optimal performance: 0.5ms timer, disabled core parking, network optimizations";
        rec.priority = 10;
        rec.canAutoApply = true;
        result.recommendations.push_back(rec);
    }
    
    if (result.hasStreamingProcess) {
        Recommendation rec;
        rec.type = RecommendationType::StreamingOptimization;
        rec.title = "Streaming Software Detected";
        rec.description = "Apply Streaming profile: balanced CPU usage, optimized network buffers, long thread quantum";
        rec.priority = 9;
        rec.canAutoApply = true;
        result.recommendations.push_back(rec);
    }
    
    if (result.ramUsagePercent > 80.0f) {
        Recommendation rec;
        rec.type = RecommendationType::MemoryOptimization;
        rec.title = "High Memory Usage";
        rec.description = "RAM usage at " + std::to_string((int)result.ramUsagePercent) + "%. Consider clearing standby list or optimizing memory priority.";
        rec.priority = 7;
        rec.canAutoApply = false;
        result.recommendations.push_back(rec);
    }
    
    if (result.cpuUsage > 70.0f) {
        Recommendation rec;
        rec.type = RecommendationType::PowerOptimization;
        rec.title = "High CPU Usage";
        rec.description = "CPU usage at " + std::to_string((int)result.cpuUsage) + "%. Ensure core parking is disabled and high performance power plan is active.";
        rec.priority = 6;
        rec.canAutoApply = true;
        result.recommendations.push_back(rec);
    }
    
    if (!result.hasGamingProcess && !result.hasStreamingProcess && result.cpuUsage < 30.0f) {
        Recommendation rec;
        rec.type = RecommendationType::WorkstationOptimization;
        rec.title = "Low System Load";
        rec.description = "System is underutilized. Consider Balanced or Workstation profile for better power efficiency.";
        rec.priority = 3;
        rec.canAutoApply = false;
        result.recommendations.push_back(rec);
    }
}

SystemAnalysisResult AIAnalyzer::AnalyzeSystem() {
    SystemAnalysisResult result = {};
    
    spdlog::info("Starting AI system analysis...");
    
    AnalyzeProcesses(result);
    AnalyzeResources(result);
    GenerateRecommendationsFromAnalysis(result);
    
    std::sort(result.recommendations.begin(), result.recommendations.end(),
             [](const Recommendation& a, const Recommendation& b) {
                 return a.priority > b.priority;
             });
    
    spdlog::info("AI analysis complete: {} recommendations", result.recommendations.size());
    
    return result;
}

std::vector<Recommendation> AIAnalyzer::GenerateRecommendations() {
    auto result = AnalyzeSystem();
    return result.recommendations;
}

}
