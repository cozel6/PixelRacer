#pragma once
#pragma once
#include <string>
#include <vector>

enum class SectionType {
    Straight,
    LeftCurve,
    RightCurve,
    LeftHairpin,
    RightHairpin,
    Hill,
    Valley,
    Chicane
};

struct TrackSection {
    SectionType type;
    int length;
    float intensity;
    std::string name;

    TrackSection(SectionType t, int len, float intens = 1.0f, const std::string& n = "")
        : type(t), length(len), intensity(intens), name(n) {
    }
};

// ✨ Dificultate circuit (pentru Campaign)
enum class TrackDifficulty {
    Easy,
    Medium,
    Hard,
    Expert
};

struct TrackDefinition {
    std::string id;                     // Identificator unic (ex: "monaco_gp")
    std::string name;
    std::string country;
    float lengthKm;
    int recommendedLaps;
    std::string description;
    TrackDifficulty difficulty;
    int year;

    std::vector<TrackSection> sections;

    // ✨ Metadata pentru diferite moduri
    struct Records {
        float bestLapTime;      // Pentru Time Trial
        float longestDistance;  // Pentru Endless
        bool completed;         // Pentru Campaign
    } records;

    TrackDefinition(const std::string& trackId, const std::string& n,
        const std::string& c, float len, int laps, TrackDifficulty diff)
        : id(trackId), name(n), country(c), lengthKm(len),
        recommendedLaps(laps), difficulty(diff), year(0) {
        records.bestLapTime = 0.0f;
        records.longestDistance = 0.0f;
        records.completed = false;
    }
};

class TrackLibrary {
public:
    // Toate circuitele
    static TrackDefinition createTestTrack();
    static TrackDefinition createMonacoGP();
    static TrackDefinition createSpaFrancorchamps();
    static TrackDefinition createMonzaGP();
    static TrackDefinition createSilverstoneGP();
    static TrackDefinition createNurburgringGP();

    // Utilitare
    static std::vector<TrackDefinition> getAllTracks();
    static std::vector<TrackDefinition> getTracksByDifficulty(TrackDifficulty diff);
    static TrackDefinition* getTrackById(const std::string& id);
};