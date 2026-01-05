        #pragma once
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <array>
#include <functional>

// ═══════════════════════════════════════════════════════════════
// GAME MODES
// ═══════════════════════════════════════════════════════════════
enum class GameMode {
    Endless,
    Campaign,
    TimeTrial
};

// ═══════════════════════════════════════════════════════════════
// ENDLESS DIFFICULTY LEVELS
// ═══════════════════════════════════════════════════════════════
enum class EndlessDifficultyLevel {
    Easy,
    Medium,
    Hard
};

struct EndlessDifficultySettings {
    float potholeChance;
    float repairPickupChance;
    float bonusMultiplier;
    std::string name;
    
    static EndlessDifficultySettings getSettings(EndlessDifficultyLevel level) {
        switch (level) {
            case EndlessDifficultyLevel::Easy:
                return { 0.02f, 0.08f, 1.0f, "EASY" };
            case EndlessDifficultyLevel::Medium:
                return { 0.03f, 0.05f, 1.5f, "MEDIUM" };
            case EndlessDifficultyLevel::Hard:
                return { 0.06f, 0.02f, 2.0f, "HARD" };
            default:
                return { 0.03f, 0.05f, 1.5f, "MEDIUM" };
        }
    }
};

// ═══════════════════════════════════════════════════════════════
// ENDLESS MODE STATS
// ═══════════════════════════════════════════════════════════════
struct EndlessStats {
    float totalDistance = 0.0f;
    float currentScore = 0.0f;
    float topSpeed = 0.0f;
    float averageSpeed = 0.0f;
    float timeElapsed = 0.0f;
    int checkpointsReached = 0;
    int perfectCornersCount = 0;
    int lapsWithoutDamage = 0;
    int repairPickupsCollected = 0;
    float highscoreKm = 0.0f;
    
    void reset() {
        totalDistance = 0.0f;
        currentScore = 0.0f;
        topSpeed = 0.0f;
        averageSpeed = 0.0f;
        timeElapsed = 0.0f;
        checkpointsReached = 0;
        perfectCornersCount = 0;
        lapsWithoutDamage = 0;
        repairPickupsCollected = 0;
        highscoreKm = 0.0f;
    }
};

struct EndlessDifficulty {
    int level = 1;
    float curveMultiplier = 1.0f;
    float checkpointBonus = 1000.0f;
    
    void reset() {
        level = 1;
        curveMultiplier = 1.0f;
        checkpointBonus = 1000.0f;
    }
};

// ═══════════════════════════════════════════════════════════════
// ENDLESS CONFIG CONSTANTS
// ═══════════════════════════════════════════════════════════════
namespace EndlessConfigValues {
    constexpr float NO_DAMAGE_LAP_BONUS = 500.0f;
    constexpr float NO_DAMAGE_LAP_MULTIPLIER = 1.25f;
    constexpr float REPAIR_AMOUNT = 25.0f;
    constexpr float REPAIR_PICKUP_WIDTH = 100.0f;
}

// ═══════════════════════════════════════════════════════════════
// CAMPAIGN MODE - TASK TYPES
// ═══════════════════════════════════════════════════════════════
enum class CampaignTaskType {
    FinishRace,         // Termină cursa
    BeatTime,           // Termină sub un anumit timp
    BeatLapTime,        // Cea mai bună tură sub un timp
    NoSpinouts,         // Termină fără să faci spin
    StayOnTrack,        // Termină fără să ieși de pe drum
    TopSpeed,           // Atinge o anumită viteză maximă
    AverageLapTime,     // Media turelor sub un timp
    PerfectLaps,        // Realizează N ture perfecte
    CompleteLaps,       // Completează N ture
    NoDamage,           // Termină fără damage
    CollectPickups      // Colectează N pickup-uri
};

// ═══════════════════════════════════════════════════════════════
// CAMPAIGN TASK STRUCTURE
// ═══════════════════════════════════════════════════════════════
struct CampaignTask {
    int id;
    CampaignTaskType type;
    float targetValue;
    std::string description;
    bool completed;
    
    CampaignTask()
        : id(0), type(CampaignTaskType::FinishRace), targetValue(0.0f), 
          description(""), completed(false) {}
    
    CampaignTask(int _id, CampaignTaskType t, float val, const std::string& desc)
        : id(_id), type(t), targetValue(val), description(desc), completed(false) {}
};

// ═══════════════════════════════════════════════════════════════
// TRACK DIFFICULTY FOR CAMPAIGN
// ═══════════════════════════════════════════════════════════════
enum class CampaignTrackDifficulty {
    Easy,       // Necesită 1 task completat
    Medium,     // Necesită 2 task-uri completate
    Hard        // Necesită 3 task-uri completate
};

