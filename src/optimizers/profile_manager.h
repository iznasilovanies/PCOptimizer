#pragma once
#include <string>
#include <map>
#include <vector>
#include <Windows.h>

namespace Optimizer {

enum class ProfileType {
    Gaming,
    Streaming,
    Workstation,
    Balanced
};

struct Profile {
    std::string name;
    ProfileType type;
    
    double timerResolution;
    bool disableCoreParking;
    int processPriority;
    DWORD_PTR processAffinity;
    
    bool operator==(const Profile& other) const {
        return name == other.name && type == other.type;
    }
};

class ProfileManager {
public:
    static ProfileManager& Get();
    
    bool ApplyProfile(ProfileType type);
    bool ApplyCustomProfile(const std::string& name);
    
    Profile GetDefaultProfile(ProfileType type);
    bool SaveProfile(const std::string& name, const Profile& profile);
    bool LoadProfile(const std::string& name, Profile& profile);
    
    std::vector<std::string> GetSavedProfiles();
    
    ProfileType GetCurrentProfileType() const { return m_currentProfile; }
    
private:
    ProfileManager() = default;
    
    void ApplyGamingProfile();
    void ApplyStreamingProfile();
    void ApplyWorkstationProfile();
    void ApplyBalancedProfile();
    
    ProfileType m_currentProfile = ProfileType::Balanced;
};

}
