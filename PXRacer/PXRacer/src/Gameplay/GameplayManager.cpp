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
{
    m_boostResult = {false, false, false, false, 1.0f};
    
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

void GameplayManager::initializeForMode() {
    switch (m_mode) {
        case GameMode::Endless:
            m_road.generate(500);
            break;
        case GameMode::Campaign:
            m_road.generate(200);
            break;
        case GameMode::TimeTrial:
            m_road.generate(300);
            break;
    }
}

void GameplayManager::loadTrack(const TrackDefinition& track) {
    TrackBuilder::buildTrack(m_road, track);
    std::cout << "[TRACK] " << track.name << " loaded (" << track.lengthKm << " km)" << std::endl;
}

void GameplayManager::updateCountdown(float deltaTime) {
    m_countdownTimer -= deltaTime;
    
    switch (m_countdownPhase) {
        case CountdownPhase::Ready:
            if (m_countdownTimer <= 0.0f) {
                m_countdownPhase = CountdownPhase::Light1;
                m_countdownTimer = m_lightInterval;
                std::cout << "[COUNTDOWN] Light 1 ON" << std::endl;
            }
            break;
            
        case CountdownPhase::Light1:
            if (m_countdownTimer <= 0.0f) {
                m_countdownPhase = CountdownPhase::Light2;
                m_countdownTimer = m_lightInterval;
                std::cout << "[COUNTDOWN] Light 2 ON" << std::endl;
            }
            break;
            
        case CountdownPhase::Light2:
            if (m_countdownTimer <= 0.0f) {
                m_countdownPhase = CountdownPhase::Light3;
                m_countdownTimer = m_lightInterval;
                std::cout << "[COUNTDOWN] Light 3 ON" << std::endl;
            }
            break;
            
        case CountdownPhase::Light3:
            if (m_countdownTimer <= 0.0f) {
                m_countdownPhase = CountdownPhase::Light4;
                m_countdownTimer = m_lightInterval;
                std::cout << "[COUNTDOWN] Light 4 ON" << std::endl;
            }
            break;
            
        case CountdownPhase::Light4:
            if (m_countdownTimer <= 0.0f) {
                m_countdownPhase = CountdownPhase::Light5;
                m_countdownTimer = m_lightInterval;
                std::cout << "[COUNTDOWN] Light 5 ON - All reds!" << std::endl;
            }
            break;
            
        case CountdownPhase::Light5:
            if (m_countdownTimer <= 0.0f) {
                m_countdownPhase = CountdownPhase::Go;
                m_countdownTimer = GO_DISPLAY_TIME;
                m_boostWindowTimer = GOOD_WINDOW;
                m_raceStarted = true;
                std::cout << "[COUNTDOWN] LIGHTS OUT! GO GO GO!" << std::endl;
            }
            break;
            
        case CountdownPhase::Go:
            m_boostWindowTimer -= deltaTime;
            if (m_boostWindowTimer <= 0.0f) {
                m_boostWindowPassed = true;
            }
            
            if (m_countdownTimer <= 0.0f) {
                m_countdownPhase = CountdownPhase::Finished;
                m_boostWindowPassed = true;
                std::cout << "[COUNTDOWN] Finished" << std::endl;
            }
            break;
            
        case CountdownPhase::Finished:
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
            std::cout << "[START] PERFECT START! 2x BOOST!" << std::endl;
        } else if (timeSinceGo <= GOOD_WINDOW) {
            m_boostResult.good = true;
            m_boostResult.boostMultiplier = GOOD_BOOST;
            m_activeBoostTimer = BOOST_DURATION;
            m_currentBoostMultiplier = GOOD_BOOST;
            std::cout << "[START] Good start! 1.5x boost!" << std::endl;
        } else {
            m_boostResult.boostMultiplier = 1.0f;
            std::cout << "[START] Normal start" << std::endl;
        }
    } else if (m_countdownPhase != CountdownPhase::Finished) {
        m_boostResult.jumpStart = true;
        m_boostResult.boostMultiplier = 0.0f;
        m_activeBoostTimer = JUMP_START_PENALTY;
        m_currentBoostMultiplier = 0.0f;
        std::cout << "[START] JUMP START! 3 second penalty!" << std::endl;
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
        m_road.update(m_player.getZ());
        return;
    }
    
    m_currentLapTime += deltaTime;
    
    WheelSurfaces wheelSurfaces = getWheelSurfaces();
    const RoadSegment* segment = m_road.getSegmentAt(m_player.getZ());
    float roadCurve = segment ? segment->curve : 0.0f;
    m_player.update(deltaTime, wheelSurfaces, roadCurve);
    
    // ✅ CHECK POTHOLE COLLISIONS (integrate in Road)
    checkPotholeCollisions();
    
    if (m_currentBoostMultiplier > 1.0f && m_activeBoostTimer > 0.0f) {
        float currentSpeed = m_player.getSpeed();
        float boostedSpeed = currentSpeed + (PlayerConfig::ACCELERATION * (m_currentBoostMultiplier - 1.0f) * deltaTime);
        m_player.setSpeed(std::min(boostedSpeed, PlayerConfig::MAX_SPEED * 1.1f));
    }
    
    handleTrackLooping();
    
    if (m_mode == GameMode::Endless) {
        updateEndlessScoring(deltaTime);
        updateDifficultyProgression();
        checkCheckpoints();
        evaluateCornerPerformance();
    }
    
    m_road.update(m_player.getZ());
}

