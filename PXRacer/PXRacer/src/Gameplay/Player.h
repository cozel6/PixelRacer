#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

namespace PlayerConfig {
    constexpr float MAX_SPEED = 90.0f;
    constexpr float ACCELERATION = 30.0f;
    constexpr float BRAKING = 50.0f;
    constexpr float DECELERATION = 8.0f;
    constexpr float STEER_SPEED = 1000.0f;
    constexpr float MAX_LATERAL_POSITION = 1000.0f;

    constexpr float CAR_WIDTH = 80.0f;
    constexpr float CAR_HEIGHT = 120.0f;
    constexpr float WHEEL_OFFSET_X = CAR_WIDTH * 0.4f;
    constexpr float WHEEL_OFFSET_Z = CAR_HEIGHT * 0.35f;

    constexpr float GRASS_FRICTION = 0.95f;
    constexpr float SPIN_SPEED = 180.0f;
    constexpr float RECOVERY_TIME = 2.0f;

    constexpr float MAX_DAMAGE = 100.0f;
}

enum class SurfaceType { Road, Grass, OffTrack };

struct WheelSurfaces {
    SurfaceType frontLeft; SurfaceType frontRight;
    SurfaceType rearLeft; SurfaceType rearRight;

    int wheelsOnRoad() const {
        int count = 0;
        if (frontLeft == SurfaceType::Road) count++;
        if (frontRight == SurfaceType::Road) count++;
        if (rearLeft == SurfaceType::Road) count++;
        if (rearRight == SurfaceType::Road) count++;
        return count;
    }
    bool anyOnGrass() const {
        return frontLeft == SurfaceType::Grass || frontRight == SurfaceType::Grass ||
            rearLeft == SurfaceType::Grass || rearRight == SurfaceType::Grass;
    }
};

class Player {
public:
    Player();
    void update(float deltaTime, const WheelSurfaces& wheelSurfaces, float roadCurve = 0.0f);
    void render(sf::RenderWindow& window, float screenX, float screenY, float scale);
    void loadTextures();

    // Getters necesari pentru GameplayManager și HUD
    float getZ() const { return m_positionZ; }
    float getX() const { return m_positionX; }
    float getSpeed() const { return m_speed; }
    bool isSpinning() const { return m_isSpinning; }
    float getTotalDamage() const { return m_totalDamage; }
    bool isDestroyed() const { return m_totalDamage >= PlayerConfig::MAX_DAMAGE; }

    // Metodă cerută de GameplayManager pentru coliziuni/suprafețe
    void getWheelPositions(float& flX, float& flZ, float& frX, float& frZ,
        float& rlX, float& rlZ, float& rrX, float& rrZ) const;

    void setZ(float z) { m_positionZ = z; }
    void setX(float x) { m_positionX = x; }
    void setSpeed(float speed) { m_speed = speed; }

    void addDamage(float damage);
    void repair(float amount);
    void resetPosition();

private:
    float m_positionZ;
    float m_positionX;
    float m_speed;
    float m_rotation;
    int m_steeringVisualState;

    sf::Texture m_textureStraight;
    sf::Texture m_textureLeft;
    sf::Texture m_textureRight;
    bool m_texturesLoaded = false;

    bool m_isSpinning;
    float m_spinTimer;
    float m_offRoadTimer;
    float m_wobbleOffset;
    float m_wobbleTimer;

    float m_totalDamage;

    void handleInput(float deltaTime, float gripMultiplier);
    void applyPhysics(float deltaTime, const WheelSurfaces& wheelSurfaces, float roadCurve);
    void handleSpinOut(float deltaTime);
};