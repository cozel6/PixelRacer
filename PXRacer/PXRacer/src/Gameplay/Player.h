#pragma once
#include <SFML/Graphics.hpp>

namespace PlayerConfig {
    // ✅ F1 ANII '90 - VITEZE REALISTE!
    constexpr float MAX_SPEED = 90.0f;           // ~324 km/h
    constexpr float ACCELERATION = 30.0f;        
    constexpr float BRAKING = 50.0f;             
    constexpr float DECELERATION = 8.0f;         
    constexpr float STEER_SPEED = 1000.0f;       
    constexpr float MAX_LATERAL_POSITION = 1000.0f;
    
    // Car dimensions
    constexpr float CAR_WIDTH = 80.0f;
    constexpr float CAR_HEIGHT = 120.0f;
    constexpr float WHEEL_OFFSET_X = CAR_WIDTH * 0.4f;
    constexpr float WHEEL_OFFSET_Z = CAR_HEIGHT * 0.35f;
    
    // Physics parameters
    constexpr float ROAD_GRIP = 1.0f;
    constexpr float GRASS_GRIP = 0.3f;
    constexpr float GRASS_FRICTION = 0.95f;
    constexpr float SPIN_THRESHOLD = 0.5f;
    constexpr float SPIN_SPEED = 180.0f;
    constexpr float RECOVERY_TIME = 2.0f;
    
    // Grip thresholds
    constexpr float FULL_GRIP = 1.0f;
    constexpr float PARTIAL_GRIP_3 = 0.75f;
    constexpr float PARTIAL_GRIP_2 = 0.5f;
    constexpr float PARTIAL_GRIP_1 = 0.25f;
    constexpr float NO_GRIP = 0.1f;
    
    // Arcade challenge physics
    constexpr float CENTRIFUGAL_FORCE = 40.0f;
    constexpr float CORNER_SPEED_PENALTY = 0.9f;
    constexpr float SPEED_WOBBLE_THRESHOLD = 0.95f;
    constexpr float SPEED_WOBBLE_INTENSITY = 15.0f;
    constexpr float BRAKE_IN_CORNER_PENALTY = 0.2f;
    constexpr float DRIFT_THRESHOLD = 0.4f;
    constexpr float SAFE_CORNER_SPEED = 0.7f;
    
    // Damage system
    constexpr float MAX_DAMAGE = 100.0f;
    constexpr float DAMAGE_SPEED_PENALTY_THRESHOLD = 10.0f;
    constexpr float DAMAGE_SPEED_PENALTY = 5.56f;
}

enum class SurfaceType {
    Road,
    Grass,
    OffTrack
};

struct WheelSurfaces {
    SurfaceType frontLeft;
    SurfaceType frontRight;
    SurfaceType rearLeft;
    SurfaceType rearRight;
    
    int wheelsOnRoad() const {
        int count = 0;
        if (frontLeft == SurfaceType::Road) count++;
        if (frontRight == SurfaceType::Road) count++;
        if (rearLeft == SurfaceType::Road) count++;
        if (rearRight == SurfaceType::Road) count++;
        return count;
    }
    
    bool anyOnGrass() const {
        return frontLeft == SurfaceType::Grass || 
               frontRight == SurfaceType::Grass ||
               rearLeft == SurfaceType::Grass || 
               rearRight == SurfaceType::Grass;
    }
    
    bool allOffTrack() const {
        return frontLeft == SurfaceType::OffTrack && 
               frontRight == SurfaceType::OffTrack &&
               rearLeft == SurfaceType::OffTrack && 
               rearRight == SurfaceType::OffTrack;
    }
};

class Player {
public:
    Player();

    void update(float deltaTime, const WheelSurfaces& wheelSurfaces, float roadCurve = 0.0f);
    void render(sf::RenderWindow& window, float screenX, float screenY, float scale);

    // Getters
    float getZ() const { return m_positionZ; }
    float getX() const { return m_positionX; }
    float getSpeed() const { return m_speed; }
    bool isSpinning() const { return m_isSpinning; }
    bool isDrifting() const { return m_isDrifting; }
    float getTotalDamage() const { return m_totalDamage; }
    bool isDestroyed() const { return m_totalDamage >= PlayerConfig::MAX_DAMAGE; }
    
    void getWheelPositions(float& flX, float& flZ, 
                           float& frX, float& frZ,
                           float& rlX, float& rlZ,
                           float& rrX, float& rrZ) const;

    // Setters
    void setZ(float z) { m_positionZ = z; }
    void setX(float x) { m_positionX = x; }
    void setSpeed(float speed) { m_speed = speed; }
    
    // ✅ Damage system
    void addDamage(float damage);
    void repair(float amount);  // ✅ NEW
    float getMaxSpeedWithDamage() const;
    
    void resetPosition();

private:
    float m_positionZ;
    float m_positionX;
    float m_speed;
    float m_rotation;
    
    bool m_isSpinning;
    float m_spinTimer;
    float m_currentGrip;
    float m_offRoadTimer;
    
    // Challenge physics state
    bool m_isDrifting;
    float m_driftAngle;
    float m_wobbleOffset;
    float m_wobbleTimer;
    bool m_isBraking;
    
    // Damage state
    float m_totalDamage = 0.0f;
    int m_lastDamageThreshold = 0;
    
    void handleInput(float deltaTime, float gripMultiplier, float roadCurve);
    void applyPhysics(float deltaTime, const WheelSurfaces& wheelSurfaces, float roadCurve);
    void applyCentrifugalForce(float deltaTime, float roadCurve);
    void applySpeedWobble(float deltaTime);
    void handleSpinOut(float deltaTime);
    float calculateGripFromWheels(const WheelSurfaces& wheelSurfaces) const;
    float calculateCornerGrip(float roadCurve) const;
};