#include "GameplayManager.h"
#include "TrackBuilder.h"
#include <iostream>
#include <cmath>

constexpr float UNITS_PER_METER = 1.0f;
constexpr float MS_TO_KMH = 3.6f;

constexpr float LIGHT_INTERVAL = 1.0f;
constexpr float GO_DISPLAY_TIME = 0.5f;
constexpr float PERFECT_WINDOW = 0.15f;
constexpr float GOOD_WINDOW = 0.3f;
constexpr float BOOST_DURATION = 2.0f;
constexpr float PERFECT_BOOST = 2.0f;
constexpr float GOOD_BOOST = 1.5f;
constexpr float JUMP_START_PENALTY = 3.0f;

GameplayManager::GameplayManager(GameMode mode, const TrackDefinition* track)
    : m_mode(mode)
    , m_raceStarted(false)
    , m_countdownPhase(CountdownPhase::Ready)
    , m_countdownTimer(0.0f)
    , m_lightInterval(LIGHT_INTERVAL)
    , m_clutchHeld(false)
    , m_boostWindowTimer(0.0f)
    , m_boostWindowPassed(false)
    , m_activeBoostTimer(0.0f)
    , m_currentBoostMultiplier(1.0f)
    , m_currentLapTime(0.0f)
    , m_bestLapTime(0.0f)
    , m_lastLapTime(0.0f)
    , m_endlessDifficulty(EndlessDifficultyLevel::Medium)
    , m_lapStartDamage(0.0f)
    , m_lapHadDamage(false)
{
    m_boostResult = {false, false, false, false, 1.0f};
    m_difficultySettings = EndlessDifficultySettings::getSettings(m_endlessDifficulty);
    
    if (track) {
        std::cout << "[RACE] Building track: " << track->name << std::endl;
        loadTrack(*track);
        m_trackName = track->name;
    } else {
        std::cout << "[RACE] Using procedural generation" << std::endl;
        initializeForMode();
        m_trackName = "Procedural Track";
    }
    
    m_countdownTimer = 1.0f;
    m_countdownPhase = CountdownPhase::Ready;
    
    std::cout << "[RACE] Countdown starting..." << std::endl;
}

// Constructor for Endless with difficulty
GameplayManager::GameplayManager(GameMode mode, EndlessDifficultyLevel difficulty)
    : m_mode(mode)
    , m_raceStarted(false)
    , m_countdownPhase(CountdownPhase::Ready)
    , m_countdownTimer(0.0f)
    , m_lightInterval(LIGHT_INTERVAL)
    , m_clutchHeld(false)
    , m_boostWindowTimer(0.0f)
    , m_boostWindowPassed(false)
    , m_activeBoostTimer(0.0f)
    , m_currentBoostMultiplier(1.0f)
    , m_currentLapTime(0.0f)
    , m_bestLapTime(0.0f)
    , m_lastLapTime(0.0f)
    , m_endlessDifficulty(difficulty)
    , m_lapStartDamage(0.0f)
    , m_lapHadDamage(false)
{
    m_boostResult = {false, false, false, false, 1.0f};
    m_difficultySettings = EndlessDifficultySettings::getSettings(difficulty);
    
    initializeEndless(difficulty);
    m_trackName = "Endless - " + m_difficultySettings.name;
    
    m_countdownTimer = 1.0f;
    m_countdownPhase = CountdownPhase::Ready;
    
    std::cout << "[ENDLESS] Mode started with " << m_difficultySettings.name << " difficulty" << std::endl;
}

void GameplayManager::initializeForMode() {
    switch (m_mode) {
        case GameMode::Endless:
            m_road.generateWithDifficulty(500, m_endlessDifficulty);
            break;
        case GameMode::Campaign:
            m_road.generate(200);
            break;
        case GameMode::TimeTrial:
            m_road.generate(300);
            break;
    }
    m_traffic.init(m_road.getLength());
}

// Initialize Endless mode with specific difficulty
void GameplayManager::initializeEndless(EndlessDifficultyLevel difficulty) {
    m_road.generateWithDifficulty(500, difficulty);
    m_traffic.init(m_road.getLength());
    m_stats = EndlessStats{};  // Reset stats
    m_lapStartDamage = 0.0f;
    m_lapHadDamage = false;
}

