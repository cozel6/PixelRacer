#include "ObstacleSystem.h"
#include "Player.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>

ObstacleEffect Obstacle::calculateEffect(float impactSpeed, std::mt19937& rng) const {
    ObstacleEffect effect;
    
    switch (type) {
        case ObstacleType::Pothole: {
            float speedFactor = impactSpeed / PlayerConfig::MAX_SPEED;
            effect.damage = ObstacleConfig::POTHOLE_DAMAGE * speedFactor;
            effect.speedModifier = ObstacleConfig::POTHOLE_SPEED_PENALTY;
            effect.effectDuration = 0.5f;
            
            float flatChance = ObstacleConfig::POTHOLE_FLAT_CHANCE * (0.5f + speedFactor * 0.5f);
            std::uniform_real_distribution<float> dist(0.0f, 1.0f);
            if (dist(rng) < flatChance) {
                effect.causedFlatTire = true;
                effect.effectDuration = ObstacleConfig::FLAT_TIRE_DURATION;
            }
            break;
        }
        
        case ObstacleType::OilSlick: {
            if (impactSpeed > 30.0f) {
                effect.causedSpin = true;
                effect.effectDuration = ObstacleConfig::OIL_SPIN_DURATION;
            }
            effect.gripModifier = ObstacleConfig::OIL_GRIP_MULTIPLIER;
            break;
        }
        
        case ObstacleType::Debris: {
            float speedFactor = impactSpeed / PlayerConfig::MAX_SPEED;
            effect.damage = 5.0f * speedFactor;
            break;
        }
        
        case ObstacleType::Puddle: {
            effect.gripModifier = 0.5f;
            effect.effectDuration = 0.8f;
            break;
        }
    }
    
    return effect;
}

ObstacleSystem::ObstacleSystem() 
    : m_rng(std::random_device{}())
{
}

void ObstacleSystem::update(float deltaTime, float playerZ, float playerX, 
                            float playerSpeed, float carWidth, float carHeight) {
    if (m_spawnEnabled) {
        m_spawnCooldown -= deltaTime;
        if (m_spawnCooldown <= 0.0f) {
            trySpawnObstacle(playerZ);
        }
    }
    
    checkCollisions(playerZ, playerX, playerSpeed, carWidth, carHeight);
    removeInactiveObstacles(playerZ);
}

