#pragma once
#include "Player.h"
#include "Road.h"
#include "GameModeConfig.h"
#include "TrackDefinition.h"

class GameplayManager {
public:
    GameplayManager(GameMode mode, const TrackDefinition* track = nullptr);
    
    void update(float deltaTime);
    void render(sf::RenderWindow& window);
    
    float getPlayerSpeed() const;
    float getPlayerSpeedKmh() const;
    float getDistance() const;
    float getDistanceKm() const;
    
    const EndlessStats& getStats() const;
    int getCurrentLevel() const;
    
    GameMode getGameMode() const;
    const std::string& getTrackName() const { return m_trackName; }

private:
    void initializeForMode();
    void loadTrack(const TrackDefinition& track);
    void handleTrackLooping();  // ✅ NEW: Extracted method
    
    // Endless mode specific
    void updateEndlessScoring(float deltaTime);
    void updateDifficultyProgression();
    void checkCheckpoints();
    void evaluateCornerPerformance();
    
    WheelSurfaces getWheelSurfaces() const;
    SurfaceType getSurfaceTypeAt(float x, float z) const;
    
    GameMode m_mode;
    Player m_player;
    Road m_road;
    std::string m_trackName;
    
    // Endless mode state
    EndlessStats m_stats;
    EndlessDifficulty m_difficulty;
    
    float m_lastCheckpointZ = 0.0f;
    float m_checkpointInterval = 5000.0f;
    
    bool m_inCorner = false;
    float m_cornerEntrySpeed = 0.0f;
    
    int m_lapCount = 0;  // ✅ NEW: Track lap count for seamless looping
};