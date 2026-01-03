#pragma once
#include <string>

// ✅ ENUM PRINCIPAL - Definește tipurile de joc
enum class GameMode {
    Endless,        // Mod infinit cu dificultate progresivă
    Campaign,       // Campanie cu niveluri
    TimeTrial       // Contra-cronometru
};

// ✨ Configurare pentru Campaign Mode
struct CampaignObjective {
    enum class Type {
        FinishRace,         // Termină cursa
        BeatTime,           // Sub un timp specific
        NoSpinouts,         // Fără pierderi de control
        StayOnTrack,        // Nu ieși de pe pistă
        PerfectLap          // Un tur perfect
    };

    Type type;
    float targetValue;      // Ex: timp țintă în secunde
    std::string description;
    int rewardPoints;

    CampaignObjective(Type t, float val, const std::string& desc, int points)
        : type(t), targetValue(val), description(desc), rewardPoints(points) {
    }
};

struct CampaignLevel {
    std::string trackId;
    int levelNumber;
    std::string title;
    CampaignObjective objective;
    bool unlocked;
    bool completed;
    int starsEarned;        // 0-3 stele

    CampaignLevel(const std::string& track, int level, const std::string& t,
        const CampaignObjective& obj)
        : trackId(track), levelNumber(level), title(t), objective(obj),
        unlocked(level == 1), completed(false), starsEarned(0) {
    }
};

// ✨ Configurare pentru Time Trial Mode
struct TimeTrialConfig {
    int ghostLaps;          // Număr de tururi pentru ghost
    bool showIdealLine;     // Arată linia ideală
    bool showSectorTimes;   // Arată timpii pe sectoare
    int sectorsPerLap;      // Câte sectoare per tur (ex: 3)

    TimeTrialConfig()
        : ghostLaps(3), showIdealLine(false), showSectorTimes(true), sectorsPerLap(3) {
    }
};

// ✨ Configurare pentru Endless Mode
struct EndlessConfig {
    float difficultyIncrease;   // Cu cât crește dificultatea (ex: 0.05 = 5% per minut)
    bool allowTrackChange;      // Permite schimbarea pistei
    int checkpointInterval;     // La câte secunde checkpoint-uri

    EndlessConfig()
        : difficultyIncrease(0.05f), allowTrackChange(true), checkpointInterval(30) {
    }
};

// ✅ NEW: Endless Mode Statistics
struct EndlessStats {
    float timeElapsed = 0.0f;           // Total time played
    float totalDistance = 0.0f;         // Total distance traveled in meters
    float currentScore = 0.0f;          // Current score
    float topSpeed = 0.0f;              // Highest speed achieved (km/h)
    float averageSpeed = 0.0f;          // Average speed (km/h)
    int checkpointsReached = 0;         // Number of checkpoints passed
    int perfectCornersCount = 0;        // Number of perfect corners
    
    // Reset all stats
    void reset() {
        timeElapsed = 0.0f;
        totalDistance = 0.0f;
        currentScore = 0.0f;
        topSpeed = 0.0f;
        averageSpeed = 0.0f;
        checkpointsReached = 0;
        perfectCornersCount = 0;
    }
};

// ✅ NEW: Endless Mode Difficulty Progression
struct EndlessDifficulty {
    int level = 1;                      // Current difficulty level (1-10)
    float curveMultiplier = 1.0f;       // Ajustare fine pentru dificultate
    float checkpointBonus = 1000.0f;          // Bonus de timp per checkpoint
    
    // Reset to default
    void reset() {
        level = 1;
        curveMultiplier = 1.0f;
        checkpointBonus = 1000.0f;
    }
};

// ✅ NEW: Time Trial Settings
struct TimeTrialSettings {
    float initialTime = 60.0f;      // Starting time in seconds
    float checkpointBonus = 15.0f;  // Time added per checkpoint
    float currentTime = 60.0f;      // Current remaining time
    bool isGameOver = false;
};

// ✅ NEW: Campaign Settings
struct CampaignSettings {
    int currentTrack = 0;
    int totalTracks = 5;
    int currentPosition = 8;  // Out of 8 racers
    int lapsRequired = 3;
    int currentLap = 0;
};