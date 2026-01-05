#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include <functional>
#include "Road.h"  // ✅ Pentru RoadConfig

namespace ObstacleConfig {
    // ═══════════════════════════════════════════════════════════════
    // 🎲 SPAWN SETTINGS
    // ═══════════════════════════════════════════════════════════════
    constexpr float MIN_SPAWN_DISTANCE = 150.0f;
    constexpr float SPAWN_AHEAD_DISTANCE = 500.0f;
    constexpr float DESPAWN_BEHIND = 50.0f;
    constexpr int MAX_OBSTACLES = 30;
    
    // ═══════════════════════════════════════════════════════════════
    // 🕳️ POTHOLE CONFIG
    // ═══════════════════════════════════════════════════════════════
    constexpr float POTHOLE_WIDTH = 150.0f;
    constexpr float POTHOLE_HEIGHT = 100.0f;
    constexpr float POTHOLE_DAMAGE = 8.0f;
    constexpr float POTHOLE_FLAT_CHANCE = 0.15f;
    constexpr float POTHOLE_SPEED_PENALTY = 0.85f;
    
    // ═══════════════════════════════════════════════════════════════
    // 🛢️ OIL SLICK CONFIG
    // ═══════════════════════════════════════════════════════════════
    constexpr float OIL_WIDTH = 250.0f;
    constexpr float OIL_HEIGHT = 180.0f;
    constexpr float OIL_SPIN_DURATION = 1.5f;
    constexpr float OIL_GRIP_MULTIPLIER = 0.1f;
    
    // ═══════════════════════════════════════════════════════════════
    // 🔧 FLAT TIRE SETTINGS
    // ═══════════════════════════════════════════════════════════════
    constexpr float FLAT_TIRE_MAX_SPEED = 5.5f;
    constexpr float FLAT_TIRE_DURATION = 8.0f;
    constexpr float FLAT_TIRE_GRIP = 0.4f;
}

enum class ObstacleType {
    Pothole,
    OilSlick,
    Debris,
    Puddle
};

struct ObstacleEffect {
    bool causedSpin = false;
    bool causedFlatTire = false;
    float damage = 0.0f;
    float gripModifier = 1.0f;
    float speedModifier = 1.0f;
    float effectDuration = 0.0f;
    
    bool hasEffect() const {
        return causedSpin || causedFlatTire || damage > 0.0f || 
               gripModifier < 1.0f || speedModifier < 1.0f;
    }
};

struct Obstacle {
    ObstacleType type;
    float worldZ;
    float worldX;
    float width;
    float height;
    bool isActive = true;
    bool wasHit = false;
    
    sf::Color color;
    float alpha = 255.0f;
    
    float screenX = 0.0f;
    float screenY = 0.0f;
    float scale = 0.0f;
    
    float getLeft() const { return worldX - width / 2.0f; }
    float getRight() const { return worldX + width / 2.0f; }
    float getFront() const { return worldZ + height / 2.0f; }
    float getBack() const { return worldZ - height / 2.0f; }
    
    ObstacleEffect calculateEffect(float impactSpeed, std::mt19937& rng) const;
};

class ObstacleSystem {
public:
    ObstacleSystem();
    
    void update(float deltaTime, float playerZ, float playerX, float playerSpeed,
                float carWidth, float carHeight);
    void render(sf::RenderWindow& window, float cameraZ, float cameraX,
                float screenCenterX, float screenCenterY);
    
    void setSpawnEnabled(bool enabled) { m_spawnEnabled = enabled; }
    void setDifficulty(float difficulty) { m_difficulty = difficulty; }
    void clear() { m_obstacles.clear(); m_lastSpawnZ = 0.0f; }
    
    using CollisionCallback = std::function<void(const ObstacleEffect&)>;
    void setCollisionCallback(CollisionCallback callback) { m_onCollision = callback; }
    
    const std::vector<Obstacle>& getObstacles() const { return m_obstacles; }
    int getActiveCount() const;

private:
    std::vector<Obstacle> m_obstacles;
    std::mt19937 m_rng;
    
    bool m_spawnEnabled = true;
    float m_difficulty = 0.5f;
    float m_lastSpawnZ = 0.0f;
    float m_spawnCooldown = 0.0f;
    
    CollisionCallback m_onCollision;
    
    void trySpawnObstacle(float playerZ);
    Obstacle createObstacle(ObstacleType type, float z, float x);
    ObstacleType getRandomType();
    float getRandomLateralPosition();
    void checkCollisions(float playerZ, float playerX, float playerSpeed,
                         float carWidth, float carHeight);
    bool checkCollision(const Obstacle& obs, float playerZ, float playerX,
                        float carWidth, float carHeight) const;
    void projectObstacle(Obstacle& obs, float cameraZ, float cameraX,
                         float screenCenterX, float screenCenterY);
    void removeInactiveObstacles(float playerZ);
};
