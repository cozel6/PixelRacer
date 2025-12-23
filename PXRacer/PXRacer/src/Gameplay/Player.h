#pragma once
#include <SFML/Graphics.hpp>

namespace PlayerConfig {
    // ✅ F1 ANII '90 - VITEZE REALISTE!
    // MAX_SPEED în m/s: 320 km/h ÷ 3.6 = 88.89 m/s
    constexpr float MAX_SPEED = 90.0f;           // ~324 km/h (realist F1 '90s)
    constexpr float ACCELERATION = 30.0f;        // 0-100 km/h în ~2.8s
    constexpr float BRAKING = 50.0f;             // Frânare puternică F1
    constexpr float DECELERATION = 8.0f;         // Decelerare naturală
    constexpr float STEER_SPEED = 1000.0f;       // Păstrăm pentru control responsive
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
    
    // Grip thresholds based on wheels on road
    constexpr float FULL_GRIP = 1.0f;
    constexpr float PARTIAL_GRIP_3 = 0.75f;
    constexpr float PARTIAL_GRIP_2 = 0.5f;
    constexpr float PARTIAL_GRIP_1 = 0.25f;
    constexpr float NO_GRIP = 0.1f;
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

    void update(float deltaTime, const WheelSurfaces& wheelSurfaces);
    void render(sf::RenderWindow& window, float screenX, float screenY, float scale);

    // Getters
    float getZ() const { return m_positionZ; }
    float getX() const { return m_positionX; }
    float getSpeed() const { return m_speed; }
    bool isSpinning() const { return m_isSpinning; }
    
    // Get wheel positions for collision detection
    void getWheelPositions(float& flX, float& flZ, 
                           float& frX, float& frZ,
                           float& rlX, float& rlZ,
                           float& rrX, float& rrZ) const;

    // Setters
    void setZ(float z) { m_positionZ = z; }
    void setX(float x) { m_positionX = x; }
    
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
    
    void handleInput(float deltaTime, float gripMultiplier);
    void applyPhysics(float deltaTime, const WheelSurfaces& wheelSurfaces);
    void handleSpinOut(float deltaTime);
    float calculateGripFromWheels(const WheelSurfaces& wheelSurfaces) const;
};