// ═══════════════════════════════════════════════════════════════
// CAMPAIGN TRACK WITH TASKS
// ═══════════════════════════════════════════════════════════════
struct CampaignTrackData {
    int id;
    std::string trackId;
    std::string name;
    std::string description;
    CampaignTrackDifficulty difficulty;
    int requiredLaps;
    bool unlocked;
    std::array<CampaignTask, 3> tasks;
    int selectedTaskIndex;          // Task-ul selectat curent (-1 = none)
    float bestTime;
    
    CampaignTrackData()
        : id(0), trackId(""), name(""), description("")
        , difficulty(CampaignTrackDifficulty::Easy), requiredLaps(3)
        , unlocked(false), selectedTaskIndex(-1), bestTime(0.0f) {}
    
    // Returnează câte task-uri sunt necesare pentru a completa circuitul
    int getRequiredTaskCount() const {
        switch (difficulty) {
            case CampaignTrackDifficulty::Easy:   return 1;
            case CampaignTrackDifficulty::Medium: return 2;
            case CampaignTrackDifficulty::Hard:   return 3;
            default: return 1;
        }
    }
    
    // Returnează câte task-uri sunt completate
    int getCompletedTaskCount() const {
        int count = 0;
        for (const auto& task : tasks) {
            if (task.completed) count++;
        }
        return count;
    }
    
    // Verifică dacă circuitul este complet (toate task-urile necesare sunt făcute)
    bool isTrackCompleted() const {
        return getCompletedTaskCount() >= getRequiredTaskCount();
    }
    
    // Returnează task-ul selectat
    const CampaignTask* getSelectedTask() const {
        if (selectedTaskIndex >= 0 && selectedTaskIndex < 3) {
            return &tasks[selectedTaskIndex];
        }
        return nullptr;
    }
    
    CampaignTask* getSelectedTaskMutable() {
        if (selectedTaskIndex >= 0 && selectedTaskIndex < 3) {
            return &tasks[selectedTaskIndex];
        }
        return nullptr;
    }
};

// ═══════════════════════════════════════════════════════════════
// CAMPAIGN PROGRESS TRACKER
// ═══════════════════════════════════════════════════════════════
struct CampaignProgress {
    int currentLap = 0;
    int totalLaps = 3;
    float raceTime = 0.0f;
    float bestLapTime = 0.0f;
    float topSpeed = 0.0f;
    float averageSpeed = 0.0f;
    int spinoutCount = 0;
    int offTrackCount = 0;
    int perfectLapsCount = 0;
    bool currentLapPerfect = true;
    float totalDamage = 0.0f;
    int pickupsCollected = 0;
    
    void reset() {
        currentLap = 0;
        raceTime = 0.0f;
        bestLapTime = 0.0f;
        topSpeed = 0.0f;
        averageSpeed = 0.0f;
        spinoutCount = 0;
        offTrackCount = 0;
        perfectLapsCount = 0;
        currentLapPerfect = true;
        totalDamage = 0.0f;
        pickupsCollected = 0;
    }
    
    bool checkTask(const CampaignTask& task) const {
        switch (task.type) {
            case CampaignTaskType::FinishRace:
                return currentLap >= totalLaps;
                
            case CampaignTaskType::BeatTime:
                return currentLap >= totalLaps && raceTime <= task.targetValue;
                
            case CampaignTaskType::BeatLapTime:
                return bestLapTime > 0.0f && bestLapTime <= task.targetValue;
                
            case CampaignTaskType::NoSpinouts:
                return currentLap >= totalLaps && spinoutCount == 0;
                
            case CampaignTaskType::StayOnTrack:
                return currentLap >= totalLaps && offTrackCount == 0;
                
            case CampaignTaskType::TopSpeed:
                return topSpeed >= task.targetValue;
                
            case CampaignTaskType::AverageLapTime:
                return currentLap >= totalLaps && currentLap > 0 &&
                       (raceTime / static_cast<float>(currentLap)) <= task.targetValue;
                       
            case CampaignTaskType::PerfectLaps:
                return perfectLapsCount >= static_cast<int>(task.targetValue);
                
            case CampaignTaskType::CompleteLaps:
                return currentLap >= static_cast<int>(task.targetValue);
                
            case CampaignTaskType::NoDamage:
                return currentLap >= totalLaps && totalDamage == 0.0f;
                
            case CampaignTaskType::CollectPickups:
                return pickupsCollected >= static_cast<int>(task.targetValue);
                
            default:
                return false;
        }
    }
};

