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
    float trafficDensity = 0.1f;        // Traffic density (0.0 - 1.0)
    float minTrafficSpeed = 0.4f;       // Min traffic speed multiplier
    float maxTrafficSpeed = 0.6f;       // Max traffic speed multiplier
    float hazardChance = 0.0f;          // Chance of hazards appearing (0.0 - 1.0)
    
    // Reset to default
    void reset() {
        level = 1;
        trafficDensity = 0.1f;
        minTrafficSpeed = 0.4f;
        maxTrafficSpeed = 0.6f;
        hazardChance = 0.0f;
    }
};