void GameplayManager::loadTrack(const TrackDefinition& track) {
    TrackBuilder::buildTrack(m_road, track);
    std::cout << "[TRACK] " << track.name << " loaded (" << track.lengthKm << " km)" << std::endl;
    m_traffic.init(m_road.getLength());
}

void GameplayManager::updateCountdown(float deltaTime) {
    m_countdownTimer -= deltaTime;
    
    switch (m_countdownPhase) {
        case CountdownPhase::Ready:
            if (m_countdownTimer <= 0.0f) {
                m_countdownPhase = CountdownPhase::Light1;
                m_countdownTimer = m_lightInterval;
                AudioManager::getInstance().playSfx("countdown_beep");
            }
            break;
        case CountdownPhase::Light1:
            if (m_countdownTimer <= 0.0f) {
                m_countdownPhase = CountdownPhase::Light2;
                m_countdownTimer = m_lightInterval;
                AudioManager::getInstance().playSfx("countdown_beep");
            }
            break;
        case CountdownPhase::Light2:
            if (m_countdownTimer <= 0.0f) {
                m_countdownPhase = CountdownPhase::Light3;
                m_countdownTimer = m_lightInterval;
                AudioManager::getInstance().playSfx("countdown_beep");
            }
            break;
        case CountdownPhase::Light3:
            if (m_countdownTimer <= 0.0f) {
                m_countdownPhase = CountdownPhase::Light4;
                m_countdownTimer = m_lightInterval;
                AudioManager::getInstance().playSfx("countdown_beep");
            }
            break;
        case CountdownPhase::Light4:
            if (m_countdownTimer <= 0.0f) {
                m_countdownPhase = CountdownPhase::Light5;
                m_countdownTimer = m_lightInterval;
                AudioManager::getInstance().playSfx("countdown_beep");
            }
            break;
        case CountdownPhase::Light5:
            if (m_countdownTimer <= 0.0f) {
                m_countdownPhase = CountdownPhase::Go;
                m_countdownTimer = GO_DISPLAY_TIME;
                m_boostWindowTimer = GOOD_WINDOW;
                m_raceStarted = true;
                
                
            }
            break;
        case CountdownPhase::Go:
            static bool goSoundPlayed = false;
            if (!goSoundPlayed) {
                AudioManager::getInstance().playSfx("countdown_go");
                goSoundPlayed = true;
            }

            m_boostWindowTimer -= deltaTime;
            if (m_boostWindowTimer <= 0.0f) {
                m_boostWindowPassed = true;
            }
            if (m_countdownTimer <= 0.0f) {
                m_countdownPhase = CountdownPhase::Finished;
                m_boostWindowPassed = true;
            }
            break;
    }
}

void GameplayManager::handleStartInput(bool clutchPressed) {
    if (m_boostResult.attempted) return;
    
    m_clutchHeld = clutchPressed;
    
    if (!clutchPressed) return;
    
    m_boostResult.attempted = true;
    
    if (m_countdownPhase == CountdownPhase::Go) {
        float timeSinceGo = GO_DISPLAY_TIME - m_countdownTimer;
        
        if (timeSinceGo <= PERFECT_WINDOW) {
            m_boostResult.perfect = true;
            m_boostResult.boostMultiplier = PERFECT_BOOST;
            m_activeBoostTimer = BOOST_DURATION;
            m_currentBoostMultiplier = PERFECT_BOOST;
        } else if (timeSinceGo <= GOOD_WINDOW) {
            m_boostResult.good = true;
            m_boostResult.boostMultiplier = GOOD_BOOST;
            m_activeBoostTimer = BOOST_DURATION;
            m_currentBoostMultiplier = GOOD_BOOST;
        } else {
            m_boostResult.boostMultiplier = 1.0f;
        }
    } else if (m_countdownPhase != CountdownPhase::Finished) {
        m_boostResult.jumpStart = true;
        m_boostResult.boostMultiplier = 0.0f;
        m_activeBoostTimer = JUMP_START_PENALTY;
        m_currentBoostMultiplier = 0.0f;
    }
}