// ═══════════════════════════════════════════════════════════════
// CAMPAIGN DATA MANAGER
// ═══════════════════════════════════════════════════════════════
class CampaignDataManager {
public:
    static std::vector<CampaignTrackData> getTracks() {
        return {
            // ═══════════════════════════════════════════════════════════
            // EASY TRACKS (1 task necesar)
            // ═══════════════════════════════════════════════════════════
            createTrack(1, "countryside", "COUNTRYSIDE", 
                "A scenic route through rolling hills",
                CampaignTrackDifficulty::Easy, 3, true,
                {
                    CampaignTask(1, CampaignTaskType::FinishRace, 0.0f, "Complete the race"),
                    CampaignTask(2, CampaignTaskType::BeatTime, 180.0f, "Finish in under 3:00"),
                    CampaignTask(3, CampaignTaskType::TopSpeed, 200.0f, "Reach 200 km/h")
                }),
            
            createTrack(2, "coastal", "COASTAL HIGHWAY",
                "Fast coastal road with gentle curves",
                CampaignTrackDifficulty::Easy, 3, false,
                {
                    CampaignTask(1, CampaignTaskType::FinishRace, 0.0f, "Complete the race"),
                    CampaignTask(2, CampaignTaskType::BeatTime, 165.0f, "Finish in under 2:45"),
                    CampaignTask(3, CampaignTaskType::NoSpinouts, 0.0f, "Finish without spinning")
                }),
            
            // ═══════════════════════════════════════════════════════════
            // MEDIUM TRACKS (2 task-uri necesare)
            // ═══════════════════════════════════════════════════════════
            createTrack(3, "mountain", "MOUNTAIN PASS",
                "Challenging mountain road with steep hills",
                CampaignTrackDifficulty::Medium, 3, false,
                {
                    CampaignTask(1, CampaignTaskType::BeatTime, 150.0f, "Finish in under 2:30"),
                    CampaignTask(2, CampaignTaskType::TopSpeed, 260.0f, "Reach 260 km/h"),
                    CampaignTask(3, CampaignTaskType::BeatLapTime, 45.0f, "Set a lap under 0:45")
                }),
            
            createTrack(4, "desert", "DESERT DUNES",
                "Hot desert road with tricky sand patches",
                CampaignTrackDifficulty::Medium, 3, false,
                {
                    CampaignTask(1, CampaignTaskType::NoSpinouts, 0.0f, "Finish without spinning"),
                    CampaignTask(2, CampaignTaskType::StayOnTrack, 0.0f, "Stay on track entire race"),
                    CampaignTask(3, CampaignTaskType::BeatTime, 140.0f, "Finish in under 2:20")
                }),
            
            createTrack(5, "city", "CITY STREETS",
                "Tight city circuit with sharp corners",
                CampaignTrackDifficulty::Medium, 4, false,
                {
                    CampaignTask(1, CampaignTaskType::StayOnTrack, 0.0f, "Stay on track entire race"),
                    CampaignTask(2, CampaignTaskType::PerfectLaps, 2.0f, "Complete 2 perfect laps"),
                    CampaignTask(3, CampaignTaskType::BeatLapTime, 40.0f, "Set a lap under 0:40")
                }),
            
            // ═══════════════════════════════════════════════════════════
            // HARD TRACKS (3 task-uri necesare)
            // ═══════════════════════════════════════════════════════════
            createTrack(6, "forest", "FOREST TRAIL",
                "Winding forest road through the trees",
                CampaignTrackDifficulty::Hard, 5, false,
                {
                    CampaignTask(1, CampaignTaskType::BeatTime, 200.0f, "Finish in under 3:20"),
                    CampaignTask(2, CampaignTaskType::NoDamage, 0.0f, "Finish without taking damage"),
                    CampaignTask(3, CampaignTaskType::PerfectLaps, 3.0f, "Complete 3 perfect laps")
                }),
            
            createTrack(7, "night_city", "NIGHT CIRCUIT",
                "City at night with neon lights",
                CampaignTrackDifficulty::Hard, 5, false,
                {
                    CampaignTask(1, CampaignTaskType::BeatLapTime, 35.0f, "Set a lap under 0:35"),
                    CampaignTask(2, CampaignTaskType::TopSpeed, 300.0f, "Reach 300 km/h"),
                    CampaignTask(3, CampaignTaskType::NoSpinouts, 0.0f, "Finish without spinning")
                }),
            
            createTrack(8, "championship", "CHAMPIONSHIP",
                "The ultimate test of skill",
                CampaignTrackDifficulty::Hard, 5, false,
                {
                    CampaignTask(1, CampaignTaskType::BeatTime, 180.0f, "Finish in under 3:00"),
                    CampaignTask(2, CampaignTaskType::PerfectLaps, 4.0f, "Complete 4 perfect laps"),
                    CampaignTask(3, CampaignTaskType::NoDamage, 0.0f, "Finish without taking damage")
                })
        };
    }
    
