#include "profile_manager.h"
#include "timer_optimizer.h"
#include "power_optimizer.h"
#include "network_optimizer.h"
#include "quantum_tweaker.h"
#include <spdlog/spdlog.h>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

#undef min
#undef max

namespace Optimizer {

ProfileManager& ProfileManager::Get() {
    static ProfileManager instance;
    return instance;
}

bool ProfileManager::ApplyProfile(ProfileType type) {
    m_currentProfile = type;
    
    switch (type) {
        case ProfileType::Gaming:
            ApplyGamingProfile();
            break;
        case ProfileType::Streaming:
            ApplyStreamingProfile();
            break;
        case ProfileType::Workstation:
            ApplyWorkstationProfile();
            break;
        case ProfileType::Balanced:
            ApplyBalancedProfile();
            break;
    }
    
    return true;
}

void ProfileManager::ApplyGamingProfile() {
    spdlog::info("Applying Gaming Profile");
    
    TimerOptimizer::Get().SetTimerResolution(0.5);
    
    PowerOptimizer::Get().SetCoreParking(100, 100);
    PowerOptimizer::Get().DisableThrottling();
    
    NetworkOptimizer::Get().OptimizeForGaming();
    
    QuantumTweaker::Get().OptimizeForForeground(true);
    QuantumTweaker::Get().SetLongQuantum(false);
    QuantumTweaker::Get().SetVariableQuantum(true);
    
    spdlog::info("Gaming profile applied successfully");
}

void ProfileManager::ApplyStreamingProfile() {
    spdlog::info("Applying Streaming Profile");
    
    TimerOptimizer::Get().SetTimerResolution(1.0);
    
    PowerOptimizer::Get().SetCoreParking(75, 100);
    
    NetworkOptimizer::Get().OptimizeForStreaming();
    
    QuantumTweaker::Get().OptimizeForForeground(true);
    QuantumTweaker::Get().SetLongQuantum(true);
    
    spdlog::info("Streaming profile applied successfully");
}

void ProfileManager::ApplyWorkstationProfile() {
    spdlog::info("Applying Workstation Profile");
    
    TimerOptimizer::Get().SetTimerResolution(1.0);
    
    PowerOptimizer::Get().SetCoreParking(50, 100);
    
    NetworkOptimizer::Get().OptimizeTcpParameters();
    
    QuantumTweaker::Get().SetLongQuantum(true);
    QuantumTweaker::Get().SetVariableQuantum(false);
    
    spdlog::info("Workstation profile applied successfully");
}

void ProfileManager::ApplyBalancedProfile() {
    spdlog::info("Applying Balanced Profile");
    
    TimerOptimizer::Get().ResetToDefault();
    
    PowerOptimizer::Get().SetCoreParking(50, 100);
    
    NetworkOptimizer::Get().ResetToDefault();
    
    spdlog::info("Balanced profile applied successfully");
}

Profile ProfileManager::GetDefaultProfile(ProfileType type) {
    Profile profile;
    profile.type = type;
    
    switch (type) {
        case ProfileType::Gaming:
            profile.name = "Gaming";
            profile.timerResolution = 0.5;
            profile.disableCoreParking = true;
            profile.processPriority = HIGH_PRIORITY_CLASS;
            break;
        case ProfileType::Streaming:
            profile.name = "Streaming";
            profile.timerResolution = 1.0;
            profile.disableCoreParking = false;
            profile.processPriority = ABOVE_NORMAL_PRIORITY_CLASS;
            break;
        case ProfileType::Workstation:
            profile.name = "Workstation";
            profile.timerResolution = 1.0;
            profile.disableCoreParking = false;
            profile.processPriority = NORMAL_PRIORITY_CLASS;
            break;
        case ProfileType::Balanced:
            profile.name = "Balanced";
            profile.timerResolution = 15.6;
            profile.disableCoreParking = false;
            profile.processPriority = NORMAL_PRIORITY_CLASS;
            break;
    }
    
    return profile;
}

bool ProfileManager::SaveProfile(const std::string& name, const Profile& profile) {
    json j;
    j["name"] = profile.name;
    j["type"] = static_cast<int>(profile.type);
    j["timerResolution"] = profile.timerResolution;
    j["disableCoreParking"] = profile.disableCoreParking;
    j["processPriority"] = profile.processPriority;
    j["processAffinity"] = profile.processAffinity;
    
    std::string filename = "profiles/" + name + ".json";
    std::ofstream file(filename);
    if (!file.is_open()) {
        spdlog::error("Failed to save profile: {}", filename);
        return false;
    }
    
    file << j.dump(4);
    file.close();
    
    spdlog::info("Profile saved: {}", filename);
    return true;
}

bool ProfileManager::LoadProfile(const std::string& name, Profile& profile) {
    std::string filename = "profiles/" + name + ".json";
    std::ifstream file(filename);
    if (!file.is_open()) {
        spdlog::error("Failed to load profile: {}", filename);
        return false;
    }
    
    json j;
    file >> j;
    file.close();
    
    profile.name = j["name"];
    profile.type = static_cast<ProfileType>(j["type"].get<int>());
    profile.timerResolution = j["timerResolution"];
    profile.disableCoreParking = j["disableCoreParking"];
    profile.processPriority = j["processPriority"];
    profile.processAffinity = j["processAffinity"];
    
    spdlog::info("Profile loaded: {}", filename);
    return true;
}

bool ProfileManager::ApplyCustomProfile(const std::string& name) {
    Profile profile;
    if (LoadProfile(name, profile)) {
        ApplyProfile(profile.type);
        return true;
    }
    return false;
}

std::vector<std::string> ProfileManager::GetSavedProfiles() {
    return {"Gaming", "Streaming", "Workstation", "Balanced"};
}

}