void GameplayManager::applyStartBoost() {
}

void GameplayManager::update(float deltaTime) {
    if (!m_raceStarted || m_countdownPhase == CountdownPhase::Go) {
        updateCountdown(deltaTime);
    }
    
    if (m_activeBoostTimer > 0.0f) {
        m_activeBoostTimer -= deltaTime;
        if (m_activeBoostTimer <= 0.0f) {
            m_currentBoostMultiplier = 1.0f;
            m_activeBoostTimer = 0.0f;
        }
    }
    
    if (!m_raceStarted) {
        return;
    }
    
    if (m_boostResult.jumpStart && m_activeBoostTimer > 0.0f) {
        m_road.update(m_player.getZ(), deltaTime);
        return;
    }
    
    m_currentLapTime += deltaTime;
    
    WheelSurfaces wheelSurfaces = getWheelSurfaces();
    const RoadSegment* segment = m_road.getSegmentAt(m_player.getZ());
    float roadCurve = segment ? segment->curve : 0.0f;
    m_player.update(deltaTime, wheelSurfaces, roadCurve);

    m_traffic.update(deltaTime, m_road.getLength(), m_player.getZ(), m_player.getSpeed());
    // Campaign mode updates
    if (m_mode == GameMode::Campaign) {
        updateCampaignProgress(deltaTime);
        
        if (m_raceFinished) {
            return;  // Stop updates when race finished
        }
    }
    
    // Do not call checkPotholeCollisions() and checkRepairPickups() in Campaign
    if (m_mode == GameMode::Endless) {
        checkPotholeCollisions();
        checkRepairPickups();
    }
    
    if (m_currentBoostMultiplier > 1.0f && m_activeBoostTimer > 0.0f) {
        float currentSpeed = m_player.getSpeed();
        float boostedSpeed = currentSpeed + (PlayerConfig::ACCELERATION * (m_currentBoostMultiplier - 1.0f) * deltaTime);
        m_player.setSpeed(std::min(boostedSpeed, PlayerConfig::MAX_SPEED * 1.1f));
    }
    
    // Add call to handleTrackLooping() to detect finish line crossing
    handleTrackLooping();
    
    if (m_mode == GameMode::Endless) {
        updateEndlessScoring(deltaTime);
        updateDifficultyProgression();
        checkCheckpoints();
        evaluateCornerPerformance();
    }
    
    m_road.update(m_player.getZ(), deltaTime);
}

void GameplayManager::checkPotholeCollisions() {
    float flX, flZ, frX, frZ, rlX, rlZ, rrX, rrZ;
    m_player.getWheelPositions(flX, flZ, frX, frZ, rlX, rlZ, rrX, rrZ);
    
    float damage = 0.0f;
    
    if (m_road.checkPotholeCollision(flX, flZ, damage) ||
        m_road.checkPotholeCollision(frX, frZ, damage) ||
        m_road.checkPotholeCollision(rlX, rlZ, damage) ||
        m_road.checkPotholeCollision(rrX, rrZ, damage)) {
        
        m_player.addDamage(damage);
        m_lapHadDamage = true;  // Track damage for bonus
        std::cout << "[POTHOLE] Hit! Damage: " << damage << std::endl;
    }
}

// NEW: Check repair pickup collisions
void GameplayManager::checkRepairPickups() {
    float healAmount = 0.0f;
    
    if (m_road.checkRepairPickupCollision(m_player.getX(), m_player.getZ(), healAmount)) {
        float oldDamage = m_player.getTotalDamage();
        m_player.repair(healAmount);
        m_stats.repairPickupsCollected++;
        
        std::cout << "[REPAIR] Collected! Healed " << healAmount 
                  << " DMG (" << oldDamage << " -> " << m_player.getTotalDamage() << ")" << std::endl;
    }
}