    static std::string getTaskProgress(const CampaignTask& task, const CampaignProgress& progress) {
        std::ostringstream oss;
        
        switch (task.type) {
            case CampaignTaskType::FinishRace:
                oss << "Lap " << progress.currentLap << "/" << progress.totalLaps;
                break;
                
            case CampaignTaskType::BeatTime:
                oss << formatTime(progress.raceTime) << " / " << formatTime(task.targetValue);
                break;
                
            case CampaignTaskType::BeatLapTime:
                if (progress.bestLapTime > 0.0f) {
                    oss << "Best: " << formatTime(progress.bestLapTime);
                } else {
                    oss << "Target: " << formatTime(task.targetValue);
                }
                break;
                
            case CampaignTaskType::NoSpinouts:
                oss << "Spinouts: " << progress.spinoutCount;
                break;
                
            case CampaignTaskType::StayOnTrack:
                oss << "Off-track: " << progress.offTrackCount;
                break;
                
            case CampaignTaskType::TopSpeed:
                oss << static_cast<int>(progress.topSpeed) << " / " 
                    << static_cast<int>(task.targetValue) << " km/h";
                break;
                
            case CampaignTaskType::PerfectLaps:
                oss << "Perfect: " << progress.perfectLapsCount << "/" 
                    << static_cast<int>(task.targetValue);
                break;
                
            case CampaignTaskType::CompleteLaps:
                oss << "Laps: " << progress.currentLap << "/" 
                    << static_cast<int>(task.targetValue);
                break;
                
            case CampaignTaskType::NoDamage:
                oss << "Damage: " << static_cast<int>(progress.totalDamage) << "%";
                break;
                
            case CampaignTaskType::CollectPickups:
                oss << "Pickups: " << progress.pickupsCollected << "/" 
                    << static_cast<int>(task.targetValue);
                break;
                
            default:
                oss << "--";
        }
        
        return oss.str();
    }
    
    static std::string getDifficultyString(CampaignTrackDifficulty diff) {
        switch (diff) {
            case CampaignTrackDifficulty::Easy:   return "EASY";
            case CampaignTrackDifficulty::Medium: return "MEDIUM";
            case CampaignTrackDifficulty::Hard:   return "HARD";
            default: return "UNKNOWN";
        }
    }
    
private:
    static CampaignTrackData createTrack(int id, const std::string& trackId,
        const std::string& name, const std::string& desc,
        CampaignTrackDifficulty diff, int laps, bool unlocked,
        std::array<CampaignTask, 3> tasks) 
    {
        CampaignTrackData track;
        track.id = id;
        track.trackId = trackId;
        track.name = name;
        track.description = desc;
        track.difficulty = diff;
        track.requiredLaps = laps;
        track.unlocked = unlocked;
        track.tasks = tasks;
        track.selectedTaskIndex = -1;
        track.bestTime = 0.0f;
        return track;
    }
    
    static std::string formatTime(float seconds) {
        int mins = static_cast<int>(seconds) / 60;
        int secs = static_cast<int>(seconds) % 60;
        std::ostringstream oss;
        oss << mins << ":" << std::setfill('0') << std::setw(2) << secs;
        return oss.str();
    }
};

// ═══════════════════════════════════════════════════════════════
// BACKWARD COMPATIBILITY - DEPRECATED TYPES
// ═══════════════════════════════════════════════════════════════
// Keeping old types for backward compatibility
using CampaignObjectiveType = CampaignTaskType;
using CampaignObjective = CampaignTask;
using CampaignTrack = CampaignTrackData;
using CampaignData = CampaignDataManager;

// ═══════════════════════════════════════════════════════════════
// TIME TRIAL MODE
// ═══════════════════════════════════════════════════════════════
struct TimeTrialConfig {
    int ghostLaps = 3;
    bool showIdealLine = false;
    bool showSectorTimes = true;
    int sectorsPerLap = 3;
};

struct TimeTrialSettings {
    float initialTime = 60.0f;
    float checkpointBonus = 15.0f;
    float currentTime = 60.0f;
    bool isGameOver = false;
};