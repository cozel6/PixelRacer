#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include "GameModeConfig.h"

class CurveProcessor;

namespace RoadConfig {
    constexpr float SEGMENT_LENGTH = 10.0f;
    constexpr int ROAD_WIDTH = 2000;
    constexpr int LANES = 3;
    constexpr float CAMERA_HEIGHT = 1000.0f;
    constexpr float CAMERA_DEPTH = 1.0f / 300.0f;
    constexpr int DRAW_DISTANCE = 300;
    
    // Pothole settings
    constexpr float POTHOLE_SPAWN_CHANCE = 0.03f;
    constexpr float POTHOLE_DAMAGE = 5.0f;
    constexpr float POTHOLE_MIN_WIDTH = 80.0f;
    constexpr float POTHOLE_MAX_WIDTH = 150.0f;
    
    // Repair pickup settings
    constexpr float REPAIR_SPAWN_CHANCE = 0.05f;
    constexpr float REPAIR_HEAL_AMOUNT = 25.0f;
    constexpr float PICKUP_FLOAT_HEIGHT = 120.0f;
    constexpr float PICKUP_BOB_SPEED = 2.5f;
    constexpr float PICKUP_BOB_AMOUNT = 20.0f;
    
    // Dynamic pickup spawn rates based on damage
    constexpr int PICKUPS_LOW_DAMAGE = 5;      // 0-29 DMG: 5 pickups per turn
    constexpr int PICKUPS_MED_DAMAGE = 3;      // 30-49 DMG: 3 pickups per turn
    constexpr int PICKUPS_HIGH_DAMAGE = 1;     // 50+ DMG: 1 pickup per turn
}

struct Pothole {
    float offsetX = 0.0f;
    float width = 150.0f;
    bool exists = false;
    bool wasHit = false;
};

struct RepairPickup {
    float offsetX = 0.0f;
    float width = 100.0f;
    bool exists = false;
    bool collected = false;
    float healAmount = 25.0f;
    float animTimer = 0.0f;
    float bobOffset = 0.0f;
};

class RoadSegment {
public:
    RoadSegment();

    float worldZ;
    float curve;
    float worldY;

    sf::Color roadColor;
    sf::Color grassColor;
    sf::Color rumbleColor;
    sf::Color laneColor;

    float screenX;
    float screenY;
    float screenW;
    float scale;
    
    Pothole pothole;
    RepairPickup repairPickup;
};

class Road {
public:
    Road();

    void generate(int segmentCount);
    void generateWithDifficulty(int segmentCount, EndlessDifficultyLevel difficulty);
    void init(int segmentCount);
    void initClean(int segmentCount);  // Pentru Campaign - fără gropi și pickup-uri
    void update(float playerZ, float deltaTime);
    void render(sf::RenderWindow& window, float cameraZ);

    float getCurveAt(float z) const;
    RoadSegment* getSegmentAt(float z);
    const RoadSegment* getSegmentAt(float z) const;

    int getSegmentCount() const { return static_cast<int>(m_segments.size()); }
    float getLength() const { return m_segments.size() * RoadConfig::SEGMENT_LENGTH; }

    void addStraight(int startIndex, int count);
    void addCurve(int startIndex, int count, float curvature);
    void addHill(int startIndex, int count, float height);
    void setSegmentCurve(int index, float curve);
    
    void generatePotholes();
    void generatePotholesWithChance(float chance);
    bool checkPotholeCollision(float wheelX, float wheelZ, float& damageOut);
    void resetPotholes();
    
    void generateRepairPickups(float chance);
    void generateRepairPickupsFixed(int count);
    
    // ✅ NEW: Dynamic pickup system based on damage
    void regeneratePickupsForDamage(float currentDamage);
    
    bool checkRepairPickupCollision(float playerX, float playerZ, float& healOut);
    void resetPickups();
    
    void generateForCampaign(int segmentCount);

private:
    std::vector<RoadSegment> m_segments;
    float m_playerZ;
    std::mt19937 m_rng;
    
    float m_potholeChance = RoadConfig::POTHOLE_SPAWN_CHANCE;
    float m_repairChance = RoadConfig::REPAIR_SPAWN_CHANCE;
    int m_potholeCount = 0;

    void project(RoadSegment& segment, float cameraX, float cameraY, float cameraZ);
};