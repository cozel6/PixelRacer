#include "GameplayManager.h"
#include "TrackBuilder.h"
#include <iostream>
#include <cmath>

// ✅ CONVERSII REALISTE - Definim constantele aici
constexpr float UNITS_PER_METER = 1.0f;     // 1 unitate = 1 metru
constexpr float MS_TO_KMH = 3.6f;           // m/s → km/h (multiply by 3.6)

GameplayManager::GameplayManager(GameMode mode, const TrackDefinition* track)
    : m_mode(mode)
{
    if (track) {
        std::cout << "Building track: " << track->name << std::endl;
        loadTrack(*track);
        m_trackName = track->name;
    } else {
        std::cout << "No track selected, using procedural generation" << std::endl;
        initializeForMode();
        m_trackName = "Procedural Track";
    }
}

void GameplayManager::initializeForMode() {
    switch (m_mode) {
        case GameMode::Endless:
            m_road.generate(500);
            std::cout << "Initialized Endless Mode (procedural)" << std::endl;
            break;
        case GameMode::Campaign:
            m_road.generate(200);
            std::cout << "Initialized Campaign Mode (procedural)" << std::endl;
            break;
        case GameMode::TimeTrial:
            m_road.generate(300);
            std::cout << "Initialized Time Trial Mode (procedural)" << std::endl;
            break;
    }
}

void GameplayManager::loadTrack(const TrackDefinition& track) {
    TrackBuilder::buildTrack(m_road, track);
    
    std::cout << " Track loaded: " << track.name << std::endl;
    std::cout << "   Country: " << track.country << std::endl;
    std::cout << "   Length: " << track.lengthKm << " km" << std::endl;
    std::cout << "   Segments: " << m_road.getSegmentCount() << std::endl;
}

void GameplayManager::update(float deltaTime) {
    WheelSurfaces wheelSurfaces = getWheelSurfaces();
    m_player.update(deltaTime, wheelSurfaces);

    // ✅ Handle track looping for ALL modes (not just Endless!)
    handleTrackLooping();

    if (m_mode == GameMode::Endless) {
        updateEndlessScoring(deltaTime);
        updateDifficultyProgression();
        checkCheckpoints();
        evaluateCornerPerformance();
    }

    m_road.update(m_player.getZ());
}

void GameplayManager::handleTrackLooping() {
    // ✅ SMOOTH WRAP-AROUND pentru orice mod de joc
    float roadLength = m_road.getLength();
    float playerZ = m_player.getZ();

    // ✅ Smooth modulo pentru loop seamless
    if (playerZ >= roadLength) {
        // În loc să facem un hard reset, folosim modulo pentru continuitate
        float overflow = playerZ - roadLength;
        m_player.setZ(overflow);
        
        std::cout << "🔄 Track loop completed seamlessly" << std::endl;
    } else if (playerZ < 0.0f) {
        // Wrap backwards (pentru reverse)
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
        
        m_difficulty.trafficDensity = 0.1f + (newLevel - 1) * 0.05f;
        m_difficulty.minTrafficSpeed = 0.4f + (newLevel - 1) * 0.03f;
        m_difficulty.maxTrafficSpeed = 0.6f + (newLevel - 1) * 0.04f;
        m_difficulty.hazardChance = 0.0f + (newLevel - 1) * 0.02f;
        
        std::cout << "🔥 LEVEL UP! Now at Level " << newLevel << std::endl;
    }
      }

void GameplayManager::checkCheckpoints() {
    float playerZ = m_player.getZ();
    
    // ✅ Handle checkpoint wrap-around pentru loop seamless
    float roadLength = m_road.getLength();
    float effectiveDistance = playerZ + (m_lapCount * roadLength);
    
    if (effectiveDistance - m_lastCheckpointZ >= m_checkpointInterval) {
        m_stats.checkpointsReached++;
        m_lastCheckpointZ = effectiveDistance;
        
        float bonus = 1000.0f * m_difficulty.level;
        m_stats.currentScore += bonus;
        
        std::cout << "✅ CHECKPOINT! Bonus: " << bonus << " points" << std::endl;
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
            
            std::cout << "🎯 PERFECT CORNER! Bonus: " << bonus << std::endl;
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