void GameplayManager::checkPotholeCollisions() {
    float flX, flZ, frX, frZ, rlX, rlZ, rrX, rrZ;
    m_player.getWheelPositions(flX, flZ, frX, frZ, rlX, rlZ, rrX, rrZ);
    
    float damage = 0.0f;
    
    if (m_road.checkPotholeCollision(flX, flZ, damage)) {
        m_player.addDamage(damage);
        std::cout << "[POTHOLE] Front-left wheel hit! Damage: " << damage << std::endl;
    }
    if (m_road.checkPotholeCollision(frX, frZ, damage)) {
        m_player.addDamage(damage);
        std::cout << "[POTHOLE] Front-right wheel hit! Damage: " << damage << std::endl;
    }
    if (m_road.checkPotholeCollision(rlX, rlZ, damage)) {
        m_player.addDamage(damage);
        std::cout << "[POTHOLE] Rear-left wheel hit! Damage: " << damage << std::endl;
    }
    if (m_road.checkPotholeCollision(rrX, rrZ, damage)) {
        m_player.addDamage(damage);
        std::cout << "[POTHOLE] Rear-right wheel hit! Damage: " << damage << std::endl;
    }
}

void GameplayManager::handleTrackLooping() {
    float roadLength = m_road.getLength();
    float playerZ = m_player.getZ();

    if (playerZ >= roadLength) {
        m_lastLapTime = m_currentLapTime;
        
        if (m_bestLapTime == 0.0f || m_currentLapTime < m_bestLapTime) {
            m_bestLapTime = m_currentLapTime;
            std::cout << "[LAP] NEW BEST: " << m_bestLapTime << "s!" << std::endl;
        }
        
        m_currentLapTime = 0.0f;
        m_lapCount++;
        
        float overflow = playerZ - roadLength;
        m_player.setZ(overflow);
        
        // ✅ Reset potholes for new lap
        m_road.resetPotholes();
        
        std::cout << "[LAP " << m_lapCount << "] Time: " << m_lastLapTime << "s" << std::endl;
    } else if (playerZ < 0.0f) {
        float underflow = playerZ + roadLength;
        m_player.setZ(underflow);
    }
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
    m_stats.averageSpeed = (m_stats.averageSpeed * (m_stats.timeElapsed - deltaTime) + speedKmh * deltaTime) / m_stats.timeElapsed;
    
    float baseScore = speed * deltaTime * 1.0f;
    float speedBonus = std::max(0.0f, (speedKmh - 100.0f) / 100.0f);
    float multiplier = 1.0f + speedBonus;
    
    if (m_player.getSpeed() < PlayerConfig::MAX_SPEED * 0.5f) {
        multiplier *= 0.5f;
    }
    
    m_stats.currentScore += baseScore * multiplier;
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
        
        float bonus = 1000.0f * m_difficulty.level;
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
            float bonus = 500.0f * m_difficulty.level;
            m_stats.currentScore += bonus;
        }
    }
}
        
void GameplayManager::render(sf::RenderWindow& window) {
    m_road.render(window, m_player.getZ());

    const float playerScreenX = window.getSize().x * 0.5f + m_player.getX() * 0.5f;
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