void GameplayManager::handleTrackLooping() {
    float roadLength = m_road.getLength();
    float playerZ = m_player.getZ();

    if (playerZ >= roadLength) {
        m_lastLapTime = m_currentLapTime;
        
        // Campaign mode - track lap progress
        if (m_mode == GameMode::Campaign) {
            m_campaignProgress.currentLap++;
            
            // Update best lap time
            if (m_campaignProgress.bestLapTime == 0.0f || 
                m_currentLapTime < m_campaignProgress.bestLapTime) {
                m_campaignProgress.bestLapTime = m_currentLapTime;
            }
            
            // Track perfect laps
            if (m_campaignProgress.currentLapPerfect) {
                m_campaignProgress.perfectLapsCount++;
                std::cout << "[CAMPAIGN] Perfect lap! Total: " 
                          << m_campaignProgress.perfectLapsCount << std::endl;
            }
            m_campaignProgress.currentLapPerfect = true;  // Reset for next lap
            
            std::cout << "[CAMPAIGN] Lap " << m_campaignProgress.currentLap 
                      << "/" << m_campaignProgress.totalLaps 
                      << " | Time: " << m_currentLapTime << "s" << std::endl;
                      
            // Check if race finished
            checkCampaignObjective();
        }
        
        // Endless mode - scoring
        if (m_mode == GameMode::Endless) {
            calculateLapScore();
            
            // Update best lap
            bool isNewBestLap = false;
            if (m_bestLapTime == 0.0f || m_currentLapTime < m_bestLapTime) {
                m_bestLapTime = m_currentLapTime;
                isNewBestLap = true;
            }
            
            // Apply bonus for new best lap
            if (isNewBestLap && m_lapCount > 0) {
                float bestLapBonus = 100.0f * m_difficultySettings.bonusMultiplier;
                m_stats.currentScore += bestLapBonus;
                std::cout << "[NEW BEST LAP] Bonus! +" << bestLapBonus << " points!" << std::endl;
            }
            
            // Check for bonus lap without damage
            if (!m_lapHadDamage && m_mode == GameMode::Endless) {
                m_stats.lapsWithoutDamage++;
                float bonus = EndlessConfigValues::NO_DAMAGE_LAP_BONUS * m_difficultySettings.bonusMultiplier;
                m_stats.currentScore += bonus;
                std::cout << "[LAP BONUS] No damage! +" << bonus << " points!" << std::endl;
            }
        }
        
        // Save current time for comparison
        m_previousLapTime = m_currentLapTime;
        
        m_currentLapTime = 0.0f;
        m_lapCount++;
        
        float overflow = playerZ - roadLength;
        m_player.setZ(overflow);
        
        // Only for Endless mode - regenerate obstacles
        if (m_mode == GameMode::Endless) {
            m_road.resetPotholes();
            m_road.regeneratePickupsForDamage(m_player.getTotalDamage());
        }
        
        // Reset lap damage tracking
        m_lapStartDamage = m_player.getTotalDamage();
        m_lapHadDamage = false;
        
        std::cout << "[LAP " << m_lapCount << "] Time: " << m_lastLapTime 
                  << "s, Score: " << m_stats.currentScore << std::endl;
                  
    } else if (playerZ < 0.0f) {
        float underflow = playerZ + roadLength;
        m_player.setZ(underflow);
    }
}

void GameplayManager::calculateLapScore() {
    float baseScore = 100.0f;
    float multiplier = 1.0f;
    
    // If not first lap, check if we were faster
    if (m_previousLapTime > 0.0f && m_lapCount > 0) {
        float delta = m_previousLapTime - m_currentLapTime;
        
        if (delta > 0.0f) {
            // We were FASTER! Calculate multiplier
            // Formula: 1.0 + (delta_seconds * 0.5)
            multiplier = 1.0f + (delta * 0.5f);
            multiplier = std::min(multiplier, 5.0f);  // Cap at 5x
            
            std::cout << "[LAP SCORE] " << delta << "s faster! Multiplier: " << multiplier << "x" << std::endl;
        } else {
            std::cout << "[LAP SCORE] " << (-delta) << "s slower. Base score only." << std::endl;
        }
    }
    
    float finalScore = baseScore * multiplier * m_difficultySettings.bonusMultiplier;
    m_stats.currentScore += finalScore;
    
    m_lapScoreMultiplier = multiplier;
    
    std::cout << "[LAP SCORE] +" << finalScore << " points" << std::endl;
}