void ObstacleSystem::render(sf::RenderWindow& window, float cameraZ, float cameraX,
                            float screenCenterX, float screenCenterY) {
    const float windowWidth = static_cast<float>(window.getSize().x);
    const float windowHeight = static_cast<float>(window.getSize().y);
    
    // Sort: far to near (for correct drawing order)
    std::vector<Obstacle*> sortedObs;
    for (auto& obs : m_obstacles) {
        if (obs.isActive) {
            sortedObs.push_back(&obs);
        }
    }
    
    std::sort(sortedObs.begin(), sortedObs.end(), 
        [](const Obstacle* a, const Obstacle* b) {
            return a->worldZ > b->worldZ;
        });
    
    for (Obstacle* obs : sortedObs) {
        float dz = obs->worldZ - cameraZ;
        
        // Skip if behind camera
        if (dz <= 1.0f) continue;
        
        // Projection using the same constants as Road.cpp
        float screenYNorm = RoadConfig::CAMERA_DEPTH * RoadConfig::CAMERA_HEIGHT / dz;
        float screenY = windowHeight * 0.5f + screenYNorm * windowHeight;
        
        // Skip if above horizon or too low
        if (screenY < windowHeight * 0.5f || screenY > windowHeight) continue;
        
        // Scale based on distance
        float scale = RoadConfig::CAMERA_DEPTH / dz;
        
        // Screen X position
        float screenX = windowWidth * 0.5f + (obs->worldX - cameraX) * scale * windowWidth * 0.5f / RoadConfig::ROAD_WIDTH * 2.0f;
        
        // Scaled dimensions
        float drawW = obs->width * scale * windowWidth * 0.25f;
        float drawH = obs->height * scale * windowHeight * 0.12f;
        
        // Size constraints
        drawW = std::clamp(drawW, 3.0f, 250.0f);
        drawH = std::clamp(drawH, 2.0f, 120.0f);
        
        // Render
        sf::RectangleShape shape(sf::Vector2f(drawW, drawH));
        shape.setPosition(sf::Vector2f(screenX - drawW / 2.0f, screenY - drawH));
        
        sf::Color color = obs->color;
        color.a = static_cast<std::uint8_t>(obs->alpha * (obs->wasHit ? 0.4f : 1.0f));
        shape.setFillColor(color);
        
        // Outline for visibility
        float outlineSize = std::max(1.0f, scale * 50.0f);
        shape.setOutlineThickness(outlineSize);
        
        // Outline color based on type
        sf::Color outlineColor;
        switch (obs->type) {
            case ObstacleType::Pothole:
                outlineColor = sf::Color(100, 80, 60);
                break;
            case ObstacleType::OilSlick:
                outlineColor = sf::Color(50, 50, 80);
                break;
            case ObstacleType::Debris:
                outlineColor = sf::Color(80, 80, 80);
                break;
            case ObstacleType::Puddle:
                outlineColor = sf::Color(60, 120, 180);
                break;
        }
        outlineColor.a = color.a;
        shape.setOutlineColor(outlineColor);
        
        window.draw(shape);
        
        // Visual effects specific to each type
        if (obs->type == ObstacleType::OilSlick && drawW > 10.0f) {
            // Rainbow sheen effect
            sf::RectangleShape sheen(sf::Vector2f(drawW * 0.6f, drawH * 0.3f));
            sheen.setPosition(sf::Vector2f(screenX - drawW * 0.3f, screenY - drawH * 0.85f));
            sheen.setFillColor(sf::Color(255, 255, 255, 60));
            window.draw(sheen);
        }
        else if (obs->type == ObstacleType::Pothole && drawW > 10.0f) {
            // Dark center
            sf::RectangleShape center(sf::Vector2f(drawW * 0.5f, drawH * 0.5f));
            center.setPosition(sf::Vector2f(screenX - drawW * 0.25f, screenY - drawH * 0.75f));
            center.setFillColor(sf::Color(15, 15, 15, color.a));
            window.draw(center);
        }
        else if (obs->type == ObstacleType::Puddle && drawW > 10.0f) {
            // Water highlight
            sf::RectangleShape highlight(sf::Vector2f(drawW * 0.4f, drawH * 0.25f));
            highlight.setPosition(sf::Vector2f(screenX - drawW * 0.2f, screenY - drawH * 0.9f));
            highlight.setFillColor(sf::Color(220, 240, 255, 90));
            window.draw(highlight);
        }
    }
}

void ObstacleSystem::trySpawnObstacle(float playerZ) {
    using namespace ObstacleConfig;
    
    if (static_cast<int>(m_obstacles.size()) >= MAX_OBSTACLES) return;
    
    float spawnZ = playerZ + SPAWN_AHEAD_DISTANCE;
    
    // Reset spawn tracker if player has advanced
    if (m_lastSpawnZ < playerZ) {
        m_lastSpawnZ = playerZ;
    }
    
    if (spawnZ - m_lastSpawnZ < MIN_SPAWN_DISTANCE) {
        m_spawnCooldown = 0.1f;
        return;
    }
    
    // Spawn chance based on difficulty
    std::uniform_real_distribution<float> chanceDist(0.0f, 1.0f);
    float spawnChance = 0.5f + m_difficulty * 0.35f;  // 50-85%
    
    if (chanceDist(m_rng) > spawnChance) {
        m_spawnCooldown = 0.15f;
        return;
    }
    
    ObstacleType type = getRandomType();
    float lateralPos = getRandomLateralPosition();
    
    Obstacle obs = createObstacle(type, spawnZ, lateralPos);
    m_obstacles.push_back(obs);
    
    m_lastSpawnZ = spawnZ;
    m_spawnCooldown = 0.25f + (1.0f - m_difficulty) * 0.35f;  // 0.25-0.6s
}

