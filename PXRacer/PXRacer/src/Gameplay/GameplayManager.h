#pragma once
#include "Player.h"
#include "Road.h"
#include "GameModeConfig.h"
#include "TrackDefinition.h"
#include "TrafficSystem.h"
#include "../Core/AudioManager.h"

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
    GameplayManager(GameMode mode, EndlessDifficultyLevel difficulty);
    
    void update(float deltaTime);
    void render(sf::RenderWindow& window);
    
    // Countdown system
    void handleStartInput(bool clutchPressed);
    bool isRaceStarted() const { return m_raceStarted; }
    bool isCountingDown() const { return !m_raceStarted && m_countdownPhase != CountdownPhase::Finished; }
    CountdownPhase getCountdownPhase() const { return m_countdownPhase; }
    float getCountdownTimer() const { return m_countdownTimer; }
    const StartBoostResult& getBoostResult() const { return m_boostResult; }
    bool hasBoostWindowPassed() const { return m_boostWindowPassed; }
    
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
    float getLastLapMultiplier() const { return m_lapScoreMultiplier; }
    float getPreviousLapTime() const { return m_previousLapTime; }
    
    const Player& getPlayer() const { return m_player; }
    
    // Endless mode getters
    EndlessDifficultyLevel getEndlessDifficulty() const { return m_endlessDifficulty; }
    float getHighscoreKm() const { return m_stats.highscoreKm; }
    int getLapsWithoutDamage() const { return m_stats.lapsWithoutDamage; }


    // Campaign mode - Public interface
    void setCampaignTrack(const CampaignTrack& track);  
    const CampaignProgress& getCampaignProgress() const { return m_campaignProgress; }
    const CampaignTrack& getCampaignTrack() const { return m_campaignTrack; }
    bool isObjectiveCompleted() const { return m_objectiveCompleted; }
    bool isRaceFinished() const { return m_raceFinished; }

private:
    void initializeForMode();
    void initializeEndless(EndlessDifficultyLevel difficulty);
    void loadTrack(const TrackDefinition& track);
    void handleTrackLooping();
    
    void updateCountdown(float deltaTime);
    void applyStartBoost();
    
    void updateEndlessScoring(float deltaTime);
    void updateDifficultyProgression();
    void checkCheckpoints();
    void evaluateCornerPerformance();
    void checkPotholeCollisions();
    void checkRepairPickups();
    
    CampaignTrack m_campaignTrack;
    CampaignProgress m_campaignProgress;
    bool m_objectiveCompleted = false;
    bool m_raceFinished = false;
    
    void updateCampaignProgress(float deltaTime);
    void checkCampaignObjective();

    WheelSurfaces getWheelSurfaces() const;
    SurfaceType getSurfaceTypeAt(float x, float z) const;
    
    GameMode m_mode;
    Player m_player;
    Road m_road;

    TrafficSystem m_traffic;
    
    std::string m_trackName;
    
    // Endless difficulty
    EndlessDifficultyLevel m_endlessDifficulty = EndlessDifficultyLevel::Medium;
    EndlessDifficultySettings m_difficultySettings;
    
    // Countdown state
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
    
    // Lap timing
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
    
    // Lap damage tracking
    float m_lapStartDamage = 0.0f;
    bool m_lapHadDamage = false;
    
    // Lap scoring system
    float m_previousLapTime = 0.0f;
    float m_lapScoreMultiplier = 1.0f;
    
    void calculateLapScore();
};