WheelSurfaces GameplayManager::getWheelSurfaces() const {
    float flX, flZ, frX, frZ, rlX, rlZ, rrX, rrZ;
    m_player.getWheelPositions(flX, flZ, frX, frZ, rlX, rlZ, rrX, rrZ);
    
    WheelSurfaces surfaces;
    surfaces.frontLeft = getSurfaceTypeAt(flX, flZ);
    surfaces.frontRight = getSurfaceTypeAt(frX, frZ);
    surfaces.rearLeft = getSurfaceTypeAt(rlX, rlZ);
    surfaces.rearRight = getSurfaceTypeAt(rrX, rrZ);
    
    return surfaces;
}

SurfaceType GameplayManager::getSurfaceTypeAt(float x, float z) const {
    const RoadSegment* segment = m_road.getSegmentAt(z);
    if (!segment) return SurfaceType::OffTrack;
    
    const float roadHalfWidth = RoadConfig::ROAD_WIDTH * 0.5f;
    
    if (std::abs(x) <= roadHalfWidth) {
        return SurfaceType::Road;
    } else if (std::abs(x) <= roadHalfWidth + 500.0f) {
        return SurfaceType::Grass;
    } else {
        return SurfaceType::OffTrack;
    }
}

float GameplayManager::getPlayerSpeedKmh() const {
    return m_player.getSpeed() * MS_TO_KMH;
}

float GameplayManager::getDistanceKm() const {
    return m_player.getZ() * UNITS_PER_METER / 1000.0f;
}

const EndlessStats& GameplayManager::getStats() const {
    return m_stats;
}

int GameplayManager::getCurrentLevel() const {
    return m_difficulty.level;
}

void GameplayManager::updateEndlessScoring(float deltaTime) {
    m_stats.timeElapsed += deltaTime;
    
    float speed = m_player.getSpeed();
    float speedKmh = speed * MS_TO_KMH;
    
    m_stats.totalDistance += speed * deltaTime;
    m_stats.topSpeed = std::max(m_stats.topSpeed, speedKmh);
    
    // Calculate average speed
    if (m_stats.timeElapsed > 0.0f) {
        m_stats.averageSpeed = (m_stats.averageSpeed * (m_stats.timeElapsed - deltaTime) + speedKmh * deltaTime) / m_stats.timeElapsed;
    }
    
    // Update highscore (in km traveled)
    float currentKm = getDistanceKm() + (m_lapCount * m_road.getLength() / 1000.0f);
    if (currentKm > m_stats.highscoreKm) {
        m_stats.highscoreKm = currentKm;
    }
}

void GameplayManager::updateDifficultyProgression() {
    float distanceKm = getDistanceKm();
    
    int newLevel = 1 + static_cast<int>(distanceKm / 10.0f);
    newLevel = std::min(newLevel, 10);
    
    if (newLevel != m_difficulty.level) {
        m_difficulty.level = newLevel;
    }
}

void GameplayManager::checkCheckpoints() {
    float playerZ = m_player.getZ();
    float roadLength = m_road.getLength();
    float effectiveDistance = playerZ + (m_lapCount * roadLength);
    
    if (effectiveDistance - m_lastCheckpointZ >= m_checkpointInterval) {
        m_stats.checkpointsReached++;
        m_lastCheckpointZ = effectiveDistance;
        
        float bonus = 1000.0f * m_difficulty.level * m_difficultySettings.bonusMultiplier;
        m_stats.currentScore += bonus;
    }
}

void GameplayManager::evaluateCornerPerformance() {
    const RoadSegment* segment = m_road.getSegmentAt(m_player.getZ());
    if (!segment) return;
    
    bool isInCurve = std::abs(segment->curve) > 0.1f;
    
    if (isInCurve && !m_inCorner) {
        m_inCorner = true;
        m_cornerEntrySpeed = m_player.getSpeed();
    }
    
    if (!isInCurve && m_inCorner) {
        m_inCorner = false;
        float exitSpeed = m_player.getSpeed();
        
        if (exitSpeed >= m_cornerEntrySpeed * 0.9f && !m_player.isSpinning()) {
            m_stats.perfectCornersCount++;
            float bonus = 500.0f * m_difficulty.level * m_difficultySettings.bonusMultiplier;
            m_stats.currentScore += bonus;
        }
    }
}
        
