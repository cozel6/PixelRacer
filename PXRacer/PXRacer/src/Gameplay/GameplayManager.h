#pragma once
#include "Player.h"
#include "Road.h"
#include "GameModeConfig.h"
#include "TrackDefinition.h"

enum class CountdownPhase {
    Ready,
    Light1,
    Light2,
    Light3,
    Light4,
    Light5,
    Go,
    Finished
};

struct StartBoostResult {
    bool attempted;
    bool perfect;
    bool good;
    bool jumpStart;
    float boostMultiplier;
};

class GameplayManager {
public:
    GameplayManager(GameMode mode, const TrackDefinition* track = nullptr);
    
    void update(float deltaTime);
    void render(sf::RenderWindow& window);
    
    // COUNTDOWN SYSTEM
    void handleStartInput(bool clutchPressed);
    bool isRaceStarted() const { return m_raceStarted; }
    bool isCountingDown() const { return !m_raceStarted && m_countdownPhase != CountdownPhase::Finished; }
    CountdownPhase getCountdownPhase() const { return m_countdownPhase; }
    float getCountdownTimer() const { return m_countdownTimer; }
    const StartBoostResult& getBoostResult() const { return m_boostResult; }
    bool hasBoostWindowPassed() const { return m_boostWindowPassed; }
    
    // ✅ NEW: Game over
    bool isGameOver() const { return m_player.isDestroyed(); }
    
    // Getters
    float getPlayerSpeed() const;
    float getPlayerSpeedKmh() const;
    float getDistance() const;
    float getDistanceKm() const;
    const EndlessStats& getStats() const;
    int getCurrentLevel() const;
    GameMode getGameMode() const;
    const std::string& getTrackName() const { return m_trackName; }
    int getLapCount() const { return m_lapCount; }
    float getCurrentLapTime() const { return m_currentLapTime; }
    float getBestLapTime() const { return m_bestLapTime; }
    float getLastLapTime() const { return m_lastLapTime; }
    
    const Player& getPlayer() const { return m_player; }

private:
    void initializeForMode();
    void loadTrack(const TrackDefinition& track);
    void handleTrackLooping();
    
    void updateCountdown(float deltaTime);
    void applyStartBoost();
    
    void updateEndlessScoring(float deltaTime);
    void updateDifficultyProgression();
    void checkCheckpoints();
    void evaluateCornerPerformance();
    void checkPotholeCollisions();
    
    WheelSurfaces getWheelSurfaces() const;
    SurfaceType getSurfaceTypeAt(float x, float z) const;
    
    GameMode m_mode;
    Player m_player;
    Road m_road;
    std::string m_trackName;
    
    // COUNTDOWN STATE
    bool m_raceStarted;
    CountdownPhase m_countdownPhase;
    float m_countdownTimer;
    float m_lightInterval;
    StartBoostResult m_boostResult;
    bool m_clutchHeld;
    float m_boostWindowTimer;
    bool m_boostWindowPassed;
    float m_activeBoostTimer;
    float m_currentBoostMultiplier;
    
    // LAP TIMING
    float m_currentLapTime;
    float m_bestLapTime;
    float m_lastLapTime;
    
    // Endless mode state
    EndlessStats m_stats;
    EndlessDifficulty m_difficulty;
    
    float m_lastCheckpointZ = 0.0f;
    float m_checkpointInterval = 5000.0f;
    
    bool m_inCorner = false;
    float m_cornerEntrySpeed = 0.0f;
    
    int m_lapCount = 0;
};