Obstacle ObstacleSystem::createObstacle(ObstacleType type, float z, float x) {
    Obstacle obs;
    obs.type = type;
    obs.worldZ = z;
    obs.worldX = x;
    obs.isActive = true;
    obs.wasHit = false;
    
    switch (type) {
        case ObstacleType::Pothole:
            obs.width = ObstacleConfig::POTHOLE_WIDTH;
            obs.height = ObstacleConfig::POTHOLE_HEIGHT;
            obs.color = sf::Color(50, 40, 35);
            break;
            
        case ObstacleType::OilSlick:
            obs.width = ObstacleConfig::OIL_WIDTH;
            obs.height = ObstacleConfig::OIL_HEIGHT;
            obs.color = sf::Color(25, 25, 40, 220);
            break;
            
        case ObstacleType::Debris:
            obs.width = 100.0f;
            obs.height = 70.0f;
            obs.color = sf::Color(110, 110, 110);
            break;
            
        case ObstacleType::Puddle:
            obs.width = 180.0f;
            obs.height = 120.0f;
            obs.color = sf::Color(70, 140, 200, 170);
            break;
    }
    
    return obs;
}

ObstacleType ObstacleSystem::getRandomType() {
    std::uniform_int_distribution<int> dist(0, 100);
    int roll = dist(m_rng);
    
    if (roll < 30) return ObstacleType::Pothole;
    else if (roll < 55) return ObstacleType::OilSlick;
    else if (roll < 80) return ObstacleType::Debris;
    else return ObstacleType::Puddle;
}

float ObstacleSystem::getRandomLateralPosition() {
    // Spawn on road, in the center zone to be hit
    std::uniform_real_distribution<float> dist(-500.0f, 500.0f);
    return dist(m_rng);
}

void ObstacleSystem::checkCollisions(float playerZ, float playerX, float playerSpeed,
                                     float carWidth, float carHeight) {
    for (auto& obs : m_obstacles) {
        if (!obs.isActive || obs.wasHit) continue;
        
        if (checkCollision(obs, playerZ, playerX, carWidth, carHeight)) {
            obs.wasHit = true;
            
            ObstacleEffect effect = obs.calculateEffect(playerSpeed, m_rng);
            
            if (m_onCollision && effect.hasEffect()) {
                m_onCollision(effect);
            }
            
            obs.alpha = 80.0f;
        }
    }
}

bool ObstacleSystem::checkCollision(const Obstacle& obs, float playerZ, float playerX,
                                    float carWidth, float carHeight) const {
    float playerLeft = playerX - carWidth / 2.0f;
    float playerRight = playerX + carWidth / 2.0f;
    float playerFront = playerZ + carHeight / 2.0f;
    float playerBack = playerZ - carHeight / 2.0f;
    
    bool xOverlap = playerRight > obs.getLeft() && playerLeft < obs.getRight();
    bool zOverlap = playerFront > obs.getBack() && playerBack < obs.getFront();
    
    return xOverlap && zOverlap;
}

void ObstacleSystem::projectObstacle(Obstacle& obs, float cameraZ, float cameraX,
                                     float screenCenterX, float screenCenterY) {
    // Not used - projection is directly in render()
}

void ObstacleSystem::removeInactiveObstacles(float playerZ) {
    m_obstacles.erase(
        std::remove_if(m_obstacles.begin(), m_obstacles.end(),
            [playerZ](const Obstacle& obs) {
                return obs.worldZ < playerZ - ObstacleConfig::DESPAWN_BEHIND;
            }),
        m_obstacles.end()
    );
}

int ObstacleSystem::getActiveCount() const {
    return static_cast<int>(std::count_if(m_obstacles.begin(), m_obstacles.end(),
        [](const Obstacle& obs) { return obs.isActive && !obs.wasHit; }));
}