void GameplayManager::render(sf::RenderWindow& window) {
    m_road.render(window, m_player.getZ());

   
    m_traffic.render(window, m_player.getZ(), m_player.getX());
    const float playerScreenX = window.getSize().x * 0.5f + (m_player.getX() / RoadConfig::ROAD_WIDTH) * window.getSize().x * 0.5f;
    const float playerScreenY = window.getSize().y * 0.85f;
    m_player.render(window, playerScreenX, playerScreenY, 1.0f);
}

float GameplayManager::getPlayerSpeed() const {
    return m_player.getSpeed();
}

float GameplayManager::getDistance() const {
    return m_player.getZ();
}

GameMode GameplayManager::getGameMode() const {
    return m_mode;
}

// Add Campaign functions:

void GameplayManager::setCampaignTrack(const CampaignTrack& track) {
    m_campaignTrack = track;
    m_campaignProgress.reset();
    m_campaignProgress.totalLaps = track.requiredLaps;
    m_objectiveCompleted = false;
    m_raceFinished = false;
    
    // Display selected task if it exists
    const CampaignTask* selectedTask = track.getSelectedTask();
    if (selectedTask) {
        std::cout << "[CAMPAIGN] Track: " << track.name 
                  << " | Task: " << selectedTask->description << std::endl;
    } else {
        std::cout << "[CAMPAIGN] Track: " << track.name 
                  << " | No task selected" << std::endl;
    }
}

void GameplayManager::updateCampaignProgress(float deltaTime) {
    if (m_raceFinished) return;
    
    m_campaignProgress.raceTime += deltaTime;
    
    // Track top speed
    float speedKmh = getPlayerSpeedKmh();
    if (speedKmh > m_campaignProgress.topSpeed) {
        m_campaignProgress.topSpeed = speedKmh;
    }
    
    // Track spinouts
    if (m_player.isSpinning()) {
        static bool wasSpinning = false;
        if (!wasSpinning) {
            m_campaignProgress.spinoutCount++;
            m_campaignProgress.currentLapPerfect = false;
            std::cout << "[CAMPAIGN] Spinout! Count: " << m_campaignProgress.spinoutCount << std::endl;
        }
        wasSpinning = true;
    } else {
        static bool wasSpinning = false;
        wasSpinning = false;
    }
    
    // Track off-track
    WheelSurfaces surfaces = getWheelSurfaces();
    bool anyWheelOffTrack = (surfaces.frontLeft == SurfaceType::OffTrack ||
                             surfaces.frontRight == SurfaceType::OffTrack ||
                             surfaces.rearLeft == SurfaceType::OffTrack ||
                             surfaces.rearRight == SurfaceType::OffTrack);
    
    if (anyWheelOffTrack) {
        static bool wasOffTrack = false;
        if (!wasOffTrack) {
            m_campaignProgress.offTrackCount++;
            m_campaignProgress.currentLapPerfect = false;
            std::cout << "[CAMPAIGN] Off track! Count: " << m_campaignProgress.offTrackCount << std::endl;
        }
        wasOffTrack = true;
    } else {
        static bool wasOffTrack = false;
        wasOffTrack = false;
    }
    
    // Check objective in real-time
    checkCampaignObjective();
}

void GameplayManager::checkCampaignObjective() {
    const CampaignTask* selectedTask = m_campaignTrack.getSelectedTask();
    
    if (selectedTask && m_campaignProgress.checkTask(*selectedTask)) {
        if (!m_objectiveCompleted) {
            m_objectiveCompleted = true;
            std::cout << "[CAMPAIGN] TASK COMPLETED!" << std::endl;
        }
    }
    
    // Race finished when completing all laps
    if (m_campaignProgress.currentLap >= m_campaignProgress.totalLaps) {
        if (!m_raceFinished) {
            m_raceFinished = true;
            std::cout << "[CAMPAIGN] RACE FINISHED!" << std::endl;
        }
    }
}
